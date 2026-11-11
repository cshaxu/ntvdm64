/* Retain the existing four original-body assertions. Only its service doubles
 * are extended here; the complete original wcall16 and actual control facade
 * are independently compiled by the producer. Not real guest/CPU evidence. */
#define main fixture_original_four
#define host_simulate fixture_old_simulate
#define GetPModeVDMPointerAssert fixture_old_resolver
#define wow_callback_frame_trace fixture_unused_trace
#include "wow_original_callback_frame_fixture.c"
#undef main
#undef host_simulate
#undef GetPModeVDMPointerAssert
#undef wow_callback_frame_trace

static WORD cs_value=0x2467;
static int32_t pe_value=1,vm_value,cpl_value=3;
static BOOL active=TRUE;
static unsigned variant;
static DWORD expected_callback,shared_change;
/* The complete adapter TU also defines the separate resource-copy API.
 * The tested original CallBack16 uses its protected resolver, not that API.
 * Keep these three exact service edges fail-closed, never fake-successful. */
int __cdecl mvdm_softpc_guest_memory_acquire(uint32_t address,uint32_t count,
    uint32_t access,guest_memory_lease **lease,uint8_t **bytes)
{(void)address;(void)count;(void)access;(void)lease;(void)bytes;ExitProcess(94);return 0;}
int __cdecl mvdm_softpc_guest_memory_release(guest_memory_lease *lease,int commit)
{(void)lease;(void)commit;ExitProcess(94);return 0;}
int __cdecl mvdm_softpc_protected_address(uint16_t selector,uint32_t offset,uint32_t *address)
{(void)selector;(void)offset;(void)address;ExitProcess(94);return 0;}
USHORT __cdecl getCS(void) {return cs_value;}
USHORT __cdecl getAX(void) {return 0;}
USHORT __cdecl getBX(void) {return 0;}
USHORT __cdecl getBP(void) {return 0;}
USHORT __cdecl getSI(void) {return 0;}
USHORT __cdecl getDI(void) {return 0;}
int32_t __cdecl c_getPE(void) {return pe_value;}
int32_t __cdecl c_getVM(void) {return vm_value;}
int32_t __cdecl c_getCPL(void) {return cpl_value;}
void __cdecl c_setPE(int32_t value) {pe_value=value;}
void __cdecl c_setVM(int32_t value) {vm_value=value;}
void __cdecl c_setCPL(int32_t value) {cpl_value=value;}
int32_t __cdecl c_setCS(uint16_t value) {cs_value=value;return variant==4?13:0;}
int32_t __cdecl c_setSS(uint16_t value) {ss_value=value;return 0;}
BOOL WINAPI wow_user_worker_active(void) {return active;}
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage,DWORD error)
{
    if(strcmp(stage,"callback-control-selector") || error!=ERROR_INVALID_ADDRESS)++errors;
    RaiseException(0xe4470005,0,0,NULL);ExitProcess(91);
}
PVOID FASTCALL GetPModeVDMPointerAssert(DWORD address)
{
    ++resolutions;
    if(resolutions==1)
        expected_callback=((task.dwFlags&TDF_INITCALLBACKSTACK?task.vpStack:task.vpCBStack)-sizeof(CBVDMFRAME))&~1u;
    if(resolutions==fail_at)RaiseException(0xe4470002,0,0,NULL);
    switch(resolutions){
    case 1:if(address!=source_stack)++errors;return caller;
    case 2:
        if(address!=expected_callback)++errors;
        protect(caller,PAGE_NOACCESS);return outbound;
    case 3:if(address!=return_stack)++errors;return returned;
    default:++errors;RaiseException(0xe4470003,0,0,NULL);return NULL;
    }
}
void __cdecl host_simulate(void)
{
    fixture_old_simulate();
    if(fail_at==4){
        cs_value=0x3347;pe_value=0;vm_value=1;cpl_value=0;shared_change=0xabcddcba;
        if(variant==2)task.dwFlags|=TDF_IGNOREINPUT|TDF_INITCALLBACKSTACK;
        if(variant==3)active=FALSE;
        SetLastError(0x7281);RaiseException(0xe4470002,0,0,NULL);
    }
}
static void reset_abort(BOOL reserved)
{
    protect(caller,PAGE_READWRITE);protect(outbound,PAGE_READWRITE);
    ZeroMemory(&task,sizeof(task));ZeroMemory(caller,sizeof(*caller));
    ZeroMemory(outbound,sizeof(*outbound));ZeroMemory(returned,sizeof(*returned));
    task.vpStack=source_stack;task.htask16=0x1237;
    task.vpCBStack=reserved?source_stack-0x100:0;
    task.dwFlags=reserved?TDF_FORCETASKEXIT:TDF_INITCALLBACKSTACK;
    caller->wTDB=task.htask16;caller->wLocalBP=0x5678;
    resolutions=simulations=errors=variant=0;active=TRUE;shared_change=0;
    ss_value=HIWORD(source_stack);sp_value=LOWORD(source_stack);ip_value=0x1111;cs_value=0x2467;
    pe_value=1;vm_value=0;cpl_value=3;
}
static int abort_filter(DWORD code)
{
    if(code!=0xe4470002 && code!=0xe4470005)return EXCEPTION_CONTINUE_SEARCH;
    if(fail_at==1){
        /* A native first-pass modal filter can service another task before
         * this original call's finally, even before its first resolver ends. */
        cs_value=0x3347;ip_value=0x9999;pe_value=0;vm_value=1;cpl_value=0;
        ss_value=HIWORD(return_stack);sp_value=LOWORD(return_stack);
        shared_change=0xabcddcba;SetLastError(0x7281);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
static int abort_case(const char *name)
{
    BOOL reserved=strstr(name,"reserved")!=NULL;
    VPVOID old_cursor;DWORD old_flags,output=0xdeadbeef,exception=0;PARM16 args={0};
    reset_abort(reserved);old_cursor=task.vpCBStack;old_flags=task.dwFlags;
    fail_at=strstr(name,"first")?1u:strstr(name,"prep")?2u:strstr(name,"return")?3u:4u;
    if(!strcmp(name,"teardown"))variant=2;
    if(!strcmp(name,"terminal"))variant=3;
    if(!strcmp(name,"selector"))variant=4;
    __try {if(!CallBack16(RET_ENUMWINDOWPROC,&args,0x15070020,&output))++errors;}
    __except(abort_filter(GetExceptionCode())){exception=GetExceptionCode();}
    if(variant==3){
        void *inaccessible=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_NOACCESS);
        if(!inaccessible)goto wrong;
        /* Terminal check must precede even reading the control record. */
        if(wow_callback_control_restore(inaccessible,source_stack))goto wrong;
        VirtualFree(inaccessible,0,MEM_RELEASE);
        if(exception!=0xe4470002 || active || ip_value!=0x9999 || cs_value!=0x3347 ||
                pe_value || vm_value!=1 || cpl_value || task.vpCBStack!=expected_callback)goto wrong;
    }else if(variant==4){
        if(exception!=0xe4470005 || output!=0xdeadbeef || errors)goto wrong;
    }else{
        DWORD expected_flags=variant==2?old_flags|TDF_IGNOREINPUT|TDF_INITCALLBACKSTACK:old_flags;
        if(exception!=0xe4470002 || errors || output!=0xdeadbeef ||
                task.vpStack!=source_stack || task.vpCBStack!=old_cursor || task.dwFlags!=expected_flags ||
                ss_value!=HIWORD(source_stack) || sp_value!=LOWORD(source_stack) || ip_value!=0x1111 ||
                cs_value!=0x2467 || pe_value!=1 || vm_value || cpl_value!=3 ||
                ((fail_at==1 || fail_at==4) && (shared_change!=0xabcddcba || GetLastError()!=0x7281)))goto wrong;
    }
    printf("CALLBACK_ABORT_OK name=%s resolve=%u simulate=%u\n",name,resolutions,simulations);return 0;
wrong:
    printf("CALLBACK_ABORT_WRONG name=%s logical=%08lx cursor=%08lx flags=%08lx cpu=%04x:%04x code=%04x:%04x mode=%ld/%ld/%ld exception=%08lx errors=%u\n",
        name,task.vpStack,task.vpCBStack,task.dwFlags,ss_value,sp_value,cs_value,ip_value,
        (long)pe_value,(long)vm_value,(long)cpl_value,exception,errors);return 1;
}
int __cdecl main(int argc,char **argv)
{
    int result=0;unsigned i;
    const char *cases[]={"first-init","first-reserved","prep-init","prep-reserved","return-init","return-reserved",
        "live-init","live-reserved","teardown","terminal","selector"};
    SetEnvironmentVariableA("MVDM_WOW_CALLBACK_FRAME_TRACE_PATH",NULL);
    if(argc!=2 || !strcmp(argv[1],"--verify"))
        return fixture_original_four();
    caller=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    outbound=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    returned=VirtualAlloc(NULL,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!caller || !outbound || !returned)return 1;
    teb.WOW32Reserved=&task;
    if(!strcmp(argv[1],"--verify-abort"))
        for(i=0;i<ARRAYSIZE(cases);++i)result|=abort_case(cases[i]);
    else if(!strncmp(argv[1],"--abort-",8))result=abort_case(argv[1]+8);
    else result=2;
    VirtualFree(caller,0,MEM_RELEASE);VirtualFree(outbound,0,MEM_RELEASE);VirtualFree(returned,0,MEM_RELEASE);
    return result;
}
