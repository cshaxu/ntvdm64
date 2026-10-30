#include <windows.h>
#include <stdio.h>
int main(void) { HMODULE h=GetModuleHandleW(L"KernelBase.dll"); const char *names[]={"RaiseException","DebugBreak","UnhandledExceptionFilter"};int i;if(!h)return 1;for(i=0;i<3;i++){FARPROC p=GetProcAddress(h,names[i]);printf("%s rva=%08lx\n",names[i],(DWORD)((BYTE*)p-(BYTE*)h));}return 0;}
