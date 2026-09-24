#include <windows.h>
#include <stdio.h>

/* This is the exact x86 layout read by original walias.c.  It is a probe,
 * not an alternate GDI implementation: S3 uses the result to decide whether
 * the modern system's private GdiQueryTable still satisfies this contract. */
typedef struct _ENTRYWOW {
    LONG l1;
    LONG l2;
    USHORT FullUnique;
    USHORT us1;
    LONG l3;
} ENTRYWOW;

typedef PVOID (WINAPI *gdi_query_table)(void);

int __cdecl main(void)
{
    HMODULE gdi = LoadLibraryA("gdi32.dll");
    gdi_query_table query;
    HDC dc;
    HBITMAP bitmap;
    ENTRYWOW *entries;
    DWORD handle;
    WORD guest;
    DWORD index;
    DWORD rebuilt = 0;
    int readable = 0;

    if (!gdi || !(query = (gdi_query_table)GetProcAddress(gdi,
            "GdiQueryTable"))) {
        fprintf(stderr, "GDI_QUERY_TABLE unavailable error=%lu\n",
            GetLastError());
        if (gdi) FreeLibrary(gdi);
        return 2;
    }
    if (!(dc = CreateCompatibleDC(NULL)) || !(bitmap = CreateCompatibleBitmap(
            dc, 1, 1))) {
        fprintf(stderr, "GDI_BITMAP_CREATE failed error=%lu\n", GetLastError());
        if (dc) DeleteDC(dc);
        FreeLibrary(gdi);
        return 3;
    }

    entries = (ENTRYWOW *)query();
    handle = (DWORD)(ULONG_PTR)bitmap;
    guest = (WORD)(handle << 2);
    index = (DWORD)(guest >> 2);
    __try {
        if (entries) {
            rebuilt = index | ((DWORD)entries[index].FullUnique << 16);
            readable = 1;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        readable = 0;
    }
    printf("GDI_QUERY_TABLE pointer=%p readable=%d handle=%08lX guest=%04X "
        "index=%04lX rebuilt=%08lX equal=%d\n", entries, readable,
        (unsigned long)handle, guest, (unsigned long)index,
        (unsigned long)rebuilt, readable && rebuilt == handle);

    DeleteObject(bitmap);
    DeleteDC(dc);
    FreeLibrary(gdi);
    return readable && rebuilt == handle ? 0 : 1;
}
