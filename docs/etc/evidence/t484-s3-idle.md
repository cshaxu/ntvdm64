# T484 S3 P56 idle witness producers

## Change and boundary

B0061 now delegates its two native host witnesses and four authored Win16
assembly variants to CMD/native CMake targets. Separate and shared-worker
profiles retain their previous defines and compiler flags. Prebuilt guest
inputs still require both expected hashes before copying. Production sources,
original guest bytes and runtime lifecycle assertions are unchanged.

BuildOnly explicitly reports guestRuntimeExecuted=false and stops before
runtime staging. It verifies producer integration, not guest acceptance.
The old CompilerDriver/Nasm parameters remain accepted for caller compatibility;
production tool selection belongs to the native build entry.

## Verification

The original CL/NASM commands were replayed in an owned background baseline.
All four NE binaries and four listings are byte-identical. Both native EXEs
have identical lengths and differ in only two PE timestamp bytes each.
Four runner combinations (separate/shared, built/prebuilt guests) pass.
The x64 target is rejected with exit2. Native host-only composition passes.
Detailed process identities, commands, output and results are under
build/t484-s3-idle; concise comparisons are retained in
artifacts/build-migration/t484/s3-idle.

## Remaining work

B0061 remains pending because its physical runtime/package staging helpers
still use the old interpreted chain. Local compiler migration does not close
that dependency. No new real-guest lifecycle pass is claimed for this batch.

Cached full host build passed. All11 raw host inputs and22 published hashes
at assets/release and O:/winnt/system32 remain unchanged; no recopy was
needed. Runner parsing and documentation governance passed. Existing runtime
evidence is reused under unchanged-product authorization, not a fresh run.
