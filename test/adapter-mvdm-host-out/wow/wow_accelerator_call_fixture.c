#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The production body is extracted from current waccel.c at test time. */
typedef void *VPVOID;
typedef char *PSZ;
#define GETVDMPTR(v,s,p) do {(void)(s);(p)=(PSZ)(v);}while(0)
#define FREEVDMPTR(p) ((void)(p))
#define FETCHWORD(v) ((WORD)(v))
#define malloc_w(n) malloc(n)
#define free_w(p) free(p)
#define LOGDEBUG(a,b) ((void)0)
#define LOG_ALWAYS 0
#define DBG 0
#pragma warning(disable:4213) /* retained original lvalue pointer increments */
#include "accel-create32.inc"

static int check(HACCEL table, UINT count, const ACCEL *expected)
{
    ACCEL actual[4] = {{0}};
    UINT copied=CopyAcceleratorTable(table,actual,4);
    UINT i;
    if(copied!=count){printf("copied=%u expected=%u\n",copied,count);return 1;}
    for(i=0;i<count;++i){
        if(actual[i].fVirt!=expected[i].fVirt||actual[i].key!=expected[i].key||actual[i].cmd!=expected[i].cmd){
            printf("entry=%u got=%u,%u,%u expected=%u,%u,%u\n",i,actual[i].fVirt,actual[i].key,actual[i].cmd,expected[i].fVirt,expected[i].key,expected[i].cmd);return 1;
        }
    }
    return 0;
}
int main(void)
{
    BYTE records[11]={FVIRTKEY,'A',0,100,0,FVIRTKEY|0x80,'B',0,101,0,0xff};
    /* The final 16-bit 0x80 sentinel delimits the source resource. USER32's
     * native table intentionally stores only the public FVIRTKEY bits. */
    ACCEL expected[2]={{FVIRTKEY,'A',100},{FVIRTKEY,'B',101}};
    HACCEL table=CreateAccel32(records,sizeof(records));
    int errors=check(table,2,expected);
    if(table)DestroyAcceleratorTable(table);
    /* A partial tail must be ignored by the original 5-byte record rule. */
    printf("WOW_ACCEL_CREATE32 errors=%d entries=2 partial_tail=1\n",errors);
    return errors?3:0;
}
