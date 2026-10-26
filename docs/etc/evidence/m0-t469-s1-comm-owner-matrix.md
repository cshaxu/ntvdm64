# M0 T469 S1 — WOW32 COMM owner and endpoint matrix

## Question and method

This source audit traces the selected Win16 USER COMM ordinals from the thunk
table to their original WOW32 owner, then to modern serial resources and task
retirement. It does not claim a hardware success path from a lexical export or
from a controller-only loopback.

## Original entry and state owners

`wow32/wutbl2.h` maps USER ordinals 200--215 and 245 to the original
`wow32/wucomm.c` bodies: open, state, error, read/write, transmit, close,
event mask, break, unget, DCB parse, escape/flush and notification. The same
owner keeps the process-local twelve-entry `PortTab`: COM1--COM9 and
LPT1--LPT3. A successful COM open owns the native file handle, read/write
events, write buffer, critical section, writer thread, DEB mapping, optional
modem-event thread and the originating WOW task thread id. LPT uses the
separate original DCB-only branch.

| Family | Original owner and host boundary | Failure/return owner | Retirement owner |
| --- | --- | --- | --- |
| Open/configuration | `WU32OpenComm`, `WU32BuildCommDCB`, `WU32Set/GetCommState`; `CreateFile`, `SetupComm`, DCB/timeouts and original DEB conversion | Original `IE_*` result mapping and staged error labels | `WU32CloseComm` calls `DeletePortTabEntry`; failed construction unwinds its own partial allocation. |
| Read/write/status | `WU32ReadComm`, `WU32WriteComm`, `WU32GetCommError`, `WU32UngetCommChar`; overlapped native I/O plus original queue/DEB projection | Original zero/negative-count and accumulated `COMSTAT`/`ComErr` behavior | `DeletePortTabEntry` signals/cancels the owner threads before freeing queue/events/handle. |
| Events/notification | `WU32Set/GetCommEventMask`, `WU32EnableCommNotification`, `WOWModemIntThread`; `WaitCommEvent`, modem status and original DEB event word | Original `WOW_WM_COMMNOTIFY`/event mask state | Close sets `fClose`, clears mask and waits for the modem thread. |
| Task loss | `wkman.c::WowOriginalFreeTask` calls `FreeCommSupportResources(ptd->dwThreadID)` | No project lifecycle substitute | Original `FreeCommSupportResources` delegates to `DeletePortTabEntry`. |

The only accepted real external endpoint evidence remains the existing
[T420 device boundary](m0-t420-s30-external-com-boundary.md): COM3 opens, but
has no CTS/DSR-capable peer, so its real DOS transmit correctly reaches the
original timeout branch. Its unchanged-source mock proves the normal native
provider lifecycle only; it is not real-device acceptance.

## Multi-device retirement discrepancy

The unchanged original Win16 USER `user/comdev.c::SP_GPFaultCommCleanup`
iterates every device owned by a faulting task and closes each. In contrast,
the original WOW32 `FreeCommSupportResources` scans `PortTab` by originating
thread id but executes `break` after its first match. Since the Win16 source
documents independent COM/LPT opens and associates each with `GetCurrentTask`,
one task can own more than one device. Therefore a second port can survive the
current WOW32 task-loss path.

This is an inherited original-host discrepancy: `wucomm.c` is unchanged from
the restored WOW32 baseline, and the inspected SoftPC tree has no matching
correction. Under source policy it is not repaired by inventing a new manager
or cleanup mechanism. The next subtask must first locate an admissible matching
source correction or retain the task-loss edge as an explicit original-host
limit. It must separately audit the original 1.5/5-second close waits and
their interaction with the project-approved event-driven lifecycle policy.

## Next boundary

S2 owns the complete operation/endpoint audit, including whether current
modern serial APIs preserve the original pending I/O and close semantics. Any
production change requires a matching correction or a separately admitted
minimal host binding; guest media and external-provider policy remain out of
scope.
