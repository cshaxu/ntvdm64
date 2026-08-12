# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Post-target COMMAND lifecycle and no-next-command attribution | Establish why the normal-return path reaches `54:01` rather than `54:11`, and classify the original COMMAND/host lifecycle needed for a finite no-next disposition. | Source/trace map for `54:01`, its caller and no-next behavior, with explicit exclusion of generic machine BOP handling. |
| 2 | Engine-owned declared DOS terminal-result transport | Expose a captured, fixed-width DOS status to the CLI only after normal `54:11` return is runtime-proven and a distinct engine/Bochs transport ABI is designed. | A real normal-return trace, source/test proof of its continuation, and a fixed-width transport that does not overload process exit codes or introduce host broker semantics. |
| 3 | Finite subsequent declared-target envelope | Consider CF-set re-entry only as a separately declared finite sequence of guest targets, never as GetNextVDMCommand or an ambient host queue. | A source-backed guest continuation contract, immutable profile sequence ABI, and rejection tests for all undeclared targets. |

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
