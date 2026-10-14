# T445 S10 default-button fidelity

## Admission and baseline

Owner reopens T445: "重开T445，准入新S修复default button" and authorizes
implementation. Baseline4f8c181487cc69e27f0ca7cb1c1c17ee5872ffa0 was clean,
pushed and published; S9 r045 retains the coherent13-image identities.
Single-person implementation followed by a separate sequential review.
No guest, adopted mirror, hook policy, public ABI or20px typography change.

## Original owner and root cause

The screenshot's OK identity is confirmed independently: owner-supplied
O:/w31/SYSTEM/SHELL.DLL RT_DIALOG100 About %s has BUTTON ID1, text OK,
style0x50010001 (BS_DEFPUSHBUTTON). Input SHA256:
667422428E0934D947C9B56D622CFFE6ACD576F8014CA3692D7639D5457044D6.
`inspect-win31-dialogs.ps1 -Image O:/w31/SYSTEM/SHELL.DLL -OutputRoot <fresh>`
produces the resource/style record; S10 shell-dialogs/exit.txt is0. This is
a standard Win16 NE-dialog decoder, not a general malformed-image validator;
display strings use ASCII and do not certify original localized text.

USER.EXE SHA256
98036967F8396B387DA431699A9D3BE95EB56FCD0118F529E1FC2F25105EFAC6,
segment25:18BC and1:8E0C..90A8 establish the drawing contract. Retail VGA.DRV
SHA25653312ED013BDA513C74E33659F4EE7D1B4F2B1CF1A06AED672432A8FBC4A820A
is unchanged; USER owns these borders and button state, GDI/driver executes
the primitives. Existing NE segment inspector and retail-1 through retail-43
record disassembly plus relocation chains. Raw far-call selector words are
unresolved; relocation metadata identifies their targets. D1 shift-by-one
bytes are interpreted as one despite ndisasm displaying0 in this mode.

The project previously drew a separate outer frame, inset two units, then
the ordinary frame: a visible gap rather than the retail continuous thick
border. It also reversed the raised bevel on press and outlined the whole
face for keyboard focus. The correction uses enabled-default two-unit versus
ordinary one-unit continuous black frame, corner cutouts, retail raised versus
pressed relief, and focus around the measured label. Disabled default loses
the extra emphasis; native BM_SETSTYLE/DM_SETDEFID remain authoritative.

Source-first ledger: original Win3.1 binary research establishes appearance;
OpenNT windows/core/ntuser/client/btnctl.c establishes button/dialog ownership
but its NT4 DrawEdge is not the visual reference. Direct source composition
and a same-shaped shim stop at private PWND/BUTTON/USER services (the previously
registered USER boundary); no usable standalone original drawing unit exists
at the public HWND/HDC boundary. External intrusion is unnecessary. Repair
the already admitted authored painter, keeping native behavior/state and the
current palette/DPI adaptation. No Microsoft executable code is imported.

## Verification entrypoints and failures retained

All runs are hidden owned background processes under build/M0-T445/S10,
with per-case timeout/PID records. No computer use or physical-pointer claim.

- verify-default-button.ps1: both real DLL widths, pixel assertions for
  ordinary/default/pressed/disabled frames, measured focus boundary,
  BM_SETSTYLE and DM_SETDEFID transitions, native IsDialogMessage Enter
  activation, remove/reinstall and DLL unload. Previous release is a negative:
  it must fail the continuous-black-border assertion, not just exit nonzero.
- verify-win16-style.ps1: retained geometry, menus, controls, scrolling,
  font, paint ownership, injected-leak negative and DLL/demo lifecycle.
- verify-style-attachment.ps1 and verify-ntw16s-boundary.ps1: deployed-shape
  dual-width API, resource, demo and attachment boundary.
- verify-wow-style-chains.ps1 ui-off/ui/ui-all: real WINMINE About ownership
  and unchanged game after closing its dialog; not a physical mouse test.
- verify-wow-mine-gameplay.ps1 Off/Win16/All and Invoke-ProductVerification.ps1
  Product/Control: retained ordinary DOS routes, native execution, WINMINE
  gameplay and independent SOL/WRITE frontiers. Known application failures
  remain failures, not improved by this button rendering change.

Early admission format errors and one PowerShell5 module-path failure are
retained; corrected governance and pwsh research pass. r001/r002/r004 first
used unstable aggregate GDI baselines around font recreation. r003/r007
full UI runs failed the existing x64 aggregate no-growth assertion, although
the exact painter-owned allocation balance was zero. Independent old/new
presentation probes each pass three consecutive runs. The separate named
font-recreation diagnostic reports2/4 for both original and corrected DLLs
in both widths; it explicitly does not count as a default-button pass.
Warm the complete reinstall closure before measuring the focused test;
retain exact no-growth and the independent allocation/leak tests. r006 was
denied isolated-desktop creation (error5); rerun outside the restricted
sandbox, not counted as a product failure or a pass.

## Final focused results

r018 passes both actual DLL widths with all default/state/keyboard assertions
and steady-state reinstall GDI16/16. Its negative against the S9 release fails
exactly `GetPixel(dc,x,25)==RGB(0,0,0)` in both widths. Immutable S9 r034/package
can supply the same negative on reproduction; per-run owner manifests pin
the actual input hashes, rather than relying on the later mutable release path.

r019 passes the complete unchanged UI suite:27,261 checks per width, attachment
GDI54/54, controls, menu, scrolling, font, injected-leak detection and actual
DLL/demo boundaries. x86/capture.bmp-gallery-1.png records the corrected
ordinary/default side-by-side appearance. These are probe captures, not a
claim of original-font/DPI whole-window pixel equality.

r009 attachment expired its55s child budget while another UI suite was active;
r020 runs serially and passes both widths, including30 lifetimes, GDI3/3,
live-window removal and unload. r010 DLL/demo boundaries pass. r011 refuses
an existing unowned O:/winnt broker; under standing owner authority the exact
deployed ntsrv/ntvwm processes were closed. r021 then passes real WINMINE
About Off/Win16/All. No timeout/protection refusal is relabelled as a pass.

Sequential review: only controls.c changes product behavior, using existing
palette/frame primitives and native state. No added allocation, global hook,
new API, guest mutation or adopted-code diff. Disabled/default emphasis,
pressed relief and label focus are the adjacent same-owner corrections, not
a new control state machine. Existing propagation/native-API integration
code and the ten core runtime images are unchanged; their prior exhaustive
propagation evidence is retained, with new attachment, WOW and runtime gates
covering the changed DLLs.

## Runtime, publication and disposition

r012-Off/Win16/All each pass actual WINMINE flag/counter, F2 reset, opening a
cell, WM_CLOSE and Run16 completion. r013 Product passes all3 gate groups in
161,101ms (independent WOW frontiers plus Console17/Window17 routes). r014
Control passes all11 groups in214,095ms: RPC, native GUI, version negatives,
strict DIR, EDIT return, cooked/rapid/interactive return, isolation, retirement
and nested-window I/O handoff. product.exit is0. These retain the known
SOL/WRITE failures explicitly; no claim of new application functionality.

r015 backs up the previous13 images at both destinations and release manifest,
publishes the exact tested candidate, verifies SHA256 equality across tested
package/assets/release/O:/winnt/system32 and passes deployed both-width DLL
and demo checks. exit.txt is0; verified-manifest.json records all13 hashes.
Only the two style DLLs and rebuilt W16Style.exe change; all ten core images
remain baseline-identical. No guest or ordinary configuration changed.
Recoverable prior files remain in r015/backups.json and its listed backups.

Documentation governance and git diff checks pass; the delivery commit includes
source, reproducible fixtures, evidence and the coherent release manifest.
S10 P1 is ready for final owner acceptance after commit/push. T445 remains
reopened at S10; this does not claim manual acceptance or retire the existing
owner-deferred native-menu animation TODO. Queue order is unchanged.
