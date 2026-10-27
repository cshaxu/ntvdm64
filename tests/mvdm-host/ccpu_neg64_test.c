/* Test the actual selected object and original declaration; no CPU state stub. */
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <c_neg64.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef char signed_word_is_32[sizeof(IS32)==4 ? 1 : -1];
typedef char unsigned_word_is_32[sizeof(IU32)==4 ? 1 : -1];
typedef char native_pointer_is_32[sizeof(void *)==4 ? 1 : -1];
typedef void (__cdecl *negate_type)(IS32 *, IS32 *);
static negate_type const selected_negate=neg64;
#ifdef CCPU_NEG64_WRONG_ABI
void (__cdecl *ccpu_neg64_incompatible)(IS32, IS32 *)=neg64;
#endif

static uint64_t cases;
static int wrong_oracle;
static int check(uint32_t high,uint32_t low,unsigned order)
{
    struct guarded_pair { uint32_t before;IS32 first;uint32_t between;IS32 second;uint32_t after; } value;
    IS32 *hr=order ? &value.second : &value.first;
    IS32 *lr=order ? &value.first : &value.second;
    uint64_t original=((uint64_t)high<<32)|low,expected=UINT64_C(0)-original;
    uint32_t got_high,got_low;
    value.before=0x12345678u;value.between=0x9abcdef0u;value.after=0x55aa00ffu;
    memcpy(hr,&high,4);memcpy(lr,&low,4);
    selected_negate(hr,lr);
    memcpy(&got_high,hr,4);memcpy(&got_low,lr,4);
    if(value.before!=0x12345678u||value.between!=0x9abcdef0u||value.after!=0x55aa00ffu){
        printf("FAIL NEG64 first-call-guard case=%llu\n",(unsigned long long)cases);return 1;
    }
    if(wrong_oracle)expected^=1u;
    if(got_high!=(uint32_t)(expected>>32)||got_low!=(uint32_t)expected){
        printf("FAIL NEG64 case=%llu order=%u input=%08x:%08x got=%08x:%08x expected=%08x:%08x\n",
            (unsigned long long)cases,order,high,low,got_high,got_low,(uint32_t)(expected>>32),(uint32_t)expected);
        return 1;
    }
    selected_negate(hr,lr);
    memcpy(&got_high,hr,4);memcpy(&got_low,lr,4);
    if(got_high!=high||got_low!=low||value.before!=0x12345678u||value.between!=0x9abcdef0u||value.after!=0x55aa00ffu){
        printf("FAIL NEG64 involution-or-guard case=%llu\n",(unsigned long long)cases);return 1;
    }
    ++cases;return 0;
}

int main(int argc,char **argv)
{
    static const uint32_t edge[]={0,1,2,3,0x7fff,0x8000,0xffff,0x10000,0x7ffffffe,0x7fffffff,0x80000000,0x80000001,0xffffffff};
    uint32_t i,j,state=0x91bc734du;
    unsigned order;
    wrong_oracle=argc==2&&!strcmp(argv[1],"--wrong-oracle");
    if(argc!=1&&!wrong_oracle)return 2;
    for(order=0;order!=2;++order)
        for(i=0;i<ARRAYSIZE(edge);++i)for(j=0;j<ARRAYSIZE(edge);++j)
            if(check(edge[i],edge[j],order))return 1;
    for(i=0;i<65536u;++i){
        uint32_t high=(i<<16)|i;
        if(check(high,0,0)||check(~high,i,1))return 1;
    }
    for(i=0;i<250000u;++i){
        uint32_t high,low;
        state=state*1664525u+1013904223u;high=state;
        state=state*1664525u+1013904223u;low=state;
        if(check(high,low,0)||check(high,low,1))return 1;
    }
    if(cases!=631410u)return 3;
    printf("PASS CCPU NEG64 cases=%llu pointer_orders=2 unsigned_mod64=1 guards=1 involution=1\n",(unsigned long long)cases);
    return 0;
}
