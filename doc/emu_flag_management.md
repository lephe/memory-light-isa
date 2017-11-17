# Emulator Flag Management

Here is how the emulator calculates its flags (without using double-word
values, which would be 128-bit large). The following table does not distinguish
between registers and constants because both behave in the same way.

    Operation        Result         Z         N          C
    ------------------------------------------------------------------------
    add x y          r = x + y      r == 0    s(r) < 0   y && u(x) >= -u(y)
    sub x y          r = x - y      r == 0    s(r) < 0   u(x) < u(y)
    cmp x y          r = x - y      r == 0    s(r) < 0   u(x) < u(y)
    shift left x n   r = u(x) << n  r == 0    s(r) < 0   (u(x) >> (n-1)) & 1
    shift right x n  r = u(x) >> n  r == 0    s(r) < 0   s(u(x) << (n-1)) < 0
    or x y           r = x | y      r == 0    s(r) < 0   0
    and x y          r = x & y      r == 0    s(r) < 0   0
    xor x y          r = x ^ y      r == 0    s(r) < 0   0
    asr3 r x n       r = s(x) >> n  r == 0    s(r) < 0   (u(x) >> (n-1)) & 1

Three-operand instructions perform the same calculations as their two-operand
counterparts (ie, the calculate the same `r`) and thus have the same behavior.

    Legend:
    s(x)   Signed interpretation of x
    u(x)   Unsigned interpretation of x

All instructions that are not listed do not affect any flag.
