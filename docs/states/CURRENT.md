# Project Status

## Current Work

**Active: M0 T482 S1 — Win95 restart-to-DOS root cause and minimal repair.**

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T482 S1, Ordinary Mode. |
| Admission And Approval | Owner admits the next queued task, asks the agent to set a goal, establish a clear root cause and complete the repair. |
| Objective | Reproduce Restart in MS-DOS mode, prove the requested transition and first failing owner, then repair the project-owned seam and demonstrate usable DOS return. |
| Non-goals | No original guest modification/rebuild, unrelated CPU fixes, identity redesign or changes to Win31/Win101 setup. No Computer Use. |
| Reference Baseline | 782854d6f; T481 closed; published package 7773a9b4-e360-4469-962a-df26bb16faf4. |
| Files And ABI Surface | Existing private-desktop Win95 probes, reached shutdown/return host or addon seam; exact implementation scope follows source/trace attribution. |
| Applicable Rules | Source-first recovery, minimal mirror diff, central NTSRV lifecycle, immutable guest, owned hidden background runs. |
| Verification | Actual requested shutdown option, normal shutdown control, DOS input/commands and restart; focused regression and applicable Full; all production deliveries cached-built, published, committed and pushed. |
| Expected Markers | Request identity, reached caller/provider, failure/control comparison, correct DOS interactive return and preserved lifecycle; no forced success or exit-code normalization. |
| Asset Needs | Existing released assets and isolated copy of approved O:/WINDOWS; original media unchanged. |
| Reporting Requirements | Root cause, minimal change, exact runtime evidence and unsupported boundaries; periodic progress. |
| Stop Conditions | Guest mutation or unknown semantic replacement requires explicit owner review, not guessed fallback. |
| Exit Criteria | Reproduced failure explained and repaired with positive/control coverage and complete delivery; an immutable boundary must be proved and reviewed, not reported as repaired. |
| Original Owner Request | Admit a new T, set the goal autonomously, make root cause clear and fix thoroughly. |
| Similar-Issue Sweep | Normal shutdown versus DOS return versus reboot, driver/DOSONLY state, caller identity, I/O and worker retirement. |

Plan: [bounded investigation and acceptance](../etc/operations/m0-t482-win95-dos-restart-plan.md).
Scratch: build/t482; owned processes and disposable guest copies only; 8-hour,
20-GiB budget, review after 30 minutes without new evidence. Reuse historical
successful probe/token paths before declaring any capability unavailable.

## S1 Evidence Checkpoint

Actual Win95 volume capability is 12-character components/no LFN. Localized
Exit-to-DOS PIF and ASCII long paths fail before DEM CreateFileOem; short paths
reach DEM and succeed. A short-PIF diagnostic reaches session-end notifications,
but usable DOS return is not verified. Evidence is indexed under T482.
A proper Win95 filesystem-provider addition is a scope expansion, not a minimal
NTSRV exit fix; owner choice between design review and gap transfer is pending.
P1 investigation is committed/pushed as 3508c115e. Additional companion-off
controls also reach session-end notifications without verified DOS return;
there is no evidence for a one-driver exit fix. The plan contains the bounded
provider/transition decision brief. No implementation expansion is admitted.
No product or original guest changes, publication, Full pass or repair claimed.

## Current Technical Baseline

Published package: 7773a9b4-e360-4469-962a-df26bb16faf4, all 11 images
hash-verified in assets/release and O:/winnt/system32. S20 cached x86/x64
builds and focused tests passed. S21 repeated cached builds, focused launcher
tests and all 16 selected Full gates successfully. External
DOS4GW/NESticle is a missing-fixture skip. S20 P1 is 843cae6b4.
This closure changes Win95 tooling, tests and documentation; runtime, addons, original guest,
winnt.zip and installed Windows configurations are unchanged.

The confirmed DOS program identity/wrong-waiter defect and approved Win95
exit-code boundary are retained in [TODO](TODO.md), not declared repaired.

## Recent M0 Closures

- [T481](../history/m0-t481-closure.md): guest multitask delivery, launch profiles, mouse repairs and test cleanup; explicit retained boundaries.
- [T480](../history/m0-t480-closure.md): adjacent mirror audit closure.
- [T479](../history/m0-t479-closure.md): guest multitask transferred to T481.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md) for current governance.
