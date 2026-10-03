# T424 S10 frontend notification evidence

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Question and boundary

Does an undecided frontend join lose its notification when NTCON checks for
channel work after checking for joins? S10 repairs project-added NTSRV
notification maintenance, not original DOS/WOW execution or the public wire.
T424 remains open. The experiment history below retains failed attempts;
the final delivery section records S10 publication and bounded closure.
The owner subsequently reopened S10 for the Terminal DIR regression below;
the previous gates did not establish correct DIR prompt placement.

## Inputs and procedure

Baseline: main `8ff029fbd`, delivered S9 protocol/RPC33. Before this experiment
the only working-tree change was S10 admission in CURRENT. Production service
sources remain unchanged during the failing experiment. The existing x86 /MT
fixture links the production service archive; no copied service implementation
or replacement provider is introduced.

Reproducible entrypoint:
`tests/adapter-basesrv/base_service_reservation_test.c`,
`frontend_notification()` / `frontend_join_wait()`.

Commands from the repository root:

```powershell
cmd /c build\M0-T424\S2\r001\run-ninja-parallel.cmd basesrv-service-reservation-test.exe
& build/M0-T424/S2/r001/basesrv-service-reservation-test.exe --frontend-notification-baseline
& build/M0-T424/S2/r001/basesrv-service-reservation-test.exe --frontend-notification
```

Reuse the valid S2 cache. New logs are in
`build/M0-T424/S10/r001/notification-baseline.log` and
`notification-before-fix.log`. The build recompiles only the modified fixture
and relinks it against unchanged production inputs.

The fixture admits a real suspended child process as the frontend, using the
existing authenticated exact-object startup/registration/lease mechanisms.
It then performs this ordered interleaving:

1. Root `FrontendJoinCandidate` reports no candidate.
2. Another thread enters the real `AcquireFrontendRoot` and publishes a join.
3. Waiting on the actual capability event establishes that publication occurred.
4. Root `FrontendRequest` reports no channel work.
5. A zero-time event query observes whether the join notification survived.
6. Root still obtains the candidate and explicitly refuses it; the acquiring
   thread returns access denied and has no exported capabilities.
7. Join thread, connections, service, suspended fixture child and handles are
   cleaned up before evaluating the deliberately failing final assertion.

No Sleep positions this race. Five-second waits are bounded failure guards,
not delays used to generate or conceal the interleaving. No desktop input,
guest mutation or O:/winnt replacement occurs.

## Observations and interpretation

- Build: success; existing imported-header anonymous-union warnings only.
- Baseline witness: notification result **258 / WAIT_TIMEOUT**, exit **0**,
  `PASS: S9 lost-wakeup interleaving reproduced`.
- Required invariant: identical result **258 / WAIT_TIMEOUT**, exit **1**,
  failing the requirement that an undecided join keeps the event signaled.
- The candidate remains retrievable after the channel-empty check. Therefore
  this is a notification/state divergence, not absence of a join request.

Causal source locations: `frontend_registry.c` sets the capability after
publishing `frontend_join_caller` in AcquireFrontendRoot;
`service_frontend_idle()` considers only `frontend_request_root` and resets
the same capability. NTCON's `session_service.c` checks candidates before
channel requests. The test exercises precisely the gap between those checks.
Confidence is high for this specific lost-wakeup defect. It does not prove
that every historical rapid-launch timeout has this cause.

## Ownership and remaining work

All affected state is project-added NTSRV adaptation in its private modules;
the `opennt/source` path is not the original `src/opennt-host` mirror.
Original execution/records/scheduling, independent completion receipts,
retire/restored acknowledgements and direct worker I/O remain untouched.
The defect belongs to the existing service lock and manual-reset notification,
not worker-base or a new scheduler.

The next implementation must project all actionable work under that lock:
undecided joins, pending request markers and actionable native routes. It must
update publication, decision, cancellation, attachment and rundown; decided
joins awaiting a lease must not keep the event signaled. Route deletion and
worker-exit cleanup are part of the review. Event-operation errors need an
explicit existing-owner failure contract, not silent success.

## First production repair and focused tests

`service_refresh_frontend_work()` now maintains the single notification under
the existing service lock. It represents undecided joins, pending DOS request
markers and native routes which still need attachment to a live worker.
Attached/delivered routes and decided joins are not authentication-pump work.
Native command payload delivery remains a separate worker condition-variable
contract; it is not incorrectly added to this frontend event.

Join publication, decision, per-call cleanup, request publication/rollback,
attachment, route removal/cancellation and connection rundown invoke this
private mechanism. The old partial `service_frontend_idle()` implementation
is removed. An event operation failure marks the root closing and wakes the
existing broker state/condition paths; result-bearing operations propagate
the error. Cleanup cannot claim successful acquisition after such a failure.
No original mirror, guest, wire, worker execution or frontend rendering changed.

The original strict reproducer passes after repair: notification **0 /
WAIT_OBJECT_0**. Extended fixture cases cover:

- refusal with a wrong-nonce denial, then final event reset;
- approval while the previous lease remains outstanding: launcher still waits,
  the candidate is no longer actionable and the notification resets;
  LeaseReady then grants the actual frontend capabilities;
- simultaneous undecided join and native attachment: deciding/refusing the
  join leaves the event signaled for attachment; completing the final
  attachment resets it. Worker process cleanup is awaited through the real
  service callback rather than a timing delay.

All three cases pass in `notification-overlap-r006.log`. This run links the
same selected production archives/fixture objects into the fresh build-only
`frontend-notification-r006.exe`, using `link-notification.cmd`, then runs
`--frontend-notification`. Rebuilding an existing test output was blocked by
retained failed test children; the fresh filename avoids replacing a live
test image, not any product verification assertion.

The next extension adds worker death before attachment. The actual worker
process-exit watch removes the route; FrontendRequest consumes the stale caller
marker and resets the event. All **four** cases pass in
`notification-worker-loss-r007.log`, with the same fresh fixture entrypoint.

Earlier extended-fixture runs are retained non-passes: the first lacked the
root's authenticated Console identity before native reservation; after fixing
that setup, service stop ran before the asynchronous worker-watch cleanup.
Adding the actual cleanup callback/event fixes the fixture synchronization.
One relink failed with LNK1104 and an older test executable was run afterward;
that run is not evidence for the newer source. Subsequent commands explicitly
stop on compile/link failure. Old failed fixture children remain a cleanup
item: process enumeration could not return their executable paths, and the
auto-review refused name-only termination. No broad kill or permission change
was performed.

Existing focused modes pass: `--native-command`, `--native-backend`,
`--frontend-root`, `--frontend-rundown` and `--frontend-wait`.
Legacy `--frontend-wait-root-loss` and `--frontend-wait-request-loss` fail at
their RequestFrontend setup: they substitute a different root after the
launcher already retained its initial root. The unchanged RetainFrontendRoot
authentication rejects this before the new notification publisher is reached.
These are recorded non-passes, not weakened into acceptance. Authenticated
root/route cancellation still needs a current-boundary fixture.

Incremental product build also passes:
`cmd /c build\M0-T424\S2\r001\run-ninja-parallel.cmd run16.exe ntsrv.exe ntvdm.exe ntvwm.exe ntcon.exe ntmon.exe VDMREDIR.dll`.
Six dependent EXE links rebuild against the changed service archive; the
NTVDM VdmTib storage audit passes. Unchanged DLL identity still needs the
coherent release manifest, not an inference from this build.

## Actual rapid-relaunch comparison

An isolated eight-file candidate is now staged under
`build/M0-T424/S10/r001/runtime`; `candidate-manifest-r001.json` pins its
cache sources and hashes. It copies the accepted S9 immutable guests and
configuration. No formal product files are replaced. Tests temporarily map
only Z: and remove it in finally.

`tests/observation/verify-frontend-rapid-relaunch.ps1` runs a real outer CMD
batch with twelve immediate native CMD /C and DOS COMMAND /C MEM pairs,
followed by one final native CMD /C request. There is no line pacing or
inter-launch sleep. It asserts actual outer completion, real Console output
and the final native output marker; it is not proof of interactive keyboard
handoff, worker PID reuse or simultaneous launcher isolation.

- Candidate `rapid-relaunch-r001.txt`: outer exits 19; strict output assertion
  fails because the final pair's native marker is not captured.
- Candidate `rapid-relaunch-r002.txt`: final explicit native request still
  does not leave its marker; outer exits 19 and strict output assertion fails.
- Accepted S9 control `rapid-s9-control-r001.txt`, using the identical second
  script: outer exits 19; identical strict output failure.
- Comparing the candidate r002 and S9 control Console captures yields **no
  differing lines**. Both contain MEM output, older native markers and outer
  `S10-RAPID-COMPLETE`, but not `S10-NATIVE-FINAL`.

These are **non-passes**, not a relaxed exit-code-only rapid-launch gate.
Evidence separates the repaired deterministic notification race from an
additional baseline output/handoff defect or capture boundary; its causal
owner is not yet established. No Sleep, redraw, assertion deletion or original
guest exemption has been applied. The r001 assertion was refined to place an
explicit native request last instead of requiring an old line after a DOS
screen replacement; the newer stricter current-output assertion still fails.

Console17/Window17 completed against the coherent candidate with the existing
unchanged `Verify-CommandExitStatus.ps1` gate. The actual process returned zero
and all seventeen cases in each mode passed; `matrices-r001.log` retains the
34 result lines. This does not substitute for the strict rapid-output test.

The fixture's fifth phase disconnects the authenticated root while a join is
pending. The waiting caller wakes and obtains a replacement-root reservation,
without an old process/capability/retire/restored handle. It clears that
reservation through the existing admission cleanup. The fresh x86 link
`frontend-notification-r008.exe --frontend-notification` passes all five
phases in `notification-root-loss-r008.log`. The two legacy setup failures
above remain non-passes; this is a separate current-boundary cancellation
test, not a reinterpretation of their results.

`run-retained.ps1` completed with exit zero. `retained-r001.log` records actual
modern EDIT/Ctrl+Q -> native echo -> DOS MEM; same outer CMD DOS/native/DOS
relaunch; independent Console isolation and resumed input; and all four
NTVDM/NTVWM worker-loss/frontend-loss retirement cases. These use the unchanged
tracked observation scripts and candidate runtime. Temporary Z: is removed
by the runner's finally block.

`run-output-diagnostics.ps1` under the build run now isolates native-only,
DOS/native with no final outer write, and DOS/native with a final outer write.
It retains native exit/output observations without changing the tracked
strict rapid-output assertion. Three rounds in each variant complete with
outer exit 19 and retain the final native marker (`output-diagnostics-r001.log`).
Twelve native-only rounds also retain every native marker and the final outer
marker in `output-diagnostics-r002.log`. Twelve DOS/native rounds without a
final outer write retain `S10-DIAGNOSTIC-FINAL` at buffer row 29, with viewport
rows 1..28. The otherwise equivalent case with a final outer CMD echo loses
that marker: the outer text is captured at row 28, viewport rows 2..29.
All three executions return 19, and the runner completes with exit zero.
That diagnostic exit only means observations were collected, not that the
strict rapid-output gate passed. The paired observation shows the final native
text was delivered before outer output, narrowing the missing-marker issue to
screen/cursor return or subsequent host scrolling, not simply failure to launch
or publish native output. The exact source transition remains unproved; no
repaint/delay or original-semantic change is justified by this result alone.

## Rapid-output causal follow-up: native VT and viewport origin

The previous diagnostic handle 29746 completed with exit zero, as did the
new r008 handle 60912. This is observation completion, not a strict rapid
product pass. Z: is removed. Production inputs and O:/winnt remain unchanged.

`output-dos-native-outer-output-r007.txt.target-output.txt` observes the real
native target before and after its WriteConsole output. Before output, its
hidden Console has buffer 120x9011, viewport (0,0)..(79,27), but cursor (0,29).
After output, viewport is (0,1)..(79,28), cursor (0,28). The r008 split-write
probe records output mode 7 (processed, wrap and VT): writing the marker
leaves cursor (20,29); writing CRLF moves it to (0,28). The final marker is
then overwritten by the outer CMD's next echo. Canonical snapshots show the
same post-target state. Pure native rounds do not exhibit this loss.

Read-only hidden snapshots in r006 selected the actual kind=2 worker from
NTSRV's management projection, not arbitrary process discovery. The failed
r004/r005 attempts misquoted the CMD FINDSTR pipeline, printed diagnostic
errors into the Console and produced no valid hidden comparison; neither is
a pass. r003/r006/r007/r008 reports stay under the S10 build run.

The tracked `tests/app/frontend_chain_input.c` adds private-desktop-only
`--viewport-write-test <report>` and `--active-viewport-write-test <report>`.
They create isolated Console buffers, preserve/restore the original active
buffer, and compare classic/VT output with mixed-origin/current viewport
coordinates. These are Console API fixtures, not product helpers or copied
handoff providers. `--snapshot-current` reads inherited Console state;
`--output-snapshot <marker> <report>` is an actual native target and logs
before-marker/after-marker/after-CRLF state without imposing geometry.

Build: `cmd /c build\M0-T424\S2\r001\run-ninja-parallel.cmd frontend-chain-input.exe`.
Run the S2 observer on the fixture with MVDM_OBSERVER_PRIVATE_DESKTOP=1,
working directory build/M0-T424/S10/r001, a fresh observer report, fixture
arguments above and `--observation-timeout-ms 10000`. Observer exit must be
zero and target result `exit=0x00000000`; inspect the cell report separately.

`viewport-write-r001` (inactive classic) and r002 (active classic) both
advance cursor 29 to 30 even with the mixed-origin viewport. This falsifies
the initial hypothesis that mismatched coordinates alone explain the loss.
The active r003 four-case fixture separates the missing prerequisite:

| Mode / incoming viewport | Before cursor | After CRLF cursor / viewport |
| --- | --- | --- |
| classic / rows 0..27 | 0,29 | 0,30 / rows 3..30 |
| classic / rows 2..29 | 0,29 | 0,30 / rows 3..30 |
| VT / rows 0..27 | 0,29 | 0,28 / rows 1..28 |
| VT / rows 2..29 | 0,29 | 0,30 / rows 3..30 |

This reproduces the real native target's erroneous cursor transition without
CMD, broker, frontend painting or timing. It supports a project-added seed
coordinate defect interacting with normal Windows VT output, not an original
guest defect or missing completion wait. It does not establish every other
rapid-launch observation's cause. Do not disable VT, compensate rows, add
Sleep or force redraw to hide it.

Source boundary: console_channel.c gives both channels the root's
logical_window. DOS projection makes this origin-relative (0..27), while
canonical cursor/storage remain absolute. console_frontend.c SCREEN_INFO
replaces only srWindow with that logical rectangle. NTVWM presentation_seed
then applies that mixed snapshot to its actual Console before target launch.
The owner subsequently approved this bounded adjacent repair within S10;
CURRENT and the working checklist now include it before implementation. Preserve
DOS logical geometry, current grid/cursor and VT mode; review the native
snapshot origin under its existing ownership/snapshot lock.

Complete cancellation/error/reset sweep, actual rapid output/interaction
acceptance, retained full runtime gates, coherent eight-file publication and
P delivery remain **not completed**. O:/winnt stays on verified S9.
This was the checkpoint before the subsequently approved coordinate repair;
the final delivery below supersedes that pending status, not its test history.

## Approved coordinate repair and follow-up

The existing NTCON io_lock serializes apply_binding and the native snapshot
barrier. Native activation now rebases the logical rectangle to the canonical
viewport origin before publishing the native owner. Width/height remain unchanged;
the real canonical cells, cursor, VT mode and physical buffer are not rewritten.
DOS activation continues to prepare its zero-origin surface. No mirror or wire
change, old CMD snapshot restoration, polling change or new process is involved.

console-origin-r005.txt passes the real private Console API fixture: canonical
120x100, physical viewport (4,2)..(83,29), absolute cursor (4,29), DOS-local
80x28 seed. Production native binding yields a consistent absolute snapshot;
grid/mode/physical geometry remain unchanged and real VT CRLF reaches row30.
Ordinary teardown preserves that final cursor. Existing 80x25/80x28 handoffs pass.

rapid-coordinate-r004.txt passes twelve immediate native/DOS pairs, MEM output,
S10-NATIVE-FINAL, S10-RAPID-COMPLETE and actual outer CMD exit19. It uses only
temporary Z: and removes it after the real service retires. The strict assertion
is unchanged. r003 used the known unsupported long package path and failed DOS
startup with outer82; it is not a coordinate-repair result or acceptance pass.

Notification extension r009 passes all seven cases, including SetEvent and
ResetEvent access-denied faults. A test-only read-only duplicated event drives
the actual production error paths: the root becomes closing and acquisition
wakes without granting stale root handles. It does not replace production policy.

remaining-gates-r002.log passes nine real RPC cases, native-command/native-worker
and five GUI cases. Its later classification invocation lacked required image
arguments and failed; the runner was corrected, not the production classifier.
The cached Console lifetime link also lacked common-transport.lib; the tracked
Ninja generator now declares that existing dependency, with a fresh explicit
build-only link for current testing. Affected full runtime gates remain pending.

coordinate-regression-r002.log subsequently confirms classification227/0,
NTVWM execution lifetime1073/0, completed48/cancelled16, target survival and
remaining-handles0; the private full Console channel fixture also exits0.
Console17/Window17 and retained runtime gates continue under that same runner;
their terminal result, not partial PASS lines, is required for acceptance.
Documentation governance, relative links and git diff --check pass after the
packet update. Candidate-manifest-r002 pins the coordinate-repaired build package;
O:/winnt, commit/push and S10 closure remain pending.

## Adjacent notification/ownership sweep

| Mechanism | Current owner and why it remains separate |
| --- | --- |
| frontend_capability | NTSRV helper alone Set/Resets under service lock; undecided join, caller request marker and unattached live native route are its complete actionable predicate. |
| Join/route rundown | service_clear_frontend clears borrowed channels, deletes delivered/native routes, retains a cancelled DOS identity only for its original waiter, and refreshes the affected root after removal. Disconnect removes the caller from the connection list before refreshing its cancelled join. |
| Attach/delivery | Root authentication precedes mutation; AttachFrontendRequest clears its caller marker then refreshes. Attached routes have a pipe and are not pending work; taking a route changes pipe-present to delivered, neither is an actionable pending route. |
| frontend_changed CV | Existing acquisition predicate/recheck holds the service lock; decided joins awaiting an old lease are excluded from pump work, not deprived of their lease notification. |
| retire/restored | Independent Console lease acknowledgment. NTCON parks/restores before LeaseReady; its pump resets retire before publishing that acknowledgment. Receipt completion cannot substitute for restoration. |
| frontend_state/lifetime_changed | Existing broker-owned state-change notifications and monotonic shutdown decisions, not another partially-reset work queue. No manual ResetEvent in service_core/lifecycle for the auto-reset lifetime event. |
| NTCON pump | One owner lists channels and processes joins before channel requests; wait-set includes notification, stop, retire, creator, Console anchor and state changes. NTSRV retirement is checked first. |
| NTVWM admission_ready | Membership lock owns presentation/presenting and the reset/set transitions. begin_io rechecks under that same lock and waits on ready, stop and thread death with the existing admission deadline. |
| NTVWM idle/receipt | Execution lock owns active count and idle transition. Direct Windows completion and its broker-failure latch remain independent; neither becomes frontend authentication work. |
| Input/OVERLAPPED | NTCON io_lock owns unsent input and readiness. common pipe_transfer resets only the exclusively-owned operation event before issuing I/O, drains pending cancellation before release, and preserves explicit completion/death priority. |

No additional same-class partial predicate/reset defect was established by this
sweep. Existing capture retries/30ms presentation polling remain explicitly
outside this S; they are neither introduced nor disguised as event-driven here.
The passing fault/receipt/lease/input/isolation gates constrain these retained
contracts; source review is not a blanket guarantee against all future races.

coordinate-regression-r002 now terminates with exit0: Console17/Window17,
modern EDIT return, outer CMD relaunch, independent Console/worker close and
all four NTVDM/NTVWM worker/frontend-loss retirement cases pass. The strict
rapid-interactive-r001 probe also terminates0: twelve /K CMD targets consume
queued exit lines without per-line delay, each launcher returns before the
next launch, final native/outer markers survive and outer CMD returns19.

The first concurrent-native-r001 attempt completed every one of its eight
direct targets with exit0, but its pre-first-CreateProcess handle baseline
assertion failed (probe86). It remains a non-pass. Follow-up adds explicit
counts and one actual initial pair as warm-up, retaining exact equality across
four subsequent measured pairs rather than accepting a leak allowance.

concurrent-native-r002 terminates0: one warm-up pair plus four measured
same-Console concurrent pairs, each real target/launcher exit0 and exact
probe handle count89 -> 89. Version negatives reject all five app/protocol/
legacy-interface variants before task delivery. WOW retains actual WINMINE
window and SOL/WRITE memory-dialog frontiers, not three gameplay passes.
gui-coordinate-r003 passes all five GUI cases against the final NTCON repair,
including carrier retirement while GUI survives and text -> GUI -> text.

Final incremental x86 products and the corrected Console lifetime Ninja target
build successfully; unchanged products are reused by identity. The frozen
release-source-manifest-r003 covers139 source/test/tool inputs and the eight
candidate files match their build-cache products. Documentation and relative
links pass. No mvdm/opennt-host mirror diff exists.

The eight x86 files are now published to O:/winnt, with all candidate hashes
verified. accepted-s9-recovery and its manifest retain the prior eight files;
guest/configuration were not overwritten. Postpublication smoke and reviewed
P delivery remain pending; publication alone is not S10 closure.

## Final delivery

postpublication-r001 terminates0 against the actual O:/winnt package:
twelve immediate DOS/native pairs, twelve interactive CMD exit/relaunches,
same outer CMD DOS -> native -> DOS -> cooked exit19, GUI startup exit0 and
--wait actual exit37. Its final eight-file identity check passes. The published
manifest pins the exact x86 /MT CCPU40 APP0.0.424 protocol/RPC33 files; the
S9 recovery remains under build/M0-T424/S10/r001/accepted-s9-recovery.

Production changes are limited to NTSRV's private notification helper/call
sites and NTCON's native ownership coordinate rebase. The displaced partial
idle reset is removed; no second queue, scheduler, worker registry or transport
was introduced. The build generator gains the existing common-transport test
dependency; tests exercise real service/Console and real target completion.
No original mirror, guest, shared lib, registry, ABI or startup syntax change.

Reproduction/test entrypoints:

- frontend-notification-r009.exe --frontend-notification: seven production
  service cases; unchanged S9's baseline interleaving fails before repair.
- console-channel-r005.exe --private-desktop <report>: real coordinate/grid/
  cursor/VT test; --private-desktop-full retains channel/fault/resource tests.
- tests/observation/verify-frontend-rapid-relaunch.ps1 and
  verify-frontend-rapid-interactive.ps1: no launch/line-delay masking.
- coordinate-regression-r002: classifier227, execution lifetime1073/zero
  remaining handles, Console17/Window17, EDIT, relaunch/isolation and four
  worker/frontend fault cases. concurrent-native-r002: four measured pairs,
  all direct exits0 and exact89 -> 89 probe handles after one initial pair.
- remaining-gates-r002: nine RPC plus native-command/native-worker and GUI;
  its later harness argument failure is retained as a non-pass, followed by
  the corrected classification/lifetime runner. gui-coordinate-r003 reruns
  all five GUI cases against the final source, and versions-wow-r001 passes
  all five version negatives while retaining the known WOW frontiers.

Frozen139 source/test/tool hashes and eight candidate/published hashes have
been reviewed. Governance, relative links, diff checks and origin ownership
are required again on the containing P. That P supplies commit/push and clean
main; S10 reaches its bounded conclusion, not T424 completion or a promise
that every possible hang is repaired. S11 frontend naming is not admitted here.
Desktop/RDP manual observations, existing presentation polling and the legacy
incorrect-root setup fixtures remain explicitly unclaimed; their assertions
were not weakened into successful results.

## Reopened S10: Terminal DIR prompt overwrites directory cells

The owner's retained O:/winnt scene was read without screen/input mutation.
Snapshot `build/M0-T424/S10/r001/reported-live-21164.txt` reports an actual
80x28 Console buffer with cursor (9,13); the DOS prompt overwrites QBASIC.HLP
while later directory entries and the file total remain below it. This is
buffer/cursor corruption, not merely a Terminal-painted cursor mismatch.

The test-only production receiver records the causal sequence in
`dir-geometry-z.txt`: native output has 80x30 storage; WINDOW_RECT (0,0,79,27)
shrinks it to 80x28 on ConPTY. The next capture grows storage to 30, but its
WINDOW_RECT (0,2,79,29) shrinks it again. Cell publication still includes row
28. The real cursor commit then fails with ERROR_INVALID_PARAMETER (87).
DOS resume inherits the old cursor, and the next stream prompt overwrites
directory output. The independent inactive-buffer API probe does not resize
the canonical buffer; shadow creation alone is not the cause.

Repair boundary: project-added NTCON receiver only. Native channels mark
their canonical output as a projected viewport. Copied worker WINDOW_RECT
updates logical metadata, not the host viewport/storage. Explicit BUFFER_SIZE
remains authoritative and first fits the physical viewport when storage must
shrink. DOS channels still apply geometry to their private logical surface.
Original cell-grid resize, task execution and completion remain unchanged.
No terminal-brand detection, fixed-row offset, retry, delay, forced repaint,
guest/mirror/shared-library edit, protocol change or history promise.

Confirmed targeted checks:

- x86 /MT incremental NTCON and test links pass, retaining existing warnings.
- `console-channel-lifetime-test.exe --private-desktop-full channel-dir-fixed.txt`
  exits 0: projected viewport preserves row-29 cells/cursor; explicit native
  shrink to 25 remains valid; retained channel/teardown/resource cases execute.
- `terminal-observer-strict.exe 80 30 dir-strict-fixed.raw --s34-full-dir`
  with TEST_RUNTIME_ROOT=Z:/, DIRECT_CMD=1 and REPEAT_DIR=1 exits 0.
  Assertions now require actual directory completion, a clean cursor/prompt
  below the directory summary, and fail rather than pass on timeout.
- Recorded candidate order in `dir-geometry-fixed.txt`: native storage remains
  30; cursor (0,29) succeeds; DOS resume cursor is (0,27). Prompt is at row 27,
  summary above it, and visible rows 28/29 blank. This is logical DOS 28-row
  projection, not a requirement that Terminal itself shrink to 28 rows.

Eight-file candidate published to O:/winnt with hash equality in
`dir-repair-published-manifest.json`; only NTCON differs from delivered S10.
Recovery is `accepted-s10-dir-recovery`. Guest/config files were not replaced.
Console17 passes (`t424-s10-console17-dir-r004-*`). Window17 r004 fails its
native-zero case before scripted Window delivery: CAF helper reports failure,
the target remains at the DOS prompt, and the observer terminates it with its
test-only timeout code 0x53504354. This is not a native target exit code and is
not counted as a pass or attributed to the repair without further evidence.
Standalone Window r005 stops at the foreign-package guard because the owner's
published O:/winnt manual session owns BaseSrv; no process is changed by that
attempt. Broad retained regression and P closure remain pending. The owner
desktop Terminal observation is not replaced by the isolated ConPTY probe.

### Reopened packet final delivery

Window17 `t424-s10-window17-dir-r006-*` passes all seventeen unchanged cases,
including the earlier failed native-zero/CAF case. The r004 failure is retained,
not relabelled as passed or proved to be a production regression. Retained
`dir-r006` checks pass actual modern EDIT screen/Ctrl+Q/CMD echo/DOS MEM,
same-Console DOS/native relaunch and outer cooked CMD exit 19, two independent
sessions with surviving session exit 23, and all four worker/frontend loss
cases with broker-ordered retirement, failed direct receipt and empty-service
retirement. Together with Console17 r004 and the strict repeated-DIR/geometry
fixture this closes the reopened bounded repair. No test assertion was weakened.
The DIR-repair publication manifest remains the eight-file product identity;
only the frontend changed from the preceding S10 delivery. The containing P
supplies reviewed commit/push. Owner next approves S11 naming, then a stop;
T424 remains open and S12 is not admitted.
