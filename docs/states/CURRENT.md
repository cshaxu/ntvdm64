# Project Status

## Current Work

**Active: M0 T483 S14** (Ordinary Mode). S13 completes the actual physical-mapping
implementation migration. The broader C-layer cleanup remains active.

## Active Packet

| Field | Record |
| --- | --- |
| Original Owner Request | 实际公共实现及必要状态下沉B，不只是移动头文件或增加转发；不把NTVDM/NTSRV核心搬进B；按批准的S队列连续执行。 |
| Objective | Audit the complete responsibility and implementation ownership of ntvdm-exe, mvdm-host, non-mvdm and compat, including the historical session split; establish a file/symbol/state/lifecycle migration ledger before further relocation, then implement complete chains to eliminate all (B+C)-to-D/E/F implementation dependencies. B owns independent compatibility mechanics and resources; C retains original machine/subsystem algorithms; F owns product composition and lifecycle. No forwarding disguise; the owner-approved CCPU/SAS/USER adaptation exception permits B-to-C, never B-to-D/E/F. |
| Files And ABI Surface | Complete ntvdm-exe, mvdm-host, non-mvdm and compat inventory; historical session split, production compile/link providers and indirect calls. Inspect external consumers/providers as needed for complete chains without unrelated product expansion. |
| Reference Baseline | S13 P17; package92f469bf-1555-4ff0-b16d-ef2d9292934a; source/evidence in the linked closure. Direct ledger144 eliminated/424 open is not an implementation-migration count. |
| Applicable Rules | Six-layer architecture, original-first source policy, coding/execution/documentation rules and approved implementation migration. |
| Verification | Cached full x86/x64/reference builds; dual11-image publication and22 hashes; affected memory/lease/page-domain and isolated guest tests; governance and relative links; commit and push. |
| Asset Needs | Existing fixed OpenNT baseline, cached graphs and approved immutable guest fixtures. |
| Candidate Proposal | [T483 parent proposal](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Identifier Mode | M0 T483 S14, Ordinary Mode. |
| Admission And Approval | Owner approved sequential implementation migration on2026-11-19. S13 first-chain verification completed; S14 admitted at its closure delivery. Later S remain planned. |
| Non-goals | No guest modification/rebuild, whole-session/core migration, duplicate TLS/state machine, B-to-D/E/F dependency or forwarding-only completion. |
| Expected Markers | Actual inward implementations and state ownership, removed superseded providers/forwarding, retained original memory semantics and lifetime. |
| Similar-Issue Sweep | All acquire/use/release, thread and cross-image consumers of each selected chain; distinguish original C provider algorithms from common compatibility mechanics. |
| Reporting Requirements | File entry/exit, mirror points/lines and cumulative totals; direct edges, actual migration, deleted forwarding and remaining ownership counted separately. |
| Stop Conditions | Owner standing approval covers net non-error mirror increases below5 lines and at most2 points; larger increases or materially changed semantic/ownership boundaries require prior approval. B/C outward dependencies must be eliminated, not retained as exceptions. |
| Exit Criteria | Complete four-component ownership and migration ledger before further relocation; implement admitted complete chains with final-code targeted tests, coherent dual release and commit/push. Overall closure requires no (B+C)-to-D/E/F implementation dependency; unresolved dispositions are not elimination. |

## Current Technical Baseline

P81 [USER/page-domain batch](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-user-batch-owner.json)
migrated 125 source/header paths into B and eliminated 213 IDs.

P82 [USER tail](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-user-tail-owner.json)
moved three providers and two headers into B, eliminating 24 additional IDs.

P79 [inner lifecycle authority](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-worker-liveness-owner.json)
moved the worker-active query and sole lifecycle state to B. Ordinal 269 and one NTVDM TLS were preserved.

P83 [Console mouse ownership](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-console-mouse-owner.json)
moved mouse adaptation into B; C -8. Five isolated DOS mouse cases passed.

P90 [Console text](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-console-text-owner.json) delivered C -7.
P108 [WOW hook and task wait](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-wow-hook-owner.json) delivered C -2.
P109 [Native publication](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-native-publication-owner.json) delivered B -1.
P110 [Input terminal signals](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-input-terminal-owner.json) delivered C -1.
P111 [CCPU thread exit](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-ccpu-thread-exit-owner.json) delivered C -1.
P126 [C header closure](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-final-c-header-closure.json) delivered C -2.
P127 [CPU start ownership](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-cpu-start-owner.json) delivered C -1.
P128 [PIC import owner](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-interrupt-export-owner.json) moved ordinal 84 to C.
P129 [WOW trace module](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-trace-module-owner.json) closed B0036.
P130 [Resource receipts](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-receipt-owner.json) moved receipt storage to B.
P131 [Resource scope](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-resource-scope-owner.json) closed B0025/B0026.
P132 [VM IRQ owner](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-vm-notification-owner.json) moved PIC notification to B.
P133 [VM wake and input owner](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-vm-wake-owner.json) moved the queue to B.
P134 [Thread binding](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-thread-binding-owner.json) removed B0023/B0024 and three VDMREDIR F implementations.
P135 [WOW close result](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-wow-close-result-owner.json) removed C0462.
P136 [B routes](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-final-b-routes.json) removed B0010/B0021. C tracks both F routes.
P137 [WOW startup ticket](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-wow-start-ticket-owner.json) removed C0461.
P138 [Pending WOW exit](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-wow-pending-exit-owner.json) completed NTSRV retirement.
Delivered: B 0 known calls; C 9 direct calls, 1 runtime slot, 1 header, 1 image.

P75 [page-domain context ownership](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-page-context-owner.json)
moved page-domain identity and cleanup to B and removed three F calls.
The [compiled publication cut](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-publication-cut-impact.json)
prioritizes menu104/window103 reachable roots (overlapping); preserve original
metadata/commit ownership and capture/lookup failure ordering in the next cut.

Earlier P72-P74 deliveries restored nine native USER query/property/region
calls and moved independent dialog/failure implementations into B. Mirror
changes were zero; the current direct ledger is182 eliminated/386 open.
Their exact packages, source contracts and passed verification remain in
[P72](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-native-query-owner.json),
[P73](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-native-dialog-owner.json)
and [P74](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-native-property-owner.json).
S14 remains active; these deliveries do not close the coupled USER chain.

P71 [native capture extraction](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-native-capture-owner.json)
moves9 functions/320 body lines from F to B as a114-root prerequisite.
Public USER sampling/message operands/scratch normalization have no outer callbacks;
9 bodies are identical after4 private helper renames. Mirror delta0/0, direct-ID
delta0; guest mapping, retirement and publication coordination still require C ownership.
Cached x86/x64/reference builds, dual11 publication/22 hashes, native-chain controls,
21 reacquire cases, required-root, retirement/nested/terminal and8-phase Win16 dialog
pass, as do A/B compiled gates. Package5d88ed0b-2d52-4d41-9415-8b49f8bfa233.
Final document gates and enclosing P71 commit/push provide the delivery receipt.
P71 ledger was170 eliminated/398 open; P72 above supersedes these counts.

P70 [registry binding migration](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-registry-binding-owner.json)
places actual token/lock/cleanup implementation and the C-facing header in B.
Twelve provider IDs and C0114 are verified eliminated; mirror delta0/0.
Final cached x86/x64/reference builds, dual11 publication/22 hashes, registry
shutdown/persistence,31 embedding checks,8-phase Win16 dialog and A/B gates pass.
Package:aa150097-9a16-4557-aea0-d6ad74abb81c. Initialization now shares the
existing commit mutex after testing reproduced error32 against atomic replacement;
12 isolated double-writer repetitions pass. Ledger:170 eliminated/398 open,
43 supporting/6 eliminated-transitive/1 excluded. Final document gates pass; the enclosing P70 commit is the delivery receipt.
S14 and the114-root batch remain open.

[C adapter placement](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-c-adapter-placement-decision.md)
is superseded by the owner's S14 decision: use existing B compat for the
reviewed CCPU/SAS and original USER/task adapters, as explicit B-to-C
exceptions. No new C component; B-to-D/E/F remains prohibited. Actual
implementations, state and cleanup must migrate together, with named C
providers and callback targets. Product notifications stay F. Approval changes
placement, not the182/386 ledger or the P76 released package.

P68 [shared USER runtime ownership](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p68-runtime-owner.json)
places the actual runtime body/header in B, removes outward retirement callbacks,
and keeps explicit retirement/disposal composition in F. One worker TLS instance
serves EXE/DLL consumers. C0618 and C0018 are eliminated; 39 C compilation units
consume the B header. Non-error mirror delta: 0 points/0 lines.
Cached full x86/x64/reference builds, dual11 publication/22 hashes, lifecycle,
terminal schedule, CCPU/page-domain, immutable Win16 and architecture gates pass.
Package:360c2b7f-3879-4f20-9a36-55e8395fd196. Only NTVDM/WOW32 differ materially
from P65 after package identity/timestamp normalization. Ledger:157 eliminated,
411 open,43 supporting,6 eliminated-transitive,1 excluded. The 114-root batch
remains open; its scheduler and projection implementations have not moved.

P69 [compiled return/retirement cycle](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p69-native-cycle-ownership.json)
reconciles the114 roots with current P68 objects. The largest static cycle spans
37 root APIs and50 objects (23 F,27 C);362 COFF sections are not362 functions.
Source tracing reaches original control WM_FINALDESTROY through pending-window
retirement, consistent with pinned createw.c::xxxFreeWindow. This rejects a
pure-B wrapper/property move; it is not a runtime recursion defect or whole-file
migration approval. Production/mirror delta0; no further IDs eliminated.

Owner priority update: select complete batches by actual outward-dependency
removal count, not the previous chain order. The [batch census](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-batch-priority.json)
identifies a114-ID WOW API/discovery/menu cluster,39 console IDs and12 registry
IDs. These are candidates, not delivered removals. Review the114-ID cluster
first, including its scheduler/publication prerequisites; the unimplemented
CPU-context proposal is deferred. Mirror-cost approval rules remain in force.
The [114-ID ownership review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-wow-native-batch-ownership.json)
confirms original taskman scheduling, copied USER projection and one shared
EXE/DLL runtime as coupled prerequisites. Directly copying wrappers into B
would preserve forbidden callbacks. Concrete C carrier/mirror-cost work remains;
no additional dependency is counted eliminated by this review.
The [compiled relocation census](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-native-batch-relocations.json)
reaches44 F objects from the114 roots, plus54 C and9 B objects. This disproves
a three-file-only relocation, not the batch objective. Section reachability
is not runtime coverage or authority to relocate whole files.
The [runtime ownership cut](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-native-runtime-cut.json)
records both production unbind sites and41 test calls in13 files. Retirement
refusal retains TLS; process-disposal refusal after successful retirement must
retain registration without recreating TLS. B cannot retain an invoked task
retirement callback. Registration/storage must move with its complete callers.

P65 [WOW direct-pointer completion](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p65-wow-flush-owner.json)
removes the F empty provider into the existing C macro. Owner approved
non-error mirror +1 point/+2 lines. Cached builds and dual11 publication pass;
package d071bef3-e777-4e65-a2ee-c1563b18bc0e. SIM32, frame/abort, real CCPU/page-domain and immutable Win16 dialog tests pass.
C0456 is eliminated:156 eliminated/412 open; only WOW32 materially differs
from P63. Commit/push receipt is the enclosing P65 delivery commit. S14 remains active.

P64 [SIM32 contract probe](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p64-sim32-contract.json)
rejects a blind original-provider redirect: real CCPU/SIM32 evidence shows stale
video scratch overwrites direct writes. The prior CPU40 macro/cdecl rationale
is corrected. Research/test only; P63 remains the runtime package. C0456 stays
open and the direct ledger remains155 eliminated/413 open.

S14 P63 [callback ownership delivery](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p63-callback-owner.json)
removes C abort cleanup into the existing F CallBack16 wrapper, preserving
nested restoration/scope retirement. Non-error mirror7/26 ->3/9: -4 points/-17
lines. Cached full builds, dual11 publication/22 hashes, frame4/abort11,
real CCPU, WOW page domain and actual Win16 dialog pass. Package
73a9df4f-26a9-4e44-b8d1-744ac2a54da3; only WOW32 materially changes from P62.
Direct ledger155 eliminated/413 open. C0456 remains open; S14 is not closed.
The enclosing commit is the delivery receipt; commit/push verification follows.

S14 P62 [lease ownership delivery](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p62-lease-owner.json)
places machine coordination in C, independent resources in B and session
admission in F. B no longer stores CPU read/write callbacks; session.c no
longer links CCPU. Non-error mirror13/78 ->13/370: approved0 points/+292 lines.
Final cached builds and dual11-image/22-hash publication completed with package
8ec5f8b9-ea6f-43b8-9810-73d7592e7950. x86/x64 resource and coordination tests,
real CCPU, COMMAND, WOW, immutable Win16 and DOS4GW targeted checks pass.
DOS DIR lifecycle passes; date-rendering diagnostics also reproduce on P26
and are separately recorded in TODO. P62 verification/publication complete;
commit/push receipt is the enclosing delivery commit. Direct ledger152 eliminated/416 open is unchanged;
this bounded coordinator delivery does not close S14 or all ten chains.
Temporary status-size exception: retain indexed S14 history until chain handoff;
this file remains below the 32 KiB hard limit.

Earlier delivered baseline:

S14 P26 moves copied-window graph validation, its relation ABI and scratch
cleanup from F/D to B. No outward callback or new B-to-C exception is added.
The24-operation whole-domain candidate remains experiment evidence only.
Cached full builds, dual11 publication/22 hashes,11 unit cases, actual CCPU
page-domain/graph-snapshot fixtures and immutable Win16 dialog acceptance pass.
Package e1fc8800-0ebc-412b-8498-22d1313dd486 is published. Mirror delta0/0;
cumulative remains-1 point/+43 lines. Direct ledger150 eliminated/418 open
is unchanged: this is implementation migration, not whole-domain closure.
[P26 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p26-wow-window-graph-owner.json)
records the failed observer invocation and corrected final-package test.
WOW CPU/guest-memory-dependent implementation remains; S14 is active.

S14 P25 completes the XMS-specific outward-provider cleanup. Descriptor reads
use B; range validation and forward copying use C; invalid transport enters
the existing C termination chain. F cancellation/copy providers are removed.
Owner approved the worker-fatal policy and non-error mirror increase
5 points/29 lines to6/33. Cumulative net is-1 point/+43 lines. Cached full
builds, dual11 publication/22 hashes and targeted tests passed. Package
8b383dfc-9e18-4c1a-ace8-f449a81f2669 is published. Real BOP probes prove
worker exit487, launcher error1067, no guest continuation and successful
subsequent starts. Full was not run under the owner's small-delivery scope.
[P25 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p25-xms-owner-delivery.json)
closes C0040/C0224/C0257:150 eliminated/418 open of618. Generic F lifecycle
authority and WOW USER/page-domain ownership remain; S14 is still active.

Earlier delivered S14 memory work remains recorded in
[P24 SAS ownership](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p24-sas-provider.json),
[P22 EMS header cleanup](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p22-ems-header-delivery.json),
[P21 shared clock](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p21-wow-clock-implementation.json),
and [P20 memory access](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p20-memory-access-implementation.json).
These preserve package/test identities and mirror accounting. They do not
prove absence of indirect provider dependencies; the expanded ownership
review explicitly includes the B lease callbacks to C SAS.

The owner-directed four-component audit now precedes further relocation.
The [B implementation ledger](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-compat-implementation-ledger.json)
covers all49 C implementation files with complete body reads, including
shadow registry. It does not establish full caller/lifetime validation.
Private service, thread and memory callbacks remain explicit unresolved
ownership chains; no dependency count or production baseline changes.
[Coverage and session history](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-four-component-ownership-baseline.json)
and [callback chains](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-callback-provider-ownership.json)
record source findings, not migrated-code or runtime acceptance. The complete
migration ledger remains unfinished; no production change follows this review.
The [WOW resource/callback review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-wow-resource-callback-ownership.md)
classifies 59 complete provider bodies and their ownership constraints.
Private USER/CPU operations cannot move wholesale to B; class publication,
abnormal callback return and message reentry retain explicit ordering duties.
These are source findings, not eliminated dependencies or runtime verification.
The [provider crosswalk](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-open-provider-crosswalk.json)
maps all418 open records:345 symbols from79 provider files,60 headers,
12 include roots and1 image import. The WOW record covers51 of those79 files
and260 symbol records, plus8 associated bodies; no row is marked eliminated.
[CPU/execution review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-cpu-execution-provider-ownership.md)
and [device/presentation review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-device-presentation-provider-ownership.md)
and [request/lifecycle review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-request-lifecycle-provider-ownership.md)
and [Console ownership review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-console-provider-ownership.md)
and [complete page-domain review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-page-domain-complete-ownership.md)
add28 providers/85 symbol records: combined79/79 files and345/345
symbol records. C descriptor/IRQ carriers are identified; initialization and
thread-exit orchestration still need lifetime proof. No production changes.
P52 [inventory reconciliation](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-four-component-reconciliation.json) confirms all1494 inventory file hashes unchanged.
The existing migration plan groups ten chains; exact C carriers and outer-policy
control-flow separation remain unfinished. Provider coverage is not whole-root closure.
P53 [storage-owner design](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-original-storage-owner-design.json) specifies nine symbol rows: B suffix/capture resources and C disabled hook storage.
Mirror cost and final ABI candidates remain to validate; statuses stay open.
P54 [C caller reconciliation](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-c-caller-reconciliation.json) covers190 prior callers:179 source-unchanged,11 cumulative deltas reviewed.
COMMAND/DPMI/VM header forwarding remains unresolved, separately from delivered SAS/XMS/mapping implementations.
P55 [lease split design](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-lease-no-callback-design.json) preserves hydration, consume-on-failed-commit and copy-retirement locking while removing B callbacks to C.
Eight location access operations also require C ownership; exact ABI/mirror candidate remains pending.
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

[Approved S worklist](../etc/operations/t483-compat-implementation-migration-plan.md):
S14 remains active for all B/C to D/E/F implementation edges. S15 covers D to
E/F, S16 covers E to F, and S17-S19 cover the three internal reverse edges in
order. The former functional work groups feed S14 outward cleanup.

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

P56 [XMS header reconciliation](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-p56-xms-header-reconciliation.json) corrects C0048, removed in P25 but omitted from ledger closure.
Current counts supersede the historical figures above:151 eliminated/417 open of618;
open coverage is345 symbols,59 headers,12 roots and1 image import. No product change.

P57 [header contract ownership](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-header-contract-ownership.json) classifies56 of59 open headers with their implementation chains.
Three generated headers,12 include roots and the image import remain to reconcile;
exact migration carriers/ABI and mirror cost are not yet complete. Counts unchanged.

P58 [build/ABI reconciliation](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-build-abi-boundary-reconciliation.json) traces12 include roots and12 VCDEX ordinal imports to C providers.
Generated headers retain provenance/layout constraints; C0137 has no current
C-command hit and needs historical reconciliation. No dependency status change.

P59 [COMMAND root/lease contract](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-command-root-and-lease-contract.json) closes stale C0137: current ledger152 eliminated/416 open of618.
The proposed callback-free lease ABI preserves hydration/consume/retirement;
reservation reentry and exact C mirror cost still need evidence. No product change.

S14 P60 [readiness and counts](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-migration-readiness.md)
assigns all416 open IDs exactly once to ten existing chains. Provider body
coverage79/79 and header classification56/59 remain source-review counts.
Whole-chain candidate readiness is0/10; exact destinations, control-flow
separation and mirror cost remain required. No production changes or new
eliminations;152 eliminated/416 open remains current.

S14 P61 [generated-header disposition](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-generated-header-disposition.json)
completes header responsibility classification59/59. Three original-carrier
proposals preserve the selected CPU/SAS ABI but still require layout/selection
proof, final mirror measurement and policy approval. No production change;
152 eliminated/416 open, whole-chain readiness0/10.
