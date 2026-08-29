# M0 T310 S8 P4 — host CRT buffer ABI closure

## Scope and finding

Original `softpc.new/host/src/fprt.c` sends a private CRT buffer through
`DWORD` before modifying its terminating byte.  That private host pointer
truncates on x64.  Its `strlen` count is also assigned to the source's signed
`int` after the preceding `nt_fgets` bounded it by the same signed `len`.

## Disposition

`MVDM-HOST-DIV-070` preserves the original direct buffer mutation and return
order.  The buffer remains native-width; it is neither guest memory nor an
external identity and therefore must not use the mapping manager.  The count
has an explicit bounded conversion to its original `int` representation.

## Verification

Forced formal Ninja object builds completed for both architectures:

- `build/M0-T310/S8/p1-machine-source/x64/fprt-forced-x64.log`
- `build/M0-T310/S8/p1-machine-source/x86/fprt-forced-x86.log`

Both logs clear the selected C4311/C4312 and x64-only C4267 diagnostics.
Unrelated source warnings remain visible.
