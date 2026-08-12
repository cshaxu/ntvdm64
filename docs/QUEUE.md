# Queue

This is the authoritative ordered queue of **candidate T task packages**.
Candidates stay unnumbered until the owner admits one into
[STATUS.md](STATUS.md), at which point it receives the next global numeric T
identifier. The active package is recorded only in Status; its S-level plan is
supporting material, not queue content.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Runtime-capability continuation | Continue only the bounded, read-only DOS capability family after the active package closes or is explicitly re-scoped. | Completed active-package exit audit and an owner-approved next objective. |
| 2 | Engine-composition integration | Materialize a tested CLI-to-adapter launch contract for a selected DOS-engine input without widening the public CLI or Bochs boundary. | A closed capability checkpoint, selected engine artifact, and cross-island ABI admission. |
| 3 | Reproducible guest-path validation | Exercise a smallest real guest path through completed capabilities and classify every stop at the guest, adapter, backend, or host boundary. | Runnable mechanics conclusion and immutable command/trace fixtures. |

Queue order expresses priority, not approval, assignment, or a reservation of
numeric identifiers. Do not add S or P entries here; revise the active Status
packet when a candidate becomes an admitted T.
