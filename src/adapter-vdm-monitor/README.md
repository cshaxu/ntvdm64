# adapter-vdm-monitor

Same-shaped user-mode VDM monitor interface family. It binds bounded
session/thread context and never recreates kernel or CSRSS behavior.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-VDM-MONITOR-001 | NT4 `NtVdmControl` entered a kernel VDM product service for operations such as `VdmQueryDir`. | Modern Windows has no supported kernel VDM control service, and this product must not recreate kernel VDM, CSRSS or BaseSrv. | Retain the reached enum, `VDMQUERYDIRINFO` layout, function spelling and status shape. Dispatch only synchronous `VdmQueryDir` through the bound session control seam; all other services return `STATUS_NOT_IMPLEMENTED`. | `include/vdm.h`, `source/vdm_control.c` |
