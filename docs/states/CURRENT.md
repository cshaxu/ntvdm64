# Project Status

## Current Work

**Active: M0 T481 S10 — native EDIT.EXE mouse input in guest Win95.**
S9's mouse callback repair was owner-accepted, formally published, committed
and pushed; see [S9 closure](../etc/evidence/m0-t481-s9-closure.md).

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S10, Ordinary Mode. |
| Admission And Approval | Owner requested native EDIT.EXE window mouse failure be investigated after S9 formal delivery; automatic S continuation was previously approved. |
| Objective | Reproduce, attribute and repair Win95 windowed native EDIT.EXE physical mouse clicks, with a real matched test and minimal shared input handling. |
| Non-goals | Do not modify or rebuild original guest images; do not change unrelated guest or 486 behavior; do not use Computer Use. |
| Reference Baseline | Published package 90b4ec7f-bdf0-4011-8b2d-af156031f27d, commit 41fd60439, released multitask drivers; owner reports desktop and EDIT.COM clicks work but EDIT.EXE clicks do not. |
| Files And ABI Surface | As evidence requires: guest VM input bridge, NTVWM native input mapping, owned addon source, related tests and exception registration. |
| Applicable Rules | Execution, source policy, guest immutability, NTSRV lifecycle ownership, source-first and minimal implementation. |
| Verification | Contrast physical frontend and guest USER synthetic input; trace target VM, mouse records, child console mode and actual menu behavior; run focused, relevant Win3.1/Win95 and applicable Full gates. |
| Expected Markers | Real mouse click reaches and operates native EDIT.EXE in its own Win95 DOS window; EDIT.COM and multi-VM behavior remain correct, lifecycle is preserved. |
| Asset Needs | Published host package, authored released drivers, existing Win95 copy and private probe infrastructure; O:/tmp task copies cleaned after use. |
| Reporting Requirements | Distinguish raw host, guest USER and NTVDM/NTVWM routes; identify evidence, exact published artifacts and unverified boundaries. |
| Stop Conditions | Original guest mutation or unapproved observable semantic retreat. |
| Exit Criteria | Reproduced defect repaired or causally disposed, focused and applicable regressions passed, reviewed coherent package published, committed and pushed; remaining acceptance handed onward. |
| Original Owner Request | Publish S9 repair first, then investigate physical mouse clicks not working in windowed native EDIT.EXE. |
| Similar-Issue Sweep | Other native CUI mouse consumers, DOS EDIT.COM, Win3.1/Win95 VM focus and host/guest coordinate ownership. |

S11 retains the previously planned Win3.1 and Win95 batch-launched PE32
investigation. S12 retains the other multitask gaps, normal Win95 shutdown
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

S10 disposable workspace: `build/t481-s10`, owner S10; budget 8 hours,
20 GiB, 30-minute no-progress review. Stop and join owned processes, record
results, and clean task-specific O:/tmp copies before retirement. Preserve
the separate owner diagnostic package until its follow-up use is finished.

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
