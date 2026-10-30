#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "suballoc.h"
PVOID ExtMemSA;
static WORD bx,cx,dx,si,di; static int cf, fail_decommit; static ULONG calls;
WORD getBX(void){return bx;} WORD getCX(void){return cx;} WORD getDX(void){return dx;} WORD getSI(void){return si;} WORD getDI(void){return di;}
void setBX(WORD v){bx=v;} void setCX(WORD v){cx=v;} void setSI(WORD v){si=v;} void setDI(WORD v){di=v;} void setCF(int v){cf=v;}
static NTSTATUS commit_cb(ULONG addr, ULONG size){return VirtualAlloc((void*)(ULONG_PTR)addr,size,MEM_COMMIT,PAGE_READWRITE)==(void*)(ULONG_PTR)addr?0:(NTSTATUS)0xc0000001;}
static NTSTATUS decommit_cb(ULONG addr, ULONG size){calls++; if(fail_decommit){fail_decommit=0;return (NTSTATUS)0xc0000001;} return VirtualFree((void*)(ULONG_PTR)addr,size,MEM_DECOMMIT)?0:(NTSTATUS)0xc0000001;}
static VOID move_cb(ULONG d, ULONG s, ULONG n){memmove((void*)(ULONG_PTR)d,(void*)(ULONG_PTR)s,n);}
void DpmiAllocateXmem(void); void DpmiFreeAppXmem(void); void DpmiFreeXmem(void);
int main(int argc,char **argv){int inject=(argc>1 && strcmp(argv[1],"--inject")==0);void *base; ULONG a=0,b=0,avail,largest; WORD hs[32],hd[32]; MEMORY_BASIC_INFORMATION mbi; int i;
 base=VirtualAlloc(NULL,0x20000,MEM_RESERVE,PAGE_NOACCESS); if(!base)return 1;
 ExtMemSA=SAInitialize((ULONG)(ULONG_PTR)base,0x20000,commit_cb,decommit_cb,move_cb);if(!ExtMemSA)return 2;
 for(i=0;i<32;i++){dx=(i==16)?0x27:0x28;bx=0;cx=0x1000;DpmiAllocateXmem();if(cf)return 3;hs[i]=si;hd[i]=di;if(i==16)a=((ULONG)bx<<16)|cx;}
 fail_decommit=inject;dx=0x27;DpmiFreeAppXmem();if(!SAQueryFree(ExtMemSA,&avail,&largest))return 4;
 si=hs[0];di=hd[0];DpmiFreeXmem();if(cf)return 5;
 for(i=0;i<2;i++){dx=0x28;bx=0;cx=0x1000;DpmiAllocateXmem();if(cf)return 6;b=((ULONG)bx<<16)|cx;}
 *(volatile BYTE*)(ULONG_PTR)b=0x5a;dx=0x27;DpmiFreeAppXmem();if(!VirtualQuery((void*)(ULONG_PTR)b,&mbi,sizeof(mbi)))return 7;
 fprintf(stderr,"a=%08lx b=%08lx overlap=%d after_retry_state=%lx decommit_calls=%lu avail_after_failed_free=%lu\n",(unsigned long)a,(unsigned long)b,a==b,(unsigned long)mbi.State,(unsigned long)calls,(unsigned long)avail);
 return (a==b && avail==4096 && calls==(inject?3u:2u) && mbi.State==(inject?MEM_RESERVE:MEM_COMMIT))?0:8;}
