/* Controlled host-side VDD for S44.  It uses only the original public VDD
 * API and is loaded through the original SetupInstallableVDD configuration
 * path, whose standalone binding reads package-local NTVDM.REG. */

#include <windows.h>
#include <nt_vdd.h>

static HANDLE s_module;
static void trace(const char *marker);
static VDD_IO_PORTRANGE s_port_range={0x00e9u,0x00e9u};
#if defined(S44_MEMORY_PROVIDER)
static BOOL s_memory_cycle;
static BOOL s_irq_cycle;
static BOOL s_dma_cycle;

static VOID on_memory_fault(PVOID address, ULONG access)
{
    (void)address;
    (void)access;
    trace("MEMORY_FAULT");
}
#endif

static VOID on_inb(WORD port, BYTE *data)
{
    (void)port;
    *data=0xffu;
    trace("IO_INB");
}

static VOID on_outb(WORD port, BYTE data)
{
    (void)port;
    (void)data;
    trace("IO_OUTB");
}

static VDD_IO_HANDLERS s_io_handlers={
    on_inb, NULL, NULL, NULL, on_outb, NULL, NULL, NULL
};

static void trace(const char *marker)
{
    CHAR path[MAX_PATH], line[96];
    HANDLE file;
    DWORD written;

    if (!GetEnvironmentVariableA("S44_VDD_TRACE_PATH", path, sizeof(path)))
        return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    wsprintfA(line, "%s\r\n", marker);
    WriteFile(file, line, (DWORD)lstrlenA(line), &written, NULL);
    CloseHandle(file);
}

static VOID on_create(USHORT pdb)
{
    (void)pdb;
    trace("CREATE");
#if defined(S44_TERMINATE_PROVIDER)
    trace("TERMINATE_VDM_REQUEST");
    VDDTerminateVDM();
#endif
#if defined(S44_MEMORY_PROVIDER)
    if (s_memory_cycle) trace("MEMORY_CYCLE_OK");
    if (s_irq_cycle) trace("IRQ_CYCLE_OK");
    if (s_dma_cycle) trace("DMA_CYCLE_OK");
#endif
}

static VOID on_terminate(USHORT pdb)
{
    (void)pdb;
    trace("TERMINATE");
}

static VOID on_block(VOID)
{
    trace("BLOCK");
}

static VOID on_resume(VOID)
{
    trace("RESUME");
}

BOOL WINAPI VDDInitialize(HANDLE module, DWORD reason, PCONTEXT context)
{
    (void)context;
    if (reason == DLL_PROCESS_ATTACH) {
        s_module = module;
#if defined(S44_ENTRY_USER_HOOK)
        if (!VDDInstallUserHook(s_module, on_create, on_terminate,
                on_block, on_resume))
            return FALSE;
#elif defined(S44_BOP_PROVIDER)
        trace("ATTACH");
        if (!VDDInstallUserHook(s_module, on_create, on_terminate,
                on_block, on_resume)) {
            trace("HOOK_INSTALL_FAIL");
            return FALSE;
        }
        trace("HOOK_INSTALL_OK");
        if (VDDInstallUserHook(NULL, on_create, on_terminate,
                on_block, on_resume)) {
            trace("HOOK_INVALID_UNEXPECTED");
            return FALSE;
        }
        trace("HOOK_INVALID_OK");
        if (!VDDInstallIOHook(s_module, 1, &s_port_range, &s_io_handlers)) {
            trace("IO_INSTALL_FAIL");
            return FALSE;
        }
        trace("IO_INSTALL_OK");
#elif defined(S44_MEMORY_PROVIDER)
        VDD_DMA_INFO dma;
        WORD irq;
        if (!VDDInstallUserHook(s_module, on_create, on_terminate,
                on_block, on_resume))
            return FALSE;
        if (!VDDInstallMemoryHook(s_module, (PVOID)0x000d0000u, 0x1000u,
                on_memory_fault))
            return FALSE;
        if (!VDDAllocMem(s_module, (PVOID)0x000d0000u, 0x1000u) ||
            !VDDFreeMem(s_module, (PVOID)0x000d0000u, 0x1000u) ||
            !VDDDeInstallMemoryHook(s_module, (PVOID)0x000d0000u, 0x1000u))
            return FALSE;
        s_memory_cycle = TRUE;
        irq = VDDReserveIrqLine(s_module, 0xffffu);
        if (irq == 0xffffu || !VDDReleaseIrqLine(s_module, irq))
            return FALSE;
        s_irq_cycle = TRUE;
        if (!VDDQueryDMA(s_module, 0, &dma) ||
            !VDDSetDMA(s_module, 0, VDD_DMA_ALL, &dma) ||
            VDDQueryDMA(s_module, 0xffffu, &dma) ||
            GetLastError() != ERROR_INVALID_ADDRESS)
            return FALSE;
        s_dma_cycle = TRUE;
#elif defined(S44_TERMINATE_PROVIDER)
        if (!VDDInstallUserHook(s_module, on_create, on_terminate,
                on_block, on_resume))
            return FALSE;
#else
#error S44 controlled VDD requires one explicit runtime scenario.
#endif
    } else if (reason == DLL_PROCESS_DETACH) {
#if defined(S44_ENTRY_USER_HOOK)
        VDDDeInstallUserHook(s_module);
#elif defined(S44_BOP_PROVIDER)
        trace("DETACH");
        if (!VDDDeInstallUserHook(s_module))
            trace("HOOK_REMOVE_FAIL");
        else
            trace("HOOK_REMOVE_OK");
#elif defined(S44_MEMORY_PROVIDER)
        if (!VDDDeInstallUserHook(s_module))
            trace("HOOK_REMOVE_FAIL");
        else
            trace("HOOK_REMOVE_OK");
#elif defined(S44_TERMINATE_PROVIDER)
        (void)VDDDeInstallUserHook(s_module);
#else
#error S44 controlled VDD requires one explicit runtime scenario.
#endif
    }
    return TRUE;
}

/* These two exports are deliberately reached by the original guest BOP 58
 * registration/dispatch/deregistration path, not by the VDD configuration
 * loader above.  The test deploys a second copy of this DLL for that route. */
VOID WINAPI S44BOPInit(VOID)
{
    trace("BOP_INIT");
}

VOID WINAPI S44BOPDispatch(VOID)
{
    trace("BOP_DISPATCH");
}
