# T423 S8 GUI launch and wait

## Baseline and scope

S8 was admitted after S7 P1 99276d68d and closure P2 125fd13ff.
O:/winnt retains the verified seven-file S7 publication, including ntkvm.exe
and ntsrv.exe. The launcher-option/native-GUI candidate is wired and under test;
the published package remains unchanged. This is a bounded launch/wait
audit, not ConPTY, path lookup, a new scheduler or guest modification.

## Source evidence, 2026-09-27

Read-only inputs:

- OpenNT-4.5 nt/private/windows/cmd/cext.c, SHA256
  5E977445963F60F9797643C1C23BE49BEAE269F89E5A3A9C16671BEF4F8409D6.
  Its fEnableExtensions/CurBat/fSingleBatchLine/fSingleCmdLine/AI_SYNC
  predicate changes WOW/GUI execution to AI_DSCD only in the selected shell
  context. AI_SYNC calls WaitProc; AI_DSCD closes the process handle and
  returns creation status. These are shell-owned execution choices, not
  BaseSrv policy. The whole CMD translation unit is not a standalone launcher
  dependency; its parser/global shell state is unavailable in run16.
- Original OpenNT windows/core/ntuser/kernel/queue.c, SHA256
  0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8.
  UserNotifyProcessCreate flags 4 allocates/resets WOWTHREADINFO keyed by
  task ID, records wait object and parent, for startup/WaitForInputIdle.
  This is before task creation, not proof of a loaded target. Its kernel
  process/thread/USER-global dependencies prevent direct whole-unit reuse.
- Selected [srvvdm.c](../../../src/opennt-host/base/win32/server/srvvdm.c):
  BaseSrvUpdateWOWEntry creates paired parent wait handles and conditionally
  calls UserNotifyProcessCreate. BaseSrvRemoveWOWRecordByITask signals the
  parent event on removal. BaseSrvGetVDMExitCode returns zero for the shared
  WOW sentinel. Parent completion is therefore not a load-success event and
  does not supply an actual per-application Win16 exit code.
- Selected [wkman.c](../../../src/mvdm/wow32/wkman.c): W32Thread transfers
  iW32ExecTaskId into td.VDMInfoiTaskID, then clears the global. It calls
  pfnInitTask with that shared task ID; failure destroys the task and exits
  its host thread. WK32WowFailedExec calls ExitVDM for the still-pending task.
  The call has no original numeric loader-error argument. Do not fabricate
  a detailed load error code or confuse the two failure/completion meanings.
- Existing [registration bridge](../../../src/wow32-dll/source/wow_user_registration_bridge.c)
  registered_init_task forwards the task identity into
  [task lifecycle](../../../src/wow32-dll/source/wow_user_task_lifecycle.c),
  whose xxxInitTask call retains the original owner. The local FIRSTIDLE
  binding signals a present task idle event and otherwise clears FIRSTIDLE.
  This existing path is the first reuse candidate, not a reason to add a
  window observer as a production readiness protocol.
- Existing [service binding](../../../src/ntsrv-exe/opennt/source/base_service.c)
  sets UserNotifyProcessCreate=NULL. service_abandon_launch distinguishes
  claimed reservations from pending unclaimed creation, and only the latter
  uses original UNDO_CREATION. Audit both new-worker and reused-WOW paths
  before allowing an async launcher to disconnect.
- Existing [run16](../../../src/run16-exe/main.c): launch_gui waits for the
  real child process, while launch_vdm waits the parent event/worker and
  obtains the original VDM result. No current async GUI option or launch-only
  acknowledgement exists. Text/native frontend routes already have separate
  ownership and must not change for this stage.

## Contract direction and remaining design gates

Use explicit launcher wait policy rather than guessing parent shell context.
The proposed public syntax is run16 [--wait] <binary> [arguments]: default
GUI launch-only, explicit --wait retains synchronous GUI completion, and
DOS/Win32 text remain synchronous. Batch or CMD /c callers requiring final
GUI results use --wait explicitly; this is a documented standalone CLI
contract, not a claim that run16 can recover CMD's private interactive flags.
Existing return-37 and twelve-target fixtures must request synchronous GUI
execution explicitly and retain every current result/topology assertion.
This syntax direction is not yet implemented or runtime-accepted.

Win32 creation success and Win16 loader acknowledgement are different gates.
Prefer the existing original InitTask/failed-exec boundaries and retained task
identity; inspect original USER startup records and current authenticated
service messages before defining any missing finite transport. Do not use
queue insertion, worker readiness, first-window polling or a background waiter.
The exact acknowledgement ABI and failure/rundown binding remain to be
designed and tested; no source import or semantic replacement is yet admitted.

## Loader ordering and identity audit

Further source inspection on 2026-09-27 resolves these design questions:

- `src/mvdm/wow16/test/shell/wowexec.c::ExecProgram` calls LoadModule and
  maps its result to a message resource. Its caller displays MyMessageBox
  before calling WowFailedExec, even on failure. A pending failure can thus
  wait for the original modal interaction. Do not introduce a timeout that
  labels this as success, nor promise immediate loader-error reporting from
  the existing no-argument failed-exec thunk. This file is source evidence;
  guest bytes have not been rebuilt or changed.
- `wkman.c::W32Thread` moves the launch ID to `td.VDMInfoiTaskID` and clears
  `iW32ExecTaskId` before calling InitTask. Its final InitTask failure calls
  W32DestroyTask, whose original ExitVDM path uses the saved task ID (or
  ALL_TASKS for the last task). Instrumenting only WK32WowFailedExec would
  miss this failure. A positive acknowledgement must follow successful
  InitTask, not the first attempted call, and does not prove application
  initialization, first idle or a usable window.
- `wkman.c` also intentionally skips a redundant shared WOWEXEC command and
  completes its record with ExitVDM without starting a new task. This must
  remain a separately classified original no-op, not be mistaken for a
  failed application load solely because no InitTask acknowledgement exists.
- `base_service.c::OpenNtBaseServiceCheck` saves `connection->task` only in
  VDM_NOT_PRESENT. The ready/reuse path retains the parent receipt/event but
  not the returned task ID. Any new per-task startup query therefore needs
  to bind the original returned iTask on both paths under the service lock;
  a worker ID or parent receipt alone is not a substitute. This is a gap in
  the proposed new acknowledgement, not proof of an existing sync regression.
- `OpenNtBaseServiceExit` already dispatches the original WOW task removal.
  Reuse that removal boundary for an unacknowledged-task outcome; do not add
  a second task scheduler or infer a precise loader error absent from source.
  The authenticated worker-to-record check and redundant-WOWEXEC disposition
  must be finalized before production wiring.

## Wait-prefix candidate and focused evidence

The launcher owns its explicit CLI policy; OpenNT CMD's private parser and
interactive/batch globals are not available to it. This owner-approved CLI
exception uses a small `launch_options.c/h` parser in run16-exe, not a mirror
change, shared parser or replacement for COMMAND. Its only output is a wait
bit and a pointer into the unchanged local command string. No pointer crosses
IPC. It recognizes only an unquoted exact leading `--wait`; `--` ends prefix
processing. Missing targets and duplicate leading --wait fail. Target arguments
are not parsed or reconstructed. Quoted option-looking filenames and options
after the executable remain target text. Original classification and command
execution remain the eventual consumers.

`tests/app/run16_launch_options_test.c` exercises 19 cases, including compact
command/c, quoting, redirection, trailing whitespace, option-like executable
names, explicit terminator, duplicate flags and null/empty inputs. The generated
graph now exposes `run16-launch-options-test.exe`; the candidate is deliberately
not yet linked into run16.exe pending the full native/WOW implementation.

Focused run: `build/M0-T423/S8/launch-options-r1`, VS2022 BuildTools
VsDevCmd `-arch=x86 -host_arch=x64`, then:

```text
cl.exe /nologo /MT /W4 /WX /Fe:launch-options-test.exe ../../../../tests/app/run16_launch_options_test.c ../../../../src/run16-exe/launch_options.c
launch-options-test.exe
```

Both commands returned zero; marker `RUN16-LAUNCH-OPTIONS-PASS cases=19`.
Objects and executable remain in that build directory. This proves only the
prefix parser, not GUI launch/wait integration. No runtime package was changed,
no full regression is claimed, and this work is not a delivered production P.

The formal x86 graph was regenerated in the retained
`build/M0-T423/S1/restart-formal-x86` cache. Its dedicated
`ninja -j4 run16-launch-options-test.exe` target compiled both translation
units, linked and returned the same 19-case pass marker. The first sandboxed
Ninja invocation produced no output and was explicitly interrupted; its exact
processes were confirmed absent before the scoped elevated retry. Only the
completed retry is passing graph evidence. No product target was rebuilt or
published by this focused run.

## Native GUI production-path candidate

The candidate `main.c` now consumes the tested leading prefix before its
original classifier and tail handling. Native GUI creation retains the
existing restricted handle/environment materialization. Default success closes
the launcher's child handles without waiting or terminating the target;
`--wait` waits the actual process and returns GetExitCodeProcess. DOS/native
text still use their existing synchronous route. Win16 remains synchronous
until the authenticated startup binding is implemented; this deliberate
incomplete candidate is not published as the unified S8 product.

`tests/app/run16_gui_wait_test.c` builds both a Console supervisor and a
windowless GUI target. It launches the actual candidate run16 with
CREATE_NO_WINDOW. Named test events and a mapping establish that the target
has started and is still alive; the supervisor then releases it to exit 37.
It asserts asynchronous launcher result 0 while the child remains alive,
explicit --wait result 37, --wait with -- termination, and a killed waiting
launcher result 96 with its child still running and subsequently exiting 37.
No UI is shown, no unrelated process is killed, and the child has a bounded
test-only timeout on supervisor failure. These are real Win32 process tests,
not WOW or frontend acceptance.

Formal incremental graph builds succeeded for run16.exe and both fixture
targets. The first fixture compile failed on missing `_countof` declaration;
adding its CRT header fixed it. Final run r3:

- Log: `O:/winnt/Logs2/t423-s8-gui-wait-r3.log`.
- Command: from the repository root, run the formal-cache
  run16-gui-wait-test.exe with absolute paths to its run16.exe and
  run16-gui-wait-target.exe; redirect output to that log.
- Four `GUI-WAIT ... PASS` rows and `RUN16-GUI-WAIT-PASS cases=4`, exit 0.
- Candidate run16 SHA256:
  7C21849107DB605486FDE3BA902EFC50CCCFB835F36A898569C907845D2DC656.
- Supervisor SHA256:
  665FC533B8BE360A5222248228B13E96AAE4F18ABE6343FE8ABFDA8D57B919FE.
- GUI target SHA256:
  062945EDBCEFCF1A687D40C41F211C47020D0C297031328BE4CC1F5783E783D9.

The 19-case prefix test also passed after entry integration. Missing/bad image,
immediate target completion, real shell contexts, WOW and full cumulative
regression remain open. Concurrent ntsrv-exe/main.c edits observed during this
run were preserved and are not covered by this broker-free native-GUI test.

## Authenticated WOW acknowledgement design refinement

Original `srvvdm.h` WOWHEAD already holds its worker SequenceNumber; worker
Connect publishes it using BaseSrvUpdateVDMSequenceNumber. Original
BaseSrvExitWOWTask accepts task removal only for this same registered sequence.
Original GetNextVDMCommand marks the selected WOWRECORD fDispatched and returns
its iTask only after the command buffers are copied. These existing identities
are sufficient authority: require authenticated WOW worker role, matching
WOWHead sequence and a dispatched original task before accepting a successful
InitTask notification. A copied task ID alone is never authority.

The launcher-side acknowledgement must be attached to its existing connection
and original task/parent receipt on both new/reuse Check paths. Retain a
latched result through an immediate task exit; detach notification resources
on launcher disconnect without undoing a handed-off original record. Use an
OS event attachment and a finite status query, not polling windows or creating
a background waiter. The original parent completion remains independent.
Actual RPC shape, negative authorization tests, redundant WOWEXEC disposition
and failure cleanup are not yet implemented or accepted.

## Real WOW loader access failure and same-worker recovery

Extended wow_startup_fault_test with `loader`: at the debugger-observed
WOW32 load boundary, after run16 has classified original WINMINE as Win16,
open the candidate WINMINE read-only with no sharing. The original guest
loader's later open therefore fails; the file's bytes are never changed.
Unlike process-fault modes, this mode continues guest/WOWExec initialization
and waits for original task failure. The launcher must return 1114 (missing
startup notification plus original zero completion), not success. The worker
must remain alive. Then release the test lock, detach the debugger, launch
WINMINE again and require startup 0 while the same worker remains alive.

Retain all attempts: r1 returned 1114 but failed its worker assertion without
enough handle diagnostics; its cause is not retroactively declared solved.
r2 added handle diagnostics and passed failure/still-active checks. r3's
added recovery run timed out before the first task completed within the
initial 30-second debug bound, and is not acceptance. Loader mode now uses
the existing integration-scale 60-second bound because it continues complete
WOWExec initialization after injection; worker/broker kill modes retain 30s.
This changes test timing only, not product results or failure assertions.

`t423-s8-wow-loader-r4` in O:/winnt/Logs2 passed: first run16 exit 1114,
worker wait=258 / queried exit=259; recovery launch exit 0 and original worker
still alive. RPC trace records VDM_NOT_PRESENT (1) for the failed first request,
original ExitVDM task 1, then VDM_PRESENT_AND_READY (4) for recovery. There is
one successful InitTaskNotification for the recovery, not the failed request.
Markers WOW-LOADER-RECOVERY and NEW-WOW-STARTUP-FAULT-PASS; fixture exit 0.
Build: MSVC x86 /MT /W4, build/M0-T423/S8/wow-startup-fault-build-r6.log.
Source SHA256
9B58B27D7E7B5F7AF8EC1B64BBF754888D7D43757C5FCAC685634CCD7C3E4263;
fixture SHA256
244EFA9305FD36E1E223964F6413C763A0474E07983873A0DD9243D15ABFE61F.
Candidate WINMINE SHA256 matches the retained S7 original:
A9D2AFBD1AA98E38F3679F3A1A5DC0463367DD47C9AB1792E5AB8BBDC3441C0C.

This proves actual post-classification loader access failure, truthful startup
result, original task retirement and recovery. It is not an exhaustive NE
malformation/error-code matrix, nor a claim to preserve a guest error number
the original completion ABI does not publish. No production change or formal
package publication was made by this test work.

## Real new-worker startup fault gate

Added `tests/observation/wow_startup_fault_test.c`. The authored native
supervisor creates a private unswitched desktop, owns a fresh broker and
starts real run16 WINMINE with DEBUG_PROCESS. It identifies the new worker
by exact physical image path and injects at that worker's WOW32 DLL-load
debug event, before guest InitTask can run. No product hook, guest mutation
or timing-only process lookup. Debug events are continued normally, foreign
exceptions are not swallowed, and cleanup/detachment uses owned handles/PIDs.
This debugger-controlled test proves failure ordering, not launch performance.

Both r1 launcher-result cases passed. Strengthened r2 additionally requires
the worker's own EXIT_PROCESS_DEBUG_EVENT and expected result before cleanup;
test cleanup cannot satisfy that assertion. With separate prefixes
`t423-s8-new-wow-worker-r2` and `t423-s8-new-wow-broker-r2` in O:/winnt/Logs2:

- Worker injection: run16 1067; worker 97 (the injected termination status).
- Broker injection: run16 1722; worker 1722 from its own service-loss path.

Both emit NEW-WOW-STARTUP-FAULT-PASS and exit 0. The runner also requires
run16-vdm-state 1 in each RPC trace and rejects an InitTaskNotification in
registration evidence. Builds use MSVC Win32/x86 /MT /W4 under
build/M0-T423/S8, build logs wow-startup-fault-build-r1/r2.log. r2 compiles
without diagnostics. Invocation: wow-startup-fault-test.exe, existing stage
alias, physical build/M0-T423/S8/p, then `worker` or `broker`; set the two
trace-path environment variables to each prefix's RPC/registration logs.
Fixture source SHA256
629C0F0948C6D9562A4120EA729E7533E94E4E69027E1B55EEFD1728EA86F465;
executable SHA256
91D52E16DBEAE021A65B33A109B921AFD12762679E483DDF7B59DCAA6EE721D4.

Together with shared-WOW fault evidence, both new/reused startup routes now
have real worker/broker failure evidence. Actual WOW loader rejection remains
a separate open gate; forced process loss is not loader-failure acceptance.
No production source change, formal publication or P delivery in this step.

## Refreshed separate WOW headless frontiers

Ran observe-wow-frontiers.ps1 on the unchanged S8 candidate, S7 observer and
window snapshot tool, WaitTarget enabled, separate WINMINE/SOL/WRITE runs.
Prefix `t423-s8-wow-frontier-r3` under O:/winnt/Logs2; runner exit 0 and all
owned observation processes cleaned up. Inspected each window report rather
than treating the runner status as guest completion:

- WINMINE: visible localized main class (UTF16 00C900A800C000D7), plus WOWExec.
- SOL: retained localized out-of-memory text (00C400DA00B400E600B200BB00B900BB).
- WRITE: retained "Not enough memory for Write to complete this operation."

These reproduce the separate inherited headless observation frontiers, not
new SOL/WRITE usability or WINMINE gameplay acceptance. The bounded waiting
observations are not normal task completion tests. Shared-WOW close/completion
and native lifecycle tests remain separately evidenced. No guest/profile
changes and no formal-package deployment occurred.

## Cumulative Console and Window failure matrices

Both `verify-frontend-lifetime.ps1 -ExpandedFaults` and its `-Window`
variant passed all five actual-runtime cases: normal, frontend loss,
launcher loss, worker loss and helper loss. Prefixes
`t423-s8-fault-console-r3` and `t423-s8-fault-window-r3` under O:/winnt/Logs2;
both runner exit 0. Evidence roots are build/M0-T423/S8/fault-console-r3 and
fault-window-r3. The Window variant requires proof of an actual visible
frontend Window before transition on its private desktop; both variants
require distinct unrelated frontend/native target survival. Helper-loss
assertions retain original pipe-error handling and explicit termination,
not fabricated successful task completion. No owner desktop switch.

The initial Console r2 attempt was refused before runtime because the S8
stage lacked the authored NOIOLIFE/NOIO probes. Copied the S7 renamed-package
test inputs into the existing build-stage tests directory; their source has
no diff from S7 P1. NOIOLIFE.EXE SHA256
8869C8B5509E060705B6739AF4E8663C2B3ED24BA914E89142D2B90BC8B0590F;
NOIO.COM SHA256
C5686BF3619BA5020DA62181AA74D4097669C43873C6D334C7F939E4DB1F721D.
They are authored probes, not modified original guest media. r2 is a missing-
prerequisite result, not a product failure or a pass. No publication occurred.

## Refreshed component contract regression

Ran verify-frontend-transport-contracts.ps1 with ExpandedBackend, the formal
x86 build root and S7 private-desktop observer. r2 passed, but a subsequent
Ninja dependency dry run found nine compile/link steps pending; r2 is not
accepted as evidence for all current inputs. Incremental rebuild r3 completed
successfully (build/M0-T423/S8/contract17-build-r3.log). A second dry run for
all eleven distinct executables then reported no work to do.

Fresh run `t423-s8-contract17-r3` under O:/winnt/Logs2 passed all 17 cases,
runner exit 0. Per-case output witnesses and expected nonzero close-callback
statuses remained enabled. Covers Console operations, copied frames, pointer
mock contract, frontend client/retirement, capture/presentation, channel
lifetimes, close/error callbacks and six native hidden-backend cases including
actual result 41, input return, target survival and helper-loss distinction.
These are component contracts, not replacement DOS/WOW guest acceptance.

## Native rejection fixture resolution

r9 private-desktop enumeration identified the actual blocking window:
visible Shell_Dialog, title "This app can't run on your PC", owned by the
Windows error host rather than CMD. The earlier CMD-PID #32770 handler could
not reach it. r9 remains failed diagnostic evidence. No production changes
were necessary.

The fixture now closes only a visible Shell_Dialog with that rejection-title
prefix enumerated on its freshly created private desktop; it sends WM_CLOSE,
never elevation approval and never enumerates the owner desktop. This applies
to the native CMD control and tested launcher waits. It retains bounded waits,
native direct CreateProcess error-193 assertion, exact shell/run16 exit-code
comparison, and cleanup of only owned handles and the authored build-local
image copy. It does not claim unattended product suppression of OS dialogs.

MSVC x86 /MT build native-rejection-build-r10.log succeeded. Runtime
`O:/winnt/Logs2/t423-s8-native-rejection-r10.log` passed all ten cases, marker
RUN16-GUI-WAIT-PASS cases=10 and exit 0: async success, explicit wait,
option delimiter, launcher-death target survival, locked image (two modes),
missing image (two modes), DLL-marked non-launchable PE (two modes). The
rejection control and both run16 cases returned 1 after closing the OS error
notice. Source SHA256
6A23BCAD182E0B13C888217544030EC44571FD076EF8B62A824F3E081D628F76;
supervisor SHA256
3AB12B46004C4090F7A3F66D5FF154401DC20D586625A0C315B7B060729B08C6.
This resolves the fixture's native bad-target gate, not WOW loader-error or
new-worker startup-fault coverage. Earlier malformed-byte DOS/path-161 runs
remain non-acceptance observations and were not relabelled as GUI failures.

## Native rejection fixture follow-up: host control still pending

Replaced the ambiguous malformed 128-byte input with a copy of this project's
authored GUI probe, setting only IMAGE_FILE_DLL in that build-local copy.
The direct CreateProcess control rejects it with ERROR_BAD_EXE_FORMAT (193),
so the image is now demonstrably non-launchable rather than guessed from its
contents. No original guest/media or production image was modified.

Runs t423-s8-native-rejection-r4 through r8 remain failed fixture attempts,
not acceptance. Stage diagnostics localized the failure before the run16 bad-
image calls: direct COMSPEC /c of the DLL-marked executable does not return
within the 10-second bound. r8 explicitly reports wait=258 (WAIT_TIMEOUT),
queried=1, exit=259 (STILL_ACTIVE). Missing-image control returns 1, and the
original six lifecycle/locked-image cases plus two missing-image cases pass.
Setting inherited SEM_FAILCRITICALERRORS/SEM_NOGPFAULTERRORBOX/
SEM_NOOPENFILEERRORBOX did not resolve this control wait. A test-only handler
for exact CMD-PID #32770 dialogs on the private desktop observed no such
dialog; do not infer that no other dialog or native shell wait exists.
Only fixture-owned process handles are cleaned up; the transient copied
image is deleted by the fixture. Runtime logs remain under O:/winnt/Logs2;
corresponding r4-r8 build logs remain under build/M0-T423/S8.

The bad-image acceptance remains open. Next establish the shell control's
actual wait/dialog owner or select a source-proven noninteractive rejection
fixture, retaining the direct native rejection and actual run16 result
assertions. No product change or reduced success criterion is justified by
this test-control timeout. No test run is pending after r8 cleanup.

## Window regression and pending malformed-image classification investigation

Current renamed candidate passed all 17 Window DOS cases with the same S7
observer/G7 inputs as Console17, OrdinaryFrontend and both
MVDM_OBSERVER_PRIVATE_DESKTOP=1 and MVDM_OBSERVER_WINDOW_INPUT=1. Prefix
`t423-s8-window17-r2` under O:/winnt/Logs2 retains text/interaction reports
and `.runner.log`; runner exit 0. The staged seven files match their formal
build outputs byte-for-byte, including the separately built WOW32 DLL. This
does not close remaining S8 startup/fault or full integration gates.

The native GUI fixture was extended with missing and malformed image attempts
under default/--wait, only using owned transient build files. Missing paths
passed both modes (result 1); the first malformed-image assertion failed,
actual 161 versus assumed 1. r2 added a real CMD control and r3 supplied an
authored malformed MZ/PE header instead of non-MZ bytes, but both still fail
the comparison: CMD returns 1, run16 returns 161. No product changes were made.
Retain `t423-s8-native-rejection-r1.log`, `-r2.log`, `-r3.log` and r3.rpc.log
under O:/winnt/Logs2, plus build/M0-T423/S8/native-rejection-build-r1..r3.log.

The r3 trace proves binary=16 (DOS), VDM_NOT_PRESENT, new frontend, and worker
status 161. Thus the fixture does not yet exercise Win32 GUI bad-loader
semantics, and its CMD-equivalence assertion is not justified for that route.
The original classifier can classify failed image mappings as DOS through
STATUS_INVALID_IMAGE_PROTECT / BaseIsDosApplication. Exact chosen status and
the worker's path failure still require diagnosis before selecting the final
bad-image assertion/fixture. Do not relabel this failed run a ten-case pass.
Current fixture source SHA256:
3E83272F5E535BF0D9569E068E78E01B2F36F86CE010DD8C15D1CF2222078AAB.
Its earlier six-case pass remains historical evidence, not acceptance of the
expanded current fixture. The new failed fixture blocks joint delivery until
resolved; no formal package replacement or P commit has occurred.

## Owner-admitted task monitor executable rename

Owner requested monitor.exe -> ntmon.exe as part of this S. Only the product
filename changes; src/monitor-exe and the NTVDM Task Monitor title remain.
Updated the formal Ninja output/product-programs dependency, executable lists
in runnable tests, current architecture/coding/publication authorities and the
queued WOW delivery requirement. Closed historical evidence/proposals retain
their original names. No original mirror or monitor implementation changed.

Regenerated the existing formal x86 graph and built ntmon.exe incrementally:
only the new output link ran. Logs: build/M0-T423/S8/ntmon-graph-r1.log and
ntmon-build-r1.log. SHA256:
DAE97C2CCC405DA33946060A4A43B54646E0B957CA3B154579B0A633752412FF.
`tests/observation/verify-monitor-layout.ps1 -BuildRoot
build/M0-T423/S8/ntmon-layout-r1` built and passed the EDIT frame, four arrows,
colors, 25 rows, overflow selection, horizontal extent, shrink, empty and
confirmation assertions. This fixture is layout coverage, not a new real RPC
termination acceptance claim. Build/test runner exited 0.

Copied ntmon.exe only into the build-contained S8 candidate. Its former
monitor.exe is retained at build/M0-T423/S8/monitor-before-rename.exe. The
formal O:/winnt package is unchanged until combined S8 verification passes;
publication must back up/retire its old monitor.exe and publish ntmon.exe as
one member of the coherent seven-file package, not leave both names active.

## Cumulative Console regression on the current candidate

`tools/audit/Verify-CommandExitStatus.ps1` passed all 17 cases with the S7
observer, original G7 fixture, MVDM_OBSERVER_PRIVATE_DESKTOP=1,
OrdinaryFrontend, 60000-ms observation bound, existing staging alias and
physical build/M0-T423/S8/p. Prefix `t423-s8-console17-r2` under
O:/winnt/Logs2 retains per-case evidence and `.runner.log`; runner exit 0.
Cases: empty, native-zero, missing, native-seven, native-streams, native-eof,
mem, nested-empty, nested-mem, mem-repeat, direct-mem, command-c,
command-c-seven, guest-seven, command-c-mem, direct-seven and edit.
Source-defined DOS result 1 in several successful cases is retained, not
normalized to zero. The existing observer's text/interaction gates remained
enabled; no exit-only acceptance substituted. This is Console regression,
not the separate Window17 or remaining fault/WOW loader gates. The candidate
was not published and no production inputs changed during this run.

## Native locked-image failure and frontend documentation review

Extended `tests/app/run16_gui_wait_test.c` with two locked-image negative
cases using only its authored GUI executable. The native control attempts
CreateProcess while a write handle remains open and must fail with 32.
The first fixture incorrectly expected run16 to return that same code;
`t423-s8-native-create-failure-r1.log` retains its failure (actual 1).
Source review showed original GetBinaryTypeW image mapping also fails before
GUI dispatch, so the existing COMSPEC fallback returns shell failure 1.
This does not prove a post-classification GUI creation failure and no product
semantic change was made to force it into that path.

Both default and --wait negative cases now assert the actual existing shell
failure 1 while retaining the native error-32 control. All supervisor children
use a private unswitched desktop, including possible frontend/shell fallback.
The fixture links user32 explicitly for this isolation. Build r3's missing
USER32 imports are retained as a failed build, not runtime evidence; corrected
MSVC Win32/x86 /MT r4 built successfully. Logs are under build/M0-T423/S8.
Runtime `O:/winnt/Logs2/t423-s8-native-locked-r4.log` passed all six cases:
four original success/wait/launcher-death cases and both negative cases,
RUN16-GUI-WAIT-PASS cases=6, exit 0. Source SHA256
D2E99772FCF666E60E899938822DF84FCE2838EC131A79E6B8B1930CD7C64646;
supervisor SHA256
F0EA6D4A44ACED00D8D467EBCA6C7D169EFF6995BE42E2EF637EEEBE8F0F80C1.

Updated src/run16-exe/README.md: it still described the superseded root-run16
renderer/input pump and files now owned by ntkvm. It now documents actual
launcher/frontend ownership and the S8 explicit wait contract, without claiming
S8 verification/publication complete. Actual malformed/missing-image and WOW
loader failures still need their distinct evidence; locked-image rejection is
not a substitute.

## Real shared-WOW startup fault verification

Extended `tests/observation/wow_shared_launch_test.c` with `--worker-fault`
and `--broker-fault`. These are test-only switches, not product controls.
After a real asynchronous WINMINE launch and verified worker image path, the
fixture finds that worker's WOWExecClass thread on its private, unswitched
desktop. It suspends only that dispatcher, leaving the broker watcher live,
then starts a second default-asynchronous WINMINE. Before injecting a fault it
requires the second launcher's exact PID trace to report CheckVDM state 4
(READY) and verifies that launcher has not exited. Thus the test exercises an
admitted reused-worker request pending startup notification, not a guessed
timing interval or a mock service result. Only owned process handles are
terminated; the suspended thread's process is always cleaned up on failure.

Build: MSVC Win32/x86 `/MT /W4`, fixture and object under
`build/M0-T423/S8`; build log `wow-fault-build-r1.log`. Build succeeded with
one signed/unsigned comparison warning in the test assertion, no production
source changes. Source SHA256
8BB13488F468F86EAD1EFF02533271B47BB4BD360FFF415E15C3784612400604;
fixture executable SHA256
C891FD1343CAE18C81E3B358804B8C9A604EA4D26CF513EAE7508AB05E53AB00.
Arguments: the existing T-drive staging alias and physical
`O:/repos.hobby/ntvdm64/build/M0-T423/S8/p`, followed by the respective mode;
set MVDM_S34_TRACE_PATH and
MVDM_WOW_REGISTRATION_TRACE_PATH to the prefix's `.rpc.log` and
`.registration.log` under O:/winnt/Logs2.

- `t423-s8-wow-worker-fault-r1`: exact owned worker terminated while startup
  was pending; second launcher returned 1067, marker
  `WOW-SHARED-STARTUP-FAULT-PASS`, fixture exit 0.
- `t423-s8-wow-broker-fault-r1`: exact owned broker terminated while startup
  was pending; second launcher and worker both returned 1722, same pass marker,
  fixture exit 0. No automatic reconnect or replacement worker.
- `t423-s8-wow-fault-regression-r1-async` and `-wait`: rebuilt fixture also
  passed both original normal modes, including original single-instance
  WINMINE, redundant WOWEXEC success, retained first window and independent
  explicit-wait completion. Both returned 0 and WOW-SHARED-LAUNCH-PASS.

The first attempted fault run was refused before launch because the formal
package endpoint was occupied. Following the owner's standing authorization,
the exact O:/winnt ntvdm PID 25176 and ntsrv PID 29436 were path-checked and
ended, without recursive process termination or desktop switching. No package
files were replaced. This proves reused-worker pending-startup failure only;
new-worker startup faults, actual loader failures and final cumulative
regression/publication remain open, not implicitly covered by these passes.

## Owner-admitted side-chat ntsrv repair

On 2026-09-27 the owner confirmed the side chat had finished and instructed
this S to audit and include its changes in the joint delivery. The reviewed
production change is confined to `src/ntsrv-exe/main.c`: 97 added / 58 removed
lines relative to the current HEAD. Its SHA256 is
916F58447B98397CE1B67FBD2A08BAE724149B13745ECA2AA59B4C2D6E908648.
No MVDM/OpenNT mirror, task scheduling, worker idle timeout or protocol version
is changed by this repair. The existing ten-second empty-service grace is
unchanged; old comments incorrectly described one minute.

Review disposition: retain the repair. A single waitable timer replaces the
timer-queue epoch callbacks. Only Connect admission cancels the grace;
management authentication/snapshot polling no longer pins an empty service.
`pending_connects` protects registration while `idle_lock` serializes it with
the final stop claim. `__finally` releases this hold on ordinary rejection and
exceptions. Once stop is claimed, a later Connect returns unavailable rather
than creating state after shutdown selection. IsEmpty still requires no
worker watches, registered processes or reservations. The worker-empty
notification is invoked after releasing the service lock, so its acquisition
of idle_lock does not invert the idle_lock -> IsEmpty/service-lock ordering.
Timer/stop failures fail explicitly rather than leaving a silently resident
broker. No original BaseSrv policy is replaced: empty retention is the existing
standalone product entry's responsibility.

The side chat supplied actual RPC evidence in
`build/M0-T423/S8/ntsrv-idle/idle-results.json` for seven passing scenarios:
manual empty start, observer polling, live connection then disconnect,
identity rejection, version rejection, client crash/rundown and late Connect.
The earlier MAXULONG compile error is fixed to the public Win32 MAXDWORD in
the final source. Its bad-peer test now supplies a real process handle for a
different PID, reaching peer authentication instead of failing typed event
marshalling before Connect. Those are retained side-chat observations, not
substitutes for the independent rerun.

The accompanying runner was adjusted during review: use the ordinary
dependency-driven Ninja build rather than unconditionally replay every build
command; keep compiler products/logs under build; write runtime captures and
results to an existing approved log directory with a unique prefix. Independent
command (Node 22, scoped execution permission, no running ntsrv beforehand):

```text
node tests/broker/verify_ntsrv_idle.mjs build/M0-T423/S1/restart-formal-x86 <native-ninja.exe> O:/winnt/Logs2 t423-s8-idle-audit-r2
```

The test only terminates handles of processes it creates and does not adopt an
existing broker. Its deterministic fixture compiles actual main.c with clock,
timer and peer/service boundaries substituted; real RPC cases separately test
the unmodified production broker. Full-package DOS/WOW regression, coherent
publication and the joint P gate remain required even if these cases pass.

Independent rerun completed with exit 0: deterministic unit assertions, fatal
SetWaitableTimer/CreateWaitableTimer/stop error checks, x86 Windows-subsystem
identity, and all seven real RPC scenarios passed. The results are retained at
`O:/winnt/Logs2/t423-s8-idle-audit-r2-idle-results.json`; scenario durations
were 10190, 10171, 21587, 16597, 11610, 10200 and 21604 ms in the order above.
Tested ntsrv.exe SHA256:
B8080B82C086ADA4CF774A77D685FA7B8482EBBEF3A808335A2F906F6511CDB7.
This accepts the side-chat change for inclusion in S8's joint P, not separate
publication or a claim that the still-open GUI/WOW scope has completed.

Owner-requested integration recheck (2026-09-27): reviewed the current combined
main.c, including the S8 protocol-9 startup calls, without confusing those
calls with the side-chat retention change. Current main.c SHA256 is
ED85D189E30587070013272D64E99829954CD39D5319238E67615203997AE8FC.
The combined diff is 120 added / 59 removed lines, not the earlier isolated
97/58 count. Rechecked worker notification after service-lock release, pending
Connect exception cleanup, stop/admission serialization and observer behavior.
No new production correction was needed in this bounded review.

Repeated the same verification command with prefix
`t423-s8-sidechat-review-r3`. Endpoint absence was checked before launch;
only fixture-owned processes were used, without switching the owner desktop.
Ninja confirmed unchanged production inputs (`no work to do`); both x86 /MT
test clients rebuilt successfully. Deterministic and fatal-path checks and
all seven real RPC cases passed, runner exit 0. Scenario durations were
10058, 10150, 21597, 16586, 11598, 10199 and 21582 ms. Results:
`O:/winnt/Logs2/t423-s8-sidechat-review-r3-idle-results.json`.
The tested ntsrv.exe SHA256 remains
FAEE345492997C3CC4CA5C78DF20DD8337E5857B6EEFE8D9DBD0B4E4504C1F49.
Retain this implementation and its checked-in test sources in the S8 joint
delivery. This recheck does not waive the outstanding combined S8 failure,
cumulative regression and coherent publication gates; no production P or
O:/winnt replacement was made by this audit.

## Finite startup-notification binding admission

The original `srvvdm.c::BaseSrvGetWOWTaskId` wraps and skips only currently
live records. A retained launcher cannot therefore be matched by iTask alone.
Use the original record's `hWaitForParentServer` and the launcher's retained
parent receipt as the second identity. Existing service_wait_deliver already
allocates these receipts service-wide; they are not native handle numbers.
The broker's original dispatch lock protects both record lookup and receipt
association. Do not add a second task registry or retain a freed WOWRECORD.

Recovery ladder: original srvvdm dispatch/records and original WOW InitTask
remain selected. Original USER's notification/idle registration in queue.c
requires private USER thread/process/SMS state and cannot be composed as a
whole at the admitted stopping boundary. Existing registration_bridge already
owns the finite successful InitTask return. A same-shaped public USER API
cannot expose that historical shared-WOW task identity. The necessary new
binding is restricted to (a) an authenticated WOW worker reporting successful
InitTask for its dispatched original record and (b) the submitting launcher
obtaining a wait-only event and latched boolean for its own parent receipt.
Only connection-local notification state is added. Original completion and
exit-code policy remain unchanged. This is a startup milestone, not first idle,
window creation, gameplay acceptance or an invented Win16 process exit code.

Positive notification must precede that task's original ExitVDM on the same
worker call path. Verify registered WOW worker role, original WOWHead sequence,
dispatched record and matching parent receipt. No original record means reject;
no remaining launcher means no notification consumer, not task termination.
The latch survives task removal and is reset only for a new accepted submission
or connection destruction. Query duplicates SYNCHRONIZE rights only. Wrong
peer/generation/receipt, non-WOW or undispatched reports must fail. Tests must
cover notification before/after query, immediate original task exit, reused
task identity and disconnect. RPC/version/export integration follows service
tests; this admission alone is not a production capability claim. Redundant
WOWEXEC remains a separate original no-op disposition to bind before closure.

### Service binding implementation and directed proof

Implemented the two bounded service entrypoints in existing base_service.c/h,
without changing either original mirror. Successful Check now retains the WOW
task identity for reuse as well as creation. Added connection-local event/latch
cleanup and original parent-receipt matching; no new task table or polling.
Service-wide wait receipts fail at exhaustion instead of wrapping
(`vdm_receipt.c`), so this identity does not inherit WOW task-ID reuse.

MSVC Win32/x86 /MT incremental target:
`basesrv-service-reservation-test.exe` in
`build/M0-T423/S1/restart-formal-x86`. Build log:
`s8-wow-start-service-build-r2.log` in that cache. The similarly named
`basesrv-reservation-test.exe` does not contain the service lifecycle fixture;
its earlier build is not evidence for these assertions.

Both no-argument and `--wow-start-late-query` runs passed with exit 0. Runtime
logs are `O:/winnt/Logs2/t423-s8-wow-start-r2-early.stdout.log` and
`t423-s8-wow-start-r2-late.stdout.log` (matching stderr files retained).
The fixture composes the real original Check/Update/Get/Exit dispatch and uses
registered test processes, not a live guest. It verifies denied launcher-as-
reporter, wrong worker generation, undispatched task, wrong receipt and worker-
as-query-client; pending event cannot be signalled by the recipient; successful
notification does not complete the original parent wait; immediate ExitVDM
completes the parent while the success latch remains readable. Late-query mode
does not create a notification event until after both report and original exit.
The early case additionally substitutes only the test record's parent receipt
to prove identical task ID cannot notify an unrelated receipt, then restores it.
This is an identity fault test, not a claim to have executed 2^32 submissions.

Exact SHA256:

- base_service.c: D0D2AB30CBE2E21C4A8F2059A0D907BB9740F28424A8882EAD25E1BEC1A352EA
- base_service.h: 3395921B8AC29BF237BA1F8F883D224C0148EB5E9443660D6A6F7349EB765FBF
- fixture source: 73C948EBB46990C5BD1B59774B3EB1EFAC763B897FCC9E4C36BE5E36DA671180
- fixture EXE: A6BD5CE6A4012391A171EFC83A3E65DEECB22FC30DC3FB4619E3FF307F0CA2C0

The new service ABI is not yet exposed in RPC or called from WOW InitTask;
real Win16 async launch remains unimplemented. Reused-WOW submission, client
rundown, no-op WOWEXEC and complete production integration remain mandatory.
These changes invalidate the final combined broker artifact identity; rerun
the accepted side-chat RPC tests after the final broker rebuild. No publication
or production-P acceptance is claimed from this service-only fixture.

### RPC transport integration

The service entrypoints now have two finite typed RPC calls, WowStarted and
WowStartup. The latter transfers the service-owned SYNCHRONIZE-only duplicate
through an event attachment, never a sender-local numeric handle. Both calls
authenticate the attached process before service role/generation/receipt
validation. Client queries take the original local parent completion handle
and resolve its existing transport receipt internally, matching the existing
GetVDMExitCode binding rather than exposing receipt guessing to run16.

The IDL interface and shared APP_PROTOCOL_VERSION advance together from 8 to
9; client interface discovery and server registration use v9_0. All seven
components must be rebuilt/identity-verified together before publication.
Formal x86 /MT incremental ntsrv/run16 links passed with generated MIDL code;
build evidence is `s8-wow-rpc-build-r1.log` and `s8-wow-rpc-build-r2.log` in
the existing formal cache. No version-9 component is published at O:/winnt.

The real idle-client fixture now invokes both calls from an ordinary non-WOW
connection and requires ERROR_ACCESS_DENIED, null output event and zero success
flag. The RPC9 idle rerun also exercises the approved side-chat timer change;
its completed outcome is recorded below. This negative check does
not prove successful WOW event marshalling or real InitTask integration.
Remaining work includes that positive RPC path, WOW call/export binding,
launcher async/sync selection, no-op/failure disposition and cumulative tests.

RPC9 rerun completed with exit 0: deterministic/fatal checks and all seven
real idle scenarios passed, durations 10186, 10111, 21584, 16581, 11610, 10207
and 21592 ms. Result file:
`O:/winnt/Logs2/t423-s8-rpc9-idle-r1-idle-results.json`.
The connected-then-disconnect log records
`WOW STARTUP UNAUTHORIZED REJECTED` before CONNECTED/DISCONNECTED, proving
both denied calls passed through real typed RPC without an event leak or
successful flag. Tested ntsrv.exe SHA256:
FAEE345492997C3CC4CA5C78DF20DD8337E5857B6EEFE8D9DBD0B4E4504C1F49.
The runner's explicit marker assertion was added while the run was already
active; the running script used its pre-edit text. The client itself enforced
both return/output assertions, and the retained marker was independently read.
Do not claim this run executed the subsequently added runner assertion.

### InitTask notification failure ordering

The selected lifecycle init rejects an already-bound task (`binding->thread`)
before allocation. Therefore a failed required startup notification after USER
initialization must not run initialization again or report async success. The
registration callback returns FALSE with the notification error; original
wkman.c performs its bounded callback retries and then W32DestroyTask plus
host_ExitThread. The existing bound-thread guard rejects those repeat init
attempts without allocating duplicate tasks. W32DestroyTask owns ExitVDM and
WOWCleanup; runtime unbind owns the remaining USER lifecycle retirement. No
parallel destroy algorithm or whole-worker kill is added. This extends the
callback's standalone registration transaction, not a claim that USER's
xxxInitTask itself failed. Preserve separate trace stages for the distinction.
Skip notification for shared_id 0 (separate WOW) and UINT_MAX (guest-internal
task with no submitting BaseSrv record). Failure/retry/retirement and ordinary
shared WOW positive paths require verification before publication.

### Production callback and DLL linkage candidate

`registered_init_task` now reports successful shared-record initialization via
OpenNtBaseClientWowStarted, imported from ntvdm.exe. It preserves the distinct
InitTask and InitTaskNotification trace stages and follows the failure ordering
above. Both build generators register the x86 stdcall/export alias. No mirror,
guest, library, task scheduler or frontend code is changed by this wiring.

The formal worker and all selected WOW32 provider bodies link successfully in
the existing x86 /MT caches. Logs: `s8-wow-notify-worker-r1.log` in the formal
cache and `s8-wow-notify-provider-r1.log` in the WOW cache. Link maps resolve
_OpenNtBaseClientWowStarted@4 to worker rpc_client.obj and the DLL import to
ntvdm.exe, not a separately connected DLL client. SHA256:

- registration bridge: 1084C5EFAA2DB7CCDE7931FF794CCB285E10B3FA7F08CC826633D0E67894FB33
- ntvdm.exe: 8BA2FFF5976AA721568BC550824C53C424CA082C12DEBF0304D68A7C8AF70593
- wow32.dll: 95954D650C50CEAFD7F96F9B23E5BB3D1CB1ECDC68BBC07FFB9E46981DCA55A4

This is production-path wiring and compile/link evidence only. The candidate
is not published. Real InitTask notification and injected failure retirement
remain unverified, as do launcher consumption, original WOWEXEC no-op and
reused-worker integration. Prior service/negative-RPC passes cannot certify
this new callback's runtime behavior. Complete those rows before the joint P.

### Launcher consumption and first real shared-WOW notification

run16 now uses its explicit wait option for both GUI families. Default Win16
launch waits on the authenticated startup event and original parent event;
new-worker launch also observes the retained worker process. It re-queries the
latch after wake to preserve immediate-exit ordering. With no successful latch,
original zero completion maps to generic ERROR_DLL_INIT_FAILED, not an invented
guest LoadModule code. Worker death remains ERROR_PROCESS_ABORTED. `--wait`
retains the original completion route. Redundant WOWEXEC's original successful
no-op still needs explicit disposition before acceptance.

Rebuilt the six formal targets plus the WOW DLL and copied the consistent
seven-file candidate with unchanged S7 guest/configuration to
`build/M0-T423/S8/p`, accessible by test-only T: mapping. No live O:/winnt
files changed. Candidate run16 SHA256:
9003161B3011270CFD6BF0C9371416E3621C1047112D5A254FBE935C62AD38FD.

Private-desktop `observe-wow-frontiers.ps1` WINMINE run
`t423-s8-wow-async-r2` uses `-PostExitObservationMs 5000`, reusing the existing
observer's bounded post-exit retention environment option. It records launcher
exit 0, worker PID 22120 still active after launcher's completion, and at 8042
ms a visible WINMINE main window (UTF16 class/title 00C900A800C000D7), not only
WOWExec. The registration log records InitTask success then
`InitTaskNotification result=00000000`. Logs are under O:/winnt/Logs2 with that
prefix. This proves real guest InitTask -> worker RPC -> broker latch ->
launcher async return and retained visible application; it does not prove
gameplay, reused-worker admission or failure cleanup. The earlier r1 returned
0 but lacked post-exit window observation, so it is weaker evidence.

The first explicit-wait observation `t423-s8-wow-wait-r1` failed because the
script passed `--wait WINMINE.EXE` as one quoted target argument. It created a
native fallback rather than a WOW worker and exited 1; this is a fixture failure,
not a product or guest pass. The script now supplies separate argument tokens;
all three failed logs are retained, with corrected r2 as independent evidence.

Corrected `t423-s8-wow-wait-r2` completed all three private-desktop observations.
WINMINE retained its visible main window at 8/12/16-second samples while the
explicit-wait launcher remained pending until the observer's 20-second bound.
SOL retained its original localized out-of-memory modal; WRITE retained the
Write dialog with `Not enough memory for Write to complete this operation.`
All three reports are bounded `result=timeout`, not successful task completion
or gameplay passes. Thus window/frontier retention is observed, while normal
close/result return and failure-dialog dismissal still require separate tests.
The script cleans up only the selected staged package processes. SYSTEM.INI
remains identical to S7, SHA256
F20A94235712C26B0499D61E7CC080B612BBFE230DE0C65A6832494FDBB1ACAA.

### Reused WOW service and retained execution regressions

Extended the existing service fixture to submit a second original CheckWOW
request on the same live WOWHead. It requires VDM_PRESENT_AND_READY, a fresh
task and parent receipt, an initially unsignalled startup latch, rejection of
the previous receipt, successful original Get/notification, and independent
ExitVDM completion. No test-created task record or second worker substitutes
for reuse. Default run `t423-s8-wow-reuse-r1` passes. The late-query mode and
existing launcher-exit-survival, launcher-disconnect-survival,
unfinished-worker-exit and completed-worker-exit modes all exit 0 in
`t423-s8-wow-reuse-r2` logs below O:/winnt/Logs2. Those four lifetime cases
retain their original DOS scope; they are not new real-WOW rundown evidence.
Fixture source SHA256:
11EBFDE8B3916EF09386A83E99E5E44BACBA05282CA6554BF48C0AB3DF70FA41;
fixture EXE:
EA2588C41929614B58845FB08C69A3E15B3E562D7209D033FE8205C3D6F6852C.

The twelve-target chain fixture now explicitly supplies --wait at every GUI
launch, including the root launch. It retains all 24 ENTER/RETURN, two frontend
identities, result and per-stage input/output assertions; default asynchronous
GUI evidence is separate. Protocol-9 chain fixtures are incrementally rebuilt.
Both real twelve-target chains passed with exit 0:
GGGWDWGGGDWD and GGGDDWGGGWWD, including all retained nesting, original DOS
identity, return-code, two-frontend topology, visible-input/output and retirement
assertions. Generated plans are retained in `build/M0-T423/S8/chain-r1` and
runtime logs below O:/winnt/Logs2 with prefix `t423-s8-chain-r1`. This establishes
explicit GUI waiting preserves the existing mixed nested execution contract;
it does not replace still-open WOW reuse/failure or ordinary DOS17 gates.

### Real shared-WOW probe: retained failure, not reuse acceptance

The private-desktop `tests/observation/wow_shared_launch_test.c` probe uses
the staged T:/ package and its physical build/M0-T423/S8/p identity. Run
`t423-s8-wow-shared-r1` below O:/winnt/Logs2 reports the first asynchronous
launcher returned successfully with WINMINE worker 33388 still alive, then
fails its second-main-window assertion (line 98). The registration trace
does not contain a second application InitTask notification. Its printed
GetLastError 4390 is not established as the failing operation's error and
must not be used as a causal diagnosis. The harness retired its retained
owned process handles; it did not interact with the owner's desktop.

This is an unresolved production/fixture-path failure, not proof of a guest
single-instance limitation and not a pass based on the service fixture.
Next investigation must distinguish second-launch completion/state, original
WOWExec command notification/delivery, and window-observation assumptions.
The S8 joint production P remains pending full verification. The side-chat
idle repair remains accepted for inclusion: its reviewed broker hash
FAEE345492997C3CC4CA5C78DF20DD8337E5857B6EEFE8D9DBD0B4E4504C1F49
matches the protocol-9 seven-scenario RPC evidence recorded above. No
candidate replaces O:/winnt's accepted S7 package at this point.

### Shared-WOW notification narrowing

Private-desktop rerun `t423-s8-wow-shared-r2` records successful authenticated
WOWExec registration, a second Check returning READY (state 4), task 2 and a
still-running second launcher (259). There is no subsequent worker Get call.
The same desktop contains the existing WINMINE and Shared WOWExec windows.
Diagnostic-only r3 reposts WM_USER directly to WOWExec after the failed wait:
PostMessage succeeds (error 0), but no second main window appears and no new
Get is observed. The diagnostic option always retains failure, even if a
repost recovers; it cannot substitute for the ordinary broker notification.

Original wow16/test/shell/wowexec.c skips PeekMessage when
WowWaitForMsgAndEvent returns TRUE (hardware servicing). Original USER
taskman.c checks client fsChangeBits before selecting a hardware-only wake.
The current lifecycle binding does not populate that posted-message field.
This is a source-grounded candidate cause, not yet a proven repair. A focused
fixture now supplies native posted input together with the original pending
hardware-wake flag and requires the message-processing FALSE result.

The reused fixture runner initially lacked a selected support object, then
the new test used a macro private to the original taskman compilation, then
linking exposed four frontend-client dependencies missing from the old test
composition. These are build failures, not executed functional tests. The
fixture now uses the documented original flag value and the runner composes
the selected production RPC client, stub and transport libraries, building
its declared dependencies. The before-fix r4 run executes and fails exactly
the posted-plus-hardware result assertion (fixture line 510), with
`WOW_USER_TASK_LIFECYCLE errors=1 native_direct=4`. The printed last-error is
incidental: the demonstrated defect is the returned TRUE instead of FALSE.

The candidate extends the existing ADAPTER-WOW-051 native queue binding:
before original xxxSleepTask, replace the worker-local client fsChangeBits
with LOWORD(GetQueueStatus(QS_ALLINPUT)). The original taskman translation
unit remains composed and unchanged; its message-before-hardware decision
is retained. Direct use of NT4 kernel queue storage is unavailable on modern
USER, so the smallest same-shaped data projection is selected. No scheduler,
SMS queue, guest patch, mirror intrusion or new event policy is introduced;
later recovery of that native queue-view boundary would replace this binding.
A second fixture assertion requires hardware-only wake still return TRUE
after the posted message is consumed, excluding an unconditional FALSE fix.
The initial after-fix build named the wrong local client structure; it failed
compilation and was corrected to the original-facing thread.pcti queue view.
After-fix r2 passes `WOW_USER_TASK_LIFECYCLE errors=0 native_direct=4` and
`WOW_FIXTURE_OK case=task-lifecycle`, including posted-plus-hardware and
hardware-only outcomes. Logs are under O:/winnt/Logs2/t423-s8-wow-wake-after-r2.log.

The production DLL was incrementally linked, then updated only in the build
staging package; its SHA256 is
56193AC5D5BF2BC3A7D4233D6FD986EE4AFE43874BBA95B9F39FAD1C8149BA2E.
Real `t423-s8-wow-shared-r4` still fails: first async launch survives, second
launcher 18876 waits for task 2 in worker 30756 without a new Get or second
main window. Thus the focused queue-view gap is reproduced and its fixture
passes with the candidate, but it is insufficient to repair real shared-WOW
delivery. Continue auditing native queue observation/clearing and WOWExec
dispatch before accepting this binding. No ordinary O:/winnt publication,
commit or S8 closure is claimed. Both real-test sessions have completed and
their owned processes were cleaned up by the harness.

### Pending native posts: source-backed correction and real closure

Diagnostic r5 captured WOWExec's queue snapshot 0x00080000: native posted
input remained pending but native change bits had already been consumed.
Its HWND and waiting thread identity matched. The LOWORD-only candidate was
therefore insufficient. Extended fixture `t423-s8-wow-seen-before-r1` first
observes but does not remove a native post, then supplies hardware wake;
it fails the required FALSE message-processing result with errors=1.

The finite ADAPTER-WOW-051 binding now supplies LOWORD(queue_status) plus
pending QS_POSTMESSAGE from HIWORD specifically to WOWEXEC's original
xxxSleepTask call. Source rationale: taskman.c reads its client queue before
hardware selection; original queue.c::xxxSleepThread restores examined but
unconsumed bits from fsWakeBits/fsChangeBitsRemoved after receive processing.
Modern native queue observers and the original guest do not share that
private accounting, so a native old-but-unremoved post must remain visible
to WOWEXEC's unfiltered PeekMessage. Snapshot replacement removes consumed
posts; generic filtered Get/Peek and WaitMessage implementations are unchanged.
No original mirror code, guest, scheduler or secondary message queue is added.
`t423-s8-wow-seen-after-r1` passes the complete lifecycle fixture, including
old-post-plus-hardware and consumed-post/hardware-only cases. Temporary queue
and message diagnostic logging used by r5/r6 was removed after diagnosis.

Real r6 now reaches WM_USER dispatch, GetNextVDMCommand, a second successful
InitTask notification and task-2 ExitVDM; the second launcher returns 0.
Its two-window assertion remained false. Read-only original
`O:/repos.external/OpenNT-4.5/nt/private/windows/ep/winmine/winmine.c` MMain,
WIN16 branch lines 124-133, explicitly tests hPrevInstance, activates the
existing window and returns FALSE. Source SHA256:
83796E8EC74E2710B39D906EDD290F12FBB02A5D63F78CCFB7C7F5D6F533C1F7.
That agrees with real second InitTask/normal retirement and the surviving
first window; two simultaneous WINMINE windows are not the original contract.

The checked-in shared-launch fixture now verifies the source-defined
single-instance result, same surviving first HWND/worker, and independent
completion. Its --wait-first case also requires the first launcher remain
pending after the second returns and return 0 only after its own WM_CLOSE.
R7 async passed; wait-first exposed a fixture race enumerating an initially
empty desktop before the launcher ran. The observer now resets last error
and treats empty enumeration with no error as pending, retaining actual
enumeration errors. No product code changed for that fixture correction.

Both `t423-s8-wow-shared-r8-async` and `t423-s8-wow-shared-r8-wait` pass on
the same cleaned-up candidate, each with two InitTaskNotification success
records. The wait case additionally records task 2 completion and launcher
exit before task 1 completion and its launcher exit. Runtime logs are under
O:/winnt/Logs2; no user's desktop was switched or injected. Invocation:
`build/M0-T423/S8/wow-shared-launch-test.exe T:/ <physical S8/p>` and the same
with `--wait-first`. The supervisor launches/retains its own broker and
cleans only its own recorded process handles. Exact tested identities:

- lifecycle source: 98C217840AF33206AF97B3CC87B7DF390935D152E4172C33B3F3116D172C7F5D;
- shared-launch fixture: 89AE4A48F7857D72EBACCBE179135F6F338610E4BBDEE6E477A3C5537E3207DD;
- staged WOW32.DLL: D3D49CFDEC6A35DC1C926FCF2A0F806C6BDA45B17F98E899CB1A95DA7FE197C1.

This closes the reproduced shared-WOW command delivery/start/wait case, not
all S8 gates: startup failures/no-op, cumulative regression and production
publication remain pending. O:/winnt's accepted S7 package is unchanged.

After temporary diagnostic removal, the final source also passes both
`t423-s8-queue-final-task-lifecycle` and `t423-s8-queue-final-native-seen`
fixtures through the selected production objects. The latter retains native
synchronous-send handling after its queue status was already observed:
`WOW_NATIVE_SEND_RETURN early=0 result=114`, errors=0, and the case's explicit
completion marker. Both logs remain below O:/winnt/Logs2. The fixture build
runner batches dependency requests to reuse the same x86 graph and avoid
re-entering the toolchain once per library; its deliberate pre-existing
fixture-only duplicate-symbol link warnings remain visible, not evidence of
a clean production link or a replacement for real guest acceptance.

### Original redundant-WOWEXEC disposition binding

Register MVDM-HOST-DIV-319 before implementation. Original wkman.c already
classifies and completes a redundant shared WOWEXEC command without InitTask.
The new CLI startup acknowledgement otherwise mistakes that source-defined
successful no-op for failed initialization. Do not duplicate its name test in
run16 or BaseSrv, and do not treat all unacknowledged completions as success.

Recovery ladder: the whole original wkman translation unit remains composed;
the original skip/ExitVDM ordering is retained. A pure external adapter cannot
distinguish this branch from failed-exec using the shared ExitVDM arguments.
Therefore one registered original-owner hook before its existing ExitVDM
reports the accepted no-op through the already-authenticated startup receipt.
The implementation body stays in wow32-dll's registration bridge, not the
mirror. No new IPC, name classifier, scheduler, guest or private USER import
is needed. Earlier NT4 USER startup internals cannot be composed due to kernel
thread/queue dependencies. A failed notification leaves the parent without a
positive receipt; original ExitVDM still completes the record and the CLI
reports failure, never false success. This boolean acknowledges an accepted
request (InitTask or original no-op), not proof of a new task/window.

Required proof: real repeated lowercase system32/wowexec.exe in a shared
worker returns 0 without another InitTask, retains the existing application,
and traces the no-op acknowledgement. A missing/bad ordinary application must
not obtain that success. The hook can be removed if a future original-shaped
USER startup binding exposes the same no-op disposition without interception.

The registered hook is implemented as three added mirror lines (two comment
lines and one binding call), with no name test copied out of wkman.c. The
existing mirror is LF in HEAD while upstream is CRLF; an attempted global
CRLF conversion was discarded to avoid a full-file formatting delta. The
retained diff is exactly +3/-0 and leaves every pre-existing byte unchanged
outside the insertion. This is not a claim of byte-identical upstream text.

Both real `t423-s8-wow-noop-r1-async` and `t423-s8-wow-noop-r1-wait` pass:
each has exactly two successful InitTaskNotification entries, followed by
one successful StartupNoOpNotification for lowercase system32/wowexec.exe.
The same first WINMINE HWND and worker survive the no-op; wait-first remains
pending until closing that window, then returns 0. Logs are under
O:/winnt/Logs2; only the build candidate DLL changed. Tested DLL SHA256:
0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A;
wkman source B822C23447348C40C7D38DA7328D30F1EB6865511A57CD1B1679F70B1B43492D;
shared fixture 75311A21A8A7DFD1549CCDCB52A9551736A98F816ADF5A2196FA54969F2557E1.

The original service fixture now also completes a third dispatched WOW task
without any startup notification. It requires the parent completion event
to signal, the startup event to remain unsignalled with FALSE latch, and a
late worker report to be rejected after original record removal. Build r1
failed because a fixture-local variable was out of scope; its subsequent run
used the older binary and is explicitly not evidence for the new assertion.
Corrected build/run `t423-s8-wow-failed-receipt-r2` exits 0 and contains the
new required marker `PASS failed WOW completion has no startup acknowledgement;
late report rejected`. This tests original failed-exec service semantics,
not yet a real malformed guest image and dialog-dismissal path.

Current staged run16 also passes the 19 option cases and four native GUI
cases in `t423-s8-native-final-options-r1` / `t423-s8-native-final-wait-r1`:
default launch returns 0 with live target, explicit wait returns 37, --wait --
retains that result, and killing the waiting launcher with 96 leaves the
target alive to finish with 37. Real startup fault/caller-shell cases and
the cumulative production-P gates remain open.

## Final caller-context and startup-fault refresh

The x86 /MT native GUI fixture was rebuilt from current source and passed
`t423-s8-native-shell-r2.log` under O:/winnt/Logs2 with
`RUN16-GUI-WAIT-PASS cases=16`. In addition to direct launch, explicit wait,
launcher-loss survival and image-error cases, it exercises CMD /c, a generated
batch caller and CMD's input loop without /c. Each shell case checks default
return 0 while the GUI target survives, or explicit-wait return 37 only after
target release. CMD input is supplied through an owned pipe on an unswitched
private desktop; this is not a physical-keyboard/focus observation. These
native cases do not substitute for DOS COMMAND or Win16 caller-context proof.

Exact identities for that run:

- fixture source: 9070BE414B4ED7FB49F5D637A3DE355D2AA6CAF87E5C116D91343E5D9DEEAF6D;
- supervisor: 931CF21F315D718A8F82DEBB8FFBC0603E2A31C0DDC3CFE7ADAFE79C416DA9A5;
- GUI target: A16E35B69A893E975095B432E0E5E1403D5728BAAD65152239EB1F452E3AAEA3.

The final loader-recovery version of `wow_startup_fault_test.c` was also
rerun in both process-loss modes. Invoke
`build/M0-T423/S8/wow-startup-fault-test.exe T:/ <physical S8/p> worker`
or replace the last argument with `broker`, with no pre-existing broker.
Logs `t423-s8-wow-final-worker-r1.log` and
`t423-s8-wow-final-broker-r1.log` both contain
`NEW-WOW-STARTUP-FAULT-PASS`. Worker injection yields launcher 1067 and owned
worker exit 97; broker injection yields launcher and worker exit 1722.
The fixture observes actual debugger EXIT_PROCESS events, not merely a wait
timeout. In the broker run the process HANDLE was not yet signalled at the
immediate query despite its recorded exit event/code; that transient query
is not used to claim a surviving worker. No owner desktop was switched and
only fixture-owned handles were terminated.

Fault source SHA256 is
9B58B27D7E7B5F7AF8EC1B64BBF754888D7D43757C5FCAC685634CCD7C3E4263;
fixture executable SHA256 is
244EFA9305FD36E1E223964F6413C763A0474E07983873A0DD9243D15ABFE61F.
These results refresh the final test-source identity; they do not close
remaining prompt/context, cumulative publication or delivery gates.

### Final twelve-target chain refresh and parser correction

`verify-twelve-target-chain.ps1` was run with the S7 renamed observer,
S1/restart-formal-x86 build cache, T:/ candidate, evidence root
`build/M0-T423/S8/chain-final-r2`, and Logs2 prefix
`t423-s8-chain-final-r2`. Both actual twelve-target chains completed their
24-event nesting, direct results, character-session identity, visible I/O,
live topology and natural frontend/helper retirement checks. The resulting
summary contains distinct frontend pairs 34900:2 / 12016:13 for WDW-DWD and
32408:38 / 38188:52 for DDW-WWD.

The runner exited 1 in the second post-run record audit: Windows PowerShell's
ConvertFrom-Json emitted the two-record array as one pipeline object, and the
extra @() wrapper retained a nested array. Member enumeration then joined two
EventsPath values into one invalid pathname. This was a test parser defect,
not an observed product failure. Assigning the decoded value directly lets
foreach enumerate its records in both singleton and multi-record summaries.
The corrected `verify-twelve-chain-records.ps1 -EvidenceRoot
build/M0-T423/S8/chain-final-r2` passes both cases against the retained actual
snapshots, including its existing malformed/unfinished-record negative
controls. The original exit-1 runner log is retained; it is not rewritten as
a successful invocation. No product code or runtime assertion was relaxed.

### Win16 shell caller-context matrix

Extend the retained shared-WOW fixture rather than introducing another
provider. Its optional caller argument wraps only the first real run16 launch;
the existing shared-worker, single-instance WINMINE, redundant WOWEXEC and
independent first-task completion assertions remain intact.

The x86 /MT build `build/M0-T423/S8/wow-shell-build-r1.log` completed (the
existing signed/unsigned comparison warning remains recorded). Invoke
`build/M0-T423/S8/wow-shared-launch-test.exe T:/ <physical S8/p>` followed by
each combination of `--async` / `--wait-first` and `--cmd-c` / `--cmd-input` /
`--batch`. All six runs passed with Logs2 prefix `t423-s8-wow-shell-r1`:

- CMD /c: asynchronous shell completion with surviving WINMINE, and explicit
  wait until that application's own window closes;
- CMD input loop without /c: the same two contracts, through inherited test
  input pipe rather than physical keyboard injection;
- generated batch caller: the same two contracts, including propagation of
  the actual run16 result through `exit /b %errorlevel%`.

Every run additionally checks the original second WINMINE activation/no-new-
window behavior and redundant WOWEXEC acknowledgement on the same worker.
Private desktops remain unswitched. The generated batch exists only under
the build-backed candidate's tests directory and is removed after its caller
has completed. Original guest bytes and production artifacts are unchanged.
These six runs used fixture source
0E6BB07CF006F9662665FABA52FE24D41CBA9383CB62DAFA5C6923878F50BB89 and
executable F27BD919BE319A28C62FAA8CB16D26B85020BFC7832D8F74E15C919DB8A1F86E.
The subsequent DOS COMMAND caller extension has a separate build/run identity;
these shell results do not themselves prove that guest shell route.

### Original DOS COMMAND single-command caller

The same fixture now accepts `--dos-c` after `--async` or `--wait-first`.
It starts the real outer run16 / original COMMAND.COM /c / inner run16 /
WINMINE chain. Initial r1/r2 correctly failed the required WINMINE-window
assertion despite outer exit 0. R2's retained RPC trace shows the fixture
generated the /c command with two backslashes between the package drive
colon and run16.exe, followed by WINMINE.EXE;
there was no successful inner launch. Native CreateProcess tolerated that
root-join spelling elsewhere, but it was invalid for this DOS caller test.
The fixture now adds a separator only when the supplied root lacks one.
No product or original COMMAND behavior was changed, and exit 0 alone remains
insufficient evidence.

Build `wow-shell-build-r4.log` and runtime Logs2 files
`t423-s8-wow-dos-r3-async.log`, `t423-s8-wow-dos-r3.rpc.log`,
`t423-s8-wow-dos-r3-wait.log`, and `t423-s8-wow-dos-r3-wait.rpc.log`
prove both corrected cases. Async completes the DOS caller with WINMINE alive;
explicit wait keeps the caller pending until the first WINMINE closes. Both
retain shared-WOW single-instance and redundant-WOWEXEC checks. The outer
COMMAND /c result is its original 0, not a fabricated propagation of every
child exit code. Current fixture source SHA256:
EEEECF8D2920AEEA3619212B8C790B7D58FDEB003A9DE0D79F01A5F36394CCE9;
executable SHA256:
6223E367296CE78BC4AEAB87E20BA3B3871A3B6EEC7E39861343E2FAC5360B72.

This closes the tested single-command Win16 route, not persistent interactive
DOS prompt recovery, native-GUI DOS callers, or coherent publication. Those
remain distinct acceptance work; earlier failure evidence is retained.

### Persistent DOS prompt recovery, not merely COMMAND /c completion

Both GUI fixtures reuse the retained S7 `observer-renamed.exe`, launched on
the supervisor's unswitched private desktop with its own Console. They supply
three actual input lines: launch the GUI via run16, MEM, then EXIT. They
require `scripted-console-input=delivered` and the guest Console text witness
`bytes total conventional memory`, not only a successful observer exit.
The immutable COMMAND's final result remains 1 in these interactive routes;
the observer's own success code is not substituted for that guest result.

WOW invocation adds `--async --dos-input` or `--wait-first --dos-input` to
the shared-launch fixture. Set WOW_TEST_OBSERVER to the S7 observer and
WOW_TEST_REPORT to a fresh Logs2 report. Both
`t423-s8-wow-prompt-r1-async` and `t423-s8-wow-prompt-r1-wait` pass, including
their actual MEM text, original shared-WOW identity/no-op assertions and
first-window completion. The asynchronous case executes MEM and exits the
DOS prompt while the original WINMINE window/worker still survive. Source
24EBC84BC595D4C06350826BC5D68D966ECA38FC913EA723E251606109846838;
fixture 13316A7FF13D72B728510C2963FB687A04768EE4B7BE3B5128A25B257C4813DC.

Native invocation uses the existing `run16-gui-wait-test.exe`, short candidate
launcher and `tests/GW8.EXE` authored GUI target, followed by `--dos-async` or
`--dos-wait`. GUI_TEST_OBSERVER and GUI_TEST_REPORT select the same observer
and a fresh Logs2 report; optional GUI_TEST_TARGET_LOG records target return
and received argument character codes. Test target copies stay under the
build-backed candidate tests directory, not among original guest media.

Native r1-r3 fail the inherited 10-second readiness budget. R2's wide/binary
diagnostic initially suggested an argument terminator issue; r3's explicit
character-code evidence disproves that inference (last two units both 0031)
and records target result 37 after cleanup released it. Extending only the
DOS readiness budget to 30 seconds and the bounded target hold to 90 seconds
allows r4 async to pass. R4 wait then fails the separate inherited 5-second
observer completion deadline; its retained final report nevertheless shows
MEM and EXIT completed. R5 extends that DOS observer completion wait to its
existing 60-second observation budget, retaining all result/content checks.

R5 also strengthens explicit waiting: wait six seconds after target readiness
(longer than paced observer input), require the caller still pending and no
MEM result in its second-line snapshot, release the GUI target, then require
target exit 37 and actual MEM output. `t423-s8-native-prompt-r5-wait.log`
passes those checks. This is test timing/negative-assertion repair, not a
product behavior change. Native r4 async and r5 wait are distinct identities;
the final fixture still needs its consolidated rerun before delivery.
Current native source: 5D96410A0F460EEEDD396F5C160934AE13216C9415389B907898D1F4BB3F442B;
supervisor: 544166A9D20DFB22540BB18E543F99A02CC75AABA9AE3FD9D45878705FBC5D29;
target: 6C6F87A7612E7F2E95347EFCD88C08B19FE69BED9D2656E657A2C32245B8B2D4.

The same strengthened six-second pending/no-MEM-before-close assertion now
also passes for Win16 in `t423-s8-wow-prompt-r2-wait.log`, followed by MEM
after WM_CLOSE. WOW source:
15524F78C355DDBB5EEE0D7FCB62AD3F6B0DFAC0B153914AE97BFEA40F1A943C;
fixture: B20A1629C90E1F19E6C9E66615A950B7BED90C89394FDB82FFD385FAC0B7F1C7.

The final native identities above subsequently pass the consolidated 16-case
`t423-s8-native-shell-r3.log` and asynchronous DOS prompt run
`t423-s8-native-prompt-r5-async.log`, in addition to r5 wait. The combined
runner initially stopped before launching the prompt case because a broker
still existed after the image-error controls. A subsequent process query
confirmed no product processes remained; only then was the not-yet-started
prompt case executed with its fresh report. No active test was restarted,
and no unknown broker/session was adopted or terminated.

### S8 cumulative mouse and graphics refresh

The retained build-local S7 mouse orchestration is now reproducible as
`tests/observation/verify-window-mouse.ps1`. It preserves all five original
real-guest assertions, uses an unswitched private desktop, refuses a
pre-existing broker and restores observer control environment variables.
Invocation: S7 observer-renamed.exe, PackageRoot T:/, existing immutable
O:/winnt/tests fixtures and LogPrefix `t423-s8-mouse-r1`. All five cases pass:
count, graphics drawing/erasing, text cursor, movement/button release and
source retirement. Each requires zero guest exit and its captured PASS marker.
The separate no-input run `t423-s8-mouse-negative-r1` requires and obtains
WINDOW-MOUSE-FAIL with original stage-2 exit, proving missing input is not
silently accepted.

Probe identities (SHA256): MCOUNT.COM
11314D379848BE456ED0D01BF9E54C2D44896E674297E7BAF9179D21519F7662;
MCVIDEO.COM 642B672E5D8087E761B6E1616214BDFC3A5661916211F6494CC27C1A0CA45955;
MCTEXT.COM D918F3877A74A0137BDB41620A8A2AF7873041D0ED0351E4DAE73B76A5E87A2D;
WMS7.COM A60EDF938A866810CC3BD3D92800E72A5BEB73369C12580A14F1BEA8BA337E6A;
S7MOUSE.dll E756BBDC2C7F3FE85ADEBD46871A4D4CC7548C50594FA88CD56F9D665B7C01A9.

`Verify-CommandExitStatus.ps1`, ordinary frontend, direct-graphics-return,
retained S6 graphics-handshake-r2/VIDTST.COM, PRIVATE_DESKTOP=1 and
GRAPHICS_RETURN=1 passes `t423-s8-graphics-return-r1`. Its graphics report
asserts automatic Window, X preserving the graphics Window/frontend, and
mode-3 text closing Window while frontend survives; guest exits zero.

The current-source component17-r3 native-console-frontend report explicitly
retains its real ReadConsoleInput mouse witness: capture gesture excluded,
left/right pairs and focus release, alongside stalled-helper and return-I/O
tests. It is not inferred merely from the generic component17 summary.

Console retention run `t423-s8-console-mouse-r1` uses the S7 keymouse-r32
observer, TEST_RUNTIME_ROOT=T:/, SHARED_CONSOLE=1, FOCUS_TRANSITION=1 and
COMMAND=O:/winnt/tests/KMTST.COM under the existing MVDM_TEST_KEYMOUSE_ names.
The top-level private-desktop observer records exit zero. The guest report
contains all nine original capability markers and
`keymouse passed=yes stage=command-exit error=0 exit=1
focus-record-transition=injected`. This is injected Console records, not
physical focus/hardware observation. Its roughly 1 MiB flattened screen row
makes full-line terminal printing unsuitable; bounded marker extraction was
used to verify the actual report after all product processes had exited.
Observer hashes: top-level
3B325872BE4E510A1AEFDF70ED12346EA0286CE91C494A90AA8A8832718F1F10;
keymouse observer D54794619A39E8CC4E4898370DD4194065D9F09A7A157D2D1AE81E550B6552C2;
KMTST.COM 92C642D5F92EC4EA8C255D956977B5075C068B26423287CA4BDB66F81A8F6629.

Both formal product and WOW Ninja graphs report no pending work. All seven
staged artifacts match their formal outputs, including ntmon.exe. No shared
library or guest changed; mirror delta remains wkman.c +3/-0 plus four README
registration lines. Publication, final source review and pushed delivery are
still pending; these runtime results do not themselves close S8.

## Reviewed coherent publication checkpoint

Final review rechecked the complete production diff: run16's exact prefix
parser/target-tail preservation and GUI wait split; authenticated WOW startup
receipt/latch and task-plus-parent-receipt matching; wait-only event export
and disconnect cleanup; original redundant-WOWEXEC hook; WOWEXEC-only native
pending-post binding; side-chat empty-broker locking, pending Connect hold,
failure cleanup and ten-second empty-only retention. Original DOS record
policy remains unchanged. The checked-in source/build/current-test sweep has
no old version-8 RPC symbol or monitor.exe runtime reference. Historical
records are retained, not rewritten. Side-chat seven-case real RPC evidence
remains `t423-s8-sidechat-review-r3-idle-results.json`, all passed.

`build/M0-T423/S8/publish-s8.ps1` verified no live package process, exact seven
candidate hashes and unchanged SYSTEM.INI/CONFIG.NT/AUTOEXEC.NT. It backed up
the complete previous seven-file package and present configuration under
`build/M0-T423/S8/pre-publication`, then copied and verified the complete S8
set at O:/winnt. Only backed-up monitor.exe was retired; ntmon.exe is the sole
new monitor executable name. NTVDM.REG and guest/configuration were preserved.
The recoverable before/after hash manifest is pre-publication/publication.json.
All file hashes equal the candidate/formal identities already recorded above.
Published-path regression completed: `t423-s8-published-console17-r1`
passes all 17 text-gated DOS cases; `t423-s8-published-window-r1` passes MEM,
nested MEM and EDIT. `t423-s8-published-wow-r1` observes each application
separately: visible WINMINE class 00C900A800C000D7, SOL's inherited OOM
text 00C400DA00B400E600B200BB00B900BB, and WRITE's unchanged "Not enough
memory for Write to complete this operation." These preserve the baseline;
SOL/WRITE remain limitations, not functional passes. Runner exit zero alone
was not used as application acceptance.

Final boundary review verified all 44 imported library files and LICENSE.nxvm
against the import manifest, and no guest-root diff. Source ownership checks
and all five leakage-negative controls pass. S8 P2 is ready for commit/push;
this checkpoint does not itself claim committed delivery or T closure.

## Closure checklist

- [x] Record the tested S7 baseline and source ownership of shell wait policy.
- [x] Distinguish shared-WOW completion, startup notification and final result.
- [x] Finalize/implement explicit wait parsing without changing target tails.
- [x] Complete source-first authenticated Win16 load/failure acknowledgement
  design, new/reused worker cleanup and early-completion races.
- [x] Implement the bounded Win32/Win16 GUI launch/wait contract, reusing
  original owners; register any necessary semantic-carrier exception first.
- [x] Real sync/async GUI success/failure, caller/broker/worker failure, target
  survival, prompt availability, batch/CMD-c and nested result tests.
- [x] Retain component, DOS17, mouse/display, both chain/fault matrices and
  three separate WOW frontiers; publish a coherent verified seven-file set.
- [ ] Review, governance, commit/push and clean-state closure.

Research findings alone are not runtime acceptance. The checked rows above
refer to the subsequent real runtime evidence, including explicit limitations.
S8 remains active pending committed delivery and closure registration. The reviewed S8
package is at O:/winnt; its preceding S7 package is recoverably backed up.
