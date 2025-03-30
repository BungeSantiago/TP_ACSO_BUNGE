.text
mov X1, 0x1000    
mov H10, 0x1234      
sturh H10, [X1, 0x0]  
ldurh H11, [X1, 0x0] 
HLT 0           