# Project Status

## Current Work

**Active: M0 T428 S2** (Ordinary Mode).

Owner admits the queue-head worker control/lifecycle/ownership unification
package on 2026-10-04, asking first for current-state audit and architecture
decisions before implementation. T427's [closure](../history/m0-t427-system-root-search-isolation-closure.md)
remains accepted. Owner has accepted the S1 architecture conclusions and
approved implementation. S2 repairs shared native GUI carrier residency;
the [implementation plan](../etc/operations/t428-worker-unification-plan.md)
records the subsequent bounded stages.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T428 S2; Ordinary Mode. |
| Admission And Approval | Owner: “批准你的所有的回复…目标：精简，去重复，统一逻辑。NTVDM符合OpenNT的原始语义，NTVWM参照NTVDM的方法来做。开始。” S1 source review concluded; its documentation P remains pending, not claimed delivered. Admit the first bounded shared-GUI residency repair. |
| Candidate Proposal | [Worker control/lifecycle/ownership unification](../proposals/proposal-worker-control-lifecycle-naming-unification-001.md). |
| Objective | Admitted shared GUI-only NTVWM remains available after direct startup/completion, like shared WOW; remove its added unbound ten-second idle retirement. |
| Non-goals | No original mirror/guest changes, new framework/helper/scheduler/registry, descendant observation, performance work, parent-resume or management-stop migration in this stage; exclusive native sessions follow in the planned bounded stage. |
| Reference Baseline | main 0e1597632; accepted T427 S5 production 62a71fd90/cc245ca95, system32 eight-file package, APP0.0.427/RPC38/I/O25. Preserve side-session planning edits. |
| Files And ABI Surface | NTSRV lifecycle.c/service_core.c/service_internal.h; production-linked service fixture, GUI routing/residency fixture and runners; current design, indexed plan/evidence. No copied wire/IDL change. |
| Applicable Rules | Full AGENTS reading set, source provenance, original execution/cleanup ownership, shared-mechanism boundaries and documentation rules. |
| Verification | Incremental affected x86 /MT CCPU40 build; shared-carrier policy fixture before/at/after old deadline, active/idle transitions, retained frontend grace/loss tests; Console17/Window17/WOW and coherent eight-file publication before production P. Governance, relative links and diff checks. |
| Expected Markers | No admitted native-carrier idle timeout; actual shutdown remains clear after arbitrary explicit-time advances; existing frontend loss and legitimate startup deadlines remain effective. |
| Asset Needs | Existing MSVC14.43/SDK22621 build cache and immutable runtime media; all new outputs under build/M0-T428/S2. |
| Reporting Requirements | Report removed policy/field, paired source semantics, actual tests/publication and remaining stages; no whole-T completion claim. |
| Stop Conditions | New execution/lifetime policy or expanded original mirror changes require owner decision before implementation. Preserve unrelated modifications. |
| Exit Criteria | Shared GUI residency repaired and required focused/runtime gates, coherent publication, reviewed commit/push complete; remaining T work explicitly retained. |
| Original Owner Request | Unify lifecycle and control with minimal duplicate logic; NTVWM follows original NTVDM DOS/WOW shared/exclusive policy, not an extra ten-second GUI-worker deadline. |
| Similar-Issue Sweep | GUI target versus carrier completion, native active/idle reuse, frontend workerless grace, broker empty grace, startup reservations and shared WOW/DOS source lifetime. |

The [S1 source audit](../etc/evidence/m0-t428-s1-worker-control-lifecycle-audit.md)
corrects stale two-route-loop findings, confirms shared spawn/I/O/client
mechanisms and identifies remaining parent-restore, association, cancellation,
occupancy/close and naming differences. Owner approved all corrected boundaries;
the plan records the decisions. S1 is source-only concluded with P pending.
S2 implementation and affected x86 build passed. The [S2 evidence](../etc/evidence/m0-t428-s2-shared-gui-worker-residency.md)
records 22 service cases, Console17/Window17/WOW, five GUI routing/residency
cases, coherent eight-file publication and published Console/Window smoke.
Reviewed P delivery is pending. Independent GUI same-worker reuse remains unproved/failed
in the extra r006 probe and is retained for association consolidation;
S2 proves residency, not complete lifecycle unification.

## S1 Closure Record

Owner accepted the [source audit](../etc/evidence/m0-t428-s1-worker-control-lifecycle-audit.md)
and corrected architecture decisions on 2026-10-04. This is source-only
closure, no runtime capability pass. Documentation P is deferred to the next
reviewed delivery; no commit/push is claimed for it. T428 remains open.

## Current Technical Baseline

Production delivery 62a71fd90, followed by pushed registration cc245ca95,
is the T427 S5 system32 host layout. MSVC14.43/SDK22621/Win32 x86 /MT
CCPU40; APP0.0.427/RPC38/I/O25. O:/winnt/system32 contains all six EXEs
and both host DLLs. Each loaded EXE's parent defines the product Windows root.
Original runtime media use declared system32 locations; root SYSTEM.INI and
beside-worker NTVDM.REG ownership remain. User search is ordinary CWD/PATH
or explicit paths. Native host APIs and Win16 directory roles remain distinct.

[S5 evidence](../etc/evidence/m0-t427-s5-system32-host-colocation.md) records
affected builds, focused tests, r012 Console17/Window17/WOW, recoverable r011
publication and r013 published smoke/hash checks. The accepted eight-file set
equals build/M0-T427/S5/r001/runtime. This documentation-only closure changes
no production input and does not rebuild, redeploy or stop running programs.

Owner has placed extra Windows 3.1 WINMINE/SOL/WRITE applications at
O:/winnt. Same-name NT4 setup entries do not establish these binaries'
provenance. They remain user applications. Sealed WOW results retain their
tested inputs, not a new test claim after the owner's relocation. Physical
RDP/focus, broader WOW usability and scrollback retain disclosed limits.

Other sessions' queue/proposal edits are preserved outside this closure.
Only the admitted queue head becomes T428; later candidates keep their order.

## Recent M0 Closures

| Task | Outcome and retained evidence |
| --- | --- |
| T427 | Owner-accepted root/search isolation and system32 host co-location; S5 delivery 62a71fd90. [Closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree, S4 delivery 700b3d862. [Closure](../history/m0-t426-console-root-monitor-tree-closure.md). |
| T425 | Owner-directed closure after S11 typeahead proof. [Closure](../history/m0-t425-worker-neutral-frontend-closure.md). |
| T424 | Owner-closed at audited ff50ff588, production S12 46abdb554. [Closure](../history/m0-t424-component-control-closure.md). |
| T423 | Owner-accepted S40 f64559086. [Closure](../history/m0-t423-console-window-runtime-closure.md). |
| T422 | Initial WOW32 and planning milestone. [Closure](../history/m0-t422-initial-wow32-closure.md). |

## Recent Governance

Owner accepts T427 on 2026-10-04. This documentation-only closure moves its
closed chronology to history, preserves indexed S evidence and unrelated
planning edits, and leaves no active packet. Governance, relative-link and
diff checks apply. Owner subsequently admits T428 S1 source audit only;
implementation design decisions remain for owner review.
