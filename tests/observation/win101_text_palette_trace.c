/* Test-only palette/capture boundary observation. Preserve real result/error. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "ntvdm-exe/win32/console_client.h"
static void record(const char *kind,void *palette,DWORD result,DWORD error){
    char path[32768];FILE *file;DWORD n=GetEnvironmentVariableA("WIN101_PALETTE_TRACE",path,sizeof(path));
    if(n && n<sizeof(path) && fopen_s(&file,path,"a")==0 && file){
        fprintf(file,"%s palette=%p result=%lu error=%lu\n",kind,palette,result,error);fclose(file);
    }
    SetLastError(error);
}
BOOL win101_observe_text_palette(PALETTEENTRY colours[16]){
    BOOL result=ntvdm_console_text_palette(colours);DWORD error=GetLastError();
    record("text-cache",NULL,result,error);return result;
}
UINT WINAPI win101_observe_palette_entries(HPALETTE palette,UINT first,UINT count,LPPALETTEENTRY values){
    UINT result=GetPaletteEntries(palette,first,count,values);DWORD error=GetLastError();
    record("gdi-fallback",palette,result,error);return result;
}
