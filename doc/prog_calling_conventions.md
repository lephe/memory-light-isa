# Program Calling Conventions

These are the conventions that should be respected when a function call occurs.

## Caller rules

- The caller saves `r0` through `r3` if it intends to use their value after the
  call ends.
- The caller places the first arguments in registers `r1`, `r2` and `r3`. If
  there are more arguments, they are pushed to the stack *in reverse order*.
- After the call, the caller reads the return value in register `r0`, and all
  arguments that were on the stack have been popped.

## Callee rules

- The callee can freely use registers `r0` to `r3`. The first three arguments
  are read from `r1`, `r2` and `r3`, and others are popped from the stack in
  natural order.
- The callee must read all of the arguments that are pushed to the stack. SP
  must point to the first byte after all arguments when the call ends.
- The callee saves the values of `r4` through `r7`, `a0` and `a1` before using
  them, and restores them at the end of the call.
- The callee places the return value in register `r0`.

## Argument passing

- The size of pushed arguments is decided by the prototype of each function. It
  is acceptable to always blindly push 64-bit values.
