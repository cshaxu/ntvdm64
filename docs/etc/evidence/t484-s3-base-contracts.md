# T484 S3 P83 BASE contract producers

B0112-B0114 previously wrote one compiler batch per architecture. They now
invoke CMD targets base-classifier-path, base-resource and base-vdm-config.
One native declaration shares only their build mechanics. Each executable
still links the actual product object: x86 worker for all three; x64 launcher
for classifier-path and x64 service for resources/config. Provider algorithms
are not recompiled into test-specific replacements. Missing product objects
remain required link dependencies, never silently replaced by source builds.

Caller flags retain /W4 /WX /MT and ntdll; classifier alone retains its original
external SDK header paths. Native compiler dependencies cover caller headers,
and the explicit product object dependency invalidates the link. Runners
retain fresh output validation, input hashes before/after, owned background
compiler/runtime PIDs, time budgets, exit codes and their original assertions.
The only execution-path change is selecting the native cache executable.

Initial classifier verification PID14220 exited0. Final three-contract run
PID22188 exited0: classifier has three cases at layouts12/24, resource has
errors0/duplicates1/closes1, and VDM configuration has failures0 on both
architectures. These cover actual path conversion, resource binding/ownership,
and independent-thread configuration semantics, not merely successful linking.
No production, mirror, original guest or package algorithm changed.

[Contract receipts](../../../artifacts/build-migration/t484/s3-base-contracts/contracts-result.json)
retain the six runtime witnesses and input identities. Raw logs and owned
scripts are under build/t484-s3-classifier. The cached full product build,
no-op output identities, dual publication hashes and governance are separate
receipts; no unchanged product image is claimed as a fresh publication.

After successful delivery all three rows are complete. Fixed355 inventory:
161 complete,194 remaining (53 pending producer/staging/verification rows,
141 awaiting reconciliation). S3 remains active; S4/S5 are not complete.
