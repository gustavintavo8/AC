	.code

main:
  ori r9, r0, 4
  daddi r1, r0, 5
loop:
  beq r9, r1, exit
  dmul r5, r1, r2
  or r5, r3, r1
  daddi r9, r9, 1
  j loop
exit:
  ld r6, 40(r5)
  dadd r3, r5, r6
  sd r3, 100(r2)