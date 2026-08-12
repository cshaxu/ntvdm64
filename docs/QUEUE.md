# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Complete v5 runtime selection projection | Project validated v5 command placement, boot-file materialization and declared slot-zero compatibility fields into adapter runtime selection; retain the existing two-slot contract and boot namespace provider. | The completed bounded observation proves a `54:0C` loop before `54:01`; source audit identifies missing v5 selection fields, not a Bochs or BOP semantic gap. |
| 2 | Engine-owned declared DOS terminal-result transport | Expose a captured, fixed-width DOS status to the CLI only after normal `54:11` return is runtime-proven and a distinct engine/Bochs transport ABI is designed. | A real normal-return trace, source/test proof of its continuation, and a fixed-width transport that does not overload process exit codes or introduce host broker semantics. |

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
