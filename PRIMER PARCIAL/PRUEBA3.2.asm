	.code

main:
	ori r1, r0, 30
start:
	beqz r1, end
	dsub r2, r3, r4
	daddi r1, r1, -3
	j start
end:
	sd r2, 100(r0)
	xor  r2, r2, r2