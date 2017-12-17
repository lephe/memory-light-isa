# Memory layout for CHIP-8 emulation program

Starts in the data segment, usually at 512k. Addresses here are relative to the
data segment.

    Offset     Size     Content              Address
    ------------------------------------------------
    0x0000     32768    CHIP-8 memory        0x80000
    | 0x0000   4096     Font data            0x80000
    | 0x1000   28672    Progra memory        0x81000
    ------------------------------------------------
    0x8000     128      v0 .. vf             0x88000
    0x8080     16       PC                   0x88080
    0x8090     16       I                    0x88090
    ------------------------------------------------
    0x80a0     256      Stack                0x880a0
    0x81a0     16       SP                   0x881a0
    ------------------------------------------------
    0x81b0     16       Key buffer           0x881b0
    ------------------------------------------------
    0x81c0     32       RNG seed             0x881c0
    ------------------------------------------------
    0x81e0     8        Delay timer          0x881e0
    0x81f0     8        Audio timer          0x881f0
    ------------------------------------------------
    0x8200     64       HP48 flags           0x88200
    ------------------------------------------------
    0x8240     8        Frequency timer      0x88240
    0x8248     8        Frequency counter    0x88248

The emulated file has to be loaded at address DATA + 0x1000 (which is normally
0x81000) before starting emulation.

## Video memory

The emulated CHIP-8 program can use the video memory. The screen is cleared to
black when the game starts and the emulator uses a screen of 64 * 32 centered
on the 160 * 128 graphical window. Which gives a VRAM area with the following
properties:

    Address:  0x114300
    Depth:    16-bit per pixel, black = 0x0000, white = 0xffff
    Stride:   0xa00 bits per line

