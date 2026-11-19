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
owner. Preserve the two existing approved B-to-C exceptions; do not expand
them as a shortcut.

Owner clarification: do not relocate the NTSRV/NTVDM core into B. For mapping,
B owns records, address conversion and allocation/release; F decides worker
creation, request selection and disposal timing. Request queues, DIRECT and
OBSERVED identity, waiter completion, Run16 reentry policy, session completion,
cancellation and termination escape are not B migration candidates. Extract
only the minimum binding needed by the selected public capability, not a
generic replacement session manager or service transport. If a candidate
requires moving that core merely to satisfy layering, redesign the boundary
instead of widening B.

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
to exempt private provider calls. Retain the two approved B-to-C exceptions;
the rejected 24-operation WOW machine callback candidate is not admitted.

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
   is not a reason to add a B callback. B must not absorb private C layouts.
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
