# S11 initial cached WOW build observation

The owned command started 2026-10-28 21:49:39 local in the repository root:
`cmake --build build/.ninja/msvc-x86-release --target wow32`. The observed
processes were CMake PID 24196 and its Ninja PID 24176. The tool session holds
stdout/stderr; its first output reported the cached worker target and
`ninja: no work to do`. No WOW build or exit result has yet been observed.
The tool session is an owned background command, but unlike the preferred
runner it did not provide separate stdout/stderr files. This record is not a
build pass. Subsequent source edits after the worker observation require a
new build after this process has finished or has been safely stopped under
the recorded no-progress policy; they cannot be attributed to this run.

At 21:59:40 local, after the ten-minute no-progress budget and an identity
check, the coordinator terminated only this owned CMake/Ninja process tree;
the tool session returned exit 1. No other build was stopped and no cache was
cleared. The historical successful VS-initialized hidden runner is the next
attempt; this is a recorded failed attempt, not evidence of a compiler defect.

That VS-initialized retry started at 22:00:01 local as owned CMD PID 2992.
It wrote updated `c_main.obj` and `c_xfer.obj` by 22:00:18 but did not
archive or link. At 22:10:36, after ten minutes without output or object
progress, process-tree inspection found no active compiler or linker. Only
the owned tree was stopped and its receipt records exit 1. The same-graph
serial fallback follows the documented T422 precedent; its own result must
be checked before treating any output as a build or release candidate.

The cached x64 build similarly completed configuration but did not start a
visible compiler or linker. Its own CMD PID 28604 was stopped after its
ten-minute no-progress budget, and its receipt records exit 1. The fallback
expands each of the six existing x64 component graphs with the configured
Ninja, runs those commands in the VS x64 environment and retains per-graph
command hashes. Other sessions and their processes were not stopped.

At 2026-10-29 00:57:20 local, a cached Ninja cvidc-vector-binding-fixture.exe build was started in an owned hidden PowerShell PID 7860 with Ninja PID 5968, stdout build/t473-s11/cvidc-fixture-current.out and stderr cvidc-fixture-current.err. At last inspection both processes were live, Ninja had no compiler/linker children and both logs remained empty. The ten-minute no-progress budget has not yet expired; do not restart or terminate based on this observation. A separate explicit link to build/t473-s11/cvidc-selected-fixture.exe used the existing cached libraries and passed; that is not a fresh build. Ninja -n showed 102 pending actions after the sas.h comment/line-ending change, so the explicit-link result must not be treated as the pending build's result.

At 2026-10-29 01:07:32 local, after more than ten minutes with both output files empty, process-tree inspection showed only the owned PowerShell 7860, Ninja 5968 and its console host, with no compiler or linker child. The coordinator stopped only Ninja 5968; both Ninja and its parent then exited. This build attempt did not pass. Its output was empty and no full-build receipt exists. The separately linked cached-library C-VID fixture pass remains limited to that explicit result; the 102 pending Ninja actions are not claimed completed and no cache was cleared.

After the stalled Ninja exited, a separate hidden serial fallback executed the exact 102 commands from ninja -n -v for cvidc-vector-binding-fixture.exe in dependency order, with the graph's fixed ccache environment, then ran the fixture. PID 28708 exited 0; all 102 markers and the fixture PASS were observed. [Receipt, commands, steps and hashes](s11-foundation-evidence/cvidc-serial/exit.json). This verifies this target through the fallback, not a successful Ninja invocation or a full product rebuild; the fallback does not update Ninja's dependency log, and later full builds must account for that.

At 2026-10-29 01:15:27 local, an isolated window-borrow fixture runner PID 22848 was started with build root build/t473-s11/graph-mutation/baseline-run2. At 01:21:21 it remained live with child CMD 20776; baseline2.err was empty and build.log had not progressed past the two initial no-work Ninja checks. The ten-minute no-progress budget has not expired; retain the live process, do not restart or kill it merely because this observation elapsed. A separate direct link of the previously recorded baseline provider completed the new same-scope mutation regression, but does not convert the pending runner into a pass.

At 2026-10-29 01:26 local, the owned window-borrow runner exceeded ten minutes with its output unchanged. Process-tree inspection identified its child CMD 20776 and stalled Ninja 21488, with no compiler/linker child. Only Ninja 21488 was stopped; the runner exited with compiler-command error -1. This runner is a non-pass. The separate direct-link same-scope regression remains a bounded pass and no cache was cleared.

The uninstrumented class-index candidate's first Full gate attempt failed before gates on an input/sealed-image hash mismatch; the corrected sealed cache attempt completed WOW, Console-17 and Window-17 steps but failed before RPC because four existing control fixture executables were absent from the newly constructed cache. Both receipts and errors are in class-index-full-nonpass. The fixture binaries have been copied from the previously used test cache and the complete Full suite is running again in full-run3; this is not a product binary release or an assertion that prior failures passed. An independent scheduled-creation diagnostic was also started while the second Full attempt was active, so that attempt's timing is not isolated.

2026-10-29 18:01 local: the top-level x86 dry run returned no work; x64 requested CMake regeneration. An initial hidden x64 cached-build launch (PID 29368, command and logs in `build/t473-s11/x64-cached-recheck`) ended without an exit marker and produced no output, so it is not a pass. The process handle was absent on inspection. A subsequent owned `cmd /d /c build\t473-s11\x64-cached-recheck\run.cmd` was launched in persistent tool session 12798 with stdout/stderr redirected to `stdout-retry.txt` and `stderr-retry.txt`; the session was still live on last observation. No second build should be started against this root until that session terminates or the owned run is inspected under the no-progress rule. Neither an observation slice nor empty logs establish failure or success.

The owned x64 retry (tool session 12798) subsequently exited 0. Its log records CMake regeneration followed by Ninja `no work to do`; a separate dry run also reports no work. [Captured output](s11-foundation-evidence/atomic-task-retire/execution-check/cached-x64/) includes the stderr text `ntvdm.src`. This confirms the top-level x64 cached graph as observed, not a fresh full rebuild, a full ten-image identity comparison, product tests or publication. The earlier no-marker launch remains a non-pass.

The x86 top-level `cmake --build build/.ninja/msvc-x86-release` then exited 0 and reported `ninja: no work to do`; follow-up dry runs for the separately generated NTVDM worker and WOW32 inner graphs likewise reported no work. [Command and logs](s11-foundation-evidence/atomic-task-retire/execution-check/cached-x86/) are retained. This is a cache/incremental observation, not a clean build, full ten-component tests or publication.

The explicit x64 inner graph check later found three service tasks despite the top-level no-work result. The x64 service rebuild exited 0; its new NTSRV input hash differs from the previously tested package. The S11 candidate must be restaged and revalidated; do not interpret the previous top-level cache check as current complete-package identity. See [service evidence](s11-foundation-evidence/atomic-task-retire/execution-check/cached-service/).

The refreshed ten-image candidate `build/t473-s11/current-ten-image-package` was staged from the current cached inputs and passes the 59 immutable guest-file check. Eight of ten unsealed input hashes differ from the earlier tested manifest (NTVDM and WOW32 match); no assumption is made that all differences are behavioral. New manifest SHA-256 is `37823EA1869847EDE3E2A89EF7FC8AD120713A315F268A6A343D7916E438F8A8`. A first Console/Window matrix attempt failed before starting any case because sandbox access to the established `O:\tmp` staging root was denied. An authorized retry is running in owned tool session 52386, script `build/t473-s11/current-ten-product-matrix-r2-run.ps1`, stdout/stderr `build/t473-s11/current-ten-product-matrix-r2.stdout.txt` / `.stderr.txt`, results `build/t473-s11/current-ten-product-matrix-r2`. The session was live and Console cases were producing reports on last inspection. Do not restart or stage/publish the package based on this partial progress.

The new ten-image candidate completed its independently rerun Console-17 and Window-17 matrices: 17 cases in each summary, all actual results equal expected, runner exited 0. [Summaries and staged manifest](s11-foundation-evidence/atomic-task-retire/execution-check/current-ten-matrix/) bind the run. The first attempt had failed before executing cases due to sandbox access to the established short staging directory; the authorized retry supplied these results. This is not the Full product suite or control/WOW application acceptance of the new candidate.

New-package Control attempt 1 exited 1 before any gate: it did not pass the explicit x64 native component chain, so machine validation rejected run16.exe. The corrected attempt explicitly supplies the sealed x64 worker/frontend/monitor/launcher/service and hook plus the original service-build path for version-test compilation. Owned tool session 55802 runs `build/t473-s11/current-ten-control-r2-run.ps1`; its output is `current-ten-control-r2.stdout.txt` / `.stderr.txt`, with case logs under `current-ten-control-r2`. It was live at last observation. This is not yet a test pass; do not restart against that session's package stage while it remains active.

Corrected Control attempt r2 ran and passed ntvdm-export-ordinals, rpc, native-gui and version-negatives, then exited 1 because the coordinator omitted the required Terminal observer. The partial timing record is `build/t473-s11/current-ten-control-r2/timings.json`; it is not complete Control acceptance. A fresh r3 with `terminal-observer.exe` explicitly supplied is running in owned session 29764, with output `build/t473-s11/current-ten-control-r3.stdout.txt` and `.stderr.txt`, and case logs in `build/t473-s11/current-ten-control-r3`. Do not interpret the earlier partial gates as a complete run or restart while r3 is alive.

New-package Control r3 completed with tool session exit 0. All 12 gates in `timings.json` passed, including RPC, GUI routing, version rejection, strict DIR, native EDIT return, Console return/relaunch, session isolation, retirement and nested-window handoff. The runtime manifest binds the staged ten images; see [captured evidence](s11-foundation-evidence/atomic-task-retire/execution-check/current-ten-control/). This is the Control suite, not the Full suite, nor a resolution of the SOL latency or Winmine gate restriction. Earlier r1/r2 failures remain recorded.

The refreshed ten-input candidate passed the exact-hash prebuilt shared-WOW idle and common-dialog witness: distinct tasks in one worker, 17 dialogs with 16 explicit closes, both tasks retiring and idle continuing after the dialog task exit. The runner's isolated stage used a fresh package identity, so its sealed binary hashes differ while its manifest records the build input identities. [Logs, guest reports and stage manifest](s11-foundation-evidence/atomic-task-retire/execution-check/current-ten-idle-dialog/) retain the result. This does not establish all guest callback/reentry cases or replace Full acceptance.

The refreshed ten-input candidate also completed three SOL Options interactions; [receipt](s11-foundation-evidence/atomic-task-retire/execution-check/current-ten-sol/review.md) records its separate manifest and timings. Seconds-long latency remains; this is not Full acceptance.

The refreshed candidate passed single isolated Notepad and Write Find interactions and normal exits. [Receipts](s11-foundation-evidence/atomic-task-retire/execution-check/current-ten-notepad-write/) do not prove editing, persistence, performance resolution or Full acceptance.

The refreshed candidate additionally passed a Program Manager New dialog interaction and normal exit; [receipt](s11-foundation-evidence/atomic-task-retire/execution-check/current-ten-progman/review.md) retains the input manifest and limitations. It is not Full acceptance or proof of group persistence.

After the new candidate's tests, its manifest was rechecked against all ten current unsealed build input paths; all ten SHA-256 input hashes still match. This does not replace per-test stage manifests or prove behavior. `docs/states/CURRENT.md` now identifies the restaged candidate and its passed scopes; it remains below the owner's 16 KiB limit.

The refreshed ten-input candidate passed the isolated SOL gameplay witness: stock draw, Deal cleared a nonempty waste, redealt tableau was present, another stock draw succeeded, and window/launcher completion was normal. [Output and image hashes](s11-foundation-evidence/atomic-task-retire/execution-check/current-ten-sol-gameplay/) bind the witness. The controlled visual checks do not prove arbitrary play, solve the Options latency, or replace the Full suite.

A full default-target dry run of the x86 worker inner graph, unlike the earlier ntvdm.exe-only check, exposed one pending `original-mvdm-redir.lib` archive action. The VS-initialized owned build completed it with exit 0, and a subsequent default-target dry run reported no work. The archive is not a dependency of the `VDMREDIR.dll` link rule, which links the same selected objects directly; after the build the NTVDM, VDMREDIR and nthook32 binary hashes still match the current ten-image manifest inputs. [Logs](s11-foundation-evidence/atomic-task-retire/execution-check/cached-x86-inner/) retain the exact action. This is not a product Full test.

[Current ten-input qualification ledger](s11-foundation-evidence/atomic-task-retire/execution-check/current-ten-qualification.md) collects passing scopes and outstanding acceptance without mixing package identities.

A raw PE section comparison of the older tested package and the refreshed package found equal `.text` section hashes for all ten corresponding binaries. For NTSRV, `.rdata` has the same size and differs in two bytes, both inside the `IMAGE_DEBUG_DIRECTORY.TimeDateStamp` field; all ten also show `.rsrc` differences. [Per-file hashes and differing sections](s11-foundation-evidence/atomic-task-retire/execution-check/package-section-comparison.json) are recorded. Headers, resource contents, executable sections other than `.text`, loader behavior and runtime equivalence were not established; this observation neither makes the packages interchangeable nor replaces revalidation.

A full-file byte comparison further replaced each package's one occurrence of its own recorded package GUID with zeros before comparing. Nine image pairs then match byte-for-byte; NTSRV differs at four bytes only, two in its COFF timestamp and two in the `IMAGE_DEBUG_DIRECTORY` timestamp. Lengths and exact offsets are in [normalized comparison](s11-foundation-evidence/atomic-task-retire/execution-check/package-normalized-comparison.json). Package GUID is itself a functional isolation identity and is not being declared irrelevant; this observation does not make the differently sealed packages interchangeable or waive new-package tests.

## Probe stage identity receipt

SOL Options and the Win16 multi-dialog probe now validate the hashes of all components listed in the staged package manifest and preserve that manifest in their output. For the current ten-image candidate, each probe passed one interaction and retained a matching manifest. An isolated helper check accepted a valid ten-record manifest, rejected a modified component and duplicate record, and accepted an eight-record legacy manifest after the helper was relaxed to preserve the existing supported package shapes. The actual SOL and Notepad runs predate that relaxation but exercised the same component hash verification; the final helper was rechecked with synthetic manifests. No Winmine was run. [Receipts](s11-foundation-evidence/atomic-task-retire/execution-check/stage-manifest-receipt/) do not establish Full product acceptance.
