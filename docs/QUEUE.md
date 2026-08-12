# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Post-observation machine BOP `02h` disposition | Classify and repair only the first source-proven failure exposed by the active observation; do not add a device or service by inference. | The active packet's single trace, recorded source/ABI evidence, and a newly admitted S brief. |
| 2 | Machine-BOP catalogue expansion | Classify the remaining BIOS-table machine selectors into original handler, native Bochs owner, unavailable, or collision dispositions before any invocation. | The machine-composition catalogue boundary and a completed `02h` execution classification. |
| 3 | CRT-island convergence audit | Decide whether the retained Bochs/MSVC image and original OpenNT object can share one supported CRT model before broader native execution. | The retained `LNK4098` warning, map/object directives, and an isolated toolchain audit. |

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
