# M0 T346 — SoftPC graphics presentation window and Alt+Enter transition

## Source-selected product boundary

T346 is the P19 product-presentation package. It preserves the original
SoftPC video controllers, VGA/EGA memory, ports and update ordering. It does
not select `X86GFX`, a Console Server registration protocol, a kernel VDM
fullscreen route or host VGA access.

## S plan

### S1 — Source/ABI/failure inventory

Map original text and graphics surfaces, current Console façade, app/session
ownership and all unavailable Console-Server forms. Select a bounded modern
presentation cohort without treating an opaque handle as pixels.

### S2 — Session presentation-surface contract

Define and implement a bounded, session-owned presentation surface interface:
source-facing adapter bindings publish only validated text/graphics snapshots
or leases, while original SoftPC controller algorithms remain unchanged. The
old event-only callback is not a presentation implementation.

### S3 — App window and session-scoped Alt+Enter lifecycle

Implement the app-owned public Win32 window, keyboard routing, open/close and
deterministic creation-failure fallback. Alt+Enter selects this app lifecycle;
it never enters original X86GFX/fullscreen code.

### S4 — Local, paired and integration verification

Prove console-to-window/window-to-console transition, guest-frame presentation,
keyboard ownership, close/error cleanup and unchanged SoftPC machine state on
x86 and x64; then run one selected DOS graphics-mode workload.

## Prohibitions

No private Console Server, CSRSS, `X86GFX`, `NtVdmControl`, MONITOR, kernel
VDM, fixed host addresses, direct VGA aperture, Bochs route, synthetic video
device or guest/controller rewrite.
