;-----------------------------------------------------------------------------;
;  Signed 64 * 64 -> 64 multiplication                                        ;
;-----------------------------------------------------------------------------;

    ; Initializing operands.
    leti    r0 -0x738b6c
    leti    r1 0xc4b3213e
    ; r0 * r1 = 0xffa73867'd2874fd8


    ; Getting the sign of the result.
    xor3    r4 r0 r1

    ; Used to calculate sub r1 0 r1 (ie. taking the opposite).
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

    ; Calculating product of two positive integers.
nonzero:
    shift   right r0 1
    jump    nc next
    add2    r2 r1
next:
    shift   left r1 1
    cmp     r0 0
    jump    nz nonzero


    ; Adjusting the sign of the result.
    shift   left r4 1
    jumpif  nc r2_is_nonnegative
    sub3    r2 r3 r2
r2_is_nonnegative:

    ; End of the program.
halt:
    jump    halt
