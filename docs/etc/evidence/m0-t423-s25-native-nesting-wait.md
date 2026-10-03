# M0 T423 S25 — Native Nesting and Wait Evidence

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Question and baseline

Why does the S24-accepted direct NTW32 package pass sequential native reuse
but time out when a CMD batch starts another `run16 cmd` before its own CMD
exits? The baseline is the owner-accepted protocol-23 seven-file package;
the S24 evidence records its hashes and passing direct tests. This record
does not change or invalidate that bounded acceptance.

## Inputs and procedure

The retained `build/M0-T423/S24-helperless-clean/native-reuse.cmd` launches
three inner `run16 cmd /c` commands sequentially, checking their return codes.
The private-desktop Console observer starts `run16 cmd /d /c` with that batch
and captures a screen, timeout process snapshot and process exit. The S25
reproduction uses a copied accepted package under `build/M0-T423/S25/` and a
temporary short drive mapping. The owner subsequently required **Z:** as the
only such mapping; it was removed after the test. No guest file was modified.
The long-path setup returned 5 even for `cmd /c ver` and is a known separate
limitation; it is not used to diagnose native nesting.

## Observations

- Short-path `run16 cmd /d /c ver` returned 0 and displayed the Windows
  version text. This controls for package startup and visible output.
- Short-path `run16 cmd /d /c native-reuse.cmd` timed out after 30 and then
  45 seconds, with no completed batch text. The latter observer report is
  `build/M0-T423/S25/z-nested-live.txt`; the package remains unmodified.
- At the 45-second run, the outer CMD remained alive, its first inner
  `run16 cmd /c ver` remained alive, and both NTW32 and NTCON remained alive.
  The process snapshot showed outer CMD parented by NTW32 and inner run16
  parented by that CMD. Only test-owned mapped-drive leftovers were ended;
  unrelated installed-package processes were not touched.
- `src/ntw32-exe/main.c` calls `ntw32_executions_wait_idle()` before every
  `worker_base_get_next_command()`. The wait is infinite until all active
  direct serving threads end. Meanwhile the outer CMD waits for the inner
  run16, so its direct serving thread cannot end. The inner request needs
  the same NTW32 to call GetNext. The broker's same-root path already permits
  an in-flight direct record, and NTW32's execution owner already counts
  multiple active requests; the main-loop idle gate is the divergent barrier.

## S25 candidate repair and first proof

The NTW32 main loop no longer waits for all serving threads to become idle
before calling `GetNextNativeCommand`. A second authenticated request may be
taken while an outer CMD is waiting for its inner `run16`; existing frontend
identity checks still reject a different frontend during activity. Broker
completion failure remains fatal to the resident worker even if its main
thread is blocked in the next synchronous GetNext RPC. The request-lifetime
fixture binds a nonterminating fault observer and passed 445 checks with zero
failures, including failure latching and cancellation cleanup.

The same short-path private-desktop `native-reuse.cmd` probe that previously
timed out now returned `exit=0x00000000` in about three seconds. Its visible
Console capture contains both Windows-version lines from the first and third
inner `cmd /c ver`; the intervening `cmd /c exit 37` result was checked by the
batch before it returned zero. Evidence files are
`build/M0-T423/S25/nested-fixed-r6.txt` and its `.console.txt` companion.
The repository's existing `tests/app/native-console-nested.cmd` then passed
from the same isolated package: the outer CMD returned `0x17` (23), the
captured screen displayed `INNER-NATIVE-RETURNED`, and the script itself
checked both an inner exit code 37 and a redirected inner exit code 41.
Its report and screen are `build/M0-T423/S25/nested-fixed-r7.txt` and
`.console.txt`. Neither test required an extra helper or Job observer.
Only the changed NTW32 executable was copied into the **build-only** test
package; `O:/winnt` remains the accepted S24 set. The temporary short drive
for this run was `Z:`. The S25 candidate is not yet a published P: bounded
startup/handoff waits, full x86 and runtime gates, side-session NTCON review,
coherent package publication, and governance remain outstanding.

The first local probes returned error 5 even for the accepted `O:/winnt`
package. A control using the identical accepted package succeeded when run
with normal IPC permissions outside the restricted execution sandbox. The
subsequent Z: candidate test used those permissions. Error 5 is therefore
not counted as a product-code failure or a nested-timeout result.

## Protocol-24 startup wait candidate

S25 added a launcher-private NTSRV worker-state event, separate from NTCON's
auto-reset retirement event. Native worker registration, reservation release,
and worker death signal every waiting launcher's own event. Run16 rechecks
selection/admission after wake, waits on the selected worker and root
frontend death handles, and applies one ten-second deadline to its native
startup handoff. The event is advisory, not task authority; broker records
and authenticated grants still decide admission. Both direct and nested
CMD continued to pass after the protocol-version bump to 24.

A negative probe substituted a live but deliberately non-registering
`ntw32.exe` fixture in a separate build-only package. The observer returned
`exit=0x000005b4` (`ERROR_TIMEOUT`) after about 11.6 seconds including test
setup, rather than hanging or starting a target. Raw report:
`build/M0-T423/S25/protocol24-stuck-live.txt`. The fixture source is
`tests/observation/ntw32_unregistered_worker.c`; it is not in any product
link. The complete protocol-24 native candidate then returned 0 and showed
the Windows version for `cmd /c ver`, and returned `0x17` with
`INNER-NATIVE-RETURNED` for the repository's nested script. Raw reports are
`build/M0-T423/S25/protocol24-simple.txt` and
`build/M0-T423/S25/protocol24-nested.txt`, each with a `.console.txt` file.
After adding bounded NTW32 and NTVDM I/O admission retries, an incremental
x86 rebuild and the NTW32 lifetime fixture passed; the real nested native
test again returned `0x17`, and a copied original DOS package ran
`COMMAND.COM /c MEM` with exit 0 and visible memory figures. The corresponding
reports are `protocol24-bounded-nested.txt` and `protocol24-dos-mem.txt`.

The MSVC x86 `/MT` formal graph is
`build/M0-T423/S25/formal/build.ninja`; its first build completed 586 rules,
then the I/O deadline change completed 8 incremental rules. This is still an
isolated candidate, not the side-test publication. `Z:` was the only temporary
drive mapping and was removed after each test batch. No guest binary was
changed; original guest/configuration files were copied into the build-only
candidate.

## Short-path regression and zero-request resume correction

The owner confirmed that DOS long-path handling is a known separate defect.
All new isolated runtime checks therefore used only the temporary `Z:`
mapping; every command removed it in `finally`. No other temporary drive
letter was used, and no accepted `O:/winnt` binary was replaced.

The protocol-24 candidate passed all 17 default
`Verify-CommandExitStatus.ps1` cases with the ordinary frontend, including
actual COMMAND/MEM/EDIT text, guest exit code 7, nested COMMAND, and native
delegation. The reports use prefix `m0-t423-s25-proto24-dos17-a` under
`O:/winnt/Logs2`. The side-session NTCON viewport restoration was present
in this build; its `console-channel-lifetime-test.exe` passed when started
in its own hidden Console. Direct invocation without a Console returned
`ERROR_INVALID_HANDLE` and is not a product result.

The first headless WOW run reached the retained WINMINE frontier, then its
test-only cleanup raced process shutdown and received access denied. The
test script now ignores that exception only when a fresh exact-PID CIM query
confirms the process is already gone; a live denied cleanup remains fatal.
With that guard, all three frontier observations passed under prefix
`m0-t423-s25-proto24-wow-c`: WINMINE main window, SOL's original
out-of-memory modal, and WRITE's original out-of-memory modal. This is
non-regression, not application functionality acceptance.

Supplemental nesting exposed an additional concrete contract mismatch.
NTSRV `CompleteWorkerChannel` accepts request ID zero for an I/O-resume
channel without a Direct task. The new `worker-base` client and NTW32
execution-start check had rejected zero. The worker's previously ignored
completion then returned 87 (`ERROR_INVALID_PARAMETER`); the S25 explicit
completion-fault policy correctly terminated NTW32, surfacing 1067 to the
launcher. Existing error-only NTW32 tracing captured
`stage=broker-complete error=87` under
`m0-t423-s25-ntw32-repeat-error-b.txt`. The accepted S24 package instead
timed out on the same `native-cmd-dos-repeat` supplemental case. The repair
allows zero through the common completion client and NTW32 I/O-resume
execution path, matching NTSRV's existing contract. The focused
`worker-base-next-command-test` and NTW32 lifetime fixture passed (448
checks, no failures, including three exact zero-request completions); after
the repair, the real
`native-cmd-dos-repeat` case captured two MEM reports and exited zero under
prefix `m0-t423-s25-proto24-zero-resume-a`.

After that change, the full x86 `/MT` graph was incrementally rebuilt. The
service reservation fixture passed its original DOS/WOW and worker-binding
checks. The same isolated candidate passed all 17 ordinary Console cases
again in Window-input mode (prefix
`m0-t423-s25-proto24-window17-a`, all actual input/text/result assertions).
The formal and build-only runtime package hashes match for all seven product
files; the unchanged WOW32 DLL matches the accepted `O:/winnt` copy.
The deliberately live but non-registering NTW32 negative was repeated after
the zero-request change: report `m0-t423-s25-proto24-stuck-after-zero.txt`
records `result=exited`, `exit=0x000005b4` (`ERROR_TIMEOUT`), with no
unbounded startup wait. The observer executable's own zero exit is not used
as the assertion; its child result and the absence of leftover product
processes are the evidence.
The ordinary Console DOS17 gate was also repeated *after* the zero-request
source correction: all 17 actual text/result cases passed under prefix
`m0-t423-s25-proto24-final-console17`. No public package was replaced.

The deeper `dos-native-dos` and `frontend-chain-b` executions first returned
their expected exits (1 and 23) but failed final-screen assertions. Per-line
snapshots proved the earlier text was visible and later overwritten. In the
80-column chain, NTW32 seeded a scrolled DOS page at its old viewport origin,
overwriting live native history. A focused presentation case reproduced this
with an A-H page followed by a C-J page: A and B were lost. The candidate now
aligns a trustworthy, nonblank prefix of the returned page with native
history; without such an overlap it retains the old placement rule. The x86
private-Console fixture passed 414 checks with no failures in
`build/M0-T423/S25/presentation-history-overlap-guarded.log`. The build-only
candidate then passed the real `frontend-chain-b` case: both MEM reports,
both native return lines and actual exit 23 are present in
`O:/winnt/Logs2/m0-t423-s25-overlap-d-frontend-chain-b.txt` and its final
screen. The earlier failing run remains at
`m0-t423-s25-proto24-zero-chain-b-frontend-chain-b.txt`.

`dos-native-dos` is **still failed**: its line 03 snapshot has the original
CMD banner; after the visible frontend reduces its deep buffer to DOS 80x25,
line 04 has already lost that history. Restoring the original buffer shape
at exit cannot restore the lost cells. This also reproduces when the test
Console starts at 80 columns, so it is the loss of retained rows, not only
120-to-80 reflow. This is a distinct frontend geometry/history boundary, not
fixed by NTW32's hidden-Console import. Evidence is
`O:/winnt/Logs2/m0-t423-s25-overlap-c-dos-native-dos.txt` (120 columns) and
`m0-t423-s25-overlap-e-dos-native-dos.txt` (80 columns), with per-line
snapshots. Neither assertion was weakened. The S24 control had
timed out before reaching these screens; no S25 package has been published
or claimed complete while this failure and the remaining gates are reviewed.

## Remaining S25 wait inventory

| Path | Classification and disposition |
| --- | --- |
| Run16 native reserve/register and unaccepted submit | Previously unbounded 10-ms status polling; candidate now uses per-launcher broker transition events and one 10-s deadline. Accepted requests are never replayed. |
| NTW32 `begin_io` | The new candidate removes its 10-ms readiness retry. The existing presentation thread publishes a manual-reset admission event only after it has attached and acquired the frontend; a request waits on that event, its stop handles, the presentation-thread death handle, and the same 10-s deadline. The retained 30-ms hidden-Console observer is still a timed producer and is disclosed separately. Focused real-chain proof passed; broader lost-wake/fault review remains. |
| NTVDM Console activation | Previously unbounded `ERROR_BUSY` 10-ms retry. The current candidate makes one activation request; NTCON waits on a private per-request manual-reset event plus cancellation/root-stop handles, with a 10-s deadline. Ownership transitions signal all registered waiters under the ownership lock. The focused real routes and subsequent 17+17 formal package gates pass; publication/owner acceptance remain separate. |
| NTW32 hidden-Console output pump | Deliberate 30-ms output observer accepted by the owner until a reliable producer signal exists; retain and disclose. |
| NTW32 screen capture retries and Console-close checks | Bounded 8-attempt capture consistency retry and bounded explicit-close observation, not startup readiness polling; retain pending negative-path regression. |
| NTCON synchronous-input cancellation join | Replaced repeated 50-ms cancellation with a single cancel and bounded 10-s join. Hidden-Console normal, 32 cancel/wait races and forced held-lock timeout fixtures pass; a timeout retains borrowed storage for terminal process cleanup. |
| Run16 broker-connect, NTMON refresh, WOW clock | Finite broker-connect retry and deliberate periodic observer/producers; classify and preserve unless a distinct failure is proved. |
| Run16 worker-death completion grace | One 2-second wait for the direct DOS/WOW parent completion after worker death, followed by explicit failure if no completion arrives; bounded failure observation, not a readiness poll. |
| NTVDM mouse-input backpressure | `console_compat.c` yields for 1 ms only when the downstream mouse queue has zero capacity, leaving the raw record queued and returning to the original event loop. This is neither startup readiness nor broker/worker scheduling; retain pending its separate input-pressure contract. |
| Explicit close acknowledgements | NTSRV's 10-s native-Console close and 5-s process-exit waits, plus NTVDM Console client's 5-s close wait, are one-shot bounded shutdown acknowledgements rather than periodic request-selection retries. They remain independent of the S25 10-s startup deadline. |

The supplemental final-screen history failure, coherent `O:/winnt`
publication, governance, and push remain S25 work. Console17, Window17,
retained headless WOW frontiers, the side-session NTCON lifetime fixture,
and the x86 formal graph have passed as stated above; they do not turn the
remaining items into passes.

NTW32's admission event is local to the worker, not a second broker queue or
new frontend protocol. The presentation owner sets it under the membership
lock only after `ntw32_presentation_begin` succeeds, and clears it when that
presentation ends or the route detaches. A request holds its own admission
count while it waits and rechecks the predicate under the same lock after a
wake; no auto-reset event is shared by competing request waiters. The first
isolated x86 source compilation and link succeeded. The focused real cases
`native-zero`, `native-cmd-dos-repeat`, and `frontend-chain-b` passed with
actual output and exits 0, 0, and 23 under prefix
`m0-t423-s25-admission-event-focused` in `O:/winnt/Logs2`. This changed
`ntw32.exe` was copied only to a new build-local package
`build/M0-T423/S25/runtime-admission-event`; `O:/winnt` remains untouched.
The repeated programmatic Ninja invocation stalled before launching a child
compiler process; the exact graph-selected `main.c` compile and link commands
were run directly with the same x86 MSVC environment. A successful Ninja
incremental product build remains required before a production P.

The S25 build audit also found that Ninja's default target compiles only the
original SoftPC library collection; it is not a product build. Its separate
`product-programs` alias omitted both `ntw32.exe` and `ntcon.exe`, so invoking
that alias could falsely suggest that the two-worker product was up to date.
The graph generator now includes both executables in the alias. Regeneration
under `build/M0-T423/S25/formal` showed all seven declared product targets,
and the x86 MSVC `product-programs` target completed successfully. The
unchanged WOW32 DLL is supplied from the previously accepted package and is
not a target in this particular graph. No accepted runtime package was
replaced by this build check.

The seven formally built product files were copied without modification into
the isolated `build/M0-T423/S25/runtime-protocol24` package; each SHA-256
matched its formal-build source. With only that package temporarily mapped to
`Z:` (and the mapping removed in `finally`), the current candidate passed all
17 ordinary Console cases and all 17 Window-input cases, including the actual
COMMAND/MEM/EDIT output and exit-code assertions. The new reports use prefixes
`m0-t423-s25-aliased-console17` and `m0-t423-s25-aliased-window17` under
`O:/winnt/Logs2`. These repetitions are later than the scrollback-alignment
change and the build-graph correction. They do not waive the separate
supplemental history failure or the outstanding wait/teardown work above.

The headless WOW frontier check initially used the wrong fixture mode: the
isolated package has `network.drv=wfwnet.drv`, while the first script invocation
omitted `-PackageNetworkProfile` and therefore expected the deliberately
absent `NETWORK.DRV` modal. Its WINMINE failure was a test invocation error,
not a product finding. Repeating with `-PackageNetworkProfile` passed all
three retained frontiers under `m0-t423-s25-aliased-wow-profile`: WINMINE's
localized main window, SOL's original out-of-memory modal, and WRITE's
original out-of-memory modal. The observer also required no character
frontend. This proves only non-regression at those depths, not full SOL or
WRITE functionality. Each run used only temporary `Z:` and removed it in
`finally`.

## Interpretation and next proof

This was a host-side circular wait, not a guest defect or evidence that the
30-ms hidden-Console output sampler caused the hang. The candidate removes
that wait and has one positive real nested-chain proof, but requires repeated
and fault-path regression before production publication. A finite startup
deadline alone would merely have converted this deadlock into a timeout; it
was not used as the repair.

## DOS activation wait candidate

NTVDM now makes one Console activation request instead of sleeping 10 ms and
retrying `ERROR_BUSY`. The first NTCON candidate used a condition variable
under its `io_lock`; the later cancellation audit replaced it with a private
manual-reset event for each waiting request and a wait set containing that
event, channel cancel and root stop. Ownership transitions set the registered
events under `io_lock`; each waiter resets its own event under the same lock
before waiting outside it. The deadline is 10 seconds. Timeout/cancel clears only this channel's
pending DOS binding; it does not terminate the native owner. This candidate
is confined to the worker/frontend adapter, not the original OpenNT task
scheduler or guest.

The graph-selected x86 source compile and link commands produced
`ntcon.exe` and `ntvdm.exe`, including the VDM TIB storage audit. They were
copied into the new build-only `build/M0-T423/S25/runtime-binding-event`
package; `O:/winnt` was not replaced. With this package temporarily mapped
only to `Z:` and unmounted after each run, `native-zero`,
`native-cmd-dos-repeat`, and `frontend-chain-b` passed with actual exits
0, 0, and 23. All 17 Console cases and all 17 private-desktop Window cases
passed under `m0-t423-s25-binding-event-console17` and
`m0-t423-s25-binding-event-window17` in `O:/winnt/Logs2`. The retained
headless WOW check with `-PackageNetworkProfile` preserved the WINMINE main
window and the original SOL/WRITE out-of-memory modal frontiers under
`m0-t423-s25-binding-event-wow`; these are not full SOL/WRITE acceptance.

The strict supplemental `dos-native-dos` screen-history case still fails:
the intermediate snapshot contains the real CMD banner, but the final
Console snapshot after returning to DOS has lost that older banner while
the task itself exits with its expected code. See
`m0-t423-s25-binding-event-dos-native-dos-dos-native-dos.txt` and its
line-02/final Console snapshots in `O:/winnt/Logs2`. This assertion remains
intact and is not counted as passed. The explicit pending-binding rollback
was added after those full-matrix runs. Its x86 NTCON rebuild and focused
`native-zero`, `native-cmd-dos-repeat`, and `frontend-chain-b` real cases
passed in the separate build-only `runtime-binding-rollback` package under
`m0-t423-s25-binding-rollback-focused`; the full matrices have not been
repeated for this later binary. A further lost-wake review found that stop
notifications had to acquire the same `io_lock` as the condition predicate;
otherwise cancellation could occur between the check and sleep. The corrected
NTCON binary was built into the distinct `runtime-binding-wake` package, and
the same three real focused cases again passed under
`m0-t423-s25-binding-wake-focused`. The current x86
`console-channel-lifetime-test.exe` also exited zero in its own hidden
Console; running it without a Console or with redirected stdout is not its
specified environment. The fixture was then extended with 32 repeated
cancel-versus-wait races while a native owner held the frontend. It verifies
prompt `ERROR_OPERATION_ABORTED`, clears the canceled DOS pending slot, and
admits a different DOS owner afterward; its rebuilt x86 executable exited
zero in a hidden Console. These checks do not prove every fault/lost-wake case.
At this intermediate stage, NTCON cancellation-teardown, remaining
fault/lost-wake cases, coherent publication, governance and push were still
open S25 gates; later results are recorded below.

The formal `product-programs` x86 Ninja target subsequently completed with
the current sources and an intact Visual Studio/Windows SDK environment. A
follow-up dry run reported `no work to do`, restoring the incremental cache.
All seven produced binaries were copied into the coherent build-only
`runtime-formal-all` package with the unchanged accepted `WOW32.DLL`; the
eight hashes were recorded in the build run output. Using only a temporary
`Z:` mapping, this exact formal package passed all 17 ordinary Console cases
and all 17 private-desktop Window cases under
`m0-t423-s25-formal-console-17` and `m0-t423-s25-formal-window-17`.
The network-profile headless WOW gate preserved the WINMINE main-window and
SOL/WRITE original modal frontiers under `m0-t423-s25-formal-wow` (not full
SOL/WRITE acceptance). No formal product binary was published to `O:/winnt`
at this point. The strict supplemental screen-history assertion and the
remaining fault/teardown gates were still open at this stage.

## Bounded NTCON channel teardown

The prior channel stop repeatedly issued `CancelSynchronousIo` and joined in
50-ms slices without a terminal bound. The current code signals stop, issues
one synchronous cancel and one pipe `CancelIoEx`, then waits once for at most
10 seconds. On a timeout it returns `ERROR_TIMEOUT` without closing/freeing
storage that the still-running channel thread may reference. The frontend
service preserves that channel/root and reports failure; NTCON's process
exit then supplies terminal resource cleanup rather than a use-after-free.
No DOS task, native target or worker is killed by this mechanism.

The incremental formal x86 product build plus
`console-channel-lifetime-test.exe` and
`frontend-scope-lifetime-test.exe` succeeded. The scope fixture passed its
authenticated-admission/lifetime assertions; the channel fixture exited zero
in its own hidden Console, including 32 cancel-versus-wait races, real pipe
EOF and exact post-warm-up handle equality. A fresh build-only
`runtime-formal-teardown` package then passed all 17 Console and all 17
private-desktop Window cases under `m0-t423-s25-teardown-console-17` and
`m0-t423-s25-teardown-window-17`, plus the unchanged WINMINE/SOL/WRITE
headless frontier checks under `m0-t423-s25-teardown-wow`.
The strict supplemental final scrollback assertion remains a known failure;
it is not silently treated as a pass. `O:/winnt` remains untouched.

The initial condition-variable cancellation wake also had a teardown hazard:
the stopping thread could block acquiring `io_lock` before it reached the
bounded join if another channel held that lock in a Console operation. The
per-waiter event design removes that acquisition from stop entirely; cancel
and root-stop are persistent handles directly in the wait set. The formal
x86 product and two lifetime-fixture targets built; both fixtures passed,
including the 32 cancellation races. The separate build-only
`runtime-formal-waitset` package passed `native-zero`,
`native-cmd-dos-repeat`, and `frontend-chain-b` with exits 0, 0 and 23 under
`m0-t423-s25-waitset-focused`. The x86 channel
fixture now has a `--stop-timeout` fault mode: its dispatch deliberately holds
the frontend I/O lock while the caller stops the channel. In a hidden Console,
the injected case returned zero after approximately 11.4 seconds, proving the
10-second stop result is observable without waiting on that lock. The fixture
then releases the dispatch, joins it and cleans up. Its normal hidden-Console
mode also returned zero. This proves the bounded teardown case, not every
possible blocked Windows Console operation.

The concurrent NTCON restoration change in the shared worktree records the
calling Console's buffer extent, viewport and cursor at frontend creation and
restores them before root teardown. Its focused channel fixture mutates the
DOS-sized buffer and cursor, then verifies the caller's exact geometry and
cursor are returned; the fixture passed. This restores shell ownership on
exit, but does not reconstruct scrollback already clipped during an earlier
native-to-DOS shrink.

The exact `runtime-formal-waitset` package then passed all 17 ordinary Console
cases and all 17 private-desktop Window cases under
`m0-t423-s25-waitset-console17` and `m0-t423-s25-waitset-window17` in
`O:/winnt/Logs2`. The headless WOW gate returned zero under
`m0-t423-s25-waitset-wow`: WINMINE reached its localized main window, while
SOL and WRITE remained at their original out-of-memory modal frontiers.
Those are depth-preservation checks, not claims that SOL or WRITE now run.
All of these runs used only a temporary `Z:` mapping, removed in `finally`.
The supplemental final screen-history assertion is still separate and failing.
It was repeated on this latest package as
`m0-t423-s25-waitset-history-dos-native-dos`: the script exited 1 because the
final screen again lacked the earlier `Microsoft Windows [Version` marker.
The latest 17+17 package results must not be read as passing that supplemental
assertion.
The final formal x86 fixture binaries were also rerun with their specified
arguments: `worker-base-next-command-test`, `ntw32-execution-lifetime-test`,
`ntw32-presentation-test` (including `--input-return`),
`frontend-scope-lifetime-test`, and `basesrv-service-reservation-test` all
exited zero. `console-frontend-test` and `ntw32-close-test` also exited zero.
The execution-lifetime log reports 448 checks/zero failures, 12 completed
requests, 16 cancellations, target survival and zero remaining handles. The
presentation input-return log reports 677 checks/zero failures. Both are
under `build/M0-T423/S25/`.
An initial launch of the execution-lifetime fixture without its required log
argument returned its documented usage code 2; the proper invocation passed.
The latest Run16/NTSRV package with only `ntw32.exe` substituted by the
build-only live-but-unregistered fixture returned actual launcher status
`0x000005b4` (`ERROR_TIMEOUT`) in
`m0-t423-s25-waitset-stuck-final.txt`; it did not hang. A subsequent process
query found no remaining test-owned Run16/NTCON/NTW32/NTSRV process.
The independent `base-client-rpc-first-test --native-reservation` fixture
initially rejected the protocol-24 broker because that fixture binary was
still protocol 23; the formal product target does not include this fixture.
After Ninja rebuilt its two affected x86 rules against the current header,
the controlled-broker invocation exited zero and proved duplicate reservation
rejection, release/reuse and stale identity rejection. This was a stale test
artifact, not a production protocol mismatch. Its owned broker was stopped.
The original S24-timeout `native-reuse.cmd` was also rerun against the final
wait-set package. Its observer report
`m0-t423-s25-waitset-native-reuse.txt` records actual launcher exit zero,
and its Console capture contains two real Windows-version lines. The outer
PowerShell invocation used `Start-Process -Wait`, which additionally waited
for intentionally resident NTSRV/NTW32 descendants after the observer had
already written its completed report. That harness wait was interrupted;
the test-owned processes were verified gone and `Z:` was removed. This was
not a product timeout and must not be confused with the observer's result.
An additional broker-death injection was attempted on the stuck-worker
package. The first two probes did not identify a live broker before the
launcher's normal 10-second timeout. A third probe killed a newly started
broker PID, but the observer subsequently reported a different live broker
PID and the same `ERROR_TIMEOUT`; it does **not** prove death of the broker
that owned the tested launcher connection. These reports remain diagnostic,
not a passing broker-death acceptance. No test-owned process or `Z:` mapping
remained after cleanup. The unchanged BaseClient broker watcher is source
evidence of intended failure containment, not a substitute for this runtime
proof.

A controlled follow-up eliminated the identity ambiguity: the test started
one NTSRV instance itself and held that exact process handle before launching
the isolated observer. While the deliberately non-registering NTW32 fixture
kept Run16 in startup wait, terminating that owned broker made the launcher
exit with `0x000006ba` (`RPC_S_SERVER_UNAVAILABLE`), not with the 10-second
timeout. Report: `m0-t423-s25-waitset-broker-death-owned.txt`. This fixture
does not establish real-worker shutdown because its fake `ntw32.exe` never
connects to the broker. A separate ordinary `cmd /c ver` run on the real
`runtime-formal-waitset` package returned zero, left one resident NTW32, then
terminated the exact owned NTSRV process. That NTW32 process signalled exit
within five seconds. Report:
`m0-t423-s25-waitset-worker-broker-loss.txt`. Both probes used only temporary
`Z:` mappings, removed afterward; no test-owned processes remained.

For frontend death, the first isolated injection killed NTCON as soon as it
appeared, before bootstrap completed. The launcher returned error 5; this
tests early-bootstrap failure only and is not evidence about a pending
worker-state wait. A second controlled injection waited until the
deliberately non-registering NTW32 fixture had actually started, then killed
that run's NTCON. The waiting launcher returned `0x000000e9`
(`ERROR_PIPE_NOT_CONNECTED`) promptly rather than waiting for its deadline.
Report: `m0-t423-s25-waitset-frontend-death-ready.txt`. The owned broker,
fake worker and temporary `Z:` mapping were cleaned afterward.

The same strict case was run against the isolated preceding S24 package as
`m0-t423-s25-s24-history-control`: that package returned an unexpected
status before reaching the final-screen assertion. It is not a
valid passing screen-history baseline. The S25 candidate reaches the later
DOS task and returns the expected code, revealing an additional display
frontier. Source inspection identifies a concrete risk: NTCON's
`run16_console_prepare_dos` resizes the physical Console buffer down to the
DOS 80x25 page on native-to-DOS handoff, which necessarily discards older
Console scrollback. No assertion was weakened, and the current package is
not claimed to preserve the prior native banner on final return.
The current `opennt_console_resize_grid` binding explicitly mirrors OpenNT's
`ResizeScreenBuffer` row-copy rule: it retains only cursor-containing rows
that fit the new height. Therefore the strict final-banner assertion may be
stronger than that original resize contract. This is a source-backed
interpretation of the failed display check, not a passing test or permission
to change it without review.

S25's admitted acceptance is native nesting, bounded worker/frontend waits,
failure containment and preservation of the previously accepted package.
The strict final-banner check was added during this investigation; it has no
passing S24 baseline and asks for history beyond OpenNT's documented
cursor-row retention on shrink. The failure is retained as an explicit
product-experience observation, **not** counted as a pass. Consistent with
the owner's original-semantics/minimal-diff rule, S25 does not add a shadow
scrollback store to make this extra assertion green. Any product requirement
to retain all prior native rows across a DOS 80x25 transition must be
separately admitted and verified; this is not silently treated as completed.

## Current isolated package identity

`runtime-formal-waitset` is a build-only protocol-24 x86 package, not the
published `O:/winnt` set. Its SHA-256 values are:

| File | SHA-256 |
| --- | --- |
| `run16.exe` | `7C23E88A3DF5B312319ECF0924AAB940352D32F1A45C1008F05495683D55D780` |
| `ntsrv.exe` | `23707D9E3A22B241E1F815FB3B72D8B78656C25E2964802667EC773BEA5F4045` |
| `ntvdm.exe` | `A7FCD3FB2B54105902FFFD3653F462CCB851EE21AC5CE2403F5B195072826A2C` |
| `ntw32.exe` | `D60137301B96847AF9B1F8730B9C9B4A379B1AD47B3EB2538CF664C839CB10AA` |
| `ntcon.exe` | `D71A85C094C2B96AFCA80BDD98FAEC88C5B491765FC1F706D75BFDD7CDDECFF5` |
| `ntmon.exe` | `22F748D18A34100E00932F0F777EA554382C6E97CD0D501668762854288F919C` |
| `VDMREDIR.dll` | `D768C09D84EC83597186FB6BC08871C0045915FACC6ACD135820DAD9CAF9BD77` |
| `WOW32.DLL` | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |

The formal Ninja dry run reports `no work to do`. A fresh source or build-input
change invalidates the affected binary tests and this manifest; nothing in
this section authorizes publication of an unverified mixture.
Five additional consecutive real `native-cmd-dos-repeat` runs on this exact
package all exited zero and passed their required DOS memory-output markers.
Their distinct reports use `m0-t423-s25-waitset-repeat-1` through `-5` under
`O:/winnt/Logs2`; the temporary `Z:` mapping was removed and no test-owned
product processes remained. This increases confidence in the notification
handoff but does not prove the strict final scrollback assertion.
For simultaneous admission, two separate hidden Console observers launched
`run16 cmd.exe /d /c ver` at the same time against one owned protocol-24
NTSRV. Both reports (`m0-t423-s25-concurrent-a.txt`,
`m0-t423-s25-concurrent-b.txt`) recorded real launcher exit zero and visible
Windows-version output. Two NTW32 workers remained resident afterward,
consistent with independent frontend roots; only this test's broker and
workers were then ended. This is an actual concurrent-start check, not proof
of every possible reservation interleaving.
Before any replacement, the currently installed eight files were copied to
`build/M0-T423/S25/prepublication-eight-file-backup/` and each copy was
SHA-256 checked against `O:/winnt`. The installed set remains unchanged;
this backup is a recovery artifact, not a second product publication.

## S25 publication and closure verification

The eight formal-package files in the identity table were copied as one
coherent set into `O:/winnt` after checking that its old hashes still matched
the recovery backup and that no product process was running. Every published
file was SHA-256 checked against the exact tested build-only package; all
eight matched. COMMAND.COM, MEM.EXE, EDIT.COM, `config.nt`, `autoexec.nt` and
`system.ini` already had identical hashes in both locations and were not
modified. The rollback copy path was prepared but not needed.

Post-publication actual-output smoke tests on `O:/winnt` passed:
`native-zero` exited 0 with Windows-version text; `mem` exited 1 with its
expected memory text; `edit` exited 1 with its expected guest interaction;
`native-cmd-dos-repeat` exited 0 with two DOS memory-output markers. Reports
use prefixes `m0-t423-s25-published-native-zero`, `-mem`, `-edit`, and
`-native-cmd-dos-repeat` in `O:/winnt/Logs2`. An initial invocation passed
four `-Cases` values as one literal string and was rejected before any case
ran; the corrected individual invocations are the evidence. All 17+17 and
WOW checks were run on the same eight hashes before publication, as recorded
above. No guest binary, configuration or shared library source was changed.

The strict supplemental final-banner check remains a recorded failure under
OpenNT's original fixed-row shrinking semantics; it is not a product pass or
a claimed scrollback-preservation feature. S29 must retain the discrepancy
for owner audit. S25's admitted worker-control and native-nesting conditions
are met, subject to final repository governance, commit and push.
