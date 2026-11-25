# Project Status

## Current Work

**Active: M0 T485 S2** (Ordinary Mode). Migrate H001-H008 in directory order. [S1 inventory and feasibility](../etc/evidence/t485-s1/README.md) is complete. H001–H008 have local source changes. The cached x64/x86 product build passed and twelve images were staged locally. The DEM x86 conversion ABI has been corrected. Product20 passed the real DOS4GW PIF gate with the SP3 guest. The x64 observer captured the 640-by-400 NESticle menu before Escape, hidden, and restored. The selected DPMI fixture was relinked with XP stack.c; its full set of modes passed, including the 4 GB stack case. See [package evidence](../etc/evidence/t485-s1/h005-h008.md). Product22 rebuilds NTVDMD.DLL from the top-level target with the SP1 version resource and passes both the menu and real debugger event tests. The package-stamped SP3 retail control also passes. The owner accepted the self-built SP1 DLL with the SP3 behavioral comparison. Both release locations match the product22 host manifest. See [debugger image acceptance](../etc/evidence/t485-s1/h001-sas-lifetime.md). S2 accounts for the eight H001–H008 batches in this delivery; 151 remain.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S2, Ordinary Mode. |
| Original Owner Request | Complete the T485 plan autonomously. Use local repos.external sources and the supplied XP installation discs. SP2 or SP3 binaries are acceptable. |
| Admission And Approval | Owner approved the S plan and continuous execution. The owner explicitly authorized use of the pinned XP source throughout T485 on the same basis as OpenNT. Project work and delivery of the source migration proceed without a separate license-evidence gate. This records the project decision; it makes no finding about third-party rights. The owner approved the SP3 NTVDMD.DLL retaining the guest RAM base within one SAS initialization in its own worker. Verify last use before SAS teardown and reacquisition on initialization. |
| Candidate Proposal | [T485 Windows XP baseline](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md). |
| Objective | Reconcile H001-H008 against both upstream revisions, implement necessary host adaptations and verify the resulting guest/host contracts. Start with H001 debugger dispatch and its required provider. |
| Non-goals | SOL.EXE, TASKMAN.EXE, WINMINE.EXE, WINVER.EXE and WRITE.EXE are outside migration scope. Leave the five programs untouched. Independent new XP components belong to the next T. No guest rebuild, unapproved winnt.zip additions or redistribution of installation media outside the approved package scope. |
| Reference Baseline | [T485 plan](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md), [S1 inventory](../etc/evidence/t485-s1/README.md), eleven-image T484 rollback package. |
| Files And ABI Surface | H001-H008, related build inputs and consumers. H001 includes its necessary NTVDMD interface. H002 and H005 track COMMAND and DOSX counterparts. |
| Applicable Rules | [Source policy](../etc/operations/policy/source-policy.md), [architecture](../rules/ARCHITECTURE.md), [execution](../rules/EXECUTION.md). |
| Verification | Compare old/local/XP, resolve directory membership, compile supported product and verify the actual product caller, guest path, failure and lifecycle. Inspect image identity, exports and imports. |
| Expected Markers | H001-H008 per-file dispositions, matched provider ABI, product verification and rollback receipts. |
| Asset Needs | Pinned local XP SP1 source baseline 5.1.2600.1106 (see proposal), verified English x86 Professional SP3 binary reference and T484 baseline. |
| Reporting Requirements | Count completed batches and files once, record early companion paths and distinguish local evidence from published delivery. |
| Stop Conditions | Incompatible ABI, guest immutability conflict or failed product gates block the affected delivery. |
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

The T485 S2 product22 package is installed. The previous T484 S7 package is the rollback baseline. T485 S1 catalogues 159 batches and 3,986 tracked files. P4 verifies eleven rollback images at both locations. The candidate declares 5.1.2600.1106. Its README reports leaked provenance. The owner approved local source use on the OpenNT basis. The owner selected English x86 Professional SP3 media. The 61 direct matches were independently verified on the selected disc. Source/media ABI compatibility is verified per batch. The project owner has approved XP source use and ordinary project delivery for all T485 batches. H001 build ownership is traced.

## Recent Governance

Existing-component batches can include a required provider from another component. XP-only additions with independent scope are reserved for the next T.
