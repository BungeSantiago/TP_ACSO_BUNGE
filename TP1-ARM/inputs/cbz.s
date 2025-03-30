adds X2, X2, 6
adds X3, X3, 6
cbz X3, falselabel
subs X4, X3, X2
cbz X3, truelabel

falselabel:
adds X5, X3, X2

truelabel:
adds X6, X3, X2