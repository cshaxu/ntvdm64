/*
 * Runs the published product in a real Windows Console without any of the
 * MVDM_* diagnostic environment used by console_startup_observer.  Codex's
 * command runner supplies pipe standard handles, which are not equivalent to
 * an interactive cmd.exe Console; explicitly supplying CONIN$/CONOUT$ makes
 * this a release integration test for the latter contract.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_BYTES 32768u
#define RUN_TIMEOUT_MS 30000u

static BOOL append_quoted(char *line, size_t capacity, size_t *used, const char *argument)
{
    size_t count = strlen(argument);
    if (*used + count + 4u >= capacity) return FALSE;
    if (*used != 0u) line[(*used)++] = ' ';
    line[(*used)++] = '"'; memcpy(line + *used, argument, count); *used += count;
    line[(*used)++] = '"'; line[*used] = '\0'; return TRUE;
}
static void clear_console(HANDLE output)
{
    CONSOLE_SCREEN_BUFFER_INFO info; DWORD written; COORD origin = { 0, 0 };
    if (!GetConsoleScreenBufferInfo(output, &info)) return;
    FillConsoleOutputCharacterA(output, ' ', (DWORD)(info.dwSize.X * info.dwSize.Y), origin, &written);
    FillConsoleOutputAttribute(output, info.wAttributes, (DWORD)(info.dwSize.X * info.dwSize.Y), origin, &written);
    SetConsoleCursorPosition(output, origin);
}
static BOOL read_console(HANDLE output, char *text, DWORD capacity)
{
    CONSOLE_SCREEN_BUFFER_INFO info; DWORD count = 0; COORD origin = { 0, 0 };
    if (capacity == 0u || !GetConsoleScreenBufferInfo(output, &info)) return FALSE;
    if (!ReadConsoleOutputCharacterA(output, text, capacity - 1u, origin, &count)) return FALSE;
    text[count] = '\0'; return TRUE;
}
static BOOL screen_has(HANDLE output, char *screen, DWORD capacity,
                       const char *text)
{
    return read_console(output,screen,capacity) && strstr(screen,text)!=NULL;
}
static BOOL wait_screen_has(HANDLE output, char *screen, DWORD capacity,
                            const char *text, DWORD timeout)
{
    ULONGLONG deadline=GetTickCount64()+timeout;
    do {
        if(screen_has(output,screen,capacity,text))return TRUE;
        Sleep(20u);
    } while(GetTickCount64()<deadline);
    return FALSE;
}
static BOOL send_console_text(HANDLE input, const char *text)
{
    INPUT_RECORD records[128];
    DWORD written=0; size_t index,count=strlen(text);
    if(count*2u>ARRAYSIZE(records))return FALSE;
    for(index=0;index<count;++index) {
        INPUT_RECORD *down=&records[index*2u],*up=&records[index*2u+1u];
        memset(down,0,sizeof(*down));memset(up,0,sizeof(*up));
        down->EventType=KEY_EVENT;down->Event.KeyEvent.bKeyDown=TRUE;
        down->Event.KeyEvent.wRepeatCount=1;down->Event.KeyEvent.uChar.AsciiChar=text[index];
        up->EventType=KEY_EVENT;up->Event.KeyEvent.bKeyDown=FALSE;
        up->Event.KeyEvent.wRepeatCount=1;up->Event.KeyEvent.uChar.AsciiChar=text[index];
    }
    return WriteConsoleInputA(input,records,(DWORD)(count*2u),&written) &&
        written==count*2u;
}
static int run_case(const char *product, const char *working_directory,
                    const char *command, const char *expected, HANDLE input,
                    HANDLE output, char *screen, DWORD screen_capacity)
{
    STARTUPINFOA startup = { sizeof(startup) }; PROCESS_INFORMATION child = { 0 }; char command_line[4096]; size_t used = 0u; DWORD wait_status, exit_code = 0u;
    command_line[0] = '\0'; if (!append_quoted(command_line, sizeof(command_line), &used, product) || !append_quoted(command_line, sizeof(command_line), &used, command)) return 10;
    clear_console(output); startup.dwFlags = STARTF_USESTDHANDLES; startup.hStdInput = input; startup.hStdOutput = output; startup.hStdError = output;
    if (!CreateProcessA(NULL, command_line, NULL, NULL, TRUE, 0, NULL, working_directory, &startup, &child)) return 11;
    wait_status = WaitForSingleObject(child.hProcess, RUN_TIMEOUT_MS); if (wait_status == WAIT_OBJECT_0) GetExitCodeProcess(child.hProcess, &exit_code);
    if (wait_status == WAIT_TIMEOUT) {
        TerminateProcess(child.hProcess, 0x53504354u);
        WaitForSingleObject(child.hProcess, 1000u);
    }
    CloseHandle(child.hThread); CloseHandle(child.hProcess); if (wait_status != WAIT_OBJECT_0) return 12; if (exit_code != 0u) return 13; if (!read_console(output, screen, screen_capacity)) return 14;
    return expected == NULL || strstr(screen, expected) != NULL ? 0 : 15;
}
static int run_interactive_nested_command(const char *product,
    const char *working_directory,HANDLE input,HANDLE output,char *screen,
    DWORD screen_capacity)
{
    STARTUPINFOA startup={sizeof(startup)};PROCESS_INFORMATION child={0};
    char command_line[4096];size_t used=0u;DWORD status,exit_code=0u;
    if(!append_quoted(command_line,sizeof(command_line),&used,product) ||
        !append_quoted(command_line,sizeof(command_line),&used,"command"))return 20;
    clear_console(output);startup.dwFlags=STARTF_USESTDHANDLES;
    startup.hStdInput=input;startup.hStdOutput=output;startup.hStdError=output;
    if(!CreateProcessA(NULL,command_line,NULL,NULL,TRUE,0,NULL,working_directory,
        &startup,&child))return 21;
    if(!wait_screen_has(output,screen,screen_capacity,"O:\\WINNT>",RUN_TIMEOUT_MS))
        {TerminateProcess(child.hProcess,0x53504354u);CloseHandle(child.hThread);CloseHandle(child.hProcess);return 22;}
    clear_console(output);
    if(!send_console_text(input,"command\r"))
        {TerminateProcess(child.hProcess,0x53504354u);CloseHandle(child.hThread);CloseHandle(child.hProcess);return 23;}
    /* The newly cleared screen prevents the first prompt from satisfying this
     * second-stage condition: only the second COMMAND banner proves entry. */
    if(!wait_screen_has(output,screen,screen_capacity,"Microsoft(R) Windows NT DOS",RUN_TIMEOUT_MS) ||
        !send_console_text(input,"ver\r"))
        {TerminateProcess(child.hProcess,0x53504354u);CloseHandle(child.hThread);CloseHandle(child.hProcess);return 24;}
    if(!wait_screen_has(output,screen,screen_capacity,"MS-DOS Version 5.00.500",RUN_TIMEOUT_MS) ||
        !send_console_text(input,"exit\rexit\r"))
        {TerminateProcess(child.hProcess,0x53504354u);CloseHandle(child.hThread);CloseHandle(child.hProcess);return 25;}
    status=WaitForSingleObject(child.hProcess,RUN_TIMEOUT_MS);
    if(status==WAIT_OBJECT_0)GetExitCodeProcess(child.hProcess,&exit_code);
    if(status==WAIT_TIMEOUT) {
        TerminateProcess(child.hProcess,0x53504354u);
        WaitForSingleObject(child.hProcess,1000u);
    }
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    return status!=WAIT_OBJECT_0 ? 26 : exit_code ? 27 : 0;
}
int main(int argc, char **argv)
{
    typedef struct direct_case { const char *label; const char *command; const char *expected; } direct_case;
    static const direct_case cases[] = {
        { "ver", "ver", "Microsoft Windows" },
        { "mem", "mem", "655360 bytes total conventional memory" },
        { "command/c ver", "command/c ver", "MS-DOS Version 5.00.500" },
        { "command/c", "command/c", NULL },
        { "command/c command /c ver", "command/c command /c ver", "MS-DOS Version 5.00.500" },
        { "cmd/c ver", "cmd/c ver", "Microsoft Windows" }
    };
    SECURITY_ATTRIBUTES inherit = { sizeof(inherit), NULL, TRUE }; HANDLE input, output; char screen[SCREEN_BYTES], directory[MAX_PATH]; char *separator; FILE *report; DWORD index; int results[ARRAYSIZE(cases)],interactive_result;
    if (argc != 3) return 64;
    if (GetFullPathNameA(argv[1], sizeof(directory), directory, NULL) == 0u || (separator = strrchr(directory, '\\')) == NULL) return 65;
    *separator = '\0';
    input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit, OPEN_EXISTING, 0, NULL);
    if (input == INVALID_HANDLE_VALUE) { if (!AllocConsole()) return 65; input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit, OPEN_EXISTING, 0, NULL); }
    output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit, OPEN_EXISTING, 0, NULL);
    if (input == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE) return 66;
    for (index = 0u; index < ARRAYSIZE(cases); ++index)
        results[index] = run_case(argv[1], directory, cases[index].command,
                                  cases[index].expected, input, output, screen,
                                  sizeof(screen));
    interactive_result=run_interactive_nested_command(argv[1],directory,input,
        output,screen,sizeof(screen));
    if (fopen_s(&report, argv[2], "wb") == 0 && report != NULL) {
        fprintf(report, "container=clean-console-no-mvdm-diagnostics\n");
        for (index = 0u; index < ARRAYSIZE(cases); ++index)
            fprintf(report, "case=%s\nresult=%d\n", cases[index].label, results[index]);
        fprintf(report,"case=interactive-command-then-ver\nresult=%d\n",interactive_result);
        fclose(report);
    }
    CloseHandle(input); CloseHandle(output);
    for (index = 0u; index < ARRAYSIZE(cases); ++index) if (results[index] != 0) return results[index];
    return interactive_result;
}
