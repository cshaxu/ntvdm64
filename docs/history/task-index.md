# Legacy Identifier Index

## Status

The former `T001`--`T093` crosswalk was an invalid renumbering introduced
during the initial documentation migration. It is retired. `M0`--`M92`,
`G0.4`, and `R1`--`R5` remain historical record identifiers only; they are not
available `T` task numbers and do not define the current task sequence.

## Closed Numeric Tasks

| Task | Closure disposition | Record |
| --- | --- | --- |
| `M0 T95` | Closed by owner-authorized reprioritization. It delivered the Bochs/adapter mechanics, full BOP observation, selected source-led endpoint evidence, controlled terminal stop, and CLI child-EOF evidence. Its unfinished contained file/search and full real-path closure are transferred as explicitly uncompleted work. | [m0-t95-closure-20260811.md](m0-t95-closure-20260811.md) |

| M0 T96 S1 P1 | Complete static selector/service/BIOS provider and module map; no provider enabled. Local commit pending because no Git remote is configured. | [m0-t96-s1-p1-20260811.md](m0-t96-s1-p1-20260811.md) |
| M0 T96 S2 P1 | Shared BOP ingress classification and typed pass-through gate; provider registry and endpoint migration remain incomplete. Local commit pending because no Git remote is configured. | [m0-t96-s2-p1-20260811.md](m0-t96-s2-p1-20260811.md) |
| M0 T96 S2 P2 | Stable provider-family selection registry; direct provider linkage and legacy endpoint migration remain deferred to service planes and S8. Local commit pending because no Git remote is configured. | [m0-t96-s2-p2-20260811.md](m0-t96-s2-p2-20260811.md) |

## Current Numbering Authority

The current task sequence is governed by
[task-identifier-governance.md](../etc/operations/task-identifier-governance.md).
Its active implementation packet is **M0 T96**. New task numbers continue
strictly from the last closed numeric `T`; they are never inferred from legacy
`M`, `G`, or `R` labels.

Lowercase `r<n>` build and fixture labels remain legacy run evidence, not task
identifiers. Their T95 mapping is retained in the closed-task record and
identifier governance record.

## Historical Lookup

Use the pre-governance [status ledger](../planning/status.md) and the retained
evidence records to locate legacy work. The presence of a legacy identifier is
evidence of an earlier record, not proof that it is a current task, milestone,
subtask, or deliverable.
