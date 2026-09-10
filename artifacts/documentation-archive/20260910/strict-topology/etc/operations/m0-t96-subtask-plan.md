# M0 T96 Subtask Plan

## Status

Supporting plan for active `M0 T96`. It does not replace the one active packet
in `docs/STATUS.md` or allocate later tasks.

| S | Scope | Initial disposition |
| --- | --- | --- |
| S1 | Complete BOP provider, dispatcher, module, and disposition map | Complete in P1; see history/m0-t96-s1-p1-20260811.md. |
| S2 | Shared adapter ingress, provider selection, result, and unavailable ABI | Complete in P1--P2; see history/m0-t96-s2-p1-20260811.md and m0-t96-s2-p2-20260811.md. |
| S3 | DOS/DEM host-service plane migration | Complete in P1--P11 and closure; see history/m0-t96-s3-closure-20260811.md. |
| S4 | COMMAND host-service plane migration | Complete in P1--P5 and closure; see history/m0-t96-s4-closure-20260811.md. |
| S5 | XMS/DPMI provider and profile disposition | Complete; see history/m0-t96-s5-closure-20260811.md. |
| S6 | Redirector, debugger, notification, VDD/mouse host-provider dispositions | Complete; see history/m0-t96-s6-closure-20260811.md. |
| S7 | SoftPC BIOS selector ownership and native-owner reconciliation | Complete; see history/m0-t96-s7-closure-20260811.md. |
| S8 | Existing endpoint migration and no-bypass verification | Complete; see history/m0-t96-s8-closure-20260811.md. |
| S9 | Full inventory/provider/capability regression | Complete; see history/m0-t96-s9-closure-20260812.md. |
| S10 | Real-path trace and T96 closure checkpoint | Active; P1 checkpoint is complete in etc/research/t96-s10-real-path-checkpoint-001.md. Formal T closure awaits owner selection/admission of the next Queue candidate. |

These are module-scale packages. No service number is an S allocation. Every
provider follows the architecture's precedence: original OpenNT provider,
original provider with contained CLI capability substitution, minimal
source-derived provider after a recorded composition blocker, or evidenced
unavailable/deferred behavior.
