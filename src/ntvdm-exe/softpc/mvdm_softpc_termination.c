#include "mvdm_softpc_termination.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "ntvdm-exe/session/session.h"

/* Original host close is idempotent: it rejects both pre-start and repeated
 * calls. Direct standalone termination otherwise longjmps around the normal
 * original host_main -> host_applClose tail. */
extern void host_applClose(void);

void mvdm_softpc_report_termination(char const *stage,
    void const *return_address)
{
    char path[MAX_PATH];
    DWORD bytes;
    HANDLE file;
    char line[64];
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_TERMINATE_VDM_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line), "%s return=%p\r\n", stage, return_address);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_virtual_memory_failure(unsigned long status)
{
    char path[MAX_PATH];
    DWORD bytes;
    HANDLE file;
    char line[64];
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_TERMINATE_VDM_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line), "vdm-allocate status=%08lx\r\n", status);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_bop_state(unsigned long cr0, unsigned long cr3,
    unsigned short cs, unsigned short ip, unsigned long ds_base,
    unsigned long ds_limit)
{
    char path[MAX_PATH];
    char line[144];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S41_WOW_BOP_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "wow-bop cr0=%08lx cr3=%08lx csip=%04x:%04x dsbase=%08lx dslimit=%08lx\r\n",
        cr0, cr3, (unsigned int)cs, (unsigned int)ip, ds_base, ds_limit);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_wow_dispatch_return_state(unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip)
{
    char path[MAX_PATH];
    char line[112];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S42_WOW_RETURN_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "wow-dispatch-return cr0=%08lx cr3=%08lx csip=%04x:%04x\r\n",
        cr0, cr3, (unsigned int)cs, (unsigned int)ip);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_mode_state(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip,
    unsigned long ds_base, unsigned long ds_limit)
{
    char path[MAX_PATH];
    char line[112];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S41_DPMI_MODE_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (stage == NULL || bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "%s cr0=%08lx cr3=%08lx csip=%04x:%04x dsbase=%08lx dslimit=%08lx\r\n",
        stage, cr0, cr3, (unsigned int)cs, (unsigned int)ip, ds_base, ds_limit);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_descriptor(unsigned short selector,
    unsigned long source_low, unsigned long source_high,
    unsigned long gdt_low, unsigned long gdt_high, unsigned long ldt_low,
    unsigned long ldt_high)
{
    char path[MAX_PATH];
    char line[208];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S42_DPMI_DESCRIPTOR_REPORT_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "selector=%04x source=%08lx:%08lx gdt=%08lx:%08lx ldt=%08lx:%08lx\r\n",
        (unsigned int)selector, source_low, source_high, gdt_low, gdt_high,
        ldt_low, ldt_high);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

void mvdm_softpc_report_dpmi_set_descriptor(unsigned short es,
    unsigned short bx, unsigned short ax, unsigned short cx,
    unsigned long descriptor_pointer, unsigned long protected_mode)
{
    char path[MAX_PATH];
    char line[176];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes=GetEnvironmentVariableA("MVDM_S42_DPMI_SET_DESCRIPTOR_REPORT_PATH",
        path, (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) return;
    file=CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return;
    snprintf(line, sizeof(line),
        "dpmi-setdesc esbx=%04x:%04x ax=%04x cx=%04x ptr=%08lx pm=%lu\r\n",
        (unsigned int)es, (unsigned int)bx, (unsigned int)ax,
        (unsigned int)cx, descriptor_pointer, protected_mode);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
}

int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code)
{
    (void)vdm_for_wow;
    /* Restore the original host close cohort before the standalone escape.
     * In NT4 this escape was ExitProcess; in this process it is longjmp, so
     * an outer application entry cannot be the cleanup owner. */
    host_applClose();
    return session_terminate_current(completion_code);
}
