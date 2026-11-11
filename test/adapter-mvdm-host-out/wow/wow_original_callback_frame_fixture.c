/* Actual selected wcall16.c / CallBack16, with controlled resolver and CPU
 * boundaries. This is NOT a real guest/nonpresent-segment execution test. */
#include "precomp.h"
#include "wow_callback_frame_lease.h"
#include <stdio.h>

static OPENNT_SUPPORT_TEB teb;
static TD task;
static PVDMFRAME caller;
static PCBVDMFRAME outbound, returned;
static unsigned resolutions, simulations, fail_at, errors;
static WORD ss_value, sp_value, ip_value;
static DWORD source_stack = 0x17871000, return_stack = 0x17972000;
BOOL fWowMode = TRUE;
/* Link-only storage required by the complete translation unit's GetExePtr16;
 * this fixture never calls that service or claims its cache behavior. */
HMODCACHE ghModCache[CHMODCACHE];

POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID) { return &teb; }
USHORT __cdecl getSP(void) { return sp_value; }
USHORT __cdecl getSS(void) { return ss_value; }
USHORT __cdecl getIP(void) { return ip_value; }
void __cdecl setSP(USHORT value) { sp_value=value; }
void __cdecl setIP(USHORT value) { ip_value=value; }
void __cdecl setSS(USHORT value) { ss_value=value; }

static void protect(void *page, DWORD access)
{
    DWORD old;
    if(!VirtualProtect(page,4096,access,&old)) ExitProcess(90);
}
PVOID FASTCALL GetPModeVDMPointerAssert(DWORD address)
{
    ++resolutions;
    if(resolutions==fail_at) RaiseException(0xe4470002,0,0,NULL);
    switch(resolutions) {
    case 1:
        if(address!=source_stack) ++errors;
        return caller;
    case 2:
        if(address!=((source_stack-sizeof(CBVDMFRAME))&~1u)) ++errors;
        /* A second resolver may invalidate the caller mapping. The actual
         * DIV178 snapshot must be consumed, not the now-inaccessible page. */
        protect(caller,PAGE_NOACCESS);
        return outbound;
    case 3:
        if(address!=return_stack) ++errors;
        return returned;
    default: ++errors; RaiseException(0xe4470003,0,0,NULL); return NULL;
    }
}
#ifndef Sim32FlushVDMPointer
BOOL Sim32FlushVDMPointer(ULONG address,USHORT size,PBYTE bytes,BOOL mode)
{
    (void)address;(void)size;(void)bytes;(void)mode;return TRUE;
}
#endif
void wow_callback_frame_trace(const char *phase,uint32_t ret_id,
    uint32_t stack,uint32_t cb,uint32_t linear,uint16_t id,uint32_t proc,
    uint16_t message,uint32_t parameter)
{
    (void)phase;(void)ret_id;(void)stack;(void)cb;(void)linear;
    (void)id;(void)proc;(void)message;(void)parameter;
}
void __cdecl host_simulate(void)
{
    ++simulations;
    if(outbound->wTDB!=0x1237 || outbound->wLocalBP!=0x5678 ||
        outbound->vpStack!=source_stack || outbound->wRetID!=RET_ENUMWINDOWPROC)
        ++errors;
    /* Guest return is represented by a fresh mapping and different VP.
     * No physical CCPU execution or relocation is claimed by this witness. */
    *returned=*outbound;
    returned->wAX=0xabcd;returned->wDX=0x1234;
    protect(outbound,PAGE_NOACCESS);
    ss_value=HIWORD(return_stack);sp_value=LOWORD(return_stack);
    ip_value=0x9999;
}
int __cdecl main(void)
{
    unsigned test;
    caller=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    outbound=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    returned=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!caller || !outbound || !returned)return 1;
    teb.WOW32Reserved=&task;
    for(test=0;test!=4;++test) {
        DWORD output=0xdeadbeef, exception=0; PARM16 args={0};
        protect(caller,PAGE_READWRITE);protect(outbound,PAGE_READWRITE);
        ZeroMemory(&task,sizeof(task));ZeroMemory(caller,sizeof(*caller));
        ZeroMemory(outbound,sizeof(*outbound));ZeroMemory(returned,sizeof(*returned));
        task.vpStack=source_stack;task.htask16=0x1237;
        task.dwFlags=TDF_INITCALLBACKSTACK;
        caller->wTDB=task.htask16;caller->wLocalBP=0x5678;
        resolutions=simulations=errors=0;fail_at=test;ip_value=0x1111;
        __try {
            if(!CallBack16(RET_ENUMWINDOWPROC,&args,0x15070020,&output))return 2;
        } __except(GetExceptionCode()==0xe4470002 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            exception=GetExceptionCode();
        }
        if(errors)return 3;
        if(!test) {
            if(exception || resolutions!=3 || simulations!=1 ||
                output!=0x1234abcd || task.vpStack!=source_stack || ip_value!=0x1111)
                return 4;
        } else if(exception!=0xe4470002 || resolutions!=test ||
            simulations!=(test==3 ? 1u:0u) || output!=0xdeadbeef) return 5;
        printf("ORIGINAL_CALLBACK_FRAME phase=%u resolve=%u simulate=%u output=%08lx exception=%08lx\n",
            test,resolutions,simulations,output,exception);
    }
    VirtualFree(caller,0,MEM_RELEASE);VirtualFree(outbound,0,MEM_RELEASE);
    VirtualFree(returned,0,MEM_RELEASE);
    puts("PASS selected CallBack16 snapshot/re-resolution/exception propagation/output preservation");
    return 0;
}
