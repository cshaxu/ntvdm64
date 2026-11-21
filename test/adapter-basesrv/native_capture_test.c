/* Real original capture allocator and local ABI, x86 and x64. No CSR wire. */
#include <nt.h>
#include <ntlpcapi.h>
#pragma push_macro("NTSYSAPI")
#undef NTSYSAPI
#define NTSYSAPI
#include <ntcsrdll.h>
#pragma pop_macro("NTSYSAPI")
#include "compat/include/base_capture_heap.h"
#include <stdio.h>
#include <stdint.h>
static unsigned checks,failures;
#define CHECK(x) do{++checks;if(!(x)){++failures;printf("FAIL %u %s\n",(unsigned)__LINE__,#x);}}while(0)
int main(void)
{
    unsigned count,index,round;DWORD before,after;
    CHECK(sizeof(*((PCSR_CAPTURE_HEADER)0)->MessagePointerOffsets)==sizeof(void *));
    CHECK(sizeof(CSR_CAPTURE_HEADER)==(sizeof(void *)==8 ? 48 : 28));
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    if(!opennt_base_capture_heap_begin())return 1;
    CHECK(CsrPortHeap!=NULL);
    for(round=0;round<16;++round)for(count=1;count<=9;++count) {
        PCSR_CAPTURE_HEADER capture=CsrAllocateCaptureBuffer(count,0,256);
        PVOID values[9]={0};
        CHECK(capture!=NULL);if(!capture)continue;
        CHECK(((ULONG_PTR)capture->FreeSpace%sizeof(void *))==0);
        CHECK(capture->FreeSpace>=(PCHAR)(capture->MessagePointerOffsets+count));
        for(index=0;index<count;++index) {
            ULONG length=CsrAllocateMessagePointer(capture,index+1,&values[index]);
            CHECK(length==((index+1+sizeof(void *)-1)&~(sizeof(void *)-1)));
            CHECK(capture->MessagePointerOffsets[index]==(ULONG_PTR)&values[index]);
            CHECK(((ULONG_PTR)values[index]%sizeof(void *))==0);
            CHECK((PCHAR)values[index]+length<=(PCHAR)capture+capture->Length);
        }
        CHECK(capture->CountMessagePointers==count);
        CsrFreeCaptureBuffer(capture);
    }
    CHECK(opennt_base_capture_heap_end() && CsrPortHeap==NULL);
    CHECK(opennt_base_capture_heap_end());
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && before==after);
    printf("native capture bits=%u checks=%u failures=%u; original allocation/table/cleanup\n",(unsigned)(8*sizeof(void *)),checks,failures);
    return failures?1:0;
}
