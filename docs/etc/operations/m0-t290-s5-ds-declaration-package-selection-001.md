# M0 T290 S5 — DS declaration package selection

## Decision

Import the complete original OpenNT `public/internal/ds/inc` declaration
package into the same relative root under `mvdm-platform-abi`:

`source/public/internal/ds/inc`

This is a declaration carrier only.  It does not select a Lanman runtime,
server, RPC implementation or Redirector service route.

## Evidence

- OpenNT contains 95 files; OpenNT-4.5 `nt/private/net/inc` contains 102.
  The latter has seven extra network/product headers, while OpenNT has 37
  one-sided private/security headers.  Neither directory is a simple exact
  superset as a whole.
- The ten headers reached by `vdmredir/vrnetapi.c` are selected as the OpenNT
  package lineage.  Nine shared candidates are byte-identical in both roots:
  `apinums.h`, `remdef.h`, `remtypes.h`, `rxp.h`, `apiparam.h`, `xstypes.h`,
  `xsprocs.h`, `netlibnt.h` and `rxuser.h`.
- `crypt.h` is OpenNT-only within this particular package; it is retained with
  its original package rather than substituted from unrelated crypto trees.
- This resolves the existing `m0-t274-s2-platform-abi-direct-include-ledger`
  ambiguity for the reached `vrnetapi.c` declaration closure at package scope.

## Reproducibility

The companion [per-file manifest](m0-t290-s5-ds-declaration-package-manifest-001.tsv)
records all 95 logical paths, the exact selected OpenNT source paths and
SHA-256 values. It is an import/provenance record, not a build manifest.

## Boundaries

- Every imported file remains original text and outside the host link/runtime
  selection until an owner package proves a source body can compose.
- Any additional OpenNT-4.5-only DS declaration is selected only by a later
  complete package review; this import does not silently combine editions.
- The source path, role and direct compile result are recorded by the S5
  evidence/manifest after import.
