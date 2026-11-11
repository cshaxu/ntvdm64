/* Native protocol probe for original dbg.c module notices.  Production now
 * selects the byte-exact original body; this fixture independently verifies
 * the documented public event layout with a standard Win32 debugger. */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vdmdbg.h>

static void send_note(WORD type, SEGMENT_NOTE *note)
{
    DWORD parameters[4] = { (DWORD)type, 0u, (DWORD)(ULONG_PTR)note, 0u };

    RaiseException(STATUS_VDM_EVENT, 0, 4, (const ULONG_PTR *)parameters);
}

static int child(void)
{
    SEGMENT_NOTE note;

    ZeroMemory(&note, sizeof(note));
    note.Segment = 0x1234u;
    note.Length = 0x5678ul;
    strcpy_s(note.Module, sizeof(note.Module), "MEMORYTOO");
    strcpy_s(note.FileName, sizeof(note.FileName), "O:\\winnt\\MEM.EXE");
    send_note(DBG_MODLOAD, &note);
    ZeroMemory(&note, sizeof(note));
    note.Selector1 = 0x1234u;
    note.Selector2 = 0x2345u;
    strcpy_s(note.Module, sizeof(note.Module), "MEM");
    strcpy_s(note.FileName, sizeof(note.FileName), "O:\\winnt\\MEM.EXE");
    send_note(DBG_SEGMOVE, &note);
    ZeroMemory(&note, sizeof(note));
    strcpy_s(note.Module, sizeof(note.Module), "MEM");
    strcpy_s(note.FileName, sizeof(note.FileName), "O:\\winnt\\MEM.EXE");
    send_note(DBG_MODFREE, &note);
    puts("S43_DEBUGGER_MODULE_CHILD_CONTINUED");
    return 76;
}

static int expected_note(WORD type, const SEGMENT_NOTE *note)
{
    const char *module_name = (type == DBG_MODLOAD) ? "MEMORYTOO" : "MEM";

    if (lstrcmpA(note->Module, module_name) != 0 ||
        lstrcmpA(note->FileName, "O:\\winnt\\MEM.EXE") != 0)
        return 0;
    if (type == DBG_MODLOAD)
        return note->Segment == 0x1234u && note->Length == 0x5678ul;
    if (type == DBG_SEGMOVE)
        return note->Selector1 == 0x1234u && note->Selector2 == 0x2345u;
    return type == DBG_MODFREE;
}

int main(int argc, char **argv)
{
    WCHAR image[MAX_PATH], command[(MAX_PATH * 2) + 32];
    STARTUPINFOW startup;
    PROCESS_INFORMATION process;
    DEBUG_EVENT event;
    HANDLE child_process = NULL;
    DWORD expected[] = { DBG_MODLOAD, DBG_SEGMOVE, DBG_MODFREE };
    DWORD seen = 0, exit_code = 0;

    if (argc == 2 && lstrcmpA(argv[1], "--child") == 0)
        return child();
    if (argc != 1 || !GetModuleFileNameW(NULL, image, MAX_PATH) ||
        wsprintfW(command, L"\"%s\" --child", image) == 0) {
        puts("FAIL setup");
        return 1;
    }
    ZeroMemory(&startup, sizeof(startup));
    startup.cb = sizeof(startup);
    ZeroMemory(&process, sizeof(process));
    if (!CreateProcessW(image, command, NULL, NULL, FALSE,
            DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &startup, &process)) {
        puts("FAIL CreateProcess");
        return 1;
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);

    for (;;) {
        DWORD continue_status = DBG_CONTINUE;

        if (!WaitForDebugEvent(&event, 10000)) {
            puts("FAIL WaitForDebugEvent");
            return 1;
        }
        if (event.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT) {
            child_process = event.u.CreateProcessInfo.hProcess;
            if (event.u.CreateProcessInfo.hFile != NULL)
                CloseHandle(event.u.CreateProcessInfo.hFile);
        } else if (event.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT) {
            if (event.u.LoadDll.hFile != NULL)
                CloseHandle(event.u.LoadDll.hFile);
        } else if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
            event.u.Exception.ExceptionRecord.ExceptionCode == STATUS_VDM_EVENT) {
            EXCEPTION_RECORD *record = &event.u.Exception.ExceptionRecord;
            SEGMENT_NOTE note;
            SIZE_T transferred;

            if (child_process == NULL || seen >= 3u ||
                record->NumberParameters != 4 ||
                record->ExceptionInformation[0] != expected[seen] ||
                record->ExceptionInformation[1] != 0 ||
                record->ExceptionInformation[3] != 0 ||
                !ReadProcessMemory(child_process,
                    (LPCVOID)record->ExceptionInformation[2], &note,
                    sizeof(note), &transferred) || transferred != sizeof(note) ||
                !expected_note((WORD)expected[seen], &note)) {
                puts("FAIL module event layout");
                return 1;
            }
            ++seen;
        } else if (event.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT) {
            exit_code = event.u.ExitProcess.dwExitCode;
            if (!ContinueDebugEvent(event.dwProcessId, event.dwThreadId,
                    continue_status)) {
                puts("FAIL ContinueDebugEvent");
                return 1;
            }
            break;
        }
        if (!ContinueDebugEvent(event.dwProcessId, event.dwThreadId,
                continue_status)) {
            puts("FAIL ContinueDebugEvent");
            return 1;
        }
    }
    if (child_process != NULL)
        CloseHandle(child_process);
    if (seen != 3u || exit_code != 76u) {
        puts("FAIL module events or exit");
        return 1;
    }
    puts("S43_DEBUGGER_MODULE_EVENT_TRANSPORT_OK");
    return 0;
}
