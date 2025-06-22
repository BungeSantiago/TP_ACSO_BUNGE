#include "thread-pool.h"
using namespace std;

ThreadPool::ThreadPool(size_t numThreads) : workers(numThreads) {
    // armo todos los workers y los dejo libres
    for (size_t i = 0; i < numThreads; ++i) {
        idleWorkers.push(i); // lo anoto como disponible
        readySem.signal(); // sumo uno a la cuenta de libres
        workers[i].thr = thread(&ThreadPool::worker, this, i); // corro el hilo
    }

    // levanto el hilo que reparte las tareas
    dispThr = thread(&ThreadPool::dispatcher, this);
}


void ThreadPool::schedule(const function<void(void)>& thunk) {
    {
        lock_guard<mutex> lock(tasksMtx);
        tasks.push(thunk);
        ++pending;
    }
    tasksSem.signal(); // avisamos al dispatcher
}

void ThreadPool::wait() {
    unique_lock<mutex> lk(doneMtx);
    doneCV.wait(lk, [this] { return pending.load() == 0; });
}

ThreadPool::~ThreadPool() {
    wait(); // espera a que se terminen todas las tareas
    shutting = true; // avisa que vamos a cerrar todo

    // despertamos al dispatcher y a los workers por si están esperando algo
    tasksSem.signal(); // por si el dispatcher esta dormido
    for (auto& w : workers) w.sem.signal(); // despertamos a todos los hilos

    if (dispThr.joinable()) dispThr.join(); // esperamos a que el dispatcher termine

    // esperamos a que terminen todos los workers
    for (auto& w : workers) {
        if (w.thr.joinable()) w.thr.join();
    }
}


void ThreadPool::dispatcher() {
    while (true) {
        
        tasksSem.wait(); // espera hasta que llegue una tarea
        if (shutting && tasks.empty()) break; // si ya cerramos y no hay tareas se va

        // agarro una tarea de la cola
        function<void(void)> job;
        {
            lock_guard<mutex> lock(tasksMtx);
            if (tasks.empty()) continue; // sigue esperando
            job = tasks.front();
            tasks.pop();
        }

        
        readySem.wait(); // espero a tener un worker libre
        size_t wid;
        {
            lock_guard<mutex> lock(idleMtx);
            wid = idleWorkers.front();
            idleWorkers.pop(); // lo saco de la lista de libres
        }

        //le paso la tarea al worker y lo despierto
        workers[wid].task = job;
        workers[wid].sem.signal();
    }
}


void ThreadPool::worker(size_t id) {
    worker_t& self = workers[id];

    while (true) {
        // esperar tarea o señal de cierre
        self.sem.wait();
        if (shutting) break;

        // ejecutar la tarea asignada
        self.task();

        // actualizar contador de tareas pendientes
        if (pending.fetch_sub(1) == 1) {
            lock_guard<mutex> lk(doneMtx);
            doneCV.notify_all();
        }

        // marcarse como libre
        {
            lock_guard<mutex> lock(idleMtx);
            idleWorkers.push(id);
        }
        readySem.signal();
    }
}
