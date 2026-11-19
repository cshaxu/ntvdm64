# Project Status

## Current Work

**Active: M0 T483 S14** (Ordinary Mode). S13 completes the actual physical-mapping
implementation migration. The broader C-layer cleanup remains active.

## Active Packet

| Field | Record |
| --- | --- |
| Original Owner Request | 实际公共实现及必要状态下沉B，不只是移动头文件或增加转发；不把NTVDM/NTSRV核心搬进B；按批准的S队列连续执行。 |
| Objective | Audit and migrate the remaining reusable guest-memory/lease, XMS and WOW page-domain mechanics with their state/cleanup to B, preserving C algorithms and product lifecycle ownership. |
| Files And ABI Surface | Existing guest-location/memory contracts, F guest-memory/lease/XMS/page-domain providers, D page-domain declarations, production callers and selected tests. Follow the approved plan by complete ownership chain. |
| Reference Baseline | S13 P17; package92f469bf-1555-4ff0-b16d-ef2d9292934a; source/evidence in the linked closure. Direct ledger144 eliminated/424 open is not an implementation-migration count. |
| Applicable Rules | Six-layer architecture, original-first source policy, coding/execution/documentation rules and approved implementation migration. |
| Verification | Cached full x86/x64/reference builds; dual11-image publication and22 hashes; affected memory/lease/page-domain and isolated guest tests; governance and relative links; commit and push. |
| Asset Needs | Existing fixed OpenNT baseline, cached graphs and approved immutable guest fixtures. |
| Candidate Proposal | [T483 parent proposal](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Identifier Mode | M0 T483 S14, Ordinary Mode. |
| Admission And Approval | Owner approved sequential implementation migration on2026-11-19. S13 first-chain verification completed; S14 admitted at its closure delivery. S15-S19 remain planned. |
| Non-goals | No guest modification/rebuild, whole-session/core migration, duplicate TLS/state machine, new B-to-C exception or forwarding-only completion. |
| Expected Markers | Actual inward implementations and state ownership, removed superseded providers/forwarding, retained original memory semantics and lifetime. |
| Similar-Issue Sweep | All acquire/use/release, thread and cross-image consumers of each selected chain; distinguish original C provider algorithms from common compatibility mechanics. |
| Reporting Requirements | File entry/exit, mirror points/lines and cumulative totals; direct edges, actual migration, deleted forwarding and remaining ownership counted separately. |
| Stop Conditions | Prior approval for net mirror-diff increases or materially changed semantic/ownership boundaries; investigate before migration. |
| Exit Criteria | Complete eligible chains and concrete dispositions for unresolved original/private boundaries, final-code targeted tests, coherent dual release, updated ledgers, commit/push, then sequential S15 admission. |

## Current Technical Baseline

S14 P24 moves the three checked SAS provider bodies from F to C. Final
production and real-CCPU fixture maps resolve them to ccpusas4.obj; F retains
registration/lifetime and forward-copy admission. Cached full builds,
dual11 publication/22 hashes and targeted lease/memory/thread/redirector/WOW,
published NESticle and DOS DIR tests passed. Package
3df9d92c-3912-4efa-8866-7e2667f45785 is published.
[P24 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p24-sas-provider.json)
records owner approval for +1 point/+52 raw inserted lines. Non-error diff
changes12 points/27 lines to13/78; one extra #line is counted separately.
Prior gross reduction3/12 remains recorded; cumulative net is-2 points/+39
lines after this approved ownership adaptation. Direct ledger147 eliminated/
421 open is unchanged. XMS and USER boundaries remain; S14 is not closed.

S14 P22 removes the remaining F guest-memory include from EMS; actual copy
implementations already moved to B in P20. Cached full builds and dual11
publication/22 hashes passed. All11 normalized images match P21 after
package/PE timestamp masking; runtime tests are reused under owner approval.
Packagec7d90841-65da-481f-8a89-090e0e41e922 is published.
[P22 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p22-ems-header-delivery.json)
records compiler dependencies and XMS return-owner findings. C0040 remains
open for XMS; ledger147 eliminated/421 open. Mirror delta0/0, cumulative
reduction3 points/12 lines. S14 remains active.

S14 P21 moves the actual shared WOW clock resource into B: host page,
stop event, publisher thread and cleanup. F retains guest mapping and CPU/
session lifecycle. Stop/join precedes unmap; page release follows unmap.
[P21 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p21-wow-clock-implementation.json)
records cached full builds, dual11 publication/22 hashes,23 clock assertions,
lease/memory/redirector/thread checks, actual CCPU WOW clock update/reentry,
and published NESticle/DOS DIR. Packageceb7fcbe-4574-4763-8e1d-64bf4be708ab
is published. Mirror delta0/0; cumulative reduction3 points/12 lines.
Direct ledger remains147 eliminated/421 open. S14 still owns unresolved
SAS/XMS and USER projection boundaries; this is not its closure.

S14 P20 migrates five guest-memory acquire/release/copy/move implementations
and their contract into B. F keeps SAS providers and lifecycle ownership;
there is no new binding or state machine. Cached full builds, dual11 publication
and22 hashes,75 lease-boundary assertions,33 linear-memory assertions,
redirector27/14,thread,WOW page-domain,NESticle and DOS DIR passed.
Package3b7fca15-11f2-47b3-b598-37319f654d3d is published.
[P20 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p20-memory-access-implementation.json)
records actual symbol ownership. C0225-C0227 are eliminated:147 eliminated,
421 open of618. Mirror delta0/0; cumulative reduction3 points/12 lines.
The mixed SAS header/provider, XMS cancellation and WOW ownership remain open;
[source investigation](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-sas-xms-owner-review.json)
explains why raw SAS void operations cannot replace fallible leases.

S14 P19 removes the F guest-location source/private header and three-operation
forwarding table/installers. Nine operation bodies now execute in B using the
existing thread binding and a typed reference to the same B lease object.
F retains composition, resource admission and retirement timing; private session
ABI is12. [P19 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p19-location-implementation.json)
records cached full x86/x64/reference builds, dual11 publication/22 hashes,
wrong-owner/cancellation and lease tests, redirector27/14 assertions, thread,
WOW page-domain, NESticle and DOS DIR. Package e26d2851-fb17-4d62-805f-43713cd2c470
is published. Mirror delta0/0; cumulative reduction3 points/12 lines. The direct
ledger remains144 eliminated/424 open; five implementation receipts are updated,
not counted again. S14 remains active for SAS/XMS/WOW ownership.

S14 P18: the bounded guest-memory lease implementation and definitions have
moved to B, with the leaf boundary test under test/compat. Session still
composes the lease object and owns ACTIVE admission and retirement timing.
Implementation bodies and layouts match P17 apart from include paths; mirror
delta is zero. Cached full x86/x64/reference builds, dual11 publication and22
hashes, lease/session/memory/WOW targeted checks, NESticle and DOS DIR passed.
Package f0903b3b-8522-4e86-97fd-2999957c1f95 is the P18 publication; S14 remains active.
[P18 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p18-lease-implementation.json)
records the corrected build-generator array failure and final verification.
The [memory ownership inventory](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-memory-ownership-inventory.json)
separates lease mechanics, SAS providers, XMS cancellation and WOW page-domain
ownership. P19 removes guest-location forwarding; the mixed CCPU/WOW provider
and SAS lease provider remain open and are not claimed as migrated.

[S13 implementation closure](../etc/evidence/m0-t483-s13-implementation-closure.md)
and [P17 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-p17-mapping-implementation.json)
record the eight actual B mapping implementations, removed F provider/table,
and one relocated thread-context binding. F retains task lifecycle decisions,
counts, cancellation/completion and termination escape. The private session
structure ABI is11; no guest/task protocol changes.

Cached x86/x64/reference builds,8 A/B graphs and targeted memory, alias,
session, cross-DLL thread and WOW page-domain tests passed. Actual NESticle
menu/Escape and DOS DIR passed. Package92f469bf-1555-4ff0-b16d-ef2d9292934a
is published twice with22 verified hashes. Only NTVDM has substantive image
changes after package-GUID/PE-debug-timestamp masking. No Full or new manual
Win3.x/Win95 desktop acceptance is claimed.

Mirror cohort22 points/92 lines is unchanged; cumulative reduction3 points/
12 lines. Direct ledger:144 eliminated,424 open,44 supporting,5 eliminated
transitive and1 excluded of618. Nine rows receive implementation evidence,
not a second elimination count. Build-generation failures are retained in P17.

## Sequential Work

[Residual chain review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-residual-chain-review.json)
confirms XMS failure propagation spans the void service table and BOP caller,
which ignores the dispatch return. SAS/USER backing retains CPU-specific
failure semantics. S14 remains active until these provider boundaries are
resolved; this source investigation is not new runtime acceptance.

[WOW page-domain ownership review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-wow-page-domain-owner-review.json)
separates clock ownership from CPU and USER backing. P21 completes the
shared host-clock extraction. USER backing remains coupled to SAS and still
requires its own boundary resolution; the source review is not runtime proof.

[Approved S13-S19 worklist](../etc/operations/t483-compat-implementation-migration-plan.md):
S13 closed; S14 memory active; S15 input/display, S16 COMMAND/BASE separation,
S17 devices, S18 WOW and S19 original-owner recovery/final reconciliation
are planned. This supersedes the former S13/S14 allocation, not its evidence.

The unpublished keymouse forwarding trial was withdrawn with
[patch/hash evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-keymouse-withdrawal.json).
It was never released or runtime-accepted. All prior P13-P15 direct-boundary
receipts remain in the [dependency tracker](../../artifacts/mvdm-audit/t483-c-layer-dependencies/README.md).

## S13 Closure Record

[Actual physical-mapping implementation migration](../etc/evidence/m0-t483-s13-implementation-closure.md).

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
