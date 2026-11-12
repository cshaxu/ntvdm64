#include "mvdm_softpc_termination.h"

#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "ntvdm-exe/session/session.h"

/* Original host close is idempotent: it rejects both pre-start and repeated
 * calls. Direct standalone termination otherwise longjmps around the normal
 * original host_main -> host_applClose tail. */
extern void host_applClose(void);

void mvdm_softpc_report_xms_move(unsigned long source,
    unsigned long destination, unsigned long byte_count, int completed)
{
    static __declspec(thread) char path[MAX_PATH];
    static __declspec(thread) unsigned initialized, count;
    DWORD saved = GetLastError(), bytes, written;
    HANDLE file;
    char line[176];

    if (!initialized) {
        bytes = GetEnvironmentVariableA("MVDM_WIN31_SETUP_XMS_TRACE_PATH",
            path, (DWORD)sizeof(path));
        if (bytes == 0u || bytes >= sizeof(path)) path[0] = 0;
        initialized = 1u;
    }
    if (path[0] && count++ < 256u) {
        file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE) {
            bytes = (DWORD)snprintf(line, sizeof(line),
                "tid=%lu xms-move %s src=%08lx dst=%08lx bytes=%08lx\r\n",
                GetCurrentThreadId(), completed ? "done" : "begin", source,
                destination, byte_count);
            if (bytes < sizeof(line)) (void)WriteFile(file, line, bytes, &written, NULL);
            CloseHandle(file);
        }
    }
    SetLastError(saved);
}

void mvdm_softpc_report_wow_bop_state(unsigned long cr0, unsigned long cr3,
    unsigned short cs, unsigned short ip, unsigned long ds_base,
    unsigned long ds_limit)
{
    DWORD saved_error = GetLastError();
    char path[MAX_PATH];
    char line[144];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S41_WOW_BOP_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) goto restore_error;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) goto restore_error;
    snprintf(line, sizeof(line),
        "wow-bop cr0=%08lx cr3=%08lx csip=%04x:%04x dsbase=%08lx dslimit=%08lx\r\n",
        cr0, cr3, (unsigned int)cs, (unsigned int)ip, ds_base, ds_limit);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
restore_error:
    SetLastError(saved_error);
}

void mvdm_softpc_report_wow_stage(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip)
{
    DWORD saved_error = GetLastError();
    char path[MAX_PATH];
    char line[144];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    if (stage == NULL) goto restore_error;
    bytes = GetEnvironmentVariableA("MVDM_S42_WOW_RETURN_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) goto restore_error;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) goto restore_error;
    snprintf(line, sizeof(line),
        "wow-stage stage=%s cr0=%08lx cr3=%08lx csip=%04x:%04x\\r\\n",
        stage, cr0, cr3, (unsigned int)cs, (unsigned int)ip);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
restore_error:
    SetLastError(saved_error);
}

void mvdm_softpc_report_wow_fast_callback_binding(char const *stage,
    unsigned long address)
{
    DWORD saved_error = GetLastError();
    char path[MAX_PATH];
    char line[128];
    DWORD bytes;
    DWORD written;
    HANDLE file;

    if (stage == NULL) goto restore_error;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) goto restore_error;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) goto restore_error;
    snprintf(line, sizeof(line), "wow-fast-binding stage=%s address=%08lx\\r\\n",
        stage, address);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
restore_error:
    SetLastError(saved_error);
}

void mvdm_softpc_report_dpmi_mode_state(char const *stage, unsigned long cr0,
    unsigned long cr3, unsigned short cs, unsigned short ip,
    unsigned long ds_base, unsigned long ds_limit)
{
    DWORD saved_error = GetLastError();
    char path[MAX_PATH];
    char line[112];
    DWORD bytes;
    HANDLE file;
    DWORD written;

    bytes = GetEnvironmentVariableA("MVDM_S41_DPMI_MODE_REPORT_PATH", path,
        (DWORD)sizeof(path));
    if (stage == NULL || bytes == 0u || bytes >= sizeof(path)) goto restore_error;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) goto restore_error;
    snprintf(line, sizeof(line),
        "%s cr0=%08lx cr3=%08lx csip=%04x:%04x dsbase=%08lx dslimit=%08lx\r\n",
        stage, cr0, cr3, (unsigned int)cs, (unsigned int)ip, ds_base, ds_limit);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
restore_error:
    SetLastError(saved_error);
}

void mvdm_softpc_report_cpu40_descriptor_publish(unsigned short selector,
    unsigned long source_low, unsigned long source_high,
    unsigned long gdt_low, unsigned long gdt_high, unsigned long ldt_low,
    unsigned long ldt_high)
{
    DWORD saved_error = GetLastError();
    char path[MAX_PATH], filter[32], line[224];
    DWORD bytes, written;
    unsigned long selected;
    HANDLE file;

    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH", path,
        (DWORD)sizeof(path));
    if (bytes == 0u || bytes >= sizeof(path)) goto restore_error;
    bytes = GetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR", filter,
        (DWORD)sizeof(filter));
    if (bytes == 0u || bytes >= sizeof(filter)) goto restore_error;
    selected = strtoul(filter, NULL, 16);
    if ((unsigned short)selected != selector) goto restore_error;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) goto restore_error;
    snprintf(line, sizeof(line),
        "descriptor-publish selector=%04x source=%08lx:%08lx gdt=%08lx:%08lx ldt=%08lx:%08lx\r\n",
        (unsigned int)selector, source_low, source_high, gdt_low, gdt_high,
        ldt_low, ldt_high);
    WriteFile(file, line, (DWORD)strlen(line), &written, NULL);
    CloseHandle(file);
restore_error:
    SetLastError(saved_error);
}

__declspec(noreturn) void mvdm_softpc_invalid_virtual_instance(unsigned long id)
{
    EXCEPTION_RECORD record = {0};
    record.ExceptionCode = 0xc0000420; /* Existing broken-owner convention. */
    record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
    record.NumberParameters = 1;
    record.ExceptionInformation[0] = id;
    RaiseFailFastException(&record, NULL, 0);
    abort();
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
