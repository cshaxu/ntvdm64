# T204 S1 contained guest startup configuration map

## Objective

Map the complete declared source-built startup configuration path from NTIO
entry through NTDOS/COMMAND handoff, configuration completion, and every
fatal or recoverable configuration terminal.  The deliverable is one
owner/ABI/failure map and an implementation ordering for the **whole startup
configuration package**.

## Required scope

- Identify the original guest/host owners, BOP selectors, guest structures,
  inputs, output/continuation rules and failure semantics on every path that
  can reach `demExitVDM`, including configuration completion and command
  startup.
- Classify each required input as source-built guest artifact, copied bx-vdm
  record, bx-mantle machine input, normal user-mode host capability, or an
  unavailable/deferred historical composition dependency.
- Reconcile the current profile, boot namespace, command boot input and DEM
  misc/session routes against that map; give each workaround one retain,
  migrate, replace or delete disposition.
- Define subsequent whole-package S boundaries and family regressions before
  changing guest startup behavior.

## Explicit non-goals

No one-off `50:3D` patch; no trace-selected BOP implementation; no synthetic
`config.nt`/`autoexec`/`COMMAND.COM` file; no host-system modification; no
new Bochs device or semantic; no terminal-result ABI; and no guest image or
host filesystem mutation.

## Evidence baseline

Original `demExitVDM` displays the bad-`config.nt` diagnostic then calls
`TerminateVDM`; it is an error terminal.  T203's two-run engine witness
reaches its `C4 C4 50 3D` BOP through the product bridge.  T202 has already
closed DEM/COMMAND package routing and profile foundations, but those
closures do not prove a guest startup configuration is correct.

## Completion criteria

The map covers every reachable startup configuration owner rather than only
the reached `50:3D` leaf; distinguishes actual missing input/semantic
precondition from an unavailable historical host dependency; names the
correct layer for each remedy; and leaves no unclassified workaround.  It
must state whether a later package can restore a contained normal startup path
without changing Bochs or fabricating a BOP result.
