#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static DWORD scripted_window_frontend,scripted_window_key_messages,scripted_window_enters;
static HWND target;
static BOOL CALLBACK find(HWND w,LPARAM p){DWORD id;char c[128];GetWindowThreadProcessId(w,&id);GetClassNameA(w,c,sizeof(c));if(id==scripted_window_frontend&&!strcmp(c,"LibKvmWindow"))target=w;return TRUE;}static BOOL send_window_keyboard_message(HWND window, UINT message,
    WPARAM key, LPARAM bits, DWORD control, WORD virtual_key, BOOL down)
{
    char desktop[96]; DWORD needed, pid = 0, thread;
    BYTE saved[256], state[256] = {0}; MSG pending;
    DWORD_PTR result; BOOL sent = FALSE;
    thread = GetWindowThreadProcessId(window, &pid);
    if (!thread || pid != scripted_window_frontend ||
        !GetUserObjectInformationA(GetThreadDesktop(GetCurrentThreadId()),
            UOI_NAME, desktop, sizeof(desktop), &needed) ||
        (strncmp(desktop, "NTVDMConsoleTest-", 17) &&
         !GetEnvironmentVariableA("MVDM_OBSERVER_CAF_OWNED_CONSOLE",NULL,0))) return FALSE;
    (void)PeekMessageW(&pending, NULL, 0, 0, PM_NOREMOVE);
    if (!GetKeyboardState(saved) ||
        !AttachThreadInput(GetCurrentThreadId(), thread, TRUE)) return FALSE;
    state[VK_NUMLOCK] = (control & NUMLOCK_ON) ? 1 : 0;
    state[VK_CAPITAL] = (control & CAPSLOCK_ON) ? 1 : 0;
    state[VK_SCROLL] = (control & SCROLLLOCK_ON) ? 1 : 0;
    if (control & SHIFT_PRESSED) state[VK_SHIFT] = state[VK_LSHIFT] = 0x80;
    if (control & LEFT_CTRL_PRESSED) state[VK_CONTROL] = state[VK_LCONTROL] = 0x80;
    if (control & RIGHT_CTRL_PRESSED) state[VK_CONTROL] = state[VK_RCONTROL] = 0x80;
    if (control & LEFT_ALT_PRESSED) state[VK_MENU] = state[VK_LMENU] = 0x80;
    if (control & RIGHT_ALT_PRESSED) state[VK_MENU] = state[VK_RMENU] = 0x80;
    if (virtual_key < 256 && down) state[virtual_key] |= 0x80;
    /* Explicit negative diagnostic: prove uncontrolled Caps reproduces the
     * strict echo failure before Enter; never enabled by normal matrices. */
    if (GetEnvironmentVariableA("MVDM_OBSERVER_WINDOW_CAPS_DIAGNOSTIC",NULL,0))
        state[VK_CAPITAL] = 1;
    if (SetKeyboardState(state))
        sent = SendMessageTimeoutW(window, message, key, bits,
            SMTO_ABORTIFHUNG, 3000, &result) != 0;
    if (!AttachThreadInput(GetCurrentThreadId(), thread, FALSE)) sent = FALSE;
    if (!SetKeyboardState(saved)) sent = FALSE;
    if (sent) {
        ++scripted_window_key_messages;
        if (message == WM_KEYDOWN && virtual_key == VK_RETURN && down)
            ++scripted_window_enters;
    }
    return sent;
}
int main(int argc,char **argv){if(argc!=5&&argc!=6)return 64;scripted_window_frontend=strtoul(argv[1],0,10);HANDLE owned=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,scripted_window_frontend); WCHAR image[1024];DWORD length=1024;if(!owned||!QueryFullProcessImageNameW(owned,0,image,&length)||_wcsicmp(image,L"O:\\tmp\\n403\\system32\\ntcon.exe"))return 72;CloseHandle(owned);SetEnvironmentVariableA("MVDM_OBSERVER_CAF_OWNED_CONSOLE","1");if(strcmp(argv[2],"Default")&&strncmp(argv[2],"NTVDMConsoleTest-",17))return 65;HDESK d=OpenDesktopA(argv[2],0,FALSE,GENERIC_ALL);if(!d||!SetThreadDesktop(d))return 66;EnumDesktopWindows(d,find,0);if(!target)return 67;WORD key=(WORD)strtoul(argv[3],0,0);DWORD control=strtoul(argv[4],0,0);LPARAM bits=1|((LPARAM)MapVirtualKeyW(key,0)<<16);if(control & ENHANCED_KEY) bits |= ((LPARAM)1 << 24);BOOL a=TRUE,b=TRUE;if(argc==5||strcmp(argv[5],"up"))a=send_window_keyboard_message(target,WM_KEYDOWN,key,bits,control,key,TRUE);if(argc==5||strcmp(argv[5],"down"))b=send_window_keyboard_message(target,WM_KEYUP,key,bits|0xc0000000,control,key,FALSE);printf("target=%p key=%u down=%d up=%d\n",target,key,a,b);return a&&b?0:68;}