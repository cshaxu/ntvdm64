# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | OpenNT DEM/COMMAND provider composition replacement | Replace or withdraw each registered legacy source-derived DEM/COMMAND runtime endpoint as one coherent OpenNT host-service composition. Preserve each original dispatcher’s ownership, call/return order, layouts, and failure outcome. Prefer independently composable original OpenNT providers; where historical CCPU/SAS/NT-host dependencies block that, record the exact edge and use only a contained CLI capability adaptation or a minimal source-derived rehost. Eliminate dual runtime paths; do not add trace-led endpoint patches, DOS/BIOS/device semantics, or host-invasive APIs to the adapter. | Per-component OpenNT dispatcher/module and ABI/failure map; exact composition-blocker record for every retained rehost; provider replacement-or-withdrawal matrix covering every endpoint in the registered legacy endpoint register; focused positive/negative fixtures; owner-approved first-profile capability and failure policy. |
| 2 | Read-only DOS namespace, file, and search continuation | Continue the contained host-capability family after the closed predecessor package: immutable drive policy, DOS-visible read-only namespace, file-token lifecycle, paired configuration materialization, and pathname/FCB search. Do not expose arbitrary host paths, mutation, guest-triggered rescans, or host pointers. | Completed provider-and-module map and relevant source/guest-ABI contract; owner-approved profile and failure policy. |
| 3 | Engine-composition integration | Materialize a tested CLI-to-adapter-to-Bochs launch contract for one selected DOS-engine bundle. Keep runner arguments distinct from stock Bochs arguments; keep Bochs configuration and firmware composition outside guest-service semantics. | A closed capability checkpoint, selected/pinned engine artifact, explicit cross-island ABI admission, and CLI integration evidence. |
| 4 | Reproducible real guest-path validation | Run the smallest real guest path through completed capabilities and classify every stop as guest, adapter service, Bochs mechanics, or unavailable host capability. A successful BOP hit alone does not establish continuous execution. | Runnable mechanics conclusion; immutable command, trace and log fixtures; byte/transaction correlation; explicit limitation classification. |
| 5 | First-profile capability checkpoint | Reconcile DOS/WOW16 artifact closure, reached and deferred BOP families, host capabilities, Bochs mechanics, adapter coverage, CLI composition, and real-path evidence. Decide whether the current first profile closes or which next capability family warrants admission. | Updated evidence-backed capability matrix with one classification per required item and an owner-approved next objective. |

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
