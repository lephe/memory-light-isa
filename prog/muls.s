    ; Initialization of the values.
    leti    r0 -02138213
    leti    r1 1242748123

    

    ; We can store the sign bit here. We assume here that r0 * r1 is signed
    ; and stay on 64 bits. So it's signed value stays on 63 bits.
    ; r3 = 0x8000000000000000
    leti r3 1
    shift left r3 63


    ; Getting the sign on the result. 
    xor3    r2 r1 r0
    and2    r2 r3

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
    cmpi    r2 0
    shift   left  r2 1 ; Remove the last bit
    shift   right r2 1
    jumpif  sgt multr2endshift
    sub3    r2 r3 r2
multr2endshift:

    ; End of the programm.
halt:
    jump    halt
