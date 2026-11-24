# T484 S4 CMD isolated package identity refresh

## Contract and implementation

P5 moves the shared `New-PhysicalRuntimeStage -FreshPackageIdentity` operation
from PowerShell JSON generation to `refresh-package.cmd`, generated from the
LF native-build template. The helper still owns physical directory allocation,
copying and cleanup-marker creation; those remaining interpreted preparation
steps are not declared migrated by this delivery.

The existing `ntvdm-package-id` tool gains `--manifest`, a bounded reader for the
six-field product manifest schema. It accepts the current ten/eleven component
sets, unique names, canonical system32-relative paths, supported machine values,
nonzero and consistent UUIDs, and complete hexadecimal SHA256 fields. It rejects
unknown/duplicate fields, missing or duplicate images, embedded NUL, oversized,
truncated and trailing input. JSON field order, whitespace, UTF8 BOM and escaped
ASCII names are supported. It emits no rows until the complete input validates.
The fixed output alphabet is safe for the CMD token consumer; it is not a script
interpreter or a generic JSON execution facility.

CMD hashes every input before stamping any file, creates the new UUID through
the existing native tool, stamps only the caller-owned copy, and writes the
new manifest using the actual post-stamp hashes. It preserves Machine and
InputSha256. Default physical copying retains the original UUID. A failure
after stamping starts removes the completion manifest; the existing physical
stage owner then cleans its own failed copy. Preflight failure leaves all images
and the input manifest unchanged.

## Verification and evidence

The existing `test/build/Test-PackageIdentityTool.ps1` now covers manifest parsing
and invokes the real shared physical-stage provider for both ordinary copying
and fresh identity. It checks all11 resource IDs, manifest hashes, provenance,
source preservation and removal through the existing ownership helper. A wrong
hash in the final record proves that no earlier image is stamped prematurely.
The original UUID, PE architecture, stamping and malformed-PE cases remain.

- `build/t484-s4-stage-manifest`: PID27984 passed native build and parser tests,
  2026-11-24T16:00:55Z-16:01:03Z.
- `build/t484-s4-stage-manifest/refresh`: PID2088 passed actual shared copying,
  identity refresh and negative preflight,16:03:24Z-16:03:36Z. Its diagnostic
  stderr includes the intentionally rejected bad-hash package.
- `build/t484-s4-stage-manifest/delivery` records the final cached full build,
  complete package-tool tests, the existing real broker copy-identity test and
  unchanged11 raw/22 dual-publication hashes. Its launch/result JSON and logs
  record terminal status independently from source review.

The broker case uses the checked-in `verify-package-copy-identity.ps1` runner:
two physical copies keep one UUID; the second broker must fail without disturbing
the first. The preparatory test package gets a fresh ID to avoid unrelated live
package endpoints. Test-owned short O:/tmp stages are removed by the existing
identity-checked cleanup helper. No original guest is rebuilt or modified.

## Accounting and remaining work

No frozen inventory file is fully disposed merely by this shared suboperation.
Counts stay327/355 disposed,28 pending, plus24 overlapping preparation chains.
Directory copying, ownership marker production and caller-specific configuration
still need S4 handling. Final clean interpreter-free acceptance remains S5.
No fresh Full runtime result is claimed; unrelated known runtime failures remain.
