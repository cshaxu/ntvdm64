# BaseSrv family

This family supplies only the reached local BaseClient/BaseSrv VDM command
protocol. It is neither a CSRSS replacement nor a generic CSR transport.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-BASESRV-001 | BaseSrv stores DOS command records, checks buffer capacities, copies VDMINFO fields and returns required sizes. | The original `BaseSrvGetNextVDMCommand` is coupled to CSRSS console/DOS/WOW records and remote duplicated handles, which cannot enter the non-intrusive host process. | `base_vdm_local.c` preserves the reached DOS-only copy, capacity, environment/no-command, reentry and status order in a copied session-owned record. WOW, PIF, child, global and cross-process paths remain unavailable. | `include/base_vdm_local.h`, `source/base_vdm_local.c` |
| ADAPTER-BASESRV-002 | BaseClient exposes `GetNextVDMCommand(VDMINFO *)` to MVDM callers. | The original client path relies on private CSR capture/transport and the prior monitor callback was a project-defined parallel protocol. | The historical public spelling owns a caller-local `VDMINFO` request/result snapshot, then calls the named local BaseSrv record dispatcher. It never retains a caller pointer after return. | `include/base_vdm_local.h`, `source/base_vdm_client.c`, `source/base_vdm_local.c` |
