# M0 T423 S40 native alternate-screen geometry

## Scope and baseline

Owner requested investigation and repair of COMMAND -> CMD -> installed modern
EDIT unexpectedly ending CMD. Baseline is S39 `3c592ec1e`, protocol/RPC 28,
the coherent eight-file O:/winnt package. No guest, mirror, shared library,
worker completion policy, process/component or polling change is made.

## Proven failure

Private-desktop actual-package reproductions are retained under
build/M0-T423/S39/r001 as edit-chain-01, edit-chain-04 and edit-chain-05 reports
and geometry error logs. The test-only fast process witness
edit-chain-fast-processes-05.json pins actual process handles: EDIT started,
then CMD and EDIT exited with 0xC000013A; NTCON exited with 87. CMD without
EDIT stayed alive until deliberate observer cleanup. A WMI process-event
subscription was denied; no WMI event coverage is claimed.

NTCON capture already orders native geometry transfer correctly: grow storage,
apply viewport, then shrink storage. EDIT changes its alternate buffer to
80x28. NTKVM's WINDOW_RECT handler incorrectly treated matching cached logical
geometry as proof that the actual canonical viewport was already applied.
The real canonical viewport could still be 120x30 or 80x30. Its next
BUFFER_SIZE(80,28) consequently failed with ERROR_INVALID_PARAMETER.

The cached-geometry early exit originated in S24 `80f704afd7`. The error reaches
NTCON's existing unrecoverable presentation failure path, which closes its
Console session and exits. This explains both CMD and EDIT termination; it
is not an EDIT launch failure or a scheduler/receipt issue.

Windows requires buffer dimensions to contain the actual viewport, as specified
by [SetConsoleScreenBufferSize](https://learn.microsoft.com/en-us/windows/console/setconsolescreenbuffersize).
Separate screen buffers can have separate windows; the viewport change must be
applied to the actual output buffer, not inferred from another cached region.
See [SetConsoleWindowInfo](https://learn.microsoft.com/en-us/windows/console/setconsolewindowinfo).

## Repair and adjacent audit

Only src/ntkvm-exe/console_frontend.c changes production behavior. Compute the
same physical projection as before, compare it against GetConsoleScreenBufferInfo's
actual srWindow, and skip the resize only when those physical rectangles match.
Publish the logical rectangle only on success. Existing channel locking,
pixel constraints, OpenNT cell-grid primitive and repeated-request no-op remain.

No ERROR_INVALID_PARAMETER is swallowed. Genuine fatal Console-session failure
still takes the existing NTCON close path. NTCON's already-correct grow/window/
shrink ordering is retained. SCREEN_INFO's logical region remains a deliberate
presentation contract, not proof of actual host geometry. NTVDM and guest
execution, original text-height policy and frame/transport layouts are unchanged.

## Reproducible tests

Build root: build/M0-T423/S40/r001. VS2022 MSVC14.43.34808 x86 /MT, SDK
10.0.22621.0, CCPU40; unchanged source/build inputs reuse the sealed S39 cache.
Generate with tools/build/New-T310OriginalSoftpcNinja.ps1, Architecture=x86,
BuildRoot=the above root, NodeExecutable=O:/.nvm/versions/node/v22.22.1/bin/node.exe.
Execute its compile/link commands for console_frontend.obj, ntkvm.exe and
console-channel-lifetime-test.exe. Only NTKVM changes among the eight products.
The observer is separately compiled x86 /MT from console_startup_observer.c
with user32.lib and dbghelp.lib, with object and EXE under the build root.

| Checklist | Entrypoint and assertions | Evidence/result |
| --- | --- | --- |
| Baseline distinguishes defect | New test_native_geometry_projection(25/28), compiled against unchanged production dispatcher; logical viewport matches while real viewport remains 80x30 | console-channel-before-fix-test.exe --private-desktop s40-geometry-before-02.txt: fails actual viewport assertion at line 529 |
| Actual viewport and storage | console-channel-lifetime-test.exe --private-desktop-full s40-channel-final.txt; 25/28 rows, repeated request, shrink/grow, cells/cursor preserved, invalid rectangle rejected without cache mutation | Pass, including existing Console park/resume, lifecycle/handle fixture |
| Actual program return | tests/observation/verify-command-native-edit-return.ps1 with Observer=build-root/observer.exe, PackageRoot=Z:/ and ReportPath=build-root/edit-return-02.txt; SCRIPT uses installed original EDIT, Ctrl+Q, exact CMD echo, DOS MEM, final original COMMAND result 1 | Pass: EDIT menu/status frame, native return text, real DOS memory output and launcher completion |
| Client completion/lifetime | frontend-scope-lifetime-test.exe and frontend-request-client-test.exe from unchanged S39 inputs | Pass: retirement/join, isolated capability, actual exit 37, resume and malformed/EOF/version/failure checks |
| Product regression | tools/audit/Verify-CommandExitStatus.ps1, Observer=build-root/observer.exe, PackageRoot=Z:/, ProcessPackageRoot=build-root/runtime, LogRoot=build-root, GuestFixturePath=build/M0-T423/S38/candidate/G7.COM, OrdinaryFrontend | Console17 and Window17 each 17/17 pass; summaries s40-console17-summary.json and s40-window17-summary.json |
| Retained WOW frontier | tests/observation/observe-wow-frontiers.ps1, Observer=build-root/observer.exe, WindowObserver=build/M0-T423/S9/publication-window-reader.exe, Prefix=s40-wow-frontier, PackageRoot=Z:/, ProcessPackageRoot=build-root/runtime, LogRoot=build-root, PostExitObservationMs=5000 | All three observations complete; WINMINE visible guest window retained; SOL/WRITE usability not claimed |

First geometry fixture attempt s40-geometry-before.txt failed its own default
buffer preparation, not the target assertion; it was corrected before the
valid baseline negative control. First edit-after probe also returned normally
and displayed EDIT, but its VER witness was native Windows, so it is not used
as DOS-return proof. edit-return-02 uses DOS MEM instead. Expected geometry
retry 1237 and unavailable optional configuration 1168 are not reclassified
as failures or suppressed by the repair.

Observer change is test-only: Ctrl+Q uses paired physical Control/Q records;
it does not terminate EDIT or fake output. Tests require real Console APIs,
installed modern EDIT, an authenticated production package, and a private
desktop. There is no target/provider substitute in the actual-program test.

An additional synthetic Window-chord diagnostic edit-return-window-03.txt
does not pass: SendMessage's Ctrl/Q messages do not establish physical UI-thread
Control state, and EDIT receives an ordinary q. Its actual EDIT frame stays
alive, without the original geometry error 87, until observer cleanup. This is
not proof of physical Ctrl+Q or normal EDIT return in Window. The final observer
now explicitly rejects this unsupported control-chord route, and the dedicated
return fixture rejects Window-input configuration rather than claiming success.
The completed Window17 matrix does not use this added Ctrl+Q-only route; its
existing inputs/production artifacts are unchanged by that harness safeguard.

## Publication and verification

Focused tests pass and the coherent package has been published to O:/winnt.
Recovery copy is build-root/published-recovery; prepublication-hashes.json and
published-hashes.json prove all eight files. Only NTKVM differs from S39:
SHA256 `886E2ECC9D8937F17E21A63E634018AC3F8087766632D9BB582B30F731EE1F94`.
Other seven hashes exactly match the indexed S39 evidence. Guest/configuration
and NTVDM.REG are not overwritten. Only exact O:/winnt package processes are
eligible for the owner's permanently authorized publication stop.

Final observer and return fixture also pass against the actual O:/winnt
publication: edit-return-published-04.txt. This checks the precise return code
1 from original COMMAND as well as EDIT menu/status, CMD's exact echo witness
and DOS MEM text; no error exit is treated as a successful return chain.

Console17 and Window17 each have 17 entries and zero mismatches, enforcing
guest output/interaction as well as actual exit results. Private-desktop WOW
observations retain the prior frontiers; unrelated host windows are not guest
successes. Final diff review covers both viewport validation and its existing
enter/leave ownership lock, genuine-fatal policy, repeated requests, malformed
geometry, cell/cursor retention and shrink/grow behavior. Documentation
governance and diff checks are required before delivery. T423 remains open.
No physical desktop, RDP, or owner Windows Terminal observation is claimed;
SOL/WRITE retained frontier limitations remain non-passes.
