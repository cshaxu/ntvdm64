/* Native diagnostic recorder/reader unit double only, not WOW acceptance. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "wow_callback_cost_instrument.h"
typedef struct wow_window_dispatch_target {DWORD unused;} wow_window_dispatch_target;
static BOOL WINAPI callback_cost_body(HWND,UINT,WPARAM,LPARAM,
    const wow_window_dispatch_target *,LONG *);
#include "wow_callback_cost_dispatch.h"
#include "wow_callback_cost_probe.h"
static unsigned errors;
static int mode;
#define CHECK(value) do {if(!(value)){fprintf(stderr,"callback-cost unit line=%u error=%lu\n",__LINE__,GetLastError());++errors;}}while(0)
static BOOL WINAPI callback_cost_body(HWND window,UINT message,WPARAM wp,LPARAM lp,
    const wow_window_dispatch_target *target,LONG *result)
{
    (void)message;
    CHECK(window==(HWND)0x1234 && wp==3 && lp==4 && target!=NULL);
    if(mode==1){
        LONG nested=0,untracked=0;
        callback_cost_phase_enter();
        mode=0;SetLastError(0x447100u);
        CHECK(wow_window_dispatch_immutable(window,WM_RBUTTONUP,wp,lp,target,&nested)==7);
        CHECK(nested==0x55 && GetLastError()==0x447200u);
        SetLastError(0x447100u);
        CHECK(wow_window_dispatch_immutable(window,WM_NULL,wp,lp,target,&untracked)==7);
        callback_cost_phase_leave();
        *result=0x66;SetLastError(0x447300u);return TRUE;
    }
    if(mode==2){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    if(mode==3){SetLastError(0x447400u);RaiseException(0xe4470011u,0,0,NULL);}
    CHECK(GetLastError()==0x447100u);
    callback_cost_phase_enter();callback_cost_phase_leave();
    *result=0x55;SetLastError(0x447200u);return 7;
}
int main(void)
{
    wow_window_dispatch_target target={0};
    callback_cost_data readback;
    LONG value=0,saved;
    LONGLONG saved_phase;
    HANDLE process=NULL;
    ULONG_PTR address=0;
    DWORD rva,pid=GetCurrentProcessId(),before=0,after=0;
    char path[MAX_PATH],*name;
    unsigned i;
    GetModuleFileNameA(NULL,path,sizeof(path));name=strrchr(path,'\\');name=name?name+1:path;
    rva=(DWORD)((ULONG_PTR)&T447CallbackCosts-(ULONG_PTR)GetModuleHandleA(NULL));
    mode=1;SetLastError(0x447100u);
    CHECK(wow_window_dispatch_immutable((HWND)0x1234,WM_RBUTTONDOWN,3,4,&target,&value)==TRUE);
    CHECK(value==0x66 && GetLastError()==0x447300u && T447CallbackCosts.reserved==2 &&
        T447CallbackCosts.rows[0].entered<=T447CallbackCosts.rows[1].entered &&
        T447CallbackCosts.rows[0].returned>=T447CallbackCosts.rows[1].returned &&
        T447CallbackCosts.rows[0].phase_count==1 && T447CallbackCosts.rows[1].phase_count==1 &&
        callback_cost_active==-1);
    mode=2;CHECK(!wow_window_dispatch_immutable((HWND)0x1234,WM_RBUTTONDOWN,3,4,&target,NULL));
    CHECK(GetLastError()==ERROR_INVALID_PARAMETER && T447CallbackCosts.rows[2].normal &&
        !T447CallbackCosts.rows[2].accepted && !T447CallbackCosts.rows[2].result);
    mode=3;
    __try{(void)wow_window_dispatch_immutable((HWND)0x1234,WM_RBUTTONUP,3,4,&target,NULL);CHECK(FALSE);}
    __except(GetExceptionCode()==0xe4470011u?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){
        CHECK(T447CallbackCosts.reserved==4 && T447CallbackCosts.rows[3].complete &&
            !T447CallbackCosts.rows[3].normal && !T447CallbackCosts.rows[3].accepted);
    }
    mode=0;SetLastError(0x447100u);
    CHECK(wow_window_dispatch_immutable((HWND)0x1234,WM_NULL,3,4,&target,&value)==7);
    CHECK(value==0x55 && GetLastError()==0x447200u && T447CallbackCosts.reserved==4);
    CHECK(callback_cost_open(pid,name,rva,&process,&address));if(!process)return 1;
    CHECK(callback_cost_read(process,address,&readback)==1 &&
        readback.reserved==4 && readback.rows[0].result==0x66 && readback.rows[1].result==0x55);
    T447CallbackCosts.magic=0;CHECK(callback_cost_read(process,address,&readback)==-1);T447CallbackCosts.magic=CALLBACK_COST_MAGIC;
    T447CallbackCosts.version=3;CHECK(callback_cost_read(process,address,&readback)==-1);T447CallbackCosts.version=2;
    T447CallbackCosts.capacity=1;CHECK(callback_cost_read(process,address,&readback)==-1);T447CallbackCosts.capacity=CALLBACK_COST_CAPACITY;
    T447CallbackCosts.rows[0].complete=0;CHECK(callback_cost_read(process,address,&readback)==0);T447CallbackCosts.rows[0].complete=1;
    T447CallbackCosts.rows[0].complete=2;CHECK(callback_cost_read(process,address,&readback)==-1);T447CallbackCosts.rows[0].complete=1;
    saved=T447CallbackCosts.reserved;T447CallbackCosts.reserved=-1;CHECK(callback_cost_read(process,address,&readback)==-1);T447CallbackCosts.reserved=saved;
    T447CallbackCosts.rows[0].phase_count=2;CHECK(callback_cost_read(process,address,&readback)==-1);T447CallbackCosts.rows[0].phase_count=1;
    saved_phase=T447CallbackCosts.rows[0].phase_entered;T447CallbackCosts.rows[0].phase_entered=T447CallbackCosts.rows[0].returned+1;CHECK(callback_cost_read(process,address,&readback)==-1);T447CallbackCosts.rows[0].phase_entered=saved_phase;
    saved_phase=T447CallbackCosts.rows[0].phase_returned;T447CallbackCosts.rows[0].phase_returned=T447CallbackCosts.rows[0].entered-1;CHECK(callback_cost_read(process,address,&readback)==-1);T447CallbackCosts.rows[0].phase_returned=saved_phase;
    for(i=0;i<CALLBACK_COST_CAPACITY;++i){SetLastError(0x447100u);CHECK(wow_window_dispatch_immutable((HWND)0x1234,WM_RBUTTONDOWN,3,4,&target,&value)==7);}
    CHECK(T447CallbackCosts.reserved==68 && callback_cost_read(process,address,&readback)==-1);
    CloseHandle(process);process=NULL;
    GetProcessHandleCount(GetCurrentProcess(),&before);
    CHECK(!callback_cost_open(pid,name,MAXDWORD,&process,&address) && !process);
    CHECK(!callback_cost_open(pid,"wrong-module.dll",rva,&process,&address) && !process);
    CHECK(!callback_cost_open(MAXDWORD,name,rva,&process,&address) && !process);
    GetProcessHandleCount(GetCurrentProcess(),&after);CHECK(before==after);
    printf("CALLBACK_COST_UNIT errors=%u nested=1 exact_result_error=1 rejection=1 exception=1 untracked=1 readonly=1 overflow=1 negative_states=9 negative_identities=3 handles=%lu/%lu phase_tls=1\n",errors,before,after);
    return errors?1:0;
}
