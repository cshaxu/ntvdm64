# T449 S3 targeted WOW termination progress

## Question

Does the current candidate preserve original shared-WOW task ownership while
allowing one management request to target exactly one live task incarnation?

## Selected boundary

`W32Thread` has both source-owned `VDMInfoiTaskID` and `htask16` after the
original `InitTask` succeeds.  The project sidecar binds that exact pair to an
opaque BaseSrv ticket only for shared WOW; a separate WOW receives no durable
task identity and remains worker-close-only.  `W32HungAppNotifyThread` remains
the original termination algorithm.  Its two explicit user-cancel exits report
ticket failure so BaseSrv may accept a later explicit request; normal original
`W32DestroyTask` retires the ticket.

The selected original handler also owns one global completion event and target
TDB slot.  Its normal USER registration is serial; the adapter therefore
allows one in-flight handler.  A concurrent request is reported as failed and
requires a later explicit request, rather than running two original handlers
against the same source globals.  The serial gate is released when that
original handler thread exits, not deferred until task destruction; its ticket
remains close-requested until the original destroy path retires it or an
explicit original cancellation/"task already terminated" path reopens it.

## Rejected original control surface

The source tree's public `VDMTerminateTaskWOW` implementation in
`src/mvdm/vdmdbg/vdmdbg.c` was traced before retaining the local bridge.  It
opens the global `msvdmdbg.wow` shared mapping, finds a process by PID, and
uses `CreateRemoteThread` with the shared `pfnW32HungAppNotifyThread` pointer
and a caller-supplied `HTASK`.  That preserves the same original handler but
is not the selected integration boundary: BaseSrv manages an authenticated
`iTask` incarnation, whereas VDMDBG has only mutable `PID + HTASK` inputs.
Using it would either require BaseSrv to retain/expose `HTASK` or force a
second lookup bridge, and would reinstate external cross-process injection
beside the established RPC control path.  The selected bridge therefore keeps
the verified `{ticket, HTASK}` association in the WOW owner process and starts
the same original handler locally only after an existing authenticated
worker-base instruction.  It is an adaptation of the original handler's
thread-exit calling convention, not a second terminator or scheduler.  Its
local waiter is established before the handler starts, owns only the handler
thread handle, and never uses `TerminateThread`.

## Candidate and procedure

Inputs were the current stable Ninja trees, `build/t449-s3/media-r6`, and the
unchanged `O:/w31` guest.  The WOW32 parent-import generators were regenerated
after adding the existing `OpenNtBaseClientFailWowTaskClose` client symbol to
their narrow import surface.  WOW32 then linked through its real `ntvdm.lib`.

Focused results:

- `basesrv-service-reservation-test.exe` passed the authenticated ticket,
  one-delivery, failure-reopen, retirement, stale and worker-reuse cases.
- `worker-input-watch-test.exe` passed 641 assertions, including close
  priority and 50-cycle handle reuse.
- `dumpbin` verified the candidate WOW32 exports
  `W32TerminateTaskTicket` and imports
  `OpenNtBaseClientFailWowTaskClose` from its parent contract.
- x86 WOW32 and all current x64 product components rebuilt successfully; the
  subsequent x86/WOW32 incremental pass was `no work to do`.
- `media-r6` staged and hash-verified all thirteen required host images at
  both `assets/release` and `O:/winnt/system32`.

## Limitation and follow-up

The available automation surface has no desktop application windows.  A
bounded real probe launched `run16 O:/w31/WINMINE.EXE` and
`run16 O:/w31/SOL.EXE`; it produced one `ntvdm.exe -f -w -a
O:/winnt/system32/krnl386.exe` and both launchers remained live, but after
fifteen seconds WOW32 was not loaded.  Thus this session stops before original
WOW task publication.  A freshly rebuilt current-protocol management client
did nevertheless read the live tree: one shared WOW worker with both launch
records as children, each correctly lacking a close action until its local
original `HTASK` binds.  The earlier `RPC_S_UNKNOWN_IF` observation came from
that diagnostic executable predating the V65 interface, not from NTSRV.
All probe processes were explicitly stopped afterward.
It does not prove task publication or termination.  S3 remains active until a
real shared WOW worker hosts two task identities and demonstrates: select A or
B; only the selected original task retires; the sibling remains usable;
duplicate, reuse, worker loss and whole-worker close retain their defined
semantics.

## Desktop execution witness (2026-10-23)

The real desktop witness subsequently became available.  A same-session
launcher started NTSRV and then `run16 WINMINE.EXE` plus `run16 SOL.EXE` from
`O:/w31`.  Both launchers returned zero and the live management snapshot
contained one shared WOW worker (`pid 21668`) with independently closeable
children `iTask` 1 (`WINMINE.EXE`) and 2 (`SOL.EXE`).  Closing task 1 through
the authenticated management RPC removed only that child; the worker remained
with task 2 and the surviving Solitaire window still accepted its menu key.

The same worker then accepted a new WinMine incarnation (`iTask` 3).  Two
close submissions for the selected row were accepted under the service's
idempotent close-request contract, and the post-close snapshot retained only
the Solitaire child.  A final management worker-close accepted the original
worker row; after its last task departed, the service's documented idle policy
ended the server and the next snapshot returned `RPC_S_SERVER_UNAVAILABLE`.
This is the expected terminal state, not a task-close success report.  The
desktop-only launch/RPC wrappers and their outputs are bounded, untracked
`build/t449-s3` evidence; the committed regression adds only indexed selection
to the existing monitor RPC test so the same real task row can be selected.
