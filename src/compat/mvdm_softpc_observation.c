#include "include/mvdm_softpc_observation.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
