
.microarch
forwarding = true
branch_prediction = always_not_taken
exb_no_pipelined_cycles = 5
exb_instructions = [dmul, ddiv]

.data
samples: dword 10, 20, 13, -8

.code
	ori r6, r0, 32
	daddi r2, r0, 25
init:
	daddi r1, r0, 32
loop:
	beq r6, r0, exit
	daddi r1, r1, -8
	ld r3, samples(r1)
	dmul r5, r2, r2
	ddiv r2, r2, r6
	dadd r2, r3, r3
	daddi r6, r6, -8
	bne r1, r0, loop
	beqz r6, endprogram
	xor r2, r1, r3
endprogram:
	daddi r1, r0, 8
exit:
	nop
	


