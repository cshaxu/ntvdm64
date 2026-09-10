# M0 T310 S8 P4 — Storage-controller call-contract audit

## Scope

This review covers the selected original SoftPC floppy-controller sources in
`mvdm-host`: `base/disks/floppy.c` and `base/disks/floppy_i.c`.  It only
classifies x86/x64 representation and call-contract issues; ordinary source
diagnostics remain visible and are not suppressed.

## Findings

- Both sources include the selected original `host_def.h` declaration
  carrier.  That carrier already publishes `void host_simulate(void)`, the
  exact controller re-entry contract selected for the current SoftPC/CCPU
  build.
- Clean formal x86 and x64 rebuilds of both objects emit no `C4013` for
  `host_simulate`.  The two historical undefined-call records are therefore
  stale and are closed as `closed-existing-host-simulate-declaration`.
- The 36 `C4431` records are original K&R-style declarations/functions whose
  implicit `int` return value is not consumed as a host pointer or native
  object.  They occur identically on both architectures and do not change
  pointer width, parameter layout, or calling convention.  They are retained
  visibly as `not-x86-x64-retained-source-warning`; no mirror body was
  modified.

## Verification

The formal Ninja graph was cleaned and rebuilt outside the sandbox for:

```
ninja -C build/M0-T310/S8/p1-machine-source/x64 obj/disks/floppy.obj obj/disks/floppy_i.obj
ninja -C build/M0-T310/S8/p1-machine-source/x86 obj/disks/floppy.obj obj/disks/floppy_i.obj
```

Both architecture builds succeeded.  Remaining diagnostics are retained
historical macro, default-int, range, unused-value and dataflow warnings; this
audit does not misclassify them as 64-bit ABI repairs.
