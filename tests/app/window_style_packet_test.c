#include "common/codec/native_launch.h"
#include <stdio.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"STYLE PACKET FAIL line %d\n",__LINE__);return 1;} } while(0)
int main(void)
{
    run16_native_start start={0};
    DWORD policy,bytes;BYTE *data;WCHAR *strings[4];run16_native_launch_packet decoded;
    start.command=L"example.exe /original-tail";start.directory=L"C:\\";start.environment=L"";
    for(policy=WINDOW_STYLE_OFF;policy<=WINDOW_STYLE_ALL;++policy){
        start.window_style=policy;
        CHECK(run16_native_launch_pack(&start,&data,&bytes)==ERROR_SUCCESS);
        CHECK(run16_native_launch_unpack(data,bytes,&decoded,strings)==ERROR_SUCCESS);
        CHECK(decoded.window_style==policy && !wcscmp(strings[1],start.command));
        CHECK(!decoded.capabilities[0] && !decoded.capabilities[1]);
        ((run16_native_launch_packet *)data)->window_style=3;
        CHECK(run16_native_launch_unpack(data,bytes,&decoded,strings)==ERROR_INVALID_DATA);
        HeapFree(GetProcessHeap(),0,data);
    }
    start.window_style=3;
    CHECK(run16_native_launch_pack(&start,&data,&bytes)==ERROR_INVALID_PARAMETER);
    start.window_style=MAXDWORD;
    CHECK(run16_native_launch_pack(&start,&data,&bytes)==ERROR_INVALID_PARAMETER);
    puts("STYLE PACKET PASS three policies, GUI/no-console context, invalid values");return 0;
}
