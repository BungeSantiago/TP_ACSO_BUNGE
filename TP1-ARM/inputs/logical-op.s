.text
movz X0, 0x11
movz X1, 0x10
ands X2, X0, X1
eor X3, X0, X1
orr X4, X0, X1

HLT 0
