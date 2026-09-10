# M0 T287 S2 — original XMS static composition

## Question

Can the complete common XMS source set build as original, selector-disabled
objects on both supported host architectures without selecting its historical
direct-address back end?

## Inputs and procedure

`New-T287OriginalXmsStaticNinja.ps1` generated one formal Ninja graph per
architecture.  Each graph forces the existing declaration carriers and selects
only these exact `mvdm-host/xms.486` units:

- `xms.c`, `xmsa20.c`, `xmsblock.c`;
- `xmsdisp.c`, `xmsmisc.c`, `xmsumb.c`.

It generated a SHA-256 source manifest, compiled with MSVC `/MT`, and archived
the resulting objects.  `lib /list` then verified the archive members.

## Observations

- x86 and x64 each built `original-xms-common.lib` successfully.
- Each archive contains exactly six members: `xms`, `xmsa20`, `xmsblock`,
  `xmsdisp`, `xmsmisc`, and `xmsumb`.
- The two generated source manifests carry the same six source hashes as the
  S1 source ledger.
- Neither graph selects `i386/xmsmem86.c`, `xmsmemr.c`, non-x86 `sources`, an
  executable, an ingress route, a Bochs source, or `src.old`.
- MSVC reports historical warnings in the unchanged source.  In particular,
  x64 reports `ULONG`/pointer conversions in `xmsblock.c` and `xmsumb.c`.
  The static archive does not make those expressions a safe runtime binding;
  S4 must supply their same-shaped bounded-lease/mapping-manager successor.

## Interpretation

The original common package is now a real, source-identity-preserving static
island.  It does not enable XMS behavior: unresolved external contracts remain
unresolved archive imports and no `52:xx` ingress is selected.  This proves
the next operation may bind the smallest common A20/guest-memory seam without
cropping or replacing the XMS dispatcher.

## Follow-up

T287 S3 binds the original `xmsa20.c` call sequence to the session-owned
`guest_memory` mapping manager and typed `adapter-bochs` A20 mechanics.  It
must not consume the direct-address backend or create an XMS-specific mapper.
