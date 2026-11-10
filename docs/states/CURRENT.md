# Project Status

## Current Work

**Active: M0 T481 S11 — Win3.1/Win95 batch-launched PE32 investigation.**
S10 completed the owner's acceptance and full delivery; see
[S10 evidence](../etc/evidence/m0-t481-s10-edit-mouse.md).

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S11, Ordinary Mode. |
| Admission And Approval | Owner explicitly ordered batch startup as S11 and console close investigation as S12. |
| Objective | Reproduce and attribute batch-launched PE32 failures in Win3.1 and Win95 and repair the evidenced owned boundary without modifying original guest binaries. |
| Non-goals | Do not modify or rebuild original guest images; do not change unrelated guest or 486 behavior; do not use Computer Use. |
| Reference Baseline | Published package e09695f6-40f3-453d-8b96-e732d1050f4d; historical Win95 DOS EXEC evidence records CONAGENT DOS version mismatch and separate missing batch dispatch. |
| Files And ABI Surface | As evidence requires: original DOS execution and version handling, authored multitask bridge, owned host adapters, relevant tests and exception registration. |
| Applicable Rules | Execution, source policy, guest immutability, NTSRV lifecycle ownership, source-first and minimal implementation. |
| Verification | Reproduce both guest versions using isolated fixtures; distinguish DOS version handling, actual executable dispatch and output; run focused and applicable Full gates. |
| Expected Markers | Batch-launched native executables produce correct output and result in Win3.1 and Win95; ordinary DOS and interactive paths remain correct. |
| Asset Needs | Published host package, authored released drivers, existing Win95 copy and private probe infrastructure; O:/tmp task copies cleaned after use. |
| Reporting Requirements | Identify each actual failing call chain, supported behavior, exact published artifacts and unverified boundaries. |
| Stop Conditions | Original guest mutation or unapproved observable semantic retreat. |
| Exit Criteria | Reproduced defect repaired or causally disposed, focused and applicable regressions passed, reviewed coherent package published, committed and pushed; remaining acceptance handed onward. |
| Original Owner Request | Investigate Win3.1 and Win95 batch-launched PE32 separately, before the console close error. |
| Similar-Issue Sweep | Batch and interactive launch, internal commands, native CUI and guest DOS executables. |

S12 investigates the owner-observed pipe-disconnection dialog on console window
close. S13 retains
the other multitask gaps, normal Win95 shutdown
evidence and integrated acceptance. These obligations are not waived.
The previously observed direct-start large inherited-environment boundary
remains recorded in [S9 direct startup](../etc/evidence/m0-t481-s9-direct-startup.md);
no guest workaround is claimed.

Known baseline: the Win95 dual-editor test uses guest USER synthetic mouse
events and therefore does not prove the owner's physical mouse route. The
published O:/WINNT/edit.exe and C:/Windows/System32/edit.exe have identical
SHA-256 `5F54A0FA8CBF5423AEF8BE8502173A59183CF3ADBC913212B1066EF8FFF3D9C0`.
An independent real hidden-console probe observed Edit input mode 0x288:
native MOUSE_EVENT records were ignored while the selected NTVWM VT mouse
conversion opened its File menu. This does not establish end-to-end delivery
through Win95, focus, geometry or the physical RDP route.

[S10 evidence](../etc/evidence/m0-t481-s10-edit-mouse.md) records the input-chain investigation.

S11 disposable workspace: `build/t481-s11`, owner S11; budget 8 hours,
20 GiB, 30-minute no-progress review. Stop and join owned processes, record
results, and clean task-specific O:/tmp copies before retirement. The owner
diagnostic package remains available for the subsequent close investigation.

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
