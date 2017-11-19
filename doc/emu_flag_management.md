# Emulator Flag Management

Here is how the emulator calculates the flags after executing an arithmetical
or logical instruction (without using double-word values). Instrctions that are
not listed do not change any flag.

The Z and N flags are always defined as follows (`r` being the result of the
operation):

    Z    r == 0
    N    s(r) < 0

The carry and overflow flags are defined by the following table. (All
comparisons are signed, except when operands are surrounded by "u()").

    Operation           C                     V
    -----------------------------------------------------------------------
    add x y             y && u(x) >= u(-y)    x ^ y >= 0 && x ^ (x + y) < 0
    sub x y             u(x) < u(y)           x ^ y <  0 && x ^ (x - y) < 0
    cmp x y             u(x) < u(y)           x ^ y <  0 && x ^ (x - y) < 0
    shift left x n      (x >> (n-1)) & 1      (not changed)
    shift right x n     (x << (n-1)) < 0      (not changed)
    or x y              0                     (not changed)
    and x y             0                     (not changed)
    xor x y             0                     (not changed)
    asr3 r x n          (x >> (n-1)) & 1      (not changed)

Three-operand instructions perform the same calculations as their two-operand
counterparts (ie, the calculate the same `r`) and thus have the same behavior.
