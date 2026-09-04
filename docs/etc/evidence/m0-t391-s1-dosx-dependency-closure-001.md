# M0 T391 S1 — reached DOSX dependency closure

## Question

What is the complete original owner and media dependency chain reached by the
default `AUTOEXEC.NT → DOSX.EXE` route, and can it be treated as a pure-DOS
extender workload without admitting WOW16 bootstrap?

## Inputs

- Current default guest script:
  `src/mvdm-guest/bin86/autoexec.nt`.
- Original MVDM sources: `mvdm-host/dpmi/486/dxboot.asm`,
  `mvdm-host/dpmi32/dpmi32.c`, `mvdm-host/softpc.new/host/src/nt_bop.c`,
  `mvdm-host/softpc.new/host/src/nt_pif.c`, and
  `mvdm-host/dos/command/cmdconf.c`.
- Current stage contract: `O:\ntvdm64\runtime-manifest.json` and the files
  below `O:\ntvdm64`.
- Prior fixed observations:
  [T378 S1–S5](m0-t378-s1-dos-extender-workload-owner-admission-001.md),
  [T389 S8](m0-t389-s8-child-command-reentry-boundary-001.md), and M0 T390.

## Procedure

1. Read the selected `AUTOEXEC.NT`, original PIF/config expansion and
   original DPMI guest/host transition without changing a guest, provider or
   machine source file.
2. Compare every media dependency reached before protected-mode client entry
   with the current runtime manifest and installed `O:\ntvdm64` stage.
3. Re-read the DOSX initialization and its source-owned child selection to
   distinguish a generic DOS-extender client from the selected Windows/DPMI
   bootstrap carrier.

## Observations

1. The unmodified `AUTOEXEC.NT` loads original `REDIR` and then original
   `%SystemRoot%\system32\DOSX`; original `GetPIFConfigFiles` and
   `cmdconf.c:ExpandConfigFiles` retain the selection/expansion ownership.
   App only supplies the selected executable-relative system root.
2. The current stage has byte-identical original `HIMEM.SYS`, `REDIR.EXE` and
   `DOSX.EXE`.  `src/mvdm-host/dpmi/486/dosx.exe`,
   `build/output/dos/DOSX.EXE` and `O:\ntvdm64\system32\DOSX.EXE` are each
   36,616 bytes with SHA-256
   `c5af29a29abf167b243daabf877459e8278b8c9a339bf8e1e2576ead5f6ceeff`.
3. The same stage **does not contain** `SYSTEM.INI` at its system root or
   `KRNL386.EXE` under `system32`.  The selected immutable originals exist at
   `src/mvdm-guest/bin86/system.ini` (219 bytes, SHA-256
   `6f533ccc79227e38f18bfc63bfc961ef4d3ee0e2bf33dd097ccf3548a12b743b`)
   and `src/mvdm-guest/wow16/kernel31/retail3/krnl386.exe` (85,824 bytes).
   Neither is a current runtime-manifest input.
4. Original `dxboot.asm` calls `GetExeName`, reads `SYSTEM.INI`, requires the
   existing HIMEM service, establishes DPMI tables, then calls
   `ParseCommandLine` and `FindFile`.  In the selected Windows conditional it
   replaces the child name with `krnl?86.exe` before `FindFile`.  This is not a
   generic arbitrary-DOS-extender workload selection.
5. The earlier fixed source-owned observation independently saw the resulting
   `SYSTEM.INI` and `KRNL386.EXE` search after opening `DOSX.EXE`; it did not
   reach `MS_bop_3`, `53:xx`, or a protected-mode client.  Thus the missing
   assets/control path, rather than an isolated DPMI BOP leaf, is the first
   unresolved whole-owner cohort.
6. When a DPMI guest does issue the historical monitor BOP, original
   `nt_bop.c:MS_bop_3` directly calls original `DpmiDispatch`; the retained
   25-entry `DpmiDispatchTable` includes `DpmiInitDosx`.  This host side is
   already selected in the CPU40 formal graph, but its formal link alone is
   not runtime proof of guest protected-mode entry.

## Original-owner chain and disposition

```text
app system-root selection
  → original PIF/config expansion
  → CONFIG.NT: HIMEM.SYS → original XMS/A20/UMB/INT15 cohort
  → AUTOEXEC.NT: REDIR.EXE → DOSX.EXE
  → DOSX real-mode setup / SYSTEM.INI
  → selected Windows child KRNL?86.EXE discovery
  → (only after that source-owned transition) MS_bop_3 → DpmiDispatch
```

`HIMEM`, XMS and the host DPMI32 table remain original MVDM owners.  DOSX,
SYSTEM.INI and the selected `KRNL386.EXE` are immutable guest/media owners;
they must not be recreated by app or an adapter.  `Sim32GetVDMPointer` uses
the existing bounded machine mapping lease only at its established historical
host boundary; selectors, BOP bytes and guest addresses remain original
numeric values.

## Interpretation and confidence

High confidence: the former queue wording incorrectly implied that this
default `DOSX.EXE` path can prove a pure-DOS extender workload while Win16 is
deferred.  The selected original carrier is a DPMI/**WOW bootstrap** path and
has direct `SYSTEM.INI`/`KRNL386.EXE` prerequisites.  The current stage lacks
those immutable inputs.  Adding an app parser, a synthetic client, a direct
`53:xx` success path, or a CPU change would violate the owner chain.

## Follow-up and stop disposition

T391 S1 is closed as an audit.  The active T must pause for owner-approved
replanning:

- if pure DOS plus native-child execution remains the priority, move original
  `cmdExec32` stage-to-return ahead of this package and defer DOSX/WOW;
- if the DOSX default route is retained as next priority, revise this package
  as a combined original DOSX/DPMI/WOW bootstrap admission, first stage the
  exact selected `SYSTEM.INI` and `KRNL386.EXE` media, then recover the
  complete source-owned bootstrap rather than a DPMI leaf.

No source, guest asset, formal build selection or runtime behavior changed by
this S.
