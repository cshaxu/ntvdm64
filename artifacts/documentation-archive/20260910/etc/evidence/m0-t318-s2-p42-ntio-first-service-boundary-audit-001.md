# M0 T318 S2 P42 — NTIO first-service boundary audit

## Purpose

Close the source-defined bridge between the CCPU40 first fetch and the existing
original `SVC_DEMLOADDOS` owner package. This prevents a startup observation
from being misclassified as a COMMAND, later BOP, or guest EXEC result.

## Entry identity

The selected staged `NTIO.SYS` remains the original guest artifact. Its
adjacent original `doskrnl/bios/ntio.map` identifies public `init` at
`0000:0370` in the `BIOS_DATA_INIT` segment. The source body is
`doskrnl/bios/msinit.asm::init`; this agrees with the selected image's map
identity and the host-side original CS:IP load sequence established in P32.

## First guest-owned cohort

Before its first service request, `msinit.asm::init` performs only original
guest/machine initialization:

1. saves the original ROM interrupt vectors into the IVT;
2. installs original `INT 15h` and `INT 19h` vectors;
3. creates the local real-mode stack at `0000:0700`;
4. initializes the original break, character-output and interrupt-vector
   entries; and
5. derives the DOS data segment and the `dos_load_seg` target.

The source explicitly leaves serial and printer initialization disabled in the
historical NTVDM profile. It performs no COMMAND acquisition, no NTDOS EXEC,
no BaseSrv/CSR request, and no invented adapter call in this prefix.

The first cross-owner transition is the original macro invocation:

```
SVC SVC_DEMLOADDOS     ; DI is segment to load DOS
```

The adjacent original comment states that a failure does not return. The
existing original `mvdm-host/dos/dem/demdisp.c` dispatch table maps that service
to `demLoadDos`, and `demmisc.c::demLoadDos` is the existing original NTDOS
loader owner. This is the same route frozen by P12 and T318 S1; P42 only
proves that it is the first NTIO host-service boundary.

## Disposition

No guest source, service route, host provider, or machine behavior changes.
The next valid runtime marker after fixed-container proof of NTIO reachability
is this already-selected `SVC_DEMLOADDOS` transition. NTDOS/EXEC evidence
remains later work and must not be inferred from this static boundary.
