#include "wow_native_process.h"

BOOL WINAPI wow_native_create_process_a(LPCSTR application,LPSTR command,
    LPSECURITY_ATTRIBUTES process_attributes,LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit,DWORD flags,LPVOID environment,LPCSTR directory,
    LPSTARTUPINFOA startup,LPPROCESS_INFORMATION process)
{
    if(!process || !startup){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    return CreateProcessA(application,command,process_attributes,thread_attributes,
        inherit,flags,environment,directory,startup,process);
}
