# BaseSrv family

This family supplies only the reached local BaseClient/BaseSrv VDM command
protocol. It is neither a CSRSS replacement nor a generic CSR transport.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-BASESRV-001 | BaseSrv stores DOS command records, checks buffer capacities, copies VDMINFO fields and returns required sizes. | The original `BaseSrvGetNextVDMCommand` is coupled to CSRSS console/DOS/WOW records and remote duplicated handles, which cannot enter the non-intrusive host process. | `base_vdm_local.c` preserves the reached DOS-only copy, capacity, environment/no-command, reentry and status order in a copied session-owned record. WOW, PIF, child, global and cross-process paths remain unavailable. | `include/base_vdm_local.h`, `source/base_vdm_local.c` |
| ADAPTER-BASESRV-002 | BaseClient exposes `GetNextVDMCommand(VDMINFO *)` to MVDM callers. | The prior monitor callback was a project-defined parallel protocol and did not model BaseSrv record ownership. | The historical public spelling now belongs to this named BaseSrv adapter family; it binds one session-owned copied DOS record and never retains the caller's `VDMINFO` pointer. | `include/base_vdm_local.h`, `source/base_vdm_local.c` |
