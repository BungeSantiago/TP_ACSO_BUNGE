.text
mov X1, 0x1000
lsl X1, X1, 16
mov X10, 0x1234
sturh w10, [X1, 0x0]
sturh W10, [X1, 0x6]
ldur X13, [X1, 0x0]
ldurh w14, [X2, 0x4]
ldurh W15, [X3, 0x6]
HLT 0
