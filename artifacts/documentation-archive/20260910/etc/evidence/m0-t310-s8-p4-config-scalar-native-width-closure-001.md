# M0 T310 S8 P4 — original configuration scalar native-width closure

## Scope

This closure covers the original SoftPC `config_inquire()` scalar consumers in
the selected BIOS, ROM, CMOS, keyboard, video and host-graph packages.

## Source contract

`config_inquire()` intentionally returns `void *`: some option IDs return
object/string addresses, while option IDs such as `C_GFX_ADAPTER`, `C_LIM_SIZE`
and `C_WIN_SIZE` historically encode a small scalar in that carrier.  NT4 x86
could cast the carrier directly to `ULONG` or `SHORT`; x64 reports C4311 when
that private carrier is first narrowed as a pointer.

These values are neither guest addresses nor host-resource identities.  They
must not enter the session mapping manager.

## Resolution

`MVDM-HOST-DIV-075` retains the original `config_inquire` interface and each
original option/destination.  The reached scalar consumers now convert through
`ULONG_PTR` before their original narrow enumerated destination.  Real pointer
consumers remain untouched.

## Verification

The formal Ninja graphs rebuilt the complete selected cluster successfully:

- x64: `build/M0-T310/S8/p1-machine-source/x64/config-scalar-abi.log`
- x86: `build/M0-T310/S8/p1-machine-source/x86/config-scalar-abi.log`

The repaired reset and ROM C4311 diagnostics no longer occur.  Remaining
`rom.c` C4267 entries describe bounded ROM string length conversion and remain
visible for their separate x64-size audit; no warning is suppressed here.
