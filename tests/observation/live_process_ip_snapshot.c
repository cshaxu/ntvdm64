/* Read-only x86 instruction-pointer snapshot for an already-running worker.
 * Intended for bounded diagnosis only: each thread is suspended solely while
 * GetThreadContext copies its control registers, then resumed immediately. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    DWORD pid;
    HANDLE snapshot;
    HANDLE process;
    THREADENTRY32 entry = { sizeof(entry) };

    if (argc != 2 || !argv[1][0]) return 64;
    pid = (DWORD)strtoul(argv[1], NULL, 10);
    if (!pid) return 64;
    process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                          FALSE, pid);
    if (process) (void)SymInitialize(process, NULL, TRUE);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 65;
    if (Thread32First(snapshot, &entry)) do {
        HANDLE thread;
        CONTEXT context = { 0 };
        if (entry.th32OwnerProcessID != pid) continue;
        thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT,
                            FALSE, entry.th32ThreadID);
        if (!thread) continue;
        if (SuspendThread(thread) != (DWORD)-1) {
            context.ContextFlags = CONTEXT_CONTROL;
            if (GetThreadContext(thread, &context)) {
                STACKFRAME64 frame = { 0 };
                CONTEXT walk = context;
                DWORD index;
                printf("pid=%lu tid=%lu eip=%08lX ebp=%08lX esp=%08lX\n",
                       (unsigned long)pid, (unsigned long)entry.th32ThreadID,
                       (unsigned long)context.Eip, (unsigned long)context.Ebp,
                       (unsigned long)context.Esp);
                if (process) {
                    frame.AddrPC.Offset = walk.Eip;
                    frame.AddrPC.Mode = AddrModeFlat;
                    frame.AddrStack.Offset = walk.Esp;
                    frame.AddrStack.Mode = AddrModeFlat;
                    frame.AddrFrame.Offset = walk.Ebp;
                    frame.AddrFrame.Mode = AddrModeFlat;
                    for (index = 0u; index < 16u && StackWalk64(
                             IMAGE_FILE_MACHINE_I386, process, thread, &frame,
                             &walk, NULL, SymFunctionTableAccess64,
                             SymGetModuleBase64, NULL); ++index) {
                        printf("tid=%lu frame=%lu pc=%08llX\n",
                               (unsigned long)entry.th32ThreadID,
                               (unsigned long)index,
                               (unsigned long long)frame.AddrPC.Offset);
                        if (frame.AddrReturn.Offset == 0u) break;
                    }
                }
            }
            (void)ResumeThread(thread);
        }
        CloseHandle(thread);
    } while (Thread32Next(snapshot, &entry));
    CloseHandle(snapshot);
    if (process) { SymCleanup(process); CloseHandle(process); }
    return 0;
}
