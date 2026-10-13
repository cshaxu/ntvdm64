#include "ntw16s-dll/ntw16s.h"

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
