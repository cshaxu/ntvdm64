#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <ios.h>
#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_ccpu_wait.h"

/* Formal original CCPU/SAS providers, not replacement decode/reset bodies. */
extern void *setup_global_data_ptr(void);
extern void setup_vga_globals(void);
extern void sas_init(uint32_t);
extern void sas_term(void);
extern void c_cpu_init(void);
extern void c_cpu_simulate(void);
extern void c_cpu_interrupt(int, unsigned short);
extern uint32_t c_cpu_q_ev_get_count(void);
extern void c_setIP(unsigned short);
extern void c_setSI(unsigned short);
extern ULONG c_getEAX(void), c_getEFLAGS(void);
extern void c_setEFLAGS(ULONG);
extern unsigned short c_getIP(void);
extern unsigned short c_getAX(void);
extern unsigned short c_getBX(void);
extern void c_setBX(unsigned short);
extern void c_sas_stores(uint32_t, unsigned char *, uint32_t);
extern unsigned char *c_GetPhyAdd(uint32_t);
extern int debug_exception_pending;
extern BOOL doing_contributory;
extern void ica0_init(void), ica1_init(void);
extern void InitializeIcaLock(void);
extern int ica_intack(uint32_t *hook);
extern uint32_t CCPU_DR[8], CCPU_GR[8];
extern void MOV_DR(uint32_t, uint32_t);
extern void ccpu_restore_debug_context(const uint32_t *, BOOL);
extern void c_setGDT_BASE_LIMIT(uint32_t, uint16_t);
extern void c_setLDT_SELECTOR(uint16_t), c_setLDT_BASE_LIMIT(uint32_t, uint32_t);
extern void c_setIDT_BASE_LIMIT(uint32_t, uint16_t);
extern void c_setTR_SELECTOR(uint16_t);
extern void c_setTR_BASE_LIMIT_AR(uint32_t, uint32_t, uint16_t);
extern int c_setCS(uint16_t);
extern int c_setSS(uint16_t);
extern int c_setDS(uint16_t);
extern int c_setES(uint16_t);
extern int c_setFS(uint16_t);
extern unsigned short c_getBP(void);
extern void c_setSP(uint16_t);
extern void c_setCR3(uint32_t);
extern void c_setCR0(uint32_t);
extern uint32_t c_getCR0(void);
extern void c_setPG(int);
extern void c_setPE(int);
extern void c_sas_enable_20_bit_wrapping(void),c_sas_disable_20_bit_wrapping(void);
extern BOOL mvdm_debugger_read_debug_registers(USHORT, USHORT, PULONG);

static void report(const char *text)
{
    DWORD written;
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), text, lstrlenA(text), &written, NULL);
}

extern void c_setCPL(int), c_setIOPL(int), c_setIF(int);
extern uint32_t c_getEIP(void);
extern void ica_outb(uint32_t, unsigned char), ica_hw_interrupt(uint32_t,uint32_t,int);
extern long VdmInstallFaultHandler(void *), VdmInstallHardwareIntHandler(void *);
extern unsigned long DelayIrqLine;
extern void OUT8(uint32_t,uint32_t);
extern void ica_inb(uint32_t,unsigned char *);
extern BOOL host_exint_hook(long,long);
extern unsigned char c_getAL(void);
static int test_case;
extern void do_intrupt(unsigned short, BOOL, BOOL, unsigned short);
static volatile uint32_t test_fault, test_irq;
static volatile uint32_t frame_fault_eip, test_fault_vector, test_fault_error;
extern uint32_t lin2phy(uint32_t,int);
extern int xtrn2phy(uint32_t,unsigned char,uint32_t *);
static int duplicate_tlb_mapping(void) {
    uint32_t *pte=(uint32_t *)c_GetPhyAdd(0x5000),first,second,actual,monitor=0;
    int hit;
    c_cpu_init(); ZeroMemory(c_GetPhyAdd(0x4000),0x2000);
    *(uint32_t *)c_GetPhyAdd(0x4000)=0x5007; pte[2]=0x8007;
    c_setCR3(0x4000);c_setPG(1);
    first=lin2phy(0x2000,2); pte[2]=0x9007;
    second=lin2phy(0x2000,3);actual=lin2phy(0x2000,2);
    hit=xtrn2phy(0x2000,2,&monitor);
    {char line[160];wsprintfA(line,"TLB_DUPLICATE first=%lx write=%lx cpu=%lx monitor=%lx hit=%d\n",first,second,actual,monitor,hit);report(line);}
    c_setPG(0);
    return first==0x8000 && second==0x9000 && actual==0x9000 && hit==1 && monitor==actual;
}
static int monitor_cache_contract(void) {
    uint32_t *pte=(uint32_t *)c_GetPhyAdd(0x5000),phys=0;
    int read,hit,deny,write,absent;
    c_cpu_init();ZeroMemory(c_GetPhyAdd(0x4000),0x2000);
    *(uint32_t *)c_GetPhyAdd(0x4000)=0x5003;pte[2]=0x8003;
    c_setCR3(0x4000);c_setPG(1);
    read=xtrn2phy(0x2000,0,&phys) && phys==0x8000;
    if(lin2phy(0x2000,0)!=0x8000){c_setPG(0);return 0;}
    pte[2]=0x9007;*(uint32_t *)c_GetPhyAdd(0x4000)=0;
    hit=xtrn2phy(0x2000,0,&phys) && phys==0x8000;
    deny=!xtrn2phy(0x2000,2,&phys);
    *(uint32_t *)c_GetPhyAdd(0x4000)=0x5003;
    write=xtrn2phy(0x2000,1,&phys) && phys==0x9000;
    absent=!xtrn2phy(0x3000,0,&phys);
    c_setPG(0);
    {char line[128];wsprintfA(line,"CACHE_CONTRACT read=%d stale_hit=%d cached_deny=%d write_miss=%d absent=%d\n",read,hit,deny,write,absent);report(line);}
    return read && hit && deny && write && absent;
}

static unsigned io_reads, wrong_string_write;
static void probe_read(io_addr port, half_word *value) { (void)port; ++io_reads; *value=0xa5; }

static BOOL test_sti_fault(unsigned long vector, unsigned long error) {
    test_fault_vector=vector; test_fault_error=error; ++test_fault; return FALSE;
}
extern void c_setEIP(uint32_t);
extern void c_setRF(IBOOL);
extern IBOOL c_getCF(void);
extern BOOL ThreadSetDebugContext(PULONG);
extern void DpmiSetDebugRegisters(void);
static void debug_broadcast_out(io_addr port, half_word value) {
    ULONG registers[6]={0xf1002,0,0,0,0,1};
    (void)port; (void)value;
    if (test_case>=114) DpmiSetDebugRegisters();
    else if (!ThreadSetDebugContext(registers)) ++wrong_string_write;
}

static BOOL probe_debug_fault(unsigned long vector, unsigned long error) {
    (void)error;if(vector!=1)return FALSE;
    ++test_fault;frame_fault_eip=c_getEIP();c_setEIP(test_case==116 ? 0x1004 : test_case>=113 ? 0x1003 : test_case>=109 ? 0x1001 : test_case==47 ? 0x101d : test_case==46 ? 0x101a : 0x1019);return TRUE;
}
static int restored_debug_execution(void) {
    unsigned char gdt[40]={0}, program[]={0x43,0xb8,0xef,0xbe,0xd6,0xfe};
    uint32_t registers[6]={0xf1000,0,0,0,0,0};
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    c_cpu_init();CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));
    CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);
    c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);c_setSP(0x800);
    c_setBX(0);c_setEIP(0x1000);test_fault=frame_fault_eip=0;
    if(test_case==109 || test_case==111 || test_case==117) registers[5]=1;
    if(test_case==111) c_setRF(1);
    ccpu_restore_debug_context(registers,test_case==110);
    if(test_case==117) {
        const unsigned char reset_program[]={0xb8,0xef,0xbe,0xd6,0xfe};
        CopyMemory(c_GetPhyAdd(0xffff0),reset_program,sizeof(reset_program));
        c_cpu_interrupt(0,0);
    }
    VdmInstallFaultHandler((void *)probe_debug_fault);
    c_cpu_simulate();VdmInstallFaultHandler(NULL);MOV_DR(7,0);
    {char line[120];wsprintfA(line,"RESTORED_DEBUG case=%d fault=%lu eip=%lx bx=%04x\n",test_case,test_fault,frame_fault_eip,c_getBX());report(line);}
    if(test_case==111 || test_case==112)
        return test_fault==0 && c_getBX()==1 && c_getAX()==0xbeef;
    if(test_case==117) return !test_fault && c_getBX()==0 && c_getAX()==0xbeef;
    return test_fault==1 && frame_fault_eip==0x1000 && c_getBX()==0 && c_getAX()==0xbeef;
}
static int broadcast_debug_execution(void) {
    unsigned char gdt[40]={0}, program[]={0xe6,0x80,0x43,0xb8,0xef,0xbe,0xd6,0xfe};
    half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;
    c_cpu_init();io_init();io_define_outb(adapter,debug_broadcast_out);
    io_connect_port((io_addr)0x80,adapter,IO_WRITE);
    if(test_case==116) {
        const unsigned char bop[]={0xd6,0x53,0x0e,0x43,0xb8,0xef,0xbe,0xd6,0xfe};
        CopyMemory(c_GetPhyAdd(0xf1000),bop,sizeof(bop));
    }
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x00009a0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x00009300u;
    CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));
    if(test_case!=116) CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1);c_setPE(1);c_setCPL(0);
    c_setCS(0x18);c_setSS(0x20);c_setDS(0x20);c_setSP(0x800);
    if(test_case>=114) {
        ULONG registers[6]={0xf1002,0,0,0,0,1};
        if(test_case==116) registers[0]=0xf1003;
        CopyMemory(c_GetPhyAdd(0x300),registers,sizeof(registers));
        c_setSI(test_case==115 ? 0xfff0 : 0x300);
    }
    c_setEIP(0x1000);c_setBX(0);
    test_fault=frame_fault_eip=wrong_string_write=0;
    VdmInstallFaultHandler((void *)probe_debug_fault);
    c_cpu_simulate();VdmInstallFaultHandler(NULL);MOV_DR(7,0);
    {char line[120];wsprintfA(line,"BROADCAST_DEBUG fault=%lu eip=%lx bx=%04x\n",test_fault,frame_fault_eip,c_getBX());report(line);}
    if(test_case==115) return !wrong_string_write && !test_fault && c_getBX()==1 && c_getCF();
    if(test_case==116) return !test_fault ? 0 : frame_fault_eip==0x1003 && c_getBX()==0;
    return !wrong_string_write && test_fault==1 && frame_fault_eip==0x1002 && c_getBX()==0;
}

extern unsigned short c_getSI(void),c_getDI(void),c_getCX(void);

/* NT4 VdmUserCr0MapIn/Out preserve each guest-visible EM/MP pair. */

/* Original host instruction reads set accessed on a page-table walk. */

/* NT4 increments the linear instruction pointer modulo 32 bits. */

/* OpcodeGenericPrefix permits instruction length through 128 before dispatch. */

/* A code selector's translated base does not establish a linear host alias
 * for an independently remapped later code page. */
static int dpmi_remapped_code_page(void) {
    unsigned char ldt[40]={0}; ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000);
    unsigned j, mode=test_case>=99 ? test_case-6 : test_case>=96 ? test_case-3 : test_case; BOOL handled;
    ULONG expected_cr0;
    BOOL negative=(test_case>=104 && test_case<=106);
    BOOL cr_case=(test_case>=150 && test_case<=153) || test_case==155 || test_case==156;
    BOOL cr_write=(test_case==155 || test_case==156);
    extern ULONG Cpu40LdtShadowAddress, IntelBase;
    extern LDT_ENTRY *Ldt;
    extern USHORT DosxRmCodeSelector;
    extern BOOL DpmiEmulateInstruction(void), DpmiFaultHandler(ULONG, ULONG);
    extern void c_setEAX(ULONG);
    if (cr_case) { mode=(test_case & 1) ? 95 : 93; negative=test_case>=152 && !cr_write; }
    if (negative && !cr_case) mode=test_case==104 ? 93 : test_case==105 ? 94 : 95;
    c_cpu_init(); *(uint32_t *)(ldt+0x18)=0x0000ffffu; *(uint32_t *)(ldt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(ldt+0x20)=0x0000ffffu; *(uint32_t *)(ldt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x3000),ldt,sizeof(ldt)); ZeroMemory(c_GetPhyAdd(0x4000),0x2000);
    *(ULONG *)c_GetPhyAdd(0x4000)=0x5007; for(j=0;j<512;j++)pte[j]=(j<<12)|7;
    pte[0xf1]=mode==94 ? 0xf1007 : 0x6007; *c_GetPhyAdd(0xf1000)=0x90;
    if(mode==95) { *c_GetPhyAdd(0xf0fff)=0x8e; *c_GetPhyAdd(0x6000)=0xe0; }
    else { unsigned long code=mode==94 ? 0xf1000 : 0x6000;
        *c_GetPhyAdd(code)=0x8e; *c_GetPhyAdd(code+1)=0xe0; }
    if(negative) {
        if(mode==95) { *c_GetPhyAdd(0x6000)=0x90; *c_GetPhyAdd(0xf1000)=0xe0; }
        else { *c_GetPhyAdd(mode==94 ? 0xf1000 : 0x6000)=0x90;
            if(mode==93) { *c_GetPhyAdd(0xf1000)=0x8e; *c_GetPhyAdd(0xf1001)=0xe0; } }
    }
    if (cr_case) {
        unsigned long first=mode==95 ? 0xf0fff : 0x6000;
        unsigned long second=mode==95 ? 0x6000 : 0x6001;
        unsigned long third=mode==95 ? 0x6001 : 0x6002;
        *c_GetPhyAdd(first)=0x0f;
        *c_GetPhyAdd(second)=negative ? 0x21 : (cr_write ? 0x22 : 0x20);
        *c_GetPhyAdd(third)=0xc0;
        if (mode==93) {
            *c_GetPhyAdd(0xf1000)=0x0f; *c_GetPhyAdd(0xf1001)=0x20;
            *c_GetPhyAdd(0xf1002)=0xc0;
        } else { *c_GetPhyAdd(0xf1000)=0x20; *c_GetPhyAdd(0xf1001)=0xc0; }
    }
    c_setLDT_SELECTOR(0x28); c_setLDT_BASE_LIMIT(0x3000,sizeof(ldt)-1);
    c_setPE(1); c_setCPL(3); c_setCS(0x1f); c_setSS(0x27); c_setIOPL(0); c_setEIP(mode==95 ? 0xfff : 0x1000);
    IntelBase=(ULONG)(ULONG_PTR)c_GetPhyAdd(0); Ldt=(LDT_ENTRY *)c_GetPhyAdd(0x3000);
    Cpu40LdtShadowAddress=0x3000; DosxRmCodeSelector=0x1f;
    c_setCR3(0x4000); c_setPG(1); expected_cr0=c_getCR0();
    if(cr_write) c_setEAX(expected_cr0 ^ 4u);
    if(test_case>=99 && !negative && !cr_case) {
        VdmInstallFaultHandler((void *)DpmiFaultHandler);
        handled=host_exint_hook(13,0); VdmInstallFaultHandler(NULL);
    } else handled=test_case>=96 && !negative && !cr_case ? DpmiFaultHandler(13,0) : DpmiEmulateInstruction();
    if(cr_write && c_getCR0()!=(expected_cr0 ^ 4u)) { c_setPG(0); return 0; }
    c_setPG(0);
    {char line[120];wsprintfA(line,"DPMI_REMAPPED_CODE handled=%d eip=%08lx\n",handled,c_getEIP());report(line);}
    if(negative) return !handled && c_getEIP()==(mode==95 ? 0xfff : 0x1000);
    if(cr_case && !cr_write && c_getEAX()!=expected_cr0) return 0;
    return handled && c_getEIP()==(mode==95 ? (cr_case ? 0x1002 : 0x1001) : (cr_case ? 0x1003 : 0x1002));
}
/* A missing second page must not combine the corrected first opcode with
 * operands taken from the older contiguous SIM32 alias. */
static int dpmi_remapped_decode_fallback(void) {
    unsigned char ldt[40]={0}; ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000);
    unsigned j; BOOL handled;
    extern ULONG Cpu40LdtShadowAddress, IntelBase;
    extern LDT_ENTRY *Ldt;
    extern USHORT DosxRmCodeSelector;
    extern BOOL DpmiEmulateInstruction(void);
    c_cpu_init(); *(uint32_t *)(ldt+0x18)=0x0000ffffu; *(uint32_t *)(ldt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(ldt+0x20)=0x0000ffffu; *(uint32_t *)(ldt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x3000),ldt,sizeof(ldt)); ZeroMemory(c_GetPhyAdd(0x4000),0x2000);
    *(ULONG *)c_GetPhyAdd(0x4000)=0x5007; for(j=0;j<512;j++) pte[j]=(j<<12)|7;
    pte[0xf1]=0x6007; pte[0xf2]=0;
    *c_GetPhyAdd(0x6fff)=0x0f; *c_GetPhyAdd(0xf1fff)=0x90;
    *c_GetPhyAdd(0xf2000)=0x20; *c_GetPhyAdd(0xf2001)=0xc0;
    c_setLDT_SELECTOR(0x28); c_setLDT_BASE_LIMIT(0x3000,sizeof(ldt)-1);
    c_setPE(1); c_setCPL(3); c_setCS(0x1f); c_setSS(0x27); c_setIOPL(0); c_setEIP(0x1fff);
    IntelBase=(ULONG)(ULONG_PTR)c_GetPhyAdd(0); Ldt=(LDT_ENTRY *)c_GetPhyAdd(0x3000);
    Cpu40LdtShadowAddress=0x3000; DosxRmCodeSelector=0x1f;
    c_setCR3(0x4000); c_setPG(1); handled=DpmiEmulateInstruction(); c_setPG(0);
    report(handled ? "DPMI_DECODE_FALLBACK mixed-path\n" : "DPMI_DECODE_FALLBACK original-path\n");
    return !handled && c_getEIP()==0x1fff;
}
/* A changed descriptor selects a nonpresent decoder page after the CPU has
 * fetched the original #UD instruction through its cached CS descriptor. */
static BOOL dpmi_decode_page_fault(unsigned long vector, unsigned long error) {
    extern BOOL DpmiFaultHandler(ULONG, ULONG);
    if(vector==6 || (test_case==108 && vector==13)) {
        BOOL handled=DpmiFaultHandler(vector,error);
        if(!handled) c_setEIP(test_case==108 ? c_getEIP()+1 : 0x1002);
        return TRUE;
    }
    if(vector==14) {
        char line[140];
        ++test_fault;
        wsprintfA(line,"DPMI_NESTED_FETCH vector=%lu eip=%08lx flags=%08lx\n",vector,c_getEIP(),c_getEFLAGS());
        report(line);
        c_setEIP(test_case==108 ? c_getEIP()+1 : 0x1002);
        return TRUE;
    }
    if(test_case==108 && vector==8) {
        ++test_irq;
        report("DPMI_NESTED_SECOND vector=8\n");
        c_setEIP(0x1002);
        return TRUE;
    }
    return FALSE;
}
static int dpmi_faulting_decode_page(void) {
    unsigned char gdt[40]={0}, program[]={0x0f,0x0b,0xb8,0xef,0xbe,0xd6,0xfe,0};
    ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000); unsigned i;
    extern ULONG Cpu40LdtShadowAddress, IntelBase;
    extern LDT_ENTRY *Ldt;
    extern BOOL doing_contributory;
    c_cpu_init();
    if(test_case==108) { program[0]=0xec; program[1]=0xec; program[2]=0xb8;
        program[3]=0xef; program[4]=0xbe; program[5]=0xd6; program[6]=0xfe; }
    *(uint32_t *)(gdt+0x18)=0x0000ffffu; *(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu; *(uint32_t *)(gdt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt));
    CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    ZeroMemory(c_GetPhyAdd(0x4000),0x2000); *(ULONG *)c_GetPhyAdd(0x4000)=0x5007;
    for(i=0;i<512;i++)pte[i]=(i<<12)|7;
    pte[0x21]=0;
    c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1); c_setPE(1); c_setCPL(3);
    c_setCS(0x1b); c_setSS(0x23); c_setIOPL(0); c_setIF(1); c_setEIP(0x1000);
    *(uint32_t *)c_GetPhyAdd(0x301c)=0x0000fa02u;
    IntelBase=(ULONG)(ULONG_PTR)c_GetPhyAdd(0); Cpu40LdtShadowAddress=0x3000;
    Ldt=(LDT_ENTRY *)c_GetPhyAdd(0x3000);
    c_setCR3(0x4000); c_setPG(1); test_fault=0; test_irq=0;
    VdmInstallFaultHandler((void *)dpmi_decode_page_fault);
    c_cpu_simulate(); VdmInstallFaultHandler(NULL); c_setPG(0);
    {char line[150];wsprintfA(line,"DPMI_NESTED_RESULT faults=%lu doubles=%lu ax=%04x flags=%08lx contributory=%d\n",test_fault,test_irq,c_getAX(),c_getEFLAGS(),doing_contributory);report(line);}
    return c_getAX()==0xbeef;
}
/* The NT4 protected dispatcher is not the V86 #GP dispatcher. */
static int monitor_v86_mode_boundary(void) {
    unsigned char ldt[40]={0}; ULONG before_flags; int handled; extern void c_setVM(int);
    c_cpu_init(); InitializeIcaLock(); ica0_init(); ica1_init();
    *(uint32_t *)(ldt+0x18)=0x0000ffffu; *(uint32_t *)(ldt+0x1c)=0x0000fa01u;
    CopyMemory(c_GetPhyAdd(0x3000),ldt,sizeof(ldt));
    c_setLDT_SELECTOR(0x28); c_setLDT_BASE_LIMIT(0x3000,sizeof(ldt)-1);
    c_setPE(1); c_setVM(1); c_setCPL(3);
    c_setCS(0x1f); c_setIP(0x0100); c_setIOPL(0); c_setIF(1);
    *c_GetPhyAdd(0x2f0)=test_case==102 ? 0xfa : 0xfb;
    *c_GetPhyAdd(0x10100)=test_case==102 ? 0xfa : 0xfb;
    *(ULONG *)c_GetPhyAdd(0x714)=0x200; before_flags=c_getEFLAGS(); test_fault=0;
    VdmInstallFaultHandler((void *)test_sti_fault); handled=host_exint_hook(13,0);
    VdmInstallFaultHandler(NULL);
    {char line[150];wsprintfA(line,"MONITOR_V86 case=%d handled=%d fault=%lu ip=%lx flags=%lx state=%lx\n",test_case,handled,test_fault,c_getEIP(),c_getEFLAGS(),*(ULONG *)c_GetPhyAdd(0x714));report(line);}
    return !handled && test_fault==1 && c_getEIP()==0x100 &&
        c_getEFLAGS()==before_flags && *(ULONG *)c_GetPhyAdd(0x714)==0x200;
}
extern void BlockWOWIdle(BOOL);

#pragma pack(push,1)
typedef struct { USHORT cs, flags; ULONG eip; } SWINT_HANDLER;
#pragma pack(pop)
extern SWINT_HANDLER DpmiInterruptHandlers[256];
typedef struct { USHORT cs, ss; ULONG eip, esp, flags; } FAULT_HANDLER;
extern FAULT_HANDLER DpmiFaultHandlers[32];
extern USHORT LockedPMStackSel;
extern ULONG LockedPMStackCount;
extern LDT_ENTRY *Ldt;
extern ULONG IntelBase, Cpu40LdtShadowAddress;
extern USHORT CurrentAppFlags;
extern BOOL DpmiSwIntHandler(ULONG);
extern BOOL host_swint_hook(long);
extern long VdmInstallSoftwareIntHandler(void *);
extern void c_setVM(int), c_setEAX(ULONG), c_setEBP(ULONG), c_setEBX(ULONG);
extern USHORT c_getCS(void), c_getSS(void), c_getSP(void);
extern void INTx(ULONG);
extern void ZFRSRVD(ULONG);
static BOOL swint_origin_probe(ULONG vector) {test_irq=vector; return TRUE;}

/* The original hook delegates to its registered fault owner. The CPU
 * retains guest IDT and I/O bitmap enforcement when the fault is declined. */
static int protected_monitor_ownership(void) {
    unsigned char gdt[40]={0};
    const unsigned char gate[8]={0x00,0x11,0x1b,0x00,0x00,0x86,0x00,0x00};
    unsigned char faulting[]={0xe4,0x80,0xb8,0xad,0xde,0xd6,0xfe};
    const unsigned char target[]={0xbb,0xef,0xbe,0xd6,0xfe};
    const int adapter=23;
    BOOL handled,ok; char line[160];
    c_cpu_init();io_init();io_define_inb(adapter,probe_read);
    io_connect_port((io_addr)(test_case>=171 ? 0x60 : 0x80),adapter,IO_READ_WRITE);
    if(test_case>=171)faulting[1]=0x60;
    *(ULONG *)(gdt+0x18)=0x0000ffffu;*(ULONG *)(gdt+0x1c)=0x0000fa0fu;
    *(ULONG *)(gdt+0x20)=0x0000ffffu;*(ULONG *)(gdt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));
    CopyMemory(c_GetPhyAdd(0xf1000),faulting,sizeof(faulting));
    CopyMemory(c_GetPhyAdd(0xf1100),target,sizeof(target));
    CopyMemory(c_GetPhyAdd(0x9000+13*8),gate,sizeof(gate));
    c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1);c_setIDT_BASE_LIMIT(0x9000,13*8+7);
    c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setSP(0x800);
    c_setIOPL(0);c_setIF(1);c_setIP(0x1000);c_setBX(0);c_setTR_SELECTOR(0);
    if(test_case>=171) {
        /* Match the observed Win3.1 boundary: an actual 386 TSS contains
         * the I/O bitmap and bit zero of its port-60 byte denies access. */
        ZeroMemory(c_GetPhyAdd(0xa000),0x200);
        *(USHORT *)c_GetPhyAdd(0xa066)=0x68;
        *c_GetPhyAdd(0xa074)=(test_case==171 ? 1 : 0);
        c_setTR_SELECTOR(0x28);c_setTR_BASE_LIMIT_AR(0xa000,0x1ff,0x8b);
    }
    io_reads=0;test_fault=0;VdmInstallFaultHandler(NULL);
    if(test_case==169) {
        /* Registration delegates the exception unchanged. It does not grant
         * an extra instruction emulator permission to consume the fault. */
        VdmInstallFaultHandler((void *)test_sti_fault);
        handled=host_exint_hook(13,0);
        ok=!handled && test_fault==1 && test_fault_vector==13 &&
            test_fault_error==0 && io_reads==0 && c_getEIP()==0x1000;
        VdmInstallFaultHandler(NULL);c_setIP(0x1000);io_reads=0;
        handled=host_exint_hook(13,0);
        ok=ok && !handled && io_reads==0 && c_getEIP()==0x1000;
    } else if(test_case==168) {
        handled=host_exint_hook(13,0);
        ok=!handled && io_reads==0 && c_getEIP()==0x1000;
    } else {
        /* Run the real CCPU fault path and require the guest IDT handler,
         * rather than the port device, to receive the denied instruction. */
        c_cpu_simulate();
        if(test_case==172)ok=c_getBX()==0 && io_reads==1 && c_getSP()==0x800;
        else ok=c_getBX()==0xbeef && io_reads==0 && c_getSP()==0x7f8;
    }
    wsprintfA(line,"PROTECTED_MONITOR_OWNERSHIP case=%d pass=%d reads=%u bx=%x sp=%x\n",
        test_case,ok,io_reads,c_getBX(),c_getSP());report(line);return ok;
}

static int protected_fault_decline_cpu(void) {
    unsigned char gdt[40]={0};
    const unsigned char gate[8]={0x00,0x11,0x1b,0x00,0x00,0x86,0x00,0x00};
    const unsigned char faulting[]={0xf7,0xf3,0xd6,0xfe};
    const unsigned char target[]={0xbb,0xef,0xbe,0xd6,0xfe};
    BOOL ok; char line[140];
    extern BOOL DpmiFaultHandler(ULONG,ULONG);
    c_cpu_init();
    *(ULONG *)(gdt+0x18)=0x0000ffffu; *(ULONG *)(gdt+0x1c)=0x0000fa0fu;
    *(ULONG *)(gdt+0x20)=0x0000ffffu; *(ULONG *)(gdt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));
    CopyMemory(c_GetPhyAdd(0x9000),gate,sizeof(gate));
    CopyMemory(c_GetPhyAdd(0xf1000),faulting,sizeof(faulting));
    CopyMemory(c_GetPhyAdd(0xf1100),target,sizeof(target));
    c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1); c_setIDT_BASE_LIMIT(0x9000,7);
    c_setPE(1); c_setCPL(3); c_setCS(0x1b); c_setSS(0x23); c_setSP(0x800);
    c_setIOPL(test_case==149 ? 0 : 3); c_setIF(1); c_setIP(0x1000); c_setBX(0); c_setEAX(1);
    Ldt=(LDT_ENTRY *)c_GetPhyAdd(0x3000); ZeroMemory(Ldt,8);
    DpmiFaultHandlers[0].cs=0;
    VdmInstallFaultHandler((void *)DpmiFaultHandler);
    c_cpu_simulate(); VdmInstallFaultHandler(NULL);
    ok=c_getBX()==0xbeef && c_getSP()==0x7fa &&
       (*(USHORT *)c_GetPhyAdd(0x7fe)&0x200)!=0;
    wsprintfA(line,"PROTECTED_FAULT_DECLINE_CPU case=%d pass=%d sp=%x saved_flags=%x bx=%x\n",
              test_case,ok,c_getSP(),*(USHORT *)c_GetPhyAdd(0x7fe),c_getBX());
    report(line); return ok;
}

static BOOL v86_unadmitted_fault_probe(ULONG vector, ULONG error) {
    (void)vector; (void)error; ++test_fault; return FALSE;
}
static int v86_explicit_software_int(void) {
    unsigned vector=(test_case==160 || test_case==162 || test_case==164 || test_case==166) ? 3 : 4;
    unsigned char code[]={0xcd,0,0xd6,0xfe};
    BOOL ok; char line[100];
    c_cpu_init(); c_setPE(1); c_setVM(1); c_setCPL(3);
    c_setCS(0x100); c_setSS(0x200); c_setSP(0x800); c_setIP(0x100);
    c_setIOPL(3); code[1]=(unsigned char)vector;
    if(test_case==164 || test_case==165) c_setIOPL(0);
    if(test_case>=162) {
        code[0]=vector==3 ? 0xcc : 0xce; code[1]=0xd6; code[2]=0xfe;
        if(vector==4) c_setEFLAGS(c_getEFLAGS() | 0x800u);
    }
    CopyMemory(c_GetPhyAdd(0x1100),code,sizeof(code));
    if(test_case>=166) {
        unsigned i; ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000);
        ZeroMemory(c_GetPhyAdd(0x4000),0x2000); *(ULONG *)c_GetPhyAdd(0x4000)=0x5007;
        for(i=0;i<512;i++) pte[i]=(i<<12)|7;
        c_setCR3(0x4000); c_setPG(1);
    }
    test_irq=0; test_fault=0; VdmInstallSoftwareIntHandler((void *)swint_origin_probe);
    if(test_case>=164) VdmInstallFaultHandler((void *)v86_unadmitted_fault_probe);
    c_cpu_simulate(); VdmInstallFaultHandler(NULL); VdmInstallSoftwareIntHandler(NULL);
    ok=test_fault==0 && test_irq==vector && c_getCS()==0x100 && c_getSP()==0x800;
    wsprintfA(line,"V86_EXPLICIT_SWINT case=%d pass=%d vector=%lu\n",test_case,ok,test_irq);
    report(line); return ok;
}

static int v86_a20_word_ram_control(void) {
    extern void c_sas_connect_memory(ULONG,ULONG,int);
    extern void spr_write_word(ULONG,USHORT);
    unsigned char *base=c_GetPhyAdd(0); char line[120];
    c_sas_connect_memory(0xff000,0xfffff,0);
    c_sas_enable_20_bit_wrapping();
    base[0xfffff]=0; base[0]=0x66; base[0x100000]=0x77;
    spr_write_word(0xfffff,0xa55a);
    wsprintfA(line,"A20_ODD_RAM fffff=%02x zero=%02x above=%02x\n",base[0xfffff],base[0],base[0x100000]);
    report(line); c_sas_disable_20_bit_wrapping();
    return base[0xfffff]==0x5a && base[0]==0xa5 && base[0x100000]==0x77;
}
static LONG v86_installed_exception(EXCEPTION_POINTERS *ep) {
    char line[180];
    wsprintfA(line,"V86_REAL_INSTALLED_EXCEPTION code=%lx at=%p access=%lu address=%p\n",
              ep->ExceptionRecord->ExceptionCode,ep->ExceptionRecord->ExceptionAddress,
              ep->ExceptionRecord->NumberParameters ? ep->ExceptionRecord->ExceptionInformation[0] : 0,
              ep->ExceptionRecord->NumberParameters > 1 ? (void *)ep->ExceptionRecord->ExceptionInformation[1] : NULL);
    report(line); return EXCEPTION_EXECUTE_HANDLER;
}
static int vdd_sas_unbound_control(void) {
    extern void c_sas_connect_memory(ULONG,ULONG,int);
    extern unsigned char spr_read_byte(ULONG);
    extern void spr_write_byte(ULONG,unsigned char);
    unsigned char before; char line[90];
    c_sas_connect_memory(0xd0000,0xd0fff,4);
    *c_GetPhyAdd(0xd0000)=0x5a;
    before=spr_read_byte(0xd0000);
    wsprintfA(line,"VDD_SAS_UNBOUND read=%02x\n",before); report(line);
    __try {spr_write_byte(0xd0000,0xa5); report("VDD_SAS_UNBOUND_WRITE_RETURNED\n");}
    __except(v86_installed_exception(GetExceptionInformation())) {return 1;}
    return 0;
}
static BOOL cursor_copy_fault(unsigned long vector, unsigned long error) {
    if(vector!=13 || error!=0)return FALSE;
    ++test_fault;frame_fault_eip=c_getEIP();c_setEIP(0x1010);return TRUE;
}
/* Execute the DIBENG 8-bit cursor-save inner loop through the original CPU.
 * The FS limit ends at the final byte, so an extra iteration cannot pass. */
static int protected_cursor_copy_loop(void) {
    unsigned char gdt[48]={0};
    unsigned char program[]={0xbd,0x30,0x00,0x67,0x26,0x8a,0x0e,
        0x64,0x88,0x0d,0x66,0x46,0x47,0x4d,0x75,0xf3,
        0xb8,0xef,0xbe,0xd6,0xfe};
    BOOL paged=(test_case==175 || test_case==176);
    BOOL overflow=(test_case==174 || test_case==176);
    unsigned i,destination=paged ? 0xff0 : 0x600;
    if(overflow)program[1]=49;
    c_cpu_init();
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    *(uint32_t *)(gdt+0x28)=destination+47;*(uint32_t *)(gdt+0x2c)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(paged ? 0x3000 : 0x1000),gdt,sizeof(gdt));
    CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    if(paged) {
        uint32_t *pte=(uint32_t *)c_GetPhyAdd(0x5000);
        ZeroMemory(c_GetPhyAdd(0x4000),0x2000);*(uint32_t *)c_GetPhyAdd(0x4000)=0x5007;
        for(i=0;i<512;i++)pte[i]=(i<<12)|7;pte[1]=0x9007;
    }
    for(i=0;i<48;i++){
        unsigned target=paged && i>=16 ? 0x9000+i-16 : destination+i;
        *c_GetPhyAdd(0x500+i)=(unsigned char)(i^0xa5);*c_GetPhyAdd(target)=0;
    }
    *c_GetPhyAdd(paged ? 0x9020 : 0x630)=0x7e;
    c_setGDT_BASE_LIMIT(paged ? 0x3000 : 0x1000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);
    c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);c_setES(0x23);c_setFS(0x2b);
    c_setSP(0x800);CCPU_GR[6]=0x500;CCPU_GR[7]=destination;c_setIP(0x1000);
    if(paged){c_setCR3(0x4000);c_setPG(1);}
    test_fault=frame_fault_eip=0;VdmInstallFaultHandler((void *)cursor_copy_fault);
    c_cpu_simulate();VdmInstallFaultHandler(NULL);if(paged)c_setPG(0);
    if((overflow ? (test_fault!=1 || frame_fault_eip!=0x1007 || c_getBP()!=1) :
        (test_fault!=0 || c_getBP()!=0)) || c_getAX()!=0xbeef || CCPU_GR[6]!=0x530 ||
       (CCPU_GR[7]&0xffff)!=destination+48 || *c_GetPhyAdd(paged ? 0x9020 : 0x630)!=0x7e)return 0;
    for(i=0;i<48;i++){
        unsigned target=paged && i>=16 ? 0x9000+i-16 : destination+i;
        if(*c_GetPhyAdd(target)!=(unsigned char)(i^0xa5))return 0;
    }
    return 1;
}
int main(int argc, char **argv) {
    session owner;
    int ok;
    test_case=argc>1 ? atoi(argv[1]) : -1;
    session_initialize(&owner, 412);
    if (!session_activate(&owner) || !session_thread_bind(&owner) || !setup_global_data_ptr()) return 2;
    if (!mvdm_softpc_ccpu_wait_begin()) return 3;
    sas_init(0x200000);
    if (!mvdm_softpc_guest_memory_begin(&owner)) return 4;
    setup_vga_globals();c_cpu_init();
    if(test_case>=173 && test_case<=176) ok=protected_cursor_copy_loop();
    else if(test_case>=168 && test_case<=172) ok=protected_monitor_ownership();
    else if(test_case==148 || test_case==149) ok=protected_fault_decline_cpu();
    else if(test_case>=160 && test_case<=167) ok=v86_explicit_software_int();
    else if(test_case==107 || test_case==108) ok=dpmi_faulting_decode_page();
    else if(test_case==145) ok=v86_a20_word_ram_control();
    else if(test_case==146) ok=vdd_sas_unbound_control();
    else if(test_case==154) ok=dpmi_remapped_decode_fallback();
    else if((test_case>=93 && test_case<=101) || (test_case>=104 && test_case<=106) || (test_case>=150 && test_case<=153) || test_case==155 || test_case==156) ok=dpmi_remapped_code_page();
    else if(test_case==102 || test_case==103) ok=monitor_v86_mode_boundary();
    else if(test_case>=113 && test_case<=116) ok=broadcast_debug_execution();
    else if((test_case>=109 && test_case<=112) || test_case==117) ok=restored_debug_execution();
    else if(test_case==49) ok=duplicate_tlb_mapping();
    else if(test_case==76) ok=monitor_cache_contract();
    else return 5;
    return ok ? 0 : 1;
}
