; Spaces between '#' and IMM
mov  r1, # -5

; Invalid characters in string
STR1: .string "abc$ef"
STR2: .string "ghi^jk"

; Too many operands
D: add r1, r2, r3

; Label starting with a number (invalid)
1INVALIDLABEL: hlt