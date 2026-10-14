# T445 S11 dialog and control coverage

## Admission and boundary

Owner approves full coverage after the native About screenshot shows a grey
body, white text islands and native license typography. Baseline is delivered
e046ee3d783cfee567b87a73c2ce767df403aa18. Ordinary Mode: implementation then a
separate sequential review. S10 manual acceptance is not inferred.

The actual x86 WINMINE About inventory (S11 r001) contains ten STATICs,
one SysLink and one BUTTON. The STATIC set includes a one-pixel etched
horizontal separator, an icon, text and a hidden label. The Windows11 brand
banner is painted by the host dialog, not a text control. Keep that genuine
OS content and all application artwork. There is no executable/title/ID
special case and no guest, mirror, consumer, ABI or global-settings change.

## Ownership and source-first ledger

WOW still calls native ShellAboutA in its existing explicit creation scope.
The dialog supplies its native background while the previous authored STATIC
painter supplied white, explaining the islands. SysLink was absent from both
control attachment paths. A native etched STATIC has WS_EX_STATICEDGE and
can have zero client height: painting only WM_PAINT/WM_PRINTCLIENT does not
own its visible line. A separate sweep found multiline EDIT attachment was
rolled back when no scrollbar was present, despite valid native edit state.

Original USER dialog/STATIC ownership is retained at the public window-message
boundary, as already admitted in S1/S4/S10. Direct source reuse and a
same-shaped shim stop at the registered private USER server boundary; no
standalone original HWND implementation can be linked here. SysLink is a
later native common control, not a Win3.1 control to reimplement. External
intrusion is unnecessary. The smallest authored adaptation supplies palette
and font, keeps native link markup/hit testing/notifications and native dialog
layout/commands, and extends the existing painter only for flat separators.

Public contracts checked:
[WM_CTLCOLORSTATIC](https://learn.microsoft.com/en-us/windows/win32/controls/wm-ctlcolorstatic),
[SysLink](https://learn.microsoft.com/en-us/windows/win32/controls/syslink-overview),
[SysLink styles](https://learn.microsoft.com/en-us/windows/win32/controls/syslink-control-styles).
No source or binary resource is imported. Original button work and provenance
remain in [S10](m0-t445-s10-default-button.md).

## Coverage inventory

| Surface | Presentation owner and behavior |
| --- | --- |
| Standard modal/modeless/owned #32770 dialog | Existing frame subclass supplies white background and text palette through native CTLCOLOR; application painting and branding remain application-owned. |
| BUTTON ordinary/default/group/check/three-state/radio and automatic forms | Existing native state and authored painter; S10 default/focus contract retained. |
| STATIC left/center/right/simple/no-wrap text | Existing20px font/painter; disabled text and wrapping retained. |
| STATIC rectangles/frames | Existing flat geometry; no font requirement for non-text shapes. |
| STATIC etched horizontal/vertical/frame | New flat-border mapping, complete client/nonclient geometry and print ownership; native style and outer rectangle retained and restored on detach. |
| STATIC icon/bitmap | Original handle/content retained, not replaced with fabricated Win3.x artwork. |
| EDIT single/multiline/password/read-only/disabled | Native input/selection/caret with existing font/border/scroll integration; multiline without scrollbars now attaches too. |
| LISTBOX, COMBOBOX and internal lists, SCROLLBAR | Retained S4/S9 renderers and interaction; full existing regression retained. |
| SysLink | Shared20px font, dialog palette; native markup, underline/link color, hit-testing, focus, NM_CLICK/NM_RETURN and latest-font restoration. Both explicit creation and demo control-scope paths include it. |
| Owner-drawn/custom controls and host artwork | Application ownership, deliberately not overwritten. Unsupported custom types are not silently claimed covered. Modern tree/list-view/ribbon/browser UI is not a Win3.x standard-control implementation. |

Existing unsupported modern style combinations (including owner-draw lists,
BUTTON icon/bitmap/pushlike and EDIT client-edge) retain their explicit
unsupported/native boundary; this S does not claim every arbitrary Win32 UI
can be generically repainted. All classes actually present in this About are
covered or explicitly identified as genuine application artwork.

## Verification

All runs use owned hidden background processes, explicit deadlines and
build/M0-T445/S11 evidence; no computer use or live-pointer claim.

- r001 records original About inventory and Off/on images.
- r004/r006/r007 expose overly broad initial font assertions: SCROLLBAR and
  non-text STATIC shapes do not return a font; corrected without changing
  their native semantics. r009 identifies multiline EDIT rollback.
- r010 candidate passes; its negative fixture used a relative DLL path and
  fails LoadLibrary, not the intended negative. Absolute-path repair is r011.
- r011 both widths pass dialog/link behavior and GDI45/45; old release fails
  the missing separator attachment assertion. r005 corrects the observer's
  DPI virtualization: PrintWindow must use a physical-size bitmap.
- r012/r014 add separator pixel assertions and expose zero client height.
  This is retained failure evidence, not a pass. Subsequent correction owns
  the line rectangle while preserving native outer geometry and style bits.

- r015 passes both-width dialog/control tests, link mouse/Enter notifications,
  Off and detach/font restoration, and repeat cleanup GDI46/46 (x86) and
  48/48 (x64). Old DLLs
  fail the expected missing separator attachment. r013 real WOW About passes
  Off/Win16/All with complete physical-size captures and the flat black line.
- r016 passes actual DLL boundary, opposite-width rejection, shared fonts,
  thread/ABI checks, and all28 shipped demo pages with normal close/exit.

- r017 full UI passes27,261 checks per width, GDI54/54, including existing
  menus, controls, scroll input/paint and fonts. r018 actual attachment tests
  pass30 lifetimes GDI3/3, removal/unload, theme reentry and nested Off.
- r019 retains all S10 default-button pixels, state/Enter and cleanup16/16.
- r022-Off/Win16/All real WINMINE gameplay passes flag/counter, reset, open
  cell, close and launcher completion.
- r020 Product passes3 gates in164,736ms; r021 Control passes11 gates in
  214,039ms. Existing SOL/WRITE initialization frontiers remain bounded
  baseline failures, not newly claimed application success.
- Sequential review adds custom non-dialog background preservation, late
  SysLink via the second control-scope path, and separator outer-rectangle /
  native style restoration. r023 fails fixture compilation on local variable
  shadowing. r025 exposes a fixture brush shared incorrectly with a registered
  class: UnregisterClass owns deletion of that brush. Use a stock brush for
  the negative custom-window class, independent of the dialog brush. The
  [documented class-brush lifetime](https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassa)
  confirms the fixture error; no product patch is needed for either failure.
- r026 runs the final fixtures against the frozen actual candidate DLLs,
  not newly linked substitutes. Both widths pass all positive assertions;
  old release fails the expected separator attachment. Repeat cleanup is
  GDI46/46 x86 and47/47 x64. Native combo height may legitimately change when
  its original font returns; geometry invariance is asserted for the new
  separator ownership, not used to override native combo behavior.

## Sequential review and delivery

Reviewed the owner request against the complete changed source and inventory:
palette changes are gated by standard dialog class, links retain native
behavior, no executable/title/ID predicate exists, no text or image contents
are rewritten, and detach restores native ownership/font. Retained unsupported
application-owned/modern surfaces above are not claimed as repaired. The20px
policy, current process scopes and public ABI are unchanged. All production
changes are in the authored style DLL and thin demo; original mirrors and
guest components have zero diff.

S11 r024 backs up both destinations and publishes only changed
ntw16s32.dll, ntw16s64.dll and W16Style.exe. Ten core-image identities remain
unchanged. It verifies all13 hashes across candidate, assets/release and
O:/winnt/system32, updates release-manifest.json, and passes the deployed
actual-DLL boundary plus all28 demo pages. Backups and verified-manifest.json
are retained there; publication exit is0. The runtime fixture came from the
previous sealed test package, not a bulk copy of the user's O:/winnt tree.

At automated delivery, no physical-pointer or user manual acceptance was
claimed. P1 was committed and pushed as
b39ceb7e6c230db29519f2fd1be60ad4e9c4fa42 with a clean tree.

## Owner acceptance and bounded closure

Owner subsequently confirms "验收通过" for this delivered version. S11 is
accepted and closed. This acknowledgment admits no next task and does not
retroactively change earlier stages' separate acceptance limitations.
The existing unstyled-menu animation TODO and queue order remain unchanged.
Acceptance recording is documentation-only: no source, binary, manifest or
runtime configuration changes, and no rebuild or redeployment is required.
