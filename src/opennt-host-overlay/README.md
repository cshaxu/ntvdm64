# OpenNT host private overlay

This is the private implementation partition of `opennt-host`, not an adapter
or an additional source component. Only its matching mirror includes it.

| Register | File | Owner and disposition |
| --- | --- | --- |
| OPENNT-HOST-031 | base/win32/server/dos_worker_wait.inc | Existing standalone pending-wait query; included inside srvvdm.c::BaseSrvDOSWorkerWaitPending and compiled into opennt-base-server.lib. No separately callable overlay API. |

T412 S10 relocates the existing S5 body unchanged. Original BaseSrv record
lookup, critical-section and event state remain authoritative. The standalone
Console binding cannot infer guest command fetch from VDM_READY alone. This
is not an original OpenNT function and must not be counted as recovered source
or a net algorithm deletion. Retain until a source-proven worker-delivery
contract eliminates the query. Focused service reservation tests cover a
READY record without a wait versus an actual pending GetNext command wait.
