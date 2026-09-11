# M0 T404 S1 WOW hard-error recovery evidence

## Scope and provenance

S1 investigated the first reached original owner on the selected x86
`WRITE.EXE` startup path: `softpc.new/host/src/nt_error.c:WOWpSysErrorBox`.
That owner makes one fixed `NtRaiseHardError` request:

```
STATUS_VDM_HARD_ERROR | 0x10000000, 4, (1 << 2) | (1 << 3), parameters, 0
```

The four parameters are three `SEB_*` button descriptors followed by two
Unicode strings.  The original receiver is a private hard-error/CSRSS route,
which is unavailable in the standalone host.

## Recovery disposition

1. **Original source reuse:** `WOWpSysErrorBox` remains the reached original
   owner and its request layout, button ordering, and ordinal response rule
   remain unchanged.  Its private receiver cannot compose in this host.
2. **Smallest same-shaped facade:**
   `adapter-mvdm-host-out/win32/source/wow_hard_error_dialog.c` accepts only
   that exact request shape, validates and copies its bounded data, presents
   the offered buttons through public Win32, and returns only original
   ordinals 1--3.  The declaration is
   `adapter-mvdm-host-out/win32/include/wow_hard_error_dialog.h`.
3. **External-code intrusion:** none.  The original WOW owner and call site
   were not changed.
4. **New behavior:** none beyond the bounded transport to the public dialog;
   unmatched `NtRaiseHardError` requests retain the existing explicit
   unavailable-boundary behavior in `wow_private_unavailable.c`.

No guest pointer, HWND, private USER/CSRSS endpoint, broker connection, or
generic reply policy crosses this seam.

## Build and runtime evidence

The formal x86 graph rooted at
`build/M0-T404/S1/r033-wow-hard-error-x86/` compiled the new adapter and
linked `original-softpc-process.exe`.  The final PE is x86 (`14C`) with
SHA-256:

```
E9AC6BED67AB9E569946EF397E268807DED3D3559287BBCF3E026B47453EE802
```

The focused runtime observation (`O:\ntvdm64\logs\T404-r034-wow-hard-error`)
showed the public `16 bit Windows Subsystem` dialog.  A subsequent direct
runtime report (`O:\ntvdm64\logs\T404-r036-direct-report-next-frontier`)
reached normal configuration, DEM opens for `COMMAND.COM` and `DOSX.EXE`,
and later observed:

```
MVDM-CPU-RETF source=D20B:0006 target=0000:0000
```

The report contains no `WRITE.EXE`, `KRNL386`, or successful WOW32 load
marker.  The process was stopped by the bounded observer after eight seconds;
no dialog response was synthesized and S1 makes no claim that an operator
selected a response.  The empty r035 observer directory is rejected as
evidence.

## Current result and next boundary

S1 has closed the private hard-error receiver as the immediate blocker in its
technical investigation, but its governed delivery and the next S admission
remain pending.  It does not close the selected application lifecycle.  The
next owner boundary is the producer and integration contract behind the
`D20B:0006` return.  It is not a proven CPU or MVDM semantic defect: the
default hypothesis for a future S2 is a missing or misordered standalone
`CreateProcess`/BaseSrv/worker integration input.  Any MVDM semantic change
requires a source-equivalent integration ledger and an exact demonstrated
mismatch first.
