# T445 S8 style integration implementation evidence

## Scope and baseline

Owner approves the [design](../operations/t445-s8-style-integration-design.md)
and requests single-person implementation followed by independent-role review,
background build/tests, coherent publication, commit/push and manual acceptance.
Baseline product is S7 292664b02, design delivery550f4905f. Three admission-document edits from
the preceding turn are retained. No guest or original mirror changes admitted.

## Current implementation and boundaries

- NTW16S ABI2 adds nested same-thread creation scopes and actual attachment
  queries. Off scopes mask enabled outer scopes. CBT captures original style
  and local control-theme state before native construction; returned creation
  installs the existing renderer, not a duplicate. Destruction releases state.
- Both native Hook widths call that DLL scope. WOW32 wraps its existing
  task-owned window/dialog/message boundaries; ntvdm.exe is not a style owner.
- Native Hook binds the finite scroll APIs. WOW build bindings select only
  original wuser/wuser31/wuwind scroll callers through project-owned facades,
  without editing mirrors. Native USER retains range/position/visibility state.
- Theme capture/override is per fresh control, never a global setting. Native
  API binding, callback reentry, retained application strings and detach restore
  are exercised in both widths; original native leaves avoid recursion without
  bypassing unrelated application reentry.

## Attempts and observations

All outputs below are under build/M0-T445/S8. Owned hidden runners retain a
pinned process handle and impose the stated deadline. Ninja caches are mutable
style-cache plus the established S6 worker/hook/provider caches; S7 r090 package
is retained separately and never overwritten.

| Run | Start local / PID / deadline | Result |
| --- | --- | --- |
| r002 admission | 17:17:22 /39264 /120s | Documentation governance exit0; diff check passes. |
| attachment | 17:22:18 /19484 /300s | x86 compile fails on fixture function/data-pointer casts under /WX; fixed explicit function types. |
| attachment-r004 | 17:23:51 /26924 /300s | Builds; sandbox denies private desktop before assertions. |
| attachment-r005 | 17:25:15 /22408 /300s | Outside sandbox, desktop created; scoped hook denied because fixture omitted DESKTOP_HOOKCONTROL. Add that right only to the owned desktop. |
| attachment-r006 | 17:25:51 /25228 /300s | Both widths pass real DLL nested Off, controls created in WM_CREATE, later child, remove, failed creation and30 lifetimes; GDI3/3 each. |
| consumers | 17:27:17 /45656 /900s | Hook32/64 and WOW32 build. x86 native window/MessageBox cases pass; x64 outer windows observer reaches90s deadline. Not an integrated pass. |
| attachment-r008 | 17:35:14 /47012 /300s | /WX rejects possibly uninitialized SEH local in theme helper; initialized to E_FAIL. |
| attachment-r009 | 17:36:08 /43108 /300s | Both widths pass extended List/Combo/tab-state/theme-enabled creation and30 lifetimes; GDI3/3 each. |

Entrypoint: tests/observation/verify-style-attachment.ps1 -BuildRoot
build/M0-T445/S8/style-cache. It builds both DLLs and tests/app/style_attachment.c
with MSVC /MT /O2 /W4 /WX. These are focused mechanical tests, not visual or
whole-product acceptance. The accepted standalone gallery and complete product
gates have not yet been rerun for this candidate.

The first consumer run uses build-consumers.ps1 with the pre-scroll-binding
candidate and the retained native-title observer. Its x64 All output records
four advancing GUI-thread suites at tick576174265..576264281 (90.016s), ending
with child PASS before the outer90s timeout. Off/Win16-only suites also show
native modal delays on later GUI threads; this is not proof of a style-only
hang. The outer deadline becomes120s to contain the composed suites and the
existing nested45s wait; assertions and product waits are unchanged. Need a
passing repeat and actual DLL-attachment assertions, now added to the fixture;
do not reinterpret the failed run as success.

## Follow-up implementation and diagnosis (2026-10-13)

- ABI2 binds exactly three original native leaves (GetScrollInfo,
  SetScrollInfo, SetWindowTheme) before native attachment. WOW/direct callers
  use native defaults. This avoids ambient recursion guards that could bypass
  application reentry. r017 passes both widths including invalid/repeated/
  conflicting binding, nested theme requests and restoration of the latest
  successful request. Theme removal now checks its attachment owner identity,
  not just a potentially recycled combo-list HWND. Scroll notification repaint
  is restricted to bars, not the whole frame/client.
- r018 links current WOW owner objects and passes task style policy, A/W modal,
  same-runtime Off peer and parent retention (errors0, native_direct4).
- r016 standalone fails live arrow pixels; r019 repeats the identical failure
  using the **published S7 DLL**. r020 records screen640x480 and an empty visible
  DC clip for the bar at x1108. This is not evidence of a new S8 painter defect.
  Owner cannot connect or test; adapt test-owned viewport placement before each
  input sequence, not the sampling operation, and keep real visible-pixel checks.
  No display configuration change or substituted WM_PRINT success is allowed.
- r015 actual native x86 window/descendant matrix passes, including attachment
  queries. The second MessageBox GUI thread times out at its sixth case; r024
  under concurrent GUI suites also exceeds the outer window deadline. Neither
  is a complete pass. Add per-case timestamps and run GUI suites serially;
  remaining fresh repeats are required. The observer's per-thread bound is60s.
- r023 actual shared WOW Off WINMINE / styled SOL error-dialog case passes.
  Reverse order fails because WINMINE creates its window with WS_MINIMIZE
  (style20CA0000), which the standalone core formerly rejected. Repair the
  general initial minimized/maximized attachment boundary, not an executable
  exception: retain native normal placement and skip resizing its transient
  minimized/maximized client. Focused state/restore assertions pass in r027+
  but those attempts fail test-desktop cleanup, not counted as full passes.
- The attachment probe now bounds USER auxiliary GUI lifetime with an owned
  child process and closes its private desktop only after child retirement.
  r031 passes both widths: initial minimized/maximized restoration, theme
  reentry and 30 attachment lifetimes (GDI3->3); child exit0 and private desktop
  closure also pass. Formal release is still the S7 package.

## Active-session repeat

The owner restored the local desktop session after r032 again showed an empty
visible DC clip, even with the owned test window on-screen. r033 reruns
`tests/observation/verify-win16-style.ps1` with the active console session and
passes both widths, including live list/combo/edit scroll pixels, native state,
27 gallery cases and 80 attachment cycles (GDI54->54). The final fixture reports
27261 checks. No desktop resolution, global theme or security setting changed.
This resolves the prior pixel-test prerequisite, not by weakening assertions.

r034 runs the updated actual Hook fixture serially; it adds installed-DLL
assertions and real control/scroll/theme calls. r035 stages the updated thirteen
images from the retained zip baseline and named fixtures. r036 passes launch
option, RPC style lifetime and frontend/launch link-boundary regression gates.
Actual native/WOW and full package gates remain in progress, not yet delivered.

r034 subsequently passes both widths: Off/Win16/All, A/W windows and dialogs,
six MessageBox APIs, later GUI threads, cross-width descendants, actual
attachment, owner-draw preservation, theme override and wide scroll ranges.
r037 passes real shared WOW in both policy orders, separate Off/Win16/All and
direct Win16 child inheritance. In particular the initially minimized WINMINE
now receives the enabled style without changing its Off SOL peer. r038 passes
both-width ABI/wrong-width/thread/unload checks and all27 shipped demo pages.

r039 rebuilds the remaining component closure; r040 is the new coherent
thirteen-image package. r041 Product passes all selected gates with identical
ten core runtime hashes. Its following Control invocation incorrectly passed
the packaged NTSRV path where the version-negative builder also expects its
generated service.h and static libraries; compilation failed before a negative
runtime assertion. r046 repeats Control with the actual NTSRV build directory,
first asserting its binary hash equals r040. No product-source change or
assertion waiver is made for this invocation error.

r046 passes the version negatives and earlier Control gates, then fails
session-isolation: acknowledged selected close ends NTVWM with1223 but its CMD
remains alive at the existing5s deadline. Preserve the exact pinned participant
record in r046/control-gates/isolation.txt.close-state.txt. r051 compares the
old S7 package, the S8 package and S8 with only the S7 Hooks restored; all three
pass the unchanged isolated two-session close test. This does not establish a
root cause or erase r046. Full old/new Control repeats are r052/r053; publication
remains gated while that result is unresolved. No timeout extension or close
implementation change is made.

r049 additionally passes actual injected-Hook fail-closed creation for missing
DLL (126) and wrong-width DLL (193), both x86 and x64, using the source-owned
style_loader_failure_test.cpp and verify-style-loader-failures.ps1. The wrong
or absent payload exists only in fresh build-owned negative copies.

The full old S7 r052 and new S8 r053 Control repeats both pass, including
selected close, independent-session survival, frontend loss and nested handoff.
The isolated and full repeats do not reproduce r046, so its root cause remains
unproved; no claim is made that S8 repaired Console shutdown. Its original
failure is retained alongside the passing repeats, without changing the bound.

Sequential review then adds a stronger DLL-unload check. r055 reproduces
exitC000041D after explicit removal and FreeLibrary followed by destruction of
still-live controls. Inactive single-line EDIT theme-capture subclasses were
left behind, including the internal edit of a combo. The minimal correction
captures EDIT theme state only where the multiline presentation needs it,
removes unowned capture on detach and unsupported combos, and records the
themed HWND before enable so failure rollback can release it. r056 and r068
pass both widths, including an unsupported owner-draw combo left native,
unload before window destruction and GDI3->3. No renderer or guest redesign.

r042 passes all18 actual shared/separate/relay variants (both native widths,
A/W separate creation and three policies), now with trusted style-DLL path
witnesses. r043's optional capture blocks when repainting WINMINE again after
the SOL modal starts; its180s observer deadline fails and is not a product
pass. The probe now restores and captures each owned window once, before
starting the next task; it does not repaint a modal-blocked peer a second time.

r059 stages the repaired final candidate; native-r058 retains the full GUI
API matrix. The separate CUI/DOS chain fixture now uses chain-child/leaf for
policy, real attachment, dynamic caption, parent retention, child return and
completion witnesses in both directions. Full A/W/modeless/modal/MessageBox
and later-thread checks still run in the native gate; they are not repeatedly
duplicated for each transport route. Final UI/WOW/package repeats are pending.

Build-only attempt notes: r010 /WX rejected static dllimport pointer identity;
use ordinary default leaf functions. r012/r013 used an incompatible Windows
PowerShell module environment; use the current pwsh runtime. r014 supplied an
absolute path to a generator expecting a repo-relative Hook root; corrected.
r021 stages a candidate from the retained assets/winnt.zip baseline plus only
named guest fixtures, not a copy of the owner's installed product. r022 failed
before launch because package/tests was absent; r023 includes that directory.

## Final candidate review and verification

Single-person reviewer role inspects the actual production diff after the
implementation runs. Only NTW16S owns rendering and HWND lifetime; native
Hooks and WOW32 own creation entry/exit and policy. WOW never receives nthook.
Off scopes mask reentrant enabled WOW tasks; dynamic native children and later
threads use the same process policy. Window-local scroll selection preserves
native state and LastError. ABI2 and exact sibling DLL paths are checked before
binding; missing/wrong-width libraries fail closed. Ownerdraw/custom content
is not claimed to be restyled. No guest or original mirror diff is added.

Final immutable candidate is r059/package/system32, not a later mutable cache
relink. native-r058 passes both widths' full API and attachment matrix.
boundary-r061 passes both DLL widths and thin demo checks. wow-r062 passes18
real relay/separate cases; wow-r063 passes five shared/direct policy cases.
r064 passes CUI launcher, DOS leaf and DOS launcher chains. negative-r067
passes missing/wrong-width DLL rejection in both widths. r065 Product passes
all3 gates in158175ms; r066 Control passes every selected gate in211619ms.
final-r059.exit is0. Each package gate records exact ten-core-image hashes.

standalone-r060 fails its existing1s outer-menu keyboard cleanup bound.
Identical standalone-r069 repeats pass both widths and27261 checks with
GDI54->54. Neither the assertion nor deadline was relaxed; the intermittent
failure is retained, not described as a repaired renderer bug. The earlier
r046 close failure also remains visible alongside old/new passing comparisons
and the final r066 session-isolation/retirement passes.

The optional r063 PrintWindow probe returned TRUE but wrote all-zero pixels.
Its BMPs are explicitly not visual evidence. The probe now rejects an empty
capture and joins the created private desktop before acquisition. r074 obtains
the real WINMINE frame/menu; r076 passes all five shared/direct cases with
nonblank captures. This corrects test desktop affinity, not product rendering.
PrintWindow is not a full physical-screen/gameplay fidelity proof; S9 integrated
visual/interaction acceptance remains unperformed. attachment-r073 additionally
passes both widths against the exact r059 DLLs: remove, unload, then destroy
still-live native HWNDs; unsupported ownerdraw remains native;30 lifetimes GDI3/3.

An ignored root command_stream_observer.obj predates this work (2026-10-10).
It is moved recoverably to build/M0-T445/S8/root-artifact-retained; SHA256
36C6E338C126CD8C20D67E6899EF05459C361E6AD16E6FB7F11080E43215EEBB.
The responsible Build-T420S34CommandStreamObserver.ps1 now explicitly sets
/Fo under its build output; observer-r071 builds successfully without root OBJ.

## Publication and acceptance boundary

r075 backs up the previous thirteen images and release manifest under build,
then publishes the immutable r059 candidate to assets/release and the configured
O:/winnt/system32. All13 candidate/release/deployed hashes match. The deployed
DLL boundary gate passes both widths, opposite-width rejection and the shipped
W16Style27-page demo's viewport movement, close and process exit. Core package
Product/Control identities and boundary-r061 identities cover all13 inputs.
assets/release/release-manifest.json is the authoritative identity list.

r077 passes documentation governance (including relative links), diff whitespace,
no mirror/root-OBJ checks and all13 three-way hashes. Final hidden runner ledger
(2026-10-13 local log-open time; waits include prior serialized runs):

| Run | Time / PID / outer deadline | Exit |
| --- | --- | --- |
| Final package r072 | 19:06:11 /33848 /3600s | 0 |
| Exact DLL lifecycle r073 | 19:09:03 /11740 /3900s | 0 |
| Same-desktop capture r074 | 19:14:20 /37168 /3900s | 0 |
| Five capture cases r076 | 19:17:41 /44424 /600s | 0 |
| Publication r075 | 19:18:38 /6512 /600s | 0 |
| Governance r077 | 19:20:22 /13744 /180s | 0 |

The P commit carries this evidence and exact published release manifest;
owner manual acceptance is pending. S9 is not admitted by this delivery.
SOL/WRITE initialization errors remain non-usability outcomes; custom/ownerdraw
client content and physical integrated interaction are not blanket guarantees.
No new source import, guest modification or original mirror diff is included.
