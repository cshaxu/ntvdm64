# T445 S1 — original Windows 3.1 appearance research

## Question and scope

Historical research checkpoint: subsequent owner approval selects
src/win16-style, and the [S1 closure design](m0-t445-s1-installation-design.md)
resolves installation/policy choices and assigns implementation proofs to
later stages. Open questions/status below describe this earlier checkpoint,
not a competing current packet.

The owner requires Windows 3.1, explicitly not Windows 95 or a modern classic
theme. This is source/document/image research, not implementation or a runtime
appearance pass. Existing NTCON, guest desktop and application client pixels
are outside the appearance layer. No original code or resource is imported.

## Reference hierarchy and procedure

1. Owner-supplied images define the selected visual reference, not universal
   default colours or authoritative unscaled pixel measurements.
2. Microsoft's original April 1992 Windows 3.1 User's Guide, PC21669-0492,
   supplies original behaviour and structural diagrams. Research copy:
   build/M0-T445/S1/r002/win31-users-guide.pdf, SHA256
   9A45622467B80C6973ABD9D8582E86F276B65F83BBD62052FA5372629D729B56.
   [Original manual scan](https://bitsavers.trailing-edge.com/pdf/microsoft/windows_3.1/Microsoft_Windows_3.1/PC21669-0492_Windows_3.1_Users_Guide_Apr92.pdf).
   Downloaded read-only research material into build after sandbox DNS failed;
   not redistributed in assets or committed as product input.
3. Contemporary Microsoft API documentation constrains modern integration,
   not historical appearance. OpenNT NT4 USER implementation is not a Windows
   3.1 pixel authority and is not admitted as a new runtime dependency.

Used bundled pypdf to inspect printed pages 8–10, 21–23 and 27–29 (PDF pages
35–37, 48–50, 54–56). Rendered PDF pages 35–37 with bundled pdftoppm,
`-f 35 -l 37 -scale-to 1400 -png`; visually reviewed the labelled page 35.
Web extraction of the large manual failed; local extraction/render succeeded.
PyMuPDF was unavailable; no dependency was installed. The Getting Started
manual PC21670-0492 additionally describes closing via the control box on
printed page 36. Manuals are evidence, not traced original retail USER source.

## Owner image inventory

Images remain owner inputs; their C: sources were read only. Research copies
belong under the above build directory, not production resources.

| Image | Content / meaning |
| --- | --- |
| codex-clipboard-871adc67-ae3b-469a-8c0b-fd697ee63fb9.png | Program Manager with Main child menu; centered pale-blue titles, left control box, down/up buttons, black/grey border. SHA256 5C7BF9662E4DD0D46620BEA9C960955553F30D19347EB3CFBB6D7AFB988626C1. |
| codex-clipboard-8b5dbf6e-595a-48c2-97ad-ac58b0b075b6.png | Control Panel has minimize only; no maximum-button slot. SHA256 B84D0E6D6C1BB6F602FDB71A3EE0ED982D88EF43BC7C38105A8E135000794F02. |
| codex-clipboard-c576f52b-032e-42e4-a19b-7829be582fd8.png | Color dialog has neither right button; active and inactive titles and basic/custom colour choices. |
| codex-clipboard-2e3017da-a792-4f56-bb40-99f221d35367.png | Same dialog with control menu: Move and Close Alt+F4. |

## Confirmed appearance and behaviour

| Element / state | Required behaviour | Evidence / caution |
| --- | --- | --- |
| Standard caption | Centered title; left horizontal-bar control-menu box; right minimize down-arrow and maximize up-arrow when supported. | Owner images and manual pp.8–9; no Win95 upper-right X, app-icon replacement or left-aligned caption. |
| Maximized | Restore uses up/down arrows, not overlapping Win95 squares. | Manual p.9, pp.27–28. Need original unscaled glyph reference before freezing pixels. |
| Minimize only | Single rightmost minimize button, no empty/disabled maximize slot. | Control Panel image. Do not infer absence from a disabled system-menu command alone. |
| Dialog | No min/max slots; retain control box where supplied; centered title. This supplied dialog offers Move/Close. | Color images. Do not force a control box onto every dialog or hard-code all dialogs to this menu. |
| Active / inactive | Distinct semantic colour roles; supplied active is pale blue and inactive is white. | Owner Color preview; manual p.8. Both text contrast and border roles need separate state. |
| System menu | Use the actual window's menu/state; retain enabled/disabled/default items and application additions. | Manual pp.22–23. Top-level and document/MDI menus differ. |
| MDI/document child | Main's Close Ctrl+F4 and Next Ctrl+F6 are child semantics; maximized document merges into outer title/menu presentation. | Owner Main menu and manual pp.10,27–28. Not every WS_CHILD is an MDI window. |
| Move / size | Title drag and resize edges/corners; original move gives an outline and supports ESC cancellation. | Manual p.23. Modern host full-window-drag policy differs; do not change a global setting to imitate it. |
| Close / keyboard | Control-box double-click closes; top-level Alt+Space menu and Alt+F4; child Alt+minus menu and child close/next shortcuts. | Original manuals. Deliver commands to the application; do not terminate its process from the appearance layer. |
| Border and menu | Square corners, thin dark separators with grey/light relief, ordinary underlined menu labels. | Owner images. Win3.1 does use relief shading; calling it entirely flat would also be wrong. |

Colour correction: the supplied basic/custom chooser is not a sixteen-option
list. Patterned swatches show display realization/dithering, not sixteen
fixed semantic UI slots. Keep configurable UI colour values separate from
display-depth realization and from NTCON's indexed text protocol. Do not
clamp GUI colour choices to that protocol or present arbitrary screenshot RGB
as the unique factory palette. Client controls in these images are reference
context, not authorization to repaint application clients.

## Current project ownership — static findings

- wow_window_creation_binding.c:67–151 uses a scoped same-thread CBT hook to
  attach WW/class/owner metadata before calling host CreateWindowExA/W, with
  bounded failure cleanup. This is a candidate direct installation boundary,
  not an existing appearance implementation.
- wow_dialog_creation_binding.c:73,201–231 has separate scoped CBT/return
  hooks and native dialog creation. Its WM_CREATE/W32InitDlg and WM_INITDIALOG
  ordering must survive; hooking only CreateWindowEx return is insufficient.
- wow_window_words_binding.c:337–358 dispatches to original WOW callbacks and
  releases metadata on WM_NCDESTROY. Do not replace it with DefWindowProc or
  swallow arbitrary guest non-client notifications.
- ntvwm-exe/execution.c:103–114 installs matching nthook before target resume.
  nthook32-dll/create_process.cpp:269–277 hooks CreateProcessA/W only.
  nthook32-dll/entry.cpp:21–32 runs attach in DllMain and disables thread
  notifications. Process-hook propagation does not prove window/dialog or
  later-GUI-thread coverage.

The proposed common renderer is compiled for x86/x64 with explicit per-window
state. WOW calls it at its existing project-owned boundary; native processes
reach it through matching Hook installation. It owns neither a task registry
nor a lifecycle policy. Exact specialist source placement remains a design
decision; no new generic common owner is authorized by this research.

Modern API constraints:
[SetWindowSubclass](https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-setwindowsubclass)
cannot install across threads; it is a candidate, not a proved solution for
all application WNDPROC replacement cases.
[DllMain](https://learn.microsoft.com/en-us/windows/win32/Dlls/dllmain)
must not become the place for USER/GDI initialization or window operations.
[DWM custom frames](https://learn.microsoft.com/en-us/windows/win32/dwm/customframe)
require explicit geometry and hit testing. Its example expanding client area
over the whole window cannot be copied blindly: existing client coordinates,
contents and input must remain correct.

## What must be measured or proved next

1. Establish an unscaled VGA reference: caption/button/menu/frame metrics,
   original glyphs and font metrics, active/inactive/pressed/disabled states.
   Do not read pixel dimensions off resized screenshots. No pixel-exact claim
   is made yet, and no synthetic visual mockup has been accepted.
2. O:/W31 currently selects VGA.DRV and VGASYS.FON in [boot], but its descriptive
   label still says Video 7 256 colours. Labels alone are not runtime evidence.
   WIN.INI has BorderWidth=3; it is one installation's setting, not a universal
   fixed frame width. VGASYS.FON SHA256 is
   7BAEFA14EF8EF2355374D0B5061043DD5F4F8C33FF57680366E4A725F91646F8.
   No runtime was started or profile modified. Font resource reuse/import needs
   a separate provenance/selection decision, not an assumed modern font alias.
3. Prove same-thread creation coverage for A/W windows, modal/modeless dialogs,
   later GUI threads and creation failure, without process-wide window polling.
4. Prove subclass ordering, title/state invalidation, WM_NCDESTROY cleanup and
   application procedure replacement; custom frames must retain native fallback
   unless support is established, without executable-name exceptions.
5. Preserve client dimensions while selecting original non-client metrics.
   Test initial sizing, min/max constraints, menu wrapping and MDI maximization;
   a cosmetic WM_NCPAINT hook alone does not establish this contract.
6. Original minimized desktop icons and original outline dragging belong partly
   to desktop/window-manager policy. Current proposal preserves modern native
   system commands; do not recreate Program Manager or change host shell/global
   settings. Report these distinctions before claiming complete original UX.
7. High DPI/RDP and high contrast require a declared scaling/accessibility
   policy. No modern DPI setting is a historical pixel measurement.

S1 remains active. This delivery records research and concrete design risks,
not S1 completion or permission to proceed to S2/S3. No production, mirror,
guest, release image or live process changed. Governance/link/diff verification
is retained under build/M0-T445/S1/r003.
