/* Test-only private-desktop hook. Sets keyboard state on the target UI thread
 * for key messages emitted by the owned observation process. */
#include <windows.h>
#if defined(_M_IX86)
#pragma comment(linker, "/EXPORT:caf_probe_hook=_caf_probe_hook@12")
#endif
__declspec(dllexport) LRESULT CALLBACK caf_probe_hook(int code, WPARAM wp, LPARAM lp)
{
    if (code >= 0) {
        CWPSTRUCT *msg=(CWPSTRUCT *)lp;
        if (msg->message==WM_KEYDOWN || msg->message==WM_KEYUP ||
            msg->message==WM_SYSKEYDOWN || msg->message==WM_SYSKEYUP) {
            BYTE state[256];
            if (GetKeyboardState(state)) {
                unsigned vk=(unsigned)msg->wParam;
                if (vk<256) {
                    BOOL down=msg->message==WM_KEYDOWN || msg->message==WM_SYSKEYDOWN;
                    state[vk]=(BYTE)((state[vk]&1)|(down ? 0x80 : 0));
                    if(vk=='F' && (msg->message==WM_SYSKEYDOWN || msg->message==WM_SYSKEYUP)) {
                        state[VK_CONTROL]=0x80;
                        state[VK_MENU]=0x80;
                        SetKeyboardState(state);
                    }
                }
            }
        }
    }
    return CallNextHookEx(NULL,code,wp,lp);
}
