# T211 S3 — COMMAND launch/execution source, ABI and disposition map

## Scope

This is the whole original COMMAND launch/execution component selected by
T211 S1: `cmddisp.c` services `54:06`, `07`, `08`, and `0A`.  It is not a
trace-derived handler list.  It reconciles current bx-vdm code with the
source before any route is expanded.

Primary source: `base/mvdm/dos/command/cmdredir.c` (`cmdGetStdHandle`) and
`cmdexec.c` (`cmdCheckBinary`, `cmdExec`, `cmdExecComspec32`, `cmdExec32`,
`cmdCreateProcess`).  T202 S7 remains the governing no-admission decision
for the historical BaseSrv/CSR re-entry branch.

## Member map

| BOP | Original ABI / behavior | Historical composition dependency | Current bx-vdm route | S3 disposition |
| --- | --- | --- | --- | --- |
| `54:06 cmdGetStdHandle` | `CX` selects stdin/out/err; `AX:BX` is `PREDIRCOMPLETE_INFO *`; returns raw 32-bit handle in `BX:CX`, size in `DX:AX`; pipe paths allocate/copy and can `TerminateVDM`. | Raw VDM address, `HANDLE`, process-wide standard handles, Redirector completion and UI/termination. | `command_stream_child` returns only versioned opaque stream tokens, never a host handle. | retain the opaque-token boundary; map precise direct/readonly errors; no raw `HANDLE` ABI or Redirector reconstruction. |
| `54:07 cmdCheckBinary` | Converts guest OEM path, calls `GetBinaryType`, then applies DOS/WOW/PIF/first-call checks and may rewrite command state. | `GetVDMAddr`, `DontCheckDosBinaryType`, WOW/PIF/VDM globals and command broker state. | `execution_lifecycle` returns only the declared DOS-domain success branch. | retain this narrow source-derived DOS success; ordinary `GetBinaryTypeW` is available but no isolated path-classifier is admitted until WOW/PIF and rewrite failure contracts are represented. |
| `54:08 cmdExec` | Bounded command tail at `DS:SI`, environment `ES:0`, packed standard-stream record at `SS:BP`; calls `cmdExec32`; returns low child result or re-entry CF. | Raw VDM pointers, host environment/COMSPEC, worker/event thread, BaseSrv broker and process global streams. | direct profile copies/validates opaque stream tokens, performs one synchronous `CreateProcessW`, closes private duplicates and returns terminal low byte; other profiles use explicit unsupported result. | retain as terminal child capability only; it must never publish a child-to-DOS command or raw host handle. |
| `54:0A cmdExecComspec32` | Reads ambient host `COMSPEC`, reads `ES:0` environment, then calls `cmdExec32`. | Process environment recapture plus same broker/event/redirection path. | `execution_lifecycle` yields `ERROR_NOT_SUPPORTED` form; stream-child no longer accepts the selector. | explicit deferred/unavailable. Do not substitute CLI's admission-time processor text or host environment as a fake COMSPEC contract. |

## Modern API availability

`GetBinaryTypeW`, `GetStdHandle`, `GetFileType`, `DuplicateHandle`,
`CreateProcessW`, `WaitForSingleObject`, `GetExitCodeProcess`, and
`GetEnvironmentVariableW` are ordinary user-mode APIs.  None requires host
installation modification.  Their existence alone is not historical
composition compatibility: raw guest pointer/handle ABI, the BaseSrv broker,
WOW/PIF state, Redirector completion, worker/event scheduling and ambient
state capture remain explicit blockers.

## Current workaround actions

| Current area | Action in S3 |
| --- | --- |
| `command_stream_child_v1` | retain and move under one launch/execution owner; preserve private-handle containment and bounded terminal completion. |
| `command_execution_lifecycle_v1` | split/reconcile its `07/08/0A` service dispositions with the new component owner; it must not be the implicit policy selector. |
| session-local `dispatch_child` helpers | migrate into the component provider or delete after equivalent tested ownership exists; no leaf chooses a profile directly. |
| `54:06` stream token producer | retain as the only safe CLI replacement for raw-handle publication; complete malformed-token and non-direct tests at family scope. |
| `54:0A` unsupported result | retain as explicit deferred result; do not revive ambient `COMSPEC` lookup. |

## S3 implementation boundary

The next implementation step is one versioned launch/execution provider that
owns all four selectors and consumes the shared COMMAND mutation profile.
It may invoke normal user-mode APIs only in an admitted direct capability,
with copied input/output and session-scoped teardown.  It may not add a
BaseSrv/CSR queue, handle/pointer export, host-wide redirection mutation or a
Bochs change.
