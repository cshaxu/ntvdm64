# T484 S3 P33: private Console API diagnostic producer

The old build-win31-console-api-probe.ps1 is replaced by CMD x64
console-api-probes. Normal and PROCESS_HOOK_LATE_GUARD_TEST variants retain
all six original trace/guard/patch/decoder units and the input wrapper. The
probe executable reuses the existing private-mouse-probe target. These images
remain isolated beneath console-api/{normal,fault}; never publish them.

The retired producer forced a nonexistent process_hook_msvc.h. Current source
compiles without that obsolete header, as do the existing native process-hook
owners. No new shim, product or guest change is introduced.

[Build](../../../artifacts/build-migration/t484/s3-console-api/build-result.json)
and [bounded checks](../../../artifacts/build-migration/t484/s3-console-api/verification.json)
pass for both variants: actual linked trace/guard/export symbols and invalid
probe argument rejection. No installation of hooks, Win3.1 interaction or
runtime fault-injection acceptance is claimed.

[Cached full build](../../../artifacts/build-migration/t484/s3-console-api/product-build-result.json)
and [publication identities](../../../artifacts/build-migration/t484/s3-console-api/publication.json)
pass:11 raw images and22 hashes at both publication locations remain unchanged;
no recopy is necessary. S3 remains active.

This batch also introduces the original-candidate reconciliation. Mechanical
file status, explicit disposition and committed delivery are separate fields;
local deletion cannot count as delivered retirement. It does not substitute
for final live-call and newly added file audits.
