# M0 T291 S5 P2 — Original BaseSrv source-package audit

## Question

Does an approved original source package exist for the BaseSrv side of
`GetNextVDMCommand`, and where does it belong in the product layout?

## Inputs and procedure

The audit inspected both approved OpenNT editions at their corresponding
`base/win32` paths, then read the original MVDM consumer declarations and
callers already mirrored in `mvdm-host` and `mvdm-platform-abi`.

## Observations

- `base/win32/server/srvvdm.c` is byte-identical in the two approved editions:
  SHA-256 `c1e2177c6c00679d85cfa475f620841f6736b0e56d8dbf790b71afe33e1ed80b`.
  It implements `BaseSrvGetNextVDMCommand`, command records, DOS and WOW
  record chains, paired wait handles, re-entry count, PIF handling and
  `BaseSrvRegisterWowExec`.
- `base/win32/server/srvvdm.h` supplies the exact VDM record declarations and
  server entrypoints. `base/win32/server/srvinit.c` registers the VDM server
  handlers. `base/win32/client/vdm.c` implements the original client-side
  `GetNextVDMCommand` capture/retry/wait/copy sequence. `base/win32/inc/basemsg.h`
  carries the BaseSrv message layout; `basesrv.h` is the direct source include
  carrier for the server body.
- The original server body depends on CSR server/client structures and calls,
  private process/console records, NTDLL RTL/NT primitives, and dynamic USER
  exports. These are a product-boundary dependency map, not evidence that the
  original command algorithm is unavailable.
- `mvdm-platform-abi` contains shared declaration-only carriers required by
  many unrelated MVDM packages. Merging it wholesale into a BaseSrv mirror
  would mix generic declarations with a behavior-bearing Windows Base package.

## Decision

Create an `opennt-host` original mirror for the selected OpenNT BaseSrv/client
VDM package. It is not a second MVDM provider and not a new generic adapter:
it owns the original Windows-host service code that MVDM historically called.
The package will initially preserve its upstream-relative paths and source
identity. It may consume the existing `mvdm-platform-abi` declaration mirror.

Do **not** merge the whole `mvdm-platform-abi` component into `opennt-host`.
If a declaration is proved to be private to the imported BaseSrv package, a
later source-selection record may relocate that exact declaration; generic
public/private ABI carriers remain separate.

## Next work

P3 imports the selected exact files and establishes their component README and
source/hash manifest. P4 maps each CSR/private-NT dependency to a same-shaped
`adapter-opennt-host` facade when it is BaseSrv-specific, an existing shared
adapter family only when the original interface is demonstrably shared, a
bounded `broker` transport where the original server requires a copied
cross-process record, or an explicit source-shaped unavailable result. No
self-authored command queue may precede that mapping.

## Confidence

High for source availability and package ownership: the identical server body,
original dispatch registration and client capture/wait sequence were inspected
directly. Build composition is intentionally not claimed until P3/P4.
