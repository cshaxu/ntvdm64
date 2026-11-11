/* Diagnostic-only replacement TU; original DEM implementation is unchanged. */
#define CreateFileOem trace_CreateFileOem
#include "../../src/mvdm-host/dos/dem/demfile.c"
#undef CreateFileOem
#include <stdio.h>
HANDLE WINAPI CreateFileOem(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
HANDLE WINAPI trace_CreateFileOem(LPCSTR name,DWORD access,DWORD share,
    LPSECURITY_ATTRIBUTES security,DWORD disposition,DWORD flags,HANDLE template_file)
{
    HANDLE result=CreateFileOem(name,access,share,security,disposition,flags,template_file);
    DWORD error=GetLastError();char path[MAX_PATH];
    if(name && !_strnicmp(name,"O:\\TMP\\Q94\\",11) &&
       GetEnvironmentVariableA("MVDM_FILE_DIAGNOSTIC_LOG",path,sizeof(path))) {
        FILE *log=fopen(path,"ab");
        if(log){fprintf(log,"path=%s disposition=%lu handle=%p error=%lu\n",
            name,disposition,result,error);fclose(log);}
    }
    SetLastError(error);return result;
}
