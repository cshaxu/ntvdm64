#include <windows.h>
#include "mvdm_softpc_firmware.h"
#include "common/system_root.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;} } while(0)

int main(void)
{
    static const char input[]="PATH=C:\\Windows\0SYSTEMROOT=C:\\Windows\0";
    char root[MAX_PATH],short_root[MAX_PATH],host[MAX_PATH],after[MAX_PATH];
    char expected[MAX_PATH+32],*block,*original;
    DWORD bytes,host_bytes;
    CHECK(common_system_root_a(root,sizeof(root))==0);
    CHECK(GetShortPathNameA(root,short_root,sizeof(short_root))>0);
    while(*short_root && short_root[strlen(short_root)-1]=='\\')short_root[strlen(short_root)-1]=0;
    host_bytes=GetEnvironmentVariableA("SYSTEMROOT",host,sizeof(host));
    CHECK(host_bytes && host_bytes<sizeof(host));
    bytes=sizeof(input);block=malloc(bytes);CHECK(block);
    memcpy(block,input,bytes);
    CHECK(mvdm_softpc_project_wow_initial_environment(&block,&bytes));
    CHECK(!strcmp(block,"PATH=C:\\Windows"));
    CHECK(sprintf_s(expected,sizeof(expected),"SYSTEMROOT=%s",short_root)>0);
    CHECK(!strcmp(block+strlen(block)+1,expected));
    CHECK(bytes==strlen("PATH=C:\\Windows")+1+strlen(expected)+2);
    CHECK(block[bytes-1]==0 && block[bytes-2]==0);
    /* The replacement is CRT-owned, as cmdmisc's original free requires. */
    free(block);
    CHECK(GetEnvironmentVariableA("SYSTEMROOT",after,sizeof(after))==host_bytes);
    CHECK(!strcmp(host,after));
    bytes=sizeof(input);block=malloc(bytes);CHECK(block);original=block;
    memcpy(block,input,bytes);
    CHECK(!mvdm_softpc_project_wow_initial_environment(&block,NULL));
    CHECK(GetLastError()==ERROR_INVALID_PARAMETER && block==original);
    --bytes;
    CHECK(!mvdm_softpc_project_wow_initial_environment(&block,&bytes));
    CHECK(GetLastError()==ERROR_INVALID_PARAMETER && block==original);
    CHECK(!memcmp(block,input,sizeof(input)));
    free(block);
    puts("WOW-BOOT-ENVIRONMENT-PASS");
    return 0;
}
