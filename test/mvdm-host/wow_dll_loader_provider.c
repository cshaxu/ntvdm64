/* Controlled provider linked to the real bridge for a Windows loader test. */
#include <windows.h>

static HANDLE bitmap_event, registry_event;

static unsigned char __stdcall provider_result(unsigned long reason)
{
    char value[2];
    return reason != DLL_PROCESS_ATTACH ||
        (GetEnvironmentVariableA("WOW_BRIDGE_TEST_ATTACH", value, sizeof(value)) == 1 && value[0] == '1');
}

/* BOOLEAN is returned in AL; deliberately poison the upper EAX bytes. */
__declspec(naked) BOOLEAN __stdcall W32DllInitialize(void *instance,
    unsigned long reason, void *reserved)
{
    __asm {
        push dword ptr [esp+8]
        call provider_result
        or eax, 0a55a0000h
        ret 12
    }
}

__declspec(dllexport) void __stdcall test_set_events(HANDLE bitmap, HANDLE registry)
{
    bitmap_event = bitmap;
    registry_event = registry;
}

void __stdcall wow_bitmap_bindings_shutdown(void) { if (bitmap_event) SetEvent(bitmap_event); }
void __stdcall wow_registry_binding_shutdown(void) { if (registry_event) SetEvent(registry_event); }
