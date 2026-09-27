# T423 S8 GUI launch and wait

## Baseline and scope

S8 was admitted after S7 P1 99276d68d and closure P2 125fd13ff.
O:/winnt retains the verified seven-file S7 publication, including ntkvm.exe
and ntsrv.exe. No S8 production code has changed. This is a bounded launch/wait
audit, not ConPTY, path lookup, a new scheduler or guest modification.

## Source evidence, 2026-09-27

Read-only inputs:

- OpenNT-4.5 nt/private/windows/cmd/cext.c, SHA256
  5E977445963F60F9797643C1C23BE49BEAE269F89E5A3A9C16671BEF4F8409D6.
  Its fEnableExtensions/CurBat/fSingleBatchLine/fSingleCmdLine/AI_SYNC
  predicate changes WOW/GUI execution to AI_DSCD only in the selected shell
  context. AI_SYNC calls WaitProc; AI_DSCD closes the process handle and
  returns creation status. These are shell-owned execution choices, not
  BaseSrv policy. The whole CMD translation unit is not a standalone launcher
  dependency; its parser/global shell state is unavailable in run16.
- Original OpenNT windows/core/ntuser/kernel/queue.c, SHA256
  0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8.
  UserNotifyProcessCreate flags 4 allocates/resets WOWTHREADINFO keyed by
  task ID, records wait object and parent, for startup/WaitForInputIdle.
  This is before task creation, not proof of a loaded target. Its kernel
  process/thread/USER-global dependencies prevent direct whole-unit reuse.
- Selected [srvvdm.c](../../../src/opennt-host/base/win32/server/srvvdm.c):
  BaseSrvUpdateWOWEntry creates paired parent wait handles and conditionally
  calls UserNotifyProcessCreate. BaseSrvRemoveWOWRecordByITask signals the
  parent event on removal. BaseSrvGetVDMExitCode returns zero for the shared
  WOW sentinel. Parent completion is therefore not a load-success event and
  does not supply an actual per-application Win16 exit code.
- Selected [wkman.c](../../../src/mvdm/wow32/wkman.c): W32Thread transfers
  iW32ExecTaskId into td.VDMInfoiTaskID, then clears the global. It calls
  pfnInitTask with that shared task ID; failure destroys the task and exits
  its host thread. WK32WowFailedExec calls ExitVDM for the still-pending task.
  The call has no original numeric loader-error argument. Do not fabricate
  a detailed load error code or confuse the two failure/completion meanings.
- Existing [registration bridge](../../../src/wow32-dll/source/wow_user_registration_bridge.c)
  registered_init_task forwards the task identity into
  [task lifecycle](../../../src/wow32-dll/source/wow_user_task_lifecycle.c),
  whose xxxInitTask call retains the original owner. The local FIRSTIDLE
  binding signals a present task idle event and otherwise clears FIRSTIDLE.
  This existing path is the first reuse candidate, not a reason to add a
  window observer as a production readiness protocol.
- Existing [service binding](../../../src/ntsrv-exe/opennt/source/base_service.c)
  sets UserNotifyProcessCreate=NULL. service_abandon_launch distinguishes
  claimed reservations from pending unclaimed creation, and only the latter
  uses original UNDO_CREATION. Audit both new-worker and reused-WOW paths
  before allowing an async launcher to disconnect.
- Existing [run16](../../../src/run16-exe/main.c): launch_gui waits for the
  real child process, while launch_vdm waits the parent event/worker and
  obtains the original VDM result. No current async GUI option or launch-only
  acknowledgement exists. Text/native frontend routes already have separate
  ownership and must not change for this stage.

## Contract direction and remaining design gates

Use explicit launcher wait policy rather than guessing parent shell context.
The proposed public syntax is run16 [--wait] <binary> [arguments]: default
GUI launch-only, explicit --wait retains synchronous GUI completion, and
DOS/Win32 text remain synchronous. Batch or CMD /c callers requiring final
GUI results use --wait explicitly; this is a documented standalone CLI
contract, not a claim that run16 can recover CMD's private interactive flags.
Existing return-37 and twelve-target fixtures must request synchronous GUI
execution explicitly and retain every current result/topology assertion.
This syntax direction is not yet implemented or runtime-accepted.

Win32 creation success and Win16 loader acknowledgement are different gates.
Prefer the existing original InitTask/failed-exec boundaries and retained task
identity; inspect original USER startup records and current authenticated
service messages before defining any missing finite transport. Do not use
queue insertion, worker readiness, first-window polling or a background waiter.
The exact acknowledgement ABI and failure/rundown binding remain to be
designed and tested; no source import or semantic replacement is yet admitted.

## Closure checklist

- [x] Record the tested S7 baseline and source ownership of shell wait policy.
- [x] Distinguish shared-WOW completion, startup notification and final result.
- [ ] Finalize/implement explicit wait parsing without changing target tails.
- [ ] Complete source-first authenticated Win16 load/failure acknowledgement
  design, new/reused worker cleanup and early-completion races.
- [ ] Implement the bounded Win32/Win16 GUI launch/wait contract, reusing
  original owners; register any necessary semantic-carrier exception first.
- [ ] Real sync/async GUI success/failure, caller/broker/worker failure, target
  survival, prompt availability, batch/CMD-c and nested result tests.
- [ ] Retain component, DOS17, mouse/display, both chain/fault matrices and
  three separate WOW frontiers; publish a coherent verified seven-file set.
- [ ] Review, governance, commit/push and clean-state closure.

Research findings are source evidence only, not runtime acceptance. S8 is
active and the existing S7 package remains available for owner testing.
