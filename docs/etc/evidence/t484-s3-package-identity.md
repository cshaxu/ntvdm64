# T484 S3 P156 native package identity test producer

## Changes

B0016 verify-package-identity.ps1 exercises the current original package-scope
provider. Add package-identity to the existing CMD/native worker fixture graph,
linking its unchanged source against the current common-base-srv-transport
archive. Preserve its separate-identities, mixed-component and missing-resource
assertions and the current provider's ten-component scope contract. This does
not expand that contract or claim11-component runtime identity enforcement.

Set-PackageIdentity.ps1 previously compiled C# through Add-Type on demand.
Replace that implementation with the already built native ntvdm-package-id
writer. Preserve empty-GUID rejection, sequential path resolution/write and
failure propagation. The wrapper requires native tools to be built rather
than hiding an interpreter compiler. The remaining PowerShell publication
orchestration is explicitly still S4 work; B0337 is not closed here. B0015
real two-package isolation and the shared identity reader are not closed by
this writer change.

## Verification

Background PID19540 builds and runs the unchanged identity fixture: separate
package IDs produce distinct endpoints, mixed images and missing identity
resources reject. PID21732 checks empty/missing/locked writer failures with
unchanged input hashes, then passes cached full x86/x64 and11 raw/22 published
hash checks. Host product images remain unchanged; no republication claimed.

[Receipts](../../../artifacts/build-migration/t484/s3-package-identity/result.json).
These are unit/provider checks, not a real-package RPC isolation run or Full.
No product scope algorithm, original guest or assets changed. One disposition
completed:286 of355,69 remain; S3 and S4/S5 remain open.
