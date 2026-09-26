# Project Status

## Current Work

## Active Packet

**Active: M0 T423 S1**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T423 S1, Ordinary Mode, owner-approved restart. |
| Candidate Proposal | [Console/Window frontend](../proposals/proposal-kvm-window-graphics-presentation-001.md). |
| Admission And Approval | Owner authorizes reference backup, local main rollback and new S1. Remote history not rewritten. |
| Objective | Fix DOS/native CUI/native GUI lifecycle, exact task exit codes and launcher/Console remnants on baseline I/O; preserve original re-entry. |
| Non-goals | No hidden Console, display, Window or library import in S1; no guest or original scheduling change. |
| Reference Baseline | T422 closure 3821036ae. Old T423 snapshot 286d54a306bcb8e991891fae849b79db540e897a on codex/t423-original-reference-20260925. |
| Files And ABI Surface | run16 launch/wait, original BaseSrv task completion, worker lifecycle and Console attachments. New I/O transport belongs to S2. |
| Applicable Rules | Source-first, minimal mirror diff, immutable guest, EXECUTION gates. |
| Verification | Wait-chain evidence, cancellation and nested tests, CMD/Explorer exit, x86, DOS17 and WOW frontier non-regression; six-binary publication for production P. |
| Expected Markers | Actual output, scrolling, single input owner, exact exit code, CMD prompt recovery and no Explorer Console remnant. |
| Asset Needs | Existing OpenNT and immutable package media; no new libraries. |
| Reporting Requirements | Separate design/fixture/product evidence; exact wait-chain and Console attachment diagnosis. |
| Stop Conditions | Guest mutation, speculative scheduling, unauthenticated association, regression or unbounded dependencies. |
| Exit Criteria | All S1 proposal rows pass including launcher hang; tested deployment and synchronized delivery. |
| Original Owner Request | Preserve old T work, restart from prior T closure; latest six-stage plan separates lifecycle S1 from I/O migration S2. |
| Similar-Issue Sweep | Input ownership, output drain, waits/cancellation, task vs worker lifetime, residual Console attachments. |

All old commits and source/document/test WIP are preserved on the reference
branch. After rollback, src/tests/tools match 3821036ae; only latest planning,
queue and execution rules are carried forward. Old build caches and O:/winnt
remain untouched, not new-baseline acceptance. Implementation has not started.
Former T423 S1/S2 evidence belongs to reference-branch history, not this S1.

## Current Technical Baseline

Production source is T422 closure 3821036ae. New plan: S1 lifecycle, S2 DOS
I/O frontend migration, S3 hidden Console, S4 display/Window without Window
mouse, S5 Window mouse, S6 audit. No new production acceptance is claimed.

## Recent M0 Closures

Prior status chronology is retained in [restart evidence](../etc/evidence/m0-t423-restart-prior-status.md).
T422 remains closed; none of its historical packets is reopened.

## Recent Governance

Restart planning preserves prior governance in the linked evidence. Current
six-stage scope supersedes earlier presentation sequences; no runtime pass is claimed.
