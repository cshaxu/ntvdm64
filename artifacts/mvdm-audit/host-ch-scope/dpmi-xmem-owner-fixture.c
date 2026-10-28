#include <windows.h>
#include <stdio.h>
static WORD bx,cx,dx,si,di;static int cf,allocs,frees;static ULONG fail_address;static ULONG freed[8];
WORD getBX(void){return bx;}WORD getCX(void){return cx;}WORD getDX(void){return dx;}WORD getSI(void){return si;}WORD getDI(void){return di;}
void setBX(WORD v){bx=v;}void setCX(WORD v){cx=v;}void setSI(WORD v){si=v;}void setDI(WORD v){di=v;}void setCF(int v){cf=v;}
LONG DpmiAllocateVirtualMemory(PVOID *address,ULONG *size){(void)size;*address=(PVOID)(ULONG_PTR)(0x40000UL+(unsigned long)allocs++*0x10000UL);return 0;}
LONG DpmiFreeVirtualMemory(PVOID *address,ULONG *size){ULONG v=(ULONG)(ULONG_PTR)*address;(void)size;freed[frees++]=v;return v==fail_address?(LONG)0xC0000001UL:0;}
BOOL DpmiReallocateVirtualMemory(PVOID a,ULONG b,PVOID*c,ULONG*d){(void)a;(void)b;(void)c;(void)d;return FALSE;}
void DpmiAllocateXmem(void);void DpmiFreeXmem(void);void DpmiFreeAppXmem(void);
static int make(WORD owner){dx=owner;bx=0;cx=0x1000;cf=9;DpmiAllocateXmem();return cf==0;}
int main(void){WORD bsi,bdi;if(!make(0x27)||!make(0x28))return 1;bsi=si;bdi=di;if(!make(0x27))return 2;dx=0x27;fail_address=0x40000;cf=0;DpmiFreeAppXmem();fprintf(stderr,"OWNER first calls=%d a=%08lx b=%08lx cf=%d\n",frees,(unsigned long)freed[0],(unsigned long)freed[1],cf);if(frees!=2||freed[0]!=0x60000||freed[1]!=0x40000||cf!=0)return 3;fail_address=0;DpmiFreeAppXmem();fprintf(stderr,"OWNER retry calls=%d c=%08lx\n",frees,(unsigned long)freed[2]);if(frees!=3||freed[2]!=0x40000)return 4;si=bsi;di=bdi;DpmiFreeXmem();fprintf(stderr,"OWNER other calls=%d d=%08lx cf=%d\n",frees,(unsigned long)freed[3],cf);if(frees!=4||freed[3]!=0x50000||cf)return 5;return 0;}
