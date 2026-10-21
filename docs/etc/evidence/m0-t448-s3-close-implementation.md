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
