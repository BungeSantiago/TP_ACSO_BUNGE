.text
movz X0, 0x1234567890ABCDEF 
movz X1, 0x2000     
movz X2, 0x42               
movz X3, 0xABCD 
STUR X0, [X1, #0]      
STURB X2, [X1, #8]  
STURH X3, [X1, #16]     
HLT 0
