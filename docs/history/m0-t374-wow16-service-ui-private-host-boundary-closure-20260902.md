# M0 T374 — WOW16 service/UI private-host boundary closure

## Outcome

Closed without a provider implementation. This is an exact source-shaped
unavailable boundary, not a claim that WOW16/Win16 execution works.

## Evidence

T374 S1 established that `wow32` is a 77-body original provider DLL, not a
collection of independently substitutable BOP leaves. S2 established that
`W32Init` reaches shared WOW state and the Base global-memory hook. S3
established that original `UserRegisterWowHandlers` returns one indivisible
`PFNWOWHANDLERSOUT` contract: 20 callback outputs plus `dwBldInfo` metadata,
whose targets and `gSharedInfo` identity belong to private NTUSER/Win32k and
CSRSS product machinery. The original Base hook must be recovered together
with its `GlobalFree` consumption lifetime.

The relevant records are:

- `docs/etc/evidence/m0-t374-s1-whole-wow32-provider-matrix-001.md`;
- `docs/etc/evidence/m0-t374-s2-original-wow32-init-boundary-decision-001.md`;
- `docs/etc/evidence/m0-t374-s3-wow-user-registration-contract-matrix-001.md`.

## Disposition

No partial registration table, static `W32Dispatch`, fake `gSharedInfo`,
synthetic Win16 task/NE loader, or recreated CSRSS/Win32k product shell was
added. The dynamic original `WOW32` ingress therefore remains unavailable
until a later owner-approved package can recover a complete same-shaped cohort
through bounded public/session bindings.

The owner reprioritized the recovery program: pure DOS CPU40/SoftPC boot,
COMMAND, `.COM` and MZ execution now precede any resumed WOW work. Queue and
runtime program were updated accordingly; no production source or guest media
changed.
