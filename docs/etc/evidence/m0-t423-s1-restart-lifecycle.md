# T423 S1 restarted lifecycle evidence

## Scope and identity

The owner-approved restart uses T422 closure `3821036ae` as production
baseline. Planning admission is local commit `5c78e59a0`. The former T423
source and WIP remain on `codex/t423-original-reference-20260925` at
`286d54a306bcb8e991891fae849b79db540e897a`; they are not the new implementation.
S1 retains baseline I/O; protocol/frontend migration belongs to S2.
This is ongoing evidence, not an S/P closure or a complete regression verdict.

Build roots: `build/M0-T423/S1/restart-formal-x86` and `restart-wow-x86`.
Use New-T310OriginalSoftpcNinja.ps1 and New-T404S5Wow32ProviderNinja.ps1, MSVC
2022 Win32/x86 /MT, Windows SDK 10.0.22621, Ninja -j4. Explicitly link
run16.exe, basesrv.exe, ntvdm.exe, dtmgr.exe, VDMREDIR.dll and wow32.dll;
the first graph's default target builds libraries, not the final package.
Both graphs compile and link. Build logs are
`O:/winnt/logs/t423-s1-restart-{build,link}-r1.log` and
`t423-s1-restart-wow-build-r1.log`.

O:/winnt initially had no six-file product set at its root. The retained set
in O:/winnt/builds/0925 was copied to build/M0-T423/S1/recovery-0925 before
staging the newly built baseline. Guest/configuration files were not changed.
This candidate is not a delivered P. Later run16/dtmgr edits invalidate their
initial hashes and require the combined gate again.

## Tests and observed facts

The existing `tests/observation/console_startup_observer.c` now supports
MVDM_OBSERVER_PRIVATE_DESKTOP=1, without switching the input desktop. It
records Console members after task completion. MVDM_OBSERVER_POST_EXIT_MS
optionally delays this snapshot (at most 5000 ms), without killing the worker.
Its existing text/exit assertions remain unchanged.

`tools/audit/Verify-CommandExitStatus.ps1`, with
`-Observer O:/winnt/tests/t423-s1-console-observer.exe`, ran these cases:

| Evidence prefix | Input / result | Interpretation |
| --- | --- | --- |
| t423-s1-baseline-r1 | Package under build/M0-T423/S1/pkg; exit 1067 before DOS prompt | Invalid test location: root length 43 cannot fit original COMMAND SHELL value. No lifecycle pass. |
| t423-s1-baseline-r2 | empty, direct-mem, command-c-mem, direct-seven pass with actual screen markers; exits 0/0/0/7 | Four baseline routes work from O:/winnt. |
| t423-s1-baseline-r3 | empty times out before input/prompt | Failed run inherited VS compiler environment. Not a pass. |
| t423-s1-baseline-r4 | empty and direct-mem pass; 5000-ms post-exit observation still has worker and observer attached | Task completion is independent of resident worker lifetime. A retained parent Console must not be mistaken for Explorer-only ownership. |

Build-root package rejection follows
src/product-package/package_layout.c::app_package_layout_command_value_fits:
the original command value includes the root twice and must fit 64 bytes.
No attempt is made to modify guest or relax that guard.

`tests/observation/run16_new_console_lifecycle.c` launches run16 on a private
desktop with CREATE_NEW_CONSOLE. It never attaches to that Console before
the launcher exits. It observes exact child handles, waits five seconds for
normal retirement and checks for remaining Console windows. Cleanup is after
the verdict and cannot make a failing run pass. The small hidden STATIC
sentinel makes desktop enumeration observable even when all product windows
have gone; a failed enumeration is not counted as zero windows.

Invoke the built observer as:

```text
O:/winnt/tests/t423-s1-new-console-lifecycle.exe O:/winnt MEM.EXE O:/winnt/logs/<fresh-report>.txt
```

Use a normal runtime shell, separate from the VsDevCmd build process. Compile
the observer with explicit /Fo and /Fe under the admitted build root and
user32.lib. Its timeout-only x86 host stack/module capture does not patch guest
memory or install a debugger.

- Explorer-before r1-r6 were polluted by the VS build environment. r4's
  actual worker frame chain maps through original illegal_op_int, host_error
  and ErrorDialogBox; r6 captures the existing host diagnostic
  `CS:03f4 IP:1f8f OP:63 75 72 73 6f`. This is a DOS startup failure, not proof
  of a missing completion event. Its precise environment/guest cause remains
  unproven and must not be called an original guest defect.
- Explorer-before r7, same binary/input but normal runtime environment:
  launcher exits 0 in 2281 ms; worker is still alive after five seconds.
- Explorer-after r1, run16 change only in this lifecycle path: launcher exits
  0 in 2906 ms; worker exits 0 naturally; broker is still alive. The initial
  observer could not certify an empty desktop, so this run is process evidence
  only, not a full test pass.
- Explorer-after r2, corrected sentinel enumeration: launcher exits 0 in
  3250 ms; worker exits 0 naturally; broker remains alive; zero Console
  windows, successful desktop enumeration, pass=1. Evidence:
  `O:/winnt/logs/t423-s1-explorer-after-r2.txt`.
- Native CUI and GUI fixtures each return 37 through run16, create no worker,
  and leave zero Console windows with successful enumeration. Evidence:
  `O:/winnt/logs/t423-s1-NCCUI-lifecycle.txt` and
  `O:/winnt/logs/t423-s1-NCGUI-lifecycle.txt`.
- DOS17 r3 completes all 17 cases successfully, including nested COMMAND,
  repeated MEM, EDIT, native streams/EOF and exit-code propagation. Evidence:
  `O:/winnt/logs/t423-s1-dos17-r3-summary.json` and per-case Console captures.
  Assertions normalize wrapped whitespace; nested-MEM counting is consolidated
  into the common marker assertion. Earlier r1/r2 assertion failures are not
  counted as product passes. This does not discharge the independent WOW gates
  or a subsequent changed-source build.

All new compiler/linker intermediate outputs must stay below repository-root
`build/`, with explicit output paths; observation logs remain in the approved
`O:/winnt/logs` location. Tracked historical mirror artifacts are not new build
outputs and must not be deleted as an incidental cleanup.

## Source-first recovery and smallest change

Original source owners already exist and remain unmodified:

- opennt-host/base/win32/client/vdm.c::BaseCheckVDM distinguishes a new Console
  through bNewConsole and publishes a null-Console/session-id request.
- mvdm/dos/command/cmdmisc.c::cmdGetNextCmd uses DosSessionId and the original
  PIF CloseOnExit rule to terminate an independent DOS session after its task.
- mvdm/softpc.new/host/src/nt_pif.c defaults CloseOnExit to one.
- BaseSrv retains original completion, exit-code and record cleanup policy.

The standalone Console-subsystem run16 already owns a Console by the time
it calls BaseCheckVDM. That is not evidence of an external parent Console.
Previously it passed flags zero in both cases, choosing the resident route.
The minimal app seam snapshots Console membership before submitting the task:
if the DOS launcher is the sole attached process, pass the original new-
Console creation intent to BaseCheckVDM, but let the worker inherit the Console
already allocated for run16. A retained CMD/observer/nested caller continues
using the shared-Console path. No new timer, kill policy, guest mutation,
mirror diff or parallel scheduler is added.

Recovery ladder: (1) the original task/exit owners are directly composed;
(2) this run16-only facade supplies the lost pre-creation Console distinction;
(3) no external-code intrusion; (4) no replacement DOS lifecycle implementation.
This is an app integration correction, not a CCPU or COMMAND semantic change.

## Remaining gates

### WOW source/artifact baseline reconciliation

Owner subsequently permitted a short foreground test. Computer Use enumerated
the ordinary WINMINE launch's actual `File Error` dialog; accessibility text
was `Cannot find NETWORK.DRV`. The package SYSTEM.INI has no boot driver
selection. Existing O:/winnt/WFWNET.DRV matches the pinned original hash
`4C321D43511F845EC1A95518507133D1CF4CF3993BAE422037DBA95CE6258CCA`.
Under the previously admitted reversible profile experiment, backup
`build/M0-T423/S1/before-wow-visual-system.ini` was retained and a controlled
`[boot] network.drv=wfwnet.drv` profile selected that original binary.
Current source-built WOW32 (not E80) then exposed the WINMINE window
`É¨À×`, its Game/Help menus and Shared WOWExec to Computer Use. No guest bytes,
provider or source were changed for this comparison.

Actual gameplay remains unverified: screenshot capture failed with
`IGraphicsCaptureItemInterop.CreateForMonitor` / 0x80070057; foreground
activation failed with `GetCursorPos` / access denied 0x80070005. Fresh
window selection and text-only capture succeeded, but do not establish
rendering or gameplay. No input was sent after the access failure. Product
test processes were cleaned, SYSTEM.INI was restored byte-identically, and
all six binaries were checked against the published hash manifest. This
controlled-profile result is not ordinary-profile acceptance. SOL/WRITE
were not exercised in this foreground attempt. The next action must resolve
interactive desktop access and explicitly reconcile the ordinary package's
driver selection, not infer a WOW code regression from the initial modal.

The T422 closure and `m0-t422-s2-planning-handoff.md` explicitly preserve a
source/runtime split: source retains later E81/E91 research, while the last
restored ordinary provider is E80. Rebuilding closure commit 3821036ae is
therefore not, by itself, reproducing the prior deployed WOW provider.

Read-only hash verification finds both retained E80 copies intact:
`build/M0-T422/S2/e80-tested-wow32.dll` and
`build/M0-T422/S2/desktop-range-artifacts-20260924/e80-tested-wow32.dll`.
Both SHA-256 values are
`B423B07C81A259B6788D37BF15B4A23B7285EB97566D73BA91B2AD603D2BB9CF`.
The current source-built provider is separately recorded in the six-file
publication manifest; neither receives the other's historical acceptance.

Ordinary-profile WINMINE launch on a private desktop through the current
Console observer records `t423-s1-winmine-frontier-r1.txt`: launcher waits
at the 20-second observation bound, worker remains alive. That observer has
no GUI gameplay assertion, so this is neither a playable pass nor evidence
of regression. Exact package test processes were cleaned after observation.
No guest/configuration/provider substitution or WOW source change was made.
Next comparison must observe actual application behavior and retain separate
current-source, E80 and historical gameplay identities; do not lower the
playable WINMINE bar to a window/process-existence check.

### Additional lifecycle verification

Native CUI fixture now also selects a private alternate Console screen buffer,
writes/reads two full-width Unicode characters, sets/checks its cursor and
restores the inherited buffer before returning 37. Run
`t423-s1-native-cui-r3{,-child}.txt` passes these assertions and the independent
launcher/no-worker/no-residual-Console checks. r1/r2 failed the fixture's
two-cell read assumption: the observed read returned one full-width character.
Reading the four occupied cells retrieves both characters; no product change
was made to obtain the pass. This tests buffer content/coordinates, not font
glyph rendering. Native GUI rerun `t423-s1-native-gui-final{,-child}.txt` also
passes actual window creation/destruction and exit 37. Reports are under
O:/winnt/logs; fixture builds remain under build/M0-T423/S1.

Read-only review of the combined dtmgr diff finds title/frame/scroll rendering,
viewport state and left/right scrolling changes; its broker RPC and task
termination request paths are unchanged. This review complements, not replaces,
the layout fixture. All six published runtime hashes still match
`t423-s1-control-publication.json` after these fixture-only checks.

Supplemental native/DOS nesting cases pass on the current deployed package:
`Verify-CommandExitStatus.ps1 -Observer
O:/winnt/tests/t423-s1-console-observer.exe -Cases native-cmd-dos,dos-native-dos
-LogPrefix t423-s1-native-nesting-r2`, with private-desktop mode enabled.
The first runs native CMD -> run16 MEM and asserts the memory report and exit
0. The second enters native CMD from DOS COMMAND, runs run16 MEM, exits CMD,
runs MEM again in DOS and exits; assertions require two memory reports,
native version output and original final exit 1. The unchanged default suite
remains DOS17; these cases are explicit opt-in supplements, not silently
included or counted in earlier DOS17 evidence.

The r1 first workload passed but its verifier could not clean grandchildren
created through the native CMD relay after that relay exited. The r2 verifier
records the observer's descendant identities while they are alive, then
restricts cleanup to exact package paths; it never broadens ownership merely
because a process has a product filename. Both r2 cases complete their
cleanup check. Reports and captures are under O:/winnt/logs with that prefix.

Real native control-event test: `tests/observation/run16_control_event_test.c`,
compiled x86 `/MT` with explicit `/Fo` and `/Fe` below `build/M0-T423/S1`.
Run the fixture as `--run <run16.exe> <O:/winnt/logs/report> c` or `break`.
It creates a private desktop/Console, launches an actual native child through
run16, waits for the child's registered handler readiness, and generates the
real Console control event. No key injection, debugger, or job teardown is
used. It requires the child to receive the event, finish cleanup and return
37, and requires run16 to return the same 37.

- Before: both `t423-s1-control-c-before.txt` and
  `t423-s1-control-break-before.txt` record child 37, launcher 3221225786
  (`0xC000013A`), pass=0. The parent default control handler prematurely
  terminated run16 independently of its child.
- After: `t423-s1-control-c-after.txt` and
  `t423-s1-control-break-after.txt` record child/launcher 37, pass=1.
  The new run16 handler consumes only C/Break in the waiting launcher. It is
  a non-inherited callback, not the inherited NULL-handler ignore attribute;
  the child/VDM still owns its event response. Close/logoff/shutdown events
  retain default dispatch. This is launcher lifecycle policy, not a new guest,
  OpenNT, Console transport or scheduling algorithm. No mirror changes.
- The focused test references the documented native event contract:
  [GenerateConsoleCtrlEvent](https://learn.microsoft.com/en-us/windows/console/generateconsolectrlevent).
- Incremental build log: `build/M0-T423/S1/control-build.log`. Coherent six-file
  candidate published to O:/winnt; identities in
  `O:/winnt/logs/t423-s1-control-publication.json`, recovery set in
  `build/M0-T423/S1/before-control-publication`. Post-change DOS17 passes all
  17 cases (`O:/winnt/logs/t423-s1-dos17-control-r1-summary.json`);
  this is not a delivered P.
- Direct DOS control events separately pass through
  `tests/observation/verify-run16-dos-control.ps1 -Observer
  O:/winnt/tests/t423-s1-console-observer.exe -LogPrefix t423-s1-dos-control-r2`.
  The observer generates real C and Break events after the actual COMMAND
  prompt, then enters VER/MEM/EXIT. Both cases assert event generation,
  successful version and XMS output, drained input and the original exit 1.
  Reports, generated-event witnesses and Console captures are under
  `O:/winnt/logs/t423-s1-dos-control-r2-{c,break}.txt*`.
  This proves prompt recovery and subsequent execution, not interruption of
  every possible running DOS workload. The observer's own non-inherited
  handler protects only the test controller from the generated event.

- Independent COMMAND `/c ver`: `t423-s1-explorer-command-r1.txt`, launcher
  returns 0 in 2500 ms, worker retires naturally, broker stays alive, desktop
  enumeration succeeds with zero Console windows. Together with Explorer MEM
  and native CUI/GUI cases, this closes the independent-Console lifetime row;
  it does not substitute for shared-Console interactive tests.
- `Verify-BrokerFinalLifecycle.ps1 -BrokerLoss -TwoWorkers` with the private
  desktop observer: `t423-s1-broker-loss-r4-results.txt`. Both launchers return
  1722, both workers terminate, a fresh MEM launch prints its XMS report.
- Same script with `-WorkerLoss -TwoWorkers`:
  `t423-s1-worker-loss-r1-results.txt`. Affected launcher returns 1067;
  unrelated worker and broker survive, other task finishes normally, fresh
  MEM prints its XMS report.
- Same script with `-LauncherLoss -TwoWorkers`:
  `t423-s1-launcher-loss-r1-results.txt`. The killed launcher has a nonzero
  outcome; its already-claimed worker and unrelated worker/broker survive.
  Other task finishes normally; subsequent MEM prints its XMS report.
- After the launcher control-event correction, all three loss cases were
  rerun successfully on the published package, using prefixes
  `t423-s1-final-broker-loss`, `t423-s1-final-worker-loss`, and
  `t423-s1-final-launcher-loss`. Their `-results.txt` and per-process reports
  supersede pre-correction loss evidence for the current candidate.
- All runtime records above live under `O:/winnt/logs`. Commands use
  `-Observer O:/winnt/tests/t423-s1-console-observer.exe`, a fresh `-LogPrefix`,
  and `MVDM_OBSERVER_PRIVATE_DESKTOP=1` in an ordinary runtime shell.
  This uses private desktop input only; it does not switch the user's desktop.
- The historical loss script expected removed production trace hooks. Its r1
  failure is an obsolete readiness assertion, not product failure. Loss cases
  now wait for actual COMMAND prompt captures and read the observer's explicit
  launcher identity; no production hook was restored. Environment is explicitly
  passed through non-shell process creation. The remaining historical default
  and barrier branches have not been revalidated and are not claimed here.
- Combined dtmgr source passes `tests/observation/verify-dtmgr-layout.ps1
  -BuildRoot build/M0-T423/S1/dtmgr-final-review`: frame, four arrows, colors,
  25 rows, overflow selection, horizontal extent, shrink, empty and confirmation.
  Formal incremental build recompiles/relinks only dtmgr; log:
  `build/M0-T423/S1/final-incremental-build.log`.

- [x] Corrected independent-Console test, ordinary environment, including
      COMMAND /c and GUI/CUI exit cases.
- [x] Full DOS17 on the final combined source, including EDIT then MEM and
      multi-level COMMAND/native re-entry.
- [x] Recorded abnormal launcher/broker/worker and stream/EOF/control-event cases.
- [ ] Three independent WOW frontier comparisons and side-test publication.
- [ ] Combined dtmgr focused test, source/hash review, governance, commit and
      authorized remote synchronization; no T closure before owner acceptance.

Owner permits ending exact O:/winnt product processes when they block testing
or staging, without another question. Never target unrelated processes.

## Bounded verification conclusion: owner exception, 2026-09-25

Owner: “构建能通过就行 特批你完成当前验证目标”. This accepts current
S1 verification on formal build success, not every original checklist row.
Unexecuted/partial rows, including three WOW application frontier and
interactive acceptance checks, are non-pass outcomes waived for this delivery
only. Future S/P gates and T423 closure are not waived. Detailed passing
evidence above remains limited to its exact tested artifacts.

Final x86 incremental verification: Ninja explicit targets run16.exe,
basesrv.exe, ntvdm.exe, dtmgr.exe and VDMREDIR.dll in
build/M0-T423/S1/restart-formal-x86 succeeded (no work needed); the default
WOW32 target in build/M0-T423/S1/restart-wow-x86 relinked successfully.
WOW32 retained LNK4017 DESCRIPTION and duplicate-export LNK4197 warnings,
with no build error. The relink changed file identity; earlier runtime
results are not claimed for the relinked DLL. Its publication is covered by
the explicit build-only exception. Final WOW32 SHA256:
09E02F6B4FE0A27E7074A2ABD353D149CFF1B2AEA2683DA39FC250BD1A656A3D.
Other five hashes remain those in t423-s1-control-publication.json. All six
published O:/winnt files were checked equal to the final build. Temporary
SYSTEM.INI changes were restored and its hash matched the saved original;
guest binaries remain unmodified.

Local S1 implementation/verification concludes under this exception.
Remote P delivery remains deferred: restarted local main and origin/main
diverge, and the proposal requires explicit remote-history rewrite authority
beyond ordinary push permission. Preserve the reference branch; do not merge
rejected prototype commits or force-push to manufacture synchronized status.
