#include "ntw16s-dll/ntw16s.h"
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

void win16_style_gallery_run(void);

int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR command,int show)
{
    (void)instance;(void)previous;(void)command;(void)show;
    if(Ntw16sGetAbiVersion()!=NTW16S_ABI_VERSION){
        MessageBoxW(NULL,L"The NTW16S DLL has an incompatible interface version.",
            L"W16Style",MB_OK|MB_ICONERROR);
        return ERROR_REVISION_MISMATCH;
    }
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    win16_style_gallery_run();
    return 0;
}
