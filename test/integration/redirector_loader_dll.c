#include <windows.h>
static LONG *detached;
__declspec(dllexport) void Configure(LONG *p){detached=p;}
BOOL WINAPI DllMain(HINSTANCE h,DWORD reason,void *reserved){(void)h;(void)reserved;if(reason==DLL_PROCESS_DETACH && detached)InterlockedIncrement(detached);return TRUE;}
__declspec(dllexport) ULONG VrDispatch(void){return 0;}
__declspec(dllexport) ULONG VrInitialized(void){return 0;}
__declspec(dllexport) ULONG VrReadNamedPipe(void){return 0;}
__declspec(dllexport) ULONG VrWriteNamedPipe(void){return 0;}
__declspec(dllexport) ULONG VrIsNamedPipeName(void){return 0;}
__declspec(dllexport) ULONG VrIsNamedPipeHandle(void){return 0;}
__declspec(dllexport) ULONG VrAddOpenNamedPipeInfo(void){return 0;}
__declspec(dllexport) ULONG VrConvertLocalNtPipeName(void){return 0;}
__declspec(dllexport) ULONG VrRemoveOpenNamedPipeInfo(void){return 0;}
