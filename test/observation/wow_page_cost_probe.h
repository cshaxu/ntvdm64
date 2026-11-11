#include <tlhelp32.h>
#include "wow_page_cost_data.h"

static DWORD page_cost_rva;
static HANDLE page_cost_process;
static ULONG_PTR page_cost_address;

static BOOL page_cost_snapshot(const char *phase, DWORD operation)
{
    wow_page_cost_data data;
    LONG after;
    SIZE_T bytes;
    LARGE_INTEGER at;
    ULONGLONG deadline = GetTickCount64() + 1000;
    unsigned kind;
    if (!page_cost_process) {
        MODULEENTRY32 module = {sizeof(module)};
        HANDLE snapshot;
        page_cost_process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
            FALSE, owner_pid);
        if (!page_cost_process) return FALSE;
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, owner_pid);
        if (snapshot == INVALID_HANDLE_VALUE) return FALSE;
        if (Module32First(snapshot, &module)) do {
            if (!_stricmp(module.szModule, "ntvdm.exe") &&
                    module.modBaseSize >= sizeof(data) &&
                    page_cost_rva <= module.modBaseSize - sizeof(data)) {
                page_cost_address = (ULONG_PTR)module.modBaseAddr + page_cost_rva;
                break;
            }
        } while (Module32Next(snapshot, &module));
        CloseHandle(snapshot);
        if (!page_cost_address) return FALSE;
    }
    do {
        if (!ReadProcessMemory(page_cost_process, (LPCVOID)page_cost_address,
                &data, sizeof(data), &bytes) || bytes != sizeof(data)) return FALSE;
        if (!ReadProcessMemory(page_cost_process,
                (LPCVOID)(page_cost_address + FIELD_OFFSET(wow_page_cost_data, sequence)),
                &after, sizeof(after), &bytes) || bytes != sizeof(after)) return FALSE;
        if (!(data.sequence & 1) && data.sequence == after) break;
        Sleep(1); /* Observer only, outside the measured posted-input interval. */
    } while (GetTickCount64() < deadline);
    if ((data.sequence & 1) || data.sequence != after ||
            data.magic != WOW_PAGE_COST_MAGIC || data.version != 1 ||
            !data.frequency) return FALSE;
    QueryPerformanceCounter(&at);
    for (kind = 0; kind < PAGE_COST_COUNT; ++kind) {
        const wow_page_cost_row *row = &data.rows[kind];
        printf("cost phase=%s operation=%lu kind=%u calls=%llu ticks=%llu items=%llu largest=%llu longest=%llu frequency=%llu qpc=%lld sequence=%ld\n",
            phase, operation, kind, row->calls, row->ticks, row->items,
            row->largest_items, row->longest_ticks, data.frequency, at.QuadPart,
            data.sequence);
    }
    return TRUE;
}
