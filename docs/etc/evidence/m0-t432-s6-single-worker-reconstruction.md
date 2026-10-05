# T432 S6 single-worker reconstruction

## Request and inputs

Owner rejects superseded dual-worker implementation in main. New target:
one x86 ntvwm.exe, nthook32/64.dll,16/32/64 execution and handoff, NTSRV
actual64 task identity and NTMON WIN64 display. Accepted baseline is
T431 S2 P2,12160c65725d986a59519f479458fbe35cb5a1f1.

## Procedure and results

An alternate Git index below build/M0-T432/S6/r001 captured all tracked and
untracked candidate files using read-tree/add/write-tree/commit-tree/update-ref.
Evidence branch evidence/t432-dual-worker-superseded-20261005 points to
3d5bc9466cf7208112ea53b9ff3ebf97534b0354. Snapshot/worktree comparison passed.
Main and normal index were not changed. Other-session proposal/TODO preserved.
Applied baseline reverse diffs to49 tracked src/tests/tools files; removed
four archived untracked candidate source/test/build files. All are recoverable
from the evidence branch. No destructive Git history reset was used.
`git diff --exit-code 12160c657 -- src tests tools` passed with empty output.
No build, process termination or publication occurred at this step.

## Interpretation and remaining gates

Baseline source identity is proved, new Hook64/WIN64 behavior is not. Archived
S2–S5 test successes cannot prove reconstruction; their native-zero failure
remains a non-pass. Next individually design/reimplement required shared
Hook/native metadata without width-selected workers, build under S6, run
matching/cross-width actual child, DOS/WOW, fault/isolation/product gates,
publish verified ten-image manifest and review/commit/push. T stays open.

## Other-session documentation audit and S6 P1

Owner requests joint audit/submission of the other-session proposal and TODO.
Proposal's2026-10-04 queue promotion is historical; T428 closure owns its
later admitted disposition. Correct the present-tense queue-head and old
active-packet claims, not implement the old inventory again.
TODO's20ms output capture and event-driven input description matches delivered
T429 S7/S8 and current ntvwm main's20ms-active/INFINITE-idle wait. The reported
mysmb16 performance issue remains unmeasured debt, not an attributed defect.
Removed Bochs obligations concern a retired provider; shell-out research has
delivered broker/Hook successors. Removed old ConPTY wheel debt concerns the
replaced backend; its strict historical failure remains indexed, not a claim
that horizontal-wheel capability now passes. No source behavior is changed.

Normalized src/tests/tools diff against accepted baseline and HEAD is empty.
Some transient Git status entries reflect stat/line-ending refresh, not source
changes. Commit must contain documentation only. Governance, relative links
and diff checks are required. S6 P1 records reconstruction/admission and joint
review; it does not close S6, prove Hook64, or replace the published package.
