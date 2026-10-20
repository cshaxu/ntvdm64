/* Compile the complete selected wow32.c, including the original W32Exception.
 * Controlled TD/memory/CPU and error-box boundaries; not real guest evidence.
 * The Shell Close/Ignore guest tests provide the complementary runtime proof. */
#include "precomp.h"
#include "wow_user_thunk_scope.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_native_call.h"
#include "wow_message_lifetime.h"
#include <stdio.h>

static OPENNT_SUPPORT_TEB teb;
static OPENNT_SUPPORT_PEB peb;
static RTL_CRITICAL_SECTION peb_lock;
static TD task;
static TDB tdb;
static BYTE stack_memory[65536];
static PVDMFRAME frame;
static PVDMFRAME nested_frame;
static DWORD flat[8192], ldt[16384];
static DWORD *ldt_pointer=ldt;
DWORD *fixture_flat_import=flat;
DWORD **fixture_ldt_import=&ldt_pointer;
#pragma comment(linker,"/alternatename:__imp__FlatAddress=_fixture_flat_import")
#pragma comment(linker,"/alternatename:__imp__ExpLdt=_fixture_ldt_import")
static WORD cpu_ss, cpu_sp;
static unsigned phase, sets, calls, dialogs, errors, resumed;
static unsigned box_enters,box_leaves,box_live;
static BOOL box_throw,box_reject,box_return_reject;
static DWORD box_entry_error;
static const DWORD caller_stack=0x17871000, callback_stack=0x17870fda;
W32 aw32WOW[1]; /* Unused numeric-thunk branch, required by original TU. */
M32 aw32Msg[1024]; /* Exact original WndProc body only uses the selected slot. */
BOOL gfIgnoreInputAssertGiven;
__declspec(noreturn) void __cdecl fixture_unreachable_dependency(void);

/* The selected WndProc function is extracted byte-for-byte at build time from
 * the current mirror.  The fixture supplies only its inactive conversion
 * dependencies: this phase uses WM_USER+7, which the original body sends
 * straight to CallBack16.  W32Dispatch/W32Exception themselves are compiled
 * from their complete selected TU below, so this is not a retyped filter. */
static unsigned idle_blocks,idle_unblocks,stack_enters,stack_leaves,unwinds;
static WW callback_words;
void WINAPI wow_message_stack_enter(wow_message_stack *scope)
{
    ZeroMemory(scope,sizeof(*scope));scope->task16=CURRENTPTD()->htask16;++stack_enters;
}
void WINAPI wow_message_stack_leave(wow_message_stack *scope,BOOL abnormal)
{ (void)scope;(void)abnormal;++stack_leaves; }
void WINAPI wow_message_unwind32(LPWM32MSGPARAMEX message)
{ (void)message;++unwinds; }
void WINAPI wow_message_unwind16(MSGPARAMEX *message)
{ (void)message;fixture_unreachable_dependency(); }
PWC FindPWC(HANDLE window) { (void)window;fixture_unreachable_dependency();return NULL; }
WNDPROC GetStdClassWndProc(DWORD kind)
{ (void)kind;fixture_unreachable_dependency();return NULL; }
BOOL FASTCALL WM32NoThunking(LPWM32MSGPARAMEX message)
{ (void)message;fixture_unreachable_dependency();return FALSE; }
#include "original-wndproc.inc"

/* Link-only resolution for the TU's unrelated initialization graph. The
 * verifier rejects a map retaining those owners; this is never a provider. */
__declspec(noreturn) void __cdecl fixture_unreachable_dependency(void)
{ fputs("FAIL unrelated original dependency reached\n",stderr); ExitProcess(91); }
BYTE fixture_unreachable_data[256];
/* The adapter TU also retains its real thunk facade. This test substitutes
 * invocation, not filter policy; its unrelated borrow closure is fail-closed.
 * The verifier's map gate must discard that facade and these functions. */
__declspec(thread) BOOL fWhoCalled,fThunkDDEmsg;
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{(void)scope;fixture_unreachable_dependency();return FALSE;}
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{(void)scope;fixture_unreachable_dependency();}
__declspec(noreturn) VOID WINAPI wow_user_native_call_failed(const char *stage,DWORD error)
{
    if(!(box_reject || box_return_reject))ExitProcess(91);
    if(strcmp(stage,box_reject?"hard-error-native-entry":"hard-error-native-return") || error!=ERROR_INVALID_STATE)++errors;
    RaiseException(0xe4470008,0,0,NULL);ExitProcess(91);
}
BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call,HWND target)
{
    ++box_enters;box_entry_error=GetLastError();
    if(target || box_live)++errors;
    if(box_reject){SetLastError(ERROR_INVALID_STATE);return FALSE;}
    ZeroMemory(call,sizeof(*call));call->held=TRUE;box_live=1;
    SetLastError(0x3031);return TRUE;
}
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{
    ++box_leaves;if(!call->held || box_live!=1)++errors;box_live=0;
    SetLastError(box_return_reject?ERROR_INVALID_STATE:0x3031);
    return !box_return_reject;
}

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
VOID __cdecl BlockWOWIdle(BOOL blocking)
{
    if(blocking)++idle_blocks;else ++idle_unblocks;
}
BOOL CallBack16(WORD ret,PPARM16 args,VPPROC proc,PULONG result)
{
    if(phase==7 || phase==8){
        if(ret!=RET_WNDPROC || proc!=(VPPROC)0x12340056 ||
                args->WndProc.wMsg!=WM_USER+7 || idle_unblocks!=idle_blocks+1)++errors;
        RaiseException(0xe4470004,0,0,NULL);
    }
    (void)ret;(void)args;(void)proc;(void)result; ++errors; return FALSE;
}
ULONG __cdecl WOWSysErrorBox(LPSTR title,LPSTR text,USHORT first,
    USHORT second,USHORT third)
{
    (void)title;(void)text;(void)second;
    ++dialogs;
    if(box_live!=1 || GetLastError()!=box_entry_error)++errors;
    if(first!=SEB_CLOSE || third!=(SEB_IGNORE|SEB_DEFBUTTON)) ++errors;
    SetLastError(0x2021);
    if(box_throw)RaiseException(0xe4470007,0,0,NULL);
    return (phase==6 || phase==8) ? 1 : 3; /* Original Close and Ignore, not a fake filter. */
}
static BOOL hard_error_scope_cases(void)
{
    unsigned mode;
    for(mode=0;mode<5;++mode){
        DWORD result=0xdeadbeef,code=0;
        phase=mode?9:6;box_enters=box_leaves=box_live=dialogs=errors=0;
        box_throw=mode==2;box_reject=mode==3;box_return_reject=mode==4;
        SetLastError(0x1010);
        __try {result=wow_user_sys_error_box("title","message",SEB_CLOSE,0,SEB_IGNORE|SEB_DEFBUTTON);}
        __except(GetExceptionCode()==0xe4470007 || GetExceptionCode()==0xe4470008?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){code=GetExceptionCode();}
        if(errors || box_enters!=1 || box_live || box_leaves!=(mode==3?0u:1u) || dialogs!=(mode==3?0u:1u) ||
            result!=(mode>=2?0xdeadbeef:mode?3u:1u) || code!=(mode==2?0xe4470007:mode>=3?0xe4470008:0) ||
            GetLastError()!=(mode>=3?ERROR_INVALID_STATE:0x2021))return FALSE;
        printf("HARD_ERROR_SCOPE mode=%u enter=%u leave=%u dialogs=%u exception=%08lx\n",mode,box_enters,box_leaves,dialogs,code);
    }
    box_throw=box_reject=box_return_reject=FALSE;return TRUE;
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
    if(phase==5 || phase==6) {
        task.vpStack=callback_stack; /* An original callback has not unwound yet. */
        RaiseException(0xe4470004,0,0,NULL);
    }
    if(phase==2 || phase==4) RaiseException(0xe4470004,0,0,NULL);
    if(phase==3) {
        RaiseException((DWORD)STATUS_INVALID_HANDLE,0,0,NULL);
        ++resumed;
        if(sets || VDMSTACK()!=callback_stack) ++errors;
    }
    if(phase==7 || phase==8){
        callback_words.iClass=WOWCLASS_WIN16;
        callback_words.flState=WWSTATE_ICLASSISSET;
        callback_words.hInstance=(HANDLE)0x5678;
        (void)W32Win16WndProcEx((HWND)1,WM_USER+7,0x4567,-17,
            (VPWNDPROC)0x92340056,&callback_words);
        ++errors; /* W32Dispatch must receive CallBack16's exception. */
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
    unsigned only=argc==2 && !strcmp(argv[1],"--abort-ignore") ? 5u :
        argc==2 && !strcmp(argv[1],"--abort-close") ? 6u : 0u;
    teb.WOW32Reserved=&task;
    teb.ClientId.UniqueThread=(HANDLE)1;
    peb.FastPebLock=&peb_lock;
    peb_lock.OwningThread=(HANDLE)1; /* Skip irrelevant AeDebug profile lookup. */
    fWowMode=TRUE;
    frame=(PVDMFRAME)(stack_memory+LOWORD(caller_stack));
    nested_frame=(PVDMFRAME)(stack_memory+LOWORD(callback_stack));
    flat[HIWORD(caller_stack)>>3]=(DWORD)stack_memory;
    flat[0x1237>>3]=(DWORD)&tdb;
    ldt[(caller_stack>>18)|1]=0x8000;
    ldt[((DWORD)0x12370000>>18)|1]=0x8000;
    for(index=0;index<CRITICAL_STRING_COUNT;++index) aszCriticalStrings[index]="fixture";
    for(phase=debuggee?4:only;phase!=(debuggee?5u:only?only+1:9u);++phase) {
        DWORD caught=0;
        if(!debuggee && phase==4)continue;
        ZeroMemory(&task,sizeof(task));ZeroMemory(frame,sizeof(*frame));
        ZeroMemory(nested_frame,FIELD_OFFSET(VDMFRAME,wLocalBP)+sizeof(WORD));
        nested_frame->wTDB=0x1237;nested_frame->wRetID=RET_ENUMWINDOWPROC;
        task.vpStack=caller_stack;task.htask16=0x1237;
        frame->wCallID=(DWORD)wow_user_invoke_thunk;frame->wTDB=task.htask16;
        cpu_ss=HIWORD(caller_stack);cpu_sp=LOWORD(caller_stack);
        sets=calls=dialogs=errors=resumed=0;
        idle_blocks=idle_unblocks=stack_enters=stack_leaves=unwinds=0;
        box_enters=box_leaves=box_live=0;
        __try { W32Dispatch(); }
        __except(GetExceptionCode()==0xe4470004 && phase==4 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught=GetExceptionCode(); }
        if(errors || (((DWORD)cpu_ss<<16)|cpu_sp)!=(phase==4?callback_stack:caller_stack) || task.vpStack!=caller_stack ||
           caught!=(phase==4?0xe4470004u:0u) ||
           sets!=((phase==1 || phase==4)?0u:1u) || calls!=(phase==1?0u:1u) ||
           dialogs!=((phase==1 || phase==2 || phase==5 || phase==6 || phase==7 || phase==8)?1u:0u) || resumed!=(phase==3?1u:0u) ||
           box_enters!=dialogs || box_leaves!=dialogs || box_live) {
            printf("DISPATCH_ABORT_WRONG phase=%u logical=%08lx cpu=%08lx entryret=%04x nestedret=%04x\n",
                phase,task.vpStack,((DWORD)cpu_ss<<16)|cpu_sp,frame->wRetID,nested_frame->wRetID);
            return 1;
        }
        if(phase==6 && (frame->wRetID!=RET_FORCETASKEXIT || nested_frame->wRetID!=RET_ENUMWINDOWPROC)) {
            puts("DISPATCH_ABORT_WRONG phase=6 kill_target");return 1;
        }
        if((phase==7 || phase==8) && (idle_blocks || idle_unblocks!=1 ||
                stack_enters!=1 || stack_leaves!=1 || unwinds!=1 ||
                (phase==8 && frame->wRetID!=RET_FORCETASKEXIT))) {
            printf("DISPATCH_IDLE_WRONG phase=%u block=%u unblock=%u stack=%u/%u unwind=%u ret=%04x\n",
                phase,idle_blocks,idle_unblocks,stack_enters,stack_leaves,unwinds,frame->wRetID);return 1;
        }
        if((phase==0 || phase==3) && (frame->wAX!=0xabcd || frame->wDX!=0x1234))return 2;
        printf("DISPATCH_EXCEPTION phase=%u sets=%u calls=%u dialogs=%u resumed=%u\n",
            phase,sets,calls,dialogs,resumed);
    }
    if(debuggee || only)return 0;
    if(!search_debuggee())return 3;
    if(!hard_error_scope_cases())return 4;
    puts("PASS selected W32Dispatch and W32Exception early/handled/resumable boundaries");
    return 0;
}
