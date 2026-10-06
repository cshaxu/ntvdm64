# M0 T433 S5 — native launcher migration

## Scope and implementation

The single run16.exe is built as AMD64, with architecture-local common/RPC,
client/MIDL, context-only Hook and original BaseClient/capture/RTL dependencies.
No second launcher, worker, helper, classifier, search policy or protocol is
introduced. NTSRV/NTVDM/WOW32/VDMREDIR/Hook32 remain I386. NTCON/NTVWM/NTMON/
run16/Hook64 are AMD64; APP0.0.433/RPC41/I/O25 and the ten filenames remain.

The original classifier is composed after native declarations with the existing
Hook64 classifier-local guest-rule header. Project SEC_IMAGE fallback retains
both native target widths. CWD then PATH and per-directory COM/EXE/BAT/PIF
search, CLI argument semantics, GUI startup-only/explicit wait, Win16 startup,
direct receipts and the outer Console restoration barrier are unchanged.
The launcher remains context-only, not an interception worker.

frontend_scope.c now parses and formats full-width local HANDLE locators,
rejecting zero, negative, overflow and trailing data before broker validation.
These strings are not authorization or copied wire handles. Both-width fixtures
prove that a high locator reaches authenticated validation unchanged on x64
and is rejected rather than truncated on x86.

## Minimal source/provenance ledger

No MVDM/guest source or binary changes. Selected original environ.c/error.c
algorithms remain unchanged. The following registered expressions/declarations
are the entire new historical-boundary diff against admission6aaee3809:

| Register / owner | Required adaptation and unchanged contract | Verification |
| --- | --- | --- |
| OPENNT-HOST-065; original base/win32/client/vdm.c | Four expressions: two native pointer distances before numeric narrowing, STARTF_USEHOTKEY's numeric DWORD and UndoCreation's numeric task ID through ULONG_PTR. Same classification, environment, command and cleanup ordering. | Strict native compile, image-matched map, search/classification, actual DOS/WOW and mixed-width routes. |
| OPENNT-HOST-066; original base/ntdll/csrutil.c and ntcsrmsg.h declaration | Full local pointer-table elements and native allocation alignment. DWORD lengths/counts and copied wire records are not widened. x86 retains four-byte alignment and32-bit elements; x64 uses eight. | Real original allocator,3462 checks per width,48/28-byte header, complete stack-pointer metadata and handle-neutral free. |
| OPENNT-HOST-067; existing standalone base/ntos/rtl/zwapi.h carrier | Modern local NT VM lengths/region pointers use SIZE_T/PSIZE_T; ZeroBits ULONG_PTR. No original RTL algorithm change, private modern PEB cast or new wrapper. x86 widths unchanged. | Native mutable environment failed before the declaration correction and passes after; unchanged x86 full RTL fixture also passes. |

Upstream source remains the pinned selected OpenNT package. Read-only source
hashes: O:/repos.external/OpenNT/base/win32/client/vdm.c
3F03D0DBB08E0163F2D9CF415DAAD0981E42E1B1855F6F48A3B59022B7374173;
base/ntdll/csrutil.c
7A176482498E1ACA00E9651C326A9B62A8BA2ADA0AA7F90F8B585ACD1A376E6A;
public/sdk/inc/ntcsrmsg.h
896D25BB240D105C64F96AB06F97BEE8E013AD7C4E679A0292B07C5E594C6FC7.
Existing selected subsets and earlier seams are retained, not claimed identical
to entire upstream files. zwapi.h is an existing project declaration carrier,
not imported original algorithm. Register entries live in both owner READMEs.

build/M0-T433/S5/r002-native/mirror-source-identity.json and mirror-source.diff
retain baseline Git blobs, current hashes and complete changed expressions.
Line accounting including comments: vdm.c11+/4-, csrutil.c8+/5-, ntcsrmsg.h4+/2-,
zwapi.h6+/4-. No new original function body. GetVdmConfigInfo's distance fix is
necessary to compile the selected TU but that function/config bodies are absent
from the actual run16 map; it is not a newly exposed runtime policy.

The standalone provider still ignores original capture rebasing metadata and
encodes fixed copied records; this does not revive NTDLL CSR transport. Native
capture alignment is a local allocation choice, not a new wire requirement.
The finite RTL fixture's native selection excludes x86 arithmetic bodies;
environment/error assertions are retained. Its minimal private PEB supplements,
rather than substitutes for, real launcher environment ingress.

## Builds, reproducible tests and retained attempts

Baseline is sealed S4 build/M0-T433/S4/r002-runtime, production03456b24ce2616080fbb3bc84090939c42b71f22,
closuredf3a50c70. All new intermediate/evidence products are under S5 build/.

- r001-mirror-audit: selected original client/classifier/capture/environment/error
  strict AMD64 compile. Initial pointer-narrowing failures remain evidence;
  no casts are hidden by disabling narrowing diagnostics.
- Native graph: New-NativeWorkerNinja.ps1 -Component Launcher -BuildRoot
  build/M0-T433/S5/r002-native -ReferenceGraph build/M0-T427/S2/r001/build.ninja;
  run-ninja.cmd run16.exe native-capture-test.exe frontend-scope-lifetime-test.exe
  application-search-test.exe run16-image-classification-test.exe rtl-x86-fixture.exe.
  MSVC14.43/SDK22621/MT; /we4013 /we4311 /we4302, native libraries/MIDL and map.
- Formal x86 generator imports explicit NativeLauncher alongside NativeWorker,
  NativeFrontend and NativeMonitor; run16-source-closure remains auditable.
  NTSRV/NTVDM, Hook32, VDMREDIR and separate WOW32 closure are rebuilt for changed
  dependency inputs. Unaffected native consumers are dependency-checked/relinked.
- r009-abi: tests/observation/verify-native-launcher-abi.ps1 with NativeBuild
  r002-native, X86Build build/M0-T427/S2/r001, PreviousLauncher S4 sealed run16,
  Hook64 S2 cache and fresh LogRoot. Both capture3462 and lifetime fixtures pass;
  original mutable environment/query/delete/clone/free/error tests pass both
  widths, native classification424 checks/handle delta0 and search pass. Real
  native import preserves the hash; x86 image, DLL and outside-build inputs are
  rejected. Source recipe has no x86 launcher fallback.
- r003/r004 are initial startup evidence, not publication inputs. Empty COMMAND,
  direct MEM, native CMD and nested MEM pass. r005/r006 is superseded by the
  final NT VM declaration fix. r006 passes13 groups but fails the last script's
  PowerShell comparison typo; this is not a complete Full pass. Both retained
  ABI-sensitive Sysnative scripts are corrected; no assertion is weakened.
- Initial native RTL fixture arithmetic link fails because those original x86
  bodies are deliberately unselected. Finite environment/error selection then
  exposes the real wrong-width NT VM declaration: first set returnsC0000005.
  With the local API declaration correction all retained environment tests pass;
  diagnostics and failure logs are preserved, not counted as passes.
- Final package r007-final-runtime is staged with explicit NativeLauncher and
  all other native producer inputs using Stage-System32ProductPackage.ps1.
  Product media/configuration and Detours notice remain baseline-identical.
  r008-full reruns Invoke-ProductVerification.ps1 -Suite Full with this package,
  matching x86/native caches, native observer and MonitorRpc, G7.COM, Terminal
  observer and preceding S4/S3 WOW baselines. Final results/publication follow
  below; until recorded this evidence is implementation progress, not closure.

## Current delivery gate — still open

r008 Full is NOT a pass: WOW and Console17 pass, then Window's native-zero
case times out with observer53504354. The recorded COMMAND page stops atVER;
the observer's first output milestone times out and remaining input is not
delivered. This differs from the earlier passing r006 observation. It is not
proof that CMD receipt code itself failed, nor permission to waive the case.
No production package has been published or committed for S5.

r010 runs both actual Hook installers using the final package's launcher/DLLs
in owned test directories:147 assertions per origin pass, including alternating
widths, real Windows child handles/suspension, GUI/CUI and failure cleanup.
r011 compares the exact native-zero Window case serially against sealed S4 and
final S5, three runs each, same observer/geometry/inputs/assertions. All six
pass; this does not identify the failed r008 cause or prove it repaired.
The comparison initially finds a concurrently started O:/winnt broker and
refuses ownership. That attempt is not a pass. The exact approved published
processes are stopped, the test-owned Z mapping is removed, then the isolated
comparison runs. The O:/winnt broker was created after r008 failed, so it is
not claimed as r008's cause. Z is released after the valid comparison.

The initial stop condition kept S5 open without publication. The owner then
directed continued investigation; the following diagnosis supersedes the
earlier uncertainty, not the retained failed observation.

### Window timeout diagnosis and strict test correction

The first and only milestone wait is for the typed lower-case ver before
Enter; the recorded page contains upper-case VER. write_console_input_text
returns at that echo mismatch and never reaches its Enter branch. Thus this
was not evidence of native target completion deadlock: no VER command had
been submitted. SendMessage supplies WM_KEYDOWN but does not update the target
UI thread's keyboard-state table; the provider legitimately translates the
actual modifier/toggle state. The Console fixture had explicit NUMLOCK_ON
records while the Window fixture omitted the equivalent native input facts.

r012 builds a fresh AMD64 observer from the same selected graph. Its test-only
keyboard message wrapper guards the owned private desktop and expected Window
PID, connects the two input queues using AttachThreadInput, supplies the exact
record's modifier/toggle state through SetKeyboardState, sends the existing
message, detaches and restores the observer's prior state on all paths. No
global SendInput/focus change, guest write, helper or production protocol.
Strict case-sensitive echo, stale-row/cursor/tail negatives remain unchanged;
the unit fixture adds explicit MEM versus mem rejection. Enter/message counters
record submission separately from completion. This does not claim physical
focus or synthetic Window Ctrl chords, which remain explicitly unsupported.

Microsoft documents shared keyboard state for attached threads and that posted
messages do not themselves update another queue's state:
[GetKeyboardState](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeyboardstate),
[AttachThreadInput](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-attachthreadinput),
[SetKeyboardState](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setkeyboardstate).
This local state does not change the physical keyboard's indicator/global
state; the helper refuses ordinary desktops and attachment across desktops
is rejected by Windows.

r013 controlled negative sets Caps in that private shared table: real VER,
one failed exact echo milestone, six key messages and zero Enter reproduce
the r008 signature. Correct input supplies ver then exit and reports two Enter
events, normal output and real exit0. Reproducible entrypoint is
tests/observation/verify-owned-window-input-state.ps1 with RuntimeRoot
r007-final-runtime, Observer r012-observer/console-startup-observer.exe and
fresh build LogRoot. r014 reruns the entire Full suite using the corrected
observer against the unchanged final product bytes. No longer timeout, injected
retry or relaxed marker is used. S4 remains published until the full gate and
subsequent delivery below; S6 is not admitted.

### Retained rapid-interactive failure and further evidence

r014 passes WOW, Console17, Window17, RPC7, GUI6, version5, strict DIR,
modern EDIT, cooked return and immediate relaunch; its interactive-relaunch
group fails outer81 after the11th marker. This is a separate observation:
input was delivered and six records remain. The batch's existing errorlevel
predicate fired; the original report does not preserve the direct launcher's
actual nonzero value, so neither target exit1 nor a broker fault is asserted.
No input/exit assertion is weakened or changed to explicit exit0.

The test now prints the actual launcher status only in its already-failing
branch, preserving predicate and outer81, and enables existing optional NTVWM
error evidence in a build-owned file. Success-path output/queue behavior is
unchanged. r022 compares three unmodified twelve-round bursts per S4/S5
package: all six pass. r023/r024 is a diagnostic-only launcher composition
using tests/observation/launcher_receipt_probe.c: the unchanged production main
and real providers, failure-only wrappers separating startup, receipt code/
completion, retirement and restoration. It never supplies a receipt or changes
Win32 last-error. The diagnostic image is not a publication input, and its
copied baseline manifest must not be used as a sealed product manifest.
r025 completes twelve twelve-round diagnostic bursts with no failures144 total;
this is bounded diagnostic evidence, not a claim the r014 cause was repaired.

r026 reruns the full unchanged final product with the enhanced failure evidence.
All historical failures remain distinct from its results. Repeated isolated
passes do not substitute for the full gate or authorize weaker assertions.

## Final verification and delivery

r026 Full passes all14 groups in442666ms with the unchanged r007 product:
Console17/Window17, independent WOW frontiers, RPC7, GUI6, version negatives5,
strict DIR, real modern EDIT return, cooked CMD return,12 immediate relaunch
pairs,12 interactive relaunches, independent sessions/management, real frontend
loss/worker shutdown/receipt1067, and actual I386 nested Window return23.
No failed case is removed. The final rapid result is outer19 with the required
final/complete markers and no error tolerance; the isolated earlier81 remains
an explicitly unclassified debt, not claimed repaired or source-attributed.

r027 runs the committed owned-Window input entrypoint: controlled Caps negative
still fails before Enter; exact normal input passes. r028/r029 each pass ten
actual native Hook-to-legacy/search/parent-return cases and visible WINMINE
startup, for both I386 and AMD64 origins and the single AMD64 launcher. r030
adds actual AMD64 CMD/DOS/MEM/parent-return/CAF nested Window exit23. These are
actual processes, not the diagnostic launcher. r031 rejects missing native
launcher selection, I386 launcher input and outside-build input during package
staging; native import negatives are additionally in r009.

The reviewed native import regenerates from the final explicit producer and
Ninja reports no work; source-manifest selection is AMD64, not an x86 fallback.
Production run16 SHA-256 is
B92F9E400BBAFBDAE2D8D9E7E0F3833497A52079EE4A27D649693231529DCF77.
Formal cache, sealed r007 and published binary match. Original mirror accounting,
maps, source/dependency rebuilds, compiler narrowing checks and documentation/
relative links/diff review remain in r002-native.

r020 publication requires all14 final groups, verifies the S4 published set,
backs up its ten images/notice/configuration, copies only the final ten product
images and notice, and verifies all hashes at O:/winnt/system32. Guest media,
configuration and immutable recovery remain unchanged. r021 deployed smoke
passes COMMAND/empty0, native VER0, nested MEM1 and direct MEM0, then explicit
System32 AMD64 and SysWOW64 I386 CMD VER/output/exit0 using the actual published
launcher. All ten hashes plus notice match and Z is released. The initial
smoke script fails before process launch because it constructs a Z path before
mapping; that attempt is not a pass. Its corrected explicit alias construction
and complete run are retained separately, without any product change.
The diagnostic launcher in r024 is never published. S5 is a bounded automated
native migration delivery; T433 remains open and S6 is not admitted.

## Verification boundaries

No physical desktop/RDP/Windows Terminal manual acceptance is claimed. Real
process tests use the owned observer Console with explicit SHORT_HISTORY
geometry. Default private-desktop geometry, arbitrary large-environment behavior
and known long-path limitations are not repaired; Z: alone is used and released.
WINMINE visible startup is not gameplay; SOL/WRITE preserve independent known
frontiers. T433 remains open; S6 requires separate admission.
