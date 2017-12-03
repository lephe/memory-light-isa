# Emulator Memory Layout

The emulator's memory is laid out as follows:

     0000 +-------------------+
          |      Program      |
    STACK +-------------------+
          |       Stack       |
     DATA +-------------------+
          |       Data        |
     VRAM +-------------------+
          |    Video memory   |
      END +-------------------+

The program data is loaded at the beginning of the memory when the emulator
starts. The stack pointer is initialized with address DATA and moves up towards
lower addresses. The data area starts at address DATA after the stack. The
video memory starts immediately at address VRAM.

The geometry of the memory can be customized at runtime using the --geometry
command-line argument to specify the size of each segment:

    --geometry <text>:<stack>:<data>:<vram>

All four arguments are sizes in *bits*. All of them must be multiples of 64,
and a compatibility warning is issued the video RAM starts at any other address
than 0x10000. Sizes may be expressed in bits (eg. 256), in kibibits (eg. 8k),
or in mibibits (eg. 2M). The default configuration is equivalent to:

    --geometry 32k:16k:16k:327680

The standard video memory is a buffer of 160 * 128 pixels with 16-bit depth,
thus its size is fixed to 327680 bits.

The four counters are initialized at startup with the following values:

    PC    0x000000
    SP    DATA
    A0    DATA
    A1    VRAM

To be truly compatible with the variable memory layout, programs could only
rely on these values. Programs in the /prog and /chip8 directory do not do this
and will break if the memory geometry is changed carelessly.
