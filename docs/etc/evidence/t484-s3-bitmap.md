# T484 S3 P96 bitmap producers and retention review

B0160 retains real bitmap binding and failure-cleanup contracts. The shared
WOW native graph now owns compilation, RC and linking, using current product
profiles/objects and the existing clres bitmap-fixture selection. Remove
command-line substitution and historical provider-cache overrides.

The fixed synthetic2x1 black/white BMP is66 bytes, authored test data, not
guest media. Store it beside the existing RC input and delete the old
PowerShell generator. RC tracks this input. No product, mirror, original
guest or approved runtime asset is changed.

First runtime PID27824 passed both native fixtures but failed the static
registration audit. That audit assumed output assignments still lived in
the B bridge; their actual owner is C clglobal.c. Correct only the source
owner selection/name and retain the original21-input/20-output, pinned
layout, bitmap/full-handle and scheduler-owner assertions. Corrected full
runner PID32144 exited0:
- binding: nested30, parallel4x50, named2, missing2, GDI0 before/after;
- failure: locks2, screen10/10;
- registration:21/20 slots, build84000000, static audit only.

Final PID17516 exited0: cached full host build, two stable fixture
hashes/mtimes, synthetic input identity, x64 rejection2,11 raw host hashes
and22 published-copy hashes. Product bytes unchanged; existing dual
publication was verified. This is not real Win16 callback/guest acceptance.

[Receipts](../../../artifacts/build-migration/t484/s3-bitmap/result.json)
include first failure and corrected verification; raw evidence remains
build/t484-s3-bitmap. One inventory entry completes:193 complete/162 remaining
of355. The removed helper and retained registration audit are outside that
inventory. S3 and retrospective retention audit remain active; S4/S5 pending.
