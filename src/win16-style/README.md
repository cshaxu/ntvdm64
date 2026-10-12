# Experimental Win3.1 window frame

Project-authored same-thread static library with owner-approved original VGA
button pixels embedded locally; see [resource provenance](resources.md).
The title uses the host System font by owner-approved visual exception;
no guest/mirror executable code is imported. S1's source ladder rejects
NTUSER server composition at the existing private server boundary; this is the
owner-admitted local presentation feature, not an NTUSER implementation.

win16_style_install/remove operate only on the calling GUI thread's HWND.
The window retains its application procedure, client drawing and Windows
system-command semantics. No worker identity, broker, Hook, polling or thread
is introduced. No production executable links this experiment.

layout.c owns geometry shared by paint/hit-testing. win16_style.c owns the
subclass, local GDI resources, palette, title painting and button gestures.
The public header is a process-local API, not a wire format.
Normal installation/removal preserve client origin and dimensions.
Maximization keeps native window management; while maximized, client sizing,
painting and hit testing share the visible monitor-work-area intersection so
the host's off-screen resize border cannot clip the custom title/buttons.
Destruction
removes the subclass; explicit removal is required before unloading code.
Repeated installation fails with ERROR_ALREADY_EXISTS; unsupported frame forms
fail with ERROR_NOT_SUPPORTED and remain native.

The S3 profile accepts top-level caption-button combinations, captionless
sizeable/thin frames, borderless windows and dialog-style windows.
WS_EX_DLGMODALFRAME explicitly selects the wider caption-colored
dialog border and light inner outline; no title or button-absence heuristic is
used. WS_DLGFRAME alone cannot distinguish this because WS_CAPTION includes it.
Scrollbars, client-edge/tool/layered windows and minimized/maximized
installation are deliberately unsupported. S4 admits string menu bars and
the standard MDI merged system/restore controls; arbitrary application
owner-drawn or bitmap menu bars are not supported.
S4 now supports normal child/MDI frames with parent-relative client preservation
and the original child system-box image. Maximized MDI children retain native
management while the parent presents the merged system/restore controls.
Consumers must not introduce unsupported forms while attached. System and
ordinary string popup menus use retro painting with native menu tracking.
System-menu tracking remains native, not a separate TrackPopupMenu loop.
If the host reports an empty native system-button rectangle (modern dialog
frames), our button enters the system menu through SC_KEYMENU/space after
release inside its rectangle, using existing capture/cancellation tracking.
While this fallback menu runs, a scoped thread-local WH_GETMESSAGE hook maps
double-clicks inside our system button back to native NC handling after the
loop exits. USER's menu capture uses screen coordinates and can consume the
click before WH_MSGFILTER. Disabled-close handling remains native; no delay,
global mouse hook or global setting change is used. The binding is restored
and the hook removed before returning, including window removal/destruction.
Normal non-client activation, cursor/hover and compatibility caption-draw
messages belong to the custom renderer; delegating their drawing can leak
native buttons into the shorter caption's client area. Client cursor handling,
WM_ACTIVATE/focus and system commands remain downstream. Minimized activation
and mouse presentation remain native. No extra repaint timer or larger frame
is used to cover native drawing.
This is not a creation/Hook adapter or real title-prefix implementation.

The supplied reference palette is a preset, not a universal Win3.1 default.
High contrast reads host colors; no global settings are written. Caption
System font is requested at a 16-pixel base size then scaled to window DPI.
No original font is embedded, privately registered, or installed. Exact face
and glyph realization belong to the host and are not original-pixel guarantees.
The owner-approved typography candidate keeps the earlier 20-row text canvas
and clips its scaled result into the corrected 18-row caption. This preserves
the earlier vertical scaling phase without enlarging buttons or the frame;
it is a visual exception, not exact original caption rasterization.
Extended/unsupported Unicode mapping remains outside the ASCII pixel proof.
The demo alone opts into per-monitor DPI awareness; the library never does.

Build/test (invoke from an owned background PowerShell process):

    tests/observation/verify-win16-style.ps1 -BuildRoot <repo>/build/<run>

This generates independent x86/x64 Ninja graphs, /MT /O2 /W4 /WX archives and
GUI-subsystem win16-style-demo.exe. Without arguments it opens a 19-entry
selector: double-click a row or choose Open selected. Close samples closes
only the demo's sample HWNDs. The first 14 entries exercise styled top-level
forms/states; entry 18 now exercises styled MDI frames with native MDI management.
Entry 15 exercises the retro menu bar and popups. Entries 16, 17 and 19
remain native scrollbar/child/ordinary references.
Minimized presentation is also still native. F2 changes title,
F3 changes caption colors and Esc closes a sample (including borderless ones).
--test <bitmap> runs on its own private desktop and captures only its HWNDs.
--menu-test <absolute-build-bitmap-path> uses the input desktop and only its
owned windows for real popup, selected-pixel and close-command checks.
Run it in an owned, bounded background process; private-desktop dispatch
checks deliberately do not claim real menu tracking.
The build also produces native-menu-probe.exe and native-menu-v6-probe.exe:
same standalone native source without this library, respectively without and
with a Common Controls v6 manifest. They isolate host menu drawing. No arguments
opens a manual comparison. `--probe <absolute-build-bitmap-path>` captures a
visible owned system popup after 500 ms and exits; optional `--popup`,
`--popup-no-animation`, `--file`, `--file-no-animation`, `--file-keyboard` or
`--system-dpi` selects a diagnostic control. Exit 5 is unavailable observation,
not success. A final optional delay argument (1..10000 milliseconds) tests
whether drawing recovers without hover; the default remains 500 milliseconds.
After an explicit delay, `--trace` logs only this thread's native popup messages.
`--isolated-print`/`--isolated-client` print into an owned DIB on a private
desktop; `--isolated-hidden-print`/`--isolated-hidden-client` sample before show.
`--isolated-return` additionally wraps only the probe's native popup procedure
to record its unchanged WM_PRINT return. `--isolated-classic` is a process-local
theme negative control. None is physical-display evidence or a runtime fix.
No global animation/theme setting changes. These controls do not
repair menu drawing. `--queued-menu-test` measures 18 native menu-loop cases,
including disabled Close and outside-button input; these are queued messages,
not physical mouse input. `--gallery-trace <build-log-path>` records only this
demo thread's queue, dispatch and modal-filter messages for physical diagnosis.
The repaired gallery-12 double click also has a separate actual-input check.
PrintWindow capture is rendered-window evidence, not a physical-input/RDP test
or capture of DWM composition. The fixture itself uses no computer-use automation.
The non-client regression additionally reads existing WindowDC pixels without
PrintWindow: assert a white client-top strip, unchanged caption/buttons through
cursor/hover, title/icon/theme/activation changes and minimize/restore. It also
checks all resize cursor directions and application client-cursor delegation.
Owner subsequently authorized actual-desktop computer-use inspection of the
standalone demo; it is separate evidence, not replaced by these probes.

S4 work in progress: menu_paint supplies borrowed-DC row measurement/drawing.
menu_scope provides same-thread native popup-frame scopes. The shared window
component now enters/exits these at WM_ENTERMENULOOP/WM_EXITMENULOOP and cleans
them up on removal/destruction; applications do not wire a separate border hook.
Nested scopes support an outer owner's removal while an inner owner is active.
It neither owns HMENU data nor replaces input tracking. Only that thread's
popup windows shown/painted within the scope are temporarily subclassed.
No global hook/theme/animation change is made. Ordinary string popup entries
are temporarily adapted and restored; application owner-draw rows remain with
their owner. Tests now exercise actual queued popup loops for keyboard and
hover switching, nested tracking, MDI frame/child WM_NEXTMENU transitions,
merged system-button double-click, and callback removal/destruction. These
are bounded automated cases, not full physical-interaction acceptance.
Final S4 review and owner visual acceptance remain open. The private-desktop no-animation menu
tests and direct-print images do not certify the deferred native animation bug.
