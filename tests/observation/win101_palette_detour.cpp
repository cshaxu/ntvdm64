// Test-only observation; no result substitution or guest mutation.
#include <windows.h>
#include <stdio.h>
#include "../../src/nthook32-dll/detours/detours.h"
extern "C" BOOL ntvdm_console_text_palette(PALETTEENTRY colours[16]);
extern "C" BOOL WINAPI GetNextVDMCommand(void *info);
extern "C" DWORD ntcon_worker_call(void *client,void *request,void *reply);
static BOOL (*cache_call)(PALETTEENTRY[16])=ntvdm_console_text_palette;
static BOOL (WINAPI *next_call)(void *)=GetNextVDMCommand;
static DWORD (*io_call)(void *,void *,void *)=ntcon_worker_call;
static UINT (WINAPI *palette_call)(HPALETTE,UINT,UINT,LPPALETTEENTRY)=GetPaletteEntries;
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
    char kind[48];sprintf_s(kind,"io-operation-%lu",op);
    log_result(kind,client,r,e);return r;
}
static UINT WINAPI observe_palette(HPALETTE p,UINT first,UINT count,LPPALETTEENTRY entries){
    UINT r=palette_call(p,first,count,entries);DWORD e=GetLastError();log_result("gdi-fallback",p,r,e);return r;
}
static void __cdecl start_trace(){
    if(DetourTransactionBegin()!=NO_ERROR || DetourUpdateThread(GetCurrentThread())!=NO_ERROR ||
       DetourAttach(&(PVOID&)cache_call,observe_cache)!=NO_ERROR ||
       DetourAttach(&(PVOID&)next_call,observe_next)!=NO_ERROR ||
       DetourAttach(&(PVOID&)io_call,observe_io)!=NO_ERROR ||
       DetourAttach(&(PVOID&)palette_call,observe_palette)!=NO_ERROR ||
       DetourTransactionCommit()!=NO_ERROR)ExitProcess(127);
}
#pragma section(".CRT$XCU",read)
extern "C" __declspec(allocate(".CRT$XCU")) void (__cdecl *win101_palette_trace_entry)()=start_trace;
