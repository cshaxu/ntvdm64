#include <windows.h>
#include <stdio.h>
#include <string.h>

static DWORD observed;
static DWORD exception_ip;
static int filter(EXCEPTION_POINTERS *p) {
    observed=p->ExceptionRecord->ExceptionCode;
    exception_ip=p->ContextRecord->Eip;
    return EXCEPTION_EXECUTE_HANDLER;
}
int main(void) {
    const unsigned char samples[][6]={{0xcd,3,0xc3},{0xf2,0xcd,3,0xc3},
        {0xf3,0xcd,3,0xc3},{0x67,0xf3,0xcd,3,0xc3},{0xf0,0xcd,3,0xc3}};
    const DWORD expected[]={EXCEPTION_BREAKPOINT,EXCEPTION_BREAKPOINT,
        EXCEPTION_BREAKPOINT,EXCEPTION_BREAKPOINT,EXCEPTION_ILLEGAL_INSTRUCTION};
    /* Windows reports the breakpoint context one byte before post-INT EIP. */
    const DWORD offsets[]={1,2,2,3,0};
    unsigned i; int ok=1;
    unsigned char *p=(unsigned char *)VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    if(!p)return 2;
    for(i=0;i<sizeof(expected)/sizeof(expected[0]);i++) {
        memcpy(p,samples[i],sizeof(samples[i])); FlushInstructionCache(GetCurrentProcess(),p,6);
        observed=0;
        __try { ((void (__cdecl *)(void))p)(); }
        __except(filter(GetExceptionInformation())) { }
        printf("case=%u exception=%08lx expected=%08lx ip-offset=%lu\n",i,observed,expected[i],exception_ip-(DWORD)p);
        if(observed!=expected[i] || exception_ip-(DWORD)p!=offsets[i])ok=0;
    }
    VirtualFree(p,0,MEM_RELEASE); return ok ? 0 : 1;
}
