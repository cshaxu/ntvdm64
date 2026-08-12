# First OpenNT Owner Transaction Closure Audit

## Result

The original source route is behaviorally evidenced but is **not a portable
runtime closure**:

```text
C4 C4 50 11
  -> BIOS[50h]
  -> MS_bop_0
  -> DemDispatch(0x11)
  -> demLoadDos
```

The existing default-disabled i386 fixture reaches and returns from precisely
that chain, including the original NTDOS image bytes at historical
`DI:0000`. It remains a trace fixture, not a Bochs runtime candidate.

## Why the original fixture cannot be promoted

The fixture target in `src/opennt/CMakeLists.txt` includes:

- archived CCPU monitor/SAS foundation and effective-address reconstruction;
- the complete original BIOS table plus BIOS provider objects;
- original SIM32, NT BOP, DEM/CCPU/SAS, XMS, UMB, EMM, and host lifecycle
  object families;
- video, disk, comms, sound, key/mouse, system, support, and platform
  providers; and
- the full original DEM dispatcher table.

This is the historical product composition that the adopted-Bochs design
explicitly excludes. In particular, `DemDispatch` materializes all 73
`apfnSVC[]` function addresses even when input is service `0x11`; retaining
only `demLoadDos` or replacing the table would not execute the original
dispatcher.

## Exact first-service behavior

`MS_bop_0` reads the service byte at current `CS:IP` through
`Sim32GetVDMPointer`, calls `DemDispatch`, increments IP by one, and then
performs its idle lifecycle action. `demLoadDos` receives its destination in
`DI:0000`, appends `\\ntdos.sys` to a mutable historical path, opens it,
performs repeated 16 KiB host reads directly into the historical guest
pointer, and terminates the VDM on open/read failure.

The source has no all-or-nothing memory publication, checked Bochs RAM
aperture, typed state writeback, or bounded modern-host failure result. Those
were provided by the historical CCPU/SAS/NT4 host composition, not by the
service source alone.

## Boundary consequence

The completed Bochs/adapter work can establish the exact owner identity, but
cannot legally call this original implementation: doing so would either import
the rejected CCPU/SAS product graph or substitute its state and memory
providers. Neither is a small link adjustment.

The next executable unit, if admitted, must be a named **adapter-owned NTDOS
load transaction** with a source-derived contract, not a falsely labelled
original `MS_bop_0` or `DemDispatch` call. It would need, at minimum:

1. an immutable, pre-opened BYOB NTDOS input;
2. a checked full-image ordinary-RAM destination computed from captured DI;
3. an all-or-nothing Bochs guest-memory publication transaction;
4. explicit, bounded failure/stop reporting; and
5. a distinct CPU-state/IP result protocol after the historical three-byte
   boundary and service byte have been consumed.

That is a limited replacement for this one unavailable composition boundary,
not recovery of the complete DEM/BIOS product. It needs explicit owner
approval and its own contract before code. It must not grow into a general
service dispatcher, DOS filesystem, or device implementation.

## Evidence

- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c` (`MS_bop_0`);
- `src/opennt/base/mvdm/dos/dem/demdisp.c` (73-entry table and dispatch);
- `src/opennt/base/mvdm/dos/dem/demmisc.c` (`demLoadDos`);
- `src/opennt/CMakeLists.txt` (`ntdos64-opennt-original-demload-dos-byob-fixture`);
- `src/opennt/local/trace/fixtures/original_demload_dos_byob_fixture.c`; and
- `docs/M61-DEMLOADDOS-BOOTSTRAP-ABI-LEDGER.md`.
