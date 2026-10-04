#include "run16-exe/guest_environment.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if(!(x)) { \
    fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1; } } while(0)

int main(void)
{
    static const char input[]="PATH=C:\\Windows\0SYSTEMROOT=C:\\Windows\0"
        "WIN16DIR=O:\\package\0systemroot=incorrect\0";
    static const char expected[]="PATH=C:\\Windows\0WIN16DIR=O:\\package\0"
        "SYSTEMROOT=O:\\package\0";
    static const char missing[]="PATH=C:\\Windows\0";
    static const char trailing[]="A=B\0\0HIDDEN=X\0";
    char preserved[sizeof(input)],host_root[MAX_PATH],host_after[MAX_PATH];
    PSTR projected=NULL;
    DWORD bytes=0,host_bytes;
    host_bytes=GetEnvironmentVariableA("SYSTEMROOT",host_root,sizeof(host_root));
    CHECK(host_bytes && host_bytes<sizeof(host_root));
    memcpy(preserved,input,sizeof(input));
    CHECK(run16_guest_environment_root(input,sizeof(input),"O:\\package",
        &projected,&bytes)==0);
    CHECK(bytes==sizeof(expected));
    CHECK(!memcmp(projected,expected,bytes));
    CHECK(!memcmp(input,preserved,sizeof(input)));
    CHECK(GetEnvironmentVariableA("SYSTEMROOT",host_after,sizeof(host_after))==host_bytes);
    CHECK(!strcmp(host_root,host_after));
    HeapFree(GetProcessHeap(),0,projected);
    CHECK(run16_guest_environment_root(missing,sizeof(missing),"O:\\package",
        &projected,&bytes)==0);
    CHECK(!memcmp(projected,missing,sizeof(missing)-1u));
    CHECK(!strcmp(projected+sizeof(missing)-1u,"SYSTEMROOT=O:\\package"));
    HeapFree(GetProcessHeap(),0,projected);
    CHECK(run16_guest_environment_root(input,sizeof(input)-1u,"O:\\package",
        &projected,&bytes)==ERROR_INVALID_PARAMETER);
    CHECK(!projected && !bytes);
    CHECK(run16_guest_environment_root(trailing,sizeof(trailing),"O:\\package",
        &projected,&bytes)==ERROR_INVALID_PARAMETER);
    CHECK(!projected && !bytes);
    CHECK(run16_guest_environment_root(input,sizeof(input),"",
        &projected,&bytes)==ERROR_INVALID_PARAMETER);
    puts("GUEST-ENVIRONMENT-ROOT-PASS");
    return 0;
}
