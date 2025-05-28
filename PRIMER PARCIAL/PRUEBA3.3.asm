	.code

main:
	daddi r1, r0, 27
startloop:
	beqz r1, exitloop
	daddi r1, r1, -3
	xor r3, r7, r6
	j startloop
exitloop:
	sd r3, 100(r0)
	xor  r3, r3, r3