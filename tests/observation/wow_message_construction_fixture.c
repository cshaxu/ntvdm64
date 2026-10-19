/* Supplementary exact-original-body ownership/fault evidence. Allocation,
 * mapping and window services are doubles; this is not a guest/runtime pass. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#define FASTCALL __stdcall
typedef WORD HAND16,HMEM16,HMENU16,HWND16,HDC16,HTASK16,HMIDIIN16,HMIDIOUT16,HWAVEIN16,HWAVEOUT16,BOOL16;
typedef DWORD VPVOID,VPBYTE,VPSZ,VPPROC,VPINT16,VPRECT16;
#define GlobalLock16 WOWGlobalLockSize16
#define GlobalUnlockFree16 WOWGlobalUnlockFree16
typedef LPSTR LPSZ;
typedef struct {DWORD dwExStyle,dwStyle;} WW,*PWW;
#include "message_construction_types.h"
typedef struct {DWORD unused;} BLKCACHE,*PBLKCACHE;
#define MAXNODEALIAS 0x10
#define ARRAYCOUNT(a) (sizeof(a)/sizeof((a)[0]))
#define PARAM_16 1
#define PARAM_32 2
#include "message_construction_map_types.h"
#include "message_construction_messages.h"
#include "../../src/wow32-dll/include/wow_message_lifetime.h"
BOOL DeleteParamMap(DWORD,UINT,BOOL *);
typedef struct _TD {DWORD dwWOWCompatFlags;HAND16 htask16;DWORD vpStack,vpCBStack,dwFlags;} PTD_VALUE,*PTD;
typedef struct {WORD cbData;BYTE data[14];} HLP,*LPHLP;
#define WOWCF_DSBASEDSTRINGPOINTERS 1
#define CURRENTPTD() current_task
#define TDF_INITCALLBACKSTACK 1u
#define TDF_IGNOREINPUT 2u
#define THUNKMSG TRUE
#define UNTHUNKMSG FALSE
#define OLDCB_DIR (WM_USER+5)
#define SIZE_BOGUS 256
#ifndef WM_WINHELP
#define WM_WINHELP 0x0038
#endif
static PTD_VALUE task={0,0x1234,0,0,0};
static PTD current_task=&task;
static BOOL worker_live=TRUE;
static BYTE callback_stack[4096];
#define STACK_BASE 0x3000f000u
static WW words;
static BOOL dead_window;
typedef struct {BYTE bytes[256];unsigned kind,locks;} SLOT;
static SLOT slots[64];
static unsigned fail_at,allocations,bad_map,bad_fill,bad_foreign,failures,scenarios;
static unsigned map_fail,map_live,map_allocations,flat_release,relocated;
static unsigned fault_at,fault_step,release_fault_at,release_calls;
static unsigned native_locks,global_locks,global_unlocks,foreground_calls;
static BOOL copyout_fault,callback_fault,callback_reject;
static const DWORD fault_code=0xe0447101u;
static BYTE poison[256];
static char text[32]="original text",moved_text[32]="original text";
#define TEXT_FAR 0x10001000u
static VPVOID far_value(unsigned i){return 0x20000000u+i*0x1000u;}
static void *guest_pointer(DWORD value){
 unsigned i;
 if(fault_at && ++fault_step==fault_at)RaiseException(fault_code,0,0,NULL);
 if(value>=STACK_BASE && value<STACK_BASE+sizeof(callback_stack))return callback_stack+value-STACK_BASE;
 if(value==TEXT_FAR)return relocated ? moved_text : text;
 for(i=0;i<64;++i)if(slots[i].kind && value==far_value(i))return slots[i].bytes;
 ++bad_map;return poison;
}
static unsigned owned(void){unsigned i,n=0;for(i=0;i<64;++i)if(slots[i].kind==1)++n;return n;}
static VPVOID allocate_guest(unsigned bytes,unsigned kind){
 unsigned i;
 if(kind==1 && ++allocations==fail_at)return 0;
 if(bytes>256){++bad_map;return 0;}
 for(i=0;i<64;++i)if(!slots[i].kind){slots[i].kind=kind;memset(slots[i].bytes,0,sizeof(slots[i].bytes));return far_value(i);}
 return 0;
}
static VPVOID FASTCALL malloc16(UINT bytes){return allocate_guest(bytes,1);}
static BOOL FASTCALL free16(VPVOID value){
 unsigned i;
 for(i=0;i<64;++i)if(slots[i].kind && far_value(i)==value){
  if(slots[i].kind!=1){++bad_foreign;return FALSE;}
  slots[i].kind=0;
  if(++release_calls==release_fault_at)RaiseException(fault_code,0,0,NULL);
  return TRUE;
 }
 ++bad_foreign;return FALSE;
}
static void fill_memory(void *pointer,SIZE_T bytes,BYTE value){
 if(pointer==poison)++bad_fill;
 if(bytes<=256)memset(pointer,value,bytes);else ++bad_fill;
}
static void putstr16(VPVOID value,const char *source,INT bytes){memcpy(guest_pointer(value),source,(size_t)bytes);}
static void getstr16(VPVOID value,char *target,INT bytes){
 const char *source=guest_pointer(value);
 if(copyout_fault){if(bytes)target[0]=source[0];RaiseException(fault_code,0,0,NULL);}
 memcpy(target,source,bytes<0 ? strlen(source)+1 : (size_t)bytes);
}
static LONG fixture_window_words(void){return dead_window ? 0 : (LONG)&words;}
static BOOL fixture_foreground(HWND value){(void)value;++foreground_calls;return TRUE;}
BOOL WINAPI wow_user_worker_active(void){return worker_live;}
static BOOL W32CheckThunkParamFlag(void){return TRUE;}
static void *GetPModeVDMPointer(DWORD value,DWORD ignored){(void)ignored;return guest_pointer(value);}
static void *CacheBlockAllocate(PBLKCACHE cache,DWORD bytes){
 void *value;(void)cache;++map_allocations;if(map_fail)return NULL;
 value=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytes);if(value)++map_live;return value;
}
static void CacheBlockFree(PBLKCACHE cache,void *value){(void)cache;if(value){HeapFree(GetProcessHeap(),0,value);--map_live;}}
static VPVOID GlobalAllocLock16(UINT flags,DWORD bytes,HAND16 *handle){
 VPVOID value=malloc16(bytes);(void)flags;
 if(value){unsigned index=(value-0x20000000u)/0x1000u;slots[index].locks=1;
  if(handle)*handle=(HAND16)(index+1);}
 return value; /* Failure deliberately leaves caller's output untouched. */
}
static VPVOID WINAPI GlobalLock16(HMEM16 handle,LPDWORD bytes){
 if(!handle || handle>64 || !slots[handle-1].kind)return 0;
 ++slots[handle-1].locks;++global_locks;if(bytes)*bytes=256;return far_value(handle-1);
}
static BOOL GlobalUnlock16(HAND16 handle){
 if(!handle || handle>64 || !slots[handle-1].locks){++bad_foreign;return FALSE;}
 --slots[handle-1].locks;++global_unlocks;return slots[handle-1].locks!=0;
}
static HMEM16 WINAPI GlobalUnlockFree16(VPVOID value){
 unsigned index=(value-0x20000000u)/0x1000u;
 if(index>=64 || !slots[index].kind){++bad_foreign;return 0;}
 if(slots[index].locks)--slots[index].locks;
 if(!slots[index].locks)(void)free16(value);
 return 0;
}
static void *fixture_native_lock(HGLOBAL handle){++native_locks;return (void *)handle;}
static BOOL fixture_native_unlock(HGLOBAL handle){(void)handle;--native_locks;return FALSE;}
#undef GlobalLock
#undef GlobalUnlock
#define GlobalLock fixture_native_lock
#define GlobalUnlock fixture_native_unlock
#define LOG_ALWAYS 0
#define LOGDEBUG(level,args) ((void)0)
#if defined(TRANSACTION_GROUP) && !defined(BASELINE_IMAGE)
#define WOWCLASS_EDIT 4 /* Original walias.h, not a native private class. */
static BOOL notification_fault;
static LRESULT CALLBACK notification_proc(HWND w,UINT m,WPARAM p,LPARAM l){
 (void)w;(void)m;(void)p;(void)l;
 current_task->vpCBStack-=4;
 if(notification_fault)RaiseException(fault_code,0,0,NULL);
 return 42;
}
#define GetStdClassWndProc(c) ((WNDPROC)notification_proc)
#undef CallWindowProc
#define CallWindowProc(fn,w,m,p,l) (fn)(w,m,p,l)
#endif
#define WOW32ASSERT(x) ((void)0)
#define WOW32ASSERTMSG(x,s) ((void)0)
#undef GetWindowLong
#define GetWindowLong(w,i) fixture_window_words()
#define GWL_WOWWORDS (-100)
#define GETHWND16(v) ((WORD)(ULONG_PTR)(v))
#define GETHINST16(v) ((WORD)(ULONG_PTR)(v))
#define GETHMENU16(v) ((WORD)(ULONG_PTR)(v))
#define GETHWNDIA16(v) ((WORD)(ULONG_PTR)(v))
#define HWNDIA32(v) ((HWND)(LONG_PTR)(SHORT)(v))
#define HCURSOR32(v) ((HCURSOR)(ULONG_PTR)(WORD)(v))
#define HWND32(v) ((HWND)(LONG_PTR)(SHORT)(v))
#define HIW(v) (*(WORD *)((BYTE *)&(v)+2))
#define HIW16(v) HIWORD(v)
#define LOW(v) (*(WORD *)&(v))
#define GETVDMPTR(v,n,p) ((p)=guest_pointer((DWORD)(v)))
#define FREEVDMPTR(p) ((void)(p))
#define FLUSHVDMPTR(v,n,p) ((void)(p))
#define GETMISCPTR(v,p) ((p)=guest_pointer((DWORD)(v)))
#define FREEMISCPTR(p) ((void)(p))
#define GETPSZPTR(v,p) ((p)=(v) ? guest_pointer((DWORD)(v)) : NULL)
#define FREEPSZPTR(p) ((void)(p),++flat_release)
#define STOREDWORD(d,v) (*(DWORD *)&(d)=(DWORD)(v))
#define STORELONG(d,v) (*(LONG *)&(d)=(LONG)(v))
#define STOREWORD(d,v) (*(WORD *)&(d)=(WORD)(v))
#define FETCHWORD(v) ((WORD)(v))
#define FETCHDWORD(v) ((DWORD)(v))
#define stackfree16(v) ((void)(v))
#undef RtlFillMemory
#define RtlFillMemory fill_memory
#undef RegisterWindowMessage
#define RegisterWindowMessage(s) ((UINT)0xc001)
#undef SetForegroundWindow
#define SetForegroundWindow fixture_foreground
PVOID ParamMapUpdateNode(DWORD,UINT,VOID *);
BOOL FASTCALL WM32MDICreate(LPWM32MSGPARAMEX);
static void putmsg16(VPVOID value,const MSG *message){memcpy(guest_pointer(value),message,18);}
static void putrect16(VPVOID value,const RECT *rectangle){
 PRECT16 target=guest_pointer(value);target->left=(SHORT)rectangle->left;target->top=(SHORT)rectangle->top;
 target->right=(SHORT)rectangle->right;target->bottom=(SHORT)rectangle->bottom;
}
#define PUTRECT16 putrect16
static void putpaintstruct16(VPVOID value,const PAINTSTRUCT *paint){memcpy(guest_pointer(value),paint,sizeof(*paint));}
#if defined(TRANSACTION_GROUP) || !defined(BASELINE_IMAGE)
static BOOL FASTCALL WM32CopyData(LPWM32MSGPARAMEX value){(void)value;++failures;return FALSE;}
static VOID FASTCALL UnThunkCBMsg16(LPMSGPARAMEX value){(void)value;++failures;}
#endif
#ifdef TRANSACTION_GROUP
typedef struct {DWORD Proc16;} HOOKSTATEDATA,*LPHOOKSTATEDATA;
typedef DWORD *PVPVOID;
#define RET_HOOKPROC 1
#define WOWCLASS_UNKNOWN 0
static PWW FindPWW(HWND window,int cls){(void)window;(void)cls;return dead_window ? NULL : &words;}
static LRESULT fixture_send_message(HWND window,UINT message,WPARAM wp,LPARAM lp){(void)window;(void)message;(void)wp;(void)lp;return 3;}
#undef SendMessage
#define SendMessage fixture_send_message
static void getrect16(VPVOID value,RECT *rectangle){
 PRECT16 source=guest_pointer(value);rectangle->left=source->left;rectangle->top=source->top;
 rectangle->right=source->right;rectangle->bottom=source->bottom;
}
#define GETRECT16 getrect16
#define ALLOCVDMPTR GETVDMPTR
static void putintarray16(VPVOID value,INT count,const INT *array){
 SHORT *target=guest_pointer(value);INT i;for(i=0;i<count;++i)target[i]=(SHORT)array[i];
}
static void getintarray16(VPVOID value,INT count,INT *array){
 SHORT *source=guest_pointer(value);INT i;for(i=0;i<count;++i)array[i]=source[i];
}
#define PUTCBTACTIVATESTRUCT16(p,v) ((p)->fMouse=(WORD)(v)->fMouse,(p)->hWndActive=GETHWND16((v)->hWndActive))
#define GETCBTACTIVATESTRUCT16(p,v) ((v)->fMouse=(p)->fMouse,(v)->hWndActive=HWND32((p)->hWndActive))
#define PUTMOUSEHOOKSTRUCT16(p,v) ((p)->pt.x=(SHORT)(v)->pt.x,(p)->pt.y=(SHORT)(v)->pt.y,(p)->hwnd=GETHWND16((v)->hwnd))
#define GETMOUSEHOOKSTRUCT16(p,v) ((v)->pt.x=(p)->pt.x,(v)->pt.y=(p)->pt.y,(v)->hwnd=HWND32((p)->hwnd))
static BOOL CallBack16(INT id,PARM16 *parameters,VPPROC procedure,PVPVOID result){
 (void)parameters;(void)procedure;
 if(id!=RET_HOOKPROC)++failures;
 if(callback_fault)RaiseException(fault_code,0,0,NULL);
 *result=0;return !callback_reject;
}
#endif
#include "message_construction_exact_body.h"

static void expect(BOOL condition,const char *name){if(!condition){printf("FAIL %s\n",name);++failures;}}
static void reset_case(void){
 /* Explicit fixture reset is not evidence of the owner's successful free. */
 FreeParamMap(task.htask16);memset(slots,0,sizeof(slots));memset(&words,0,sizeof(words));
 allocations=fail_at=bad_map=bad_fill=bad_foreign=0;map_fail=map_allocations=flat_release=relocated=0;
 task.dwWOWCompatFlags=0;dead_window=FALSE;++scenarios;
 task.vpStack=STACK_BASE+0xf00;task.vpCBStack=STACK_BASE+0xd00;task.dwFlags=TDF_INITCALLBACKSTACK;
 current_task=&task;worker_live=TRUE;fault_at=fault_step=release_fault_at=release_calls=0;
 native_locks=global_locks=global_unlocks=foreground_calls=0;copyout_fault=callback_fault=callback_reject=FALSE;
}
static WM32MSGPARAMEX make_message(void *payload,UINT message,UINT count){
 WM32MSGPARAMEX value;memset(&value,0,sizeof(value));value.fThunk=TRUE;
 value.lParam=(LONG)payload;value.uMsg=message;value.uParam=count;
 value.Parm16.WndProc.lParam=(LONG)payload;value.Parm16.WndProc.wParam=(WORD)count;
 value.pww=&words;return value;
}
static void get_text_cases(void){
 unsigned compat,answer;char output[16];WM32MSGPARAMEX value;BOOL ok;
 reset_case();fail_at=1;value=make_message(output,WM_GETTEXT,8);
 ok=WM32GetText(&value);
 expect(!ok && !bad_map && !bad_fill && !owned(),"GETTEXT_CONSTRUCTOR");
 for(compat=0;compat<2;++compat)for(answer=0;answer<4;++answer){
  reset_case();task.dwWOWCompatFlags=compat ? WOWCF_DSBASEDSTRINGPOINTERS : 0;
  memset(output,0,sizeof(output));value=make_message(output,WM_GETTEXT,8);
  expect(WM32GetText(&value),"GETTEXT_NORMAL_CONSTRUCT");
  memcpy(guest_pointer(value.Parm16.WndProc.lParam),"abc",4);
  value.fThunk=FALSE;value.lReturn=answer;
  expect(WM32GetText(&value) && value.lReturn==(answer==2 ? 2 : 3) && !strcmp(output,"abc") &&
   !owned() && !bad_map && !bad_fill,"GETTEXT_ORIGINAL_QUIRKS");
 }
 reset_case();memset(output,0x55,sizeof(output));value=make_message(output,WM_GETTEXT,0);
 expect(WM32GetText(&value),"GETTEXT_ZERO_CONSTRUCT");value.fThunk=FALSE;value.lReturn=0;
 expect(WM32GetText(&value) && output[0]==0x55 && !owned(),"GETTEXT_ZERO_UNTOUCHED");
 reset_case();memset(output,0,sizeof(output));value=make_message(output,WM_GETTEXT,0);
 expect(WM32GetText(&value),"GETTEXT_ZERO_LEGACY_CONSTRUCT");
 memcpy(guest_pointer(value.Parm16.WndProc.lParam),"abc",4);value.fThunk=FALSE;value.lReturn=3;
 expect(WM32GetText(&value) && !strcmp(output,"abc") && !owned(),"GETTEXT_ZERO_LEGACY_COPY");
}
static MDICREATESTRUCT mdi_input(void){
 MDICREATESTRUCT value;memset(&value,0,sizeof(value));value.szClass="inner";value.szTitle="inner title";return value;
}
static void mdi_cases(void){
 unsigned stage,mode;MDICREATESTRUCT mdi;CREATESTRUCT create;
 WM32MSGPARAMEX value;VPVOID foreign;BOOL ok;
 for(stage=1;stage<=3;++stage){
  reset_case();mdi=mdi_input();fail_at=stage;value=make_message(&mdi,WM_MDICREATE,0);
  expect(!WM32MDICreate(&value) && !owned() && !bad_foreign,"MDI_PARTIAL_ROLLBACK");
 }
 reset_case();mdi=mdi_input();value=make_message(&mdi,WM_MDICREATE,0);
 expect(WM32MDICreate(&value) && owned()==3,"MDI_NORMAL_CONSTRUCT");
 foreign=allocate_guest(32,3);
 ((PMDICREATESTRUCT16)guest_pointer(value.Parm16.WndProc.lParam))->vpszTitle=foreign;
 value.fThunk=FALSE;expect(WM32MDICreate(&value) && !owned() && !bad_foreign,"MDI_TITLE_OWNERSHIP");
 for(stage=1;stage<=5;++stage){
  reset_case();mdi=mdi_input();memset(&create,0,sizeof(create));
  create.lpszClass="outer";create.lpszName="outer title";create.lpCreateParams=&mdi;
  words.dwExStyle=WS_EX_MDICHILD;fail_at=stage;value=make_message(&create,WM_CREATE,0);
  ok=WM32Create(&value);expect(!ok,"NESTED_CREATE_REJECTION");
  if(ok){value.fThunk=FALSE;(void)WM32Create(&value);}
  expect(!owned() && !bad_foreign,"NESTED_CREATE_ROLLBACK");
 }
 for(mode=0;mode<4;++mode){
  PCREATESTRUCT16 guest_create;PMDICREATESTRUCT16 guest_mdi;
  reset_case();mdi=mdi_input();memset(&create,0,sizeof(create));
  create.lpszClass="outer";create.lpszName="outer title";create.lpCreateParams=&mdi;
  words.dwExStyle=WS_EX_MDICHILD;value=make_message(&create,WM_CREATE,0);
  expect(WM32Create(&value) && owned()==5,"NESTED_NORMAL_CONSTRUCT");
  guest_create=guest_pointer(value.Parm16.WndProc.lParam);guest_mdi=guest_pointer(guest_create->vpCreateParams);
  if(mode==1)guest_mdi->vpszTitle=allocate_guest(32,3);
  if(mode==2){words.dwExStyle=0;value.pww=NULL;dead_window=TRUE;}
  if(mode==3){foreign=allocate_guest(sizeof(MDICREATESTRUCT16),3);guest_create->vpCreateParams=foreign;}
  value.fThunk=FALSE;expect(WM32Create(&value) && !owned() && !bad_foreign,"NESTED_CAPTURED_OWNERSHIP");
 }
 reset_case();memset(&create,0,sizeof(create));create.lpszClass="outer";create.lpszName="title";
 value=make_message(&create,WM_CREATE,0);value.pww=NULL;dead_window=TRUE;
 expect(!WM32Create(&value) && !owned(),"CREATE_DEAD_WINDOW_ROLLBACK");
}
static void map_cases(void){
 MSGPARAMEX value;WM32MSGPARAMEX reverse;DWORD pointer;BOOL free_pointer;
 reset_case();memset(&value,0,sizeof(value));value.Parm16.WndProc.wMsg=WM_SETTEXT;
 value.Parm16.WndProc.lParam=TEXT_FAR;map_fail=1;
 expect(ThunkWMMsg16(&value) && value.lParam==(LONG)text && !map_live,"MAP_FAILURE_TEXT");
 reverse=make_message((void *)value.lParam,WM_SETTEXT,0);
 expect(WM32SetText(&reverse) &&
  !strcmp(guest_pointer(reverse.Parm16.WndProc.lParam),text),"MAP_FALLBACK_RECEIVE");
 reverse.fThunk=FALSE;expect(WM32SetText(&reverse) && !owned(),"MAP_FALLBACK_OWNED_RELEASE");
 UnThunkWMMsg16(&value);expect(flat_release==1 && !map_live,"MAP_FALLBACK_RELEASE");
 reset_case();memset(&value,0,sizeof(value));value.Parm16.WndProc.wMsg=WM_SETTEXT;
 expect(ThunkWMMsg16(&value) && !value.lParam && !gParamMap.pHead && !map_allocations,"MAP_NULL_ENTRY");
 UnThunkWMMsg16(&value);
 reset_case();memset(&value,0,sizeof(value));value.Parm16.WndProc.wMsg=WM_SETTEXT;value.Parm16.WndProc.lParam=TEXT_FAR;
 expect(ThunkWMMsg16(&value) && map_live==1,"MAP_NORMAL_CONSTRUCT");
 reverse=make_message((void *)value.lParam,WM_SETTEXT,0);
 expect(WM32SetText(&reverse) && reverse.Parm16.WndProc.lParam==TEXT_FAR,"MAP_BORROW_IDENTITY");
 reverse.fThunk=FALSE;expect(WM32SetText(&reverse) && map_live==1 && !owned(),"MAP_BORROW_RETURN");
 UnThunkWMMsg16(&value);expect(!map_live && !gParamMap.pHead,"MAP_NORMAL_RELEASE");
 reset_case();pointer=(DWORD)AddParamMap((DWORD)text,TEXT_FAR);relocated=1;
 expect(ParamMapUpdateNode(pointer,PARAM_32,NULL)==moved_text && GetParam16(pointer)==TEXT_FAR,"MAP_ORIGINAL_MOVEMENT");
 expect(DeleteParamMap(TEXT_FAR,PARAM_16,NULL),"MAP_ALIAS_RELEASE");
 expect(DeleteParamMap((DWORD)moved_text,PARAM_32,&free_pointer) && !map_live,"MAP_MOVED_RELEASE");
}
static void adjacent_cases(void){
 HLP help={16,{0}};WM32MSGPARAMEX value;MSG msg={0};unsigned stage;char name[8]="format";
 for(stage=0;stage<2;++stage){
  reset_case();fail_at=stage;value=make_message(&msg,WM_GETDLGCODE,0);
  if(stage)expect(!WM32GetDlgCode(&value) && !owned(),"DLGCODE_FAILURE");
  else {expect(WM32GetDlgCode(&value),"DLGCODE_CONSTRUCT");value.fThunk=FALSE;expect(WM32GetDlgCode(&value) && !owned(),"DLGCODE_RELEASE");}
 }
 for(stage=0;stage<2;++stage){
  reset_case();fail_at=stage;value=make_message(name,WM_ASKCBFORMATNAME,8);
  if(stage)expect(!WM32AskCBFormatName(&value) && !owned(),"ASKFORMAT_FAILURE");
  else {expect(WM32AskCBFormatName(&value),"ASKFORMAT_CONSTRUCT");value.fThunk=FALSE;expect(WM32AskCBFormatName(&value) && !owned(),"ASKFORMAT_RELEASE");}
 }
 reset_case();value=make_message(&help,WM_WINHELP,0);fail_at=1;
 expect(WM32WinHelp(&value) && !value.Parm16.WndProc.lParam && !value.dwParam && !owned(),"WINHELP_FAILURE_HANDLE");
 reset_case();value=make_message(&help,WM_WINHELP,0);
 expect(WM32WinHelp(&value) && owned()==1,"WINHELP_CONSTRUCT");value.fThunk=FALSE;
 expect(WM32WinHelp(&value) && !owned(),"WINHELP_RELEASE");
}
#ifdef TRANSACTION_GROUP
static int fault_filter(DWORD code){return code==fault_code ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;}
static void heap_transactions(void){
 static const struct {LPFNM32 function;UINT message,count;BOOL line;} cases[]={
  {WM32GetText,WM_GETTEXT,8,FALSE},{WM32GetDlgCode,WM_GETDLGCODE,0,FALSE},
  {WM32AskCBFormatName,WM_ASKCBFORMATNAME,8,FALSE},{WM32DropObject,WM_QUERYDROPOBJECT,0,FALSE},
  {WM32EMGetRect,EM_GETRECT,0,FALSE},{WM32EMSetRect,EM_SETRECT,0,FALSE},
  {WM32EMReplaceSel,EM_REPLACESEL,0,FALSE},{WM32EMGetLine,EM_GETLINE,0,TRUE},
  {WM32EMSetTabStops,EM_SETTABSTOPS,2,FALSE},{WM32CBAddString,CB_ADDSTRING,0,FALSE},
  {WM32CBGetLBText,CB_GETLBTEXT,0,FALSE},{WM32CBGetDropDownControlRect,CB_GETDROPPEDCONTROLRECT,0,FALSE},
  {WM32LBGetText,LB_GETTEXT,0,FALSE},{WM32LBDir,LB_DIR,0,FALSE},
  {WM32LBGetSelItems,LB_GETSELITEMS,2,FALSE},{WM32LBSetTabStops,LB_SETTABSTOPS,2,FALSE},
  {WM32LBGetItemRect,LB_GETITEMRECT,0,FALSE},{WM32LBAddString,LB_ADDSTRING,0,FALSE},
  {WM32msgSHAREVIOLATION,0xc001,0,FALSE}};
 unsigned i,mode,family_before=0;BYTE payload[256];WM32MSGPARAMEX value;BOOL constructed,trapped;
 for(i=0;i<ARRAYCOUNT(cases);++i)for(mode=0;mode<5;++mode){
  if(!mode)family_before=failures;
  reset_case();memset(payload,0,sizeof(payload));memcpy(payload,"abc",4);
  if(cases[i].line)*(WORD *)payload=8;
  value=make_message(payload,cases[i].message,cases[i].count);value.lpfnM32=cases[i].function;
  if(mode==4){
   fault_at=1;trapped=FALSE;
   __try{__try{(void)cases[i].function(&value);}__finally{wow_message_unwind32(&value);}}
   __except(fault_filter(GetExceptionCode())){trapped=TRUE;}
   expect(!owned() && !bad_foreign,"TRANSACTION_HEAP_PRECALL_COPY_ABORT");
   printf("TRANSACTION_FAMILY index=%u message=%04X modes=5 errors=%u copy_fault=%u\n",i,cases[i].message,failures-family_before,trapped);
   continue;
  }
  if(mode==0)fail_at=1;
  constructed=cases[i].function(&value);
  if(mode==0){expect(!constructed && !owned(),"TRANSACTION_OOM");continue;}
  expect(constructed && owned()==1,"TRANSACTION_HEAP_CONSTRUCT");
  if(mode==1){
   SetLastError(0x4471);wow_message_unwind32(&value);wow_message_unwind32(&value);
   expect(!owned() && !bad_foreign && GetLastError()==0x4471,"TRANSACTION_HEAP_ABORT");
  }else{
   value.fThunk=FALSE;value.lReturn=3;
   if(mode==3){copyout_fault=TRUE;fault_at=1;fault_step=0;}
   trapped=FALSE;
   __try{__try{cases[i].function(&value);}__finally{wow_message_unwind32(&value);}}
   __except(fault_filter(GetExceptionCode())){trapped=TRUE;}
   if(mode==2)expect(!trapped,"TRANSACTION_NORMAL_UNTHUNK");
   expect(!owned() && !bad_foreign,"TRANSACTION_COPYBACK_ABORT");
  }
 }
 /* Distinguish an output copy that has already written from a map failure.
  * Abort cleanup may free its key, but must never replay or undo the copy. */
 reset_case();memset(payload,0x55,sizeof(payload));
 value=make_message(payload,WM_GETTEXT,8);value.lpfnM32=WM32GetText;
 expect(WM32GetText(&value),"TRANSACTION_PARTIAL_COPY_CONSTRUCT");
 memcpy(guest_pointer(value.Parm16.WndProc.lParam),"abc",4);
 value.fThunk=FALSE;value.lReturn=3;copyout_fault=TRUE;trapped=FALSE;
 __try{__try{WM32GetText(&value);}__finally{wow_message_unwind32(&value);}}
 __except(fault_filter(GetExceptionCode())){trapped=TRUE;}
 expect(trapped && payload[0]=='a' && payload[1]==0x55 && !owned() && !bad_foreign,
  "TRANSACTION_PARTIAL_COPY_RELEASE_ONLY");
}
static void create_transactions(void){
 unsigned nested,stage;CREATESTRUCT create;MDICREATESTRUCT mdi;WM32MSGPARAMEX value;BOOL trapped;
 for(nested=0;nested<2;++nested)for(stage=1;stage<=(nested ? 6u : 3u);++stage){
  reset_case();mdi=mdi_input();memset(&create,0,sizeof(create));create.lpszClass="outer";create.lpszName="title";
  if(nested){words.dwExStyle=WS_EX_MDICHILD;create.lpCreateParams=&mdi;}
  value=make_message(&create,WM_CREATE,0);value.lpfnM32=WM32Create;fault_at=stage;trapped=FALSE;
  __try{__try{(void)WM32Create(&value);}__finally{wow_message_unwind32(&value);}}
  __except(fault_filter(GetExceptionCode())){trapped=TRUE;}
  expect(trapped && !owned() && !bad_foreign,nested ? "TRANSACTION_NESTED_COPY_ABORT" : "TRANSACTION_CREATE_COPY_ABORT");
 }
 /* A completed free which escapes must not be repeated; remaining owners drain. */
 reset_case();mdi=mdi_input();value=make_message(&mdi,WM_MDICREATE,0);value.lpfnM32=WM32MDICreate;
 expect(WM32MDICreate(&value),"TRANSACTION_MDI_CONSTRUCT");release_fault_at=1;value.fThunk=FALSE;trapped=FALSE;
 __try{__try{WM32MDICreate(&value);}__finally{wow_message_unwind32(&value);}}
 __except(fault_filter(GetExceptionCode())){trapped=TRUE;}
 expect(trapped && !owned() && !bad_foreign,"TRANSACTION_RELEASE_ESCAPE");
}
static void global_transactions(void){
 unsigned family,mode;BYTE payload[sizeof(PAINTSTRUCT)]={0};WM32MSGPARAMEX value;BOOL trapped,help;
 *(WORD *)payload=16;
 for(family=0;family<3;++family)for(mode=0;mode<3;++mode){
  help=family==2;
  reset_case();value=make_message(payload,help ? WM_WINHELP : (family==1 ? WM_PAINTCLIPBOARD : WM_SIZECLIPBOARD),0);
  value.lpfnM32=help ? WM32WinHelp : WM32SizeClipBoard;trapped=FALSE;
  if(mode==0)fault_at=1;
  __try{__try{
   expect(value.lpfnM32(&value),"TRANSACTION_GLOBAL_CONSTRUCT");
   if(mode==2){value.fThunk=FALSE;value.lpfnM32(&value);}
  }__finally{wow_message_unwind32(&value);}}
  __except(fault_filter(GetExceptionCode())){trapped=TRUE;}
  expect((mode!=0 || trapped) && !owned() && !bad_foreign && !native_locks,"TRANSACTION_GLOBAL_LOCKED_ABORT");
  if(mode==0)expect(!global_locks && !foreground_calls,"TRANSACTION_NO_EXTRA_LOCK_OR_ACTIVATION");
  if(mode==1 && !help)expect(global_locks==1 && global_unlocks==1,"TRANSACTION_UNLOCKED_HANDLE_RELEASE");
  if(help)expect(foreground_calls==(mode==2 ? 1u : 0u),"TRANSACTION_WINHELP_NORMAL_ACTIVATION_ONLY");
 }
}
static void cbt_transactions(void){
 unsigned mode;CREATESTRUCT create;CBT_CREATEWND cbt;HOOKSTATEDATA hook={1};BOOL trapped;DWORD cursor;
 for(mode=0;mode<3;++mode){
  reset_case();memset(&create,0,sizeof(create));create.lpszClass="outer";create.lpszName="title";
  cbt.lpcs=&create;cbt.hwndInsertAfter=NULL;cursor=task.vpCBStack;trapped=FALSE;
  callback_fault=mode==0;callback_reject=mode==1;
  __try{(void)ThunkCbtHook(HCBT_CREATEWND,1,(LONG)&cbt,&hook);}
  __except(fault_filter(GetExceptionCode())){trapped=TRUE;}
  expect((mode!=0 || trapped) && !owned() && !bad_foreign,"TRANSACTION_CBT_ABORT");
  if(mode<2)expect(task.vpCBStack==cursor && (task.dwFlags & TDF_INITCALLBACKSTACK),"TRANSACTION_CBT_FAILED_STACK");
 }
}
#ifndef BASELINE_IMAGE
static void stack_transactions(void){
 wow_message_stack scope;DWORD old;PTD_VALUE other;unsigned mode;
 for(mode=0;mode<7;++mode){
  reset_case();old=task.vpCBStack;wow_message_stack_enter(&scope);(void)stackalloc16(8);
  if(mode==0)scope.completed=TRUE;
  if(mode==1)task.dwFlags|=0x80;
  if(mode==2){task.dwFlags|=TDF_IGNOREINPUT;task.dwFlags&=~TDF_INITCALLBACKSTACK;}
  if(mode==3){worker_live=FALSE;current_task=(PTD)(ULONG_PTR)1;}
  if(mode==4){other=task;current_task=&other;}
  if(mode==5)++task.htask16;
  if(mode==6)scope.completed=TRUE;
  SetLastError(0x4499);wow_message_stack_leave(&scope,mode==6);
  expect(GetLastError()==0x4499,"TRANSACTION_STACK_ERROR_PRESERVED");
  if(mode==1 || mode==2 || mode==6)expect(task.vpCBStack==old,"TRANSACTION_STACK_FAILURE_RESTORE");
  else expect(task.vpCBStack!=old,"TRANSACTION_STACK_NORMAL_OR_FOREIGN_UNCHANGED");
  if(mode==2)expect(!(task.dwFlags & TDF_INITCALLBACKSTACK) && (task.dwFlags & TDF_IGNOREINPUT),"TRANSACTION_SIGNAL_FLAGS_RETAINED");
  if(mode==1)expect(task.dwFlags & 0x80,"TRANSACTION_OTHER_FLAGS_RETAINED");
 }
 reset_case();old=task.vpCBStack;notification_fault=FALSE;
 expect(fixture_notification_current(NULL,EM_SCROLLCARET,0,0)==0 && task.vpCBStack==old-4,
  "TRANSACTION_NORMAL_NATIVE_BYPASS");
 reset_case();old=task.vpCBStack;
 expect(fixture_notification_omitted(NULL,EM_SCROLLCARET,0,0)==0 && task.vpCBStack==old,
  "TRANSACTION_NATIVE_BYPASS_OMISSION_CONTROL");
 reset_case();old=task.vpCBStack;notification_fault=TRUE;
 __try{fixture_notification_current(NULL,EM_SCROLLCARET,0,0);}
 __except(fault_filter(GetExceptionCode())){}
 expect(task.vpCBStack==old,"TRANSACTION_NATIVE_BYPASS_FAILURE");notification_fault=FALSE;
 reset_case();{wow_message_stack inner;old=task.vpCBStack;wow_message_stack_enter(&scope);(void)stackalloc16(8);
  wow_message_stack_enter(&inner);(void)stackalloc16(8);wow_message_stack_leave(&inner,FALSE);
  expect(task.vpCBStack==inner.cursor,"TRANSACTION_NESTED_STACK_INNER");wow_message_stack_leave(&scope,FALSE);
  expect(task.vpCBStack==old,"TRANSACTION_NESTED_STACK_OUTER");}
 reset_case();{WM32MSGPARAMEX value=make_message("text",WM_GETTEXT,8);value.lpfnM32=WM32GetText;
  expect(WM32GetText(&value),"TRANSACTION_TERMINAL_CONSTRUCT");worker_live=FALSE;current_task=(PTD)(ULONG_PTR)1;
  wow_message_unwind32(&value);expect(!value.dwTmp[1] && !release_calls && owned()==1,"TRANSACTION_TERMINAL_ABANDON_NOT_FALSE_FREE");}
 reset_case();
}
#endif
static void transaction_group(void){
 unsigned before=failures;
 heap_transactions();create_transactions();global_transactions();cbt_transactions();
 /* All fourteen delegated heap families share the same original call scratch. */
 expect(failures==before,"TRANSACTION_CONTROL_ABORT");
#ifndef BASELINE_IMAGE
 stack_transactions();
#endif
 printf("TRANSACTION_GROUP errors=%u heap_families=19 nested=1 global_locks=1 stack=1 cbt=1 guest_proof=0\n",failures-before);
}
#endif
int main(void){
 get_text_cases();mdi_cases();map_cases();adjacent_cases();reset_case();
#ifdef TRANSACTION_GROUP
 transaction_group();
#endif
 printf("MESSAGE_CONSTRUCTION failures=%u scenarios=%u abi=%zu scratch_offset=%zu scratch_slots=%zu parm=%zu\n",
  failures,scenarios,sizeof(WM32MSGPARAMEX),offsetof(WM32MSGPARAMEX,dwTmp),
  sizeof(((WM32MSGPARAMEX *)0)->dwTmp)/sizeof(DWORD),sizeof(PARM16));
 return failures ? 1 : 0;
}
