#ifndef ADAPTER_VDM_MONITOR_VDM_COMMAND_H
#define ADAPTER_VDM_MONITOR_VDM_COMMAND_H

#include <stdint.h>
#include <nt.h>
#include <vdmapi.h>

/* Source-shaped local command-acquisition operation. */
#define ADAPTER_VDM_MONITOR_GET_NEXT_COMMAND UINT32_C(0x56444d01)
#define ADAPTER_VDM_MONITOR_COMMAND_REQUEST_VERSION UINT32_C(1)

/*
 * DIVERGENCE: the original client passed PVDMINFO through a private CSR
 * capture buffer.  The one-session binding retains the public entry and
 * VDMINFO shape, but wraps the synchronous call so no caller pointer or
 * HANDLE can be retained by session or by the source owner.
 */
typedef struct adapter_vdm_monitor_command_request {
    uint32_t version;
    uint32_t struct_bytes;
    PVDMINFO information;
} adapter_vdm_monitor_command_request;

BOOL APIENTRY GetNextVDMCommand(PVDMINFO information);

#endif
