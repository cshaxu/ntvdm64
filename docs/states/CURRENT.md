# Project Status

## Current Work

**Active: M0 T485 S3** (Ordinary Mode). S2 delivered H001–H008 in commit 574d526e0. Both installed locations match the twelve-image manifest. The guest and host inputs are listed in the [S2 package receipt](../etc/evidence/t485-s1/h001-h008-package22.csv). H011 communications brings the total to 11/159. H012 is next. See the [H011 receipt](../etc/evidence/t485-s1/h011.md).

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S3, Ordinary Mode |
| Admission And Approval | Owner approved use of the pinned XP SP1 source throughout the [migration sequence](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md), including delivery to the existing origin. |
| Host binary acceptance | Ship the locally compiled host components, including NTVDMD.DLL and necessary project adaptations. Use the selected SP3 binaries for ABI and behavior comparisons. Verify both installed copies against the manifest of the project build. |
| Objective and scope | Migrate H009–H022 in inventory order. Reconcile the SoftPC base directories and necessary providers against OpenNT, current product code and pinned XP SP1. |
| Inputs | S1 file ledger, pinned XP source, S2 product22 runtime and selected immutable SP3 media. |
| Verification and closure | Compile supported targets, test affected CPU/DPMI and display/input boundaries, publish the paired package, verify hashes, commit and push. Record directory membership, preserved adaptations and rollback. |
| Constraints | Preserve guest binaries, approved archive membership and unrelated files. Independent new XP components remain queued. |

## S1 Closure Record

[Inventory, media qualification, H001 ABI evidence and rollback](../etc/evidence/t485-s1/README.md). The selected English Professional SP3 disc supplies 61 direct binary matches. Source and media span different service packs. Product migration and runtime verification begin in S2.

## Recent M0 Closures

- [T484](../history/m0-t484-closure.md).
- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Current Technical Baseline

The S3 product25 package is installed. Product24 at e75ceacc0 is the H011 rollback baseline. T485 S1 catalogues 159 batches and 3,986 tracked files. P4 verifies eleven rollback images at both locations. The candidate declares 5.1.2600.1106. Its README reports leaked provenance. The owner approved local source use on the OpenNT basis. The owner selected English x86 Professional SP3 media. The 61 direct matches were independently verified on the selected disc. Source/media ABI compatibility is verified per batch. The project owner has approved XP source use and ordinary project delivery for all T485 batches. H001 build ownership is traced.

## Recent Governance

Existing-component batches can include a required provider from another component. XP-only additions with independent scope are reserved for the next T.
