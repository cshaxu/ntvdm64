/* Independent hardware observation; both candidate word locations are owned. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint16_t storage[5000];

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
    return base[-1]==0&&base[4095]==0&&carry==1 ? 0 : 1;
}
