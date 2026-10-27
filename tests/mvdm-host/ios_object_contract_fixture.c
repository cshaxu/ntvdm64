/* Actual ios.obj routing under a controlled CPU-virtualisation boundary.
 * No hardware, guest, port instruction or device emulator is substituted. */
#include <stdio.h>
#include <string.h>
#include "insignia.h"
#include "ios.h"
#include "../../src/mvdm/softpc.new/base/ccpu386/in.h"
#include "../../src/mvdm/softpc.new/base/ccpu386/out.h"

#ifndef MVDM_IO_EXPECT_WORD_BYTES
#define MVDM_IO_EXPECT_WORD_BYTES 2
#endif
typedef char io_word_abi[(sizeof(word) == MVDM_IO_EXPECT_WORD_BYTES) ? 1 : -1];
typedef char io_port_abi[(sizeof(io_addr) == 2 && sizeof(IU32) == 4 && sizeof(void *) == 4) ? 1 : -1];
extern void io_define_ind_routine(half_word, void (*)(io_addr, IU32 *), void (*)(io_addr, IU32 *, word));
extern void io_define_outd_routine(half_word, void (*)(io_addr, IU32), void (*)(io_addr, IU32 *, word));
extern void (*Ios_insd_function[])(io_addr, IU32 *, word);
extern void (*Ios_outsd_function[])(io_addr, IU32 *, word);
extern void (**get_inb_ptr(io_addr))(io_addr, half_word *);

static unsigned checks, failures, callbacks, used, virtual_calls;
static struct { io_addr port; IU32 value; unsigned adapter; } trace[64];
static int virtual_mode, virtual_output;
static io_addr virtual_port;
static IU8 virtual_width;
static IU32 virtual_value;
#define A SPARE_ADAPTER1
#define B SPARE_ADAPTER2
#define P ((io_addr)0x240)
#define CHECK(x) do { ++checks; if (!(x)) { ++failures; fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x); } } while (0)

IBOOL c_IOVirtualised(io_addr port, IU32 *value, LIN_ADDR offset, IU8 width)
{
    (void)offset;
    ++virtual_calls;
    virtual_port = port; virtual_width = width;
    if (!virtual_mode) return 0;
    if (virtual_output) virtual_value = *value;
    else *value = 0xa1b2c3d4UL;
    return 1;
}
static void note(io_addr port, IU32 value, unsigned adapter)
{
    ++callbacks;
    if (used < sizeof(trace)/sizeof(trace[0])) {
        trace[used].port=port; trace[used].value=value; trace[used].adapter=adapter;
    }
    ++used;
}
static void read_a(io_addr port, half_word *value) { *value=(half_word)(0x10+(port&15)); note(port,*value,A); }
static void read_b(io_addr port, half_word *value) { *value=(half_word)(0x80+(port&15)); note(port,*value,B); }
static void write_a(io_addr port, half_word value) { note(port,value,A); }
static void write_b(io_addr port, half_word value) { note(port,value,B); }
static void read_new(io_addr port, half_word *value) { *value=0xee; note(port,*value,A); }
static void swap_read(io_addr port, half_word *value) { *value=0x55; note(port,*value,A); io_redefine_inb(A,read_new); }
static void read_word(io_addr port, word *value) { *value=0x7654; note(port,*value,A); }
static void write_word(io_addr port, word value) { note(port,value,A); }
static void read_dword(io_addr port, IU32 *value) { *value=0x12345678UL; note(port,*value,A); }
static void write_dword(io_addr port, IU32 value) { note(port,value,A); }

int main(void)
{
    half_word bv, bytes[5]; word wv, words[4]; IU32 dv, dwords[4];
    unsigned i, before;
    io_init();
    inb(P,&bv); CHECK(bv==0xff);
    inw(P,&wv); CHECK(wv==0xffff);
    ind(P,&dv); CHECK(dv==0xffffffffUL);
    io_define_inb(A,read_a); io_define_inb(B,read_b);
    io_define_outb(A,write_a); io_define_outb(B,write_b);
    for (i=0;i<4;++i) CHECK(io_connect_port((io_addr)(P+i),(half_word)((i&1)?B:A),IO_READ_WRITE));
    used=0; inw(P,&wv); CHECK(wv==0x8110 && used==2);
    CHECK(trace[0].port==P && trace[1].port==P+1 && trace[0].adapter==A && trace[1].adapter==B);
    used=0; ind(P,&dv); CHECK(dv==0x83128110UL && used==4);
    for(i=0;i<4;++i) CHECK(trace[i].port==P+i);
    used=0; outw(P,0x1234); CHECK(used==2 && trace[0].value==0x34 && trace[1].value==0x12);
    used=0; outd(P,0x44332211UL); CHECK(used==4);
    for(i=0;i<4;++i) CHECK(trace[i].port==P+i && trace[i].value==((0x44332211UL>>(i*8))&255));

    /* String operations repeat one port; guard cells check the write extent. */
    memset(bytes,0xa5,sizeof(bytes)); used=0; Ios_insb_function[A](P,bytes+1,3);
    CHECK(used==3 && bytes[0]==0xa5 && bytes[4]==0xa5);
    for(i=0;i<3;++i) CHECK(bytes[i+1]==0x10 && trace[i].port==P);
    before=callbacks; Ios_insb_function[A](P,bytes+1,0); CHECK(callbacks==before);
    memset(words,0xa5,sizeof(words)); used=0; Ios_insw_function[A](P,words+1,2);
    CHECK(used==4 && words[0]==0xa5a5 && words[3]==0xa5a5 && words[1]==0x8110 && words[2]==0x8110);
    memset(dwords,0xa5,sizeof(dwords)); used=0; Ios_insd_function[A](P,dwords+1,2);
    CHECK(used==8 && dwords[0]==0xa5a5a5a5UL && dwords[3]==0xa5a5a5a5UL && dwords[1]==0x83128110UL && dwords[2]==0x83128110UL);
    used=0; Ios_outsb_function[A](P,bytes+1,3); CHECK(used==3);
    for(i=0;i<3;++i) CHECK(trace[i].port==P && trace[i].value==bytes[i+1]);
    used=0; Ios_outsw_function[A](P,words+1,2); CHECK(used==4);
    used=0; Ios_outsd_function[A](P,dwords+1,2); CHECK(used==8);

    io_define_in_routines(A,read_a,read_word,NULL,NULL);
    io_define_out_routines(A,write_a,write_word,NULL,NULL);
    used=0; inw(P,&wv); CHECK(wv==0x7654 && used==1);
    used=0; outw(P,0xabcd); CHECK(used==1 && trace[0].value==0xabcd);
    io_define_ind_routine(A,read_dword,NULL); io_define_outd_routine(A,write_dword,NULL);
    /* Only SFELLOW CCPU uses direct dword slots; the NT profile splits words. */
    used=0; Ios_ind_function[A](P,&dv); CHECK(dv==0x12345678UL && used==1);
    used=0; Ios_outd_function[A](P,0x87654321UL); CHECK(used==1 && trace[0].value==0x87654321UL);
    used=0; IN32(&dv,P); CHECK(dv==0x83ff81ffUL && used==2);
    used=0; OUT32(P,0x87654321UL); CHECK(used==2 && trace[0].value==0x43 && trace[1].value==0x87);
    used=0; ind(P,&dv); CHECK(dv==0x83ff81ffUL && used==2);
    used=0; outd(P,0x87654321UL); CHECK(used==2 && trace[0].value==0x43 && trace[1].value==0x87);
    io_define_inb(A,read_a); io_define_outb(A,write_a);

    /* Separate read/write owners, followed by the NT whole-port disconnect. */
    CHECK(io_connect_port(P,A,IO_READ)); CHECK(io_connect_port(P,B,IO_WRITE));
    used=0; inb(P,&bv); outb(P,0x49); CHECK(used==2 && trace[0].adapter==A && trace[1].adapter==B);
    io_disconnect_port(P,SPARE_ADAPTER3);
    inb(P,&bv); CHECK(bv==0x10);
    io_disconnect_port(P,A); before=callbacks; inb(P,&bv); outb(P,0x51);
    CHECK(bv==0xff && callbacks==before);

    CHECK(io_connect_port(P,A,IO_READ_WRITE));
    CHECK(get_inb_ptr(P)==&Ios_inb_function[A]);
    io_redefine_inb(A,swap_read); used=0; Ios_insb_function[A](P,bytes+1,3);
    CHECK(used==3 && bytes[1]==0x55 && bytes[2]==0x55 && bytes[3]==0x55);
    inb(P,&bv); CHECK(bv==0xee); /* Original string loop snapshots its callback. */
    io_define_inb(A,read_a);

    CHECK(io_connect_port(0xffff,A,IO_READ)); CHECK(io_connect_port(0,B,IO_READ));
    used=0; inw(0xffff,&wv); CHECK(wv==0x801f && used==2 && trace[0].port==0xffff && trace[1].port==0);

    virtual_mode=1; virtual_output=0; before=callbacks;
    inb(P,&bv); CHECK(bv==0xd4 && virtual_width==1 && virtual_port==P);
    inw(P,&wv); CHECK(wv==0xc3d4 && virtual_width==2);
    ind(P,&dv); CHECK(dv==0xa1b2c3d4UL && virtual_width==4);
    virtual_output=1;
    outb(P,0x12); CHECK(virtual_value==0x12 && virtual_width==1);
    outw(P,0x3456); CHECK(virtual_value==0x3456 && virtual_width==2);
    outd(P,0x789abcdeUL); CHECK(virtual_value==0x789abcdeUL && virtual_width==4);
    CHECK(callbacks==before);
    /* The CCPU leaves avoid recursively re-entering the virtualisation shim. */
    before=virtual_calls;
    IN8(&dv,P); CHECK(dv==0x10);
    IN16(&dv,P); CHECK(dv==0x8110);
    IN32(&dv,P); CHECK(dv==0x83128110UL);
    used=0; OUT8(P,0x21); CHECK(used==1 && trace[0].value==0x21);
    used=0; OUT16(P,0x4321); CHECK(used==2);
    used=0; OUT32(P,0x87654321UL); CHECK(used==4);
    CHECK(virtual_calls==before);
    virtual_mode=0; io_init(); inb(P,&bv); CHECK(bv==0xff);
    CHECK(io_connect_port(P,A,IO_READ)); inb(P,&bv); CHECK(bv==0x10);
    printf("IOS_OBJECT_CONTRACT checks=%u failures=%u callbacks=%u\n",checks,failures,callbacks);
    return failures?1:0;
}
