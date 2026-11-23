# T484 S3 P64 CRT verification producer

B0072 now invokes CMD targets in the existing native WOW32 graph. The graph
owns standalone test/provider compilation, links against the actual WOW32 CRT
object, and exposes explicit missing/duplicate-provider negative link targets.
The verifier no longer emits compiler.cmd or constructs CL/LINK invocations.
The two standalone objects retain /MT and the test's original stdio definitions;
the production object retains its real profile. No product source changes.

The old obj/adapter-win32/crt_compat.obj path no longer identifies the native
graph's object. A generated object-path record now refers directly to the
production object target; Ninja command/dependency checks inspect that object.
MAP checks accept the native crt_compat.c.obj spelling while still requiring
the actual provider archive and symbols. No header-isolation, formatting or
negative-link assertion was removed.

The actual verifier passes: independent and production-object formatting,
scanning, invalid-input behavior; production source/MAP ownership; absence of
outer-layer headers; LNK2019 for missing provider; LNK2005 for duplicate provider.
The negative targets are excluded from product/default builds and run only
when explicitly requested. Raw runs are under build/t484-s3-crt and concise
receipts under artifacts/build-migration/t484/s3-crt. This standalone verifier
has no runtime staging dependency and can be closed after delivery verification.

Cached full build passed. Final verification preserves both CRT executable
hashes/timestamps on a no-op build, rejects all three x64 requests, verifies
11 raw host inputs and22 published hashes unchanged, and passes governance.
No product recopy or guest runtime rerun is necessary for this test-build-only
change. Fixed inventory:355 entries,135 completed dispositions,220 remaining
(54 pending producer chains and166 awaiting reconciliation).
