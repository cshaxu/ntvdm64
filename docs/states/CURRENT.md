# Project Status

## Current Work

**Active: M0 T485 S4** (Ordinary Mode). S2 delivered H001–H008 in commit 574d526e0. The S2 receipt identifies its twelve host images. The guest and host inputs are listed in the [S2 package receipt](../etc/evidence/t485-s1/h001-h008-package22.csv). S3 delivered H009–H022, bringing the total to 22/159. S4 delivered H023 host headers and H024 link resources, bringing the total to 24/159. H025 host sources brings the total to 25/159. H026 delivered the worker image and exports, bringing the total to 26/159. H027 delivered the suballocator baseline, bringing the total to 27/159. H028 delivered the XP VCDEX provider, bringing the total to 28/159. H029 restored the sample1 directory selector (29/159). H030 is next. See the [H029 receipt](../etc/evidence/t485-s1/h029.md). See the [H028 receipt](../etc/evidence/t485-s1/h028.md). See the [H027 receipt](../etc/evidence/t485-s1/h027.md). See the [H026 receipt](../etc/evidence/t485-s1/h026.md). See the [H025 receipt](../etc/evidence/t485-s1/h025.md). See the [H023–H024 receipt](../etc/evidence/t485-s1/h023-h024.md). See the [S3 closure and H022 release receipt](../etc/evidence/t485-s1/h022.md).

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S4, Ordinary Mode |
| Admission And Approval | Owner approved use of the pinned XP SP1 source throughout the [migration sequence](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md), including delivery to the existing origin. |
| Host binary acceptance | Ship the locally compiled host components, including NTVDMD.DLL and necessary project adaptations. Use the selected SP3 binaries for ABI and behavior comparisons. Verify both installed copies against the manifest of the project build. |
| Objective and scope | Migrate H023–H036 in inventory order. Reconcile the SoftPC host, VDD, redirector, WOW32 and XMS directories against OpenNT, current product code and pinned XP SP1. |
| Inputs | S1 file ledger, pinned XP source, S3 product34 runtime and selected immutable SP3 media. |
| Verification and closure | Compile supported targets, test affected host bindings and paired guest interfaces, publish the paired package, verify hashes, commit and push. Record directory membership, preserved adaptations and rollback. |
| Constraints | Preserve guest binaries, approved archive membership and unrelated files. Independent new XP components remain queued. |

## S1 Closure Record

[Inventory, media qualification, H001 ABI evidence and rollback](../etc/evidence/t485-s1/README.md). The selected English Professional SP3 disc supplies 61 direct binary matches. Source and media span different service packs. Product migration and runtime verification begin in S2.

## Recent M0 Closures

- [T484](../history/m0-t484-closure.md).
- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Current Technical Baseline

H024 Full checkpoint: product36 was built for x64/x86 and published at both locations. The DPMI fixes, package identity and test evidence are in the [checkpoint receipt](../etc/evidence/t485-s1/h024-full-checkpoint.md). Full passed all 20 gates on product36. H026 product38 passed its targeted PE export, resource and DOS4GW tests.

The S4 product36 package contains thirteen host images, including VJOY.DLL. Commit 4e6e4bd7f contains the previous product34 package. T485 S1 catalogues 159 batches and 3,986 tracked files. P4 verifies eleven rollback images at both locations. The candidate declares 5.1.2600.1106. Its README reports leaked provenance. The owner approved local source use on the OpenNT basis. The owner selected English x86 Professional SP3 media. The 61 direct matches were independently verified on the selected disc. Source/media ABI compatibility is verified per batch. The project owner has approved XP source use and ordinary project delivery for all T485 batches. H001 build ownership is traced.

## Recent Governance

Existing-component batches can include a required provider from another component. XP-only additions with independent scope are reserved for the next T.
