; Double definition of label
DUPLABEL: clr r1
DUPLABEL: inc r1

; Missing instruction/action after label
EMPTYLABEL: 

; Entry on extern label
.extern EXTERNLABEL
.entry EXTERNLABEL

; Extern on entry label
.entry ENTRYLABEL
.extern ENTRYLABEL

; Missing operand
mov r1,