	.code

main:
	daddi r1, r0, 32
start:
	beqz r1, end
	daddi r1, r1, -4
	dsub r3, r7, r6
	j start
end:
	sd r3, 100(r0)
	xor  r3, r3, r3