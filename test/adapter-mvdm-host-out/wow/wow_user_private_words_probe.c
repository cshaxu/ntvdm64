#include <windows.h>
#include <stdio.h>

/* Only inspect committed readable bytes in this process. The original WW
 * layout is eight DWORDs (wowuserp.h); a pointer alone proves no ABI match. */
static int snapshot_words(LONG address, DWORD *copy, SIZE_T size)
{
    MEMORY_BASIC_INFORMATION region;
    SIZE_T got = 0;
    ULONG_PTR p = (ULONG_PTR)(DWORD)address;
    ULONG_PTR end;
    if (!p || !VirtualQuery((void *)p, &region, sizeof(region))) return 0;
    end = (ULONG_PTR)region.BaseAddress + region.RegionSize;
    if (region.State != MEM_COMMIT || (region.Protect & PAGE_GUARD) ||
        (region.Protect & 0xff) == PAGE_NOACCESS || p > end || size > end - p)
        return 0;
    return ReadProcessMemory(GetCurrentProcess(), (void *)p, copy, size, &got)
        && got == size;
}

static void inspect_window_words(HWND window)
{
    DWORD before[8], after[8];
    LONG address = GetWindowLongA(window, -1);
    unsigned i;
    if (!snapshot_words(address, before, sizeof(before))) {
        puts("WW readable=0");
        return;
    }
    printf("WW readable=1 words=");
    for (i = 0; i < 8; ++i) printf(" %08lx", before[i]);
    printf("\nWW original_layout_style=%d original_layout_instance=%d\n",
        before[6] == (DWORD)GetWindowLongA(window, GWL_STYLE),
        before[7] == (DWORD)GetWindowLongA(window, GWL_HINSTANCE));
    /* Change only the test window's private WOW words through the original
     * setter ABI, never by writing the returned pointer. Restore each word. */
    for (i = 0; i < 3; ++i) {
        DWORD set_error, restore_error;
        LONG previous;
        DWORD marker = 0x13570000u + i;
        SetLastError(0);
        previous = SetWindowLongA(window, -30 - (int)i, (LONG)marker);
        set_error = GetLastError();
        printf("WW slot=%u previous=%08lx set_error=%lu", i,
            (DWORD)previous, set_error);
        if (snapshot_words(GetWindowLongA(window, -1), after, sizeof(after)))
            printf(" offset_value=%08lx matches=%d", after[i], after[i] == marker);
        SetLastError(0);
        if (!set_error)
            SetWindowLongA(window, -30 - (int)i, previous);
        restore_error = GetLastError();
        printf(" restore_error=%lu restored=%d\n", restore_error,
            snapshot_words(GetWindowLongA(window, -1), after, sizeof(after)) &&
            after[i] == before[i]);
    }
}

static void inspect_class_words(HWND window)
{
    int i;
    for (i = 0; i < 2; ++i) {
        DWORD previous, error, words[2];
        SetLastError(0);
        previous = SetClassLongA(window, -28 - i, 0x24680000u + i);
        error = GetLastError();
        printf("WC slot=%d previous=%08lx set_error=%lu readable=%d\n",
            i, previous, error,
            snapshot_words((LONG)GetClassLongA(window, -27), words, sizeof(words)));
        if (!error) {
            SetLastError(0);
            SetClassLongA(window, -28 - i, previous);
            printf("WC slot=%d restore_error=%lu\n", i, GetLastError());
        }
    }
}

/* Inspect only a window owned by this test process. No visible UI, private
 * system mutation, guest execution or assumption that missing slots work. */
static int inspect_handle_reuse(HINSTANCE instance, LPCSTR class_name)
{
    DWORD *seen = (DWORD *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        65536u * sizeof(DWORD));
    unsigned attempt;
    int result = 0;
    if (!seen) return 5;
    for (attempt = 1; attempt <= 4096u; ++attempt) {
        HWND window = CreateWindowExA(0, class_name, "", 0,
            0, 0, 1, 1, HWND_MESSAGE, NULL, instance, NULL);
        WORD low;
        DWORD full, prior;
        HWND narrow;
        if (!window) { result = 6; break; }
        full = (DWORD)(UINT_PTR)window;
        low = (WORD)full;
        prior = seen[low];
        narrow = (HWND)(INT_PTR)(SHORT)low;
        SetWindowLongA(window, GWLP_USERDATA, (LONG)attempt);
        if (prior && prior != full) {
            BOOL stale_valid = IsWindow((HWND)(UINT_PTR)prior);
            BOOL alias_matches = GetWindowLongA(narrow, GWLP_USERDATA) ==
                (LONG)attempt;
            printf("HWND_REUSE observed=1 attempt=%u low=%04x old=%08lx "
                "new=%08lx stale_valid=%d narrow_matches_new=%d\n",
                attempt, low, prior, full, stale_valid, alias_matches);
            if (stale_valid || !alias_matches) result = 7;
            if (!DestroyWindow(window)) result = 8;
            break;
        }
        seen[low] = full;
        if (!DestroyWindow(window)) { result = 8; break; }
    }
    if (attempt > 4096u) puts("HWND_REUSE observed=0 bounded_attempts=4096");
    HeapFree(GetProcessHeap(), 0, seen);
    return result;
}

int main(void)
{
    WNDCLASSA cls = {0};
    HWND window;
    HWND narrow;
    LONG words;
    DWORD error;
    int result = 0;

    cls.hInstance = GetModuleHandleA(NULL);
    cls.lpfnWndProc = DefWindowProcA;
    cls.lpszClassName = "NtvdmWowPrivateWordsProbe";
    if (!RegisterClassA(&cls)) return 1;
    window = CreateWindowExA(0, cls.lpszClassName, "", 0,
        0, 0, 1, 1, HWND_MESSAGE, NULL, cls.hInstance, NULL);
    if (!window) {
        UnregisterClassA(cls.lpszClassName, cls.hInstance);
        return 2;
    }
    SetLastError(0);
    words = GetWindowLongA(window, -1); /* OpenNT GWL_WOWWORDS */
    error = GetLastError();
    printf("GWL_WOWWORDS value=%08lx error=%lu\n", (DWORD)words, error);
    SetLastError(0);
    words = (LONG)GetClassLongA(window, -27); /* OpenNT GCL_WOWWORDS */
    error = GetLastError();
    printf("GCL_WOWWORDS value=%08lx error=%lu\n", (DWORD)words, error);
    inspect_window_words(window);
    inspect_class_words(window);
    narrow = (HWND)(INT_PTR)(SHORT)(WORD)(UINT_PTR)window;
    printf("HWND native=%p low16_sign_extended=%p native_valid=%d narrow_valid=%d\n",
        (void *)window, (void *)narrow, IsWindow(window), IsWindow(narrow));
    if (!DestroyWindow(window)) result = 3;
    printf("destroyed_native_valid=%d destroyed_narrow_valid=%d\n",
        IsWindow(window), IsWindow(narrow));
    if (result == 0) result = inspect_handle_reuse(cls.hInstance, cls.lpszClassName);
    if (!UnregisterClassA(cls.lpszClassName, cls.hInstance)) result = 4;
    return result;
}
