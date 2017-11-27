

  jump main

.include lib_draw.s

draw:
  ; draw a line between (r1, r2) and (r3, r4) with color r0

  push 64 r7

  ; e(1, 0) : r6 <- (r4 - r2) * 2
  let r6 r4
  sub r6 r2
  shift left r6 1

  ; e : r5 <- (r1 - r3) <- -(r1 - r3)
  let r5 r1
  sub r5 r3

  ; e(0, 1) : r7 <- (r5) * 2 <- (r1 - r3)*2 <- -(r1 - r3)*2
  let r5 r1
  let r7 r5
  shift left r7 1


  __begin_for_r1_leq_r3:
    ; r1 != r3
    cmp r1 r3
    jumpif z __end_for_r1_leq_r3
    add r1 1
    ; core of the for loop


    ; call plot(r1, r2, r0)
    push 64 r0
    push 64 r1
    push 64 r2
    push 64 r3
    push 64 r4
    push 64 r5
    push 64 r6
    push 64 r7

    let r3 r0
    call plot ; draw pixel (r1, r2) with color r3

    pop 64 r7
    pop 64 r6
    pop 64 r5
    pop 64 r4
    pop 64 r3
    pop 64 r2
    pop 64 r1
    pop 64 r0

    __begin_if_r5_plus_r4_ge0:
      add r5 r7
      cmp r5 0 
      jumpif slt __then
      jump __else
    __then:
      sub r5 r7

      add r2 1
      add r5 r6


      jump __end_if
    __else:
      sub r5 r7
    __end_if:


    jump __begin_for_r1_leq_r3

  __end_for_r1_leq_r3:

  pop 64 r7
  
  return

__end_draw:




main:
  let r0 0xFFFF
  let r1 32
  let r2 32
  let r3 80
  let r4 32

  call draw
