#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

static DWORD game_pid;
static HWND game_window;
static BOOL CALLBACK find_window(HWND window,LPARAM unused)
{
    DWORD pid=0;(void)unused;
    GetWindowThreadProcessId(window,&pid);
    if(pid==game_pid && !GetWindow(window,GW_OWNER))game_window=window;
    return TRUE;
}
static DWORD find_game(const WCHAR *path)
{
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    PROCESSENTRY32W entry={sizeof(entry)};DWORD found=0;
    if(snapshot==INVALID_HANDLE_VALUE)return 0;
    if(Process32FirstW(snapshot,&entry))do {
        HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,entry.th32ProcessID);
        WCHAR image[MAX_PATH];DWORD size=MAX_PATH;
        if(process){
            if(QueryFullProcessImageNameW(process,0,image,&size) && !_wcsicmp(image,path))found=entry.th32ProcessID;
            CloseHandle(process);
        }
    }while(!found && Process32NextW(snapshot,&entry));
    CloseHandle(snapshot);return found;
}
static int exercise(WCHAR **argv)
{
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION child={0};
    WCHAR command[2048];DWORD code=99;ULONGLONG until;
    INPUT_RECORD keys[2]={0};DWORD written=0;HANDLE input;
    HWND carrier;FILE *report;BOOL hidden;
    WCHAR *separator;
    for(separator=argv[3];*separator;++separator)if(*separator==L'/')*separator=L'\\';
    swprintf_s(command,2048,L"\"%s\" \"%s\"",argv[2],argv[3]);
    if(!CreateProcessW(argv[2],command,NULL,NULL,FALSE,0,NULL,NULL,&startup,&child))return 2;
    CloseHandle(child.hThread);
    until=GetTickCount64()+15000;
    do {
        game_pid=find_game(argv[3]);
        if(game_pid)EnumWindows(find_window,0);
        if(game_window)break;
        Sleep(50);
    }while(GetTickCount64()<until);
    if(!game_window){CloseHandle(child.hProcess);return 3;}
    /* Capture the game's actual shared carrier, never infer it by title. */
    FreeConsole();
    if(!AttachConsole(game_pid)){CloseHandle(child.hProcess);return 4;}
    carrier=GetConsoleWindow();
    Sleep(1000);
    /* MySMB selects text mode when it inherits a Console. Exercise its real
     * Console input rather than pretending the hidden GUI is the presenter. */
    input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    keys[0].EventType=KEY_EVENT;keys[0].Event.KeyEvent.bKeyDown=TRUE;
    keys[0].Event.KeyEvent.wRepeatCount=1;keys[0].Event.KeyEvent.wVirtualKeyCode=VK_ESCAPE;
    keys[0].Event.KeyEvent.wVirtualScanCode=1;keys[0].Event.KeyEvent.uChar.UnicodeChar=27;
    keys[1]=keys[0];keys[1].Event.KeyEvent.bKeyDown=FALSE;
    if(input==INVALID_HANDLE_VALUE || !WriteConsoleInputW(input,keys,2,&written) || written!=2)return 5;
    CloseHandle(input);FreeConsole();
    if(WaitForSingleObject(child.hProcess,15000)!=WAIT_OBJECT_0)return 6;
    GetExitCodeProcess(child.hProcess,&code);CloseHandle(child.hProcess);
    hidden=carrier && IsWindow(carrier) && !IsWindowVisible(carrier);
    if(_wfopen_s(&report,argv[4],L"w"))return 7;
    fprintf(report,"game-pid=%lu game-window=%p carrier=%p exit=%lu hidden=%d\n",game_pid,game_window,carrier,code,hidden);
    fclose(report);
    return code || !hidden ? 8 : 0;
}
int wmain(int argc,WCHAR **argv)
{
    WCHAR name[80],image[MAX_PATH],command[2048];HDESK desktop;
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION child={0};DWORD code=99;
    if(argc==5 && !wcscmp(argv[1],L"child"))return exercise(argv);
    if(argc!=4)return 1;
    swprintf_s(name,80,L"NTVWM-Game-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);
    if(!desktop || !GetModuleFileNameW(NULL,image,MAX_PATH))return 9;
    swprintf_s(command,2048,L"\"%s\" child \"%s\" \"%s\" \"%s\"",image,argv[1],argv[2],argv[3]);
    startup.lpDesktop=name;startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
    if(!CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&child))return 10;
    CloseHandle(child.hThread);
    if(WaitForSingleObject(child.hProcess,45000)==WAIT_OBJECT_0)GetExitCodeProcess(child.hProcess,&code);
    else {TerminateProcess(child.hProcess,99);WaitForSingleObject(child.hProcess,5000);}
    CloseHandle(child.hProcess);CloseDesktop(desktop);return (int)code;
}
