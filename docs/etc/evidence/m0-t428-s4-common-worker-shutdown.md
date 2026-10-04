# T428 S4 common worker shutdown

## Source boundary and candidate

Baseline: S3 ae28ef7ce, APP0.0.427/RPC38/I/O25. Original MVDM/OpenNT mirrors
are not changed. Implementation, verification and publication are complete;
the reviewed P is recorded in CURRENT. T428 remains open.

| Logic / source | Existing owner | Sharing decision / necessary independent operation |
| --- | --- | --- |
| Authenticated service connection, broker loss and shutdown event; project adaptation | worker-base connection and NTSRV worker registry | Existing shared paths retained. |
| Run local close callback, bounded existing close grace, actual success acknowledgment and carrier exit; project adaptation | NTVDM console watcher / NTVWM presentation watcher | Extracted into worker-base with explicit callback, grace, result and acknowledgment ownership. No new lifetime policy. |
| Original DOS close handler, session bind and forced Console-close semantics | NTVDM adapter calls original CntrlHandler | Original handler remains in place; existing project five-second handler grace and CONTROL_C_EXIT fallback retained. |
| Native true Console-session close | NTVWM console_state | Remains worker-local. Only successful real closure signals closed; failure ends carrier with its error, not a fabricated acknowledgment. |
| Native management controls formerly first-text-bind-only | NTSRV RegisterNativeBackend / NTVWM main | Register existing controls at worker initialization without frontend capability; subsequent text binding retains the same worker-owned events and watcher. Authenticated claimed native reservation remains mandatory. |
| Explicit DOS/WOW management termination versus native Console-close acknowledgment | NTSRV lifecycle | Existing operations retained. No new mandatory cooperative-then-force policy; WOW never requests text I/O just for management. |

NULL frontend is an existing nullable RPC handle. No wire shape, DTO or version
changes. It authorizes close registration, not text I/O or root ownership.
Control/events stay pinned under the service lock; callbacks remain worker-local.
The existing native 30ms presentation interval is unchanged.

## Verification contract

New production-linked `worker-shutdown-test.exe` exercises actual process exit,
successful and failed acknowledgment, blocked handler and existing VDM fallback.
The native-worker service fixture adds authenticated pre-text registration,
wrong-generation, event-alias, signaled-event and duplicate-registration checks.
`verify-native-gui-routing.ps1 -MonitorRpc` adds a GUI-only carrier management
close while retaining its independent live GUI target and real exit37.

Initial graph generation failed without explicit Node22; retained in
build/M0-T428/S4/r001/graph.log. The retry uses the same pinned Node22 as S3.
Build/runtime/publication/review results follow below.

## Focused results

| Gate / reproducible entrypoint | Evidence | Actual result |
| --- | --- | --- |
| `New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot build/M0-T427/S2/r001 -NodeExecutable C:/Users/neko/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/bin/node.exe`; r001 build.cmd | r001 graph-retry/build-elevated/build-review logs | Affected x86 /MT CCPU40 links and new production-linked shutdown test passed. Existing mirror/compiler warnings remain. |
| `build/M0-T427/S2/r001/worker-shutdown-test.exe` | r002/shutdown.txt | 30 assertions passed: inline/threaded success, callback error, handler timeout, invalid acknowledgment handle, VDM forced-close fallback. Actual process exit and event state checked; no RPC/provider substitutes. |
| Same production-linked test after handle-ownership review | r010/shutdown-final.txt; r001/build-final-test.log | 37 assertions passed, including no handle growth across repeated success/failure/timeout cases after the asserted first Windows process-creation setup. Before119/after119. Only the test changed; eight production files retained their tested/published hashes. |
| `verify-service-fixtures.ps1 -Observer build/M0-T427/S4/r049/console-startup-observer.exe -Fixture build/M0-T427/S2/r001/basesrv-service-reservation-test.exe -LogRoot build/M0-T428/S4/r010` | r010 timings and 25 reports | All 25 passed, 3754ms. Includes management terminate/unfinished worker failure, exact parent completion, native pre-bind authentication/event negatives and existing route/close barriers. |
| `verify-native-gui-routing.ps1 -Observer build/M0-T427/S4/r049/console-startup-observer.exe -PackageRoot build/M0-T428/S4/r003/runtime -LaunchRoot Z:/ -ReportPrefix build/M0-T428/S4/r006/gui -MonitorRpc build/M0-T427/S2/r001/monitor-rpc-test.exe` with Z: mapped to that runtime | r006 six GUI reports/run.log | All six passed. GUI-only native carrier closes and exits1223 without killing its separate GUI; test-controlled GUI later exits37. Other startup/wait, GUI-to-text, residency and text-to-GUI-to-text cases retained. |

r001's sandboxed Ninja remained idle and was ended by its exact owned PID;
the same build succeeded elevated. r002/service initially failed the retained
unregistered release-event denial. The candidate eligibility check was narrowed
to registered native close controls (or an existing root); the denial assertion
was not weakened. r005's original 23 cases and r010's expanded 25 passed.
All failed attempts remain retained, not counted as passes. Z: was unmapped.

The first added handle-count check compared cold startup with post-CreateProcess
state (116 versus119) and failed. r010/shutdown-reviewed and
shutdown-handle-diagnostic retain that evidence. The final test asserts a first
actual successful close before its baseline, then all six success/failure cases;
no per-operation growth is accepted. This changes test initialization, not
production resource ownership or result assertions.

## Product, lifetime and delivery results

| Gate / entrypoint | Evidence | Actual result |
| --- | --- | --- |
| `Invoke-ProductVerification.ps1 -RuntimeRoot build/M0-T428/S4/r003/runtime -BuildCache build/M0-T427/S2/r001 -Observer build/M0-T427/S4/r049/console-startup-observer.exe -WindowObserver build/M0-T427/S4/r049/worker-window-snapshot.exe -LogRoot build/M0-T428/S4/r004 -Suite Product -GuestFixture build/M0-T425/S9/r008/G7.COM -WowBaselineRoots build/M0-T428/S3/r004` | r004 manifest/timings/reports, r003/product.log | Console17 66094ms, Window17 76269ms, three retained WOW frontiers 67166ms passed; total214607ms. Same coherent eight-file x86 package throughout. |
| `verify-broker-retirement.ps1` native/DOS frontend loss; `verify-ntvwm-management.ps1 -TwoSessions`, `-WorkerLoss`, `-FrontendClose` | r007 management.ps1/run.log and reports; Z: mapped to r003/runtime for this serial run | Both frontend losses close workers and return1067. Selected real native Console close succeeds; independent session accepts fresh input and exits23. Unexpected worker death returns1067 while native target survives. Explicit frontend close ends its real Console/worker/target. |
| r008 publish.ps1 against r004 tested manifest | r008 recovery-manifest/published-manifest/run.log | Eight files published to O:/winnt/system32; coherent S3 backup in r008/recovery. Guest media, NTVDM.REG and configuration untouched. |
| r009 published-smoke.ps1 | r009/run.log; O:/winnt/Logs2/t428-s4-published-* | Empty, native-zero, MEM and EDIT passed in both Console and Window; all eight deployed hashes equal r003 candidate. |

Final diff review confirms source provenance, unchanged mirror files and wire
versions, service authentication/lock/handle duplication boundaries, explicit
callback and acknowledgment ownership, no new lifetime policy, thread or
registry. Native's existing presentation watcher starts before first text bind;
its original thread/event resources remain owned by membership_close. NTVWM
cannot rebind to a different root. Governance, relative links and diff checks
pass. Side-session proposal/queue edits remain excluded from this P.

## Remaining boundary

This eliminates duplicated project close execution/acknowledgment/carrier-exit
mechanics and the GUI-only native close-registration gap. It does not merge
original CntrlHandler, VGA/DOS cleanup or native Console close into a generic
worker scheduler. Explicit DOS/WOW management force-close and native real
Console-session acknowledgment remain different local operations by the
approved S1 contract; no new cooperative-then-force policy is implied.
Root-worker association/reuse, cancellation, exclusive native retirement and
ownership naming remain S5/S6. The S2 independent GUI reuse limitation is
still open there. Physical RDP/focus is owner-waived, not tested/passed;
WOW results retain their existing frontier limits, not new full usability.
