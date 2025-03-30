.text
movz X0, 2      // X0 = 10
movz X1, 3      // X1 = 5
cmp X0, X1       // Compare X0 and X1, set flags accordingly
bge greater      // If X0 >= X1, branch to 'greater'

// Code here will be skipped if branch is taken
movz X2, 100     // This instruction is skipped if the branch occurs.

greater:
movz X3, 200     // This instruction is executed if X0 >= X1 (branch is taken)
hlt 0