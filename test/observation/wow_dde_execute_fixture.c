/* Exact forward EXECUTE case; bounded allocation/alias doubles, not guest proof. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef WORD HAND16, HWND16;
typedef DWORD VPVOID;
#include "execute_types.h"
typedef struct { struct { WORD hwnd,wMsg,wParam; LONG lParam; } WndProc; } PARM16;
typedef struct { PARM16 Parm16; LONG uParam,lParam; void *ddePostConversion; } MSGPARAMEX,*LPMSGPARAMEX;
static unsigned mode,locks,unlocks,native_live,duplicate_live,tracked,errors,cases;
static BYTE data[16]="command",copy[16],duplicate[16];
#define WOWDDE_POSTMESSAGE 1
#define FULLHWND32(v) ((HWND)(ULONG_PTR)(v))
#define LOGDEBUG(l,a) ((void)0)
#define GETMISCPTR(v,p) ((p)=(LPBYTE)(ULONG_PTR)(v))
#define FREEMISCPTR(p) ((void)0)
#define FLUSHVDMPTR(v,n,p) ((void)0)
static VPVOID lock16(HAND16 h,INT *cb){
 if(cb)*cb=mode==1?0:8;
 if(mode==1)return 0;
 if(h==11){++locks;return (VPVOID)data;}
 return (VPVOID)duplicate;
}
static BOOL unlock16(HAND16 h){if(h==11){++unlocks;--locks;}return TRUE;}
static VPVOID alloc16(UINT f,INT cb,HAND16 *h){(void)f;(void)cb;if(mode==4)return 0;*h=12;duplicate_live=1;return (VPVOID)duplicate;}
static HAND16 free16(VPVOID p){(void)p;duplicate_live=0;return 0;}
static HANDLE alloc32(UINT f,INT cb){(void)f;(void)cb;if(mode==2)return NULL;native_live=1;return copy;}
static HANDLE free32(HANDLE h){if(h)native_live=0;return NULL;}
static HANDLE pair(HWND16 to,WORD from,HAND16 h){(void)to;(void)from;(void)h;return mode>=3&&mode!=6?(HANDLE)data:NULL;}
static BOOL add(HWND16 to,WORD from,HAND16 h,HANDLE mem,PDDEINFO info){(void)to;(void)from;(void)h;(void)mem;(void)info;if(mode==5||mode==6)return FALSE;++tracked;return TRUE;}
static BOOL track(PDDECONVERSION c,HWND16 to,WORD from,HAND16 h,HANDLE mem,PDDEINFO info){(void)c;return add(to,from,h,mem,info);}
#define GlobalLock16 lock16
#define GlobalUnlock16 unlock16
#define GlobalAllocLock16 alloc16
#define GlobalUnlockFree16 free16
#define GlobalAlloc alloc32
#define WOWGLOBALALLOC(flags, bytes) GlobalAlloc(flags, bytes)
#define GlobalLock(h) ((LPVOID)(h))
#define GlobalUnlock(h) ((void)0)
#define GlobalFree free32
#define DDEFindPair32 pair
#define DDEAddhandle add
#define DDETrackhandle track
#include "execute_body.h"
#define CHECK(x) do{++cases;if(!(x)){++errors;printf("FAIL mode=%u line=%u\n",mode,__LINE__);}}while(0)
int main(void){
 for(mode=0;mode<7;++mode){
  MSGPARAMEX m={0};BOOL ok;
  locks=3;unlocks=native_live=duplicate_live=tracked=0;
  m.Parm16.WndProc.hwnd=1;m.Parm16.WndProc.wParam=2;
  m.Parm16.WndProc.wMsg=WM_DDE_EXECUTE;m.Parm16.WndProc.lParam=MAKELONG(0,11);
  ok=execute(&m,WOWDDE_POSTMESSAGE);
  CHECK(ok);CHECK(locks==3);CHECK(unlocks==(mode==1?0u:1u));
  if(mode==2||mode==4||mode==5||mode==6){CHECK(!m.lParam);CHECK(!native_live&&!duplicate_live&&!tracked);}
  else{CHECK(m.lParam==(LONG)copy&&native_live&&tracked==1);CHECK(mode==1||memcmp(copy,data,8)==0);CHECK(duplicate_live==(mode==3?1u:0u));}
 }
 printf("DDE_EXECUTE cases=%u errors=%u modes=7 guest_proof=0\n",cases,errors);
 return errors?1:0;
}
