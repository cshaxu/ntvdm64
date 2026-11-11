/* Original PIF parser plus real session/root provider; no host-root substitute. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <pif.h>
#include "nt_pif.h"
#include "ntvdm-exe/session/session.h"
#include "mvdm_softpc_firmware.h"
ULONG DosSessionId;
BOOL bPifFastPaste;
PIF_DATA pfdata;
int RcMessageBox(UINT id,CHAR *a,CHAR *b,DWORD flags)
{(void)id;(void)a;(void)b;(void)flags;ExitProcess(99);return 0;}
#define CHECK(x) do {if(!(x)){printf("PIF_ROOT_FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
int main(void)
{
    session instance; char temp[MAX_PATH],root[MAX_PATH],path[MAX_PATH],copy[MAX_PATH],small[2];
    STDPIF standard;PIFEXTHDR header;PIF_DATA data;HANDLE file;DWORD written;
    CHECK(GetTempPathA(MAX_PATH,temp));CHECK(GetTempFileNameA(temp,"pfr",0,root));
    CHECK(DeleteFileA(root)&&CreateDirectoryA(root,NULL));
    CHECK(sprintf_s(path,sizeof(path),"%s\\_default.pif",root)>0);
    memset(&standard,0,sizeof(standard));memset(&header,0,sizeof(header));header.extnxthdrfloff=LASTHEADER;
    file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_TEMPORARY,NULL);
    CHECK(file!=INVALID_HANDLE_VALUE);
    CHECK(WriteFile(file,&standard,sizeof(standard),&written,NULL)&&written==sizeof(standard));
    CHECK(WriteFile(file,&header,sizeof(header),&written,NULL)&&written==sizeof(header));CHECK(CloseHandle(file));
    session_initialize(&instance,1);
    CHECK(session_set_mvdm_system_root(&instance,root)&&session_activate(&instance)&&session_thread_bind(&instance));
    CHECK(NtvdmGetWindowsDirectoryA(copy,sizeof(copy))==strlen(root)&&!strcmp(copy,root));
    CHECK(NtvdmGetWindowsDirectoryA(small,sizeof(small))==strlen(root)+1&&small[0]==0);
    memset(&data,0,sizeof(data));CHECK(GetPIFData(&data,""));
    CHECK(data.xmsdes==DEFAULTXMSLMT&&data.emsdes==DEFAULTEMSLMT);
    CHECK(DeleteFileA(path));data.xmsdes=123;
    CHECK(!GetPIFData(&data,"")&&data.xmsdes==DEFAULTXMSLMT);
    CHECK(session_thread_unbind(&instance)&&session_dispose(&instance));
    CHECK(RemoveDirectoryA(root));puts("PIF_REAL_SESSION_DEFAULT_ROOT_PRESENT_ABSENT_OK");return 0;
}
