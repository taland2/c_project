.entry LIST
.extern W
.define sz=32
MAIN: mov r3, LIST[sz]
LOOP: jmp W
mov STR[5], STR[2]
sub r1, r4
cmp K, #2
bne W
L1: inc L3
.entry LOOP
bne LOOP
END: hlt
STR: .string "abcdef"
LIST: .data 6, -9, 4
K: .data 22
.extern L3
