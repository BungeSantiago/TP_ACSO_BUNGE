.text
movz X0, 0x5     
movz X1, 0x1000  
lsl  X1, X1, 16  
add  X1, X1, target 
br   X1           

movz X2, 0xA  

target:
    add X3, X0, X0  
    hlt 0
