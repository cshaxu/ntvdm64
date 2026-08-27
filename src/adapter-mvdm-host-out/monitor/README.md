# monitor family

Same-shaped user-mode VDM monitor interface family. It binds bounded
session/thread context and never recreates kernel or CSRSS behavior.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-VDM-MONITOR-001 | NT4 `NtVdmControl` entered a kernel VDM product service for operations such as `VdmQueryDir`. | Modern Windows has no supported kernel VDM control service, and this product must not recreate kernel VDM, CSRSS or BaseSrv. | Retain the reached enum, `VDMQUERYDIRINFO` layout, function spelling and status shape. Dispatch only synchronous `VdmQueryDir` through the bound session control seam; all other services return `STATUS_NOT_IMPLEMENTED`. | `include/vdm.h`, `source/vdm_control.c` |
| ADAPTER-VDM-MONITOR-002 | OpenNT `GetNextVDMCommand` marshalled `VDMINFO` through BaseSrv/CSR capture buffers and could block on a duplicated server event. | Those private product interfaces do not compose with the one-session public-API profile. | Keep the exported spelling and `VDMINFO` layout; synchronously dispatch a versioned wrapper only while the caller is active. The app-owned source copies bounded bytes, returns original-style capacity results, and rejects first-VDM, WOW, PIF and blocking paths. | `include/vdm_command.h`, `source/vdm_command.c` |

Kernel VDM, CSR and BaseSrv product services are not created by this boundary.
# monitor family

Same-shaped historical VDM-monitor declarations and session-local state.
It never supplies a kernel VDM, CSRSS/CSR broker, CCPU executor or Bochs
object.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| ADAPTER-MONITOR-001 | Make the per-thread VDM TIB's DPMI state and `pNtVDMState` visible to original `dpmi32` sources. | NT4 supplied these through its private kernel/user VDM product composition, which is absent and must not be recreated. | Preserve the reached type/member/global spellings as thread-local session-worker storage.  The companion source-shaped `vint.h` omits only the historical fixed-low-address state pointer so `vdm.h` supplies the same spelling safely.  There is no kernel VDM pointer, CCPU executor, cross-session broker or persistent guest alias. | `include/vdm.h`, `include/vint.h`, `mvdm_vdm_tib.c` |
