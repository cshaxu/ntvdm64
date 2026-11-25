# Project Status

## Current Work

**Active: M0 T485 S2** (Ordinary Mode). Migrate H001-H008 in directory order. [S1 inventory and feasibility](../etc/evidence/t485-s1/README.md) is complete. Production migration stands at 0/159 batches.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S2, Ordinary Mode. |
| Original Owner Request | Complete the T485 plan autonomously. Use local repos.external sources and the supplied XP installation discs. SP2 or SP3 binaries are acceptable. |
| Admission And Approval | Owner approved the S plan and continuous execution. The local XP tree is approved for project use on the OpenNT basis. |
| Candidate Proposal | [T485 Windows XP baseline](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md). |
| Objective | Reconcile H001-H008 against both upstream revisions, implement necessary host adaptations and verify the resulting guest/host contracts. Start with H001 debugger dispatch and its required provider. |
| Non-goals | Independent new XP components belong to the next T. No guest rebuild, unapproved winnt.zip additions or publication of XP payloads lacking qualified distribution scope. |
| Reference Baseline | [T485 plan](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md), [S1 inventory](../etc/evidence/t485-s1/README.md), eleven-image T484 rollback package. |
| Files And ABI Surface | H001-H008, related build inputs and consumers. H001 includes its necessary NTVDMD interface. H002 and H005 track COMMAND and DOSX counterparts. |
| Applicable Rules | [Source policy](../etc/operations/policy/source-policy.md), [architecture](../rules/ARCHITECTURE.md), [execution](../rules/EXECUTION.md). |
| Verification | Compare old/local/XP, resolve directory membership, compile supported product and verify the actual product caller, guest path, failure and lifecycle. Inspect image identity, exports and imports. |
| Expected Markers | H001-H008 per-file dispositions, matched provider ABI, product verification and rollback receipts. |
| Asset Needs | Owner-supplied local XP source candidate 5.1.2600.1106, verified English x86 Professional SP3 binary reference and T484 baseline. |
| Reporting Requirements | Count completed batches and files once, record early companion paths and distinguish local evidence from published delivery. |
| Stop Conditions | Unresolved provenance for distribution, incompatible ABI, guest immutability conflict or failed product gates block the affected delivery. |
| Similar-Issue Sweep | Check dynamic imports and reverse EXE imports, initial dispatch before debugger attach, and setup-file moves into child directories. |
| Exit Criteria | Reconcile all eight batches, demonstrate retained behavior, verify allowed package delivery and resolve or explicitly escalate outstanding release boundaries. |

## S1 Closure Record

[Inventory, media qualification, H001 ABI evidence and rollback](../etc/evidence/t485-s1/README.md). The selected English Professional SP3 disc supplies 61 direct binary matches. Source and media span different service packs. Product migration and runtime verification begin in S2.

## Recent M0 Closures

- [T484](../history/m0-t484-closure.md).
- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Current Technical Baseline

The T484 S7 package in the active packet remains the runtime baseline. T485 S1 catalogues 159 batches and 3,986 tracked files. P4 verifies eleven rollback images at both locations. The candidate declares 5.1.2600.1106. Its README reports leaked provenance. The owner approved local source use on the OpenNT basis. The owner selected English x86 Professional SP3 media. The 61 direct matches were independently verified on the selected disc. Source/media ABI compatibility and third-party distribution evidence remain open. H001 build ownership is traced.

## Recent Governance

Existing-component batches can include a required provider from another component. XP-only additions with independent scope are reserved for the next T.
