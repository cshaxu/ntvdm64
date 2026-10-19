/* Supplementary exact-original-body ownership/fault evidence. Allocation,
 * mapping and window services are doubles; this is not a guest/runtime pass. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#define FASTCALL __stdcall
typedef WORD HAND16,HMENU16,HWND16,HDC16,HTASK16,HMIDIIN16,HMIDIOUT16,HWAVEIN16,HWAVEOUT16;
typedef DWORD VPVOID,VPBYTE,VPSZ;
typedef LPSTR LPSZ;
typedef struct {DWORD dwExStyle;} WW,*PWW;
#include "message_construction_types.h"
typedef struct {DWORD unused;} BLKCACHE,*PBLKCACHE;
#define MAXNODEALIAS 0x10
#define ARRAYCOUNT(a) (sizeof(a)/sizeof((a)[0]))
#define PARAM_16 1
#define PARAM_32 2
#include "message_construction_map_types.h"
typedef struct {PARM16 Parm16;LONG lParam;UINT uParam;} MSGPARAMEX,*LPMSGPARAMEX;
typedef struct {DWORD dwWOWCompatFlags;HAND16 htask16;} PTD_VALUE;
typedef struct {WORD cbData;BYTE data[14];} HLP,*LPHLP;
#define WOWCF_DSBASEDSTRINGPOINTERS 1
#define CURRENTPTD() (&task)
#ifndef WM_WINHELP
#define WM_WINHELP 0x0038
#endif
static PTD_VALUE task={0,0x1234};
static WW words;
static BOOL dead_window;
typedef struct {BYTE bytes[256];unsigned kind;} SLOT;
static SLOT slots[64];
static unsigned fail_at,allocations,bad_map,bad_fill,bad_foreign,failures,scenarios;
static unsigned map_fail,map_live,map_allocations,flat_release,relocated;
static BYTE poison[256];
static char text[32]="original text",moved_text[32]="original text";
#define TEXT_FAR 0x10001000u
static VPVOID far_value(unsigned i){return 0x20000000u+i*0x1000u;}
static void *guest_pointer(DWORD value){
 unsigned i;
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
static VPVOID FASTCALL stackalloc16(UINT bytes){return allocate_guest(bytes,2);}
static BOOL FASTCALL free16(VPVOID value){
 unsigned i;
 for(i=0;i<64;++i)if(slots[i].kind && far_value(i)==value){
  if(slots[i].kind!=1){++bad_foreign;return FALSE;}
  slots[i].kind=0;return TRUE;
 }
 ++bad_foreign;return FALSE;
}
static void fill_memory(void *pointer,SIZE_T bytes,BYTE value){
 if(pointer==poison)++bad_fill;
 if(bytes<=256)memset(pointer,value,bytes);else ++bad_fill;
}
static void putstr16(VPVOID value,const char *source,INT bytes){memcpy(guest_pointer(value),source,(size_t)bytes);}
static void getstr16(VPVOID value,char *target,INT bytes){memcpy(target,guest_pointer(value),(size_t)bytes);}
static LONG fixture_window_words(void){return dead_window ? 0 : (LONG)&words;}
static BOOL fixture_foreground(HWND value){(void)value;return TRUE;}
static void *GetPModeVDMPointer(DWORD value,DWORD ignored){(void)ignored;return guest_pointer(value);}
static void *CacheBlockAllocate(PBLKCACHE cache,DWORD bytes){
 void *value;(void)cache;++map_allocations;if(map_fail)return NULL;
 value=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytes);if(value)++map_live;return value;
}
static void CacheBlockFree(PBLKCACHE cache,void *value){(void)cache;if(value){HeapFree(GetProcessHeap(),0,value);--map_live;}}
static VPVOID GlobalAllocLock16(UINT flags,DWORD bytes,HAND16 *handle){
 VPVOID value=malloc16(bytes);(void)flags;
 if(value && handle)*handle=(HAND16)((value-0x20000000u)/0x1000u+1);
 return value; /* Failure deliberately leaves caller's output untouched. */
}
static void GlobalUnlockFree16(VPVOID value){(void)free16(value);}
#define LOG_ALWAYS 0
#define LOGDEBUG(level,args) ((void)0)
#define WOW32ASSERT(x) ((void)0)
#define WOW32ASSERTMSG(x,s) ((void)0)
#undef GetWindowLong
#define GetWindowLong(w,i) fixture_window_words()
#define GWL_WOWWORDS (-100)
#define GETHWND16(v) ((WORD)(ULONG_PTR)(v))
#define GETHINST16(v) ((WORD)(ULONG_PTR)(v))
#define GETHMENU16(v) ((WORD)(ULONG_PTR)(v))
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
#include "message_construction_exact_body.h"

static void expect(BOOL condition,const char *name){if(!condition){printf("FAIL %s\n",name);++failures;}}
static void reset_case(void){
 /* Explicit fixture reset is not evidence of the owner's successful free. */
 FreeParamMap(task.htask16);memset(slots,0,sizeof(slots));memset(&words,0,sizeof(words));
 allocations=fail_at=bad_map=bad_fill=bad_foreign=0;map_fail=map_allocations=flat_release=relocated=0;
 task.dwWOWCompatFlags=0;dead_window=FALSE;++scenarios;
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
int main(void){
 get_text_cases();mdi_cases();map_cases();adjacent_cases();reset_case();
 printf("MESSAGE_CONSTRUCTION failures=%u scenarios=%u abi=%zu scratch_offset=%zu scratch_slots=%zu parm=%zu\n",
  failures,scenarios,sizeof(WM32MSGPARAMEX),offsetof(WM32MSGPARAMEX,dwTmp),
  sizeof(((WM32MSGPARAMEX *)0)->dwTmp)/sizeof(DWORD),sizeof(PARM16));
 return failures ? 1 : 0;
}
