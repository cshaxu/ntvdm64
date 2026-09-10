# M0 T359 S2 P1 — COMMAND transient SAS-write binding

## Purpose

The paused T358 run reaches `03F4:2171`, an immutable `COMMAND.COM` help-text
offset, before `54:01`.  T359/S1 identifies the preceding original owner as
the complete resident-to-transient lifecycle, not an individual BOP result.
This record supplies the smallest source-shaped observation needed to prove
whether that lifecycle writes its original control words and copy destination.

## Original control contract

- `mvdm-guest/dos/v86/cmd/command/rdata.asm::EndInit` obtains the largest DOS
  block, calculates and writes `TrnSeg`, reverse-copies the transient and
  transfers to `LodCom_Trap`.
- `mvdm-guest/dos/v86/cmd/command/command2.asm::{LodCom,LodCom1}` reallocates
  or checks/reloads the transient through ordinary INT 21h file paths.
- `command2.asm::HavCom` copies `DATARES:TranVars` into
  `TrnSeg:TRANGROUP:HeadCall`; the first word is the resident `HeadFix_Trap`
  continuation defined in `rdata.asm`.
- The selected CPU40 SAS owner is the original
  `mvdm-host/softpc.new/base/ccpu386/ccpusas4.c`.  Its original byte, word and
  dword entrypoints complete `bios_write_*` before returning.

The existing selected-image recorder observed only `c_sas_store`, so it could
not see `mov TrnSeg,ax` when CPU40 takes `c_sas_storew`.  It also described a
specific CONFIG slot although the recovery question is a general source-owned
SAS write.

## Bound binding

`MVDM-HOST-DIV-184` now retains the original completed store first, then calls
the one adapter-owned, default-off function
`mvdm_softpc_record_sas_store(address, width, value)` from each of
`c_sas_store`, `c_sas_storew` and `c_sas_storedw`.

The function is disabled unless both `MVDM_SAS_STORE_REPORT_PATH` and
`MVDM_SAS_STORE_LINEAR` are supplied.  `MVDM_SAS_STORE_LENGTH` is optional
and defaults to one byte.  It records at most eight scalar records whose
half-open write range intersects the configured half-open watch range.
It does not acquire a guest lease, translate an address, retain a pointer,
read or write guest memory, alter CPU state, affect a mapping, or participate
in BOP routing.  The current console observer preserves a caller-supplied
watch address and otherwise uses its previous `commnd` default; it remains an
observer, not a product ABI.

Thus one generic, selector-blind SAS boundary can observe both the existing
CONFIG write and the T359 `TrnSeg`/transient-control writes without adding a
CPU instruction trace or COMMAND-specific implementation.

## CCPU/physical-mapping disposition

The same source review finds the only earlier CCPU divergence that can affect
physical lookup: `c_GetPhyAdd` first asks the session mapping manager for an
existing external page binding, then retains the original ordinary-RAM path.
The provider is `nt_mem.c::VdmAddVirtualMemory`; its original comment and
callers identify DIB/EMS external-page use, not COMMAND initial allocation or
the ordinary NTDOS file path.  No S2 code makes that mapping active.  The
current binding therefore observes ordinary transient writes without changing
that adapter's disposition; later DIB/EMS/WOW work remains its owner package.

## Formal source/build evidence

The fresh generated CPU40/x86 graph at
`build/M0-T359/S2/formal-x86` selected the original CCPU, host roots, DEM,
COMMAND, XMS and DPMI source sets and linked:

```text
ninja -C build/M0-T359/S2/formal-x86 original-softpc-process.exe
```

Result: success (`428/428` actions).  Product SHA-256:

```text
B8B852035E5ADC6585C0DEE05DACCE3130350582F2512DA116C193491D9D9B27
```

Compiler warnings from the existing original source selection remain visible;
this P neither suppresses nor reclassifies them.  No runtime observation was
run in S2: the renewed immutable-stage execution remains the separately
bounded S4 action.

## Remaining S2 work

Use the established source/caller map and this generic recorder to define the
single local normal/failure test boundary for `EndInit → LodCom → HavCom` and
its NTDOS prerequisite cohort.  S2 must not claim that the transient lifecycle
is already repaired merely because the formal product links.
