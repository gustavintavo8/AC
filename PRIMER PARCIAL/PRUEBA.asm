	.code

main:
	j label
	dadd r4, r1, r5
label:
	xor r5, r5, r5
	bne r5, r0, label
	dmul r3, r8, r7
	and r3, r3, r9 