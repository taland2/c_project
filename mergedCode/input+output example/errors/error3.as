; Operand without preceding instruction or action
#1234

; Floating label without following colon
FLOATLABEL mov r1, r2

; Nested instructions
E: .data 5, .string "hello", 6

; Two instructions/actions on one line
F: clr r1 , hlt

; Use undefined label as operand
G: mov r2, UNDEFINED