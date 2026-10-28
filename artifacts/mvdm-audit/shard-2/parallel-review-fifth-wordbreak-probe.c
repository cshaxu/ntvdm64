/* Isolated original-object probe. Fake only the guest linear-memory mapping
 * and callback; real wcall32.obj and walloc16.obj own copy and allocation. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef DWORD VPVOID;
VPVOID __stdcall malloc16(UINT);
BOOL __stdcall free16(VPVOID);
int __stdcall W32EditNextWord(char *, int, int, int, DWORD);
extern UINT viFreeIndex;
extern BYTE vahdr[];

static BYTE heap[0x2000];
static unsigned callbacks;

/* Unrelated retained object sections fail closed if accidentally reached. */
HANDLE __stdcall LocalAlloc16(WORD a, int b, HANDLE c) { (void)a;(void)b;(void)c; abort(); }
HANDLE __stdcall LocalReAlloc16(HANDLE a, int b, WORD c) { (void)a;(void)b;(void)c; abort(); }
DWORD __stdcall LocalLock16(HANDLE a) { (void)a; abort(); }
BOOL __stdcall LocalUnlock16(HANDLE a) { (void)a; abort(); }
DWORD __stdcall LocalSize16(HANDLE a) { (void)a; abort(); }
HANDLE __stdcall LocalFree16(HANDLE a) { (void)a; abort(); }
DWORD __stdcall ParamMapUpdateNode(DWORD a, DWORD b, void *c) { (void)a;(void)b;(void)c; abort(); }
LONG __stdcall wow_user_get_window_longA(HWND a, int b) { (void)a;(void)b; abort(); }
LONG __stdcall wow_user_set_window_longA(HWND a, int b, LONG c) { (void)a;(void)b;(void)c; abort(); }
void *__stdcall opennt_support_current_teb(void) { abort(); }

VPVOID __stdcall WOWGlobalAllocLock16(WORD flags, DWORD bytes, void *unused)
{
    (void)flags; (void)unused;
    return bytes == sizeof(heap) ? 0x10000 : 0;
}

void __stdcall WOWGlobalUnlockFree16(VPVOID block)
{
    (void)block;
}

void *__stdcall GetPModeVDMPointerAssert(DWORD guest)
{
    if (guest < 0x10000 || guest >= 0x10000 + sizeof(heap)) return NULL;
    return heap + (guest - 0x10000);
}

BOOL __stdcall CallBack16(int type, void *arguments, DWORD proc, DWORD *result)
{
    (void)type; (void)arguments; (void)proc;
    ++callbacks;
    *result = 7;
    return TRUE;
}

static int run_case(unsigned length)
{
    char text[258];
    VPVOID first, adjacent;
    int result;
    memset(text, 'A', length); text[length] = '\0';
    first = malloc16(256);
    adjacent = malloc16(256);
    if (first != 0x10000 || adjacent != 0x10100) return -1;
    heap[256] = 0x5a;
    if (!free16(first)) return -2;
    result = W32EditNextWord(text, 0, (int)length, 0, 0x20007);
    if (result != 7 || callbacks == 0 || (vahdr[1] & 1) == 0) return -3;
    result = heap[256];
    if (!free16(adjacent)) return -4;
    viFreeIndex = 0; /* Keep the two controlled layouts identical. */
    return result;
}

int main(int argc, char **argv)
{
    int safe, boundary;
    (void)argv;
    safe = run_case(255);
    boundary = run_case(256);
    printf("length255_adjacent=%d length256_adjacent=%d callbacks=%u\n", safe, boundary, callbacks);
    if (safe < 0 || boundary < 0 || callbacks != 2) return 2;
    /* --strict tests the invariant rather than unconditionally failing. */
    if (argc > 1) return safe != 0x5a || boundary != 0x5a;
    return safe != 0x5a || boundary != 0;
}
