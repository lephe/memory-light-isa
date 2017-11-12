    ; Initialization of the values.
    leti    r0  -0x64234
    leti    r1  0x1234123


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

    shift   left r4 1
    jumpif  nc r2_is_positive
    sub3    r2 r3 r2
r2_is_positive:

    ; End of the programm.
halt:
    jump    halt
