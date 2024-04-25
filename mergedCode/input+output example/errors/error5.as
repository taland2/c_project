; Undefined action
J: undefaction r1

; Multiple spaces and tabs between operands
K: mov       r1,        r2

; Using a decimal number
L: .data 5.5

; Invalid characters in labels
M@: clr @r1
N$: inc @r2

; Invalid register names
O: mov rX, r10