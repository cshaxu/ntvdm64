/* Exercise original mirror setup and actual production libraries.
 * No fixture calls or links a private overlay directly. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "insignia.h"
#include "host_def.h"
#include "egacpu.h"
#define CPU_PRIVATE
#include "cpu4.h"
#include "sas.h"
#include "Evid_c.h"
#include "gdpvar.h"
#include "j_c_lang.h"
#include "cpu_vid.h"
#include "ga_mark.h"
#include "evidfunc.h"

extern struct VideoVector C_Video;
extern struct SasVector Sas;
extern IHP Gdp;
extern IHP setup_global_data_ptr(void);
extern void setup_vga_globals(void);
extern void setReadPointers(IUH);
extern void setWritePointers(void);
extern void setMarkPointers(IUH);
extern void c_sas_overwrite_memory(PHY_ADDR, PHY_ADDR);
extern EVID_READ_POINTERS c_ev_read_ptr;
extern EVID_WRT_POINTERS c_ev_write_ptr;
static unsigned char expected[65536];
static IUH *scratch[] = { &r0,&r1,&r2,&r3,&r4,&r5,&r6,&r7,
    &r8,&r9,&r10,&r11,&r12,&r13,&r14,&r15,&r16,&r17,&r18,&r19,
    &r20,&r21,&r22,&r23,&r24,&r25,&r26,&r27,&r28,&r29,&r30,&r31 };
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x); return 1; } } while (0)
#define CHECK_FIELD(name,type,offset) do { \
    type value=(type)values[test]; \
    memset(Gdp,0xa5,65536); memcpy(expected,Gdp,65536); \
    memcpy(expected+(offset),&value,sizeof(value)); \
    setVideo##name(value); \
    CHECK(getVideo##name()==value); \
    CHECK(memcmp(Gdp,expected,65536)==0); \
    for(i=0;i<32;i++) CHECK(*scratch[i]==100+i); \
} while(0)
#define CHECK_READ_SELECTION() do { \
    CHECK(c_ev_read_ptr.b_read != 0 && c_ev_read_ptr.w_read != 0 && \
          c_ev_read_ptr.d_read != 0 && c_ev_read_ptr.str_fwd_read != 0 && \
          c_ev_read_ptr.str_bwd_read != 0); \
} while (0)
#define CHECK_WRITE_SELECTION() do { \
    CHECK(c_ev_write_ptr.b_write != 0 && c_ev_write_ptr.w_write != 0 && \
          c_ev_write_ptr.d_write != 0 && c_ev_write_ptr.b_fill != 0 && \
          c_ev_write_ptr.w_fill != 0 && c_ev_write_ptr.d_fill != 0 && \
          c_ev_write_ptr.b_fwd_move != 0 && c_ev_write_ptr.b_bwd_move != 0 && \
          c_ev_write_ptr.w_fwd_move != 0 && c_ev_write_ptr.w_bwd_move != 0 && \
          c_ev_write_ptr.d_fwd_move != 0 && c_ev_write_ptr.d_bwd_move != 0); \
} while (0)
#define CHECK_WRITE_TABLE(expected) do { \
    EVID_WRT_POINTERS *e=(expected); \
    CHECK(c_ev_write_ptr.b_write==e->b_write && c_ev_write_ptr.w_write==e->w_write && \
          c_ev_write_ptr.d_write==e->d_write && c_ev_write_ptr.b_fill==e->b_fill && \
          c_ev_write_ptr.w_fill==e->w_fill && c_ev_write_ptr.d_fill==e->d_fill && \
          c_ev_write_ptr.b_fwd_move==e->b_fwd_move && c_ev_write_ptr.b_bwd_move==e->b_bwd_move && \
          c_ev_write_ptr.w_fwd_move==e->w_fwd_move && c_ev_write_ptr.w_bwd_move==e->w_bwd_move && \
          c_ev_write_ptr.d_fwd_move==e->d_fwd_move && c_ev_write_ptr.d_bwd_move==e->d_bwd_move); \
} while (0)
#define CHECK_READ_TABLE(expected) do { \
    EVID_READ_POINTERS *e=(expected); \
    CHECK(c_ev_read_ptr.b_read==e->b_read && c_ev_read_ptr.w_read==e->w_read && \
          c_ev_read_ptr.d_read==e->d_read && c_ev_read_ptr.str_fwd_read==e->str_fwd_read && \
          c_ev_read_ptr.str_bwd_read==e->str_bwd_read && getVideoread_byte_addr()==(IHP)e->b_read); \
} while (0)

static DWORD WINAPI refresh(void *unused)
{
    unsigned i;
    (void)unused;
    for(i=0;i<100000;i++) setVideodirty_total(1000000);
    return 0;
}

int main(void)
{
    static const IUH values[]={0,0x5a,0x80000000,0xffffffff};
    unsigned i,test;
    HANDLE thread;
    DWORD written;
    CHECK(sizeof(struct VideoVector)==81*sizeof(void*));
    CHECK(setup_global_data_ptr()!=0);
    memset(Gdp,0,65536);
    setup_vga_globals();
    CHECK(Cpu.Video==(IHP)&Video && Cpu.Sas==&Sas && Cpu.Private!=0);
    CHECK(Sas.Sas_overwrite_memory==c_sas_overwrite_memory);
    CHECK(Cpu.ClearHwInt==0);
    CHECK(Cpu.SetSTATUS!=0);
    Cpu.SetSTATUS(0x7fd5);
    CHECK(Cpu.GetNT()==1 && Cpu.GetIOPL()==3 && Cpu.GetOF()==1 &&
          Cpu.GetDF()==1 && Cpu.GetIF()==1 && Cpu.GetTF()==1 &&
          Cpu.GetSF()==1 && Cpu.GetZF()==1 && Cpu.GetAF()==1 &&
          Cpu.GetPF()==1 && Cpu.GetCF()==1);
    Cpu.SetSTATUS(0);
    CHECK(Cpu.GetNT()==0 && Cpu.GetIOPL()==0 && Cpu.GetOF()==0 &&
          Cpu.GetDF()==0 && Cpu.GetIF()==0 && Cpu.GetTF()==0 &&
          Cpu.GetSF()==0 && Cpu.GetZF()==0 && Cpu.GetAF()==0 &&
          Cpu.GetPF()==0 && Cpu.GetCF()==0);
    CHECK(Cpu.Private->GetCpuState==0 && Cpu.Private->SetCpuState==0);
    CHECK(Video.GetVideolatches==C_Video.GetVideolatches);
    CHECK(Video.SetVideolatches==C_Video.SetVideolatches);
    CHECK(Video.setReadPointers==0 && Video.setWritePointers==0 && Video.setMarkPointers==0);
    CHECK(Cpu.GetJumpCalibrateVal()==0 && Cpu.GetJumpInitialVal()==100);
    Cpu.SetJumpInitialVal(73);
    setup_vga_globals();
    CHECK(Cpu.GetJumpCalibrateVal()==0 && Cpu.GetJumpInitialVal()==73);
    for(i=0;i<32;i++) *scratch[i]=100+i;
    for(test=0;test<sizeof(values)/sizeof(values[0]);test++) {
#include "cvidc_accessor_cases.inc"
    }
    memset(Gdp,0,65536); setup_vga_globals();
    /* Exercise every original C-VID table-selection family.  The fixture
     * verifies publication only; executing selected EGA memory routines is
     * the selected video package's S23 guest-workload responsibility. */
    Video.SetVideolatches(0x12345678); CHECK(Video.GetVideolatches()==0x12345678);
    for(i=UNCHAINED;i<=CHAIN_4;i++) {
        setVideochain(i);
        setReadPointers(READ_MODE_0); CHECK_READ_SELECTION(); CHECK_READ_TABLE(&read_mode0_evid[i]);
        setReadPointers(READ_MODE_1); CHECK_READ_SELECTION(); CHECK_READ_TABLE(&read_mode1_evid[i]);
    }
    setReadPointers(DISABLED_RAM); CHECK_READ_SELECTION(); CHECK_READ_TABLE(&ram_dsbld_read_evid);
    setReadPointers(SIMPLE_READ); CHECK_READ_SELECTION(); CHECK_READ_TABLE(&simple_read_evid);
    {
        EVID_MARK_POINTERS *marks[]={&simple_mark_evid,&cga_mark_evid,
            &unchained_mark_evid,&chain4_mark_evid};
        for(i=SIMPLE_MARK;i<=CHAIN_4_MARK;i++) {
            setMarkPointers(i);
            CHECK(getVideomark_byte()==(IHP)marks[i]->b_mark &&
                  getVideomark_word()==(IHP)marks[i]->w_mark &&
                  getVideomark_string()==(IHP)marks[i]->str_mark);
        }
        /* The original switch leaves the last selection intact for an
         * unknown selector. This is not acceptance of invalid chain indexes. */
        setMarkPointers(4);
        CHECK(getVideomark_byte()==(IHP)marks[CHAIN_4_MARK]->b_mark &&
              getVideomark_word()==(IHP)marks[CHAIN_4_MARK]->w_mark &&
              getVideomark_string()==(IHP)marks[CHAIN_4_MARK]->str_mark);
        setReadPointers(SIMPLE_READ); setReadPointers(4);
        CHECK_READ_TABLE(&simple_read_evid);
    }
    setVideodither(1);
    for(i=MODE_0;i<=MODE_3;i++) {
        setVideowrmode(i); setWritePointers(); CHECK_WRITE_SELECTION(); CHECK_WRITE_TABLE(&dith_evid[i]);
    }
    setVideodither(0);
    for(i=UNCHAINED;i<=CHAIN_4;i+=CHAIN_4) {
        setVideochain(i); setVideorotate(1); setWritePointers(); CHECK_WRITE_SELECTION(); CHECK_WRITE_TABLE(&gricvid_evid);
        setVideorotate(0); setVideowrstate(0);
        for(test=MODE_0;test<=MODE_3;test++) {
            EVID_WRT_POINTERS *table=(i==UNCHAINED?unchained_evid:chain4_evid);
            static const unsigned offsets[]={0,12,13,17};
            setVideowrmode(test); setWritePointers(); CHECK_WRITE_SELECTION(); CHECK_WRITE_TABLE(&table[offsets[test]]);
        }
    }
    setVideochain(CHAIN_2);
    for(i=MODE_0;i<=MODE_3;i++) {
        setVideowrmode(i); setWritePointers(); CHECK_WRITE_SELECTION(); CHECK_WRITE_TABLE(&chain2_evid[i]);
    }
    setVideochain(SIMPLE_WRITES); setWritePointers(); CHECK_WRITE_SELECTION(); CHECK_WRITE_TABLE(&simple_evid);
    /* Original table contract for each write state; these offsets identify
     * generated providers, not just non-null pointers. */
    {
        static const unsigned mode0_index[32]={
            0,1,2,3,2,3,2,3,0,1,2,3,2,3,2,3,
            8,9,10,11,10,11,6,7,8,9,10,11,10,11,10,11};
        static const unsigned mode23_index[16]={0,1,2,3,2,3,2,3,0,1,2,3,2,3,2,3};
        unsigned chain,mode,state,index;
        setVideodither(0); setVideorotate(0);
        for(chain=UNCHAINED;chain<=CHAIN_4;chain+=CHAIN_4) {
            EVID_WRT_POINTERS *table=(chain==UNCHAINED?unchained_evid:chain4_evid);
            setVideochain((IU8)chain);
            for(mode=MODE_0;mode<=MODE_3;mode++) {
                setVideowrmode((IU8)mode);
                for(state=0;state<32;state++) {
                    setVideowrstate((IU8)state); setWritePointers();
                    index=mode==0?mode0_index[state]:mode==1?12:
                        (mode==2?13:17)+mode23_index[state&15];
                    CHECK_WRITE_TABLE(&table[index]);
                }
            }
        }
        setVideochain(SIMPLE_WRITES); setWritePointers();
        CHECK(c_ev_write_ptr.b_write!=chain2_evid[0].b_write);
    }
    /* Negative control: the original generated setter reproduces the
     * shared-scratch clobber; the newly selected direct setter must not. */
    r21=121;
    C_Video.SetVideodirty_total(1000000);
    CHECK(r21!=121);
    for(i=0;i<32;i++) *scratch[i]=100+i;
    thread=CreateThread(NULL,0,refresh,NULL,0,NULL); CHECK(thread!=NULL);
    for(test=0;test<100000;test++) for(i=0;i<32;i++) CHECK(*scratch[i]==100+i);
    CHECK(WaitForSingleObject(thread,10000)==WAIT_OBJECT_0); CloseHandle(thread);
    CHECK(getVideodirty_total()==1000000);
    for(i=0;i<32;i++) CHECK(*scratch[i]==100+i);
    { const char message[]="PASS original C-VID setup: 38 pairs x 4 values, 81 slots, exact read/write/mark selection, 256 write-state cases, unknown read/mark selectors, negative control, concurrent scratch preservation\r\n";
      WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,sizeof(message)-1,&written,NULL); }
    return 0;
}
