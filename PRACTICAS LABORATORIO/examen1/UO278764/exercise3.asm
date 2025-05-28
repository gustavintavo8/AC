.microarch
	exb_no_pipelined_cycles = 3
	exb_instructions = [dmul, ddiv]

.data
list:   byte  13, 22, 19, 64, 97, 21, 12, 10
count:  dword 0

.code	
	xor   r1, r1, r1
	xor   r7, r7, r7
	daddi r5, r0, 2
	daddi r6, r0, 8
loop:
    lb    r3, list(r1)
	ddiv  r2, r3, r5
	dmul  r4, r2, r5
	daddi r1, r1, 1
	bne   r4, r3, cond
	daddi r7, r7, 1
cond:
	bne   r6, r1, loop
	nop	
	sd    r7, count(r0)
