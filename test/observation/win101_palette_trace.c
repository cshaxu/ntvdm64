// Test-only observation; no result substitution or guest mutation.
#include <windows.h>
#include <stdio.h>
#include "../../src/lib-win32/process-hook/startup_hooks.h"
BOOL ntvdm_console_text_palette(PALETTEENTRY colours[16]);
BOOL WINAPI GetNextVDMCommand(void *info);
DWORD ntcon_worker_call(void *client,void *request,void *reply);
static BOOL (*cache_call)(PALETTEENTRY[16]);
static BOOL (WINAPI *next_call)(void *);
static DWORD (*io_call)(void *,void *,void *);
static UINT (WINAPI *palette_call)(HPALETTE,UINT,UINT,LPPALETTEENTRY);
static void log_result(const char *kind,void *palette,DWORD result,DWORD error){
    char path[32768];FILE *f;DWORD n=GetEnvironmentVariableA("WIN101_PALETTE_TRACE",path,sizeof(path));
    if(n && n<sizeof(path) && fopen_s(&f,path,"a")==0 && f){fprintf(f,"%s palette=%p result=%lu error=%lu\n",kind,palette,result,error);fclose(f);}
    SetLastError(error);
}
static BOOL observe_cache(PALETTEENTRY c[16]){
    BOOL r=cache_call(c);DWORD e=GetLastError();log_result("text-cache",NULL,r,e);return r;
}
static BOOL WINAPI observe_next(void *info){
    BOOL r=next_call(info);DWORD e=GetLastError();
    log_result("get-next",info,r,e);return r;
}
static DWORD observe_io(void *client,void *request,void *reply){
    DWORD op=((DWORD *)request)[3];
    DWORD r=io_call(client,request,reply);DWORD e=GetLastError();
    char kind[48];sprintf_s(kind,sizeof(kind),"io-operation-%lu",op);
    log_result(kind,client,r,e);return r;
}
static UINT WINAPI observe_palette(HPALETTE p,UINT first,UINT count,LPPALETTEENTRY entries){
    UINT r=palette_call(p,first,count,entries);DWORD e=GetLastError();log_result("gdi-fallback",p,r,e);return r;
}
static void __cdecl start_trace(void){
    DWORD error;
    cache_call=ntvdm_console_text_palette;next_call=GetNextVDMCommand;
    io_call=ntcon_worker_call;palette_call=GetPaletteEntries;
    error=process_hook_startup_begin();
    if(!error)error=process_hook_startup_attach(&cache_call,(void *)observe_cache);
    if(!error)error=process_hook_startup_attach(&next_call,(void *)observe_next);
    if(!error)error=process_hook_startup_attach(&io_call,(void *)observe_io);
    if(!error)error=process_hook_startup_attach(&palette_call,(void *)observe_palette);
    if(error){process_hook_startup_abort();ExitProcess(127);}
    if(process_hook_startup_commit())ExitProcess(127);
}
#pragma section(".CRT$XCU",read)
__declspec(allocate(".CRT$XCU")) void (__cdecl *win101_palette_trace_entry)(void)=start_trace;
