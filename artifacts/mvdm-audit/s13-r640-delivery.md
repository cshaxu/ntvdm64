# S13: original BIOS and device guard line mappings

Four previously changed files completed their conditional review: their only
original condition groups are SCCSID and SEGMENTATION, and selected source
lines match the pinned original when the already rejected arms and diagnostic
directives are projected out. See [r638](s13-r638-simple-profile-review.json).

Ten further BIOS, CCPU and device source files received original diagnostic
and continuation line mappings for 17 previously rejected SCCSID or
SEGMENTATION groups. Their other condition groups remain pending. Fifteen
guard-ledger physical anchors were reconciled with the changes; see
[r639](s13-r639-basic-guard-lines.json).

Cached x86, x86-extra and x64 product builds passed, as did 181 retained guard
controls and 17 isolated MSVC diagnostic/continuation controls. Eight product
images are byte-identical to the preceding release; two differ only in PE
timestamps, as recorded in [the comparison](s13-r640-build-image-compare.json).
The previous Full 17/17 evidence is reused per owner instruction, not rerun.
All ten components were published and hash-verified in both destinations.
Guest files were not modified. Full source and build-context review remains
open.
