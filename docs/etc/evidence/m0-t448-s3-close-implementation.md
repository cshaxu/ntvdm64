# T448 S3 close and loss implementation

Active implementation, not acceptance or publication. S2 candidate remains
preserved, with its remaining WRITE gate and P delivery deferred as stated
in CURRENT. Single-person implementer/reviewer. Guest and original mirrors are
unchanged so far.

## Finite approved groups

1. NTSRV alone advances management close after acceptance, independently of the
   requesting task. Root generation identifies its one terminal operation; a
   duplicate cannot reset the ten-second deadline. Distinguish frontend
   quiescence from final restoration. Late facts can complete a timed-out
   operation without reopening admission. Existing worker watches remain the
   process-exit authority, not a copied task registry.
2. Worker-base dispatches service loss to executable-owned cleanup rather than
   hard-killing the process. Keep CUI carrier cleanup, WOW original cleanup and
   native GUI target non-ownership separate at the executable owners.
3. Healthy-service local failures and original natural whole-worker exit sites
   require service disposition before carrier teardown. Preserve original task
   cleanup and the registered maximum three-host-file/seven-call mirror budget;
   no guest modification or global ExitProcess interception.
4. Verify close/completion races, each cleanup failure, late/stale acknowledgements,
   abandoned request rundown and real CAF pre-Enter input. Then full coherent
   build, regression, publication and reviewed delivery; no partial package.

## Current source work

Removed requester-stack waits and temporary worker-handle arrays from Console
close. Worker close similarly returns acceptance after its instruction is
latched. Console close is retained on the existing authenticated root and
advanced by the existing lifecycle owner. A separate FrontendCloseQuiesced RPC
prevents a duplicate preparation acknowledgement being mistaken for final
Console restoration after the worker exits. Protocol candidate advances to 49;
no candidate-48/49 images may be mixed.

Tests call the actual service owner with explicit before/at-deadline values,
duplicate requests, stale generations, premature final acknowledgement and
late worker exit. These are implementation intentions until run results are
recorded below; fixture containment is not production cleanup proof.

## Recorded runs

- Admission governance PID 34252 rejected the missing explicit S2 conclusion
  heading; corrected without changing the gate. PID 25196 passed. Deferral
  remains explicit, not functional acceptance.
- Full thirteen-component and second incremental build PID 31656 passed,
  2026-10-21 04:01:28-04:04:13 UTC. Isolated package-r1 PID 23860 matched all
  thirteen hashes; immutable original guest came from assets/winnt.zip.
- Fixture supervisors 6676 and 35640 exposed a missing new client mock and a
  test-string escape error; corrected. PID 31040 then failed before execution
  because Windows PowerShell could not load Get-FileHash; the retained test
  requires PowerShell 7 APIs. PID 29872 executed and exposed an obsolete zero-
  time process-exit assertion after asynchronous acceptance. The fixture now
  waits for actual process exit separately, retaining that required assertion.
- PID 27872, PowerShell 7, 04:08:21-04:08:44 UTC: all 33 service cases passed.
  Actual before/at deadline checks replace repeated wall-clock timeout waits;
  no production timeout was shortened.
- CAF runner PID 28168: actual isolated conhost, internal NTMON Delete/Y.
  CMD case PID 32304 passed 04:08:34-04:08:50 UTC; direct NTMON PID 2908 passed
  04:08:51-04:09:04 UTC. Both report self-ntmon-confirmed=1,
  management-close-complete=1, echo-before-enter=1 and active-echo-before-enter=1.
  COMMAND is not passed: its inherited startup failure remains recorded in S2.
  These two runs do not certify all concurrency or service-loss paths.

Subsequent source review found that actual frontend context rundown could remove
the draining root before the scanner saw it. The surviving worker watch must
retain Console-close semantics, not switch to worker-close. The scanner now
uses Console-close for lost frontend ownership, without competing with an
already issued worker-close; the timeout fixture now removes the real context.
This post-r1 change requires rebuild/retest.

## Post-r1 verification and loss-race finding

- Full thirteen-image/incremental builds r2 (PID 13760) and r3 (PID 32840)
  passed. r2 service tests exposed the stale root-loss worker-close expectation
  and a real blocked-GetNextNativeCommand gap: it recognized shutdown but not
  Console-close. Production now recognizes both distinct instructions.
- r3 first run (PID 31484) found the same old expectation in the separate
  shared-worker fixture. That fixture now verifies Console-close is signalled
  while worker-close remains unsignalled for both DOS and native watches.
  r3b supervisor PID 2164 passed all 33 cases at 04:22:35 UTC.
- Runtime supervisor PID 12976 sealed package-r3b against all thirteen r3
  hashes. The six console-client fixtures passed, including lifecycle loss,
  WOW without a frontend, callback and forced-close fallback. These fixtures
  prove dispatch/binding, not actual process-loss cleanup.
- Real native service-loss injection failed: Run16 returned 6, but the pinned
  CUI target survived the ten-second cleanup bound. Test containment killed
  that exact owned target afterward; this is explicitly not a cleanup pass.
  The run stopped before WOW and CAF, so neither is a result for this package.
- Source review found competing direct NTVWM exits in completion/input/pump
  error paths. They could terminate the carrier before the service-loss owner
  closed its hidden Console. The loss path also lacked the launch-lock fence
  used by management close, permitting teardown before suspended-target
  handoff/rollback. r4 routes confirmed loss from these exits and main through
  the same executable-owned terminal owner and takes that fence. Competing
  terminal observers cannot return to a direct process exit. r4 requires real
  reinjection proof; this finding does not prove every orphan had this cause.

The remaining healthy-service fault/whole-worker exit permission group is not
implemented. No original mirror/guest changes, formal publication or pushed
implementation commit have occurred in these runs.

## r4 runtime results and controlled environment

- Full thirteen-image and incremental r4 build PID 35076 passed; service
  supervisor PID 14756 passed all 33 cases. Six console-client fixtures passed.
- The first native loss run proved carrier/direct-target exit and Run16 6 at
  171 ms, but stage deletion failed on a still-loaded nthook64.dll in the
  test's PING child. That is not complete descendant cleanup proof. Retain the
  startup-race boundary for further verification; no product tree-kill added.
- PID 9940 explicitly waited for and pinned the test PING child before loss
  injection. Actual running CUI carrier, CMD and PING exited before containment;
  Run16 returned 6 in 165 ms. Actual internal CAF deletion of CMD and direct
  NTMON again passed the active-buffer echo-before-Enter assertions.
- The first WOW loss test incorrectly used MainWindowTitle from outside its
  private desktop. PID 23420 reused the existing Minesweeper probe's bounded
  private-desktop window/board observation, with a new ready-only mode that
  sends no game/close input. Actual WOW worker and Run16 both exited with 6;
  observed Run16 return 170 ms. No NTCON was required by this WOW path.
- DOS environment comparison PID 28652: candidate and published baseline MEM
  both returned 0 with a short PATH/TEMP. PID 31308 separated the variables:
  short PATH alone passed; short TEMP with inherited 2457-character PATH still
  failed. The same candidate then passed all seventeen existing DOS/EDIT
  cases in a controlled package+host-System32 PATH, including nested COMMAND,
  native return, direct guest exit 7, streams, EOF and EDIT return. No guest or
  product environment normalization was changed. The command fixture now
  declares this fixed test PATH instead of importing build-tool directories.
  This does not establish which PATH entry/length triggers the original
  failure, or claim long-PATH product compatibility.
- WRITE with that short environment still fails its startup frontier: now
  the original Write "Not enough memory" dialog, not MSWRITE_MENU/DOC/PAGEINFO.
  Thus the DOS gate has positive current-candidate evidence, while WRITE is
  still not passed. No waiver or T completion is implied.

### Follow-up evidence (not a stability claim)

- CAF supervisor PID 27540 reran all three variants with the explicit short
  fixture PATH. CMD (PID 8700), COMMAND (PID 29620) and direct NTMON (PID 27756)
  all passed actual internal deletion and both pre-Enter echo assertions,
  ending 2026-10-21 04:40:17 UTC. This includes the formerly blocked COMMAND
  route, not just its synthetic service equivalent.
- NTVWM carrier-order fixture passed eight production-linked I/O cases. The
  old close-priority fixture still referenced removed function names and kept
  the unused full executable entry; repaired those fixture boundaries. First
  build PID 25644 exposed unresolved unused-entry references. Rerun PID 37140
  passed five cancellation/worker-close/Console-close/loss-fence cases. The
  loss case holds the real launch lock, observes that close has not happened,
  then releases it and requires process exit 6 plus the close acknowledgement.
- Baseline WRITE supervisor PID 37364 passed all four visible startup samples
  under the same short environment. Subsequent existing read-only registration,
  file and thunk traces (PID 36312) produced successful startup for **both**
  candidate and baseline. Consequently the candidate's earlier out-of-memory
  failure cannot yet be labelled a deterministic regression, nor declared
  repaired. Traced success does not replace failed untraced evidence.
- Interrupted test stage O:/tmp/ne03 was the exact retained package-r4 copy.
  After its test clients exited, every remaining file was hash-compared to that
  immutable source, and absence of reparse points/Git/active package processes
  checked before deletion. Removed 6,490,106 bytes; source package and injection
  evidence remain. No user installation, original media or cache was deleted.
- Governance PID 3328 passed before this evidence append; final recheck remains
  necessary before delivery. No build/product source changed after build-r4;
  subsequent edits are fixtures, observation runners and records.

Remaining finite S3 work is still: healthy-service reportable-failure and
natural whole-worker exit authorization (including pre-resource-teardown and
WOW execution-lock boundaries), bootstrap-phase loss/rollback proof, and the
remaining concurrency/restoration-error matrix. Existing original ExitVDM
ALL_TASKS also occurs in last-task cleanup, so it cannot blindly be repurposed
as a whole-process permission hook. Direct production TerminateVDM reaches
terminate() before host_applClose; a late ExitProcess wrapper alone cannot
authorize that earlier resource teardown. No unproved mirror hook has been
inserted. The remaining S4 integration/publication requirements are unchanged.
## Close-scope arbitration follow-up (r5)

Source review found two remaining competing-instruction paths. A worker-close
accepted before Console-close could later receive a second Console-close from
the retirement scanner. Conversely, worker-close accepted while Console-close
awaited frontend quiescence could bypass that barrier. Replace the worker's
termination boolean with one service-owned close kind. The first accepted kind
remains authoritative through frontend context rundown; a subsequent Console
transaction waits for an already-closing worker instead of changing its reason.

The same sweep found a late authenticated worker binding in service_core.c
signalling Console-close before the frontend quiescence acknowledgement. That
binding now joins the retained transaction, with immediate delivery only after
quiescence or confirmed frontend departure. No worker-base type policy, guest
change, or additional wire protocol is introduced by this group.

Two production-linked cases cover both acceptance orders, including root
rundown. They extend the service suite from 33 to 35 cases. Build r5 and verify
r5 were owned background jobs (PIDs 31356 and 28936). Both passed: thirteen
images and second incremental build, then all 35 service cases, at
2026-10-21 04:58:25 UTC. These changes have not been published or committed.
A further production-linked case claims a real prepared DOS reservation after
Console-close acceptance and checks that neither close event is delivered
until frontend quiescence. The first r5b run failed because the fixture's mode
allowlist omitted the new entry (not a product assertion). After adding that
entry, r5c (PID 19404) passed all 36 cases at 2026-10-21 05:02:28 UTC, including
the actual late reservation claim and competing worker-close assertions.

The healthy-service original worker-exit permission boundary remains separate
unfinished work: original terminate() performs device cleanup before
host_applClose/host_terminate, so replacing only the final ExitProcess cannot
establish permission before teardown. NTVDM lifecycle observation also starts
before original host initialization; partial-initialization close safety remains
an explicit verification requirement, not an assumed property.

## Closing-worker admission follow-up (r6)

Original task records outlive the service's close acceptance until physical
worker cleanup. Native selection already consults close_kind; original
CheckVDM did not. The owned BaseSrv dispatch now rejects publication into the
selected closing DOS worker or original shared WOW head, using the same
service-owned close kind. This is not an event-state heuristic or a second
original task list. The late-binding fixture also checks rejection before
Console-close delivery. Full build r6 and service verification r6 are owned
background PIDs 31200 and 25320. Full/incremental build passed; the added test
failed and exposed DOS's pre-dispatch not-ready-to-new-worker fallback, which
had erased the selected Console before the new guard. Retain the authenticated
selected Console across that fallback and use it for the guard. This correction
does not carry a caller-supplied raw Console handle across RPC.

r6b fixture linking failed because r6's failed assertion left its suspended
child alive: the test runner recognized only quoted executable paths. Permit
either exact quoted or unquoted fixture path followed by an argument separator,
still requiring the recorded test parent's PID. The exact remaining child
13520 of failed fixture 35544 was verified and stopped; unrelated old fixture
processes were not touched. r6c (PID 28396) then passed all 36 service cases at
2026-10-21 05:09:49 UTC. Its corrected source still requires a new coherent
product build; no publication is claimed. The shared-WOW guard has source
coverage only so far, not a dedicated closing-WOW admission assertion.

Exit-boundary source sweep additionally distinguishes these actual routes:
TerminateVDM enters terminate before host_applClose; original main calls
host_applClose after host_start_cpu returns; reset.c calls cmdRebootVDM before
host_terminate; cmdPushExitInConsoleBuffer calls host_applClose and ExitVDM
before posting WM_CLOSE. These are not interchangeable final ExitProcess
sites. HUNTER keyboard termination and video dump diagnostics must be checked
against the selected build rather than added to production requirements merely
because their source exists. WOW's five fatal/whole-process sites retain their
original ExitVDM call; single-task W32DestroyTask is not whole-worker intent.
The existing WOW native-call boundary releases USER state and, unless task
locking forbids it, guest execution ownership. It is not proof that every
fatal exit can safely wait under that boundary. No speculative mirror changes
have been made for these findings.

## Owner-requested incremental delivery checkpoint (r7, S3 P1)

Owner explicitly requested publishing verified progress without waiting for all
remaining S3 work. Freeze r7 as the coherent candidate; do not treat this
checkpoint as full S3/T closure. Thirteen-image build and unchanged incremental
passed (PID 37108). Service r7 initially failed only at fixture service Stop:
the process was signalled but its asynchronous service wait had not retired.
The fixture now waits for service emptiness using its existing state event and
a ten-second budget. r7b passed all 36 cases (PID 24916), including closing
shared WOW rejection with no newly published original task record.

The earlier reset.c cmdRebootVDM source finding is not a selected production
exit: it is nested under contradictory !NTVDM / NTVDM preprocessor branches.
Do not enlarge the mirror diff for that dead branch. COMMAND's TSR exit calls
cmdPushExitInConsoleBuffer and remains a real source owner to cover later.

Delivery runtime runner PID 18144 uses the exact hash-verified package-r7.
WRITE, without diagnostic tracing, passed four startup samples with visible
MSWRITE_MENU, MSWRITE_DOC and MSWRITE_PAGEINFO. Editing/saving and normal WRITE
exit are not established by that check; the earlier intermittent failure is
retained and not called fixed. DOS17, WINMINE/SOL gameplay and real CAF internal
NTMON deletion were the remaining checkpoint runs at that observation. Their
completed results and actual publication follow. Mirror and guest diff remain zero.

### Completed runtime and publication evidence

The same delivery runner passed DOS17 (declared short package/System32 PATH),
WINMINE flag/counter/reset/open-cell and SOL deal/stock-draw, including both
games' close/Run16 return. WRITE's four untraced startup samples passed; this
does not establish editing/saving, normal WRITE exit or an intermittency fix.
The original delivery runner failed its final CAF group: an installed old
NTSRV appeared and rejected the candidate's protocol. The r7b retry likewise
refused another live installed broker before launch. Those failed records are
retained, not relabelled as passing product tests.

After owner permission to close project processes and avoid concurrent launches,
r7c (owned runner PID 7580) passed all three real CAF internal-NTMON deletion
cases: cmd PID 35804, command PID 24540, direct ntmon PID 14784. Each verifies
self-NTMON confirmation, management close completion and visible input before
Enter, including the active-buffer assertion. Run interval was
2026-10-21 05:23:12 through 05:23:57 UTC. Exact reproducer is
tests/observation/verify-console-caf-self-delete.ps1 with package-r7,
the x64 console-startup-observer and handoff-pty harness; results are retained
under build/t448-s3/delivery-r7c-caf. This proves those runs, not universal
elimination of the historically intermittent defect.

Publication PID 11392 completed at 2026-10-21 05:24:17 UTC. It backed up the
coherent old thirteen-image set, copied the tested package to assets/release
and O:/winnt/system32, and verified every SHA-256 against build-r7/result.json.
The committed release-manifest.json is the durable exact image inventory;
publication-r7.json retains the job/result and publication-r7-backup the
recoverable prior set. No guest or configuration replacement occurred.

Final scope review retains S3's healthy-service pre-teardown exit permission,
partial-initialization loss cleanup and remaining startup/fault races as finite
unfinished work. The previously reviewed normal-chain changes and close-kind
arbitration are an incremental delivery, not S3/T functional closure. S4 still
owns integrated acceptance. No additional source change followed the r7 build;
the final documentation gate and diff check cover the delivery records.

## Spontaneous exit arbitration continuation (r8)

P1 was committed and pushed as d0814cd4c; the worktree was verified clean.
The next group begins with RequestWorkerExit at the actual NTSRV lifecycle
owner. It validates the reporting connection and resolves a generation-keyed
management identity, then reuses the existing close arbiter. No PID lookup
after lock release, independent close state, forced teardown or alternate
Console-close sequence is introduced. Acceptance alone is not exit permission.
The existing service-issued instruction retains that role, including when
frontend quiescence is still pending or its transaction has timed out.

Production-linked cases worker-exit-then-console and console-then-worker-exit
reuse the existing close-order fixture. They verify authentication rejection,
idempotence, unchanged deadline, first accepted close kind, no pre-quiescence
instruction and late quiescence after timeout. Background fixture build and
service runner PID 36244 passed all 38 cases at 2026-10-21 05:30:32 UTC;
service execution took 5531 ms. Evidence is build/t448-s3/verify-r8-intent-result.json
and service-r8-intent. The r7 build prerequisite in that runner is historical;
this run rebuilds the affected fixture closure, not a new full product package.

The operation is not yet connected through RPC/worker-base or to executable
exit sites. Next integrate those callers, removing their local exit decisions,
and verify initialization-phase cleanup before a new coherent build/publication.
No end-to-end exit-permission claim follows from the owner-side fixture. The
installed package remains exactly P1. Mirror/guest changes remain zero.

### Component wiring and local permission test

Protocol 50 now carries RequestWorkerExit through authenticated RPC and the
policy-free worker-base forwarding function. NTVWM input, presentation and
completion-report faults join one request/permission path. stop_requested is
now local admission cancellation only, no longer a worker-close instruction.
The caller sends at most one request and waits on the existing service close
events or authenticated service death; it never treats an RPC error or local
wait failure as teardown permission. Other accepted close/loss observers remain
responsible if one reporting thread cannot observe the terminal instruction.

The production-main-linked native close fixture passed seven cases, including
fault acceptance with no teardown until either worker-close or Console-close
is signalled, and local stop without permission. Carrier-order's eight cases
also passed. Owned runner PID 38280 completed at 2026-10-21 05:33:47 UTC;
build/t448-s3/native-close-r8-result.json and close-priority-r8 retain evidence.
These replace only the RPC boundary and actual Console close in the fixture,
not the production permission/close logic. They are not real-RPC fault proof.

Full r8 build failed because three source/test locations still named the
generated protocol-49 interface. Corrected all four references to protocol 50;
r8b is the new owned full/incremental run (PID 6784), followed by service runner
15268. The failed run and its skipped service tests are retained. No package
has been replaced by this group. Final review must also bound the new request
RPC independently of the long-lived command-receive call; do not change the
shared binding timeout underneath concurrent GetNextNativeCommand.

### Local RPC timeout correction and communication audit

r8b and r8c full thirteen-component plus second incremental builds passed;
r8c service fixtures passed all 38 cases (runner 1144, 2026-10-21 05:43:44 UTC).
The actual suspended-service test then failed: worker-exit-deadline-r8c reached
its 25-second diagnostic limit instead of returning at ten seconds. The captured
main-thread frame at image offset 0x571f maps to Client_RequestWorkerExit's
generated RPC stub, not to local cleanup after the call. This is a failed gate.

common/rpc/local_binding.c selects ncalrpc. Microsoft's
[RpcBindingSetOption contract](https://learn.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-rpcbindingsetoption)
explicitly says RPC_C_OPT_CALL_TIMEOUT is ignored for local RPC. The previous
frontend-timeout fixture suspends NTCON, not NTSRV: its passing result proves
the live service's restoration deadline, not client escape from a stalled
service. Neither result proves a context-binding override hypothesis.

Reuse the already selected common/rpc/async_call.h completion/cancellation
mechanism used by DOS/native observation. RequestWorkerExit now has an async
ACF declaration with context_handle_noserialize, retaining authenticated
connection identity and the same service close arbiter. This removes its
copied binding and ineffective timeout option. A blocking GetNext must not
serialize the exit report. No helper thread, alternate endpoint, peer-control
pipe, retry or new transport is introduced. r8d is the owned build (25648)
and subsequent fixture/staging/deadline runner (22604), started 05:53:06 UTC;
results remain pending at this entry.

The owner additionally requires retrospective communication unification.
The finite audit includes Connect/BrokerProcess startup, Run16's bounded
final-disposition calls, worker exit requests, frontend control/acknowledgement
and management close, plus intentional long-lived command receives. The
remaining SetCallTimeout wrapper still applies the ignored binding option;
its callers are not considered corrected by the RequestWorkerExit change.
Separate per-call deadlines from unbounded task execution and command receive;
preserve cancellation storage lifetime, late-service execution, no replay and
context rundown. The existing NTCON named pipe is the worker I/O data path,
not evidence of a third control transport. Do not remove authenticated event
attachments merely because RPC conveys them: trace their actual signalling
owner and reject endpoint-to-endpoint control. No guest or mirror change.

r8d completed: full thirteen-image build and no-work second incremental pass,
38 service cases pass (6192 ms), and the real suspended-NTSRV exit RPC test
returns ERROR_TIMEOUT (1460) in exactly 10000 ms. After resuming the same
service, an unrelated ordinary RPC succeeds; the test does not replay the
exit request. Owned runner 22604 completed at 05:57:17 UTC. Exact inputs and
output are worker-exit-deadline-r8d/inputs.json and observer.txt.console.txt;
deadline-r8d-result.json records success. This validates the new operation,
not all synchronous lifecycle methods or valid-worker concurrent GetNext yet.

The I/O audit found an obsolete control surface: CONSOLE_IO_ACTIVATE,
ntcon_worker_activate and frontend_console.activate. No production caller
remains; console_channel had already installed a rejection callback, while
some fixtures still granted ownership through that obsolete dispatcher.
Remove the client operation, callback and dispatch branch. Preserve numeric
wire value 38 as a rejected hole (subsequent operation identities unchanged),
with negative tests for both values of its activation flag. Fixtures apply
the existing frontend-local grant directly only to model NTSRV authorization;
the production local activation function remains behind service coordination.
Presentation fixtures reject the retired opcode and retain data/paint/input
assertions rather than keeping unused activation counters. This cleanup is
20 added / 46 removed lines including tests (net -26), zero mirror/guest diff.
Owned focused build/test runner 38684 passed at 05:59:52 UTC: real private
Console channel lifetime, frontend dispatch, original-shape client close and
native presentation. Evidence is io-r8e/result.json and its per-case logs.
Review tightened the retired-command negative to use a valid reserved-mode
field for both input=0 and input=1, so rejection cannot be attributed only to
malformed payload. That final fixture-only refinement passed the same focused
group in io-r8f (runner 22016, 06:01:31 UTC), including 509 presentation
assertions with zero failures. This is local verification, not publication;
remaining lifecycle RPC timeout callers and NTVDM exit-permission integration
still block S3 closure.

### Remaining lifecycle RPC inventory (r8g)

| Chain / actual owner | Current mechanism and required disposition |
| --- | --- |
| Run16 bootstrap -> OpenNtBaseClientConnectCurrentTimeout -> Connect/BrokerProcess | Synchronous local RPC still uses the ineffective option. Convert through the existing async mechanism with one remaining absolute startup budget; test a stalled service and partial context/attachment cleanup. |
| Run16 submission -> native_command / launch_style | Existing authenticated RPC, no third channel. Audit each submitted operation's bound and cleanup before removing the misleading global SetCallTimeout API; no replay on ambiguous reply. |
| Native/DOS result receipt -> FinishNativeRequest / ExitCode | Receipt waits represent normal task execution and remain unbounded with service-death observation. Result RPC and final disposition need a real bounded wait, not a task lifetime limit. |
| Final Console return -> WaitFrontendConsoleRestored | r8g converts the existing exchange to async and reuses common_rpc_finish_async. The service's original handoff owner stays unchanged. Tests separately suspend NTCON or NTSRV, then assert timeout plus immediate local disconnect without another RPC. |
| Worker fault -> RequestWorkerExit | Real suspended-service timeout passed r8d. Legitimate-worker/concurrent command-receive permission evidence remains required; local permission fixtures alone do not establish that chain. |
| NTMON -> CloseManagementNode; NTCON -> state queries/acknowledgements | Existing service RPCs, not peer control. Synchronous pending-call behavior still needs the common bounded mechanism and phase-specific failure verification. |
| Worker command receive / live task | Intentional indefinite work wait, not a finalization deadline. Keep separate from bounded control transactions and retain service-loss cancellation/cleanup. |
| Worker <-> NTCON presentation/input | Existing common/console/client and common/transport/pipe_transfer provide ordered overlapped I/O, cancellation and buffer drain. Retired activation command removed; service-delivered local grant remains. No new transport or independent ownership protocol. |

r8g full build PID 37244 and serial fixture/staging/three-deadline runner
35648 started at 06:03:14 UTC. Product inputs include the I/O removal and
final-return async conversion; immutable package-r8d does not certify these
later changes. No publication or closure claim until the actual gates finish.

r8g completed at 06:07:44 UTC. The full thirteen-image and second incremental
builds pass; the service matrix passes 38 cases in 5161 ms. All three actual
deadline cases pass on package-r8g: worker exit with suspended NTSRV returns
1460 in 10015 ms and preserves the ordinary connection after resume; final
restoration with suspended NTCON returns 1460 in 10000 ms; final restoration
with suspended NTSRV returns 1460 in 10015 ms. Both finalization cases assert
that a repeated local query and disconnect finish within one second while the
faulted process is still suspended, without replay or an extra cleanup RPC.
Exact results are deadlines-r8g-result.json and deadline-r8g-{worker,frontend,service}.
These elapsed values include normal scheduler granularity around the unchanged
10000 ms deadline. Documentation governance reports success in
governance-r8g.stdout.txt. The inventory's remaining synchronous calls are
still open; this is not an all-RPC timeout claim or a new published baseline.

### Startup and shared async ownership consolidation (r8h)

Connect and BrokerProcess now use the existing async RPC interface mechanism.
The client retains one absolute startup deadline and passes only its remaining
budget to each completion wait; failed bootstrap still skips cleanup RPC and
leaves server-side context rundown authoritative. The shared async header now
owns event initialization and exactly-once completion/event release, including
the distinct synchronous-stub-exception path. DOS/native observation, worker
exit and final Console return reuse it; no parallel cancellation implementation
or background transport thread is added.

Raw-wire version/authentication fixtures retain their intentional varied inputs
through tests/broker/rpc_async_test.h, which calls the real generated stubs and
the production async completion mechanism. The raw-wire negatives are not
replaced by a simulated service. The startup fault case establishes its exact
owned service during setup, disconnects, suspends that service, and then tests
a fresh bounded connection. Late-context/attachment rundown and the separate
UNKNOWN_IF diagnostic query still require review; RpcMgmtInqIfIds is currently
synchronous and must not be mistaken for a proved bounded startup path.

Owned full build 21364 and serial fixture/staging/four-deadline runner 37884
started 06:12:33 UTC. No result or publication is asserted at this entry.

r8h completed successfully: full thirteen-component build and second increment
ended 06:15:55 UTC; service38 passed in 5090 ms; all four actual deadline cases
completed by 06:17:09 UTC, including a fresh connection to a suspended service.
This proves that tested startup branch, not the separate UNKNOWN_IF diagnostic.

### Removal of the ineffective timeout API (r8i)

Removed OpenNtBaseClientSetCallTimeout and its RPC_C_OPT_CALL_TIMEOUT call.
Run16 already establishes its bounded connection through ConnectCurrentTimeout;
it no longer reports success from an option ignored by local RPC. The deadline
fixture now uses that same bounded connection entry, rather than changing a
binding option after connecting. Existing synchronous control calls remain
explicit gaps in the inventory above, not silently certified by this removal.
The raw management observation negatives now also reuse the common async
begin/end owner, removing a second manual event/cancel/drain implementation.
Their bounded decision uses ten seconds, preserving the negative wire inputs.

Owned full build 34616 and serial verification 37188 started 06:18:12 UTC.
No new release has been published; protocol-50 work remains a local candidate.

r8i full/incremental builds passed at 06:21:23 UTC. Its worker deadline
fixture failed after correctly returning 1460 at 10000 ms: the fixture had
incorrectly switched the worker to the launcher's failure-latching connection
policy. Consequently its unrelated post-timeout RPC was refused locally.
The fixture now uses the real worker ConnectCurrent entry for that case and
the real bounded launcher entry for the finalization cases. No runtime policy
or assertion was relaxed. r8i wire runner 5232 refused its failed prerequisite.

r8j reuses the identical r8i product artifacts and rebuilds the affected
fixture. Verification PID 24136 passed service38 (5219 ms) and all four actual
deadline cases by 06:24:13 UTC. Raw management/version wire verification PID
29048 follows serially; its result is not yet asserted here. Repository source
search finds no remaining SetCallTimeout implementation or call and no retired
CONSOLE_IO_ACTIVATE / ntcon_worker_activate entry. The explanatory comment
about why the old option failed is intentionally retained.

r8j raw-wire runner failed because its temporary fixture directory contained
only NTSRV, while the real management test requests NTCON and launches Run16.
The precise observed status was ERROR_FILE_NOT_FOUND at StartFrontend, not an
RPC transport failure. No assertions or product logic changed. r8k copies the
candidate System32 inputs and uses the existing private-Console observer.
PID 24884 passed at 06:25:54 UTC: generation/capability/payload/role rejection,
inner Run16 false capability rejection, restricted Console rights and rundown,
DOS/native observation authentication, and application/protocol mismatch checks.
It does not claim a valid worker's concurrent exit-report/GetNext proof.

Same-package runtime verification PID 29896 uses package-r8j, preserving r8i
product hashes. It repeats DOS17, WINMINE/SOL gameplay, WRITE's established
startup-only frontier and all three internal CAF deletion cases. Results are
pending; the package has not been published at this point.

### S3 P2 coherent progress delivery

Runtime runner 29896 completed at 06:29:02 UTC: DOS17, WINMINE and SOL gameplay,
WRITE startup frontier (not editing/saving/normal exit), and all three internal
CAF NTMON deletion cases pass. The latter checks visible echo before Enter.
Review runner 32248 passed documentation governance, diff whitespace and zero
mirror/guest-change checks at 06:29:33 UTC. These checks complement the actual
source review; they do not certify the open S3 exit paths.

Publication runner 5496 completed at 06:30:19 UTC on 2026-10-21. All thirteen
images in assets/release and O:/winnt/system32 match build-r8i/result.json and
the tested package-r8j. Protocol is 50; application version remains 0.0.448.
The previous coherent package/manifest is retained in publication-r8-backup.
Read-only installed-process and project-DLL-holder inspection found no holders;
no user processes were terminated for this publication. publication-r8.json
records all hashes, both destinations and recovery paths.

This P2 removes an I/O control bypass and an ineffective timeout API, retains
one RPC completion/cancellation owner, and delivers the tested progress. It
does not close S3 or the remaining inventory: synchronous controls and version
diagnostics still need real bounds; a valid worker's concurrent GetNext/exit
request and late cancellation cleanup need evidence; NTVDM and NTVWM startup/
orderly-exit ownership still need completion. No guest change or mirror diff
was introduced. S4 still owns final integrated acceptance.

### Next close-control group (r9, not published)

The five remaining synchronous calls on the close/handoff control chain now
reuse the same async RPC owner: CloseManagementNode, ReturnFrontendConsole,
FrontendCloseQuiesced, FrontendConsoleRestored and RetireFrontend. The existing
service authentication and phase owners are unchanged; no task/worker may
grant its own exit, and cancellation does not replay a management command.
Protocol 51 fences the changed generated signatures from the published P2.
Bounded BaseClient callers latch timeout before another finalization/cleanup
RPC. Connectionless management retains its own binding and does not poison a
separate BaseClient context after one cancelled call.

Five additional suspended-service cases exercise these real generated calls,
the unchanged ten-second bound and local-disconnect/no-replay behavior. They
prove transport/timeout behavior, not frontend role authorization: actual CAF
and raw-wire negatives remain required. The old common-management mock now
tests issuance exceptions only for asynchronous CloseManagementNode; real
reply/authentication/management effects belong to the existing wire and CAF
tests, not fabricated async completion in a mock. No test success is claimed
until the new build and runs complete. Guest and mirror inputs remain untouched.

r9 owned full build PID 2144, serial deadline runner 19328 and dependent wire
runner 22984 started at 06:36:35 UTC. Do not change their mutable inputs while
they run. Source review additionally confirms the production NTCON still uses
ConnectCurrent (unbounded policy), and its pump calls synchronous
RetireWorkerlessFrontend before acknowledging quiescence. Therefore a bounded
fixture alone cannot certify real frontend retirement on a stalled service.
The same group must include that query and NTCON's bounded connection/failure
latch, then exercise the actual child: pause NTCON, request return through
NTSRV, pause NTSRV, resume NTCON, require local retirement and Console input
restoration without resuming the service first. This is a concrete remaining
consumer gap, not a new feature or a claim that r9 is deliverable.

r9 full/incremental product builds passed at 06:40:04 UTC. The fixture compile
then failed because its newly used real management declarations require the
generated service header include directory; no runtime tests ran. The actual
fixture build rule now declares that include input. NTCON now uses the existing
bounded connection entry, and RetireWorkerlessFrontend shares the same async
query/failure latch. The added mode 9 tests the actual NTCON process's timed
retirement and exact original input mode with NTSRV still paused. Ordinary
frontend mechanism mocks are updated to issuance-failure coverage (including
the previously stale WaitFrontendConsoleRestored signature); actual wire and
CAF tests retain positive behavior coverage. No old test failure is called a
product pass. The r9a run will include both mechanism unit targets explicitly.

r9a full/incremental builds passed at 06:47:38 UTC. The deadline runner failed
its actual-NTCON mode 9 at 06:50:06 UTC; the wire runner correctly refused its
prerequisite. Earlier individual cases produced their pass markers; the child
failure had insufficient diagnostics and is not assigned a speculative cause.

The production-call audit also proves ReturnFrontendConsole and RetireFrontend
have no production consumers. Keeping them merely to drive older tests would
preserve duplicate lifecycle controls. Remove their RPC/server/client methods
and declarations instead of converting them: normal return tests use the actual
WaitFrontendConsoleRestored barrier; forced retirement tests use authenticated
management close; busy-root tests use the production retire-decision query.
All remaining assertions concerning admission denial, live task retention and
return acknowledgement remain. Mode 9 now identifies the real NTCON through
the management snapshot and submits the same management close as NTMON before
stalling the service. It prints child wait/exit/input-mode evidence. This also
tests a real management request rather than a test-only return signal.
There is no guest or original mirror change. The protocol-51 candidate remains
unpublished; package-r8j / commit ab3d9829f stays the installed baseline.

### Actual frontend pump stall and bounded RPC sweep

r9b full/incremental and 38 service cases passed; actual NTCON mode 9 still
failed with wait=258, exit=259 (STILL_ACTIVE) after 12000 ms. Its input mode
already equalled the original 0x1f7, which does not prove retirement. The
background stack diagnostic PID 12876 completed at 07:20:18 UTC on October 21.
The immutable package-r9b NTCON PID 7612 / TID 29540 stack passed through
RPCRT4 into image offsets 0x9a64, 0x14629 and 0x199b. The matching build map
identifies FrontendRequest, OpenNtBaseClientFrontendRequest and frontend_pump.
Evidence: build/t448-s3/stack-frontend-r9/case/frontend-stacks.txt and the
matching stable-tree frontend/ntcon.exe.map. The diagnostic test failed its
retirement assertion; successful stack capture is not a product pass.

This proves a synchronous request remained after the bounded retirement
query: a preceding reply cannot prevent the pump entering a later call as
the server stalls. The finite repair covers the pump's five remaining RPCs:
FrontendJoinCandidate, FrontendJoinDecision, FrontendLeaseReady,
FrontendRequest and FrontendIoDisconnected. All reuse common async RPC
completion/cancel/drain, preserve authenticated service decisions and latch
the frontend timeout before later cleanup requests. No new communication
transport, polling protocol or peer-control fence is introduced. Confirmed
service-process death in the pump returns the already approved original
ERROR_INVALID_HANDLE rather than the transport ERROR_BROKEN_PIPE. The existing
bootstrap loss assertions likewise use 6, not the superseded 1722.

The existing native stack probe now supports both x86 and x64 context layouts;
it temporarily suspends only the explicitly selected test process's threads
and resumes them after capture. The diagnostic harness uses the existing
bootstrap trace and remains test-only. Ordinary successful replies continue
to require actual wire/CAF proof; mechanism mocks cover issuance failure and
resource cleanup, not synthetic async completion.

r9c full-component/incremental build PID 21684 and dependent serial service /
deadline runner PID 26056 started at 07:26:07 UTC. Logs are build-r9c and
deadlines-r9c stdout/stderr under build/t448-s3. Actual NTCON mode 9 must pass
with NTSRV still suspended, without the stack probe, before this group can
proceed to wire/runtime/publication. Protocol 51 remains a local candidate.

r9c full/incremental completed at 07:29:22 UTC; service38 passed in 5469 ms.
All eight actual deadline cases passed at 07:31:29 UTC, including NTCON mode 9:
wait=0, exit=1460, original/restored input=0x1f7, elapsed=10015 ms, with NTSRV
still suspended. This is real frontend retirement, not only a wrapper result.

The wire runner then exposed a first-use RPC runtime handle-count difference
in the newly built frontend mechanism fixture. Independent raw API diagnosis
(x86 PID 34940, 07:33:26 UTC) invoked only RpcAsyncInitializeHandle and
RpcRaiseException: first initialization added six handles; the next two
initializations and all three raises added zero. The x64 raw check added five
on first use and zero thereafter. Tests now initialize the RPC runtime before
the per-call baseline; they do not warm a project call or permit a nonzero
leak. Both frontend and worker mechanism suites passed exactly zero handle
delta (135 and 238 checks) on r9d. Management mechanism and actual monitor /
identity wire cases also completed before the next failure.

r9d's actual bootstrap test failed at 07:34:25 UTC: NTCON returned the required
6, but NTVWM returned 1722. Source identifies the remaining mismatch: CUI's
GetNext error reaches main's return unchanged even when close_if_service_lost
has recorded confirmed death in the shared client. Both NTVWM entrypoints now
resolve their final result through OpenNtBaseClientFailure before disconnect
destroys that client identity. This adds no transport or alternate exit owner;
ordinary errors remain unchanged unless the existing authority confirms loss.
The updated test is not relaxed back to 1722.

r9e full/incremental PID 34992, dependent deadline PID 6468 and wire/bootstrap
PID 35828 started 07:35:15 UTC with fresh logs/results. Earlier candidate gates
are retained as failed attempts. Product publication remains P2/protocol50.

r9e full/incremental and eight deadline cases passed (actual NTCON 10000 ms),
but bootstrap again returned worker 1722. This disproves treating final-result
normalization alone as the demonstrated cause of that fixture failure. A
complete source trace shows broker_worker_start_admitted returns immediately
after ResumeThread; it does not await the child's Connect. The old fixture
killed NTSRV immediately afterward, conflating pre-authentication startup
failure with loss of an established connection. Reuse the fixture's existing
native result path to complete a real `cmd /d /c exit 37` first, require its
successful result (no permitted I/O error in this non-kill case), then kill
the owned service and require both consumers to exit with 6. No arbitrary
settling delay or 1722 waiver is added. The final-result normalization remains
a source-correct application of the existing loss owner, not proof of the old
fixture's cause by itself.

Reviewer also moved failure normalization before FrontendRequest's attachment
cleanup and JoinCandidate's output reset: a concurrent confirmed service loss
must not turn a successful reply into an error after output cleanup has already
been skipped. r9f build PID 32576, dependent deadlines PID 21600 and wire PID
37756 began 07:43:01 UTC. Source and test inputs are frozen for these runs.

r9f full/incremental, service38 and all eight deadline cases passed. Bootstrap
proved actual native completion (status 0, result 37, completed 1), then failed
with worker 1726. Thus startup timing was a real fixture distinction but not
the whole remaining runtime defect. The command receive can fail before the
service process becomes signalled; main then performed local teardown and
membership_close stopped the service watcher before final error normalization.
No read of the current process signal at the final return can fix that
ownership violation reliably.

For a fully initialized/managed CUI worker, main now consumes already-issued
Console/worker close first, otherwise routes a failed command receive into
the existing request_fault_exit owner. It retains the watcher and control
connection until a service instruction or confirmed service death, exactly
as the existing input/presentation/completion fault paths do. Partial bootstrap
and GUI launchers retain their distinct boundaries; no new timer, peer wait,
event family or alternate control mechanism is introduced. Their remaining
initialization/exit-permission gaps are not declared solved. r9g full build
PID 18152, deadline PID 33096 and wire PID 27132 began at 07:51:01-02 UTC.

### r9g reviewed candidate

Full thirteen-image build and second incremental passed at 07:53:52 UTC.
Service38 and all eight real deadline cases passed at 07:55:55 UTC. Actual
NTCON retirement was 10016 ms, exit 1460, original/restored input 0x1f7,
while NTSRV remained paused. Wire/bootstrap completed at 07:56:50 UTC:
management mechanism 114 checks, frontend 135 and worker 238; both handle
delta assertions are exactly zero. Real monitor/identity negatives pass.
The established-worker case reports native result 37 then both consumers'
disconnect code 6. Workerless grace and live-worker cancellation/last-worker
loss pass with the existing real ten-second policy assertions.

Reviewer checked all production diffs against the role/authority boundary:
authenticated service methods retain their old policy and output owners;
async cancellation drains before releasing call storage; failed frontend
attachments are cleared after final error classification; management still
does not depend on the requesting task surviving. There is no new transport
or peer-control dependency. Production source delta is +122/-151, net -29;
tests/evidence increase to retain actual stalled-server and established-worker
proof. No original mirror or guest/media changes. S3 remains open for the
explicit remaining call families and original-host exit-permission integration.

Same-package runtime runner PID 18444 began at 07:53:19 UTC and waited for
wire success before running. WRITE retains only the four-sample startup
frontier; no editing/saving/normal-exit or intermittent-crash fix claim.
Review PID 23908 passed documentation/link/diff and zero mirror/guest checks
at 07:53:19 UTC; delivery records will receive a fresh final review.

Runtime r9g passed at 07:59:42 UTC: DOS17, WINMINE/SOL gameplay, WRITE startup
frontier and all three actual CAF self-delete/active-buffer echo-before-Enter
cases. Additional runner PID 25772 started 08:00:05 UTC: the shared native
result helper's original worker-failure and completed-worker-loss cases both
pass; the native stack diagnostic also compiles as x86, complementing its
actual x64 stack capture. Build r9g retains 1398 unchanged intermediate files
on the second build. Publication still requires the fresh final governance
result and coherent thirteen-image hash validation at both destinations.

### S3 P3 delivery

Final pre-publication review PID 38000 passed at 08:01:20 UTC. Publication
PID 20524 completed at 08:01:41 UTC on 2026-10-21. All thirteen package-r9g
images match build-r9g/result.json, assets/release and O:/winnt/system32.
The release manifest records every SHA-256, including unchanged images.
Recovery is build/t448-s3/publication-r9g-backup; exact publication evidence
is publication-r9g.json. No installed project process or selected style/Hook
DLL holder was found in the final preflight, so no owner process was killed.

This P delivers bounded management/frontend RPCs, removal of the two unused
retirement operations, actual frontend timeout cleanup, and the managed native
command-loop fault convergence. Production source is +122/-151 (net -29).
Tests are +183/-77 (net +106), build selection +1/-1. The extra test code is
actual process-stall/retirement proof and dual-width diagnosis, not another
communications implementation. Original mirrors and guest media remain
unchanged. S3 is not closed: startup/registration and remaining finite control
families, ordinary disconnect/diagnostics, partial-initialization cleanup and
the registered original-host exit-permission integration remain explicitly
uncompleted. S4 retains final integrated acceptance. The narrow actual CAF
cases passed; they do not prove every host/interaction variant or repair the
historical intermittent WOW/WRITE issues.

### S3 next candidate: remove client-owned reservation controls

Source audit on c9efafca0 plus the current candidate finds no production caller
of OpenNtBaseClientAcquireFrontendRoot or CancelFrontendRootReservation.
The retained StartFrontend service implementation calls acquisition at
frontend_registry.c and cancels its reservation on failure before returning.
Remove the two IDL methods, server wrappers, client/common facade, unused scope
fixture stubs and obsolete facade test cases. Keep service reservation and
concurrency tests unchanged. The live StartFrontend facade retains invalid
input, attachment cleanup, error and exact handle-delta tests.

Candidate protocol is 52; production and test source combined changes are
+8/-101 (net -93). No mirror or guest change. Full component and second
incremental runner PID 38656 started 2026-10-21 08:09:07 UTC with
build/t448-s3/build.ps1 -Round r10; stdout/stderr are build-r10.out/.err.
Results and coherent publication are pending, not a delivery or S3 closure.

r10 completed full/incremental compilation, but review caught the required
RPC interface-major change before verification. Waiting verifiers 13964,
22332, 29208 and 5268 were stopped; no tests or publication are claimed for
that candidate. Updated service.idl and every generated-interface reference
to major 52, including the real identity fixture's major/protocol assertion.
The replacement r10b run will verify this coherent ABI, preserving r10 evidence.

r10b background build PID 17196 started 08:13:02 UTC; verification chain PIDs
12932 (service/deadlines) and 19272 (wire/bootstrap) wait for explicit success
results before executing. All use fresh r10b logs below build/t448-s3.
The source review also found FrontendUsage now has only a real fixture caller,
but its pending-I/O assertion must be mapped to a production observation before
removing that surface. Retain it for this candidate rather than deleting the
assertion. RetainFrontendRoot is not obsolete: inherited Run16 frontend scope
still uses its authenticated lookup. Those conclusions prevent a name-based
bulk removal from weakening either identity checks or lifecycle verification.

r10b full thirteen-component build and unchanged 1398-intermediate second
build passed at 08:16:14 UTC. Service38 passed; all eight actual deadline
scenarios completed at 08:18:22 UTC. Governance review PID 25972 passed at
08:16:26 UTC with zero mirror/guest changes. Frontend facade now has 102
checks, exact handle delta zero; worker facade retains 238 checks, also zero.
Removed checks cover only the deleted operation, not live startup behavior.

Runtime PID 35452 and additional worker-failure PID 27736 began waiting at
08:16:06 UTC; startup/rollback PID 34224 began at 08:19:02 UTC and waits for
both to complete. Its existing frontend-scope fixture and actual adversarial
startup-rejections/startup-timeout cases specifically retain the deleted
remote facade's service-owned success/failure responsibilities. Source and
fixture inputs stay unchanged during these runs. Results remain pending.

r10b same-package runtime passed at 08:22:12 UTC: DOS17, WINMINE/SOL gameplay,
WRITE's unchanged four-sample startup frontier and all three CAF self-delete
echo-before-Enter cases. Worker-failure/completed-worker-loss passed at
08:22:22 UTC. Scope lifetime and actual startup rejection/timeout rollback
passed at 08:22:37 UTC. Reviewer checked deletion completeness, preserved
service ownership, major-version isolation and retained test assertions.
Production +5/-79 (net -74); tests +8/-27 (net -19): combined net -93.
No new transport, fallback, retry, mirror diff or guest change. Installed-image
and selected DLL-holder preflight found zero matching processes/modules.
The complete package still requires final governance and coherent publication.

### S3 P4 delivery

Final review PID 34768 passed at 08:23:31 UTC. Publication PID 14024 completed
at 08:23:59 UTC on 2026-10-21: all thirteen package-r10b image hashes match
build-r10b/result.json, assets/release and O:/winnt/system32. Recovery package
is publication-r10b-backup; publication-r10b.json records all hashes and paths.
No owner process was killed. Unchanged WOW/style images were included and
verified, not omitted from publication. Pending statements above describe
their earlier observation points and are superseded by these run results.

This P deletes two obsolete client-control operations and their dead test
facades; service policy and reservation concurrency coverage remain intact.
Runtime coverage and historical limitations remain those recorded above.
Ordinary Disconnect/UNKNOWN_IF diagnostics, live startup-registration and
remaining worker control bounds, partial initialization and original-host
exit permission remain S3 obligations. Neither this P nor the protocol bump
claims those paths are finished. Final commit includes the complete release
manifest; S4 integrated acceptance and owner T acceptance remain pending.

### Next registration/attachment candidate

On clean pushed 3907b52a0, the production frontend startup path still called
RegisterFrontendRoot, ReportConsoleMembers, RegisterFrontendLease and
AttachFrontendRequest synchronously. With an authenticated but paused NTSRV,
these could prevent the role from reaching its existing timeout cleanup.
Convert the four calls and server stubs using the existing async RPC helper,
without changing service reservation/identity/I/O policy. RPC major/protocol
53 isolates the updated call contract. Member arrays survive until cancellation
drains; failed attachment never publishes a connection generation.

The frontend main's respond label always reports startup status, so the facade
must reject that report locally after a prior terminal failure rather than
issue a second blocking call. New actual deadline modes 10/11/12/13 cover all
four operations against a suspended real NTSRV and assert startup/report/
attachment follow-ups plus disconnect finish locally within one second.
The raw monitor forged-root negative uses the same production async helper.
Other finite control families and original-host exit permission remain open.
No guest/mirror modifications. Candidate is not yet built or delivered.

r11 build PID 18544 began 2026-10-21 08:29:44 UTC. Service/deadline runner
27092 waits for that result; wire runner 22656 waits for service/deadline
success. Logs are build/deadlines/wire-r11.out/.err and fresh result files.
The twelve-case deadline suite retains the previous eight scenarios and adds
four registration/attachment cases. Member storage is freed only after the
existing helper completes cancellation; the real channel caller's failure
branch stops its activated channel and closes the unexported server handle.
Those are source-review facts, not yet actual timed-failure proof.

r11 full thirteen-image build and second incremental passed at 08:33:08 UTC;
service38 passed, elapsed 5381 ms. Governance PID 16008 passed at 08:33:02 UTC,
zero mirror/guest changes. Twelve actual deadline cases passed at 08:36:11 UTC.
The new modes 10/11/12/13 return ERROR_TIMEOUT (1460) after respectively
10015/10000/10016/10015 ms while the real authenticated service remains paused.
Each also proves the startup-result and all four control follow-ups plus
disconnect remain local, completing in under one second; attachment generation
is zero after failure. These cases use live RPC, not mocked successful returns.
They do not claim every real NTCON initialization phase has been paused in situ.

Same-package runtime PID 28880, extra worker-failure PID 35744 and startup
rollback PID 5456 began waiting on their predecessors at 08:32:41-42 UTC.
Source delta so far is production +51/-23 (net +28), tests +42/-4 (net +38).
Together with P4's removal this is a net reduction of 27 source/test lines;
the new lines implement bounded calls and their actual failure evidence,
not an additional transport/state framework. Delivery remains pending.

Adjacent source audit: run16/frontend_scope.c immediately closes both the
process/restored outputs from StartFrontend, retaining only its capability.
Its inherited-root lookup likewise immediately closes the returned process.
These are remaining redundant resource exports, not current peer waits. Audit
test observation needs separately when simplifying the pending startup RPC
group; do not preserve unused production attachments solely for fixture
convenience or delete identity/retirement assertions to remove them.

r11 wire/bootstrap/retirement completed at 08:37:13 UTC, including forged
frontend registration rejection and protocol identity checks. Frontend and
worker mechanism fixtures retain exact zero handle deltas (102/238 checks).
Runtime completed 08:40:03 UTC: DOS17, WINMINE/SOL gameplay, WRITE startup-only
frontier and all three CAF self-delete/echo-before-Enter cases. Worker failure
and completed-worker-loss passed 08:40:13 UTC; scope lifetime and real startup
rejection/timeout rollback passed 08:40:28 UTC. Preflight found no installed
project process or selected Hook/style DLL holder.

Reviewer verified all four new server methods complete every authentication
and service-error branch once; client arrays/handles remain live until RPC
completion drains. The startup-result guard observes the same terminal latch,
and failed attachment does not publish a success generation. RPC major and
application protocol both equal 53, including the real identity fixture.
No guest/mirror changes; no third control mechanism. The ordinary successful
startup-result RPC itself is still synchronous, as are the explicitly open
startup/state/Disconnect diagnostic families; this candidate fixes the four
named operations and their failure continuation, not those remaining paths.

### S3 P5 delivery

Final pre-publication review PID 31488 passed at 08:41:23 UTC. Publication
PID 25968 completed at 08:41:54 UTC on 2026-10-21. All thirteen package-r11
images match build-r11/result.json and both assets/release and O:/winnt/system32;
publication-r11.json records exact SHA-256 values and publication-r11-backup
holds the previous complete package. No installed process was killed.
Source/test changes are +93/-27 (net +66), offset by P4's -93; no mirror or
guest changes. This is the four-operation registration/attachment checkpoint,
not S3 closure or full lifecycle completion. Remaining boundaries are listed
above and in Current. No historical WRITE/intermittent-WOW fix is claimed.

### Next frontend startup/control candidate

Baseline 823ba0d90 is clean and pushed. Convert StartFrontend,
FrontendStateChanged and FrontendStartupResult to existing asynchronous RPC,
major/protocol 54. Service authentication and startup policy remain unchanged.
The executable facades reject calls after terminal failure and close outputs
after final failure normalization, including a wire-success/service-loss race.
Three new paused-service modes 14/15/16 check ten-second completion, cleared
outputs and immediate failed follow-ups. Normal task waits remain unchanged.

The common frontend fixture now tests real issuance-exception cleanup rather
than pretending to complete an async call synchronously. Success moves to the
already-required real monitor/bootstrap/NTCON tests (returned root, capability,
restoration and state-event use). The synthetic successful-null state reply
case is no longer executed: its existing null validation remains in source,
but this is not claimed as malformed-server-reply runtime coverage. Raw forged
capability/stale generation negatives use the production common RPC client,
not another wrapper or test transport. All scope/rollback/runtime gates remain
required. No guest/mirror changes; candidate not yet delivered.

r12 full/incremental build PID 38252 began 2026-10-21 08:49:06 UTC. Service
and fifteen-deadline runner 37496 waits for build success; wire runner 38512
waits for those results. Fresh logs/results use r12 below build/t448-s3.
Mutable source inputs remain frozen until these owned runs finish.

r12 full thirteen-image build and unchanged 1398-intermediate second build
passed at 08:52:23 UTC. Service38 passed (5189 ms). Governance PID 32464 passed
08:53:16 UTC, zero guest/mirror changes. All fifteen actual deadline cases
passed 08:55:58 UTC. Modes 14/15/16 returned ERROR_TIMEOUT after respectively
10016/10015/10000 ms; returned attachments were null and all latched follow-up
calls/disconnect completed locally within one second while NTSRV stayed paused.
Same-package runtime PID 25884, worker failures PID 38276 and startup rollback
PID 30852 began waiting on predecessors at 08:52:56 UTC. Delivery pending.

r12 wire/bootstrap/retirement passed at 08:56:56 UTC. Common frontend now
performs 85 issuance/cleanup checks with exact zero handle delta; worker retains
238 checks and zero delta. Actual monitor validates startup success and forged
capability/stale-generation rejection. Runtime passed 08:59:47 UTC: DOS17,
WINMINE/SOL gameplay, unchanged WRITE startup-only frontier and CAF-three
self-delete/echo-before-Enter. Worker failures passed 08:59:56 UTC; scope,
startup rejection and timeout rollback passed 09:00:11 UTC.

Reviewer checked server completion on success/invalid-parameter/authentication
failure; async output lifetime and client terminal-failure normalization precede
handoff. Production +57/-20 (net +37), tests +50/-33 (net +17). This group
adds bounded calls and real timeout assertions; it does not claim a net code
reduction. No guest/mirror change or third transport. Installed process/DLL
preflight found no matching holder. Ordinary Disconnect/UNKNOWN_IF diagnosis,
remaining worker and inherited-console binding controls, partial initialization
and original-host exit permission remain open; final S4 acceptance is unchanged.

### S3 P6 delivery

Final pre-publication review PID 21760 passed at 09:01:49 UTC. Publication
PID 25824 completed at 09:02:29 UTC on 2026-10-21. All thirteen package-r12
images match build-r12/result.json, assets/release and O:/winnt/system32.
publication-r12.json records every SHA-256; publication-r12-backup retains
the previous complete package. No installed process was killed. This P
delivers the three named frontend calls and their output/failure contracts,
not the remaining S3 boundaries. Source/test net +54, mirrors/guest unchanged.
Actual runtime frontiers and the synthetic-reply coverage limitation remain
explicit above. Final records undergo governance before commit/push.

### Ordinary disconnect candidate

Source review confirms the remaining synchronous Disconnect could block after
the caller had already obtained its task result. The candidate converts that
existing method to async RPC, retaining authenticated context validation,
serialized context rundown and service-owned cleanup. Client cancellation is
drained before releasing resources. The cleanup API remains void and never
overwrites task results. Protocol 55 requires a coherent thirteen-image delivery.
Existing real-wire positive/forged-generation cases use the same async helper;
new deadline mode 17 pauses the owned NTSRV, verifies ten-second local cleanup,
cleared connection and immediate repeated disconnect. No guest/mirror changes.
Build/runtime/delivery evidence pending; this is not S3 closure.

r13 build PID 14012 started 2026-10-21 09:11:17 UTC. Deadline/service runner
34940 and wire runner 34372 wait for their predecessors. Evidence uses fresh
r13 paths under build/t448-s3; no mutable test/build input changes while running.

Full thirteen-image and second incremental build passed 09:14:34 UTC; service38
passed 09:14:54 UTC. Governance PID 25744 passed 09:14:04 UTC. Sixteen actual
deadline cases passed 09:18:25 UTC; mode 17 reports exactly 10000 ms while the
service remains paused/alive, clears the connection and repeats cleanup within
one second. Runtime 25672, extra failure 13904 and startup/scope 7616 runners
started 09:13:42-43 UTC and remain gated by their serial predecessors.
Reviewer verified the ordinary disconnect preserves context serialization and
all authentication errors, completes every async server branch, and retains
the old void-result cleanup contract. Source +24/-15 (net +9), tests +37/-14
(net +23 before the reviewer addition below); this safety group is not claimed
as a code-size reduction.

r13 wire tests passed 09:19:25 UTC; DOS17, WINMINE/SOL gameplay, WRITE's existing
four-sample startup frontier and CAF-three self-delete/echo-before-Enter passed
09:22:16 UTC. Extra worker-failure tests passed 09:22:26 UTC; scope and startup
rejection/timeout rollback passed 09:22:40 UTC. No expanded WRITE editing/save
or intermittent WOW crash claim. Installed process/image-holder preflight was
empty. Review found the updated broker service-client negative Disconnect
generation case was not in the executed wire selection. Add the same assertion
to the executed monitor fixture (rejection retains context, then valid close
succeeds), and rerun wire/bootstrap on identical package-r13 before publication.
This adds two test lines; final test net +25, production net +9.

### S3 P7 delivery

Supplemental actual-wire runner PID 4600 passed 09:24:21 UTC on the unchanged
package-r13, including the newly executed wrong-generation Disconnect rejection
and following successful authorized close. Final review PID 31412 passed
09:23:44 UTC. Publication PID 25404 completed 09:24:45 UTC: all thirteen images
match build-r13/result.json, assets/release and O:/winnt/system32; publication-
r13.json retains SHA-256 and publication-r13-backup retains both old sets.
No installed process needed termination. The unchanged second build verified
1398 intermediates. Final documentation/diff review precedes commit and push.
No new transport, guest change or mirror diff. S3 remains open for the finite
remaining communication, partial-initialization and original-exit boundaries;
S4 integrated acceptance remains required. This P does not claim total code
reduction: source/test net +34, or that every service-loss race is tested.

### Notification-handle consolidation candidate

The four retained notification/capability getters repeat the same wire, exception and
output-ownership pattern. One typed internal helper now uses the existing
async RPC completion/cancellation mechanism for all four, preserving required
versus optional successful attachments. A single executable-facade helper
normalizes terminal failure before issuance and after completion, without
poisoning an unbounded shared worker connection on an individual timeout.
No new wire method, generic transport or worker policy is added. Protocol 56
must be delivered coherently. All four reject an unadmitted launcher.

Actual deadline modes 18-21 cover each getter, its pre-fault access contract,
null failure output and immediate latched follow-ups. Successful use is exercised
by the service-created DOS/native/WOW/CAF paths; source trace ties NTVDM's
worker_lifecycle_begin and console_client_begin/activation to the four getters.
Service fixtures retain successful/shutdown/release access-right checks.
Unit stubs retain
issuance-exception attachment closure, invalid arguments and exact zero handle
delta; they no longer pretend a synchronous stub return completes async RPC.
The former synthetic successful-null worker-event replies are not runtime-
covered in this candidate; unchanged required-handle validation remains source
review evidence, not claimed adversarial-server coverage. Other synchronous
checkpoint/transition and Take/Wait tests retain their existing assertions.
No guest or mirror change. Build, test and delivery pending.

The preliminary r14 candidate also converted WorkerStateChanged. Source-call
audit then proved that only old tests used it: production has single service-
owned submit, while that bootstrap fixture still retried ERROR_NOT_READY.
Remove the obsolete wire/client/provider, per-connection event and unused mock;
retain internal service condition-variable notification. The actual bootstrap
fixture now submits once and keeps its completion/failure/result assertions.
The historical extraction-only verify-common-worker-control.mjs requires a
pre-extraction snapshot and literal source equality; it is not a current gate
for these intentionally changed contracts. Retire it rather than rewriting it
into a duplicate behavior checker. Its historical T424 evidence remains and
the script is recoverable in Git; current tests exercise real RPC and resources.

r14 full/incremental build PID 29336 passed 09:35:26 UTC on 2026-10-21 and was
not interrupted or published. Waiting runners 21900/32988 were stopped only
after confirming no test child was running; inputs were then changed after
the build completed. Final candidate uses fresh r14b evidence and the same
valid compiler caches; no old CMake tree movement or cache clearing.

r14b build PID 35732 started 09:38:01 UTC; service/deadline runner 1324 and
wire runner 22040 wait for predecessors. Source inputs are frozen through
verification. Current production diff is +56/-155 (net -99); tests +54/-113
(net -59), including retirement of the historical 74-line extraction checker.
The actual runtime and assertions, not that line count, determine acceptance.

r14b full/incremental build passed 09:41:18 UTC; service38 passed 09:41:43 UTC.
Governance PID 24704 passed 09:40:08 UTC. Runtime runner 20536, extra failure
runner 14504 and startup/scope runner 37648 started at 09:39:47-48 UTC, waiting
for the real RPC gates. No runtime source/test input is edited during these
runs. Candidate publication remains blocked until all gates complete.

All twenty r14b deadline cases passed individually, but the additionally
selected old --reservation-parent failed at line 219, RegisterFrontendRoot,
before any new positive assertion. Source proves it tries to self-register an
arbitrary frontend; frontend_registry.c correctly requires the service-created
process and exact inherited capability. This legacy fixture is not a valid
current-admission positive oracle. No product behavior is relaxed or this
failure marked passed. Undo only its newly added selection/assertions, retaining
the failure evidence. Successful current ownership is verified through actual
service-created product runs; base_service_reservation_test and the I/O fixture
retain rights/state assertions, and monitor_rpc_test rejects forged registration.
The old fixture's broader migration is not claimed by this P.

Downstream r14b runners correctly failed their prerequisite and ran no product
cases. Rerun the corrected test selection as r14c against byte-identical
build-r14b product images: no product input changed after that full/incremental
build. Rebuild affected fixture sources; rerun service38, all twenty deadlines,
wire/bootstrap, fault/startup and runtime gates. This is not a failed product
regression waiver or a weakened timeout/output assertion.

r14c service38 passed 09:49:10 UTC; all twenty actual deadline cases passed
09:53:30 UTC. Wire runner PID 28520 passed 09:54:30 UTC, including 85 frontend
and 206 worker mechanism checks with zero handle delta, actual authenticated
monitor/identity negatives and one-shot bootstrap/workerless grace/cancel.
Runtime PID 35800, extra PID 13252 and startup PID 31160 run serially on
package-r14c. Review PID 636 passed 09:53:27 UTC. After removing the newly added
obsolete-fixture selection, final production diff is +56/-155 (net -99), tests
+37/-112 (net -75): combined net -174, excluding documentation and binaries.
No mirror/guest diff. Product images are exactly build-r14b's thirteen hashes;
publication must use that explicit build identity, not invent a build-r14c.

### S3 P8 verification and delivery

r14c runtime passed 09:57:21 UTC: DOS17, WINMINE/SOL gameplay, WRITE's four-
sample startup frontier and all three CAF self-delete/echo-before-Enter cases.
Extra worker failure/completed-worker loss passed 09:57:30 UTC; frontend scope
and startup rejection/timeout rollback passed 09:57:43 UTC. These retain the
previous frontier, not WRITE editing/save or universal intermittent-crash proof.
All thirteen package hashes match build-r14b; installed process and selected
DLL-holder preflight was empty. No user session needed termination.
Final governance/diff review precedes coherent publication, commit and push.
The obsolete reservation fixture failure above remains non-pass evidence.
Remaining UNKNOWN_IF diagnosis, worker/inherited-console controls, partial-
initialization and original-host exit permission are still S3 blockers; this
delivery does not close S3 or claim every RPC/fault path is unified.

Final review PID 38104 passed 09:59:11 UTC. An early publication invocation
PID 27192 refused the not-yet-written review result before creating a backup
or changing any image. After verifying the completed gate and no side effects,
publication PID 1036 succeeded 09:59:34 UTC: thirteen images hash-identical to
build-r14b at both assets/release and O:/winnt/system32. publication-r14c.json
retains hashes; publication-r14c-backup retains both prior coherent sets.
Post-publication documentation review, scoped commit and push follow; S3 stays
open. No input/test assertion was changed after the passing r14c verification.

### Remaining worker-control group candidate

Source trace: worker-base connection.c invokes transition/acquire, WaitFrontend,
checkpoint and release; NTVDM console_client retains the inherited execution
capability, Run16 frontend_scope binds it, and NTVWM registers its backend.
Seven previously synchronous wire calls now reuse async RPC and ten-second
cancellation. Bind/backend share one typed invocation body; facade failure
guards preserve bounded-launcher latching and shared-worker policy. Every
failed attachment output is cleared/closed, and checkpoint failure cannot
publish a release decision. Existing noserialize annotations are preserved.

WaitFrontend is an attachment wait after an admitted I/O grant, not the
long-running GetNextNativeCommand/task wait. Its server CV wait was unbounded;
it now has the existing ten-second startup deadline. A new service fixture
waits for real expiry, verifies empty outputs, then attaches and consumes the
route exactly once, proving the expired call does not steal later delivery.
Seven additional paused-service cases exercise facade timeout, empty output
and all immediate follow-ups. The actual raw-wire monitor fixture retains
successful inherited-context binding and wrong-generation/forged negatives.
Unit stubs test issuance exceptions and resource closure, not fake synchronous
success for async calls. Synthetic malformed-success checkpoint/capability
replies are not runtime-proved here; valid KEEP/RELEASE and access rules remain
covered by service fixtures and the product I/O chain.

Protocol 57 is local only until full package verification/publication. Guest
and mirrors unchanged. Use existing build/t448-s3 budget/caches and fresh r15
run evidence; no live input edits during builds/tests. No new transport,
submission retry, restored hidden-window mechanism or GUI management rights.

r15 build PID 12920, service/deadline PID 28556, wire PID 32496 and review
PID 29436 started at 10:06:49 UTC on 2026-10-21. Review passed 10:07:08 UTC.
Full thirteen-image/second incremental build passed 10:10:06 UTC, with 1398
unchanged intermediates. Service39 passed 10:10:40 UTC, including the new
ten-second attachment wait followed by successful exactly-once delivery.
Runtime PID 38284, extra PID 31768 and startup PID 8624 are gated on preceding
tests. Current source +107/-58 (net +49), tests +98/-42 (net +56): this group
adds 105 lines for explicit bounds/output cleanup and proof; with P8 the two
groups net -69. Do not trade assertions or ownership for a lower line count.
No mirror change. Paused-service and runtime results are still pending.

r15 deadline modes through 23 passed, but mode 24 failed its pre-fault role
assertion: RegisterNativeBackend deliberately returns ERROR_INVALID_STATE for
an authenticated non-worker, unlike the other six APIs' ERROR_ACCESS_DENIED.
The new fixture assumed the latter for all seven. Source worker_registry.c
proves the existing distinction; preserve it and print actual/expected status.
No product input changes. r15 downstream runners correctly terminated on the
failed prerequisite without executing runtime cases. Keep this failure visible.

r15b rebuilds only the affected client fixture and reruns all seven new modes.
Retain r15's twenty unchanged pre-existing deadline cases after checking their
individual PASS and zero-exit files; the only intervening source edit is the
mode>=22 pre-fault assertion/diagnostic. Reuse service39 and build-r15 based on
unchanged inputs and thirteen package hashes, then run all wire/runtime/fault
gates against freshly staged package-r15b. No claim that r15's whole runner
passed; the combined evidence identifies both rounds explicitly.

r15b deadline PID 25348 passed 10:19:28 UTC: seven fresh modes plus twenty
retained unchanged modes, all actual process/RPC assertions. Governance PID
23536 passed 10:18:07 UTC. Wire PID 31776 passed 10:20:28 UTC, including actual
typed Console context success/forged identity/generation/root rundown, single-
submit bootstrap and workerless grace/cancel. Mechanism tests passed 85 frontend
and 163 worker assertions with zero handle delta. These counts are narrower
than the retired synchronous-success mocks; actual service39 and runtime
provide the valid behavior evidence, not the number of unit assertions.
Final source/test net is +109 after adding the pre-fault diagnostic (+49 source,
+60 tests), or net -65 across P8 and this candidate. Runtime/fault gates remain
pending. All current product images are build-r15; no new product build was
needed for the isolated test expectation correction.

### S3 P9 delivery

Runtime PID 37504 passed 10:23:18 UTC: DOS17, WINMINE/SOL gameplay, WRITE's
existing four-sample startup frontier, and CAF-three self-delete/echo-before-
Enter. Extra worker failure/completed-worker loss PID 25024 passed 10:23:28;
scope/startup rejection/timeout PID 10992 passed 10:23:43. Review PID 31668
passed 10:21:37. No claim for WRITE editing/save, physical desktop observation
or elimination of every intermittent WOW failure.

All thirteen package-r15b images match build-r15/result.json. Installed
process and selected DLL-holder preflight was empty; no user process was
terminated. Publication PID 33988 passed 10:24:32 UTC on 2026-10-21, with all
thirteen hashes equal at assets/release and O:/winnt/system32. Manifest is in
publication-r15b.json and recoverable prior images in publication-r15b-backup.
Post-publication documentation gate and scoped main commit/push follow.

This closes the bounded seven-call implementation group, not S3. Remaining
work is UNKNOWN_IF diagnosis; production launch/result control calls and
retirement of unused historical facades; partial-initialization/fault cleanup;
and the approved original-host exit-permission sites. Existing intentional
GetNextNativeCommand/task execution waits must not be replaced with ten-second
task termination. Guest and mirrors unchanged; no new transport or permission
retreat. Quality review verified output ownership, immutable identity checks,
context serialization annotations, no replay and provider CV bounded lifetime.

### Protocol 57 complete RPC inventory

S3 P10 is a documentation/source-audit delivery on published P9 revision
8929196513a01ece0ece649ff2a76a22c37e1269. No source/test/binary change or new
runtime result is claimed. Question: does the informal remaining-control list
cover the actual interface and distinguish dead client policy from live service
algorithms? Procedure: enumerate every error_status_t method in service.idl,
join exact method names against service.acf async annotations, find Client_*
references throughout src, then trace executable consumers beyond facade
definitions. Check tests/tools separately rather than counting them as product
consumers. All counts below are source facts at this exact revision.

| Group | Exact wire methods | Owner and disposition |
| --- | --- | --- |
| Finite launch/result/context (9) | SubmitNativeRequest, FinishNativeRequest, StartVdmWorker, StartNativeWorker, CompleteWorkerChannel, NativeStartupResult, BindNativeTarget, RetainFrontendRoot, RequestFrontend | common/rpc/native_command.c and NTSRV base_rpc_client_core/launch_style; real Run16, NTVDM and NTVWM consumers. Convert as one coherent group, preserving authenticated reply outcomes and output ownership. |
| Original VDM/stream/WOW finite controls (14) | First, Check, Get, Exit, Update, ExitCode, Reenter, AttachFile, AttachPipe, RevokeStream, RegisterWowExec, WowStyle, WowStarted, WowStartup | base_rpc_client_launch_style.c; retain original behavior, typed stream/event receipts and NTSTATUS mapping. Bound transport, not guest execution. |
| Management reads (2) | TaskSnapshot, WorkerTaskTrace | common/rpc/management.c; real NTMON readers. Bound RPC and reclaim returned arrays on failure; remain read-only observations. |
| Obsolete client surfaces (8) | NativeExitCode, SelectNativeWorker, CommandWorker, AttachFrontend, Reserve, Prepare, Release, FrontendUsage | No production caller beyond their own facade bodies. Do not add new bounded adapters. Migrate valid tests before removing client/IDL/server-dispatch exposure. Service-internal implementations are a separate reachability decision. |
| Deliberate command wait (1) | GetNextNativeCommand | NTVWM resident receiver; native_commands.c waits for future commands and checks worker/Console close. Preserve long-lived receive with service-loss/management cleanup; do not impose a task lifetime. |
| Existing async (34) | Remaining IDL methods, including the eleven worker notification/I/O methods delivered by P8/P9 | Existing async designation is not whole-system lifecycle proof. Preserve prior evidence and inspect caller timeout/cleanup policy; do not count annotations as completed acceptance. |

Arithmetic: 68 total = 34 async + 34 synchronous; synchronous = 9+14+2 live
finite + 8 obsolete + 1 deliberate receive. This corrects the narrower early
estimate of eight startup wrappers: StartVdmWorker is in launch_style, not core.
No generated-interface parser/checker or extra authority file is introduced.

Important source findings and concrete consequences:

- Get is not the same as GetNextNativeCommand. base_service.c
  OpenNtBaseServiceGet dispatches original BaseSrvGetNextVDMCommand and exports
  WaitObjectForVDM; base_rpc_client_launch_style.c applies that event to the
  original caller. The original srvvdm.c sets the event field and returns.
  Therefore transport can be bounded without timing out the original wait.
- FinishNativeRequest is not a boolean success query. native_commands.c consumes
  NativeExitCode, sets target_completed, and can then return io_error. Existing
  base_service_reservation_test.c asserts ERROR_WRITE_FAULT + exit 37 + completed,
  followed by ERROR_NOT_FOUND and zero outputs on a second consumption. Preserve
  those assertions and add async-wire coverage; the old synchronous unit stub
  alone is not sufficient evidence. Transport cancellation must clear partial
  scalars, while a completed application-error reply must retain them.
- NativeExitCode's client/wire wrapper has no consumer, but its service provider
  is called by FinishNativeRequest. SelectNativeWorker's provider is called by
  StartNativeWorker. RetainCommandWorker is used by native submission and frontend
  selection. FrontendUsage's service provider is used by lifecycle/management.
  Deleting these provider algorithms merely because the client wrapper is dead
  would break the intended central owner.
- Old base_client_rpc_first_test branches still manually reserve/prepare workers,
  select them, attach frontends and retry submission. The active one-shot
  bootstrap tests and service fixtures are the current architecture. The old
  Verify-BasesrvProduct.mjs invokes --reservation-parent; it must be explicitly
  migrated/retired with its assertion mapping, not silently left uncompilable or
  treated as a passing product gate. P8's recorded self-registration failure is
  consistent with this source finding, not a reason to weaken registration.
- RpcMgmtInqIfIds is outside the IDL count. Its diagnosis is still synchronous;
  a transport-only version probe must not turn same-version endpoint drain into
  a false protocol mismatch. No solution to that outstanding boundary is claimed
  by this inventory. Likewise the native observation async helper still uses
  250ms; async conversion alone did not establish uniform approved timing.

Acceptance mapping for the fixed implementation set: all finite methods need
paused-service deadline/local-failure evidence; mutable methods need exact
identity, ambiguous reply/no replay, rundown and late completion evidence;
typed outputs need rights, clearing and leak checks; Get needs immutable guest
DOS/WOW delivery and event/stream alias coverage; Finish needs successful and
application-error outcome preservation, failed-worker result and single consume;
management readers need version/authentication negatives and returned-array
cleanup. Preserve the complete package runtime gate and input-identity rules.
The old-client test migration cannot erase a required behavior assertion.

Audit conclusion: S3 remains incomplete. The finite wire sweep above, external
diagnosis/timing checks, initialization/fault cleanup and already approved
original-host exit permission are the remaining work, followed by S4 integrated
acceptance. This is more precise than saying only a few core methods remain;
it neither expands T into new WOW features nor reduces its original objective.

### Protocol 58 launch/result candidate — not delivered

Following pushed inventory P10 f658ad721, the fixed nine-method group now has
matching async ACF, service dispatch and production client issuance. Existing
context_noserialize annotations remain. The same completion/cancellation owner
retains storage until RPC completion and supplies the ten-second bound; no new
transport, retry, guest or mirror modification. Core facades check the existing
latched failure before issuing, normalize errors before publishing attachments,
and clear/close failed outputs. StartVdmWorker publishes the original receipt
mapping only after successful normalization and output validation.

FinishNativeRequest explicitly separates common_rpc_finish_async transport
status from the service reply, preserving completed/exit-code outputs on a
real application-error reply and clearing them on transport failure. The
intentional GetNextNativeCommand receive is unchanged. These are source-review
facts, not runtime proof. Raw monitor negatives have matching async test calls;
the common_native_command_test synchronous mocks still require migration and
must not be run or counted as passing against this candidate. In particular,
its success, malformed completion and partial application-error assertions
must gain equivalent coverage, not disappear during the conversion.

Owned build r17 started at 2026-10-21 10:50:31 UTC, PowerShell PID 7836,
build/t448-s3/build.ps1 -Round r17; captured build-r17.out/.err and
build-r17/steps.json. The x86 configuration completed successfully at 10:51:14;
x64 configuration was confirmed live on the following observation. Full build,
second incremental, affected fixtures, all nine actual paused-service cases,
wire/outcome proof, complete runtime gates and coherent publication remain
unclaimed until their results exist. Do not restart the runner from this
record alone: inspect its current PID/result. Installed/released binaries remain
the P9 protocol-57 package; candidate changes are uncommitted.

The r17 build ended unsuccessfully at 10:52:11 UTC: generated protocol-58
headers rejected the remaining v57 ifspec identifier in core. The sweep also
found matching server registration/unregistration and identity-test references;
all now use v58. This was a compile failure, not a runtime result, and r17 is
retained rather than overwritten.

The existing common-native unit now compiles the unchanged production client
body with an explicitly mocked RPC completion primitive. Every previous reply,
attachment, malformed-completion and exception assertion remains, with added
ERROR_WRITE_FAULT/exit-37/completed preservation and completion-transport-error
clearing checks. This is deliberately unit evidence only, not real asynchronous
RPC proof. The native worker failure/completed-worker-loss wire cases remain
required, and a deterministic actual application-error reply still needs proof.
Nine additional paused-service modes 29--37 call each public launch/result
facade, assert cleared outputs and then verify all nine return the latched
timeout locally. No task-execution deadline or submission retry was introduced.

Owned r18 full/incremental build PID 17648 and serial verification PID 33616
started at approximately 10:54:55 UTC. Commands are build.ps1 -Round r18 and
run-r18.ps1; stdout/stderr are build-r18 and all-r18 respectively. The runner
requires full build, service matrix, existing deadline matrix, common-native
unit, nine new deadline modes, wire, runtime, worker-loss and startup gates.
First x86 compilation passed; x64 compilation was still live at the following
observation. No final result, publication, commit or acceptance is claimed.

Subsequent result: r18 full thirteen-component build and second incremental
verification passed at 10:58:12 UTC (build-r18-result.json, exit 0). Build PID
17648 ended; serial verification PID 33616 remained live. Runtime/test results
are still pending; this supersedes only the preceding build-in-progress status.

r18 service39 and all 27 retained actual deadline scenarios passed. At
11:04:43 the sequence stopped at the new client unit's final handle count:
141 checks, one failure, delta +6. Its baseline preceded the first
RpcAsyncInitializeHandle, unlike the existing common-worker/frontend units.
The corrected unit initializes only the RPC runtime before measuring; no
project call is warmed up and the exact zero-delta assertion is retained.
r18b rebuilds only that unit and passes 142 checks, zero failures, zero handle
delta. This supports per-call cleanup, not real transport completion proof.

Serial continuation run-r18b.ps1, PID 6640, validates r18 build/service/deadline
results and each of the 27 saved PASS/zero-exit case reports, then runs nine
fresh modes 29--37 against unchanged package-r18 and the unchanged actual RPC
fixture. It proceeds to wire/runtime/loss/startup checks only on success.
all-r18b.out/.err and all-r18b-result.json own the continuation result; do not
replace r18's failed unit record. Last observation confirmed the continuation
alive after the unit pass. No delivery or S closure is claimed.

At 11:08:20 UTC r18b completed all nine fresh real deadline modes; together
with r18 this is 36 actual scenarios, explicitly retained/fresh rather than a
single rerun. Frontend-control 85 and worker-control 163 unit checks also
passed with zero handle delta, and actual bootstrap reached its PASS. The
same owned continuation was still running later wire/runtime gates. The
deterministic real application-error reply requirement remains separate from
the mocked completion unit and must not be inferred from these deadline cases.

### S3 P11 launch/result verification and review

r18b completed successfully at 11:12:31 UTC. Real wire identity/authentication,
bootstrap and workerless grace/cancellation pass. The unchanged package-r18
passes DOS17, playable WINMINE/SOL, WRITE's four-sample startup frontier only,
and CAF-three internal deletion/return. Native unfinished-worker failure and
completed-worker loss, scope lifetime, startup rejection and timeout rollback
pass. WRITE editing/saving is not claimed.

The additional real reply fixture deliberately substitutes only the isolated
test package's trusted ntvwm sibling. Actual NTSRV creates/authenticates that
worker; production RPC delivers the command, validates the suspended target,
reports startup, records completion and consumes the result. Only the final
I/O status is injected, without a product test hook. The first r19 attempt
returned 87 before result verification because it omitted the normal CUI
frontend association. r19b creates real NTCON first and passes at 11:16:10:
actual status=29 (ERROR_WRITE_FAULT), result=37, completed=1; wrong request and
second consumption return ERROR_NOT_FOUND with zero outputs. Its changed
bootstrap/failure/completed-loss cases were rerun successfully. No service
authentication was relaxed. r19's failure remains retained.

Final source review: nine matching client/service/ACF signatures, preserved
context serialization exceptions, one existing completion/cancellation helper,
typed-output failure cleanup, no replay, original result consumption semantics,
and no new I/O control command. Protocol 58 fences changed wire signatures.
Tests distinguish mocked client completion from the actual service result case.
No guest, original mirror or configuration changes. Fixed remaining scope is
16 live finite methods, eight obsolete client surfaces, diagnosis/timing,
partial-initialization/fault cleanup and original-host exit permission; the
intentional native command receive remains unbounded by task duration.
S3 is not closed. Coherent publication, governance and commit/push are pending.

Delivery update: governance/review-r19b passes at 11:18:16 UTC. Installed
project processes and relevant Hook/style DLL holders both counted zero;
no owner session was terminated. publication-r18b passes at 11:18:49 UTC:
all thirteen package-r18 hashes equal both assets/release and O:/winnt/system32.
The previous coherent set is retained in publication-r18b-backup. Unchanged
images were verified too, not omitted because Git has no binary diff for them.
Source net +78 and test net +167 (combined +245); no reduction claim for this
checkpoint. No mirror/guest delta. Commit/push follows the final record gate;
S3 and T448 remain active with the fixed remaining groups above.

### Obsolete-client test migration in progress

P11 is pushed as 877e22543d99208b37a6b64c358fcd148453f980. Its published
protocol-58 package remains unchanged. The next cleanup first migrates tests
which still self-register a frontend, reserve/prepare workers from a launcher,
or retry failed native submission. No eight-method wire deletion is claimed yet.
The scope-lifetime unit's unused reserve/release/usage mocks are removed and
its native-start mock implements the current API directly.

The native execution fixture now uses actual NTSRV-created NTCON, one request
submission, authenticated execution-context plus frontend retention, and the
real service Console-return operation. It no longer impersonates the frontend
to attach a pipe. The removed local inactive-barrier assertion maps to
tests/observation/ntvwm_presentation_test.c run_case(0/6): ERROR_NOT_READY on
the first barrier, continued frame publication and a successful later barrier,
with operation 38 (implicit activation) explicitly forbidden. This mapping
still needs a fresh affected-fixture run; it is not new passing evidence.

Owned background migration-r20 through r20f retain all results under
build/t448-s3 (PIDs 21900, 19368, 36312, 38472, 25204, 2164). All ended.
The fixture compiled; r20/r20b exposed missing frontend retention in its old
descendant setup. r20c/d/e then returned 5 from nested public Run16. Source
tracing shows the x86 test's System32 spelling is redirected for its direct
request, but names Win64 cmd.exe when interpreted by x64 Run16. The fixture
now pins the same Win32 image using the actual WOW64 directory. r20f proves
exact 37/19 results, alias/EOF, failed-target reuse, descendant 73 and public
Run16 61 through the same worker, rejected forged context and service return.

r20f's second case, concurrent public first startup, fails: child 0 returns
183 instead of 61, with no submission retry. Do not weaken the assertion or
count the pair as passing. The service selection/create-reservation boundary
must be traced before retiring this test's old APIs. Separately, source shows
StartNativeWorker reuses a CUI worker without comparing requested bitness,
while BindNativeTarget compares actual type to that worker's creation type.
The observed mixed-width result and this source disagreement require an exact
request/worker ownership decision within the existing classification contract;
removing the suspended-target validation is not a solution. Neither finding
is evidence that a third transport is needed. Both remain explicit migration
blockers, alongside the unconverted reservation/root-role test families.

Only tests and this evidence changed in this migration checkpoint. No product
build input, guest, mirror, published image or wire version changed; no new
publication or S closure is claimed. Next work retains the existing finite
eight-API retirement scope and its assertion migration, rather than adding new
features or erasing the newly exposed failures.

### Native admission corrections — candidate, not published

The next source review confirms that SelectNativeWorker and reservation create
were separate decisions: a competing first start could return 183 even though
the reservation belonged to the same execution Console. StartNativeWorker now
holds the service lock across that decision and waits on the existing condition
for competing startup, with the existing ten-second limit. It neither replays
SubmitNativeRequest nor introduces a wire method. Worker registration and
reservation release already wake that condition.

BindNativeTarget formerly compared every target against the carrier's initial
reservation type. Launcher admission now supplies native_request_type, copied
into each direct record as native_type under the service lock. Binding compares
the actual target with that record, retaining the CUI/GUI role check. The
reservation-exact GUI selection remains unchanged. No wire, guest or original
mirror change. Source review establishes the intended ownership; tests still
must prove it.

The native reuse fixture deliberately exercises Win32 exit 73 followed by x64
Run16's Win64 System32 target exit 61 on the same carrier. The service fixture
also supplies a real suspended wrong-width cmd.exe to a Win64 request, expects
ERROR_ACCESS_DENIED with unsignalled receipt, terminates only that owned test
process, then binds the correct target. This is not permission to run or alter
any guest image.

Owned full/incremental build r21 started at 2026-10-21 11:35:46 UTC, controller
PID 17608. Serial verification controller PID 37832 waits for that exact result
before compiling fixtures or testing. Scripts are build.ps1 -Round r21 and
run-r21.ps1; stdout/stderr and eventual JSON results stay under build/t448-s3.
The last observation had completed x86 and entered x64 first build. No final
build/test success, publication, commit or S closure is claimed at this point.

r21 full thirteen-image build and unchanged-intermediate second build pass at
11:38:41 UTC. Serial r21 verification then failed the native-command fixture
at line 1572: two reconnect branches still used selection without the current
typed startup admission. They now call StartNativeWorker, retaining the same
queue/disconnect assertions. r21b failed management-gui because its explicitly
synthetic admitted record did not seed the new expected type. The fixture now
seeds that field alongside its existing GUI admission seed; no production
check was relaxed. Both failed attempts and logs remain retained.

Continuation r21c (controller PID 29788, run-r21.ps1 -Round r21c) reuses the
unchanged r21 product build and recompiles the affected test source. Service39
passes at 11:42:03 UTC, including real wrong-width target rejection followed by
correct binding. Actual native execution/reuse and simultaneous public startup
both pass at 11:42:18 UTC. Win32 73 and Win64 61 use the same worker; concurrent
startup preserves distinct 61/62 results. Scope lifetime assertions pass too.
The controller next entered its 36 real deadline cases and remaining wire,
runtime, error-result and startup gates. Those later outcomes remain pending;
do not infer publication or complete acceptance from these focused passes.

Candidate input identities: migration-r21c's base-client-rpc-first-test.exe
SHA-256 501b84246f1b02083a106fc86c271ccabced104314eeb4435fa337c2d5762651;
service fixture 80514cfb7b6beb816215b7d6e27e8eaa35bf3a61265e8bcff5564bdb2b58f06e;
package-r21c ntsrv.exe fa9f7264a30b42a2845afeec36a6fa711751523c292ad67d54d43b36d62464e0.
Full-build evidence verifies 1,398 unchanged intermediates on its second pass.

Post-run controller PID 25524 (post-r21c.ps1) is explicitly waiting for the
owned serial verifier PID 29788 to finish and requires its success result. It
then builds/runs the native completion unit and original presentation fixture
on an isolated desktop, followed by governance. The presentation run retains
the inactive-barrier continuation assertions formerly misplaced in a launcher
fixture. It must not overlap the real session matrix. At 11:47:59 UTC the serial
verifier had entered 27 deadline scenarios; no final outcome was recorded.

Source review of the candidate: no new RPC signature or transport, no guest or
mirror change. Existing worker role and exact GUI reservation remain distinct
from immutable per-request expected type; no dynamic cleanup resource was
added. Source net +25, tests net +13 at this checkpoint (combined +38), so this
is a correctness repair, not yet the planned eight-interface reduction.

At 11:50:01 UTC all 36 actual deadline scenarios pass; wire identity, role,
bootstrap and workerless grace/cancellation pass at 11:50:55. Frontend/worker
RPC units retain 85/163 checks with zero failures and zero handle delta.
Product runtime and post-run presentation/review remain in progress.

Read-only quality review finds one now-unused state projection: after binding
moved to per-request native_type, OPENNT_BASE_CONNECTION.native_launch_type
is assigned only by service_core and seeded by the GUI observation fixture,
with no production reader. The similarly named worker-watch field is still
read by reservation-exact selection and must remain. Delete only the unused
connection field/assignment and seed the fixture's request directly after the
current immutable-package run finishes, then rebuild/verify final source.
Do not publish the pre-cleanup candidate as if it includes that review fix.

r21c serial verification completes successfully at 11:54:12 UTC. Runtime
passes DOS17, playable WINMINE/SOL, WRITE's unchanged four-sample startup-only
frontier, and CAF-three internal deletion with active-buffer echo before Enter.
Worker failure/completed-worker loss, startup rejection/timeout rollback and
actual ERROR_WRITE_FAULT plus exit 37/completed/single-consumption pass.
Post-run verification ends at 11:54:40: native completion unit passes, and the
presentation fixture reports 509 checks, zero failures, including inactive
barrier continuation. Governance passes. Both controllers have ended.

The quality-review fix then removes the unused connection native_launch_type
field and service_core assignment; the GUI observation fixture directly seeds
its request's native_type. The worker-watch field and its exact-reservation
comparison remain. Final-source full/incremental build r22 starts under PID
38252, with serial verification PID 1924 waiting for that build's result before
running. Same scripts, fresh r22 evidence/package paths. Earlier r21c evidence
is retained but is not substituted for final-source publication gates. No new
release or commit yet. Candidate source/test combined net is now +35 rather
than +38; the larger eight-interface reduction remains unfinished.

Final-source r22 full/incremental build passes at 11:58:36 UTC. Service39
passes at 11:59:08 and both actual native migration scenarios pass at 11:59:23.
Serial controller PID 1924 then continues the deadline/wire/runtime sequence.
Post-run controller PID 21988 waits for that exact process and result before
completion/presentation units and governance. No input source is changed while
these run. The obsolete connection field now has no remaining reference;
the worker-watch type still has its exact-reservation consumer.

Delivery controller PID 27248 (deliver-r22.ps1) waits for post-run completion,
requires all/post/startup/migration/service success, checks the changed source
file set and SHA-256 values against its frozen inputs, and rejects known
installed Hook/style/VDM DLL holders before calling the existing complete
publication transaction. That transaction separately rejects installed project
processes, verifies all thirteen candidate hashes, backs up both destinations,
and verifies replacement or rollback. This is a scheduled owned foreground-free
delivery, not evidence that publication already occurred. Any failure stops it;
commit/push still require the final evidence/status review.

## P12 final-source delivery

r22 completes all 36 real deadline scenarios at 12:07:06 UTC, wire gates at
12:08:00, and same-package runtime at 12:10:49 on 2026-10-21. Runtime retains
DOS17, playable WINMINE/SOL, WRITE four-sample startup only (editing/saving
unverified), and CAF-three internal deletion with pre-Enter active-buffer echo.
The full serial suite finishes at 12:11:11; post verification at 12:11:39.
Native completion reports 142 checks, zero failures and zero handle delta;
presentation reports 509 checks and zero failures. Service39, both actual native
migration cases, wrong-width rejection, startup rollback, real error result and
single consumption all pass. Earlier failed migration attempts remain above.

Publication controller 27248 completes at 12:11:53 UTC: all thirteen package
images match both assets/release and O:/winnt/system32. Recoverable prior sets
are in build/t448-s3/publication-r22-backup. Final NTSRV SHA-256 is
d4d4f67bf1e1d660043117b26dab436103a9f1884b462d1d5d41b6ad1a16118d;
the committed release-manifest.json records every image. Protocol remains 58.

Review: selection/reservation is serialized at the existing service owner;
the condition wait releases that lock and has the existing ten-second bound.
No client submission retry or new IPC is introduced. Target type belongs to
each admitted request; exact GUI reservation, worker role and actual suspended
target validation remain enforced. The unused connection-type projection is
removed. Source net +23 and tests net +12, combined +35: this correctness repair
does not claim the unfinished eight-interface reduction. Zero guest/media or
original mirror changes; no new mirror exception required. S3 remains open for
the already fixed RPC retirement, finite-call and exit-permission groups.

## Post-P12 obsolete-test migration

P12 is pushed at 8fd14dc63; the worktree was clean before this continuation.
The old frontend_root_parent fixture registered the launcher itself as a
frontend, which no longer represents the production role contract. It now
starts actual NTCON through StartFrontend, rejects launcher registration,
checks non-signallable capability and non-terminating process rights, and
delivers that capability to a real descendant. An independent event remains
rejected. The service management snapshot supplies the exact frontend key;
CloseManagementNode closes that frontend, then a second descendant verifies
the old capability is rejected. No direct task-to-frontend control is added.

Background migration-r23, controller PID 38668, ran from 12:15:27 through
12:15:36 UTC on 2026-10-21 against unchanged package-r22. It rebuilt the actual
client fixture and passed frontend-root-parent on an isolated desktop with
real RPC and service-created NTCON. Source is tests-only so far; no product
publication change, guest/mirror change or interface retirement is claimed.

Remaining old reservation tests cannot simply be deleted: they include original
GetNext command/re-entry, repeated local wait-handle identity, receipt exit 7,
frontend rights, delayed delivery and cancellation. The current service tests
already cover original re-entry and worker death, but those passes alone do
not prove the old real-RPC wait-handle identity assertions. Preserve that wire
boundary in a service-admitted worker fixture before retiring Reserve/Prepare/
Release. Frontend delivery/cancellation belongs to actual frontend/worker
fixtures, not a launcher pretending to own a frontend. The independent native
reservation create/duplicate/release/stale checks belong to the service-owned
reservation tests once the client creation surface disappears. Eight-interface
retirement remains one unfinished group, not a completed deletion.

The replacement admitted-vdm fixture now enters through real StartFrontend,
original CheckVDM and StartVdmWorker. A fresh build-owned package substitutes
only its test executable as ntvdm.exe; this is a host wire fixture, never a
guest execution claim or a released product. The child checks original command
delivery, re-entry increment/decrement and three GetNext replies preserving the
same live local wait handle. The parent checks a wait-only worker handle,
original BaseCheckForVDM exit 7 and successful completion of all child assertions.
Its explicit test release event coordinates assertions only, not product IPC.

r24 (PID 22748, 12:18:54–12:19:03 UTC) passes that original-command route.
r24b (PID 28272, 12:19:54–12:20:14) adds real frontend delivery rights but fails
the parent-completion wait: the synthetic worker omitted WORKER_IO_ACQUIRE
before WaitFrontend. The actual worker-base/connection.c:worker_base_io_open
establishes this prerequisite. Adding the same existing RPC transition, without
changing any production checks, produces passing r24c (PID 14732,
12:20:52–12:21:02 UTC). The child also verifies ready/capability handles cannot
be signalled, the frontend cannot be terminated through the borrowed handle,
the retained root identity matches, and duplicate TakeFrontend/WaitFrontend
return ERROR_ALREADY_EXISTS with cleared outputs. Fixture SHA-256:
f7dec5cf161b370707f644137201632c7ceb7f5109de14b549e428c9b1f053cc.

Remaining retirement mapping: service fixture frontend-wait covers deferred
attachment and handle delivery; frontend-wait-root-loss/request-loss/worker-loss,
frontend-wait-timeout and frontend-rundown cover distinct cancellation and EOF
contracts. These provider tests must remain alongside actual wire tests, not
be described as real RPC cancellation evidence. The old launcher's root-loss
setup conflicts with the new roles and cannot simply be retained unchanged.
The old native reservation test also needs its create/release/reuse/stale
assertions retained at the service owner. No obsolete wire entry is removed yet.

r25 (PID 25436, 12:22:19–12:22:36 UTC) runs all four migrated cases serially
against unchanged package-r22 and passes: ntvwm-execution,
ntvwm-public-startup, frontend-root-parent and admitted-vdm. The service's
native-worker fixture now retains null-output rejection, reservation release,
new unique reservation identity and rejection of the released old identity
before continuing its existing authenticated worker binding. Full service
verification r26, controller PID 17608, is running against the P12 product
build prerequisite; its outcome is not yet known. These are test migrations,
not newly delivered product changes or completed interface removal.

r26 finishes successfully at 12:24:15 UTC (start 12:23:43): all 39 service
cases pass, including the moved reservation lifecycle assertions and the
existing frontend cancellation/rundown family. This supplies the provider
coverage mapping alongside r25's real wire evidence. The next implementation
step is the bounded removal of the eight obsolete remote surfaces and their
obsolete test-role scaffolding while preserving those internal providers.

## Protocol 59 retirement candidate

The eight unused remote methods NativeExitCode, SelectNativeWorker,
CommandWorker, AttachFrontend, Reserve, Prepare, Release and FrontendUsage
are removed from IDL, server RPC entrypoints and client/common facades.
Live service-internal providers remain: Finish consumes NativeExitCode;
StartNativeWorker uses selection/reservation; command routing uses retained
worker identity; lifecycle uses frontend usage; service startup owns prepare
and rollback. Protocol major/version metadata advance together from 58 to 59.
The remaining 60 methods comprise 43 async controls, 16 finite synchronous
controls and one intentional long receive; finite-call conversion is still open.

Removed old fixture scaffolding no longer reserves/spawns its own worker or
impersonates NTCON. Its command/re-entry/wait identity/exit 7 and rights/duplicate
delivery assertions are retained by r25 admitted-vdm; root capability/descendant/
stale assertions by r25 frontend-root-parent; reservation release/reuse/stale
assertions by r26 native-worker; cancellation/provider ordering by the r26
frontend-wait family. Native results use the already exercised Finish path.
The retired FrontendUsage facade's mock-only marshalling checks are removed,
not converted into another public usage query. Its internal state algorithm
and service assertions remain. The raw broker negative test uses the retained
authenticated context-acquisition method for invalid generation/no capability.

Two T412 tooling entrypoints were already incompatible with current source
paths, bitness and roles. They now explicitly fail with current test entrypoints
rather than generating obsolete patched launchers or returning false acceptance.
Files remain for indexed historical links; no historical run is relabelled.

Full thirteen-image and second-incremental build r27 starts at 12:28:05 UTC,
controller PID 13064. Sources remain frozen during the build. Initial review
also identifies stale adjacent IDL comments for removed CommandWorker/Prepare;
remove those before final candidate verification. No protocol-59 package has
been published. P12/protocol58 remains the coherent installed baseline.

r27 fails at 12:29:38 UTC during x86 client compilation: the explicit generated
RPC interface descriptor still named v58. The build did not produce a release.
All four descriptor references in client/server/identity-test are changed to
v59 and both obsolete adjacent IDL comments are removed. A repository search
finds no remaining removed Client_/Server_ surface reference or v58 descriptor
in src/tests/tools. Internal service providers still have live callers.

Final-candidate build r28 starts under PID 31244; serial verification PID 21932
starts at 12:31:38 UTC and waits for its build result. It includes service39,
all four migrated wire cases, the 36 actual deadline scenarios and prior full
wire/runtime/error/startup gates. Current diff versus P12: src -270 lines,
tests -193, tools -171 (combined -634, excluding documentation). No guest or
original mirror change. This is candidate reduction, not publication or S3
completion. Remaining finite-call and original-host exit authorization work
stay in the fixed S3 scope after this coherent progress delivery.

r28 full thirteen-image and second-incremental build passes at 12:34:41 UTC.
Service39 passes at 12:35:14 and all four migrated actual-RPC cases pass at
12:35:33. Serial controller 21932 continues real deadlines and runtime;
post controller 18176 waits for that exact process before the completion/
presentation/governance gates. Delivery controller 22176 freezes the changed
source/test set and hashes, waits for post success, requires all gates, checks
installed image/DLL holders and then invokes the existing recoverable full
thirteen-image publication. These scheduled operations do not establish that
publication has occurred. Commit/push require final result and diff review.

All 36 r28 deadline cases pass at 12:43:20 UTC; wire gates pass at 12:44:19.
Frontend RPC unit now has 75 checks (the retired facade's cases are gone),
worker RPC 163, both zero failures/handle delta. Runtime completes at 12:47:08:
DOS17, playable WINMINE/SOL, WRITE startup-only frontier and CAF-three pre-Enter
echo pass. Full serial verification finishes at 12:47:31. Post-run native
completion and presentation509 pass, and document governance passes, but
git diff --check rejects one added blank line at the end of
base_rpc_client_core.c. Post and delivery therefore fail at 12:48:02 before
any publication; retain those failed results rather than overwriting them.

After both controllers end, remove only that trailing blank line. Build r29,
controller PID 23832, starts at 12:48:49 UTC. Compare its complete image hashes
with the tested r28 package before deciding evidence reuse; unequal product
identity requires new runtime evidence. No statement that the whitespace-only
source change automatically makes untested binaries acceptable is authorized.

r29 full/incremental build passes at 12:51:46 UTC; independent governance
review-r29 passes at 12:50:00. Seven image hashes equal r28; ntvdm, nthook32,
ntvwm, ntcon, run16 and ntsrv differ. Read-only byte comparison finds equal
lengths and four changed bytes each (including the PE timestamp); no assumption
of identical tested package is made. Run the entire matrix again on r29.
Serial controller PID 13788 owns that run; post controller PID 33328 waits for
it. Delivery PID 15560 waits for post success and the same complete set of
gates, source/test hash freeze, installed-holder checks and backup/hash-verified
thirteen-image transaction. No successful publication or new commit yet.

## P13 verified retirement delivery

The final r29 package passes service39 (12:54:00 UTC), all four migrated
actual-RPC cases (12:54:21), all 36 actual deadline cases (13:02:07), wire
negatives and RPC units (13:03:01), and the complete runtime group (13:05:50).
The runtime frontier is DOS17, playable WINMINE/SOL, WRITE startup-only and
three internal CAF self-delete routes with visible echo before Enter. WRITE
editing/save/normal exit is not proved. Serial controller 13788 completes at
13:06:13; post controller 33328 completes at 13:06:41 after native-RPC142,
isolated presentation509 and governance/diff checks. All checks pass; RPC unit
handle deltas are zero. Presentation explicitly reports production-activation=no;
the actual frontend activation/return evidence comes from the runtime gates.

Delivery controller 15560 verifies the frozen source/test hashes and installed
holder preflight, then publishes all thirteen images at 13:06:55 UTC on
2026-10-21. The prior coherent package is recoverable under
build/t448-s3/publication-r29-backup. A subsequent independent hash comparison
confirms every image in tested package-r29/system32, assets/release and
O:/winnt/system32 equals release-manifest.json. Protocol is 59; guest media,
assets/winnt.zip and both original mirrors are unchanged. Reviewed net changes:
src -271, tests -193, tools -171; combined -635 excluding documentation/binaries.

Review confirms the direct Console pipe carries I/O, not activation/lifecycle
instructions; retired wire value 38 remains reserved. NTSRV retains the live
internal reservation, selection, result and lifecycle providers. One internal
AttachFrontend wrapper now has only provider-test callers; migrate those to
the real request entrypoint before removing that wrapper in the fixed cleanup
group. The changed legacy tests/broker/service_client.c is not executed by r29;
the current identity/wire fixtures supply this delivery's actual RPC evidence.
No test result is attributed to either retired T412 runner.

S3 remains open for the sixteen finite synchronous methods, UNKNOWN_IF
diagnosis and observation timeout consistency, partial-initialization/fault
cleanup and original-host exit permission. No new transport, semantic waiver,
guest change or S3/T acceptance is introduced by P13.

## Remaining finite-control group

P13 is pushed at d8051c3ab and the worktree is clean before this group.
Convert all sixteen remaining finite methods together: First, Check, Get,
Exit, Update, ExitCode, Reenter, AttachFile, AttachPipe, RevokeStream,
RegisterWowExec, WowStyle, WowStarted, WowStartup, TaskSnapshot and
WorkerTaskTrace. Reuse the existing async RPC completion/cancellation owner
and ten-second policy; preserve the original application reply separately
from transport failure. Get returns the original wait attachment rather than
waiting for task completion. GetNextNativeCommand remains the one intentional
long receive. Keep all argument/output storage alive through cancellation drain,
close unconsumed typed attachments on failure and retain stream aliasing,
receipt and repeated worker-wait identity. No guest/mirror changes are planned.
Update raw-wire tests and mocks with the real signature, then build the whole
package and verify actual stalled-service return, original DOS/WOW behavior,
negative authentication and resource cleanup before publication. UNKNOWN_IF,
partial initialization and original-host exit permission remain separate fixed
obligations, not covered by this conversion alone.

Candidate protocol60 implements that sixteen-method conversion. One server
completion helper covers normal and early authentication/validation returns;
AttachPipe delegates completion exactly once to AttachFile. The launch facade
retains arguments through the existing cancellation drain, consolidates distinct
stream rollback and releases unconsumed typed handle arrays even when timeout
wins a concurrent reply. Management readers use the same async mechanism;
native creation observation now uses the shared ten-second constant rather than
250ms. No private transport or alternative completion mechanism is introduced.

Full/incremental build r30, controller 35448, passes from 13:14:22 to
13:17:39 UTC on 2026-10-21: thirteen images and 1,398 unchanged intermediates.
After that controller ends, add deadline cases 38-54 to the actual RPC fixture.
They exercise original command callers where local preconditions are available;
ExitCode, WowStartup and RevokeStream instead exercise their real wire contracts
without fabricating a receipt in the production client's private mapping.
Management readers and native observation are also tested separately from the
launcher's latched failure state. These are distinct evidence scopes, not claims
that a raw-wire case proves every public facade's local prerequisite handling.
Serial verification controller 26820 uses package-r30, rebuilds the changed test
fixture and runs these seventeen cases before the thirty-six established ones
and full wire/runtime gates. Results remain pending; installed/released binaries
remain P13/protocol59 until the complete new candidate passes publication gates.

r30 verification stops at 13:19:52 before running cases: the expanded fixture
omitted base_rpc_client_launch_style.h for WowStyle. The product build remains
valid; add that existing declaration and rerun as r30b without changing product
sources or rewriting the failed r30 result.

r30b controller 12932 passes rebuilt fixtures and service39 at 13:21:16 UTC,
then all four migrated actual-RPC cases at 13:21:37. It is running the new
deadline cases before the established matrix. Post controller 33972 waits for
that exact process before native completion/presentation/governance checks;
neither another serial test nor publication is started in parallel. Source and
test inputs remain frozen until these controllers finish. No protocol60 runtime
completion or publication is yet claimed.

During the frozen r30b run, source enumeration confirms 60 IDL methods and
59 ACF async declarations; GetNextNativeCommand alone remains the intentional
long receive. Cases 38-54 all pass actual paused-service deadlines by 13:25 UTC;
the earlier thirty-six cases continue. Delivery controller 33560 waits for post
controller 33972 and every existing gate, pins changed source/test hashes,
checks installed holders and uses the existing complete-package recovery
transaction. Scheduling that controller is not a publication result.

Read-only next-group review confirms host_applClose closes the original event,
sound, heartbeat, video and device resources before host_terminate calls
ExitProcess. base/support/terminat.c also performs base cleanup before those
calls. The final exit substitution alone therefore cannot prove pre-teardown
permission; preserve this ordering obligation when selecting the earliest
owned/source seam. No exit-permission hook is implemented in this candidate.

For the separate UNKNOWN_IF diagnosis, Microsoft documents that
[RpcCancelThreadEx](https://learn.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-rpccancelthreadex)
does not support ncalrpc, and
[RpcBindingSetOption](https://learn.microsoft.com/en-us/windows/win32/api/rpcdce/nf-rpcdce-rpcbindingsetoption)
call timeout is ignored for local RPC. These rule out treating a synchronous
diagnostic thread plus either setting as a proven bounded implementation.
This research changes no runtime or approved semantics; do not classify an
unverified missing interface as a confirmed version mismatch.

## P14 finite-control delivery

r30b completes all 53 actual deadline cases, service39, four migrated actual
RPC cases, wire/error/startup and same-package runtime gates at 13:37:09 UTC
on 2026-10-21. Post verification completes at 13:37:39, including native142,
presentation509 and governance. Management tests retain 128 checks, zero
failures and zero outstanding allocations. WRITE evidence remains startup-only;
this is not complete editor acceptance or proof that every failure interleaving
has been exhausted. The changed legacy broker service_client fixture is not
claimed as directly executed by this matrix.

Delivery controller 33560 completes at 13:37:53 UTC. All thirteen host images
are published to assets/release and O:/winnt/system32, with recoverable backups
in build/t448-s3/publication-r30b-backup. Independent SHA-256 comparison checks
all thirteen against package-r30b/system32 and both destinations. Protocol60
has 59 async methods and the intentional long command receive; no third
transport, guest modification or mirror diff is introduced.

Production source nets +141 lines and tests +184 (+325 combined); combined
with P13's removal this is -310 source/test/tool lines, excluding documentation
and binaries. UNKNOWN_IF diagnosis, partial-initialization/fault cleanup and
original-host pre-teardown exit permission remain fixed S3 obligations. This
checkpoint delivers verified progress, not S3 closure or owner acceptance.

## UNKNOWN_IF bounded-management feasibility

P14 is pushed at e3fc09f2c with a clean worktree before this investigation.
The remaining synchronous call is classify_missing_interface's
RpcMgmtInqIfIds, after Connect reports RPC_S_UNKNOWN_IF. Its current distinction
between an older interface and a draining current endpoint must remain;
UNKNOWN_IF alone does not prove ERROR_REVISION_MISMATCH.

Read-only source tracing finds the original management wire declaration in
O:/repos.external/OpenNT-4.5/nt/private/rpc/runtime/rtifs/mgmt.idl, SHA-256
59084245FD49A3C182F131E9902E78238CFAD9C5B3CB32A5E632CBE64F71ED15,
Copyright (c) 1991 Microsoft Corporation. Operation zero returns a counted
vector of interface UUID/major/minor identifiers and a separate status output.
The adjoining nbase.idl defines those exact data fields. No RPC runtime
implementation or private NT transport is imported. The original synchronous
ACF cannot provide the required cancellable local call; an async client ACF
can retain the same wire operation and use the existing completion owner.
Microsoft's [local-interface mapping](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-rpce/c898d4cb-c22b-465f-8fe3-1631669dc30a)
also identifies the standard management query's Windows counterpart.

The corresponding nbase.idl SHA-256 is
4FB4130221E8C42F2E12D959F321E3C9F9E44546CEC2696BE60D52503AE3D146;
its Microsoft 1991 notice is retained by the derived declaration selection.

Research-only files under build/t448-s3/mgmt-proof retain the original notice
and source identity, select only operation zero, spell the exact UUID/version
data layout without the unavailable historical nbase.idl include, and generate
an async client using MSVC /MT and MIDL /target NT100. These are disposable
probe inputs, not product imports. The first attempt fails because the SDK
does not provide nbase.idl; the corrected probe does not depend on an external
source tree at build or runtime. The probe creates a private PID-qualified
ncalrpc endpoint in its own process, registers a dummy interface, and uses the
host RPC runtime's management provider. It touches no installed project service,
guest, frontend or user application.

Background matrix controller 23020 passes at 13:48:44 UTC on 2026-10-21:

- x86 and x64: async interface-vector contents equal RpcMgmtInqIfIds output;
  both normal queries return transport/application zero and one interface.
- An authorization callback deliberately delays for twelve seconds. Both
  clients return ERROR_TIMEOUT (1460) at 10,000ms through the existing async
  cancellation/drain helper. Returned vector is null, not consumed as a reply.
- Subsequent denied queries retain application status 5, transport zero and
  no vector. Each test waits for its own delayed provider before final shutdown.
- Both processes exit zero; no product binary or protocol revision changes.

Retained probe hashes: query.idl
FFEBD43D9F9C1B5E32266626DECA013382E3A331DD27135FADD9CDCCF85B2234;
query.acf D822BDAAE7EFA4CF9A03B73D09C56A0547A7C63793F1451021DA4B5BD874B806;
proof.c D32F4B72C79D79642CA7DDBD22D37191854165309C341505C85988A0BE0A433C.

This proves the bounded standard-RPC route is feasible on the tested host,
not that production diagnosis has been repaired. Integration must keep the
same authenticated binding, remaining Connect deadline and original
classification; free every returned allocation after cancellation completion.
Admit only the declaration/client-generation slice under the existing RPC
owner, not a new server, transport, helper process or management framework.
Required production tests include old/current/draining interface classification,
access denial, delayed management response, resource cleanup and shared-deadline
exhaustion. Those production gates, partial initialization and original-host
pre-teardown permission remain open. Published P14 is unchanged.

## Standard management query integration candidate

The feasibility record is pushed at 47aedfec5 before implementation. Select
only the standard operation-zero declaration and client generation under
common/protocol and common/rpc. The original UUID, operation number, counted
interface vector, UUID/version fields and separate application status are
retained; symbols are namespaced to avoid SDK conflicts. The source notice and
exact upstream identity remain above and in rpc_management.idl. Full rtifs,
RPC runtime, server management implementation and historical transport are
rejected: the modern host already provides this operation, and none of those
implementation dependencies are needed. No historical executable code enters
a product or mirror. This is the smallest declaration/async-client adaptation,
not a new management protocol. No guest or mirror diff is added.

common_rpc_classify_interface replaces the synchronous public query at the
existing client diagnostic call. It borrows the authenticated binding and the
Connect absolute deadline, does not retry or obtain a fresh budget, and keeps
the previous old-major/minor versus current/missing-interface classification.
The standard method returns void plus an application-status output: use the
existing async finish/drain helper with no return-value storage, then inspect
that output only after completion. Every returned pointer/vector is released
through the generated client's allocator after drain, including late outputs
when timeout wins. It creates no detached thread or helper process.

The existing x86 graph and its native x64 derived graphs generate the selected
client into their own stable build trees and archive it with common-rpc.
Only consumers that reference the diagnostic pull those archive members.
No application protocol bump is needed: the project service wire is unchanged.
Production-linked rpc_interface_query_test uses the real host management
provider on a private process-qualified endpoint; it does not mock reply,
cancellation or marshaling. Cases cover old major/minor, current and missing
UUID, denial, already exhausted deadline without another server call, sixteen
repeated queries with handle-count comparison, and a delayed provider's actual
ten-second cancellation. Full build controller 22012 and dependent query
controller 37560 are running for r31; no result or publication is yet claimed.

r31 full-component/second-incremental build passes at 13:56:48 UTC on
2026-10-21. Query controller 37560 completes at 13:57:17: x86 and x64 each
pass 27 checks, actual timeout 10,000ms, and repeated-call process handle
counts remain 142/142 and 122/122 respectively. These are production-library
tests, unlike the prior research probe. Serial controller 25604 now runs the
existing complete same-package matrix; post controller 22648 waits for it.
Inputs are frozen. Actual endpoint draining/unregistration and a nonzero
partially consumed connection budget still need explicit targeted evidence;
the matching-version/missing-UUID and already-expired cases do not substitute
for those transitions. No candidate publication or S3 closure is claimed.

While serial inputs remain frozen, supplemental private-endpoint probes link
the same built common-rpc libraries without changing product/test sources.
Controller 23120 passes x86/x64 at 13:59:57 UTC: registered old interface is
1306; actual RpcServerUnregisterIf changes the query result to 1817 (no
interfaces), not a false revision mismatch. After re-registration, a four-second
provider delay with only 1,500ms remaining returns 1460 after 1,500ms/x86 and
1,515ms/x64. This proves the helper consumes the supplied remaining budget;
it does not by itself prove every production Connect race. The supplemental
source/output remains in build/t448-s3/query-transitions.* and must be folded
into the permanent query fixture after the running frozen matrix finishes.

Read-only inspection of the fixed partial-bootstrap group also finds that
worker_base_connect calls OpenNtBaseClientConnectCurrent, whose default still
passes INFINITE. That value currently serves two distinct purposes: bootstrap
deadline and whether a later worker report timeout poisons its shared connection.
The latter is intentionally preserved by the existing worker deadline test:
after a report timeout, a healthy shared worker connection remains usable.
Simply changing the argument to 10000 would silently change that policy.
The required repair must separate the finite ten-second connection budget from
the existing role-specific failure latch, retain worker report behavior, and
prove actual default-worker bootstrap timeout. This belongs to the already
fixed initialization/fault group, not a newly expanded task. No such code
change is made while r31 inputs are frozen.

## P15 verification and review

r31 completes 53 actual deadline cases at 14:09:29 UTC on 2026-10-21,
wire gates at 14:10:25 and the complete serial matrix at 14:13:41. Post
controller 22648 passes at 14:14:26, including native142 with zero handle
delta, presentation509 and governance. Runtime covers DOS17, WINMINE/SOL,
WRITE startup only and the three CAF self-delete/echo cases; it does not
certify complete WRITE editing or every possible desktop schedule.

After those controllers exit, extend the permanent query fixture with actual
interface unregistration, a partially consumed deadline and allocation counts.
No production source or package changes after r31 build. r31b query controller
22580 passes at 14:16:57: both architectures pass 33 checks with zero outstanding
MIDL allocations. Timeout is 10,015ms/x86 and 10,000ms/x64; remaining-budget
timeout is 1,516ms on both; repeated-query handle counts are 156/156 and 138/138.
The supplemental transition cases are now permanent source, not only build logs.

Actual candidate Run16 versus retained protocol59 NTSRV passes at 14:15:58:
candidate exits 1306 before target launch. The first harness attempt fails on
an empty-log null expression at 14:14:27, kills only its owned test service,
and never exercises the candidate. Keep that failed result; r31b fixes only
the log poll and succeeds. Server SHA-256 is
00B90397AC4110336357F4C351B40448594B1AE965A5B1AD1E74256818E78182;
candidate Run16 is
2678A93D707571BF9FB68AD0104F3AF1408CDB963ED9ED7A5FFA02283DBC089D.

Reviewer confirms this replaces the lone synchronous diagnostic RPC with
the same standard operation and one existing async completion owner. Source
nets +64, build scripts +19 and permanent tests +88 (+171 total, excluding
docs/binaries); combined with P13/P14 the three implementation groups net -139.
There is no guest/mirror diff or service wire revision change. This establishes
the diagnostic group, not S3 closure: finite default-worker bootstrap with
unchanged report-latch policy, partial initialization and original-host exit
permission remain open. Complete publication and reviewed push are pending.

Final governance passes at 14:18:04 UTC. Delivery controller 21900 publishes
all thirteen host images at 14:18:56 to assets/release and O:/winnt/system32;
independent comparison confirms all images match package-r31/system32 and
both destinations. Recoverable prior images are in
build/t448-s3/publication-r31-backup. Protocol remains 60; guest and selected
mirrors are unchanged. Reviewed main commit/push follows this complete
progress delivery. The finite remaining S3 groups above are not waived.

## Finite default-worker bootstrap candidate

P15 is pushed at b0b5cbb12 and the worktree is clean before this group.
Separate the connection budget from post-connect timeout disposition in the
existing client owner: private connect_current takes the finite budget and
the existing latch policy independently. Default worker connection uses ten
seconds and does not latch individual later report timeouts; launcher/frontend
connection takes its remaining budget, capped at ten seconds, and retains
terminal timeout latching. No new state owner, transport, retry, protocol
revision or guest/mirror change is introduced. A failed initial connection
still uses local context rundown without a second cleanup RPC budget.

Actual deadline fixture case55 ends its setup connection, suspends only its
owned NTSRV, then calls OpenNtBaseClientConnectCurrent rather than the explicit
timeout overload. It requires 1460 within the ten-second envelope while the
service remains alive. Existing worker report-timeout/recovery coverage remains
unchanged and must pass, proving that finite bootstrap did not poison the
shared worker connection policy. Full build controller 11256 and dependent
serial controller 21988 run r32 (54 deadline cases). Inputs are frozen; no
new result or publication is yet claimed. Partial initialization cleanup and
original-host pre-teardown permission remain separate fixed S3 obligations.

r32 full-component and second incremental builds finish successfully at
14:26:19 UTC on 2026-10-21. The new actual paused-service case55 passes with
fixture exit zero and the startup-deadline/no-cleanup-RPC assertion. Its
fixture SHA-256 is
A7F2E7D2A78FA1F46B9631F58FF94D61F31605B4530B89F30A42D85262C67EC3.
Serial controller 21988 is verified alive; post-review controller 28868 starts
at 14:28:14 UTC and waits for that same serial run. No duplicate test run or
new runtime change is started. The full regression and publication remain
pending; P15 is still the deployed baseline.

Final source review of this candidate finds exactly three production caller
families: Run16 supplies its remaining startup budget, NTCON supplies ten
seconds, and worker-base uses the default entry. Both entrypoints reach one
private implementation; no caller retains an infinite connection budget.
Connect, optional interface diagnosis and BrokerProcess consume one absolute
deadline. Failed bootstrap still performs local rundown, not a second RPC.
The existing worker-exit timeout/recovery test passes in r32, so the new
bootstrap bound does not latch later worker report failures. Source plus
permanent tests net +9 lines; protocol, original mirrors and guest stay
unchanged. This review does not certify the still-running complete matrix.

## Owner correction: remove overextended timeout policy

On 2026-10-21 the owner rejects blanket control-call and lifecycle deadlines.
r32 is withdrawn from publication regardless of its test outcome. Existing
controllers 21988/28868 only run tests/review; neither publishes. Keep their
evidence but do not treat timeout-test success as design acceptance.

The source inventory identifies client deadlines in common/rpc controls,
base_rpc_client_core/launch_style and service_client, plus service-side waits
and expiry in frontend_registry, worker_registry, native_commands and lifecycle.
Run16 WOW startup, NTVWM presentation startup and NTCON teardown waits require
separate purpose/provenance review. Zero-time process liveness checks, frame
pacing and test-harness safety budgets are not blanket product timeouts.
Preserve the explicit Run16/new-service startup and NTSRV empty-service grace;
do not infer approval for other deadlines. Removal must cover timeout latches,
expiry consequences and obsolete tests as well as wait constants. Preserve
normal result delivery, real failure, service-loss handling and RPC completion
storage lifetime. No product correction has yet been built or published.

### Wait-policy removal map and original close evidence

r32 serial testing ends successfully at 14:43:06 UTC, post-review at 14:43:35.
Both controllers are terminal. The owner-withdrawn candidate is not published;
its deadline passes do not justify retaining the rejected product policy.

| Chain | Necessary dependency | Timeout-policy disposition |
| --- | --- | --- |
| Run16 -> newly started NTSRV | Wait for usable service or startup failure | Retain the explicitly approved total ten-second startup budget; do not extend it to ordinary admitted calls |
| NTSRV empty retirement | Cancellable idle grace with no active owners | Retain the explicitly approved ten seconds |
| Common RPC controls and BaseClient core/style calls | Actual reply/error or confirmed service loss; preserve issued-call storage until completion | Remove arbitrary per-call deadlines and timeout-only connection poisoning; keep real errors and cancellation cleanup |
| Service frontend/native admission and finalization | Registration/result/handoff acknowledgement or actual participant loss | Remove expiry-only failure/retirement branches while retaining authenticated identities, ordering, result delivery and wakeups |
| NTCON/NTVWM thread and I/O cleanup | Stop users before freeing their storage | Preserve join/completion dependencies; separately review inherited bounded joins, never treat expiry as successful cleanup |
| DOS Console-close | Original handler completion and Console service's close decision | Separate original semantic review below; not justification for universal RPC timeouts |

Read-only original evidence: OpenNT/windows/core/ntcon/server/input.c,
SHA-256 84D7D4C5A422208D14900C8F3A86B0F9648A3755F03D6D49765B35266617252A,
around lines 2740-2890. The Console server creates the target control callback,
waits gCmsHungAppTimeout for CTRL_CLOSE_EVENT, and on non-forced timeout offers
the end-task dialog with retry/cancel handling. ntuser/inc/user.h line2635
defines its default as five seconds; the setting is not a universal RPC budget.
The current worker-base shutdown_close helper instead applies its caller's
ten-second grace then unconditional process termination. Thus citing original
CntrlHandler alone does not prove equivalence. No original code is imported or
modified for this investigation; do not silently choose a new force-close policy.

The minimal removal should preserve existing RPC ownership and async machinery
where needed for genuine service loss. No transport replacement, whole-commit
rollback, or new timeout manager is required. Tests that assert arbitrary
ten-second failure must be removed/replaced with normal completion, explicit
failure and service-loss assertions; harness safety deadlines are separate.

The subsequent owner clarification selects five seconds for the three explicit
project policies: new-service startup, empty-service retirement and Console-close
handler grace. Source research also confirms original WOW task-end five-second
stages (wkman.h CMS_WAITTASKEXIT), Console logoff/shutdown's configurable default
twenty seconds, DOS event-thread quiescence's 360000ms fault limit, and indefinite
WOW creation/VDM-command event waits. These original internal contracts are not
blanket rewritten or claimed to be fully composed here.

Removal implementation has begun after both r32 controllers finished: delete
the client timeout-latch field/branch and default-worker deadline, use ordinary
connection for NTCON, remove fifty per-call budgets in six client files and the
DOS-observation budget, retain actual cancellation/error result ownership, and
share a five-second constant for the three approved policies. WOW startup waits
for readiness/service or worker exit, not a product timer. Native submission now
has one service implementation without a deadline-wrapper or deadline argument;
Console-return finalization waits for acknowledged state/participant failure,
not elapsed time. Remaining service expiry fields, frontend/worker waits and
test migration are still open. This intermediate source is not built/published.

The close transaction now retains only its accepted-request boolean in place of
deadline/timed-out fields. Remove its timer scheduling and expiry transition;
duplicate requests still join the same transaction and actual quiescence/worker
exit advance its phases. The service fixture retains identity, duplicate,
competing worker/Console-close and participant-loss assertions; simulated elapsed
time must not close participants. Native worker reservation contention also waits
on its existing condition variable without a timer. Static search finds no old
close-deadline/timed-out fields or native-submit-until wrapper in source/tests.
Remaining workerless/admission expiry and fixture migration still prevent build
freeze and delivery; none of these edits is claimed runtime-verified yet.

Frontend startup now waits directly on its existing startup-result, child-process
and creator-process handles; delete its extra waitable timer, due-time setup,
timeout branch and timer cleanup. Frontend acquisition, worker I/O transition
and attachment wait retain their existing condition-variable wakeups and
re-resolve authenticated objects after wake, without elapsed-time failure.
Source tracing finds completion/rundown notifications in frontend_registry,
worker_registry and service_core; runtime loss/concurrency proof is still
required. The remaining frontend admission/workerless deadline fields are
explicitly pending, not hidden behind a changed constant.

Remove the workerless/admission deadline fields, root-retirement deadline
calculation and frontend timer in NTSRV's main loop. Its existing lifetime-change
event now drives OpenNtBaseServiceReconcileFrontends; actual borrowed-Console
loss and acknowledged management close remain, with no replacement idle timer
or immediate workerless kill. NTSRV's distinct empty-service timer remains.
Delete the corresponding deadline-reset loops in worker admission. NTCON return
and channel joins and NTVWM presentation admission now await their existing
completion/stop/thread-exit conditions. Old timer-specific fixtures still need
migration; NTVWM's hidden-Console disappearance poll and imported UI component
joins remain explicit pending audit items. No build or publication yet.

The following source/test migration removes all references to the deleted
frontend expiry APIs. Shared-worker residency still checks native busy/idle,
WOW residency and paired DOS/native root authority. Frontend-authority tests
retain authenticated startup, worker loss, root rundown and exact handle
delivery; a live workerless frontend now parks without expiry. Close-ordering
tests retain duplicate acceptance, quiescence ordering, competing terminal
scopes and actual participant loss. Rename the old timeout cases to root-loss
and prepare-pending; remove the separate attachment-timeout case, retaining
normal delayed attachment and worker-loss cases. These are changed assertions
for the owner's new contract, not proof that the new implementation passes.

NTVWM hidden-Console disappearance retains its existing exact-window identity
checks and poll but no arbitrary deadline. S4 owns conversion/justification of
that poll; this is not represented as event-driven completion. The obsolete
shared ten-second constant and startup-deadline alias are removed. Imported UI
join policy and RPC deadline fixtures still need migration.

Background service verification r34 starts controller PID 14628 using
build/t448-s3/service-r34.ps1, the existing x64 service Ninja tree and the
production-linked service-fixture matrix (38 cases after removing the obsolete
attachment-timeout case). Output is service-r34.out/.err and service-r34-result.json
under the active disposable root; per-case observer budgets remain diagnostic
guards, not product deadlines. This run is not a full-package publication gate.

r34 completes successfully at 2026-10-21 15:04:00 UTC (start 15:03:42 UTC):
the x64 production-linked service fixture builds and all 38 cases pass,
matrix elapsed 5417 ms at concurrency four. This proves the retained service
assertions after expiry removal, not the still-unmigrated RPC stall tests,
UI joins, full package or physical Console return. Published assets remain P15.

UI join audit: kvm_component_destroy queues STOP (or accepts already closed
admission after a terminal fault), calls join_worker, then disposes only after
success. The Win32 join waits on the worker thread handle before closing ready
and worker handles and releasing worker_state. Replace the previous registered
ten-second imported-header exception with LIB_UINT32_MAX, the existing Win32
infinite-wait representation; no new ABI, cancellation mechanism or disposal
owner. Update README and nxvm-import.json with the new exception and local
SHA256 6D93EA9346C9F9820F411F72174E55D7EC762E17F90CD683DB8A677C4D20E319.
This changes one imported library policy line, not an OpenNT or guest mirror.

Replace --stop-timeout with --stop-join in the production channel fixture:
the dispatcher holds an existing lock while a separate test thread starts
the actual stop. Assert stop remains pending, release the dispatcher and
require the same stop call to complete successfully before frontend disposal.
No second stop or expiry recovery remains; the test's observation bounds are
diagnostic only. The pending check proves ownership ordering, not arbitrary
long-duration behavior.

Background frontend r35 PID 34304 (15:07:08-15:07:18 UTC) builds the changed
library/channel fixtures and passes library and stop-join cases. Management
return exits zero but the runner fails its PASS-text gate: the fixture replaces
its Console, so the observer's old Console cannot capture its final output.
Do not classify the whole run as passed. Add an optional explicit report to
that fixture and rerun all three cases as r35b controller PID 32156, retaining
r35 failure evidence. Both runs use the stable x64 frontend tree, isolated
observer desktop and build/t448-s3/frontend-r35* reports; no publication.

r35b completes at 15:08:24 UTC (start 15:08:14 UTC): all pinned import hashes
match, changed fixtures build, and all three library/stop-join/management-return
cases pass. The management assertion report confirms input-mode restoration,
CAF destruction, rejection of late acquisition/video, repeated return and
terminal disposal. This is isolated API-level evidence, not the real internal
NTMON self-delete acceptance or a complete thirteen-image build/publication.

RPC-test migration removes the obsolete numbered timeout/latch matrix and its
query-only scaffolding. The replacement --rpc-wait contract has five named
cases: reply, default connect, disconnect, real service loss and explicitly
bounded bootstrap. Ordinary reply/connect/disconnect are held beyond the old
ten-second policy and must remain pending, then complete after service resume;
reply/connect also perform a subsequent ordinary query. Disconnect checks
released client state and idempotent cleanup. Loss requires the authenticated
service process to exit and checks ERROR_INVALID_HANDLE. Bootstrap alone
requires the approved five-second timeout. Test threads keep their stack/client
ownership until joined; suspension/termination targets only the isolated test
service. These five shared-mechanism checks do not replace the existing
operation-specific normal/admission/error/output tests or claim every RPC
method's lifecycle proven. Those remain part of the full acceptance inventory.

Rename the real-package runner to tests/observation/verify-client-rpc-wait.ps1;
old disposable deadline runners are invalid for this candidate. Full-component
and second-incremental build r36 starts in background PID 28064. RPC controller
36452 waits for that exact process and successful build result, then compiles
the x86 fixture, stages all thirteen hash-matched images and executes the five
named cases serially. Logs/results remain in build/t448-s3/build-r36* and
rpc-r36*. Neither startup nor staging authorizes release or S3 closure.

r36 full thirteen-image build and unchanged second-incremental verification
pass at 15:15:05 UTC (start 15:11:36 UTC). RPC controller 36452 passes reply
(ordinary call survives beyond ten seconds, resumes and accepts the next query),
then stops at connect with exit 1 at 15:15:42 UTC. No loss/disconnect/bootstrap
pass is inferred. The connect setup disconnects its only service client before
suspension; the five-second empty-service timer may therefore compete with
the resumed connection after eleven seconds. This is an unproved harness
interaction, not a diagnosed product defect. Add explicit returned-status and
service-liveness reporting before resolving it; retain the failed evidence.

Further sweep found old workerless-grace/cancel and frontend startup-timeout
expectations in the real bootstrap fixture. Remove the timeout-only startup
case while retaining real rejection and handle-leak checks; rename the two
workerless cases to park/worker-loss-park and assert no expiry across the old
deadline. These migrated cases still require execution.

After r36 staging, remove another production ten-second wait in DOS observation
teardown. The producer is first unregistered; drain waits on the existing
drained event or actual publisher-thread exit, with no timeout parameter.
RPC failure can finish publication and service-loss cancellation remains with
the existing stop path. Update the unit test to inspect pending state directly
and use the unbounded drain contract after release. This post-r36 source change
invalidates r36 as final delivery evidence; a fresh affected/full package and
observer drain/cancellation tests are required. No guest/OpenNT mirror change.

Diagnostic r36b controller PID 3316 builds the RPC and observation fixtures;
the copied ordered outbox/overflow/off-producer/cancellation-join test passes.
Connect reproduces exit 1 with RPC status 1817 (no registered interfaces) and
the server handle still unsignalled, recorded at 15:17:50 UTC. Source tracing
shows the empty timer claims stopping before RpcServerUnregisterIf completes;
the test had removed its only client before pausing for eleven seconds.
This supports an empty-service retirement race, not an ordinary-call timeout.

For connect/bootstrap only, the fixture now starts an exact same-image child
that creates a real authenticated client context and holds it until an explicit
test release event. This prevents the approved empty-service policy competing
with the connection contract. No product keeper, retry or transport is added.
The child is joined and checked before test cleanup; all other scenarios are
unchanged. Background controller 25156 runs r36c connect/disconnect/loss/
bootstrap/reply against the unchanged sealed r36 package. Comparison results
remain pending; retaining the client does not itself prove the diagnosis.

r36c completes successfully 15:20:16 UTC (start 15:19:15 UTC): connect,
disconnect, actual service loss, bootstrap and reply all pass. The only change
for the failed connect comparison is retaining a genuine client while the
caller reconnects, supporting the empty-service race attribution. No product
timeout or idle-policy relaxation was used. These five tests exercise the
sealed r36 binaries, not the later DOS-observation drain change.

Add observer-unit coverage of a publisher exiting with pending facts: drain
must return ERROR_OPERATION_ABORTED rather than successful delivery. Latest
full-package r37 build/incremental controller PID 35624 starts after the r36
build and fixture compilation have completed. Serial controller 21564 waits
for it, then runs service38, observer unit, same-package RPC-five, four migrated
admission cases, wire/bootstrap/workerless-park cases, DOS17, independent WOW
frontiers, internal CAF-three and negative completion/startup checks followed
by governance. Logs are build-r37*, all-r37* and referenced per-group records
under build/t448-s3. Any failed group stops progression; this controller does
not publish. No completion or delivery is inferred from starting it.

r37 full/incremental passes at 15:23:32 UTC; service38 passes at 15:23:53 UTC,
and the extended observer unit passes including publisher exit with undelivered
facts. Independent idle-boundary controller 24800 passes at 15:24:42 UTC:
exact five seconds, no renewal, before/at expiry, Connect cancellation,
occupied-service rearm, stopping and overflow. The test now checks literal
five-second boundaries rather than inheriting the production constant.

Serial r37 stops at loss at 15:24:48 UTC after reply/connect/disconnect pass.
Actual result is RPC_S_CALL_FAILED (1726), followed by a signalled service
process; r36c had observed ERROR_INVALID_HANDLE (6). The client's source only
normalizes to 6 once the authenticated process is signalled, so asynchronous
RPC failure can legitimately precede that observation. Split the assertion:
initial failed call must be 6 or the observed 1726 (never success/timeout), and
after confirmed process death both Failure(actual_error) and Failure(0) must
equal 6. This preserves the confirmed-loss contract and avoids imposing a
new scheduler ordering; no production behavior changes for this result.

RPC r37b controller 38564 recompiles the test and repeats all five cases
against sealed package-r37. Controller 11808 waits for its successful result
before resuming four migrated cases, wire/bootstrap/park, real DOS/WOW/CAF,
negative results and governance with fresh r37b reports. Earlier r37 failure
evidence remains; unchanged product hashes reuse r37 full-build evidence.
The active plan/proposal now explicitly remove stale transaction-deadline
clauses rather than relying only on a superseding introductory paragraph.

### Service-death and peer-I/O failure audit checkpoint

RPC r37b five cases and migration r37b four cases pass. r37c controller 32476
terminates unsuccessfully at 15:29:14 UTC: the wire runner still supplies retired
workerless-grace/workerless-cancel case names to the migrated ValidateSet. This
is a harness invocation error, not a product pass or failure. Management128,
frontend-control75 and worker-control163 pass before that invocation failure.
Replace those runner arguments with workerless-park/worker-loss-park. Fresh
r37d controller 31340 runs the unchanged sealed package-r37 serially, using
resume-r37c.ps1 -Round r37d; stdout/stderr and final result are all-r37d under
build/t448-s3. It must not publish or overwrite previous evidence.

Owner's service-death clarification is now explicit in the proposal. Source
review finds NTVWM input_failure/presentation_pump -> request_fault_exit ->
worker_base_request_exit -> service RequestWorkerExit -> management arbitration;
the participant waits for a service instruction or authenticated process death.
It does not rediscover the frontend after a broken transport. This is source
evidence, not yet a full fault-injection proof of every error branch.

NTCON is NOT equivalent: session_service.c frontend_pump returns
ERROR_PIPE_NOT_CONNECTED when its channel exits without an authorized release;
main.c session_entry consumes that result, calls frontend_service_close,
FrontendConsoleRestored and DisconnectCurrent, then returns to wmain. The
ConsoleRestored provider only acknowledges restoration (and rejects draining);
it does not authorize this unexpected exit. This is a confirmed S3 lifecycle
ownership gap. Required repair: report failure through existing RPC/service
arbitration, stop failed I/O, and obtain service disposition before retirement;
confirmed service death alone takes the local terminal path. Validate both
peer-loss directions and no reconnect to a replacement service. No runtime
repair or publication is claimed by this audit checkpoint.

### Candidate participant-fault disposition (protocol 61)

r37d finishes successfully at 15:37:55 UTC, including the sealed protocol-60
package's wire/park, runtime, CAF self-delete/echo, startup rejection and native
failure cases. This does not validate the subsequent protocol-61 edits.

Generalize the existing authenticated RequestWorkerExit RPC to
RequestParticipantExit (same parameters, no additional transport). The service
derives worker versus frontend identity from its connection, never caller-sent
role or a supplied target PID, then invokes existing management arbitration.
NTCON's fault path reports once and waits for service-selected return/final
close or actual service death. Normal and faulted paths share one retirement
handler. Failed report/restore does not authorize exit, retry or a false
acknowledgement. Full failure disposition when restoration itself is impossible
remains a coverage/design-review item, not a demonstrated successful handoff.

Service fixtures extend competing worker/Console close cases with frontend
self-report, stale generation rejection and idempotent repeated requests.
frontend_scope_lifetime_test adds four production-pump unit cases: withheld
permission, ordered return/final close, actual loss notification, rejected
report and failed restore. The service/process handles are substituted with
events in this unit test; it does not prove real RPC or process-death wiring.

Background controller 25756, verify-r38.ps1, waits for controller 31340 before
building the complete protocol-61 package/incremental and running service and
frontend scope fixtures. Logs fault-r38.out/.err, build-r38*, verify-r38*.
The initially invoked script contains an incorrect launcher fixture path;
the saved script is corrected to the actual x86 worker graph. If the running
parsed script retains the old path, record its invocation failure and run the
fixture separately without repeating a successful full build. No publication.

r38 stops in the first x86 build at 15:39:46 UTC: the interface version changed
to 61 but four generated-interface symbol references still named v60_0.
Update client interface inquiry, service register/unregister and the identity
fixture to v61_0; no ABI fallback is added. Fresh controller 36080 runs
verify-r38.ps1 -Round r38b with corrected paths and retained cache. Evidence
fault-r38b*, build-r38b*, verify-r38b* remains pending; no successful build or
delivery is claimed for r38.

Symmetric source sweep additionally finds that NTVDM console_client send_video
returns transport failure to publication, which records the failure and ends
the publication thread without reporting participant intent. The generic
common Console client intentionally only latches/returns transport errors.
The NTVDM executable's lifecycle watch currently receives service instructions
and service death, but has no local I/O-fault report input. This missing
NTVDM-owner binding belongs to the same S3 fault chain, not S4 polling work.
Retain the generic transport boundary: do not make common own lifecycle or
inject an executable-specific policy into the common client. Before wiring,
trace both synchronous Console API errors and asynchronous publication/input
errors to choose one executable-owned report path without holding I/O locks
across teardown or changing ordinary API parameter-error semantics.

r38b completes successfully at 15:44:26 UTC: complete thirteen-image build,
second incremental (1414 intermediate inventory), service cases and the expanded
frontend scope fixture pass. This is candidate build/unit evidence, not deployed
product or actual fault-RPC acceptance.

The NTVDM binding is now implemented using a borrowed local failure event in
the existing common Console client. Sticky transport/framing failures notify
that event; ordinary server-operation and argument errors retain API returns.
NTVDM owns a duplicate for its I/O lifetime, so lifecycle-watch destruction
cannot leave a reused/closed handle in a producer. Rebinding a service-authorized
new I/O channel reattaches the same local notification. The existing independent
NTVDM lifecycle watcher submits participant intent once, without rearming the
fault notification, while retaining Console/worker close reception. This adds
no thread, timer, cross-process event or communication transport. WOW allocates
no character-I/O fault event. Common knows no service policy or executable role.
Input-watch failure uses the same latch; synchronous invalid framing and
asynchronous video transport share the common notification.

console_client_test now verifies real broken-pipe I/O and an asynchronous video
send failure, with substituted service acceptance deliberately withholding a
close instruction. Repeated I/O failures must report once and leave the worker
alive. Existing WOW/service-loss entry checks remain separate. These are
production-linked owner/transport tests, not real service/guest acceptance.
Background controller 33504 runs verify-r39.ps1: full/incremental build,
service fixtures, frontend scope and isolated Console-client cases. Evidence
io-r39*, fault-r39*, build-r39*, verify-r39* is pending; no publication yet.

r39 controller 33504 completes successfully at 15:50:42 UTC. Full/thirteen
images and second incremental pass, as do service fixtures, frontend scope,
real Console transport broken-pipe and asynchronous broken-video assertions,
independent WOW close reception and service-loss cleanup-entry checks. Service
decisions in these owner fixtures are substituted; they do not prove RPC wiring.

Add a distinct frontend-io-fault real-RPC case to the existing admitted VDM
fixture: a test carrier gets the original first command then deliberately
closes its admitted pipe while staying alive. Actual NTCON must report the EOF
to actual NTSRV and prepare return; the carrier waits for a real Console-close
instruction, verifies NTSRV and NTCON are still alive, then honors that order.
The parent requires both carrier and NTCON exit with NTSRV still alive. This
is a test carrier, not guest execution. The original normal admission fixture
previously closed the pipe before exercising commands; it now retains the
lease and performs the real pause/release-begin/physical-close/released sequence.
This removes an unintended fault from its positive-path contract, not an
assertion relaxation; duplicate acquisition/rights and original exit 7 remain.

Controller 27552 runs all-r39.ps1: seal package-r39 from exact thirteen build
hashes, observer outbox, five real RPC wait/loss/bootstrap cases, five migration
cases including the new EOF route, wire/bootstrap/park, runtime DOS17 and
independent WOW frontiers plus CAF-three, negative results and governance.
Evidence all-r39*, rpc-r39*, migration-r39*, wire-r39*, runtime-r39* is pending.
No publication starts automatically. All source modifications remain local.

r39 intermediate verification: rpc-r39-result passes at 15:53:26 UTC;
migration-r39-result passes at 15:53:47 UTC; wire-r39-result passes at
15:54:38 UTC. The migration group includes actual frontend-io-fault and the
repaired normal admitted-vdm case. Runtime regression remains in progress;
these results do not prove the unfinished runtime gates or S3 host-exit closure.
Supplemental controller 25592 (post-verify.ps1 -Round r39 -PriorPid 27552)
waits for the serial controller before native-completion, presentation and
governance checks. Both controllers were verified alive during observation.

Remove the obsolete execution-rule demand for generic ten-second lifecycle
tests/-FullDeadlines. The rule now links the architecture's approved policies
and separates diagnostic budgets from product deadlines; no production input
or running test is changed by this documentation correction.

runtime-r39-result passes at 15:57:31 UTC: WRITE frontier, DOS17, WINMINE,
SOL and CAF-three using package-r39. The serial log explicitly records actual
NTMON self-delete with active-buffer echo before Enter. extra-r39-result passes
at 15:57:40 UTC for native worker failure and completed-worker loss. Serial
stdout also reports startup-rejections and native-io-error assertions passed.
Final controller/governance and supplemental results remain pending at this
checkpoint; no publication or S3 closure is claimed. These runtime observations
do not resolve the separately recorded original-host exit-permission gap.

Both controllers subsequently finish successfully: all-r39-result at
15:58:02 UTC and post-r39-result at 15:58:34 UTC. The supplemental run passes
native-completion and isolated presentation assertions plus documentation and
diff gates. Neither controller remains live. The candidate still requires
final source review, coherent publication and commit/push; these successful
gates do not close the outstanding S3 source-exit inventory.

### P16 reviewed progress publication

Final source review checks authenticated participant classification, generation
re-resolution after dropping the service lock, shared management-close
arbitration, one-shot fault notification without rearm, and independent duplicate
event ownership until I/O producers have joined. NTCON does not resume failed
I/O or acknowledge failed restoration. Service death remains distinct from
submission failure. The existing input watcher carries NTVDM notification;
no thread, timer or additional cross-process transport is introduced.

Controller 34616 runs deliver-r39.ps1 (fresh review-r39final gate followed by
publish-r8.ps1 -Round r39 -ReviewRound r39final -BuildRound r39 -Package
package-r39). It finishes publication at 2026-10-21 16:00:37 UTC. All thirteen
images match the tested build hashes at assets/release and O:/winnt/system32;
publication-r39.json records the full set. Prior installed and committed assets
plus manifest are backed up in publication-r39-backup. Read-only discovery found
no installed component or Hook/style DLL holder requiring termination.

Before recording this delivery, diff accounting including the new RPC-wait
test is: production/non-Markdown source +268/-345 (net -77), tests +470/-568
(net -98), combined net -175. Documentation is counted separately and includes
retained failed-attempt evidence. OpenNT and guest/media diff remains zero.
The existing NTCON kvm-base join-limit exception changes two lines each way,
with updated README and import hash; it adds no mirror lines or new API.

This is P16 progress, not S3 closure: L08 original-host exit permission and
the remaining startup/partial-initialization inventory still need source-led
implementation and coverage. S4 polling audit has not begun. Commit/push and
clean-tree identity will be verified separately after the final documentation
gate; no owner acceptance is implied by publication.

P16 repository delivery is verified: commit
0e1f3f501c513730769f9cfa2241201f1a402663 matches origin/main, the working tree
is clean, and commit-r39-result records success at 16:02:42 UTC. Git emitted
an unreachable-object maintenance warning; no manual prune was executed.

### L08 source-order audit after P16

Read-only tracing against P16 confirms that the earlier seven final
ExitProcess substitutions are only an inventory, not a sufficient design:

| Chain | Actual ordering / consequence |
| --- | --- |
| SoftPC terminate (base/support/terminat.c) | Device/video/storage shutdown precedes host_applClose, host_terminate and ExitProcess; permission only at the final exit cannot protect resource ownership. The repeated-termination branch also calls CRT exit directly. |
| Normal CPU return (base/support/main.c) | host_start_cpu returns into host_applClose before the standalone outer finish; authorization only in outer cleanup is too late for original host resources. |
| Authored session escape (mvdm_softpc_termination.c) | mvdm_softpc_terminate_current_session calls host_applClose before session_terminate_current; an owned pre-close seam exists, but it is not the only termination caller. |
| WOW whole-process exits (wkman.c and wow32.c) | All five inspected final exits first call ExitVDM(WOWVDM, ALL_TASKS). Original task-record cleanup must not precede the intended service decision merely because process exit itself is delayed. Single-task ExitVDM remains a different contract. |
| Received worker close / service loss | worker_base_shutdown_close with INFINITE calls the original close callback on the receiving thread. ntvdm_worker_lifecycle_end joins the input watcher, so routing this callback through standalone cleanup would self-join. The finite Console-close grace instead runs its handler on another thread. |
| Existing service Exit RPC | OpenNtBaseServiceExit invokes original record cleanup under the service lock and returns wait-identity disposal, not a distinct pre-cleanup permission. It must not silently be treated as an already-existing permission handshake. |

The inspection covers source ordering, not runtime proof of a new fix. No
mirror or product source changed in this audit. Implementation must establish
permission before the first whole-worker teardown, preserve received-command
and service-loss paths without a second request, and avoid moving teardown
onto a thread that it joins. The prior three-file/seven-site estimate is not
authority to ship a late check or to intercept ExitProcess globally. Resolve
the existing original and authored entry seams before changing the measured
mirror budget; retain normal task completion separately from worker retirement.

The L08 continuation identifies a concrete authored escape-ownership defect:
session_terminate_current checked only session binding and a process-shared
armed flag, although obj.vdm/ntvdm.c establishes setjmp on the main thread.
CntrlHandler's CNTRL_PUSHEXIT branch calls the same escape from a bound Console
handler thread. A shared session binding is not ownership of another thread's
saved C stack. Add thread-local ownership at arm/disarm/terminate in session.c;
foreign-thread termination returns the existing refused-operation result and
cannot mutate completion or disarm the owner's escape. The service-authorized
Console handler can then return through its existing shutdown owner rather
than jumping across threads. No mirror, protocol, guest or global hook change.

Extend the existing session_lifecycle_test with a real foreign thread, unchanged
owner state/arming, same-thread completion code 37 and disarm/reuse checks.
Expose it through the existing source-owner Ninja generator, linking the actual
session/guest-memory objects. session-r40 runs configure and that focused test
in the stable x86 tree; its five-second thread join is a diagnostic test budget,
not product policy. This repair is only an L08 safety prerequisite, not proof of
the pending service-permission handshake or complete Console teardown.

Controller 27520 completes session-r40 successfully at 16:13:22 UTC: real
Ninja compiles session.c and session_lifecycle_test.c and links the production
guest-memory object; the executable exits zero. This proves foreign-thread
refusal/no mutation, owner-thread completion and disarm/reuse in the focused
fixture. It is not a full package, a reproduced user-facing failure or a new
delivery. The deployed P16 package remains unchanged while L08 implementation
continues; no guest or OpenNT mirror diff was introduced.

### L08 rejected shortcut and existing arbitration verification

Further source tracing supplies the exact counterexample behind the S1 ban on
inferring process exit from ExitVDM arguments: wow32/wkman.c W32DestroyTask
calls ExitVDM(WOWVDM, ALL_TASKS) when nWOWTasks is zero, then continues
WOWCleanup, WK32DeleteTask and thread-handle cleanup. Therefore a generic hook
on ALL_TASKS would incorrectly turn task cleanup into an asynchronous process
termination point, and could interrupt the remaining source-owned cleanup.
Keep this route distinct from WK32ExitKernel/WK32FatalExit/WOWStartupFailed and
the explicit fatal/hung-process sites. The alternative nt_term.c body is not
selected by the current Ninja graph; do not patch it as another runtime owner.
The direct keyba.c terminate caller is HUNTER-only, not evidence of a current
product entry. Existing namespace-only mvdm_softpc_symbol_compat.h renames both
the definition and calls; changing that macro alone cannot insert a wrapper.

The potential management race was checked before changing code: accepted
frontend close already sets each associated worker watch's close_kind to
WORKER_CLOSE_CONSOLE under the service lock. service_terminate_worker then
recognizes the accepted kind and does not signal shutdown. The existing
reservation fixture's competing==2/self_exit branch verifies that subsequent
participant intent leaves both events unsignaled before quiescence, then only
Console-close signals afterward. No extra root scan, timer, retry or second
close state is needed for that path; retain the existing implementation.

The exit integration must use explicit process-intent call sites before
whole-worker teardown, preserve task-local cleanup as-is, and retain the same
service arbitration rather than adding another controller. The source audit
does not yet prove lock-safe insertion at all of those sites. This finite L08
obligation remains open; neither its seven-call inventory nor the passing
session-escape fixture is a substitute for that proof.

Controller 36080 starts full-r40.ps1 at 16:20:05 UTC for the authored session
escape change: full thirteen-component build and unchanged second incremental,
service/owner fault fixtures, real session ownership fixture, sealed package,
RPC migration/wire, DOS17/WOW/CAF runtime and supplemental checks. The reusable
disposable runners take a round parameter rather than copying another complete
runner. Removed their obsolete wait for old PID 31340; supplemental verification
supports PriorPid=0 for same-controller sequencing, never waiting on PID zero.
The first launch precheck matched its own command text and refused before any
runner started; the corrected check matches actual -File invocations and the
new controller is the only run. There is no automatic publication or commit.
Production source inputs remain fixed while this group is running.

Session-escape review checks both actual callers: standalone cleanup disarms
before its thread unbind, and mvdm_softpc_execution_run_until_return disarms on
both normal and escaped fixture returns before unbinding. The TLS pointer is
only stack-target ownership, not another lifecycle authority or session ABI
field. Foreign-thread refusal occurs before completion-code/state mutation;
the owning thread can still disarm after completion. The real-thread fixture
joins its foreign caller before the owner longjmps and uses static session
storage, so no modified automatic local is read after longjmp. Product builds
must continue sharing the worker's existing session/TLS through their existing
imports; no duplicate session implementation is introduced into WOW32.

At the current r40 observation, both configurations and all first-pass product
builds pass, including the redirector's session_thread_finish import check.
Second incremental validation is in progress. These are intermediate build
facts, not completed runtime or publication evidence.

r40 finishes successfully at 16:29:49 UTC (full-r40-result.json). Full thirteen
images and unchanged second incremental cover 1415 intermediates; service38,
frontend scope, four Console fault/lifecycle cases and actual session escape
ownership pass. Five RPC wait/loss cases, five migration cases, wire/park and
WRITE frontier/DOS17/WINMINE/SOL/CAF-three pass on package-r40. The negative
result/startup/I/O cases and supplemental native RPC (142 checks, zero handle
delta), presentation and governance checks also pass. WRITE remains startup
frontier only, not editing/exit acceptance. Controller 36080 is no longer live.
Publication and commit are still separate gates.

P17 review accounting: authored production change +11/-3 (net +8), test
+46/-1 (net +45), existing build generator +3/-0. This small safety fix adds
56 non-documentation lines; no protocol, OpenNT mirror or guest change.
Together with P16's net -175, the two changes still reduce non-documentation
source/test/build lines by 119. It intentionally does not merge task completion
and process exit or claim the remaining L08 handshake is implemented.

P17 publication controller 5024 finishes at 2026-10-21 16:31:41 UTC:
deliver-r39.ps1 -Round r40 runs a fresh final gate and publishes the complete
thirteen-image package-r40 using build-r40 and its completed regressions.
publication-r40.json records successful SHA-256 equality at both destinations;
publication-r40-backup preserves the exact prior package and release manifest.
Read-only installation/module discovery found no process needing termination.
Protocol remains 61. No global interception, original mirror or guest change
is present. Commit/push is verified after the final post-record documentation
gate. L08 permission placement/concurrency and initialization remain S3 work,
not silently deferred to the polling or WOW successor stages.

### L08 pre-cleanup and startup receiver closure

Read-only follow-up against pushed P17 (3fb04dd46) traces callers of
host_applClose, not merely ExitProcess. The following findings extend the
existing L08 insertion audit, not the task's feature scope:

- mvdm_standalone_worker_begin connects before session activation, installs
  ntvdm_worker_lifecycle_begin before WOW binding/guest-memory initialization,
  and arms the session escape before returning to the actual setjmp in
  obj.vdm/ntvdm.c. input_watch_create immediately starts its receiver thread.
  A management callback can therefore enter original termination during
  partial initialization. P17 prevents foreign-thread longjmp but does not
  serialize this teardown with initialization.
- The begin failure label and mvdm_standalone_worker_finish both enter local
  cleanup without service permission. Cleanup joins the lifecycle receiver.
  Moving that cleanup into the receiver would instead introduce self-join:
  worker_base_input_watch_destroy waits indefinitely for its own thread.
  Startup resource preparation, receiver activation and cleanup ownership
  must be treated together, not repaired by relocating one cleanup call.
- Original base/support/terminat.c cleans devices before host_applClose;
  gating only host_applClose is too late for this path. Original
  base/support/main.c also calls host_applClose directly after CPU return.
  The owned termination adapter and bounded execution fixture do likewise;
  fixture composition must not accidentally acquire production service policy.
- Original dos/command/cmdmisc.c routes fTSRExit to
  nt_event.c:cmdPushExitInConsoleBuffer. That function sets CNTRL_PUSHEXIT,
  calls host_applClose and ExitVDM, posts WM_CLOSE, then exits its thread.
  It is a separate Console-group intent, not evidence that the seven final
  process-exit sites cover the cleanup closure. Preserve its classification
  while moving control arbitration to NTSRV; do not equate it silently with
  worker-only close or wait until its final ExitThread.
- ega_trcr.c's additional host_applClose call is inside EGA_DUMP. Its object
  being present in the Ninja graph alone does not prove that debug body is
  in the selected product. No new product repair is inferred from that hit.

Implementation constraints from this audit: the actual closing executor must
not join itself; service authorization must precede the first worker-wide
cleanup; startup must not expose partially initialized original resources to
a closing executor; and Console-group intent must retain its distinct service
scope. These are source findings, not runtime reproductions. No new timeout,
transport, guest change or mirror modification was made. L08 remains open.

### L08 scoped participant intent candidate

Protocol 62 extends the existing RequestParticipantExit RPC with a copied
scope scalar. Existing NTCON/worker fault callers retain participant scope;
the Console scope resolves only the authenticated worker watch's retained
frontend generation. No frontend PID/key crosses this request. WOW and native
GUI reject Console scope; missing association and unknown scopes fail without
starting a close. The service delegates to the same management transaction,
including quiescence, first-accepted close kind and requester-independent
progress. No new event, retry, deadline or transport is introduced.

The existing production-linked reservation fixture adds worker-origin Console
closure, authentication/scope/kind/association negatives, and duplicate intent
while preserving the pending-quiescence assertions and original competition
cases. The normal fixture runner includes this case. The first r41 compile
finds the remaining version-61 MIDL interface names; these are corrected in
service registration, client identity lookup and the identity fixture. r41b
then builds NTSRV/the fixture and passes all five focused cases, including
both worker/Console ordering directions. r41c additionally builds both native
client widths and checks repeated Console intent. Its result is recorded after
completion, not inferred from launching the controller.

r41c controller 11396 completes successfully at 2026-10-21 16:51:27 UTC:
NTSRV, the production-linked fixture and x86/x64 client archives build in the
existing Ninja/ccache trees without reconfiguration. All five focused cases
and documentation governance pass. Diff review confirms that authentication
precedes scope handling and the close key is revalidated by generation after
dropping the lock. These tests exercise the actual service provider, not yet
the new scope over a live cross-process RPC connection.

This is a staged edge of the admitted L08 group, not an independently completed
feature: original fTSRExit/pre-cleanup callers, initialization and closing-thread
ownership still require integration. The installed P17/protocol-61 package is
unchanged. No guest or OpenNT mirror changes have been made. Do not publish
protocol-62 components individually or describe these service fixtures as an
end-to-end original DOS exit test.

### L08 original Console-group entry candidate and real RPC proof

r42 controller 35820 completes at 2026-10-21 16:57:33 UTC. Existing Ninja trees
incrementally build the protocol-62 service, frontend and x86 RPC fixture.
The new real worker-console-exit case leaves the actual NTCON pipe intact,
submits Console intent twice, observes only the Console-close instruction,
and proves frontend retirement while NTSRV stays alive. Existing frontend EOF
and original command/reentry/exit-7 cases pass on the same inputs. This is real
RPC and frontend execution with a test worker, not original guest acceptance.
Documentation governance passes. No installed image is changed by these tests.

Original source tracing: OpenNT base/mvdm/softpc.new/host/src/nt_event.c SHA-256
7F555A87BA029627D6811C0F7B96964BB8A96AF90C60C89012975A4AC5442076 has
cmdPushExitInConsoleBuffer clean the host, call ExitVDM, post WM_CLOSE and retire
the calling thread; CntrlHandler's CNTRL_PUSHEXIT branch then calls ExitProcess(0).
Unchanged guest tcode.asm sets scs_tsrexit after Do16BitPrompt returns the exit
condition. cmdmisc.c copies/releases its CMDINFO guest lease before dispatching
that intent. CmdDispatch/MS_bop_4 add no lock around the call. Frontend quiescence
uses its own presentation thread and restoration acknowledgement, not a callback
into that guest command frame. This establishes the reached call edges; it is
not a blanket claim that every other termination entry is lock-free.

DIV-355 stages this original Console-group path: submit the existing scoped RPC
through worker-base, retire the original calling thread, and defer its same
host_applClose/ExitVDM/ExitProcess sequence to the existing Console-close handler.
The service-created handler thread replaces the old Console-window control
delivery; no WM_CLOSE is sent to a host window. This removes the obsolete
DIV-147 session escape at this one branch. Failed submission does not retry or
authorize worker-resource teardown. Other natural/fatal/init entries still need
their own pre-cleanup integration and remain L08 blockers.

Controller 3732 runs full-r40.ps1 -Round r43 for the fixed candidate: thirteen
components, unchanged second incremental, service/owner fixtures, six real-RPC
migration cases including worker Console intent, wire/runtime and supplemental
checks. Source edits are stopped during this run. No publication or commit is
claimed until the resulting evidence and mirror review are complete.

### r43 completed regression and original TSR entry witness

Controller 3732 completed successfully at 2026-10-21 17:11:07 UTC:
`build/t448-s3/full-r43-result.json`. Full thirteen-component and unchanged
second incremental builds completed at 17:04:25 UTC, followed by service and
owner-fault fixtures, six RPC migration cases, wire, same-package runtime and
supplemental checks. The CAF check reports actual NTMON self-delete and active
buffer echo before Enter. This covers the staged Console-scope change, not the
remaining original natural/fatal exits or partial initialization.

Independent `tests/observation/console_tsr_exit.asm` uses DOS INT 21h/AH=31h;
it does not replace or patch any original guest. The temporary runner is
`build/t448-s3/tsr-r43.ps1`. Initial PID 22328 failed before execution because
the stage lacked a tests directory. After creating that directory, PID 25132
timed out with a blank screen under the inherited long environment. Neither
attempt proves a product exit-path result. PID 16160 repeats the established
DOS regression environment (host System32 PATH, stage TEMP/TMP), reaching
`CONSOLE-TSR-READY`, the resident COMMAND prompt and delivered `exit` input.
The observer remains alive, sees only itself attached to the returned Console,
and records Run16 exit 0x42b (1067). Its provisional zero-exit assertion fails;
the failure is retained in tsr-r43c-result.json and is not waived as a pass.

The identical probe against package-r40 (PID 21828) loses the observer before
its final report; it is not a measured zero-exit baseline. Its NTVDM hash
C7D29E800B3E3EFE3AAE222CCD3D47BD23F8A7E67497A7B48A3063157537D615
matches the published P17 asset. Do not attribute the observer's termination
solely from this incomplete report.

Source attribution for the candidate's result: cmdmisc.c handles fTSRExit
before GetNextVDMCommand can report ordinary task completion;
service_preserve_parent_results marks an unsignalled parent receipt failed
before original ExitVDM removes its record, and OpenNtBaseServiceExitCode
returns ERROR_PROCESS_ABORTED. Whole-worker exit zero is therefore not proof
of an ordinary task completion zero. Resolve the probe's acceptance against
that explicit distinction; do not change service completion semantics merely
to make a provisional zero assertion pass. No additional product source or
published binary changed during these supplementary probes.

The source-defined distinction supplies r43d's explicit assertion, not a new
product semantic change: unfinished receipt returns ERROR_PROCESS_ABORTED;
do not infer its result from the worker's exit code. PID 34720 ran from
17:16:59 to 17:17:06 UTC and passed. `tsr-r43d-result.json` records the
independent COM hash
31F19B6C57C98A8B59AF574B4F7DBBF2DAEF3BD2D93A7F96A05F94FA1BA0DD67.
It verifies delivered exit input, resident prompt/marker, Run16 return 1067,
exactly the observer left attached to the Console, and no stage Run16,
NTVDM or NTCON process **before** fixture cleanup. NTSRV remains alive in
the observer report. This is original TSR Console-entry acceptance, not
acceptance of the remaining natural/fatal or initialization group.

Reproduction uses NASM `-f bin` on the tracked independent ASM, placing the
COM in the isolated package's tests directory. With PATH set to host System32
and TEMP/TMP to the stage, run the built console-startup-observer with arguments
`<stage>/system32/run16.exe <stage> <report> <stage>/tests/TSREXIT.COM
--observe-console-input-text <exit-plus-CR> --observation-timeout-ms 30000`.
The 30-second limit belongs only to the diagnostic observer. Check the above
report/screen assertions and exact stage executable paths before cleanup.

### P18 owner-directed progress publication

Owner explicitly requests publishing the latest fully tested candidate before
continuing the remaining original logic. No later product edits are included.
Controller 19472 runs publish-r8.ps1 -Round r43 and completes at
2026-10-21 17:20:19 UTC. All thirteen images from package-r43 are copied to
assets/release and O:/winnt/system32 and each destination is SHA-256 equal
to build-r43/result.json. publication-r43.json records the exact list; the
recoverable preceding package is publication-r43-backup. No installed product
process was running at the publication preflight. No partial package or guest
replacement is published.

Review checks authentication before scope selection, service-derived frontend
identity, preserved quiescence/duplicate handling, WOW/GUI rejection and original
cleanup order. The new route does not grant cleanup on RPC acceptance. DIV-355
is the only changed mirror code file: nt_event.c changes +13/-22 relative to
P17, not a claim of eliminating its accumulated upstream divergence. The pinned
upstream comparison remains +148/-41 for that file. Complete S3 original-exit
and partial-initialization permission remain explicitly open. This is P18
progress delivery, not S3 acceptance or T closure.

### Post-P18 startup instruction admission correction

Source tracing finds a concrete startup authorization obstruction:
service_core.c creates the authenticated worker watch and manual-reset close
events during Connect, but service_terminate_worker rejects a native participant
until registered_worker is set by frontend/backend registration. That condition
predates the nonblocking close transaction and incorrectly makes a failed
backend a prerequisite for asking NTSRV to terminate it. Remove only that
readiness condition; retain connection/generation/watch authentication,
first-accepted close kind and Console quiescence. Execution admission still
requires its existing backend readiness. No new transport, deadline or guest
change follows.

The production-linked preparing-worker-exit case requests participant exit
before native backend registration and acquires its wait-only shutdown handle
after acceptance, requiring it already signalled. It then exercises duplicate
intent and a competing Console close without changing the first close kind.
This verifies instruction latching, not yet actual startup-failure teardown
or original-host initialization safety. The installed P18 package is unchanged
until the remaining implementation group and delivery gates complete.

Controller 27156 runs exit-scope-r41.ps1 -Round r44 from 17:29:39 to
17:30:18 UTC. Incremental service/provider build, preparing-worker-exit,
worker-console-exit, worker-exit-then-console, console-then-worker-exit,
management-worker-then-console and management-console-then-worker pass,
followed by documentation governance. No source edits overlap the run.
Native GetNextCommand already refuses signalled worker/Console instructions
before delivery; RetainCommandWorker excludes a closing watch. The adjacent
backend-registration comment is updated afterwards to distinguish execution
readiness from close admission, without executable changes. This is a local
fix/verification step within the remaining group, not another published P.

### Post-P18 native consumer initialization ordering

NTVWM main.c previously marked itself managed only after carrier creation,
backend registration, membership threads, execution storage and service watcher
all succeeded. membership_initialize also destroyed partial membership on an
error, including the very close-event handles needed by request_fault_exit.
The consumer now acquires shutdown and Console-close handles before carrier
creation, then marks that instruction-equipped connection managed. Failed
membership initialization retains resources for the existing request/dispatch
path; a missing local stop_requested event is harmless, not an attempted
SetEvent(NULL). No timeout, replay, new IPC or mirror change is introduced.

Reproducible test: tests/observation/verify-ntvwm-close-priority.ps1 with
BuildRoot below build, CompilerWrapper for MSVC x64 and FontInclude naming the
generated frontend-font directory. It compiles the actual main.c owner and
production worker-base shutdown helper. Case 7 injects backend registration
ERROR_NOT_ENOUGH_MEMORY, asserts both original close handles remain valid and
unchanged, rejects any attempted receiver creation, then requires no cleanup
on request acceptance and actual cleanup/acknowledgement only after shutdown.
The seven existing cancellation, worker/Console-close, service-loss lock-fence
and request-versus-permission cases remain. This is a provider-boundary test,
not real NTSRV startup or complete product acceptance.

Sandbox controller 8184 ended before creating a test directory. Elevated
controllers 4640 and 18956 reached compile/link but failed because the new
reachable initialization function pulled unrelated I/O callback dependencies
into the small fixture. They are retained as failed runs. Fixture-only consumer
creation substitutes now reject unexpected calls without linking that unused
I/O closure. Controller 37384 passed all eight cases at 17:39:00 UTC; artifacts
are build/t448-s3/close-priority-r45c. No installed binary was changed.

Controller 26124 runs startup-r45.ps1 from 17:40:00 to 17:40:33 UTC:
production x64 ntvwm.exe rebuilds only main.obj and its link through the stable
Ninja tree with repository-pinned ccache paths; the second build reports no
work. The eight final-I/O/carrier ordering cases and documentation gate pass.
This evidence paragraph and Current update follow that gate, so its result
does not claim a check of these later documentation edits.

The earlier connected-but-not-yet-instruction-equipped failure window is NOT
claimed repaired. NTVDM original pre-cleanup authorization, original callback
ownership and initialization remain the other finite group boundaries. Keep
these explicit rather than treating the new native case as group completion.

### Original pre-cleanup authorization candidate (DIV-356)

The first original-entry implementation uses the existing worker lifecycle
owner rather than a late ExitProcess hook. Its closing field now identifies
the one cleanup thread: a natural caller claims it before requesting service
exit, receives the actual worker/Console instruction or observes service death,
then continues original cleanup. Management callbacks use the same claim;
the finite Console helper transfers ownership to its actual handler thread.
Recursive terminate -> host_applClose -> host_terminate entries by that owner
do not submit another request. A competing terminal caller cannot run the
original non-atomic repeated-terminate shortcut over another cleanup.

Three SoftPC insertion sites precede actual original destruction: terminate's
device/CMOS/video work, host_applClose's event/timer/device close, and direct
host_terminate's ExitVDM. Five WOW process exits likewise obtain permission
before original ExitVDM(ALL_TASKS), keeping their ExitProcess after it. The
parent export carries the original chosen exit code, replaced with 6 only for
verified service death. ExitKernel/FatalExit copy/free their guest argument
alias before the blocking call. W32DestroyTask and ordinary task completion
are unchanged. DIV-356 in src/mvdm/README.md records the four upstream files
and exact hashes. This is a candidate, not a runtime-restoration claim.

Controller 14012 runs original-exit-r46.ps1 from 17:44:13 to 17:44:22 UTC:
actual ntvdm.exe and console-client-test.exe compile; --natural-exit and
--natural-loss require acceptance to leave the caller blocked until an event,
and repeated same-thread authorization must issue one request. Existing
--lifecycle-wow, --lifecycle-loss and --lifecycle-console callbacks call the
new guard recursively and retain exit codes 74/74/73. All five pass. Original
cleanup bodies are substituted in this focused fixture, so this is ownership
and permission proof, not runtime proof of every original caller.

The later WOW export/code-carrying extension and guest-alias release are not
covered by r46. Full-component/repeated-incremental generation and the amended
five permission cases are running under controller 28804, original-exit-r47;
do not treat its start as a pass. Partial initialization, the earlier connection
capability-acquisition failure window, and lock/join behavior at the complete
original cleanup call chains remain explicit pending acceptance boundaries.

Controller 28804 completes at 17:50:33 UTC: all thirteen components build,
the repeat preserves 1,415 intermediates, and all five permission cases pass.
The amended natural-loss case returns 6 through the actual parent authorization
export; normal natural-exit preserves 131. These are still fixture exit codes,
not proof of each real WOW fatal/normal entry. Review then removes an accidental
CRLF rewrite in terminat.c, restoring the selected upstream LF formatting.
The incremental four-file mirror change versus P18 is +41/-6 (net +35), with
README registration counted separately (+28). No original guest is changed.
This necessary pre-cleanup insertion increases the mirror budget; no claim of
mirror reduction or completed S3 is made.

Controller 37356 starts full-r40.ps1 -Round r48 at 17:52:06 UTC against those
final source bytes. The group now includes the native partial-initialization
case and natural-exit/loss assertions in addition to prior service, I/O, RPC,
DOS/WOW and CAF checks. Its explicit budgets remain diagnostic runner budgets,
not product timeout policies. r47 is retained as the earlier complete build;
r48 must finish before any new runtime-pass/publication claim. P18 remains the
installed thirteen-image baseline while the remaining initialization/ownership
work is unfinished.

### r48 SOL environment limitation and owner-authorized temporary bypass

Controller 37356 ends failed at 17:59:42 UTC. Complete build/incremental,
service/owner fixtures, real RPC and wire checks pass. Runtime records WRITE
frontier, DOS17 and WINMINE passing, then SOL probe exit 4. Its log proves
screen=427x320, maxtrack=447x340, initial client=431x261; an ordinary resize
requests 585x384 but obtains 431x281. wow_sol_gameplay_probe.c returns at its
geometry precondition, before deal/stock/close assertions. The entire r48
result stays failed; no claim that gameplay or its exit behavior passed.

Controller 5556 runs sol-geometry-r49.ps1 from 18:01:56 to 18:02:13 UTC,
serially using immutable package-r43 (published P18) and package-r48 with the
same probe/environment. Both reproduce exit 4 and exactly the same desktop,
window and clamped-client dimensions. This attributes this test obstruction
to its available geometry, not a demonstrated candidate regression; it does
not prove candidate SOL gameplay is correct. Captures and input hashes remain
in sol-geometry-r49-package-r43 and sol-geometry-r49-package-r48.

Owner explicitly replies: "我确实没有别的方案 你就先暂时bypass sol".
Temporarily exclude this SOL full-gameplay gate while the desktop restriction
persists, label it unverified/environment-limited, and retain its normal test
source/assertions unchanged for later restoration. All other gates remain.
Controller 24508 starts remaining-r48.ps1 at 18:03:01 UTC for CAF, supplementary
negative checks and review. Its result cannot overwrite the original failed
full-r48 result or turn the waived SOL row into a pass.

### r50 remaining checks and protocol-63 initialization work

Controller 24508 finishes at 18:03:46 UTC: all three CAF cases pass, then the
old supplementary orchestrator rejects r48's failed runtime prerequisite.
This is not a second product failure. New remaining-r50.ps1 explicitly checks
the three passed runtime rows plus all three CAF rows, retains the failed SOL
result, and runs the omitted native failure/completion tests and review.
Controller 5556 completes 18:06:59--18:07:27 UTC with passed=true and an explicit
SOL UNVERIFIED waiver. No failed result is rewritten.

The finite initialization group now stages protocol 63's atomic Connect
capabilities, documented in the working design. New provider assertions cover
non-worker/WOW/native role selection, stale generation, denied SetEvent rights
and rollback when the second duplicate fails. Worker owners borrow cached
handles through worker-base; no late capability RPC or independent policy is
introduced. Build/runtime verification is pending; P18 is still installed.

r51 controller 18700 builds all thirteen protocol-63 images at
18:11:59--18:15:29 UTC with 1,415 unchanged second-build intermediates.
Service fixtures pass by 18:15:54, including atomic controls role/rights/rollback.
The owner group then fails lifecycle-wow: its new controls mock incorrectly
creates a Console-close event for WOW (FAIL line 345). Correct the mock's role,
retain the original no-Console assertion; do not mark r51 owner tests passed.

The remaining startup change is now staged: borrow controls immediately after
Connect and before session activation; retain them on partial allocation
failure; obtain service permission before standalone startup cleanup/finish.
Delay original cleanup receivers until host_start_cpu after original main's
host_init_screen/InitialiseDosEmulation. DIV-356 records the single new hook.
Owner tests add pending Console-close before activation and partial-startup
Console permission without entering the uninitialized original handler. Full
real original-entry runtime proof and obsolete capability RPC removal remain
pending; no new product deadline, guest mutation or release is claimed.

### r56 capability-query cleanup focused verification

The protocol-64 working change removes the superseded BrokerProcess,
WorkerShutdownEvent and WorkerConsoleCloseEvent RPC declarations/dispatch.
Connect already supplies these authenticated capabilities atomically. Real RPC
tests now obtain the service handle from Connect rather than the deleted query.
The worker-control exception fixture retains the two remaining event interfaces;
both loops use ARRAYSIZE and the expected issuance count derives from that size.
This fixes an otherwise out-of-bounds second loop after interface removal.

Hidden controller PID 15208 ran build/t448-s3/verify-controls-r56.ps1 from
2026-10-21 18:39:22.554 through 18:39:29.330 UTC. The stable x86 Ninja tree
rebuilt nine dependency-selected edges and the actual fixture passed 133 checks,
zero failures and zero process handle delta. stdout/stderr and the terminal JSON
are retained under build/t448-s3/controls-r56.* and controls-r56-result.json.
This proves only the remaining wrapper issuance-exception/attachment cleanup
contract, not protocol-64 package or runtime acceptance. Old provider-only test
calls still require migration; no new package was published. P19 remains the
installed and pushed baseline.

### r57 provider consolidation

Removed the two superseded service event-query functions and declarations.
Existing reservation scenarios now select their needed handle from the actual
ConnectionControls provider via a test-only projection that closes the unused
handle and does not implement role/authentication policy. Nonworker Connect
success returns no worker capabilities; its assertion now checks success plus
NULL instead of the retired query's ACCESS_DENIED result. Stale generations
still require ACCESS_DENIED. Existing direct provider cases retain both-output
atomic rollback and wait-only rights assertions. Repository source/test search
finds no remaining callers or declarations of the three removed RPCs or the
two retired provider functions. No mirror or guest modification is added.

Hidden controller PID 35164 runs verify-r39.ps1 -Round r57 for the full package,
unchanged incremental build, service fixtures and nine owner lifecycle cases.
Its exact command and live CMake child were verified through the OS process
table; missing sandbox process visibility did not cause a duplicate restart.
Terminal evidence will be read from io-r57-result.json before any acceptance
claim. Installed P19 is unchanged during verification.

### Original-exit caller and cleanup-thread source review

Reviewed WK32ExitKernel and WK32FatalExit: each copies the scalar exit code and
releases GETARGPTR before permission acquisition; ExitVDM(ALL_TASKS) and actual
ExitProcess remain after permission. W32HungAppNotifyThread releases gcsHungApp
before the guest-interrupt wait and its eventual whole-process permission call.
Its earlier successful single-task branch remains unchanged. WOWStartupFailed
and unexpected W32Thread return similarly obtain permission before ALL_TASKS
retirement. These are source-order findings, not runtime execution of every
original fatal/hung branch.

RequestParticipantExit drops the service lock before generation-keyed management
arbitration, which accepts/signals rather than waiting for the requesting task.
The spontaneous-exit caller claims the cleanup thread before this request;
competing watcher callbacks cannot enter original cleanup, and same-thread
recursive terminate/host_applClose/host_terminate calls reuse the claim.
The finite Console helper explicitly transfers that claim to its handler thread.
Worker/service-loss callbacks use the infinite-grace helper's inline callback,
not a second joining cleanup thread. Original TerminateVDM goes through terminate
to host_terminate/ExitProcess; it does not call standalone_worker_cleanup.
The separate standalone finish/startup-failure paths authorize before stopping
and joining watchers and before disconnect closes borrowed capabilities.

This review supports the caller ordering and absence of a service/task wait
cycle for these paths. It is not a substitute for fault-injection and real
runtime coverage; r57 still must pass the delivered owner/activation cases and
the serial package regressions. Hidden controller 16036 waits for the verified
live r57 process handle, then runs all-r39 and post-verify for r57; a failed
prerequisite terminates that controller without staging/publication.

### r57 build and owner verification result

Controller 35164 completes successfully at 2026-10-21 18:44:50.374 UTC.
FullComponentBuild reports thirteen images and 1,415 unchanged intermediates
after second x86/x64 builds (build-r57-result.json, completed 18:44:15.906).
Service fixtures pass at 18:44:38.064, frontend-scope checks at 18:44:43.541;
all nine console owner scenarios pass, including pre-activation Console close,
WOW without a Console capability, service death and spontaneous/partial-startup
exit authorization. The terminal io-r57-result.json is passing. Controller
16036 continues serialized package verification; no publication is inferred.

### Protocol-64 diff review and serial verification progress

Relative to P19, the reviewed production diff adds seven and removes 116 lines;
tests add 50 and remove 47 lines. Combined owned code is reduced by 106 lines,
excluding documentation and generated/released binaries. No mirror or guest
delta is added. The test-only capability projection has no role decisions;
it delegates to the actual provider and closes the unused output. Connect's
service-process rights check replaces the removed BrokerProcess query test;
authenticated stale-generation rejection remains covered at the provider and
other wire operations, not by retaining a dead endpoint.

The lifecycle plan also contained two stale ten-second bootstrap phrases.
They now agree with the already approved five-second policy; no runtime timeout
is added or changed. r57 RPC reply/connect/disconnect/loss/bootstrap succeeds;
six migration cases pass at 18:46:07.639 UTC and wire identity/bootstrap/park
checks pass at 18:46:55.876. Product regression remains active, not a completed
acceptance claim. A guarded P20 commit runner is prepared but not executed;
it requires passing acceptance, coherent publication, hash equality, governance
and reviewed paths before staging and pushing.

### S3 P20 verification and publication

Controller 16036 completes accept-r57 at 2026-10-21 18:50:29.354 UTC, successful.
The immutable package-r57 passes RPC, six migration cases, wire identity and
bootstrap/park checks, DOS17, WINMINE gameplay, WRITE's four-sample prior frontier,
CAF cmd/command/ntmon before-Enter echo, native failure/completion-loss and
startup/I/O rejection cases. SOL is owner-confirmed startup, not a gameplay or
layout pass. Supplemental native-command and presentation checks and documentation
governance also pass. WRITE editing/normal exit remain explicitly unverified,
not an expanded claim from its startup frontier.

Hidden publication controller 7488 verifies every gate and publishes all thirteen
protocol-64 images at 18:50:51.243 UTC, with SHA-256 equality between package,
assets/release and O:/winnt/system32. publication-r57.json records exact hashes;
publication-r57-backup retains both prior P19 sets and manifest. No installed
project process was present at the checked publication boundary. P20 commit/push
is the remaining delivery operation. S3's final fixed-scope reconciliation is
still required; this publication alone does not close S3 or T448.

### S3 closure audit: L06 restoration fault coverage

P20 commit 2b754a31e5b20a5e6dc0e6584bb599a0b3ee89b7 is pushed and the
delivery runner verified a clean worktree at 18:51:52.113 UTC. Subsequent
closure review found that frontend-scope tests assert generic restore failure
and real management return, but do not individually inject each Console restore
API failure required by L06. This is a bounded missing test, not a newly proven
product defect or a reason to move L06 to a later S.

The existing console_frame_failure_test translation unit now wraps the actual
frontend_session.c calls to SetConsoleMode, SetConsoleScreenBufferInfoEx,
SetConsoleCursorInfo and SetConsoleActiveScreenBuffer with thread-local test
faults. Four individual failures and one combined failure require every
independent step to be attempted, the first error to survive, input restoration
to remain independent, the presentation thread to be joined and retained
resources to remain safely cleanable. No production or guest change is made.
The permanent runner is tests/observation/verify-frontend-restoration.ps1.

Hidden r58 controller 31752 failed at 18:54:40.427 UTC before the new cases:
stop-join's initial geometry setup returned error87, largest58x15, requested
80-column canvas. The original observer and console logs are retained. The
fixture already establishes a 4x6 Terminal font for private-full; stop-join now
uses that same setup, with all geometry assertions retained. Controller15952
runs restore-r58b.ps1; results remain pending, never inferred from a successful
compile or the earlier P20 runtime package.

r58b completes successfully at 18:55:37.240 UTC: the existing window library,
stop/join and management-return cases pass, as do the four independent restore
API faults and combined fault. restore-r58b-result.json and observer/assertion
logs retain the exact execution. This covers actual frontend destruction and
independent restoration under injected OS errors, not a claim that Windows can
restore a Console whose underlying OS resources are permanently unavailable.

### S3 fixed-scope closure reconciliation

Closure basis is the published P20 protocol64 package plus test-only r58b/r59.
The following maps the admitted L04-L10 obligations to implementation and
actual evidence; it does not claim every possible OS fault/interleaving.

| Scope | Implemented owner and boundary | Verification and remaining boundary |
| --- | --- | --- |
| L04 role dispatch | ConnectionControls exports authenticated role capabilities; worker-base dispatches, NTVDM/NTVWM select cleanup. WOW has no character initialization. | r57 preparing-worker-exit, management-gui, lifecycle-wow/pending and real ntvwm startup/execution. Native GUI targets remain unmanaged; no new task controls. |
| L05 close arbitration | lifecycle.c RequestParticipantExit/CloseManagementNode/ReconcileFrontends retain the generation-keyed operation, independent of requester survival. | r57 management-frontend-close/order/root-loss/prepare-pending, both worker/Console orderings, late worker close, worker-exit orderings and frontend-completion-barrier. CAF-three verifies actual self-delete return; S5 adds the planned integrated host/presentation permutations. |
| L06 quiescence/restoration | NTCON freezes terminal return, stops producer access, joins before release, attempts independent restoration and preserves error; service receives actual phase facts. | r57 frontend-scope faults reject false success; r58b stop-join and management-return reject late bind/video; r59 full frame fixture covers stale ownership, EOF, partial publications and handle stability. New four individual plus combined restore faults prove all steps attempted/first error/input independence. Permanent OS restoration failure remains an error, never claimed successful. |
| L07 service loss | Connection-owned process capability, common async cancellation and executable-owned cleanup; no recovery fence or reconnect. | r57 raw reply/connect/disconnect/loss/bootstrap, owner lifecycle-loss/natural-loss and frontend root-parent cases; explicit error6 versus real task-result assertions. No guarantee of peer restoration ordering after service death, as explicitly approved. |
| L08 original cleanup | DIV-355 Console intent and DIV-356 pre-cleanup permission/activation retain original cleanup; source review above covers recursion, caller locks and released guest aliases. | r57 natural/partial-startup/management/loss owner cases, ordinary DOS17, WOW startup/gameplay and CAF runtime, plus retained original TSR entry proof. Original fatal/hung branches have source-order proof, not a claim that every fatal guest state was induced. No guest changes. |
| L09 GUI cleanup | Service startup disposition and native target binding/rollback; transient launcher has no native GUI target lifetime ownership. | r57 management-gui, native-command/native-worker, raw startup-rejections, actual ntvwm-execution/public-startup and completed-worker-loss. Existing startup-only/wait contracts retained; no task-manager substitute. |
| L10 rundown | Existing connection/reservation generations, pinned handles and actual process observation own removal; stale facts cannot revive records. | r57 stale-generation/capability failure assertions, route cancellation, request/worker/root loss, abandoned/completed launcher rundown, reentry cases and common RPC zero-handle-delta checks. S5 repeats integrated concurrency; this is not mathematical proof of every OS handle-reuse schedule. |

r59 hidden controller23360 completes at 2026-10-21 18:57:40.193 UTC. Full
console-frame-failure-test --private-desktop-full passes the retained allocation,
projection, EOF, geometry, ordered-input and stable-handle assertions; the new
restoration matrix passes again. Ninja reports no work; governance and diff
checks pass. All thirteen P20 release, installed and package hashes still match;
there are no production/src, assets or build-tool changes. Rebuilding or copying
identical product images is unnecessary for this test/document-only P21.

No open S3 foundation defect was identified by this reconciliation. S3 closes
with the P21 reviewed test/evidence delivery; the existing S4 polling cleanup
and S5 integrated acceptance remain required before T closure. SOL layout and
WRITE editing/exit frontiers remain separately recorded limitations, not new
passing claims or lifecycle waivers. P20's net106-line reduction is followed
by necessary bounded test coverage; report test additions separately rather
than disguising them as product simplification.

### Owner SOL acceptance clarification

Owner subsequently states: "你只要记录sol可以正常启动即可 我手测可以
只是窗口内容layout有点问题而已". Record successful startup as owner-provided
manual evidence, with no inferred package hash or complete gameplay coverage.
For this lifecycle delivery the required SOL milestone is startup, not the
full-gameplay geometry probe. Keep r48/r49 failures unchanged as diagnostic
evidence; they do not negate the owner's startup result and are not converted
to passes. The reported content-layout issue is retained in TODO for queued
WOW32 usability work. Other lifecycle and publication requirements are unchanged.

r52 controller 22860 completes 18:18:32--18:21:53 UTC: full thirteen-image build,
unchanged second-build intermediates, service fixtures and all nine owner cases
pass. Cases are broken-pipe/video, lifecycle WOW/loss/Console/pending instruction,
and natural exit/loss/partial-startup Console permission. These are owner/adapter
proof, not a substitute for the subsequent real-package regression. accept-r52
will run native close-priority, RPC, migration, runtime/CAF and post checks;
its SOL record explicitly cites owner startup evidence, not gameplay pass.

r52 acceptance controller 37572 stops before package staging at 18:23:08 UTC:
the native-close fixture has twelve failures, isolated in diagnostic r53 to
modes 5--7 (fault-report waits). The new fixture had substituted the self
pseudo-handle for the old duplicated real process handle. r54 directly measures
WaitForMultipleObjects: pseudo-process gives WAIT_FAILED/6; a real wait-only
duplicate gives WAIT_TIMEOUT. Restoring the real handle makes all eight
unchanged close assertions pass (controller 22152, 18:25:45 UTC). Product
Connect already exports real handles; no product workaround is added.
accept-r55 resumes the as-yet-unrun package stage/runtime checks using r52's
unchanged product images and r54 native-close evidence. Failed attempts remain.

### P19 coherent progress delivery

accept-r55 controller 25492 completes 18:26:31--18:32:09 UTC with passed=true.
It consumes unchanged build-r52 images, r52 nine owner cases and r54 eight
native-close cases. Real RPC reply/connect/disconnect/loss/bootstrap, six
migration scenarios, wire identity/bootstrap/park, WRITE four-sample frontier,
DOS17, WINMINE gameplay, three CAF self-delete/before-Enter echo cases,
native failure/completed-loss, startup rejection/native-I/O failure and final
presentation/native-command checks pass. SOL has an explicit owner-startup
record, no invented gameplay or package-hash claim. Full build and second-run
1,415-intermediate identity are retained in build-r52/result.json.

Source review confirms the original main.c exit(1) is SECURE-only, not enabled
in the production graph. nt_reset's missing-f return rejects direct invocation;
the authenticated worker constructor uses original BaseGetVdmConfigInfo, whose
vdm.c lines 1953 onward always append -f. No additional mirror change is made
for these non-admitted startup paths. Real service process/event exports are
wait-only, partial exports close before connection rundown, and consumer
borrowed handles remain valid through joined teardown until disconnect.
Original fatal/hung caller lock coverage and obsolete three capability RPCs
remain explicitly pending for final S3 review, not silently certified here.

The five changed mirror files retain LF and DIV-356 registration; delta versus
P18 is +46/-8 (net +38), not a reduction. Owned product net growth is +117;
tests/build net growth including the new 14-line verifier is +142. New tests
are correctness coverage; obsolete RPC removal is the identified reduction
opportunity, not permission to delete their retained identity/rights coverage.
Guest files and assets/winnt.zip remain unchanged.

Publication controller 29896 succeeds at 18:32:44.548 UTC: all thirteen images
from package-r52/system32 match assets/release and O:/winnt/system32. The
coherent old set is recoverable in publication-r52-backup. No installed process
needed termination. publication-r52.json records every SHA-256. P19's delivering
commit records the exact repository identity; this is a progress release,
not S3 closure or a claim that all T448 work is complete.
