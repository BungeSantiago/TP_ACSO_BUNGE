movz X2, 6
movz X3, 6  
cbnz X3, falselabel
subs X4, X3, X2
cbnz X4, truelabel

falselabel:
adds X5, X3, X2

truelabel:
adds X6, X3, X2

HLT 0
