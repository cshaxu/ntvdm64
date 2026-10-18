/* Source-led performance witness for the actual English WINMINE board.
 * Reuse the existing pixel/identity helpers; no guest/runtime hook or patch.
 * Wall time includes posted input, its handling, drawing and pixel polling.
 * Statistical sampling is optional and must be reported separately. */
#define main wow_mine_gameplay_not_run
#include "wow_mine_gameplay_probe.c"
#undef main
#ifdef T447_PAGE_COST_PROFILE
#include "wow_page_cost_probe.h"
#endif

int main(int argc, char **argv)
{
    HANDLE process, gate;
    HDESK desk;
    DWORD before, counter, previous, current, operation;
    ULONGLONG deadline;
    char ready[MAX_PATH];
    FILE *file;
    BOOL inherited;
#ifdef T447_PAGE_COST_PROFILE
    if (argc != 6 || !(page_cost_rva = strtoul(argv[5], NULL, 16))) return 64;
    argc = 5;
#endif
    if (argc != 5 || !(owner_pid = strtoul(argv[1], NULL, 10))) return 64;
    inherited = !strcmp(argv[2], "-");
    process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE, owner_pid);
    desk = inherited ? GetThreadDesktop(GetCurrentThreadId()) :
        OpenDesktopA(argv[2], 0, FALSE, DESKTOP_READOBJECTS |
            DESKTOP_WRITEOBJECTS | DESKTOP_ENUMERATE);
    if (!process || !desk || (!inherited && !SetThreadDesktop(desk))) return 65;
    deadline = GetTickCount64() + 15000;
    do {
        EnumDesktopWindows(desk, find_mine, 0);
        if (mine) break;
        Sleep(25);
    } while (GetTickCount64() < deadline &&
        WaitForSingleObject(process, 0) == WAIT_TIMEOUT);
    if (!mine) return 66;
    /* A patterned startup surface can still be an incomplete/occluded
     * drawing. Finish painting only this owned window before arming inputs;
     * do not count its completion as the first click's pixel change. */
    if (!SetWindowPos(mine, HWND_TOP, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE) ||
        !RedrawWindow(mine, NULL, NULL,
            RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN) ||
        !wait_board(process, &before, &counter)) return 66;
    gate = OpenEventA(SYNCHRONIZE, FALSE, argv[4]);
    if (!gate || sprintf_s(ready, sizeof(ready), "%s.ready", argv[3]) < 0 ||
        fopen_s(&file, ready, "w")) return 67;
    fprintf(file, "worker=%lu window=%p\n", owner_pid, mine);
    fclose(file);
    if (WaitForSingleObject(gate, 20000) != WAIT_OBJECT_0) return 68;
    CloseHandle(gate);
    previous = before;
    for (operation = 0; operation < 12; ++operation) {
        LARGE_INTEGER start, stop, frequency;
        QueryPerformanceFrequency(&frequency);
#ifdef T447_PAGE_COST_PROFILE
        if (!page_cost_snapshot("before", operation)) return 74;
#endif
        QueryPerformanceCounter(&start);
        if (!click(WM_RBUTTONDOWN, WM_RBUTTONUP, MK_RBUTTON) ||
            !wait_cell(process, previous, FALSE, "toggle")) return 69;
        QueryPerformanceCounter(&stop);
#ifdef T447_PAGE_COST_PROFILE
        if (!page_cost_snapshot("after", operation)) return 75;
#endif
        current = region_hash(mine, 12, 54, 14, 14);
        if (!current || current == previous) return 70;
        printf("latency operation=%lu microseconds=%llu old=%08lx new=%08lx start-qpc=%lld stop-qpc=%lld frequency=%lld\n",
            operation, (unsigned long long)((stop.QuadPart - start.QuadPart) *
            1000000 / frequency.QuadPart), previous, current,
            start.QuadPart, stop.QuadPart, frequency.QuadPart);
        fflush(stdout);
        previous = current;
    }
    if (previous != before || !capture(mine, argv[3], "final")) return 71;
    if (!PostMessageW(mine, WM_CLOSE, 0, 0)) return 72;
    deadline = GetTickCount64() + 10000;
    while (IsWindow(mine) && GetTickCount64() < deadline) Sleep(20);
    if (IsWindow(mine)) return 73;
    puts("PASS twelve actual board changes, initial board restored, normal window close");
    CloseHandle(process);
#ifdef T447_PAGE_COST_PROFILE
    CloseHandle(page_cost_process);
#endif
    if (!inherited) CloseDesktop(desk);
    return 0;
}
