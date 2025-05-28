	.code

main:
	ori r5, r0, 3 ; r5 = 3
	ddiv r2, r4, r5
dest:
	daddi r5, r5, -2 ; r5 = r5 - 2
	beq r5, r0, dest ; branch on r5 equal to r0
	or r3, r0, r9
