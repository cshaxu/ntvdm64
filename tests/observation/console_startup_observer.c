/*
 * Fixed, non-debug startup observation container for the selected x86
 * SoftPC product.  It owns a real CONIN$/CONOUT$ console, launches exactly
 * one product command line, waits a bounded interval, and records only the
 * observable process result.  It never attaches a debugger, searches guest
 * memory, installs breakpoints, or changes product inputs.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#include <stdio.h>
#include <string.h>

#define OBSERVATION_TIMEOUT_MS 8000u
#define OBSERVATION_TIMEOUT_EXIT 0x53504354u
#define OBSERVATION_STACK_WORDS 16u
#define OBSERVATION_THREAD_LIMIT 16u
#define OBSERVATION_FRAME_LIMIT 16u

typedef struct observation_thread_context {
    DWORD thread_id;
    BOOL context_available;
    CONTEXT context;
    DWORD frame_count;
    DWORD64 frames[OBSERVATION_FRAME_LIMIT];
} observation_thread_context;

typedef struct observation_image_identity {
    BOOL available;
    DWORD base_address;
    DWORD image_size;
    char module_name[MAX_MODULE_NAME32 + 1];
    char module_path[MAX_PATH];
} observation_image_identity;

static void capture_process_image(DWORD process_id,
                                  observation_image_identity *identity)
{
    HANDLE snapshot;
    MODULEENTRY32 entry;

    memset(identity, 0, sizeof(*identity));
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);
    if (snapshot == INVALID_HANDLE_VALUE) return;
    entry.dwSize = sizeof(entry);
    if (Module32First(snapshot, &entry)) {
        identity->available = TRUE;
        identity->base_address = (DWORD)(ULONG_PTR)entry.modBaseAddr;
        identity->image_size = entry.modBaseSize;
        strncpy_s(identity->module_name, sizeof(identity->module_name),
                  entry.szModule, _TRUNCATE);
        strncpy_s(identity->module_path, sizeof(identity->module_path),
                  entry.szExePath, _TRUNCATE);
    }
    CloseHandle(snapshot);
}

static DWORD capture_process_threads(HANDLE process, DWORD process_id,
                                     observation_thread_context *records,
                                     DWORD record_capacity)
{
    HANDLE snapshot;
    THREADENTRY32 entry;
    DWORD record_count = 0;

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    entry.dwSize = sizeof(entry);
    if (Thread32First(snapshot, &entry)) {
        do {
            HANDLE thread;
            if (entry.th32OwnerProcessID != process_id ||
                record_count == record_capacity) continue;
            records[record_count].thread_id = entry.th32ThreadID;
            records[record_count].context_available = FALSE;
            records[record_count].frame_count = 0;
            memset(&records[record_count].context, 0,
                   sizeof(records[record_count].context));
            thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT,
                                FALSE, entry.th32ThreadID);
            if (thread != NULL) {
                if (SuspendThread(thread) != (DWORD)-1) {
                    records[record_count].context.ContextFlags =
                        CONTEXT_CONTROL | CONTEXT_INTEGER;
                    records[record_count].context_available =
                        GetThreadContext(thread, &records[record_count].context);
                    if (records[record_count].context_available) {
                        STACKFRAME64 frame = { 0 };
                        CONTEXT walk_context = records[record_count].context;
                        frame.AddrPC.Offset = walk_context.Eip;
                        frame.AddrPC.Mode = AddrModeFlat;
                        frame.AddrStack.Offset = walk_context.Esp;
                        frame.AddrStack.Mode = AddrModeFlat;
                        frame.AddrFrame.Offset = walk_context.Ebp;
                        frame.AddrFrame.Mode = AddrModeFlat;
                        while (records[record_count].frame_count <
                               OBSERVATION_FRAME_LIMIT &&
                               StackWalk64(IMAGE_FILE_MACHINE_I386, process,
                                           thread, &frame, &walk_context, NULL,
                                           SymFunctionTableAccess64,
                                           SymGetModuleBase64, NULL)) {
                            records[record_count].frames[
                                records[record_count].frame_count++] =
                                frame.AddrPC.Offset;
                            if (frame.AddrReturn.Offset == 0) break;
                        }
                    }
                }
                CloseHandle(thread);
            }
            ++record_count;
        } while (Thread32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return record_count;
}

static void write_console_snapshot(HANDLE output, const char *report_path)
{
    char screen[4097];
    char path[MAX_PATH];
    DWORD count = 0;
    FILE *file = NULL;

    if (!ReadConsoleOutputCharacterA(output, screen, 4096,
                                     (COORD){ 0, 0 }, &count)) return;
    screen[count] = '\0';
    snprintf(path, sizeof(path), "%s.console.txt", report_path);
    if (fopen_s(&file, path, "wb") == 0 && file != NULL) {
        fwrite(screen, 1, count, file);
        fclose(file);
    }
}

static void clear_console(HANDLE output)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD written;
    DWORD cells;

    if (!GetConsoleScreenBufferInfo(output, &info)) return;
    cells = (DWORD)info.dwSize.X * (DWORD)info.dwSize.Y;
    (void)FillConsoleOutputCharacterA(output, ' ', cells, (COORD){ 0, 0 }, &written);
    (void)FillConsoleOutputAttribute(output, info.wAttributes, cells,
                                     (COORD){ 0, 0 }, &written);
    (void)SetConsoleCursorPosition(output, (COORD){ 0, 0 });
}

int main(int argc, char **argv)
{
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    SECURITY_ATTRIBUTES inherit = { sizeof(inherit), NULL, TRUE };
    HANDLE input;
    HANDLE output;
    DWORD wait_status;
    DWORD exit_code = STILL_ACTIVE;
    CONTEXT timed_context = { 0 };
    DWORD timed_stack[OBSERVATION_STACK_WORDS] = { 0 };
    observation_thread_context timed_threads[OBSERVATION_THREAD_LIMIT] = { 0 };
    BOOL have_timed_context = FALSE;
    BOOL have_timed_stack = FALSE;
    SIZE_T timed_stack_bytes = 0;
    DWORD timed_thread_count = 0;
    observation_image_identity image_identity = { 0 };
    BOOL symbols_initialized = FALSE;
    DWORD suspend_result = (DWORD)-1;
    char command_line[MAX_PATH * 2];
    char exception_report_path[MAX_PATH];
    char previous_exception_report_path[MAX_PATH];
    char main_return_report_path[MAX_PATH];
    char previous_main_return_report_path[MAX_PATH];
    char bop_return_report_path[MAX_PATH];
    char previous_bop_return_report_path[MAX_PATH];
    char dem_open_report_path[MAX_PATH];
    char previous_dem_open_report_path[MAX_PATH];
    char config_done_report_path[MAX_PATH];
    char previous_config_done_report_path[MAX_PATH];
    char config_command_store_report_path[MAX_PATH];
    char previous_config_command_store_report_path[MAX_PATH];
    char previous_config_command_linear[16];
    char dem_read_report_path[MAX_PATH];
    char previous_dem_read_report_path[MAX_PATH];
    char dem_seek_report_path[MAX_PATH];
    char dem_ioctl_report_path[MAX_PATH];
    char report_base_path[MAX_PATH];
    DWORD report_base_path_length;
    DWORD previous_exception_report_length;
    DWORD previous_main_return_report_length;
    DWORD previous_bop_return_report_length;
    DWORD previous_dem_open_report_length;
    DWORD previous_config_done_report_length;
    DWORD previous_config_command_store_report_length;
    DWORD previous_config_command_linear_length;
    DWORD previous_dem_read_report_length;
    BOOL had_previous_exception_report;
    BOOL had_previous_main_return_report;
    BOOL had_previous_bop_return_report;
    BOOL had_previous_dem_open_report;
    BOOL had_previous_config_done_report;
    BOOL had_previous_config_command_store_report;
    BOOL had_previous_config_command_linear;
    BOOL had_previous_dem_read_report;
    char fixed_system_root[MAX_PATH];
    char fixed_system_root_short[MAX_PATH];
    DWORD fixed_system_root_short_length = 0;
    FILE *report = NULL;

    if (argc < 4) return 64;
    report_base_path_length = GetFullPathNameA(argv[3],
        (DWORD)sizeof(report_base_path), report_base_path, NULL);
    if (report_base_path_length == 0 ||
        report_base_path_length >= sizeof(report_base_path)) return 68;
    if (snprintf(fixed_system_root, sizeof(fixed_system_root), "%s\\mvdm",
                 argv[2]) < 0) return 68;
    fixed_system_root_short_length = GetShortPathNameA(fixed_system_root,
        fixed_system_root_short, (DWORD)sizeof(fixed_system_root_short));
    if (!AllocConsole() && GetLastError() != ERROR_ACCESS_DENIED) return 65;
    input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                        OPEN_EXISTING, 0, NULL);
    output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                         OPEN_EXISTING, 0, NULL);
    if (input == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE) return 66;

    clear_console(output);
    SetStdHandle(STD_INPUT_HANDLE, input);
    SetStdHandle(STD_OUTPUT_HANDLE, output);
    SetStdHandle(STD_ERROR_HANDLE, output);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = input;
    startup.hStdOutput = output;
    startup.hStdError = output;
    if (argc == 4) {
        snprintf(command_line, sizeof(command_line),
                 "\"%s\" -f -o --command EXIT", argv[1]);
    } else {
        size_t command_length;
        int argument_index;

        snprintf(command_line, sizeof(command_line), "\"%s\"", argv[1]);
        command_length = strlen(command_line);
        for (argument_index = 4; argument_index < argc; ++argument_index) {
            size_t argument_length = strlen(argv[argument_index]);
            if (command_length + 1u + argument_length >= sizeof(command_line))
                return 68;
            command_line[command_length++] = ' ';
            memcpy(command_line + command_length, argv[argument_index],
                   argument_length + 1u);
            command_length += argument_length;
        }
    }

    snprintf(exception_report_path, sizeof(exception_report_path), "%s.exception.txt",
             report_base_path);
    snprintf(main_return_report_path, sizeof(main_return_report_path), "%s.return.txt",
             report_base_path);
    snprintf(bop_return_report_path, sizeof(bop_return_report_path), "%s.bop-return.txt",
             report_base_path);
    snprintf(dem_open_report_path, sizeof(dem_open_report_path), "%s.dem-open.txt",
             report_base_path);
    snprintf(config_done_report_path, sizeof(config_done_report_path),
             "%s.config-done.txt", report_base_path);
    snprintf(config_command_store_report_path,
             sizeof(config_command_store_report_path),
             "%s.config-command-store.txt", report_base_path);
    snprintf(dem_read_report_path, sizeof(dem_read_report_path),
             "%s.dem-read.txt", report_base_path);
    snprintf(dem_seek_report_path, sizeof(dem_seek_report_path),
             "%s.dem-seek.txt", report_base_path);
    snprintf(dem_ioctl_report_path, sizeof(dem_ioctl_report_path),
             "%s.dem-ioctl.txt", report_base_path);
    previous_exception_report_length = GetEnvironmentVariableA(
        "MVDM_EXCEPTION_REPORT_PATH", previous_exception_report_path,
        (DWORD)sizeof(previous_exception_report_path));
    had_previous_exception_report = previous_exception_report_length != 0 &&
        previous_exception_report_length < sizeof(previous_exception_report_path);
    previous_main_return_report_length = GetEnvironmentVariableA(
        "MVDM_MAIN_RETURN_REPORT_PATH", previous_main_return_report_path,
        (DWORD)sizeof(previous_main_return_report_path));
    had_previous_main_return_report = previous_main_return_report_length != 0 &&
        previous_main_return_report_length < sizeof(previous_main_return_report_path);
    previous_bop_return_report_length = GetEnvironmentVariableA(
        "MVDM_BOP_RETURN_REPORT_PATH", previous_bop_return_report_path,
        (DWORD)sizeof(previous_bop_return_report_path));
    had_previous_bop_return_report = previous_bop_return_report_length != 0 &&
        previous_bop_return_report_length < sizeof(previous_bop_return_report_path);
    previous_dem_open_report_length = GetEnvironmentVariableA(
        "MVDM_DEM_OPEN_REPORT_PATH", previous_dem_open_report_path,
        (DWORD)sizeof(previous_dem_open_report_path));
    had_previous_dem_open_report = previous_dem_open_report_length != 0 &&
        previous_dem_open_report_length < sizeof(previous_dem_open_report_path);
    previous_config_done_report_length = GetEnvironmentVariableA(
        "MVDM_CONFIG_DONE_REPORT_PATH", previous_config_done_report_path,
        (DWORD)sizeof(previous_config_done_report_path));
    had_previous_config_done_report = previous_config_done_report_length != 0 &&
        previous_config_done_report_length < sizeof(previous_config_done_report_path);
    previous_config_command_store_report_length = GetEnvironmentVariableA(
        "MVDM_CONFIG_COMMAND_STORE_REPORT_PATH",
        previous_config_command_store_report_path,
        (DWORD)sizeof(previous_config_command_store_report_path));
    had_previous_config_command_store_report =
        previous_config_command_store_report_length != 0 &&
        previous_config_command_store_report_length <
            sizeof(previous_config_command_store_report_path);
    previous_config_command_linear_length = GetEnvironmentVariableA(
        "MVDM_CONFIG_COMMAND_LINEAR", previous_config_command_linear,
        (DWORD)sizeof(previous_config_command_linear));
    had_previous_config_command_linear = previous_config_command_linear_length != 0 &&
        previous_config_command_linear_length < sizeof(previous_config_command_linear);
    previous_dem_read_report_length = GetEnvironmentVariableA(
        "MVDM_DEM_READ_REPORT_PATH", previous_dem_read_report_path,
        (DWORD)sizeof(previous_dem_read_report_path));
    had_previous_dem_read_report = previous_dem_read_report_length != 0 &&
        previous_dem_read_report_length < sizeof(previous_dem_read_report_path);
    SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH", exception_report_path);
    SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH", main_return_report_path);
    SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH", bop_return_report_path);
    SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH", dem_open_report_path);
    SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH", config_done_report_path);
    SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_STORE_REPORT_PATH",
        config_command_store_report_path);
    /* S5 attributes this selected image's original trys `commnd` store to
     * live CS 8E08 plus map offset 3466. This is a fixed-image observer
     * input, not a guest ABI or an address translation facility. */
    SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_LINEAR", "0x914e6");
    SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH", dem_read_report_path);
    SetEnvironmentVariableA("MVDM_DEM_SEEK_REPORT_PATH", dem_seek_report_path);
    SetEnvironmentVariableA("MVDM_DEM_IOCTL_REPORT_PATH", dem_ioctl_report_path);

    if (!CreateProcessA(NULL, command_line, NULL, NULL, TRUE, 0, NULL, argv[2],
                        &startup, &child)) {
        if (had_previous_exception_report)
            SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH",
                                    previous_exception_report_path);
        else
            SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH", NULL);
        if (had_previous_main_return_report)
            SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH",
                                    previous_main_return_report_path);
        else
            SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH", NULL);
        if (had_previous_bop_return_report)
            SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH",
                                    previous_bop_return_report_path);
        else
            SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH", NULL);
        if (had_previous_dem_open_report)
            SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH",
                                    previous_dem_open_report_path);
        else
            SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH", NULL);
        if (had_previous_config_done_report)
            SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH",
                                     previous_config_done_report_path);
        else
            SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH", NULL);
        if (had_previous_config_command_store_report)
            SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_STORE_REPORT_PATH",
                                    previous_config_command_store_report_path);
        else
            SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_STORE_REPORT_PATH", NULL);
        if (had_previous_config_command_linear)
            SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_LINEAR",
                                    previous_config_command_linear);
        else
            SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_LINEAR", NULL);
        if (had_previous_dem_read_report)
            SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH",
                                    previous_dem_read_report_path);
        else
            SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH", NULL);
        CloseHandle(input);
        CloseHandle(output);
        return 67;
    }
    if (had_previous_exception_report)
        SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH",
                                previous_exception_report_path);
    else
        SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH", NULL);
    if (had_previous_main_return_report)
        SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH",
                                previous_main_return_report_path);
    else
        SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH", NULL);
    if (had_previous_bop_return_report)
        SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH",
                                previous_bop_return_report_path);
    else
        SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH", NULL);
    if (had_previous_dem_open_report)
        SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH",
                                previous_dem_open_report_path);
    else
        SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH", NULL);
    if (had_previous_config_done_report)
        SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH",
                                 previous_config_done_report_path);
    else
        SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH", NULL);
    if (had_previous_config_command_store_report)
        SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_STORE_REPORT_PATH",
                                previous_config_command_store_report_path);
    else
        SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_STORE_REPORT_PATH", NULL);
    if (had_previous_config_command_linear)
        SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_LINEAR",
                                previous_config_command_linear);
    else
        SetEnvironmentVariableA("MVDM_CONFIG_COMMAND_LINEAR", NULL);
    if (had_previous_dem_read_report)
        SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH",
                                previous_dem_read_report_path);
    else
        SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH", NULL);
    wait_status = WaitForSingleObject(child.hProcess, OBSERVATION_TIMEOUT_MS);
    capture_process_image(child.dwProcessId, &image_identity);
    if (wait_status == WAIT_TIMEOUT) {
        /* The fixed container observes the product without a debugger.  A
         * bounded suspension gives the evidence record one architectural
         * stop state before the existing watchdog terminates the process.
         * It neither changes product inputs nor resumes/modifies the thread.
         */
        suspend_result = SuspendThread(child.hThread);
        if (suspend_result != (DWORD)-1) {
            memset(&timed_context, 0, sizeof(timed_context));
            timed_context.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
            have_timed_context = GetThreadContext(child.hThread, &timed_context);
            if (have_timed_context) {
                have_timed_stack = ReadProcessMemory(
                    child.hProcess, (LPCVOID)(ULONG_PTR)timed_context.Esp,
                    timed_stack, sizeof(timed_stack), &timed_stack_bytes) &&
                    timed_stack_bytes == sizeof(timed_stack);
            }
            symbols_initialized = SymInitialize(child.hProcess, NULL, TRUE);
            timed_thread_count = capture_process_threads(child.hProcess,
                                                         child.dwProcessId,
                                                         timed_threads,
                                                         OBSERVATION_THREAD_LIMIT);
        }
        TerminateProcess(child.hProcess, OBSERVATION_TIMEOUT_EXIT);
        WaitForSingleObject(child.hProcess, 1000);
    }
    GetExitCodeProcess(child.hProcess, &exit_code);
    write_console_snapshot(output, argv[3]);
    if (fopen_s(&report, argv[3], "wb") == 0 && report != NULL) {
        fprintf(report, "container=console-owning-nondebug\n");
        fprintf(report, "pid=%lu\n", (unsigned long)child.dwProcessId);
        fprintf(report, "result=%s\n", wait_status == WAIT_TIMEOUT ? "timeout" : "exited");
        fprintf(report, "exit=0x%08lx\n", (unsigned long)exit_code);
        fprintf(report, "timeout-ms=%u\n", OBSERVATION_TIMEOUT_MS);
        fprintf(report, "fixed-system-root=%s\n", fixed_system_root);
        fprintf(report, "fixed-system-root-chars=%lu\n",
                (unsigned long)strlen(fixed_system_root));
        fprintf(report, "fixed-system-root-short-chars=%lu\n",
                (unsigned long)fixed_system_root_short_length);
        if (fixed_system_root_short_length != 0 &&
            fixed_system_root_short_length < sizeof(fixed_system_root_short))
            fprintf(report, "fixed-system-root-short=%s\n",
                    fixed_system_root_short);
        if (image_identity.available) {
            fprintf(report, "image-module=%s\n", image_identity.module_name);
            fprintf(report, "image-path=%s\n", image_identity.module_path);
            fprintf(report, "image-base=0x%08lx\n",
                    (unsigned long)image_identity.base_address);
            fprintf(report, "image-size=0x%08lx\n",
                    (unsigned long)image_identity.image_size);
        } else {
            fprintf(report, "image-identity=unavailable\n");
        }
        if (have_timed_context) {
            fprintf(report, "stop-eip=0x%08lx\n", (unsigned long)timed_context.Eip);
            fprintf(report, "stop-esp=0x%08lx\n", (unsigned long)timed_context.Esp);
            fprintf(report, "stop-ebp=0x%08lx\n", (unsigned long)timed_context.Ebp);
            fprintf(report, "stop-eax=0x%08lx\n", (unsigned long)timed_context.Eax);
            fprintf(report, "stop-ebx=0x%08lx\n", (unsigned long)timed_context.Ebx);
            fprintf(report, "stop-ecx=0x%08lx\n", (unsigned long)timed_context.Ecx);
            fprintf(report, "stop-edx=0x%08lx\n", (unsigned long)timed_context.Edx);
            if (have_timed_stack) {
                DWORD stack_index;
                for (stack_index = 0; stack_index < OBSERVATION_STACK_WORDS;
                     ++stack_index) {
                    fprintf(report, "stop-stack-%02lu=0x%08lx\n",
                            (unsigned long)stack_index,
                            (unsigned long)timed_stack[stack_index]);
                }
            } else {
                fprintf(report, "stop-stack=unavailable\n");
            }
            {
                DWORD thread_index;
                for (thread_index = 0; thread_index < timed_thread_count;
                     ++thread_index) {
                    observation_thread_context *thread =
                        &timed_threads[thread_index];
                    if (thread->context_available) {
                        DWORD frame_index;
                        fprintf(report,
                                "thread-%02lu=id:0x%08lx,eip:0x%08lx,esp:0x%08lx\n",
                                (unsigned long)thread_index,
                                (unsigned long)thread->thread_id,
                                (unsigned long)thread->context.Eip,
                                (unsigned long)thread->context.Esp);
                        for (frame_index = 0; frame_index < thread->frame_count;
                             ++frame_index) {
                            fprintf(report, "thread-%02lu-frame-%02lu=0x%08llx\n",
                                    (unsigned long)thread_index,
                                    (unsigned long)frame_index,
                                    (unsigned long long)thread->frames[frame_index]);
                        }
                    } else {
                        fprintf(report, "thread-%02lu=id:0x%08lx,context:unavailable\n",
                                (unsigned long)thread_index,
                                (unsigned long)thread->thread_id);
                    }
                }
            }
        } else if (wait_status == WAIT_TIMEOUT) {
            fprintf(report, "stop-context=unavailable\n");
        }
        fclose(report);
    }
    if (symbols_initialized) SymCleanup(child.hProcess);
    CloseHandle(child.hThread);
    CloseHandle(child.hProcess);
    CloseHandle(input);
    CloseHandle(output);
    return 0;
}
