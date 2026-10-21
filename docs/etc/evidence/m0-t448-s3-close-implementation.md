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
