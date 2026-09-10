# M0 T310 S28 P1 — host-platform source boundary

## Scope

This record closes only the first reached host-platform edge set for the
selected Win32/x86 CCPU40 graph.  It does not close S28.

## Findings and disposition

- `RegisterConsoleVDM` is called by the original `nt_det.c` and `nt_fulsc.c`.
  Its NT4 Console Server transaction duplicates hardware events and creates
  state/text mappings.  Public modern Console APIs do not provide that
  protocol.  `adapter-mvdm-host-out/win32/source/console_compat.c` now exports
  the complete original signature and returns `FALSE` with
  `ERROR_CALL_NOT_IMPLEMENTED`; it does not fabricate a partial registration.
- `ExitVDM` is not a missing platform service.  It already has the
  source-shaped Base client route `base_vdm_client.c -> base_vdm_local_exit`.
  The formal graph now compiles and whole-archives that pair through
  `basesrv-bindings.lib`, so selected SoftPC teardown reaches the existing
  session-controlled termination direction rather than an unresolved edge.
- `CPU_30_STYLE` is no longer supplied by any T310 formal build command line.
  It remains only where the selected original CCPU40 headers define/use their
  historical compatibility carrier.  No CCPU30/MONITOR source, runtime row,
  or acceptance result is selected.

## Evidence

- Fresh graph: `build/M0-T310/S28/formal-host-platform-x86-r4`.
- `original-softpc-candidate` completed all 374 selected x86 CCPU40 Ninja
  edges.  Its compile command contains `CPU_40_STYLE` and no command-line
  `CPU_30_STYLE`.
- The preceding forced-link graph (`r3`) has no unresolved
  `RegisterConsoleVDM` or `ExitVDM`; remaining unresolved edges belong to
  their existing owner packets (debug trace, EMS/LIM, DEM/COMMAND, SIM32,
  BOP, VDD, and the still-unrecovered platform endpoints).

## Remaining S28 work

`trace_file`, `ActivityCheckAfterTimeSlice`, fullscreen regeneration memory,
WOW hard-error behavior, and the remaining selected host platform declarations
still require their declaration/definition/caller owner review.  This P does
not claim integrated machine execution.
