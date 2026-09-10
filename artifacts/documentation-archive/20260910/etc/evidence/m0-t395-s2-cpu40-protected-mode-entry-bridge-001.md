# M0 T395 S2 — CPU40 protected-mode entry bridge

Date: 2026-09-06

## Question

Can the selected original DOSX bootstrap cross its former CPU40 `53:01`
protected-mode-entry boundary and continue into later original DPMI32 service
owners without importing an `i386/` kernel-VDM provider?

## Inputs

| Item | Identity / role |
| --- | --- |
| Formal product | `build/M0-T395/S1/formal-x86/original-softpc-process.exe`, SHA-256 `28ECFDC0D47DBD89AA4A66CE662D5B4C472807E7871595B8ACB87FA096E1BE59` |
| DPMI cohort | 15 CPU40-profile C bodies: thirteen portable `sources` members plus `dpmimemr.c` and `dpmimscr.c`; `i386/dpmi386.c`, `i386/dpmimem.c`, and `i386/dpmimisc.c` are excluded kernel-VDM bodies. |
| Workload | Immutable short-root `O:\T395I`, with existing `system32\\DOSX.EXE`; the staging extension adds existing load-only `system.ini` and `system32\\KRNL386.EXE`, never a host link input. |
| Observation | Console-owning observer, 30-second bound, raw return trace retained only in disposable `build/M0-T395/S2/`. |

## Source-shaped recovery

The original `dpmi386.c::switch_to_protected_mode` is a kernel-VDM owner and
cannot be composed into CCPU40.  The smallest compatible seam is the existing
CPU40 mode-switch owner: `modesw.c` captures the source real-mode frame before
enabling PE/CPL3, then restores the same CS:EIP, SS:ESP and DS through CCPU's
active selector cache.  It deliberately does not invent kernel-VDM state.

The subsequent original DOSX sequence exposed three necessary CPU40 mechanics:

- `53:0F` publishes the DOSX LDT, which is bound to CCPU with the existing
  `Ldt`/`IntelBase` guest-linear relationship;
- `53:00` copies the source descriptor writes into that bound LDT after the
  original conversion-cache update;
- `53:11` returns a guest-linear `VDM_DPMIINFO` projection through `CX:DX`.
  This is required because DOSX installs that value as `SEL_VDMTIB` and then
  dereferences it in guest protected mode.  The projection uses existing
  shared-XMS allocation; no host pointer is published.

`53:07` likewise uses the existing CPU40 shared-XMS allocation mechanics.
The registered divergences are `MVDM-HOST-DIV-222` through `DIV-226` in
`src/mvdm-host/README.md`.

## Procedure and observations

1. Regenerated the formal x86 graph with
   `New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot build/M0-T395/S1/formal-x86`.
   Recompiled the changed DPMI objects, rebuilt `original-mvdm-dpmi32.lib`,
   and linked the named product successfully.
2. Ran the console-owning observer with declared
   `system32\\DOSX.EXE` and the immutable `O:\T395I` stage.  The retained raw
   trace has SHA-256
   `477384A2350174581B56AD259643F77FDED44BAE700A893748F38BF0E382A125`.
3. A companion `system32\\KRNL386.EXE` observation reached the same DPMI
   bootstrap family.  Its dispatch counts were `00=22`, `01=1`, `02=255`,
   `03=1`, `04=1`, `07=1`, `0A=32`, `0F=2`, and `11=2`.

The direct observed post-transition excerpt is:

```text
MVDM-BOP-DISPATCH 53:01
MVDM-BOP-RETURN 53:01 cs=00CF ip=1101 ax=0001 cf=0 if=0
MVDM-BOP-DISPATCH 53:0F
MVDM-BOP-DISPATCH 53:00
MVDM-BOP-DISPATCH 53:04
MVDM-BOP-DISPATCH 53:07
MVDM-DPMI-XMEM request=00001000 status=00000000 stage=4
MVDM-BOP-DISPATCH 53:11
```

The final observer result is `0xC0000409`, after the BIOS `INT 19h` path
(`F000:E05E` contains `CD 19`).  That is a controlled session-termination
path in this bare bootstrap observation, not evidence of a `53:01` reset.

## Interpretation and confidence

The S2 exit criterion is met: the selected DOSX workload crosses `53:01` and
executes downstream original DPMI services, including descriptor publication,
shared-XMS allocation, protected-mode TIB projection, and fault/interrupt
handler registration (`53:0A` / `53:02`).  The formal cohort contains no
kernel-VDM DPMI body.

Confidence is high for this bounded boundary.  It is not evidence that every
`53:00`--`53:18` service or a real Win16 client lifecycle succeeds.  In
particular, naked `DOSX.EXE`/`KRNL386.EXE` termination does not substitute for
an original DPMI client start-and-return proof.

## Follow-up

T395 S3 owns the complete dispatch/lifecycle audit for the still-unobserved
`53:12`--`53:18` surface and must select a non-WOW32 original DPMI client or
record the exact owner boundary before a further behavior change.
