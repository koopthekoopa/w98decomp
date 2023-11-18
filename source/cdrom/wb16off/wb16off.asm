; wb16off application

entryPoint:
	CLI ; dont need interrupts for now

	; writing and reading some ports?
	MOV	AL, 0xC2
	OUT	0x22, AL
	IN	AL, 0x23
	MOV	AH, AL
	
	AND	AH, 0xBF
	MOV	AL, 0xC2
	OUT	0x22, AL
	MOV	AL, AH

	OUT	0x23, AL
	STI ; restore interrupts
	MOV	AH, 0x4C
	INT	0x21


