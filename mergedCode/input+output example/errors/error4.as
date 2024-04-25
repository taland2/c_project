; Exceeding character limit (assuming 81 is the file_number)
LONG: .string "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopq"

; Label that is an instruction
.string: mov r2, r1

; Label that is an CODE
mov: add r1, r2

; Two colons after a label
H:: clr r3