# T424 S4 broker-centered launch and control migration

## Question, baseline and method

Owner admits implementation after the component-edge audit: NTSRV creates,
authenticates, binds and retires frontend/workers; run16 keeps task submission
and direct-result waiting. The owner's subsequent refinement removes the
Console-handoff IPC exception too: all coordination/acknowledgement goes
through NTSRV, and only actual Console operations stay in NTKVM.
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

## Owner-directed bounded replanning disposition

The owner requests S4 conclusion and a new stage routing Win32 GUI through
run16 -> NTSRV -> NTW32 with worker-side GUI/text classification and service-held
GUI handles projected into NTMON UNBOUND. Source inspection still finds local
`launch_gui` and PE subsystem selection in run16/main.c; NTMON currently has
only worker snapshots, not an independent UNBOUND section. No runtime pass is
claimed for that new route.

The owner's subsequent clarification supersedes this premature conclusion:
S4 remains active with DOS/WOW service creation, broker-only native submission/
preflight/final-status/parent-resume, obsolete-path/fixture removal and full
regression/publication still required. S5 follows S4 delivery and owns GUI
routing/registration only; monitor/UNBOUND display belongs to the queue-head
NTMON T candidate. Preserve all candidate changes and focused evidence.
O:/winnt remains S3; no production delivery is claimed.
The new S5 acceptance and worker-release question are recorded in the
[working plan](../operations/t424-worker-frontend-renaming-plan.md).

## Current edges versus required edges

| Edge | Current production purpose/source | Migration decision |
| --- | --- | --- |
| run16 -> NTSRV | `main.c`, `frontend_scope.c`: discovery, original Check/Update, root/worker reservation, submission and result lookup. | Keep discovery/typed submission/results; move resource creation orchestration to service. |
| run16 -> NTKVM | `bootstrap_client.c`: creates frontend, inherits Console-handoff capabilities, receives bootstrap acknowledgement; scope owns lease-return/restored events. | Remove this edge entirely. NTSRV creates/authorizes frontend and coordinates Console takeover/return acknowledgements; preserve restored-before-outer-CMD-input barrier through service receipts. |
| run16 -> NTVDM | `worker_launch.c` + `main.c`: creates suspended worker, Prepare/Update, resumes, sometimes waits on worker process as well as DOS record. | Remove launcher creation/process ownership and worker-death result inference; NTSRV creates/binds and completes failures. Retain original DOS receipt/result shape. |
| run16 -> NTW32 | `frontend_scope.c`, `native_request_client.c`: creates/selects worker; direct launch payload/reply/final-status pipe and null-payload parent-resume request; waits worker/root handles. | Remove these task/control edges. Copied launch request plus typed stream attachments and structured result are broker-owned; parent execution resume is broker-routed. |
| NTSRV -> frontend/workers | Registration, process watches, request/route binding, highest-priority orderly close. No actual frontend/worker creation today. | Add finite exact-sibling creation to the existing authenticated admission; reuse watches and reservations, not a second registry. |
| NTKVM <-> workers | `worker-base/console_client.c`, executable-local console bindings: copied frame/input operations, ownership transfer and final I/O acknowledgements. | Keep direct I/O only; authentication/transport cancellation is not lifecycle authority. |
| workers -> NTSRV | Original DOS GetNext/completion; native GetNext, registration, shutdown event and direct result report. | Preserve original shape; extend only project-owned native payload/status boundary as required. |
| worker -> inner run16 | Original shell-out process creation/wait, not a task-control IPC channel. | Keep original execution semantics; inner launcher uses same broker-only task path. |
| NTMON -> NTSRV | Copied TaskSnapshot and explicit TerminateWorker; no direct worker traffic. | Already matches; verify unchanged. |

Required topology (no launcher/frontend IPC exception):

```text
run16 ----- task / Console-handoff control ----- NTSRV ----- NTMON
                                                  |
                                     create / bind / control / receipts
                                          /               \
                                       NTKVM <--- I/O ---> NTVDM / NTW32
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
| Project-added frontend bootstrap | `run16-exe/bootstrap_client.c`, `ntkvm-exe/main.c` | Service owns creation and authenticated handoff request/acknowledgement. Derive actual caller process from authenticated launcher connection; supply restricted capabilities to frontend. Remove direct launcher pipe/events; never attach frontend to service Console. |
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
   and route have been restored. NTKVM reports restoration to NTSRV; NTSRV
   signals the root launcher's service-owned handoff receipt. Task completion
   alone cannot signal restoration. Inner launchers cannot return the root's
   lease. This is not waiting for the entire resident frontend to exit or
   restoring an obsolete startup geometry/cursor snapshot.

## Ordered implementation checklist

Open rows remain open until production wiring, obsolete-path removal and exact
test evidence exist. Documentation/source audit alone checks no runtime row.

- [x] Admit S4, preserve S3 source/publication baseline, separate current and target.
- [x] Audit creation/completion/Console-transfer edges and reusable broker watch.
- [ ] Define finite typed broker launch/result attachments, coherent next RPC/application protocol and generation authentication; no speculative unrelated methods.
- [ ] Service-created NTKVM with broker-mediated real-caller Console bootstrap/return/restoration receipts and same-root reuse; remove frontend CreateProcess and all direct NTKVM IPC from run16.
- [ ] Service-created NTVDM/NTW32 with existing original Check/Update and native GetNext contracts; remove launcher worker CreateProcess/resume/process fallback waits.
- [ ] Broker-only native launch/preflight/completion/I/O-status receipt plus broker-routed parent resume; delete replaced direct launcher/worker request path.
- [ ] Both workers/frontend obey broker control; broker-owned ten-second workerless deadline; no local idle policy or new death polling.
- [ ] Unified launcher direct service wait/failure/result flow, retaining original DOS and native result sources, Win16/GUI startup-only and broker-mediated Console restoration barrier.
- [ ] Exact creation/binding/error/rollback tests, runtime matrices, previous frontiers, edge/mirror audit, coherent publication and commit/push.

Follow dependency order. Do not publish an intermediate mixture of old/new
interfaces. An admission/document-only P leaves O:/winnt's accepted S3 set
unchanged. Reuse valid incremental build cache under build/; sealed S3 runtime
and evidence are not overwritten.

## Verification to implement and run

| Required invariant | Test source/entrypoint and assertions |
| --- | --- |
| Exact process ownership and removed edges | Add controlled service-launch fixture/probe under tests/app or tests/observation: record actual parent identity/generation for NTKVM/NTVDM/NTW32; assert launcher only creates missing NTSRV, no direct frontend bootstrap or native launch/completion IPC remains. Static edge audit supplements, not replaces runtime. |
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

## First implementation checkpoint — uncommitted candidate

The admission facts above describe the documentation P, not the current
candidate. Production migration has started. Protocol and RPC major are both
30; MIDL regeneration and the hard-coded client/server interface references
are synchronized. No mirror, guest or shared-library source changed.

Implemented candidate subset:

- NTSRV `StartFrontend` creates the exact sibling NTKVM suspended, records
  its process/event grant before Resume, and authenticates its bootstrap reply.
  Only the exact created process and inherited event objects can register its
  root/Console lease. The authenticated actual launcher process, never the
  service Console, is the AttachConsole target.
- run16 no longer creates NTKVM or sends its return event. `ReturnFrontendConsole`
  requests return through the service; NTKVM reports restoration via RPC.
  `WaitFrontendConsoleRestored` waits on the service condition variable, not
  the frontend process. The acknowledgement is retained for the individual
  launcher lease, independently of a reusable root event reset.
- Existing finite bootstrap transfer/restricted inheritance is reused. The
  broker process capability adds only query-limited identity access to its
  prior synchronize access, not terminate/duplicate-resource rights.
- Existing scope/channel lifetime assertions remain. The bootstrap test
  executable now selects the broker-owned fixture, preserving `--identity`,
  five adversarial reply cases and the ten-second silent-peer timeout. Fake
  providers are confined to an isolated build test package; production RPC
  does not accept a caller-selected executable.

Initial real bootstrap failed with pipe error 109; bounded process-exit
attribution exposed access denied 5. Existing opt-in trace localized this to
duplicating the authenticated broker handle with query access that its old
wait-only grant did not contain. The minimal read-only grant correction
resolved it; temporary stage/argument tracing was removed. These failed runs
are retained, not counted as passes.

Exact commands executed (PowerShell, from repository root):

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot build/M0-T424/S2/r001 -NodeExecutable O:/.nvm/versions/node/v22.22.1/bin/node.exe
cmd.exe /c build\M0-T424\S2\r001\run-ninja-parallel.cmd product-programs
cmd.exe /c build\M0-T424\S2\r001\build-supplement.cmd
cmd.exe /c build\M0-T424\S2\r001\run-ninja-parallel.cmd frontend-scope-lifetime-test.exe frontend-bootstrap-test.exe broker-frontend-bootstrap-test.exe
build/M0-T424/S2/r001/frontend-scope-lifetime-test.exe
$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
$repo=(Resolve-Path .).Path
build/M0-T424/S2/r001/observer.exe (Join-Path $repo 'build/M0-T424/S2/r001/broker-frontend-bootstrap-test.exe') O:/winnt (Join-Path $repo 'build/M0-T424/S4/r001/broker-bootstrap-grant.txt') --observation-timeout-ms 20000 probe
build/M0-T424/S2/r001/observer.exe (Join-Path $repo 'build/M0-T424/S2/r001/frontend-bootstrap-test.exe') O:/winnt (Join-Path $repo 'build/M0-T424/S4/r001/bootstrap-rejections.txt') --observation-timeout-ms 25000 --startup-rejections
Remove-Item Env:MVDM_OBSERVER_PRIVATE_DESKTOP
```

Results: x86 product links and WOW32 relink exit 0; scope fixture passes all
four marker groups; actual RPC bootstrap/parent/authentication/return/broker-loss
probe exits 0. Five rejection cases exit 0 with errors 1306, 1306, 5, 109 and 5,
respectively; each preserves local handle count and returns no capabilities.
Reports and Console output are under build/M0-T424/S4/r001.
The same observer command with `--startup-timeout`, a 20000ms observation
bound and report `bootstrap-timeout.txt` also exits 0: the production ten-second
deadline returns 1460, preserves local handle count and clears all returned
capabilities. No test-owned service/frontend remains after cleanup.

A coherent protocol-30 eight-file candidate was assembled under
build/M0-T424/S4/r001/runtime using unchanged S3 guest/configuration copies.
With this runtime mapped temporarily as Z:, isolated `run16 command.com /c ver`
and `run16 cmd.exe /c ver` both exit 0 and display the actual DOS/native version.
These are two focused output-gated smokes, not the Console17/Window17 matrix.
Fixture-owned processes were cleaned up and Z: removed. O:/winnt was not
modified, and no candidate publication, P delivery or S4 closure is claimed.

Still open: service-created workers; broker-only native launch/final-status and
parent-resume; removal of worker/root fallback waits; service ten-second
workerless-root policy; old manually registered frontend fixture migration and
unused old bootstrap test/API cleanup; same-root concurrency and generation
negatives; full product/WOW regression and publication gates. Strict root
creation grants intentionally require those old fixtures to use a real
service-created root; no production test bypass is admitted.

## Native creation and broker-owned grace checkpoint — uncommitted

The first checkpoint's open-worker/grace rows above are superseded only for
the following tested subset; S4 remains open.

`StartNativeWorker` now selects a registered same-Console native worker or
creates the exact same-package `ntw32.exe` in NTSRV through the existing
reservation. The recovered startup transaction in
`src/ntsrv-exe/transport/worker_spawn.c` keeps its temporary startup-only Job
through Prepare/Resume and disarms it before successful handoff. It has no
Job observer or target lifetime pairing and accepts no RPC-selected image.
The native creation body in run16's scope is deleted. DOS/WOW still uses its
old launcher startup transaction and remains an explicit migration row.

NTSRV's existing event/timer loop now owns a cancellable ten-second deadline
for workerless roots, not immediate S3 orphan retirement. A live associated
worker clears that deadline. Last-worker loss starts a fresh grace; rechecks
do not renew it. Legitimate authenticated startup is still bounded. NTKVM
has no added timer, member poll or autonomous idle decision.

Exact focused commands, using the previously recorded x86 cache:

```powershell
cmd.exe /c build\M0-T424\S2\r001\run-ninja-parallel.cmd product-programs broker-frontend-bootstrap-test.exe frontend-scope-lifetime-test.exe
build/M0-T424/S2/r001/frontend-scope-lifetime-test.exe
$repo=(Resolve-Path .).Path
$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
build/M0-T424/S2/r001/observer.exe (Join-Path $repo 'build/M0-T424/S2/r001/broker-frontend-bootstrap-test.exe') O:/winnt (Join-Path $repo 'build/M0-T424/S4/r001/broker-worker-parent-grace.txt') --observation-timeout-ms 20000 probe
build/M0-T424/S2/r001/observer.exe (Join-Path $repo 'build/M0-T424/S2/r001/broker-frontend-bootstrap-test.exe') O:/winnt (Join-Path $repo 'build/M0-T424/S4/r001/broker-workerless-grace.txt') --observation-timeout-ms 20000 --workerless-grace
build/M0-T424/S2/r001/observer.exe (Join-Path $repo 'build/M0-T424/S2/r001/broker-frontend-bootstrap-test.exe') O:/winnt (Join-Path $repo 'build/M0-T424/S4/r001/broker-workerless-cancel.txt') --observation-timeout-ms 35000 --workerless-cancel
Remove-Item Env:MVDM_OBSERVER_PRIVATE_DESKTOP
cmd.exe /c build\M0-T424\S2\r001\build-supplement.cmd
```

All three actual process probes report exited/0 and their exact marker
assertions. The parent probe confirms NTSRV created both NTKVM and NTW32,
rejects a launcher trying to register itself as a root, and proves broker
loss terminates both clients. The grace probe observes survival for eight
seconds then service-requested exit. The cancellation probe observes survival
past eleven seconds with a worker, kills only its own exact test worker, then
observes a fresh ten-second root grace. Toolhelp parent enumeration is test
evidence only; no production process enumeration was introduced.

After assembling the coherent eight-file candidate under
build/M0-T424/S4/r001/runtime and mapping only that runtime to Z:, this command
passed with actual conventional-memory output at both DOS prompts, native VER,
and the outer CMD's real exit 19:

```powershell
tests/observation/verify-frontend-relaunch.ps1 -Observer build/M0-T424/S2/r001/observer.exe -PackageRoot Z:/ -ReportPath (Join-Path $repo 'build/M0-T424/S4/r001/native-service-create-relaunch.txt')
```

Exact candidate-owned processes were cleaned up; a later explicit `subst Z: /d`
removed the mapping after the command session handle was no longer present.
These tests do not prove the full Console17/Window17/WOW gate. O:/winnt is
unchanged. The subsequently built launcher change arms the existing
authenticated, nonpolling broker-process watcher immediately at connection
rather than after worker startup, removing four redundant call sites. It
requires a new runtime comparison before any production delivery.

Next required production work is still DOS/WOW service-created startup and
copied, typed native submission/result/resume through NTSRV. The existing
launcher-to-native-worker pipe, remote stream-handle exchange, root/worker
fallback waits and obsolete manually registered-root fixtures remain open;
their retention in this intermediate tree is not a final architecture claim.

## Native failure receipt checkpoint — uncommitted

The launcher watcher is armed at broker admission. Native completion now waits
only on its service receipt; the old worker/root process fallback is removed
from that wait. Before attempting the still-retained final presentation pipe,
the client queries the broker result, so worker failure cannot block reading
a reply from a dead worker. The direct submission/final-status/resume pipe is
still an open migration row, not an accepted final edge.

On native worker rundown, NTSRV preserves each existing receipt-bearing
Win32Record on its authenticated launcher's existing record list. An unfinished
record becomes a terminal infrastructure failure (1067, no invented target
exit code); an already completed record retains its actual exit code. Records
without a live launcher are deleted. No separate completion registry or
single-slot result cache is introduced. Record transfer, receipt signalling,
disconnect cleanup and result query share the existing service lock. Queries
remain authenticated by launcher generation and request, consume only their
own terminal record once, and do not count retained results as active tasks.
Original DOS/WOW records and mirror source are unchanged.

Affected x86 product links and the supplemental WOW32 link pass in the same
recorded cache. `frontend-request-client-test.exe` passes its existing launch,
reply rejection, final-presentation and resume assertions plus the new case:
a broker 1067 result returns without reading an invalid presentation channel.
`frontend-scope-lifetime-test.exe` retains all four existing resource/identity
groups. The actual production client/provider probe is:

```powershell
cmd.exe /c build\M0-T424\S2\r001\run-ninja-parallel.cmd product-programs frontend-request-client-test.exe frontend-scope-lifetime-test.exe frontend-bootstrap-test.exe
build/M0-T424/S2/r001/frontend-request-client-test.exe
build/M0-T424/S2/r001/frontend-scope-lifetime-test.exe
cmd.exe /c build\M0-T424\S2\r001\build-supplement.cmd
$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
build/M0-T424/S2/r001/observer.exe (Join-Path $repo 'build/M0-T424/S2/r001/frontend-bootstrap-test.exe') O:/winnt (Join-Path $repo 'build/M0-T424/S4/r001/native-failure-retained-record.txt') --observation-timeout-ms 25000 --native-worker-failure
Remove-Item Env:MVDM_OBSERVER_PRIVATE_DESKTOP
```

That probe reports exited/0 and explicitly proves: actual service-created
worker and live CMD target, injected exact worker death, broker receipt wakes,
wrong request cannot consume the result, correct request returns 1067 without
dead-channel I/O, and a second query returns NOT_FOUND. Test-only process
termination is not production policy. The fixture's executable is in build;
using O:/winnt as its unchanged guest/root setting does not deploy any binary.

The candidate eight-file package was refreshed under build/M0-T424/S4/r001/runtime.
With only that package temporarily mapped to Z:, the following strict probes
exercise the launcher watcher and result path. Four fault cases require the
specific broker task failure 1067, not merely a nonzero exit. The frontend
wait bound now covers the approved ten-second service grace rather than the
old eight-second immediate-retirement expectation; no production Sleep or
test success bypass is added.

```powershell
tests/observation/verify-broker-retirement.ps1 -Observer build/M0-T424/S2/r001/observer.exe -PackageRoot Z:/ -ProcessPackageRoot (Join-Path $repo 'build/M0-T424/S4/r001/runtime') -LogRoot (Join-Path $repo 'build/M0-T424/S4/r001') -LogPrefix retained-record-retirement
tests/observation/verify-frontend-relaunch.ps1 -Observer build/M0-T424/S2/r001/observer.exe -PackageRoot Z:/ -ReportPath (Join-Path $repo 'build/M0-T424/S4/r001/retained-record-relaunch.txt')
```

The four latest fault cases pass. Same-outer-CMD relaunch also passes with
both DOS MEM outputs, native VER and outer exit 19. Z: is removed after this
run; no full Console17/Window17/WOW verdict, publication, commit or S4 closure
follows from these focused checks. DOS/WOW
service-created startup, copied broker-only native submission/final-status and
parent-resume, stale fixture/API cleanup and full regression remain open.

Additional result comparison: `--native-completed-worker-loss` runs a real
CMD target exiting 37, waits for its broker completion, kills the exact worker
and queries the result after actual process death. The result remains 37;
wrong-request and duplicate-consumption negatives pass. Its runtime assertion
does not establish the exact callback timing: preservation on worker rundown
is additionally verified by the source ownership/lock review and the unfinished
target's receipt test (which cannot wake before service failure handling).
Final probe reports are `record-result-native-worker-failure.txt` and
`record-result-native-completed-worker-loss.txt`, both exited/0 with their
strict markers. Invoke the same observer command above with these fresh report
names and the corresponding case argument.

The earlier `retained-record-native-worker-failure.txt` is a retained test
harness failure, not a product pass: adding SelectNativeWorker as a death
observer returned INVALID_STATE (5023), because that stateful selection API
is not a management query for a launcher already holding a reservation. The
probe was corrected to wait on the exact test process and the service receipt,
without changing production code or adding delays/polls. The failure log is
not removed. Final governance/relative links and diff checks pass. Candidate
test processes and Z: have been cleaned up; O:/winnt remains accepted S3.

## DOS/WOW broker creation candidate (uncommitted, not published)

StartVdmWorker consumes the authenticated original Check result. NTSRV selects
its trusted sibling ntvdm.exe, prepares the original configuration, creates
suspended, calls its existing source-shaped Update adapter, requests the DOS
frontend, and resumes. run16 no longer creates/resumes this new worker. This
does not yet remove its old worker-result fallback or native direct pipe.

Recovery ledger: directly linking the original vdm.c client archive was
attempted and failed with OpenNtBaseClientCallServer/CsrPortHeap imports that
belong to client RPC/capture, not the service. The smallest usable next rung is
a generated build-only carrier selecting the unchanged BaseGetVdmConfigInfo
and BaseSetLastNTError bodies from that tracked mirror. The build manifest
records source identity and the selected closure. Existing base_config binding
supplies package paths; no mirror modification, new configuration algorithm or
fake CSR client is used. Exceptional intrusion and new algorithm rungs are
unnecessary and not selected.

The x86 product-programs and broker-frontend-bootstrap-test.exe links, followed
by build-supplement.cmd, passed. The first production relaunch test did not:
service-vdm-create-relaunch.txt reports timeout; captured Console lines never
show the DOS prompt. The Windows PowerShell 5 invocation first failed before
launch (ArgumentList unavailable); rerunning with current PowerShell produced
the actual timeout. Neither result is a pass.

Read-only existing worker_thread_snapshot.c captured the exact private-test
worker waiting in Client_WaitFrontend. Process enumeration proved NTSRV was
the actual parent of NTKVM and NTVDM. Source comparison against Server_Update
identified the borrowed receipt-table event being passed directly as a typed
RPC output. Typed transfer consumes that handle; the existing Update path
instead uses export_handles to duplicate it. The new admission callback now
prepares its own export duplicate before Resume, preserving the table event.
All fallible worker handle preparation likewise precedes Resume; after the
worker can consume a published record there is no export-failure rollback.
The corrected candidate's actual startup/reuse retest is recorded below.

Existing probe source/command:

```powershell
cmd.exe /c build\M0-T424\S2\r001\run-ninja-parallel.cmd product-programs broker-frontend-bootstrap-test.exe
cmd.exe /c build\M0-T424\S2\r001\build-supplement.cmd
# Copy only the eight candidate binaries into build/M0-T424/S4/r001/runtime.
subst.exe Z: (Join-Path $repo 'build/M0-T424/S4/r001/runtime')
try {
    & tests/observation/verify-frontend-relaunch.ps1 -Observer build/M0-T424/S2/r001/observer.exe -PackageRoot Z:/ -ReportPath (Join-Path $repo 'build/M0-T424/S4/r001/service-vdm-create-relaunch.txt')
} finally { subst.exe Z: /d }
```

Use a fresh report path for every repeat. Test-only failed-session cleanup
targeted the enumerated private probe PIDs, not user or published processes.
Full gate and S4 closure remain open; accepted O:/winnt remains S3.

Corrected run: service-vdm-create-relaunch2.txt reports exited/19; its line-03
and line-07 Console captures both contain MEM's total conventional-memory
output, line-05 contains native Microsoft Windows VER. The strict script
passes. This verifies real new DOS startup plus same-Console reuse across
native execution, not a mocked caller.

broker-frontend-bootstrap-test.exe now additionally calls the actual new RPC
without original Check admission and with an unterminated environment. It
requires INVALID_STATE/INVALID_PARAMETER respectively and no exported worker
or parent handles. Its existing authentic-root, exact service parent, Console
return and broker-loss assertions remain enabled. Observer run
service-vdm-admission-negative.txt reports exited/0 and its Console capture
contains all three strict PASS groups. These assertions do not prove every
post-Check rollback branch.

The existing observe-wow-frontiers.ps1 command with prefix
service-vdm-create-wow and PostExitObservationMs 5000 retains three independent
observations: WINMINE startup returns 0 and its guest main window remains;
SOL and WRITE startup return 0 and retain their known original memory-error
dialogs with live WOW workers. This is frontier preservation, not gameplay or
SOL/WRITE usability acceptance. Guest/configuration files were not modified.
Z: was removed after both actual workload runs. No S4 P, full regression gate
or O:/winnt publication has yet been delivered.

## Native broker-only control increment (uncommitted)

This increment supersedes the preceding progress note's still-open native
pipe statement, not its retained failure chronology. SubmitNativeRequest
receives a copied launch packet through authenticated RPC. NTSRV creates and
owns the connected worker-control pipe, retains it on the existing direct
Win32Record, and returns typed target/receipt attachments. FinishNativeRequest
consumes that record's actual completion and checks the final I/O reply. A
latched worker-failure result does not require reading a dead worker channel.
Native parent resume uses the same broker request with no launch payload.
NTW32 target creation/completion and direct worker/frontend I/O remain intact.
GUI routing/classification and monitor UNBOUND are not implemented here.

The actual initial production relaunch report
broker-native-control-relaunch.txt passes the strict same-outer-CMD DOS
MEM/EXIT, native VER, DOS MEM/EXIT, cooked CMD exit-19 assertions. This is
selected-chain evidence, not Console17/Window17 or S4 closure.

The first failure probes broker-native-control-failure.txt through failure5
exit 1. Their observer had not enabled its existing private-desktop mode;
the separately captured failure5 stdout reports bootstrap 87 before native
execution. Direct execution without a real Console reproduces that fixture
prerequisite failure. These runs are not production failure-receipt passes.
With MVDM_OBSERVER_PRIVATE_DESKTOP=1, failure6 exits 0 and its Console snapshot
asserts failure 1067, wrong-request rejection, one-time consumption and no dead
presentation-channel read. broker-native-control-completed-loss.txt exits 0;
its snapshot asserts actual target result 37 survives worker rundown and is
consumed once. Test-only cleanup uses its own target handles.

The old client-only fixture still expected launcher-owned pipes and was not
valid after migration. Its broker substitute now owns the byte channel and
uses production native_control reply validators selected into the NTSRV
binding archive. The same negative matrix remains: invalid version,
contradictory error/handle, EOF, partial reply, delayed final presentation,
WRITE_FAULT, missing final reply, resume rejection and broker failure. The
launcher receives no completion pipe. This focused fixture passes exit 0;
actual RPC authentication and process lifecycle remain separate probes.

Commands, using the existing admitted object cache:

```powershell
cmd.exe /c build\M0-T424\S2\r001\run-ninja-parallel.cmd product-programs frontend-request-client-test.exe broker-frontend-bootstrap-test.exe
& build/M0-T424/S2/r001/frontend-request-client-test.exe
$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
try {
    & build/M0-T424/S2/r001/observer.exe (Join-Path $repo 'build/M0-T424/S2/r001/broker-frontend-bootstrap-test.exe') (Join-Path $repo 'build/M0-T424/S4/r001/runtime') (Join-Path $repo 'build/M0-T424/S4/r001/broker-native-control-failure6.txt') --observation-timeout-ms 25000 --native-worker-failure
} finally { Remove-Item Env:MVDM_OBSERVER_PRIVATE_DESKTOP }
```

Product and focused-test x86 links pass.
After extracting the production validators, the refreshed eight-file build
candidate also passes broker-native-control-relaunch2.txt, using the same
strict script and exit-19/text assertions. Z: was removed in finally.
The final full regression/publication
gate is not run; unused old control API/fields and DOS process-result fallbacks
remain mandatory cleanup, not deferred S5 work. Accepted O:/winnt is unchanged.

## Broker-only DOS result and independent Console association increment

Run16 now waits only its service receipt for DOS, including the original
nonzero-DosSessionId branch whose Update returns no parent event. NTSRV
retains the created process and installs a one-shot process wait before
Resume, then translates its actual exit into a parent receipt. Original
cmdmisc.c CloseOnExit/TerminateVDM behavior and DOS records are unchanged.
Disconnect cancels and joins the callback before freeing the connection.
Native client worker/pipe fields and obsolete wrapper names are removed too.

broker-only-new-console-mem.txt failed with ACCESS_DENIED (5). Original
CheckDOS allocated a distinct execution Console, while RetainFrontendRoot
required the visible-root Console identity. The fix prepares a ConsoleContext
association from the launcher's already-authenticated root generation and
the service-allocated execution Console before publishing the source record.
Source failure deletes it; root rundown releases it. Worker/root lifetime
checks use that association, not PID guesses or untrusted member lists.

mem2 returns zero and observes NTVDM exit but fails the fixture's obsolete
five-second teardown observation. S4 gives NTSRV ten seconds to retire a
workerless root. The fixture now waits pinned NTVDM/NTKVM exit handles for
at most fifteen seconds, retaining all no-live-worker/frontend/Console-window
assertions. broker-only-new-console-mem3.txt passes: launcher exit 0,
teardown wait 0, worker/frontend exit 0, no live participants or Console windows.
This is independent-Console lifecycle evidence, not the full S4 release gate.

Exact probe: MSVC x86 /MT /O2 builds
tests/observation/run16_new_console_lifecycle.c with object and executable
outputs in build/M0-T424/S4/r001. With Z: mapped to its runtime candidate:

```powershell
& build/M0-T424/S4/r001/new-console-lifecycle.exe Z:\ MEM.EXE build/M0-T424/S4/r001/broker-only-new-console-mem3.txt 0 1 1
```

Z: is removed in finally. Product and focused bootstrap fixture incremental
x86 links pass. broker-only-result-relaunch.txt passes actual DOS MEM/native
VER/DOS MEM/cooked CMD exit 19 after removal of launcher process fallbacks;
native-request and frontend-scope fixtures also pass. Unused SubmitWorkerChannel
API and old launcher creator test selection remain cleanup rows. Full runtime
matrix, fault/authentication/WOW and coherent publication remain open.
O:/winnt is unchanged; S4 is not closed and GUI/UNBOUND stay out of scope.

## Owned Console close-on-exit symmetry increment (uncommitted)

Owner requests symmetric DOS/native text close-on-exit. Original nonzero
DosSesId (`task`) worker exit and cmdmisc/PIF CloseOnExit remain in their
original owners, with no mirror change. NTSRV's shared
`service_retire_completed_root` now checks the authenticated root, other
workers, pending requests/channels and unfinished native result consumers,
pre-arms the existing Console-return latch, and sends the existing retirement
notification. Native qualifies only for its creator's self-owned Console,
never a borrowed CMD Console or an inner launcher. NTW32's resource check is
native-local; broker connection/death/shutdown and frontend I/O still use the
existing worker-base mechanisms. Launcher uses the existing broker-only
result and Console-return paths. No new cross-component edge is introduced.

Native final-completion wire advances NATIVE_REQUEST_VERSION 5 to 6 with a
validated CONSOLE_EMPTY flag. Unpublished S4 APP/RPC remains 30. Unknown flags
and old versions fail. The resource check occurs once after actual target
completion, not at fixed intervals; list growth only resizes the read buffer.
It ignores the completed target while its handle pins its identity, accepts
already-dead attachments, and conservatively refuses close for unknown/live
attachments or no Console. This never creates tasks, observed records or a
stack. NTSRV acts after the final private-channel acknowledgement, not the
earlier task-completion RPC; no helper, Job or process-tree kill is added.

Initial close1/close2 probes returned native result 37 but failed retirement.
The broker queried PID from a synchronize-only wait handle. The fix retrieves
the owning connection in the existing authoritative result lookup, removing
that extra process scan/query. `broker-owned-native-close3.txt` passes:
launcher 37, NTKVM/NTW32 exited and no live frontend/worker/Console windows.
NTW32's broker-directed GetNext shutdown exits 1223; this is not substituted
for the direct target's result 37.

`broker-owned-native-residual2.txt` establishes a native child before CMD
returns 37 and proves NTW32/NTKVM remain live. residual1 did not establish
that precondition and failed; it remains a non-pass. This resource observation
does not prove a complete descendant history or prevent future attachment
races. The enhanced real `native-console-capture-test.exe` separately proves
live attachment rejection, pinned completed-target acceptance and no-Console
conservative rejection without weakening geometry/cell/cursor/palette tests.

`broker-shared-close-regression1.txt` passes DOS MEM/EXIT -> native VER -> DOS
MEM/EXIT -> outer cooked CMD exit 19. Native execution-lifetime fixture:
586 checks, zero failures, target survival and zero remaining handles.
Native GetNext fixture and wire-negative fixture pass, including delayed final
ACK, write failure, EOF, versions, contradictory/partial replies and worker
failure receipts. Affected x86 links pass. Raw evidence remains under
build/M0-T424/S4/r001 and the recorded S2/r001 cache; Z: is removed in finally.

Full Console17/Window17/WOW and coherent publication remain open. S4 is not
closed or published; O:/winnt remains the accepted S3 package. GUI/UNBOUND,
guest/media and shared-library changes are not included.

The owned-Console candidate subsequently passes Console17 and Window17
(`t424-s4-owned-console17-summary.json`, `t424-s4-owned-window17-summary.json`):
17 cases each, expected versus actual match and original guest/interaction
assertions retained. These runs precede the final unused-bootstrap-field
cleanup, so they are regression evidence for that candidate, not the final
source/package publication gate. Removed from the production interface:
unused frontend_bootstrap_start/start_lease declarations, direct channel and
launcher-retire fields; the scope fixture now uses a private fixture start
function, not a pretend public direct-bootstrap implementation.

Cleanup x86 links and the existing scope lifetime fixture pass. With the
cleaned source's package, `broker-clean-bootstrap-owned-dos1.txt` and
`broker-clean-bootstrap-owned-native1.txt` both pass actual independent
Console teardown, no live frontend/worker and no Console-window residue.
Original DOS result is 0; actual native result is 37. Old creator files and
public SubmitWorkerChannel RPC were also deleted earlier in this candidate;
the remaining native queue primitive is NTSRV-private and not launcher RPC.
Documentation governance and diff checks pass. Final-source matrices,
fault/authentication/version/WOW checks and publication remain pending.

## Shared-path review and final-source regression increment

The owner requires reuse without new DOS/native control forks. The source
review confirms these production boundaries:

| Mechanism and provenance | Shared owner / production consumers | Deliberately independent boundary |
| --- | --- | --- |
| Project-added broker connect/death watch/disconnect and shutdown-event client | worker-base/connection.c; NTVDM and NTW32 entries | Backend heap and original worker execution remain local. |
| Project-added ordered frontend exchange, frame chunks, validation/cancellation and input encoding | worker-base/console_client.c; both workers | NTKVM owns the server and rendering; interface owns wire declarations. |
| Project-added independent-Console retirement and return acknowledgement | NTSRV service_retire_completed_root; original independent DOS exit and native final-I/O completion | DOS DosSesId/PIF completion remains original; NTW32 reports actual backend resource state, not retirement policy. |
| Project-added direct receipt wait | run16_wait_direct_event; DOS and native launch paths | Original BaseCheckForVDM and native FinishNativeRequest decode their actual broker records; WOW startup remains distinct. |

NTSRV and run16 do not link worker-base just to reuse their own policy. No
original execution algorithm is relocated, no second worker scheduler is
introduced, and no mirror/guest/library edit is part of this increment.

The cleaned-source runtime passes `t424-s4-final-console17-summary.json` and
`t424-s4-final-window17-summary.json`: 17/17 each with original expected results
and assertions retained. `final-native-wire.txt`, `final-frontend-scope.txt`
and `final-native-capture.txt` pass protocol, scope and actual Console-resource /
geometry checks. Evidence is under build/M0-T424/S4/r001; temporary Z: was
removed by each matrix's finally block. These are candidate tests, not a
publication or S4 closure.

The broad legacy in-process service fixture remains a non-pass. Its precise
diagnostic (`final-service-reservation4.txt` and Console capture) is
`later root admission error=5`: the fixture creates a suspended child and
tries RegisterFrontendRoot without the broker StartFrontend grant. The earlier
line-only diagnostic was incorrectly described as ReportConsoleMembers failure;
the rejection is before membership publication. Production RegisterFrontendRoot
requires the exact broker-created process and inherited event. Do not weaken
that authentication or remove the positive reuse assertions to make the fixture
pass. Its setup needs migration, and the outstanding full fault/auth/version/WOW
and review/publication rows remain open. Failed fixture children were cleaned
by exact captured PID/command line; no product target was killed as a test pass.

### Admission-fixture migration

The legacy fixture setup is now migrated rather than exempted. Production
StartFrontend's borrowed exact-process/event tuple preparation and clearing
are extracted as NTSRV-private `broker_frontend_admit` /
`broker_frontend_clear_admission` in the existing service owner. The declaration
is transport/frontend_admission.h, not a new RPC or public client privilege.
Trusted CreateProcess still precedes admission and ResumeThread follows it;
the owning Start call pins and releases its resources. RegisterFrontendRoot
and RegisterFrontendLease still compare the exact admitted objects.

The in-process service producer fixture uses that same boundary and retains
the actual object-type, generation, duplicate, context, completion, reuse and
failure assertions. `admitted-service-reservation1.txt` now passes the full
original Check/Update/Get/Exit lifecycle and WOW startup/reuse/failure checks.
Console-identity, frontend-root, native-backend, three nested reentry variants,
completed-worker-loss and completion-rundown-race pass. Frontend-authority and
worker-channel first exposed obsolete expectations and failed; retained `-2`
runs pass after matching the approved ten-second workerless deadline and
broker-owned pipe producer identity. The latter negative still rejects a PID
which does not own the real connected byte-pipe ends; no assertion is relaxed
to accept arbitrary pipes.

Actual broker RPC tests separately prove an uncreated process cannot register
a root. `admitted-broker-normal.txt`, `admitted-broker-startup-rejections.txt`,
`admitted-broker-native-worker-failure.txt` and
`admitted-broker-native-completed-worker-loss.txt` pass exact broker-parent
creation, forged capability/restoration denial, malformed VDM environment,
wrong bootstrap/application versions, truncated/unregistered startup, final
Console return, broker loss, failure result 1067 and retained completed target
result 37. This is not evidence from the fixture's simulated producer alone.

Affected x86 and supplemental links pass. A new final package gate with prefix
`t424-s4-admission-final` is in progress; earlier final-source matrices predate
this extraction and are retained as earlier evidence only. Publication and
S4 closure remain unclaimed.

### Nullable resume request and post-fix integration

The actual RPC fixture uncovered a production wire defect, not merely stale
fixture setup: native parent resume sends an empty payload, but SubmitNativeRequest
declared that pointer as a required MIDL reference. RPC returned
RPC_X_NULL_REF_POINTER (1780) before NTSRV could validate the request. The
unpublished protocol/RPC 30 candidate now declares the payload `unique` with
its existing byte count. NTSRV's size, operation, generation and capability
checks remain authoritative; a null pointer does not authorize a nonempty
request. MIDL was regenerated and the affected x86 and supplemental WOW
closure relinked. No delivered protocol version was reused for a changed wire.

`admitted-monitor-rpc4.txt` passes actual RPC positive and negative cases:
broker-created frontend admission, forged-root denial, stale generation and
capability rejection, application protocol/version mismatch, and native
request validation. The launcher cannot consume the NTKVM root's frontend
request queue (ACCESS_DENIED); the fixture no longer pretends they are the
same connection. Earlier failed runs are retained, not counted as passes.

With the corrected package, `t424-s4-nullable-relaunch.txt` passes DOS MEM /
native VER / DOS MEM and outer cooked-CMD result 19. The actual modern EDIT
return test `t424-s4-nullable-edit-return.txt` passes COMMAND -> CMD -> EDIT ->
CMD echo -> DOS MEM -> launcher completion. The first combined script's later
isolation setup rejected the still-live empty broker during its ten-second
grace; that setup failure is not an isolation pass. After normal broker exit,
`t424-s4-nullable-isolation2.txt` and its second-session report pass selected
worker closure without affecting the independent Console, subsequent input
and result 23. Temporary Z: is removed in finally blocks.

Fresh post-wire-fix Console17/Window17 matrices are running under prefix
`t424-s4-wire-final`. This increment is candidate evidence, not publication,
commit or S4 closure. O:/winnt remains the accepted previous package.

Those post-wire-fix matrices subsequently pass 17/17 each. Final header-input
rebuild relinks five EXEs, so that exact earlier package is preserved under
`wire-tested-recovery`, not silently equated to the new binary hashes. The
full current eight-file set is fixed in `release-candidate-manifest.json`;
its separate full regression uses prefix `t424-s4-release`. Supplemental WOW
linking is complete. Source provenance remains unchanged: no MVDM/OpenNT-host
mirror, guest or shared-library diff.

Current focused tests `wire-final-frontend-request-client-test.txt`,
`wire-final-frontend-scope-lifetime-test.txt` and
`wire-final-native-console-capture-test.txt` all exit zero with their assertion
output inspected. `wire-final-request-lifetime.txt` reports 586 checks, zero
failures, 12 completed / 16 cancelled requests, handed-off target survival and
zero remaining-handle delta. These fixtures cover broker completion failure
stopping reentry, final-I/O/resume errors, failed export/startup and cancellation;
they do not replace actual guest/worker tests.

The shared-path review distinguishes actual extraction from reuse: existing
worker-base connection and console_client implementations remain production
consumers for both workers; this S4 does not invent a second copy or move the
original DOS scheduling into them. New creation is NTSRV-local worker_spawn,
called for both kinds; native status decoding is its explicit wire boundary.
The common broker retirement helper and launcher receipt wait are the new
two-kind paths. NTW32's hidden Console attachment observation stays local
because DOS task/PSP/PIF state is not that resource; sharing it would fabricate
equivalent backend semantics. Historical fake-root RPC fixture variants are
not claimed passing after authentication changed; actual broker bootstrap,
monitor RPC and product reentry tests are the selected authenticated evidence.

## Final S4 package verification and publication

The fixed `release-candidate-manifest.json` eight-file package passes
`t424-s4-release-console17-summary.json` and
`t424-s4-release-window17-summary.json`, 17/17 each, using the unchanged
Verify-CommandExitStatus assertions and S3 case list. The four actual fault
cases in `t424-s4-release-retirement-{ntvdm,ntw32}-{worker,frontend}.txt`
return broker failure 1067, obey peer retirement and then the empty-service
deadline. This is real process/guest evidence, not just event mocks.

`release-monitor-rpc.txt` passes the actual RPC authentication and application
protocol/version negatives. Old S3 RPC29 run16 is tested against RPC30 with
the current empty-management check first proving endpoint readiness. It exits
1306 and prints `broker RPC interface incompatible` in
`release-old-rpc-rejection2.txt.console.txt`. Original S3
classify_missing_interface deliberately maps a confirmed wrong major to
ERROR_REVISION_MISMATCH; the first harness incorrectly expected raw 1717 and
failed, and its report is retained. The corrected assertion follows that
verified original client behavior, not an arbitrary accepted failure code.

Final-package `release-relaunch.txt` passes DOS MEM -> native VER -> DOS MEM
and outer cooked CMD exit 19. `release-edit-return.txt` passes actual modern
EDIT screen, Ctrl+Q, CMD echo, DOS MEM and launcher completion.
`release-isolation` verifies selected worker closure does not terminate the
independent Console and that it accepts subsequent input and returns 23.
Tests wait actual broker process exit between singleton scenarios, rather
than mistaking its admitted empty grace for a hang or inserting production
delays. Temporary Z: is removed after testing.

`t424-s4-release-wow-{winmine,sol,write}` retains the three independently
compared S3 frontiers: visible WINMINE guest main class/text
00C900A800C000D7; SOL's known memory dialog
00C400DA00B400E600B200BB00B900BB; WRITE's known not-enough-memory dialog.
This is noninteractive private-desktop observation, not gameplay or a claim
that SOL/WRITE are usable. RDP capture and foreground manual UX are not
revalidated by these probes.

Final affected MSVC x86 /MT CCPU40 targets and MIDL30 generated closure link;
supplemental WOW32 relink completes. All eight PE machine fields are x86.
`release-source-manifest.json` pins 39 changed surviving source/test/build
inputs; no production input changed during the final gate. Existing compiler
warnings remain; a zero-warning claim is not made. Mirror/guest/shared-library
diff checks, documentation governance, relative links and diff checks pass.

Publication copies only the eight current product files into O:/winnt after
preserving its hash-matching accepted S3 files and existing configuration under
`accepted-s3-recovery`, with `accepted-s3-recovery-manifest.json`. Original
COMMAND/MEM/EDIT/NTIO/NTDOS and SYSTEM.INI match the tested assets and are not
changed. `published-manifest.json` matches all eight candidate hashes. The
postpublication smoke in `published-relaunch.txt` passes DOS/native/DOS,
outer cooked CMD result 19 and normal service retirement. Publication is not
inferred from copying files alone.

S4 closes the creation/control migration, not every subsequent cleanup or the
T. The owner's newly inserted S5 checklist retains duplicate native transport,
shared primitive ownership, launcher diagnostic target-handle dependencies,
NTKVM dead state and obsolete direct-bootstrap fixture cleanup as explicit
next-stage work. Naming follows S6, GUI routing S7, frontend naming S8 and
final audit S9. No helper, descendant registry, new scheduler, original DOS/PIF
policy or launch-syntax change is introduced by S4.
