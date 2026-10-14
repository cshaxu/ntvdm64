# T446 S1 carrier visibility guard

## Admission and ownership

Owner admits queued item1 and requests execution. Baseline is6fc7e3db5 and
the coherent T445 S11 package. The proposal's mechanism/integration/test steps
are consolidated into one end-to-end S1 delivery, with no scope expansion.
Single implementer, then sequential review; no new agent or guest changes.

The original source ladder stops at the private Console-server boundary:
this product's separate NTVWM hidden carrier and NTCON presentation are
project-owned policy, not an original DOS/WOW execution behavior. Existing
console_state.c and the prior Console WinEvent probe provide the usable local
owner and observation mechanism. Importing OpenNT Console/USER server code
or applying an external intrusion supplies no needed original state machine.
The smallest added code is a local guard, not a window virtualization API.

The callback's HWND is captured once after carrier initialization. No PID,
title, class or executable-name discovery determines eligibility. Hook start,
same-thread unhook and stop/join retain the public contract documented by
[SetWinEventHook](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwineventhook)
and [UnhookWinEvent](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-unhookwinevent).
The [asynchronous hide](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindowasync)
does not block the callback on the window owner. No imported source/resource.

## Implementation review

console_state owns an opaque object with ready/stop/thread handles. One TLS
binding belongs solely to the installing thread's callback. Startup error and
runtime error have separate fields, avoiding an unsynchronized startup read.
The wait uses INFINITE and messages; zero-time stop checks only bound message
draining, not periodic sampling. Callback errors are reported after join,
never through the capture logger inside the callback.

main owns the borrowed HWND and guard. Startup happens before memberships or
requests; partial initialization cleans the guard. The shared close_console
wrapper stops/joins before the existing Console-close function; both fatal
paths use the wrapper. Normal membership cleanup also stops it. end_io keeps
checkpoint/final-I/O ordering and hides before release or return, without
overwriting errors. No presentation-loop cadence or lifetime changes.

## Verification chronology

Runs are under build/M0-T446/S1, owned background processes with deadlines.
focus1 failed on a missing x64 compiler-wrapper path; corrected by an explicit
build-owned x64 wrapper. focus2/focus3 pass30 real hook cycles, exact-window
filtering, unrelated visible HWND, stop/show races, no-HWND no-op and handle
counts87/87. Test polling observes the result only; production has no polling.
The first native graph generation used an absolute path where its interface
requires a relative build root; corrected without changing the generator.
The AMD64 /MT /O2 worker build passes. No other image is rebuilt or changed.

The first production comparison is blocked by live deployed NTSRV PID26532;
the ownership gate refuses to control that owner session. Owner permission
to close it has been requested. This is not product test failure or a pass.
No candidate is published, committed or presented for acceptance yet.

Subsequent sequential review places guard destruction after input-watch
destruction, so no remaining input-failure callback can concurrently stop it.
The updated worker is rebuilt. worker-fixtures passes presentation (530
checks, zero failures), input return and execution lifetime on private
desktops with local typed broker substitutes; these do not occupy the live
BaseSrv endpoint and do not substitute for actual production validation.
Documentation governance passes. The candidate package staged before this
last main.c adjustment must be refreshed from worker/ntvwm.exe before any
production run or publication.

Reproducible entries: tests/observation/verify-ntvwm-carrier-guard.ps1 with
BuildRoot and CompilerWrapper; verify-ntvwm-carrier-product.ps1 with a sealed
PackageRoot, compiled Fixture and fresh BuildRoot (Negative for old release).
The latter requires two actual Run16 requests with unchanged exit37 and the
same hidden carrier after completion; old release must show the captured
carrier and return the fixture's visibility-failure7.

## Completed verification and sequential review

The owner permanently authorizes identity-checked project-process closure
for testing and replacement; EXECUTION records the boundary. PID26532 had
already ended when rechecked, so no owner process was terminated. All later
test cleanup remains limited to pinned processes in owned physical stages.

The final worker was refreshed into package/system32 before product tests.
negative2 uses the prior S11 worker: both direct requests return visibility
failure7, with carrier187B0820 still visible. product2 uses the final worker:
both requests return37, share carrier0B87068C (window owner47464), and keep
it hidden after completion; target PIDs31500/12712 each execute three show
cycles with98/98 handles. Outer Console09E607D0 is distinct. No fixture
guard is installed in product mode: these exercise actual NTVWM integration.

order3 runs verify-ntvwm-carrier-order.ps1 with BuildRoot, CompilerWrapper
and FontInclude. It includes actual main.c and substitutes only reached I/O
dependencies. All eight KEEP/RELEASE/error cases pass: final I/O precedes
hide, hide precedes release, original error codes survive cosmetic failure,
and stop precedes close while retaining its return123. order1/2 were fixture
link failures corrected by giving the unused renamed main internal linkage.
These injected cases supplement, not replace, the real-package checks.

The final worker-fixtures lifetime result is1084 checks, zero failures,
48 completed,16 cancelled, target-survival=yes and zero remaining handles.
Presentation passes530 checks; input-return also passes. The guard test
passes30 start/show/stop cycles, including actual asynchronous events and
unrelated-window filtering. No-HWND is tested by an explicit null input;
this does not claim a separate real pseudoconsole host was exercised.

| Required suite | Retained run | Result |
| --- | --- | --- |
| Product | S1/r020/product-gates | All3 groups,155775ms: WOW frontiers, Console17 routes, Window17 routes. |
| Control | S1/r023/control-gates | All11 groups,201836ms: RPC, native GUI, version negatives, strict interactive DIR, EDIT return and remaining lifecycle/handoff gates. |
| WINMINE gameplay | S1/mine-Off, mine-Win16, mine-All | Flag/counter, reset, cell opening, WM_CLOSE and Run16 completion pass in all three modes; original screenshots retained. |
| Actual MySMB Esc | S1/game4 | Process47620, game HWND09FB07D0, carrier02671370; exit0 and same carrier hidden. |
| Published-byte product check | S1/publication/deployed-check | Two real requests/reuse pass using ntvwm.exe copied back from the published installation into an owned isolated package. |

Control r021 initially failed because version-negative compilation received a
runtime directory instead of the NTSRV build directory. r023 uses the retained
native-service build with matching binary identity; it does not waive or
change version negatives. SOL/WRITE retain their separately recorded prior
initialization-failure frontiers; these are not functional success claims.

MySMB game1 wrongly waited for a COMMAND prompt; no Esc was delivered. Its
cleanup also omitted the explicit game image, leaving its DLL mapped.
game2/3 corrected the input model but failed the probe's path comparison
because a forward slash differed from QueryFullProcessImageName output.
The final probe normalizes separators, identifies the exact staged image,
captures its Console HWND through AttachConsole, and writes Esc to that
Console. MySMB selects text mode when a parent Console exists. Thus game4
proves real game Esc/completion/carrier behavior, not simulated physical
keyboard delivery through NTCON; front-end routing is covered by Product
and Control. Cleanup now includes the staged game in its exact identity set.
Failed reports remain in build; none is counted as a pass.

Final cleanup also canonicalizes the explicit game path in the PowerShell
ownership set. game5 repeats Esc/hidden completion successfully with this
final harness. The three partially removed failed-run stages n7bd/nca3/ncb7
are deleted only after verifying their exact O:/tmp paths, this package's
ownership markers and process identities. They were disposable copies;
original inputs and all failure evidence remain. Final documentation
governance and git diff --check pass.

Reproduction: verify-ntvwm-carrier-game.ps1 takes PackageRoot,
CompilerWrapper (AMD64 MSVC), Game (explicit supplied mysmb64.exe), and a
fresh BuildRoot. It builds ntvwm_carrier_game_test.c and uses an unswitched
private desktop. verify-wow-mine-gameplay.ps1 takes PackageRoot, Observer,
CompilerDriver, fresh BuildRoot and WindowStyle Off/Win16/All. Background
runner PIDs, deadlines, commands and output paths are in the S1 owner JSON
and scripts. Product/Control entrypoints and exact arguments are retained
in gates.ps1 and control.ps1; timings and runtime manifests pin their inputs.

Final source review checks all normal/fatal close paths, post-input-watch
destruction ordering, same-thread unhook, startup/runtime error separation,
callback isolation and unchanged final-I/O semantics. No original mirror,
guest, shared ABI, routing policy or presentation cadence changes occur.
The source manifest predates the final cleanup-order edit; fixtures rebuilds
main.c and publication/source-hashes.json records final source identity.

## P1 publication and acceptance boundary

publication/exit.txt is0. Recoverable backups of all13 images in both targets
and the old release manifest precede replacement. Only ntvwm.exe changes;
the other12 retain the tested T445 identities. The verified candidate,
assets/release and O:/winnt/system32 all match for every image.
Final ntvwm.exe SHA-256:
5BBFF7BCE0FEE0D44448DF48E861AB8D0DFBFC9F09F39BB96B6300F2A67DCAA8.
No live installed project processes required termination during publication.
The post-publication check runs an identical-byte owned stage, not an
interactive test inside the user's installation.

This S1 P1 records the reviewed delivery for owner acceptance. The event-driven hide
is asynchronous best effort: it cannot prevent every transient visible frame
before event delivery. This delivery makes no zero-flicker, Console-retirement
or unrelated Win3.1 performance claim. The owner subsequently confirmed
acceptance and authorized whole-T closure and next-task admission; the
[closure](../../history/m0-t446-carrier-visibility-closure.md) records this
disposition against delivered ca0abb7c4 without new runtime changes.
