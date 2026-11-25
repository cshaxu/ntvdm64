# Project Status

## Current Work

**Active: M0 T485 S1** (Ordinary Mode). Windows XP NTVDM baseline upgrade.
S1 establishes the component inventory, candidate provenance and feasibility. The [ordered migration ledger](../etc/evidence/t485-s1/README.md) covers existing components. XP-only additions pass to the next T unless required by an existing batch.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S1, Ordinary Mode. |
| Original Owner Request | 好 准入下一个t任务 |
| Admission And Approval | The owner approved admission of the queue head after T484 closure. S1 is a read-only inventory and feasibility review. |
| Candidate Proposal | [Windows XP NTVDM baseline upgrade](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md). |
| Objective | Map the current host and every guest component to the candidate XP NTVDM edition and build. Identify available inputs, provenance, interface differences, and blockers, then propose a coherent migration and verification sequence. |
| Reference Baseline | T484 S7 P1 `f7e3b2e09`, eleven host images in package `0b82f7de-640f-43e1-962e-73f904c0bf60`; [T484 closure](../history/m0-t484-closure.md) and [Win95 restart handoff](../history/m0-t482-closure.md). |
| Files And ABI Surface | Current host and guest manifests, source ownership, build and runtime bindings, guest/host audio, DOS LFN and Win95 IFS, DOS return lifecycle. S1 records evidence in documentation. |
| Applicable Rules | [Source policy](../etc/operations/policy/source-policy.md), [architecture](../rules/ARCHITECTURE.md) and [execution](../rules/EXECUTION.md). Preserve guest media and existing CMD/BAT build ownership. |
| Verification | Match the T484 package and repository manifests. Identify each candidate's edition, build, architecture, provenance and availability, and link each mapped interface or gap to source or artifact evidence. |
| Expected Markers | Current-to-XP component matrix, explicit missing or unapproved inputs, and a staged migration plan with acceptance and rollback requirements. |
| Asset Needs | Current verified package and guest inventories, plus authorized XP source and media references. Record unavailable inputs explicitly. |
| Non-goals | No source import, guest replacement, production code change, publication, or release migration during S1. |
| Similar-Issue Sweep | Distinguish XP DOS LFN support from the Win95 Shell's IFS requirement, and keep Win95 restart filesystem and downstream DOS return as separate obligations. |
| Reporting Requirements | Record scope by component, ownership, candidate identity, known gaps, and evidence needed for the next S. |
| Stop Conditions | Pause acquisition or integration if provenance, rights, guest immutability or compatibility require an explicit decision. |
| Exit Criteria | Complete the inventory, state which candidate can be qualified and why, and record verified acceptance requirements and the next proposed S. |

## Recent M0 Closures

- [T484](../history/m0-t484-closure.md).
- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Current Technical Baseline

The T484 S7 package in the active packet remains the runtime baseline. T485 S1 P1 catalogues 159 migration batches and 3,986 tracked files.

## Recent Governance

Existing-component batches can include a required provider from another component. XP-only additions with independent scope are reserved for the next T.
