#include "wow_user_probe_bindings.h"
#include <stdio.h>
static unsigned errors;
#define CHECK(x) do { if(!(x)){++errors;printf("FAIL %d\n",__LINE__);} } while(0)
static DWORD probe(const void *p,ULONG bytes,ULONG alignment)
{
    DWORD code=0;
    __try { ProbeForRead(p,bytes,alignment); }
    __except(EXCEPTION_EXECUTE_HANDLER) { code=GetExceptionCode(); }
    return code;
}
int __cdecl main(void)
{
    SYSTEM_INFO info;
    ULONG_PTR limit;
    BYTE *page=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_NOACCESS);
    DWORD fault=0;
    ULONG alignment;
    volatile BYTE value=0;
    GetSystemInfo(&info);limit=(ULONG_PTR)info.lpMaximumApplicationAddress+1;
    CHECK(page!=NULL);if(!page)return 2;
    for(alignment=1;alignment<=16;alignment*=2)CHECK(probe(page,16,alignment)==0);
    CHECK(probe(page+1,1,2)==0x80000002u);
    CHECK(probe((void *)0xffffffff,0,16)==0); /* Zero length skips all checks. */
    CHECK(probe((void *)0xffffffff,1,2)==0x80000002u); /* Alignment precedes range. */
    CHECK(probe((void *)0xfffffff0,32,16)==0xc0000005u);
    CHECK(probe((void *)(limit-1),1,1)==0);
    CHECK(probe((void *)limit,1,1)==0xc0000005u);
    CHECK(probe(NULL,1,1)==0); /* This original probe does not read pages. */
    CHECK(probe(page,4096,1)==0);
    __try { value=*page; }
    __except(EXCEPTION_EXECUTE_HANDLER) { fault=GetExceptionCode(); }
    (void)value;
    CHECK(fault==0xc0000005u);
    CHECK(VirtualFree(page,0,MEM_RELEASE));
    printf("WOW_ORIGINAL_PROBE errors=%u alignment=5 range=3 no_page_touch=1 access_fault_caught=1\n",errors);
    return errors!=0;
}
