MOD_A: BEGIN
SECTION TEXT
	MOD_B: EXTERN
	PUBLIC FAT
	PUBLIC N
	INPUT N
UM: EQU 1
	IF UM
	LOAD N
	FAT: SUB ONE
	JMPZ FIM
	JMP MOD_B
	FIM: OUTPUT N
	STOP
SECTION DATA
	N: SPACE
	M: CONST 2
	ONE: CONST 1
END

