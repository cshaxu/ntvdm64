/* Observe original WOW debugger registration without changing guest code. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <vdmdbg.h>
typedef struct {
    DWORD ldt_base, ldt_limit, gdt_base, gdt_limit;
    WORD kernel_segment, padding;
    DWORD thhook, context, remote_address, remote_block;
    BOOL is_386;
} VDMINFO32;

typedef struct { DWORD pid; BOOL found; DWORD tid; } READY_WINDOW;
static BOOL CALLBACK find_ready_window(HWND window, LPARAM parameter)
{
    READY_WINDOW *probe = (READY_WINDOW *)parameter;
    DWORD pid = 0;
    WCHAR class_name[80];
    DWORD tid = GetWindowThreadProcessId(window, &pid);
    if (pid == probe->pid && IsWindowVisible(window) &&
        GetClassNameW(window, class_name, ARRAYSIZE(class_name)) &&
        !_wcsicmp(class_name, L"Minesweeper")) {
        probe->found = TRUE;
        probe->tid = tid;
        return FALSE;
    }
    return TRUE;
}

static BOOL read_exact(HANDLE process, ULONG_PTR address, void *output, SIZE_T size)
{
    SIZE_T read = 0;
    return ReadProcessMemory(process, (LPCVOID)address, output, size, &read) && read == size;
}

/* Diagnostic snapshot of the selected CCPU layout, not an external debugger
 * ABI: the debug event keeps the worker stopped during the page-table walk. */
static BOOL access_linear(HANDLE process, DWORD memory, DWORD length,
    const DWORD cr[4], DWORD linear, void *out, DWORD size, BOOL write)
{
    BYTE *bytes = out;
    while (size) {
        DWORD physical = linear, pde, pte, chunk = 0x1000 - (linear & 0xfff);
        if (chunk > size) chunk = size;
        if (cr[0] & 0x80000000u) {
            DWORD index = (cr[3] & 0xfffff000u) + 4 * (linear >> 22);
            if (index > length - 4 || !read_exact(process, memory + index, &pde, 4) || !(pde & 1) || (pde & 0x80)) return FALSE;
            index = (pde & 0xfffff000u) + 4 * ((linear >> 12) & 1023);
            if (index > length - 4 || !read_exact(process, memory + index, &pte, 4) || !(pte & 1)) return FALSE;
            physical = (pte & 0xfffff000u) + (linear & 0xfff);
        }
        if (physical > length || chunk > length - physical) return FALSE;
        if (write) {
            SIZE_T written = 0;
            if (!WriteProcessMemory(process, (LPVOID)(ULONG_PTR)(memory + physical), bytes, chunk, &written) || written != chunk) return FALSE;
        } else if (!read_exact(process, memory + physical, bytes, chunk)) return FALSE;
        bytes += chunk; linear += chunk; size -= chunk;
    }
    return TRUE;
}

static BOOL access_communication_block(HANDLE process, ULONG_PTR image, const DWORD rva[4], DWORD block, BOOL request, BOOL *response)
{
    DWORD memory, length, cr[4], ldt_base, descriptor_base, linear, header[4];
    BYTE descriptor[8];
    DWORD selector = HIWORD(block), offset = LOWORD(block);
    if (!(selector & 4) ||
        !read_exact(process, image + rva[0], &memory, 4) ||
        !read_exact(process, image + rva[1], &length, 4) || length < 4 ||
        !read_exact(process, image + rva[2] + 8, &ldt_base, 4) ||
        !read_exact(process, image + rva[3], cr, sizeof(cr)) ||
        !access_linear(process, memory, length, cr, ldt_base + (selector & ~7u), descriptor, 8, FALSE)) return FALSE;
    descriptor_base = descriptor[2] | ((DWORD)descriptor[3] << 8) |
        ((DWORD)descriptor[4] << 16) | ((DWORD)descriptor[7] << 24);
    linear = descriptor_base + offset;
    if (!(descriptor[5] & 0x80) || !access_linear(process, memory, length, cr, linear, header, sizeof(header), FALSE)) return FALSE;
    printf("WOW_COMM_SNAPSHOT selector=%04lx linear=%08lx block=%08lx bytes=%u\n",
        selector, linear, header[0], (unsigned)(header[3] & 0xffffu));
    if (header[0] != block || (header[3] & 0xffffu) != 4096) return FALSE;
    if (response) *response = (header[3] >> 16) == 1 && header[1] != 0xfefefefeu;
    if (request) {
        static const char names[] = "KERNEL\0GetVersion";
        BYTE written[sizeof(header) + sizeof(names)];
        header[2] = 0; /* original VDMCallRemote16: zero argument words and bytes */
        if (!access_linear(process, memory, length, cr, linear, header, sizeof(header), TRUE) ||
            !access_linear(process, memory, length, cr, linear + sizeof(header), (void *)names, sizeof(names), TRUE) ||
            !access_linear(process, memory, length, cr, linear, written, sizeof(written), FALSE) ||
            memcmp(written, header, sizeof(header)) ||
            memcmp(written + sizeof(header), names, sizeof(names))) return FALSE;
        printf("WOW_COMM_REQUEST KERNEL!GetVersion argc=0\n");
    }
    return TRUE;
}

int wmain(int argc, wchar_t **argv)
{
    wchar_t launcher[MAX_PATH], command[3 * MAX_PATH];
    STARTUPINFOW startup = {0};
    PROCESS_INFORMATION child = {0};
    DEBUG_EVENT event;
    DWORD worker = 0, events = 0, info_reads = 0, remote_address = 0, remote_block = 0, remote_id = 0;
    HANDLE remote = NULL, paused_owner = NULL;
    ULONG_PTR worker_base = 0, registration_rva, termination_rva = 0;
    DWORD snapshot_rva[4];
    ULONGLONG deadline;
    BOOL found = FALSE, completed = FALSE, request = FALSE, response = FALSE, invoke, failed = FALSE, pause_owner, ready;
    BOOL task_started = FALSE, break_requested = FALSE, ready_paused;
    DWORD saved_address = 0, saved_block = 0;
    int index;
    if (argc < 8 || argc > 11) return 2;
    invoke = argc >= 9 && !_wcsicmp(argv[8], L"invoke");
    pause_owner = argc == 10 && !_wcsicmp(argv[9], L"pause-owner");
    ready = argc >= 10 && !_wcsicmp(argv[9], L"ready");
    ready_paused = argc >= 10 && !_wcsicmp(argv[9], L"ready-paused");
    if (ready_paused) ready = TRUE;
    if ((argc >= 9 && !invoke) || (argc >= 10 && !pause_owner && !ready)) return 2;
    if (argc == 11) termination_rva = (ULONG_PTR)wcstoul(argv[10], NULL, 16);
    for (index = 0; index != 4; ++index) snapshot_rva[index] = wcstoul(argv[4 + index], NULL, 16);
    registration_rva = (ULONG_PTR)wcstoul(argv[3], NULL, 16);
    if (!registration_rva) return 2;
    if (!swprintf(launcher, MAX_PATH, L"%ls\\system32\\run16.exe", argv[1]) ||
        !swprintf(command, 3 * MAX_PATH, L"\"%ls\" \"%ls\"", launcher, argv[2])) return 3;
    startup.cb = sizeof(startup);
    if (!CreateProcessW(launcher, command, NULL, NULL, FALSE, DEBUG_PROCESS,
            NULL, argv[1], &startup, &child)) {
        fprintf(stderr, "CreateProcess error=%lu\n", GetLastError());
        return 4;
    }
    CloseHandle(child.hThread);
    CloseHandle(child.hProcess);
    deadline = GetTickCount64() + 30000;
    while (GetTickCount64() < deadline && (!found || (invoke && !completed))) {
        DWORD disposition = DBG_CONTINUE;
        if (!WaitForDebugEvent(&event, 250)) {
            if (ready && task_started && !break_requested) {
                READY_WINDOW probe = { worker, FALSE, 0 };
                EnumWindows(find_ready_window, (LPARAM)&probe);
                if (probe.found) {
                    HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
                        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, worker);
                    if (ready_paused) {
                        paused_owner = OpenThread(THREAD_SUSPEND_RESUME, FALSE, probe.tid);
                        if (!paused_owner || SuspendThread(paused_owner) == (DWORD)-1) failed = TRUE;
                        else printf("WOW_OWNER_SUSPENDED tid=%lu\n", probe.tid);
                    }
                    if (failed || !process || !DebugBreakProcess(process)) failed = TRUE;
                    else { break_requested = TRUE; printf("WOW_APPLICATION_READY\n"); }
                    if (process) CloseHandle(process);
                    if (failed) break;
                }
            }
            continue;
        }
        if (event.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT) {
            WCHAR path[MAX_PATH], *name;
            DWORD count = MAX_PATH;
            if (QueryFullProcessImageNameW(event.u.CreateProcessInfo.hProcess, 0, path, &count) &&
                (name = wcsrchr(path, L'\\')) && !_wcsicmp(name + 1, L"ntvdm.exe"))
                { worker = event.dwProcessId; worker_base = (ULONG_PTR)event.u.CreateProcessInfo.lpBaseOfImage;
                  printf("WOW_WORKER pid=%lu base=%p\n", worker, (void *)worker_base); }
            if (event.u.CreateProcessInfo.hFile) CloseHandle(event.u.CreateProcessInfo.hFile);
        } else if (event.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT &&
                   event.dwProcessId == worker && event.dwThreadId == remote_id && termination_rva) {
            WOW64_CONTEXT context = {0};
            context.ContextFlags = WOW64_CONTEXT_DEBUG_REGISTERS;
            if (!Wow64GetThreadContext(event.u.CreateThread.hThread, &context)) failed = TRUE;
            else {
                context.Dr0 = (DWORD)(worker_base + termination_rva);
                context.Dr7 = (context.Dr7 & ~0x000f0003u) | 1;
                if (!Wow64SetThreadContext(event.u.CreateThread.hThread, &context)) failed = TRUE;
                else printf("REMOTE_TERMINATION_WATCH tid=%lu address=%08lx\n", remote_id, context.Dr0);
            }
            CloseHandle(event.u.CreateThread.hThread);
        } else if (event.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT) {
            WCHAR path[MAX_PATH];
            if (event.u.LoadDll.hFile && GetFinalPathNameByHandleW(event.u.LoadDll.hFile, path, MAX_PATH, 0) &&
                wcsstr(path, L"WOW32.DLL"))
                printf("WOW32_IMAGE base=%p\n", event.u.LoadDll.lpBaseOfDll);
            if (event.u.LoadDll.hFile) CloseHandle(event.u.LoadDll.hFile);
        } else if (event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
            printf("DEBUG_PROCESS_EXIT pid=%lu code=%08lx worker=%d\n",
                event.dwProcessId, event.u.ExitProcess.dwExitCode, event.dwProcessId == worker);
            if (event.dwProcessId == worker) failed = TRUE;
        } else if (event.dwDebugEventCode == EXIT_THREAD_DEBUG_EVENT &&
                   event.dwProcessId == worker && event.dwThreadId == remote_id && remote_id) {
            HANDLE process = OpenProcess(PROCESS_VM_READ, FALSE, worker);
            if (process) {
                completed = event.u.ExitThread.dwExitCode == 0 &&
                    access_communication_block(process, worker_base, snapshot_rva, remote_block, FALSE, &response) && response;
                CloseHandle(process);
            }
            printf("WOW_REMOTE_THREAD_EXIT code=%08lx response=%u\n", event.u.ExitThread.dwExitCode, response);
        } else if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
            EXCEPTION_RECORD *record = &event.u.Exception.ExceptionRecord;
            if (event.dwProcessId == worker && event.dwThreadId == remote_id && termination_rva &&
                (record->ExceptionCode == EXCEPTION_SINGLE_STEP || record->ExceptionCode == 0x4000001eu)) {
                WOW64_CONTEXT context = {0};
                HANDLE thread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, remote_id);
                context.ContextFlags = WOW64_CONTEXT_CONTROL | WOW64_CONTEXT_INTEGER | WOW64_CONTEXT_DEBUG_REGISTERS;
                if (thread && Wow64GetThreadContext(thread, &context)) {
                    printf("REMOTE_TERMINATION_TRAP eip=%08lx ebx=%08lx bop=%02lx\n",
                        context.Eip, context.Ebx, context.Ebx & 0xffu);
                    context.Dr7 &= ~1u;
                    Wow64SetThreadContext(thread, &context);
                }
                if (thread) CloseHandle(thread);
            }
            if (ready && break_requested && !found && event.dwProcessId == worker &&
                (record->ExceptionCode == EXCEPTION_BREAKPOINT || record->ExceptionCode == 0x4000001fu)) {
                HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, worker);
                if (process) {
                    found = access_communication_block(process, worker_base, snapshot_rva, saved_block, TRUE, NULL);
                    CloseHandle(process);
                }
                if (!found) failed = TRUE;
                else { remote_address = saved_address; remote_block = saved_block; request = TRUE; }
            }
            if (event.dwProcessId == worker && record->ExceptionCode == STATUS_VDM_EVENT &&
                record->NumberParameters == 4 && record->ExceptionInformation[3]) {
                VDMINFO32 info;
                SIZE_T read = 0;
                HANDLE process = OpenProcess(PROCESS_VM_READ | (invoke ? PROCESS_VM_WRITE | PROCESS_VM_OPERATION : 0), FALSE, worker);
                ++events;
                if (remote && record->ExceptionInformation[0] == DBG_GPFAULT) failed = TRUE;
                if (process && ReadProcessMemory(process, (LPCVOID)record->ExceptionInformation[3],
                        &info, sizeof(info), &read) && read == sizeof(info)) {
                    SEGMENT_NOTE note;
                    SIZE_T note_read = 0;
                    ++info_reads;
                    printf("WOW_VDM_EVENT kind=%lu remote=%08lx block=%08lx\n",
                        (DWORD)record->ExceptionInformation[0], info.remote_address, info.remote_block);
                    if (record->ExceptionInformation[0] == DBG_GPFAULT && info.context) {
                        VDMCONTEXT context;
                        if (read_exact(process, info.context, &context, sizeof(context)))
                            printf("WOW_REMOTE_GP cs=%04lx ip=%08lx ss=%04lx sp=%08lx flags=%08lx\n",
                                context.SegCs, context.Eip, context.SegSs, context.Esp, context.EFlags);
                    }
                    if ((record->ExceptionInformation[0] == DBG_SEGLOAD ||
                         record->ExceptionInformation[0] == DBG_MODLOAD ||
                         record->ExceptionInformation[0] == DBG_MODFREE) &&
                        record->ExceptionInformation[2] &&
                        ReadProcessMemory(process, (LPCVOID)record->ExceptionInformation[2],
                            &note, sizeof(note), &note_read) && note_read == sizeof(note)) {
                        note.Module[sizeof(note.Module)-1] = 0;
                        note.FileName[sizeof(note.FileName)-1] = 0;
                        printf("WOW_VDM_MODULE %s %s\n", note.Module, note.FileName);
                    }
                    if (!found && info.remote_address && info.remote_block &&
                        (!invoke || record->ExceptionInformation[0] == DBG_TASKSTART)) {
                        if (ready) {
                            saved_address = info.remote_address;
                            saved_block = info.remote_block;
                            task_started = TRUE;
                        } else {
                        WOW64_LDT_ENTRY descriptor;
                        HANDLE thread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT,
                            FALSE, event.dwThreadId);
                        printf("WOW_REMOTE_REGISTERED worker=%lu address=%08lx block=%08lx\n",
                            worker, info.remote_address, info.remote_block);
                        if (thread) {
                            SetLastError(ERROR_SUCCESS);
                            if (Wow64GetThreadSelectorEntry(thread, HIWORD(info.remote_block), &descriptor)) {
                                DWORD base = descriptor.BaseLow | ((DWORD)descriptor.HighWord.Bytes.BaseMid << 16) |
                                    ((DWORD)descriptor.HighWord.Bytes.BaseHi << 24);
                                printf("HOST_SELECTOR selector=%04x base=%08lx\n", HIWORD(info.remote_block), base);
                            } else {
                                printf("HOST_SELECTOR selector=%04x error=%lu\n", HIWORD(info.remote_block), GetLastError());
                            }
                            CloseHandle(thread);
                        }
                        found = access_communication_block(process, worker_base, snapshot_rva, info.remote_block, invoke, NULL);
                        if (found) { remote_address = info.remote_address; remote_block = info.remote_block; request = invoke; }
                        if (found && pause_owner) {
                            paused_owner = OpenThread(THREAD_SUSPEND_RESUME, FALSE, event.dwThreadId);
                            if (!paused_owner || SuspendThread(paused_owner) == (DWORD)-1) failed = TRUE;
                            else printf("WOW_OWNER_SUSPENDED tid=%lu\n", event.dwThreadId);
                        }
                        }
                    }
                }
                if (process) CloseHandle(process);
            } else if (record->ExceptionCode != STATUS_VDM_EVENT &&
                record->ExceptionCode != EXCEPTION_BREAKPOINT &&
                record->ExceptionCode != EXCEPTION_SINGLE_STEP &&
                record->ExceptionCode != 0x4000001eu && /* STATUS_WX86_SINGLE_STEP */
                record->ExceptionCode != 0x4000001fu) { /* STATUS_WX86_BREAKPOINT */
                fprintf(stderr, "DEBUG_EXCEPTION pid=%lu code=%08lx first=%lu\n",
                    event.dwProcessId, record->ExceptionCode, event.u.Exception.dwFirstChance);
                if (event.dwProcessId == worker &&
                    (record->ExceptionCode == 0xc0000029u || record->ExceptionCode == 0xc0000005u)) {
                    WOW64_CONTEXT context = {0};
                    HANDLE thread = OpenThread(THREAD_GET_CONTEXT, FALSE, event.dwThreadId);
                    context.ContextFlags = WOW64_CONTEXT_CONTROL;
                    if (thread && Wow64GetThreadContext(thread, &context))
                        fprintf(stderr, "WOW_UNWIND_THREAD tid=%lu eip=%08lx esp=%08lx workerbase=%p\n",
                            event.dwThreadId, context.Eip, context.Esp, (void *)worker_base);
                    if (thread) CloseHandle(thread);
                }
                disposition = DBG_EXCEPTION_NOT_HANDLED;
            }
        }
        if (!ContinueDebugEvent(event.dwProcessId, event.dwThreadId, disposition)) break;
        if (failed) break;
        if (request && !remote) {
            HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
                PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, worker);
            if (process) {
                remote = CreateRemoteThread(process, NULL, 0,
                    (LPTHREAD_START_ROUTINE)(ULONG_PTR)remote_address, NULL, 0, &remote_id);
                CloseHandle(process);
            }
            if (!remote) { fprintf(stderr, "REMOTE_CREATE_ERROR error=%lu\n", GetLastError()); break; }
        }
    }
    if (!found) fprintf(stderr, "WOW_REMOTE_NOT_OBSERVED worker=%lu events=%lu info_reads=%lu\n", worker, events, info_reads);
    if (invoke && !completed) fprintf(stderr, "WOW_REMOTE_NOT_COMPLETED thread=%lu response=%u\n", remote_id, response);
    if (paused_owner) { if (!failed) ResumeThread(paused_owner); CloseHandle(paused_owner); }
    if (remote) CloseHandle(remote);
    return found && (!invoke || completed) ? 0 : 1;
}
