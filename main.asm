
#include "SD8005B.INC"

#include "SD8005B_GeneralReg.inc"

#include "e:\sdic\SDIC_IDE\SDIC_IDE\bin\SubProgram.inc"
	org		0x0000	
	bra		Label_Preprocess
	org		0x0008	
	bra		Label_Hint
	org		0x0020	

Label_Preprocess:
	call		ClearRam,1
	movlw		.1
	movlb		0x02
	movwf		REG2FF,	1
	movlw		.0
	movwf		REG2FE,	1
	movlw		.2
	movwf		REG2FD,	1
	movlw		.0
	movwf		REG2FC,	1
	movlw		.3
	movwf		REG2FB,	1
	movlw		.0
	movwf		REG2FA,	1
	movlw		.0
	movwf		REG2F9,	1
	movlw		.0
	movwf		REG2F8,	1
	movlw		.1
	movwf		REG2F7,	1
	movlw		.0
	movwf		REG2F6,	1
	movlw		.2
	movwf		REG2F5,	1
	movlw		.0
	movwf		REG2F4,	1
	movlw		.0
	movwf		REG2F3,	1
	movlw		.0
	movwf		REG2F2,	1
	bra		Label_begin

Label_begin:
	bra		Label_main

Label_main:
	movlw		.0
	movwf		REG2F1,	1
	movlw		.0
	movwf		REG2F0,	1
	movlw		.2
	movwf		REG2EF,	1
	movlw		.0
	movwf		REG2EE,	1
	movlw		.4
	movwf		REG2ED,	1
	movlw		.0
	movwf		REG2EC,	1
	movlw		.1
	cpfseq		REG2EF,	1
	bra		Label_main_3_next
	movlw		.0
	cpfseq		REG2EE,	1
	bra		Label_main_3_next
	bra		Label_main_3

Label_main_3_next:
	movlw		.2
	cpfseq		REG2EF,	1
	bra		Label_main_4_next
	movlw		.0
	cpfseq		REG2EE,	1
	bra		Label_main_4_next
	bra		Label_main_4

Label_main_4_next:
	bra		Label_main_5

Label_main_3:
	movlw		.2
	movwf		REG2EF,	1
	movlw		.0
	movwf		REG2EE,	1
	bra		Label_main_6

Label_main_4:
	movlw		.3
	movwf		REG2EF,	1
	movlw		.0
	movwf		REG2EE,	1
	bra		Label_main_6

Label_main_5:
	movlw		.255
	movwf		REG2EF,	1
	movlw		.255
	movwf		REG2EE,	1
	bra		Label_main_6

Label_main_6:
	movf		REG2EF,	0,	1
	movlb		0x01
	movwf		Divdend + 0,	1
	movlb		0x02
	movf		REG2ED,	0,	1
	movlb		0x01
	movwf		Divsor + 0,	1
	movlb		0x02
	movf		REG2EE,	0,	1
	movlb		0x01
	movwf		Divdend + 1,	1
	movlb		0x02
	movf		REG2EC,	0,	1
	movlb		0x01
	movwf		Divsor + 1,	1
	call		Division,1
	movf		Remainder + 0,	0,	1
	movlb		0x02
	movwf		REG2CD,	1
	movlb		0x01
	movf		Remainder + 1,	0,	1
	movlb		0x02
	movwf		REG2CC,	1
	movf		REG2CD,	0,	1
	movwf		REG2EB,	1
	movf		REG2CC,	0,	1
	movwf		REG2EA,	1
	movf		REG2FF,	0,	1
	movwf		REG2DB,	1
	movf		REG2FE,	0,	1
	movwf		REG2DA,	1
	movf		REG2FD,	0,	1
	movwf		REG2D9,	1
	movf		REG2FC,	0,	1
	movwf		REG2D8,	1
	movf		REG2FB,	0,	1
	movwf		REG2D7,	1
	movf		REG2FA,	0,	1
	movwf		REG2D6,	1
	movf		REG2EF,	0,	1
	movlb		0x01
	movwf		Multiplicand + 0,	1
	movlb		0x02
	movf		REG2ED,	0,	1
	movlb		0x01
	movwf		Multiplier + 0,	1
	movlb		0x02
	movf		REG2EE,	0,	1
	movlb		0x01
	movwf		Multiplicand + 1,	1
	movlb		0x02
	movf		REG2EC,	0,	1
	movlb		0x01
	movwf		Multiplier + 1,	1
	call		Multiplication,1
	movf		Product + 0,	0,	1
	movlb		0x02
	movwf		REG2DC,	1
	movlb		0x01
	movf		Product + 1,	0,	1
	movlb		0x02
	movwf		REG2DD,	1
	movf		REG2DC,	0,	1
	movwf		REG2D5,	1
	movf		REG2DD,	0,	1
	movwf		REG2D4,	1
	movf		REG2ED,	0,	1
	subwf		REG2EF,	0,	1
	movwf		REG2DE,	1
	movf		REG2EC,	0,	1
	subwfb		REG2EE,	0,	1
	movwf		REG2DF,	1
	movlw		.1
	movwf		REG2E0,	1
	movwf		REG2E1,	1
	tstfsz		REG2DE,	1
	bra		Label_main_26
	tstfsz		REG2DF,	1
	bra		Label_main_26
	bra		Label_main_27

Label_main_26:
	btfss		STATUS,	0,	0
	bra		Label_main_27
	bra		Label_main_28

Label_main_27:
	movlw		.1
	movwf		REG2EF,	1
	movlw		.0
	movwf		REG2EE,	1
	bra		Label_main_29

Label_main_28:
	movlw		.2
	movwf		REG2EF,	1
	movlw		.0
	movwf		REG2EE,	1
	bra		Label_main_29

Label_main_29:
	movf		REG2ED,	0,	1
	movlb		0x01
	movwf		Divdend + 0,	1
	movlb		0x02
	movf		REG2EF,	0,	1
	movlb		0x01
	movwf		Divsor + 0,	1
	movlb		0x02
	movf		REG2EC,	0,	1
	movlb		0x01
	movwf		Divdend + 1,	1
	movlb		0x02
	movf		REG2EE,	0,	1
	movlb		0x01
	movwf		Divsor + 1,	1
	call		Division,1
	movf		Quotient + 0,	0,	1
	movlb		0x02
	movwf		REG2E2,	1
	movlb		0x01
	movf		Quotient + 1,	0,	1
	movlb		0x02
	movwf		REG2E3,	1
	movf		REG2E2,	0,	1
	movwf		REG2D3,	1
	movf		REG2E3,	0,	1
	movwf		REG2D2,	1
	movlw		.1
	movlb		0x01
	movwf		REG100,	1
	movlw		.0
	movwf		REG101,	1
	movlw		.3
	movwf		REG102,	1
	movlw		.0
	movwf		REG103,	1
	call		Label_add,1
	movf		REG100,	0,	1
	movlb		0x02
	movwf		REG2E4,	1
	movlb		0x01
	movf		REG101,	0,	1
	movlb		0x02
	movwf		REG2E5,	1
	movf		REG2E4,	0,	1
	movwf		REG2D1,	1
	movf		REG2E5,	0,	1
	movwf		REG2D0,	1
	movf		REG2ED,	0,	1
	movlb		0x01
	movwf		REG100,	1
	movlb		0x02
	movf		REG2EC,	0,	1
	movlb		0x01
	movwf		REG101,	1
	movlb		0x02
	movf		REG2EF,	0,	1
	movlb		0x01
	movwf		REG102,	1
	movlb		0x02
	movf		REG2EE,	0,	1
	movlb		0x01
	movwf		REG103,	1
	call		Label_sub,1
	movf		REG100,	0,	1
	movlb		0x02
	movwf		REG2E6,	1
	movlb		0x01
	movf		REG101,	0,	1
	movlb		0x02
	movwf		REG2E7,	1
	movf		REG2E6,	0,	1
	movwf		REG2CF,	1
	movf		REG2E7,	0,	1
	movwf		REG2CE,	1
	movf		REG2D5,	0,	1
	movlb		0x01
	movwf		REG100,	1
	movlb		0x02
	movf		REG2D4,	0,	1
	movlb		0x01
	movwf		REG101,	1
	return		1

Label_add:
	movf		REG100,	0,	1
	movlb		0x02
	movwf		REG2E8,	1
	movlb		0x01
	movf		REG101,	0,	1
	movlb		0x02
	movwf		REG2E9,	1
	movlb		0x01
	movf		REG102,	0,	1
	movlb		0x02
	movwf		REG2CB,	1
	movlb		0x01
	movf		REG103,	0,	1
	movlb		0x02
	movwf		REG2CA,	1
	movf		REG2E8,	0,	1
	movwf		REG2C7,	1
	movf		REG2E9,	0,	1
	movwf		REG2C6,	1
	movf		REG2CB,	0,	1
	movwf		REG2C5,	1
	movf		REG2CA,	0,	1
	movwf		REG2C4,	1
	movlw		.1
	subwf		REG2C7,	0,	1
	movwf		REG2C3,	1
	movlw		.0
	subwfb		REG2C6,	0,	1
	movwf		REG2C2,	1
	movlw		.1
	movwf		REG2C1,	1
	movwf		REG2C0,	1
	tstfsz		REG2C3,	1
	bra		Label_add_5
	tstfsz		REG2C2,	1

Label_add_5:
	bra		Label_add_11
	bra		Label_add_6

Label_add_6:
	movlw		.1
	movlb		0x01
	movwf		REG100,	1
	movlb		0x02
	movf		REG2C7,	0,	1
	movlb		0x01
	addwf		REG100,	0,	1
	movlb		0x02
	movwf		REG2BF,	1
	movlw		.0
	movlb		0x01
	movwf		REG100,	1
	movlb		0x02
	movf		REG2C6,	0,	1
	movlb		0x01
	addwfc		REG100,	0,	1
	movlb		0x02
	movwf		REG2BE,	1
	movf		REG2BF,	0,	1
	movlb		0x01
	movwf		REG100,	1
	movlb		0x02
	movf		REG2BE,	0,	1
	movlb		0x01
	movwf		REG101,	1
	movlb		0x02
	movf		REG2C5,	0,	1
	movlb		0x01
	movwf		REG102,	1
	movlb		0x02
	movf		REG2C4,	0,	1
	movlb		0x01
	movwf		REG103,	1
	call		Label_add,1
	movf		REG100,	0,	1
	movlb		0x02
	movwf		REG2BD,	1
	movlb		0x01
	movf		REG101,	0,	1
	movlb		0x02
	movwf		REG2BC,	1
	movf		REG2BD,	0,	1
	movwf		REG2C7,	1
	movf		REG2BC,	0,	1
	movwf		REG2C6,	1
	movlw		.5
	movwf		REG2C9,	1
	movlw		.0
	movwf		REG2C8,	1
	bra		Label_add_12

Label_add_11:
	movlw		.3
	movwf		REG2C9,	1
	movlw		.0
	movwf		REG2C8,	1
	bra		Label_add_12

Label_add_12:
	movf		REG2C9,	0,	1
	movlb		0x01
	movwf		REG100,	1
	movlb		0x02
	movf		REG2C8,	0,	1
	movlb		0x01
	movwf		REG101,	1
	return		1
