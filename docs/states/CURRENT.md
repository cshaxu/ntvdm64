# Project Status

## Current Work

**Active: M0 T448 S12**, Ordinary Mode, single implementer/reviewer. T448 remains open.

## Active Packet

| Field | S12 reproducible host TODO remediation packet |
| --- | --- |
| Identifier Mode | M0 T448 S12, Ordinary Mode; one implementer and independent self-review. |
| Candidate Proposal | [NTSRV lifecycle authority](../proposals/proposal-ntsrv-lifecycle-authority-001.md). |
| Admission And Approval | Owner expands active S12 to fix all reproducible host-side TODO defects; S13 investigates the remaining not-necessarily-reproducible TODO cases. |
| Objective | Audit the complete TODO by actual owner and reproducibility; repair evidenced reproducible host defects, with original-source exception records and real regression evidence. Preserve original guest limitations. |
| Non-goals | No guest modification, rebuild or hot patch; no speculative fixes or unrelated lifecycle and WOW expansion. |
| Reference Baseline | Published S9 package; S10 commits e097bccb0 and 25a507a7d, with full-size SOL gameplay still unverified. |
| Files And ABI Surface | Original host providers and project-owned adapters as proven by each case; preserve guest binaries and existing ABI unless separately justified. |
| Applicable Rules | Reading set, execution, source policy, guest immutability, original-owner evidence, and documentation governance. |
| Verification | Revalidate each original failing witness and owner; prove repairs with positive, negative, lifecycle and actual guest paths, run affected regressions, and distinguish unavailable from nonreproducible. |
| Expected Markers | INT33 checksum, VGA cursor-disable, 8042 C0, redirector zero capacity and DEM deleted-name cases dispositioned with source, tests and mirror-diff accounting. |
| Asset Needs | Published package, immutable guest inputs, isolated desktop and independently authored disposable probes. |
| Reporting Requirements | Per TODO report classification, observed behavior, owner, fix or explicit remaining boundary and evidence. Disposable build/t448-s12 budget 2 GiB and two hours per run; inspect after ten minutes without progress. |
| Stop Conditions | Unapproved semantic change, guest modification, or evidence requiring materially broader implementation. |
| Exit Criteria | All confirmed reproducible host defects in the frozen inventory repaired and tested, or an explicit authority/prerequisite blocker reported; full appropriate build, release, review, commit and push; await owner acceptance. |
| Original Owner Request | Expand S12 to all reproducible host-side TODO repairs; use S13 for all not-necessarily-reproducible TODO investigation. |
| Similar-Issue Sweep | Distinguish original guest defects, original host defects, adapter defects and invalid test witnesses. |

[S11 investigation](../etc/evidence/m0-t448-s11-window-startup.md) reproduces
the already documented original COMMAND large-environment lifetime defect on
the current package. The specific post-prompt CAF report remains unverified;
no product fix or publication is claimed. Prior DOS/mouse S12 observations
are retained in that ledger, not treated as completed triage.

S12 candidate DIV-357–361 repairs the INT33 checksum, VGA cursor-disable,
8042 response, buffered directory-entry loss and redirector zero capacity.
Focused provider/guest/frame tests pass. The four-fix candidate passes DOS17
in both modes, WINMINE, SOL gameplay, the previous WRITE startup frontier and
three CAF self-delete/echo routes. DIV-361 full-package tests also pass;
WRITE editing/exit remain unverified. The 13-image five-fix checkpoint is
published and hash-verified; S12 remains open. DEM's deleted-name
fallback remains unresolved: the original indexed kernel IRP lacks a documented
user-mode equivalent. See the [evidence ledger](../etc/evidence/m0-t448-s11-window-startup.md)
for alternatives and limits; no new semantic boundary has been approved.

Earlier stages' complete status and chronology are preserved in the indexed
[pre-compaction snapshot](https://github.com/cshaxu/ntvdm64/blob/ba156ef61/docs/states/CURRENT.md).
It is historical, not an additional active packet. S10 full-size SOL gameplay
remains unverified and owner acceptance remains separate; S11's original
COMMAND environment limitation and unverified exact CAF report are retained.

## Planned S sequence

S1-S3 implementation and historical P9-P21 attempts are recorded in the
[S3 ledger](../etc/evidence/m0-t448-s3-close-implementation.md). Historical
ten-second deadline experiments there are superseded by the approved three
five-second policies; they are not current instructions or open S3 work.

1. S1: whole-system ownership audit and complete transition design.
2. S2: complete normal admission/result/handoff chain, including frontend policy and lifecycle-only WOW binding.
3. S3: unified management-close transactions, concurrency and role-specific service-loss cleanup.
4. S4: owner-added autonomous-code polling audit and cleanup; reuse existing events and completion notifications, justify retained polling, verify wakeup/exit/idle behavior and code delta.
5. S5: integrated fault/concurrency and real Console handoff acceptance, full release and closure audit (formerly S4).
6. S6: owner-added unused fallback/interface cleanup and NTMON change-only rendering with one-second sampling.
7. S7: SoftPC shared-library compatibility and remediation audit; bounded audit conclusion, no runtime import.
8. S8: pinned byte-exact public-library import and NTCON integration, full verification and publication.
9. S9: native Console title initialization and handoff, preserving app-set and restored titles.
10. S10: SOL environment-aware integration evidence and owner-directed TODO triage planning.
11. S11: investigate and repair reported command CAF and SOL window startup failures.
12. S12: audit all TODO and remediate reproduced host-side defects; preserve original guest boundaries.
13. S13: investigate and test not-necessarily-reproducible TODO cases with explicit dispositions and evidence.

The [proposal](../proposals/proposal-ntsrv-lifecycle-authority-001.md) owns the
finite inherited gap list. Cross-group changes must preserve a coherent working
protocol, not publish incompatible partial upgrades.

## S1 Closure Record

Design-only delivery: [source audit](../etc/evidence/m0-t448-s1-lifecycle-audit.md).

## S2 Closure Record

Bounded delivery, not owner functional acceptance: [S2 ledger](../etc/evidence/m0-t448-s2-lifecycle-implementation.md) and [S3 delivery](../etc/evidence/m0-t448-s3-close-implementation.md).

## S3 Closure Record

Delivered protocol 64 and bounded reconciliation: [S3 ledger](../etc/evidence/m0-t448-s3-close-implementation.md).

## S4 Closure Record

Polling audit and cleanup: [S4 ledger](../etc/evidence/m0-t448-s4-polling-audit.md).

## S5 Closure Record

Integrated acceptance with explicit limitations: [S5 ledger](../etc/evidence/m0-t448-s5-integrated-acceptance.md).

## S6 Closure Record

Owner accepted: [S6 ledger](../etc/evidence/m0-t448-s6-unused-monitor-cleanup.md).

## S7 Closure Record

Audit-only conclusion: [S7 ledger](../etc/evidence/m0-t448-s7-shared-library-import-audit.md).

## S8 Closure Record

Owner accepted functionality: [S8 ledger](../etc/evidence/m0-t448-s8-shared-library-import.md).

## S9 Closure Record

Owner approved closure: [S9 ledger](../etc/evidence/m0-t448-s9-console-title-handoff.md).

## S10 Closure Record

Bounded test-only delivery; full-size gameplay and owner acceptance remain separate: [S10 evidence](../etc/evidence/m0-t448-s9-console-title-handoff.md).

## S11 Closure Record

Owner reports normal testing; no causal repair claimed: [S11 ledger](../etc/evidence/m0-t448-s11-window-startup.md).

## Current Technical Baseline

The five-fix 13-image checkpoint was built, tested, published and hash-verified;
the [S12 ledger](../etc/evidence/m0-t448-s11-window-startup.md) and
[release manifest](../../assets/release/release-manifest.json) record its
identity, actual test coverage and limits. Guest binaries remain unchanged.
S12 is not closed and the unresolved directory-reset semantic choice is not
approval to change behavior.

## Recent M0 Closures

| Task | Disposition |
| --- | --- |
| T447 | [Owner-directed scope-transfer closure](../history/m0-t447-transfer-closure.md), not full functional acceptance. |

## Recent Governance

Prior governance evidence remains indexed in the preserved pre-closure status.
