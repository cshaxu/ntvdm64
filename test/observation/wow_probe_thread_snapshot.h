#ifndef WOW_PROBE_THREAD_SNAPSHOT_H
#define WOW_PROBE_THREAD_SNAPSHOT_H
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <dbghelp.h>
#include <stdio.h>
/* PID-bound failure-only diagnostic shared by the integration probes. */
static void report_fault_module(DWORD pid, const void *address)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    HMODULE modules[256];
    DWORD bytes = 0, index;
    if (!process || !EnumProcessModules(process, modules, sizeof(modules), &bytes)) {
        if (process) CloseHandle(process);
        return;
    }
    for (index = 0; index < bytes / sizeof(modules[0]) && index < ARRAYSIZE(modules); ++index) {
        MODULEINFO info;
        WCHAR name[MAX_PATH] = L"<unknown>";
        if (!GetModuleInformation(process, modules[index], &info, sizeof(info))) continue;
        if ((const unsigned char *)address >= (unsigned char *)info.lpBaseOfDll &&
            (const unsigned char *)address < (unsigned char *)info.lpBaseOfDll + info.SizeOfImage) {
            GetModuleBaseNameW(process, modules[index], name, ARRAYSIZE(name));
            printf(WOW_PROBE_PREFIX "fault-module=%ls base=%p size=%lu rva=%08lx\n",
                name, info.lpBaseOfDll, info.SizeOfImage,
                (DWORD)((const unsigned char *)address - (unsigned char *)info.lpBaseOfDll));
            break;
        }
    }
    CloseHandle(process);
}

static void report_unknown_code(DWORD pid, const void *address)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    MEMORY_BASIC_INFORMATION memory;
    BYTE bytes[16] = {0};
    SIZE_T read = 0;
    unsigned index;
    if (!process || !VirtualQueryEx(process, address, &memory, sizeof(memory))) goto done;
    printf(WOW_PROBE_PREFIX "code-region base=%p size=%llu state=%lx protect=%lx type=%lx bytes@pc=",
        memory.BaseAddress, (unsigned long long)memory.RegionSize, memory.State,
        memory.Protect, memory.Type);
    if (ReadProcessMemory(process, address, bytes, sizeof(bytes), &read)) {
        for (index = 0; index < read; ++index) printf("%02x", bytes[index]);
    }
    puts("");
done:
    if (process) CloseHandle(process);
}

static void report_fault_stack(DWORD pid, DWORD tid)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    HANDLE thread = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
                               FALSE, tid);
    CONTEXT context = { .ContextFlags = CONTEXT_FULL };
    STACKFRAME64 frame = {0};
    unsigned index;
    if (!process || !thread || !GetThreadContext(thread, &context)) goto done;
    frame.AddrPC.Offset = context.Eip; frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.Ebp; frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.Esp; frame.AddrStack.Mode = AddrModeFlat;
    SymInitialize(process, NULL, TRUE);
    for (index = 0; index != 8 && StackWalk64(IMAGE_FILE_MACHINE_I386, process,
             thread, &frame, &context, NULL, SymFunctionTableAccess64,
             SymGetModuleBase64, NULL); ++index) {
        printf(WOW_PROBE_PREFIX "stack[%u]=%08llx\n", index,
               (unsigned long long)frame.AddrPC.Offset);
        report_fault_module(pid, (const void *)(ULONG_PTR)frame.AddrPC.Offset);
        if (index == 1) report_unknown_code(pid, (const void *)(ULONG_PTR)frame.AddrPC.Offset);
    }
    SymCleanup(process);
done:
    if (thread) CloseHandle(thread);
    if (process) CloseHandle(process);
}

static void report_stalled_worker(DWORD pid)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 entry = {sizeof(entry)};
    if (snapshot == INVALID_HANDLE_VALUE) return;
    if (Thread32First(snapshot, &entry)) do {
        HANDLE thread;
        CONTEXT context = { .ContextFlags = CONTEXT_FULL };
        if (entry.th32OwnerProcessID != pid) continue;
        thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT,
                            FALSE, entry.th32ThreadID);
        if (!thread) continue;
        if (SuspendThread(thread) != (DWORD)-1) {
            __try {
                if (GetThreadContext(thread, &context)) {
                    printf(WOW_PROBE_PREFIX "stalled-thread=%lu pc=%08lx\n",
                           entry.th32ThreadID, context.Eip);
                    printf(WOW_PROBE_PREFIX "registers tid=%lu eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx ebp=%08lx esp=%08lx\n",
                           entry.th32ThreadID, context.Eax, context.Ebx,
                           context.Ecx, context.Edx, context.Esi, context.Edi,
                           context.Ebp, context.Esp);
                    report_fault_module(pid, (const void *)(ULONG_PTR)context.Eip);
                    report_fault_stack(pid, entry.th32ThreadID);
                }
            } __finally {
                ResumeThread(thread);
            }
        }
        CloseHandle(thread);
    } while (Thread32Next(snapshot, &entry));
    CloseHandle(snapshot);
}

#endif
