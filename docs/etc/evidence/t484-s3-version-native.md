# T484 S3 P146 version-negative producer checkpoint

## Scope

B0231 is a real Full gate and cannot be retired. Its JS runner previously
constructed sources, MIDL commands and a separate hand-maintained linker
closure. The five negative peers now have a CMD target, version-negatives,
in the existing native service graph. Current production entry/stub flags
and image link inputs are reused. Strict unique mutation boundaries preserve
old-app, wrong-protocol, reply-app, reply-protocol and legacy-interface cases.
Legacy MIDL uses the current service ACF, including its asynchronous contract.
Original product/guest sources are not modified.

The JS runner delegates production to build-fixtures.cmd and consumes these
five images. It no longer writes C/IDL/CMD recipes or calls compilers/linkers.
The obsolete equality between active task number and APP_VERSION is removed:
CURRENT explicitly retains the previous product version for this byte-identical
build migration; protocol/header/IDL agreement remains checked.

## Evidence and remaining work

Raw directory: build/t484-s3-version-native. Native five-peer build PID1276
exited0. Cached full host build PID3632 exited0 and verified11 raw input plus22
published hashes unchanged. No republication was required. Final graph/runner
syntax and documentation checks are recorded separately in closure logs.
[Receipt](../../../artifacts/build-migration/t484/s3-version-native/result.json).

This is a producer checkpoint, not B0231 delivery or S3 closure. The JS runner
still uses Set-PackageIdentity.ps1 for fixture resources. Replace this remaining
interpreted preparation dependency and verify all five real peers against both
Run16 and NTVDM in an owned isolated package. Runtime refusal/error/cleanup
results have not been rerun here. No existing Full record is used to claim
that this modified runner passes. B0231 stays pending; completion count does
not increase. B0232 onward remain ordered follow-up work.
