# T484 S3 P53 native message groups

## Question and change

B0058 already selected native CMake targets, but its PowerShell runner still
generated per-architecture CMD build programs. The three finite target groups
now belong to `test/build/wow-message-probes.cmake`; their CMD entry is
`tools/build-fixtures.cmd <x86|x64> wow-message-<external|cross|pending>`.
Existing per-target sources, definitions, options, libraries and output paths
are unchanged. The assertion runner delegates to CMD rather than writing
compiler/build programs. No product or original guest changes.

## Verification and scope

The checked-in runner is exercised with BuildOnly in allthree modes under
`build/t484-s3-message-groups`, serially sharing the existing architecture
caches. PendingSend additionally runs its existing x86/x64 native window
destruction contract. Each process has PID, arguments, deadline and logs in
the run's steps.json. BuildOnly success does not imply guest runtime success.

Retained run, product-build and publication identities belong under
`artifacts/build-migration/t484/s3-message-groups/`. Raw hash comparison is
possible only for files present before this batch; newly built peers are not
claimed byte-identical to unavailable historical outputs. Native group
membership preserves the old runner's exact architecture selections.

Allsix architecture/group builds passed. The x86/x64 native destruction
controls each passed both same-thread and cross-thread cases with status1,
error0, result447A007C and destroyed1. The one pre-existing observer image
retains its raw SHA256. These are native controls, not guest message results.
Cached full host build passed. All11 raw product inputs and22 published
hashes at assets/release and O:/winnt/system32 remain unchanged, so no product
recopy or repeated product runtime run was necessary. Parsing and documentation
governance passed. No original guest was rebuilt.

## Remaining work

B0058 remains pending: the runtime path retains historical coherent13 package
validation, media extraction/copy staging and the shared message runner's
remaining helper/staging chain. These must be reconciled with the actual
current manifest and CMD staging, not silently omitted from acceptance.
S3 is not closed; no Win16 runtime or final interpreter-free pass is claimed.
