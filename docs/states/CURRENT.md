# Project Status

## Current Work

**Active: M0 T428 S3** (Ordinary Mode).

Owner admits the queue-head worker control/lifecycle/ownership unification
package on 2026-10-04, asking first for current-state audit and architecture
decisions before implementation. T427's [closure](../history/m0-t427-system-root-search-isolation-closure.md)
remains accepted. Owner has accepted the S1 architecture conclusions and
approved implementation. S2 delivered shared native GUI carrier residency;
the [implementation plan](../etc/operations/t428-worker-unification-plan.md)
records the subsequent bounded stages.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T428 S3; Ordinary Mode. |
| Admission And Approval | Owner: “批准你的所有的回复…目标：精简，去重复，统一逻辑。NTVDM符合OpenNT的原始语义，NTVWM参照NTVDM的方法来做。开始。” Sequentially admit the approved parent-restore stage after S2 delivery 6a8934f6b, built/tested/published/pushed. |
| Candidate Proposal | [Worker control/lifecycle/ownership unification](../proposals/proposal-worker-control-lifecycle-naming-unification-001.md). |
| Objective | Move parent text-I/O resume arbitration out of run16 Console-member census into existing authenticated NTSRV relationships and exact task completion; preserve final-paint/input-return acknowledgment before parent resumes. |
| Non-goals | No original mirror/guest changes, second registry, scheduler/helper, ancestry or observed-task tracking, performance work, management-stop or exclusive-session migration. |
| Reference Baseline | main 6a8934f6b; published S2 eight-file package equals build/M0-T428/S2/r003/runtime/system32, APP0.0.427/RPC38/I/O25. Preserve side-session planning edits. |
| Files And ABI Surface | run16 frontend_scope/main; NTSRV existing Console-context/frontend/native-command/task-completion records; common client/interface only if necessary. Before wire change, record exact changed DTO/RPC and synchronize versions; no original mirror change is admitted. |
| Applicable Rules | Full AGENTS reading set, source provenance, original execution/cleanup ownership, shared-mechanism boundaries and documentation rules. |
| Verification | Source ordering audit then focused production-linked completion/authentication/negative tests; nested DOS-to-native-to-DOS and native-to-DOS-to-native, final I/O barrier, failure/disconnect and session isolation; affected x86 build, Console17/Window17/WOW, coherent publication before P; governance/links/diff review. |
| Expected Markers | run16 no longer enumerates members to decide parent resume; only authenticated corresponding parent is resumed after exact child completion and I/O release; duplicate/foreign/stale completion cannot restore unrelated input ownership. |
| Asset Needs | Existing MSVC14.43/SDK22621 incremental cache and immutable media; new outputs under build/M0-T428/S3. |
| Reporting Requirements | Report deleted launcher arbitration, existing-record ownership and lock/failure ordering, exact runtime verification and remaining stages; no whole-T closure claim. |
| Stop Conditions | New execution/lifetime policy or expanded original mirror changes require owner decision before implementation. Preserve unrelated modifications. |
| Exit Criteria | Authenticated parent-restore production path replaces launcher census and passes focused/runtime/publication/review/push gates; unfinished common shutdown, association/reuse and exclusive work remain explicit. |
| Original Owner Request | run16 must not check Console members for parent restoration; NTSRV already owns the logical relationships. Simplify and unify without rewriting original NTVDM execution. |
| Similar-Issue Sweep | DOS/native completion ownership, inner versus outer launcher restoration, root retirement confirmation, input-return/final-frame barriers, stale context and worker/root death. |

The [S1 source audit](../etc/evidence/m0-t428-s1-worker-control-lifecycle-audit.md)
corrects stale two-route-loop findings, confirms shared spawn/I/O/client
mechanisms and identifies remaining parent-restore, association, cancellation,
occupancy/close and naming differences. Owner approved all corrected boundaries;
the plan records the decisions. S1 is source-only concluded with P pending.
S2 implementation and affected x86 build passed. The [S2 evidence](../etc/evidence/m0-t428-s2-shared-gui-worker-residency.md)
records 22 service cases, Console17/Window17/WOW, five GUI routing/residency
cases, coherent eight-file publication and published Console/Window smoke.
Reviewed P 6a8934f6b is pushed. Independent GUI same-worker reuse remains unproved/failed
in the extra r006 probe and is retained for association consolidation;
S2 proves residency, not complete lifecycle unification.

## S2 Closure Record

Delivered 6a8934f6b on main and origin/main. Affected x86 build, 22 service
cases, Console17/Window17/three retained WOW frontiers, five GUI cases and
published Console/Window smoke passed; the tested eight-file package is at
O:/winnt/system32 with recoverable backup in build/M0-T428/S2/r007.
The [S2 evidence](../etc/evidence/m0-t428-s2-shared-gui-worker-residency.md)
records the separate GUI-selection reuse gap. S1 documentation is included
in this P. T428 remains open; S3 admission does not claim implementation yet.

## S1 Closure Record

Owner accepted the [source audit](../etc/evidence/m0-t428-s1-worker-control-lifecycle-audit.md)
and corrected architecture decisions on 2026-10-04. This is source-only
closure, no runtime capability pass. Documentation P is deferred to the next
reviewed delivery; no commit/push is claimed for it. T428 remains open.

## Current Technical Baseline

Current production delivery is T428 S2 6a8934f6b, verified/published from
build/M0-T428/S2/r003/runtime. It retains the following accepted T427 layout
and protocol baseline, with only the native GUI-carrier idle timer removed.

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
