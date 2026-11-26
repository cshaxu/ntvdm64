# S10 P2 sevid016.c whole-file review

Sixth S10 file, four frozen points HXP-R-00739--00742. Fixed XP path is
`Source/XPSP1/NT/base/mvdm/softpc.new/base/cvidc/sevid016.c`, revision
a2f6d7c93aa4f11efc51147566d5961c59a8ef89; files.csv SHA verified before work.
The [four-point record](s10-p2-sevid016-points.json) matches every normalized
diff to its original line and frozen ID, without omitting unused original code.

00739/00741 each change one shift immediate2 to3 in the selected
S_2625_Chain2ModeXByteWrite even/odd paths. This preserves the same physical
plane address as the independent CPU40 oracle documented in the preceding
reader reviews. No generated dispatch, masks or provider are replaced.

00740 replaces the L21_1if_d function re-entry with its same-function label.
S_2625 is actually called by wrappers and move loops in the same sevid016
crules function. LocalIUH is static; CopyLocalIUH is per C activation.
The original label helper creates a fresh CopyLocalIUH=null, frees the current
frame, and loses the enclosing saved frame. The same-function goto preserves
that frame for its actual caller. This is not a hypothetical recursion claim.

00742 has a separate verified caller: S_2628_Chain2ByteMove allocates20 bytes,
calls S_2629_Chain2ModeXFwdByteMove, then executes its original free/restore
sequence. Restoring the L21_9if_d helper re-entry would reset the shared local
pointer to null, so the wrapper cannot free its allocation. The retained goto
uses the existing label and leave sequence; no second allocation mechanism.
Other original label helpers are not rewritten merely because they look similar.

The actual production-linked C-VID fixture passed in P1, including ModeX
byte/word/dword paths, SAS moves/fills and subsequent activations. Source
ownership/data-flow evidence above establishes each retained boundary; this is
not a claim of a new per-site allocation-count runtime test in P2.

Restore4530 CRLF endings to original LF, preserving normalized source and all
original line positions. No #error or #line exists. Metrics **1/9060 ->4/12**,
delta **+3/-9048**, covered by original-format approval. Four points remain
pending owner review; zero restored frozen points or new retain approvals.

Cumulative **6/30 files,103/1060 points** reviewed,957 unreviewed. All-thirty
total **261/68840**, versus201/102863: **+60/-34023**. One file delivered;
five format-only files await cumulative P2 delivery. No P2 build, publication,
runtime test, commit or push is claimed. Next sevid017.c.
