# Project Status

## Current Work

**Active: M0 T483 S13** (Ordinary Mode). S12 is closed at its in-place cleanup boundary; C now has424 open direct dependency IDs.

## Active Packet

| Field | Record |
| --- | --- |
| Original Owner Request | 写入当前proposal，制作S任务队列，自行更新目标开始执行；目标是实际公共实现及必要状态下沉B，不只是移动头文件或增加转发。 |
| Objective | Move the physical-mapping implementation, state and cleanup into compat, extracting only its required shared instance/thread binding without a second session state machine. |
| Files And ABI Surface | session binding, physical mapping provider/contracts, production consumers, build ownership and corresponding tests; see the approved migration plan. |
| Reference Baseline | 0c165f5a4; package137949a3-b4bd-457d-82c4-1b455762eb17. Old direct ledger144 eliminated/424 open is not an implementation-migration count. |
| Applicable Rules | Six-layer architecture, original-first source policy, execution/coding/documentation rules and owner-approved implementation migration. |
| Verification | Cached full x86/x64/reference builds; dual eleven-image publication and22 hashes; affected binding/lifecycle/memory and isolated DOS/NESticle tests; governance and relative links; commit and push. |
| Asset Needs | Existing fixed OpenNT baseline, cached graphs, existing approved guest fixtures. No new guest sources or binaries. |
| Candidate Proposal | [T483 parent proposal](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Identifier Mode | M0 T483 S13, Ordinary Mode; scope revised by owner, not a new simultaneous S. |
| Admission And Approval | Owner explicitly approved the implementation migration plan and continuous execution on2026-11-19. Prior pause is revoked. S14-S19 are sequential planned successors, not yet active. |
| Non-goals | No guest modification/rebuild, whole-session migration, duplicate TLS/state machine, wholesale C extraction or forwarding-only completion. |
| Expected Markers | Actual B provider bodies and B-owned mapping state; removal of superseded F mapping provider/table; preserved binding, rollback and disposal behavior. |
| Similar-Issue Sweep | All binding/unbinding and cross-image consumers of the first chain, including failure cleanup; remaining memory services assigned to S14. |
| Reporting Requirements | Per-file entry/exit, mirror points/lines and cumulative changes; direct edges, actual implementation migration and removed forwarding counted separately. |
| Stop Conditions | Prior approval required for net mirror-diff increases or a materially different product/guest semantic boundary. |
| Exit Criteria | Complete the first production chain, targeted final-code verification, coherent dual release, updated ledgers, commit and push; admit S14 only after bounded closure. |

## Revised execution plan

[Approved S13-S19 migration worklist](../etc/operations/t483-compat-implementation-migration-plan.md).
S13 starts with binding and physical mapping; S14 remaining memory, S15 input/display,
S16 COMMAND/BASE separation, S17 devices, S18 WOW and S19 original-owner recovery
and final reconciliation. This supersedes the earlier S13/S14 allocation.

The unpublished keymouse forwarding candidate was withdrawn; its patch and hashes
are retained in [withdrawal evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-keymouse-withdrawal.json).
Its cached build passed, but it was not published or runtime-accepted. Current
production baseline remains P15. Implementation migration is now beginning;
previous direct-edge elimination does not count as actual implementation descent.

## Current Technical Baseline

S12 P107 moved three unchanged breakpoint providers F to B. Cached x86/x64 builds, reference-x86 worker,8 A/B build graphs and22 release hashes passed. All11 images match P104 after masking only package GUID and PE/debug timestamps; repeated runtime testing was waived. That preceding package was94150155-5251-42f3-b587-9803d82bc178; S13 P15 below supersedes both published copies.

S13 P13: [BIOS VM boundary](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-p13-bios-vm-boundary.json).14 BIOS DIR units reviewed. Original algorithms stay C; generated ABI/address owners remain S14. virtual.c changes only its contract include; B owns the immutable interface, F retains all VM/IRQ and controlled-stop state/behavior. Unused D/F search roots removed from BIOS compilation.

C0047/C0256/C0258 eliminated:135 eliminated/433 open of618; S13 cleared39 IDs. Cohort mirror9 points/27 lines unchanged; cumulative reduction3 points/12 lines. No guest changes.

Cached full x86/x64,reference-x86 and8 A/B graphs pass. Packagea9ed59d4-6106-40e7-8dab-85d23c0f3009 published to both destinations with22 hashes.3183 VM unit assertions,NIDDB original-object positive/negative controls and actual DOS DIR pass. Initial NIDDB fixture include failure and omitted outer exit-code check are retained; acceptance uses separate successful final NIDDB receipt. No Full or real Win3.x/Win95 desktop acceptance claimed.

S13 P14: [physical mapping boundary](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-p14-physical-mapping-boundary.json). CCPU/EMS/nt_mem use B immutable contracts; F provider bodies and session lifecycle remain unchanged.9 IDs eliminated:144 eliminated/424 open of618; S13 cleared48. Three mirrors remain22 points/92 lines, cumulative reduction3 points/12 lines.

Cached x86/x64/reference builds and8 A/B graphs passed. Package4f34bf15-fc3b-4ccc-96a4-be3a154dd923 published to both destinations;22 hashes verified. Mapping, original external memory, CCPU high-linear-page, WOW page-domain, NESticle graphics/Escape and final isolated DOS DIR pass. The first DIR wrapper failed on an obsolete receipt name before product execution; preserved aggregate failure is superseded only for DIR by strict-final.exit0. No Full or desktop acceptance claimed.

[CPU dependency follow-up](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-ccpu-dependency-review.json) confirms COMMON definitions belong to C. Generated ABI remains S14. [Remaining CCPU/comms/C-VID review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-cpu-comms-ownership-review.json) confirms generated/vector ABI ownership remains S14; no new eliminated IDs or code edits. S13 P15: [base device search roots](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-p15-base-device-search-roots.json) removes unused D/F search roots from24 comms/debug/disks/DOS units. All source bytes and selected headers unchanged. Full cached x86/x64/reference builds and8 A/B graphs pass. All11 images equal P14 after package GUID and PE/debug timestamp masking; repeated runtime tests waived. New package137949a3-b4bd-457d-82c4-1b455762eb17 published twice with22 matching manifest hashes. Shared root IDs remain open:144 eliminated/424 open of618. Mirror delta0/0; cumulative reduction3 points/12 lines. The subsequent owner-approved migration plan above replaces the old DIR continuation; generated ABI ownership is assigned to planned S19.

## S12 Closure Record

[In-place cleanup closure](../etc/evidence/m0-t483-s12-closure.md).

[Closure and successor inventory](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p108-closure-handoff.json) reconciles618 stable IDs:96 eliminated,472 open,44 supporting witnesses,5 eliminated transitive witnesses and1 excluded diagnostic. File review covers116 compile-dependent and74 additional link-only files. Mirror reduction totals2 points/10 lines; this closure changes no product source.

S13 receives428 product-boundary IDs and the lifecycle side of21 mixed declaration groups. S14 retains23 original-owner IDs and the declaration side of those21 groups. These are allocations, not claims of safe wholesale file movement or completed fixes. S14 remains unadmitted. All prior S12 receipts remain indexed in the [C dependency tracker](../../artifacts/mvdm-audit/t483-c-layer-dependencies/README.md).

## S1 Closure Record

[T483 S1 closure](../etc/evidence/m0-t483-s1-closure.md).

## S2 Closure Record

[T483 S2 closure](../etc/evidence/m0-t483-s2-closure.md).

## S3 Closure Record

[T483 S3 closure](../etc/evidence/m0-t483-s3-closure.md).

## S4 Closure Record

[T483 S4 closure](../etc/evidence/m0-t483-s4-closure.md).

## S5 Closure Record

[T483 S5 closure](../etc/evidence/m0-t483-s5-closure.md).

## S6 Closure Record

[T483 S6 closure](../etc/evidence/m0-t483-s6-closure.md).

## S7 Closure Record

[T483 S7 closure](../etc/evidence/m0-t483-s7-closure.md).

## S8 Closure Record

[T483 S8 closure](../etc/evidence/m0-t483-s8-closure.md).

## S9 Closure Record

[40-file compat audit and verification](../../artifacts/t483-s9-non-mirror-audit/sequential.md), delivered at 55643b6c5. [Successor packet delivery](../etc/evidence/m0-t483-s10-components.md) records this revision as its baseline.

## S10 Closure Record

Owner accepted and closed T483 S10. P1 shipped at 14969ed80 with the targeted verification recorded in the [S10 delivery and closure record](../etc/evidence/m0-t483-s10-components.md).

## S11 Closure Record

The owner accepted [A and B cleanup](../etc/evidence/m0-t483-s11-layer-boundaries.md) after P7 at 81f4cef3d. The [delivery record](../../artifacts/mvdm-audit/t483-s11-p7-compat-boundary.json) lists the two approved WOW32 exceptions, eight production graphs, both published eleven-image packages and twenty passing Full gates. The C dependency tracker uses direct ownership; transitive observations remain supporting evidence.

## Recent M0 Closures

- [S11 A/B boundary closure](../etc/evidence/m0-t483-s11-layer-boundaries.md): two owner-approved B exceptions remain.
- [S10 component delivery](../etc/evidence/m0-t483-s10-components.md).
- [S9 compat audit](../../artifacts/t483-s9-non-mirror-audit/sequential.md).
- [S8 closure](../etc/evidence/m0-t483-s8-closure.md).
- [S7 closure](../etc/evidence/m0-t483-s7-closure.md).
- [S6 closure](../etc/evidence/m0-t483-s6-closure.md).
- [S5 closure](../etc/evidence/m0-t483-s5-closure.md).
- [S4 closure](../etc/evidence/m0-t483-s4-closure.md).
- [S3 closure](../etc/evidence/m0-t483-s3-closure.md).
- [S2 closure](../etc/evidence/m0-t483-s2-closure.md).
- [S1 closure](../etc/evidence/m0-t483-s1-closure.md).

## Recent Governance

Each file entry and exit reports both file and whole-audit totals. The unrelated [TODO](TODO.md) remains separate. The active goal is not complete; S13 implementation and S14 remain.
