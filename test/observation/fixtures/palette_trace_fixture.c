#include <windows.h>
#include <stdio.h>
static volatile LONG cache_calls,next_calls,io_calls;
__declspec(noinline) BOOL ntvdm_console_text_palette(PALETTEENTRY colours[16])
{InterlockedIncrement(&cache_calls);if(colours)colours[0].peRed=31;SetLastError(1101);return TRUE;}
__declspec(noinline) BOOL WINAPI GetNextVDMCommand(void *info)
{InterlockedIncrement(&next_calls);(void)info;SetLastError(1102);return TRUE;}
__declspec(noinline) DWORD ntcon_worker_call(void *client,void *request,void *reply)
{InterlockedIncrement(&io_calls);(void)client;(void)request;(void)reply;SetLastError(1103);return 99;}
int main(void)
{
    PALETTEENTRY colours[16]={0};DWORD request[4]={0,0,0,27};
    if(!ntvdm_console_text_palette(colours)||cache_calls!=1||colours[0].peRed!=31||GetLastError()!=1101)return 1;
    if(!GetNextVDMCommand(NULL)||next_calls!=1||GetLastError()!=1102)return 2;
    if(ntcon_worker_call(NULL,request,NULL)!=99||io_calls!=1||GetLastError()!=1103)return 3;
    (void)GetPaletteEntries(NULL,0,1,colours);
    puts("PALETTE_TRACE_ORIGINAL_PASS");return 0;
}
