/* Independent native x86 reference data, never a guest/runtime provider. */
#include <stdint.h>
#include <stdio.h>

#if !defined(_MSC_VER) || !defined(_M_IX86)
#error This test producer requires MSVC targeting native 32-bit x86.
#endif

enum operation { ADD, ADC, AND, CMP, DEC, INC, NEG, OR, SBB, SUB, TEST, XOR };
struct variant { enum operation op; const char *stem; unsigned carry; };
static const struct variant variants[] = {
    {ADD,"ADD",0}, {ADC,"ADC0_",0}, {ADC,"ADC1_",1}, {AND,"AND",0},
    {CMP,"CMP",0}, {DEC,"DEC0_",0}, {DEC,"DEC1_",1},
    {INC,"INC0_",0}, {INC,"INC1_",1}, {NEG,"NEG",0}, {OR,"OR",0},
    {SBB,"SBB0_",0}, {SBB,"SBB1_",1}, {SUB,"SUB",0},
    {TEST,"TEST",0}, {XOR,"XOR",0}
};
static const uint32_t boundaries[] = {
    0,1,2,7,0x7f,0x80,0xff,0x100,0x7fff,0x8000,0xffff,
    0x7fffffff,0x80000000,0xffffffff
};

/* BT sets the requested incoming CF immediately before the tested opcode.
 * The following instruction defines every compared arithmetic flag, except
 * INC/DEC preserve that CF. Undefined logical AF is excluded below. */
#define BINARY(op, dst, src) \
    __asm { mov eax, lhs } \
    __asm { mov edx, rhs } \
    __asm { bt carry, 0 } \
    __asm { op dst, src } \
    __asm { pushfd } \
    __asm { pop ecx } \
    __asm { mov oracle_result, eax } \
    __asm { mov status, ecx }
#define UNARY(op, dst, ignored) \
    __asm { mov eax, lhs } \
    __asm { bt carry, 0 } \
    __asm { op dst } \
    __asm { pushfd } \
    __asm { pop ecx } \
    __asm { mov oracle_result, eax } \
    __asm { mov status, ecx }
#define WIDTHS(macro, op) \
    if (width == 8) { macro(op, al, dl); } \
    else if (width == 16) { macro(op, ax, dx); } \
    else { macro(op, eax, edx); }

static void native(enum operation op, unsigned width, unsigned carry,
                   uint32_t lhs, uint32_t rhs, uint32_t *result,
                   uint16_t *flags)
{
    uint32_t oracle_result = 0, status = 0;
    uint32_t mask = width == 32 ? UINT32_MAX : (UINT32_C(1) << width) - 1;
    switch (op) {
    case ADD: WIDTHS(BINARY, add); break;
    case ADC: WIDTHS(BINARY, adc); break;
    case AND: WIDTHS(BINARY, and); break;
    case CMP: WIDTHS(BINARY, cmp); break;
    case DEC: WIDTHS(UNARY, dec); break;
    case INC: WIDTHS(UNARY, inc); break;
    case NEG: WIDTHS(UNARY, neg); break;
    case OR: WIDTHS(BINARY, or); break;
    case SBB: WIDTHS(BINARY, sbb); break;
    case SUB: WIDTHS(BINARY, sub); break;
    case TEST: WIDTHS(BINARY, test); break;
    case XOR: WIDTHS(BINARY, xor); break;
    }
    *result = oracle_result & mask;
    /* Original logical owners explicitly clear undefined architectural AF. */
    *flags = (uint16_t)(status &
        ((op == AND || op == OR || op == TEST || op == XOR) ? 0x08c5u : 0x08d5u));
}

static void put32(unsigned char *p, uint32_t n)
{
    p[0]=(unsigned char)n; p[1]=(unsigned char)(n>>8);
    p[2]=(unsigned char)(n>>16); p[3]=(unsigned char)(n>>24);
}

static int record(FILE *file, const struct variant *v, unsigned width,
                  uint32_t lhs, uint32_t rhs)
{
    unsigned char bytes[14];
    uint32_t result;
    uint16_t flags;
    native(v->op,width,v->carry,lhs,rhs,&result,&flags);
    put32(bytes,lhs); put32(bytes+4,rhs); put32(bytes+8,result);
    bytes[12]=(unsigned char)flags; bytes[13]=(unsigned char)(flags>>8);
    return fwrite(bytes,1,sizeof bytes,file)==sizeof bytes;
}

static uint32_t next(uint32_t *state)
{
    *state = *state * UINT32_C(1664525) + UINT32_C(1013904223);
    return *state;
}

static int generate(const char *directory, const struct variant *v, unsigned width)
{
    char path[1024];
    uint32_t a,b,state=UINT32_C(0x12345678);
    uint32_t mask=width==32 ? UINT32_MAX : (UINT32_C(1)<<width)-1;
    unsigned i,j;
    int n,ok=1;
    FILE *file;
    n=snprintf(path,sizeof path,"%s/%s%u.BIN",directory,v->stem,width);
    if(n<0 || (size_t)n>=sizeof path) return 0;
    file=fopen(path,"wb");
    if(!file) return 0;
    if(width==8) {
        for(a=0;a<256 && ok;++a)
            for(b=0;b<256 && ok;++b) ok=record(file,v,width,a,b);
    } else {
        for(i=0;i<sizeof boundaries/sizeof boundaries[0] && ok;++i)
            for(j=0;j<sizeof boundaries/sizeof boundaries[0] && ok;++j)
                ok=record(file,v,width,boundaries[i]&mask,boundaries[j]&mask);
        for(i=0;i<1000 && ok;++i) {
            a=next(&state)&mask; b=next(&state)&mask;
            ok=record(file,v,width,a,b);
        }
    }
    if(fclose(file)!=0) ok=0;
    if(ok) printf("%s%u.BIN records=%u\n",v->stem,width,width==8?65536:1196);
    return ok;
}

int main(int argc,char **argv)
{
    unsigned i,width;
    if(argc!=2 || !argv[1][0]) return 2;
    for(i=0;i<sizeof variants/sizeof variants[0];++i)
        for(width=8;width<=32;width*=2)
            if(!generate(argv[1],&variants[i],width)) return 1;
    return 0;
}
