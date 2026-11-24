# T484 S3 P143 native frontend ownership audit

## Question and inputs

B0222 checked the retired combined graph, source-closure imports and fixed
historical source lists. It is a standalone architecture audit, not a Full
runtime gate or a producer. Use actual native product graphs instead.

## Implementation and scope

Retain source-level role/private-header/retired-protocol checks. Replace the
combined-graph walker and historical exact archive-membership snapshots with
11 actual product source closures: reject test fixtures, other F owners,
worker mechanisms outside the two workers, KVM outside NTCON and execution
creation in the launcher/presentation owners. Require current NTCON KVM and
NTVWM execution providers. The old order-only Run16/NTCON link spelling is
not an invariant of independent native-family graphs and is retired.

Reuse one native-source-graph reader with the BaseSrv ownership audit.
Explicit object/archive inputs and CMake implicit object/archive inputs are
followed; order-only prerequisites are not implementation providers. An import
library co-produced by an image link does not contain that image's source.
This is important for VDMREDIR -> ntvdm.lib, a legal named-image API boundary.
No private implementation exception was added.

Fixed historical archive lists (including old frontend-base membership) are
not retained as architecture rules. A/B compiler/include/prerequisite checks
remain in verify-library-build-boundary.py; this batch does not claim that
these two source-closure audits alone prove every layer or runtime callback.

## Verification

Raw runs: build/t484-s3-frontend-audit. First run exposed malformed negative
control array construction. Second run exposed the import-library expansion
false positive. Both corrected before the final run, PID11372, exit0.
All11 source closures pass;22 archive-hidden contamination controls reject.
The existing eight-product BaseSrv audit and its eight controls pass with the
shared parser. Actual source counts: NTVDM517, hook32 55, VDMREDIR48, VCDEX3,
WOW32 218, Run16 60, NTCON68, NTVWM68, NTMON16, NTSRV64, hook64 26.

[Receipt](../../../artifacts/build-migration/t484/s3-frontend-audit/result.json).
No production/build graph/guest change; no build, publication or Full runtime
acceptance claimed. S3 remains active; B0223 is next in inventory order.
