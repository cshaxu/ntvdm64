/*
 * Default-disabled trace of the original host_main -> host_applInit prefix.
 * The only local control is a fail-closed stop at an instrumented original
 * host_applInit checkpoint; no lifecycle, console, or DOS behavior is added.
 */
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#ifndef NTDOS64_HOST_APPLINIT_TRACE_STOP_STAGE
#define NTDOS64_HOST_APPLINIT_TRACE_STOP_STAGE 17
#endif

extern int host_main(int argc, char **argv);

static jmp_buf host_main_applinit_stop_environment;
static int host_main_applinit_stop_reason;
static int host_main_applinit_stop_stage = NTDOS64_HOST_APPLINIT_TRACE_STOP_STAGE;

static LONG WINAPI host_main_applinit_trace_exception_filter(
    EXCEPTION_POINTERS *exception_pointers) {
    DWORD module_base = (DWORD)(ULONG_PTR)GetModuleHandleA(NULL);
    DWORD instruction = (DWORD)(ULONG_PTR)
        exception_pointers->ExceptionRecord->ExceptionAddress;
    ULONG_PTR access_kind = exception_pointers->ExceptionRecord->NumberParameters >= 1
        ? exception_pointers->ExceptionRecord->ExceptionInformation[0]
        : 0;
    ULONG_PTR access_target = exception_pointers->ExceptionRecord->NumberParameters >= 2
        ? exception_pointers->ExceptionRecord->ExceptionInformation[1]
        : 0;
    DWORD stack_pointer = exception_pointers->ContextRecord->Esp;
    DWORD return_address = 0;
    SIZE_T return_address_bytes = 0;
    MEMORY_BASIC_INFORMATION memory_information;
    char allocation_module[MAX_PATH];

    memset(&memory_information, 0, sizeof(memory_information));
    allocation_module[0] = '\0';
    if (VirtualQuery((LPCVOID)instruction, &memory_information,
                     sizeof(memory_information)) != 0) {
        (void)GetModuleFileNameA((HMODULE)memory_information.AllocationBase,
                                 allocation_module,
                                 sizeof(allocation_module));
    }
    (void)ReadProcessMemory(GetCurrentProcess(), (LPCVOID)stack_pointer,
                            &return_address, sizeof(return_address),
                            &return_address_bytes);

    fprintf(stderr, "trace-exception=0x%08lx ip=0x%08lx rva=0x%08lx access=%lu target=0x%08lx allocation=0x%08lx module=%s sp=0x%08lx return=0x%08lx bytes=%lu\n",
            exception_pointers->ExceptionRecord->ExceptionCode,
            instruction,
            instruction - module_base,
            (unsigned long)access_kind,
            (unsigned long)access_target,
            (unsigned long)(ULONG_PTR)memory_information.AllocationBase,
            allocation_module[0] ? allocation_module : "<none>",
            (unsigned long)stack_pointer,
            (unsigned long)return_address,
            (unsigned long)return_address_bytes);
    return EXCEPTION_CONTINUE_SEARCH;
}

void ntdos64_host_applinit_trace_checkpoint(int stage) {
    if (stage == host_main_applinit_stop_stage) {
        host_main_applinit_stop_reason = stage;
        longjmp(host_main_applinit_stop_environment, stage);
    }
}

int main(int argc, char **argv) {
    char program_name[] = "ntvdm";
    char internal_flag[] = "-f";
    char stream_io_off[] = "-o";
    char *host_argv[] = {program_name, internal_flag, stream_io_off, 0};
    int result;

    SetUnhandledExceptionFilter(host_main_applinit_trace_exception_filter);

    if (argc == 2) {
        int parsed_stage = atoi(argv[1]);
        if (parsed_stage >= 10 && parsed_stage <= 6339)
            host_main_applinit_stop_stage = parsed_stage;
    }

    result = setjmp(host_main_applinit_stop_environment);
    if (result == 0) {
        (void)host_main(3, host_argv);
        return 1;
    }
    return result == host_main_applinit_stop_stage &&
                   host_main_applinit_stop_reason == host_main_applinit_stop_stage
               ? 0
               : 2;
}
