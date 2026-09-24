/* Observe only the process tree we launch. Debug exceptions retain their
 * original disposition; this is diagnosis, never a capability pass. */
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdint.h>

static void address_owner(FILE *log, DWORD pid, DWORD address)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    MODULEENTRY32 entry = {sizeof(entry)};
    if (snapshot == INVALID_HANDLE_VALUE) return;
    if (Module32First(snapshot, &entry)) do {
        DWORD base = (DWORD)(uintptr_t)entry.modBaseAddr;
        if (address >= base && address - base < entry.modBaseSize) {
            fprintf(log, " address=%08lx module=%s rva=%08lx\n", address,
                entry.szModule, address - base);
            break;
        }
    } while (Module32Next(snapshot, &entry));
    CloseHandle(snapshot);
}

static void exception_context(FILE *log, DEBUG_EVENT *event)
{
    HANDLE thread = OpenThread(THREAD_GET_CONTEXT, FALSE, event->dwThreadId);
    HANDLE process = OpenProcess(PROCESS_VM_READ, FALSE, event->dwProcessId);
    CONTEXT context = {0};
    DWORD words[24], frame[2], ebp;
    unsigned index;
    context.ContextFlags = CONTEXT_FULL;
    if (thread && process && GetThreadContext(thread, &context)) {
        fprintf(log, " eip=%08lx esp=%08lx ebp=%08lx eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx\n",
            context.Eip, context.Esp, context.Ebp, context.Eax, context.Ebx,
            context.Ecx, context.Edx, context.Esi, context.Edi);
        address_owner(log, event->dwProcessId, context.Eip);
        if (ReadProcessMemory(process, (void *)(uintptr_t)context.Esp, words, sizeof(words), NULL)) {
            for (index = 0; index < 24; ++index) fprintf(log, " %08lx", words[index]);
            fputc('\n', log);
        }
        ebp = context.Ebp;
        for (index = 0; index < 12 && ebp; ++index) {
            if (!ReadProcessMemory(process, (void *)(uintptr_t)ebp, frame, sizeof(frame), NULL)) break;
            address_owner(log, event->dwProcessId, frame[1]);
            if (frame[0] <= ebp || frame[0] - ebp > 0x100000u) break;
            ebp = frame[0];
        }
    }
    if (thread) CloseHandle(thread);
    if (process) CloseHandle(process);
}

int main(int argc, char **argv)
{
    char launcher[MAX_PATH], command[MAX_PATH * 3];
    STARTUPINFOA startup = {sizeof(startup)};
    PROCESS_INFORMATION parent;
    DEBUG_EVENT event;
    ULONGLONG deadline;
    FILE *log;
    unsigned processes = 0;
    if (argc != 4) return 2; /* runtime-root, guest name, log path */
    if (sprintf_s(launcher, sizeof(launcher), "%s\\run16.exe", argv[1]) < 0 ||
        sprintf_s(command, sizeof(command), "\"%s\" \"%s\"", launcher, argv[2]) < 0) return 2;
    if (fopen_s(&log, argv[3], "w") || !log) return 3;
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    if (!CreateProcessA(launcher, command, NULL, NULL, FALSE, DEBUG_PROCESS,
            NULL, argv[1], &startup, &parent)) {
        fprintf(log, "launch failed=%lu\n", GetLastError()); fclose(log); return 4;
    }
    CloseHandle(parent.hThread); CloseHandle(parent.hProcess);
    deadline = GetTickCount64() + 120000u;
    do {
        DWORD disposition = DBG_CONTINUE;
        if (!WaitForDebugEvent(&event, 100)) continue;
        if (event.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT) {
            char image[MAX_PATH]; DWORD length = sizeof(image);
            ++processes;
            if (QueryFullProcessImageNameA(event.u.CreateProcessInfo.hProcess, 0, image, &length))
                fprintf(log, "create pid=%lu image=%s\n", event.dwProcessId, image);
            if (event.u.CreateProcessInfo.hFile) CloseHandle(event.u.CreateProcessInfo.hFile);
            CloseHandle(event.u.CreateProcessInfo.hThread);
            CloseHandle(event.u.CreateProcessInfo.hProcess);
        } else if (event.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT) {
            fprintf(log, "thread-create pid=%lu tid=%lu start=%p\n",
                event.dwProcessId, event.dwThreadId, event.u.CreateThread.lpStartAddress);
            CloseHandle(event.u.CreateThread.hThread);
        } else if (event.dwDebugEventCode == EXIT_THREAD_DEBUG_EVENT) {
            fprintf(log, "thread-exit pid=%lu tid=%lu code=%08lx\n",
                event.dwProcessId, event.dwThreadId, event.u.ExitThread.dwExitCode);
        } else if (event.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT) {
            if (event.u.LoadDll.hFile) CloseHandle(event.u.LoadDll.hFile);
        } else if (event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
            fprintf(log, "exit pid=%lu code=%08lx\n", event.dwProcessId, event.u.ExitProcess.dwExitCode);
            --processes;
        } else if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
            EXCEPTION_RECORD *record = &event.u.Exception.ExceptionRecord;
            if (record->ExceptionCode != EXCEPTION_BREAKPOINT && record->ExceptionCode != 0x40000005u)
                disposition = DBG_EXCEPTION_NOT_HANDLED;
            if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION || !event.u.Exception.dwFirstChance) {
                fprintf(log, "exception pid=%lu first=%lu code=%08lx operation=%lu address=%08lx\n",
                    event.dwProcessId, event.u.Exception.dwFirstChance, record->ExceptionCode,
                    (DWORD)record->ExceptionInformation[0], (DWORD)record->ExceptionInformation[1]);
                exception_context(log, &event);
            }
        }
        fflush(log);
        ContinueDebugEvent(event.dwProcessId, event.dwThreadId, disposition);
    } while (processes && GetTickCount64() < deadline);
    fprintf(log, "observer end remaining=%u (diagnostic only)\n", processes);
    fclose(log);
    /* Default debugger-exit policy terminates any remaining launched debuggees. */
    return 0;
}
