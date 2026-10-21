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
