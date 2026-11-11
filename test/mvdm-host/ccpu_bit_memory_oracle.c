/* Independent hardware observation; both candidate word locations are owned. */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint16_t storage[8192];

int main(void)
{
    uint16_t *base=storage+16;
    unsigned char carry;
    memset(storage,0,sizeof(storage));
    *base=0x8000;
    __asm {
        mov edi,base
        mov bx,15
        btc word ptr [edi],bx
        setc carry
    }
    if(carry!=1||*base!=0)return 2;
    puts("BITMEM POSITIVE PASS");
    base[-1]=0x8000;
    base[4095]=0;
    __asm {
        mov edi,base
        mov bx,-1
        btc word ptr [edi],bx
        setc carry
    }
    printf("BITMEM NEGATIVE near=%04x far=%04x carry=%u\n",base[-1],base[4095],carry);
    if(base[-1]!=0||base[4095]!=0||carry!=1)return 1;
    base[-1]=0x8000;
    __asm {
        mov edi,base
        mov bx,-1
        btr word ptr [edi],bx
        setc carry
    }
    if(base[-1]!=0||base[4095]!=0||carry!=1)return 1;
    __asm {
        mov edi,base
        mov bx,-1
        bts word ptr [edi],bx
        setc carry
    }
    if(base[-1]!=0x8000||base[4095]!=0||carry!=0)return 1;
    puts("BITMEM NEGATIVE BTS/BTR PASS");
    {
        static const int16_t indices[] = {-32768, -17, -16, -15, -1, 0, 15, 16, 32767};
        unsigned i, op;
        for (i = 0; i < sizeof(indices) / sizeof(indices[0]); ++i) {
            int n = indices[i];
            int offset = n >= 0 ? n / 16 : -((-n + 15) / 16);
            int16_t index = indices[i];
            uint16_t *center = storage + 4096;
            uint16_t *target = center + offset;
            uint16_t mask = (uint16_t)(1u << ((unsigned)n & 15));
            for (op = 0; op < 4; ++op) {
                memset(storage, 0, sizeof(storage));
                *target = op == 1 ? 0 : mask;
                target[-1] = 0x55aa; target[1] = 0xa55a;
                __asm { mov edi,center }
                __asm { mov bx,index }
                if (op == 0) { __asm { bt word ptr [edi],bx } }
                else if (op == 1) { __asm { bts word ptr [edi],bx } }
                else if (op == 2) { __asm { btr word ptr [edi],bx } }
                else { __asm { btc word ptr [edi],bx } }
                __asm { setc carry }
                if (carry != (op == 1 ? 0 : 1) || *target != (op >= 2 ? 0 : mask) ||
                        target[-1] != 0x55aa || target[1] != 0xa55a) return 1;
            }
        }
    }
    puts("BITMEM WORD BOUNDARIES PASS count=9");
    puts("BITMEM WORD FAMILY PASS cases=36");
    {
        static const int32_t indices[] = {INT32_MIN, -33, -32, -31, -1, 0, 31, 32, INT32_MAX};
        unsigned char *area = VirtualAlloc(NULL, 0x20010000u, MEM_RESERVE, PAGE_NOACCESS);
        unsigned char *center;
        unsigned i, op;
        if (!area) return 2;
        center = area + 0x10001000u;
        for (i = 0; i < sizeof(indices) / sizeof(indices[0]); ++i) {
            int64_t n = indices[i];
            int64_t offset = n >= 0 ? n / 32 : -((-n + 31) / 32);
            int32_t index = indices[i];
            uint32_t *target = (uint32_t *)(center + offset * 4);
            unsigned char *pages = (unsigned char *)((uintptr_t)target & ~(uintptr_t)4095) - 4096;
            uint32_t mask = 1u << ((unsigned)n & 31u);
            if (!VirtualAlloc(pages, 12288, MEM_COMMIT, PAGE_READWRITE)) return 2;
            for (op = 0; op < 4; ++op) {
                *target = op == 1 ? 0 : mask;
                target[-1] = 0x55aa55aa; target[1] = 0xa55aa55a;
                __asm { mov edi,center }
                __asm { mov eax,index }
                if (op == 0) { __asm { bt dword ptr [edi],eax } }
                else if (op == 1) { __asm { bts dword ptr [edi],eax } }
                else if (op == 2) { __asm { btr dword ptr [edi],eax } }
                else { __asm { btc dword ptr [edi],eax } }
                __asm { setc carry }
                if (carry != (op == 1 ? 0 : 1) || *target != (op >= 2 ? 0 : mask) ||
                        target[-1] != 0x55aa55aa || target[1] != 0xa55aa55a) return 1;
            }
            if (!VirtualFree(pages, 12288, MEM_DECOMMIT)) return 2;
        }
        if (!VirtualFree(area, 0, MEM_RELEASE)) return 2;
    }
    puts("BITMEM DWORD BOUNDARIES PASS count=9");
    puts("BITMEM DWORD FAMILY PASS cases=36");
    return 0;
}
