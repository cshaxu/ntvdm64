/* Actual CCPU40 execution and original TLS/interface owners. The original
 * files are included in this test TU only to inspect their private state;
 * production exposes no ThreadSimBuf layout or diagnostic getters. */
#ifndef WOW_TEST_XTRN_SOURCE
#define WOW_TEST_XTRN_SOURCE "../../src/mvdm/softpc.new/base/ccpu386/c_xtrn.c"
#endif
#include WOW_TEST_XTRN_SOURCE
#include <string.h>
#include "ntthread.h"
#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_ccpu_wait.h"
#include "wow32-dll/include/wow_callback_frame_lease.h"
extern int fixture_simulation_level(void);
extern void fixture_thread_exit(void);
#define level fixture_simulation_level

extern void *setup_global_data_ptr(void);
extern void setup_vga_globals(void),sas_init(IU32),sas_term(void);
extern void c_cpu_init(void),c_cpu_simulate(void),c_cpu_continue(void);
extern void load_sw_cpu_access_functions(void);
extern unsigned char *c_GetPhyAdd(IU32);
extern void c_setIP(IU16),c_setSP(IU16),c_setCR3(IU32);
extern void c_setGDT_BASE_LIMIT(IU32,IU16);
extern IU16 c_getIP(void),c_getSP(void),c_getCS(void),c_getSS(void);
extern IU32 c_getGDT_BASE(void),c_getCR3(void);
extern ISM32 c_setCS(IU16),c_setSS(IU16);
extern void c_setPE(ISM32),c_setVM(ISM32),c_setCPL(ISM32);
extern ISM32 c_getPE(void),c_getVM(void),c_getCPL(void);
extern void (*BIOS[])(void);
static session owner;
static unsigned errors,continued;
/* The retained full host fixture closure deliberately supplies fprt.c's
 * debugger-routed printf/puts. Evidence must use the owned stdout HANDLE,
 * not assume those original providers are the CRT console functions. */
static int fixture_report(const char *text)
{
    DWORD written,error=GetLastError();HANDLE output=GetStdHandle(STD_OUTPUT_HANDLE);
    BOOL ok=WriteFile(output,text,(DWORD)lstrlenA(text),&written,NULL) &&
        WriteFile(output,"\r\n",2,&written,NULL);
    SetLastError(error);if(!ok)ExitProcess(95);return 0;
}
#define puts fixture_report
BOOL WINAPI wow_user_worker_active(void) {return TRUE;}
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage,DWORD error)
{
    fprintf(stderr,"UNEXPECTED_SELECTOR_FAILURE %s %lu\n",stage,error);ExitProcess(93);
}
static void raise_bop(void)
{
    SetLastError(0x7281);RaiseException(0xe4470006,0,0,NULL);
}
static void nested_bop(void)
{
    DWORD code=0;
    c_setIP(0x8100);
    __try {c_cpu_simulate();}
    __except(GetExceptionCode()==0xe4470006?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){code=GetExceptionCode();}
    /* Refuse to jump to an abandoned slot in the old-body negative. */
    if(code!=0xe4470006 || level()!=1 || GetLastError()!=0x7281){
        puts("CCPU_ABORT_WRONG nested-live-level");ExitProcess(1);
    }
    ++continued;c_setIP(0x8002);c_cpu_continue();
}
static void real_mode(void)
{
    c_setPE(0);c_setVM(0);c_setCPL(0);
    if(c_setCS(0) || c_setSS(0))++errors;
    c_setSP(0xf000);
}
static int simulate_cases(void)
{
    DWORD code=0;
    real_mode();
    c_GetPhyAdd(0x8000)[0]=0xd6;c_GetPhyAdd(0x8000)[1]=6;
    c_GetPhyAdd(0x8000)[2]=0xd6;c_GetPhyAdd(0x8000)[3]=0xfe;
    c_GetPhyAdd(0x8100)[0]=0xd6;c_GetPhyAdd(0x8100)[1]=6;
    BIOS[6]=raise_bop;BIOS[5]=nested_bop;c_setIP(0x8000);
    __try {c_cpu_simulate();}
    __except(GetExceptionCode()==0xe4470006?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){code=GetExceptionCode();}
    if(code!=0xe4470006 || level()!=0 || GetLastError()!=0x7281){
        puts("CCPU_ABORT_WRONG escaped-level");return 1;
    }
    c_GetPhyAdd(0x8000)[1]=5;c_setIP(0x8000);c_cpu_simulate();
    if(level()!=0 || continued!=1){puts("CCPU_ABORT_WRONG nested-continue");return 1;}
    /* Reuse the same level via normal FE after both kinds of escape. */
    c_setIP(0x8002);c_cpu_simulate();
    if(level()!=0 || errors)return 1;
    {
        char line[160];
        wsprintfA(line,"CCPU_ABORT_OK thread=%lu escaped=1 nested=1 continue=1 normal-fe=1",GetCurrentThreadId());
        fixture_report(line);
    }
    return 0;
}
static void interface_raise(ISM32 unused)
{(void)unused;raise_bop();}
static void interface_guest_fault(ISM32 unused)
{(void)unused;check_interface_active(13);++errors;}
static void interface_normal(ISM32 unused)
{(void)unused;}
static int interface_cases(void)
{
    DWORD code=0;
    __try {call_cpu_function((CALL_CPU *)interface_raise,TYPE_W,0,0);}
    __except(GetExceptionCode()==0xe4470006?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){code=GetExceptionCode();}
    if(code!=0xe4470006 || interface_active){puts("CCPU_INTERFACE_WRONG stale-interface");return 1;}
    if(call_cpu_function((CALL_CPU *)interface_guest_fault,TYPE_W,0,0)!=13 || interface_active ||
            call_cpu_function((CALL_CPU *)interface_normal,TYPE_W,0,0)!=0 || interface_active || errors)return 1;
    puts("CCPU_INTERFACE_OK native-unwind=1 architectural-error=13 normal-reuse=1");return 0;
}
static int control_cases(void)
{
    wow_callback_control saved;
    unsigned char gdt[]={0,0,0,0,0,0,0,0,0xff,0xff,0,0x20,0,0x9b,0,0,
        0xff,0xff,0,0x30,0,0x93,0,0};
    /* Actual protected selectors are reloaded from the current GDT, never
     * from a captured base/cache. Paging is off; CR3 is an unrelated sentinel. */
    CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1);
    c_setPE(1);c_setVM(0);c_setCPL(0);
    if(c_setCS(8) || c_setSS(16))return 1;
    c_setSP(0xf000);c_setIP(0x1234);wow_callback_control_capture(&saved);
    gdt[11]=0x40;gdt[19]=0x50;
    CopyMemory(c_GetPhyAdd(0x1800),gdt,sizeof(gdt));c_setGDT_BASE_LIMIT(0x1800,sizeof(gdt)-1);
    c_setCR3(0x9000);c_setPE(0);c_setVM(1);c_setCPL(3);c_setCS(0);c_setSS(0);c_setIP(0x9999);c_setSP(0x1111);
    SetLastError(0x7281);
    if(!wow_callback_control_restore(&saved,0x0010f000) || GetLastError()!=0x7281 ||
            c_getCS()!=8 || c_getSS()!=16 || c_getIP()!=0x1234 || c_getSP()!=0xf000 ||
            c_getPE()!=1 || c_getVM()!=0 || c_getCPL()!=0 || c_getGDT_BASE()!=0x1800 || c_getCR3()!=0x9000 ||
            GET_CS_BASE()!=0x4000 || GET_SS_BASE()!=0x5000)return 1;
    puts("CCPU_CONTROL_OK current-descriptors=1 modes=1 continuation=1 shared-cr3=1 last-error=1");return 0;
}
static DWORD WINAPI thread_cases(void *unused)
{
    int result;(void)unused;continued=errors=0;
    if(!session_thread_bind(&owner) || !mvdm_softpc_guest_memory_begin(&owner))return 2;
    ccpu386newthread();result=simulate_cases();
    fixture_thread_exit();
    mvdm_softpc_guest_memory_end(&owner);
    if(!session_thread_unbind(&owner))return 2;
    return result;
}
int main(int argc,char **argv)
{
    int result=0;unsigned i;
    session_initialize(&owner,447);
    if(!session_select_machine_backend(&owner,SESSION_MACHINE_BACKEND_SOFTPC) || !session_activate(&owner) ||
            !session_thread_bind(&owner) || !setup_global_data_ptr() || !mvdm_softpc_ccpu_wait_begin())return 2;
    sas_init(0x200000);
    if(!mvdm_softpc_guest_memory_begin(&owner))return 2;
    setup_vga_globals();load_sw_cpu_access_functions();c_cpu_init();
    if(argc==2 && !strcmp(argv[1],"--interface"))result=interface_cases();
    else if(argc==2 && !strcmp(argv[1],"--simulate"))result=simulate_cases();
    else{
        result=interface_cases()|simulate_cases()|control_cases();
        mvdm_softpc_guest_memory_end(&owner);session_thread_unbind(&owner);
        for(i=0;!result && i<2;++i){
            DWORD code=2;HANDLE thread=CreateThread(NULL,0,thread_cases,NULL,0,NULL);
            if(!thread || WaitForSingleObject(thread,10000)!=WAIT_OBJECT_0 || !GetExitCodeThread(thread,&code))result=2;
            else result=(int)code;
            if(thread)CloseHandle(thread);
        }
        if(!session_thread_bind(&owner) || !mvdm_softpc_guest_memory_begin(&owner))return 2;
    }
    mvdm_softpc_ccpu_wait_end();sas_term();mvdm_softpc_guest_memory_end(&owner);
    session_thread_unbind(&owner);session_dispose(&owner);
    return result;
}
