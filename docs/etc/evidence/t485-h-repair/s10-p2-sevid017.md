# S10 P2 sevid017.c whole-file review

Seventh S10 file, HXP-R-00743--00753. The fixed XP source path is
`Source/XPSP1/NT/base/mvdm/softpc.new/base/cvidc/sevid017.c`, revision
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89. The saved original was hash-checked.
The [eleven-point record](s10-p2-sevid017-points.json) contains exact original
and current bodies, original positions and the necessity evidence.

Four shift changes (00745,00747,00751,00753) retain the selected CPU40 plane
layout: physical(a)=4*(a&~1)+(a&1). These occur in the two parity paths of
S_2643 and S_2661 byte writers. Restoring shift2 after discarding A0 selects
compact spacing rather than the actual four-plane storage. The independent
plane oracle and grouped original negative control are recorded in the
[reader review](s10-p2-sevid000.md); this is not a per-point runtime claim.

Seven same-function continuations preserve existing parent frames. Verified
same-TU call edges, identified by their original generated numeric symbols:

| Point | Continuation | Callee | Enclosing caller |
| --- | --- | --- | --- |
| 00743 | L21_19if_d | S_2631 | S_2630 |
| 00744 | L21_24if_d | S_2635 | S_2634 |
| 00746 | L21_35if_d | S_2643 | S_2642 |
| 00748 | L21_43if_d | S_2647 | S_2646 |
| 00749 | L21_53if_d | S_2649 | S_2648 |
| 00750 | L21_58if_d | S_2653 | S_2652 |
| 00752 | L21_69if_d | S_2661 | S_2660 |

Each enclosing caller allocates LocalIUH before the call and frees/restores it
afterward. crules has static LocalIUH and automatic CopyLocalIUH initialized
to null. Original continuation helper re-entry creates a new activation without
ENTER_SECTION, frees the callee frame and resets LocalIUH to null, losing the
parent allocation; callers that continue reading their locals additionally
lose their usable frame. goto reaches the same existing leave block with its
saved parent intact. This concrete ownership defect, rather than an old T88
comment or hypothetical recursion, is the reason for these seven residuals.
No additional label helper, allocation algorithm or dispatch path is changed.

Restore3361 CRLF endings to original LF. Normalized code, line positions and
EOF shape remain unchanged; the file contains neither #error nor #line.
Non-error metrics **1/6722 ->11/36**, **+10/-6686**, under the owner's standing
format approval. All11 residuals remain pending; none is counted as a fully
restored frozen point. P1 production-linked C-VID fixture evidence is reusable
for source reasoning, not a new P2 final-package acceptance result.

Cumulative source review **7/30 files,114/1060 points**,946 unreviewed.
Current30-file total **271/62154**, versus201/102863: **+70/-40709**.
One file delivered; six format-only files await P2 delivery. No P2 build,
publication, runtime test, commit or push is claimed. Next sevid018.c.
