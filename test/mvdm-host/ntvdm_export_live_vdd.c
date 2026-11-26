/* Test-only VDD: exercise NTVDM's own exports inside its real worker. */
#include <windows.h>
#include <nt_vdd.h>
#include <vdmdbg.h>
#include <dbginfo.h>

typedef LONG (__cdecl *query_fn)(PULONG, PULONG);
typedef LONG (__cdecl *alloc_fn)(PULONG, ULONG, BOOL);
typedef LONG (__cdecl *realloc_fn)(ULONG, PULONG, ULONG);
typedef LONG (__cdecl *free_fn)(ULONG);

static HANDLE module;
static BOOL exercised;
static BOOL completed;
static USHORT exercised_pdb;

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

static BOOL check_debug_register_exports(HMODULE parent)
{
    static const char *const names[][2] = {
        {"getEAX", "setEAX"}, {"getEBX", "setEBX"}, {"getECX", "setECX"},
        {"getEDX", "setEDX"}, {"getESI", "setESI"}, {"getEDI", "setEDI"},
        {"getEBP", "setEBP"}, {"getESP", "setESP"}, {"getEIP", "setEIP"},
        {"getEFLAGS", "setEFLAGS"}
    };
    typedef ULONG (WINAPI *read32)(void);
    typedef void (WINAPI *write32)(ULONG);
    typedef USHORT (WINAPI *read16)(void);
    typedef void (WINAPI *write16)(USHORT);
    typedef UINT (WINAPI *write_segment)(USHORT);
    read16 ax = (read16)GetProcAddress(parent, "getAX");
    write16 set_ax = (write16)GetProcAddress(parent, "setAX");
    read16 es = (read16)GetProcAddress(parent, "getES");
    write_segment set_es = (write_segment)GetProcAddress(parent, "setES");
    USHORT saved_ax, saved_es;
    size_t i;
    BOOL ok;

    for (i = 0; i < ARRAYSIZE(names); ++i) {
        read32 read = (read32)GetProcAddress(parent, names[i][0]);
        write32 write = (write32)GetProcAddress(parent, names[i][1]);
        ULONG saved;
        if (!read || !write) return FALSE;
        saved = read();
        write(saved ^ 1u);
        ok = read() == (saved ^ 1u);
        write(saved);
        if (!ok || read() != saved) return FALSE;
    }

    if (!ax || !set_ax || !es || !set_es) return FALSE;
    saved_ax = ax();
    set_ax((USHORT)(saved_ax ^ 1u));
    ok = ax() == (USHORT)(saved_ax ^ 1u);
    set_ax(saved_ax);
    saved_es = es();
    set_es(saved_es);
    return ok && ax() == saved_ax && es() == saved_es;
}

/* Observe the DLL's synchronous debug notification while the DOS CPU is stopped. */
static DWORD event_thread;
static ULONG event_eax;
static PVOID event_ram;
static BOOL saw_break;

static LONG CALLBACK observe_break(EXCEPTION_POINTERS *pointers)
{
    EXCEPTION_RECORD *event = pointers->ExceptionRecord;
    VDMINTERNALINFO *info;
    VDMCONTEXT *context;
    if (GetCurrentThreadId() != event_thread ||
        event->ExceptionCode != STATUS_VDM_EVENT || event->NumberParameters != 4 ||
        LOWORD(event->ExceptionInformation[0]) != DBG_BREAK)
        return EXCEPTION_CONTINUE_SEARCH;
    info = (VDMINTERNALINFO *)event->ExceptionInformation[3];
    if (!info || info->dwIntelBase != (ULONG_PTR)event_ram || !info->vdmContext)
        return EXCEPTION_CONTINUE_SEARCH;
    report("EVENT received break and matching SAS base\r\n");
    context = (VDMCONTEXT *)info->vdmContext;
    if (context->Eax != event_eax) return EXCEPTION_CONTINUE_SEARCH;
    context->Eax ^= 0x40000000u;
    saw_break = TRUE;
    return EXCEPTION_CONTINUE_EXECUTION;
}

static BOOL check_debug_event(HMODULE worker, HMODULE dll)
{
    typedef void (__cdecl *attach_fn)(void);
    typedef BOOL (WINAPI *query_fn)(void);
    typedef void (WINAPI *notify_fn)(BOOL);
    typedef ULONG (WINAPI *read_fn)(void);
    typedef void (WINAPI *write_fn)(ULONG);
    typedef PVOID (WINAPI *map_fn)(USHORT, ULONG, VDM_MODE);
    attach_fn attach = (attach_fn)GetProcAddress(worker, "VdmDbgAttach");
    map_fn map = (map_fn)GetProcAddress(worker, "VdmMapFlat");
    query_fn debugged = (query_fn)GetProcAddress(dll, "xxxDbgIsDebuggee");
    notify_fn notify = (notify_fn)GetProcAddress(dll, "xxxDbgNotifyDebugged");
    query_fn breakpoint = (query_fn)GetProcAddress(dll, "xxxDbgBPInt");
    query_fn trace = (query_fn)GetProcAddress(dll, "xxxDbgTraceInt");
    read_fn eax = (read_fn)GetProcAddress(worker, "getEAX");
    write_fn set_eax = (write_fn)GetProcAddress(worker, "setEAX");
    BOOL previous, ok;
    PVOID handler;
    if (!attach || !map || !debugged || !notify || !breakpoint || !trace || !eax || !set_eax)
        return FALSE;
    attach();
    event_ram = map(0, 0, VDM_V86);
    if (!event_ram || map(0, 0, VDM_PM)) return FALSE;
    handler = AddVectoredExceptionHandler(1, observe_break);
    if (!handler) return FALSE;
    previous = debugged();
    event_eax = eax();
    event_thread = GetCurrentThreadId();
    saw_break = FALSE;
    notify(TRUE);
    ok = breakpoint() && saw_break && eax() == (event_eax ^ 0x40000000u);
    set_eax(event_eax);
    saw_break = FALSE;
    ok = trace() && saw_break && eax() == (event_eax ^ 0x40000000u) && ok;
    set_eax(event_eax);
    notify(previous);
    event_thread = 0;
    event_ram = NULL;
    RemoveVectoredExceptionHandler(handler);
    return ok;
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
    exercised_pdb = pdb;
    if (!GetModuleHandleW(L"NTVDMD.DLL")) {
        report("FAIL worker did not load NTVDMD.DLL\r\n");
        return;
    }
    report("PASS worker loaded NTVDMD.DLL\r\n");
    if (!check_debug_event(parent, GetModuleHandleW(L"NTVDMD.DLL"))) {
        report("FAIL XP debug event writeback\r\n");
        return;
    }
    report("PASS XP debug event writeback\r\n");
    if (!check_debug_register_exports(parent)) {
        report("FAIL XP debugger register round trip\r\n");
        return;
    }
    report("PASS XP debugger register round trip\r\n");
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
    completed = TRUE;
}

static VOID on_terminate(USHORT pdb)
{
    query_fn query;
    ULONG free_bytes = 0, largest = 0;
    if (!exercised || pdb != exercised_pdb) return;
    query = (query_fn)GetProcAddress(GetModuleHandleW(NULL), "VdmQueryFreeVirtualMemory");
    if (!completed || !query || query(&free_bytes, &largest) != 0 ||
        free_bytes < largest) {
        report("FAIL export query at matching task termination\r\n");
        return;
    }
    report("PASS export query at matching task termination\r\n");
}
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
