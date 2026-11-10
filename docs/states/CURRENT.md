# Project Status

## Current Work

**Active: M0 T481 S12 — Console-window close error investigation.**
S11 research attributed intermediate execution to Win95 SHELL VxD; the owner
explicitly states the observed batch behavior is not a bug and requires no fix.
See [S11 evidence](../etc/operations/m0-t481-multitask-completion-plan.md#s11-current-baseline).

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S12, Ordinary Mode. |
| Admission And Approval | Owner ordered S12 to investigate the System Error shown when the host Console window is closed. |
| Objective | Reproduce and attribute the Console-window close error; repair an evidenced owned lifecycle or reporting defect. |
| Non-goals | Do not modify or rebuild original guest binaries, alter unrelated behavior, or use Computer Use. |
| Reference Baseline | S10 production package e09695f6-40f3-453d-8b96-e732d1050f4d; user observed broken-pipe System Error on X close. |
| Files And ABI Surface | NTSRV lifecycle, NTCON/worker-base frontend channel and NTVDM error reporting, as evidence requires; focused integration probes. |
| Applicable Rules | Execution, source policy, guest immutability, NTSRV lifecycle ownership, source-first and minimal implementation. |
| Verification | Reproduce actual console-window X close in an isolated Win95 session; distinguish orderly shutdown from unexpected channel failure; run focused and required Full gates for a runtime change. |
| Expected Markers | Identify actual error caller and ownership ordering; no misleading dialog on orderly close while genuine errors remain visible. |
| Asset Needs | Published package, authored released drivers, isolated Win95 copy and private probe infrastructure; remove owned O:/tmp copies. |
| Reporting Requirements | Explain the actual close sequence, root cause, test evidence, package identities and unverified boundaries. |
| Stop Conditions | Original guest mutation or unapproved observable semantic retreat. |
| Exit Criteria | Observed issue repaired or causally disposed; focused and applicable regressions passed; any runtime delivery built, published, committed and pushed. |
| Original Owner Request | Investigate the System Error after clicking the console X and make exit cleanup graceful. |
| Similar-Issue Sweep | Management close, worker exit, frontend loss and genuine transport failure. |

S13 retains the other multitask gaps, normal Win95 shutdown evidence and
integrated acceptance. The direct-start large inherited-environment boundary
remains recorded in [S9 direct startup](../etc/evidence/m0-t481-s9-direct-startup.md).

Known baseline: the Win95 dual-editor test uses guest USER synthetic mouse
events and therefore does not prove the owner's physical mouse route. The
published O:/WINNT/edit.exe and C:/Windows/System32/edit.exe have identical
SHA-256 `5F54A0FA8CBF5423AEF8BE8502173A59183CF3ADBC913212B1066EF8FFF3D9C0`.
An independent real hidden-console probe observed Edit input mode 0x288:
native MOUSE_EVENT records were ignored while the selected NTVWM VT mouse
conversion opened its File menu. This does not establish end-to-end delivery
through Win95, focus, geometry or the physical RDP route.

[S10 evidence](../etc/evidence/m0-t481-s10-edit-mouse.md) records the input-chain investigation.

S12 disposable workspace: `build/t481-s12`, owner S12; budget 8 hours,
20 GiB, 30-minute no-progress review. Stop and join owned processes, record
results, and clean task-specific O:/tmp copies before retirement. The owner
diagnostic package remains available for the close investigation.

## Current Technical Baseline

The published 11-image host package and installed W31 addon pair are
hash-verified. Win95 WIN.CMD points to the formal package. Original guest
binaries remain immutable. See [S9 evidence](../etc/evidence/m0-t481-s9-softpc.md)
for the 16 Full gates, skips, Win95 observer exit boundary and historical
mirror-guard test limitation.

## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure.
- [T479](../history/m0-t479-closure.md): guest multitask transferred to T481.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md) for the current governance.
