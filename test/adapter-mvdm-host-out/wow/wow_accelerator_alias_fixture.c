#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef WORD HAND16;
typedef ULONG HAND32;
typedef WORD HTASK16;
typedef struct _ACCELALIAS {
    struct _ACCELALIAS *lpNext, *lpPrev;
    HAND16 hInst;
    HTASK16 hTask16;
    WORD f16;
    HAND16 h16;
    HAND32 h32;
} ACCELALIAS, *LPACCELALIAS;
typedef struct { HTASK16 htask16; HAND16 hInst16; } TD;
static TD td;
static unsigned flags, destroys, errors;
LPACCELALIAS lpAccelAlias = NULL;
#define CURRENTPTD() (&td)
#define HANDLE_16BIT 1
#define HANDLE_32BIT 2
#define malloc_w_small(n) malloc(n)
#define free_w_small(p) free(p)
#define DestroyAcceleratorTable(h) destroy_table(h)
#define LOGDEBUG(a,b) ((void)0)
#define WOW32ASSERT(x) do {if(!(x))++errors;}while(0)
static ULONG SetCursorIconFlag(HAND16 h, BOOL v){if(h!=0x10&&h!=0x20)++errors;if(v)++flags;return 0;}
static BOOL destroy_table(HAND32 h){if(h!=0x101&&h!=0x202)++errors;++destroys;return TRUE;}
LPACCELALIAS FindAccelAlias(HANDLE hAccel, UINT fSize);
#pragma warning(disable:4100 4244 4305 4306) /* retained original 16/32 aliases */
#include "accel-alias.inc"
int main(void)
{
    td.htask16=1;td.hInst16=8;
    if(!SetupAccelAlias(8,0x10,0x101,TRUE)||!FindAccelAlias((HANDLE)(ULONG_PTR)0x10,HANDLE_16BIT))++errors;
    td.htask16=2;td.hInst16=9;
    if(!SetupAccelAlias(9,0x20,0x202,TRUE)||!FindAccelAlias((HANDLE)(ULONG_PTR)0x202,HANDLE_32BIT))++errors;
    /* Original task exit selects CURRENTPTD, not an unrelated caller value. */
    td.htask16=1;DestroyAccelAlias(2);
    if(FindAccelAlias((HANDLE)(ULONG_PTR)0x10,HANDLE_16BIT)||destroys!=1)++errors;
    td.htask16=2;
    if(!FindAccelAlias((HANDLE)(ULONG_PTR)0x20,HANDLE_16BIT))++errors;
    DestroyAccelAlias(2);
    if(lpAccelAlias||destroys!=2||flags!=2)++errors;
    printf("WOW_ACCEL_ALIAS_RETIRE errors=%u flags=%u destroys=%u\n",errors,flags,destroys);
    return errors?3:0;
}
