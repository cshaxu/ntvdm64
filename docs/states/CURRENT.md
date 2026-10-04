# Project Status

## Current Work

**Active: M0 T428 S6** (Ordinary Mode).

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
| Identifier Mode | M0 T428 S6; Ordinary Mode. |
| Admission And Approval | Owner approved simplification/unification and sequential implementation. Admit the approved final naming/caller audit after S5 db855a66c, built/tested/published/pushed; no new GUI launch option. |
| Candidate Proposal | [Worker control/lifecycle/ownership unification](../proposals/proposal-worker-control-lifecycle-naming-unification-001.md). |
| Objective | Correct project-private ownership/format names and remove displaced aliases; finish the duplicate/caller audit across common, worker-base and the consumers. Preserve the delivered production semantics and present whole-T results for owner acceptance. |
| Non-goals | No original mirror/guest changes, second registry, scheduler/helper, ancestry/observed tracking, performance work, new cooperative-then-force timeout policy or component rename. |
| Reference Baseline | main/origin db855a66c; published eight-file package equals build/M0-T428/S5/r021/runtime/system32, with r015 recovery in S5/r029. APP0.0.427/RPC38/I/O25 unchanged. Preserve side-session planning edits. |
| Files And ABI Surface | Project-private NTCON frontend/input/format names, run16 native text/GUI client names, NTSRV record readers and their callers; common/worker-base headers only where ownership is inaccurate; audited build manifests and tests. No original mirror, wire, CLI or kind-label change. |
| Applicable Rules | Full AGENTS reading set, source provenance, original execution/cleanup ownership, shared-mechanism boundaries and documentation rules. |
| Verification | Inventory every rename and source/caller/build owner; no stale aliases or dependency inversion. Affected x86 build and focused format/input/service/GUI/lifetime tests; Console17/Window17/WOW, coherent recoverable eight-file publication and deployed smoke before production P; governance/links/diff/original-mirror checks. |
| Expected Markers | Current private names express frontend ownership or actual message format, not obsolete run16/worker ownership. Original source identities stay untouched. NTSRV remains the only relationship authority; explicit original/native execution boundaries are retained. |
| Asset Needs | Existing MSVC14.43/SDK22621 incremental cache and immutable media; new outputs under build/M0-T428/S6. |
| Reporting Requirements | Report renamed/removed private paths, remaining justified source differences, exact verified gates and disclosed limitations; T remains open pending owner acceptance. |
| Stop Conditions | New execution/lifetime policy or expanded original mirror changes require owner decision before implementation. Preserve unrelated modifications. |
| Exit Criteria | Naming and duplicate/caller audit passes source/build/runtime/publication/review/push gates; present final T428 audit and wait for owner acceptance without closing T. |
| Original Owner Request | NTSRV maintains logical frontend-worker relationships without another registry; NTVWM follows corresponding original DOS/Win16 residency, not an added ten-second GUI worker timer. Simplify and unify without extracting original mirror logic. |
| Similar-Issue Sweep | Frontend-private run16/native/dos spelling versus real Console/text/DIB format, consumer record-reader names, shared worker mechanisms versus original execution boundaries, removed aliases/callers and build selections. |

The [S1 source audit](../etc/evidence/m0-t428-s1-worker-control-lifecycle-audit.md)
corrects stale two-route-loop findings, confirms shared spawn/I/O/client
mechanisms and identifies remaining parent-restore, association, cancellation,
occupancy/close and naming differences. Owner approved all corrected boundaries;
the plan records the decisions. S1 source-only documentation was delivered with S2.
S2 implementation and affected x86 build passed. The [S2 evidence](../etc/evidence/m0-t428-s2-shared-gui-worker-residency.md)
records 22 service cases, Console17/Window17/WOW, five GUI routing/residency
cases, coherent eight-file publication and published Console/Window smoke.
Reviewed P 6a8934f6b is pushed. Independent GUI same-worker reuse remains unproved/failed
in the extra r006 probe and is retained for association consolidation;
S2 proves residency, not complete lifecycle unification.

S3 candidate removes launcher member census/selection and carries origin in
the existing service Console context. Affected x86 links and 23 service
tests passed; [S3 evidence](../etc/evidence/m0-t428-s3-authenticated-parent-resume.md)
records provenance and ordering. Console17/Window17/WOW and real nested
Console/Window parent-return gates passed. The coherent eight-file S3 package
is published to O:/winnt/system32 with recovery in build/M0-T428/S3/r008;
published smoke and all eight hash checks passed. Reviewed P ae28ef7ce is
pushed. S4's common shutdown implementation is built, tested and published.
Its [source boundary and verification record](../etc/evidence/m0-t428-s4-common-worker-shutdown.md)
records 37 shutdown assertions, 25 service cases, six actual GUI cases,
Console17/Window17/WOW, frontend/worker loss, independent close isolation and
published smoke/hash checks. Original DOS/WOW cleanup and wire versions remain
unchanged. Reviewed P 886f13758 is pushed; side-session planning stays separate.
S5 implements the existing worker-watch association authority and shared GUI
reuse; the [S5 evidence](../etc/evidence/m0-t428-s5-worker-association.md)
records focused passes and retained failures. Final r015 package regression,
lifetime, recoverable publication and deployed smoke/hash checks passed;
the reviewed implementation increment is delivered through Git. S5
is closed by pushed final P db855a66c. Paired route cancellation,
actual prepared native root-loss/Connect and asynchronous cleanup now pass
27 service cases. Final r021 package passes r022 Console17/Window17/WOW,
r027 six GUI cases and r028 lifetime/close/isolation probes. r029 publishes
that coherent package, preserving r015 recovery; r030 deployed Console/Window
smoke and eight hashes pass. Owner confirms no exclusive GUI launch option.
S6 is admitted for the approved final naming/caller audit; T428 remains open.

## S6 Delivery Review

S6's [ownership/naming inventory](../etc/evidence/m0-t428-s6-ownership-naming.md)
records private frontend API/build-owner renames, unchanged shared close-body
separation and retained source boundaries. Affected x86 builds, ownership
negative controls, 29 service cases, 37 shutdown assertions, worker-neutral
input/presentation, 34 sequential physical leases and actual Console close
fixtures pass. Final r002 package passes r007 Console17/Window17/retained WOW,
r009 six GUI cases and r010 management/loss/isolation. r011 publishes the
coherent eight-file package to O:/winnt/system32 and preserves S5 recovery;
r012 deployed Console/Window smoke and eight hashes pass. No mirror, wire,
CLI, kind mapping or native30ms polling change. Final source/diff/governance
review and sequential P delivery apply; T428 remains open for owner acceptance.

## S5 Closure Record

The [S5 evidence](../etc/evidence/m0-t428-s5-worker-association.md)
records the initial e81e9fc9f increment and final transport-phase cancellation
repair. Pending DOS/native grants retain exact-process cancellation proof;
delivered or fully closed leases are removed. Source-owned command cancellation
is unchanged. Affected x86 builds, 27 service cases, six actual GUI cases,
Console17/Window17/WOW, paired loss/receipt1067, actual close/isolation and
published smoke/hash gates pass. No original mirror or protocol change.
Final reviewed P db855a66c is pushed to main/origin.
Unrelated planning edits remain separate. Format/ownership naming remains S6.

## S4 Closure Record

The [S4 evidence](../etc/evidence/m0-t428-s4-common-worker-shutdown.md)
records provenance, retained failures and the completed verification gates.
S4 production paths use worker-base's common local close mechanism; native
GUI-only workers register control before text binding. Original NTVDM cleanup
and real native Console closure remain worker-local. Affected x86 /MT CCPU40
builds, 37 shutdown assertions, 25 service cases, six GUI cases, paired
frontend-loss/receipt checks, native close isolation and failure tests passed.
Console17/Window17/retained WOW passed in r004. The eight-file tested package
equals O:/winnt/system32; recoverable S3 backup is in build/M0-T428/S4/r008.
Published Console/Window smoke and all eight hashes passed. Governance, links
and diff/original-mirror checks passed. Reviewed P 886f13758 is pushed.
T428 stays open; S5 is admitted and S6 remains pending.

## S3 Closure Record

The [S3 evidence](../etc/evidence/m0-t428-s3-authenticated-parent-resume.md)
records source boundaries, failed probe attempts and passing delivery gates.
Affected x86 /MT CCPU40 links, 23 service cases, Console17/Window17/retained WOW
frontiers, native-to-DOS-to-native Console/Window input and exit23 passed.
Run16 member-census/worker-selection routing is removed; NTSRV uses existing
authenticated origin and exact DOS completion with the existing I/O barrier.
Original mirrors and wire versions are unchanged. Eight-file publication
equals build/M0-T428/S3/r003/runtime/system32. Other-session proposal/queue
edits remain outside this delivery. P ae28ef7ce is pushed. T428 remains open.

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
reviewed delivery in the original S1 record; it is now included in pushed
S2 6a8934f6b. T428 remains open.

## Current Technical Baseline

Current verified/published package is the T428 S6 implementation from
build/M0-T428/S6/r002/runtime, with coherent S5 recovery in S6/r011/recovery. It retains
the following accepted T427 layout/protocol baseline, S2 native GUI-carrier
residency, authenticated service-owned parent restoration and common shutdown
execution with pre-text native close-control registration.

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
