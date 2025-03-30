.text
movz X0, 10    
movz X1, 5       
cmp X0, X1  
bgt greater 
movz X2, 100

greater:
movz X3, 200 
hlt 0