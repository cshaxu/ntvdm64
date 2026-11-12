/* Actual original BIOS/PIC/IOS owners; only the external EOI callback is
 * observed here. IRQs enter service through the actual PIC INTACK. */
#include <stdio.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <ica.h>
#include "ntvdm-exe/session/session.h"

extern void sas_init(PHY_ADDR), sas_term(void), c_cpu_init(void);
extern void ica0_init(void), ica1_init(void);
extern void io_init(void);
extern ULONG DelayIrqLine;
extern void ica_outb(io_addr, IU8);
extern IS32 ica_intack(IU32 *);
extern VDMVIRTUALICA VirtualIca[2];
extern void D11_int(void), re_direct(void), int_287(void);
extern void SoftPcEoi(int, int *);
static unsigned checks, failures, eois[16];
#define CHECK(x) do { ++checks; if(!(x)) { ++failures; fprintf(stderr,"FAIL line=%u: %s\n",(unsigned)__LINE__,#x); } } while(0)
void host_EOI_hook(int irq, int count)
{ if(irq>=0 && irq<16 && count>=0) ++eois[irq]; }
ULONG host_DelayHwInterrupt(int irq, int count, ULONG delay)
{ (void)irq; (void)count; (void)delay; return 0; }
void HostIdleNoActivity(void) { }
static void initialize(int aeoi)
{
    unsigned i;
    ica_outb(0x20,0x11); ica_outb(0x21,8); ica_outb(0x21,4);
    ica_outb(0x21,(IU8)(aeoi?3:1)); ica_outb(0x21,0xff);
    ica_outb(0xa0,0x11); ica_outb(0xa1,0x70); ica_outb(0xa1,2);
    ica_outb(0xa1,(IU8)(aeoi?3:1)); ica_outb(0xa1,0xff);
    for(i=0;i<16;i++) eois[i]=0;
}
static void slave_request(unsigned line)
{
    IU32 hook=0;
    ica_outb(0x21,0xfb); ica_outb(0xa1,(IU8)~(1u<<line));
    ica_hw_interrupt(ICA_SLAVE,line,1);
    CHECK(ica_intack(&hook)==(IS32)(0x70+line));
}
int main(void)
{
    session owner;
    IU32 hook=0;
    unsigned line, round, before;
    session_initialize(&owner,1);
    if(!session_activate(&owner)||!session_thread_bind(&owner)) return 2;
    sas_init(0x200000); c_cpu_init(); io_init(); ica0_init(); ica1_init();
    /* Match the existing keyboard/PIC fixture's post-readiness boundary. */
    DelayIrqLine=0;
    initialize(0);
    D11_int();
    CHECK(VirtualIca[0].ica_isr==0 && VirtualIca[1].ica_isr==0);
    CHECK(VirtualIca[0].ica_imr==0xff && VirtualIca[1].ica_imr==0xff);
    CHECK(sas_hw_at(0x46b)==0xff);
    initialize(0); ica_outb(0x21,0xf7);
    ica_hw_interrupt(ICA_MASTER,3,1);
    CHECK(ica_intack(&hook)==11);
    D11_int();
    CHECK(VirtualIca[0].ica_isr==0 && VirtualIca[0].ica_imr==0xff);
    CHECK(eois[3]==1 && sas_hw_at(0x46b)==8);
    for(line=2;line<8;line++) {
        if(line==5) continue; /* IRQ13 has its separate original x87 owner. */
        initialize(0);
        for(round=0;round<2;round++) {
            slave_request(line);
            CHECK(VirtualIca[0].ica_isr==4 && VirtualIca[1].ica_isr==(1u<<line));
            before=eois[8+line];
            D11_int();
            fprintf(stderr,"D11 irq=%u round=%u master=%02x slave=%02x mask=%02x callbacks=%u\n",
                8+line,round,VirtualIca[0].ica_isr,VirtualIca[1].ica_isr,VirtualIca[1].ica_imr,eois[8+line]-before);
            CHECK(VirtualIca[0].ica_isr==0 && VirtualIca[1].ica_isr==0);
            CHECK(VirtualIca[1].ica_imr==0xff && VirtualIca[0].ica_imr==0xfb);
            CHECK(eois[8+line]==before+1);
            CHECK(sas_hw_at(0x46b)==4);
            if(VirtualIca[1].ica_isr) break; /* Preserve baseline failure; do not fabricate an EOI. */
        }
    }
    initialize(1); slave_request(3);
    CHECK(VirtualIca[0].ica_isr==0 && VirtualIca[1].ica_isr==0);
    before=eois[11]; D11_int();
    CHECK(eois[11]==before && VirtualIca[1].ica_imr==0xf7);
    CHECK(sas_hw_at(0x46b)==0xff);
    initialize(0); slave_request(1); re_direct();
    CHECK(VirtualIca[1].ica_isr==0 && VirtualIca[0].ica_isr==4 && eois[9]==1);
    ica_outb(0x20,0x20);
    initialize(0); slave_request(5); int_287();
    CHECK(VirtualIca[1].ica_isr==0 && VirtualIca[0].ica_isr==0 && eois[13]==1);
    /* Redirector ABI: nonspecific slave then master EOI reports each line.
     * With no ISR bit, -1 stays -1 and no callback is fabricated. */
    initialize(0); slave_request(3);
    {
        int selected = -1;
        before = eois[11];
        SoftPcEoi(ICA_SLAVE, &selected);
        CHECK(selected == 3 && VirtualIca[1].ica_isr == 0);
        CHECK(VirtualIca[0].ica_isr == 4 && eois[11] == before + 1);
        selected = -1;
        SoftPcEoi(ICA_MASTER, &selected);
        CHECK(selected == 2 && VirtualIca[0].ica_isr == 0);
        before = eois[11]; selected = -1;
        SoftPcEoi(ICA_SLAVE, &selected);
        CHECK(selected == -1 && eois[11] == before);
    }
    initialize(0);
    sas_term(); (void)session_thread_unbind(&owner); (void)session_dispose(&owner);
    fprintf(stderr,"SLAVE_BIOS_EOI checks=%u failures=%u\n",checks,failures);
    return failures?1:0;
}
