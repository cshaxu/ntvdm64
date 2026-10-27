#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#define FASTCALL __stdcall
typedef WORD HAND16,HWND16,HMENU16,HDC16,HTASK16,HMIDIIN16,HMIDIOUT16,HWAVEIN16,HWAVEOUT16;
typedef DWORD VPVOID,VPINT16,VPSZ,VPPROC,VPRECT16; typedef VPVOID *PVPVOID; typedef SHORT INT16; typedef WORD *PWORD16;
typedef struct {DWORD dwStyle;HINSTANCE hInstance;} WW,*PWW;
#include "tabstop-object-types.h"
extern void __stdcall UnThunkEMMsg16(LPMSGPARAMEX);
EDITWORDBREAKPROCA W32WordBreakProc; static int callbacks;
BOOL __stdcall CallBack16(DWORD r,void *p,DWORD f,void *o){(void)r;(void)p;(void)f;(void)o;callbacks++;return FALSE;}
int main(void){MSGPARAMEX x;ZeroMemory(&x,sizeof(x));x.uMsg=EM_SETTABSTOPS;x.lReturn=0;UnThunkEMMsg16(&x);printf("ZERO callbacks=%d return=%ld proc=%p\n",callbacks,x.lReturn,W32WordBreakProc);if(callbacks||x.lReturn)return 2;ZeroMemory(&x,sizeof(x));x.uMsg=EM_SETTABSTOPS;x.lReturn=1;UnThunkEMMsg16(&x);printf("ONE callbacks=%d return=%ld proc=%p\n",callbacks,x.lReturn,W32WordBreakProc);return callbacks==1 && (ULONG_PTR)W32WordBreakProc==1 ? 0:3;}
