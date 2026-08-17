# T224 S4 COMMAND Child-Redirection Source, ABI And Failure Map

## Decision

The next complete original-owner package is **COMMAND `54:08`, `54:0A` and
`54:0B` together**, plus the private stream/pipe-session lifecycle they create,
consume and clean up.  A leaf `CreateProcessW` call is not a compatible
replacement: OpenNT launches asynchronously, coordinates re-entry through the
VDM command broker, owns stream conversion and completes redirection during
`54:0B`.

No new leaf route is admitted by S4.  The current synchronous
`stream_session_launch` direct path is recorded as a temporary, source-derived
but incomplete seam which the next implementation package must migrate or
replace; it cannot establish COMMAND/redirection closure.

## Original Owner Map

| identity/lifecycle | OpenNT source and input | source-visible outcome | composition dependencies |
| --- | --- | --- | --- |
| `54:08 SVC_CMDEXEC` | `cmdexec.c:525-587`; `DS:SI` CR-terminated tail ≤124, `ES:0` double-NUL environment, `SS:BP` packed `STD_HANDLES`, `AL` drive, `AH` `/c` flag | malformed tail: CF clear, `AL=ERROR_BAD_FORMAT`; missing/too-long COMSPEC: CF clear, `AL=ERROR_BAD_ENVIRONMENT`; otherwise enters `cmdExec32` | `GetVDMAddr`, 32-bit RdrInfo/handles, command broker, event thread and current-directory state |
| `54:0A SVC_EXECCOMSPEC32` | `cmdexec.c:491-523`; `ES:0` env, `AL` drive | missing/too-long COMSPEC: CF clear, `AL=ERROR_BAD_ENVIRONMENT`; otherwise enters `cmdExec32` | same broker/event/child lifecycle, but no `STD_HANDLES` input |
| shared `cmdExec32` | `cmdexec.c:429-489` | starts child work; on failed thread creation CF clear / AL error; waits for `GetNextVDMCommand`; CF set only for re-entry; otherwise CF clear / child exit AL | `CreateThread`, `cmdCreateProcess`, VDM re-entry broker, controller state, event thread and SoftPC notification |
| `54:06` precursor | `cmdredir.c:225-315` | returns standard-stream materializations; pipe streams are converted to temporary files and background pumps | historical `PREDIRCOMPLETE_INFO`, raw handles, temp files, worker threads and `cmdPipeList`; T224 S2 replaces only the non-pipe stream identity with opaque tokens |
| `54:0B SVC_RETURNEXITCODE` | `cmdexec.c:590-635`; `DX` exit, `AL` drive, `BX:CX` RdrInfo | command broker may re-enter (CF set), otherwise returns CF clear / child result after directory and redirection completion | `cmdUpdateCurrentDirectories`, `cmdCheckCopyForRedirection`, VDM broker, event thread and SoftPC notification |
| DEM continuation | `demhndl.c:562-605`, `cmdredir.c:602-656` | observes the input half of that exact private pipe session | must follow, not precede, child/redirection session creation |

## Current Bound Route Audit

| current adapter route | status | S4 disposition |
| --- | --- | --- |
| `command_stream_session_v1_launch` | Direct-only synchronous host spawn after opaque `STD_HANDLES` validation; waits for exit; its state is in the stream session | incomplete: bypasses original asynchronous/re-entry/return-cleanup ownership and creates no pipe/SFT session |
| `command_execution_lifecycle_v1` | `54:08/0A` fallback: CF clear, `AL=ERROR_NOT_SUPPORTED` | retain as explicit unavailable terminal until one shared provider replaces the direct leaf |
| `command_lifecycle_provider_v1` / `cmd_get_next_service` | current `54:0B` queue/no-reentry approximation | retain but do not label as child-redirection completion; it does not own pipe cleanup |
| DEM `50:47/48` | explicit invalid-handle no-pipe terminal | retain until a typed private pipe session is created by this package |

## Four-Profile Disposition

| profile | `54:08/0A/0B` today | required future condition |
| --- | --- | --- |
| Direct/default | partial `54:08` seam plus explicit fallbacks; not a closure | a shared admitted child-redirection session, opaque stream/pipe identities, one controlled completion/re-entry boundary |
| Readonly | explicit non-launch / no-pipe outcomes | may not capture, spawn, write temporary files or fall through to Direct |
| Overlay | explicit unavailable | needs a separately admitted overlay-aware child namespace and redirection backend; no Direct fallback |
| Virtual | explicit unavailable | needs a separately admitted virtual process/redirection profile; no Direct fallback |

## Rejected Historical Imports

The direct historical units cannot be linked into the MSVC x64 `/MT` CLI
composition: CCPU/SAS `get/set` macros, guest `GetVDMAddr`, the guest-visible
32-bit `PREDIRCOMPLETE_INFO` pointer, `VDMInfo`/`GetNextVDMCommand`, SoftPC
notifications, event-thread global state, thread-kill behavior and raw
HANDLE-based pipe lists are all product-bound.  Importing them would violate
the adapter boundary and S4 stop conditions.

## Next Implementation Package

Before enabling Direct child execution, the next S must define one
COMMAND-owned **typed private child-redirection session**:

1. source-shaped checked tail/environment acquisition for both launch entries;
2. opaque S2 stream-token consumption, never raw guest handles;
3. explicit admission of non-pipe direct standard streams first;
4. a controlled child completion record that can later feed `54:0B` without
   pretending to be the original VDM broker; and
5. an extension point for a future private pipe session, without binding DEM
   `50:47/48` until the producer and cleanup semantics are present.

That is a single provider package.  It deliberately does not claim worker-pipe
or multi-program re-entry completion in its first implementation.