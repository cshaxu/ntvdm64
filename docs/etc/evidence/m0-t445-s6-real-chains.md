# T445 S6 real-chain follow-up

Continues [S6 propagation evidence](m0-t445-s6-style-propagation.md).
The active packet remains T445 S6; this record does not admit a new task.

## Mixed shared WOW and MessageBox

The immutable SOL/WRITE baseline reaches error dialogs rather than gameplay.
Those dialogs can provide title/ownership evidence, never functional application
acceptance. The private-desktop mixed probe starts original WINMINE and SOL via
real Run16, waits for each ordinary startup-only launcher to return zero, and
requires both visible witnesses to have the same actual worker PID. It never
installs a policy payload itself or changes guest bytes.

- r039 PID14504 at11:12:54, deadline180s, exit1: worker35352 has marked
  Minesweeper and an unmarked SOL error dialog, as expected for On/Off. A later
  attempt to dismiss SOL with WM_COMMAND/IDOK does not destroy its dialog and
  times out. This is not a passed normal-exit test. Further mixed title tests
  omit that interaction rather than attributing it to style propagation.
- r040/r041 supervisor PID16732 at11:14:52, deadline180s, exit1 overall.
  r040 On/Off passes on worker11160, with both launchers already returned.
  r041 Off/On fails on worker43352: unmarked Minesweeper is correct but SOL's
  error dialog still has an empty title rather than the required prefix.

Source confirms original wudlg.c::WU32MessageBox calls host MessageBox directly,
not the registered window/modeless bindings or DialogBoxIndirectParamAorW.
The existing title-only modal scope is now factored into begin/end helpers and
reused by a project-owned ANSI MessageBox binding. Only wudlg.c's selected
build receives the forced macro header; original pointer conversion, flags,
return value, guest source and binary remain unchanged. Off calls the original
host API directly. On installs a bounded same-thread creation/return hook and
removes it on return or unwind. The task's policy is captured before entering
the modal call; missing policy or marker failure returns explicit failure.
No native menu/theme setting or automatic retro renderer is introduced.

The source-level sweep also identifies native MessageBox-family coverage as
requiring its own check: the ten native window/dialog API fixtures do not
prove message-box creation coverage. This remains open, not an inferred pass.

### Real mixed-task repair verification

r042 supervisor PID18680, start 2026-10-13 11:16:20.072 -07:00,
deadline600s, completed with exit0. The rebuilt WOW provider passes the
three host lifecycle fixtures and both real mixed-task directions:

- r042 worker29064: ordinary Run16 first starts Off WINMINE, then On SOL.
  Minesweeper remains unmarked; SOL's error dialog is `(Win16 Style) `.
- r043 worker22732: On WINMINE remains `(Win16 Style) Minesweeper` while
  Off SOL's error dialog retains its empty original caption.

Both launcher processes return zero before each observation. The probe checks
actual HWND owner PIDs and rechecks the first window after the second launch;
neither result is inferred from monitor labels. Both runtime WOW32 copies have
SHA256 `04FE7011CB2072ABF2C9597FC791D39A60137929F28EB4545AB8C208C1E79102`.
Logs are `build/M0-T445/S6/r042-logs/mixed.txt` and the corresponding r043
path. These are task-isolation/title witnesses, not SOL functionality or
normal-exit acceptance. No publication occurred.

The native fixture now independently exercises MessageBox A/W, Ex A/W and
Indirect A/W, checking caption and IDOK return on both original and later
GUI threads. r044's first supervisor PID43896 at11:21:41 exits1 before the
test because CreateDesktop returns access denied. That is an execution-
permission failure, not evidence of an application regression. An approved
isolated-desktop retry uses PID28780 at11:22:23 with a660s deadline; results
must be inspected before classifying the native MessageBox route.

The r044 retry exits1: the Off child's 90s deadline expires without a caption
mismatch. Its thread-timer/IDOK control cannot yet prove MessageBox behavior.
r045 (PID13396 at11:24:37,660s supervisor deadline) moves observation to the
actual dialog activation and window timer. It observes the correct unmarked
Off caption, but GetDlgItem(IDOK) returns no control (error1421); exit1 is a
probe closing-method failure, not a style mismatch. r046 (PID18528 at11:25:34,
660s deadline) instead requests WM_CLOSE and still asserts the native IDOK
return. Its result remains pending. No production native Hook repair has
been inferred from these probe failures.

### Native MessageBox negative control and repair

r046 exits1 after Off checks pass: the cross-width child exceeds the existing
45s nested wait when all new MessageBox checks are inserted into the older
window/dialog matrix. The additional cases are now a separate `--messages`
fixture mode, preserving the existing matrix and avoiding timing dilution.

r047 PID46756 at11:28:10,660s deadline, exits1 with a genuine negative control:
x86 Off and Win16-only each pass all six MessageBox APIs on the first and
later GUI thread. All policy fails on its first MessageBoxA: actual caption
`message box`, expected `(Win16 Style) message box`. This is not a timeout
or native-application failure. Six entry points were absent from the native
title Detours table; USER need not call exported DialogBox/CreateWindow APIs
to implement its own message box.

The project-owned native Hook now wraps MessageBoxA/W, MessageBoxExA/W and
MessageBoxIndirectA/W using the existing bounded modal title scope. It keeps
the native parameters and return value, returns zero on marker failure, and
unhooks on normal return or SEH unwind. No persistent observer, global policy,
guest/mirror edit or retro drawing is added. Only All installs these native
title hooks; Win16-only still propagates without marking native windows.
r048 PID24464 at11:29:49,780s deadline, rebuilds both Hook widths and runs the
isolated six-entry/three-policy/two-thread matrix. Its final result is pending.

r048 completed with exit0. Both widths pass all 72 combinations of six APIs,
three policies and two GUI threads, including native IDOK return. Tested
Hook32 SHA256 is `97CCFE131B707CE1598023581E0D18076DFD3F6E80EEA9D2B9364B843E6376DF`;
Hook64 is `FA10B8702AE90D0D739CB50B3D59A6A98CDAFACA930895648D01555FFC9C0320`.
The tracked native-title verifier now includes this dedicated matrix in
addition to its original cross-width window/dialog checks.

r049 PID38248 at11:33:41,2400s outer deadline, builds a fresh GUI-only
runtime with those Hooks and the recorded r049-inputs.json. Actual Run16
`message-launcher` passes all six policy/width launches, including both GUI
threads and all message-box APIs; each Run16 returns zero. The same runner
then performs original cross-width window/dialog and nested override chains;
those results remain pending. This runtime deliberately omits NTCON and is
not full-package acceptance or publication.

## Original WinExec guest-child probe

The independently authored `tests/observation/win16_style_parent.asm` is a
disposable NE test, not rebuilt/patched original media. It reuses the existing
independent directory-probe header shape. Its imports are original KERNEL
InitTask91, WinExec166 and USER InitApp5, verified against the selected mirror
DEF files. It calls WinExec("WINMINE.EXE",1), validates the result, then exits.
The outer private-desktop observer uses actual Run16 --wait and checks the
surviving immutable WINMINE caption after parent completion for all three
policies. The probe lives only under the marked test stage's tests directory.
r050 PID29292 at11:36:14 exits1 before guest execution: NASM requires whitespace
around `%` in the MZ size expression. Corrected test syntax; no production
change is inferred from this assembly failure.

r051 PID33816 at11:37:11,480s deadline, exits1 after assembly and observer
compilation. The ownership guard refuses the still-live r049 broker PID45692
at its exact r049 package path, before launching any guest. This is the
intended test-isolation refusal, not a guest-child functional result. Keep
the broker guard unchanged; run this package serially after r049 terminates.

r049's pinned supervisor ultimately exits0. All three actual-Run16 matrices
pass: six message-box policy/width launches, six window/dialog cross-width
launches, and eight nested inherited/explicit-override chains. Inputs remain
the recorded r049 GUI-only package; no NTCON or full-package claim is made.

r052 PID16072 at11:40:26,2280s outer deadline, pins r049's actual process
handle and exact start timestamp, waits for its exit0 and completion marker,
then runs the Win16 probe. Off fails before any WOW registration. r053
PID30920 at11:42:42,180s deadline, improves the probe's diagnostics and shows
actual Run16 exit87, not a CreateProcess access failure (the prior generic
error5 was stale last-error evidence). r054 PID48544 at11:45:05 exits1 and
shows host GetBinaryType classifies the316-byte NE test file as DOS/type1.

The independently authored test file now pads its first file sector to512
bytes. No production or immutable guest changes. r055 PID17124 at11:45:52,
480s deadline, exits0: all three policies classify as Win16/type2 and pass
actual Run16 --wait plus surviving WINMINE observation after parent completion:

| Policy | Run16 PID / exit | Child worker | Caption |
| --- | --- | --- | --- |
| Off |38564 /0|38724|Minesweeper|
| Win16 |38112 /0|14688|(Win16 Style) Minesweeper|
| All |35324 /0|27724|(Win16 Style) Minesweeper|

For the Win16 row, the trace identifies shell039F, direct parent17C7 with
shared request1, and local child177F with shared IDFFFFFFFF and original
TDB parent17C7. Each InitTask succeeds. This proves the guest-created local
child path rather than another direct broker launch. Captions are observed
after the waiting parent launcher exits0. It does not prove WINMINE gameplay,
normal child exit, or the Win16-to-native-to-Win16 return route.
Probe SHA256 is `BE430E3BBC2FFBB833920F79997433EE8353E8BAFB0D8ADFF6E452E50D7627AF`.
Raw logs and original task IDs are under r055-{off,on,all}-logs; marked short
stages are removed by the existing exact-path ownership cleanup.

## Win16 to native to Win16 return-chain investigation

The independent parent probe gains a NATIVE_RETURN assembly variant which
WinExecs tests/STYLENAT.EXE. That GUI-only x86/x64 fixture reads the actual
installed Hook context, creates a visible NativeStyleReturn window, and calls
CreateProcess with the immutable WINMINE image (not explicit Run16/options).
The inherited Hook must redirect that Win16 target. Both HWND owners/captions,
the native exit and the original parent launch completion are asserted.

- r056 PID23660 at11:49:27,960s deadline: Off parent does not finish within
  the initial20s observer wait. No policy mismatch is established.
- r057 PID36112 at11:52:38,240s deadline: extending only the probe wait to130s
  still times out. A live sample proves Run1624300 -> broker42164 ->
  worker35104 -> native35136 -> Run1640696 targeting WINMINE. It proves actual
  native launch and Hook redirection, not the child's completion or title.
- Original wkman.c waits for native input-idle in repeated5s waits (up to
  roughly105s); ld.asm subsequently loads WINOLDAP. The initial probe waits
  synchronously for its return child before reaching a normal message loop.
  This is a sequencing concern, not yet a proved sole timeout cause.
- r058 PID24708 at11:56:00,1200s deadline: the fixture first enters a normal
  message loop and expects an observer request to create its Win16 child.
  Waiting for the original parent launcher before sending that request still
  times out. Therefore startup synchronous-wait speculation is insufficient.
- r059 PID30132 at11:59:19,300s deadline, exit1: test-only phase logging proves
  native entry, copied context read, successful visible window creation and
  entry into its message loop. The parent --wait launch still times out130s.
  The test baseline contains no WINOLDAP.MOD; the original loader references
  that module. Missing media is an investigation lead, not a causal verdict
  or permission to substitute/patch guest files.

r056/r057 cleanup reports Kill access-denied while the exact test processes
have concurrently exited. Subsequent read-only sampling finds none at either
owned path. The test cleanup now accepts a Kill exception only if its already
identity-checked, pinned process handle becomes signaled within1s; a live
process or bare access-denied error still fails cleanup. The two retained
marked stages n169/n1a6 are hash-validated and removed by r058. No user files
or installation trees are removed, and original test inputs remain in build/.

r060 PID41356 at12:02:39,240s deadline, retains the initial --wait launcher
handle instead of awaiting it before interaction. It first requires the
native process's actual WaitForInputIdle success, then requests the return
launch, checks both window policies, closes the native window normally, and
finally requires original parent launcher exit0. This changes observation
ordering, not the final completion assertions or product implementation.
Its result remains pending; this return-chain capability is not accepted yet.

r060 exits1: external WaitForInputIdle(native47608) returns0, then the actual
window receives the return request and creates its Run16 child. Waiting for
that child's startup does not complete. The earlier input-idle hypothesis is
therefore rejected as a sufficient explanation. Retain the strict failing
concurrent return-chain test; do not substitute a sequential successful chain
for it or claim propagation acceptance from child-process creation alone.

### Original dependency control

The canonical WINOLDAP.MOD is2112bytes, SHA256
`CC9C8BC27E11843C6739D0798EFA2AC41C4EF71A6A34B68F1AF38640A2BBCC7F`.
The archived T274 union ledger records identical OpenNT/OpenNT-4.5 bytes;
T297 preserves them in the guest path move and T301 confirms the third source
copy is already-tracked-identical. T348 labels it selected original mirror,
runtime-deferred-WOW16. The current repository file matches those identities.
Its original source describes a Win16 stub which waits for the native process
and then exits. Original kernel31/ld.asm LM_NTLoadModule explicitly loads
WOAName (`WINOLDAP.MOD`) after WowLoadModule launches a non-Win16 process.

S6 uses that unchanged module only as an explicit test dependency control,
copied to the test stage's system32 after verifying its historical hash. The
exact source/target/hash/test-only role is recorded in r061-x86-off-guest-
addition.json. No source import, guest rebuild/patch, default package change,
release admission or deployed-file replacement occurs.

r061 PID43820 at12:05:28,240s deadline, exits1. The same native phase sequence
reaches input-idle and return-child creation. The original module addition
does produce another successful local InitTask177F with parent17C7 in
worker33468, but the return request still does not finish before the native
fixture's bounded wait fails. Only after native teardown do cleanup/yield and
the direct Win16 request2 InitTask175F appear. This establishes a missing
dependency and a remaining scheduling/wait interaction; it does not establish
whether the latter predates S6 or identify a sufficient source repair.

Next investigation: preserve this failure, inspect the original WinOldAp
native-process wait against the project's WOW task/execution ownership seam,
and compare to an identified pre-S6 package before assigning causality. Do not
change original guest bytes, add an ad-hoc scheduler, or declare the complete
Win16/native/Win16 route passed. DOS/CUI/full-package gates and publication
also remain open behind the separately recorded NTCON security interruption.

### Source audit of the external-process wait

Read-only inspection after r061 identifies a specific missing historical
boundary, not yet a runtime-proven repair. Original OpenNT
`windows/core/ntuser/client/client.c` lines1364 onward implements MsgWait via
MsgWaitForMultipleObjectsEx. For TIF_16BIT and a nonzero timeout it calls
NtUserWaitForMsgAndEvent((HANDLE)-1) before the host wait, then
NtUserCallOneParam(DY_OLDYIELD, SFI_XXXDIRECTEDYIELD) afterward. Its comment
explicitly requires leaving the WOW scheduler so other WOW tasks can run.
The inspected external file SHA256 is
`787EDA33785A87CE9E71202C97B5913E2857F7106FED5BB97103049B1D5E3472`.
This is research-only inspection; no source was imported or changed.

The selected client.c mirror retains only its registered dispatch/class
subsets, not this wait body. Original wkman.c still calls MsgWait directly
at its WinOldAp native-process wait. The current provider graph links host
user32; wkman's new S6 forced header redirects CreateProcess only. The
existing lifecycle queue-wait adapter is a different caller path and does
not automatically wrap wkman's wait.

The data lock is not itself an execution-ownership proof: lifecycle InitTask
releases wow_user_runtime's SRW lock before returning, while original taskman
retains its separate CSOwningThread scheduling state. Existing native-message
call adapters explicitly distinguish those two contracts. Therefore merely
dropping an SRW lock or adding Sleep is not a justified repair.

Git comparison against S5 closure af8189e86 confirms no changes in wkman.c,
wow_user_runtime.c or wow_user_thunk_scope.c. This supports investigating an
older missing binding, but does not prove identical runtime behavior: S6's
new CreateProcess/Hook propagation can expose previously unreachable chains.
The next causal test remains the same explicit-Run16 native return probe on
identified pre-S6 and candidate packages, followed by verification of any
source-shaped boundary repair. No concurrent-return pass, baseline verdict,
new scheduler or production fix is claimed by this source audit.

### r062 matched pre-S6 return-chain control

The independent native probe now offers wow-baseline-return. Unlike the
strict wow-return propagation case, this control neither reads the new Hook
context ABI nor depends on S6 injecting a Hook into a Win16-created native
child. It derives the isolated root from its own tests/STYLENAT.EXE path and
explicitly starts that root's system32/run16.exe with its immutable WINMINE.
Both versions use identical probe bytes, Off policy, native message-loop /
input-idle observation, return request and45s child startup assertion.
It is a causal control, not a replacement for the full propagation test.

The new independent STYLEBASE.EXE SHA256 is
`090C8A9321C5F7E5431AE85A6D79BE4C007DCB80516BF091B406E78E2241E0F9`;
the x86 native probe is
`6E91DE8F8DE46549D077EDC6181A4BFD780631A360814D5C947AE1515CC8F8D4`.
The background supervisor PID44364 starts12:18:12 with720s deadline and
exits0 after both observations finish. This exit is orchestration completion,
not test success. Original WINOLDAP.MOD is present in both controls under the
previously documented immutable test-only identity.

| Package | Runner | Observed native/worker | Result |
| --- | --- | --- | --- |
| Published assets, each selected image verified against release-manifest.json | PID10092,12:18:27,240s deadline | native19732, worker7704 | exit1; native input-idle succeeds, return child created, return request fails |
| Current S6 candidate with recorded input hashes | PID30232,12:19:19,240s deadline | native26164, worker14380 | exit1 at the same return-request assertion after input-idle and child creation |

Both retain the strict SendMessageTimeout/reply assertion failure in
r062-{baseline,candidate}-logs/mixed-errors.txt and native phase logs.
Candidate registration logs retain subsequent cleanup/yield behavior.
Input manifests and dependency records are under the corresponding r062
prefixes. Each package omits NTCON; neither this test nor the successful
supervisor authorizes full-package acceptance or publication.

This demonstrates the blocked concurrent return capability exists in the
published pre-S6 package too. Combined with the original MsgWait source audit,
it supports investigating the missing WOW deschedule/reenter boundary, but
does not yet prove a sufficient repair. The strict implicit-Hook return test
and all policies must still pass after any repair; the control does not waive
them. No product source or guest binary was modified in this comparison.

Cleanup limitation: candidate stage O:/tmp/n65c is removed. Baseline cleanup
reports access-denied deleting its ntvdm.exe, leaving a partially removed,
marked O:/tmp/n58f stage. Preserve it until exact-path process/file identity
is rechecked; do not count cleanup as passed or use the supervisor's exit0
to conceal the per-run cleanup error. Its intact build-owned source remains
r062-baseline-wow-runtime.

### Candidate wait binding and recovery ladder

The next candidate binds wkman's existing MsgWaitForMultipleObjects call,
through its already scoped forced header, to wow_user_msg_wait in the existing
WOW lifecycle owner. Original guest and wkman bodies remain unchanged.
The adapter brackets nonzero native waits with the already composed original
xxxSleepTask(FALSE, HEVENT_REMOVEME) and xxxDirectedYield(DY_OLDYIELD), under
the existing USER data lock; the native wait runs outside that lock. Native
USER continues to own handles, queue waiting and results. Zero-time polls and
unbound host threads retain a direct native call. Native last-error is saved
across scheduler restoration, including failure; no new scheduler/table is
introduced. This candidate requires host and real return-chain validation.

Recovery ladder: (1) original client.c's complete MsgWait body requires private
GetClientInfo/TIF_16BIT, NtUserGetInputEvent and NtUserCallOneParam, which cannot
bind directly to the modern host's unaware WOW state; the selected client
subset does not contain that body. (2) retain its caller shape and original
deschedule/reenter ordering through the current lifecycle/taskman seam and
public native wait. That is the selected bounded adapter, not an inherited
algorithm rewrite. (3) no mirror intrusion is needed. (4) no new scheduling
algorithm is admitted. Host queue readiness remains native USER's operation;
tests must check timeout, ready, failed and concurrent-task outcomes before
the adapter can be accepted. This is not yet a passing or published fix.

### r063/r064 wait-adapter results

r063 supervisor PID48488 starts12:23:55 (720s deadline) and exits1. The
provider builds, but three new fixture assertions incorrectly inspect the
unused lifecycle.state member rather than the original allocated
lifecycle.process.pwpi. Wait return-value assertions themselves pass. This
is a fixture defect, not a passing scheduler test; no real-chain run occurs.
The fixture is corrected to inspect the actual original state and adds the
invalid-handle result/last-error/restored-owner assertions.

r064 supervisor PID15592 starts12:26:15 (720s deadline), exits0 at12:29:08.
Fresh task-lifecycle, native-send and style-policy fixtures all report their
success markers. The lifecycle matrix includes zero-time, timeout, ready
event and invalid-handle results, with original execution-owner restoration
and no held USER data lock afterward. The actual strict Off/x86 return chain
then passes: native33708 reaches input-idle, launches the Win16 target through
its inherited Hook, and the Run16 child finishes while that native parent is
still live. WINMINE worker30888 has the unmarked caption; native relay remains
unmarked. Native normal close, repeated child title check, and original
parent launcher43496 exit0 all pass. The test reports both parent completions;
explicit worker/broker cleanup is not claimed as their normal retirement.
Stage O:/tmp/ncc7 is removed; no cleanup error is recorded.

Tested WOW32.DLL SHA256 is
`8E3B533D11408E6A56387AD27BBBED730C5FF2E6B6023B5E7F17BB3F490AB4A7`.
The changed boundary resolves this formerly failing chain without changing
original guest or mirror source. The remaining policy/width combinations,
ordinary product gates and coherent publication are still required; Off/x86
alone does not prove complete S6 acceptance.

r065 PID35216 starts12:29:57 (900s deadline) and exits0. The other five strict
implicit-Hook return chains pass with the same provider and original test
dependency. Combined with r064 this covers3 policies x2 native widths:

| Native width / policy | Native PID | WINMINE worker | Observed markers native / Win16 |
| --- | --- | --- | --- |
| x86 / Off (r064) | 33708 | 30888 | no / no |
| x86 / Win16-only | 46012 | 15380 | no / yes |
| x86 / All | 10112 | 41712 | yes / yes |
| x64 / Off | 39408 | 21660 | no / no |
| x64 / Win16-only | 44016 | 31008 | no / yes |
| x64 / All | 45700 | 17832 | yes / yes |

Each test observes the returned Win16 window before closing the native parent,
then checks native normal completion, retained Win16 caption and original
parent completion0. Native fixtures use the actual inherited Hook to launch
WINMINE, not the explicit-Run16 baseline control. r065 stderr is empty. These
are real propagation/lifecycle checks, not WINMINE gameplay or normal final
WOW-worker retirement acceptance. Remaining full-package gates/publication
and the NTCON security issue remain open.

r066 cleanup/check supervisor PID19744 at12:31:45 exits1 before deleting
anything: the background PowerShell session lacks its Get-FileHash module.
r067 PID12924 at12:32:45 explicitly imports the built-in Utility/CimCmdlets
modules and repeats the unchanged bounded procedure, exiting0. It verifies
the n58f marker/source, all surviving file hashes, no reparse points and no
process at that exact path, then removes the residual test stage. Its complete
build-owned source is retained. Read-only checks also confirm all five r065
stage paths are absent. Documentation governance and normal git diff --check
pass in r067 (line-ending conversion warnings only). No publication, commit,
push or whole-S acceptance is claimed by these checks.

### r068 real WINMINE gameplay with title policy

The retained gameplay observer now accepts the three policy options and
checks the actual title prefix before exercising the unchanged pixel/input
assertions. Its default package ownership set is unchanged; S6 explicitly
selects the GUI/WOW subset for this focused test, with no NTCON execution.
The observer process handle is pinned immediately after creation. This does
not waive full-package acceptance or claim text routes were tested.

r068 PID41772 starts12:35:43 with600s deadline and exits0. Off worker21060,
Win16-only worker47768 and All worker24600 have respectively0/1/1 title
markers. All three prove a patterned ready board, right-click cell and mine
counter changes, F2 restoring the original covered cell, left-click cell
change, WM_CLOSE destroying the game window, and Run16 normal completion0.
Each directory r068-{Off,Win16,All} retains input hashes, four client-area
captures, operation-specific pixel hashes, probe log and exited observer
report. The GUI candidate uses the r064 provider identity above. This is
actual gameplay/non-regression evidence, not merely a created HWND; explicit
post-case package cleanup remains distinct from normal broker retirement.

Source audit of separate WOW: current Run16 maps SCS_WOW_BINARY only to
BINARY_TYPE_WIN16 and derives CheckVDM flags from PIF/Console ownership; it
has no CLI route selecting BINARY_TYPE_SEPWOW. Hook legacy redirection forwards
native creation flags to the Run16 process but does not encode a separate-WOW
request for its CheckVDM call. Therefore the existing separate-worker style
receiver cannot be declared end-to-end verified via ordinary Run16 invocations.
Do not invent a public option or count a direct broker fixture as proof of
that missing product route. Its required disposition remains open.

### r069 three independent WOW startup frontiers

r069 PID25460 starts12:37:32 with480s deadline and exits0 after serial
baseline/candidate observations. The published baseline is manifest-verified
and uses Off; the candidate uses Win16-only with the same immutable guest
media, controlled environment and observer. Each original program has four
window samples at approximately4/8/12/16s and a bounded20s observer timeout.
The timeout code53504354 is intentional observation termination, not normal
application exit or functional success.

| Guest | Published baseline | Candidate |
| --- | --- | --- |
| WINMINE | visible Minesweeper main window | same main window, title `(Win16 Style) Minesweeper` |
| SOL | empty-caption dialog, `Out of memory` and OK | same error text/button, title `(Win16 Style) ` |
| WRITE | Write dialog, `Not enough memory for Write to complete this operation.` and OK | same error text/button, title `(Win16 Style) Write` |

Raw identities, traces and all samples are in r069-{baseline,candidate}-logs
and their input manifests. No startup frontier regression is observed; this
does not repair SOL/WRITE or convert their error frontiers to functionality.
WINMINE's deeper gameplay/normal-exit proof is r068, not these timeouts. The
r069 stderr log is empty. A fresh filesystem check still finds no candidate
native-frontend/ntcon.exe, so the security interruption has not cleared.

### r070/r071 CLI and boundary audit refresh

The proposal still used the pre-rename `--win16-window-style` spellings; it
now matches the owner's short names and explicitly disclaims old aliases.
The parser implementation is unchanged. The CLI fixture grows from25 to34
cases: it checks default/non-explicit state, old names and option-looking
filenames remaining ordinary target text, both conflict orders, duplicates,
missing target, tabs and command/c target-tail preservation. Existing behavior
rejects two style switches; this run does not change precedence semantics.

r070 PID36116 at12:42:23 (240s deadline) exits0: both-width option/packet/
TDB-parent/DOS-snapshot suites, documentation governance and diff check pass.
After initializer punctuation is normalized, r071 PID23400 at12:43:49
(180s deadline) rebuilds the test inputs and repeats all four suites on both
widths, exit0. Each reports CLI34, three-policy packet validation, original
TDB bounds/signature/lease negatives and cross-thread DOS capture/cleanup.
These are component checks, not the still-unavailable real DOS/CUI gate.

The owner has been asked whether to admit forwarding the caller's existing
CREATE_SEPARATE_WOW_VDM intent through the private Hook/Run16 boundary, without
adding a public CLI switch. That capability does not currently exist in the
ordinary product route. No new route or scope reduction is inferred while
the decision is pending. Full13-image acceptance/publication remains pending
the NTCON security disposition as well as outstanding route verification.

### r072/r073/r074 delivery audit and retained component regressions

r072 PID29404 starts12:45:34 (180s deadline), exits0. All13 asset and
O:/winnt/system32 hashes equal the existing release manifest; the exact rows
are in r072-identities.json. This proves the S5 publication remains coherent,
not that S6 has been published. Documentation governance passes.

r073 PID28832 starts12:48:13 (300s deadline), exits0. Both candidate NTW16S
widths pass the existing real DLL ABI/import/reload, duplicate installation,
foreign-thread refusal, thread isolation, font restoration, unload and
opposite-width rejection tests. Each real W16Style demo opens/closes27 pages,
proves actual viewport movement and exits normally. The retained runner now
pins the process handle and rejects an unavailable exit code; its bounded
timeout uses the supported Kill()/WaitForExit API. The fixture already owns
the demo lifetime through its cleanup job. Original mirrors have no worktree
diff (`git diff --name-only -- src/mvdm src/opennt-host` is empty).

r074 PID5396 starts12:50:24 (600s deadline), exits0. Dependency-driven Ninja
relinks only basesrv-service-reservation-test.exe. The29-case in-process
service suite passes in5706ms with four bounded independent fixtures. Cases
do not bind the global RPC endpoint or compile/run NTCON. This refreshes
service lifecycle/authority/reentry coverage, not real DOS/CUI propagation.

Current completion audit remains **incomplete**:

| Requirement | Strongest current evidence | Disposition |
| --- | --- | --- |
| Short CLI options, exact target tail and component negatives | r071,34 cases per width plus packet/TDB/DOS snapshot suites | Component-proved; real command/c product regression still needed |
| Native GUI creation and inheritance through both native widths | r049 actual CLI / cross-width / nested override and MessageBox matrix | Proved for tested documented API paths; private syscalls/custom painters not claimed |
| Shared-WOW direct and local-child policy isolation | r042/r043 mixed tasks, r055 real local-child lineage, r064 refreshed style-policy fixture | Proved bounded routes |
| Win16/native/Win16 implicit-Hook return | r064/r065 six real width/policy combinations | Passed with original immutable WINOLDAP test dependency |
| Application usability non-regression | r068 WINMINE gameplay and normal exit; r069 three separate frontiers | WINMINE passed; SOL/WRITE remain matched existing errors |
| Existing style DLL/demo compatibility | r073 both widths/27 pages | Passed; no automatic renderer integration claimed |
| Broker authority/lifetime and invalid requests | r074 service29 plus retained explicit style-request fixtures | Component-proved |
| Real DOS/CUI relay and complete product gates | NTCON candidate remains blocked by host security | Not run on complete S6 package; blocked by security disposition |
| Actual separate-WOW launch/policy | Receiver component evidence only; product entry missing | Not achieved; owner scope decision requested |
| Thirteen-image publication, reviewed commit/push | r072 proves only unchanged old release; HEAD af8189e86 | Pending; do not claim S6 delivered or clean worktree |

No current test failure is relabeled as success, no safe subset substitutes
for the objective, and no publication/security override is performed.

Security revalidation after r074: Test-Path now reports the candidate NTCON
present, with creation/last-write12:49:46 and330240bytes. This turn did not
rebuild or restore it (r074's complete Ninja output is only the service-test
link). A read-only Get-FileHash fails with Windows' virus/potentially-unwanted-
software error, so no hash, trust or usable-runtime conclusion can be drawn.
The file is not executed. This external file-state change does not clear the
security blocker or authorize retries that bypass protection.

### Final bounded source review and blocked disposition

The follow-up review reads the remaining command snapshot, command-process
adapter, BaseClient wire handling, broker style lookup and WOW registration
and wait boundaries. DOS policy is captured before the detached process
thread and cleared by both finish and abort. Snapshot access checks the
existing session identity/epoch and active state. Broker delivery selects the
original BUSY record and originating receipt; shared-WOW lookup checks the
authenticated worker generation, dispatched record and originating receipt.
These are source-review observations, not substitutes for the outstanding
real DOS/CUI tests. No product code or original mirror changes are made in
this review.

Fresh Defender queries could not retrieve live detections: the current-shell
module could not load its remoting dependency, and Windows PowerShell reported
unavailable CIM resource access. Retained detection evidence and the blocked
file read therefore remain the last confirmed security state; neither a
false-positive verdict nor clearance is inferred. No blocked image was run,
restored or republished.

The same security obstacle has persisted across more than three consecutive
goal turns. The independently executable source review and retained component
tests are now exhausted. Full acceptance/publication needs an external
security disposition; the separate-WOW product-entry scope question also
remains unanswered. The goal is blocked, not completed or owner-paused. Keep
all source changes and raw evidence for continuation; do not close S6, claim
a clean worktree or replace the coherent published S5 package with a subset.
