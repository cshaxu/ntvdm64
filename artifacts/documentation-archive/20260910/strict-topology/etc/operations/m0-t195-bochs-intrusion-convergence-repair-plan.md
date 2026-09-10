# M0 T195 Bochs Intrusion Convergence Repair Plan

## Outcome

Repair the external Bochs/OpenNT intrusion surface so the adopted machine
retains only selector-blind mechanics and minimum embedding, while
`src/bx-ntvdm-adapter` owns all guest-service and historical machine-handler
selection. Completion is a repaired source/build boundary, not a BOP runtime
success claim.

## Subtask Sequence

| S | Outcome | Dependency | Exclusions |
| --- | --- | --- | --- |
| S1 | Per-entry register/source/build/map audit and disposition | Current register and current closure | No source/build/runtime mutation |
| S2 | Adapter-owned target contract and migration grouping, derived from S1 | S1 | No speculative provider or device expansion |
| S3 | Move guest-service identity/routing out of Bochs into adapter composition | S2 | No semantic change to guest-service contracts |
| S4 | Move historical machine-handler selection into the adapter machine-composition plane and reduce Bochs to opaque typed mechanics | S2 | No reimplementation of machine mechanics in adapter |
| S5 | Remove superseded intrusions or preserve needed mechanics/diagnostics default-off; update exact build surfaces | S3 and S4 | No archive/device/product-feature expansion |
| S6 | Rebuild minimal closures, run focused boundary regressions, and reconcile the register with source/maps | S5 | No broad BOP runtime campaign |

The S2 grouping may merge or split later implementation S tasks only where S1
produces a source/build dependency that requires it. It may not add a new
semantic purpose outside the policy. Each S requires its own Status admission.

## Completion Gate

T195 closes only when each register entry has a recorded final disposition, the
source and generated build/link surfaces prove that disposition, Bochs contains
no guest-service or historical-handler selection, and focused regressions
demonstrate the typed mechanical boundary. The next queued T then owns holistic
BOP runtime closure.
