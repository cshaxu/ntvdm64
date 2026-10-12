# Experimental Win3.1 window frame

Project-authored same-thread static library with owner-approved original VGA
button pixels embedded locally; see [resource provenance](resources.md).
The original VGA System font is also embedded and loaded privately from memory;
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
System-menu single and double clicks go through the native non-client owner,
not a separate TrackPopupMenu loop. This preserves native menu tracking and
disabled-close semantics without a local double-click timer.
This is not a creation/Hook adapter or real title-prefix implementation.

The supplied reference palette is a preset, not a universal Win3.1 default.
High contrast reads host colors; no global settings are written. Caption
System font is rendered at its original 16-pixel base strike then scaled to
window DPI, avoiding GDI's discrete-strike substitution. A private face alias
selects the embedded original, not a host-supplied font with the same name.
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
PrintWindow capture is rendered-window evidence, not a physical-input/RDP test
or capture of DWM composition. No computer-use automation is involved.
