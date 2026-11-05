#include "process.h"
BOOL win32_process_create(PCWSTR application, PWSTR command,
    LPSECURITY_ATTRIBUTES process_attributes, LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit, DWORD flags, LPVOID environment, PCWSTR directory,
    LPSTARTUPINFOW startup, LPPROCESS_INFORMATION child)
{
    return CreateProcessW(application,command,process_attributes,thread_attributes,
        inherit,flags,environment,directory,startup,child);
}
