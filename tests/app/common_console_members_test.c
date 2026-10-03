/* Fault injection of the exact production snapshot implementation. Console
 * identity, joining and quiescence decisions remain separate owner tests. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "common/console/members.h"

static DWORD sequence[8],errors[8],calls,allocations,frees,live,fail_alloc;
static DWORD checks,failures;
#define CHECK(x) do { ++checks; if(!(x)) { ++failures;printf("FAIL line %u: %s\n",__LINE__,#x); } } while(0)
static LPVOID fake_alloc(HANDLE heap,DWORD flags,SIZE_T bytes)
{
    void *value;
    (void)heap;(void)flags;++allocations;
    if(allocations==fail_alloc)return NULL;
    value=malloc(bytes);if(value)++live;
    return value;
}
static BOOL fake_free(HANDLE heap,DWORD flags,LPVOID value)
{
    (void)heap;(void)flags;
    CHECK(value!=NULL);++frees;--live;free(value);return TRUE;
}
static DWORD fake_members(LPDWORD buffer,DWORD capacity)
{
    DWORD result=sequence[calls],error=errors[calls],i;
    CHECK(calls<ARRAYSIZE(sequence));++calls;
    for(i=0;i<min(result,capacity);++i)buffer[i]=100+i;
    SetLastError(error);return result;
}
#define HeapAlloc fake_alloc
#define HeapFree fake_free
#define GetConsoleProcessList fake_members
#include "../../src/common/console/members.c"
#undef GetConsoleProcessList
#undef HeapFree
#undef HeapAlloc

static void reset(void)
{
    CHECK(live==0);ZeroMemory(sequence,sizeof(sequence));ZeroMemory(errors,sizeof(errors));
    calls=allocations=frees=fail_alloc=0;
}
int main(void)
{
    DWORD *members=NULL,count=0,error;
    reset();sequence[0]=3;
    CHECK(common_console_members_read(16,4096,0,&members,&count)==0);
    CHECK(count==3 && members && members[2]==102 && calls==1 && live==1);
    common_console_members_release(members);CHECK(live==0 && frees==1);
    reset();sequence[0]=20;sequence[1]=25;sequence[2]=24;
    CHECK(common_console_members_read(16,4096,0,&members,&count)==0);
    CHECK(count==24 && calls==3 && allocations==3 && frees==2);
    common_console_members_release(members);CHECK(live==0);
    reset();sequence[0]=40;sequence[1]=41;
    CHECK(common_console_members_read(32,4096,2,&members,&count)==ERROR_RETRY);
    CHECK(!members && !count && calls==2 && live==0);
    reset();sequence[0]=4097;
    CHECK(common_console_members_read(32,4096,2,&members,&count)==ERROR_BUFFER_OVERFLOW);
    CHECK(!members && !count && live==0 && calls==1);
    reset();sequence[0]=65536;sequence[1]=65536;
    CHECK(common_console_members_read(16,65536,0,&members,&count)==0);
    CHECK(count==65536 && calls==2);common_console_members_release(members);
    reset();sequence[0]=65537;
    CHECK(common_console_members_read(16,65536,0,&members,&count)==ERROR_BUFFER_OVERFLOW);
    CHECK(!members && !count && live==0);
    reset();errors[0]=ERROR_INVALID_HANDLE;
    CHECK(common_console_members_read(16,4096,0,&members,&count)==ERROR_INVALID_HANDLE);
    CHECK(!members && !count && live==0);
    reset();SetLastError(ERROR_ACCESS_DENIED);
    CHECK(common_console_members_read(16,4096,0,&members,&count)==ERROR_GEN_FAILURE);
    CHECK(!members && !count && live==0);
    reset();fail_alloc=1;
    CHECK(common_console_members_read(16,4096,0,&members,&count)==ERROR_NOT_ENOUGH_MEMORY);
    CHECK(!members && !count && !calls && live==0);
    reset();sequence[0]=40;fail_alloc=2;
    CHECK(common_console_members_read(16,4096,0,&members,&count)==ERROR_NOT_ENOUGH_MEMORY);
    CHECK(!members && !count && calls==1 && frees==1 && live==0);
    reset();members=(DWORD *)1;count=99;
    CHECK(common_console_members_read(0,4096,0,&members,&count)==ERROR_INVALID_PARAMETER);
    CHECK(!members && !count && !allocations);
    error=common_console_members_read(4097,4096,0,&members,&count);
    CHECK(error==ERROR_INVALID_PARAMETER && !members && !count);
    CHECK(common_console_members_read(1,MAXDWORD,0,&members,&count)==ERROR_INVALID_PARAMETER);
    CHECK(common_console_members_read(16,4096,0,NULL,&count)==ERROR_INVALID_PARAMETER);
    CHECK(common_console_members_read(16,4096,0,&members,NULL)==ERROR_INVALID_PARAMETER);
    common_console_members_release(NULL);CHECK(live==0 && !calls);
    printf("common Console snapshot checks=%lu failures=%lu\n",checks,failures);
    return failures ? 1 : 0;
}
