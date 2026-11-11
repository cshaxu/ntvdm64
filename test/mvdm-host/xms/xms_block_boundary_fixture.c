/* Actual original XMS block provider and suballocator; register/callback seams only. */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "suballoc.h"
#define BASE 0x110000u
#define SIZE 0x10000u
PVOID ExtMemSA;
static uint16_t ax,bx,cx,dx;
static unsigned checks, failures, commits, decommits, moves;
static unsigned char bytes[SIZE];
USHORT getAX(void){return ax;} USHORT getBX(void){return bx;} USHORT getDX(void){return dx;}
void setAX(USHORT v){ax=v;} void setCX(USHORT v){cx=v;}
void fixture_unexpected(void){fprintf(stderr,"Unexpected out-of-scope XMS move/query entry\n");abort();}
void xmsAllocBlock(void); void xmsReallocBlock(void); void xmsFreeBlock(void);
#define CHECK(x) do {++checks;if(!(x)){++failures;printf("FAIL line %d: %s\n",__LINE__,#x);}}while(0)
static NTSTATUS commit(ULONG a,ULONG n){CHECK(a>=BASE && n<=SIZE && a-BASE<=SIZE-n);++commits;return 0;}
static NTSTATUS decommit(ULONG a,ULONG n){CHECK(a>=BASE && n<=SIZE && a-BASE<=SIZE-n);++decommits;return 0;}
static void move(ULONG d,ULONG s,ULONG n){CHECK(d>=BASE && s>=BASE && n<=SIZE && d-BASE<=SIZE-n && s-BASE<=SIZE-n);++moves;memmove(bytes+d-BASE,bytes+s-BASE,n);}
static ULONG freebytes(void){ULONG f,l;CHECK(SAQueryFree(ExtMemSA,&f,&l));return f;}
int main(void){
 unsigned i;USHORT block,grown;ULONG initial;unsigned before;
 ExtMemSA=SAInitialize(BASE,SIZE,commit,decommit,move);CHECK(ExtMemSA!=NULL);initial=freebytes();CHECK(initial==SIZE);
 for(i=0;i<32;++i){dx=0;ax=0xdead;xmsAllocBlock();CHECK(ax==BASE/1024);CHECK(freebytes()==initial);}
 CHECK(commits==0 && decommits==0 && moves==0);
 dx=8;xmsAllocBlock();block=ax;CHECK(block==BASE/1024);memset(bytes,0x5a,8192);CHECK(freebytes()==initial-8192);
 before=commits;ax=block;dx=8;bx=8;cx=0xdead;xmsReallocBlock();CHECK(cx==block);CHECK(commits==before && moves==0);CHECK(bytes[0]==0x5a && bytes[8191]==0x5a);
 dx=0;xmsAllocBlock();CHECK(ax==BASE/1024);CHECK(freebytes()==initial-8192);
 dx=0;bx=0;cx=0xdead;xmsReallocBlock();CHECK(cx==BASE/1024);
 ax=BASE/1024;dx=0;bx=1;xmsReallocBlock();grown=cx;CHECK(grown!=0 && grown!=block);CHECK(freebytes()==initial-9216);CHECK(bytes[0]==0x5a && bytes[8191]==0x5a);
 ax=grown;dx=1;xmsFreeBlock();CHECK(ax==1);ax=block;dx=8;xmsFreeBlock();CHECK(ax==1);CHECK(freebytes()==initial);
 dx=0xffff;xmsAllocBlock();CHECK(ax==0);CHECK(freebytes()==initial);
 dx=4;xmsAllocBlock();block=ax;CHECK(block!=0);memset(bytes+(ULONG)block*1024-BASE,0x3c,4096);
 ax=block;dx=4;bx=8;xmsReallocBlock();grown=cx;CHECK(grown!=0);CHECK(bytes[(ULONG)grown*1024-BASE]==0x3c);CHECK(bytes[(ULONG)grown*1024-BASE+4095]==0x3c);
 ax=grown;dx=8;xmsFreeBlock();CHECK(ax==1);CHECK(freebytes()==initial);
 printf("XMS boundary assertions=%u failures=%u\n",checks,failures);return failures?1:0;
}
