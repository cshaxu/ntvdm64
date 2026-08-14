# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Engine-owned declared DOS terminal-result transport | Expose a captured, fixed-width DOS status to the CLI only after normal `54:11` return is runtime-proven and a distinct engine/Bochs transport ABI is designed. | A real normal-return trace, source/test proof of its continuation, and a fixed-width transport that does not overload process exit codes or introduce host broker semantics. |
| 2 | OpenNT DPMI host-composition recovery | Recover one complete, source-derived DPMI host composition for a declared guest profile, without exporting Bochs CR0/LDT internals or reimplementing DPMI in the adapter. | The [DPMI composition proposal](etc/research/proposal-opennt-dpmi-host-composition-001.md), a selected first DPMI guest profile, source/ABI/failure mapping for its session, and separately admitted native prerequisites or explicit profile exclusions. |
| 3 | WOW16 guest artifact and host-composition closure | Establish the guest NE/DPMI artifact inventory and the complete WOW host composition before any Win16 runtime claim. | The [WOW16 composition proposal](etc/research/proposal-wow16-guest-host-composition-001.md) source/ABI/failure map, artifact classifications, a bounded non-GUI NE fixture plan, and explicit GUI capability disposition. |

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
