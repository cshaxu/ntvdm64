#include <windows.h>
#include <string.h>
#include "lib-win32/process-hook/helper.h"
#include "lib-win32/process-hook/staging.h"
static const BYTE payload[8]={0x19,0x81,0x03,0x57,0x44,0x35,0x7a,0x21};
static DWORD apply(HANDLE child,const void *packet,DWORD size,PCWSTR dll,void *unused)
{
    (void)dll;(void)unused;
    if(size!=sizeof(payload)||memcmp(packet,payload,sizeof(payload)))return ERROR_INVALID_DATA;
    return process_hook_stage_payload(child,packet,size);
}
void CALLBACK Finish(HWND window,HINSTANCE instance,LPSTR command,INT show)
{(void)window;(void)instance;(void)command;(void)show;ExitProcess(process_hook_run_helper(apply,NULL));}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID reserved)
{(void)instance;(void)reason;(void)reserved;return TRUE;}
