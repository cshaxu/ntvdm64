# T447 S2 process-retirement connection

## Question and source decision

After checkpoint 5673a72ea, resume the approved callback/object foundation
stage. Does production retire provider process storage as well as the current
task, and does failed release preserve ownership?

Source trace: standalone worker cleanup -> wow_user_session_detach ->
wow_user_runtime_unbind -> retire_current_thread -> original _WOWCleanup and
DestroyTask. The existing wow_user_task_lifecycle_dispose has only test callers.
Worker cleanup discards detach failure and ends guest-memory services first.

Original OpenNT windows/core/ntuser/kernel/queue.c DestroyProcessInfo checks
remaining threads, destroys process classes, expects process objects already
retired and releases pwpi/pEventWowExec. Canonical file SHA256:
0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8.
Its PROCESSINFO/Win32k desktop, kernel process and shared server object closure
is not composable as a standalone provider destructor. Recreating that server
or copying its process teardown is rejected. Original _WOWCleanup/DestroyTask
remain selected and unchanged for task/resource policy. The project-owned
carrier already has finite release logic; connect and harden that existing
logic at its worker session boundary. This is the smallest owned lifecycle
binding, not a new guest shutdown algorithm or task terminator.

Four-rung disposition: retain composable original task/resource cleanup;
bind unavailable process ownership to existing worker/runtime/lifecycle
storage; no mirror intrusion is necessary; add only the missing disposal
callback and refusal/error propagation in project-owned code. Additional
mirror/guest diff: zero. No kernel process list or allocator is imported.

## Implemented boundary

- Append provider disposal callback to private worker runtime; rebuild NTVDM
  and WOW32 together. No broker wire or guest ABI changes.
- Explicit session detach requires stopped thread producers and only the
  caller's session binding. This is a quiescence precondition, not a join or
  a concurrent shutdown barrier. Existing original thread exit still owns
  normal task retirement.
- Retire current task, then process carrier; preserve registration on either
  failure. A failed process release can retry with no current USER TLS while
  the owning session binding remains live.
- Refuse live tasks, held USER lock, enrolled objects or application classes.
  Retire borrowed native-class metadata through its existing owner; never
  unregister host controls. Close only the duplicated WOWEXEC event, not the
  caller's original handle.
- Worker performs this release before ending guest-memory services. Failure
  returns ERROR_BUSY rather than dropping registration and freeing backing
  state. This does not guarantee abnormal live-task shutdown; that later
  lifetime work retains its original owner.

## Verification

Hidden background driver build/M0-T447/S2/verify-process-retirement.ps1,
PID6952, started 2026-10-14 17:40:14 local; run root
build/M0-T447/S2/process-retirement; review deadline 18:00 local.
The driver incrementally rebuilds the established MSVC x86 /MT CCPU40 worker
and provider roots, then executes checked-in entrypoints:

- tests/observation/verify-wow-user-session-binding.ps1: real peer thread,
  active-peer rejection, task rejection, process rejection and retry while
  preserving session hooks/TLS/identity. Test-only lifecycle callbacks are
  explicitly not full-provider proof.
- tests/observation/verify-wow-task-lifecycle.ps1 -Case process-retirement:
  actual session/runtime/provider chain, original task registration/init/exit,
  live-task/object/class refusal, duplicate event closure, original event
  survival and borrowed host-control registration survival. Object/class
  sentinels test refusal, not real guest objects.
- Same entrypoint cases task-lifecycle, init-notification, dialog-lifecycle,
  standard-class retain existing focused regressions.
- Documentation governance and diff review.

These are local source/provider tests, not real Win16 application acceptance.
No publication, P completion or S2 closure is claimed until the separately
required ordinary package/DOS17/application gates pass.

### Recorded results and second-role review

The focused driver completed with suite.exit=0. Worker and provider builds,
session binding, process-retirement, task-lifecycle, init-notification,
dialog-lifecycle and standard-class cases passed. Documentation governance
passed. The process-retirement case exercised the production disposal
callback, but its refusal sentinels remain fixture objects, not real guests.

The application observation driver PID26540 started at 17:43:17 local,
waited for the focused build owner to finish, and completed its observation
with result.exit=0 by 17:47 local. Inputs and hashes are retained in
build/M0-T447/S2/process-retirement-apps/candidate-hashes.json; frontiers.log
records each independently launched guest on the unswitched test desktop:

- WINMINE: visible Minesweeper window; gameplay was not exercised.
- SOL: Out of memory dialog; normal initialization still fails.
- WRITE: Not enough memory for Write to complete this operation, plus hidden
  MSWRITE_MENU; normal initialization still fails.

These observed frontiers match the preceding class-name-apps observation;
the successful observer exit means evidence collection succeeded, not that
SOL/WRITE passed. This repair has not advanced their startup frontier and
does not establish cumulative playable-WINMINE non-regression or DOS17.
No release or installed package was replaced.

Second-role source review traced the only production finish caller to
src/mvdm/softpc.new/obj.vdm/ntvdm.c main: it returns the cleanup result to the
CRT and exits. There is no automatic product cleanup-retry loop. Preserved
state enables a same-owner retry of the fallible interface; that is verified
by the fixture, not claimed as a new product shutdown policy. A live-task
refusal is not proof of normal retirement. Original process/task cleanup
policy, guest bytes and mirror files are unchanged by this repair group.

S2 remains open for the complete object/shared-view production boundary
and its real-guest acceptance. This group is locally verified only.
