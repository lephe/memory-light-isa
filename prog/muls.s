    ; Initialization of the values.
    leti    r0 0x100000000000
    leti    r1 0xffffff


    ; Getting the sign on the result. 
    xor3    r4 r0 r1

    ; To do sub r1 0 r1 (taking the negative value.)
    leti    r3 0

    ; r0 = |r0|
    cmpi    r0 0
    jumpif  sgt r0_is_positive
    sub3    r0 r3 r0
r0_is_positive:

    ; r1 = |r1|
    cmpi    r1 0
    jumpif  sgt r1_is_positive
    sub3    r1 r3 r1
r1_is_positive:

    ; Processing multiplication of two positives integers.
nonzero:
    shift   right r0 1
    jump    nc next
    add2    r2 r1
next:
    shift   left r1 1
    cmp     r0 0
    jump    nz nonzero


    ; Ajusting the sign of the result.
    cmpi    r4 0
    jumpif  sgt multr2endshift
    sub3    r2 r3 r2
multr2endshift:

    ; End of the programm.
halt:
    jump    halt
