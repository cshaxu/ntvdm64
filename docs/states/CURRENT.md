# Project Status

## Current Work

**Active: M0 T481 S1 — Integrate T480 mirror audit with T479 delivery.**
Owner closes T479 and directs all T480 commits, including guest mirror format
restoration, to be integrated, then built, tested, published, committed and
pushed. Original guest binaries remain unmodified and are not recompiled.
[T479 S9 delivery](../etc/evidence/m0-t479-s9-delivery.md) preserves its
qualified CHAIN2 package; [T479 S6 handoff](../etc/evidence/m0-t479-s6-closure-handoff.md)
and the [queued successor](../proposals/proposal-guest-multitask-io-completion-001.md)
retain unfinished guest multitasking obligations. [T480 closure](../history/m0-t480-closure.md)
records the upstream audit and its evidence limits.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | Owner-directed integration of T480. |
| Identifier Mode | M0 T481 S1, Ordinary Mode. |
| Admission And Approval | Owner explicitly directs closure, complete T480 integration and delivery. |
| Objective | Preserve both histories and verified S9 functionality; qualify and publish combined host package. |
| Non-goals | No original guest binary modification or recompilation; no unrelated feature work. |
| Reference Baseline | Main 151c01c1c and T480 a3dd057ee, common ancestor ab413287f. |
| Files And ABI Surface | T480 mirror restorations, affected build/tests/docs and eleven-image release. |
| Applicable Rules | Execution, source policy and mirror provenance. |
| Verification | Resolve conflicts; cached full builds, affected tests and Full gate; verify release hashes. |
| Expected Markers | S9 CHAIN2 remains intact, T480 selected changes retained, eleven coherent images. |
| Asset Needs | Existing build cache and test-owned guest media; no guest rebuild. |
| Reporting Requirements | Integration scope, conflicts, test results, release hashes and remaining limits. |
| Stop Conditions | Unproved semantic loss or need to modify guest binary. |
| Exit Criteria | Complete build, tests, coherent publication, review, commit and push; await acceptance. |
| Original Owner Request | Close T479, integrate all adjacent T480 results and deliver. |
| Similar-Issue Sweep | Audit overlapping mirror and S9 source changes and registrations. |
| Temporary Evidence | build/t481-integration, owned background runs and retained receipts. |

## Current Technical Baseline

S9 package 44d63565-f8b2-47a4-b79f-42d4b8465197 remains deployed; the combined T480 candidate is undergoing verification and is not yet published.


## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure and qualification limits.
- [T479](../history/m0-t479-closure.md): owner-directed closure; unfinished guest multitasking is preserved in the queued successor.

## Recent Governance

The integration remains in progress; combined release and validation are pending.
