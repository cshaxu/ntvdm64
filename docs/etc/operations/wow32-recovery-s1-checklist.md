# WOW32 recovery S1 execution checklist

## Role

Prepared for the queued [WOW32 proposal](../../proposals/proposal-wow32-complete-runtime-recovery-001.md).
This is an execution-ready planning seed, not S1 admission or audit completion.
Only Current admits a numeric packet. The [coverage ledger](wow32-recovery-coverage-ledger.md)
owns stable IDs, source/table/slot/data inventories and inherited evidence.
Use its E40/E41/E42/EH records before repeating research.

## Prepared inputs

- [x] Seed all 77 active C source names from the original sources manifest.
- [x] Identify all ten dispatch tables and proposed primary reviewers.
- [x] Record 21 input and 20 output assignments, including the explicit
  GetFullUserHandle placeholder and its S2 identity owner.
- [x] Carry forward 26 direct-data mappings and the 47-check binary verifier.
- [x] Separate confirmed desktop/handle/exit gaps from inherited fixture and
  build successes, with links to source/artifact evidence.

These ticks certify document preparation only. The audit gates below remain
open. No new production build, runtime test or complete dependency audit was
performed when preparing this seed.

## Audit gates

`tests/observation/verify-wow32-s1-formal-graph.ps1` is the reproducible
composition inventory used by A02--A03. It pins the 77 selected original
sources, 19 imported original owner objects, 21 target-local bindings, all ten
original thunk-table dispositions, named dynamic dependencies and the
source-wide guest-pointer macro surface. Its pass is static evidence only; it
does not change any later runtime, lifecycle or guest-acceptance gate.

[Normalized mirror-delta audit](../evidence/m0-t422-s1-normalized-wow32-mirror-delta.md)
separates the 68 newline-only mirror matches from nine real source deltas and
assigns every real delta to retain or minimum-diff cleanup. Its pending cleanup
is part of A03/A10, not a substitute for the owning runtime acceptance.

[Dynamic host-surface audit](../evidence/m0-t422-s1-dynamic-host-surface-audit.md)
adds actual x86 named/ordinal export checks for the selected dynamic DLL
families. It proves resolution on this host only; S7 owns complete runtime
resolution, marshalling, callback and teardown acceptance.

[Client-view contract audit](../evidence/m0-t422-s1-client-view-contract-audit.md)
proves that the full-size current allocation is not yet a desktop/window
producer, and records the exact S2 source-shaped publication/withdrawal
contract. Its explicit negative result prevents the current fixture-only view
from being promoted to WOW16 acceptance.

[Dispatch-definition audit](../evidence/m0-t422-s1-dispatch-definition-audit.md)
resolves all 1,786 selected table rows to their original local/unsupported/
host-thunk disposition and proves that all 668 selected thunks have original
selected C bodies. S1 still assigns their runtime contracts to S2--S7.

[Registration-surface audit](../evidence/m0-t422-s1-registration-surface-audit.md)
proves all original 21 input and 20 output slots are explicitly assigned,
while retaining `pfnGetFullUserHandle` as the one named S2 failure placeholder
instead of treating a non-null ABI table as callback acceptance.

| ID | State | Required result | Existing starting point / next action |
| --- | --- | --- | --- |
| A01 | Complete | Pin admission revision, actual DLL/worker composition, guest and three-application media hashes. | [T422 S1 baseline evidence](../evidence/m0-t422-s1-admission-baseline.md) pins all six immutable guest inputs, separates the old installed WOW32.DLL from the formal candidate, and records the formal local 20-slot composition check. |
| A02 | In progress | Every selected file, function/dispatch entry, export, resource, dynamic lookup and indirect callback assigned and classified. | All 1,786 dispatch rows now have source disposition and all 668 selected thunks have bodies; dynamic exports are host-verified. Complete function/resource/callback receiver allocation remains. |
| A03 | Pending | Exact original owner and four-rung disposition for every external edge; finite import/expansion/removal list with hashes and line counts. | Existing registers and E40 imports; inspect outgoing closure and autonomous duplicates before adding code. |
| A04 | Pending | Complete producer/consumer/update matrix for all guest-visible fields and object relationships. | DATA rows and E42 fault/layout proof; record authoritative facts, read/write direction, relocation, freshness and lifetime. |
| A05 | Pending | Demonstrate a feasible full desktop/WND/CLS/handle lifecycle without a second USER server. | E42 transport exists but producer is missing; establish real desktop/native-control/external-object coverage and valid phead identity. |
| A06 | Pending | Resolve callback, scheduler, queue and task/module/thread/worker cleanup order. | USER-TASK-EXIT-01; helper test is insufficient and WOWCleanup also serves live module release. |
| A07 | Pending | Experimentally resolve high-risk data freshness and reentry assumptions. | Test creation-time queries, host-originated changes without thunk entry, callback destruction, handle reuse and thread switching. Reuse valid tests; record remaining limits. |
| A08 | Pending | Complete dependency graph and executable per-family test plan across S2-S7. | Capability register; split OTHER-01 into named complete families and resolve S2/S4/S6 cycles before implementation. |
| A09 | Pending | Every modern unavailable dependency and original-guest limitation has a precise disposition. | Existing source policy: exhaustive non-invasive options, source-shaped failures, required mock/negative tests, TODO where applicable; no silent functional pass. |
| A10 | Pending | Review coherent implementation batches, immediate production wiring, exact scope and footprint report. | Preserve S1-S8 unless evidenced dependency regrouping is needed; quantify original imports separately from autonomous deletions. |

S1 closes only when every required edge has a concrete design disposition and
named implementation/test owner. Future runtime acceptance remains pending;
do not fabricate certainty. A design conflict that prevents a required
contract must be resolved before its dependent implementation proceeds.

## Experiment cards

These are bounded design experiments, not permission to implement a provider
one crash at a time. Before running, identify the contract, existing reusable
test, exact source/artifact inputs and expected observation.

| Card | Question and required observation | Owner after S1 |
| --- | --- | --- |
| X01 | Can the real desktop and required native controls have source-correct guest identity, pointer relocation and retirement? Observe original guest desktop/handle reads, not a dummy object success. | S2 |
| X02 | Are guest-consumed values updated at the required times while guest code does not call WOW32? Exercise clock/cursor and selected host-driven window changes; record ordering and freshness limits. | S2 |
| X03 | Does original guest callback entry see complete creation state, and can nested destruction preserve active borrows then invalidate stale identities? | S2 |
| X04 | Can original module release remain separate from thread/task retirement, including failure/retry and worker shutdown before backing release? | S2 |
| X05 | Can menu/submenu mutation and owner-draw values use the same publication/lifetime mechanism? Reuse E40 parser counterexample and recovery tests. | S4 |

## Required S1 delivery

- [ ] Coverage inventory reconciled with formal build and original selected
  profile; no orphan source, dispatch entry, slot or direct-data consumer.
- [ ] Every shared field group has producer, authority, mutation/retirement,
  synchronization, permissions and positive/negative evidence requirements.
- [ ] Every capability has original owner, minimal retained binding, removal
  candidates and a receiver; all S2-S7 dependency cycles explicitly settled.
- [ ] Highest-risk feasibility cards have evidence-backed conclusions; a
  failed experiment is preserved and its design conflict addressed.
- [ ] Source/import/hash/diff accounting and per-S test matrices complete;
  distinguish original imports, mirror modifications and autonomous code.
- [ ] Proposal and coverage ledger reconciled, governance/relative links and
  whitespace checked; no unapproved runtime scope or guest-media changes.
- [ ] S1 evidence reviewed and delivered under the admitted task's commit/push
  rules. This checklist does not authorize side-conversation Git mutations.

## Later S evidence contract

At each S admission, instantiate one bounded evidence record using the
coverage ledger's detailed-row template; link it in that ledger. Copy only
owned stable IDs, not the entire research chronology. Record Source, Build,
Wiring, Focused, Guest, Teardown and Delivery independently. Every live row
has a next action; blocked rows name the exact missing predecessor contract.

Implementation closure includes matching-artifact x86 verification, all 17
text-gated DOS regressions, family-specific normal/failure/reentry/cleanup
tests, mirror-format review and separate footprint accounting. All available
entrypoints are wired immediately. S8 requires WRITE/WINMINE/SOL interactions
plus per-family coverage, not only visible windows or exit codes. A failed
contract reopens its original ID with the counterevidence attached.
