# M0 T417 S3--S4 overlay owner recovery

## Result

The two private production roots are absent:

- `src/mvdm-overlay`;
- `src/opennt-host-overlay`.

This is a placement recovery, not a behavior expansion.  The governing rule
is now enforced by the selected build: neither `src/mvdm` nor
`src/opennt-host` receives a new file.  A retained body is either a minimal
change to an existing original owner or a named adapter whose modern boundary
cannot compose in the mirror.

## Per-body disposition

| Former body | Final owner | Disposition |
| --- | --- | --- |
| `localfm.c` | existing MVDM `ccpu386/localfm.c` | Retains only original `Gdp`, `Cpu`, and `Video` shared-state declarations.  `Sas` remains uniquely owned by selected `ccpusas4.c`. |
| `sas_overwrite_memory.c` | existing MVDM C-VID `accessfn.c` | Deleted.  The selected original C-VID carrier already exports `sas_overwrite_memory` and forwards through `Sas.Sas_overwrite_memory`; a second wrapper caused a duplicate symbol and was removed. |
| C-VID vector binder `.c/.h` | named SoftPC adapter | The selected source union has metadata and consumers but not the historical generated binder translation unit.  The adapter only publishes existing CCPU/SAS/video providers. |
| EOI bridge `.c/.h` | existing MVDM `host/src/nt_eoi.c` | The original public `int *` carrier is converted locally to the selected `IS32` PIC carrier and copied back. |
| `monitor_printer.c` | named monitor adapter | Exact reached printer subset remains outside the mirror because its original monitor package is kernel-VDM code and no existing standalone mirror owner can compose it. |
| `dos_worker_wait.inc` | existing OpenNT-host `base/win32/server/srvvdm.c` | Inlined as `BaseSrvDOSWorkerWaitPending`; original BaseSrv DOS lock, record and event stay the only state owners. |

## Accounting

The removed roots contained 10 tracked files and 325 physical lines, including
37 README lines.  Their eight MVDM production files and one OpenNT-host
production fragment contained 288 source/header lines.  195 source/header
lines are retained as named adapter bodies (C-VID binder and monitor-printer);
the remaining behavior was merged into existing owners or deleted.  In
particular, the 16-line duplicate SAS facade is gone rather than relocated.

The metric distinguishes root removal from code deletion: moving a body is not
reported as a net code saving.  The concrete savings are the two root
components, their wrapper/readme overhead, the deleted duplicate SAS facade,
and all now-dead build-root checks.

## Verification

- Fresh graph: `build/M0-T417/S4/formal-x86-003`, x86 CCPU40, no reference to
  either retired root.
- All affected owners compiled: `localfm`, `ccpusas4`, `nt_eoi`, the C-VID
  adapter binder, monitor-printer and OpenNT BaseSrv `srvvdm`.
- Fresh-graph products exist: `run16.exe`, `basesrv.exe`, `ntvdm.exe`, and
  `ccpu-halt-reset-test.exe`.  The CCPU fixture passed and reported
  `original-CCPU HALT RESET: AX=beef producer=0`.
- `Verify-T411CvidcVectorContract.mjs` passed with 154 public and 55 private
  slots; `Test-T415FinalRootElimination.ps1` passed; documentation governance
  passed.
- A first formal relink exposed duplicate `sas_overwrite_memory`; the
  source-owner correction above was applied, then the changed CCPU archive,
  worker link and fixture link were rebuilt successfully.  This is evidence
  that the retired wrapper was redundant, not an unresolved build limitation.
