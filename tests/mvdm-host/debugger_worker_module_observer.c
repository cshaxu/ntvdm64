/* S43 production witness: debug run16 so its worker inherits DebugPort. */
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>
#include <vdmdbg.h>

static BOOL is_worker(HANDLE process)
{
    WCHAR path[MAX_PATH];
    WCHAR *name;
    DWORD bytes = ARRAYSIZE(path);

    if (!QueryFullProcessImageNameW(process, 0, path, &bytes)) return FALSE;
    name = wcsrchr(path, L'\\');
    return name != NULL && lstrcmpiW(name + 1, L"ntvdm.exe") == 0;
}

static void remove_probe(const wchar_t *image, const wchar_t *directory)
{
    DeleteFileW(image);
    RemoveDirectoryW(directory);
}

static void report_error(const char *where, DWORD error)
{
    CHAR text[128];
    DWORD written;

    wsprintfA(text, "FAIL %s error=%lu\r\n", where, error);
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, (DWORD)lstrlenA(text), &written, NULL);
}

static void observer_trace(DWORD event, DWORD process, DWORD thread, DWORD error)
{
    CHAR path[MAX_PATH], text[128];
    DWORD bytes, written;
    HANDLE file;

    if (!GetEnvironmentVariableA("S43_OBSERVER_TRACE_PATH", path, sizeof(path)))
        return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    bytes = (DWORD)wsprintfA(text, "event=%lu pid=%lu tid=%lu error=%lu\r\n",
        event, process, thread, error);
    WriteFile(file, text, bytes, &written, NULL);
    CloseHandle(file);
}

static void observer_trace_load(DWORD process, LPCVOID base, HANDLE image)
{
    CHAR path[MAX_PATH], module[MAX_PATH], text[MAX_PATH + 96];
    DWORD bytes, written;
    HANDLE file;

    if (!GetEnvironmentVariableA("S43_OBSERVER_TRACE_PATH", path, sizeof(path)))
        return;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE)
        return;
    if (image && GetFinalPathNameByHandleA(image,module,sizeof(module),FILE_NAME_NORMALIZED))
        bytes=(DWORD)wsprintfA(text,"load pid=%lu base=%08lX path=%s\r\n",
            process,(unsigned long)(ULONG_PTR)base,module);
    else
        bytes=(DWORD)wsprintfA(text,"load pid=%lu base=%08lX path=<unavailable>\r\n",
            process,(unsigned long)(ULONG_PTR)base);
    WriteFile(file,text,bytes,&written,NULL);
    CloseHandle(file);
}

static void observer_trace_address_module(DWORD process, LPCVOID address, const char *kind)
{
    CHAR path[MAX_PATH], module[MAX_PATH], text[MAX_PATH + 96];
    DWORD bytes, written;
    HANDLE file, snapshot;
    MODULEENTRY32 module_entry;
    BOOL found=FALSE;

    if (!GetEnvironmentVariableA("S43_OBSERVER_TRACE_PATH", path, sizeof(path)))
        return;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE)
        return;
    snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,process);
    ZeroMemory(&module_entry,sizeof(module_entry));
    module_entry.dwSize=sizeof(module_entry);
    if (snapshot!=INVALID_HANDLE_VALUE && Module32First(snapshot,&module_entry)) do {
        ULONG_PTR start=(ULONG_PTR)module_entry.modBaseAddr;
        ULONG_PTR end=start+(ULONG_PTR)module_entry.modBaseSize;
        ULONG_PTR value=(ULONG_PTR)address;
        if (value>=start && value<end) {
            lstrcpynA(module,module_entry.szExePath,sizeof(module));
            found=TRUE;
            break;
        }
    } while(Module32Next(snapshot,&module_entry));
    if (found)
        bytes=(DWORD)wsprintfA(text,"%s-module pid=%lu address=%08lX path=%s\r\n",
            kind,process,(unsigned long)(ULONG_PTR)address,module);
    else
        bytes=(DWORD)wsprintfA(text,"%s-module pid=%lu address=%08lX path=<unavailable> error=%lu\r\n",
            kind,process,(unsigned long)(ULONG_PTR)address,GetLastError());
    if (snapshot!=INVALID_HANDLE_VALUE) CloseHandle(snapshot);
    WriteFile(file,text,bytes,&written,NULL);
    CloseHandle(file);
}

static void observer_trace_exception_caller(DWORD process, DWORD thread)
{
    CONTEXT context;
    HANDLE target, source;
    DWORD words[12];
    CHAR path[MAX_PATH], text[384];
    DWORD written;
    HANDLE file;

    source=OpenThread(THREAD_GET_CONTEXT,FALSE,thread);
    ZeroMemory(&context,sizeof(context));
    context.ContextFlags=CONTEXT_CONTROL;
    target=OpenProcess(PROCESS_VM_READ,FALSE,process);
    if (source && target && GetThreadContext(source,&context) &&
        ReadProcessMemory(target,(LPCVOID)(ULONG_PTR)context.Esp,
            words,sizeof(words),NULL)) {
        if (GetEnvironmentVariableA("S43_OBSERVER_TRACE_PATH",path,sizeof(path))) {
            file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,
                NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
            if(file!=INVALID_HANDLE_VALUE) {
                wsprintfA(text,"stack pid=%lu eip=%08lX esp=%08lX words=%08lX,%08lX,%08lX,%08lX,%08lX,%08lX,%08lX,%08lX\r\n",
                    process,context.Eip,context.Esp,words[0],words[1],words[2],words[3],
                    words[4],words[5],words[6],words[7]);
                WriteFile(file,text,(DWORD)lstrlenA(text),&written,NULL);
                CloseHandle(file);
            }
        }
    }
    if (target) CloseHandle(target);
    if (source) CloseHandle(source);
}

static void report_create(DWORD process_id, HANDLE process)
{
    WCHAR path[MAX_PATH];
    CHAR narrow[MAX_PATH * 3], text[MAX_PATH * 3 + 48];
    DWORD bytes = ARRAYSIZE(path), written;

    if (!QueryFullProcessImageNameW(process, 0, path, &bytes))
        lstrcpyW(path, L"<unavailable>");
    WideCharToMultiByte(CP_UTF8, 0, path, -1, narrow, sizeof(narrow), NULL, NULL);
    wsprintfA(text, "S43_CREATE pid=%lu %s\r\n", process_id, narrow);
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, (DWORD)lstrlenA(text), &written, NULL);
}

static BOOL set1_key_for_ascii(char character, WORD *virtual_key, WORD *scan_code)
{
    static const BYTE lower[26] = {
        0x1e,0x30,0x2e,0x20,0x12,0x21,0x22,0x23,0x17,0x24,0x25,0x26,
        0x32,0x31,0x18,0x19,0x10,0x13,0x1f,0x14,0x16,0x2f,0x11,0x2d,
        0x15,0x2c
    };

    if (character >= 'a' && character <= 'z') {
        *virtual_key=(WORD)(character-'a'+'A');
        *scan_code=lower[character-'a'];
        return TRUE;
    }
    if (character == '\r') {
        *virtual_key=VK_RETURN;
        *scan_code=0x1c;
        return TRUE;
    }
    return FALSE;
}

static BOOL send_console_text(DWORD worker, const char *text)
{
    HANDLE input, output;
    DWORD started=GetTickCount();
    char screen[4097];
    COORD origin={0,0};
    const char *cursor;

    (void)FreeConsole();
    if (!AttachConsole(worker)) return FALSE;
    input=CreateFileA("CONIN$",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);
    output=CreateFileA("CONOUT$",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);
    if (input==INVALID_HANDLE_VALUE || output==INVALID_HANDLE_VALUE) {
        if (input!=INVALID_HANDLE_VALUE) CloseHandle(input);
        if (output!=INVALID_HANDLE_VALUE) CloseHandle(output);
        return FALSE;
    }
    for (;;) {
        DWORD read=0;
        if (ReadConsoleOutputCharacterA(output,screen,4096,origin,&read)) {
            screen[read < sizeof(screen) ? read : sizeof(screen)-1]='\0';
            if (strchr(screen,'>')!=NULL) break;
        }
        if ((DWORD)(GetTickCount()-started)>=10000u) {
            CloseHandle(input); CloseHandle(output);
            SetLastError(ERROR_TIMEOUT);
            return FALSE;
        }
        Sleep(50u);
    }
    for (cursor=text;*cursor;++cursor) {
        INPUT_RECORD records[2];
        DWORD written;
        WORD key,scan;
        if (!set1_key_for_ascii(*cursor,&key,&scan)) {
            CloseHandle(input); CloseHandle(output); return FALSE;
        }
        ZeroMemory(records,sizeof(records));
        records[0].EventType=records[1].EventType=KEY_EVENT;
        records[0].Event.KeyEvent.bKeyDown=TRUE;
        records[0].Event.KeyEvent.wRepeatCount=1;
        records[0].Event.KeyEvent.wVirtualKeyCode=key;
        records[0].Event.KeyEvent.wVirtualScanCode=scan;
        records[0].Event.KeyEvent.uChar.AsciiChar=*cursor;
        records[0].Event.KeyEvent.dwControlKeyState=NUMLOCK_ON;
        records[1].Event.KeyEvent=records[0].Event.KeyEvent;
        records[1].Event.KeyEvent.bKeyDown=FALSE;
        if (!WriteConsoleInputA(input,records,ARRAYSIZE(records),&written) ||
            written!=ARRAYSIZE(records)) {
            CloseHandle(input); CloseHandle(output); return FALSE;
        }
        Sleep(25u);
    }
    CloseHandle(input); CloseHandle(output);
    return TRUE;
}

int wmain(int argc, wchar_t **argv)
{
    const wchar_t *root = argc == 2 ? argv[1] : L"O:\\winnt";
    wchar_t launcher[MAX_PATH], source[MAX_PATH], test_root[MAX_PATH], directory[MAX_PATH];
    wchar_t probe[MAX_PATH], command[MAX_PATH * 3];
    STARTUPINFOW startup;
    PROCESS_INFORMATION parent;
    DEBUG_EVENT event;
    ULONGLONG deadline;
    CHAR oem_probe[8];
    DWORD worker = 0, events = 0;
    BOOL saw = FALSE, saw_free = FALSE, saw_segload = FALSE;
    BOOL input_sent = FALSE, input_pending = FALSE;

    if (!wsprintfW(launcher,L"%s\\run16.exe",root) ||
        !wsprintfW(source,L"%s\\MEM.EXE",root) ||
        !wsprintfW(test_root,L"%s\\tests",root) ||
        !wsprintfW(directory,L"%s\\D%04X\x00a3",test_root,GetCurrentProcessId() & 0xffffu) ||
        !wsprintfW(probe,L"%s\\MEM.EXE",directory) ||
        !wsprintfW(command,L"\"%s\" \"%s\"",launcher,probe) ||
        !CharToOemW(L"\x00a3",oem_probe)) return 2;
    if ((!CreateDirectoryW(test_root,NULL) && GetLastError()!=ERROR_ALREADY_EXISTS) ||
        !CreateDirectoryW(directory,NULL) || !CopyFileW(source,probe,TRUE)) {
        report_error("CreateDirectory/CopyFile", GetLastError());
        remove_probe(probe,directory);
        return 3;
    }

    ZeroMemory(&startup,sizeof(startup));
    startup.cb=sizeof(startup);
    startup.dwFlags=STARTF_USESTDHANDLES;
    startup.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError=GetStdHandle(STD_ERROR_HANDLE);
    ZeroMemory(&parent,sizeof(parent));
    if (!CreateProcessW(launcher,command,NULL,NULL,TRUE,
            DEBUG_PROCESS,NULL,root,&startup,&parent)) {
        report_error("CreateProcess DEBUG_PROCESS", GetLastError());
        remove_probe(probe,directory);
        return 4;
    }
    CloseHandle(parent.hThread);
    CloseHandle(parent.hProcess);
    deadline=GetTickCount64()+30000u;

    while(GetTickCount64()<deadline) {
        DWORD disposition=DBG_CONTINUE;
        if(!WaitForDebugEvent(&event,250)) continue;
        observer_trace(event.dwDebugEventCode, event.dwProcessId, event.dwThreadId, 0);
        if (event.dwDebugEventCode==EXCEPTION_DEBUG_EVENT) {
            observer_trace(200u,event.dwProcessId,event.dwThreadId,
                (DWORD)event.u.Exception.ExceptionRecord.ExceptionCode);
            observer_trace(201u,event.dwProcessId,event.dwThreadId,
                (DWORD)(ULONG_PTR)event.u.Exception.ExceptionRecord.ExceptionAddress);
            observer_trace_address_module(event.dwProcessId,
                event.u.Exception.ExceptionRecord.ExceptionAddress,"exception");
            observer_trace_exception_caller(event.dwProcessId,event.dwThreadId);
            if (event.u.Exception.ExceptionRecord.NumberParameters)
                observer_trace(202u,event.dwProcessId,event.dwThreadId,
                    (DWORD)event.u.Exception.ExceptionRecord.ExceptionInformation[0]);
            if (event.u.Exception.ExceptionRecord.ExceptionCode==0xE06D7363u &&
                event.u.Exception.ExceptionRecord.NumberParameters>=3u)
                observer_trace_address_module(event.dwProcessId,
                    (LPCVOID)event.u.Exception.ExceptionRecord.ExceptionInformation[2],
                    "cxx-throwinfo");
        }
        if(event.dwDebugEventCode==CREATE_PROCESS_DEBUG_EVENT) {
            report_create(event.dwProcessId,event.u.CreateProcessInfo.hProcess);
            if (!worker && is_worker(event.u.CreateProcessInfo.hProcess)) worker=event.dwProcessId;
            if(event.u.CreateProcessInfo.hFile) CloseHandle(event.u.CreateProcessInfo.hFile);
        }
        if(event.dwDebugEventCode==LOAD_DLL_DEBUG_EVENT) {
            observer_trace_load(event.dwProcessId,event.u.LoadDll.lpBaseOfDll,
                event.u.LoadDll.hFile);
            if (event.u.LoadDll.hFile) CloseHandle(event.u.LoadDll.hFile);
        }
        if(event.dwDebugEventCode==EXCEPTION_DEBUG_EVENT &&
            event.dwProcessId==worker &&
            event.u.Exception.ExceptionRecord.ExceptionCode==STATUS_VDM_EVENT) {
            EXCEPTION_RECORD *record=&event.u.Exception.ExceptionRecord;
            SEGMENT_NOTE note;
            SIZE_T copied;
            HANDLE target=OpenProcess(PROCESS_VM_READ,FALSE,event.dwProcessId);
            ++events;
            if(target && record->NumberParameters==4 &&
                (record->ExceptionInformation[0]==DBG_SEGLOAD ||
                 record->ExceptionInformation[0]==DBG_MODLOAD ||
                 record->ExceptionInformation[0]==DBG_MODFREE) &&
                ReadProcessMemory(target,(LPCVOID)record->ExceptionInformation[2],
                    &note,sizeof(note),&copied) && copied==sizeof(note)) {
                note.Module[sizeof(note.Module)-1]='\0';
                note.FileName[sizeof(note.FileName)-1]='\0';
                { CHAR text[352]; DWORD written;
                  wsprintfA(text,"S43_EVENT kind=%lu module=%s path=%s\r\n",
                      (DWORD)record->ExceptionInformation[0],note.Module,note.FileName);
                  WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),text,(DWORD)lstrlenA(text),&written,NULL); }
                if (record->ExceptionInformation[0]==DBG_SEGLOAD)
                    saw_segload=TRUE;
                /* A direct OEM-named DOS image avoids conflating the
                 * debugger carrier with Terminal's interactive input stack. */
                if (!lstrcmpA(note.Module,"MEM") &&
                    strstr(note.FileName,oem_probe)!=NULL) {
                    if (record->ExceptionInformation[0]==DBG_MODLOAD) saw=TRUE;
                    else saw_free=TRUE;
                }
            }
            if(target) CloseHandle(target);
        }
        /* STATUS_VDM_EVENT is the original debugger transport and must be
         * consumed.  Every unrelated first-chance exception belongs to the
         * worker's own handlers; claiming it here corrupts the real run. */
        if(event.dwDebugEventCode==EXCEPTION_DEBUG_EVENT &&
            event.u.Exception.ExceptionRecord.ExceptionCode!=STATUS_VDM_EVENT &&
            event.u.Exception.ExceptionRecord.ExceptionCode!=EXCEPTION_BREAKPOINT &&
            event.u.Exception.ExceptionRecord.ExceptionCode!=EXCEPTION_SINGLE_STEP)
            disposition=DBG_EXCEPTION_NOT_HANDLED;
        if(!ContinueDebugEvent(event.dwProcessId,event.dwThreadId,disposition)) {
            observer_trace(event.dwDebugEventCode, event.dwProcessId, event.dwThreadId,
                GetLastError());
            break;
        }
        /* The original DEM owns the paired module notifications.  Do not
         * make their verification depend on the host DebugPort's separate
         * detach/termination policy. */
        if (saw && saw_free && saw_segload) break;
        if (input_pending) {
            observer_trace(100u,worker,0u,0u);
            if (!send_console_text(worker,"mem\rexit\r")) {
                observer_trace(101u,worker,0u,GetLastError());
                report_error("WriteConsoleInput",GetLastError());
            } else {
                observer_trace(102u,worker,0u,0u);
                input_sent=TRUE;
            }
            input_pending=FALSE;
        }
    }
    remove_probe(probe,directory);
    if(saw && saw_free && saw_segload){puts("S43_DEBUGGER_REAL_WORKER_OEM_MODULE_PAIR_AND_SEGLOAD_OK");return 0;}
    { CHAR text[112]; DWORD written;
      wsprintfA(text,"FAIL incomplete debugger events (worker=%lu events=%lu load=%u free=%u segload=%u)\r\n",worker,events,saw,saw_free,saw_segload);
      WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),text,(DWORD)lstrlenA(text),&written,NULL); }
    return 6;
}
