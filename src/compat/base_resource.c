/* Native resource binding. Authentication and delivery policy stay with the
 * caller that binds the synchronous operations; this unit owns no CSR state. */
#include "include/base_resource.h"

/* Original ntobapi.h signature; the SDK exposes NtClose but not this import. */
NTSTATUS NTAPI NtDuplicateObject(HANDLE, HANDLE, HANDLE, PHANDLE,
    ACCESS_MASK, ULONG, ULONG);

static __declspec(thread) const OPENNT_BASE_RESOURCE_BINDING *resources;
const OPENNT_BASE_RESOURCE_BINDING *OpenNtBaseBindResources(const OPENNT_BASE_RESOURCE_BINDING *binding)
{
    const OPENNT_BASE_RESOURCE_BINDING *previous=resources;
    resources=binding;
    return previous;
}
NTSTATUS NTAPI OpenNtBaseDuplicateObject(HANDLE sourceProcess,HANDLE source,HANDLE targetProcess,
    PHANDLE target,ACCESS_MASK access,ULONG attributes,ULONG options)
{
    DWORD sourcePid,targetPid=GetCurrentProcessId();
    if (resources) {
        if (!resources->Duplicate) return (NTSTATUS)0xc00000bbL;
        return resources->Duplicate(resources->Context,sourceProcess,source,targetProcess,
            target,access,attributes,options);
    }
    sourcePid=GetProcessId(sourceProcess);
    if (targetProcess) targetPid=GetProcessId(targetProcess);
    if (!sourcePid || !targetPid) return (NTSTATUS)0xc0000008L;
    if (sourcePid!=GetCurrentProcessId() || targetPid!=GetCurrentProcessId())
        return (NTSTATUS)0xc00000bbL;
    return NtDuplicateObject(sourceProcess,source,targetProcess,target,access,attributes,options);
}
NTSTATUS NTAPI OpenNtBaseClose(HANDLE handle)
{
    if (resources) {
        if (!resources->Close) return (NTSTATUS)0xc00000bbL;
        return resources->Close(resources->Context,handle);
    }
    return NtClose(handle);
}
