.entry K
.entry LOOP
.extern W
.extern R
MAIN: mov r0,r1
      cmp #-5,K
      not K
LOOP:   jmp R
	prn K
	inc r7
	red W
END: hlt
K: .data -5
	inc r7
	red W
		prn K
	inc r7
	red W
		prn K
	inc r7
	red W
		prn K
	inc r7
	red W
		prn K
	inc r7
	red W
	; avsdv