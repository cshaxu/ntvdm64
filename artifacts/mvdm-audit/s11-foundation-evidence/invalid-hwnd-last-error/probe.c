#include <windows.h>
#include <stdio.h>
int main(void) { HWND h=CreateWindowExA(0,"STATIC","",0,0,0,1,1,NULL,NULL,GetModuleHandle(NULL),NULL); DWORD pid=0,tid; if(!h) return 2; if(!DestroyWindow(h)) return 3; SetLastError(777); tid=GetWindowThreadProcessId(h,&pid); printf("dead hwnd=%p tid=%lu pid=%lu error=%lu\n",h,tid,pid,GetLastError()); SetLastError(777); tid=GetWindowThreadProcessId((HWND)(ULONG_PTR)0x77777777,&pid); printf("invalid tid=%lu error=%lu\n",tid,GetLastError()); return 0; }
