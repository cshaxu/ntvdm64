# T484 S3 P147 native package identity and version negatives

## Scope and implementation

Completes B0231's producer migration after the
[producer checkpoint](t484-s3-version-native.md). Native ntvdm-package-id
writes the same 16-byte GUID to neutral-language RCDATA NTVDM_PACKAGE_ID,
using the existing Win32 resource update contract. It rejects malformed/zero
UUIDs and returns native write errors. It belongs to the existing CMD
bootstrap tool graph. No runtime/product/guest algorithm changes.

The version runner calls that native tool, not Set-PackageIdentity.ps1 or
Add-Type. Full no longer sets its obsolete OPENNT_VERSION_PWSH parameter.
Five generated peers still use the actual service profile/link closure and
the legacy-interface peer uses current asynchronous ACF semantics. Node keeps
bounded test execution, isolated fixture copying, assertions and cleanup;
it no longer generates source or calls MIDL/cl/link/PowerShell preparation.
Other Set-PackageIdentity callers remain S4 work; this does not claim the
whole release chain has already migrated.

## Verification

Raw evidence: build/t484-s3-version-runtime. Initial native tool build failed
on ANSI RT_RCDATA used with UpdateResourceW; explicit MAKEINTRESOURCEW fixes
that compiler error. Final build PID19244 exits0 with /W4 /WX.

Runtime PID5364 exits0. Existing physical-stage helper copied the complete
package;11 current published images replaced the historical host inputs.
Native stamping assigned one fresh identity to all11; independent existing
resource reader confirmed each GUID's exact byte layout. Real old-app,
wrong-protocol, reply-app, reply-protocol and legacy-interface servers each
reject Run16 and NTVDM with error1306 and version-mismatch text:10/10.
No client retries/timeouts passed. Owned servers and stage O:/tmp/n033 were
removed; original package and O:/winnt were untouched.

PID12344 verifies malformed/zero UUID preserve input, native roundtrip,
locked image error32 preserves identity and missing image returns2. Temporary
stamp copy is removed. PID17020 cached full build exits0,11 raw host hashes
and22 published hashes unchanged; no republication required. This is the
version-negative gate only, not a newly completed Full suite.

[Receipts](../../../artifacts/build-migration/t484/s3-version-runtime/result.json).
S3 continues at B0232; final S4/S5 acceptance remains outstanding.
