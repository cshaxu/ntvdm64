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
extern ULONG sr_lookup[16];
extern IHP setup_global_data_ptr(void);
extern void setup_vga_globals(void);
extern void setReadPointers(IUH);
extern void setWritePointers(void);
extern void setMarkPointers(IUH);
extern void c_sas_overwrite_memory(PHY_ADDR, PHY_ADDR);
extern EVID_READ_POINTERS c_ev_read_ptr;
extern EVID_WRT_POINTERS c_ev_write_ptr;
extern IU32 gvi_pc_low_regen;
extern void write_byte_ev_glue(IU32, IU8);
extern IU32 read_byte_ev_glue(IU32);
extern void write_word_ev_glue(IU32, IU16);
extern void write_dword_ev_glue(IU32, IU32);
extern IU32 read_word_ev_glue(IU32);
extern IU32 read_dword_ev_glue(IU32);
static unsigned char expected[65536];
static IUH *scratch[] = { &r0,&r1,&r2,&r3,&r4,&r5,&r6,&r7,
    &r8,&r9,&r10,&r11,&r12,&r13,&r14,&r15,&r16,&r17,&r18,&r19,
    &r20,&r21,&r22,&r23,&r24,&r25,&r26,&r27,&r28,&r29,&r30,&r31 };
#define CHECK(x) do { if (!(x)) { char message[1024]; DWORD count; int length=wsprintfA(message,"FAIL line %d: %s\r\n",__LINE__,#x); WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,(DWORD)length,&count,NULL); return 1; } } while (0)
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

/* Adapted from SoftPC 1ec0d551 chain2_rule_smoke.c: same rule matrix,
 * local C-VID setup and Win32-visible failure reporting. */
static unsigned rule_mode, rule_parity, rule_slot, rule_flag;
static LONG report_rule_exception(EXCEPTION_POINTERS *exception)
{
    char message[256]; DWORD count;
    int length=wsprintfA(message,"CHAIN2 exception mode=%u parity=%u slot=%u flag=%u address=%p code=%08lx\r\n",
        rule_mode,rule_parity,rule_slot,rule_flag,
        exception->ExceptionRecord->ExceptionAddress,exception->ExceptionRecord->ExceptionCode);
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,(DWORD)length,&count,NULL);
    return EXCEPTION_EXECUTE_HANDLER;
}
/* SoftPC 8c27a3b6 independent plane oracle, bound to local real providers. */
static int check_chain2_dword_reads(IU8 *planes)
{
    /* Six seeded bytes and their paired planes must stay inside 256 KiB. */
    const IU32 offsets[] = { 0x100, 0x101, 0x103, 0x107, 0x3fff, 0xfff9 };
    const IU8 bytes[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    IU32 compare = getVideocolour_comp(), care = getVideodont_care();
    IU32 mode, entry, i;
    for (mode = 0; mode < 2; ++mode)
    for (entry = 0; entry < sizeof(offsets) / sizeof(offsets[0]); ++entry) {
        IU32 offset = offsets[entry];
        IU32 value = mode ? 0x40302010u : 0x44332211u;
        EVID_READ_POINTERS reader = mode ? read_mode1_evid[1] : read_mode0_evid[1];
        setVideocolour_comp(0);
        setVideodont_care(0);
        for (i = 0; i < sizeof(bytes); ++i) {
            IU32 address = offset + i;
            IU32 physical = 4u * (address & ~1u) + (address & 1u);
            planes[physical] = bytes[i];
            planes[physical + 2u] = 0xf0;
        }
        r1 = (IUH)Gdp; r2 = 0xa5a5a5a5u; r3 = offset;
        reader.d_read(offset);
        if ((IU32)r2 != value)
            fprintf(stderr, "CHAIN2 dword mode=%u offset=%x expected=%08x actual=%08x\n",
                mode, offset, value, (IU32)r2);
        CHECK((IU32)r2 == value);
    }
    setVideocolour_comp(compare);
    setVideodont_care(care);
    return 0;
}
static int check_chain2_rules(IU8 *planes)
{
    static IU8 expected[4u * 65536u];
    IU8 input[128], output[64], first[64];
    EVID_WRT_POINTERS saved = c_ev_write_ptr;
    IU32 mode, parity, slot, pass, flag, i, offset;
    /* Unchained DWORD must read the same four consecutive guest bytes as
       four independent byte reads, including the second word's offset. */
    for (i = 0; i < 256; ++i) planes[4u * i] = (IU8)(i * 17u + 3u);
    for (offset = 0x20; offset < 0x28; ++offset) {
        IU32 value = 0;
        for (i = 0; i < 4; ++i) {
            r1 = (IUH)Gdp; r3 = offset + i;
            read_mode0_evid[0].b_read(offset + i);
            value |= ((IU32)r2 & 0xffu) << (i * 8u);
        }
        r1 = (IUH)Gdp; r3 = offset;
        read_mode0_evid[0].d_read(offset);
        CHECK((IU32)r2 == value);
    }
    CHECK(check_chain2_dword_reads(planes) == 0);
    /* Odd/even selects a plane without compacting its address. Independent
       physical offsets distinguish adjacent cells and preserve font planes. */
    memset(planes, 0x5a, sizeof(expected));
    c_ev_write_ptr = chain2_evid[4];
    for (i = 0; i < 8; ++i) {
        r1 = (IUH)Gdp; r2 = 0x100u + i; r3 = 0x30u + i;
        c_ev_write_ptr.b_write(0x100u + i, 0x30u + i);
    }
    for (i = 0; i < 4; ++i) {
        CHECK(planes[0x400u + i * 8u] == 0x30u + i * 2u);
        CHECK(planes[0x401u + i * 8u] == 0x31u + i * 2u);
        CHECK(planes[0x402u + i * 8u] == 0x5a);
        CHECK(planes[0x404u + i * 8u] == 0x5a);
        r1 = (IUH)Gdp; r3 = 0x100u + i * 2u;
        read_mode0_evid[1].w_read(0x100u + i * 2u);
        CHECK((IU16)r2 == (IU16)((0x31u + i * 2u) * 256u + 0x30u + i * 2u));
    }
    for (i = 0; i < 7; ++i) {
        r1 = (IUH)Gdp; r3 = 0x100u + i;
        read_mode0_evid[1].w_read(0x100u + i);
        CHECK((IU16)r2 == (IU16)((0x31u + i) * 256u + 0x30u + i));
    }
    for (i = 0; i < 5; ++i) {
        r1 = (IUH)Gdp; r3 = 0x100u + i;
        read_mode0_evid[1].d_read(0x100u + i);
        CHECK((IU32)r2 == ((0x33u+i)<<24 | (0x32u+i)<<16 | (0x31u+i)<<8 | (0x30u+i)));
    }
    /* String readers must traverse the same cells as scalar reads. */
    for (mode = 0; mode < 2; ++mode)
    for (parity = 0; parity < 2; ++parity)
    for (slot = 0; slot < 2; ++slot) {
        EVID_READ_POINTERS reader = mode ? read_mode1_evid[1] : read_mode0_evid[1];
        rule_mode=mode; rule_parity=parity; rule_slot=slot; rule_flag=flag;
        offset = slot ? 0x107u - parity : 0x100u + parity;
        for (i = 0; i < 4; ++i) {
            IU32 address = slot ? offset - i : offset + i;
            r1 = (IUH)Gdp; r3 = address;
            reader.b_read(address);
            first[i] = (IU8)r2;
        }
        memset(output, 0xcc, sizeof(output));
        r1 = (IUH)Gdp; r2 = (IUH)(output + 24); r3 = offset; r4 = 4; r5 = slot;
        if (slot) reader.str_bwd_read(output + 24, offset, 4);
        else reader.str_fwd_read(output + 24, offset, 4);
        if (memcmp(first, output + 24, 4) != 0)
            fprintf(stderr, "string read mode=%u parity=%u backward=%u expected=%02x actual=%02x\n", mode, parity, slot, first[0], output[24]);
        CHECK(memcmp(first, output + 24, 4) == 0);
    }
    /* A RAM-to-video word move consumes the generated-rule descending RAM
       source convention; verify each destination CPU byte independently. */
    for (parity = 0; parity < 2; ++parity) {
        for (i = 0; i < sizeof(input); ++i) input[i] = (IU8)(0x31u + i);
        memset(planes, 0x5a, sizeof(expected));
        c_ev_write_ptr = chain2_evid[4];
        offset = 0x100u + parity;
        r1 = (IUH)Gdp; r2 = offset; r3 = (IUH)(input + 32); r4 = 4; r5 = 1;
        c_ev_write_ptr.w_fwd_move(offset, (IHPE)(input + 32), 4, 1);
        for (i = 0; i < 8; ++i) {
            IU32 address = offset + i;
            IU8 actual = planes[4u * (address & ~1u) + (address & 1u)];
            if (actual != input[32-i])
                fprintf(stderr, "copy RAM word move parity=%u byte=%u expected=%02x actual=%02x\n", parity, i, input[32-i], actual);
            CHECK(actual == input[32-i]);
        }
    }
    /* Independent physical-plane oracle for the registered byte/word Copy fills. */
    for (parity = 0; parity < 2; ++parity)
    for (slot = 0; slot < 2; ++slot) {
        IU32 width = 1u << slot;
        memset(planes, 0x5a, sizeof(expected));
        r1 = (IUH)Gdp; r2 = 0x100u + parity; r3 = 0x11111111u; r4 = 8u / width;
        if (slot == 0) c_ev_write_ptr.b_fill(0x100u + parity, 0x11, 8);
        else c_ev_write_ptr.w_fill(0x100u + parity, 0x1111, 4);
        for (i = 0; i < 8; ++i) {
            IU32 address = 0x100u + parity + i;
            if (planes[4u * (address & ~1u) + (address & 1u)] != (IU8)(0x11111111u >> ((i % width) * 8u)))
                fprintf(stderr, "copy fill parity=%u width=%u byte=%u actual=%02x\n", parity, width, i, planes[4u * (address & ~1u) + (address & 1u)]);
            CHECK(planes[4u * (address & ~1u) + (address & 1u)] ==
                (IU8)(0x11111111u >> ((i % width) * 8u)));
        }
    }
    /* SoftPC 19a68782: verify the complete plane, including untouched bytes,
       so odd heads and short tails cannot silently corrupt adjacent cells. */
    {
        const IU32 starts[] = {0x101, 0x100, 0x103, 0x107, 0x3fff, 0xffe1};
        const IU32 values[] = {0x44332211u, 0x89abcdefu, 0, 0xffffffffu};
        const IU32 counts[] = {1, 2, 7, 0};
        IU32 start, value, count;
        memset(planes, 0x5a, sizeof(expected));
        memset(expected, 0x5a, sizeof(expected));
        r1=(IUH)Gdp; r2=0x101; r3=0x1234; r4=0;
        c_ev_write_ptr.b_fill(0x101, 0x34, 0);
        r1=(IUH)Gdp; r2=0x101; r3=0x1234; r4=0;
        c_ev_write_ptr.w_fill(0x101, 0x1234, 0);
        CHECK(memcmp(planes, expected, sizeof(expected)) == 0);
        for (start = 0; start < sizeof(starts)/sizeof(starts[0]); ++start)
        for (value = 0; value < sizeof(values)/sizeof(values[0]); ++value)
        for (count = 0; count < sizeof(counts)/sizeof(counts[0]); ++count) {
            memset(planes, 0x5a, sizeof(expected));
            memset(expected, 0x5a, sizeof(expected));
            for (i = 0; i < counts[count] * 4u; ++i) {
                IU32 address = starts[start] + i;
                expected[4u * (address & ~1u) + (address & 1u)] =
                    (IU8)(values[value] >> ((i % 4u) * 8u));
            }
            r1=(IUH)Gdp; r2=starts[start]; r3=values[value]; r4=counts[count];
            c_ev_write_ptr.d_fill(starts[start], values[value], counts[count]);
            CHECK(memcmp(planes, expected, sizeof(expected)) == 0);
        }
    }
    for (i = 0; i < sizeof(input); ++i) input[i] = (IU8)(i * 17u + 0x35u);
    for (mode = 0; mode < 4; ++mode)
    for (parity = 0; parity < 4; ++parity) {
        rule_mode=mode; rule_parity=parity; rule_slot=99;
        offset = 0x100u + parity;
        c_ev_write_ptr = chain2_evid[mode];
        /* The former crash: an odd word continuation returns into dword.
           Compare with ModeX's actual two-word decomposition, stride 4.
           Copy has an independent inline four-byte algorithm; exercise its
           continuation below without assuming this ModeX decomposition.
           Upper bytes are zero: no change to legacy value-width semantics. */
        memset(planes, 0x5a, sizeof(expected));
        r1 = (IUH)Gdp; r2 = offset; r3 = 0xa5;
        c_ev_write_ptr.d_write(offset, 0xa5);
        memcpy(expected, planes, sizeof(expected));
        memset(planes, 0x5a, sizeof(expected));
        r1 = (IUH)Gdp; r2 = offset; r3 = 0xa5;
        c_ev_write_ptr.w_write(offset, 0xa5);
        r1 = (IUH)Gdp; r2 = offset + 4u; r3 = 0;
        c_ev_write_ptr.w_write(offset + 4u, 0);
        CHECK(memcmp(expected, planes, sizeof(expected)) == 0);
    }
    for (mode = 0; mode < 5; ++mode)
    for (parity = 0; parity < 4; ++parity) {
        offset = 0x100u + parity;
        c_ev_write_ptr = chain2_evid[mode];
        /* Exercise all compound entries with valid native RAM pointers.
           Replay checks stable output and a scalar sentinel checks that nested
           returns leave the next ordinary call usable. No missing video-source
           bulk callback is manufactured by this fixture. */
        for (slot = 0; slot < 12; ++slot)
        for (flag = 1; flag <= (mode == 4 && slot >= 6 ? 2u : 1u); ++flag)
        for (pass = 0; pass < 2; ++pass) {
            IBOOL in_ram = flag == 1;
            rule_mode=mode; rule_parity=parity; rule_slot=slot; rule_flag=flag;
            IHPE source = in_ram ? (IHPE)(input + ((slot & 1u) ? 64u : 32u) + (parity & 1u)) : (IHPE)0x700u;
            for (i = 0; i < sizeof(expected); ++i)
                planes[i] = (IU8)(i * 37u + (i >> 3));
            r1 = (IUH)Gdp; r2 = offset; r3 = 0xa5; r4 = 4; r5 = in_ram;
            if (slot >= 6) r3 = (IUH)source;
            switch (slot) {
            case 0: c_ev_write_ptr.b_write(offset, 0xa5); break;
            case 1: c_ev_write_ptr.w_write(offset, 0xa5); break;
            case 2: c_ev_write_ptr.d_write(offset, 0xa5); break;
            case 3: c_ev_write_ptr.b_fill(offset, 0xa5, 4); break;
            case 4: c_ev_write_ptr.w_fill(offset, 0xa5, 4); break;
            case 5: c_ev_write_ptr.d_fill(offset, 0xa5, 4); break;
            case 6: c_ev_write_ptr.b_fwd_move(offset, source, 4, in_ram); break;
            case 7: c_ev_write_ptr.b_bwd_move(offset, source, 4, in_ram); break;
            case 8: c_ev_write_ptr.w_fwd_move(offset, source, 4, in_ram); break;
            case 9: c_ev_write_ptr.w_bwd_move(offset, source, 4, in_ram); break;
            case 10: c_ev_write_ptr.d_fwd_move(offset, source, 4, in_ram); break;
            case 11: c_ev_write_ptr.d_bwd_move(offset, source, 4, in_ram); break;
            }
            r1 = (IUH)Gdp; r2 = 0x500; r3 = 0x69;
            c_ev_write_ptr.b_write(0x500, 0x69);
            if (!pass) memcpy(expected, planes, sizeof(expected));
            else CHECK(memcmp(expected, planes, sizeof(expected)) == 0);
        }
    }
    for (i = 0; i < sizeof(expected); ++i)
        planes[i] = (IU8)(i * 37u + (i >> 3));
    for (mode = 0; mode < 2; ++mode)
    for (parity = 0; parity < 4; ++parity)
    for (slot = 0; slot < 5; ++slot)
    for (flag = 0; flag < (slot >= 3 ? 2u : 1u); ++flag) {
        EVID_READ_POINTERS reader = mode ? read_mode1_evid[1] : read_mode0_evid[1];
        rule_mode=mode; rule_parity=parity; rule_slot=slot; rule_flag=flag;
        IU32 first_value = 0;
        offset = 0x100u + parity;
        for (pass = 0; pass < 2; ++pass) {
            memset(output, 0xcc, sizeof(output));
            r1 = (IUH)Gdp; r2 = (IUH)(output + 24u);
            r3 = offset; r4 = 4; r5 = flag;
            switch (slot) {
            case 0: reader.b_read(offset); break;
            case 1: reader.w_read(offset); break;
            case 2: reader.d_read(offset); break;
            case 3: reader.str_fwd_read(output + 24u, offset, 4); break;
            case 4: reader.str_bwd_read(output + 24u, offset, 4); break;
            }
            if (!pass) {
                first_value = (IU32)r2;
                memcpy(first, output, sizeof(first));
            } else {
                if (slot < 3 && first_value != (IU32)r2) {
                    char message[256]; DWORD count;
                    int length=wsprintfA(message,"read replay mode=%u parity=%u slot=%u flag=%u first=%08x second=%08x\r\n",mode,parity,slot,flag,first_value,(IU32)r2);
                    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,(DWORD)length,&count,NULL);
                }
                if (slot < 3) CHECK(first_value == (IU32)r2);
                CHECK(memcmp(first, output, sizeof(first)) == 0);
            }
            for (i = 0; i < 8; ++i) {
                CHECK(output[i] == 0xcc);
                CHECK(output[sizeof(output) - 1u - i] == 0xcc);
            }
        }
    }
    c_ev_write_ptr = saved;
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
        setMarkPointers(5);
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
    /* Execute original generated simple byte routines through the real glue.
     * The guarded local buffer is the fixture's screen memory; the trace ring
     * mirrors the generated runtime precondition without touching guest media. */
    {
        IU8 memory[258]; IUH trace[8]; IU32 saved_base=gvi_pc_low_regen;
        memset(memory,0xa5,sizeof(memory)); memset(trace,0,sizeof(trace));
        ((IUH *)Gdp)[2]=(IUH)&trace[0];
        ((IUH *)Gdp)[3]=(IUH)&trace[0];
        ((IUH *)Gdp)[4]=(IUH)&trace[7];
        setVideoscreen_ptr(memory+1); setVideodirty_total(0);
        gvi_pc_low_regen=0x1000;
        setVideodither(0); setVideochain(SIMPLE_WRITES); setWritePointers();
        setReadPointers(SIMPLE_READ);
        for(i=0;i<256;i++) {
            write_byte_ev_glue(0x1000+i,(IU8)i);
            CHECK(memory[i+1]==(IU8)i);
            CHECK((read_byte_ev_glue(0x1000+i)&0xff)==i);
            CHECK(memory[0]==0xa5 && memory[257]==0xa5);
        }
        CHECK(getVideodirty_total()==256);
        CHECK(trace[0]==2139 && trace[1]==2115);
        /* Both aligned and unaligned offsets exercise the generated byte
         * decomposition and little-endian composition. */
        for(i=0;i<253;i++) {
            IU32 value=0x80f17b00u+i;
            write_word_ev_glue(0x1000+i,(IU16)value);
            CHECK(memory[i+1]==(IU8)value && memory[i+2]==(IU8)(value>>8));
            CHECK((read_word_ev_glue(0x1000+i)&0xffff)==(value&0xffff));
            write_dword_ev_glue(0x1000+i,value);
            CHECK(memory[i+1]==(IU8)value && memory[i+2]==(IU8)(value>>8) &&
                  memory[i+3]==(IU8)(value>>16) && memory[i+4]==(IU8)(value>>24));
            CHECK(read_dword_ev_glue(0x1000+i)==value);
            CHECK(memory[0]==0xa5 && memory[257]==0xa5);
        }
        gvi_pc_low_regen=saved_base;
    }
    {
        static IU8 planes[4 * 65536];
        IUH trace[64];
        memset(trace,0,sizeof(trace));
        ((IUH *)Gdp)[2]=(IUH)trace;
        ((IUH *)Gdp)[3]=(IUH)trace;
        ((IUH *)Gdp)[4]=(IUH)&trace[63];
        setVideorplane(planes); setVideowplane(planes);
        setVideoread_shift_count(0); setMarkPointers(0);
        setVideosr_lookup(sr_lookup);
        setVideodata_and_mask(0xffffffffu);
        setVideoplane_enable(15); setVideoplane_enable_mask(0xffffffffu);
        setVideobit_prot_mask(0xffffffffu);
        __try { CHECK(check_chain2_rules(planes)==0); }
        __except(report_rule_exception(GetExceptionInformation())) { return 1; }
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
    { const char message[]="PASS original C-VID setup: 38 pairs x 4 values, 81 slots, exact read/write/mark selection, 256 write-state cases, 256 simple byte and 253 unaligned word/dword read/write cases with guards, CHAIN2 physical addresses/forward/backward strings/fills/subsequent activation, unknown read/mark selectors, negative control, concurrent scratch preservation\r\n";
      WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),message,sizeof(message)-1,&written,NULL); }
    return 0;
}
