# S10 P2 sevid004.c whole-file review

Fifth ordered file; HXP-R-00736--00738 at original lines3746,4305,4315.
All three differences are one-line immediate changes1407 to1404. The
[per-point record](s10-p2-sevid004-points.json) includes original/current text,
hashes and full metrics. The original files.csv SHA was checked against XP
revision a2f6d7c93aa4f11efc51147566d5961c59a8ef89 at
`Source/XPSP1/NT/base/mvdm/softpc.new/base/cvidc/sevid004.c`.

Original sinit012.c's GetVideorotate and sinit013.c's SetVideorotate use
GDP1404 for the DWORD rotation count. In selected x86 LITTLEND, the low byte
is at1404. The generated generic byte-write and both bytes of generic
word-write instead reading1407 would observe the high byte: valid rotation1
would become0. Changing the whole field layout or adding a wrapper would be
larger and would break the existing original getters/setters. Three immediate
changes are the smallest compatible binding; no algorithm is replaced.

The current C-VID fixture exercises the actual generic byte/word providers for
all rotations1 through7, with independently computed expected bytes and full
plane comparisons. P1 passed that fixture. The register's three entries remain
evidence-backed pending owner review, not approved retain or restored. No
additional source difference was found; unchanged original units remain intact.

Restore5179 CRLF endings to original LF. Functional text, physical/logical
line positions and EOF are unchanged; there are no #error or #line entries.
Non-error metrics **1/10358 ->3/6**, delta **+2/-10352**, under the owner's
format-restoration approval. Original-normalized diff exactly matches all
three frozen entries, with no unclassified edits.

Cumulative **5/30 files,99/1060 points** source reviewed;961 unreviewed.
Zero fully restored frozen points,99 residuals, no new retain approvals.
Current thirty-file total **258/77888**, baseline201/102863, delta**+57/-24975**.
One file delivered; four format-only files await cumulative P2 delivery.
No new build/publication/runtime test/commit/push is claimed. Next sevid016.c.
