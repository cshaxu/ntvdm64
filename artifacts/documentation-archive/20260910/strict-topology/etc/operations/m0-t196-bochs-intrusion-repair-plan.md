# M0 T196 Bochs/Adapter Boundary Repair Plan

## Outcome

T196 is a medium-sized, multi-subtask repair package. Its purpose is to
repair the complete project delta between the pinned Bochs 2.6 source and
`refs/bochs/`; it is not an audit package whose result is merely a document.
When it closes, every remaining Bochs delta must be either a justified,
selector-blind mechanical/minimal-embedding/default-off diagnostic exception,
or a separately recorded build-only compatibility correction. Guest-service,
BOP selector/service, OpenNT, DOS and SoftPC meaning must be absent from
Bochs and owned by the adapter's appropriate plane. Nonconforming deltas
must be removed or migrated, with source, build, and register evidence.

This package does not resume complete BOP execution. That is the next
unnumbered candidate T package in `docs/QUEUE.md`, admitted only after the
T196 closure gate passes.

## Subtask Sequence

| S | Outcome | Dependency | Exclusions |
| --- | --- | --- | --- |
| S1 | Immutable full-tree Bochs 2.6 diff, source identity and path/hunk manifest. | Pinned local Bochs 2.6 authority. | No classification decision, source change, build or runtime claim. |
| S2 | Complete per-hunk and per-register disposition plus a dependency-ordered repair design: retain, remove, migrate to adapter, or explicitly reject. | S1. | No speculative device/provider/BOP behavior and no implicit approval. |
| S3 | Repair deltas that improperly carry guest-service, BOP, OpenNT, DOS or SoftPC semantic selection in Bochs by removal or adapter migration. | S2 grouping and source ownership proof. | No new service/provider semantics and no reimplementation of Bochs mechanics in adapter. |
| S4 | Repair and minimize remaining Bochs-local mechanics, embedding and build-only deltas; retain only individually justified selector-blind contracts. | S2, and S3 where the same source/build surface is shared. | No optional device, firmware, plugin, GUI, host capability or OpenNT composition expansion. |
| S5 | Reconcile the complete post-repair upstream diff, exception register and generated/current build surfaces; resolve every residual hunk or re-admit it as a repair blocker. | S3 and S4. | No BOP runtime campaign. |
| S6 | Rebuild the admitted minimal closure and run focused boundary regressions proving Bochs is selector-blind and the adapter owns semantic routing/selection. | S5. | No continuous-guest-execution or BOP-completeness claim. |

Each S requires its own `STATUS.md` admission. S1 and S2 are complete; S3
closed the admitted retired/nonconforming diagnostic surface. S4 is the sole
active subtask. S5 and S6 are later repair/closure work, not BOP work and not
optional follow-up after an audit.

## Completion Gate

T196 closes only when the complete Bochs 2.6 diff has a recorded final
disposition, all nonconforming semantic changes have been removed or migrated
to `src/bx-ntvdm-adapter`, each retained exception has source/build evidence
and an exact register row, and focused checks show that Bochs sees only opaque,
typed mechanics. The following T may then take up holistic BOP host-service
and machine-composition runtime closure.
