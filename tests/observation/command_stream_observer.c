/* Test-only ConPTY host for T420 S34.  The child cmd.exe retains a Console
 * identity while its left-hand run16 process receives an inherited file or
 * pipe standard stream.  That is the product scenario; a CI pipe without a
 * Console is not a substitute for it. */
#include <windows.h>
#include <stdio.h>
#include <string.h>

static HANDLE output_read;
static HANDLE log_file;

static DWORD WINAPI copy_output(void *ignored)
{
    char data[512];
    DWORD count, written;
    (void)ignored;
    while (ReadFile(output_read, data, sizeof(data), &count, NULL) && count) {
        if (!WriteFile(log_file, data, count, &written, NULL) || written != count)
            return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    HANDLE input_read = NULL, input_write = NULL, output_write = NULL;
    HANDLE thread = NULL, job = NULL;
    HPCON console = NULL;
    SIZE_T attributes_bytes = 0;
    STARTUPINFOEXA startup = {0};
    PROCESS_INFORMATION child = {0};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = {0};
    char command[2048];
    DWORD wait, exit_code = ERROR_GEN_FAILURE;

    if (argc != 3 || strlen(argv[2]) >= sizeof(command)) return 64;
    log_file = CreateFileA(argv[1], GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (log_file == INVALID_HANDLE_VALUE) return 65;
    if (!CreatePipe(&input_read, &input_write, NULL, 0) ||
        !CreatePipe(&output_read, &output_write, NULL, 0) ||
        FAILED(CreatePseudoConsole((COORD){100, 30}, input_read, output_write,
            0, &console))) goto done;
    CloseHandle(input_read); input_read = NULL;
    CloseHandle(output_write); output_write = NULL;
    thread = CreateThread(NULL, 0, copy_output, NULL, 0, NULL);
    if (!thread) goto done;
    InitializeProcThreadAttributeList(NULL, 1, 0, &attributes_bytes);
    startup.lpAttributeList = HeapAlloc(GetProcessHeap(), 0, attributes_bytes);
    if (!startup.lpAttributeList ||
        !InitializeProcThreadAttributeList(startup.lpAttributeList, 1, 0,
            &attributes_bytes) ||
        !UpdateProcThreadAttribute(startup.lpAttributeList, 0,
            PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, console, sizeof(console),
            NULL, NULL)) goto done;
    startup.StartupInfo.cb = sizeof(startup);
    memcpy(command, argv[2], strlen(argv[2]) + 1u);
    job = CreateJobObjectA(NULL, NULL);
    if (!job) goto done;
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation,
        &limits, sizeof(limits)) || !CreateProcessA(NULL, command, NULL, NULL,
        FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, "O:\\winnt",
        &startup.StartupInfo, &child) ||
        !AssignProcessToJobObject(job, child.hProcess)) goto done;
    wait = WaitForSingleObject(child.hProcess, 15000);
    if (wait == WAIT_OBJECT_0 && GetExitCodeProcess(child.hProcess, &exit_code)) {
        printf("wait=exited exit=%lu\n", (unsigned long)exit_code);
    } else {
        printf("wait=%lu\n", (unsigned long)wait);
        exit_code = ERROR_TIMEOUT;
    }
done:
    if (child.hThread) CloseHandle(child.hThread);
    if (child.hProcess) CloseHandle(child.hProcess);
    if (job) CloseHandle(job);
    if (startup.lpAttributeList) {
        DeleteProcThreadAttributeList(startup.lpAttributeList);
        HeapFree(GetProcessHeap(), 0, startup.lpAttributeList);
    }
    if (console) ClosePseudoConsole(console);
    if (input_write) CloseHandle(input_write);
    if (output_read) CloseHandle(output_read);
    if (thread) { WaitForSingleObject(thread, 3000); CloseHandle(thread); }
    if (input_read) CloseHandle(input_read);
    if (output_write) CloseHandle(output_write);
    if (log_file != INVALID_HANDLE_VALUE) CloseHandle(log_file);
    return exit_code == 0 ? 0 : 1;
}
