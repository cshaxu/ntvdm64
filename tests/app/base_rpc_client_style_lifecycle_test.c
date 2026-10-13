#include <windows.h>
#include <stdio.h>
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "ntsrv-exe/opennt/include/base_rpc_client_launch_style.h"
#include "common/protocol/window_style.h"

#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void)
{
    DWORD style;
    unsigned repeat;
    /* No endpoint/mock: assert the real disconnected API contract. */
    for(repeat=0;repeat<3;++repeat) {
        CHECK(OpenNtBaseClientSetWindowStyle(WINDOW_STYLE_ALL)==ERROR_SUCCESS);
        CHECK(OpenNtBaseClientSetWindowStyle(3)==ERROR_INVALID_PARAMETER);
        CHECK(OpenNtBaseClientDosWindowStyle()==WINDOW_STYLE_OFF);
        CHECK(OpenNtBaseClientSeparateWowStyle(NULL)==ERROR_INVALID_PARAMETER);
        CHECK(OpenNtBaseClientSeparateWowStyle(&style)==ERROR_INVALID_STATE);
        style=99;
        CHECK(OpenNtBaseClientWowStyle(1,&style)==ERROR_INVALID_STATE);
        CHECK(style==WINDOW_STYLE_OFF);
        CHECK(OpenNtBaseClientWowStyle(1,NULL)==ERROR_INVALID_PARAMETER);
        OpenNtBaseClientDisconnectCurrent();
        OpenNtBaseClientDisconnectCurrent();
        CHECK(OpenNtBaseClientDosWindowStyle()==WINDOW_STYLE_OFF);
        CHECK(OpenNtBaseClientSeparateWowStyle(&style)==ERROR_INVALID_STATE);
    }
    puts("PASS disconnected launch-style arguments and repeated cleanup; no RPC/guest propagation claimed");
    return 0;
}
