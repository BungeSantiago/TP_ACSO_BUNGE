.text
movz X0, 1
movz X1, 2  
cmp X0, X1
bne foo
adds X4, X0, 5

foo:
adds X3, X0, X1
HLT 0
