/* Exact selected original converters, with only lower allocation/memory
 * services doubled. Supplementary fault evidence, never runtime acceptance. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#define FASTCALL __stdcall
typedef WORD HAND16,HWND16,HMENU16,HDC16,HTASK16,HMIDIIN16,HMIDIOUT16,HWAVEIN16,HWAVEOUT16;
typedef DWORD VPVOID,VPINT16,VPSZ,VPPROC,VPRECT16;
typedef VPVOID *PVPVOID;
typedef SHORT INT16;
typedef WORD *PWORD16;
typedef char *PSZ,*LPSZ;
typedef struct {DWORD dwStyle;HINSTANCE hInstance;} WW,*PWW;
#include "reverse_types.h"
typedef EDITWORDBREAKPROCA WBP;
static VPVOID WordBreakProc16;
static WBP W32WordBreakProc;
static BYTE guest[2048];
static BYTE heap[2048];
static BOOL fail_allocate;
static unsigned allocations,releases,bad_copy,live,failures,scenarios,native_calls;
static void *memory(DWORD address){return address==0x12340000 ? guest : NULL;}
static void *allocate(SIZE_T bytes){++allocations;if(fail_allocate || bytes>sizeof(heap))return NULL;++live;return heap;}
static void release(void *value){if(value){if(value!=heap || !live){++failures;return;}--live;++releases;}}
static void copy(void *target,const void *source,SIZE_T bytes){if(!target || !source){++bad_copy;return;}memcpy(target,source,bytes);}
static void *malloc_w(SIZE_T bytes){return allocate(bytes);}
static void free_w(void *value){release(value);}
static HLOCAL WINAPI local_allocate(UINT flags,SIZE_T bytes){(void)flags;return (HLOCAL)allocate(bytes);}
static HLOCAL WINAPI local_free(HLOCAL value){release(value);return NULL;}
#undef LocalAlloc
#undef LocalFree
#define LocalAlloc local_allocate
#define LocalFree local_free
#undef RtlCopyMemory
#define RtlCopyMemory copy
#define LOGDEBUG(l,a) ((void)0)
#define LOG_ALWAYS 0
#define WOW32ASSERT(a) ((void)0)
#define INT32(v) ((INT)(SHORT)(v))
#define INT32DEFAULT(v) ((INT)(SHORT)(v))
#define FETCHDWORD(v) ((DWORD)(v))
#define FETCHSHORT(v) ((SHORT)(v))
#define STOREWORD(d,v) ((d)=(WORD)(v))
#define RETURN(v) return v
#define NOTHING
#define GETVDMPTR(v,n,p) ((p)=memory((DWORD)(v)))
#define FREEVDMPTR(p) ((void)(p))
#define GETPSZPTR(v,p) ((p)=memory((DWORD)(v)))
#define GETMISCPTR(v,p) GETPSZPTR(v,p)
#define FREEPSZPTR(p) ((void)(p))
#define FLUSHVDMPTR(v,n,p) ((void)(p))
#define PUTINTARRAY16V(v,c,p) putintarray16(v,c,p)
#define GETHMEM16(v) ((WORD)(v))
#define POSTMSG(v) ((v)=0,FALSE)
#define WNDPROC_WOW 0x80000000u
#define WOWCLASS_VIRTUAL_NOT_BIT31 0x4u
#define RET_SUBCLASSPROC 0
#define FUN_WOWWORDBREAKPROC 0
#define VPFN32(v) (v)
#define CallBack16(r,p,f,o) ((void)(r),(void)(p),(void)(f),(void)(o),FALSE)
#define LBCB_CARETOFF (LB_FINDSTRINGEXACT+2)
#ifndef WM_WINHELP
#define WM_WINHELP 0x0038
#endif
static void getrect16(DWORD vp,LPRECT output){(void)vp;(void)output;}
static void putrect16(DWORD vp,LPRECT output){(void)vp;(void)output;}
typedef struct {BOOL fUseTabStops;INT *iTabPixelPositions;INT cxChar;} LBIV,*PLBIV;
static BOOL fail_free;
static BOOL user_free(void *value){unsigned before=failures;if(fail_free)return FALSE;release(value);return before==failures;}
#define UserLocalAlloc(flags,bytes) allocate(bytes)
#define UserLocalFree user_free
#define RIPERR0(a,b,c) ((void)0)
#include "reverse_exact.h"
static void check(BOOL value,const char *name){if(!value){++failures;printf("FAIL %s\n",name);}}
static void reset(void){allocations=releases=bad_copy=live=native_calls=0;memset(heap,0,sizeof(heap));}
static BOOL construct(LPMSGPARAMEX value,UINT message,UINT count,BOOL fail){
 BOOL result;
 ZeroMemory(value,sizeof(*value));value->Parm16.WndProc.wParam=(WORD)count;
 value->Parm16.WndProc.lParam=0x12340000;value->Parm16.WndProc.wMsg=(WORD)message;
 value->lParam=0x12340000;fail_allocate=fail;
 if(message==EM_REPLACESEL || message==EM_SETTABSTOPS){value->Parm16.WndProc.wMsg=(WORD)(message-EM_GETSEL+WM_USER);result=ThunkEMMsg16(value);}
 else if(message==LB_GETSELITEMS || message==LB_SETTABSTOPS){value->Parm16.WndProc.wMsg=(WORD)(message-LB_ADDSTRING+WM_USER+1);result=ThunkLBMsg16(value);}
 else result=ThunkHelp(value);
 if(result)++native_calls;
 return result;
}
static void finish(LPMSGPARAMEX value,UINT message){
 if(message==EM_REPLACESEL || message==EM_SETTABSTOPS)UnThunkEMMsg16(value);
 else if(message==LB_GETSELITEMS || message==LB_SETTABSTOPS)UnThunkLBMsg16(value);
 else UnThunkHelp(value);
}
int main(void){
 static const UINT messages[]={EM_REPLACESEL,EM_SETTABSTOPS,LB_GETSELITEMS,LB_SETTABSTOPS,WM_WINHELP};
 static const char *names[]={"EDIT_REPLACE_ALLOC","EDIT_TABS_ALLOC","LIST_SELECTION_ALLOC","LIST_TABS_ALLOC","HELP_ALLOC"};
 MSGPARAMEX value;unsigned i;BOOL ok;
 for(i=0;i<5;++i){
  reset();memset(guest,'x',sizeof(guest));guest[512]=0;if(messages[i]==WM_WINHELP)*(WORD *)guest=512;
  ok=construct(&value,messages[i],256,TRUE);++scenarios;
  check(!ok && !native_calls && !bad_copy && !live,names[i]);
  reset();memset(guest,'x',sizeof(guest));guest[512]=0;if(messages[i]==WM_WINHELP)*(WORD *)guest=512;
  check(construct(&value,messages[i],256,FALSE),"normal constructor");++scenarios;
  check(allocations==1 && live==1,"heap acquired once");
  if(messages[i]==LB_GETSELITEMS){((INT *)value.lParam)[0]=3;((INT *)value.lParam)[1]=7;value.lReturn=2;}
  else if(messages[i]==WM_WINHELP)check(!memcmp((void *)value.lParam,guest,512),"help copied exact");
  else if(messages[i]==EM_REPLACESEL)check(!memcmp((void *)value.lParam,guest,513),"replacement copied exact");
  else check(((INT *)value.lParam)[0]==(SHORT)0x7878,"tabs widened");
  finish(&value,messages[i]);check(!live && releases==1 && !bad_copy,"normal release once");
  if(messages[i]==LB_GETSELITEMS)check(((SHORT *)guest)[0]==3 && ((SHORT *)guest)[1]==7,"selection narrowed");
  if(i>0 && i<4){
   reset();check(construct(&value,messages[i],1,TRUE),"inline ignores failing heap");++scenarios;
   check(!allocations && value.lParam==(LONG)value.MsgBuffer,"original inline capacity");value.lReturn=0;finish(&value,messages[i]);check(!releases,"inline not freed");
   reset();check(construct(&value,messages[i],0,TRUE),"zero count unchanged");++scenarios;
   check(!allocations,"zero count no heap");value.lReturn=0;finish(&value,messages[i]);
   if(messages[i]==EM_SETTABSTOPS || messages[i]==LB_SETTABSTOPS){
    unsigned before=failures;
    reset();check(construct(&value,messages[i],0xffff,TRUE),"negative count submitted unchanged");++scenarios;
    check(!allocations && value.lParam==0x12340000,"negative count acquires no array");
    value.lReturn=0;finish(&value,messages[i]);
    check(failures==before && !releases,messages[i]==EM_SETTABSTOPS ? "EDIT_TABS_SIGNED_RELEASE" : "LIST_TABS_SIGNED_RELEASE");
   }
  }
 }
 {
  LBIV list={TRUE,NULL,8};INT stops[2]={4,8};unsigned before;
  reset();fail_allocate=FALSE;fail_free=FALSE;++scenarios;
  check(LBSetTabStops(&list,2,stops),"native tabs normal allocation");
  check(list.iTabPixelPositions && list.iTabPixelPositions[0]==2 && list.iTabPixelPositions[1]==8 && list.iTabPixelPositions[2]==16,"native tab conversion exact");
  check(LBSetTabStops(&list,0,NULL) && !list.iTabPixelPositions && !live && releases==1,"LIST_TABS_RESET_CLEARS_OWNERSHIP");
  before=failures;DestroyTabs(&list);check(failures==before && releases==1,"reset then original destruction releases once");
  before=releases;check(LBSetTabStops(&list,0,NULL) && releases==before,"repeat reset acquires/releases nothing");
  reset();check(LBSetTabStops(&list,2,stops),"native tabs reacquire");fail_free=TRUE;++scenarios;
  check(LBSetTabStops(&list,0,NULL) && list.iTabPixelPositions && live==1 && !releases,"LIST_TABS_FAILED_FREE_KEEPS_OWNERSHIP");
  fail_free=FALSE;if(list.iTabPixelPositions)user_free(list.iTabPixelPositions);else if(live)user_free(heap);
  reset();fail_allocate=TRUE;list.iTabPixelPositions=NULL;++scenarios;
  check(!LBSetTabStops(&list,2,stops) && !list.iTabPixelPositions && !live,"native tabs allocation failure unchanged");
 }
 printf("REVERSE_CONSTRUCTION failures=%u scenarios=%u buffer=%u record=%u\n",failures,scenarios,(unsigned)sizeof(value.MsgBuffer),(unsigned)sizeof(value));
 return failures ? 1 : 0;
}
