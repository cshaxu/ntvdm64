# Project Status

## Current Work

**Active: M0 T483 S13** (Ordinary Mode). S12 is closed at its in-place cleanup boundary; C now has440 open direct dependency IDs.

## Active Packet

| Field | Record |
| --- | --- |
| Original Owner Request | 原地依赖清理后，处理可以从 C 安全挪到 F 的产品职责；再逐案判断 F 回迁 C。迁移须分析镜像 diff 点位和行数。 |
| Objective | Remove product-owned outward C dependencies through coherent ownership changes while preserving original C algorithms and the selected ABIs. |
| Files And ABI Surface | C callers and their D/E/F providers, grouped by request, session, native-call and resource lifetime. Scope is the S13 allocation in the S12 handoff. |
| Reference Baseline | bf53bd508; package94150155-5251-42f3-b587-9803d82bc178; [complete handoff](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p108-closure-handoff.json). |
| Applicable Rules | Architecture, source policy, coding and execution authorities; original-first recovery and minimal mirror differences. |
| Verification | Real production compile/include/link dependencies, affected ABI and lifecycle cases, cached builds, complete dual publication and hash checks. Apply existing owner verification waivers only with corresponding evidence. |
| Asset Needs | Existing fixed OpenNT baseline, cached production graphs and approved immutable guest fixtures. |
| Candidate Proposal | [T483 parent proposal](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Identifier Mode | M0 T483 S13, Ordinary Mode. |
| Admission And Approval | Owner authorized three sequential C-layer scopes: in-place cleanup, safe C-to-F product responsibility changes, then individual F-to-C assessment. S12 closure admits this second scope. |
| Non-goals | No guest modification/rebuild; no wholesale C algorithm extraction; S14 original source/ABI restoration is not yet admitted. |
| Expected Markers | Each eliminated edge has actual compiler/linker evidence and the applicable behavior/lifetime validation. |
| Similar-Issue Sweep | Process coupled creation/use/retirement callers together across supported product graphs. Include mixed-header witnesses; do not ignore transitive paths. |
| Reporting Requirements | File entry/exit, stable IDs, scope, current and cumulative mirror points/lines, open/eliminated counts, and build/publication/test/commit status. |
| Stop Conditions | Obtain prior approval for net mirror diff increases or unapproved scope changes. |
| Exit Criteria | Finish the admitted ownership groups with preserved C algorithms and correct lifecycle, update tracker, verify and deliver; any unresolved original-ownership matter has a concrete S14 handoff. |

## Current Technical Baseline

S12 P107 moved three unchanged breakpoint providers F to B. Cached x86/x64 builds, reference-x86 worker,8 A/B build graphs and22 release hashes passed. All11 images match P104 after masking only package GUID and PE/debug timestamps; repeated runtime testing was waived. That preceding package was94150155-5251-42f3-b587-9803d82bc178; S13 P11 below supersedes both published copies.

S13 P11: [demsrch unavailable VDM boundary](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-p11-demsrch-vdm-boundary.json). Original demsrch.c is byte-unchanged:5 non-error points/25 lines;delta0/0. The previous F query callback had no production installer. Its unused thread/session state is deleted; B retains the same cdecl NtVdmControl ABI and existing STATUS_NOT_IMPLEMENTED result. Original DEM owns the enumeration fallback.

C0188 eliminated:128 eliminated/440 open of618;S13 has cleared32 IDs. Cumulative mirror reduction remains2 points/10 lines. No new mirror difference or guest change.

Cached full x86/x64 products, reference-x86 and8 A/B graphs pass. Packageb93146bd-24be-4120-a919-3dd1e5f58502 is published to both destinations with22 hashes verified. Original-declaration ABI/unavailability fixture,115 actual directory reset assertions and real DOS full DIR pass. The first build's single-element generator concatenation error was fixed before publication; failure evidence retained. No Full or Win3.x/Win95 acceptance claimed.

Next remaining DIR file:dpmi32/i386/dpmi386.c. S13 remains active;S14 remains unadmitted.

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
