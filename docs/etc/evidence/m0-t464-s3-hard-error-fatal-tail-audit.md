# M0 T464 S3 — WOW hard-error fatal-tail audit

## Result

Every selected hard-error response has exactly one existing cleanup owner. No
production source change is necessary or admitted in S3.

| Reached caller | Slot/result | Existing sole owner | Outcome |
| --- | --- | --- | --- |
| `nt_error.c::WowErrorDialogEvents` | 1 / abort | `ErrorDialogBoxThread` invokes registered original `W32HungAppNotifyThread(0)` | Existing current-task/hung-app cleanup; it does not return. |
| same | 2 / retry, 3 / ignore | original `RMB_RETRY` / `RMB_IGNORE` return | No termination. |
| `wudlg.c::WU32SysErrorBox` | any returned source slot | original thunk `RETURN(dwExitCode)` | Guest USER receives the slot; no task termination here. |
| `wuser.c::WU32SetEventHook` | slot 3 | original continuation | No termination. |
| same | slot 1/2 or receiver failure 0 | original caller sets `RET_FORCETASKEXIT` and `TDF_FORCETASKEXIT` | Existing task scheduler owns the subsequent exit. |
| `wow32.c::W32Exception` | slot 2 | existing debugger branch | Debugger/exception path owns continuation. |
| same | slot 3 | existing ignore/continue branch | Existing exception return path owns continuation. |
| same | slot 1 or receiver failure 0 | original `KillTask` tail sets `RET_FORCETASKEXIT` | Existing task scheduler owns the subsequent exit. |

`WOWSysErrorBox` retains its original `SuspendTimerThread`/`ResumeTimerThread`
bracket regardless of receiver outcome. The S2 receiver returns only a source
slot or an NT failure; it does not invoke `W32HungAppNotifyThread`, set a task
flag, call `KillTask`, end a worker, or add a lifecycle provider.

## Failure and loss boundary

If the narrow receiver rejects an unrecognized/private form, `NtRaiseHardError`
returns `STATUS_NOT_IMPLEMENTED`; the original producer converts that to slot
zero. The two fatal callers already treat zero as their existing fatal branch,
while `WU32SysErrorBox` returns it as the original raw result. This preserves
the original caller distinction without treating receiver failure as host
success.

The lower `W32HungAppNotifyThread` path is intentionally retained verbatim,
including its separately governed management/loss divergences. S3 neither
changes its wait policy nor substitutes a worker exit. A task/worker-loss test
remains under the existing WOW lifecycle owner; the hard-error receiver is not
a new lifecycle authority.

## S4 handoff

S4 must exercise the published package's selected ordinary WOW frontiers and,
where an admissible original hard-error requester can be safely driven, observe
source-slot return plus the relevant existing tail. The isolated private-desktop
host cannot automatically answer a modal dialog without an unapproved
polling/hidden-window mechanism; that limitation remains explicit, not a false
UI pass. No guest input or guest binary is changed.
