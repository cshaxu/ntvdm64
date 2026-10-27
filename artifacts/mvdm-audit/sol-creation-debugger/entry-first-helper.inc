/* Diagnostic only: observe the public SetLastError entry on the owned
 * Solitaire window thread, using a hardware execution breakpoint. No DLL,
 * host code bytes, guest bytes, return values or API arguments are replaced. */
typedef struct error_watch {
    DWORD thread_id, address, visits, saved_dr0, saved_dr6, saved_dr7;
    HANDLE thread;
    BOOL armed;
} error_watch;

static DWORD remote_error_entry(DWORD pid)
{
    FARPROC local = GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetLastError");
    MEMORY_BASIC_INFORMATION local_memory;
    HMODULE modules[256];
    WCHAR local_name[MAX_PATH], name[MAX_PATH];
    DWORD bytes = 0, index, result = 0;
    HANDLE process;
    BYTE code[16]; SIZE_T read;
    ULONG_PTR offset;
    if (!local || !VirtualQuery((LPCVOID)local, &local_memory, sizeof(local_memory))) return 0;
    if (!GetModuleBaseNameW(GetCurrentProcess(), (HMODULE)local_memory.AllocationBase,
            local_name, ARRAYSIZE(local_name))) return 0;
    offset = (ULONG_PTR)local - (ULONG_PTR)local_memory.AllocationBase;
    process = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pid);
    if (!process) return 0;
    if (EnumProcessModules(process, modules, sizeof(modules), &bytes) && bytes <= sizeof(modules))
        for (index = 0; index != bytes/sizeof(modules[0]); ++index) {
            if (GetModuleBaseNameW(process, modules[index], name, ARRAYSIZE(name)) &&
                    !_wcsicmp(name, local_name)) {
                ULONG_PTR address = (ULONG_PTR)modules[index] + offset;
                if (ReadProcessMemory(process, (LPCVOID)address, code, sizeof(code), &read) &&
                        read == sizeof(code) && !memcmp(code, (LPCVOID)local, sizeof(code))) {
                    result = (DWORD)address;
                    printf("SOL error-watch: verified public entry module=%ls rva=%08lx address=%08lx\n",
                        name, (DWORD)offset, result);
                }
                break;
            }
        }
    CloseHandle(process);
    return result;
}

static BOOL arm_error_watch(error_watch *watch, DWORD thread_id)
{
    CONTEXT context = { .ContextFlags = CONTEXT_DEBUG_REGISTERS };
    if (thread_id != watch->thread_id) return TRUE;
    watch->thread = OpenThread(THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_QUERY_INFORMATION|THREAD_SUSPEND_RESUME,
        FALSE, thread_id);
    if (!watch->thread || !GetThreadContext(watch->thread, &context)) return FALSE;
    if (context.Dr7 & 0xffu) { puts("SOL error-watch: existing debug-register owner; not replaced"); return FALSE; }
    watch->saved_dr0=context.Dr0; watch->saved_dr6=context.Dr6; watch->saved_dr7=context.Dr7;
    context.Dr0 = watch->address; context.Dr6 = 0; context.Dr7 = 1;
    watch->armed = SetThreadContext(watch->thread, &context);
    printf("SOL error-watch: armed=%d tid=%lu\n", watch->armed, thread_id);
    return watch->armed;
}

static void clear_error_watch(error_watch *watch)
{
    CONTEXT context = { .ContextFlags = CONTEXT_DEBUG_REGISTERS };
    if (watch->armed) {
        DWORD suspended=SuspendThread(watch->thread);
        if(suspended!=(DWORD)-1) {
            if(GetThreadContext(watch->thread, &context)) {
                context.Dr0=watch->saved_dr0; context.Dr6=watch->saved_dr6; context.Dr7=watch->saved_dr7;
                if (!SetThreadContext(watch->thread, &context))
                    printf("SOL error-watch: clear failed=%lu\n", GetLastError());
            } else printf("SOL error-watch: clear-context failed=%lu\n", GetLastError());
            (void)ResumeThread(watch->thread);
        } else printf("SOL error-watch: clear-suspend failed=%lu\n", GetLastError());
    }
    watch->armed = FALSE;
    if (watch->thread) { CloseHandle(watch->thread); watch->thread = NULL; }
}

static int observe_error_watch(error_watch *watch, DWORD pid, DWORD tid)
{
    CONTEXT context = { .ContextFlags = CONTEXT_DEBUG_REGISTERS|CONTEXT_CONTROL|CONTEXT_INTEGER };
    DWORD arguments[2] = {0}; SIZE_T read = 0;
    HANDLE process;
    BOOL captured = FALSE;
    if (!watch->armed || tid != watch->thread_id || !GetThreadContext(watch->thread, &context) ||
            !(context.Dr6 & 1u) || context.Eip != watch->address) return -1;
    ++watch->visits;
    process = OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (process) {
        if (ReadProcessMemory(process, (LPCVOID)(ULONG_PTR)context.Esp,
                arguments, sizeof(arguments), &read) && read == sizeof(arguments) &&
                arguments[1] == ERROR_INVALID_STATE) {
            captured = TRUE;
            printf("SOL error-watch: CAPTURED_5023 tid=%lu public-caller=%08lx visits=%lu\n",
                tid, arguments[0], watch->visits);
            report_fault_module(pid, (LPCVOID)(ULONG_PTR)arguments[0]);
            report_fault_stack(pid, tid);
            report_stack_words(pid, context.Esp);
        }
        CloseHandle(process);
    }
    context.Dr6 = 0;
    context.EFlags |= 0x10000u; /* Resume flag: execute the unmodified instruction once. */
    if (!SetThreadContext(watch->thread, &context))
        printf("SOL error-watch: resume-context failed=%lu\n", GetLastError());
    return captured;
}
