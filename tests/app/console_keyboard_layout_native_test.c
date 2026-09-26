/* Exact historical Console ABI availability, not guest KEYB acceptance.
 * Run in an isolated Console. A test-only local thread activation is restored;
 * no layout is loaded/unloaded and no user/foreground layout is changed. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef BOOL (WINAPI *console_layout_a)(LPSTR);
typedef BOOL (WINAPI *console_layout_w)(LPWSTR);
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d error=%lu\n",__LINE__,GetLastError()); return 1; } } while (0)
int main(int argc,char **argv)
{
    console_layout_a query_a=(console_layout_a)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"GetConsoleKeyboardLayoutNameA");
    console_layout_w query_w=(console_layout_w)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"GetConsoleKeyboardLayoutNameW");
    char name[KL_NAMELENGTH+2],thread_name[KL_NAMELENGTH];
    WCHAR wide[KL_NAMELENGTH+2];
    DWORD index,error;
    BOOL result,ansi_ok,wide_ok;
    FILE *log=NULL;
    CHECK(argc==2 && fopen_s(&log,argv[1],"wb")==0 && log);
    CHECK(query_a && query_w);
    memset(name,0x55,sizeof(name));
    for(index=0;index<ARRAYSIZE(wide);++index) wide[index]=0x5555;
    SetLastError(0);ansi_ok=query_a(name);error=GetLastError();
    fprintf(log,"attached ANSI result=%d error=%lu\n",ansi_ok,error);
    SetLastError(0);wide_ok=query_w(wide);error=GetLastError();
    fprintf(log,"attached Unicode result=%d error=%lu\n",wide_ok,error);
    fflush(log);
    CHECK(name[KL_NAMELENGTH]==0x55 && wide[KL_NAMELENGTH]==0x5555);
    if(ansi_ok && wide_ok) {
        CHECK(name[KL_NAMELENGTH-1]==0 && wide[KL_NAMELENGTH-1]==0);
        for(index=0;index<KL_NAMELENGTH;++index) CHECK((WCHAR)(BYTE)name[index]==wide[index]);
        fprintf(log,"attached console=%s ANSI/Unicode/canary=pass\n",name);
    }
    CHECK(GetKeyboardLayoutNameA(thread_name));
    fprintf(log,"attached thread=%s\n",thread_name);
    {
        HWND window=GetConsoleWindow();
        DWORD process=0,thread=window ? GetWindowThreadProcessId(window,&process) : 0;
        HKL own=GetKeyboardLayout(0),console_thread=thread ? GetKeyboardLayout(thread) : NULL;
        char class_name[128]={0};
        if(window) GetClassNameA(window,class_name,sizeof(class_name));
        fprintf(log,"console-window=%p class=%s visible=%d owner-pid=%lu owner-tid=%lu\n",
            (void *)window,class_name,window ? IsWindowVisible(window) : 0,process,thread);
        fprintf(log,"thread-hkl=%p console-window-thread-hkl=%p loaded-count=%d\n",
            (void *)own,(void *)console_thread,GetKeyboardLayoutList(0,NULL));
        /* The HWND can be a pseudoconsole message window. A successful
         * thread query alone does not identify the user's active terminal
         * input locale; do not use equality on this host as equivalence. */
        fprintf(log,"window-thread query is observation only, not Console-layout equivalence\n");
        {
            HKL layouts[32];
            int loaded=GetKeyboardLayoutList(ARRAYSIZE(layouts),layouts),item;
            for(item=0;item<loaded;++item) if(layouts[item]!=own) {
                BOOL named,restored;
                HKL selected,after;
                char changed[KL_NAMELENGTH]={0};
                /* Only this disposable fixture thread changes. No load,
                 * unload, registry mutation or foreground activation. */
                CHECK(ActivateKeyboardLayout(layouts[item],0)!=NULL);
                selected=GetKeyboardLayout(0);
                named=GetKeyboardLayoutNameA(changed);
                after=thread ? GetKeyboardLayout(thread) : NULL;
                restored=ActivateKeyboardLayout(own,0)!=NULL;
                fprintf(log,"local-switch hkl=%p name=%s console-thread-after=%p restored=%d\n",
                    (void *)selected,named ? changed : "unavailable",(void *)after,restored);
                fflush(log);
                CHECK(restored && GetKeyboardLayout(0)==own);
                CHECK(named && selected==layouts[item]);
                fprintf(log,"local-thread-differs-from-console=%d\n",selected!=after);
                break;
            }
        }
    }
    fflush(log);
    CHECK(FreeConsole());
    memset(name,0x55,sizeof(name));SetLastError(0);
    result=query_a(name);error=GetLastError();
    fprintf(log,"detached console-result=%d error=%lu first=%02x\n",result,error,(BYTE)name[0]);
    fflush(log);
    CHECK(!result && error!=ERROR_SUCCESS);
    CHECK(GetKeyboardLayoutNameA(thread_name));
    fprintf(log,"detached thread-query=%s; not equivalent to Console query\n%s\n",
        thread_name,ansi_ok && wide_ok ? "PASS" : "UNAVAILABLE: exported Console API did not succeed");
    fclose(log);
    return ansi_ok && wide_ok ? 0 : 2;
}
