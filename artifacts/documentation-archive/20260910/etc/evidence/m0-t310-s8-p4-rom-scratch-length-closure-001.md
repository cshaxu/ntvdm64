# M0 T310 S8 P4 — ROM scratch length closure

`display_string()` writes a bounded DOS message to the original ROM scratch
pad.  Its prior direct `strlen()` assignments narrowed `size_t` into the
32-bit SoftPC linear carrier before its existing one-page and scratch-pad
checks.  `MVDM-HOST-DIV-076` preserves the original guard, truncation,
byte-write and terminator ordering while holding `endLinAddr` and the temporary
length in `size_t`; only the already-bounded final `cur_loc` update converts to
the original `sys_addr` type.

This is a native host-string length, not a guest address, host handle, or
mapping-manager identity.  Formal x64 and x86 rebuilds of `obj/system/rom.obj`
completed with zero errors and no remaining C4267/C4311/C4312 diagnostics for
`rom.c`:

- `build/M0-T310/S8/p1-machine-source/x64/rom-length-abi.log`
- `build/M0-T310/S8/p1-machine-source/x86/rom-length-abi.log`
