# Troubleshooting emulator issues

A few possible issues with the debugger are known. On a fully-configured recent
system, they should not happen, but remedies have been found anyway.

## `undefined reference to symbol 'acs_map'` when building

This issues seems to be cause by binutils not automatically linking library
dependencies of dynamic libraries. In this case, `libncurses` depends on
`libtinfo` (which is the `terminfo` program), which is missing. To solve this
issue, you may try the `USE_LIBTINFO` switch when making:

    $ make USE_LIBTINFO=1

This `libtinfo` library does not exist on all platforms (it may also be a
symlink to `libncurses`), which is why this switch is not enabled by default.

## Garbled interface when starting the debugger

This one is most probably a configuration issue or a misunderstanding between
the `terminfo` entry and the actual terminal. Check your `$TERM` variable and
set it to a value supported both by ncurses and the terminal emulator. For
instance:

    $ TERM=xterm-color ./emu ../prog/mult.bin
