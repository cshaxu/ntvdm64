# M0 T310 S8 P4 — original COMMAND environment native-width closure

## Scope

This closure covers every x86/x64 width diagnostic in the selected original
`dos/command/cmdenv.c` environment transformation, initialization, merge,
set, and lookup routines.  It does not change the remaining non-width source
diagnostics.

## Source contract

The original routines gather native Windows environment strings, filter and
sort them, then publish their lengths through three distinct historical
fixed-width forms:

- `UNICODE_STRING.Length` is a `USHORT` byte count;
- the `cmdGetInitEnvironment` BOP publishes a DOS `WORD` environment size;
- `VDMENVBLK` and the COMMAND helper APIs retain `DWORD` byte counts.

NT4 x86 assigned `strlen` and wide-character deltas directly to these fields.
On x64, `size_t` is wider and the same assignments produce C4267; a cast
would merely hide a possible loss of data.

## Resolution

`MVDM-HOST-DIV-118` keeps each original collection, filtering, allocation,
OEM conversion, BOP result, and error direction in `cmdenv.c`.  The new
private `mvdm_command_length` overlay performs only checked host-length
conversion at an existing `USHORT`, `WORD`, or `DWORD` boundary.  The mirror
returns through its existing `FALSE` or zero-BX failure route when the source
field cannot represent the native length.  Environment text and all guest
structures remain owned by the original COMMAND code.

These values are local byte counts, not guest addresses, host object IDs, or
external handles; the session mapping manager is intentionally not involved.

## Verification

Forced formal Ninja `/MT` rebuilds completed for the exact original source on
both hosts:

- x64: `build/M0-T310/S8/p1-machine-source/x64/command-environment-abi.log`
- x86: `build/M0-T310/S8/p1-machine-source/x86/command-environment-abi.log`

Neither log contains a `cmdenv.c` C4244/C4267 or other pointer-width ABI
diagnostic.  Existing C4996, C4701/C4703, and C4706 diagnostics remain visible
because they are not x86/x64 transport issues and this closure neither
suppresses nor rewrites them.
