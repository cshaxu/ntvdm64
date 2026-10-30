/* Isolated ABI witness, not a replacement WOW provider or loader test. */
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#if !defined(_MSC_VER) || !defined(_M_IX86)
#error This probe requires native MSVC x86.
#endif

typedef char boolean_return_is_one_byte[sizeof(BOOLEAN) == 1 ? 1 : -1];
extern BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID);
static unsigned cleanups;
static volatile unsigned char boolean_result;

/* BOOLEAN uses AL; the remaining EAX bits are not its return value. */
__declspec(naked) BOOLEAN __stdcall W32DllInitialize(
    void *instance, unsigned long reason, void *reserved)
{
    __asm {
        mov eax, 0a55a0000h
        or al, boolean_result
        ret 12
    }
}

void __stdcall wow_bitmap_bindings_shutdown(void) { ++cleanups; }
void __stdcall wow_registry_binding_shutdown(void) { ++cleanups; }

int main(void)
{
    int attach, attach_false, thread_false, detach_false;
    boolean_result = 1;
    attach = DllMain(0,1,0);
    boolean_result = 0;
    attach_false = DllMain(0,1,0);
    thread_false = DllMain(0,2,0);
    detach_false = DllMain(0,0,0);
    printf("attach=%08lx false_attach=%08lx thread_false=%08lx detach_false=%08lx cleanups=%u\n",
           (unsigned long)attach,(unsigned long)attach_false,(unsigned long)thread_false,
           (unsigned long)detach_false,cleanups);
    if (!attach || cleanups != 2) return 2;
#ifdef EXPECT_LEGACY_WIDE_RETURN
    if (attach_false != (int)UINT32_C(0xa55a0000) ||
        thread_false != (int)UINT32_C(0xa55a0000) ||
        detach_false != (int)UINT32_C(0xa55a0000)) return 3;
    puts("CONFIRMED actual bridge propagates non-return EAX bits from BOOLEAN");
#else
    if (attach_false || thread_false || detach_false) return 4;
    puts("PASS typed BOOLEAN bridge returns false and preserves cleanup");
#endif
    return 0;
}
