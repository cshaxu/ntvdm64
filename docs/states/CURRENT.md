# Project Status

## Current Work

**Active: M0 T481 S9 — SoftPC software-emulation imports and Win95 checks.**
The owner closed S8 and directed S9 to proceed with the applicable SoftPC
repairs and Win95 investigation. The S8 delivery and its unverified repeated
switch boundary are recorded in the [S8 closure](../etc/evidence/m0-t481-s8-closure.md).
S10 retains final Win95 and multitask acceptance; the T remains open.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S9, Ordinary Mode. |
| Admission And Approval | Owner closed S8 and approved S9 SoftPC imports and Win95 repair investigation. |
| Objective | Audit recent SoftPC software-emulation repairs, import applicable minimal changes, and investigate and repair demonstrated Win95 multitask defects. |
| Non-goals | Do not modify or rebuild original guest images; do not import unrelated changes or claim unobserved bugs. |
| Reference Baseline | Published host package 95d80e95-adb7-456e-b82c-e9932054909f and released addon pair; Win3.1 repeated-switch and Win95 owner acceptance remain unverified. |
| Files And ABI Surface | Selected CCPU, C-VID, video and related host adapters; corresponding tests and exception records. |
| Applicable Rules | Execution, source policy, guest immutability and original-semantics-first. |
| Verification | Trace original and SoftPC callers; run matched positive and negative tests, cached builds, Full and relevant Win3.1 and Win95 regressions. |
| Expected Markers | Each upstream repair has applicability, provenance, selected caller and actual test or explicit boundary. |
| Asset Needs | Adjacent SoftPC source, existing isolated guest copies, complete host package and addon drivers. |
| Reporting Requirements | Separate actual failures, imported changes, tests, publication and unverified boundaries. |
| Stop Conditions | Original guest mutation or unapproved observable semantic retreat. |
| Exit Criteria | Applicable fixes reviewed, built, tested, fully published, recorded, committed and pushed; remaining S10 acceptance explicitly handed off. |
| Original Owner Request | Close S8; start S9 SoftPC software-emulation fixes and Win95 repair. |
| Similar-Issue Sweep | Check selected CCPU, BIOS, C-VID and video callers and upstream dependencies. |

S9 disposable workspace: `build/t481-s9`, owned by this packet; budget 8 hours,
20 GiB, with a 30-minute no-progress review. Join owned processes and record
results before cleanup; reusable fixtures remain in `tests/mvdm-host`.
[S9 evidence](../etc/evidence/m0-t481-s9-softpc.md) records candidate results.
The owner accepted S8, but rejected the current delivery after direct Win3.1 startup failed;
the reproduced inherited-environment guest defect and passing isolated
short-PATH control are recorded in [direct startup](../etc/evidence/m0-t481-s9-direct-startup.md).
The owner additionally reports Win+R direct `run16 command` fails, while a
CMD-launched chain can start Win3.1. Investigate this as a direct Run16/DOS
startup issue, not solely as a Win3.1 launcher issue. S9 acceptance remains failed.

## Current Technical Baseline

The 11-image host candidate and W31 addon pair are published and hash-verified.
Win95 manual acceptance and the repeated Win3.1 CAF/fullscreen sequence remain
outstanding; neither is implied by the owner's S8 closure.

## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure.
- [T479](../history/m0-t479-closure.md): guest multitask transferred to T481.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md) for the current governance.
