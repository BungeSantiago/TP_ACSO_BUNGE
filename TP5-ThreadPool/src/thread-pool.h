#ifndef _thread_pool_
#define _thread_pool_

#include <cstddef> 
#include <functional> 
#include <queue> 
#include <thread>
#include <vector> 
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Semaphore.h"
using namespace std;


/**
 * @brief Represents a worker in the thread pool.
 * 
 * The `worker_t` struct contains information about a worker 
 * thread in the thread pool. Should be includes the thread object, 
 * availability status, the task to be executed, and a semaphore 
 * (or condition variable) to signal when work is ready for the 
 * worker to process.
 */

typedef struct worker {
    thread               thr; // hilo real
    Semaphore            sem{0}; // señala cuando hay trabajo asignado
    function<void(void)> task; // tarea a ejecutar
}worker_t;

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    void schedule(const function<void(void)>& thunk);
    void wait();
    ~ThreadPool();

    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    // funciones auxiliares que corren en hilos aparte
    void dispatcher();
    void worker(size_t id);


    queue<function<void(void)>> tasks; // cola FIFO de thunks
    mutex                tasksMtx; // protege tasks
    Semaphore            tasksSem{0}; // cuenta tareas en cola

    queue<size_t>        idleWorkers; // IDs de workers libres
    mutex                idleMtx; // protege idleWorkers
    Semaphore            readySem{0}; // cuenta workers libres

    vector<worker_t>     workers; // todos los workers
    thread               dispThr; // hilo despachador

    atomic<size_t>       pending{0}; // tareas que faltan
    mutex                doneMtx; // para wait
    condition_variable   doneCV;

    atomic<bool>         shutting{false}; // cierre del pool
};
#endif