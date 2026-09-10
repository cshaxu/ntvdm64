# M0 T238 S1 — No-external BOP slice no-bypass audit

## Question

Does the M0 Td S1 P6 frozen local slice still have the claimed active
OpenNT-derived routes and focused local regressions, without selecting a
retained `bop-v1` provider or silently consuming an external-owner feature?

## Admitted inventory

The live tracker freezes 105 rows: 75 BOP entries (`55` DEM, `17` COMMAND,
`2` XMS and top-level `59`) plus 30 OpenNT dependency rows (`15` DEM and
`15` COMMAND).  This is a validation package, not an authorization to add
another implementation or to treat a later machine/Redirector/WOW/guest
lifecycle branch as local.

## Procedure

1. Recount the tracker’s frozen BOP and dependency IDs, including owner
   breakdown and local-pass classification.
2. Compare the 25 frozen OpenNT C translation units and five frozen headers
   with the active `bx-vdm` module membership and their compiled callers.
3. Inspect the active generic ingress and formal manifest for any selected
   `bop-v1` production source.
4. Generate a new disposable MSVC x64 `/MT`, CPU5/P-MMX formal Ninja graph
   under `build/M0-T238-S1/formal-r1`, build it, and run every fixture declared
   for the frozen owner groups.
5. Preserve concise results here and in the closure history; remove the
   disposable build root afterwards.

## Acceptance interpretation

The audit can close only when the local contracts of all 105 rows pass.  A
deferred extension recorded on a row (for example a Redirector pipe completion
or guest EXEC parent return) is not a bypass: it is valid only when the local
source-shaped failure/disposition and its focused local test pass.  A new
local gap instead stops the T for owner direction.

## Observations

1. The tracker recount is exact: 75 BOP rows (DEM 55, COMMAND 17, XMS 2,
   top-level 1) and 30 dependency rows (DEM 15, COMMAND 15).  Every row has
   the declared `Code complete / local-pass` classification.
2. The formal `bx-vdm` module directly compiles all 14 frozen DEM C units and
   9 of the 11 frozen COMMAND implementation C units; the five frozen headers
   are reached by those compiled callers.  `cmd.c` is a historical bootstrap
   containing only `CMDInit`, which has no admitted modern caller. `cmddata.c`
   is the historical process-global store; its selected, session-owned storage
   contract is currently supplied by `command_misc_shim.c`.  They are mirrors
   and source references, not directly linked translation units. This is a
   recorded source-composition limitation, not a hidden `bop-v1` selection.
3. The formal manifest has no `bop-v1` source. The active DEM generic bridge
   explicitly states it never enters `bop-v1`; retained mentions outside the
   retired directory are compatibility-header/reference comments only.
4. Formal r1 built all 353 declared edges, but 29 OpenNT/bx-vdm fixtures
   failed before `main` with `0xC0000138` (`STATUS_ORDINAL_NOT_FOUND`). Import
   inspection identified a static `COMCTL32` ordinal 345 for
   `TaskDialogIndirect`, introduced by the already-active BOP 59 public dialog
   seam. The host default Common Controls v5 lacks that ordinal.
5. The minimal repair is local to that seam: it requests the public Common
   Controls v6 side-by-side activation context. Formal r2 again built all 353
   edges; the 29 former loader failures now start and pass. The full focused
   sweep is **29/31 pass**.
6. Two executable local regressions remain and prevent closure:
   `t234-s2-command-dynamic-environment-fixture` exits 10 while checking that
   a session-owned multisz snapshot contains its source COMSPEC/PATH entries;
   `t231-s8-command-lifecycle-direct-fixture` exits 1 at its first imported
   `54:08` local-child execution assertion. They are not trace observations,
   machine failures, or later-owner transfers.

## Interpretation and follow-up

The audit rejects the Td P6 all-local-pass claim as currently executable. The
BOP 59 loader defect is repaired and validated, but the two COMMAND
regressions are local to the admitted slice. T238 therefore remains active and
must not be closed or advance the queue. Its next bounded work, if the owner
approves it, is to diagnose and repair those two COMMAND source/shim local
contracts before rerunning the same 31-fixture sweep. No external
compatibility/machine candidate is admitted by this finding.

## Repair and final result

The owner approved repair within T238.  The source review showed that neither
failure justified replacing an OpenNT body:

- The dynamic-environment fixture used `wcsstr` on a double-NUL wide
  environment and therefore could never find `PATH` after the first `COMSPEC`
  entry.  It now performs a bounded multi-string scan; imported `cmdenv.c` and
  the session snapshot seam are unchanged.
- The lifecycle fixture asserted an obsolete synchronous result.  It now
  requires the imported `cmdExec32` sequence—first `PENDING`, then original
  BOP re-entry and `RESUME` after worker completion—for direct, COMSPEC and
  valid standard-stream-token paths.  Imported `cmdexec.c` is unchanged.

Fresh r3 built all 353 formal MSVC x64 `/MT`, CPU5/P-MMX edges. Both repaired
fixtures pass, followed by the complete frozen-slice sweep: **31/31 pass**.
The two r1/r2 build roots were already removed; r3 is removed after this
concise result is recorded. Confidence is high for the declared local
contracts of the 105 frozen tracker rows. This does not claim machine,
Redirector, WOW or guest EXEC continuity, which remain explicit candidate-2
work.
