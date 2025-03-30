.text
movz X0, 10 
movz X1, 5  
cmp X0, X1   
ble lesser  

movz X2, 100    
HLT 0

lesser:
subs X1, X1, 5
HLT 0
