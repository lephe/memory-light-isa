# Emulator Memory Layout

The emulator's memory is laid out as follows:

```
     0000 +-------------------+
          |      Program      |
          +    -----------    +
          |       Stack       |
    STACK +-------------------+
          |    Video memory   |
     DATA +-------------------+
          |       Data        |
      END +-------------------+
```

The program data is loaded at the beginning of the memory when the emulator
starts. The stack pointer starts at address STACK and moves towards lower
addresses. The video memory starts immediately at address STACK. The data
segment is located after the video memory and takes up the end of the memory.

The size of the memory and the location of the STACK address can be changed at
runtime using the following command-line arguments:

```
  --stack-addr   <addr>
  --memory-size  <size>
```

Both `<addr>` and `<size>` may be expressed in bits (eg. 256), in kibibits (eg. 8k)
or in mibibits (eg. 2M).

The video memory holds 16 bits of information for each of the 256 * 256 pixels
of the screen, thus its size is fixed to 1M. The default configuration for the
emulator is to have four segments of 1M each.

The four counters are initialized at startup with the following values:

```
    PC    0x000000
    SP    STACK
    A0    DATA
    A1    DATA
```