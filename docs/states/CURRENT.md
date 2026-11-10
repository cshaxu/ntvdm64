# Project Status

## Current Work

**Active: M0 T481 S14 — Win95 launch multitask selection.**
S12 was closed by owner decision after the reported close dialog did not recur
in bounded probes; its root cause remains unlocated. See
[S12 evidence](../etc/operations/m0-t481-multitask-completion-plan.md#s12-preliminary-source-observations).

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S14, Ordinary Mode. |
| Admission And Approval | Owner approved Win3.1 and Win95 multitask patch packages and prioritized S13 and S14. |
| Objective | Add DOSONLY=0 / 1 selection to Win95 launch preparation, configure matching launch profiles and install the released multitask drivers for DOSONLY=0. |
| Non-goals | Do not alter or rebuild original guest binaries; do not use Computer Use; do not change unrelated product behavior. |
| Reference Baseline | S13 production package c3bc700e-d623-4059-a47a-0f69641fc7dd; existing Win95 launch package and released driver pair. |
| Files And ABI Surface | tools/win95-launch, shared multitask installer as necessary, applicable addon tools, integration tests, current task records. |
| Applicable Rules | Execution, source policy, guest immutability, source-first and minimal implementation; patch packages use CMD and existing addon tools only. |
| Verification | Validate both modes and matching launch configuration, real Win95 launch and guest DOS execution, host payload behavior for mode 0 and guest-only behavior for mode 1. |
| Expected Markers | Both profiles select the intended mode; DOSONLY=0 installs and configures both released drivers; original guest binaries remain unchanged except previously approved package adaptations. |
| Asset Needs | Released addon tools and drivers, isolated Win95 installation and test facilities; clean task-owned O:/tmp copies. |
| Reporting Requirements | Exact package changes, both mode results, hashes and unverified boundaries; retain integrated acceptance obligations. |
| Stop Conditions | Original guest mutation or unapproved observable semantic retreat. |
| Exit Criteria | Required changes and acceptance completed; coherent package built, tested, published, committed and pushed as applicable. |
| Original Owner Request | Offer the same DOSONLY choice and multitask driver setup for Win95 launch. |
| Similar-Issue Sweep | Win31 setup, shared installer and generated Win95 profiles. |

S13 retains the other multitask gaps, normal Win95 shutdown evidence and
integrated acceptance. The direct-start large inherited-environment boundary
remains recorded in [S9 direct startup](../etc/evidence/m0-t481-s9-direct-startup.md).

S13 implementation, four-mode runtime checks and Full delivery are complete.
The post-Setup probe uses a Setup executable shim; a fresh original interactive
Setup run was not repeated. The unchanged original launch and failure-exit
contract was separately tested. See [S13 evidence](../etc/evidence/m0-t481-s13-win31-setup.md).

S14 enhances `tools/win95-launch` with the same DOSONLY=0 / DOSONLY=1
selection and matching launch configuration. For DOSONLY=0, install the
released `insignia.386` and `ntvdm64.386` in the installed Win95 `PATCH`
directory and configure the existing Win95 installation for multitask
payloads. Validate both choices, including launch, guest DOS execution and
the intended host-payload behavior, while preserving original guest binaries.

S14 implementation and complete host package are published for owner
acceptance. Both choices were exercised in real isolated Win95 copies; the
guest-only fixture terminates its own session after the DOS witness and does
not establish normal Win95 shutdown. The original interactive Win3.1 Setup
was not rerun. See [S14 evidence](../etc/evidence/m0-t481-s14-win95-launch.md).

Known baseline: the Win95 dual-editor test uses guest USER synthetic mouse
events and therefore does not prove the owner's physical mouse route. The
published O:/WINNT/edit.exe and C:/Windows/System32/edit.exe have identical
SHA-256 `5F54A0FA8CBF5423AEF8BE8502173A59183CF3ADBC913212B1066EF8FFF3D9C0`.
An independent real hidden-console probe observed Edit input mode 0x288:
native MOUSE_EVENT records were ignored while the selected NTVWM VT mouse
conversion opened its File menu. This does not establish end-to-end delivery
through Win95, focus, geometry or the physical RDP route.

[S10 evidence](../etc/evidence/m0-t481-s10-edit-mouse.md) records the input-chain investigation.

S14 disposable workspace: `build/t481-s14`, owner S14; budget 8 hours,
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
