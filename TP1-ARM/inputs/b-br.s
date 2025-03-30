.text

movz X0, branch_target
movz X1, 0x30 
movz X2, 0x40

b     branch_target
movz X3, 0x50

branch_target:
movz X4, 0x60
br    X0      
movz X5, 0x70

HLT 0
