#ifndef ADAPTER_VDM_MONITOR_VDM_COMMAND_H
#define ADAPTER_VDM_MONITOR_VDM_COMMAND_H

#include <nt.h>
#include <vdmapi.h>
#include "session.h"

/*
 * DIVERGENCE: BaseSrv originally captured VDMINFO and selected the caller's
 * VDM process.  The public client entry and VDMINFO shape are preserved, but
 * the present one-session product binds one synchronous provider to the
 * monitor command plane.  The provider must not retain this caller pointer.
 */
typedef NTSTATUS (*adapter_vdm_monitor_command_provider_fn)(void *context,
    PVDMINFO information);

int adapter_vdm_monitor_bind_command_provider(session *owner,
    adapter_vdm_monitor_command_provider_fn provider, void *context);
int adapter_vdm_monitor_unbind_command_provider(session *owner);

BOOL APIENTRY GetNextVDMCommand(PVDMINFO information);

#endif
