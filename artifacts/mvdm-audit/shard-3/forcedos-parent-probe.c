#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
int wmain(int argc,wchar_t **argv) {
 STARTUPINFOW si={0}; PROCESS_INFORMATION pi={0}; DWORD flags, error, wait, code=0; FILE *log;
 if(argc!=4 || _wfopen_s(&log,argv[3],L"wb"))return 2;
 flags=wcstoul(argv[2],NULL,0); si.cb=sizeof(si);
 SetLastError(0);
 if(!CreateProcessW(argv[1],NULL,NULL,NULL,TRUE,flags,NULL,NULL,&si,&pi)) {
   error=GetLastError(); fprintf(log,"create=0 flags=%08lx error=%lu\n",flags,error); fclose(log); return 3;
 }
 fprintf(log,"create=1 flags=%08lx pid=%lu\n",flags,pi.dwProcessId); fflush(log);
 wait=WaitForSingleObject(pi.hProcess,15000);
 if(wait==WAIT_TIMEOUT) { TerminateProcess(pi.hProcess,ERROR_TIMEOUT); WaitForSingleObject(pi.hProcess,INFINITE); }
 GetExitCodeProcess(pi.hProcess,&code); fprintf(log,"wait=%lu exit=%lu\n",wait,code); fclose(log);
 CloseHandle(pi.hThread);CloseHandle(pi.hProcess); return wait==WAIT_OBJECT_0?0:4;
}
