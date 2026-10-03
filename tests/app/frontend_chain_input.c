/* Test-only UI-side driver. Never activates a window or reads the owner desktop. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static BOOL visible(HANDLE output, PCWSTR marker)
{
    CONSOLE_SCREEN_BUFFER_INFO info; WCHAR row[1025]; DWORD read; SHORT y;
    if (!GetConsoleScreenBufferInfo(output, &info) || info.dwSize.X > 1024) return FALSE;
    for (y = info.srWindow.Top; y <= info.srWindow.Bottom; ++y) {
        COORD start = {0, y};
        if (!ReadConsoleOutputCharacterW(output, row, info.dwSize.X, start, &read)) return FALSE;
        row[read] = 0;
        if (wcsstr(row, marker)) return TRUE;
    }
    return FALSE;
}

static BOOL await_text(HANDLE output, PCWSTR marker)
{
    ULONGLONG deadline = GetTickCount64() + 10000;
    do { if (visible(output, marker)) return TRUE; Sleep(10); } while (GetTickCount64() < deadline);
    return FALSE;
}

static void dump_screen(HANDLE output,FILE *report)
{
    CONSOLE_SCREEN_BUFFER_INFO info;WCHAR row[1025];SHORT y;DWORD read,mode;
    if(!GetConsoleScreenBufferInfo(output,&info) || info.dwSize.X>1024) {
        fprintf(report,"capture-error=%lu\n",GetLastError());return;
    }
    fprintf(report,"buffer=%d,%d viewport=%d,%d,%d,%d cursor=%d,%d\n",
        info.dwSize.X,info.dwSize.Y,info.srWindow.Left,info.srWindow.Top,
        info.srWindow.Right,info.srWindow.Bottom,info.dwCursorPosition.X,info.dwCursorPosition.Y);
    if(GetConsoleMode(output,&mode))fprintf(report,"output-mode=0x%08lx\n",mode);
    for(y=info.srWindow.Top;y<=info.srWindow.Bottom;++y) {
        COORD origin={0,y};
        if(!ReadConsoleOutputCharacterW(output,row,info.dwSize.X,origin,&read))break;
        while(read && row[read-1]==L' ')--read;
        row[read]=0;if(read)fprintf(report,"[%d] %ls\n",y,row);
    }
}

static BOOL send_text(HANDLE input, PCWSTR text)
{
    for (; *text; ++text) {
        INPUT_RECORD records[2] = {0}; DWORD written;
        records[0].EventType = records[1].EventType = KEY_EVENT;
        records[0].Event.KeyEvent.bKeyDown = TRUE;
        records[0].Event.KeyEvent.wRepeatCount = records[1].Event.KeyEvent.wRepeatCount = 1;
        records[0].Event.KeyEvent.uChar.UnicodeChar = records[1].Event.KeyEvent.uChar.UnicodeChar = *text;
        records[0].Event.KeyEvent.wVirtualKeyCode = records[1].Event.KeyEvent.wVirtualKeyCode =
            *text == L'\r' ? VK_RETURN : LOBYTE(VkKeyScanW(*text));
        records[0].Event.KeyEvent.wVirtualScanCode = records[1].Event.KeyEvent.wVirtualScanCode =
            (WORD)MapVirtualKeyW(records[0].Event.KeyEvent.wVirtualKeyCode, MAPVK_VK_TO_VSC);
        if (!WriteConsoleInputW(input, records, 2, &written) || written != 2) return FALSE;
    }
    return TRUE;
}

/* Console-API fixture only: compare a viewport containing the absolute cursor
 * with the mixed-origin state observed at the DOS/native seed boundary. The
 * buffers are never activated and the inherited screen is not modified. */
static int viewport_write_test(PCWSTR path,BOOL active)
{
    FILE *report=NULL; int variant,result=0;
    HANDLE original=GetStdHandle(STD_OUTPUT_HANDLE);
    if(_wfopen_s(&report,path,L"wx"))return 82;
    for(variant=0;variant<4;++variant) {
        HANDLE screen=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
        COORD extent={120,100},cursor={0,29};
        SMALL_RECT tiny={0,0,0,0},view={0,0,79,27};
        CONSOLE_SCREEN_BUFFER_INFO info; DWORD written=0;
        WCHAR marker[]=L"VIEWPORT-WRITE\r\n";
        if(screen==INVALID_HANDLE_VALUE){result=84;break;}
        if(variant&1){view.Top=2;view.Bottom=29;}
        if((active && !SetConsoleActiveScreenBuffer(screen)) ||
            !SetConsoleWindowInfo(screen,TRUE,&tiny) ||
            !SetConsoleScreenBufferSize(screen,extent) ||
            !SetConsoleMode(screen,ENABLE_PROCESSED_OUTPUT|ENABLE_WRAP_AT_EOL_OUTPUT|
                (variant>=2 ? ENABLE_VIRTUAL_TERMINAL_PROCESSING : 0)) ||
            !SetConsoleCursorPosition(screen,cursor) ||
            !SetConsoleWindowInfo(screen,TRUE,&view) ||
            !GetConsoleScreenBufferInfo(screen,&info) ||
            memcmp(&info.srWindow,&view,sizeof(view)) ||
            info.dwCursorPosition.Y!=29) {
            fprintf(report,"setup-error=%lu variant=%d\n",GetLastError(),variant);
            if(active)SetConsoleActiveScreenBuffer(original);
            CloseHandle(screen);result=84;break;
        }
        fprintf(report,"variant=%d (%s,%s) before\n",variant,
            (variant&1)?"cursor-in-view":"mixed-origin",variant>=2?"VT":"classic");
        dump_screen(screen,report);
        if(!WriteConsoleW(screen,marker,ARRAYSIZE(marker)-1,&written,NULL) ||
            written!=ARRAYSIZE(marker)-1 || !GetConsoleScreenBufferInfo(screen,&info)) {
            if(active)SetConsoleActiveScreenBuffer(original);
            CloseHandle(screen);result=83;break;
        }
        fprintf(report,"variant=%d after\n",variant);
        dump_screen(screen,report);
        /* Normal processed CRLF advances one buffer row. The mixed-origin
         * case is a diagnostic observation, not a product acceptance pass. */
        if(variant!=2 && (info.dwCursorPosition.X!=0 || info.dwCursorPosition.Y!=30))result=85;
        if(active && !SetConsoleActiveScreenBuffer(original))result=86;
        CloseHandle(screen);
    }
    fprintf(report,"fixture-result=%d\n",result);fclose(report);return result;
}

static int selftest(PCWSTR report, BOOL missing)
{
    WCHAR image[MAX_PATH], command[2048]; char text[64] = {0}; DWORD bytes, result;
    STARTUPINFOW startup = {sizeof(startup)}; PROCESS_INFORMATION child = {0};
    if (!GetModuleFileNameW(NULL, image, ARRAYSIZE(image))) return 90;
    swprintf_s(command, ARRAYSIZE(command),
        L"\"%ls\" %lu %ls ACK-SELF W \"%ls\" \"chain\r\"",
        image, GetCurrentProcessId(), missing ? L"ABSENT-READY" : L"READY-SELF", report);
    if (!CreateProcessW(image, command, NULL, NULL, FALSE, CREATE_NO_WINDOW,
        NULL, NULL, &startup, &child)) return 91;
    CloseHandle(child.hThread);
    puts("READY-SELF"); fflush(stdout);
    if (missing) {
        BOOL rejected = WaitForSingleObject(child.hProcess, 12000) == WAIT_OBJECT_0 &&
            GetExitCodeProcess(child.hProcess, &result) && result == 1;
        CloseHandle(child.hProcess);
        if (!rejected) return 94;
        puts("PASS MISSING-PROMPT-REJECTED"); return 0;
    }
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), text, sizeof(text)-1, &bytes, NULL) ||
        bytes < 6 || memcmp(text, "chain\r", 6)) { CloseHandle(child.hProcess); return 92; }
    puts("ACK-SELF"); fflush(stdout);
    if (WaitForSingleObject(child.hProcess, 12000) != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(child.hProcess, &result) || result) { CloseHandle(child.hProcess); return 93; }
    CloseHandle(child.hProcess); puts("PASS INPUT-DRIVER-SELFTEST"); return 0;
}

static int parallel_native_launch(const WCHAR *package)
{
    WCHAR image[MAX_PATH],command[2][2*MAX_PATH];
    DWORD before,after,round,index,result=0;
    if(swprintf_s(image,ARRAYSIZE(image),L"%s\\run16.exe",package)<0)return 82;
    before=0;
    /* First CreateProcess/Console use may initialize process-local OS handles.
     * Warm up once, then require exact equality across all four measured pairs. */
    for(round=0;round<5;++round) {
        PROCESS_INFORMATION children[2]={{0}};
        for(index=0;index<2;++index) {
            STARTUPINFOW start={sizeof(start)};
            if(swprintf_s(command[index],ARRAYSIZE(command[index]),
                L"\"%s\" cmd /d /c echo S10-PARALLEL-%lu-%lu",image,round,index)<0 ||
                !CreateProcessW(image,command[index],NULL,NULL,FALSE,0,NULL,package,&start,&children[index])) {
                result=84;break;
            }
            CloseHandle(children[index].hThread);children[index].hThread=NULL;
        }
        for(index=0;index<2;++index)if(children[index].hProcess) {
            DWORD code;
            if(WaitForSingleObject(children[index].hProcess,15000)!=WAIT_OBJECT_0 ||
                !GetExitCodeProcess(children[index].hProcess,&code) || code)result=85;
            CloseHandle(children[index].hProcess);
        }
        if(result)return (int)result;
        if(!round && !GetProcessHandleCount(GetCurrentProcess(),&before))return 83;
    }
    if(!GetProcessHandleCount(GetCurrentProcess(),&after))return 86;
    printf("parallel launcher handles before=%lu after=%lu\n",before,after);
    if(after!=before)return 86;
    puts("PASS four concurrent same-Console native launcher pairs, direct exit0, zero handle delta");
    return 0;
}
int wmain(int argc, WCHAR **argv)
{
    WCHAR desktop[128]; DWORD needed; HANDLE input, output; FILE *report = NULL;
    int result = 1; BOOL dos;
    /* Read-only failure observation of an explicitly named test Console;
     * no input, activation, window manipulation or production helper role. */
    if(argc==4 && !wcscmp(argv[1],L"--snapshot")) {
        FreeConsole();
        if(!AttachConsole(wcstoul(argv[2],NULL,10)))return 81;
        output=CreateFileW(L"CONOUT$",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        if(output==INVALID_HANDLE_VALUE || _wfopen_s(&report,argv[3],L"wx"))return 82;
        dump_screen(output,report);fclose(report);CloseHandle(output);FreeConsole();return 0;
    }
    if (!GetUserObjectInformationW(GetThreadDesktop(GetCurrentThreadId()),
        UOI_NAME, desktop, sizeof(desktop), &needed) ||
        wcsncmp(desktop, L"NTVDMConsoleTest-", 17)) return 80;
    if(argc==3 && !wcscmp(argv[1],L"--viewport-write-test"))
        return viewport_write_test(argv[2],FALSE);
    if(argc==3 && !wcscmp(argv[1],L"--active-viewport-write-test"))
        return viewport_write_test(argv[2],TRUE);
    if(argc==3 && !wcscmp(argv[1],L"--parallel-native"))return parallel_native_launch(argv[2]);
    /* Read the inherited Console after a launcher returns. This is a
     * test-only snapshot: no AttachConsole, input, repaint or mode mutation. */
    if(argc==3 && !wcscmp(argv[1],L"--snapshot-current")) {
        CONSOLE_SCREEN_BUFFER_INFO info;
        output=CreateFileW(L"CONOUT$",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,OPEN_EXISTING,0,NULL);
        if(output==INVALID_HANDLE_VALUE)return 82;
        if(!GetConsoleScreenBufferInfo(output,&info) || _wfopen_s(&report,argv[2],L"wx")) {
            CloseHandle(output);return 82;
        }
        dump_screen(output,report);fclose(report);CloseHandle(output);return 0;
    }
    /* Actual native output with before/after evidence, to distinguish target
     * Console behavior from worker/frontend publication. */
    if(argc==4 && !wcscmp(argv[1],L"--output-snapshot")) {
        DWORD length=(DWORD)wcslen(argv[2]),written=0;
        output=GetStdHandle(STD_OUTPUT_HANDLE);
        if(_wfopen_s(&report,argv[3],L"wx"))return 82;
        fputs("before-output\n",report);dump_screen(output,report);
        if(!WriteConsoleW(output,argv[2],length,&written,NULL) || written!=length) {
            fclose(report);return 83;
        }
        fputs("after-marker\n",report);dump_screen(output,report);
        if(!WriteConsoleW(output,L"\r\n",2,&written,NULL) || written!=2) {
            fclose(report);return 83;
        }
        fputs("after-output\n",report);dump_screen(output,report);
        fclose(report);return 0;
    }
    if (argc == 3 && !wcscmp(argv[1], L"--selftest")) return selftest(argv[2], FALSE);
    if (argc == 3 && !wcscmp(argv[1], L"--selftest-missing")) return selftest(argv[2], TRUE);
    if (argc != 7) return 80;
    dos = !wcscmp(argv[4], L"D");
    FreeConsole();
    if (!AttachConsole(wcstoul(argv[1], NULL, 10))) return 81;
    input = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    output = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (_wfopen_s(&report, argv[5], L"w")) goto done;
    fprintf(report, "DESKTOP %ls\n", desktop); fflush(report);
    {
        HWND console_window=GetConsoleWindow(); WCHAR window_class[128]={0};
        DWORD window_pid=0,window_thread=GetWindowThreadProcessId(console_window,&window_pid);
        GetClassNameW(console_window,window_class,ARRAYSIZE(window_class));
        fprintf(report,"CONSOLE window=%p valid=%d class=%ls thread=%lu process=%lu\n",
            console_window,IsWindow(console_window),window_class,window_thread,window_pid);
        fflush(report);
    }
    if (!await_text(output, argv[2])) { fputs("FAIL no visible ready marker\n", report); dump_screen(output,report); goto done; }
    fprintf(report, "READY %ls\n", argv[2]); fflush(report);
    if (!send_text(input, dos ? L"z" : argv[6])) { fputs("FAIL input delivery\n", report); goto done; }
    if (!await_text(output, argv[3])) { fputs("FAIL no visible acknowledgement\n", report); dump_screen(output,report); goto done; }
    fprintf(report, "ACK %ls\nPASS visible-output-and-input\n", argv[3]); fflush(report);
    /* Second DOS PAUSE keeps the acknowledgement on-screen until observed. */
    if (dos && !send_text(input, L"z")) { fputs("FAIL final DOS release\n", report); goto done; }
    result = 0;
done:
    if (report) fclose(report);
    if (input != INVALID_HANDLE_VALUE) CloseHandle(input);
    if (output != INVALID_HANDLE_VALUE) CloseHandle(output);
    FreeConsole(); return result;
}
