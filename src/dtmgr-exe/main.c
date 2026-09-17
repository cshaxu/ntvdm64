/* DTASKMGR is a native Console observer for this product's BaseSrv only.
 * It owns presentation/selection, never task identity or process control. */
#include <windows.h>
#include <rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "service.h"
#include "basesrv-exe/transport/rpc_security.h"
#include "product-abi/version.h"

void *__RPC_USER MIDL_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER MIDL_user_free(void *value) { free(value); }

typedef struct DTASKMGR_STATE {
    RPC_BINDING_HANDLE binding;
    broker_rpc_scope scope;
    HANDLE process;
    uint64_t selected_epoch;
    ULONG selected_sequence;
    DWORD status;
    DWORD terminate_status;
    BOOL terminate_attempted;
} DTASKMGR_STATE;
static const unsigned char app_version[APP_VERSION_BYTES]=APP_VERSION;

static BOOL bind_basesrv(DTASKMGR_STATE *state)
{
    WCHAR endpoint[128];
    RPC_WSTR text=NULL;
    RPC_STATUS rpc;
    if (!broker_rpc_capture_scope(&state->scope)) return FALSE;
    wsprintfW(endpoint,L"ntvdm-basesrv-%lu-%08lx-%08lx",state->scope.session,
        (ULONG)state->scope.logon.HighPart,(ULONG)state->scope.logon.LowPart);
    rpc=RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,(RPC_WSTR)endpoint,NULL,&text);
    if (rpc!=RPC_S_OK) { SetLastError(rpc); return FALSE; }
    rpc=RpcBindingFromStringBindingW(text,&state->binding);
    RpcStringFreeW(&text);
    if (rpc!=RPC_S_OK) { SetLastError(rpc); return FALSE; }
    rpc=RpcBindingSetAuthInfoW(state->binding,NULL,RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE);
    if (rpc!=RPC_S_OK) { RpcBindingFree(&state->binding); SetLastError(rpc); return FALSE; }
    /* A system_handle must be a real, duplicable client handle.  The BaseSrv
     * peer check compares its server-side PID with the authenticated RPC
     * caller.  This is deliberately the same shape as dtaskmgr_rpc_test,
     * rather than the process pseudo-handle. */
    state->process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,
        FALSE,GetCurrentProcessId());
    if (!state->process) { DWORD error=GetLastError(); RpcBindingFree(&state->binding); SetLastError(error); return FALSE; }
    return TRUE;
}
static const WCHAR *kind_name(ULONG kind)
{
    return kind==3u ? L"WOW16" : kind==2u ? L"Win16" : L"DOS";
}
static const WCHAR *state_name(ULONG state)
{
    if (state&0x80000000u) return L"stopping";
    /* Values are the existing srvvdm.h DOS/WOW record states, copied by
     * BaseSrv rather than invented client-side task state. */
    if (state==1u) return L"waiting";
    if (state==2u) return L"running";
    if (state==4u) return L"returned";
    if (state==8u) return L"ready";
    return L"active";
}
static void elapsed_text(const FILETIME *started,const FILETIME *now,WCHAR output[16])
{
    ULARGE_INTEGER then_value,now_value;
    ULONGLONG seconds;
    if (!started->dwLowDateTime && !started->dwHighDateTime) {
        lstrcpyW(output,L"-"); return;
    }
    then_value.LowPart=started->dwLowDateTime;then_value.HighPart=started->dwHighDateTime;
    now_value.LowPart=now->dwLowDateTime;now_value.HighPart=now->dwHighDateTime;
    seconds=now_value.QuadPart>then_value.QuadPart ?
        (now_value.QuadPart-then_value.QuadPart)/10000000ULL : 0;
    swprintf_s(output,16,L"%02llu:%02llu:%02llu",seconds/3600ULL,
        (seconds/60ULL)%60ULL,seconds%60ULL);
}
static void clear_screen(HANDLE output)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD written;
    COORD origin={0,0};
    if (!GetConsoleScreenBufferInfo(output,&info)) return;
    FillConsoleOutputCharacterW(output,L' ',info.dwSize.X*info.dwSize.Y,origin,&written);
    FillConsoleOutputAttribute(output,info.wAttributes,info.dwSize.X*info.dwSize.Y,origin,&written);
    SetConsoleCursorPosition(output,origin);
}
static void render(HANDLE output,DTASKMGR_STATE *state,DTASKMGR_WORKER *items,ULONG count)
{
    ULONG index;
    FILETIME now;
    SYSTEMTIME clock;
    clear_screen(output);
    fwprintf(stdout,L"NTVDM Task Manager  |  BaseSrv %s\n",
        state->status==ERROR_SUCCESS ? L"connected" : L"not connected");
    fwprintf(stdout,L"%-2s %-8s %-7s %-7s %-11s %-12s %-10s %s\n",L"",L"WORKER",L"TASK",L"KIND",L"STATE",L"START",L"ELAPSED",L"IMAGE");
    fwprintf(stdout,L"--------------------------------------------------------------------------------\n");
    if (!count) {
        fwprintf(stdout,L"  No product tasks.\n");
        if (state->status!=ERROR_SUCCESS)
            fwprintf(stdout,L"  BaseSrv not connected (error %lu); refresh will retry.\n",
                (unsigned long)state->status);
    }
    GetSystemTimeAsFileTime(&now);
    for (index=0;index<count;++index) {
        FILETIME started;
        WCHAR elapsed[16];
        started.dwLowDateTime=(DWORD)items[index].started_filetime;
        started.dwHighDateTime=(DWORD)(items[index].started_filetime>>32);
        FileTimeToSystemTime(&started,&clock);
        elapsed_text(&started,&now,elapsed);
        fwprintf(stdout,L"%c  %-8lu %-7lu %-7s %-11s %02u:%02u:%02u     %-10s %s\n",
            items[index].sequence==state->selected_sequence ? L'>' : L' ',
            (unsigned long)items[index].sequence,(unsigned long)items[index].task,
            kind_name(items[index].kind),state_name(items[index].state),clock.wHour,
            clock.wMinute,clock.wSecond,elapsed,items[index].image[0] ? items[index].image : L"ntvdm.exe");
    }
    if (state->terminate_attempted) {
        if (state->terminate_status==ERROR_SUCCESS)
            fwprintf(stdout,L"Termination accepted; waiting for worker exit.\n");
        else if (state->terminate_status==ERROR_CANCELLED)
            fwprintf(stdout,L"Termination cancelled.\n");
        else
            fwprintf(stdout,L"Termination failed (error %lu).\n",(unsigned long)state->terminate_status);
    }
    fwprintf(stdout,L"\nUP/DOWN=Select Task\tDEL=End Task\tESC=EXIT\n");
    fflush(stdout);
}
static DWORD refresh(DTASKMGR_STATE *state,DTASKMGR_WORKER **items,ULONG *count)
{
    hyper epoch=0;
    ULONG result_count=0;
    DTASKMGR_WORKER *result=NULL;
    DWORD error=ERROR_SUCCESS;
    if (!state->binding && !bind_basesrv(state)) return GetLastError();
    RpcTryExcept {
        error=Client_TaskSnapshot(state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,&epoch,&result_count,&result);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error) { if (result) MIDL_user_free(result); return error; }
    *items=result; *count=result_count;
    /* A restarted broker may recycle a local worker sequence.  A visual
     * selection is valid only for the epoch in which it was observed. */
    if (state->selected_epoch && state->selected_epoch!=(uint64_t)epoch)
        state->selected_sequence=0;
    state->selected_epoch=(uint64_t)epoch;
    if (result_count && !state->selected_sequence) state->selected_sequence=result[0].sequence;
    if (state->selected_sequence) {
        ULONG index; BOOL found=FALSE;
        for (index=0;index<result_count;++index) if (result[index].sequence==state->selected_sequence) found=TRUE;
        if (!found) state->selected_sequence=result_count ? result[0].sequence : 0;
    }
    return ERROR_SUCCESS;
}
static DWORD request_terminate(DTASKMGR_STATE *state)
{
    DWORD error=ERROR_SUCCESS;
    if (!state->selected_sequence) return ERROR_NOT_FOUND;
    fwprintf(stdout,L"End worker %lu and every DOS/WOW task it hosts? [Y/N] ",(unsigned long)state->selected_sequence);
    fflush(stdout);
    for (;;) {
        INPUT_RECORD record; DWORD read=0;
        if (!ReadConsoleInputW(GetStdHandle(STD_INPUT_HANDLE),&record,1,&read)) return GetLastError();
        if (record.EventType!=KEY_EVENT || !record.Event.KeyEvent.bKeyDown) continue;
        if (record.Event.KeyEvent.uChar.UnicodeChar==L'n' || record.Event.KeyEvent.uChar.UnicodeChar==L'N' ||
            record.Event.KeyEvent.wVirtualKeyCode==VK_ESCAPE) return ERROR_CANCELLED;
        if (record.Event.KeyEvent.uChar.UnicodeChar==L'y' || record.Event.KeyEvent.uChar.UnicodeChar==L'Y') break;
    }
    RpcTryExcept {
        error=Client_TerminateWorker(state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,(hyper)state->selected_epoch,state->selected_sequence);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}
int wmain(void)
{
    DTASKMGR_STATE state;
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE),output=GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    ZeroMemory(&state,sizeof(state));
    if (!GetConsoleMode(input,&mode) || !SetConsoleMode(input,mode|ENABLE_WINDOW_INPUT)) return (int)GetLastError();
    for (;;) {
        DTASKMGR_WORKER *items=NULL; ULONG count=0; DWORD wait,error;
        error=refresh(&state,&items,&count); state.status=error;
        render(output,&state,items,count);
        wait=WaitForSingleObject(input,750);
        if (wait==WAIT_OBJECT_0) {
            INPUT_RECORD record; DWORD read=0;
            if (ReadConsoleInputW(input,&record,1,&read) && record.EventType==KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
                WORD key=record.Event.KeyEvent.wVirtualKeyCode;
                ULONG index;
                if (key==VK_ESCAPE || record.Event.KeyEvent.uChar.UnicodeChar==L'q' || record.Event.KeyEvent.uChar.UnicodeChar==L'Q') { if (items) MIDL_user_free(items); break; }
                for (index=0;index<count;++index) if (items[index].sequence==state.selected_sequence) break;
                if (key==VK_UP && count) state.selected_sequence=items[index ? index-1 : 0].sequence;
                if (key==VK_DOWN && count) state.selected_sequence=items[index+1<count ? index+1 : count-1].sequence;
                if (key==VK_DELETE || record.Event.KeyEvent.uChar.UnicodeChar==L'k' || record.Event.KeyEvent.uChar.UnicodeChar==L'K') {
                    state.terminate_status=request_terminate(&state);
                    state.terminate_attempted=TRUE;
                }
            }
        }
        if (items) MIDL_user_free(items);
    }
    if (state.binding) RpcBindingFree(&state.binding);
    if (state.process) CloseHandle(state.process);
    return 0;
}
