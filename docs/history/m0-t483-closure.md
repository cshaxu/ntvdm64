# M0 T483 closure: mirror minimization and component ownership

## Owner decision and scope

The owner authorized checking and closing this T on 2026-11-22 after S20.
S21 P1 is the bounded documentation-only final reconciliation. T483 is closed;
no next T or production S is admitted. This closure accepts completed review
and delivery scopes, not blanket approval of every remaining mirror difference
or a claim that every known product defect has been repaired.

## Delivered scope and evidence

| Subtask | Bounded result | Record |
| --- | --- | --- |
| S1 | Initial ordered mirror classification | [Evidence](../etc/evidence/m0-t483-s1-closure.md) |
| S2 | Original restoration category | [Evidence](../etc/evidence/m0-t483-s2-closure.md) |
| S3 | Delete category | [Evidence](../etc/evidence/m0-t483-s3-closure.md) |
| S4 | Minimize category | [Evidence](../etc/evidence/m0-t483-s4-closure.md) |
| S5 | Original-first re-audit and restoration | [Evidence](../etc/evidence/m0-t483-s5-closure.md) |
| S6 | Fragmentation and low-use mirror work | [Evidence](../etc/evidence/m0-t483-s6-closure.md) |
| S7 | Remaining file review | [Evidence](../etc/evidence/m0-t483-s7-closure.md) |
| S8 | Final sequential mirror pass | [Evidence](../etc/evidence/m0-t483-s8-closure.md) |
| S9 | Compat simplification | [Evidence](../../artifacts/t483-s9-non-mirror-audit/sequential.md) |
| S10 | Frontend-base promotion and Hook rename | [Evidence](../etc/evidence/m0-t483-s10-components.md) |
| S11 | A/B layer cleanup baseline | [Evidence](../etc/evidence/m0-t483-s11-layer-boundaries.md) |
| S12 | C dependency inventory and in-place cleanup | [Evidence](../etc/evidence/m0-t483-s12-closure.md) |
| S13 | Physical mapping implementation descent | [Evidence](../etc/evidence/m0-t483-s13-implementation-closure.md) |
| S14 | C outward implementation and callback cleanup | [Evidence](../etc/evidence/m0-t483-s14-closure.md) |
| S15 | D outward dependency cleanup | [Evidence](../etc/evidence/m0-t483-s15-closure.md) |
| S16 | E outward/sibling dependency review | [Evidence](../etc/evidence/m0-t483-s16-closure.md) |
| S17 | F sibling dependency cleanup | [Evidence](../etc/evidence/m0-t483-s17-closure.md) |
| S18 | Original C1/C2 connections | [Evidence](../etc/evidence/m0-t483-s18-closure.md) |
| S19 | B-to-C2 private boundary removal | [Evidence](../etc/evidence/m0-t483-s19-closure.md) |
| S20 | B-to-C1 private boundary removal | [Evidence](../etc/evidence/m0-t483-s20-closure.md) |
| S21 | Closure identity, evidence and residual reconciliation; no product change | [Verification](../etc/evidence/t483-closure/result.json) |

S9 is recorded in its complete 40-file sequential ledger and P9 commit
55643b6c5; S10's accepted admission and delivery follow it. It does not need
a fabricated historical runtime run or a new production fix to close its index.
Each earlier receipt retains its own verification limits and superseding owner
decisions. S12/S13 open allocations were consumed by S14 and subsequent work;
the old planning tables are historical rather than outstanding S assignments.

The S8 mirror snapshot was 229 files with non-error differences, 1,903 blocks
and 26,243 changed lines. Those are historical S8 metrics, not a current census
after later component ownership changes. No new global mirror-hunk audit was
performed for this documentation-only closure.

Final dependency results: S19 completed38/38 B-to-C2 files; S20 completed94/94
B-to-C1 files. Private compiled headers/search roots and live private linked
dependencies are zero. Earlier A/D/E/F audits retain their bounded conclusions.
B-owned synchronous callbacks are explicit permitted runtime relationships.
C1/C2 retain five original WOW APIs/seven relocation rows; original named DLL
imports remain permitted. Four B-path header carriers are used only by C source
assembly; their five textual C includes are not B translation-unit dependencies.
No claim that all runtime relationships disappeared is made.

## Final production delivery reused

Product revision: `9e7eb19c059a3ba8e55c73c6943842416932b8d2` (S20 P85).
Package: `100b16d9-992d-4e2c-8d99-66a4e891dd26`.
Cached full product build, both11-image publications, six provider/lifetime
fixtures, real USER16/COMMAND/EDIT/CMD/MEM and Full20/20 passed in P85.
The Full receipt is scoped to complete Windows Userenv and retains the failed
inherited-agent-environment attempt and P84 comparison.

S21 rechecked all22 installed hashes, recorded modified-source hashes, the
unchanged source/test/build/asset tree against P85 and the20 passing gate rows.
No rebuild, redeployment or repeated runtime test was needed or claimed.
Documentation/link gates and diff checks are the S21 delivery requirements.
The containing S21 P1 commit and its push complete T483 closure.

Non-error mirror additions approved for ownership: S19+12points/+574lines,
S20+15points/+815lines, including P85+4points/+244lines. These are per-S deltas,
not a recomputed whole-T net metric. Original guest/media were not changed by
this closure; source and mirror policy remains unchanged.

## Retained decisions and boundaries

- The [pending mirror ledger](../../artifacts/mvdm-audit/t483-remaining-reaudit/pending-decisions.csv)
  has1,583 rows and1,563 distinct ID strings. One grouped userk.h identifier
  has21 distinct subcases; composite ID/path/problem keys are unique. These
  rows include approved adaptations retained for revisit and unapproved
  retention opinions. Rows are not current hunk counts. Closing T483 does not
  approve all retention opinions. The [main register](../../artifacts/mvdm-audit/t483-remaining-reaudit/review-register.csv)
  retains2,657 unique rows; both files remain unchanged by this closure.
- The known large-environment COMMAND startup boundary remains in
  [TODO](../states/TODO.md). P85's inherited-environment comparison is not a new
  attribution of the exact corrupting write and does not fix this boundary.
- The native Shell catch-and-resume diagnostic fails in Windows.Storage even
  without the adapter; ordinary guest Shell/outer-unwind controls pass. Preserve
  this diagnostic boundary in TODO, without declaring an adapter bug fixed.
- The odd-target sevid019.c full-copy oracle failure remains separately in
  TODO. The historical56-case probe has28 failures; no new product symptom,
  current reproduction or fix is claimed here.
- The repository-wide redundant-queue/state-machine review remains queue11.
  S14 removed eight artificial handoffs in its admitted mechanism and kept one
  necessary Console cleanup transfer; this is not completion of that broader T.
- Other inherited DOS identity, Win95 return-code and immutable-guest boundaries
  remain in TODO with their original authorities. They are not silently waived.

The existing queue order is unchanged. Its first candidate remains unified
tools/build.cmd construction and publication; admission needs the next owner
instruction. The old mirror proposal and implementation plan are archived as
historical scope/evidence, not live competing status or additional queued work.
