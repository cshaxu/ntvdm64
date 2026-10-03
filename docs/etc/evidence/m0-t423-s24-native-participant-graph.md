# M0 T423 S24 — NTSRV-Owned Native Participant Projection

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

> **S24 closed baseline.** This document retains the Job experiment
> as research evidence only. S24's publishable baseline is deliberately
> narrower: a resident NTW32 registers before its blocking get-next call,
> receives one authenticated direct command, binds that direct target identity
> before `ResumeThread`, returns the target's actual Windows result, becomes
> READY, and is reused. No Job creation, assignment, notification or
> `Observed` record may be a condition of that direct path. The proposed Job
> projection is deferred to S26 and can never control receipts, READY/EMPTY,
> shutdown, or scheduling.

## Observation Contract

PID sampling correctly removed a count guess, but cannot be authoritative: a
short-lived descendant can be created and exit between samples. It must not be
used as acceptance evidence for Win32Record order, completion or worker
`BUSY`/`EMPTY` state.

The subsequent Job-completion-port design removes that sampling window, but
also cannot satisfy the required complete graph contract. Microsoft documents
that ordinary Job completion messages are notifications whose delivery is not
guaranteed; this expressly includes `JOB_OBJECT_MSG_NEW_PROCESS`,
`JOB_OBJECT_MSG_EXIT_PROCESS`, and `JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO`.
The same documentation also warns that process identifiers may already be
recycled unless a handle is retained. Therefore Job messages may be retained
only as an observed diagnostic projection, never as the authority for every
native descendant, nesting order, or `BUSY`/`EMPTY` transition.

The current S24 candidate proved that a suspended direct target can be assigned
to an event-only Job before resume and that a short root/grandchild fixture
received two `NEW_PROCESS` and two `EXIT_PROCESS` notifications in 100 runs.
That experiment is useful compatibility evidence, not a proof that no event
can be lost. It does not close S24 or authorize publication.

The owner selected the bounded design: NTSRV owns each direct native root's
event-only Job and completion port; NTW32 creates the target suspended, calls
the existing authenticated bind, and resumes only after NTSRV has attached it.
NTSRV stores Job events
as `Observed` elements in the same Win32Record stack as the `Direct` root.
The direct root's duplicated
process handle, not a Job event, remains the authority for its run16 receipt.
Observed participants never govern worker readiness, worker shutdown, or
receipt completion.

The tracker also carries the assigned direct-root PID with every Job message.
If the direct completion removes its record before a delayed root
`NEW_PROCESS` notification is dispatched, NTSRV drops that stale root message
instead of recreating the same process as `Observed`. Descendant notifications
remain eligible for observed projection.

Authoritative sources: [Job completion-port delivery semantics](https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-jobobject_associate_completion_port)
and [ETW missing-event semantics](https://learn.microsoft.com/en-us/windows/win32/etw/about-event-tracing).

## Retained Job Projection Research (not the S24 product path)

NTSRV creates one `WIN32RECORD` list per NTW32 worker. Each element is marked
`DIRECT` or `OBSERVED` by source; those marks do not create separate tables or
separate stacks. A direct record is bound
only by the authenticated NTW32 worker passing the typed process handle from
its successful `CreateProcess`; it is not a launcher-nominated PID. Job
notifications append and retire `OBSERVED` elements on that same list.
`STACK` is exactly this list's length. Direct records alone retain their
original receipt/completion meaning. Observed records are monitor projection
only and never retain a worker in BUSY state.

## Verification

### Replanned direct-command baseline

#### Final protocol-23 build and publication candidate (2026-09-30)

The full x86 Ninja graph completed successfully in the host toolchain
environment; a subsequent `ninja -n` reported `no work to do`. The focused
`ntw32-execution-lifetime-test.exe` passed 444 checks with zero failures,
12 completed and 16 cancelled requests, a surviving target after worker
closure, and no retained handles. The worker-base next-command, native-worker
reservation, NTW32 close and isolated Job-research fixtures also passed.
The Job tracker is absent from production NTSRV initialization and linkage.

The first seven-file publication failed owner Win32 interaction and was
rolled back. After the frontend-readiness fix, the coherent seven-file output
was republished to `O:/winnt`. The pre-S24 product files remain recoverable
under `build/M0-T423/S24-helperless-clean/published-backup-20260930`.
SHA-256 equality between revised formal output and each published file was verified:

| File | SHA-256 |
| --- | --- |
| `run16.exe` | `A9FCE7D707E9155872D9B95B09BA2662FA62268F02D0778C55CC921F646CF60D` |
| `ntsrv.exe` | `CC10833CCD00F8EFFC03C606FBF9243F263FE3AEC6D124FEEFB919B8631C1805` |
| `ntvdm.exe` | `2905E54DD661D3AFF9A296D52F4FA8AD53CAE1AB645CF494F190595274D78F92` |
| `ntcon.exe` | `519814DB69A01DDD106F0A683D9B6E5B07588D1406EAC01E54A4053DCB92D7E3` |
| `ntw32.exe` | `33AEFBC83CAA74E29193D0C66420AC12F24FF13FA952CF12519032665FF57A88` |
| `ntmon.exe` | `F453D27E3BA35D069D581949D0370F3FC5B6DED3B2F4C94BE5C8B105E267E46B` |
| `VDMREDIR.dll` | `DC0CD220688A2E3C4966EA80CAEEBBAD677944C5526DD101CE5783A01FFA320C` |

In an isolated build package before the readiness correction, direct `cmd`
requests returned 0, 37, and 0
in one Console; the sequence exited 0 and left exactly one NTW32 after
starting with zero NTW32 processes. The short real-program matrix passed
direct MEM, `COMMAND /c ver`, `COMMAND /c MEM`, interactive MEM, nested
COMMAND/MEM and EDIT with guest text/exit observations. Published-package
smoke tests returned 0 for `run16 cmd /c ver`, 0 for
`run16 command /c ver`, 37 for `run16 cmd /c exit 37`, and displayed the
expected memory text for both direct MEM and `COMMAND /c MEM`.
Those initial native smoke tests asserted exit status but not visible native
output or input; they were insufficient and did not justify publication.

The initial publication was rejected by the owner: `run16 cmd` had no usable
input/output and `cmd /c ver` printed nothing. It was restored from the
seven-file backup before any further publication. The exact code regression
was in NTW32 `begin_io`: it returned success while its presentation pointer
was null, allowing the hidden-Console target to start before NTCON attached.
The revised candidate restores `ERROR_NOT_READY` until the presentation
exists, and calls the existing `ntw32_presentation_end` for the final direct
consumer. This matches NTVDM's earlier `WaitFrontend -> activate -> guest`
ordering without moving original DOS execution logic into `worker-base`.

After the fix, the same private-Console probe saw `READY`, delivered `exit`,
and observed `run16` exit 0. The plain `run16 cmd` probe saw a prompt, accepted
`exit`, and exited 0. `run16 cmd /c ver` displayed the Windows version text
and exited 0. A plain CMD running three sequential direct `run16 cmd`
requests returned 0, with the middle `exit 37` preserving code 37. DOS
`COMMAND /c ver` and interactive `COMMAND -> MEM -> EXIT` displayed the
expected text; the latter returned the original COMMAND code 1. The
published `O:/winnt` package independently passed plain `run16 cmd` input
and visible `/c ver` output. Report files are under
`build/M0-T423/S24-helperless-clean/*gated*.txt`.

The deeper `run16 cmd /c native-reuse.cmd` private-desktop probe (with the script on the mapped test drive) still
timed out and is **not** a passing native-nested result; the direct same-Console
reuse result above does not cover it. Earlier expanded matrix cases are not
used to claim an old-package defect merely because that observer missed text.
The headless `console-frontend-test.exe` stops at its real Console resize
check with Win32 error 87; its earlier checks pass, but the whole fixture is
not claimed as passing. The accepted 30 ms hidden-Console output sampler and
the previously existing 10 ms startup/handoff retries remain unchanged in
this delivery. On 2026-09-30 the owner reported basic verification passed,
accepted this seven-file package as the latest baseline and directed S24 closure.
The timed-out native-nested probe is explicitly transferred to S25 for root-cause
repair and real-program regression proof; no pass is inferred from the owner's
acceptance. Other minor side-test issues remain unspecified pending owner report.
T423 is not closed.

The published candidate latches a failed broker completion RPC as a worker
fault instead of entering another GetNext call. The focused lifetime fixture
reports `checks=444 failures=0 completed=12 cancelled=16 target-survival=yes
remaining-handles=0`. Job tracker initialization and its production library
member have been removed; the isolated Job fixture remains research evidence.

The remaining presentation timer cannot be removed merely by waiting on the
hidden Console input handle: that handle signals input, not arbitrary output
buffer writes. A bounded x86 probe in
`tests/observation/ntw32_console_winevent_probe.c` installed an out-of-context
Console WinEvent hook with a message loop, wrote to the hidden Console via
`WriteConsoleOutputCharacterW`, and moved its cursor. It received zero real
Console update/caret notifications; a synthetic `NotifyWinEvent` reached the
same callback, validating the hook. Thus this machine provides no verified
event source for arbitrary hidden-Console output. The owner then approved
retaining the bounded hidden-Console output sampler for now. The 30 ms
presentation loop and bounded geometry retries are therefore disclosed product
behavior, not a claim of event-driven output. Changing them to an infinite
wait without a reliable output signal would freeze native screen updates.
Repeated cursor-position, cursor-info, attribute and logical-window setters
in NTCON were made idempotent to avoid perturbing host cursor blink, but
this does not constitute removal of the presentation sampler or prove the
user-visible blink fixed.
This exception is specific to observing writes by unmodified native programs
in NTW32's hidden Console; it does not authorize timer polling for broker
commands, completion, handoff ordering or other controllable producers.
The updated NTCON x86 binary and `console-frontend-test.exe` link successfully
in `build/M0-T423/S24-helperless-clean`. In this headless invocation, the
frontend fixture passes its earlier protocol/dispatch sections but stops at
line 234 while resizing a real host Console (`SetConsoleScreenBufferSize`,
error 87); it is not recorded as a full pass. The NTW32 execution-lifetime
fixture separately passes its 444 checks. The WinEvent probe exits 5 by design
when no real buffer-update notification follows the synthetic callback:
`synthetic-simple=1 output-wrote=17 real-update=0`.

Relevant API contracts: [Console screen buffers](https://learn.microsoft.com/en-us/windows/console/console-screen-buffers),
[console WinEvents](https://learn.microsoft.com/en-us/windows/console/console-winevents),
[SetWinEventHook message-loop requirement](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwineventhook).

- The direct startup bind records only the authenticated direct target identity
  before `ResumeThread`; it neither creates nor assigns a Job. A direct root
  completion completes only that request. Native descendants remain
  Windows-owned and have no S24 readiness or receipt authority.
- A preflight frontend-binding rejection consumes the normal request header and
  payload, returns a structured `native_request_reply` (`ERROR_BUSY` in the
  fixture), and only then completes the broker command. The requester cannot
  wait for a reply that will never be sent.
- `ntw32-execution-lifetime-test` passed with
  `checks=432 failures=0 completed=12 cancelled=16 target-survival=yes`.
  This includes the preflight-rejection reply and proves that the rejected
  request did not launch its probe target.
- The x86 manual rebuild logged in
  `build/M0-T423/S24-helperless-clean/manual-s24-preflight-build.log` rebuilt
  `ntw32.exe` and the lifetime fixture. The exact rebuilt seven-component
  package was copied to `O:/winnt`; SHA-256 equality was checked per file.
- Actual package checks passed: `run16 cmd /c ver` returned 0,
  `run16 command /c ver` returned 0, and `run16 cmd /c exit 37` returned 37.
  Two successive `run16 cmd /c ver` commands in the same host Console both
  returned 0 and left exactly one resident `ntw32.exe`, proving direct worker
  reuse rather than a second native worker. These commands use the established
  command-line syntax; no `--` separator was introduced.

The S24 graph generated with Node 22.22.1 contains the explicit x86 product
closure for `run16.exe`, `ntsrv.exe`, `ntvdm.exe`, `ntcon.exe`, `ntmon.exe`,
`ntw32.exe` and `VDMREDIR.dll`.  The current formal-r2 x86 rebuild linked the
current protocol-23 `ntsrv.exe`, `ntw32.exe`, `run16.exe`, `ntcon.exe`,
`ntmon.exe` and `ntvdm.exe`; `ntvdm.exe` also passed its current VdmTib
ownership audit.  This is a build-closure result, not publication or a claim
that the frontend integration gate below has passed.
The graph has no standalone `WOW32.DLL` target: its currently selected WOW
runtime is worker-local in `ntvdm.exe`.  The stale seven-file publication
wording is a component-boundary governance discrepancy for S25/S26; it is not
treated as a successful standalone WOW32 build or publication.

- Current formal-r2 focused rerun: the first `monitor-rpc-test --empty`
  invocation failed at its `CreateProcess(ntsrv.exe)` check because that
  current formal directory had not yet produced `ntsrv.exe` (error 2).  After
  the same formal-r2 graph rebuilt and linked `ntsrv.exe`, the identical
  fixture passed with `PASS: empty PID-only DTASKMGR RPC rejects version and
  absent worker`.
  `ntsrv-native-job-tracker-test` also passed with `NEW_PROCESS=2` and
  `EXIT_PROCESS=2`, and confirmed that closing the event-only Job leaves the
  target alive.  The same rebuilt graph's
  `basesrv-service-reservation-test --native-worker` passed its one-list
  `Direct`/`Observed` transition.  These are focused current fixtures, not a
  whole-product publication.
- `ntsrv-native-job-tracker-test`: pass. A suspended direct root running a
  nested `cmd -> cmd` chain produced `NEW_PROCESS=2` and `EXIT_PROCESS=2`
  before the Job became empty. It also closes a populated event-only Job and
  proves its direct target remains alive. This is compatibility evidence for
  the event path, not a claim that Windows guarantees delivery.
- `basesrv-service-reservation-test --native-worker`: pass. Covers typed
  direct-target binding, reservation/authentication, route loss, rebind,
  completion and worker rundown.  Its S24 extension starts the bound Direct
  root only after the Job bind, has it create a held native child, and proves
  the single snapshot chain reaches `STACK=2` (`Direct` plus `Observed`).
  Completing the Direct receipt leaves `STACK=1` while the worker is already
  `READY`; releasing the child retires that observed tail to `STACK=0` during
  ordinary cleanup.  Five consecutive runs passed. Thus an Observed node is
  visible in the same chain but has no receipt or BUSY/EMPTY authority.
- `ntw32-execution-lifetime-test`: 369 checks, zero failures. Covers normal
  and failed execution barriers, cancellation, request-handle release and a
  target surviving worker-side request cleanup.
- `ntw32-close-test`: pass. Covers the normal Console-session close path
  without a Job kill limit.
- Fresh x86 candidate, short package-root integration: both `run16 MEM.EXE`
  and `cmd.exe /d /c "run16 MEM.EXE"` returned exit `0`; the latter produced
  the complete MEM Console witness. This isolates the S24 RPC bind/receive
  path from the historical DOS long-path `161` limitation. The independent
  observation graph now rebuilds `console-startup-observer.exe` from the
  current source as x86 `/MT`; its generator explicitly defines
  `_CRT_SECURE_NO_WARNINGS` so the selected source is reproducibly clean under
  `/W4 /WX`. Its first result exposed, and the candidate corrects,
  `ntw32-exe/main.c::end_io` using an uninitialized completion-status local.
  That local had returned random launcher codes after a successful native
  target. The current observer passes `native-cmd-dos`,
  `native-cmd-dos-repeat`, and `dos-native-dos`, each with its expected exit
  status and Console markers.
- `monitor-rpc-test --empty`: current protocol-23 pass after the formal-r2
  `ntsrv.exe` link: `PASS: empty PID-only DTASKMGR RPC rejects version and
  absent worker`.
- The historical `build/M0-T423/S24/formal` package used protocol 20.  The
  regenerated formal-r2 graph compiled the protocol-23 MIDL stubs and linked
  the affected RPC clients plus NTSRV/NTW32/NTMON.  The remaining S24 gate is
  end-to-end frontend bootstrap, not an unlinked protocol client.

The current S24 source inputs for `native_job_tracker.c`, `base_service.c`,
`execution.c` and the protocol-23 MIDL server stub also compiled with the
exact x86 commands emitted by the regenerated formal graph.  The local Ninja
executor itself intermittently exits before dispatch and leaves only its
zero-byte lock; that executor anomaly is not counted as a successful rebuild
or publication.  The independently linked current native-chain fixture and
the Job tracker fixture remain the focused S24 executable evidence.

The lifetime fixture explicitly supplies a permissive test-only local pipe
DACL.  The runner's RDP token otherwise rejects its own local pipe client with
`ERROR_ACCESS_DENIED`; production pipe authentication is unchanged.

### Separate Frontend Observation

The earlier protocol-21 bootstrap observation is superseded.  With the
candidate-only private pipe/ACL layer removed, the current protocol-23
`frontend-bootstrap-test` runs from the same formal package without a
prestarted Broker and passes its owner-only usage, retirement barrier,
released-creator capability, distinct authenticated-owner and Broker-loss
checks.  This is a current bootstrap boundary pass; it does not by itself
claim a full interactive Window presentation acceptance.

The lifetime fixture retains its explicit permissive DACL only for its own
in-process disposable test pipe under the RDP runner token.  Production
bootstrap and worker pipes use their normal default security descriptor and
the existing authenticated RPC/context checks; no permissive production pipe
ACL is present.

### Worker-shape convergence audit (2026-09-30)

The owner required that OpenNT/MVDM remain the semantic and interface-shape
authority: NTW32 must adapt to that shape, never the reverse.  A source search
of `src/mvdm` and `src/ntvdm-exe` found no project NTW32 registration, native
worker-channel, or NTW32 scheduling call in those mirrors.  The `ntw32` hits
there name the historical NT Console Server or occur in original comments;
they are not dependencies on this product's `ntw32.exe` worker.

The project-native receive path is now represented in `worker-base` as
`GetNextNativeCommand -> Complete -> GetNextNativeCommand`.  Its private
implementation may wait on the authenticated worker channel, but that
transport detail is not the worker execution contract.  Original
`GetNextVDMCommand`, its DOS/WOW records, re-entry, standard streams, waits
and completion remain in their original MVDM/BaseSrv owners.  NTW32 retains
only its native-specific work: suspended `CreateProcess`, authenticated target
bind, `ResumeThread`, Windows completion and the next receive cycle.

The RPC method and its generated client/server stubs use the same
`GetNextNativeCommand` name.  This is intentionally a native-worker analogue,
not a rename or wrapper around original `GetNextVDMCommand`: the latter stays
in the original MVDM path and NTVDM does not call, include or adapt to NTW32.
The earlier externally callable `TakeWorkerChannel` transport operation has
been removed from the service IDL and client surface.  NTSRV retains its
nonblocking take routine solely as an internal implementation detail of the
blocking native-worker receive; no worker can select a second public receive
shape.  Removing that wire operation advances the generated RPC interface to
version 23.0, so protocol-21 components fail connection rather than silently
mixing incompatible client/server procedure tables.

Focused x86 evidence is positive but deliberately bounded:

- `worker-base-next-command-test.exe`: ownership transfer, failure disposal
  and completion (`PASS`);
- `ntw32-execution-lifetime-test.exe`: 413 checks, including the new
  event-driven direct-completion-to-next-command barrier; 12 completed
  requests, 16 cancelled requests, no residual handles and no target kill on
  close (`PASS`).

The cold formal `ntw32.exe` graph has a 52-command dependency closure.  The
normal Ninja dispatcher did not make progress reliably in this environment,
so the exact generated x86 command rows were executed in dependency order
under the same VS x86 environment.  The resulting protocol-23 `ntsrv.exe`
and `ntw32.exe` linked successfully.  This is a build proof only: neither
candidate is published and the end-to-end frontend/bootstrap gate remains
open.

## Boundaries

No guest, shared library, original DOS/WOW record, helper, scheduler, polling
or process-tree termination policy changed.  S25 standardizes external worker
control, S26 standardizes the monitor DTO, and S27 removes remaining
cross-worker handling divergence.

## Owner-Selected Bounded Observation Contract

The owner selected the first truthful contract: NTSRV is authoritative for
submitted Direct native targets, while Job descendants are explicitly
best-effort `Observed` projection entries.  They are visible in the one
Win32Record stack used by NTMON, but do not govern receipt completion, worker
readiness or worker shutdown.  This is intentionally analogous to an
unobserved DOS-only nested execution: it improves management visibility without
inventing a second scheduler or pretending that Windows parent/child exit
semantics belong to BaseSrv.

No alternative observation mechanism is admitted by S24.  In particular, ETW
has documented missing-event semantics, and debugger creation would introduce
a debugger-owned execution-control loop.  The eliminated sampled implementation
remains superseded evidence only.

### Ruled-out Debugger Alternative

`CreateProcess(DEBUG_PROCESS)` would make NTW32 a Windows debugger for the
direct process and, absent `DEBUG_ONLY_THIS_PROCESS`, its descendants. Windows
then reports debug events, including process create and exit, to the debugger.
It is not an interchangeable observation API: the debugger's creator thread
must call `WaitForDebugEvent`, and the reported target thread does not continue
until the debugger calls `ContinueDebugEvent`. That would add a debugger-owned
execution-control loop, change exception handling and introduce documented
Console-lock risk. It contradicts this packet's no-second-scheduler and
preserve-native-execution constraints, so it is explicitly not admitted as an
S24 substitute.

Authoritative sources: [process debugging flags and descendant behavior](https://learn.microsoft.com/en-us/windows/win32/debug/process-functions-for-debugging),
[debug-event wait contract](https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-waitfordebugevent),
and [debug-event continuation contract](https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-continuedebugevent).

## Original OpenNT Ownership Check

The selected original BaseSrv implementation does not maintain a registry of
arbitrary native Win32 descendants. Its VDM service table exposes
`BaseCheckVDM`, `BaseUpdateVDMEntry`, `BaseGetNextVDMCommand`, `BaseExitVDM`
and related VDM calls. `srvvdm.c` maintains `DOSRECORD`, `WOWRECORD` and
`CONSOLERECORD`; it creates wait-object pairs and completes those original
VDM/WOW records. There is no original native-child event API, native process
tree, Job, Console-process enumeration, or native descendant completion
algorithm to recover.

That distinction is material: `CMD -> CMD -> EDIT` is normally a Windows
native parent/child chain whose waits and exit propagation remain Windows'
responsibility. NTSRV must be authoritative for the direct request it admits
to NTW32, but a full mirror of each Windows descendant would be a new product
task scheduler/registry rather than a restoration of BaseSrv semantics.
The authoritative direct-request boundary is therefore both the smallest
standalone design and the closest available original-owner boundary.

Source evidence: `src/opennt-host/base/win32/server/srvinit.c` (VDM service
table) and `src/opennt-host/base/win32/server/srvvdm.c` (`DOSRECORD`,
`WOWRECORD`, `CONSOLERECORD`, wait/dispatch paths).

## Helperless Existing-Console Association

The former standalone `run16 --internal-console-probe` performed a second
process's `AttachConsole`/membership query solely to associate a launcher
with an existing original `CONSOLERECORD`. It was not an OpenNT execution
path and introduced a private pipe, helper lifetime and avoidable surface.

The current protocol-23 candidate removes that executable mode and all of
its production sources, build objects and dedicated test. The already
authenticated launcher connection now calls `ReportConsoleMembers` with the
PID list returned by its own `GetConsoleProcessList`. BaseSrv validates the
connection PID/generation and requires that the snapshot contain that caller;
it compares the list only with registered connection PIDs to select an
existing `CONSOLERECORD`. The report carries no Console handle, worker
identity, command, receipt or completion authority.

Focused proof on the current x86 build:

- protocol-23 MIDL, `run16.exe`, `ntsrv.exe`, `ntcon.exe` and `ntw32.exe`
  linked from `build/M0-T423/S24-helperless-clean`;
- the reservation/lifecycle fixture passed, including its same-Console
  `CheckDOS -> existing worker -> GetNextVDMCommand` reuse case after a
  valid helperless member report;
- worker-base next-command, NTW32 close and the 413-check NTW32 request
  lifetime fixtures passed;
- the NTW32 execution fixture completed the new report RPC, then failed at
  its pre-existing native `cmd.exe` exit-code assertion. The retained
  protocol-22 baseline fails at the same assertion, so this is not evidence
  of the replacement and is not counted as a pass.

This is bounded transport cleanup, not S24 closure: the native participant
projection and full frontend/bootstrap integration remain active work.

The same audit found one local NTW32 rollback mismatch: it moved a fetched
command's attachments before `CreateThread` succeeded, so the common
worker-base caller could not perform its normal `Complete -> dispose`
rollback on that failure. Attachment ownership now transfers only after the
serving thread exists; pre-thread failures leave the command intact for the
common rollback. The current x86 `ntw32.exe` relinked with this correction.
