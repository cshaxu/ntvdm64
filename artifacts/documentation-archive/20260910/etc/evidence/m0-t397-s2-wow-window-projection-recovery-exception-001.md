# M0 T397 S2 — WOW window projection recovery exception

## Decision

The first implementable UI cohort is a session-owned, public-Win32 projection
for only the original class registration, class/window private words, native
window callback, and deterministic teardown path. It is an adapter behind the
original WOW32 client; it is not USER32, CSRSS, Win32k, a generic message
server, or a guest ABI.

## Reused original mechanisms

The original client retains `W32Win16WndProcEx` in `wmdisp32.c`. Its ordering
is preserved: resolve class/window state, build the original `WM32MSGPARAMEX`,
thunk a supported message, call `CallBack16(RET_WNDPROC, ...)`, unthunk, then
return the 16-bit result or original `DefWindowProc` fallback.

No callback stack is invented. Existing `CallBack16` already acquires bounded
caller/callback leases; `mvdm_wow_task_frame` keeps task-local callback-stack
state; `session` has independently owned mapping-manager instances. The new
projection may use a new session-local opaque identifier plane, but native
`HWND`, `WNDPROC`, pointer and lease values never enter guest state or cross a
component ABI.

## Fixed initial ABI

The adapter must provide only these source-shaped operations:

| Operation | Inputs | Result/failure |
| --- | --- | --- |
| register class | copied `WNDCLASSA`, 16-bit procedure value, copied menu identity | 16-bit atom and opaque class identifier; fail exactly as class registration fails |
| create window | opaque class/window-parent identifiers plus copied original arguments | opaque 16-bit window identifier; no native handle escape |
| lookup/set class or window WOW words | opaque identifier and one fixed-width word record | copied `PWC`/`PWW`-shaped state or failure |
| dispatch native message | native callback receives its own `HWND`; adapter resolves it locally | invokes existing `W32Win16WndProcEx`/`CallBack16` in source order |
| destroy/cleanup | opaque class/window/task identifier | destroy native object first, retire identifier and copied state, reject stale reuse |

The first implementation may cover only the `WU32RegisterClass` → first
source-owned callback creation path. Dialogs, menus, cursors, DDE, task yield,
resource loading and arbitrary `pfnOut` outputs remain explicit later cohorts.

## Original call-site changes

Only original users of private words may be redirected: `wuclass.c` class
registration, `walias.c` class lookup, and the `wmdisp32.c` initial
class/window lookup. `wuwind.c` is eligible only when S3 proves its creation
arguments and teardown order. Direct `GetClassLong`/`GetWindowLong` access at
`GCL_WOW*`/`GWL_WOW*` must not remain on the projected path.

## Four-rung justification

Original WOW32 client source and its callback ordering are retained directly.
The original USER-server provider is an explicit unavailable stopping boundary.
The smallest same-shaped adapter is therefore a copied-state projection; no
external source is admitted. New state is limited to opaque session-local
identity and copied original `PWC`/`PWW` fields because public Win32 has no
private class/window-word storage. This is the minimum new behavior needed to
retain the reached client contract.

## Negative and acceptance plan

Focused fixtures must prove: class registration returns a stable opaque atom;
window callback reaches `RET_WNDPROC` in original order; failed registration
does not publish state; a stale class/window identifier is rejected after
teardown; and no native handle or host pointer is observable through a guest
word. The first integration proof is the existing fixed-stage WOWEXEC run
advancing past `WU32RegisterClass`; it is not a visible-window or full
lifecycle claim.

## S3 admission boundary

S3 may implement only the class registration/private-word/callback foundation
above, with one x86 fixture and one bounded fixed-stage run. Any need for
dialogs, menus, task scheduling, DDE, a generic pump, or a new guest-facing
identity immediately requires a separately admitted S.
