# M0 T271 S5 P4 — `MS_bop_E` app-entry extraction

## Question

Is the remaining `0x5E` fixed-frame ingress a mirror-owned BOP body, or only
the application composition required to enter the existing OpenNT `MS_bop_E`
mirror?

## Inputs

- OpenNT `softpc.new/host/src/nt_bop.c:MS_bop_E`.
- `opennt-bop/softpc/nt_bop_config_done.c` and its reached `UMBNotify`
  fragment.
- The former `ingress/config_complete_generic_ud_bridge` wrapper.
- Formal `t245-s11-config-complete` fixture.

## Procedure

1. Compared the former wrapper with the mirrored `MS_bop_E` body.
2. Moved only the selector recognition and bounded-frame invocation wrapper to
   `app/bop/config_complete_entry.{c,h}` using `git mv`.
3. Left `MS_bop_E`, `demDasdInit` ordering, and `UMBNotify`'s original empty
   notification behavior in the SoftPC owner mirror.
4. Regenerated the formal Ninja graph and built and ran the focused fixture.

## Observations

- The moved wrapper has no provider business logic: it recognizes the
  completed configuration selector and invokes mirrored `MS_bop_E` through
  the existing app DEM session frame context.
- `opennt-bop/ingress/` now contains 12 non-mirror bodies.
- Formal Ninja rebuilt the changed app objects, linked
  `bin/t245-s11-config-complete-fixture.exe`, and the fixture exited zero.

## Interpretation and confidence

The wrapper is app-owned final composition, while the reachable source
semantics remain in the imported SoftPC mirror.  No UMB allocator, DOS
business rule, or new BOP provider was introduced.  This is therefore a safe
owner correction, not a replacement of `MS_bop_E`.

## Follow-up

Continue the S5 residual audit with the 12 remaining ingress bodies.  The
SoftPC/firmware and Redirector entries require owner-package proof before any
similar move.
