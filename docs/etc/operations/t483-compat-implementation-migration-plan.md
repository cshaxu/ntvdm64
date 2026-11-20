# T483 compatibility implementation migration plan

## Authority and objective

Owner approval on 2026-11-19 directs updating the current proposal, creating
the sequential S plan, updating the objective and beginning implementation.
This is the owner's local retro-computing compatibility project. Work is on
the named repository sources and isolated test-owned processes. Preserve
original behavior and immutable guest binaries; no Computer Use.

[Current](../../states/CURRENT.md) is the sole active packet.
[Proposal](../../proposals/proposal-unconnected-mirror-diff-cleanup-001.md)
records the approved scope. This worklist belongs to the current T, not the
candidate T Queue. Only the current S is active; later S entries are planned.

Move actual reusable implementation, necessary state and cleanup to B.
F may compose B-owned objects and decide product creation/destruction, but
must not remain the implementation provider behind a nominal B wrapper.
Keep original algorithms in C and explicit product policy at its product
owner. Preserve the two existing B-to-C exceptions. The later S14 owner
approval additionally admits the reviewed CCPU/SAS and original USER/task
adapters in B as explicit dependencies on C; it does not permit D/E/F
providers or moving original algorithms out of their mirrors.

Owner clarification: do not relocate the NTSRV/NTVDM core into B. For mapping,
B owns records, address conversion and allocation/release; F decides worker
creation, request selection and disposal timing. Request queues, DIRECT and
OBSERVED identity, waiter completion, Run16 reentry policy, session completion,
cancellation and termination escape are not B migration candidates. Extract
only the minimum binding needed by the selected public capability, not a
generic replacement session manager or service transport. If a candidate
requires moving that core merely to satisfy layering, redesign the boundary
instead of widening B.

## S14 owner-approved placement after P77

Owner: "批准放进B里，作为例外依赖C". The existing compat component is
now the destination for reviewed machine/subsystem-dependent adaptation.
This supersedes the C-only carrier restriction in the earlier chain-2/3
planning table below, not the algorithms' original ownership or verification.
No new C component is admitted. Remove actual D/E/F implementation dependencies,
including cleanup callbacks; keep product launch notifications and worker policy
outside. Record concrete B sources and C providers in the migration ledger.
The first coupled batch covers page-domain state/publication/reclamation and
its window/class storage consumers, followed by native-call scheduling handoff.
Do not count relocated headers or a B-to-F forwarding contract as closure.

## Baseline and accounting

### Owner revision: complete four-component ownership audit

Before further production relocation, audit all tracked files in ntvdm-exe,
mvdm-host, non-mvdm and compat and reconcile their implementation owners.
The existing S13-S19 worklist remains a grouping of functional chains, not
permission to skip the complete inventory or to declare unresolved outward
implementation dependencies closed. Final closure requires eliminating
C-to-D/E/F implementation dependencies, including forwarding through B.

Keep the existing C identifiers and historical evidence. For every reviewed
chain record callers, actual providers, state, creation/use/failure/destruction,
destination, superseded forwarding, mirror point/line impact and verification.
File presence, compile selection and live product use are separate facts.
An old eliminated include edge is not proof that its implementation moved.
Track public runtime module protocols separately, without using that category
to exempt private provider calls. Retain the existing two B-to-C exceptions
and record each adapter under the later explicit admission. The rejected
24-operation callback facade remains rejected: move actual implementations
and use their C providers directly, rather than install outer implementations.

Reconcile the historical src/session split against current compat and
ntvdm-exe/session bodies. Reuse the one existing host-context binding and
cleanup mechanism; do not create a third session abstraction. F retains
product identity, cancellation, completion, thread admission and disposal
timing. B owns independent compatibility resources; C owns original machine
operations. Shared transition state must be reviewed with every consumer,
not moved as an isolated header or opaque F pointer.

Task-local reproducible coverage inventory:
build/t483-s14-architecture-inventory/inventory.py. Its files.jsonl records
each tracked path/hash and references to existing ledger IDs. Its summary
is coverage evidence only; it marks no source file or dependency as reviewed.

Delivered baseline: 0c165f5a4, package137949a3-b4bd-457d-82c4-1b455762eb17.
The old direct-edge ledger has618 IDs:144 eliminated,424 open,44 supporting,
5 eliminated-transitive and1 excluded. Those counts describe direct edges,
not actual implementation migration. Do not rewrite historical delivered
results; separately track implementation ownership and residual forwarding.
Previously installed COMMAND, Console, DPMI, VM, guest-location and physical
mapping contracts are in scope even if their direct IDs were eliminated.

The unpublished keymouse experiment only added a forwarding callback and
filtered include roots. Its build.exit is0; it was not published, exercised,
committed or accepted. Preserve its patch and hashes, then withdraw only that
experiment before starting the approved implementation migration. It is not
a new accepted baseline and its cached objects are not release evidence.

## Sequential S worklist

| S | Bounded scope | Completion evidence |
| --- | --- | --- |
| S13, closed by P17 | Extract the minimum shared instance/thread binding needed by physical mapping; move the mapping implementation, records and cleanup into B as the first complete chain. | Production C calls B implementations; old F mapping provider and eight-operation forwarding table removed; one binding authority, correct bind rollback/unbind/dispose and instance isolation. |
| S14, admitted after P17 | Remaining guest-memory access/lease, XMS and WOW page-domain adaptation. | Each complete acquire/use/release chain has one state owner; C algorithms retained, F owns configuration and product lifetime only; no remaining F implementation hidden behind these B contracts. |
| S15, planned | Console compatibility, keyboard/mouse state and text/graphics adaptation. | Common operation bodies/state/cleanup move inward; frontend channels and presentation policy stay external; switching, input, cleanup and isolation validated. Classify C-owned IRQ helpers for S19 instead of wrapping them. |
| S16, planned | COMMAND capture/environment/directories/handles and CSR/BASE boundary separation. | Common mechanics execute inward; NTSRV retains request identity, authorization, scheduling, completion and waiter ownership. Resolve D transport dependencies by explicit ownership, not another B forwarding table. |
| S17, planned | Redirector, async device and printer compatibility. | Common guest-copy/device operations and cleanup migrate with their state; product connection and authorization policy stay external; real selected caller and provider tested. |
| S18, planned | WOW native-object, class, message, control/resource, clipboard and callback adaptation, in complete subgroups. | Native compatibility implementations have inward ownership; original C algorithms remain C; no private C-layout dependence silently added to B; lifecycle/reentrancy and relevant real WOW witnesses pass. |
| S19, planned | Original CPU/SAS declarations, effective-address/descriptor/vector/IRQ ownership and final reconciliation. | Original-owned code recovered at its proper C owner where justified; actual compile/link and runtime service ledgers reconciled; all remaining outward implementation dependencies have explicit unresolved or owner-approved dispositions. |

The former S14 original-owner allocation moves to S19. Its evidence and
stable IDs remain intact. If an earlier group needs a C correction to complete
its chain, record the exact prerequisite in Current; do not defer a current
correctness defect or introduce a callback solely to bypass it.

## Migration method

1. Inventory each function, state object, creation/use/destruction caller,
   selected build owner and test. Follow the complete chain; using session
   today does not prove the operation belongs to F.
2. Move a public compatibility operation and its state together to B. F
   holds/initializes that B object and chooses product lifecycle timing.
   B must not know a private F session layout or call a F state accessor.
3. Reuse and relocate the existing binding mechanism. Do not duplicate TLS,
   add a second current-instance registry, invent an independent session
   lifecycle or disguise session pointers as void pointers. C's original
   no-context signatures must retain a coherent binding and error contract.
4. Keep real original C algorithms in C. A C-to-F helper that only calls C
   is not a reason to add a B callback. The admitted B adapters may consume canonical C layouts; do not create
   duplicate layout definitions or move the original algorithms.
5. Remove superseded F bodies, service-table entries, installation code and
   unused tests only after the replacement covers the same production chain.
   Product policy that cannot move is a separate explicitly tracked boundary,
   not a claimed implementation-migration success.
6. Process coupled files completely before building. Report mirror points
   and lines before/after; preserve original logical lines and #error arms.
   Request approval before increasing mirror difference points or lines.

## First chain: concrete investigation

Sources: ntvdm-exe/session/session.[ch],
ntvdm-exe/softpc/mvdm_softpc_physical_mapping.[ch],
ntvdm-exe/softpc/mvdm_physical_mapping_binding.c,
compat/physical_mapping_services.c and its headers, and selected callers.

Current mapping implementation owns two process lists tagged by session.
Eight operations call session_thread_current; initialization/map register
session teardown. The F binding table is immutable, but does not own the
mapping state. Session binding currently has one TLS owner; bind publishes
it before hooks, failed hooks roll back, failed unbind retains binding, and
dispose rejects live bindings. Preserve that behavior in the extracted
mechanism; do not assume callbacks have returned or threads have joined.

First inspect all consumers and cross-image imports before selecting the
minimal B context and its embedding in F session. Preserve pointer ownership,
calling convention and external symbols where possible. This planning
observation is not yet a final ABI design or runtime proof.

## Verification and delivery

For each group: cached full supported x86/x64 and reference builds, publish
the coherent eleven-image package to assets/release and O:/WINNT/system32,
verify all22 hashes, run affected focused tests, commit and push. Default
runtime scope is targeted, not Full, under the owner's current instruction.
Source movement alone is not proof when binding or state ownership changes.
Do not use old candidate test/build receipts for changed final source.

The first chain must cover distinct instances, repeated binding, failed bind
rollback, failed unbind retention, teardown, mapping aliases/bounds and
existing original-memory/CCPU/DPMI consumers. Reuse existing fixtures and
isolated DOS/NESticle acceptance, without copying guest files into Git.
Retain actual failures and correct their causes; do not weaken assertions.

Per-file entry/exit reports retain the established point/line and cumulative
mirror statistics. Also report implementation relocation, deleted forwarding,
remaining outward providers and delivery status separately. A reduced direct
include count is not completion. Sync the pending mirror decision ledger
and dependency ledger without losing stable IDs or historical evidence.

The [first-chain inventory](../../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-implementation-migration-first-chain.json) pins source hashes and all eight operation locations, binding invariants and test gaps. It is source evidence, not a delivered migration.

[S13 closure](../evidence/m0-t483-s13-implementation-closure.md) records the completed first chain and its limits. Current now admits S14; subsequent rows remain planned.

## P52 consolidation: execution owners, not header destinations

Source basis: complete direct-provider body coverage at P51 (79 files,
345 symbol rows), the 49-body compat ledger, historical session review and
four-root inventory. These establish the following chain decisions. They do
not establish that every source in the four roots has been fully audited.
No new callback exception or production relocation is admitted by this table.

| Order / chain | State and execution owner | Required change, superseded indirection and validation |
| --- | --- | --- |
| 1. Guest access and leases (S14) | C owns translation/SAS effects; B owns host buffers and borrow lifetime; F selects product lifetime. | Separate lease storage from C reads/writes: C performs machine access at explicit acquire/commit boundaries, B must not call installed C access callbacks. Reconcile all direct and indirect consumers before changing the ABI. Preserve partial writes, failure and borrowed-view lifetime; never replace SAS with raw memcpy. Remove the old provider slots only after callers use the final chain. |
| 2. WOW machine context (S14 prerequisite) | C owns CR3/CR0/GDT and guest mapping; B owns existing shared clock; F admits startup and chooses quiescent teardown. | Resolve existing original C carrier and measure mirror cost before implementation. Remove F leave/reenter providers and their B DPMI table entries when C owns the actual transition. Preserve DOSX GDT restoration, A20 precondition, failed initialization cleanup and protected/real-mode round trips. No whole-domain 24-operation B provider table. |
| 3. USER projection and retained generations (S14/S18 dependency) | C owns original USER layout and guest graph interpretation; native USER owns native objects; independent copied-graph validator already B. | Keep one backing/borrow/retirement authority. Decide original C insertion points jointly with WOW consumers, not a 2673-line transplant into an unrelated mirror file. Preserve pinned old generations across handle reuse, logical withdrawal before physical free, class reachability and atomic graph/menu commit. Host-storage deallocation must not become arbitrary outer reentry. |
| 4. Console mechanics (S15) | B owns independent buffers, native identity/conversion and cleanup; E/F own authenticated endpoint and presentation transitions. | Extract actual resource bodies with their state. Four Console callbacks currently installed from F remain violations until routed operation ownership is resolved. Do not substitute local Console APIs for the selected frontend. Preserve release barrier, producer join, input receipts, layout/code page and off-screen/partial transfer semantics. |
| 5. COMMAND capture (S16) | B owns bounded copied command/environment/handles, capture epoch and borrowed views; C owns DOSWOW/CDS interpretation; F owns launch policy. | Move capture storage and implementation using the existing B context, not a session pointer accessor. C supplies machine-derived data through explicit operations; remove native-child forwarding entries after migration. Preserve single active ordinary capture and separate explicit VM-capture lifetime. Capture finish is not task completion. |
| 6. Launch, CSR and request completion (S16) | C owns original classification and retained record algorithms; NTSRV owns request identity/authorization/completion; D/F own transport and composition. | Split product policy at its real call boundary. cmdCreateProcess's apparent CreateProcess call currently reaches Run16/frontends/capabilities; moving that wrapper into B is forbidden. Preserve suspended-child handle ownership, reentry counters, receipt generation, return-code/waiter association and rollback handoff. The exact synchronous boundary is a design obligation, not permission to add callbacks. |
| 7. Thread and lifecycle binding (S16 prerequisite) | Existing B host_context owns binding/resource mechanics; C owns CPU-thread state; F owns thread admission/cancellation and final product completion. | Reconcile WOW bind/unbind hooks and exception exits before removing D bridge/F providers. No second TLS, no whole-session move. Failed unbind with live USER state must retain ownership; cleanup waits for users, never guesses quiescence from timeout. |
| 8. Redirector, printer and devices (S17) | C owns segment/VDM_TIB/CPU-device semantics; B owns native device or buffer mechanics; outer code owns connection admission. | Reuse delivered SAS and lease implementations; eliminate F-to-C guest-copy detours. Preserve chunked partial-copy and OEM bounds. Resolve printer NtVdmControl close provider without restoring hardware-V86 monitor. IRQ/reset helpers belong with original C device providers, not B callback facades. |
| 9. Native USER edges (S18) | B owns SDK-only operations with explicit inputs; C owns private WND/CLS/FNID and original callbacks; F composes product runtime. | Migrate complete object/class/message/resource subgroups; retain exact-generation references through callback reentry. Keep native object lifetime distinct from guest projection. Do not rewrite retained original controls wholesale using modern controls, or move private C layouts into B. |
| 10. Original-owned helpers and final reconciliation (S19) | C owns descriptor/effective-address/vector/IRQ operations and original storage; product entry points stay F. | Recover at actual original carriers, minimize mirror changes and remove old F providers. Suffix descriptors, CsrPortHeap and optional hooks are separate from Run16/NTSRV main bodies. Recheck final build/link providers, forced/transitive includes, imports and residual service installations. |

An independent host resource can move to B without moving its product owner:
F acquires admission, calls the inner operation, and chooses when to release
it. Conversely, a C caller that synchronously invokes product policy cannot
be fixed by placing a typedef or forwarder in B. Such a call needs a real
control-flow separation, or a change in the calling code's ownership. The
final design must show that separation and its original-semantic cost before
production changes; the current inventory does not yet justify one universal
adapter for every operation.

### Finite remaining design checks

1. Reconcile the four-root file inventory against all body/chain evidence,
   including already eliminated direct edges. Non-observed compilation is not
   proof of irrelevance; no file is deleted from this audit on that basis.
2. Resolve the 59 header, 11 include-root and one image-import records against
   their current actual users; their absence from the 79-provider body list
   must not hide them. Preserve the 416 stable open IDs until proved removed.
3. For each chain above, assign exact original C carriers or B destination,
   list all state/lifetime callers and removal sites, then measure candidate
   mirror block/line delta. Any increase still needs the owner's approval.
4. Reconcile indirect callbacks and teardown providers with these same chains;
   do not count callbacks as new approved exceptions or as eliminated edges.
5. Only after the complete migration ledger is ready, implement each admitted
   whole chain and run the required cached build, publication, targeted tests,
   commit and push. Body review, design readiness and delivered elimination
   remain separate states.
