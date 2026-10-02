# T424 S4 broker-centered launch and control migration

## Question, baseline and method

Owner admits implementation after the component-edge audit: NTSRV creates,
authenticates, binds and retires frontend/workers; run16 keeps task submission
and direct-result waiting, except its actual Console transfer to NTKVM.
The baseline is main `f9fe709aa`, S3 protocol/RPC 29 and its tested eight-file
publication. Initial worktree inspection was clean. This initial record is a
source audit/admission, not a new build, runtime pass or published capability.

Read the repository authority set and source policy. Inspect the actual
launcher bootstrap, worker creation, native request/receipt and server record
paths, distinguishing project-owned `ntsrv-exe/opennt/source` adapters from
immutable `src/opennt-host` mirrors. Repeatable inspection commands:

```powershell
git status --short
rg -n 'CreateProcess|PrepareWorker|ReserveWorker|WatchBroker|wait_direct|BaseUpdateVDM' src/run16-exe
rg -n 'AcquireFrontendRoot|CompleteWorkerChannel|BindNativeTarget|PrepareWorker|NativeExitCode' src/ntsrv-exe/opennt/source/base_service.c
rg -n 'broker_lifetime_watch|WatchBroker|WaitForMultipleObjects' src/ntsrv-exe/opennt/source/base_rpc_client.c
rg -n 'CreateProcess|GetExitCodeProcess|complete_next_command|receipt' src/ntw32-exe/execution.c
```

Source conclusions have high confidence for the named current paths. Runtime
equivalence of the proposed migration remains unproved until the tests below
run against its production implementation.

## Current edges versus required edges

| Edge | Current production purpose/source | Migration decision |
| --- | --- | --- |
| run16 -> NTSRV | `main.c`, `frontend_scope.c`: discovery, original Check/Update, root/worker reservation, submission and result lookup. | Keep discovery/typed submission/results; move resource creation orchestration to service. |
| run16 -> NTKVM | `bootstrap_client.c`: creates frontend, inherits Console-handoff capabilities, receives bootstrap acknowledgement; scope owns lease-return/restored events. | NTSRV creates frontend. Keep only authenticated actual-caller Console transfer/return; do not lose restored-before-outer-CMD-input barrier. |
| run16 -> NTVDM | `worker_launch.c` + `main.c`: creates suspended worker, Prepare/Update, resumes, sometimes waits on worker process as well as DOS record. | Remove launcher creation/process ownership and worker-death result inference; NTSRV creates/binds and completes failures. Retain original DOS receipt/result shape. |
| run16 -> NTW32 | `frontend_scope.c`, `native_request_client.c`: creates/selects worker; direct launch payload/reply/final-status pipe and null-payload parent-resume request; waits worker/root handles. | Remove these task/control edges. Copied launch request plus typed stream attachments and structured result are broker-owned; parent execution resume is broker-routed. |
| NTSRV -> frontend/workers | Registration, process watches, request/route binding, highest-priority orderly close. No actual frontend/worker creation today. | Add finite exact-sibling creation to the existing authenticated admission; reuse watches and reservations, not a second registry. |
| NTKVM <-> workers | `worker-base/console_client.c`, executable-local console bindings: copied frame/input operations, ownership transfer and final I/O acknowledgements. | Keep direct I/O only; authentication/transport cancellation is not lifecycle authority. |
| workers -> NTSRV | Original DOS GetNext/completion; native GetNext, registration, shutdown event and direct result report. | Preserve original shape; extend only project-owned native payload/status boundary as required. |
| worker -> inner run16 | Original shell-out process creation/wait, not a task-control IPC channel. | Keep original execution semantics; inner launcher uses same broker-only task path. |
| NTMON -> NTSRV | Copied TaskSnapshot and explicit TerminateWorker; no direct worker traffic. | Already matches; verify unchanged. |

Required topology (Console transfer is the deliberately retained exception):

```text
run16 -------- task admission / result -------- NTSRV -------- NTMON
  |                                              |
  | real caller Console handoff/return            | create / bind / control
  v                                              v
NTKVM <-------------- direct I/O -------------- NTVDM / NTW32
```

NTSRV does not forward frames, own a visible/hidden Console, become a GUI
launcher service or execute native text targets. NTW32 still owns target
CreateProcess/wait/Windows result. GUI boundaries still separate character
sessions. The drawing describes target ownership, not a current runtime pass.

## Source and reusable-mechanism ledger

| Mechanism and provenance | Current owner/location | Target and reason |
| --- | --- | --- |
| Original DOS Check/Update/GetNext/exit record semantics | `opennt-host/base/win32/client` and `server/srvvdm.c`, MVDM callers | Unchanged originals. Service adapter calls/binds them; never extract scheduler or rewrite records for native shape. |
| Project-added suspended worker creation and startup rollback | `run16-exe/worker_launch.c`, `main.c`, scope native creation | NTSRV finite worker-spawn owner. Preserve immutable worker command/environment/configuration and Prepare-before-Resume order. No successful-worker launcher kill pairing. |
| Project-added frontend bootstrap | `run16-exe/bootstrap_client.c`, `ntkvm-exe/main.c` | Split creation from Console transfer. Service authorizes exact frontend generation; actual caller transfers restricted pipe/event/process capabilities. Authenticate both authorities; never attach frontend to service Console. |
| Native packet bounds/resource binding | `run16-exe/native_launch_packet.c`, `native_launch.c` and NTW32 execution | Reuse existing codec/bounds and worker-local target creation. Cross RPC only copied/versioned payload and typed authenticated stream attachments; no trusted sender-local handle numbers or generic duplication API. |
| Native direct request pipe/result transport | `native_request_client.c`, `native_request_io.c`, service Win32Record, NTW32 execution | Broker submission/receipt/result owns launch/preflight error, actual exit code and final I/O status. Remove replaced launcher-to-worker pipe; do not add a parallel completion queue with independent truth. |
| Broker process death watcher | `ntsrv-exe/opennt/source/base_rpc_client.c`: `broker_lifetime_watch`, `OpenNtBaseClientWatchBroker` | Reuse authenticated broker handle and infinite event wait for run16, frontend and both workers. No heartbeat/RPC poll. Arm before admitted waits; ensure startup rollback ownership is handed to service first. |
| Project worker transport/shutdown helpers | `worker-base`, worker-local consumers | Keep reusable client mechanisms in worker-base. NTSRV control has priority, backend close/guest failure stays local. No peer-lifetime policy in I/O client. |
| Frontend UI and render resources | `ntkvm-exe` | Keep CAF/AE/X, capture release, text/graphics rendering and actual Console close handling. These local UI actions are not server scheduling. |
| Original/native target execution and descendants | NTVDM original guest owners / NTW32 execution / Windows | Unchanged execution boundaries and actual exit results. No recursive kill, Job tracking, Observed records or task-stack inference. |

No source/media import is needed. Existing source-first binding is the reusable
rung; standalone NTSRV orchestration is project-added product policy, not an
original CSRSS implementation claim. No mirror diff is admitted. If a mirror
hook appears necessary, record it as a stop condition rather than silently
enlarging this S.

## Lifetime and error contract

1. Both DOS and native text already report real task exit codes through NTSRV;
   this does not mean DOS already satisfies all target edges. Newly created DOS
   worker ownership and process fallback waits still reside in run16; native
   startup/final I/O status still resides on a direct pipe. Migrate both.
2. Idle RPC context existence is not death notification. The existing broker
   process capability is obtained/authenticated by the RPC client and waited
   without a timeout alongside local stop. Preserve it for all nonmonitor
   connected components. NTMON remains available when disconnected.
3. NTKVM has no idle or worker-death retirement policy. It may wait indefinitely
   for broker work. NTSRV owns the finite ten-second startup/workerless-root
   deadline, cancellation on legitimate new work and empty-service grace.
   A live frontend/worker/admission prevents service-empty retirement. This is
   the owner-approved target; S3's immediate orphan retirement is the current
   published implementation until migrated.
4. Shutdown instructions precede ordinary requests/I/O. Every component can
   still fail unrecoverably; report/observe failure distinctly from orderly
   retirement. A root's true user Console closure remains a frontend boundary.
5. Task completion, I/O release, component retirement and native descendant
   lifetime are distinct. Nonzero real target exit is a normal task result;
   broker/worker failure is not a fake target exit. Preserve failure 1722/1067
   where established, without automatic task replay/reconnect.
6. Service creation is restricted to admitted product frontend/worker images,
   authenticated caller/environment/desktop and existing reservation ownership.
   Do not expose a generic remote CreateProcess or raw-handle duplication API.
   Pre-resume failed creation can roll back; a handed-off worker cannot be
   treated as launcher-owned cleanup. Do not wait on creation/registration
   while holding a lock needed by the created component's Connect/register.
7. Borrowed outer CMD may regain input only after canonical Console/input modes
   and route have been restored. This acknowledgement may stay in the allowed
   run16/NTKVM Console-transfer boundary; it is not worker task completion or
   waiting for the entire resident frontend to exit.

## Ordered implementation checklist

Open rows remain open until production wiring, obsolete-path removal and exact
test evidence exist. Documentation/source audit alone checks no runtime row.

- [x] Admit S4, preserve S3 source/publication baseline, separate current and target.
- [x] Audit creation/completion/Console-transfer edges and reusable broker watch.
- [ ] Define finite typed broker launch/result attachments, coherent next RPC/application protocol and generation authentication; no speculative unrelated methods.
- [ ] Service-created NTKVM with real-caller Console bootstrap/return and same-root reuse; remove frontend CreateProcess from run16.
- [ ] Service-created NTVDM/NTW32 with existing original Check/Update and native GetNext contracts; remove launcher worker CreateProcess/resume/process fallback waits.
- [ ] Broker-only native launch/preflight/completion/I/O-status receipt plus broker-routed parent resume; delete replaced direct launcher/worker request path.
- [ ] Both workers/frontend obey broker control; broker-owned ten-second workerless deadline; no local idle policy or new death polling.
- [ ] Unified launcher direct service wait/failure/result flow, retaining original DOS and native result sources, Win16/GUI startup-only and allowed Console restoration barrier.
- [ ] Exact creation/binding/error/rollback tests, runtime matrices, previous frontiers, edge/mirror audit, coherent publication and commit/push.

Follow dependency order. Do not publish an intermediate mixture of old/new
interfaces. An admission/document-only P leaves O:/winnt's accepted S3 set
unchanged. Reuse valid incremental build cache under build/; sealed S3 runtime
and evidence are not overwritten.

## Verification to implement and run

| Required invariant | Test source/entrypoint and assertions |
| --- | --- |
| Exact process ownership and removed edges | Add controlled service-launch fixture/probe under tests/app or tests/observation: record actual parent identity/generation for NTKVM/NTVDM/NTW32; assert launcher only creates missing NTSRV, no direct native launch/completion pipe remains. Static edge audit supplements, not replaces runtime. |
| Bootstrap safety | Extend `tests/app/frontend_bootstrap_test.c` and frontend-scope lifetime tests: exact actual Console caller, service-issued creation identity, unrelated/old-generation/forged capability rejected; no arbitrary image/handle inheritance. |
| Native direct result | Extend next-command/execution and service reservation fixtures: preflight/CreateProcess failure, actual nonzero exit, final I/O failure, completion RPC failure, launcher death and broker/worker loss; exactly one corresponding result, no unrelated record completion. |
| DOS/native repeated launch and cooked return | `tests/observation/verify-frontend-relaunch.ps1`: same outer CMD, DOS -> native -> DOS, final cooked echo result19, original Console/cursor/input restoration barrier. |
| Deep nested handoff | `tests/observation/verify-command-native-edit-return.ps1`: COMMAND -> CMD -> modern EDIT -> CMD -> DOS MEM and completion; preserve screen/title/geometry/input and parent-resume ordering. |
| Broker-authoritative retirement | `tests/observation/verify-broker-retirement.ps1` plus focused deadline tests: frontend/worker loss for both kinds, pending startup, workerless10s cancellation, independent session survives, highest-priority control wakes blocked GetNext, broker death wakes all nonmonitor clients without polling. |
| Independent native roots | `tests/observation/verify-ntw32-management.ps1 -TwoSessions`: exact authorized close and unaffected session input/result23, no cross-root reuse. |
| Product regressions | Same Console17 and Window17 scripts/arguments as S3 evidence, guest output-gated 17/17 each; retained WINMINE/SOL/WRITE frontiers via `tests/observation/observe-wow-frontiers.ps1`, do not relabel known memory errors as usability passes. |
| Wire coherence | Regenerate MIDL and rebuild affected x86 links together; real service rejects old interface, protocol and application version; no mixed-package registration. |
| No mirror/lib/guest changes | Review `git diff -- src/mvdm src/opennt-host lib` and guest/config identities, exact source/build graph, deletion of superseded clients and auth/ownership/lock/failure contract review. |
| Delivery | Governance, relative links, `git diff --check`, eight-file tested/publication hash equality, recoverable S3 set, reviewed sequential P and clean pushed main. T remains open. |

Physical desktop/RDP observations remain unclaimed unless actually performed.
Windows GUI/native descendant execution remains Windows-owned; tests must not
fabricate a direct run16 receipt for unadmitted descendants. The launch syntax
is unchanged, including ordinary arguments without a mandatory `--` separator.

## Initial result and follow-up

Admission and source audit completed; all production migration/runtime rows
remain open. This is not S4 closure. Next work follows the ordered checklist,
beginning with the exact creation/Console-transfer and typed native request
contracts before modifying callers. Published S3 remains the side-test baseline.

Admission validation: `Verify-DocumentationGovernance.ps1` and
`Test-DocumentationRelativeLinks.ps1` both pass. The initial governance run
correctly rejected a missing evidence link inside the retained S3 closure
section; the link was added, without weakening the gate. `git diff --check`
passes. This documentation-only admission P does not require or claim a new
build/test/deployment. No source, mirror, guest or published file was modified.
