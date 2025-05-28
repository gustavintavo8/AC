	.code

main:
	ori r5, r0, 5
	xor r1, r4, r5
	bnez r5, label
	slt r4, r8, r7
label:
	dmul r3, r8, r6
	andi r3, r0, 7