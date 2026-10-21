/* Read-only native-architecture snapshot for an already-running process.
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
    {
        HANDLE modules = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        MODULEENTRY32 module = { sizeof(module) };
        if (modules != INVALID_HANDLE_VALUE) {
            if (Module32First(modules, &module)) do {
                printf("module base=%p bytes=%lu path=%s\n", module.modBaseAddr,
                    (unsigned long)module.modBaseSize, module.szExePath);
            } while (Module32Next(modules, &module));
            CloseHandle(modules);
        }
    }
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
#ifdef _WIN64
                DWORD machine=IMAGE_FILE_MACHINE_AMD64;
                DWORD64 ip=context.Rip,bp=context.Rbp,sp=context.Rsp;
#else
                DWORD machine=IMAGE_FILE_MACHINE_I386;
                DWORD64 ip=context.Eip,bp=context.Ebp,sp=context.Esp;
#endif
                printf("pid=%lu tid=%lu ip=%llX bp=%llX sp=%llX\n",
                       (unsigned long)pid, (unsigned long)entry.th32ThreadID,
                       (unsigned long long)ip, (unsigned long long)bp,
                       (unsigned long long)sp);
                if (process) {
                    frame.AddrPC.Offset = ip;
                    frame.AddrPC.Mode = AddrModeFlat;
                    frame.AddrStack.Offset = sp;
                    frame.AddrStack.Mode = AddrModeFlat;
                    frame.AddrFrame.Offset = bp;
                    frame.AddrFrame.Mode = AddrModeFlat;
                    for (index = 0u; index < 16u && StackWalk64(
                             machine, process, thread, &frame,
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
