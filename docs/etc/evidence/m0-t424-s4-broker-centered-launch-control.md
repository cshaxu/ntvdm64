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

S4 concludes as bounded replanning, not a delivered production P or completed
architecture. Preserve the uncommitted candidate and recorded focused results.
S5 inherits DOS/WOW service creation, native broker-only submission/preflight/
final-status/parent-resume, removal of obsolete paths and fixtures, full
regression and coherent publication. None becomes unplanned debt. O:/winnt
remains S3; no commit/push/publication is represented by this disposition.
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
