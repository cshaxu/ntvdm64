/* Test-only VDD: exercise NTVDM's own exports inside its real worker. */
#include <windows.h>
#include <nt_vdd.h>

typedef LONG (__cdecl *query_fn)(PULONG, PULONG);
typedef LONG (__cdecl *alloc_fn)(PULONG, ULONG, BOOL);
typedef LONG (__cdecl *realloc_fn)(ULONG, PULONG, ULONG);
typedef LONG (__cdecl *free_fn)(ULONG);

static HANDLE module;
static BOOL exercised;

static void report(const char *message)
{
    char path[MAX_PATH];
    DWORD count, written;
    HANDLE file;
    count = GetEnvironmentVariableA("NTVDM_EXPORT_TEST_TRACE", path, sizeof(path));
    if (!count || count >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    WriteFile(file, message, (DWORD)lstrlenA(message), &written, NULL);
    CloseHandle(file);
}

static VOID on_create(USHORT pdb)
{
    HMODULE parent = GetModuleHandleW(NULL);
    query_fn query = (query_fn)GetProcAddress(parent, "VdmQueryFreeVirtualMemory");
    realloc_fn resize = (realloc_fn)GetProcAddress(parent, "VdmReallocateVirtualMemory");
    alloc_fn allocate = (alloc_fn)GetProcAddress(parent, "VdmAllocateVirtualMemory");
    free_fn release = (free_fn)GetProcAddress(parent, "VdmFreeVirtualMemory");
    ULONG free_bytes = 0, largest = 0, address = 0, new_address = 0;
    ULONG before_free = 0, before_largest = 0, failed_address = 0x12345678u;
    (void)pdb;
    if (exercised) return;
    exercised = TRUE;
    if (!query || !resize || !allocate || !release ||
        (FARPROC)query != GetProcAddress(parent, MAKEINTRESOURCEA(270)) ||
        (FARPROC)resize != GetProcAddress(parent, MAKEINTRESOURCEA(271)) ||
        query(&free_bytes, &largest) != 0 || free_bytes < largest ||
        allocate(&address, 8192, TRUE) != 0) {
        report("FAIL resolve/query/allocate\r\n");
        return;
    }
    if (query(&before_free, &before_largest) != 0 || before_largest > 0xffffbfffu) {
        (void)release(address);
        report("FAIL failed-reallocate setup\r\n");
        return;
    }
    if (resize(address, &failed_address, before_largest + 16384u) == 0) {
        (void)release(failed_address);
        report("FAIL unexpected reallocation success\r\n");
        return;
    }
    if (failed_address != 0x12345678u) {
        (void)release(address);
        report("FAIL failed-reallocate output\r\n");
        return;
    }
    if (query(&free_bytes, &largest) != 0 || free_bytes != before_free ||
        largest != before_largest) {
        (void)release(address);
        report("FAIL failed-reallocate state\r\n");
        return;
    }
    if (resize(address, &new_address, 16384) != 0) {
        (void)release(address);
        report("FAIL reallocate\r\n");
        return;
    }
    if (release(new_address) != 0) {
        report("FAIL free\r\n");
        return;
    }
    report("PASS NTVDM export name/ordinal query/allocate/reallocate/free\r\n");
    report("PASS failed reallocation preserves allocation and free space\r\n");
}

static VOID on_terminate(USHORT pdb) { (void)pdb; }
static VOID on_block(VOID) { }
static VOID on_resume(VOID) { }

BOOL WINAPI VDDInitialize(HANDLE instance, DWORD reason, PCONTEXT context)
{
    (void)context;
    if (reason == DLL_PROCESS_ATTACH) {
        module = instance;
        return VDDInstallUserHook(module, on_create, on_terminate, on_block, on_resume);
    }
    if (reason == DLL_PROCESS_DETACH) (void)VDDDeInstallUserHook(module);
    return TRUE;
}
