# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Bochs 2.6 intrusive-change reconciliation | Compare every project modification below the pinned `src/bochs/` import with the declared original Bochs 2.6 baseline. Classify each diff hunk as removed, retained generic mechanics/default-off diagnostic, migrated adapter ownership, or an unapproved semantic intrusion; then update the intrusive-change exception register for later removal or settlement. Do not introduce new Bochs behavior, normalize upstream code, or treat an unreviewed local patch as an accepted exception. | Pinned upstream source identity and revision; reproducible full-tree diff/patch manifest with path and hunk classification; one register disposition per modification; focused source/build checks for every retained exception; and an explicit owner decision for every unapproved or unclassifiable change. |
| 2 | Holistic BOP host-service and machine-composition runtime closure | Continue the complete BOP route only after the convergence-repair T closes: adapter-owned selector/service routing and provider disposition, plus adapter-owned machine-composition selection for admitted historical handler islands. | Closed convergence-repair T; source/ABI ownership map; one bounded observation with no selector semantics in Bochs and no claim beyond observed execution. |
| 3 | Engine-owned declared DOS terminal-result transport | Expose a captured, fixed-width DOS status to the CLI only after normal `54:11` return is runtime-proven and a distinct engine/Bochs transport ABI is designed. | A real normal-return trace, source/test proof of its continuation, and a fixed-width transport that does not overload process exit codes or introduce host broker semantics. |

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
