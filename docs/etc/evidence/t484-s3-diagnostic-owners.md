# T484 S3 P31: diagnostic owner migration

Five historical PowerShell producers are retired. CMD native-diagnostics
builds the Console grid wrapper and two service observation fault images;
worker-diagnostics now builds16 variants, including COMMAND client tracing
and four independently authored startup observers. These are isolated test
images, never published product images.

Actual product compile profiles, generated-header prerequisites, ordered link
inputs and archive membership are reused. The old COMMAND builder replaced
whole core/launch libraries, which would discard current frontend_rpc_client
and base_dispatch members. Only the launch translation unit is now wrapped;
the [16 variant map checks](../../../artifacts/build-migration/t484/s3-diagnostic-owners/link-selection.json)
explicitly require both retained members. Duplicate unguarded ABI includes in
the test wrapper were removed by placing definitions after the wrapped source.
The retired forced process_hook_msvc header no longer exists; startup units
use the current production process-hook profile instead.

[Native selection checks](../../../artifacts/build-migration/t484/s3-diagnostic-owners/native-selection.json)
verify three replacement objects and two exact fault-source projections after
line-ending normalization. [Diagnostic builds](../../../artifacts/build-migration/t484/s3-diagnostic-owners/build-result.json)
pass. This is compilation/link-selection evidence, not runtime fault or guest
installation acceptance. Existing diagnostic log restrictions are unchanged.

[Cached full host build](../../../artifacts/build-migration/t484/s3-diagnostic-owners/product-build-result.json)
and [publication identity](../../../artifacts/build-migration/t484/s3-diagnostic-owners/publication.json)
pass:11 raw images and22 hashes across both published locations are unchanged,
so no recopy is performed. No product, mirror or original guest source changed.
S3 remains active; final interpreter-free clean acceptance remains S5.
