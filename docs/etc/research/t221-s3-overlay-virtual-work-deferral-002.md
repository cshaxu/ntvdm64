# T221 S3 P56 — Overlay and Virtual work deferral

## Decision

T221 S3 now executes only Direct and Readonly completion. New Overlay and
Virtual provider, BOP, package-session and CLI behavior is deferred. Their
current sources, current tests, source records and profile dispositions remain
preserved as frozen evidence; none may be deleted, weakened or silently
replaced with a Direct/Readonly fallback.

## Interface preservation

Direct/Readonly work continues through the existing selector-blind structures:

- `dem_whole_provider` remains the single owner entry;
- mutation-profile consumer authority remains outside individual BOP branches;
- file view and backend-kind/generation tokens remain typed; and
- checked guest RAM actions remain the sole machine-facing transport.

Consequently a later Overlay or Virtual resumption can supply its own backend
behind the retained view/token/profile seams. A Direct/Readonly special case
must not be embedded in generic routing, erase a frozen profile branch, or
reinterpret its unavailable/error contract.

## Resumption

The unified Overlay/Virtual debt in `docs/TODO.md` requires a separately
admitted owner-package plan, a current-state audit of the retained code, an
updated OpenNT source/ABI/failure map and its full profile regression before
new behavior is added.