/* Compile with OPENNT_BASE_NATIVE_RESOURCES: this TU calls actual native APIs.
 * Original BaseSrv algorithms are unchanged. Remote delivery requires a bound
 * authenticated recipient implementation; it never falls back to native
 * cross-process duplication if that implementation is absent or fails. */
#include <base_server.h>
#include <base_resource.h>
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
