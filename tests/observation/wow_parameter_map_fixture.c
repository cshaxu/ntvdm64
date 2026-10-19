/* Exact selected original algorithms; only lower memory/window services doubled.
 * This is not real guest, native USER exception or callback-frame acceptance. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#define FASTCALL __fastcall
typedef WORD HAND16,HMEM16,HWND16,HMENU16,HDC16,HTASK16,HMIDIIN16,HMIDIOUT16,HWAVEIN16,HWAVEOUT16;
typedef DWORD VPVOID,VPSZ,VPSTR,VPBYTE,VPRECT16,VPPROC;
#define GlobalLock16 WOWGlobalLockSize16
#define GlobalUnlockFree16 WOWGlobalUnlockFree16
typedef LPSTR LPSZ,PSZ;
typedef struct {DWORD dwStyle,dwExStyle;} WW,*PWW;
#include "../../src/opennt-abi/host-compat/include/wow_control_message_abi.h"
#include "parameter_types.h"
#include "../../src/mvdm/wow32/wparam.h"
#include "../../src/wow32-dll/include/wow_message_lifetime.h"
#include "parameter_map_records.h"
/* Address-only out-of-scope families; actual invocation is a fixture failure. */
BOOL FASTCALL WM32Create(LPWM32MSGPARAMEX p){(void)p;ExitProcess(90);}
BOOL FASTCALL WM32MDICreate(LPWM32MSGPARAMEX p){(void)p;ExitProcess(91);}
BOOL FASTCALL WM32SizeClipBoard(LPWM32MSGPARAMEX p){(void)p;ExitProcess(92);}
BOOL FASTCALL WM32WinHelp(LPWM32MSGPARAMEX p){(void)p;ExitProcess(93);}
BOOL WINAPI wow_user_worker_active(void){return TRUE;}
VPVOID WINAPI GlobalLock16(HMEM16 handle,LPDWORD size){(void)handle;(void)size;ExitProcess(94);}
HMEM16 WINAPI GlobalUnlockFree16(VPVOID pointer){(void)pointer;ExitProcess(95);}
typedef struct {DWORD dwWOWCompatFlags;HAND16 htask16;} TASK;
typedef struct {WORD wTDB;} VDMFRAME,*PVDMFRAME;
typedef struct {WORD TDB_CompatHandle;} TDB,*PTDB;
static TASK task={0,0x1234};static TDB tdb={0x77};static SETWINDOWTEXT16 arguments;
static WW words;static char text[20][32];static unsigned view_shift,mode_fault;
static unsigned failures,scenarios,guest_allocations,guest_frees,stack_releases,flat_releases,native_calls,pin_seen;
static BOOL fail_map,fail_guest,copy_fault,native_fault,mutate_arguments;
static void *last_flat_release;
static const DWORD fault_code=0xe04d4150u;
#define KEY_A 0x10001000u
#define KEY_B 0x10002000u
#define CURRENTPTD() (&task)
#define LOG_ALWAYS 0
#define LOGDEBUG(level,args) ((void)0)
#define WOW32ASSERT(x) ((void)0)
#define WOW32ASSERTMSG(x,s) ((void)0)
#define THUNKMSG TRUE
#define UNTHUNKMSG FALSE
static struct {char bytes[64];unsigned kind;} guest[32];
static void expect(BOOL ok,const char *name){if(!ok){++failures;printf("FAIL %s\n",name);}}
static unsigned node_count(void){LPPARAMNODE p;unsigned n=0;for(p=gParamMap.pHead;p;p=p->pNext)++n;return n;}
static unsigned references(DWORD key){LPPARAMNODE p;for(p=gParamMap.pHead;p;p=p->pNext)if(p->dwPtr16==key)return p->dwRefCount;return 0;}
static void *view(DWORD key){unsigned i;if(key==KEY_A || key==KEY_B)return text[view_shift];for(i=0;i<32;++i)if(key==0x20000000u+i*0x1000u && guest[i].kind)return guest[i].bytes;return NULL;}
static void *GetPModeVDMPointer(DWORD key,DWORD ignored){
 unsigned mode=mode_fault;(void)ignored;
 if(mode==1){SetLastError(0x7341);return NULL;}
 if(mode==2){SetLastError(0x7342);RaiseException(fault_code,0,0,NULL);}
 if(mode==3){DWORD borrowed;mode_fault=0;pin_seen=references(KEY_A);borrowed=GetParam16((DWORD)text[0]);if(borrowed)DeleteParamMap(borrowed,PARAM_16,NULL);}
 return view(key);
}
static void *malloc_w(DWORD bytes){return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytes);}
static void free_w(void *p){if(p)HeapFree(GetProcessHeap(),0,p);}
#define CacheBlockAllocate original_CacheBlockAllocate
#include "parameter_cache.h"
#undef CacheBlockAllocate
LPVOID CacheBlockAllocate(PBLKCACHE cache,DWORD bytes){return fail_map ? NULL : original_CacheBlockAllocate(cache,bytes);}
static VPVOID allocate_guest(unsigned bytes,unsigned kind){unsigned i;if(fail_guest)return 0;expect(bytes<=64,"bounded fixture allocation");if(bytes>64)return 0;for(i=0;i<32;++i)if(!guest[i].kind){guest[i].kind=kind;++guest_allocations;return 0x20000000u+i*0x1000u;}return 0;}
static VPVOID FASTCALL malloc16(UINT bytes){return allocate_guest(bytes,1);}
static VPVOID FASTCALL stackalloc16(UINT bytes){return allocate_guest(bytes,2);}
static BOOL FASTCALL free16(VPVOID key){unsigned i;for(i=0;i<32;++i)if(key==0x20000000u+i*0x1000u && guest[i].kind==1){guest[i].kind=0;++guest_frees;return TRUE;}expect(FALSE,"no foreign/double guest free");return FALSE;}
/* Original stackfree16 is empty. Count the release site, not a heap free. */
#define stackfree16(key) ((void)(key),++stack_releases)
static unsigned heap_owned(void){unsigned i,n=0;for(i=0;i<32;++i)if(guest[i].kind==1)++n;return n;}
static void putstr16(VPSZ key,LPSZ source,INT bytes){if(copy_fault){SetLastError(0x7351);RaiseException(fault_code,0,0,NULL);}memcpy(view(key),source,(size_t)bytes);}
static void putrect16(VPRECT16 key,LPRECT rectangle){(void)key;(void)rectangle;}
#define GETPSZPTR(key,p) ((p)=(key) ? view((DWORD)(key)) : NULL)
#define FREEPSZPTR(p) ((last_flat_release=(void *)(p)),++flat_releases)
#define GETVDMPTR(key,n,p) ((void)(n),(p)=view((DWORD)(key)))
#define FREEVDMPTR(p) ((void)(p))
#define FETCHDWORD(v) ((DWORD)(v))
#define GETARGPTR(frame,n,p) ((void)(frame),(void)(n),(p)=&arguments)
#define FREEARGPTR(p) ((void)(p))
#define HWND32(v) ((HWND)(ULONG_PTR)(WORD)(v))
#define SEGPTR(seg,off) ((void)(seg),(void)(off),&tdb)
#define RETURN(v) return (v)
static BOOL fixture_settext(HANDLE window,PSZ value){(void)window;expect(value==text[0] || !value,"native original text argument");++native_calls;if(mutate_arguments)arguments.f2=KEY_B;if(native_fault){SetLastError(0x7361);RaiseException(fault_code,0,0,NULL);}return TRUE;}
#undef SetWindowText
#define SetWindowText fixture_settext
BOOL FASTCALL WM32SetText(LPWM32MSGPARAMEX);
BOOL FASTCALL WM32CBDir(LPWM32MSGPARAMEX);
VOID FASTCALL UnThunkWMMsg16(LPMSGPARAMEX);
VOID FASTCALL UnThunkCBMsg16(LPMSGPARAMEX);
/* COPYDATA itself is exercised by retained exact/real groups, not this model. */
static BOOL FASTCALL WM32CopyData(LPWM32MSGPARAMEX value){value->dwParam=0;return TRUE;}
#include "parameter_exact.h"
static void reset(void){mode_fault=0;fail_map=fail_guest=copy_fault=native_fault=mutate_arguments=FALSE;FreeParamMap(0x1234);FreeParamMap(0x5678);expect(!node_count(),"all test task nodes retire");task.htask16=0x1234;task.dwWOWCompatFlags=0;view_shift=0;memset(guest,0,sizeof(guest));guest_allocations=guest_frees=stack_releases=flat_releases=native_calls=pin_seen=0;last_flat_release=NULL;}
static MSGPARAMEX reverse(UINT message,DWORD key){MSGPARAMEX v;ZeroMemory(&v,sizeof(v));v.Parm16.WndProc.wMsg=(WORD)message;v.Parm16.WndProc.lParam=key;v.lParam=(LONG)key;v.uMsg=message;v.pww=&words;v.lpfnUnThunk16=message==WM_USER+5 ? UnThunkCBMsg16 : UnThunkWMMsg16;return v;}
static BOOL reverse_start(LPMSGPARAMEX v){return v->lpfnUnThunk16==UnThunkCBMsg16 ? ThunkCBMsg16(v) : ThunkWMMsg16(v);}
static WM32MSGPARAMEX forward(UINT message){WM32MSGPARAMEX v;ZeroMemory(&v,sizeof(v));v.fThunk=TRUE;v.uMsg=message;v.lParam=(LONG)text[0];v.lpfnM32=message==CB_DIR ? WM32CBDir : WM32SetText;return v;}
static LONG caught(DWORD code){return code==fault_code ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;}
static void map_cases(void){
 FINDPARAM found;unsigned i;BOOL trapped;DWORD before;
 reset();++scenarios;expect(!AddParamMap(0,0) && !node_count(),"NULL_PAIR");
 reset();AddParamMap((DWORD)text[0],KEY_A);mode_fault=1;++scenarios;expect(!GetParam16(0) && references(KEY_A)==1,"NULL_LOOKUP");
 reset();AddParamMap((DWORD)text[0],KEY_A);mode_fault=1;++scenarios;
 expect(!AddParamMap((DWORD)text[0],KEY_A) && references(KEY_A)==1 && gParamMap.pHead->dwPtr32==(DWORD)text[0] && !gParamMap.pHead->nAliasCount,"UPDATE_FALSE_ROLLBACK");
 reset();AddParamMap((DWORD)text[0],KEY_A);mode_fault=2;trapped=FALSE;++scenarios;
 __try{AddParamMap((DWORD)text[0],KEY_A);}__except(caught(GetExceptionCode())){trapped=TRUE;}
 expect(trapped && references(KEY_A)==1 && GetLastError()==0x7342,"UPDATE_SEH_ROLLBACK");
 reset();AddParamMap((DWORD)text[0],KEY_A);mode_fault=3;++scenarios;
 expect(AddParamMap((DWORD)text[0],KEY_A)!=NULL && pin_seen>=2 && references(KEY_A)==2,"pin survives nested refresh borrow");DeleteParamMap(KEY_A,PARAM_16,NULL);
 reset();AddParamMap((DWORD)text[0],KEY_A);++scenarios;
 for(i=1;i<20;++i){view_shift=i;expect(ParamMapUpdateNode((DWORD)text[i-1],PARAM_32,NULL)==text[i],"original pointer relocation");}
 expect(gParamMap.pHead->nAliasCount==16 && gParamMap.pHead->rgdwAlias[0]==(DWORD)text[18],"original bounded alias policy");
 before=references(KEY_A);expect(GetParam16((DWORD)text[19])==KEY_A && references(KEY_A)==before+1,"current native pointer borrow");DeleteParamMap(KEY_A,PARAM_16,NULL);
 expect(FindParamMap(&found,(DWORD)text[18],PARAM_32)==KEY_A,"retained original alias lookup");
 reset();AddParamMap((DWORD)text[0],KEY_A);task.htask16=0x5678;AddParamMap((DWORD)text[0],KEY_B);++scenarios;FreeParamMap(0x1234);expect(!references(KEY_A) && references(KEY_B)==1,"task purge preserves other owner");
}
static void reverse_cases(void){
 unsigned combo;MSGPARAMEX v;WM32MSGPARAMEX borrowed;
 for(combo=0;combo<2;++combo){
  reset();task.dwWOWCompatFlags=WOWCF_NOCBDIRTHUNK;AddParamMap((DWORD)text[0],KEY_A);fail_map=TRUE;v=reverse(combo ? WM_USER+5 : WM_SETTEXT,KEY_B);++scenarios;
  expect(reverse_start(&v),"optional map keeps original direct view");v.lpfnUnThunk16(&v);
  expect(references(KEY_A)==1 && !references(KEY_B),combo ? "REVERSE_CB_OPTIONAL" : "REVERSE_TEXT_OPTIONAL");
  reset();task.dwWOWCompatFlags=WOWCF_NOCBDIRTHUNK;v=reverse(combo ? WM_USER+5 : WM_SETTEXT,KEY_A);++scenarios;expect(reverse_start(&v),"reverse normal construction");
  SetLastError(0x7371);wow_message_unwind16(&v);expect(!node_count() && GetLastError()==0x7371,combo ? "REVERSE_CB_ABORT" : "REVERSE_TEXT_ABORT");
  reset();task.dwWOWCompatFlags=WOWCF_NOCBDIRTHUNK;v=reverse(combo ? WM_USER+5 : WM_SETTEXT,KEY_A);++scenarios;expect(reverse_start(&v),"nested reverse construction");
  borrowed=forward(combo ? CB_DIR : WM_SETTEXT);expect(borrowed.lpfnM32(&borrowed) && borrowed.Parm16.WndProc.lParam==KEY_A && references(KEY_A)==2,"same original guest pointer returned");
  borrowed.fThunk=FALSE;borrowed.lpfnM32(&borrowed);wow_message_unwind32(&borrowed);expect(references(KEY_A)==1,"inner borrow released once");
  v.lpfnUnThunk16(&v);wow_message_unwind16(&v);expect(!node_count(),"normal plus finalizer exactly once");
 }
 reset();v=reverse(WM_USER+5,0);task.dwWOWCompatFlags=WOWCF_NOCBDIRTHUNK;++scenarios;expect(reverse_start(&v),"NULL CB_DIR unchanged");v.lpfnUnThunk16(&v);wow_message_unwind16(&v);expect(!node_count(),"NULL CB_DIR no node");
}
static void forward_cases(void){
 unsigned combo,stack;WM32MSGPARAMEX v;BOOL trapped;
 for(combo=0;combo<2;++combo){
  reset();task.dwWOWCompatFlags=WOWCF_NOCBDIRTHUNK;AddParamMap((DWORD)text[0],KEY_A);v=forward(combo ? CB_DIR : WM_SETTEXT);++scenarios;expect(v.lpfnM32(&v),"borrow constructor");
  wow_message_unwind32(&v);expect(references(KEY_A)==1 && !heap_owned(),combo ? "FORWARD_CB_ABORT" : "FORWARD_TEXT_ABORT");
  reset();fail_guest=TRUE;v=forward(combo ? CB_DIR : WM_SETTEXT);++scenarios;expect(!v.lpfnM32(&v),"guest fallback allocation rejects failure");wow_message_unwind32(&v);expect(!heap_owned() && !node_count(),"failed fallback owns nothing");
 }
 for(stack=0;stack<2;++stack){
  reset();task.dwWOWCompatFlags=stack ? WOWCF_DSBASEDSTRINGPOINTERS : 0;v=forward(WM_SETTEXT);copy_fault=TRUE;trapped=FALSE;++scenarios;
  __try{__try{WM32SetText(&v);}__finally{wow_message_unwind32(&v);}}__except(caught(GetExceptionCode())){trapped=TRUE;}
  expect(trapped && !heap_owned() && (stack ? stack_releases==1 : guest_frees==1) && GetLastError()==0x7351,stack ? "STACK_COPY_ABORT" : "HEAP_COPY_ABORT");
 }
 reset();v=forward(CB_DIR);++scenarios;expect(WM32CBDir(&v),"CB_DIR heap fallback");wow_message_unwind32(&v);expect(!heap_owned() && guest_frees==1,"CB_DIR fallback abort release");
}
static void api_cases(void){
 VDMFRAME frame={0};BOOL trapped;
 reset();AddParamMap((DWORD)text[0],KEY_A);arguments.f1=1;arguments.f2=KEY_B;fail_map=TRUE;++scenarios;expect(WU32SetWindowText(&frame)==0 && native_calls==1 && references(KEY_A)==1,"API_OPTIONAL");
 reset();arguments.f1=1;arguments.f2=KEY_A;native_fault=TRUE;trapped=FALSE;++scenarios;
 __try{WU32SetWindowText(&frame);}__except(caught(GetExceptionCode())){trapped=TRUE;}
 expect(trapped && !node_count() && GetLastError()==0x7361,"API_ABORT");
 reset();arguments.f1=1;arguments.f2=KEY_A;mutate_arguments=TRUE;++scenarios;expect(WU32SetWindowText(&frame)==0 && !node_count() && arguments.f2==KEY_B,"API cleanup uses captured guest key");
 reset();arguments.f1=1;arguments.f2=0;++scenarios;expect(WU32SetWindowText(&frame)==0 && native_calls==1 && !node_count(),"NULL native text preserves void contract");
}
static void cache_cases(void){
 unsigned a,b,c,d,i;void *blocks[4];PBLKHEADER head;
 reset();for(a=0;a<4;++a)for(b=0;b<4;++b)for(c=0;c<4;++c)for(d=0;d<4;++d){unsigned order[4];if(a==b || a==c || a==d || b==c || b==d || c==d)continue;order[0]=a;order[1]=b;order[2]=c;order[3]=d;++scenarios;
  for(i=0;i<4;++i)blocks[i]=original_CacheBlockAllocate(&gParamMap.blkCache,1000);
  for(i=0;i<4;++i)CacheBlockFree(&gParamMap.blkCache,blocks[order[i]]);
  head=gParamMap.blkCache.pCacheFree;expect(head==(PBLKHEADER)gParamMap.blkCache.pCache && head->dwSize==MAPCACHESIZE && !head->pNext,"original cache all24 release orders coalesce");
 }
 ++scenarios;blocks[0]=original_CacheBlockAllocate(&gParamMap.blkCache,MAPCACHESIZE*2);expect(blocks[0] && !IsCacheBlock(&gParamMap.blkCache,blocks[0]),"original native heap fallback");CacheBlockFree(&gParamMap.blkCache,blocks[0]);
}
static void mutation_cases(void){
 MSGPARAMEX v;unsigned abort;
 reset();AddParamMap((DWORD)text[0],KEY_A);fail_map=TRUE;v=reverse(WM_SETTEXT,KEY_B);++scenarios;expect(reverse_start(&v),"text optional constructor before native filter");v.lParam=(LONG)text[19];v.lpfnUnThunk16(&v);wow_message_unwind16(&v);
 expect(references(KEY_A)==1 && last_flat_release==text[0],"REVERSE_TEXT_MUTATION");
 for(abort=0;abort<2;++abort){
  reset();task.dwWOWCompatFlags=WOWCF_NOCBDIRTHUNK;v=reverse(OLDCB_DIR,KEY_A);++scenarios;expect(reverse_start(&v),"CB_DIR constructor before native filter");v.uMsg=CB_GETLBTEXT;v.lParam=(LONG)text[19];if(!abort)v.lpfnUnThunk16(&v);wow_message_unwind16(&v);
  expect(!node_count() && last_flat_release==text[0] && !strcmp(text[0],"original text"),abort ? "REVERSE_CB_ABORT_MUTATION" : "REVERSE_CB_MUTATION");
 }
 reset();v=reverse(WM_USER+18,KEY_A);v.lpfnUnThunk16=UnThunkCBMsg16;v.uMsg=CB_DIR;v.MsgBuffer->rect.left=WOW_TEXT_ACTIVE;++scenarios;wow_message_unwind16(&v);
 expect(!flat_releases && v.MsgBuffer->rect.left==WOW_TEXT_ACTIVE,"other combo output is never a text lease");
}
int main(void){unsigned i;for(i=0;i<20;++i)strcpy_s(text[i],sizeof(text[i]),"original text");InitParamMap();map_cases();reverse_cases();forward_cases();api_cases();cache_cases();mutation_cases();reset();free_w(gParamMap.blkCache.pCache);
 printf("PARAMETER_MAP failures=%u scenarios=%u reverse=%u buffer=%u forward=%u\n",failures,scenarios,(unsigned)sizeof(MSGPARAMEX),(unsigned)sizeof(MSGTHUNKBUFFER),(unsigned)sizeof(WM32MSGPARAMEX));return failures ? 1 : 0;
}
