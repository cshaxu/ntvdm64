#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <tlhelp32.h>
static void module_for(DWORD pid, ULONG_PTR addr) {
 HANDLE h=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid); MODULEENTRY32 m={sizeof(m)};
 if(h==INVALID_HANDLE_VALUE){printf("MODULE_ERROR pid=%lu error=%lu\n",pid,GetLastError());return;}
 if(Module32First(h,&m)) do {ULONG_PTR a=(ULONG_PTR)m.modBaseAddr;if(addr>=a && addr<a+m.modBaseSize){printf("MODULE pid=%lu base=%p offset=%08lx name=%ls\n",pid,m.modBaseAddr,(DWORD)(addr-a),m.szExePath);break;}} while(Module32Next(h,&m));CloseHandle(h);
}
static void wow_stack(DWORD pid,DWORD tid) {
 HANDLE t=OpenThread(THREAD_GET_CONTEXT,FALSE,tid), p=OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,FALSE,pid); WOW64_CONTEXT c; DWORD stack[8]={0}; SIZE_T n=0; unsigned i; ZeroMemory(&c,sizeof(c));c.ContextFlags=WOW64_CONTEXT_CONTROL;
 if(t&&p&&Wow64GetThreadContext(t,&c)&&ReadProcessMemory(p,(void*)(ULONG_PTR)c.Esp,stack,sizeof(stack),&n)) { printf("WOWSTACK eip=%08lx esp=%08lx read=%llu\n",c.Eip,c.Esp,(unsigned long long)n);for(i=0;i<4;i++){printf("STACK[%u]=%08lx\n",i,stack[i]);module_for(pid,stack[i]);} } else printf("WOWSTACK_ERROR=%lu\n",GetLastError());if(t)CloseHandle(t);if(p)CloseHandle(p);
}
int wmain(int argc,wchar_t **argv) {
 STARTUPINFOW si={sizeof(si)}; PROCESS_INFORMATION broker={0},run={0}; wchar_t exe[MAX_PATH],cmd[2*MAX_PATH],name[MAX_PATH];
 DWORD ids[128]={0},count=0,worker=0,initial[128]={0},initial_wx[128]={0},hits=0,events=0; ULONG_PTR target=0; ULONGLONG until; BOOL done=FALSE; int rc=1;
 if(argc!=2)return 87; si.dwFlags=STARTF_USESHOWWINDOW; si.wShowWindow=SW_HIDE; SetEnvironmentVariableW(L"SystemRoot",argv[1]);
 swprintf_s(exe,MAX_PATH,L"%ls\\system32\\run16.exe",argv[1]);swprintf_s(cmd,2*MAX_PATH,L"\"%ls\" --wait \"%ls\\system32\\MEM.EXE\"",exe,argv[1]);
 if(!CreateProcessW(exe,cmd,0,0,FALSE,CREATE_NEW_CONSOLE|DEBUG_PROCESS,0,argv[1],&si,&run)){printf("run_create_error=%lu\n",GetLastError());goto end;}
 if(!DebugSetProcessKillOnExit(FALSE))goto end;
 until=GetTickCount64()+60000;
 while(GetTickCount64()<until) { DEBUG_EVENT e;DWORD how=DBG_CONTINUE; unsigned i;
  if(!WaitForDebugEvent(&e,100)){if(GetLastError()!=ERROR_SEM_TIMEOUT){printf("debug_error=%lu\n",GetLastError());goto end;}if(WaitForSingleObject(run.hProcess,0)==WAIT_OBJECT_0){done=TRUE;break;}continue;}
  events++; printf("EVENT pid=%lu code=%lu\n",e.dwProcessId,e.dwDebugEventCode); for(i=0;i<count && ids[i]!=e.dwProcessId;i++){};
  if(e.dwDebugEventCode==CREATE_PROCESS_DEBUG_EVENT) {DWORD n=MAX_PATH; if(count>=128)goto end;ids[count++]=e.dwProcessId;
   if(QueryFullProcessImageNameW(e.u.CreateProcessInfo.hProcess,0,name,&n) && wcsrchr(name,L'\\') && !_wcsicmp(wcsrchr(name,L'\\')+1,L"ntvdm.exe")){worker=e.dwProcessId;target=(ULONG_PTR)e.u.CreateProcessInfo.lpBaseOfImage+0x1782e0u;printf("worker=%lu base=%p trap=%p\n",worker,e.u.CreateProcessInfo.lpBaseOfImage,(void *)target);}
   if(e.u.CreateProcessInfo.hFile)CloseHandle(e.u.CreateProcessInfo.hFile);CloseHandle(e.u.CreateProcessInfo.hThread);CloseHandle(e.u.CreateProcessInfo.hProcess);
  } else if(e.dwDebugEventCode==CREATE_THREAD_DEBUG_EVENT)CloseHandle(e.u.CreateThread.hThread);
  else if(e.dwDebugEventCode==LOAD_DLL_DEBUG_EVENT){if(e.u.LoadDll.hFile)CloseHandle(e.u.LoadDll.hFile);}
  else if(e.dwDebugEventCode==OUTPUT_DEBUG_STRING_EVENT && !e.u.DebugString.fUnicode && e.u.DebugString.nDebugStringLength<1024) { HANDLE q=OpenProcess(PROCESS_VM_READ,FALSE,e.dwProcessId); char msg[1024]={0}; SIZE_T n=0; if(q) { if(ReadProcessMemory(q,e.u.DebugString.lpDebugStringData,msg,e.u.DebugString.nDebugStringLength,&n))printf("DEBUG_STRING pid=%lu tid=%lu text=%s\n",e.dwProcessId,e.dwThreadId,msg); CloseHandle(q); } }
  else if(e.dwDebugEventCode==EXIT_PROCESS_DEBUG_EVENT) printf("EXIT pid=%lu code=%lu\n",e.dwProcessId,e.u.ExitProcess.dwExitCode); else if(e.dwDebugEventCode==EXCEPTION_DEBUG_EVENT) {DWORD code=e.u.Exception.ExceptionRecord.ExceptionCode; ULONG_PTR addr=(ULONG_PTR)e.u.Exception.ExceptionRecord.ExceptionAddress;
   printf("EXCEPTION pid=%lu code=%08lx addr=%p first=%lu\n",e.dwProcessId,code,(void*)addr,e.u.Exception.dwFirstChance); if(code==0x4000001fu) { module_for(e.dwProcessId,addr); wow_stack(e.dwProcessId,e.dwThreadId); } if(e.dwProcessId==worker && (code==EXCEPTION_BREAKPOINT || code==0x4000001fu) && addr==target){hits++;printf("SAS_BREAKPOINT pid=%lu tid=%lu addr=%p\n",worker,e.dwThreadId,(void*)addr);how=DBG_EXCEPTION_NOT_HANDLED;}
   else if(code==0x4000001fu && i<count && !initial_wx[i])initial_wx[i]=1;
   else if(code==0x40000005u) { /* Original STATUS_VDM_EVENT continues under the debugger. */ }
   else if(code==EXCEPTION_BREAKPOINT && i<count && !initial[i])initial[i]=1;
   else how=DBG_EXCEPTION_NOT_HANDLED;
  }
  if(!ContinueDebugEvent(e.dwProcessId,e.dwThreadId,how)){printf("continue_error=%lu\n",GetLastError());goto end;}
 }
 {DWORD exit=STILL_ACTIVE; if(done)GetExitCodeProcess(run.hProcess,&exit); printf("RESULT done=%u exit=%lu worker=%lu events=%lu sas_hits=%lu\n",done,exit,worker,events,hits);if(done&&exit==0&&worker&&hits==0)rc=0;}
end:
 if(worker)DebugActiveProcessStop(worker);if(run.dwProcessId)DebugActiveProcessStop(run.dwProcessId);
 if(run.hThread)CloseHandle(run.hThread);if(run.hProcess)CloseHandle(run.hProcess);if(broker.hThread)CloseHandle(broker.hThread);if(broker.hProcess)CloseHandle(broker.hProcess);return rc;
}
