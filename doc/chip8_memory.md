# Memory layout for CHIP-8 emulation program

Starts in the data segment, usually at 512k. Addresses here are relative to the
data segment.

    Address    Size     Content          Real address
    -------------------------------------------------
    0          32768    CHIP-8 memory    524288
    -------------------------------------------------
    32768      128      v0 .. vf         557056
    32896      16       PC               557184
    32912      16       I                557200
    -------------------------------------------------
    32928      256      Stack            557216
    33184      16       SP               557472
    -------------------------------------------------
    33200      16       Key buffer       557488
    -------------------------------------------------
    33216      32       RNG seed         557504

The emulated file has to be loaded at address DATA + 0x200 (which is usually
512k + 512) before starting emulation.
