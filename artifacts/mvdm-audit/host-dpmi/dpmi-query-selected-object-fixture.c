/* Selected CPU40 DPMI object; controlled register and allocator-query seams. */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static unsigned char guest[64]; static unsigned old_calls,sa_calls;void *ExtMemSA=(void*)0x1234;
uintptr_t __security_cookie=0x8424;
void __attribute__((fastcall)) __security_check_cookie(uintptr_t a){(void)a;}
unsigned short getES(void){return 1;}
void *Sim32pGetVDMPointer(DWORD a,unsigned char mode){(void)a;(void)mode;return guest;}
static DWORD di(void){return 8;} DWORD (*GetDIRegister)(void)=di;
LONG VdmQueryFreeVirtualMemory(ULONG *a,ULONG *b){++old_calls;*a=0x10000;*b=0x20000;return (LONG)0xc0000001;}
BOOL SAQueryFree(void *p,ULONG *a,ULONG *b){if(p!=ExtMemSA)ExitProcess(9);++sa_calls;*a=8*1024*1024;*b=4*1024*1024;return TRUE;}
extern void DpmiGetMemoryInfo(void);
int main(void){DWORD *m=(DWORD*)(guest+8);memset(guest,0x77,sizeof guest);DpmiGetMemoryInfo();
 printf("legacy=%u sa=%u largest=%lu free_pages=%lu max_unlocked=%lu prefix=%x suffix=%x\n",old_calls,sa_calls,m[0],m[5],m[1],guest[7],guest[44]);
 return old_calls==1&&sa_calls==1&&m[0]==4194304&&m[5]==2048&&m[1]==1024&&guest[7]==0x77&&guest[44]==0x77?0:1;
}
