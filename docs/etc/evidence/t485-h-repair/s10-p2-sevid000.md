# S10 P2 sevid000.c whole-file review

## Scope and method

Third S10 file, HXP-R-00647 through HXP-R-00710, all 64 frozen points.
Compare files.csv's fixed XP SHA at revision
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89` against the complete current file.
The upstream path is `Source/XPSP1/NT/base/mvdm/softpc.new/base/cvidc/sevid000.c`.
The [per-point record](s10-p2-sevid000-points.json) gives every ID, original
line, added/deleted size, original/current text and conclusion. The normalized
comparison has exactly 64 differences, each matched one-to-one to the register;
there are no additional unclassified functional edits. 3082 original lines
match, and every matching line retains its original physical/logical number.
There are no #error exclusions or #line directives in this file.

## Original alternative and concrete evidence

The selected production EVID tables in ev_glue.c bind these generated readers.
Restoring the entire file would restore the following incompatible calculations
and generated continuation behavior. These are not retained merely because
another adapter currently calls them.

| Category | Points | Original behavior versus selected contract | Minimal retained change |
| --- | ---: | --- | --- |
| CHAIN2 plane layout | 21 | Original `(a >> 1) << 2` compacts odd/even cells. The selected CPU40 plane image uses `4*(a & ~1)+(a & 1)`, retaining unused/font lanes. Subsequent guest bytes alternate physical steps 1 and 7. | Immediate shift 2 to 3; preserve odd lane; next pair +8; alternate step with xor6; align final latch cell with -8. |
| Low subfield addresses | 36 | Original offsets1379/1383 select high bytes of fields1376/1380 on x86; offsets1378/1382 select high words. Original sinit012 getters identify those field bases; j_c_lang.h LITTLEND gives REGBYTE=REGWORD=0. | Change only those literal subfield addresses to their existing field bases. All 36 edits were mechanically checked to contain no other changes. |
| CHAIN2 second word | 2 | A DWORD's second word starts at guest byte offset+2, not+4. | One immediate per mode. |
| Unchained second word | 1 | The WORD reader itself converts guest address to plane storage; passing offset+8 performs that scaling twice. | Original caller constant8 becomes2. |
| DWORD assembly | 2 | The original extraction writes r20, then returns LocalIUH[2], whose upper half has not received the second WORD. | Assemble the two returned WORDs into that existing result slot. |
| Nested local-frame continuation | 2 | The external label helper re-enters crules with CopyLocalIUH initialized to null. SAVED LocalIUH is static, so the inner leave frees the current WORD frame and resets LocalIUH to null rather than its saved DWORD caller. | Goto the existing same-function label, retaining the active CopyLocalIUH and original leave sequence. |

The older non-CPU40-style readers in ega_read.c also contain compact CHAIN2
formulas; they are not evidence that every historical machine profile used
the selected CPU40 plane layout. The conclusion is scoped to this product's
actual plane/BIOS/painter/provider contract, not a universal defect claim for
every original platform. Replacing the entire selected plane representation
to avoid these immediates would affect independent writers, painters and font
storage and is not a smaller same-behavior restoration.

Independent physical-plane checks are in
`test/mvdm-host/cvidc_vector_binding_fixture.c`: distinct seeded plane bytes,
even/odd/boundary offsets, both read modes, DWORD versus four guest bytes,
string readers versus scalar reads, and actual CCPU SAS callers. Mode1 masks
include non-repeated bytes to distinguish high/low subfield selection.

Historical original control in
`build/h-series-xp-audit-nonexcluded/chain2-control/results.json` records current
exit0 and restored XP group exit1. Its log reports mode0/offset100 expected
44332211, actual00000083. That grouped control proves the group cannot be
restored wholesale, not that every immediate was individually runtime-bisected.
Per-point justification also uses the explicit arithmetic, x86 field layout,
and nested activation data flow above. P1's actual production-linked fixture
passed these contracts; no new P2 execution is claimed.

Other original tail-label calls that terminate a top-level reader are retained.
They are not changed merely because they resemble the two nested WORD return
sites. No generated provider, private frame or dispatch state is replaced.

## Changes, metrics and delivery state

Restore 3134 CRLF endings to the original LF. Normalized source text, tokens,
line positions and EOF content are unchanged. All 64 functional points remain
evidence-backed owner-review entries; none is counted as a whole-point recovery.
No new runtime behavior, ABI, condition, shim or guest modification is introduced.

Byte-sensitive non-error differences: **10 points/6270 lines ->64/132**,
delta **+54 points/-6138 lines**. The increase exposes separated existing
adaptations previously joined by file-wide CRLF differences; it is covered by
the owner's standing format-restoration approval, not new functional edits.
Current three-file local audit: **3/30 files, 71/1060 points**, 989 unreviewed.
One file remains delivered; two files have format-only work awaiting the batch.
Zero fully restored frozen points; 71 reviewed residuals, no new retain approval.
All-thirty census **201/102863 ->239/95164**, cumulative **+38/-7699**.

Build/publication/runtime tests/commit/push for P2 remain pending. Format-only
changes may accumulate by owner instruction; this review does not claim a new
product delivery. The next ordered file is sevid001.c.
