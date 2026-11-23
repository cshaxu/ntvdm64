# T484 S3 P71 obsolete performance producer retirement

B0098 is not a usable current producer: build-worker-performance.ps1 requires
console_graphics_measured.c, absent both now and in fixed migration baseline
ec5b074859bc091ec685d81a7a8603ed416c3cea (git cat-file lookup exits128). It also
parses the old worker_link/frontend_link graph and defaults to T427 caches.
Search across current test/tools finds no caller. Historical T429/T440 reports
remain indexed evidence, not live source dependencies or current acceptance.

Remove this obsolete interpreted builder. Do not recreate its missing
instrumentation or claim a current performance observer was built. Remaining
measurement source, worker_performance_test.c, measure-worker-boundaries.ps1,
and verify-performance-milestones.ps1 are untouched; the measurement runner
explicitly accepts externally supplied diagnostics. Their presence is not
proof they currently pass. Their independent staging/build dependencies remain
subject to later inventory reconciliation. No supported product/addon target,
production source, native build graph or release asset is changed.

P70 cached full build remains applicable to these identical production inputs.
This batch checks publication identity and governance, without redundant
compilation or a new guest/runtime test claim. Raw evidence belongs to
build/t484-s3-performance-retirement; concise receipt is under
artifacts/build-migration/t484/s3-performance-retirement.

Fixed355 inventory:148 completed,207 remaining;50 pending producer chains and
157 awaiting reconciliation. This closes the obsolete builder only, not the
performance runtime/staging family or the overall S3.
