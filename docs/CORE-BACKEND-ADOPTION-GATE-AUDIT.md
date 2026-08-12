# Core Backend Adoption Gate Audit

Status: M36 read-only audit, 2026-08-08. This compares the current `ntvdm64/core` interface to M35. It changes neither repository.

## Matrix

| Requirement | Current evidence | Status | Future path |
| --- | --- | --- | --- |
| Initial real-mode entry and preload | `core_machine_apply_entry_plan` validates stopped state, real-mode registers, non-overlapping ordinary-RAM preloads, entry route | adequate for initial entry | mantle composes an immutable plan; this is not a runtime service-write API |
| Checked physical memory | `core_machine_memory_read/write/query` and `ORDINARY_RAM`/`PROVIDER` classification | adequate prerequisite | all later operations use this one route |
| Frozen ROM/device topology | mapped provider routing is frozen before execution | adequate prerequisite | reject provider, ROM, and hole transaction targets |
| Runtime multi-span publication | public single-span `memory_write` only | missing | generic atomic ordinary-RAM transaction |
| Rollback guarantee | no external transaction/rollback contract | missing | generic transaction with an independent consumer |
| Copied GPR/FLAGS transition | `undefined_instruction_transition` copies GPRs, flags, CS:EIP with finite outcomes | partly adequate | retain constrained response semantics |
| Historical marker observation | transition dispatch is real-mode `#UD` only | missing | neutral configured decode-point trap |
| Copied DS/ES/SS and address context | current transition input has only CS:EIP/GPRs | missing | bounded copied context or normal translation request scoped to a paused epoch |
| CPU-mode safety | current response cannot patch mode or segments | adequate | preserve this invariant |
| Session transaction lifetime | lifecycle has stopped/paused but no transaction epoch | mantle responsibility | mantle invalidates work on resume/reset/fault/stop |
| Opaque host resource input | M27 proves a contained external fixture; core has no DOS file policy | outside core | mantle binds opaque capability; adapter/profile owns containment |
| Historical ownership | M34 compiles original command group; M29/M31 retain original boundaries | source evidence only | wait for missing backend operations |

## Result

The core can support a future initial private entry plan and safe physical observation. It cannot yet support a historical prefix trace: the required marker is not guaranteed to raise `#UD`, and no all-or-none runtime transaction can publish image or record writes after a transition.

The non-invasive path is:

1. obtain an independent consumer, or a core-owner decision, for a generic decode-point transition and ordinary-RAM transaction;
2. implement and test those neutral capabilities in `ntvdm64` under its own governed task sequence;
3. bind mantle's epoch, translation, and capabilities to that public contract; then
4. let the external adapter perform a bounded prefix trace with original-chain stops and redacted observation only.

## Explicit Non-Paths

- Do not turn an accepted instruction into `#UD` to route it through the current hook.
- Do not expose a `GetVDMAddr`-style pointer, generic RAM map, or writable provider callback.
- Do not approximate atomicity with repeated `memory_write` calls.
- Do not use a self-authored handler or synthetic DOS result to bypass a missing transition.
- Do not move PIF, console, OEM path, redirection, or native-process policy into core to satisfy an old command import.

## Decision

The core backend is **not yet adoptable for an executable adapter prefix**. It is nevertheless the viable target architecture: entry plan, checked memory, frozen topology, and constrained response rules are the foundation for the two missing generic capabilities. M37 should create a governance handoff record; it authorizes no code change.
