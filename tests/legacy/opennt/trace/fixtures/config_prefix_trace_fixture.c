/* Trace the original config.c path through its first SAS lifecycle boundary. */
#include <windows.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pif.h"

extern void config(void);
#ifdef RUNNER_SESSION_TRACE_SAS_SNAPSHOT
extern unsigned char *Start_of_M_area;
extern unsigned long c_sas_memory_size(void);
extern unsigned int c_sas_memory_type(unsigned long address);

static void trace_sas_snapshot(const char *phase) {
    fprintf(stderr, "trace-sas phase=%s base=%p size=0x%08lx\n", phase,
            (void *)Start_of_M_area, c_sas_memory_size());
    fprintf(stderr,
            "trace-sas-types phase=%s low=%u ntstate=%u ram=%u video=%u\n",
            phase,
            c_sas_memory_type(0),
            c_sas_memory_type(0x714UL),
            c_sas_memory_type(0x9ffffUL),
            c_sas_memory_type(0xa0000UL));
}
#endif
#ifdef RUNNER_SESSION_TRACE_GFI_INIT
extern void gfi_init(void);
#endif
#ifdef RUNNER_SESSION_TRACE_ICA_INIT
extern void InitializeIcaLock(void);
#endif
#ifdef RUNNER_SESSION_TRACE_TIMER_INIT
extern void TimerInit(void);
#endif
#ifdef RUNNER_SESSION_TRACE_CPU_INIT
extern void c_cpu_init(void);
#endif
#ifdef RUNNER_SESSION_TRACE_CVIDC_WORKSPACE_INIT
extern unsigned long setup_global_data_ptr(void);
extern void setup_vga_globals(void);
#endif
#ifdef RUNNER_SAS_SINGLE_WRITE_TRACE
extern void c_sas_store(unsigned long address, unsigned char value);
extern void c_sas_fills(unsigned long address, unsigned char value,
                        unsigned long length);
extern void runner_sas_single_write_trace_stop(void);
#endif
extern jmp_buf runner_config_prefix_stop_environment;
extern int runner_config_prefix_stop_reason(void);
extern BOOL runner_direct_cli_transport_begin(const char *pif_path,
                                               BOOL first_vdm);
extern void runner_direct_cli_transport_end(void);

static LONG WINAPI config_prefix_trace_exception_filter(
    EXCEPTION_POINTERS *exception_pointers) {
    DWORD module_base = (DWORD)(ULONG_PTR)GetModuleHandleA(NULL);
    DWORD instruction = (DWORD)(ULONG_PTR)
        exception_pointers->ExceptionRecord->ExceptionAddress;
    ULONG_PTR access_kind = exception_pointers->ExceptionRecord->NumberParameters >= 1
        ? exception_pointers->ExceptionRecord->ExceptionInformation[0] : 0;
    ULONG_PTR access_target = exception_pointers->ExceptionRecord->NumberParameters >= 2
        ? exception_pointers->ExceptionRecord->ExceptionInformation[1] : 0;
    DWORD stack_pointer = exception_pointers->ContextRecord->Esp;
    DWORD frame_pointer = exception_pointers->ContextRecord->Ebp;
    DWORD return_address = 0;
    DWORD return_rva = 0;
    SIZE_T return_address_bytes = 0;
    DWORD stack_words[8] = {0};
    SIZE_T stack_word_bytes = 0;
    unsigned int stack_index;

    (void)ReadProcessMemory(GetCurrentProcess(), (LPCVOID)stack_pointer,
                            &return_address, sizeof(return_address),
                            &return_address_bytes);
    if (return_address >= module_base)
        return_rva = return_address - module_base;
    fprintf(stderr,
            "trace-exception=0x%08lx base=0x%08lx ip=0x%08lx rva=0x%08lx access=%lu target=0x%08lx sp=0x%08lx return=0x%08lx return-rva=0x%08lx bytes=%lu\n",
            exception_pointers->ExceptionRecord->ExceptionCode, module_base,
            instruction, instruction - module_base, (unsigned long)access_kind,
            (unsigned long)access_target, (unsigned long)stack_pointer,
            (unsigned long)return_address, (unsigned long)return_rva,
            (unsigned long)return_address_bytes);
    (void)ReadProcessMemory(GetCurrentProcess(), (LPCVOID)stack_pointer,
                            stack_words, sizeof(stack_words),
                            &stack_word_bytes);
    for (stack_index = 0;
         stack_index < stack_word_bytes / sizeof(stack_words[0]);
         ++stack_index) {
        DWORD stack_rva = stack_words[stack_index] >= module_base
            ? stack_words[stack_index] - module_base : 0;
        fprintf(stderr, "trace-stack[%u]=0x%08lx rva=0x%08lx\n", stack_index,
                (unsigned long)stack_words[stack_index],
                (unsigned long)stack_rva);
    }
    for (stack_index = 0; stack_index < 8 && frame_pointer != 0;
         ++stack_index) {
        DWORD frame_words[2] = {0};
        SIZE_T frame_bytes = 0;
        DWORD frame_rva;

        if (!ReadProcessMemory(GetCurrentProcess(), (LPCVOID)frame_pointer,
                               frame_words, sizeof(frame_words),
                               &frame_bytes) || frame_bytes != sizeof(frame_words)) {
            break;
        }
        frame_rva = frame_words[1] >= module_base
            ? frame_words[1] - module_base : 0;
        fprintf(stderr, "trace-frame[%u]=0x%08lx return=0x%08lx rva=0x%08lx\n",
                stack_index, (unsigned long)frame_pointer,
                (unsigned long)frame_words[1], (unsigned long)frame_rva);
        if (frame_words[0] <= frame_pointer) break;
        frame_pointer = frame_words[0];
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
#ifdef RUNNER_STARTUP_PREFIX_TRACE
extern void runner_startup_prefix_trace_checkpoint(int stage);
#define RUNNER_STARTUP_PREFIX_STAGE(stage) \
    runner_startup_prefix_trace_checkpoint(stage)
#else
#define RUNNER_STARTUP_PREFIX_STAGE(stage) ((void)0)
#endif
#ifdef CONFIG_PREFIX_TRACE_SAS_INIT
extern void sas_term(void);
extern uint8_t *runner_ccpu_sm0_ram(void);
#endif
#ifdef CONFIG_PREFIX_TRACE_SESSION_INIT
extern int InitialiseDosEmulation(int argc, char **argv);
#endif

static BOOL create_minimal_pif(char *file_name, DWORD file_name_size) {
    char directory[MAX_PATH];
    STDPIF standard_pif;
    PIFEXTHDR extension_header;
    HANDLE file;
    DWORD written;

    if (GetTempPathA(sizeof(directory), directory) == 0 ||
        GetTempFileNameA(directory, "ncp", 0, file_name) == 0) {
        return FALSE;
    }
    file = CreateFileA(file_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_TEMPORARY, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;

    memset(&standard_pif, 0, sizeof(standard_pif));
    memset(&extension_header, 0, sizeof(extension_header));
    extension_header.extnxthdrfloff = LASTHDRPTR;
    if (!WriteFile(file, &standard_pif, sizeof(standard_pif), &written, NULL) ||
        written != sizeof(standard_pif) ||
        !WriteFile(file, &extension_header, sizeof(extension_header), &written,
                   NULL) || written != sizeof(extension_header)) {
        CloseHandle(file);
        DeleteFileA(file_name);
        return FALSE;
    }
    CloseHandle(file);
    (void)file_name_size;
    return TRUE;
}

int main(void) {
    char pif_name[MAX_PATH];
    char prior_title[MAX_PATH] = {0};
    char restored_title[MAX_PATH] = {0};
    int stop_result;
    int result;

    SetUnhandledExceptionFilter(config_prefix_trace_exception_filter);
    if (!create_minimal_pif(pif_name, sizeof(pif_name))) return 1;
    GetConsoleTitleA(prior_title, sizeof(prior_title));
    if (!runner_direct_cli_transport_begin(pif_name, TRUE)) {
        DeleteFileA(pif_name);
        return 2;
    }

    stop_result = setjmp(runner_config_prefix_stop_environment);
    if (stop_result == 0) {
        RUNNER_STARTUP_PREFIX_STAGE(1);
#ifdef RUNNER_SESSION_TRACE_TIMER_INIT
        /* obj.vdm/ntvdm.c performs this original pre-host_main step. */
        TimerInit();
#endif
#ifdef RUNNER_SESSION_TRACE_ICA_INIT
        InitializeIcaLock();
#endif
#ifdef RUNNER_SESSION_TRACE_GFI_INIT
        gfi_init();
#endif
        RUNNER_STARTUP_PREFIX_STAGE(2);
        config();
#ifdef RUNNER_SESSION_TRACE_SAS_SNAPSHOT
        trace_sas_snapshot("after-config");
#endif
        RUNNER_STARTUP_PREFIX_STAGE(3);
#if defined(CONFIG_PREFIX_TRACE_SESSION_INIT)
#ifdef RUNNER_SESSION_TRACE_CPU_INIT
        c_cpu_init();
#endif
#ifdef RUNNER_SESSION_TRACE_SAS_SNAPSHOT
        trace_sas_snapshot("after-cpu-init");
#endif
#ifdef RUNNER_SESSION_TRACE_CVIDC_WORKSPACE_INIT
        /* Preserve base/support/main.c's original post-cpu_init ordering. */
        (void)setup_global_data_ptr();
        setup_vga_globals();
#endif
        RUNNER_STARTUP_PREFIX_STAGE(4);
#ifdef RUNNER_SAS_SINGLE_WRITE_TRACE
        c_sas_store(0, 0);
        c_sas_store(0x9ffffUL, 0);
        c_sas_fills(0, 0, 640UL * 1024UL);
        runner_sas_single_write_trace_stop();
#endif
        InitialiseDosEmulation(0, NULL);
        result = 3;
#elif defined(CONFIG_PREFIX_EXPECT_CONFIG_RETURN)
        sas_term();
        result = 0;
#else
        result = 3;
#endif
    } else {
#ifdef RUNNER_TRACE_PRINT_STOP_REASON
        fprintf(stderr, "trace-stop result=%d reason=%d\n", stop_result,
                runner_config_prefix_stop_reason());
#endif
#ifdef RUNNER_STARTUP_PREFIX_TRACE
        if ((stop_result < 141 || stop_result > 144) ||
            runner_config_prefix_stop_reason() != stop_result) {
            result = 4;
        } else {
            result = 0;
        }
#else
        if (stop_result !=
#if defined(RUNNER_SAS_SINGLE_WRITE_TRACE)
               130 || runner_config_prefix_stop_reason() != 130) {
#elif defined(RUNNER_RESET_TRACE)
               (100 + RUNNER_RESET_TRACE_STOP_STAGE) ||
               runner_config_prefix_stop_reason() !=
               (100 + RUNNER_RESET_TRACE_STOP_STAGE)) {
#elif defined(CONFIG_PREFIX_TRACE_SESSION_VINT)
               (40 + RUNNER_SESSION_TRACE_VINT_STOP_STAGE) ||
               runner_config_prefix_stop_reason() !=
               (40 + RUNNER_SESSION_TRACE_VINT_STOP_STAGE)) {
#elif defined(CONFIG_PREFIX_TRACE_SESSION_BEFORE_RESET)
               38 || runner_config_prefix_stop_reason() != 38) {
#elif defined(RUNNER_CPU_INIT_TRACE)
               (120 + RUNNER_CPU_INIT_TRACE_STOP_STAGE) ||
               runner_config_prefix_stop_reason() !=
               (120 + RUNNER_CPU_INIT_TRACE_STOP_STAGE)) {
#elif defined(CONFIG_PREFIX_TRACE_SESSION_AFTER_RESET)
               37 || runner_config_prefix_stop_reason() != 37) {
#elif defined(CONFIG_PREFIX_TRACE_SESSION_AFTER_IO)
               36 || runner_config_prefix_stop_reason() != 36) {
#elif defined(CONFIG_PREFIX_TRACE_SESSION_INIT)
               22 || runner_config_prefix_stop_reason() != 22) {
#elif defined(CONFIG_PREFIX_TRACE_ORIGINAL_GFI)
               18 || runner_config_prefix_stop_reason() != 18) {
#elif defined(CONFIG_PREFIX_TRACE_ORIGINAL_UMB)
               7 || runner_config_prefix_stop_reason() != 7) {
#elif defined(CONFIG_PREFIX_TRACE_POST_CPU40)
               6 || runner_config_prefix_stop_reason() != 6) {
#elif defined(CONFIG_PREFIX_TRACE_SAS_INIT)
               5 || runner_config_prefix_stop_reason() != 5) {
#else
               1 || runner_config_prefix_stop_reason() != 1) {
#endif
            result = 4;
        } else {
            result = 0;
        }
#endif
    }

#ifdef CONFIG_PREFIX_TRACE_SAS_INIT
    if (stop_result == 5 || stop_result == 6 || stop_result == 7 ||
        stop_result == 18 || stop_result == 22 || stop_result == 36 ||
        stop_result == 37 || (stop_result >= 101 && stop_result <= 109) ||
        (stop_result >= 121 && stop_result <= 122) ||
        /* Stages 1/2 stop before config() and therefore before sas_init(). */
        (stop_result >= 143 && stop_result <= 144) || stop_result == 130) sas_term();
    if (result == 0 && runner_ccpu_sm0_ram() != NULL) result = 8;
#endif

    SetConsoleTitleA(prior_title);
    GetConsoleTitleA(restored_title, sizeof(restored_title));
    if (result == 0 && strcmp(prior_title, restored_title) != 0) result = 5;
    runner_direct_cli_transport_end();
    if (!DeleteFileA(pif_name) && result == 0) result = 6;
    if (result == 0 && GetFileAttributesA(pif_name) != (DWORD)-1)
        result = 7;
    return result;
}
