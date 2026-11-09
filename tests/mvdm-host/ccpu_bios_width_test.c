/* Test real CCPU/SAS read bodies; paging and nested instruction execution are stubs. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#define LOCAL static
#define GLOBAL
#define IFN1(t,n) (t n)
#define always_trace1(a,b) ((void)0)
#define sub_note_trace1(a,b,c) ((void)0)
#define SAS_VERBOSE 0
#define PG_U 2
#define BIOS_RDW_OFFSET 1
#define BIOS_RDD_OFFSET 2
#define BIOS_VIRTUALISE_SEGMENT 0xf000
typedef uint8_t IU8; typedef uint16_t IU16; typedef uint32_t IU32;
typedef uint32_t LIN_ADDR; typedef uint32_t PHY_ADDR; typedef uint8_t IUM8;
static int paged=1,translated,vm=1,selectors_set=1;
static IU16 code_sel=0x14,data_sel=0x0c;
static unsigned call_count,last_access;static IU16 last_cs;static IU32 last_offset;
static int c_getPG(void){return paged;} static int getCPL(void){return vm?3:0;}
static int getVM(void){return vm;} static int getPE(void){return 1;}
static int xtrn2phy(LIN_ADDR a,IUM8 access,PHY_ADDR *p){last_access=access;*p=a;return translated;}
static IU16 phy_r16(PHY_ADDR a){(void)a;return 0x5678;}
static IU32 phy_r32(PHY_ADDR a){(void)a;return 0x12345678;}
static IU32 biosDoInst(IU16 cs,IU32 off,IU32 eax,IU16 ds,IU32 address){(void)eax;(void)ds;++call_count;last_cs=cs;last_offset=off;return (address&2)?0x90abcdefu:0x12345678u;}
#include "original_bios_width.h"
int main(void){
 for(vm=0;vm<=1;++vm){call_count=0;translated=0;
  assert(bios_read_word(0x4000)==0x5678);assert(call_count==1&&last_offset==BIOS_RDW_OFFSET);
  assert(last_cs==(vm?BIOS_VIRTUALISE_SEGMENT:code_sel));assert(last_access==(vm?6:0));
  assert(bios_read_double(0x4000)==0x12345678u);assert(last_offset==BIOS_RDD_OFFSET);
  translated=1;call_count=0;assert(bios_read_word(0x4000)==0x5678);assert(bios_read_double(0x4000)==0x12345678u);assert(call_count==0);
 }
 puts("PASS original CCPU BIOS width bodies: virtual-8086/protected fallback and mapped reads");return 0;
}
