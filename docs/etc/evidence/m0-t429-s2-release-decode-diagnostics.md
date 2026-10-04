# M0 T429 S2 release DECODE diagnostic selection

## Question, source and bounded implementation

Does project-added per-instruction attribution impose avoidable work on the
normal selected CCPU40 executor? S1 establishes the ordinary runtime baseline
and actual EDIT/queue/video measurement. S2 tests only that diagnostic cost;
it does not alter instruction semantics or speculative transport policy.

`git blame` attributes the entire DECODE observer block to project806598b9b1
(2026-09-24), not original OpenNT. Selected original c_main.c is byte-identical
in OpenNT/base/mvdm and OpenNT-4.5/nt/private/mvdm; SHA256
F6CD45A30B6575D38EF95083601594F1403A34EBEF1278DBB8362C19F4A14700.
Original DECODE fetches the opcode without this diagnostic block. Read-only
SoftPC comparison likewise lacks it (S1 evidence); no source or runtime dependency
is imported from that project.

Recovery ladder: original opcode fetch/dispatch is already selected and remains
in place. No missing historical dependency requires an adapter or replacement
CPU algorithm here. The minimal existing-mirror selection is registered as
MVDM-HOST-DIV-325: compile the existing project transition/four WOW instruction
observers only with explicit MVDM_CCPU_DECODE_DIAGNOSTICS. Normal flags do not
define it. No replacement per-instruction call/branch, guest/firmware, scheduler,
frame dedup, input policy, lifecycle, ABI or wire change is introduced.
Coarse diagnostic interfaces remain unchanged in the existing worker adapter.

## Build and object proof

The validated MSVC14.43/SDK22621/x86 /MT CCPU40 cache is
build/M0-T427/S2/r001. Its run-ninja-parallel.cmd ntvdm.exe rebuilds only
c_main.obj, original-ccpu386.lib and the actual final worker. VDM_TIB ownership
passes; existing mirror declaration/conversion warnings remain. Other EXEs/DLLs
are unchanged. S2/r002/runtime copies the sealed T428 S6/r002 package and replaces
only ntvdm.exe. It was initially isolated; the owner-requested early publication
below subsequently installs this exact candidate without claiming P closure.

`tests/observation/verify-decode-diagnostics.ps1 -BuildRoot <fresh-build-root>`
uses the exact selected CCPU flags and inspects actual normal and independently
compiled diagnostic objects. It rejects normal diagnostic selection, absent or
unexpected symbol references, and any change to the accepted diagnostic body.
S2/r001 object inspection passes all five absent-normal/present-diagnostic
assertions. The subsequent strengthened source/body assertion is separately
rerun before delivery; the initial run is not retroactively credited with it.

Normal object SHA256 B05BDE2076F7EC07345594CA96D8CE9E3660BDE78758F7B669C2E69DF7BFB13D;
diagnostic D391118874B967DCFD85D09BEADE5C0DB6BE8105254B7044A1DB39875067DBC1.
The diagnostic compile changes only its explicit macro, not compiler optimization
flags. No explicit /O flag is added to this repair. r001/manifest.json records
graph, source, flags and compiler-environment overrides.

## Runtime comparison and pending delivery

`compare-execution-performance.ps1` pins comparable original/candidate media
and binary hashes, allowing only ntvdm.exe to differ. Both use r012 S1 observer,
private desktop, ordinary geometry, the same COMMAND/EDIT/menu/MEM and actual
direct-exit assertions. It alternates pair ordering, excludes explicit warmup,
retains three samples per Console/Window empty/EDIT/native-exit7 case, and uses
identity-scoped cleanup with only temporary Z:. Sink acceptance is not completion.
S2/r003 was interrupted for owner-requested publication; S2/r008 is the
replacement serial comparison. Wall time includes test work;
phase times, native control and run-order variability must constrain inference.

The normal object repair is implemented and linked, not yet a delivered P.
Actual comparison, measured EDIT200, full Console17/Window17/retained WOW and
lifecycle gates, publication, published smoke and final review remain pending.
No speedup or physical/RDP mouse improvement is claimed from compilation.
Matched SoftPC runtime comparison remains unperformed, not passed. T429 stays open.

## Owner-requested early publication

Owner directs “你先发布吧” while r003 is running. Its Console empty/EDIT
and native-exit7 cases already pass; Window comparison is incomplete. The
background comparison is stopped and its package processes cleaned by exact
image/PID/creation identity. Its cleanup reports SUBST already removed following
the deliberate external stop; this interrupted run is not a full comparison pass.
The first cleanup attempt rejected a dual-package Z: alias hash, made no product
change, and was replaced with exact owned-path process cleanup. Z: is removed.

S2/r006 passes the strengthened accepted diagnostic-body and both object checks.
S2/r004 test-only measured worker/frontend links pass; they are not published.
S2/r007/publish.ps1 saves the coherent previous eight files and hashes, checks
x86/current-link/unchanged-seven identity, publishes S2/r002 to O:/winnt/system32
and verifies all eight hashes. NTVDM SHA256:
6A6701938052864407D07F5E85FC69B8BA0C534A93DD162883D134EB948E6F54.
Recovery is r007/recovery. Guest/configuration are unchanged.

This is the owner's explicit early candidate-testing exception, not production
P/S2 closure. publication-limit.json records incomplete full comparison, EDIT200,
Console17/Window17/WOW/lifecycle, deployed smoke and reviewed commit/push.
No statistical speedup or physical mouse improvement is asserted from this
partial run. Continue those gates before delivery; preserve the owner-testable
package and other-session proposal modifications.

## Post-publication verification in progress

The deployed eight hashes are independently rechecked against r002; all match.
The incremental `run-ninja-parallel.cmd ntvdm.exe` reports no work to do,
confirming the selected affected closure is current. Baseline/candidate
SYSTEM.INI SHA256 is identical:
F20A94235712C26B0499D61E7CC080B612BBFE230DE0C65A6832494FDBB1ACAA.
The strengthened r006 diagnostic object has SHA256
2D2F2F53B9A2124AA25C03F835217BC61844B67EB58E2B6A87D1F75C78838269;
its manifest proves the unchanged accepted attribution body and 0/5 references.
The earlier diagnostic hash above belongs to r001, not r006.

`pwsh -File tests/observation/verify-service-fixtures.ps1 -Observer
build/M0-T427/S4/r049/console-startup-observer.exe -Fixture
build/M0-T427/S2/r001/basesrv-service-reservation-test.exe -LogRoot
build/M0-T429/S2/r012 -Concurrency 4` passes all 29 retained production-archive
service assertions in 4595ms. These fixtures do not bind the global endpoint;
they run alongside, not concurrently with, another complete product matrix.
Retained `console-video-test.exe`, `frontend-window-mouse-test.exe` and
`worker-shutdown-test.exe` also pass. Shutdown reports 37 assertions,
zero failures and 91 handles both before and after. These unchanged-provider
fixtures are supplemental mechanism evidence, not proof of new CCPU execution.

The r008 comparison subsequently finishes with actual exit0 and all48 cases:
two modes, three workloads, one excluded warmup plus three paired samples.
No retry-to-pass or outlier removal occurs. Wall-clock medians include all test
input/menu/cleanup work and are not isolated CPU execution timings.

| Workload | Console baseline/candidate median(ms) | Window baseline/candidate median(ms) |
| --- | --- | --- |
| COMMAND then exit | 3956 / 3866 | 4353 / 4418 |
| COMMAND -> EDIT -> menu exit -> MEM -> exit | 17912 / 16146 | 8249 / 8218 |
| Native direct exit7 control | 2223 / 2230 | 2169 / 2185 |

Console EDIT has one slower candidate sample19483ms versus its paired
baseline18331ms; the maximum is not an improvement. The first EDIT Untitled
observation after command submission has baseline/candidate median2140/1953ms
in Console and1235/1188ms in Window. Startup medians are2344/2328ms and
2266/2360ms respectively. Thus Console aggregate time improves about9.9%,
but Window is essentially unchanged and three samples do not establish a
stable universal speedup. The proven release hot-loop cost is removed; the
remaining Window delay is not explained by that cost alone.

`powershell -File tests/observation/measure-worker-boundaries.ps1
-MeasuredWorker build/M0-T429/S2/r004/ntvdm-performance-observer.exe
-MeasuredFrontend build/M0-T429/S2/r004/ntcon-performance-observer.exe
-Observer build/M0-T429/S1/r012/console-startup-observer.exe
-ReportRoot build/M0-T429/S2/r009 -Edit -Iterations 3` finishes exit0:
disabled control plus three enabled actual EDIT200 runs, menu exit, MEM and
direct receipt all pass. Every measured queued record is consumed; merge
accounting, no overflow, final handoff/close acknowledgements and frontend
decode/present assertions pass. Queue peak17, age medians4.525--5.562ms,
max62.771--76.870ms. This is the real Window input sink and DOS guest, not
physical RawInput/RDP. It is not a claim that capture motion is fully smooth.

Successful text-transfer medians remain74.666/105.201/116.845ms (17--18 frames
per sample), while assembly medians40/45/47us and frontend presentation
medians70/73/76us. The synchronous transaction includes more than the named
pipe bytes. Its cause needs finer attribution before S3 changes production;
do not infer that transport itself or an enlarged input queue is the remedy.

The first complete-product attempt r011 fails before launching WOW because
Windows PowerShell5 lacks ProcessStartInfo.ArgumentList. PowerShell7 retry
then fails the existing identity-scoped WMI prerequisite with Access denied,
before creating r013. The same read-only query succeeds outside the sandbox;
r013 is consequently running there with the intended PowerShell7 runner.
Neither failed launch attempt is product acceptance or a product regression.
The corrected `pwsh -File tools/audit/Invoke-ProductVerification.ps1
-RuntimeRoot build/M0-T429/S2/r002/runtime -BuildCache build/M0-T427/S2/r001
-Observer build/M0-T427/S4/r049/console-startup-observer.exe
-WindowObserver build/M0-T427/S4/r049/worker-window-snapshot.exe
-LogRoot build/M0-T429/S2/r013 -Suite Product
-GuestFixture build/M0-T425/S9/r008/G7.COM
-WowBaselineRoots build/M0-T428/S6/r007` finishes exit0 outside the sandbox.
All Console17 and Window17 retained output/exit/order assertions pass, with
the same eight hashes throughout. All three WOW frontiers match the preceding
package: WINMINE's created interface and SOL/WRITE's retained memory-error
frontiers remain alive at the final sample. This is frontier nonregression,
not new gameplay or SOL/WRITE usability. Total211413ms; WOW67003ms,
Console64745ms, Window74943ms, with separately recorded cleanup in timings.json.
Lifecycle and deployed smoke remain open at this checkpoint.

Both selected upstream c_main.c counterparts are independently compared
bytewise and after newline normalization. Both comparisons differ from the
current mirror, which retains earlier registered DIV072/073/214/221/282 and
new DIV325. Upstream copies are byte-identical to one another. The new diff
against the accepted project source is5 added/4 removed lines, confined to
comments and the compile guard; the diagnostic body is unchanged. No
format-only upstream equality is present or claimed.

## Completed smoke and open native-close evidence

r015 rechecks retained console-video, frontend-window-mouse and worker-shutdown
fixtures; all pass. Shutdown reports37 assertions and86 handles before/after
in this run. Each run preserves its own before/after handle count.

r016 runs the published O:/winnt package, checking all eight against r007's
publication manifest before and after Console/Window empty, native-zero, MEM
and EDIT smoke. All actual output/exit assertions pass. Its
verified-published-manifest.json is under build/M0-T429/S2/r016; observation
is under O:/winnt/Logs2 with t429-s2-r016 prefixes. This supplements rather
than substitutes for the completed r013 full product suite.

r014's serial management tests pass broker/frontend loss for both worker
kinds, selected native-worker close with a surviving independent Console,
and unexpected worker loss returning1067 without killing its CMD. Its last
explicit frontend-close case fails despite an accepted close RPC. Test cleanup
does not prove normal retirement. r017's single baseline control passes but
cannot erase that failure.

r018 runs exactly three alternating baseline/candidate pairs through
verify-ntvwm-management.ps1 -FrontendClose, fresh isolated Z: packages and
pinned owned cleanup. No case is retried or excluded. All three candidate
samples and two baseline samples pass; baseline-3 fails. The new test-only
close-state log, captured before cleanup, records frontend15140 and worker30664
already exited, worker result1067, while target5412 is alive. The launcher
also actually returned1067. Aggregate r018 exits1, not a pass.

All seven binaries besides NTVDM are identical between baseline and candidate.
This native-CMD scenario does not execute CCPU. The reproduced baseline
failure establishes an existing native close limitation, not a DECODE-removal
regression. It does not establish the exact failure cause. The fixture acts
after target creation, not proven interactive readiness; worker transport
loss can precede broker shutdown. Neither possibility justifies a longer
wait, weakened assertion or speculative lifecycle patch.

The fixture change only adds pinned participant/exit-state capture on failure;
all timeouts, exit-code, close and independent-session assertions remain.
S2 production P stays pending while this evidence is reviewed. The published
owner-testable package remains coherent and unchanged.

r019 runs a fixed four-sample accepted-baseline diagnostic, using the existing
NTVWM_GEOMETRY_ERROR_LOG only; assertions and deadlines are unchanged. One
sample fails and three pass, aggregate exit1. In the failed sample worker2108
logs exchange BARRIER(11) cancelled995, end-io995, then broker-complete1067.
Worker and frontend are dead while direct CMD5448 remains alive. The initial
READ_TEXT_CONFIGURATION(39) error1168 also occurs in all passing samples;
it is not a discriminator and must not be presented as the cause.

Source-level ordering relevant to this failure is concrete: main's GetNext
exit sets membership.quit and ntvwm_executions_close signals owner.stop;
serve can leave its target/stop wait without target completion, call end_io,
then note_broker_failure1067. broker_completion_fault immediately terminates
the worker without waiting for the presentation thread's acknowledged Console
close. Thus teardown cancellation can take the intentional infrastructure-
fault exit path before the shutdown watcher completes Console-session close.
The observed log is consistent with this race, but does not timestamp every
participant or prove which thread won. Retain that qualification; do not
convert this performance S into a native lifecycle repair without admission.

## Finer unchanged-path attribution

The existing test-only frontend_session_measured.c now wraps the actual video
commit and Console buffer create/read/write/window/resize APIs, preserving
arguments, return values and GetLastError. Production source, protocol and
the published package are unchanged. No filtering is added. r020/r022 build
through build-worker-performance.ps1, with pinned inputs and VdmTib proof.

r021 runs measure-worker-boundaries.ps1 -Edit -Iterations1 with r020 measured
worker/frontend and S1/r012 observer. Disabled control and enabled EDIT200 pass
unchanged menu/MEM/receipt/input-conservation/close assertions, with no report
overflow. Successful worker-transfer median93.633ms matches frontend video-
commit93.358ms. Buffer creation median95us and resize122us do not explain it.
API aggregate statistics also include startup/history copies; do not divide
all API totals by frame count or mistake the transaction for pipe-only cost.

Source review finds GetConsoleOutputCP inside prepare_text_frame's per-cell
loop. r022 adds per-thread test-only aggregate timing of that API within each
publication, recording one sum rather than thousands of samples. This total
is disjoint-call duration, not a continuous phase interval; do not add it to
the enclosing commit time. r023 repeats the disabled/enabled EDIT200 contract:
both pass, overflow0. Eighteen commits total1828662us; code-page queries total
1725656us (94.36%),39120 calls across current80x28 and80x25 frames. Median
commit102459us; median code-page total97557us. One measured run establishes
attribution, not a stable production speedup. Test wrappers are not published.

The narrow next hypothesis is one current code-page snapshot per frame, not
one host query per character. Preserve glyph/attribute/style conversion,
explicit frames, geometry, ownership and commit acknowledgement. Do not change
transport, add filtering or permanently cache code pages. These timings do not
prove physical RDP/IRQ smoothness. S3 requires separate admission and measured
before/after verification. S2 still awaits the native-close disposition.

r024 builds worker_performance_test.c with the r022 measured accumulator and
the selected cached mvdm_softpc_mouse_input.obj. Modes disabled/enabled/total
all pass: disabled writes no report, input displacement stays200, GetLastError
retains0x53510002, and total records exactly1000us/count17/error0. The first
unit-build attempt incorrectly used global /TC, making CL parse input .obj
files as C; it failed before running a fixture. Removing that harness-only
compile switch fixes the build, without changing source or production input.
The corrected command is retained in build/M0-T429/S2/r024/check-total.ps1.

Post-measurement governance, relative links and git diff --check pass. Z: is
released, and published NTVDM/NTCON hashes still match r007/r016. No measured
EXE replaces a deployed component. Other-session proposal changes remain
preserved and outside this delivery.

## Approved native-close dependency repair

Owner approves the reproduced close-ordering defect as a bounded S2 dependency
repair. No retirement policy, target receipt, original mirror, protocol or
timer is changed. NTSRV GetNextNativeCommand checks the shutdown event before
returning ERROR_CANCELLED. In NTVWM, that return could instead set quit and
cancel serving threads; their unfinished target fault then terminated the
carrier before its presentation watcher performed the already ordered Console
close. A final-I/O failure could race the same watcher. The earlier r018/r019
failures remain evidence, not erased by subsequent successes.

main.c now checks existing shutdown/management-stop events before generic
GetNext teardown, completion-fault termination and presentation-pump teardown.
An issued close uses the existing I/O lock and worker_base_shutdown_close:
actual Console close, successful close acknowledgement, then carrier exit.
No signal means the previous unexpected-fault path remains unchanged. No new
event, delay, retry, scheduler or process-tree kill is introduced.

The selected x86 incremental build compiles main.c and relinks ntvwm.exe only.
The first sandbox build stalled before compilation; its exact owned Ninja
instance was ended and the same build completed outside the sandbox. Fresh
r025/runtime copies immutable r002 and replaces only NTVWM. Four fixed real
frontend-close samples all pass; each direct CMD exits with CTRL_CLOSE status.
Unexpected worker-loss still returns1067 while CMD survives. Independent
Console isolation passes: closing one worker leaves the other able to accept
new input and return23. All samples are retained, not retries until success.

ntvwm_close_priority_test.c includes the production priority decision with only
the worker-local Console operation replaced by a test event. No close
instruction leaves close/ack unsignalled and returns the infrastructure failure;
shutdown and management-stop each close and acknowledge even with quit already
signalled. Three cases pass. Existing worker_shutdown_test passes37 assertions,
handle count112 before/after. Initial fixture compilation incorrectly reused
CCPU flags and lacked native_pc_font.h; the retained build-only Ninja fragment
then used NTVWM's actual flags/includes, and the corrected build is warning-free.
This fixture proves the priority decision, not all possible thread schedules.
The fixed real samples complement it but do not prove absence of every race.

r026 completes exit0 against r025/runtime: Console17, Window17 and each
retained WOW frontier pass. Total219417ms, WOW67414ms, Console66485ms,
Window80681ms; individual cleanup and preparation are in timings.json.
Same assertions, original media, software FULLSCREEN and no frame dedup are
preserved. This rerun proves nonregression, not a new performance benchmark.
The one-factor r008 performance comparison predates the close dependency
repair; it cannot be relabelled as a benchmark of the final two-change package.

r027 backs up the prior eight files, checks x86 and immutable input identity,
stops only exact published image/creation identities, publishes all eight
and verifies every destination hash. NTVDM remains
6A6701938052864407D07F5E85FC69B8BA0C534A93DD162883D134EB948E6F54;
NTVWM becomes
E6C8CB50AFCCB082E732992B7DBAEB9DB165DF000D2FCEA01E94C2AF2DDE90E0.
Other six binaries remain byte-identical to accepted T428. Complete manifest
and recovery hashes are under r027; guest/config inputs are unchanged.
r028 finishes exit0: published Console/Window COMMAND, MEM, EDIT and native-zero
output/receipt/exit smoke pass, and all eight hashes still match. No measured
wrapper or diagnostic object is published. Z: is released.

## S2 closure review

Release DECODE excludes all five diagnostic references with no replacement
per-instruction branch; explicit diagnostic compilation retains the unchanged
observer body. Proven original instruction semantics remain in their mirror.
Matched performance evidence is limited to the reported workloads/distributions,
not physical RDP smoothness or a universal speedup. The approved native-close
dependency uses already ordered Console closure before cancellation/fault
teardown, without target-tree killing or fabricated completion.

Affected x86 links, normal/diagnostic objects, measured units, EDIT200,
service29, video/input/shutdown fixtures, fixed real close and isolation cases,
full coherent Product and published smoke pass. Governance, relative links
and diff review are required again before sequential commit/push. Other-session
proposal chronology is preserved and excluded. T429 remains open; the next
bounded repair hypothesis is a current code-page query once per text frame.
