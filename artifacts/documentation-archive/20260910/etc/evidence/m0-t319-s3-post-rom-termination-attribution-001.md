# M0 T319 S3 — Post-ROM termination attribution

## Scope

This record attributes only the fixed-container termination observed after
T319/S2 selected the original CPU40 ROM-residency body.  It does not change
FDC, PIC, CCPU, BOP, guest, media, or the observation container.

## Minimal attribution seam

The original `softpc.new/obj.vdm/ntvdm.c` top-level retains its original
`try/except(VdmUnhandledExceptionFilter(...))` and `return ret` flow.  One
registered divergence, `MVDM-HOST-DIV-166`, records that already-selected
`ret` only when the fixed observer supplies
`MVDM_MAIN_RETURN_REPORT_PATH`.  It cannot resume, alter SEH disposition,
change a thread lifetime, or alter guest state.

The existing `VdmUnhandledExceptionFilter` continues to write its original
received exception identity through the pre-existing
`MVDM_EXCEPTION_REPORT_PATH` observation seam before its original
`NtTerminateProcess` disposition.  The observer sets both child-only
environment variables, then restores its own environment immediately after
`CreateProcess`.

## Formal build closure

Fresh generated CPU40 Ninja graphs completed and produced both normal product
executables:

- `build/M0-T319/S3/termination-x86/original-softpc-process.exe`
- `build/M0-T319/S3/termination-x64/original-softpc-process.exe`

The builds retain the selected source profile and original ROM recovery from
T319/S2; no `/FORCE` link or replacement executor was used.

## Fixed x86 observation

The x86 product was staged with the same fixed guest media and firmware set
used by S2, then launched once through the existing console-owning,
non-debug observer with the unchanged arguments:

```text
original-softpc-process.exe -f -o --ordinary-child
```

Observed report:

```text
container=console-owning-nondebug
result=exited
exit=0xc0000005
timeout-ms=8000
```

The console retained the same reachable startup markers:

```text
MVDM-BOP-DISPATCH 50:11
MVDM-BOP-DISPATCH 50:3B
MVDM-BOP-DISPATCH 50:0F
MVDM-BOP-DISPATCH 50:1B
... 54:05
```

Neither the main-return report nor the existing known-thread exception report
was created.  Therefore this terminal result is not a normal return through
original `ntvdm.c::main`, and it is not a fault observed by the selected
`VdmUnhandledExceptionFilter` path.

## Narrow disposition

The remaining owner is the original CPU40 machine execution / worker cohort
after COMMAND registration, not a BOP provider, guest-image change, or
top-level host return.  The static thread audit shows the original host also
has raw `CreateThread` users (event, heartbeat, floppy, detection and comms)
beside the known-thread filter wrapper.  This evidence cannot distinguish an
uncovered worker fault from a direct raw-CCPU termination without adding a
new observer or changing machine behavior; both are outside S3.

T319 therefore closes its stated ROM-residency and fixed-container attribution
scope with the `0xc0000005` result preserved.  Any further work must be
admitted as a source-shaped CPU40 execution/worker integration task, with a
specific original cohort selected before changing its behavior.
