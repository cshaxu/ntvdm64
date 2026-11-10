# Project Status

## Current Work

**Active: M0 T481 S13 — Win3.1 setup and remaining multitask acceptance.**
S12 was closed by owner decision after the reported close dialog did not recur
in bounded probes; its root cause remains unlocated. See
[S12 evidence](../etc/operations/m0-t481-multitask-completion-plan.md#s12-preliminary-source-observations).

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S13, Ordinary Mode. |
| Admission And Approval | Owner accepted S12 closure and prioritized S13 and S14; the owner directed the Win3.1 setup enhancement and retained multitask acceptance. |
| Objective | Update Win3.1 setup to select DOSONLY=0 or 1, generate matching launch profiles and configure released multitask drivers for DOSONLY=0; complete remaining admitted multitask acceptance. |
| Non-goals | Do not alter or rebuild original guest binaries; do not use Computer Use; do not change unrelated product behavior. |
| Reference Baseline | S10 production package e09695f6-40f3-453d-8b96-e732d1050f4d; original DOS and GUI startup profiles and released driver pair. |
| Files And ABI Surface | tools/win31-setup, applicable existing addon tools, launch-profile and integration tests, current task records. |
| Applicable Rules | Execution, source policy, guest immutability, source-first and minimal implementation; patch packages use CMD and existing addon tools only. |
| Verification | Validate DOSONLY=0 and 1 for Standard and 386 Enhanced profiles, installation and launch; verify released driver staging and applicable multitask lifecycle and I/O acceptance. |
| Expected Markers | Four profiles select their intended mode; DOSONLY=0 installs and configures both released drivers; original guest binaries remain unchanged. |
| Asset Needs | Released addon tools and drivers, isolated Windows 3.1 installations and real test facilities; clean task-owned O:/tmp copies. |
| Reporting Requirements | Exact setup changes, all four case results, hashes and unverified boundaries; retain integrated acceptance obligations. |
| Stop Conditions | Original guest mutation or unapproved observable semantic retreat. |
| Exit Criteria | Required changes and acceptance completed; coherent package built, tested, published, committed and pushed as applicable. |
| Original Owner Request | Select DOSONLY at end of setup, generate matching profiles, install the two released multitask drivers for DOSONLY=0, and test four combinations. |
| Similar-Issue Sweep | Win31 setup and generated profiles; Win95 counterpart remains S14. |

S13 retains the other multitask gaps, normal Win95 shutdown evidence and
integrated acceptance. The direct-start large inherited-environment boundary
remains recorded in [S9 direct startup](../etc/evidence/m0-t481-s9-direct-startup.md).

S13 setup implementation and its coherent host package have passed the
recorded four-mode and Full checks and been published. The four-mode probe
uses a Setup executable shim; a fresh original interactive Setup run is not
yet established by that evidence. See [S13 evidence](../etc/evidence/m0-t481-s13-win31-setup.md).

S14 enhances `tools/win95-launch` with the same DOSONLY=0 / DOSONLY=1
selection and matching launch configuration. For DOSONLY=0, install the
released `insignia.386` and `ntvdm64.386` in the installed Win95 `PATCH`
directory and configure the existing Win95 installation for multitask
payloads. Validate both choices, including launch, guest DOS execution and
the intended host-payload behavior, while preserving original guest binaries.

Known baseline: the Win95 dual-editor test uses guest USER synthetic mouse
events and therefore does not prove the owner's physical mouse route. The
published O:/WINNT/edit.exe and C:/Windows/System32/edit.exe have identical
SHA-256 `5F54A0FA8CBF5423AEF8BE8502173A59183CF3ADBC913212B1066EF8FFF3D9C0`.
An independent real hidden-console probe observed Edit input mode 0x288:
native MOUSE_EVENT records were ignored while the selected NTVWM VT mouse
conversion opened its File menu. This does not establish end-to-end delivery
through Win95, focus, geometry or the physical RDP route.

[S10 evidence](../etc/evidence/m0-t481-s10-edit-mouse.md) records the input-chain investigation.

S13 disposable workspace: `build/t481-s13`, owner S13; budget 8 hours,
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
