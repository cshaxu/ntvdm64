#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
static unsigned geometry_calls,reads,scenario;
static char content(void)
{
    return (scenario==5 && reads>1) || (scenario==6 && !(reads&1)) ? 'B' : 'A';
}
static BOOL fake_info(HANDLE output,CONSOLE_SCREEN_BUFFER_INFO *info)
{
    (void)output;
    memset(info,0,sizeof(*info));
    info->dwSize.X=(scenario==1 && geometry_calls==0) ||
        (scenario==2 && !(geometry_calls&1)) ? 80 : 120;
    info->dwSize.Y=2;info->srWindow.Right=info->dwSize.X-1;
    info->srWindow.Bottom=1;++geometry_calls;return TRUE;
}
static BOOL fake_read(HANDLE output,LPSTR text,DWORD count,COORD start,LPDWORD read)
{
    (void)output;(void)start;++reads;
    if(scenario==4){SetLastError(ERROR_ACCESS_DENIED);return FALSE;}
    memset(text,content(),count);*read=scenario==3 ? count-1 : count;return TRUE;
}
static BOOL fake_rectangle(HANDLE output,PCHAR_INFO text,COORD size,COORD start,PSMALL_RECT area)
{
    unsigned i;
    (void)output;(void)start;++reads;
    if(scenario==4){SetLastError(ERROR_ACCESS_DENIED);return FALSE;}
    for(i=0;i<(unsigned)size.X*size.Y;++i)text[i].Char.AsciiChar=content();
    if(scenario==3)--area->Right;
    return TRUE;
}
#define GetConsoleScreenBufferInfo fake_info
#define ReadConsoleOutputCharacterA fake_read
#define ReadConsoleOutputA fake_rectangle
#include "console_snapshot.h"
#define CHECK(x) do{if(!(x)){printf("FAIL line %d\n",__LINE__);return 1;}}while(0)
int main(void)
{
    for(scenario=0;scenario<7;++scenario) {
        CONSOLE_SCREEN_BUFFER_INFO info;char *text=NULL;DWORD count,error;
        geometry_calls=reads=0;
        error=observer_console_snapshot(NULL,&info,&text,&count);
        if(scenario<2 || scenario==5){
            CHECK(!error && text && count==240 && info.dwSize.X==120);
            CHECK(reads==(scenario==0 ? 2u : 4u));free(text);
        }else{
            CHECK(!text && !count);
            CHECK(error==(DWORD)(scenario==4 ? ERROR_ACCESS_DENIED : ERROR_RETRY));
            CHECK(reads==(scenario==4 ? 1u : 8u));
        }
    }
    puts("PASS snapshot stable/resize/repeated resize/short read/API failure/content transition/unstable content");
    return 0;
}
