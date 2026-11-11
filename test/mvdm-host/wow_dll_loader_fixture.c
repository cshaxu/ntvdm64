#include <stdio.h>
#include <windows.h>

typedef void (__stdcall *set_events_fn)(HANDLE, HANDLE);

int main(int argc, char **argv)
{
    HMODULE module;
    HANDLE bitmap, registry;
    set_events_fn set_events;
    unsigned round;
    if (argc != 2) return 2;
    for (round = 0; round < 8; ++round) {
        SetEnvironmentVariableA("WOW_BRIDGE_TEST_ATTACH", "0");
        SetLastError(ERROR_SUCCESS);
        module = LoadLibraryA(argv[1]);
        if (module) { FreeLibrary(module); return 3; }
        if (GetLastError() != ERROR_DLL_INIT_FAILED) return 4;
        SetEnvironmentVariableA("WOW_BRIDGE_TEST_ATTACH", "1");
        bitmap = CreateEventA(NULL, TRUE, FALSE, NULL);
        registry = CreateEventA(NULL, TRUE, FALSE, NULL);
        if (!bitmap || !registry) return 5;
        module = LoadLibraryA(argv[1]);
        if (!module) return 6;
        set_events = (set_events_fn)GetProcAddress(module, "_test_set_events@8");
        if (!set_events) return 7;
        set_events(bitmap, registry);
        if (!FreeLibrary(module)) return 8;
        if (WaitForSingleObject(bitmap, 0) != WAIT_OBJECT_0 ||
            WaitForSingleObject(registry, 0) != WAIT_OBJECT_0) return 9;
        CloseHandle(bitmap);
        CloseHandle(registry);
    }
    puts("WOW_BRIDGE_OS_LOADER attach_false=failed attach_true=loaded unload=1 bitmap=1 registry=1 rounds=8");
    return 0;
}
