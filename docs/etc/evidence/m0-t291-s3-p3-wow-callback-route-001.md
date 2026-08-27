# M0 T291 S3 P3 — WOW callback transaction route

## Question

Can the new one-session WOW callback handoff coexist with the already
recovered COMMAND monitor request rather than replacing it through a global
session callback?

## Result

`session` now dispatches registered, unique numeric operations before its
older fallback. This is a neutral routing primitive: its records do not know
what a COMMAND, WOW, Bochs frame, pointer, handle or worker is. The existing
`app_command_source` now uses its original monitor operation route.

The WOW adapter creates a copied `mvdm_wow_callback_transaction`, uses the
monitor-owned task-frame projection to select the numeric callback stack, and
dispatches one synchronous operation. The request has a fixed copied parameter
buffer and numeric AX/DX result; no guest or native pointer passes to the
dispatcher.

## Verification

- The new callback fixture passed x86 and x64. It verifies exact operation,
  task/stack values, input copying, AX:DX result reconstruction and oversized
  request rejection.
- The existing original `GetNextVDMCommand`/COMMAND app route was freshly
  rebuilt and passed on x86 and x64 after the routing extension.

## Limit

This is a synchronous session handoff, not yet a completed `CallBack16`
machine re-entry. P4 must bind the handoff to the SoftPC-shaped typed
pause/resume and frame-write/read sequence. No original WOW provider body,
selector or guest route is enabled by P3.
