# M0 T261 S3 P6 — DEM composition owner re-home

## Change

- Moved the DEM native `#UD` session, direct session, and original dispatcher
  bridge from `opennt-host/dem` to `opennt-bop/{ingress,dem}`.
- Updated all production ingress/import callers, affected fixtures, and the
  formal manifest.
- Retained the host drive-policy and namespace capability sources in
  `opennt-host`; they are not part of this BOP composition move.

## Boundary

The moved code owns selector-derived dispatch, call records, typed resume
outcomes, guest-memory callback wiring and access to the original `DemDispatch`
body.  It is therefore BOP/session composition.  The one remaining inclusion
of `opennt-host/dem/demerror_lock_shim.h` is component-qualified: it provides
the still-live DEM host capability ABI and does not transfer dispatcher
ownership back to `opennt-host`.

## Verification

- Fresh formal graph: `build/M0-T261-S3/r006`.
- All three moved production objects compile; a repeated Ninja invocation is
  an incremental no-op.
- The native-session, dispatcher-import and direct-session fixture objects
  compile.
- Component-manifest and production-tree governance gates pass.

## Scope

No DEM provider behavior, filesystem policy, handle semantics, or machine
mechanics changed.  This is an ownership-only correction.
