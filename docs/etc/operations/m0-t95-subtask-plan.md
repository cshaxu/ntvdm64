# M0 T95 Subtask Plan

## Status

Supporting plan for the active `M0 T95` task package. It is not an active
packet, queue, or task allocation authority; `STATUS.md` owns the active S and
`QUEUE.md` owns future T candidates.

## Evidence And Subtask Sequence

| S | Scope | Disposition |
| --- | --- | --- |
| S1 | Bochs 2.6 backend admission and hard boundary | Historical evidence retained. |
| S2 | Frozen host-drive policy snapshot | Evidence complete; P lifecycle remains unbackfilled. |
| S3 | DOS search-family source/guest ABI contract | Evidence complete; P lifecycle remains unbackfilled. |
| S4 | Read-only host namespace projection | Evidence complete; P lifecycle remains unbackfilled. |
| S5 | Paired search transaction bridge | Evidence complete through the native-fixture gate. |
| S6 | Native minimal-target closure decision | Historical evidence retained; the current detailed ledger records its decision path. |
| S7 | Contained read-only guest-file capability | **Active.** See `../../STATUS.md`. |
| S8 | Bounded real-NTIO search trace | Future candidate within T95; admit only after the active S closes. |
| S9 | CLI engine materialization | Future candidate within T95. |
| S10 | Capability checkpoint and T95 closure decision | Final candidate within T95. |

The detailed former status content is retained in
`../evidence/m0-t95-status-ledger-20260811.md`. New detailed S evidence belongs
below `etc/evidence/` or `etc/research/` and is indexed there; it must not be
added as a second active packet in `STATUS.md`.
