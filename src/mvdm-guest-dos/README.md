# mvdm-guest-dos

Complete repository-local DOS/V86 guest mirror, recovered byte-identically
from `src.old/opennt-guest/dos-v86` during M0 T274 S3. It is a load-only guest
component: its source, resources, build descriptions, intermediates and
products never enter a host compile or link closure.

## Provenance

- [T274 S3 manifest](../../docs/etc/operations/m0-t274-s3-dos-guest-mirror-manifest.tsv)
  records all 676 carried paths, source SHA-256, size and copy disposition.
- [T274 S3 closure evidence](../../docs/etc/operations/m0-t274-s3-dos-guest-mirror-closure-001.md)
  records source/destination equality and output isolation.
- `build/output/dos` and `build/output/wow16` remain established build-output
  evidence in place. They are not copied here, host-linked, or implicitly
  repackaged.

## Divergence register

None. Every file below this component, apart from this component README, is
byte-identical to the path and hash recorded in the S3 manifest.
