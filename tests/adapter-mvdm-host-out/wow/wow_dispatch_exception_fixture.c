/* Compile the complete selected wow32.c, including the original W32Exception.
 * Controlled TD/memory/CPU and error-box boundaries; not real guest evidence.
 * The Shell Close/Ignore guest tests provide the complementary runtime proof. */
#include "precomp.h"
#include "wow_user_thunk_scope.h"
#include <stdio.h>

static OPENNT_SUPPORT_TEB teb;
static OPENNT_SUPPORT_PEB peb;
static RTL_CRITICAL_SECTION peb_lock;
static TD task;
static TDB tdb;
static BYTE stack_memory[65536];
static PVDMFRAME frame;
static DWORD flat[8192], ldt[16384];
static DWORD *ldt_pointer=ldt;
DWORD *fixture_flat_import=flat;
DWORD **fixture_ldt_import=&ldt_pointer;
#pragma comment(linker,"/alternatename:__imp__FlatAddress=_fixture_flat_import")
#pragma comment(linker,"/alternatename:__imp__ExpLdt=_fixture_ldt_import")
static WORD cpu_ss, cpu_sp;
static unsigned phase, sets, calls, dialogs, errors, resumed;
static const DWORD caller_stack=0x17871000, callback_stack=0x17870fda;
W32 aw32WOW[1]; /* Unused numeric-thunk branch, required by original TU. */

/* Link-only resolution for the TU's unrelated initialization graph. The
 * verifier rejects a map retaining those owners; this is never a provider. */
__declspec(noreturn) void __cdecl fixture_unreachable_dependency(void)
{ fputs("FAIL unrelated original dependency reached\n",stderr); ExitProcess(91); }
BYTE fixture_unreachable_data[256];

POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID) { return &teb; }
POPENNT_SUPPORT_PEB NTAPI NtCurrentPeb(VOID) { return &peb; }
USHORT __cdecl getSS(void) {
    if(phase==1) RaiseException(0xe4470004,0,0,NULL);
    return cpu_ss;
}
USHORT __cdecl getSP(void) { return cpu_sp; }
void __cdecl setSS(USHORT value) { cpu_ss=value; }
void __cdecl setSP(USHORT value) { cpu_sp=value; ++sets; }
BOOL Sim32FlushVDMPointer(ULONG address,USHORT count,PBYTE bytes,BOOL mode)
{ (void)address;(void)count;(void)bytes;(void)mode; ++errors; return FALSE; }
BOOL CallBack16(WORD ret,PPARM16 args,VPPROC proc,PULONG result)
{ (void)ret;(void)args;(void)proc;(void)result; ++errors; return FALSE; }
ULONG __cdecl WOWSysErrorBox(LPSTR title,LPSTR text,USHORT first,
    USHORT second,USHORT third)
{
    (void)title;(void)text;(void)second;
    ++dialogs;
    if(first!=SEB_CLOSE || third!=(SEB_IGNORE|SEB_DEFBUTTON)) ++errors;
    return 3; /* Exercise original ordinary Ignore / EXECUTE_HANDLER. */
}
/* This fixture never selects Cancel/debugger creation. Keep its optional
 * obsolete CSRSS import linkable without invoking it on the modern host. */
static NTSTATUS NTAPI unused_csr(void) { ++errors; return (NTSTATUS)0xc0000002; }
NTSTATUS (NTAPI *fixture_csr_pointer)(void)=unused_csr;
#pragma comment(linker,"/alternatename:__imp__CsrIdentifyAlertableThread@0=_fixture_csr_pointer")

ULONG WINAPI wow_user_invoke_thunk(LPFNW32 thunk,PVDMFRAME supplied)
{
    (void)thunk;
    ++calls;
    if(supplied!=frame || task.vpStack!=caller_stack) ++errors;
    cpu_ss=HIWORD(callback_stack);cpu_sp=LOWORD(callback_stack);
    if(phase==2 || phase==4) RaiseException(0xe4470004,0,0,NULL);
    if(phase==3) {
        RaiseException((DWORD)STATUS_INVALID_HANDLE,0,0,NULL);
        ++resumed;
        if(sets || VDMSTACK()!=callback_stack) ++errors;
    }
    return 0x1234abcd;
}
static BOOL search_debuggee(void)
{
    char image[MAX_PATH], command[MAX_PATH+40];
    STARTUPINFOA si={0}; PROCESS_INFORMATION pi={0}; DEBUG_EVENT event;
    DWORD outcome=STILL_ACTIVE, faults=0;
    ULONGLONG deadline=GetTickCount64()+10000;
    if(!GetModuleFileNameA(NULL,image,sizeof(image)) ||
       sprintf_s(command,sizeof(command),"\"%s\" --search-debuggee",image)<0)return FALSE;
    si.cb=sizeof(si);
    if(!CreateProcessA(image,command,NULL,NULL,FALSE,DEBUG_ONLY_THIS_PROCESS|CREATE_NO_WINDOW,
        NULL,NULL,&si,&pi))return FALSE;
    while(GetTickCount64()<deadline) {
        DWORD disposition=DBG_CONTINUE;
        if(!WaitForDebugEvent(&event,100))continue;
        if(event.dwDebugEventCode==EXCEPTION_DEBUG_EVENT) {
            DWORD code=event.u.Exception.ExceptionRecord.ExceptionCode;
            if(code!=EXCEPTION_BREAKPOINT)disposition=DBG_EXCEPTION_NOT_HANDLED;
            if(code==0xe4470004 && event.u.Exception.dwFirstChance)++faults;
        }else if(event.dwDebugEventCode==CREATE_PROCESS_DEBUG_EVENT) {
            if(event.u.CreateProcessInfo.hFile)CloseHandle(event.u.CreateProcessInfo.hFile);
        }else if(event.dwDebugEventCode==LOAD_DLL_DEBUG_EVENT) {
            if(event.u.LoadDll.hFile)CloseHandle(event.u.LoadDll.hFile);
        }else if(event.dwDebugEventCode==EXIT_PROCESS_DEBUG_EVENT) {
            outcome=event.u.ExitProcess.dwExitCode;
        }
        if(!ContinueDebugEvent(event.dwProcessId,event.dwThreadId,disposition))break;
        if(outcome!=STILL_ACTIVE)break;
    }
    if(outcome==STILL_ACTIVE)TerminateProcess(pi.hProcess,92);
    WaitForSingleObject(pi.hProcess,2000);
    CloseHandle(pi.hThread);CloseHandle(pi.hProcess);
    printf("DISPATCH_SEARCH debugger-first-chance=%lu child-exit=%lu\n",faults,outcome);
    return faults==1 && outcome==0;
}
int __cdecl main(int argc,char **argv)
{
    unsigned index;
    BOOL debuggee=argc==2 && !strcmp(argv[1],"--search-debuggee");
    teb.WOW32Reserved=&task;
    teb.ClientId.UniqueThread=(HANDLE)1;
    peb.FastPebLock=&peb_lock;
    peb_lock.OwningThread=(HANDLE)1; /* Skip irrelevant AeDebug profile lookup. */
    fWowMode=TRUE;
    frame=(PVDMFRAME)(stack_memory+LOWORD(caller_stack));
    flat[HIWORD(caller_stack)>>3]=(DWORD)stack_memory;
    flat[0x1237>>3]=(DWORD)&tdb;
    ldt[(caller_stack>>18)|1]=0x8000;
    ldt[((DWORD)0x12370000>>18)|1]=0x8000;
    for(index=0;index<CRITICAL_STRING_COUNT;++index) aszCriticalStrings[index]="fixture";
    for(phase=debuggee?4:0;phase!=(debuggee?5u:4u);++phase) {
        DWORD caught=0;
        ZeroMemory(&task,sizeof(task));ZeroMemory(frame,sizeof(*frame));
        task.vpStack=caller_stack;task.htask16=0x1237;
        frame->wCallID=(DWORD)wow_user_invoke_thunk;frame->wTDB=task.htask16;
        cpu_ss=HIWORD(caller_stack);cpu_sp=LOWORD(caller_stack);
        sets=calls=dialogs=errors=resumed=0;
        __try { W32Dispatch(); }
        __except(GetExceptionCode()==0xe4470004 && phase==4 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught=GetExceptionCode(); }
        if(errors || (((DWORD)cpu_ss<<16)|cpu_sp)!=(phase==4?callback_stack:caller_stack) || task.vpStack!=caller_stack ||
           caught!=(phase==4?0xe4470004u:0u) ||
           sets!=((phase==1 || phase==4)?0u:1u) || calls!=(phase==1?0u:1u) ||
           dialogs!=((phase==1 || phase==2)?1u:0u) || resumed!=(phase==3?1u:0u))return 1;
        if((phase==0 || phase==3) && (frame->wAX!=0xabcd || frame->wDX!=0x1234))return 2;
        printf("DISPATCH_EXCEPTION phase=%u sets=%u calls=%u dialogs=%u resumed=%u\n",
            phase,sets,calls,dialogs,resumed);
    }
    if(debuggee)return 0;
    if(!search_debuggee())return 3;
    puts("PASS selected W32Dispatch and W32Exception early/handled/resumable boundaries");
    return 0;
}
