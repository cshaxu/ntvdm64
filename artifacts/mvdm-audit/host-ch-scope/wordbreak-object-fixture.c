#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#define FASTCALL __stdcall
typedef WORD HAND16,HWND16,HMENU16,HDC16,HTASK16,HMIDIIN16,HMIDIOUT16,HWAVEIN16,HWAVEOUT16;
typedef DWORD VPVOID,VPINT16,VPSZ,VPPROC,VPRECT16; typedef VPVOID *PVPVOID; typedef SHORT INT16; typedef WORD *PWORD16;
typedef struct {DWORD dwStyle;HINSTANCE hInstance;} WW,*PWW;
#include "../shard-2/tabstop-object-types.h"
extern void __stdcall UnThunkEMMsg16(LPMSGPARAMEX);
extern DWORD WordBreakProc16;
EDITWORDBREAKPROCA W32WordBreakProc;
static int callbacks,callback_success;
static DWORD callback_reason; static SHORT callback_ordinal;
BOOL __stdcall CallBack16(DWORD r,void *p,DWORD f,void *o) {
 PARM16 *parm=p; (void)f;(void)o; callbacks++;callback_reason=r;callback_ordinal=parm->SubClassProc.iOrdinal;
 if(callback_success) WordBreakProc16=0x12345678; /* controlled guest side effect */
 return callback_success;
}
int main(int argc,char **argv) {
 MSGPARAMEX x; DWORD input=0,ret=0,initial16=0,expected16=0,expected32=0x11112222; int cb=0;
 if(argc!=2)return 2; ZeroMemory(&x,sizeof(x)); x.uMsg=EM_GETWORDBREAKPROC;
 if(!strcmp(argv[1],"zero")) { initial16=expected16=0x56789abc; }
 else if(!strcmp(argv[1],"guest-ldt")) { input=0x80040077;ret=0x00040077; }
 else if(!strcmp(argv[1],"guest-high")) { input=0x80000077;ret=0x80040077; }
 else if(!strcmp(argv[1],"native-fail")) { input=ret=0x00123456;expected32=expected16=input;cb=1; }
 else if(!strcmp(argv[1],"native-success")) { input=ret=0x00123456;expected32=input;expected16=0x12345678;callback_success=1;cb=1; }
 else if(!strcmp(argv[1],"native-cached")) { input=0x00123456;initial16=ret=expected16=0x56789abc; }
 else return 3;
 x.lReturn=(LONG)input; WordBreakProc16=initial16; W32WordBreakProc=(EDITWORDBREAKPROCA)(ULONG_PTR)0x11112222;
 UnThunkEMMsg16(&x);
 printf("%s ret=%08lx word16=%08lx word32=%08lx callbacks=%d reason=%lu ordinal=%d\n",argv[1],(DWORD)x.lReturn,WordBreakProc16,(DWORD)(ULONG_PTR)W32WordBreakProc,callbacks,callback_reason,callback_ordinal);
 return (DWORD)x.lReturn==ret && WordBreakProc16==expected16 && (DWORD)(ULONG_PTR)W32WordBreakProc==expected32 && callbacks==cb && (!cb || (callback_reason==27 && callback_ordinal==535))?0:4;
}
