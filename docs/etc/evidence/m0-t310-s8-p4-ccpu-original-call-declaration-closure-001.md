# M0 T310 S8 P4 — original CCPU call-declaration closure

## Scope

- Original CCPU mirror units: `zfrsrvd.c`, `c_page.c`, and `c_reg.c`.
- Prior diagnostics: implicit/unprototyped calls to `d_mem`, `limit_check`,
  `FLDENV`, `check_D`, `printf`, `INTx`, and `do_intrupt`.
- This is an internal original-CCPU declaration recovery only. It neither
  changes CPU/FPU algorithms nor introduces a machine adapter, mapping token,
  guest pointer, or provider replacement.

## Source-shaped recovery

- `zfrsrvd.c` now includes the already-selected `c_addr.h`, `c_intr.h`, and
  `intx.h`, then declares the existing `fpu.c` `FLDENV` body with its original
  `VOID *` parameter shape.
- `c_page.c` imports `yoda.h`, preserving the original `PROD` selection in
  which `check_D` is a no-op macro rather than an implicit external call.
- `c_reg.c` imports the standard CRT declaration for the existing diagnostic
  `printf` call.

## Verification

Formal MSVC `/MT` Ninja object builds on both x64 and x86 rebuilt all three
translation units. The named implicit-call diagnostics are absent on both
architectures. Remaining fixed-width guest narrowing, old-style source and
non-width diagnostics remain visible and unmodified.

## Disposition

`MVDM-HOST-DIV-082` is a registered minimal include/declaration recovery.
The existing definitions, calling order, failure paths, x87 state transitions,
interrupt route and production debug disposition remain original.
