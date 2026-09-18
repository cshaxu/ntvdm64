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
        setReadPointers(READ_MODE_0); CHECK_READ_SELECTION();
        setReadPointers(READ_MODE_1); CHECK_READ_SELECTION();
    }
    setReadPointers(DISABLED_RAM); CHECK_READ_SELECTION();
    setReadPointers(SIMPLE_READ); CHECK_READ_SELECTION();
    for(i=SIMPLE_MARK;i<=CHAIN_4_MARK;i++) {
        setMarkPointers(i);
        CHECK(getVideomark_byte()!=0 && getVideomark_word()!=0 && getVideomark_string()!=0);
    }
    setVideodither(1);
    for(i=MODE_0;i<=MODE_3;i++) {
        setVideowrmode(i); setWritePointers(); CHECK_WRITE_SELECTION();
    }
    setVideodither(0);
    for(i=UNCHAINED;i<=CHAIN_4;i+=CHAIN_4) {
        setVideochain(i); setVideorotate(1); setWritePointers(); CHECK_WRITE_SELECTION();
        setVideorotate(0); setVideowrstate(0);
        for(test=MODE_0;test<=MODE_3;test++) {
            setVideowrmode(test); setWritePointers(); CHECK_WRITE_SELECTION();
        }
    }
    setVideochain(CHAIN_2);
    for(i=MODE_0;i<=MODE_3;i++) {
        setVideowrmode(i); setWritePointers(); CHECK_WRITE_SELECTION();
    }
    setVideochain(SIMPLE_WRITES); setWritePointers(); CHECK_WRITE_SELECTION();
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
    { const char message[]="PASS original C-VID setup: 38 pairs x 4 values, 81 slots, read/mark/write table selections, republish, old-provider negative control, concurrent scratch preservation\r\n";
      WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,sizeof(message)-1,&written,NULL); }
    return 0;
}
