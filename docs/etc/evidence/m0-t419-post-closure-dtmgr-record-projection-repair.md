# M0 T419 post-closure: DTMgr record-projection repair

## Question

Why could DTMgr report a nesting depth greater than the number of nested
`COMMAND.COM` instances, and retain a stale image or nonzero depth after the
child commands had returned?

## Finding and disposition

The former management projection maintained its own per-worker push/pop list.
It appended once at CheckVDM, could append again at command delivery, and
attempted to infer completion from GetNextVDMCommand request flags.  Those RPC
operations are not task-lifetime transitions: a first delivery, environment
query, and resumed-parent request can all occur without creating or finishing
a DOS task.  Consequently five nested COMMAND instances could be displayed as
depth seven.

The original owner is `opennt-host/base/win32/server/srvvdm.c`.  Its DOS task
lifetime is the `CONSOLERECORD::DOSRecord` chain: `BaseSrvCheckDOS` appends or
reuses a `DOSRECORD`; `BaseSrvGetNextVDMCommand` marks the selected record
`VDM_BUSY`; and `BaseSrvGetVDMExitCode` removes a returned child or returns the
root record to `VDM_READY`.  This original chain is directly composed already.

The repair therefore removes the autonomous management stack.  The standalone
BaseSrv management projection now counts only original `VDM_BUSY` and
`VDM_TO_TAKE_A_COMMAND` records while holding the original DOS critical
section.  Returned and ready records contribute zero.  A tiny broker-local
sidecar retains an image label only after original GetNextVDMCommand releases
the corresponding `VDMINFO`; it is keyed by the exact surviving DOSRECORD plus
the same parent-wait identity used by original exit-code lookup.  It neither
creates, pops, nor selects task records.  Stale labels are pruned by traversal
of the original chain and all remaining labels are released on worker exit.

No `mvdm` or `opennt-host` mirror source changes.  The changed files are the
existing BaseSrv executable management adapter, its copied RPC projection, and
the existing DTMgr Console client.

## Verification

- Fresh x86 BaseSrv/DTMgr build from the selected formal Ninja graph passed.
- `basesrv-service-reservation-test.exe` passed its original
  Check/Update/Get/ExitVDM lifecycle assertion.
- `basesrv-service-reservation-test.exe --management-terminate` passed its
  authenticated worker-exit cleanup assertion.
- The focused DTMgr RPC fixture connected to the owner-run `O:\winnt` broker
  and observed its resident worker as `state=VDM_READY`, `stack_depth=0`, and
  `image=<EMPTY>`.
- `git diff --check` passed; repository CRLF notices were non-errors.
- Owner acceptance on 2026-09-17 verified nested interactive COMMAND depth:
  five nested commands display exactly 1 through 5, and each `exit` reduces
  the depth by exactly one until the resident empty worker is depth zero.

## Scope and follow-up

This is an owner-approved post-closure correction to T419, not T420 S3:
T420 S3 remains the separately planned original SoftPC `video` package unit.
The management sidecar is intentionally display-only and has no effect on the
original broker's DOS/WOW scheduling or record cleanup semantics.
