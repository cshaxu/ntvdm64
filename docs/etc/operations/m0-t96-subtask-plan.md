# M0 T96 Subtask Plan

## Status

Supporting plan for active `M0 T96`. It does not replace the one active packet
in `docs/STATUS.md` or allocate later tasks.

| S | Scope | Initial disposition |
| --- | --- | --- |
| S1 | Complete BOP provider, dispatcher, module, and disposition map | Complete in P1; see history/m0-t96-s1-p1-20260811.md. |
| S2 | Shared adapter ingress, provider selection, result, and unavailable ABI | Active. |
| S3 | DOS/DEM host-service plane migration | Pending S2. |
| S4 | COMMAND host-service plane migration | Pending S3. |
| S5 | XMS/DPMI provider and profile disposition | Pending S4. |
| S6 | Redirector, debugger, notification, VDD/mouse host-provider dispositions | Pending S5. |
| S7 | SoftPC BIOS selector ownership and native-owner reconciliation | Pending S6. |
| S8 | Existing endpoint migration and no-bypass verification | Pending S7. |
| S9 | Full inventory/provider/capability regression | Pending S8. |
| S10 | Real-path trace and T96 closure checkpoint | Pending S9. |

These are module-scale packages. No service number is an S allocation. Every
provider follows the architecture's precedence: original OpenNT provider,
original provider with contained CLI capability substitution, minimal
source-derived provider after a recorded composition blocker, or evidenced
unavailable/deferred behavior.
