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
extern void c_sas_stores(uint32_t, unsigned char *, uint32_t);
extern unsigned char *c_GetPhyAdd(uint32_t);
extern int debug_exception_pending;
extern void ica0_init(void), ica1_init(void);
extern void InitializeIcaLock(void);
extern int ica_intack(uint32_t *hook);
extern uint32_t CCPU_DR[8];
extern void MOV_DR(uint32_t, uint32_t);
extern void c_setGDT_BASE_LIMIT(uint32_t, uint16_t);
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
static volatile uint32_t frame_fault_vector, frame_fault_eip;
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
static unsigned io_reads, io_writes, string_reads, string_writes, first_output, string_visible_during_callback, string_output_uses_guest, string_word_direct;
static void probe_read(io_addr port, half_word *value) { (void)port; ++io_reads; *value=0xa5; }
static void probe_write(io_addr port, half_word value) { (void)port; if(!io_writes)first_output=value; ++io_writes; }
static void probe_string_read(io_addr port, half_word *out, word count) { (void)port;++string_reads;io_reads+=count;while(count--)*out++=0xa5; }
static void probe_string_read_visible(io_addr port, half_word *out, word count) { (void)port;++string_reads;io_reads+=count;if(count){*out=0xa5;string_visible_during_callback=(*c_GetPhyAdd(0x912)==0xa5);} }
static void probe_string_read_word(io_addr port, word *out, word count) { (void)port;++string_reads;io_reads+=count;string_word_direct=((void *)out==(void *)c_GetPhyAdd(0x7fff));if(count)*out=0x6ba5; }
static void probe_string_read_untouched(io_addr port, half_word *out, word count) { (void)port;(void)out;++string_reads;io_reads+=count; }
static void probe_string_write_wrapped(io_addr port, half_word *in, word count) { (void)port;++string_writes;io_writes+=count;if(count)first_output=*in; }
static void probe_string_write_visible(io_addr port, half_word *in, word count) { (void)port;++string_writes;io_writes+=count;string_output_uses_guest=(in==c_GetPhyAdd(0x902) && *in==0x42); }
static void probe_string_write(io_addr port, half_word *in, word count) { (void)port;(void)in;++string_writes;io_writes+=count; }
static BOOL test_sti_fault(unsigned long vector, unsigned long error) {
    (void)vector; (void)error; ++test_fault; return FALSE;
}
extern void c_setEIP(uint32_t);
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
    ++test_fault;frame_fault_eip=c_getEIP();c_setEIP(test_case==47 ? 0x101d : test_case==46 ? 0x101a : 0x1019);return TRUE;
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
    if(test_case==28)c_setES(0x2b);
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
        return !handled && test_fault==1 && !io_reads && !io_writes && c_getIP()==0x1000;
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
        ULONG *pte=(ULONG *)c_GetPhyAdd(0x5000),physical;unsigned j;int handled,cpu_allows;
        extern int xtrn2phy(unsigned long,unsigned char,unsigned long *);
        CopyMemory(c_GetPhyAdd(0xf1000),lmsw_mixed_permissions,sizeof(lmsw_mixed_permissions));
        ZeroMemory(c_GetPhyAdd(0x4000),0x2000);*(ULONG *)c_GetPhyAdd(0x4000)=test_case==58 ? 0x5003 : 0x5007;
        for(j=0;j<512;j++)pte[j]=(j<<12)|7;
        if(test_case==57)pte[2]=0x2003;
        *(uint16_t *)c_GetPhyAdd(0x2300)=6;
        c_setCR3(0x4000);c_setPG(1);cpu_allows=xtrn2phy(0x2300,2,&physical);
        handled=host_exint_hook(13,0);
        VdmInstallFaultHandler(NULL);c_setPG(0);
        return cpu_allows && physical==0x2300 && handled && test_fault==0 && c_getEIP()==0x1005 && (c_getCR0()&15)==15;
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
    if(test_case==28) return test_fault==1 && io_reads==0 && io_writes==0 && c_getAX()==0xbeef;
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
    io_connect_port((io_addr)0x80,adapter,IO_READ_WRITE);
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
    if(test_case==49) ok=duplicate_tlb_mapping();
    else if(test_case==64 || test_case==65 || test_case==66) ok=protected_wrapped_string();
    else if(test_case==62 || test_case==63) ok=protected_wrapped_cs();
    else if(test_case==60 || test_case==61) ok=protected_split_word_string();
    else if ((test_case>=10 && test_case<=14) || test_case==19 || test_case==28 || (test_case>=31 && test_case<=48) || test_case==50 || test_case==51 || test_case==53 || test_case==54 || test_case==55 || test_case==56 || test_case==57 || test_case==58) ok=protected_scalar_io();
    else if (test_case==15 || (test_case>=21 && test_case<=25) || test_case==29 || test_case==52 || test_case==59) ok=protected_string_reverse_baseline();
    else if (test_case==16) ok=protected_concurrent_if();
    else if ((test_case>=0 && test_case<=7) || test_case==17 || test_case==18 || test_case==20 || test_case==26 || test_case==27 || test_case==30) ok=protected_pending_irq_observe();
    else return 5;
    return ok ? 0 : 1;
}
