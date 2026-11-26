# S10 P2 sevid018.c whole-file review

Eighth ordered file; nine points HXP-R-00754--00762. Fixed XP revision
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89, original path
`Source/XPSP1/NT/base/mvdm/softpc.new/base/cvidc/sevid018.c`.
The [point record](s10-p2-sevid018-points.json) gives each original/current
body and verified original line. No source bytes changed:9/32 before and after.
Original LF and terminal newline already match. No #error or #line exists.

00757/00759 retain shift3 in the two parity paths of S_2679, matching the
selected physical plane formula4*(a&~1)+(a&1). The independent layout oracle
and grouped original negative control are in the [reader review](s10-p2-sevid000.md).
Restoring shift2 addresses compact storage, not this selected plane layout.

Seven continuations retain the same crules activation. Their actual same-TU
caller/callee pairs and parent allocation sizes are:

| Point | Label | Caller -> callee | Parent bytes |
| --- | --- | --- | ---: |
| 00754 | L21_77if_d | S_2664 -> S_2665 | 20 |
| 00755 | L21_87if_d | S_2666 -> S_2667 | 12 |
| 00756 | L21_92if_d | S_2670 -> S_2671 | 20 |
| 00758 | L21_103if_d | S_2678 -> S_2679 | 12 |
| 00760 | L21_111if_d | S_2682 -> S_2683 | 20 |
| 00761 | L21_121if_d | S_2684 -> S_2685 | 12 |
| 00762 | L21_126if_d | S_2688 -> S_2689 | 20 |

Each parent allocates before calling and frees/restores LocalIUH afterward.
The original external label function re-enters crules without ENTER_SECTION;
its automatic CopyLocalIUH starts null while LocalIUH is static. Its leave
sequence frees the child then loses the parent pointer. Existing goto retains
the saved pointer and original leave sequence. This is direct ownership
proof, not reliance on the T88 comment. Word fill/move loops also call these
writers and continue accessing their saved locals. No additional generated
helper needs replacement just because it shares this generated shape.

All nine points remain evidence-backed pending owner decisions. Zero new
retention approvals or complete restorations. P1 production-linked fixture
passed; no fresh per-site allocation experiment or P2 runtime pass is claimed.
Reviewed8/30 files,123/1060 points;937 remain. Total271/62154, cumulative
+70/-40709 from201/102863. One delivered file, six format-only source changes
and this unchanged review await P2. Next sevid019.c.
