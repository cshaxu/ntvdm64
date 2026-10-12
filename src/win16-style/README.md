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
Initial menus, child/MDI, scrollbars, client-edge/tool/layered
windows and minimized/maximized installation are deliberately unsupported.
Consumers must not introduce those unsupported forms while attached. Native
system popup menus remain native; later stages own menu/MDI integration.
System-menu tracking remains native, not a separate TrackPopupMenu loop.
If the host reports an empty native system-button rectangle (modern dialog
frames), our button enters the system menu through SC_KEYMENU/space after
release inside its rectangle, using existing capture/cancellation tracking.
Double-click commands retain native disabled-close semantics without a local
timer. Physical second-click routing while that menu is open still requires
acceptance; message-level close tests do not establish it.
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
forms/states; the last five explicitly labelled native references show menus,
scrollbars, child/MDI and ordinary host appearance, not implemented retro
support. Minimized presentation is also still native. F2 changes title,
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
not success. No global animation/theme setting changes. These controls do not
repair the known gallery-12 physical double-click regression.
PrintWindow capture is rendered-window evidence, not a physical-input/RDP test
or capture of DWM composition. The fixture itself uses no computer-use automation.
The non-client regression additionally reads existing WindowDC pixels without
PrintWindow: assert a white client-top strip, unchanged caption/buttons through
cursor/hover, title/icon/theme/activation changes and minimize/restore. It also
checks all resize cursor directions and application client-cursor delegation.
Owner subsequently authorized actual-desktop computer-use inspection of the
standalone demo; it is separate evidence, not replaced by these probes.
