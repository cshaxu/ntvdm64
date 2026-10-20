/* Exact selected original functions/cases; fake allocation service only.
 * This is supplementary fault/ownership evidence, not guest or USER proof. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "wow_message_lifetime.h"
/* Same effective x86 convention as original FASTCALL under /Gz. */
#define FASTCALL __stdcall
typedef WORD HAND16, HMEM16, HWND16;
typedef DWORD VPVOID;
#define GlobalLock16 WOWGlobalLockSize16
#define GlobalUnlockFree16 WOWGlobalUnlockFree16
typedef struct { struct { WORD hwnd,wMsg,wParam,hInst; LONG lParam; } WndProc;
    struct { SHORT nCode,wParam; DWORD lParam; } HookProc; } PARM16;
typedef struct { int iClass; } WW, *PWW;
typedef struct _WM32MSGPARAMEX {
    HWND hwnd; UINT uMsg,uParam; LONG lParam; PARM16 Parm16;
    BOOL fThunk; DWORD dwParam; BOOL fFree; DWORD dwTmp[4]; PWW pww; LONG lReturn;
    BOOL (FASTCALL *lpfnM32)(struct _WM32MSGPARAMEX *);
} WM32MSGPARAMEX,*LPWM32MSGPARAMEX;
typedef struct _MSGPARAMEX {
    PARM16 Parm16; HWND hwnd; UINT uMsg,uParam; LONG lParam,lReturn;
    VOID (FASTCALL *lpfnUnThunk16)(struct _MSGPARAMEX *);
    PWW pww; int iMsgThunkClass,iClass;
    union { struct { DWORD header,data,flags; } copydata;
        struct {DWORD flags,native;} text; } MsgBuffer[1];
    BOOL fThunkSucceeded;
} MSGPARAMEX,*LPMSGPARAMEX;
typedef struct { DWORD dwData,cbData,lpData; } COPYDATASTRUCT16,*PCOPYDATASTRUCT16;
typedef struct _CPDATA { struct _CPDATA *Next; HAND16 To_hwnd,From_hwnd; DWORD Mem16,Mem32,Flags; } CPDATA,*PCPDATA;
typedef struct { WORD hwnd,message,wParam; LONG lParam; } CWPSTRUCT16,*PCWPSTRUCT16;
typedef struct { WORD hwnd,message,wParam; LONG lParam; DWORD time; struct { SHORT x,y; } pt; } MSG16,*PMSG16;
typedef struct { DWORD Proc16; } HOOKSTATEDATA,*LPHOOKSTATEDATA;
typedef DWORD *PVPVOID;
static BOOL fThunkDDEmsg=TRUE;
static unsigned hook_mode,hook_depth;
static BOOL hook_cwp;
static BOOL selector_checks;
static CWPSTRUCT cwp_input;
static MSG filter_input;
static PCPDATA pCPDataFirst;
static unsigned errors,attempt,fail_at,live,scenarios;
static unsigned posted,normal_unthunks;
static BOOL copy_fault;
static void *revoke_header;
static struct { void *pointer; DWORD far_value; } allocations[64];
static COPYDATASTRUCT16 input16;
static COPYDATASTRUCT16 embedded16;
static BYTE input_bytes[8]={1,2,3,4,5,6,7,8};
#define CHECK(x) do { if(!(x)){++errors;printf("FAIL line=%d %s\n",__LINE__,#x);} } while(0)
#define THUNKMSG TRUE
#define UNTHUNKMSG FALSE
#define COPYDATA_16 1
#define COPYDATA_HEADER 2
#define GETHWND16(x) ((WORD)(ULONG_PTR)(x))
#define HWND32(x) ((HWND)(ULONG_PTR)(WORD)(x))
#define INT32(x) ((INT)(SHORT)(x))
#define LOGDEBUG(level,args) ((void)0)
#define WOW32ASSERT(x) ((void)0)
#define WOW32ASSERTMSG(x,s) ((void)0)
#define WOWCLASS_NOTHUNK 255
#define WOWCLASS_UNKNOWN 0
#define WOWCLASS_WIN16 1
static void *allocate(DWORD bytes,BOOL guest) {
    unsigned i;
    if(++attempt==fail_at)return NULL;
    for(i=0;i<64;i++)if(!allocations[i].pointer){
        allocations[i].pointer=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytes);
        allocations[i].far_value=guest?0x10000000u+i*0x10000u:0;
        if(allocations[i].pointer)++live;
        return allocations[i].pointer;
    }
    CHECK(FALSE);return NULL;
}
static void free_w(void *pointer) {
    unsigned i;
    for(i=0;i<64;i++)if(allocations[i].pointer==pointer&&pointer){
        HeapFree(GetProcessHeap(),0,pointer);allocations[i].pointer=NULL;--live;return;
    }
    CHECK(FALSE);
}
static void *malloc_w(DWORD bytes){return allocate(bytes,FALSE);}
static VPVOID GlobalAllocLock16(UINT flags,DWORD bytes,HAND16 *handle) {
    DWORD old;
    if(revoke_header){CHECK(VirtualProtect(revoke_header,4096,PAGE_NOACCESS,&old));revoke_header=NULL;}
    void *pointer=allocate(bytes,TRUE);unsigned i;(void)flags;
    for(i=0;i<64;i++)if(pointer&&allocations[i].pointer==pointer){*handle=(WORD)(i+1);return allocations[i].far_value;}
    return 0;
}
static void *guest_pointer(DWORD value) {
    unsigned i;
    if(value==0x01000000u)return &input16;
    if(value==0x02000000u)return input_bytes;
    if(value==0x03000000u)return &embedded16;
    for(i=0;i<64;i++)if(allocations[i].pointer&&allocations[i].far_value==value)return allocations[i].pointer;
    return NULL;
}
static HMEM16 WINAPI GlobalUnlockFree16(VPVOID value){free_w(guest_pointer(value));return 0;}
static void copy_bytes(void *to,const void *from,SIZE_T bytes){if(copy_fault)RaiseException(0xe0420950,0,0,NULL);memcpy(to,from,bytes);}
#undef RtlCopyMemory
#define RtlCopyMemory copy_bytes
#define GETMISCPTR(v,p) ((p)=guest_pointer((DWORD)(v)))
#define FREEMISCPTR(p) ((void)(p))
#define FREEVDMPTR(p) ((void)(p))
#define FLUSHVDMPTR(v,n,p) ((void)(p))
#define STOREWORD(d,v) (*(WORD *)&(d)=(WORD)(v))
#define STORELONG(d,v) (*(LONG *)&(d)=(LONG)(v))
#define FETCHWORD(v) ((WORD)(v))
#define FETCHLONG(v) ((LONG)(v))
#define FETCHSHORT(v) ((SHORT)(v))
#define RET_HOOKPROC 1
#define OLDCB_DIR (WM_USER+5)
#define TDF_INITCALLBACKSTACK 1u
#define TDF_IGNOREINPUT 2u
typedef struct _TD {DWORD vpCBStack,dwFlags;WORD htask16;} TD,*PTD;
static TD task={0,0,1};
#define CURRENTPTD() (&task)
BOOL WINAPI wow_user_worker_active(void){return TRUE;}
/* Other finalizer families are outside this COPYDATA fixture. Fail closed
 * if one is ever called; address comparisons are not a mock behavior pass. */
BOOL FASTCALL WM32Create(LPWM32MSGPARAMEX p){(void)p;CHECK(FALSE);return FALSE;}
BOOL FASTCALL WM32MDICreate(LPWM32MSGPARAMEX p){(void)p;CHECK(FALSE);return FALSE;}
BOOL FASTCALL WM32SizeClipBoard(LPWM32MSGPARAMEX p){(void)p;CHECK(FALSE);return FALSE;}
BOOL FASTCALL WM32WinHelp(LPWM32MSGPARAMEX p){(void)p;CHECK(FALSE);return FALSE;}
BOOL FASTCALL WM32SetText(LPWM32MSGPARAMEX p){(void)p;CHECK(FALSE);return FALSE;}
BOOL FASTCALL WM32CBDir(LPWM32MSGPARAMEX p){(void)p;CHECK(FALSE);return FALSE;}
VOID FASTCALL UnThunkCBMsg16(LPMSGPARAMEX p){(void)p;CHECK(FALSE);}
static VPVOID WINAPI GlobalLock16(HMEM16 handle,LPDWORD size){(void)handle;(void)size;CHECK(FALSE);return 0;}
BOOL FASTCALL WM32CopyData(LPWM32MSGPARAMEX);
BOOL FASTCALL ThunkWMMsg16(LPMSGPARAMEX);
VOID FASTCALL UnThunkWMMsg16(LPMSGPARAMEX);
static VOID FASTCALL count_normal_unthunk(LPMSGPARAMEX p);
typedef struct { WORD f1,f2,f3; LONG f4; } POSTAPPMESSAGE16,*PPOSTAPPMESSAGE16;
typedef void *PVDMFRAME;
#define GETARGPTR(frame,size,arg) ((arg)=(PPOSTAPPMESSAGE16)(frame))
#define FREEARGPTR(arg) ((void)(arg))
#define THREADID32(task) ((DWORD)(task))
#define WORD32(value) ((UINT)(WORD)(value))
#define LONG32(value) ((LONG)(value))
#define MSG16NEEDSTHUNKING(p) ((p)->iClass!=WOWCLASS_NOTHUNK)
#define RETURN(value) return (value)
static BOOL fake_post(DWORD thread,UINT message,WPARAM wp,LPARAM lp){
    CHECK(thread==7&&message==WM_USER+2&&wp==3&&lp==4);++posted;return TRUE;
}
#undef PostThreadMessage
#define PostThreadMessage fake_post
LONG ThunkCallWndProcHook(INT,LONG,LPCWPSTRUCT,LPHOOKSTATEDATA);
LONG ThunkMsgFilterHook(INT,LONG,LPMSG,LPHOOKSTATEDATA);
static VPVOID malloc16(DWORD bytes){HAND16 handle;return GlobalAllocLock16(0,bytes,&handle);}
#define free16 GlobalUnlockFree16
#define stackalloc16 malloc16
#define stackfree16 GlobalUnlockFree16
static BOOL CallBack16(INT code,PARM16 *args,DWORD procedure,PVPVOID result);
static BOOL FASTCALL WM32NoThunking(LPWM32MSGPARAMEX p){(void)p;return TRUE;}
static struct { BOOL (FASTCALL *lpfnM32)(LPWM32MSGPARAMEX); } aw32Msg[WM_USER];
static PWW FindPWW(HWND window,int cls){(void)window;(void)cls;return NULL;}
static BOOL (FASTCALL *apfnThunkMsg16[])(LPMSGPARAMEX)={ThunkWMMsg16,ThunkWMMsg16};
static VOID (FASTCALL *apfnUnThunkMsg16[])(LPMSGPARAMEX)={count_normal_unthunk,count_normal_unthunk};
#include "copydata_exact_body.h"
static ULONG fixture_get_queue(void);
static ULONG fixture_peek_queue(UINT);
static unsigned queue_mode,queue_depth,queue_calls;
static BOOL queue_expected;
static ULONG putmsg16(DWORD guest,LPMSG message){
    BOOL entering=fThunkDDEmsg;
    (void)guest;(void)message;++queue_calls;
    CHECK(entering==queue_expected);
    if(!queue_depth&&queue_mode==2){
        ++queue_depth;queue_expected=TRUE;
        CHECK(fixture_get_queue()==1234&&fThunkDDEmsg==entering);
        queue_expected=FALSE;
        CHECK(fixture_peek_queue(PM_NOREMOVE)==1234&&fThunkDDEmsg==entering);
        queue_expected=entering;--queue_depth;
    }
    if(queue_mode==1)RaiseException(0xe0420952,0,0,NULL);
    return 1234;
}
#include "queue_selector_intervals.h"
static VOID FASTCALL count_normal_unthunk(LPMSGPARAMEX p){++normal_unthunks;UnThunkWMMsg16(p);}
static BOOL CallBack16(INT code,PARM16 *args,DWORD procedure,PVPVOID result) {
    static DWORD outer_vp,outer_header;
    CWPSTRUCT16 *message=guest_pointer(args->HookProc.lParam);
    COPYDATASTRUCT *copy;HOOKSTATEDATA hook={1};MSG different=filter_input;
    CHECK(code==RET_HOOKPROC&&procedure==1&&message);
    if(selector_checks)CHECK(fThunkDDEmsg==TRUE);
    if(message&&message->message==WM_COPYDATA){
        copy=guest_pointer((DWORD)message->lParam);
        CHECK(copy&&copy->dwData==123&&copy->cbData==8&&
            !memcmp(guest_pointer((DWORD)copy->lpData),input_bytes,8));
        if(hook_depth){
            CHECK((DWORD)message->lParam==outer_header);
            CHECK(hook_cwp?args->HookProc.lParam!=outer_vp:args->HookProc.lParam==outer_vp);
        }else{
            outer_vp=args->HookProc.lParam;outer_header=(DWORD)message->lParam;
            if(hook_mode==3)RaiseException(0xe0420951,0,0,NULL);
            ++hook_depth;
            if(hook_mode==2){different.message=WM_APP+1;different.lParam=0;CHECK(!ThunkMsgFilterHook(0,0,&different,&hook));}
            if(hook_mode==1||hook_mode==2){
                if(hook_cwp)CHECK(!ThunkCallWndProcHook(0,0,&cwp_input,&hook));
                else CHECK(!ThunkMsgFilterHook(0,0,&filter_input,&hook));
            }
            --hook_depth;
            if(selector_checks)CHECK(fThunkDDEmsg==TRUE);
            CHECK(guest_pointer(outer_header)==copy);
            if(hook_mode==4)message->hwnd=3; /* No native-pair lifetime for this replacement. */
        }
    }
    *result=0;return TRUE;
}
static void reset(unsigned failure) {
    unsigned i;
    CHECK(live==0&&pCPDataFirst==NULL);
    for(i=0;i<64;i++)CHECK(!allocations[i].pointer);
    attempt=0;fail_at=failure;copy_fault=FALSE;++scenarios;
    input16.dwData=123;input16.cbData=8;input16.lpData=0x02000000u;
}
static WM32MSGPARAMEX forward(COPYDATASTRUCT *input) {
    WM32MSGPARAMEX p={0};p.hwnd=(HWND)1;p.uParam=2;p.uMsg=WM_COPYDATA;p.lParam=(LONG)input;p.fThunk=TRUE;p.fFree=TRUE;return p;
}
static MSGPARAMEX reverse(void) {
    MSGPARAMEX p={0};p.Parm16.WndProc.hwnd=1;p.Parm16.WndProc.wParam=2;p.Parm16.WndProc.wMsg=WM_COPYDATA;p.Parm16.WndProc.lParam=0x01000000;return p;
}
int main(void) {
    unsigned failure;WM32MSGPARAMEX a,b;MSGPARAMEX x,y;DWORD protection;
    COPYDATASTRUCT *native=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    C_ASSERT(sizeof(void *)==4);
    if(!native)return 2;
    native->dwData=123;native->cbData=8;native->lpData=input_bytes;
    aw32Msg[WM_COPYDATA].lpfnM32=WM32CopyData;
    reset(0);a=forward(native);revoke_header=native;
    __try {CHECK(WM32CopyData(&a));wow_message_unwind32(&a);CHECK(!live&&!pCPDataFirst);}
    __except(GetExceptionCode()==EXCEPTION_ACCESS_VIOLATION?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){
        CHECK(FALSE);printf("PRE_REENTRY_HEADER_READ violation=1 rollback_live=%u\n",live);
    }
    CHECK(VirtualProtect(native,4096,PAGE_READWRITE,&protection));
    {POSTAPPMESSAGE16 args={7,WM_COPYDATA,3,0x01000000};
     reset(0);CHECK(!WU32PostAppMessage(&args)&&!posted&&!normal_unthunks&&!live);
     reset(0);args.f2=WM_USER+2;args.f4=4;
     CHECK(WU32PostAppMessage(&args)==TRUE&&posted==1&&normal_unthunks==1&&!live);
    }
    for(failure=1;failure<=4;failure++){
        reset(failure);a=forward(native);CHECK(!WM32CopyData(&a));CHECK(!live&&!pCPDataFirst);
        reset(failure);x=reverse();(void)ThunkMsg16(&x);CHECK(!x.fThunkSucceeded);CHECK(!live&&!pCPDataFirst);
    }
    reset(0);a=forward(native);CHECK(WM32CopyData(&a));CHECK(live==4);
    b=forward(native);CHECK(WM32CopyData(&b));CHECK(b.Parm16.WndProc.lParam==a.Parm16.WndProc.lParam&&!b.dwParam);
    wow_message_unwind32(&b);CHECK(live==4);
    CHECK(VirtualProtect(native,4096,PAGE_NOACCESS,&protection));
    SetLastError(321);wow_message_unwind32(&a);CHECK(GetLastError()==321&&!live&&!pCPDataFirst);
    CHECK(VirtualProtect(native,4096,PAGE_READWRITE,&protection));
    wow_message_unwind32(&a);CHECK(!live);
    reset(0);x=reverse();CHECK(ThunkMsg16(&x)==(HWND)1&&x.fThunkSucceeded&&live==4);
    y=reverse();CHECK(ThunkMsg16(&y)==(HWND)1&&!y.MsgBuffer->copydata.flags);wow_message_unwind16(&y);CHECK(live==4);
    a=forward((COPYDATASTRUCT *)x.lParam);CHECK(WM32CopyData(&a));CHECK(a.Parm16.WndProc.lParam==0x01000000&&!a.dwParam);
    wow_message_unwind32(&a);CHECK(live==4);
    ((COPYDATASTRUCT *)x.lParam)->lpData=(void *)1; /* Release never rereads mutable header. */
    SetLastError(654);wow_message_unwind16(&x);CHECK(GetLastError()==654&&!live&&!pCPDataFirst);wow_message_unwind16(&x);
    reset(0);CHECK(!CopyDataDeleteNode(1,2,99));
    a=forward(native);copy_fault=TRUE;
    __try{(void)WM32CopyData(&a);CHECK(FALSE);}__except(GetExceptionCode()==0xe0420950?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){CHECK(!live&&!pCPDataFirst);}
    reset(0);x=reverse();copy_fault=TRUE;
    __try{(void)ThunkMsg16(&x);CHECK(FALSE);}__except(GetExceptionCode()==0xe0420950?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){CHECK(!live&&!pCPDataFirst);}
    reset(0);native->cbData=0;a=forward(native);CHECK(WM32CopyData(&a)&&live==2);wow_message_unwind32(&a);
    input16.cbData=0;x=reverse();(void)ThunkMsg16(&x);CHECK(x.fThunkSucceeded&&live==2);wow_message_unwind16(&x);
    reset(0);x=reverse();x.Parm16.WndProc.hwnd=0;x.Parm16.WndProc.wMsg=WM_USER+2;CHECK(!ThunkMsg16(&x)&&x.fThunkSucceeded);
    x=reverse();x.Parm16.WndProc.hwnd=0;(void)ThunkMsg16(&x);CHECK(!x.fThunkSucceeded&&!live);
    {COPYDATASTRUCT embedded={123,8,input_bytes};
     reset(0);native->cbData=sizeof(embedded);native->lpData=&embedded;
     a=forward(native);CHECK(WM32CopyData(&a)&&live==4);
     b=forward(&embedded);CHECK(WM32CopyData(&b)&&live==8&&(b.dwParam&WOW_COPY_HEADER));
     CHECK((DWORD)b.Parm16.WndProc.lParam!=a.dwTmp[0]);
     wow_message_unwind32(&b);CHECK(live==4);wow_message_unwind32(&a);CHECK(!live);
     native->lpData=input_bytes;
    }
    reset(0);embedded16.dwData=123;embedded16.cbData=8;embedded16.lpData=0x02000000u;
    input16.cbData=sizeof(embedded16);input16.lpData=0x03000000u;
    x=reverse();(void)ThunkMsg16(&x);CHECK(x.fThunkSucceeded&&live==4);
    y=reverse();y.Parm16.WndProc.lParam=0x03000000u;(void)ThunkMsg16(&y);
    CHECK(y.fThunkSucceeded&&live==8&&(y.MsgBuffer->copydata.flags&WOW_COPY_HEADER));
    CHECK((DWORD)y.lParam!=x.MsgBuffer->copydata.data);
    wow_message_unwind16(&y);CHECK(live==4);wow_message_unwind16(&x);CHECK(!live);
    native->cbData=8;
    cwp_input.hwnd=(HWND)1;cwp_input.message=WM_COPYDATA;cwp_input.wParam=2;cwp_input.lParam=(LPARAM)native;
    filter_input.hwnd=(HWND)1;filter_input.message=WM_COPYDATA;filter_input.wParam=2;filter_input.lParam=(LPARAM)native;
    {HOOKSTATEDATA hook={1};BOOL caught;
     reset(0);hook_cwp=TRUE;hook_mode=1;CHECK(!ThunkCallWndProcHook(0,0,&cwp_input,&hook));CHECK(!live&&!pCPDataFirst);
     reset(0);hook_cwp=FALSE;hook_mode=1;CHECK(!ThunkMsgFilterHook(0,0,&filter_input,&hook));CHECK(!live&&!pCPDataFirst);
     reset(0);hook_mode=2;CHECK(!ThunkMsgFilterHook(0,0,&filter_input,&hook));CHECK(!live&&!pCPDataFirst);
     reset(0);hook_mode=4;CHECK(!ThunkMsgFilterHook(0,0,&filter_input,&hook));
     CHECK(filter_input.hwnd==(HWND)1&&!live&&!pCPDataFirst);
     for(hook_cwp=FALSE;hook_cwp<=TRUE;++hook_cwp){
        reset(0);hook_mode=3;caught=FALSE;
        __try{if(hook_cwp)(void)ThunkCallWndProcHook(0,0,&cwp_input,&hook);else (void)ThunkMsgFilterHook(0,0,&filter_input,&hook);CHECK(FALSE);}
        __except(GetExceptionCode()==0xe0420951?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){caught=TRUE;}
        CHECK(caught&&!live&&!pCPDataFirst);
     }
     for(hook_cwp=FALSE;hook_cwp<=TRUE;++hook_cwp){
        reset(5);hook_mode=0;
        if(hook_cwp)CHECK(!ThunkCallWndProcHook(0,0,&cwp_input,&hook));else CHECK(!ThunkMsgFilterHook(0,0,&filter_input,&hook));
        CHECK(!live&&!pCPDataFirst);
     }
    }
    /* DIV-350: exercise the complete original filter owner, not an isolated
     * save/restore helper. COPYDATA services are doubles; the operation selector
     * is shared by DDE and must preserve the actual entering value on every
     * path, including nested calls of a different message family. */
    {HOOKSTATEDATA hook={1};unsigned entering,mode,selector_cases=0;BOOL caught;
     selector_checks=TRUE;hook_cwp=FALSE;
     for(entering=0;entering<=1;++entering)for(mode=0;mode<7;++mode){
        reset(mode==5?1:0);hook_mode=mode<5?mode:0;
        copy_fault=mode==6;fThunkDDEmsg=(BOOL)entering;caught=FALSE;
        __try{CHECK(!ThunkMsgFilterHook(0,0,&filter_input,&hook));}
        __except((GetExceptionCode()==0xe0420951||GetExceptionCode()==0xe0420950)?
                EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){caught=TRUE;}
        CHECK(caught==(mode==3||mode==6));
        CHECK(fThunkDDEmsg==(BOOL)entering&&!live&&!pCPDataFirst&&!hook_depth);
        ++selector_cases;
     }
     selector_checks=FALSE;
     printf("FILTER_SELECTOR cases=%u entry_values=2 modes=7 errors=%u guest_proof=0\n",
         selector_cases,errors);
    }
    {unsigned entering,owner,mode,cases=0,prior_errors=errors;BOOL caught;ULONG result;
     for(entering=0;entering<2;++entering)for(owner=0;owner<3;++owner)
     for(mode=0;mode<3;++mode){
        reset(0);queue_mode=mode;queue_calls=0;queue_expected=owner!=2;
        fThunkDDEmsg=(BOOL)entering;caught=FALSE;result=0;
        __try{result=owner==0?fixture_get_queue():fixture_peek_queue(owner==1?PM_REMOVE:PM_NOREMOVE);}
        __except(GetExceptionCode()==0xe0420952?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){caught=TRUE;}
        CHECK(caught==(mode==1)&&result==(mode==1?0u:1234u));
        CHECK(fThunkDDEmsg==(BOOL)entering&&!queue_depth&&queue_calls==(mode==2?3u:1u));
        ++cases;
     }
     printf("QUEUE_SELECTOR cases=%u owners=3 modes=3 errors=%u full_queue_proof=0 guest_proof=0\n",
         cases,errors-prior_errors);
    }
    CHECK(!pCPDataFirst);VirtualFree(native,0,MEM_RELEASE);
    printf("COPYDATA_LIFETIME errors=%u scenarios=%u directions=2 stages=4 nested=1 immutable_release=1 exception=2 thread_status=1 hook_roles=2 filter_nested_different=1 embedded_headers=2 filter_output_rollback=1 guest_proof=0\n",errors,scenarios);
    return errors?1:0;
}
