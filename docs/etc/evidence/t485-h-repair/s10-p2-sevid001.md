# S10 P2 sevid001.c whole-file review

Fourth ordered file; HXP-R-00711--00735, 25 frozen points. Fixed XP input is
files.csv's SHA at revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
`Source/XPSP1/NT/base/mvdm/softpc.new/base/cvidc/sevid001.c`.
The [per-point record](s10-p2-sevid001-points.json) records each original/current
line, size and disposition. All 25 normalized differences match the register
one-to-one; there is no unregistered functional difference.

## Findings

Thirteen points preserve the same selected CPU40 CHAIN2 plane image reviewed
in [sevid000](s10-p2-sevid000.md), now for backward string reads. Physical
address is `4*(a & ~1)+(a & 1)`. Decrementing an odd guest address subtracts1;
decrementing an even address subtracts7. Thus initial even stride7 and xor6
retain alternating1/7; original stride3/xor2 reads a compacted plane image.
Mode0's original direct starting offset also lacks that plane conversion.
Both read modes retain the last latch cell by adding7 and aligning down to8
after decrement, rather than the compact-layout add3/align4 sequence.

Twelve points change only colour-compare/don't-care low-byte offsets:
1379/1383 to1376/1380. Original sinit012.c getters identify the DWORD bases,
and original j_c_lang.h's selected LITTLEND profile places the low byte at
offset0. All twelve replacements were checked for literal-only changes.
They cover both CHAIN2 loops and the CHAIN4 backward reader. Repeated-byte
values can hide the error; nonuniform field values distinguish the subfields.

Actual selected providers are the backward entries in ev_glue.c read_mode0/1
tables. The existing C-VID fixture compares both modes and both starting
parities against independent scalar reads, including the backward string
entries. P1 passed this production-linked fixture. This is not a claim that
each CHAIN4 backward mask combination received an individual runtime control;
its low-byte necessity follows the explicit x86 ABI and original getter layout.

All 25 one-line adaptations remain evidence-backed pending owner review.
Do not remove uncalled original functions or replace the generated reader to
hide those points. No new helper, branch, state or provider is needed. Unlike
sevid000's two nested WORD sites, no local-frame continuation edit occurs here.

## Implemented format recovery and status

Restore 3486 CRLF endings to original LF. Normalized source, line positions,
EOF content and binding stay identical. There are no #error or #line entries.
Metrics including all format bytes: **8/6974 ->25/50**, delta **+17/-6924**.
The newly separated existing points are covered by the owner format approval.

Local review is **4/30 files, 96/1060 frozen points**, 964 unreviewed.
Zero whole-point restorations, 96 evidence-backed residuals, no new retain
approval. Delivered remains1/30 files; three format-only files await cumulative
P2 delivery. The thirty-file total is **256/88240**, versus201/102863 at S10
start: **+55 points/-14623 lines**. Current census/master/pending hashes and
evidence references are synchronized. No P2 build/publication/test/commit/push
is claimed. Next ordered file is sevid004.c.
