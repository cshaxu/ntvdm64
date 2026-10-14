# T445 S8 NTW16S integration design draft

## Owner approval

Owner approves implementation after explicitly confirming WOW32.DLL as the
Win16 attachment owner. It loads/calls ntw16s32.dll inside the existing x86
ntvdm.exe process. The executable is the carrier, not a new appearance owner;
Windows WOW64 is not a product integration component. Native integration remains
nthook32/64 calling ntw16s32/64. The design-only delivery below is historical;
Current Work now admits implementation with full product delivery gates.

## Admission and evidence

Owner requests S8 design before implementation and advances without S7 manual
acceptance. Baseline is delivered/pushed 292664b02, not a new binary candidate.
This document retains the source-backed design; [implementation evidence](../evidence/m0-t445-s8-style-integration.md)
records S8 integration tests and publication. Current Work owns the sole active packet. S9 remains final integrated
acceptance, not a place to defer known S8 installation defects.

Read-only procedure: inspect NTW16S public headers, native window_titles.cpp,
WOW wow_title_policy.c and creation bindings, W16Style gallery preparation,
and S1/S5/scrollbar evidence. Findings:

- Native creation interception already covers A/W windows, dialogs and message
  boxes, later GUI threads and nested calls. It only installs title markers
  on captioned windows when process policy is All.
- WOW resolves task policy, not process policy, and loads the x86 marker DLL.
- NTW16S already owns frame/menu/control/font/scroll rendering and lifetime.
  Its public APIs require the GUI thread and retained module reference.
- Demo explicitly prepares list/edit themes, supplies original EDIT border
  and tab state, and uses scroll API bindings. A title-install substitution
  alone cannot reproduce these capabilities in ordinary applications.

Confidence is high for these source facts; transparent automatic coverage and
creation ordering remain implementation proofs. Reuse the admitted S1
historical-source disposition: this is owner-requested host presentation, not
recovery of an original OpenNT style switch. Reuse the delivered core, do not
import USER/GDI servers, change guest media or add mirror diffs.

## Owners and policy

| Owner | S8 responsibility |
| --- | --- |
| src/ntw16s-dll | Sole rendering owner; add the smallest automatic attachment coordinator around existing frame/control/marker APIs. Own local HWND state, fonts, theme handling and cleanup. No CLI, broker or task classifier. |
| src/nthook32-dll (both widths) | Retain launch hooks and policy; adapt existing window creation scopes to the DLL. Add finite USER API interception where subclassing cannot satisfy scroll/theme contracts. No copied painter. |
| src/wow32-dll | Supply exact owning task policy at existing host window/dialog boundaries; bind affected USER APIs at project-owned facades. No nthook injection into the WOW worker. |
| src/w16style-exe | Remain thin DLL demonstration. Tests, not demo-specific production branches, prove automatic installation. |
| Run16 / NTSRV / NTVWM / NTCON | Retain S7 responsibilities; no new appearance RPC, font state or renderer dependency. |

| Existing option | Native application HWNDs | Win16 application HWNDs |
| --- | --- | --- |
| None / Off | Unchanged | Unchanged |
| --win16-style | Unchanged; descendants still inherit policy | NTW16S32 |
| --win16-style-all | NTW16S32 or NTW16S64 by actual process width | NTW16S32 |

Preserve the existing (Win16 Style) marker as the enabled witness; do not
infer enablement from title text. No new public parameters. Console surfaces
and an entire Windows 3.1 DOS desktop remain outside this HWND feature.

Load only the corresponding DLL from the trusted product System32 location:
reuse established product-root/Hook-path identity, never CWD/PATH search or
hard-coded O:/winnt. Validate Ntw16sGetAbiVersion and required exports before
attachment. Version a changed local ABI coherently; do not create a second
transport ABI. Retain the module for process lifetime while subclasses exist.
No loading/drawing/USER initialization from DllMain.

## Installation and lifetime

Proposed new code lives within NTW16S (for example attachment.[ch]); exact
export spelling follows implementation review. Prefer a small begin/end
creation-scope API receiving explicit enablement, plus per-HWND detach/query
as actually needed. Reuse existing install APIs internally. The scope has
DLL-owned allocation and same-thread lifetime; no general plugin registry.

1. Caller resolves enablement before creation. Native uses copied process
   policy; WOW uses the exact bound task. Capture this value for the request,
   including explicitly Off; never consult the last foreground task.
2. Enter a bounded nested creation scope before USER creates the window or
   dialog children. Capture original creation styles, including EDIT border,
   before USER consumes them. Preserve WOW metadata/guest callback ordering.
3. Separate early metadata capture from final attachment. Record root policy
   before nested child creation; install each supported window only when its
   native initialization permits it. Handle dialog children before the modal
   loop, not after DialogBox returns. Do not merely enumerate late and guess
   lost creation attributes. Failed creation unwinds captured state.
4. Later child creation enters a new scope and consults explicit request/owning
   task or an already attached parent. An explicit Off task cannot inherit
   appearance accidentally from an unrelated owner HWND. Combo-owned internal
   EDIT/list windows attach through the combo owner once ready, not twice.
5. Destruction releases HWND-local resources after outstanding callbacks;
   remove restores only component-owned changes. No polling, cross-thread
   subclassing, global desktop enumeration or perpetual repaint timer.

The existing native caption filter is insufficient: controls and captionless
frames must reach the coordinator. It classifies supported *window/control
forms*, not executable names. Handle owned popups, modal/modeless dialogs,
MDI frame/client/child and dynamic controls explicitly. One HWND attachment
must remain idempotent despite nested A/W/dialog scopes.

Operational attachment failures retain explicit errors and bounded rollback;
missing DLL/ABI or allocation failures cannot silently claim success. Unsupported
custom-drawn surfaces are a distinct reported native-preservation outcome,
not a reason to destroy the application. Review against current S7 creation
failure behavior before changing that contract. Never leave a half-installed
subclass or unload a DLL under a callback.

## Required integration gaps

### Scroll APIs and actual application behavior

Audit Get/SetScrollInfo, Get/SetScrollPos, Get/SetScrollRange, ShowScrollBar,
EnableScrollBar and SB_CTL message forms together. Reuse existing DLL tracking
and native range/page/position state; native API interception belongs to Hook,
WOW binding to its host facades. Unstyled windows call the original unchanged.
Styled calls enter the DLL's finite binding; its native calls must bypass the
interceptor without recursion. No blanket global macro or second scroll model.
Exercise ranges above 65535, live tracking, disabled bars, return values and
notifications. The application still moves its own content in response to
scroll notifications; the style DLL does not implement application scrolling.

### Themes, menus and controls

Move reusable per-window preparation out of demo-only practice into the DLL's
automatic attachment contract. No global theme/animation change. A prior
arbitrary theme association is not always recoverable by inspection; capture
supported set-time changes from creation, or report late attachment unsupported
rather than invent a previous state. Test app SetWindowTheme/WM_THEMECHANGED
against detach and restore. Theme interception remains narrow and opt-in.

Preserve HMENU command IDs, item data, enable/check state, accelerators and
tracking. Cover system menus, ordinary dynamic menus, MDI merged menus and
combo popup lists. Do not overwrite application owner-draw/bitmap/custom
controls with generic string rendering. Those forms need explicit capability
classification, not a universal-retro claim.

Known-state tabbed lists must retain LB_SETTABSTOPS conversions; unknown
pre-attachment tabs remain unsupported. Native edit selection/caret/password,
IME and control notifications remain native functionality, not a new editor.

### Font and layout

Retain the approved System bitmap appearance scaled to24 pixels for title,
menus and supported controls. Preserve font ownership and native hit-test,
caret and selection metrics. Arbitrary applications may use fixed control
rectangles too small for24 pixels: demo layout changes are not an automatic
application-layout algorithm. Verify clipping separately. Do not silently
shrink the font, relayout arbitrary apps or claim full compatibility. A proved
layout conflict is reported for owner disposition before expanding scope.

## Implementation order after approval

1. Add/reuse the DLL automatic attachment closure with explicit creation
   metadata, failure and teardown tests, keeping existing direct demo API valid.
2. Bind both native Hook widths; test fixtures that do not call any NTW16S API.
3. Bind WOW task-owned windows/dialogs and affected USER facades; verify shared
   Off/enabled coexistence and independent workers. Do not alter S7 propagation.
4. Finish scroll/theme/dynamic-control boundary matrix, retain original UI
   behavior and run coherent package regression before publication.

Each production P requires the existing package gate; these are work steps
within S8, not permission for partial untested publication or new S admission.

## Verification and delivery gates

- Both native widths and A/W, later GUI threads, nested creation, failing
  WM_NCCREATE/WM_CREATE, modal/modeless/MessageBox, dynamic children and MDI.
- Off/Win16/All plus S7 cross-width and DOS/native/Win16 child chains; shared
  mixed-policy WOW in both launch orders and separate WOW identity.
- Actual DLL path/width/version, real attachment state and pixel/input evidence;
  title marker alone is insufficient. Deliberately missing/wrong DLL, foreign
  thread and failure/reentry/lifetime tests; stable GDI resources after teardown.
- Retain S4/S5 menus,24px font, no native-hover leakage, double-click system-box
  close, no black backgrounds/flicker and real scrolling tests. Prefer owned
  private-desktop probes with clear physical-screen limitations.
- WINMINE retains gameplay frontier. SOL and WRITE each retain their recorded
  window/error frontiers; their existing initialization failures are not
  usable-main-window passes and are not to be hidden by styling.
- S8 delivery builds the affected closure in background, passes required DOS/
  product tests, publishes the coherent thirteen-image set to assets/release
  and configured System32, verifies hashes, commits and pushes before inviting
  acceptance. S9 remains final integrated visual/interaction acceptance.

This design-only delivery changes no release binary or deployment. Governance
and diff review are its checks; implementation results will be recorded separately.

Design review maps the owner request to the current packet and the two existing
installation entries. Initial documentation governance runs in background at
build/M0-T445/S8/r001, PID10336, start2026-10-13T17:04:15-07:00,
deadline120s, exit0; git diff --check passes. This is documentation evidence,
not a runtime or manual acceptance result.
