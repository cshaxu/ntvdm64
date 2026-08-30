# M0 T318 S2 P1 — CPU40 Recursive-Simulation Linkage

## Scope

This packet restores only source-shaped CPU40 CCPU interfaces reached by the
selected original machine and the frozen NTDOS/DEM/DPMI cohort.  It does not
claim that NTIO, NTDOS, `EXEC`, or a child return has run continuously.

## Recovered original interfaces

- `softpc.new/host/src/nt_cpu.c::host_simulate` is again an external entry
  for original DEM/DPMI/BIOS callers.  In-header CCPU clients still use the
  original `host_simulate_func` vector; the external spelling wraps the same
  `cpu_simulate` path and does not introduce a second executor.
- `nt_cpu.c::host_unsimulate` now preserves the original BIOS BOP `FE`
  spelling and delegates only to selected CCPU40 `c_cpu_unsimulate`.
- `nt_cprgs.c` selects the original generated-CCPU vector initializer for
  `CPU_40_STYLE` without defining the retired `CPU_30_STYLE` profile.
- SIM32’s selected CPU40 address adapter uses real/VM86 arithmetic and loaded
  CCPU segment-cache entries.  An unknown protected selector fails rather
  than recreating CPU30 monitor descriptor state or exposing a host pointer.

Every mirror difference is registered as `MVDM-HOST-DIV-146` through
`MVDM-HOST-DIV-153`, or `ADAPTER-SOFTPC-029` where the adapter owns the
checked CCPU address boundary.

## Formal evidence

The external x86 `CPU_40_STYLE`/CCPU40 graph at
`build/M0-T318/S2/cpu40-formal-r2` completed all 408 compile/archive actions
with `-j 6`.  A subsequent one-time initialized MSVC invocation rebuilt the
Ninja dependency database and `ninja -n original-softpc-candidate` reported
`no work to do`; this establishes both archive closure and usable incremental
tracking for the selected graph.

The complete-member forced-link audit then reduced its unresolved external
set from 15 to 10.  The removed symbols are:

- `host_simulate` (DEM and DPMI callers),
- `host_unsimulate` (BIOS BOP FE), and
- `selector_outside_table` (the pre-existing checked SIM32 adapter fallback).

## Remaining forced-link boundaries

The audit DLL is deliberately non-runnable (`/force:unresolved`).  Its ten
remaining symbols are not selected as T318 recovery work:

- `retrieve_descr_fields`: tape/INT 15 monitor descriptor service;
- `ModuleLoad`, `ModuleFree`, `ModuleSegmentMove`, `DbgPrompt`: DEM module or
  debugger owner paths;
- `ShowStartGlass`, `DBGDispatch`, `NtRaiseHardError`: historical UI/debug
  product-shell paths;
- `NtAllocateVirtualMemory`, `NtFreeVirtualMemory`: NT4 private regeneration
  memory path.

They remain explicit owner transfers.  None authorizes an app loader, a new
BOP provider, a CPU30 route, or a replacement guest lifecycle.

## Next S2 action

Use the restored CPU40 interface set only to construct the declared original
`NTIO -> 50:11 demLoadDos -> NTDOS` observation.  A blocked observation must
be recorded by its actual SoftPC, DEM, Redirector, or host owner; it may not
be resolved by an app-owned DOS loader.
