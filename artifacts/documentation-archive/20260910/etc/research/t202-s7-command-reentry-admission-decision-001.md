# T202 S7 COMMAND host-child re-entry admission decision

## Question

Can the CLI product replace the `GetNextVDMCommand`-backed re-entry branch
used by `cmdExec32`, `cmdReturnExitCode`, and `cmdGetNextCmd` with a bounded,
typed host-child-to-DOS command queue?

## Inputs

- `src/opennt/base/mvdm/dos/command/cmdexec.c`
  (`cmdCreateProcess`, `cmdExec32`, `cmdExec`, `cmdExecComspec32`, and
  `cmdReturnExitCode`);
- `src/opennt/base/mvdm/dos/command/cmdmisc.c` (`cmdGetNextCmd`);
- `src/opennt/public/internal/base/inc/vdmapi.h` (`VDMINFO` and state bits);
- `src/opennt/base/win32/client/vdm.c` and
  `src/opennt/base/win32/server/srvvdm.c` (the historical broker);
- current CLI declaration in `src/cli/byob_launch_plan_v2.[ch]`; and
- current COMMAND session and terminal-child code in `src/bx-vdm/`.

No external source, host-state change, or native trace is an input to this
decision.

## Historical contract map

| Caller/path | Historical action | Required result | Non-portable composition it relies on |
| --- | --- | --- | --- |
| `cmdExec` / `cmdExecComspec32` -> `cmdExec32` | Start a host executable on a worker thread, set `NO_PARENT_TO_WAKE | RETURN_ON_NO_COMMAND`, then call `GetNextVDMCommand`. | A returned `CmdSize > 0` means `CF=1` and `IsRepeatCall=TRUE`; no command means `CF=0`, `AL=dwExitCode32`, and event-thread/stdio restoration. | Shared globals, event thread, process-wide standard handles, and the BaseSrv command broker. |
| `cmdCreateProcess` | Increment/decrement the console re-entry count around `CreateProcess`, wait for the child, then publish `dwExitCode32`. | The count wakes or suppresses historical VDM waiters; child status is consumed by the original caller. | CSR `BasepSetReenterCount`, console records, wait handles, worker thread, and process-global `SetStdHandle`. |
| `cmdReturnExitCode` | Publish a DOS return code, copy Redirector completion state, then call `GetNextVDMCommand(RETURN_ON_NO_COMMAND)`. | Next supplied DOS command gives `CF=1`/repeat; otherwise `CF=0` and the prior host-child exit byte. | `PREDIRCOMPLETE_INFO`, `cmdCheckCopyForRedirection`, event thread, and the same broker. |
| `cmdGetNextCmd` | Build a `VDMINFO` request from `CMDINFO`; manage first/repeat state, environment sizing, current directories, PIF classification, redirection and DOS record state. | Broker-owned command/environment data is copied into COMMAND guest buffers; it may request a larger environment buffer by `CF=1`, then repeat. | Raw VDM pointers, command/DOS-record state machine, BaseSrv waits/events, PIF/UI paths and Redirector records. |

`VDMINFO` is not a queue element: it contains host pointers, three `HANDLE`s,
`STARTUPINFOA`, variable buffers and server-private state.  The state bits
confirm the conclusion: `INCREMENT_REENTER_COUNT`/`DECREMENT_REENTER_COUNT`
are separate CSR operations, while `RETURN_ON_NO_COMMAND`,
`NO_PARENT_TO_WAKE`, and `ASKING_FOR_SECOND_TIME` select BaseSrv DOS-record
and wait-handle behaviour.  They cannot be represented as a local FIFO
without reimplementing the historical host broker.

## Current product boundary

`byob_launch_plan_v2` is a copied, immutable CLI declaration with one target
and an optional declared `QUIT.COM` terminal slot.  The COMMAND session copies
it at composition admission and uses it only for the normal DOS
`54:01`/`54:0B` declared-plan continuation.  It has no producer API, host
child callback, ambient process state, pointer, handle, worker thread or
event-loop ownership.

The current direct-profile `54:08`/`54:0A` provider is deliberately narrower:
it copies bounded command/environment input, validates opaque standard-stream
tokens, duplicates only private handles for one child, waits, closes all
handles, and returns a checked terminal byte.  It does **not** claim the
historical re-entry branch.

## Profile disposition

| Profile | Host-child -> DOS re-entry | Existing, valid result |
| --- | --- | --- |
| direct | Deferred: normal Win32 `CreateProcessW` is available, but it does not supply the historical BaseSrv command/DOS-record broker. | Bounded synchronous child transaction; `CF=0`, low result byte in `AL`. |
| readonly | Deferred/unavailable: no new host-child execution capability is admitted. | Existing explicit unsupported lifecycle result. |
| overlay | Deferred/unavailable: overlay storage does not create a process or scheduler semantics. | Existing explicit unsupported lifecycle result. |
| virtual-volume | Deferred/unavailable: a volume view changes storage only, not command-broker ownership. | Existing explicit unsupported lifecycle result. |

The declared DOS plan remains available independently of all four host
mutation profiles.  Its `54:0B` continuation must not be repurposed as a
completion queue for a host child.

## Decision

**No typed re-entry queue is admitted in S7.**  There is no CLI-owned,
source-evidenced producer of a post-child DOS command, and introducing one
would invent the unavailable BaseSrv/CSR command broker, event scheduling and
Redirector completion ownership.  A queue whose only producer is the
existing static launch plan would be a duplicate and would blur the distinct
`cmdGetNextCmd` path.

The retained terminal result is source-shaped specifically for the original
`RETURN_ON_NO_COMMAND` branch.  It is not a claim that the historical
`CmdSize > 0` branch works, nor a substitute for interactive COMMAND,
Redirector pipes, VDD, or a generic scheduler.

## Consequences and follow-up

- No queue source, lifecycle state, test, or native trace is added by S7.
- The existing direct child transaction stays a terminal, session-owned
  capability; its private handle lifetime remains contained.
- `cmdReturnExitCode`'s declared DOS-plan continuation remains separately
  owned by the existing COMMAND session and may not accept host-child data.
- A future capability may revisit re-entry only after it has an explicit CLI
  command-source contract, a complete source map for scheduling and
  Redirector completion, fixed-width cancellation/teardown ownership, and
  whole-package regressions.  It is not an endpoint follow-up.

## Confidence

High for the no-admission decision: all connected original callers and both
server/client sides of `GetNextVDMCommand` were inspected, and the current
CLI declaration has no compatible producer.  This does not prove that a
future interactive CLI capability is impossible; it proves that it is outside
the current one-shot profile and T202 S7 boundary.
