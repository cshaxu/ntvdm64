# T484 S3 P132 retire expired WOW research tools

In ordered continuation after B0195, B0197 and B0198 are retired rather than
translated. No absence-of-caller argument is used as the sole justification.

B0197 compiled wow_multimedia_inventory_fixture.c and queried public WINMM
format/device counts, timer caps and invalid system device IDs. It linked only
winmm.lib, never a project owner or guest. Its own source explicitly disclaims
playback, capture, callbacks and guest tests. This was host feasibility research,
not a maintained product regression. Remove its producer and sole fixture; keep
the independent dynamic named-export audit, without claiming runtime coverage.

B0198 parses a historical custom Ninja link spelling, a fixed original USER
object census and old component placement. It expects original ntstubs, old
WOW DLL binding ownership and a fixed thunk census. Those are historical
implementation snapshots, not today's product contract. Remove the old static
verifier; do not port its stale assertions into a new native target. Preserve
historical audit evidence and mark its old checklist reference as retired.
The dynamic-host-surface comment no longer claims this removed test checks OLE.

Current tools/build/wow32/CMakeLists.txt reads the actual original sources
manifest (72 C units), compiles its selected units, composes the explicit native
owners/link order and derives the DEF directly from the original. The S2 source,
profile and export reconciliation remains its historical migration evidence.
This is not a claim that every old lexical audit has equivalent runtime coverage;
those obsolete implementation assertions are intentionally removed.

No product, mirror or guest change and no new build producer. The cached full
host build and11 raw/22 published image identity check establish that removing
these tools leaves the current product inputs/publication unchanged. This is not
Full runtime acceptance and does not close S3/S4/S5.

[Receipt](../../../artifacts/build-migration/t484/s3-retire-feasibility/result.json).
Raw evidence: build/t484-s3-retire-feasibility.
