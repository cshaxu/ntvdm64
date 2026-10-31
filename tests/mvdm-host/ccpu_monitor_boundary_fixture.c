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
extern void load_sw_cpu_access_functions(void);
extern void c_cpu_simulate(void);
extern void c_cpu_interrupt(int, unsigned short);
extern uint32_t c_cpu_q_ev_get_count(void);
extern void c_setIP(unsigned short);
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
static volatile unsigned long test_out;
static int test_case;
extern void do_intrupt(unsigned short, BOOL, BOOL, unsigned short);
static volatile uint32_t test_fault, test_irq, test_irq_eip, test_irq_isr;
static volatile uint32_t frame_fault_vector, frame_fault_eip, test_fault_vector, test_fault_error;
extern uint32_t lin2phy(uint32_t,int);
extern int ccpu_monitor_cached_access(uint32_t,unsigned char,uint32_t *);
static int duplicate_tlb_mapping(void) {
    uint32_t *pte=(uint32_t *)c_GetPhyAdd(0x5000),first,second,actual,monitor=0;
    int hit;
    c_cpu_init(); ZeroMemory(c_GetPhyAdd(0x4000),0x2000);
    *(uint32_t *)c_GetPhyAdd(0x4000)=0x5007; pte[2]=0x8007;
    c_setCR3(0x4000);c_setPG(1);
    first=lin2phy(0x2000,2); pte[2]=0x9007;
    second=lin2phy(0x2000,3);actual=lin2phy(0x2000,2);
    hit=ccpu_monitor_cached_access(0x2000,2,&monitor);
    {char line[160];wsprintfA(line,"TLB_DUPLICATE first=%lx write=%lx cpu=%lx monitor=%lx hit=%d\n",first,second,actual,monitor,hit);report(line);}
    c_setPG(0);
    return first==0x8000 && second==0x9000 && actual==0x9000 && hit==1 && monitor==actual;
}
static int monitor_cache_contract(void) {
    uint32_t *pte=(uint32_t *)c_GetPhyAdd(0x5000),phys=0;int miss,hit,deny,write_miss,ext_read,ext_deny,ext_write;
    extern int xtrn2phy(uint32_t,unsigned char,uint32_t *);
    c_cpu_init();ZeroMemory(c_GetPhyAdd(0x4000),0x2000);
    *(uint32_t *)c_GetPhyAdd(0x4000)=0x5003;pte[2]=0x8003;
    c_setCR3(0x4000);c_setPG(1);
    miss=ccpu_monitor_cached_access(0x2000,0,&phys);
    ext_read=xtrn2phy(0x2000,0,&phys) && phys==0x8000;
    if(lin2phy(0x2000,0)!=0x8000){c_setPG(0);return 0;}
    hit=ccpu_monitor_cached_access(0x2000,0,&phys);
    deny=ccpu_monitor_cached_access(0x2000,2,&phys);
    ext_deny=xtrn2phy(0x2000,2,&phys);
    write_miss=ccpu_monitor_cached_access(0x2000,1,&phys);
    ext_write=xtrn2phy(0x2000,1,&phys) && phys==0x8000;
    c_setPG(0);
    {char line[128];wsprintfA(line,"CACHE_CONTRACT miss=%d hit=%d deny=%d write_miss=%d ext=%d,%d,%d\n",miss,hit,deny,write_miss,ext_read,ext_deny,ext_write);report(line);}
    return miss==0 && hit==1 && deny==-1 && write_miss==0 && ext_read && !ext_deny && ext_write;
}
static unsigned io_reads, io_writes, string_reads, string_writes, first_output, string_visible_during_callback, string_output_uses_guest, string_word_direct, wrong_string_write;
static void probe_read(io_addr port, half_word *value) { (void)port; ++io_reads; *value=0xa5; }
static void probe_write(io_addr port, half_word value) { (void)port; if(!io_writes)first_output=value; ++io_writes; }
static void probe_string_read(io_addr port, half_word *out, word count) { (void)port;++string_reads;io_reads+=count;while(count--)*out++=0xa5; }
static void probe_string_read_visible(io_addr port, half_word *out, word count) { (void)port;++string_reads;io_reads+=count;if(count){*out=0xa5;string_visible_during_callback=(*c_GetPhyAdd(0x912)==0xa5);} }
static void probe_string_read_word(io_addr port, word *out, word count) { (void)port;++string_reads;io_reads+=count;string_word_direct=((void *)out==(void *)c_GetPhyAdd(0x7fff));if(count)*out=0x6ba5; }
static void probe_string_read_untouched(io_addr port, half_word *out, word count) { (void)port;(void)out;++string_reads;io_reads+=count; }
static void probe_string_write_wrapped(io_addr port, half_word *in, word count) { (void)port;++string_writes;io_writes+=count;if(count)first_output=*in; }
static void probe_string_write_visible(io_addr port, half_word *in, word count) { (void)port;++string_writes;io_writes+=count;string_output_uses_guest=(in==c_GetPhyAdd(0x902) && *in==0x42); }
static void probe_wrong_string_write(io_addr port, half_word *in, word count) { (void)port;(void)in;(void)count;++wrong_string_write; }
static void probe_string_write(io_addr port, half_word *in, word count) { (void)port;(void)in;++string_writes;io_writes+=count; }
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
static BOOL probe_unhandled_fault(unsigned long vector, unsigned long error) {
    if(vector!=13 || error!=0) return FALSE;
    ++test_fault;
    c_setEIP(test_case==41 ? 0x100b : 0x1009);
    return TRUE;
}
static BOOL probe_irq_frame_fault(unsigned long vector, unsigned long error) {
    (void)error;frame_fault_vector=vector;frame_fault_eip=c_getEIP();
    c_setEIP(0x1020);return TRUE;
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
static BOOL test_irq_handler(unsigned long vector) {
    unsigned char isr=0;
    (void)vector;
    ica_outb(0x20,0x0b);ica_inb(0x20,&isr);test_irq_isr=isr;
    ++test_irq; test_irq_eip=c_getEIP(); return FALSE;
}
static int protected_scalar_io(void) {
    unsigned char gdt[48]={0};
    unsigned char program[64]={0xba,0x80,0x00,0xb8,0x5a,0x00};
    unsigned used=6, width=(test_case==11 || test_case==13) ? 2 : 1;
    int immediate=(test_case==10 || test_case==11);
    half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;
    unsigned i;
    if(test_case==47) {
        const unsigned char setup[]={0x66,0xb8,0x00,0x03,0x00,0x00,0x0f,0x23,0xc0,0x66,0xb8,0x01,0x00,0x01,0x00,0x0f,0x23,0xf8,0xc6,0x06,0x00,0x03,0x5a};
        CopyMemory(program+used,setup,sizeof(setup));used+=sizeof(setup);
    }
    else if(test_case==45 || test_case==46) {
        unsigned char setup[]={0x66,0xb8,0x18,0x10,0x0f,0x00,0x0f,0x23,0xc0,0x66,0xb8,0x01,0,0,0,0x0f,0x23,0xf8,0x90};
        if(test_case==46)setup[2]=0x19;
        if(test_case==45)setup[18]=0x43; /* callback skips INC BX */
        CopyMemory(program+used,setup,sizeof(setup));used+=sizeof(setup);
        if(test_case==46)program[used++]=0x90;
    }
    else if(test_case>=42 && test_case<=44) {
        if(test_case!=43){program[used++]=0x66;program[used++]=0xb8;program[used++]=test_case==42 ? 0x78 : 2;program[used++]=test_case==42 ? 0x56 : 0;program[used++]=test_case==42 ? 0x34 : 0;program[used++]=test_case==42 ? 0x12 : 0;program[used++]=0x0f;program[used++]=0x23;program[used++]=test_case==42 ? 0xc0 : 0xf8;}
        program[used++]=0x0f;program[used++]=0x21;program[used++]=test_case==42 ? 0xc3 : (test_case==43 ? 0xe3 : 0xfb);
        program[used++]=0x66;program[used++]=0x89;program[used++]=0x1e;program[used++]=0x00;program[used++]=0x03;
    }
    else if(test_case==41) {program[used++]=0x0f;program[used++]=0x01;program[used++]=0x36;program[used++]=0x00;program[used++]=0x30;}
    else if(test_case==39) {program[used++]=0x67;program[used++]=0x0f;program[used++]=0x01;program[used++]=0x35;program[used++]=0x00;program[used++]=0x03;program[used++]=0x00;program[used++]=0x00;}
    else if(test_case==40) {program[used++]=0x66;program[used++]=0xbd;program[used++]=0x00;program[used++]=0x02;program[used++]=0x00;program[used++]=0x00;program[used++]=0x66;program[used++]=0xb9;program[used++]=0x40;program[used++]=0x00;program[used++]=0x00;program[used++]=0x00;program[used++]=0x67;program[used++]=0x0f;program[used++]=0x01;program[used++]=0x74;program[used++]=0x8d;program[used++]=0x00;}
    else if(test_case==37) {program[used++]=0xbb;program[used++]=0xfe;program[used++]=0x02;program[used++]=0x0f;program[used++]=0x01;program[used++]=0x77;program[used++]=0x02;}
    else if(test_case==38) {program[used++]=0xbd;program[used++]=0x00;program[used++]=0x02;program[used++]=0xbe;program[used++]=0x00;program[used++]=0x01;program[used++]=0x0f;program[used++]=0x01;program[used++]=0x32;}
    else if(test_case==36) {program[used++]=0x0f;program[used++]=0x01;program[used++]=0x36;program[used++]=0x00;program[used++]=0x03;}
    else if(test_case==34 || test_case==35) {
        program[used++]=0x0f;program[used++]=0x20;program[used++]=test_case==34 ? 0xc0 : 0xdb;
        program[used++]=0x66;
        if(test_case==34)program[used++]=0xa3;
        else {program[used++]=0x89;program[used++]=0x1e;}
        program[used++]=0x00;program[used++]=0x03;
    }
    else if(test_case==32 || test_case==33) { program[4]=6;program[used++]=0x0f;program[used++]=test_case==32 ? 0x22 : 0x01;program[used++]=test_case==32 ? 0xc0 : 0xf0; }
    else if(test_case==31 || test_case==48 || test_case==50) { if(test_case==50)program[used++]=0x66;program[used++]=0x0f;program[used++]=0x06;program[used++]=0x90; }
    else if(test_case==28) { program[used++]=0x6c;program[used++]=0x90;program[used++]=0x90; }
    else if(test_case==14) { program[used++]=0x66;program[used++]=0xe6;program[used++]=0x80; }
    else {
        program[used++]=immediate ? (width==1 ? 0xe6 : 0xe7) : (width==1 ? 0xee : 0xef);
        if(immediate) program[used++]=0x80;
        program[used++]=immediate ? (width==1 ? 0xe4 : 0xe5) : (width==1 ? 0xec : 0xed);
        if(immediate) program[used++]=0x80;
    }
    program[used++]=0xb8;program[used++]=0xef;program[used++]=0xbe;
    program[used++]=0xd6;program[used++]=0xfe;
    c_cpu_init(); io_init();
    io_define_inb(adapter,probe_read);io_define_outb(adapter,probe_write);
    for(i=0;i<2;i++) io_connect_port((io_addr)(0x80+i),adapter,IO_READ_WRITE);
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=test_case==51 ? 0x0040fa0fu : 0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    *(uint32_t *)(gdt+0x28)=test_case==38 || test_case==40 || test_case==54 ? 0x2000ffffu : 0x0000ffffu;*(uint32_t *)(gdt+0x2c)=test_case==38 || test_case==40 || test_case==54 ? 0x0000f300u : 0x0000f100u;
    CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));CopyMemory(c_GetPhyAdd(0xf1000),program,used);
    c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);
    c_setCS(0x1b);c_setSS(0x23);c_setDS(test_case==38 || test_case==40 || test_case==54 ? 0x2b : 0x23);c_setSP(0x800);c_setBX(0);c_setIOPL(test_case==48 ? 3 : 0);c_setIP(0x1000);
    if(test_case==28){c_setES(0x2b);c_setDI(0x300);*c_GetPhyAdd(0x300)=0;}
    if(test_case>=31)c_setCR0((c_getCR0() & ~6u) | ((test_case==34 || test_case==35) ? 14u : 8u));
    if(test_case==41){ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000);unsigned j;ZeroMemory(c_GetPhyAdd(0x4000),0x2000);*(ULONG *)c_GetPhyAdd(0x4000)=0x5007;for(j=0;j<512;j++)pte[j]=(j<<12)|7;pte[3]=0;*(uint16_t *)c_GetPhyAdd(0x3000)=6;c_setCR3(0x4000);c_setPG(1);}
    if(test_case>=36 && test_case<=40){*(uint16_t *)c_GetPhyAdd(0x300)=6;*(uint16_t *)c_GetPhyAdd(0x2300)=0;}
    if(test_case==34 || test_case==35 || (test_case>=42 && test_case<=44))*(uint32_t *)c_GetPhyAdd(0x300)=0xdeadbeef;
    if(test_case==47)*c_GetPhyAdd(0x300)=0;
    io_reads=io_writes=0;test_fault=0;VdmInstallFaultHandler((void *)(test_case>=45 && test_case<=47 ? probe_debug_fault : (test_case==14 || test_case==28 || test_case==41) ? probe_unhandled_fault : test_sti_fault));
    if(test_case==19) {
        int handled;
        *c_GetPhyAdd(0xf1000)=0xe6;*c_GetPhyAdd(0xf1001)=0x80;
        handled=host_exint_hook(13,4);
        VdmInstallFaultHandler(NULL);
        return handled && test_fault==0 && !io_reads && io_writes==1 && c_getIP()==0x1002;
    }
    if(test_case==39 || test_case==40 || test_case==50) {
        unsigned offset=test_case==40 ? 18 : 6;
        int handled;
        c_setIP((unsigned short)(0x1000+offset));
        handled=host_exint_hook(13,0);
        VdmInstallFaultHandler(NULL);
        return !handled && test_fault==1 && !io_reads && !io_writes &&
            c_getIP()==(unsigned short)(0x1000+offset);
    }
    if(test_case==51) {
        const unsigned char lmsw32[]={0x0f,0x01,0x35,0x00,0x03,0x00,0x00};
        int handled;
        CopyMemory(c_GetPhyAdd(0xf1000),lmsw32,sizeof(lmsw32));
        c_setDI(0x300);*(uint16_t *)c_GetPhyAdd(0x300)=6;
        handled=host_exint_hook(13,0);
        VdmInstallFaultHandler(NULL);
        return handled && test_fault==0 && c_getEIP()==0x1003 && (c_getCR0()&15)==15;
    }
    if(test_case==53) {
        const unsigned char lmsw_bx_si[]={0x0f,0x01,0x30};
        int handled;
        CopyMemory(c_GetPhyAdd(0xf1000),lmsw_bx_si,sizeof(lmsw_bx_si));
        c_setBX(0xffff);c_setSI(1);
        *(uint16_t *)c_GetPhyAdd(0)=0;*(uint16_t *)c_GetPhyAdd(0x10000)=6;
        handled=host_exint_hook(13,0);
        VdmInstallFaultHandler(NULL);
        return handled && test_fault==0 && c_getEIP()==0x1003 && (c_getCR0()&15)==15;
    }
    if(test_case==54) {
        const unsigned char lmsw_negative[]={0x0f,0x01,0x70,0xff};
        int handled;
        CopyMemory(c_GetPhyAdd(0xf1000),lmsw_negative,sizeof(lmsw_negative));
        c_setBX(0);c_setSI(0);*(uint16_t *)c_GetPhyAdd(0x1fff)=6;
        handled=host_exint_hook(13,0);
        VdmInstallFaultHandler(NULL);
        return handled && test_fault==0 && c_getEIP()==0x1004 && (c_getCR0()&15)==15;
    }
    if(test_case==57 || test_case==58) {
        const unsigned char lmsw_mixed_permissions[]={0x0f,0x01,0x36,0x00,0x23};
        ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000),physical;unsigned j;int handled,external_allows;
        extern int xtrn2phy(unsigned long,unsigned char,unsigned long *);
        CopyMemory(c_GetPhyAdd(0xf1000),lmsw_mixed_permissions,sizeof(lmsw_mixed_permissions));
        ZeroMemory(c_GetPhyAdd(0x4000),0x2000);*(ULONG *)c_GetPhyAdd(0x4000)=test_case==58 ? 0x5003 : 0x5007;
        for(j=0;j<512;j++)pte[j]=(j<<12)|7;
        if(test_case==57)pte[2]=0x2003;
        *(uint16_t *)c_GetPhyAdd(0x2300)=6;
        c_setCR3(0x4000);c_setPG(1);external_allows=xtrn2phy(0x2300,2,&physical);
        handled=host_exint_hook(13,0);
        VdmInstallFaultHandler(NULL);c_setPG(0);
        return external_allows && physical==0x2300 && handled && test_fault==0 && c_getEIP()==0x1005 && (c_getCR0()&15)==15;
    }
    if(test_case==55 || test_case==56) {
        const unsigned char lmsw_cross_page[]={0x0f,0x01,0x36,0xfe,0x2f};
        ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000);unsigned j;int handled,ok;
        CopyMemory(c_GetPhyAdd(0xf1000),lmsw_cross_page,sizeof(lmsw_cross_page));
        ZeroMemory(c_GetPhyAdd(0x4000),0x2000);*(ULONG *)c_GetPhyAdd(0x4000)=0x5007;
        for(j=0;j<512;j++)pte[j]=(j<<12)|7;
        if(test_case==55)pte[3]=0;
        *(uint16_t *)c_GetPhyAdd(0x2ffe)=6;c_setCR3(0x4000);c_setPG(1);
        handled=host_exint_hook(13,0);
        {char line[120];wsprintfA(line,"LMSW_CROSS_PAGE handled=%d fault=%lu eip=%lx cr0=%lx\n",handled,test_fault,c_getEIP(),c_getCR0());report(line);}
        ok=test_case==55 ? (!handled && test_fault==1 && c_getEIP()==0x1000 && (c_getCR0()&15)==9) : (handled && test_fault==0 && c_getEIP()==0x1005 && (c_getCR0()&15)==15);
        VdmInstallFaultHandler(NULL);c_setPG(0);return ok;
    }
    c_cpu_simulate();VdmInstallFaultHandler(NULL);if(test_case==41)c_setPG(0);
    {char line[120];wsprintfA(line,"MONITOR_SCALAR_IO case=%d width=%u reads=%u writes=%u fault=%lu ax=%04x\n",test_case,width,io_reads,io_writes,test_fault,c_getAX());report(line);}
    if(test_case==31 || test_case==48) return test_fault==0 && io_reads==0 && io_writes==0 && (c_getCR0()&8) && c_getAX()==0xbeef;
    if(test_case==32 || test_case==33) return test_fault==0 && io_reads==0 && io_writes==0 && (c_getCR0()&15)==15 && c_getAX()==0xbeef;
    if(test_case>=36 && test_case<=40) return test_fault==0 && io_reads==0 && io_writes==0 && (c_getCR0()&15)==15 && c_getAX()==0xbeef;
    if(test_case==34 || test_case==35) return test_fault==0 && io_reads==0 && io_writes==0 && *(uint32_t *)c_GetPhyAdd(0x300)==(test_case==34 ? (c_getCR0() & ~8u) : 0) && c_getAX()==0xbeef;
    if(test_case>=42 && test_case<=44) return test_fault==0 && io_reads==0 && io_writes==0 && *(uint32_t *)c_GetPhyAdd(0x300)==(test_case==42 ? 0x00345678u : test_case==43 ? 0u : 1u) && c_getAX()==0xbeef;
    if(test_case==45 || test_case==46) {extern uint32_t nr_inst_break;char line[120];wsprintfA(line,"MONITOR_DEBUG dr0=%08lx dr7=%08lx count=%lu fault=%lu eip=%08lx bx=%04x\n",CCPU_DR[0],CCPU_DR[7],nr_inst_break,test_fault,frame_fault_eip,(unsigned short)c_getBX());report(line);return test_fault==1 && frame_fault_eip==(test_case==46 ? 0x1019 : 0x1018) && (test_case!=45 || (unsigned short)c_getBX()==0) && c_getAX()==0xbeef;}
    if(test_case==47)return test_fault==1 && frame_fault_eip==0x101d && *c_GetPhyAdd(0x300)==0x5a && (CCPU_DR[6]&1) && c_getAX()==0xbeef;
    /* NT4 EventVdmStringIo resolves the selector without checking ES write rights. */
    if(test_case==28) return test_fault==0 && io_reads==1 && io_writes==0 && *c_GetPhyAdd(0x300)==0xa5 && c_getAX()==0xbeef;
    if(test_case==41) return test_fault==1 && io_reads==0 && io_writes==0 && (c_getCR0()&15)==9 && c_getAX()==0xbeef;
    if(test_case==14) return test_fault==0 && io_reads==0 && io_writes==1 && c_getAX()==0xbeef;
    return test_fault==0 && io_reads==width && io_writes==width && c_getAX()==0xbeef;
}
extern unsigned short c_getSI(void),c_getDI(void),c_getCX(void);
static int protected_string_reverse_baseline(void) {
    unsigned char gdt[48]={0};
    unsigned char program[32]={0xfd,0xba,0x80,0x00,0xbe,0x02,0x09,0xbf,0x12,0x09,
        0xb9,0x02,0x00,0xf3,0x6e,0xb9,0x02,0x00,0xf3,0x6c,0xfc,0xb8,0xef,0xbe,0xd6,0xfe};
    unsigned source=test_case==24 ? 0x2902 : 0x902, target=test_case==24 ? 0x3912 : (test_case==25 ? 0x2912 : 0x912), target_index=test_case==25 ? 0x912 : target;
    half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;
    if(test_case==24){program[6]=0x29;program[9]=0x39;}
    if(test_case==25){MoveMemory(program+14,program+13,13);program[13]=0x26;}
    if(test_case==29)program[13]=0xf2;
    c_cpu_init();io_init();io_define_inb(adapter,probe_read);io_define_outb(adapter,probe_write);
    if(test_case==23){io_define_in_routines(adapter,probe_read,NULL,probe_string_read,NULL);io_define_out_routines(adapter,probe_write,NULL,probe_string_write,NULL);}
    if(test_case==52)io_define_in_routines(adapter,probe_read,NULL,probe_string_read_untouched,NULL);
    if(test_case==59){io_define_in_routines(adapter,probe_read,NULL,probe_string_read_visible,NULL);io_define_out_routines(adapter,probe_write,NULL,probe_string_write_visible,NULL);}
    if(test_case==82){half_word other=adapter-1;wrong_string_write=0;
        io_define_out_routines(adapter,probe_write,NULL,probe_string_write,NULL);
        io_define_out_routines(other,probe_write,NULL,probe_wrong_string_write,NULL);
        io_connect_port((io_addr)0x80,adapter,IO_READ);io_connect_port((io_addr)0x80,other,IO_WRITE);}
    else io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    *(uint32_t *)(gdt+0x28)=0x2000ffffu;*(uint32_t *)(gdt+0x2c)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    *c_GetPhyAdd(source)=0x42;*c_GetPhyAdd(source-1)=0x43;*c_GetPhyAdd(source+1)=0x44;*c_GetPhyAdd(target)=0;*c_GetPhyAdd(target-1)=0;*c_GetPhyAdd(target+1)=0;
    if(test_case==52)*c_GetPhyAdd(target)=0x37;
    if(test_case==25)*c_GetPhyAdd(source+0x2000)=0x77;
    c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);
    c_setES(test_case==25 ? 0x2b : 0x23);
    c_setSP(0x800);c_setIOPL(test_case>=21 ? 0 : 3);c_setIP(0x1000);io_reads=io_writes=string_reads=string_writes=0;test_fault=0;
    if(test_case==22 || test_case==24) {ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000);unsigned i;ZeroMemory(c_GetPhyAdd(0x4000),0x2000);*(ULONG *)c_GetPhyAdd(0x4000)=0x5007;for(i=0;i<512;i++)pte[i]=(i<<12)|7;c_setCR3(0x4000);c_setPG(1);}
    VdmInstallFaultHandler((void *)test_sti_fault);c_cpu_simulate();VdmInstallFaultHandler(NULL);
    if(test_case==22 || test_case==24)c_setPG(0);
    {char line[140];wsprintfA(line,"MONITOR_STRING_DF cpu_si=%04x di=%04x cx=%04x reads=%u writes=%u bytes=%02x,%02x fault=%lu\n",c_getSI(),c_getDI(),c_getCX(),io_reads,io_writes,*c_GetPhyAdd(0x911),*c_GetPhyAdd(0x912),test_fault);report(line);}
    if(test_case==15) return c_getSI()==source-2 && c_getDI()==target_index-2 && c_getCX()==0 && io_reads==2 && io_writes==2 && *c_GetPhyAdd(target-1)==0xa5 && *c_GetPhyAdd(target)==0xa5 && test_fault==0 && c_getAX()==0xbeef;
    if(test_case==82)return string_writes==1 && !wrong_string_write && io_writes==1 && io_reads==1 && *c_GetPhyAdd(target)==0xa5 && !test_fault && c_getAX()==0xbeef;
    if(test_case==59)return string_visible_during_callback && string_output_uses_guest && *c_GetPhyAdd(target)==0xa5 && test_fault==0 && c_getAX()==0xbeef;
    if(test_case==52)return c_getSI()==source-1 && c_getDI()==target_index-1 && c_getCX()==2 && io_reads==1 && io_writes==1 && string_reads==1 && *c_GetPhyAdd(target)==0x37 && test_fault==0 && c_getAX()==0xbeef;
    return c_getSI()==source-1 && c_getDI()==target_index-1 && c_getCX()==2 && io_reads==1 && io_writes==1 && *c_GetPhyAdd(target)==0xa5 && *c_GetPhyAdd(target+1)!=0xa5 && test_fault==0 && c_getAX()==0xbeef && (test_case!=25 || first_output==0x42) && (test_case!=23 || (string_reads==1 && string_writes==1)) && (test_case!=24 || ((*(ULONG *)c_GetPhyAdd(0x5008)&0x20) && (*(ULONG *)c_GetPhyAdd(0x500c)&0x60)==0x60));
}
static int protected_split_word_string(void) {
    unsigned char gdt[40]={0};ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000);unsigned i;
    const unsigned char program[]={0x6d};half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;
    ULONG second=test_case==60 ? 0x9000 : 0x8000;int handled;
    c_cpu_init();io_init();io_define_in_routines(adapter,probe_read,NULL,NULL,probe_string_read_word);
    io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);c_setES(0x23);
    c_setSP(0x800);c_setDI(0x7fff);c_setDX(0x80);c_setIOPL(0);c_setIP(0x1000);
    ZeroMemory(c_GetPhyAdd(0x4000),0x2000);*(ULONG *)c_GetPhyAdd(0x4000)=0x5007;
    for(i=0;i<512;i++)pte[i]=(i<<12)|7;pte[8]=second|7;
    *c_GetPhyAdd(0x7fff)=0;*c_GetPhyAdd(second)=0;c_setCR3(0x4000);c_setPG(1);
    io_reads=string_reads=string_word_direct=test_fault=0;VdmInstallFaultHandler((void *)test_sti_fault);
    handled=host_exint_hook(13,0);VdmInstallFaultHandler(NULL);c_setPG(0);
    return handled && !test_fault && string_reads==1 && io_reads==1 &&
        string_word_direct==(test_case==61) && *c_GetPhyAdd(0x7fff)==0xa5 &&
        *c_GetPhyAdd(second)==0x6b && c_getDI()==0x8001 && c_getEIP()==0x1001;
}
static int protected_wrapped_cs(void) {
    unsigned char gdt[40]={0},program[]={0xe6,0x80,0xb8,0xef,0xbe,0xd6,0xfe};
    half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;int handled;
    c_cpu_init();io_init();io_define_outb(adapter,probe_write);io_connect_port((io_addr)0x80,adapter,IO_WRITE);
    *(uint32_t *)(gdt+0x18)=0xf000ffffu;*(uint32_t *)(gdt+0x1c)=0xff00faffu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt));CopyMemory(c_GetPhyAdd(0x1000),program,sizeof(program));
    c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);
    c_setSP(0x800);c_setAX(0x5a);c_setIOPL(0);c_setIP(0x2000);io_writes=test_fault=0;
    VdmInstallFaultHandler((void *)test_sti_fault);
    if(test_case==63){c_cpu_simulate();handled=(c_getAX()==0xbeef);}
    else handled=host_exint_hook(13,0);
    VdmInstallFaultHandler(NULL);
    {char line[100];wsprintfA(line,"WRAPPED_CS handled=%d fault=%lu out=%u eip=%lx\n",handled,test_fault,io_writes,c_getEIP());report(line);}
    return handled && !test_fault && io_writes==1 && first_output==0x5a && (test_case==63 || c_getEIP()==0x2002);
}
static int protected_wrapped_string(void) {
    unsigned char gdt[48]={0},program[]={0x6c,0xb8,0xef,0xbe,0xd6,0xfe};
    half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;int handled;
    c_cpu_init();io_init();io_define_in_routines(adapter,probe_read,NULL,probe_string_read,NULL);
    if(test_case==66){program[0]=0x6e;io_define_out_routines(adapter,probe_write,NULL,probe_string_write_wrapped,NULL);}
    io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    *(uint32_t *)(gdt+0x28)=0xf000ffffu;*(uint32_t *)(gdt+0x2c)=0xff00f3ffu;
    CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt));CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(test_case==66 ? 0x2b : 0x23);c_setES(0x2b);
    c_setSP(0x800);c_setDI(0x2000);c_setSI(0x2000);c_setDX(0x80);c_setIOPL(0);c_setIP(0x1000);
    *c_GetPhyAdd(0x1000)=test_case==66 ? 0x42 : 0;io_reads=io_writes=string_reads=string_writes=test_fault=0;
    VdmInstallFaultHandler((void *)test_sti_fault);
    if(test_case==65 || test_case==66){c_cpu_simulate();handled=(c_getAX()==0xbeef);}
    else handled=host_exint_hook(13,0);
    VdmInstallFaultHandler(NULL);
    {char line[100];wsprintfA(line,"WRAPPED_STRING handled=%d fault=%lu reads=%u eip=%lx\n",handled,test_fault,io_reads,c_getEIP());report(line);}
    if(test_case==66)return handled && !test_fault && io_writes==1 && string_writes==1 && first_output==0x42 && c_getSI()==0x2001;
    return handled && !test_fault && io_reads==1 && string_reads==1 && *c_GetPhyAdd(0x1000)==0xa5 && c_getDI()==0x2001 && (test_case==65 || c_getEIP()==0x1001);
}
static int protected_changed_descriptor(void) {
    unsigned char gdt[48]={0},program[]={0x6c,0xb8,0xef,0xbe,0xd6,0xfe};
    half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;BOOL input=(test_case==67 || test_case==75);
    c_cpu_init();io_init();io_define_in_routines(adapter,probe_read,NULL,probe_string_read,NULL);
    io_define_out_routines(adapter,probe_write,NULL,probe_string_write_wrapped,NULL);
    io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);if(!input)program[0]=0x6e;
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    *(uint32_t *)(gdt+0x28)=0x0000ffffu;*(uint32_t *)(gdt+0x2c)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt));CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);c_setES(0x2b);
    c_setSP(0x800);c_setSI(0x902);c_setDI(0x912);c_setDX(0x80);c_setIOPL(0);c_setIP(0x1000);
    *(uint32_t *)c_GetPhyAdd(input ? 0x3028 : 0x3020)=0x2000ffffu;
    *c_GetPhyAdd(0x902)=0x11;*c_GetPhyAdd(0x2902)=0x42;
    *c_GetPhyAdd(0x912)=0x11;*c_GetPhyAdd(0x2912)=0;
    io_reads=io_writes=string_reads=string_writes=first_output=test_fault=0;
    VdmInstallFaultHandler((void *)test_sti_fault);
    if(test_case==75){int handled,restored;c_setGDT_BASE_LIMIT(0x3000,0x27);doing_contributory=TRUE;
        handled=host_exint_hook(13,0);restored=doing_contributory;doing_contributory=FALSE;VdmInstallFaultHandler(NULL);
        return !handled && restored && test_fault==1 && !io_reads && c_getEIP()==0x1000;}
    c_cpu_simulate();VdmInstallFaultHandler(NULL);
    {char line[128];wsprintfA(line,"CHANGED_DESCRIPTOR case=%d fault=%lu reads=%u writes=%u old=%02x new=%02x output=%02x\n",test_case,test_fault,io_reads,io_writes,*c_GetPhyAdd(0x912),*c_GetPhyAdd(0x2912),first_output);report(line);}
    if(input)return !test_fault && io_reads==1 && string_reads==1 && *c_GetPhyAdd(0x912)==0x11 && *c_GetPhyAdd(0x2912)==0xa5 && c_getDI()==0x913 && c_getAX()==0xbeef;
    return !test_fault && io_writes==1 && string_writes==1 && first_output==0x42 && c_getSI()==0x903 && c_getAX()==0xbeef;
}
static int protected_string_segment_limit(void) {
    unsigned char gdt[48]={0},program[]={0x6d,0xb8,0xef,0xbe,0xd6,0xfe};half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;
    BOOL last_byte=(test_case==69 || test_case==71);unsigned index=last_byte ? 0xffff : 0x200;int handled;
    c_cpu_init();io_init();io_define_in_routines(adapter,probe_read,NULL,probe_string_read,probe_string_read_word);
    io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);if(!last_byte)program[0]=0x6c;
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    *(uint32_t *)(gdt+0x28)=last_byte ? 0x0000ffffu : 0x00000100u;*(uint32_t *)(gdt+0x2c)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt));CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);c_setES(0x2b);
    c_setSP(0x800);c_setDI((uint16_t)index);c_setDX(0x80);c_setIOPL(0);c_setIP(0x1000);
    *c_GetPhyAdd(index)=0;*c_GetPhyAdd(index+1)=0;io_reads=string_reads=test_fault=0;
    VdmInstallFaultHandler((void *)test_sti_fault);
    if(test_case==71 || test_case==72){c_cpu_simulate();handled=(c_getAX()==0xbeef);}
    else handled=host_exint_hook(13,0);
    VdmInstallFaultHandler(NULL);
    {char line[100];wsprintfA(line,"STRING_LIMIT case=%d handled=%d fault=%lu reads=%u\n",test_case,handled,test_fault,io_reads);report(line);}
    return handled && !test_fault && io_reads==1 && string_reads==1 && *c_GetPhyAdd(index)==0xa5 &&
        (last_byte ? (*c_GetPhyAdd(index+1)==0x6b && c_getDI()==1) : c_getDI()==0x201) &&
        (test_case>=71 || c_getEIP()==0x1001);
}
static BOOL descriptor_page_fault(unsigned long vector, unsigned long error) {
    (void)error;++test_fault;frame_fault_vector=(uint32_t)vector;
    if(test_case==74 && vector==14){((ULONG *)c_GetPhyAdd(0x5000))[3]=0x3007;c_setEIP(0x1001);}
    else c_setEIP(test_case==74 ? 0x1003 : 0x1001);
    return TRUE;
}
static int protected_missing_descriptor_page(void) {
    unsigned char gdt[48]={0},program[]={0x6c,0xb8,0xef,0xbe,0xd6,0xfe,0,0};
    ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000);unsigned i;half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;
    if(test_case==74){program[1]=0xe6;program[2]=0x80;program[3]=0xb8;program[4]=0xef;program[5]=0xbe;program[6]=0xd6;program[7]=0xfe;}
    c_cpu_init();io_init();io_define_in_routines(adapter,probe_read,NULL,probe_string_read,NULL);io_define_outb(adapter,probe_write);io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    *(uint32_t *)(gdt+0x28)=0x0000ffffu;*(uint32_t *)(gdt+0x2c)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt));CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);c_setES(0x2b);
    c_setSP(0x800);c_setDI(0x912);c_setDX(0x80);c_setIOPL(0);c_setIP(0x1000);
    ZeroMemory(c_GetPhyAdd(0x4000),0x2000);*(ULONG *)c_GetPhyAdd(0x4000)=0x5007;for(i=0;i<512;i++)pte[i]=(i<<12)|7;pte[3]=0;
    c_setCR3(0x4000);c_setPG(1);io_reads=io_writes=string_reads=test_fault=frame_fault_vector=0;
    VdmInstallFaultHandler((void *)descriptor_page_fault);c_cpu_simulate();VdmInstallFaultHandler(NULL);c_setPG(0);
    {char line[100];wsprintfA(line,"DESCRIPTOR_PAGE fault=%lu vector=%lu reads=%u ax=%04x\n",test_fault,frame_fault_vector,io_reads,c_getAX());report(line);}
    return test_fault==1 && frame_fault_vector==14 && !io_reads && (test_case!=74 || io_writes==1) && c_getAX()==0xbeef;
}
static int protected_ldt_descriptor(void) {
    unsigned char ldt[40]={0},old_code[]={0xe6,0x80,0xb8,0xef,0xbe,0xd6,0xfe,0,0,0};
    const unsigned char new_code[]={0xe4,0x80};half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;
    c_cpu_init();io_init();io_define_inb(adapter,probe_read);io_define_outb(adapter,probe_write);io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);
    if(test_case==81){const unsigned char p[]={0x0f,0x01,0x36,0x00,0x03,0xb8,0xef,0xbe,0xd6,0xfe};CopyMemory(old_code,p,sizeof(p));}
    if(test_case==84){const unsigned char p[]={0x0f,0x01,0x76,0x00,0xb8,0xef,0xbe,0xd6,0xfe};CopyMemory(old_code,p,sizeof(p));}
    *(uint32_t *)(ldt+0x18)=0x0000ffffu;*(uint32_t *)(ldt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(ldt+0x20)=0x0000ffffu;*(uint32_t *)(ldt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x4000),ldt,sizeof(ldt));CopyMemory(c_GetPhyAdd(0xf1000),old_code,sizeof(old_code));
    c_setLDT_SELECTOR(0x28);c_setLDT_BASE_LIMIT(0x4000,sizeof(ldt)-1);c_setPE(1);c_setCPL(3);
    if(c_setCS(0x1f) || c_setSS(0x27) || c_setDS(0x27))return 0;
    c_setSP(0x800);c_setAX(0x5a);c_setIOPL(0);c_setIP(0x1000);
    if(test_case==118 || test_case==119 || test_case==120) {
        int handled;
        /* NT4 Kt0d110 attempts protected-VDM opcode dispatch irrespective
         * of the #GP error code. Use its user-LDT selector profile here. */
        if(test_case==119) *c_GetPhyAdd(0xf1000)=0x90;
        if(test_case==120) c_setIOPL(3);
        io_reads=io_writes=test_fault=test_fault_vector=test_fault_error=0;
        VdmInstallFaultHandler((void *)test_sti_fault);
        handled=host_exint_hook(13,4);
        VdmInstallFaultHandler(NULL);
        if(test_case==118 || test_case==120)
            return handled && !test_fault && io_writes==1 && c_getEIP()==0x1002;
        return !handled && test_fault==1 && test_fault_vector==13 && test_fault_error==4 && !io_writes && c_getEIP()==0x1000;
    }
    if(test_case==78){*(uint32_t *)c_GetPhyAdd(0x4018)=0x5000ffffu;*(uint32_t *)c_GetPhyAdd(0x401c)=0x0000fa00u;CopyMemory(c_GetPhyAdd(0x6000),new_code,sizeof(new_code));}
    if(test_case==79)*(uint32_t *)c_GetPhyAdd(0x401c)=0x0000f20fu;
    if(test_case==80)*(uint32_t *)c_GetPhyAdd(0x4018)=0x00000fffu;
    if(test_case==85)*(uint32_t *)c_GetPhyAdd(0x401c)=0x00007a0fu;
    if(test_case==81 || test_case==84){c_setBP(0x300);*(uint32_t *)c_GetPhyAdd(0x4020)=0x5000ffffu;*(uint16_t *)c_GetPhyAdd(0x300)=0;*(uint16_t *)c_GetPhyAdd(0x5300)=6;c_setCR0((c_getCR0()&~6u)|8u);}
    io_reads=io_writes=test_fault=0;VdmInstallFaultHandler((void *)test_sti_fault);
    if(test_case==79 || test_case==80 || test_case==85){int handled=host_exint_hook(13,0);VdmInstallFaultHandler(NULL);return !handled && test_fault==1 && !io_reads && !io_writes && c_getEIP()==0x1000;}
    c_cpu_simulate();VdmInstallFaultHandler(NULL);
    {char line[120];wsprintfA(line,"LDT_MONITOR case=%d fault=%lu reads=%u writes=%u ax=%04x\n",test_case,test_fault,io_reads,io_writes,c_getAX());report(line);}
    if(test_case==81 || test_case==84)return !test_fault && c_getAX()==0xbeef && (c_getCR0()&15)==15;
    return !test_fault && c_getAX()==0xbeef && (test_case==77 ? (io_writes==1 && !io_reads) : (io_reads==1 && !io_writes));
}
static int monitor_register_matrix(void) {
    unsigned char gdt[40]={0},program[]={0x0f,0x20,0xc0};unsigned r,write,j;
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    for(write=0;write<2;write++)for(r=0;r<8;r++){
        uint32_t saved[8],expected;int handled;
        c_cpu_init();CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt));
        program[1]=(unsigned char)(write ? 0x22 : 0x20);program[2]=(unsigned char)(0xc0+r);
        CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
        c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);
        c_setIOPL(0);c_setEIP(0x1000);c_setCR0((c_getCR0()&~6u)|8u);
        for(j=0;j<8;j++)CCPU_GR[j]=0x1000+j;
        if(write)CCPU_GR[r]=6;for(j=0;j<8;j++)saved[j]=CCPU_GR[j];
        expected=c_getCR0()&~8u;test_fault=0;VdmInstallFaultHandler((void *)test_sti_fault);
        handled=host_exint_hook(13,0);VdmInstallFaultHandler(NULL);
        if(!handled || test_fault || c_getEIP()!=0x1003)return 0;
        if(write){if((c_getCR0()&15)!=15)return 0;}
        else saved[r]=expected;
        for(j=0;j<8;j++)if(CCPU_GR[j]!=saved[j])return 0;
    }
    report("MONITOR_REGISTER_MATRIX 8 read and 8 write cases passed\n");return 1;
}
/* NT4 VdmUserCr0MapIn/Out preserve each guest-visible EM/MP pair. */
static int monitor_cr0_npx_matrix(void) {
    unsigned char gdt[40]={0}, program[]={0x0f,0x22,0xc0,0x0f,0x20,0xc1};
    unsigned n; int handled;
    *(uint32_t *)(gdt+0x18)=0x0000ffffu; *(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu; *(uint32_t *)(gdt+0x24)=0x0000f300u;
    for(n=0;n<4;n++) {
        uint32_t before, readback;
        c_cpu_init(); CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt));
        CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
        c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1); c_setPE(1); c_setCPL(3);
        c_setCS(0x1b); c_setSS(0x23); c_setDS(0x23); c_setIOPL(0); c_setEIP(0x1000);
        c_setCR0((c_getCR0() & ~6u) | 8u); before=c_getCR0();
        CCPU_GR[0]=n<<1; CCPU_GR[1]=0xdeadbeefu; test_fault=0;
        VdmInstallFaultHandler((void *)test_sti_fault);
        handled=host_exint_hook(13,0);
        if(!handled || test_fault || c_getEIP()!=0x1003 ||
           (c_getCR0() & ~6u)!=(before & ~6u) || (c_getCR0() & 6u)!=(n<<1)) {
            VdmInstallFaultHandler(NULL); return 0;
        }
        handled=host_exint_hook(13,0); readback=CCPU_GR[1];
        VdmInstallFaultHandler(NULL);
        if(!handled || test_fault || c_getEIP()!=0x1006 ||
           (readback & 6u)!=(n<<1) || (readback & 8u)) return 0;
    }
    report("MONITOR_CR0_NPX_MATRIX four EM/MP states passed\n"); return 1;
}
/* Original host instruction reads set accessed on a page-table walk. */
static int monitor_fetch_page_access(void) {
    unsigned char gdt[40]={0}, program[]={0xe6,0x80};
    ULONG *pde=(ULONG *)c_GetPhyAdd(0x4000), *pte=(ULONG *)c_GetPhyAdd(0x5000);
    half_word adapter=IO_MAX_NUMBER_ADAPTORS-1; unsigned i; int handled;
    extern void flush_tlb(void);
    extern ULONG lin2phy(ULONG, int);
    c_cpu_init(); io_init(); io_define_outb(adapter,probe_write); io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);
    *(uint32_t *)(gdt+0x18)=0x0000ffffu; *(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu; *(uint32_t *)(gdt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt)); CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    ZeroMemory(c_GetPhyAdd(0x4000),0x2000); pde[0]=0x5007;
    for(i=0;i<512;i++) pte[i]=(i<<12)|7;
    c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1); c_setPE(1); c_setCPL(3);
    c_setCS(0x1b); c_setSS(0x23); c_setDS(0x23); c_setIOPL(0); c_setEIP(0x1000);
    c_setCR3(0x4000); c_setPG(1); flush_tlb(); io_writes=test_fault=0;
    if(test_case==88) {
        if(lin2phy(0xf1000,2)!=0xf1000) {c_setPG(0);return 0;}
        *c_GetPhyAdd(0xf2000)=0xf4; pte[0xf1]=0xf2007;
    }
    VdmInstallFaultHandler((void *)test_sti_fault); handled=host_exint_hook(13,0);
    VdmInstallFaultHandler(NULL);
    {char line[120]; wsprintfA(line,"MONITOR_FETCH_PAGE handled=%d fault=%lu writes=%u pde=%08lx pte=%08lx\n",handled,test_fault,io_writes,pde[0],pte[0xf1]);report(line);}
    i=handled && !test_fault && io_writes==1 && (pde[0]&0x20) && !(pte[0xf1]&0x40) &&
        (test_case==88 ? !(pte[0xf1]&0x20) : (pte[0xf1]&0x20));
    c_setPG(0); return (int)i;
}
/* NT4 increments the linear instruction pointer modulo 32 bits. */
static int monitor_wrapped_instruction_offset(void) {
    unsigned char gdt[40]={0}; half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;
    int handled,ok;
    c_cpu_init(); io_init(); io_define_outb(adapter,probe_write); io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);
    *(uint32_t *)(gdt+0x18)=0x0000ffffu; *(uint32_t *)(gdt+0x1c)=0x00cffa00u;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu; *(uint32_t *)(gdt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt)); c_sas_enable_20_bit_wrapping();
    *c_GetPhyAdd(0xfffff)=0xe6; *c_GetPhyAdd(0)=0x80;
    c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1); c_setPE(1); c_setCPL(3);
    c_setCS(0x1b); c_setSS(0x23); c_setDS(0x23); c_setIOPL(0); c_setEIP(0xffffffffu);
    io_writes=test_fault=0; VdmInstallFaultHandler((void *)test_sti_fault);
    handled=host_exint_hook(13,0); VdmInstallFaultHandler(NULL);
    {char line[100];wsprintfA(line,"MONITOR_WRAP_IP handled=%d fault=%lu writes=%u eip=%08lx\n",handled,test_fault,io_writes,c_getEIP());report(line);}
    ok=handled && !test_fault && io_writes==1 && c_getEIP()==1;
    c_sas_disable_20_bit_wrapping(); return ok;
}
/* OpcodeGenericPrefix permits instruction length through 128 before dispatch. */
static int monitor_prefix_length(void) {
    unsigned char gdt[40]={0}, program[130]; half_word adapter=IO_MAX_NUMBER_ADAPTORS-1;
    unsigned prefixes=test_case==90 ? 16 : test_case==91 ? 127 : 128; int handled;
    c_cpu_init(); io_init(); io_define_outb(adapter,probe_write); io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);
    *(uint32_t *)(gdt+0x18)=0x0000ffffu; *(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu; *(uint32_t *)(gdt+0x24)=0x0000f300u;
    FillMemory(program,prefixes,0x66); program[prefixes]=0xe6; program[prefixes+1]=0x80;
    CopyMemory(c_GetPhyAdd(0x3000),gdt,sizeof(gdt)); CopyMemory(c_GetPhyAdd(0xf1000),program,prefixes+2);
    c_setGDT_BASE_LIMIT(0x3000,sizeof(gdt)-1); c_setPE(1); c_setCPL(3);
    c_setCS(0x1b); c_setSS(0x23); c_setDS(0x23); c_setIOPL(0); c_setEIP(0x1000);
    io_writes=test_fault=0; VdmInstallFaultHandler((void *)test_sti_fault);
    handled=host_exint_hook(13,0); VdmInstallFaultHandler(NULL);
    {char line[120];wsprintfA(line,"MONITOR_PREFIX count=%u handled=%d fault=%lu writes=%u eip=%08lx\n",prefixes,handled,test_fault,io_writes,c_getEIP());report(line);}
    if(test_case==92)return !handled && test_fault==1 && !io_writes && c_getEIP()==0x1000;
    return handled && !test_fault && io_writes==1 && c_getEIP()==0x1000+prefixes+2;
}
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
static DWORD WINAPI probe_wow_state_writer(void *unused) {
    unsigned i;(void)unused;
    for(i=0;i<20000;i++){BlockWOWIdle(TRUE);BlockWOWIdle(FALSE);}
    BlockWOWIdle(TRUE);
    return 0;
}
static int protected_concurrent_if(void) {
    unsigned char gdt[40]={0};
    const unsigned char program[]={0xb9,0xff,0xff,0xfb,0xfa,0xe2,0xfc,0xb8,0xef,0xbe,0xd6,0xfe};
    HANDLE writer;DWORD result;
    c_cpu_init();InitializeIcaLock();ica0_init();ica1_init();
    *(uint32_t *)(gdt+0x18)=0x0000ffffu;*(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu;*(uint32_t *)(gdt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    *(uint32_t *)c_GetPhyAdd(0x714)=0;
    c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1);c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);
    c_setSP(0x800);c_setIOPL(0);c_setIF(0);c_setIP(0x1000);test_fault=0;
    VdmInstallFaultHandler((void *)test_sti_fault);
    writer=CreateThread(NULL,0,probe_wow_state_writer,NULL,0,NULL);if(!writer)return 0;
    c_cpu_simulate();
    if(WaitForSingleObject(writer,INFINITE)!=WAIT_OBJECT_0 || !GetExitCodeThread(writer,&result)){CloseHandle(writer);return 0;}
    CloseHandle(writer);VdmInstallFaultHandler(NULL);
    {char line[120];wsprintfA(line,"MONITOR_IF_CONCURRENT state=%08lx fault=%lu writer=%lu ax=%04x\n",*(uint32_t *)c_GetPhyAdd(0x714),test_fault,result,c_getAX());report(line);}
    return (*(uint32_t *)c_GetPhyAdd(0x714)&0x00100200u)==0x00100000u && !test_fault && !result && c_getAX()==0xbeef;
}
static int protected_pending_irq_observe(void) {
    unsigned char pic_isr=0,pic_irr=0,slave_isr=0; unsigned char gdt[40]={0};
    unsigned char gate[8]={0x00,0x11,0x1b,0x00,0x00,0x86,0x00,0x00};
    unsigned char handler[]={0xfe,0x06,0x01,0x03,0xb0,0x20,0xe6,0x20,0xcf};
    unsigned char slave_handler[]={0xfe,0x06,0x01,0x03,0xb0,0x20,0xe6,0xa0,0xe6,0x20,0xcf};
    const unsigned char program[]={0xfb,0xc6,0x06,0x00,0x03,0x01,0xb8,0xef,0xbe,0xd6,0xfe};
    const unsigned char prefixed_program[]={0x66,0xfb,0xc6,0x06,0x00,0x03,0x01,0xb8,0xef,0xbe,0xd6,0xfe};
    const unsigned char stale_program[]={0xc7,0x06,0xc4,0x53,0x03,0x10,0xfb,0xc6,0x06,0x00,0x03,0x01,0xb8,0xef,0xbe,0xd6,0xfe};
    const unsigned char fault_sentinel[]={0xb8,0xef,0xbe,0xd6,0xfe};
    unsigned char longest_program[25];
    c_cpu_init();
    *(uint32_t *)(gdt+0x18)=0x0000ffffu; *(uint32_t *)(gdt+0x1c)=0x0000fa0fu;
    if(test_case==27) *(uint32_t *)(gdt+0x1c)=0x0000fa10u;
    *(uint32_t *)(gdt+0x20)=0x0000ffffu; *(uint32_t *)(gdt+0x24)=0x0000f300u;
    CopyMemory(c_GetPhyAdd(0x1000),gdt,sizeof(gdt));
    if(test_case==20) CopyMemory(c_GetPhyAdd(0xf1000),stale_program,sizeof(stale_program));
    else if(test_case==18) CopyMemory(c_GetPhyAdd(0xf1fff),prefixed_program,sizeof(prefixed_program));
    else if(test_case==7) {
        FillMemory(longest_program,14,0x66);
        CopyMemory(longest_program+14,program,sizeof(program));
        CopyMemory(c_GetPhyAdd(0xf1000),longest_program,sizeof(longest_program));
    } else if(test_case==6) CopyMemory(c_GetPhyAdd(0xf1000),prefixed_program,sizeof(prefixed_program));
    else CopyMemory(c_GetPhyAdd(0xf1000),program,sizeof(program));
    if(test_case==26){gate[5]=0x06;CopyMemory(c_GetPhyAdd(0xf1020),fault_sentinel,sizeof(fault_sentinel));frame_fault_vector=frame_fault_eip=0;}
    if(test_case==5) CopyMemory(c_GetPhyAdd(0xf1100),slave_handler,sizeof(slave_handler));
    else CopyMemory(c_GetPhyAdd(0xf1100),handler,sizeof(handler));
    if(test_case==27){CopyMemory(c_GetPhyAdd(0x2000),program,sizeof(program));CopyMemory(c_GetPhyAdd(0x1100),handler,sizeof(handler));}
    CopyMemory(c_GetPhyAdd(0x9040),gate,sizeof(gate));
    if(test_case==5) CopyMemory(c_GetPhyAdd(0x9380),gate,sizeof(gate));
    c_setIDT_BASE_LIMIT(0x9000,test_case==5 ? 0x387 : 0x47);
    *c_GetPhyAdd(0x300)=0; *c_GetPhyAdd(0x301)=0; test_fault=test_irq=test_irq_eip=0; test_out=0;
    InitializeIcaLock(); ica0_init(); ica1_init(); DelayIrqLine=0;
    ica_outb(0x20,0x11);ica_outb(0x21,8);ica_outb(0x21,4);ica_outb(0x21,test_case==4 ? 3 : 1);ica_outb(0x21,test_case==2 ? 0xff : (test_case==5 ? 0xfb : 0xfe));
    if(test_case==5) {ica_outb(0xa0,0x11);ica_outb(0xa1,0x70);ica_outb(0xa1,2);ica_outb(0xa1,1);ica_outb(0xa1,0xfe);}
    VdmInstallFaultHandler((void *)(test_case==26 ? probe_irq_frame_fault : test_sti_fault)); VdmInstallHardwareIntHandler((void *)test_irq_handler);
    c_setGDT_BASE_LIMIT(0x1000,sizeof(gdt)-1); c_setPE(1);c_setCPL(3);c_setCS(0x1b);c_setSS(0x23);c_setDS(0x23);c_setSP(0x800);c_setIOPL(test_case==30 ? 3 : 0);c_setIF(0);c_setIP(0x1000);
    if(test_case==27){c_sas_enable_20_bit_wrapping();c_setIP(0x2000);}
    if(test_case==17 || test_case==18 || test_case==20) {
        ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000);unsigned i;
        ZeroMemory(c_GetPhyAdd(0x4000),0x2000);*(ULONG *)c_GetPhyAdd(0x4000)=0x5007;
        for(i=0;i<512;i++) pte[i]=(i<<12)|7;
        c_setCR3(0x4000);c_setPG(1);
        if(test_case==18)c_setIP(0x1fff);
    }
    if(test_case==0 || test_case==2 || test_case==4 || test_case==6 || test_case==7 || test_case==17 || test_case==18 || test_case==20 || test_case==26 || test_case==27 || test_case==30) ica_hw_interrupt(0,0,1);
    if(test_case==5) ica_hw_interrupt(1,0,1);
    if(test_case==3) c_cpu_interrupt(3,0);
    c_cpu_simulate();
    ica_outb(0x20,0x0b); ica_inb(0x20,&pic_isr); ica_outb(0x20,0x0a); ica_inb(0x20,&pic_irr);
    if(test_case==5) {ica_outb(0xa0,0x0b);ica_inb(0xa0,&slave_isr);}
    { char line[180]; wsprintfA(line,"OBSERVE PM_STI case=%d fault=%lu irq=%lu irq_eip=%04lx marker=%u out=%lu isr=%02x irr=%02x ax=%04x\n",test_case,test_fault,test_irq,test_irq_eip,*c_GetPhyAdd(0x300),test_out,pic_isr,pic_irr,c_getAX()); report(line); }
    VdmInstallHardwareIntHandler(NULL);VdmInstallFaultHandler(NULL);
    if(test_case==30)return test_fault==0 && test_irq==1 && test_irq_eip==0x1006 && !(pic_isr&1) && c_getAX()==0xbeef;
    if(test_case==27){c_sas_disable_20_bit_wrapping();return test_fault==0 && test_irq==1 && test_irq_eip==0x2001 && c_getAX()==0xbeef;}
    if(test_case==26){char line[100];wsprintfA(line,"MONITOR_IRQ_FRAME_FAULT vector=%lu ip=%04lx\n",frame_fault_vector,frame_fault_eip);report(line);return test_irq==1 && frame_fault_vector==11 && frame_fault_eip==0x1001 && c_getAX()==0xbeef;}
    if(test_case==17) {c_setPG(0);return test_fault==0 && test_irq==1 && test_irq_eip==0x1001 && !(pic_isr&1) && c_getAX()==0xbeef;}
    if(test_case==18) {c_setPG(0);return test_fault==0 && test_irq==1 && test_irq_eip==0x2000 && !(pic_isr&1) && c_getAX()==0xbeef;}
    if(test_case==20) {c_setPG(0);return test_fault==0 && test_irq==1 && test_irq_eip==0x1007 && !(pic_isr&1) && c_getAX()==0xbeef && !(*(ULONG *)c_GetPhyAdd(0x53c4)&4);}
    if(test_case==7) return test_fault==0 && test_irq==1 && test_irq_eip==0x1001 && !(pic_isr&1) && c_getAX()==0xbeef;
    if(test_case==6) return test_fault==0 && test_irq==1 && test_irq_eip==0x1001 && !(pic_isr&1) && c_getAX()==0xbeef;
    if (test_case==5) {
        char line[96];
        wsprintfA(line,"MONITOR_SLAVE_EOI master=%02x slave=%02x\n",pic_isr,slave_isr);report(line);
        return test_fault==0 && test_irq==1 && test_irq_eip==0x1001 && !(pic_isr&4) && !(slave_isr&1) && c_getAX()==0xbeef;
    }
    if (test_case==4) {
        char line[96];
        wsprintfA(line,"MONITOR_AEOI callback_isr=%02lx\n",test_irq_isr);report(line);
        return test_irq==1 && test_irq_eip==0x1001 && !(test_irq_isr&1) && !(pic_isr&1) && c_getAX()==0xbeef;
    }
    return test_fault==0 && !(pic_isr & 1) && c_getAX()==0xbeef && (test_case==0 ? (test_irq==1 && test_irq_eip==0x1001 && *c_GetPhyAdd(0x301)==1 && test_out==0 && !(pic_irr&1)) : (test_irq==0 && *c_GetPhyAdd(0x301)==0 && test_out==0 && ((pic_irr&1)==(test_case==2 ? 1 : 0))));
}

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
extern ULONG c_getEAX(void);
extern USHORT c_getCS(void), c_getSP(void);
extern void INTx(ULONG);
extern void ZFRSRVD(ULONG);
static BOOL swint_origin_probe(ULONG vector) {test_irq=vector; return TRUE;}

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

static int v86_decoded_interrupt(void) {
    unsigned char *gdt=c_GetPhyAdd(0x5000), *ldt=c_GetPhyAdd(0x3000);
    unsigned vector=test_case==122 ? 0x2a : 0x31;
    unsigned char code[]={0xcd,0x31,0xd6,0xfe};
    const unsigned char prefixes[]={0x26,0x2e,0x36,0x3e,0x64,0x65};
    unsigned char prefixed[]={0x26,0xcd,0x31,0xd6,0xfe};
    BOOL segment_prefix=test_case>=168 && test_case<=174;
    BOOL multiple_prefix=test_case>=175 && test_case<=179;
    unsigned char grouped[17]; unsigned group_length=0;
    unsigned char target[]={0xbb,0xef,0xbe,0xd6,0xfe};
    ULONG saved; BOOL ok; char line[160];
    c_cpu_init(); ZeroMemory(gdt,0x408); ZeroMemory(ldt,0x408);
    *(ULONG *)(gdt+0x200)=0x0000ffff; *(ULONG *)(gdt+0x204)=0x0000f300;
    *(ULONG *)(ldt+0x18)=0x0000ffff; *(ULONG *)(ldt+0x1c)=0x0000fa00;
    c_setGDT_BASE_LIMIT(0x5000,0x407); c_setPE(1); c_setVM(1); c_setCPL(3);
    c_setCS(0x100); c_setSS(0x200); c_setSP(0x800); c_setIP(0x100);
    c_setIOPL(3); c_setIF(1); c_setBX(0);
    IntelBase=(ULONG)(ULONG_PTR)c_GetPhyAdd(0); Ldt=(LDT_ENTRY *)ldt;
    Cpu40LdtShadowAddress=0x3000; CurrentAppFlags=0;
    if(test_case==147) {
        BOOL handled;
        DpmiInterruptHandlers[0x2a].cs=0;
        DpmiInterruptHandlers[0x2a].flags=4;
        c_setEAX(0xf0f0f0f1); c_setEBP(0xf0f0f0f1);
        saved=c_getEFLAGS();
        VdmInstallSoftwareIntHandler((void *)DpmiSwIntHandler);
        handled=host_swint_hook(0x2a);
        VdmInstallSoftwareIntHandler(NULL);
        wsprintfA(line,"V86_HOOKED_INT2A handled=%d eax=%lx flags=%lx\n",handled,c_getEAX(),c_getEFLAGS());
        report(line);
        return !handled && c_getEAX()==0xf0f0f0f1 && c_getEFLAGS()==saved &&
               c_getCS()==0x100 && c_getSP()==0x800 && c_getIP()==0x100;
    }
    if(test_case==123) {
        test_irq=0; VdmInstallSoftwareIntHandler((void *)swint_origin_probe);
        INTx(7); VdmInstallSoftwareIntHandler(NULL);
        return test_irq==7 && c_getCS()==0x100 && c_getIP()==0x100 && c_getSP()==0x800;
    }
    if(test_case==125) {
        test_irq=0; VdmInstallSoftwareIntHandler((void *)swint_origin_probe);
        c_setCR0(c_getCR0() | 4u); /* EM: real x87 prologue takes INTx(7). */
        ZFRSRVD(0); VdmInstallSoftwareIntHandler(NULL);
        return test_irq==7 && c_getCS()==0x100 && c_getIP()==0x100 && c_getSP()==0x800;
    }
    if(test_case==124) {
        code[1]=7; CopyMemory(c_GetPhyAdd(0x1100),code,sizeof(code));
        test_irq=0; VdmInstallSoftwareIntHandler((void *)swint_origin_probe);
        c_cpu_simulate(); VdmInstallSoftwareIntHandler(NULL);
        return test_irq==7 && c_getCS()==0x100 && c_getSP()==0x800;
    }
    if(test_case==126) c_setIP(0xfffe);
    if(test_case==174) c_setIP(0xfffd);
    if(test_case==179) c_setIP(0xfffc);
    if(test_case==127) c_setSP(4);
    DpmiInterruptHandlers[vector].cs=0x1f;
    DpmiInterruptHandlers[vector].flags=0; DpmiInterruptHandlers[vector].eip=0x200;
    if(test_case==122) {c_setEAX(0xf0f0f0f1); c_setEBP(0xf0f0f0f1); c_setEBX(0);}
    code[1]=(unsigned char)vector;
    if(multiple_prefix) {
        if(test_case==175) { grouped[0]=0x67; group_length=1; }
        else if(test_case==178) { memset(grouped,0x26,13); group_length=13; }
        else { grouped[0]=test_case==176 ? 0x26 : 0x67; grouped[1]=0x2e; group_length=2; }
        grouped[group_length]=0xcd; grouped[group_length+1]=0x31;
        grouped[group_length+2]=0xd6; grouped[group_length+3]=0xfe;
        CopyMemory(c_GetPhyAdd(test_case==179 ? 0x10ffc : 0x1100),grouped,group_length+4);
    } else if(segment_prefix) {
        prefixed[0]=prefixes[test_case==174 ? 1 : test_case-168];
        CopyMemory(c_GetPhyAdd(test_case==174 ? 0x10ffd : 0x1100),prefixed,sizeof(prefixed));
    } else CopyMemory(c_GetPhyAdd(test_case==126 ? 0x10ffe : 0x1100),code,sizeof(code));
    CopyMemory(c_GetPhyAdd(0x3456),target,sizeof(target));
    *(USHORT *)c_GetPhyAdd(vector*4)=0x456; *(USHORT *)c_GetPhyAdd(vector*4+2)=0x300;
    saved=c_getEFLAGS(); VdmInstallSoftwareIntHandler((void *)DpmiSwIntHandler);
    c_cpu_simulate(); VdmInstallSoftwareIntHandler(NULL);
    ok=c_getCS()==0x300 && c_getSP()==(test_case==127 ? 0xfffe : 0x7fa) && c_getBX()==0xbeef &&
       *(USHORT *)c_GetPhyAdd(test_case==127 ? 0x11ffe : 0x27fa)==((test_case==126 || test_case==174 || test_case==179) ? 0 : (multiple_prefix ? 0x102+group_length : (segment_prefix ? 0x103 : 0x102))) &&
       *(USHORT *)c_GetPhyAdd(test_case==127 ? 0x12000 : 0x27fc)==0x100 &&
       *(USHORT *)c_GetPhyAdd(test_case==127 ? 0x12002 : 0x27fe)==(USHORT)saved &&
       !(c_getEFLAGS() & 0x4300) &&
       (test_case!=122 || c_getEAX()==0xf0f0f0f1);
    wsprintfA(line,"V86_DECODED_INT case=%d pass=%d cs=%x sp=%x bx=%x\n",
              test_case,ok,c_getCS(),c_getSP(),c_getBX()); report(line);
    return ok;
}

/* Unsupported prefixes must retain the original CCPU fallback even when a
 * DPMI software hook and a valid V86 IVT target are both installed. */
static BOOL v86_prefix_fallback_fault(ULONG vector, ULONG error) {
    (void)vector; (void)error;
    ++test_fault; c_setCS(0x300); c_setIP(0x456); return TRUE;
}
static int v86_unsupported_int_prefix(void) {
    const unsigned char prefixes[]={0xf0,0xf1,0xf2,0xf3,0x66};
    unsigned char code[]={0xf0,0x26,0xcd,0x31,0xd6,0xfe};
    unsigned char overlength[18];
    const unsigned char target[]={0xbb,0xef,0xbe,0xd6,0xfe};
    unsigned char *gdt=c_GetPhyAdd(0x5000), *ldt=c_GetPhyAdd(0x3000);
    BOOL ok; char line[120];
    c_cpu_init(); ZeroMemory(gdt,0x408); ZeroMemory(ldt,0x408);
    c_setGDT_BASE_LIMIT(0x5000,0x407); c_setPE(1); c_setVM(1); c_setCPL(3);
    c_setCS(0x100); c_setSS(0x200); c_setSP(0x800); c_setIP(0x100);
    c_setIOPL(3); c_setIF(1); c_setBX(0);
    IntelBase=(ULONG)(ULONG_PTR)c_GetPhyAdd(0); Ldt=(LDT_ENTRY *)ldt;
    Cpu40LdtShadowAddress=0x3000; CurrentAppFlags=0;
    DpmiInterruptHandlers[0x31].cs=0; DpmiInterruptHandlers[0x31].flags=0;
    if(test_case==185) {
        memset(overlength,0x26,14); overlength[14]=0xcd; overlength[15]=0x31;
        overlength[16]=0xd6; overlength[17]=0xfe;
        CopyMemory(c_GetPhyAdd(0x1100),overlength,sizeof(overlength));
    } else {
        code[0]=prefixes[test_case-180]; CopyMemory(c_GetPhyAdd(0x1100),code,sizeof(code));
    }
    CopyMemory(c_GetPhyAdd(0x3456),target,sizeof(target));
    *(USHORT *)c_GetPhyAdd(0x31*4)=0x456; *(USHORT *)c_GetPhyAdd(0x31*4+2)=0x300;
    test_fault=0; VdmInstallSoftwareIntHandler((void *)DpmiSwIntHandler);
    VdmInstallFaultHandler((void *)v86_prefix_fallback_fault);
    c_cpu_simulate(); VdmInstallFaultHandler(NULL); VdmInstallSoftwareIntHandler(NULL);
    ok=test_fault>0 && c_getCS()==0x300 && c_getSP()==0x800 && c_getBX()==0xbeef;
    wsprintfA(line,"V86_PREFIX_FALLBACK case=%d pass=%d faults=%lu\n",test_case,ok,test_fault);
    report(line); return ok;
}

/* NT4 handles one-byte INT3 and taken INTO as exceptions, unlike CD nn. */
static int v86_trap_instruction(void) {
    unsigned char code[]={0xcc,0xd6,0xfe};
    const unsigned char target[]={0xbb,0xef,0xbe,0xd6,0xfe};
    unsigned vector=test_case==157 ? 3 : 4;
    unsigned char *gdt=c_GetPhyAdd(0x5000), *ldt=c_GetPhyAdd(0x3000);
    BOOL taken=test_case!=159, ok; ULONG flags; char line[160];
    extern BOOL DpmiFaultHandler(ULONG, ULONG);
    c_cpu_init(); ZeroMemory(gdt,0x408); ZeroMemory(ldt,0x408);
    c_setGDT_BASE_LIMIT(0x5000,0x407); c_setPE(1); c_setVM(1); c_setCPL(3);
    c_setCS(0x100); c_setSS(0x200); c_setSP(0x800); c_setIP(0x100);
    c_setIOPL(3); c_setIF(1); c_setBX(0);
    flags=c_getEFLAGS(); if(vector==4 && taken) flags|=0x800; else flags&=~0x800u;
    c_setEFLAGS(flags); if(vector==4) code[0]=0xce;
    IntelBase=(ULONG)(ULONG_PTR)c_GetPhyAdd(0); Ldt=(LDT_ENTRY *)ldt;
    Cpu40LdtShadowAddress=0x3000; CurrentAppFlags=0;
    DpmiFaultHandlers[vector].cs=0; DpmiFaultHandlers[vector].flags=0;
    CopyMemory(c_GetPhyAdd(0x1100),code,sizeof(code));
    CopyMemory(c_GetPhyAdd(0x3456),target,sizeof(target));
    *(USHORT *)c_GetPhyAdd(vector*4)=0x456; *(USHORT *)c_GetPhyAdd(vector*4+2)=0x300;
    test_irq=0; VdmInstallSoftwareIntHandler((void *)swint_origin_probe);
    VdmInstallFaultHandler((void *)DpmiFaultHandler);
    c_cpu_simulate(); VdmInstallFaultHandler(NULL); VdmInstallSoftwareIntHandler(NULL);
    ok=test_irq==0 && (taken ? (c_getCS()==0x300 && c_getSP()==0x7fa && c_getBX()==0xbeef &&
       *(USHORT *)c_GetPhyAdd(0x27fa)==0x101 && *(USHORT *)c_GetPhyAdd(0x27fc)==0x100 &&
       *(USHORT *)c_GetPhyAdd(0x27fe)==(USHORT)flags) :
       (c_getCS()==0x100 && c_getSP()==0x800 && c_getBX()==0));
    wsprintfA(line,"V86_TRAP_INSN case=%d pass=%d sw=%lu cs=%x sp=%x bx=%x\n",
              test_case,ok,test_irq,c_getCS(),c_getSP(),c_getBX()); report(line);
    return ok;
}

/* CD 03 and CD 04 remain software INT instructions, not the one-byte traps. */
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

static int v86_real_divide_fault(void) {
    unsigned char *gdt=c_GetPhyAdd(0x5000), *ldt=c_GetPhyAdd(0x3000);
    const unsigned char code[]={0xbb,0x00,0x00,0xf7,0xf3,0xd6,0xfe};
    const unsigned char npx_code[]={0xd9,0xe8,0xd6,0xfe};
    const unsigned char target[]={0xbb,0xef,0xbe,0xd6,0xfe};
    extern BOOL DpmiFaultHandler(ULONG, ULONG);
    ULONG flags; BOOL ok, gate=(test_case==136 || test_case==138); char line[150];
    c_cpu_init(); ZeroMemory(gdt,0x408); ZeroMemory(ldt,0x408);
    if(gate) InitializeIcaLock();
    c_setGDT_BASE_LIMIT(0x5000,0x407); c_setPE(1); c_setVM(1); c_setCPL(3);
    c_setCS(0x100); c_setSS(0x200); c_setSP(gate ? 4 : 0x800); c_setIP(0x100);
    c_setIOPL(3); c_setIF(1); c_setBX(0);
    IntelBase=(ULONG)(ULONG_PTR)c_GetPhyAdd(0); Ldt=(LDT_ENTRY *)ldt;
    Cpu40LdtShadowAddress=0x3000; CurrentAppFlags=0;
    DpmiFaultHandlers[test_case==137 ? 7 : 0].cs=0;
    DpmiFaultHandlers[test_case==137 ? 7 : 0].flags=gate ? 1 : 0;
    if(test_case==137) {
        CopyMemory(c_GetPhyAdd(0x1100),npx_code,sizeof(npx_code));
        c_setCR0(c_getCR0() | 4u);
    } else CopyMemory(c_GetPhyAdd(0x1100),code,sizeof(code));
    CopyMemory(c_GetPhyAdd(0x3456),target,sizeof(target));
    *(USHORT *)c_GetPhyAdd(test_case==137 ? 28 : 0)=0x456;
    *(USHORT *)c_GetPhyAdd(test_case==137 ? 30 : 2)=0x300;
    if(test_case==138 || test_case==139) *(ULONG *)c_GetPhyAdd(0x714)=0xa203;
    VdmInstallFaultHandler((void *)DpmiFaultHandler);
    c_cpu_simulate(); VdmInstallFaultHandler(NULL);
    flags=c_getEFLAGS();
    ok=c_getCS()==0x300 && c_getSP()==(gate ? 0xfffe : 0x7fa) && c_getBX()==0xbeef &&
       *(USHORT *)c_GetPhyAdd(gate ? 0x11ffe : 0x27fa)==(test_case==137 ? 0x100 : 0x103) &&
       *(USHORT *)c_GetPhyAdd(gate ? 0x2000 : 0x27fc)==0x100 &&
       (*(USHORT *)c_GetPhyAdd(gate ? 0x2002 : 0x27fe) & 0x3200)==0x3200 &&
       (flags & 0x20000) && ((flags & 0x200)==(gate ? 0u : 0x200u)) &&
       ((test_case!=138 && test_case!=139) ||
        *(ULONG *)c_GetPhyAdd(0x714)==(test_case==138 ? 0xa003u : 0xa203u));
    wsprintfA(line,"V86_REAL_FAULT case=%d pass=%d cs=%x sp=%x bx=%x flags=%lx\n",
              test_case,ok,c_getCS(),c_getSP(),c_getBX(),flags); report(line);
    if(test_case==138 || test_case==139) {wsprintfA(line,"V86_FIXED_IF state=%lx cpu_flags=%lx\n",*(ULONG *)c_GetPhyAdd(0x714),flags);report(line);}
    return ok;
}

static BOOL v86_iret_irq_fault(ULONG vector, ULONG error) {
    extern BOOL DpmiFaultHandler(ULONG, ULONG);
    BOOL handled=DpmiFaultHandler(vector,error);
    if (handled) ica_hw_interrupt(0,0,1);
    return handled;
}
static BOOL v86_iret_irq_handler(ULONG vector) {
    ++test_irq; test_irq_eip=c_getEIP();
    ica_outb(0x20,0x20);
    return vector==8;
}
static LONG v86_installed_exception(EXCEPTION_POINTERS *ep);
static int v86_fault_iret(void) {
    unsigned char *gdt=c_GetPhyAdd(0x5000), *ldt=c_GetPhyAdd(0x3000);
    const unsigned char code[]={0xbb,0,0,0xf7,0xf3,0xbb,0xef,0xbe,0xd6,0xfe};
    const unsigned char handler[]={0x89,0xe5,0x83,0x46,0,2,0xcf};
    extern BOOL DpmiFaultHandler(ULONG, ULONG);
    ULONG flags; BOOL ok; char line[150];
    c_cpu_init(); InitializeIcaLock(); ZeroMemory(gdt,0x408); ZeroMemory(ldt,0x408);
    if(test_case==141) {
        ica0_init(); ica1_init(); DelayIrqLine=0; test_irq=test_irq_eip=0;
        ica_outb(0x20,0x11); ica_outb(0x21,8); ica_outb(0x21,4);
        ica_outb(0x21,1); ica_outb(0x21,0xfe);
        VdmInstallHardwareIntHandler((void *)v86_iret_irq_handler);
    }
    c_setGDT_BASE_LIMIT(0x5000,0x407); c_setPE(1); c_setVM(1); c_setCPL(3);
    c_setCS(0x100); c_setSS(0x200); c_setSP(0x800); c_setIP(0x100);
    c_setIOPL(3); c_setIF(1); c_setBX(0);
    if(test_case==142) {c_sas_enable_20_bit_wrapping(); c_setSS(0xffff); c_setSP(0x20);}
    if(test_case==143) {c_sas_disable_20_bit_wrapping(); c_setSS(0xffff); c_setSP(0x20);}
    if(test_case==144) {c_sas_enable_20_bit_wrapping(); c_setSS(0xffff); c_setSP(0x11);}
    if(test_case==144) {
        extern int c_sas_memory_type(ULONG);
        char diag[100];
        wsprintfA(diag,"A20_ODD_SAS types=%d,%d\n",c_sas_memory_type(0xfffff),c_sas_memory_type(0x100000));
        report(diag);
    }
    IntelBase=(ULONG)(ULONG_PTR)c_GetPhyAdd(0); Ldt=(LDT_ENTRY *)ldt;
    Cpu40LdtShadowAddress=0x3000; CurrentAppFlags=0;
    DpmiFaultHandlers[0].cs=0; DpmiFaultHandlers[0].flags=1;
    /* CPU40 initializes the historical software-CPU selector bit. */
    *(ULONG *)c_GetPhyAdd(0x714)=0xa600;
    CopyMemory(c_GetPhyAdd(0x1100),code,sizeof(code));
    CopyMemory(c_GetPhyAdd(test_case==144 ? 0x3432 : 0x3456),handler,sizeof(handler));
    /* The wrapped high byte of saved FLAGS aliases IVT[0]'s low byte.
     * Keep its intended value 0x32 so the architecturally ordered IVT read
     * still finds the handler after the stack write. */
    *(USHORT *)c_GetPhyAdd(0)=test_case==144 ? 0x432 : 0x456;
    *(USHORT *)c_GetPhyAdd(2)=0x300;
    VdmInstallFaultHandler((void *)(test_case==141 ? v86_iret_irq_fault : DpmiFaultHandler));
    if(test_case==144) {
        __try {c_cpu_simulate();}
        __except(v86_installed_exception(GetExceptionInformation())) {VdmInstallFaultHandler(NULL); return 0;}
    } else c_cpu_simulate();
    VdmInstallFaultHandler(NULL);
    if(test_case==141) VdmInstallHardwareIntHandler(NULL);
    flags=c_getEFLAGS();
    ok=c_getCS()==0x100 && c_getSP()==(test_case==144 ? 0x11 : test_case>=142 ? 0x20 : 0x800) && c_getBX()==0xbeef &&
       (flags & 0x20000) && (flags & 0x200) &&
       *(ULONG *)c_GetPhyAdd(0x714)==0xa400;
    if(test_case==141) {
        char irqline[100];
        wsprintfA(irqline,"V86_IRET_IRQ count=%lu eip=%lx\n",test_irq,test_irq_eip);
        /* The original CCPU IRET path defers interrupt checks until after
         * the next instruction, so MOV BX completes before IRQ delivery. */
        report(irqline); ok=ok && test_irq==1 && test_irq_eip==0x108;
    }
    wsprintfA(line,"V86_FAULT_IRET pass=%d cs=%x sp=%x bx=%x flags=%lx fixed=%lx\n",
              ok,c_getCS(),c_getSP(),c_getBX(),flags,*(ULONG *)c_GetPhyAdd(0x714));
    if(test_case==142 || test_case==144) c_sas_disable_20_bit_wrapping();
    report(line); return ok;
}

static ULONG v86_fault_flags, v86_fault_sp, v86_fault_ip;
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
static ULONG v86_installed_flags, v86_installed_sp, v86_installed_cs, v86_installed_ip;
static BOOL v86_installed_handled;
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
static USHORT v86_fault_partial_cs, v86_fault_partial_flags, v86_fault_partial_ip;
static BOOL v86_stack_page_fault(ULONG vector, ULONG error) {
    ULONG *pte=(ULONG *)c_GetPhyAdd(0x9000);
    if(vector!=14 || test_fault) return FALSE;
    ++test_fault; test_fault_error=error;
    v86_fault_flags=c_getEFLAGS(); v86_fault_sp=c_getSP(); v86_fault_ip=c_getEIP();
    v86_fault_partial_cs=*(USHORT *)c_GetPhyAdd(0x7000);
    v86_fault_partial_flags=*(USHORT *)c_GetPhyAdd(0x7002);
    v86_fault_partial_ip=*(USHORT *)c_GetPhyAdd(0x2ffe);
    pte[test_case==129 ? 3 : test_case==130 ? 2 : 0]=
        test_case==129 ? 0x7007 : test_case==130 ? 0x2007 : 0x0007;
    return TRUE;
}
static BOOL v86_observe_original_fault(ULONG vector, ULONG error) {
    extern BOOL DpmiFaultHandler(ULONG, ULONG);
    ULONG before=c_getEFLAGS(); BOOL handled;
    char line[160];
    if (test_case==133 || test_case==134) {
        wsprintfA(line,"V86_REAL_INSTALLED_ENTER vector=%lu error=%lx flags=%lx sp=%x ip=%lx\n",
                  vector,error,before,c_getSP(),c_getEIP()); report(line);
    }
    if(test_case==133 || test_case==134) {
        __try { handled=DpmiFaultHandler(vector,error); }
        __except(v86_installed_exception(GetExceptionInformation())) { handled=FALSE; }
    } else handled=DpmiFaultHandler(vector,error);
    wsprintfA(line,"V86_ORIGINAL_FAULT vector=%lu error=%lx handled=%d flags=%lx/%lx sp=%x ip=%lx\n",
              vector,error,handled,before,c_getEFLAGS(),c_getSP(),c_getEIP()); report(line);
    if (test_case==133 || test_case==134) {
        extern void c_cpu_unsimulate(void);
        v86_installed_handled=handled; v86_installed_flags=c_getEFLAGS();
        v86_installed_sp=c_getSP(); v86_installed_cs=c_getCS(); v86_installed_ip=c_getEIP();
        if(test_case==133) c_cpu_unsimulate(); /* Observe before executing a handler. */
    }
    return handled;
}

/* Two adjacent V86 linear stack pages need not be physically adjacent. */
static int v86_mapped_stack(void) {
    unsigned char *gdt=c_GetPhyAdd(0x5000), *ldt=c_GetPhyAdd(test_case>=133 ? 0x4000 : 0x3000);
    ULONG *pte=(ULONG *)c_GetPhyAdd(0x9000); ULONG flags, stack1, stack2, ivt; unsigned i; BOOL ok;
    const unsigned char code[]={0xcd,0x31,0xd6,0xfe};
    const unsigned char target[]={0xbb,0xef,0xbe,0xd6,0xfe};
    char line[180];
    c_cpu_init(); ZeroMemory(gdt,0x408); ZeroMemory(ldt,0x408);
    *(ULONG *)(gdt+0x200)=0x0000ffff; *(ULONG *)(gdt+0x204)=0x0000f300;
    *(ULONG *)(ldt+0x18)=0x0000ffff; *(ULONG *)(ldt+0x1c)=0x0000fa00;
    if(test_case>=133) { *(ULONG *)(ldt+0x10)=0x0000ffff; *(ULONG *)(ldt+0x14)=0x0000f300; }
    c_setGDT_BASE_LIMIT(0x5000,0x407);
    if(test_case>=133) { c_setLDT_SELECTOR(0x28); c_setLDT_BASE_LIMIT(0x4000,0x407); }
    c_setPE(1); c_setVM(1); c_setCPL(3);
    c_setCS(0x100); c_setSS(0x200); c_setSP(0x1004); c_setIP(0x100);
    c_setIOPL(3); c_setIF(1); c_setBX(0);
    IntelBase=(ULONG)(ULONG_PTR)c_GetPhyAdd(0); Ldt=(LDT_ENTRY *)ldt;
    Cpu40LdtShadowAddress=test_case>=133 ? 0x4000 : 0x3000; CurrentAppFlags=0;
    DpmiInterruptHandlers[0x31].cs=0x1f; DpmiInterruptHandlers[0x31].flags=0;
    DpmiInterruptHandlers[0x31].eip=0x200;
    CopyMemory(c_GetPhyAdd(0x1100),code,sizeof(code));
    CopyMemory(c_GetPhyAdd(0x7456),target,sizeof(target));
    if(test_case==134) {
        const unsigned char protected_target[]={0xbb,0xce,0xfa,0xd6,0xfe};
        CopyMemory(c_GetPhyAdd(0x745),target,sizeof(target));
        CopyMemory(c_GetPhyAdd(0x555),protected_target,sizeof(protected_target));
    }
    *(USHORT *)c_GetPhyAdd(0x31*4)=0x456; *(USHORT *)c_GetPhyAdd(0x31*4+2)=0x300;
    ZeroMemory(c_GetPhyAdd(0x8000),0x2000); *(ULONG *)c_GetPhyAdd(0x8000)=0x9007;
    for(i=0;i<512;i++)pte[i]=(i<<12)|7; pte[3]=0x7007;
    *(ULONG *)c_GetPhyAdd(0x7000)=0; *(USHORT *)c_GetPhyAdd(0x2ffe)=0;
    flags=c_getEFLAGS(); test_fault=0;
    if(test_case==129 || test_case==132 || test_case>=133)pte[3]=0;
    if(test_case==130)pte[2]=0;
    if(test_case==131)pte[0]=0;
    c_setCR3(0x8000); c_setPG(1); VdmInstallSoftwareIntHandler((void *)DpmiSwIntHandler);
    if(test_case>=129 && test_case<=131) VdmInstallFaultHandler((void *)v86_stack_page_fault);
    if(test_case>=133) {
        DpmiFaultHandlers[14].cs=0x1f; DpmiFaultHandlers[14].eip=0x555;
        LockedPMStackSel=0x17; LockedPMStackCount=0;
    }
    if(test_case>=132) VdmInstallFaultHandler((void *)v86_observe_original_fault);
    c_cpu_simulate(); VdmInstallSoftwareIntHandler(NULL); VdmInstallFaultHandler(NULL);
    stack1=pte[2]; stack2=pte[3]; ivt=pte[0]; c_setPG(0);
    if(test_case>=133) {
        wsprintfA(line,"V86_REAL_INSTALLED handled=%d flags=%lx sp=%lx cs=%lx ip=%lx\n",
                  v86_installed_handled,v86_installed_flags,v86_installed_sp,v86_installed_cs,v86_installed_ip);
        report(line); DpmiFaultHandlers[14].cs=0; LockedPMStackCount=0;
        if(test_case==134) { wsprintfA(line,"V86_REAL_INSTALLED_EXEC bx=%x flags=%lx\n",c_getBX(),c_getEFLAGS()); report(line); }
        return v86_installed_handled && (v86_installed_flags & 0x20000) &&
               v86_installed_sp==0xfd0 && v86_installed_cs==0x1f && v86_installed_ip==0x555 &&
               (test_case==133 || (c_getBX()==0xbeef && (c_getEFLAGS() & 0x20000)));
    }
    ok=c_getBX()==0xbeef && c_getSP()==0x0ffe &&
       *(USHORT *)c_GetPhyAdd(0x2ffe)==0x102 &&
       *(USHORT *)c_GetPhyAdd(0x7000)==0x100 &&
       *(USHORT *)c_GetPhyAdd(0x7002)==(USHORT)flags &&
       (stack1 & 0x60)==0x60 && (stack2 & 0x60)==0x60 && (ivt & 0x20)==0x20 &&
       ((test_case==128 && !test_fault) || (test_case!=128 && test_fault==1 &&
        test_fault_error==(test_case==131 ? 0u : 2u) && v86_fault_flags==flags && v86_fault_sp==0x1004 &&
        v86_fault_ip==0x100 &&
        v86_fault_partial_cs==(test_case==130 ? 0x100 : 0) &&
        v86_fault_partial_flags==(test_case==130 ? (USHORT)flags : 0) &&
        v86_fault_partial_ip==0));
    wsprintfA(line,"V86_MAPPED_STACK pass=%d ip=%04x cs=%04x flags=%04x expected=%04x raw-cs=%04x raw-flags=%04x pte=%lx/%lx/%lx\n",
              ok,*(USHORT *)c_GetPhyAdd(0x2ffe),*(USHORT *)c_GetPhyAdd(0x7000),
              *(USHORT *)c_GetPhyAdd(0x7002),(USHORT)flags,
              *(USHORT *)c_GetPhyAdd(0x3000),*(USHORT *)c_GetPhyAdd(0x3002),stack1,stack2,ivt); report(line);
    if(test_case!=128) {wsprintfA(line,"V86_STACK_FAULT case=%d count=%lu error=%lx flags=%lx sp=%lx ip=%lx partial=%x/%x/%x\n",
        test_case,test_fault,test_fault_error,v86_fault_flags,v86_fault_sp,v86_fault_ip,
        v86_fault_partial_ip,v86_fault_partial_cs,v86_fault_partial_flags);report(line);}
    return ok;
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
    setup_vga_globals();load_sw_cpu_access_functions();c_cpu_init();
    if(test_case==148 || test_case==149) ok=protected_fault_decline_cpu();
    else if(test_case>=157 && test_case<=159) ok=v86_trap_instruction();
    else if(test_case>=160 && test_case<=167) ok=v86_explicit_software_int();
    else if(test_case>=180 && test_case<=185) ok=v86_unsupported_int_prefix();
    else if(test_case==154) ok=dpmi_remapped_decode_fallback();
    else if(test_case==155 || test_case==156) ok=dpmi_remapped_code_page();
    else if(test_case==146) ok=vdd_sas_unbound_control();
    else if(test_case==145) ok=v86_a20_word_ram_control();
    else if(test_case>=140 && test_case<=144) ok=v86_fault_iret();
    else if(test_case>=135 && test_case<=139) ok=v86_real_divide_fault();
    else if(test_case>=128 && test_case<=134) ok=v86_mapped_stack();
    else if((test_case>=121 && test_case<=127) || test_case==147 || (test_case>=168 && test_case<=179)) ok=v86_decoded_interrupt();
    else if(test_case>=113 && test_case<=116) ok=broadcast_debug_execution();
    else if((test_case>=109 && test_case<=112) || test_case==117) ok=restored_debug_execution();
    else if(test_case==107 || test_case==108) ok=dpmi_faulting_decode_page();
    else if(test_case==102 || test_case==103) ok=monitor_v86_mode_boundary();
    else if((test_case>=93 && test_case<=101) || (test_case>=104 && test_case<=106) || (test_case>=150 && test_case<=153)) ok=dpmi_remapped_code_page();
    else if(test_case>=90 && test_case<=92) ok=monitor_prefix_length();
    else if(test_case==89) ok=monitor_wrapped_instruction_offset();
    else if(test_case==87 || test_case==88) ok=monitor_fetch_page_access();
    else if(test_case==86) ok=monitor_cr0_npx_matrix();
    else if(test_case==49) ok=duplicate_tlb_mapping();
    else if(test_case==83) ok=monitor_register_matrix();
    else if((test_case>=77 && test_case<=81) || test_case==84 || test_case==85 || (test_case>=118 && test_case<=120)) ok=protected_ldt_descriptor();
    else if(test_case==76) ok=monitor_cache_contract();
    else if(test_case==73 || test_case==74) ok=protected_missing_descriptor_page();
    else if(test_case>=69 && test_case<=72) ok=protected_string_segment_limit();
    else if(test_case==67 || test_case==68 || test_case==75) ok=protected_changed_descriptor();
    else if(test_case==64 || test_case==65 || test_case==66) ok=protected_wrapped_string();
    else if(test_case==62 || test_case==63) ok=protected_wrapped_cs();
    else if(test_case==60 || test_case==61) ok=protected_split_word_string();
    else if ((test_case>=10 && test_case<=14) || test_case==19 || test_case==28 || (test_case>=31 && test_case<=48) || test_case==50 || test_case==51 || test_case==53 || test_case==54 || test_case==55 || test_case==56 || test_case==57 || test_case==58) ok=protected_scalar_io();
    else if (test_case==15 || (test_case>=21 && test_case<=25) || test_case==29 || test_case==52 || test_case==59 || test_case==82) ok=protected_string_reverse_baseline();
    else if (test_case==16) ok=protected_concurrent_if();
    else if ((test_case>=0 && test_case<=7) || test_case==17 || test_case==18 || test_case==20 || test_case==26 || test_case==27 || test_case==30) ok=protected_pending_irq_observe();
    else return 5;
    return ok ? 0 : 1;
}
