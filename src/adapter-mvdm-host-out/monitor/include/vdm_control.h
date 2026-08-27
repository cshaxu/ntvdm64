#ifndef ADAPTER_VDM_MONITOR_VDM_CONTROL_H
#define ADAPTER_VDM_MONITOR_VDM_CONTROL_H

#include "vdm.h"
#include "session.h"

typedef NTSTATUS (*adapter_vdm_monitor_query_dir_fn)(void *context,
    PVDMQUERYDIRINFO query);

/* Bind only the reached VdmQueryDir product capability. This is deliberately
 * distinct from the BaseSrv-shaped GetNextVDMCommand route. */
int adapter_vdm_monitor_bind_query_dir(session *owner,
    adapter_vdm_monitor_query_dir_fn callback, void *context);
int adapter_vdm_monitor_unbind_query_dir(session *owner);

#endif
