#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FASTCALL __stdcall
typedef WORD HAND16,HWND16,HMENU16,HDC16,HTASK16,HMIDIIN16,HMIDIOUT16,HWAVEIN16,HWAVEOUT16;
typedef DWORD VPVOID,VPINT16,VPSZ,VPPROC,VPRECT16; typedef VPVOID *PVPVOID; typedef SHORT INT16; typedef WORD *PWORD16;
typedef struct {DWORD dwStyle;HINSTANCE hInstance;} WW,*PWW;
#include "../shard-2/tabstop-object-types.h"
extern void __stdcall UnThunkEMMsg16(LPMSGPARAMEX);
extern DWORD WordBreakProc16;
extern int seam_free_count;
extern void *seam_expected_free;
EDITWORDBREAKPROCA W32WordBreakProc;
static int callbacks;
BOOL __stdcall CallBack16(DWORD r,void *p,DWORD f,void *o) { (void)r;(void)p;(void)f;(void)o; callbacks++; return FALSE; }
int main(int argc,char **argv) {
 MSGPARAMEX x; int count, heap, result, expected_free; void *buffer;
 if(argc!=4)return 2;
 count=atoi(argv[1]); heap=atoi(argv[2]); result=atoi(argv[3]);
 ZeroMemory(&x,sizeof(x)); x.uMsg=EM_SETTABSTOPS; x.Parm16.WndProc.wParam=(WORD)count; x.lReturn=result;
 buffer=heap?HeapAlloc(GetProcessHeap(),0,256):(void*)x.MsgBuffer;
 if(!buffer)return 3;
 x.lParam=(LONG)(ULONG_PTR)buffer; expected_free=(count>0 && heap); seam_expected_free=expected_free?buffer:NULL;
 UnThunkEMMsg16(&x);
 printf("count=%d heap=%d result=%d free=%d callbacks=%d ret=%ld proc=%p word16=%08lx\n",count,heap,result,seam_free_count,callbacks,x.lReturn,W32WordBreakProc,WordBreakProc16);
 if(!expected_free && heap && !HeapFree(GetProcessHeap(),0,buffer))return 4;
 return seam_free_count==expected_free && callbacks==(result?1:0) && x.lReturn==result && (ULONG_PTR)W32WordBreakProc==(ULONG_PTR)result ? 0:5;
}
