.text
movz X0, 10 
movz X1, 5  
cmp X0, X1   
bge greater  

movz X2, 100    

greater:
subs X1, X1, 5
HLT 0
