/* Real CCPU FLD/FISTP: four rounding modes, signed fractions and integers. */
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include "ntvdm-exe/session/session.h"

extern void sas_init(PHY_ADDR), c_cpu_init(void), phy_w32(IU32, IU32);
extern IU32 phy_r32(PHY_ADDR);
extern void host_simulate(void), c_setIP(IU16);
extern ISM32 c_setCS(IU16), c_setDS(IU16);
static jmp_buf boundary;
static int vector;
BOOL host_exint_hook(IS32 number, IS32 error)
{
    (void)error;
    vector = number;
    longjmp(boundary, 1);
}

int main(void)
{
    session owner;
    static const unsigned char code[] = {
        0xdb,0xe3,0xd9,0x2e,0x00,0x11,0xdd,0x06,0x00,0x12,
        0xdf,0x3e,0x00,0x13,0xd6,0xfe
    };
    static const double values[] = {2.75,-2.75,2.0,-2.0};
    static const long expected[4][4]={{3,-3,2,-2},{2,-3,2,-2},{3,-2,2,-2},{2,-2,2,-2}};
    unsigned mode,v,i,failures=0;
    session_initialize(&owner,45);
    if (!session_activate(&owner)||!session_thread_bind(&owner)) return 2;
    sas_init(0x200000);c_cpu_init();
    for(mode=0;mode<4;++mode) for(v=0;v<4;++v){
        IU32 words[2],lo,hi;
        memcpy(words,&values[v],8);
        phy_w32(0x1100,0x037f|(mode<<10));
        phy_w32(0x1200,words[0]);phy_w32(0x1204,words[1]);
        phy_w32(0x1300,0xcccccccc);phy_w32(0x1304,0xcccccccc);
        for(i=0;i<sizeof(code);++i)c_sas_store(0x8000+i,code[i]);
        (void)c_setCS(0);(void)c_setDS(0);c_setIP(0x8000);vector=-1;
        if(!setjmp(boundary))host_simulate();
        lo=phy_r32(0x1300);hi=phy_r32(0x1304);
        if(vector!=-1||lo!=(IU32)expected[mode][v]||hi!=(expected[mode][v]<0?0xffffffffu:0)){
            ++failures;fprintf(stderr,"FAIL mode=%u value=%g vector=%d result=%08lx:%08lx expected=%ld\n",mode,values[v],vector,hi,lo,expected[mode][v]);
        }
    }
    fprintf(stderr,"FIST64 cases=16 failures=%u\n",failures);
    return failures?1:0;
}
