/* Source-led performance witness for the actual English WINMINE board.
 * Reuse the existing pixel/identity helpers; no guest/runtime hook or patch.
 * Wall time includes posted input, its handling, drawing and pixel polling.
 * Statistical sampling is optional and must be reported separately. */
#ifdef T447_OBSERVER_COST
#include "wow_observer_cost.h"
#define BitBlt observer_cost_BitBlt
#define GdiFlush observer_cost_GdiFlush
#define Sleep observer_cost_Sleep
#endif
#define main wow_mine_gameplay_not_run
#include "wow_mine_gameplay_probe.c"
#undef main
#ifdef T447_PAGE_COST_PROFILE
#include "wow_page_cost_probe.h"
#endif
#ifdef T447_CALLBACK_COST
#include "wow_callback_cost_probe.h"
#ifdef T447_PAGE_COST_PROFILE
#error Separate callback and page instrumentation
#endif
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
#ifdef T447_CALLBACK_COST
    DWORD callback_rva;
    if(argc!=6 || !(callback_rva=strtoul(argv[5],NULL,16)))return 64;
    argc=5;
#endif
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
#ifdef T447_CALLBACK_COST
    fprintf(file,"callback-thread=%lu\n",GetWindowThreadProcessId(mine,NULL));
#endif
    fclose(file);
    if (WaitForSingleObject(gate, 20000) != WAIT_OBJECT_0) return 68;
    CloseHandle(gate);
    previous = before;
    for (operation = 0; operation < 12; ++operation) {
        LARGE_INTEGER start, stop, frequency;
#ifdef T447_OBSERVER_COST
        LARGE_INTEGER posted;
        observer_cost_sample cost;
        ZeroMemory(&observer_cost,sizeof(observer_cost));
#endif
        QueryPerformanceFrequency(&frequency);
#ifdef T447_PAGE_COST_PROFILE
        if (!page_cost_snapshot("before", operation)) return 74;
#endif
        QueryPerformanceCounter(&start);
        if (!click(WM_RBUTTONDOWN, WM_RBUTTONUP, MK_RBUTTON)) return 69;
#ifdef T447_OBSERVER_COST
        QueryPerformanceCounter(&posted);
#endif
        if (!wait_cell(process, previous, FALSE, "toggle")) return 69;
        QueryPerformanceCounter(&stop);
#ifdef T447_OBSERVER_COST
        cost=observer_cost;
#endif
#ifdef T447_PAGE_COST_PROFILE
        if (!page_cost_snapshot("after", operation)) return 75;
#endif
        current = region_hash(mine, 12, 54, 14, 14);
        if (!current || current == previous) return 70;
        printf("latency operation=%lu microseconds=%llu old=%08lx new=%08lx start-qpc=%lld stop-qpc=%lld frequency=%lld\n",
            operation, (unsigned long long)((stop.QuadPart - start.QuadPart) *
            1000000 / frequency.QuadPart), previous, current,
            start.QuadPart, stop.QuadPart, frequency.QuadPart);
#ifdef T447_OBSERVER_COST
        printf("observer-cost operation=%lu total-qpc=%lld post-qpc=%lld blit-qpc=%lld flush-qpc=%lld sleep-qpc=%lld blit-calls=%lu flush-calls=%lu sleep-calls=%lu frequency=%lld\n",
            operation,stop.QuadPart-start.QuadPart,posted.QuadPart-start.QuadPart,
            cost.blit_ticks,cost.flush_ticks,cost.sleep_ticks,cost.blit_calls,
            cost.flush_calls,cost.sleep_calls,frequency.QuadPart);
#endif
        fflush(stdout);
        previous = current;
    }
    if (previous != before || !capture(mine, argv[3], "final")) return 71;
#ifdef T447_CALLBACK_WORKLOAD
    /* Separate opt-in reveal workload; F2/reset is outside each stopwatch.
     * The default twelve-flag acceptance above remains unchanged. */
    for(operation=0;operation<3;++operation){
        LARGE_INTEGER start,stop,frequency;
        QueryPerformanceFrequency(&frequency);QueryPerformanceCounter(&start);
        if(!click(WM_LBUTTONDOWN,WM_LBUTTONUP,MK_LBUTTON) ||
                !wait_cell(process,before,FALSE,"reveal"))return 77;
        QueryPerformanceCounter(&stop);
        printf("callback-open operation=%lu microseconds=%llu start-qpc=%lld stop-qpc=%lld frequency=%lld\n",
            operation,(unsigned long long)((stop.QuadPart-start.QuadPart)*
                1000000/frequency.QuadPart),start.QuadPart,stop.QuadPart,frequency.QuadPart);
        fflush(stdout);
        if(!PostMessageW(mine,WM_KEYDOWN,VK_F2,1) ||
                !PostMessageW(mine,WM_KEYUP,VK_F2,0xc0000001) ||
                !wait_cell(process,before,TRUE,"reveal-reset"))return 78;
    }
    if(!capture(mine,argv[3],"reveals-final"))return 79;
#endif
#ifdef T447_CALLBACK_COST
    if(!callback_cost_dump(owner_pid,mine,callback_rva))return 76;
#endif
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
