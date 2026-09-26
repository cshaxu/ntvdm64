# Project Status

## Current Work

## Active Packet

**Active: M0 T423 S1**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T423 S1, Ordinary Mode, owner-approved restart. |
| Candidate Proposal | [Console/Window frontend](../proposals/proposal-kvm-window-graphics-presentation-001.md). |
| Admission And Approval | Owner authorizes reference backup, local main rollback and new S1. Remote history not rewritten. |
| Objective | Fix DOS/native CUI/native GUI lifecycle, exact task exit codes and launcher/Console remnants on baseline I/O; preserve original re-entry. Owner additionally requests the bounded dtmgr EDIT-style title, frame and scrollbars. |
| Non-goals | No hidden Console, display, Window or library import in S1; no guest or original scheduling change. |
| Reference Baseline | T422 closure 3821036ae. Old T423 snapshot 286d54a306bcb8e991891fae849b79db540e897a on codex/t423-original-reference-20260925. |
| Files And ABI Surface | run16 launch/wait, original BaseSrv task completion, worker lifecycle and Console attachments; owner-requested dtmgr presentation in src/dtmgr-exe/main.c and its focused Console fixture, with no broker ABI change. New I/O transport belongs to S2. |
| Applicable Rules | Source-first, minimal mirror diff, immutable guest, EXECUTION gates. |
| Verification | Original matrix retained as evidence; owner exception 2026-09-25 accepts successful formal x86 build for this current S1 verification. Untested rows are waived, never passed; six-binary publication remains required. |
| Expected Markers | Actual output, scrolling, single input owner, exact exit code, CMD prompt recovery and no Explorer Console remnant. |
| Asset Needs | Existing OpenNT and immutable package media; no new libraries. |
| Reporting Requirements | Separate design/fixture/product evidence; exact wait-chain and Console attachment diagnosis. |
| Stop Conditions | Guest mutation, speculative scheduling, unauthenticated association, regression or unbounded dependencies. |
| Exit Criteria | Current verification concludes under the bounded owner build-only exception; local commit and coherent deployment. P synchronization remains deferred pending explicit remote-history rewrite authority. This does not close T423 or waive future S gates. |
| Original Owner Request | Preserve old T work, restart from prior T closure; latest six-stage plan separates lifecycle S1 from I/O migration S2. |
| Similar-Issue Sweep | Input ownership, output drain, waits/cancellation, task vs worker lifetime, residual Console attachments. |

All old commits and source/document/test WIP are preserved on the reference
branch. At rollback, src/tests/tools matched 3821036ae; only latest planning,
queue and execution rules are carried forward. Old build caches and O:/winnt
remained untouched, not new-baseline acceptance. That restart record preceded implementation.
Former T423 S1/S2 evidence belongs to reference-branch history, not this S1.

Fresh x86 six-file baseline builds; DOS17 ordinary-runtime text/exit cases pass.
The new-Console lifecycle correction is now in run16: distinguish a sole
launcher from a retained external Console caller and use original DosSessionId
retirement. Independent MEM/COMMAND and native GUI/CUI lifetime checks pass;
broker/worker/launcher loss tests pass with two-worker isolation and real MEM
recovery output. Before/after evidence confirms natural worker exit;
real native Ctrl+C/Break tests exposed premature launcher termination and now
pass after a non-inherited launcher handler preserves the child completion wait.
The updated six-file candidate is deployed; post-change DOS17 passes all 17 cases.
S1's current verification is concluded under the owner's explicit build-only
exception; see [lifecycle evidence](../etc/evidence/m0-t423-s1-restart-lifecycle.md).
O:/winnt holds the coherent build for side testing. Unverified WOW interaction
is waived for this delivery, not a functional pass. VS build environment
startup failures remain separate from normal runtime tests. S1 implementation
is bounded-concluded; this packet remains the delivery owner pending remote
history disposition. S2 has not started.

Owner's current layout request admits the bounded dtmgr presentation correction
within this packet: independent title row, table starting on row two, vertical
and horizontal scrollbars replacing the body right and bottom edges. The
[EDIT-style evidence](../etc/evidence/dtmgr-edit-style.md) records reference
characters and focused verification. Work root: build/M0-T423/S1/dtmgr-edit-style.
The dtmgr x86 build and private Console-buffer layout fixture pass. The owner's
follow-up moves the up arrow to row four and the initial thumb to row five;
the tested correction is deployed to O:/winnt/dtmgr.exe. The owner permits
including this side-session change in the S1 commit. The current owner
exception, not this layout change, permits bounded closure.

## Current Technical Baseline

Production source is T422 closure 3821036ae. New plan: S1 lifecycle, S2 DOS
I/O frontend migration, S3 hidden Console, S4 display/Window without Window
mouse, S5 Window mouse, S6 audit. S1 adds only the documented launcher and
dtmgr corrections; no Window/frontend migration acceptance is claimed.

## Recent M0 Closures

Prior status chronology is retained in [restart evidence](../etc/evidence/m0-t423-restart-prior-status.md).
T422 remains closed; none of its historical packets is reopened.

## Recent Governance

Restart planning preserves prior governance in the linked evidence. Current
six-stage scope supersedes earlier presentation sequences; no runtime pass is claimed.
