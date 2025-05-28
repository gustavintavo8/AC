ori r1, r0, 2 ; r1 = 2
startf:
    beqz r1, endf ; jump to endf if r1 zero
    daddi r1, r1, -1 ; r1 = r1-1
    j startf ; jump to startf
endf:
    ld r5, 100(r3)