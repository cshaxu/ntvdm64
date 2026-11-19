#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>

/* OpenNT public/internal/base/inc/vdm.h declares this cdecl ABI. MSVC permits
 * its enum tag to remain incomplete here; no service value or layout is used.
 * The selected product has no kernel VDM capability. Original DEM retains
 * its own directory-enumeration fallback for STATUS_NOT_IMPLEMENTED. */
enum _VdmServiceClass;
LONG NtVdmControl(enum _VdmServiceClass service, PVOID service_data)
{
    (void)service;
    (void)service_data;
    return STATUS_NOT_IMPLEMENTED;
}
