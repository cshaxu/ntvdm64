# M0 T365 — DOS `.COM`/MZ `.EXE` execution and parent return

## Purpose

T364 proves original COMMAND command-record delivery and its ordinary
post-command wait.  T365 recovers the next independent owner package: original
DOS `$Exec` for low-dependency `.COM` and MZ `.EXE` inputs, including file/JFN/
SFT resolution, PSP/arena/environment construction, child termination and the
original COMMAND return direction.

## Source-first boundary

Primary owners are selected original MVDM sources in:

- `mvdm-host/dos/dem/*` for original host file/handle and process-adjacent
  services;
- `mvdm-host/softpc.new/bios/*` and selected DOS/NTDOS execution paths for
  `$Exec`, loader, MCB/arena, PSP and termination;
- `mvdm-host/dos/command/*` only at the original child-return continuation;
- the existing shared session mapping-manager lease only at an original
  `GetVDMAddr`/`Sim32GetVDMPointer` boundary.

The task must retain original loader layout, DOS error direction and parent
return ordering.  It may use existing same-shaped DEM/SoftPC/Win32 bindings,
but cannot add an app loader, copy a guest executable into host code, parse
the executable itself, fabricate a BOP result, or use a second CPU backend.

## S plan

1. **S1 — Original EXEC/parent-return owner ledger.**  Trace `$Exec` and the
   selected low-dependency `.COM`/MZ branches through JFN/SFT, DEM file
   providers, PSP/MCB/arena/environment, relocation/load, terminate/return
   and COMMAND continuation.  Classify all current source-versus-binding
   seams and choose one finite recovery cohort or an earlier source terminal.
2. **S2 — Bounded original loader/parent cohort recovery.**  Recover the
   selected complete source cohort and test exact positive/negative source
   outcomes.  Reuse original code and same-shaped adapters; any changed
   original guest-pointer seam uses the shared mapping manager.
3. **S3 — Formal DOS EXEC product closure.**  Link the selected original DOS,
   DEM, COMMAND, SoftPC and adapter units into the CPU40/x86 product and prove
   archive/input selection.
4. **S4 — One frozen DOS workload observation.**  Execute one immutable,
   low-dependency workload in the fixed container.  It may transfer to one
   earliest original owner but cannot create a trace-derived repair list.

## Exclusions

No app-defined executable loader, host native child, `cmdExec32`, new BOP
provider, guest media mutation, source parser, BaseSrv/CSRSS recreation,
CPU/device modification, CPU30, Bochs, x64 runtime claim, Redirector, WOW or
interactive command producer enters T365.

## Completion

T365 closes only after the complete selected original loader/parent-return
cohort has source/disposition evidence, a source-shaped recovery or exact
source terminal, formal CPU40/x86 product linkage, and one frozen runtime
result.  A file-open or BOP arrival alone is not DOS execution proof.
