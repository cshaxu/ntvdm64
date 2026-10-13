#include "nthook32-dll/hook.h"
#include <stdio.h>
#include <wchar.h>
static void failure(int line,const char *expression,DWORD error)
{
    WCHAR path[MAX_PATH+32];FILE *file=NULL;
    if(GetModuleFileNameW(NULL,path,MAX_PATH) &&
       !wcscat_s(path,ARRAYSIZE(path),L".failure.txt") &&
       !_wfopen_s(&file,path,L"at")){
        fprintf(file,"pid=%lu line=%d error=%lu: %s\n",GetCurrentProcessId(),line,error,expression);fclose(file);
    }
    fprintf(stderr,"native title FAIL %d error=%lu: %s\n",line,error,expression);
    ExitProcess(1);
}
#define CHECK(x) do{if(!(x))failure(__LINE__,#x,GetLastError());}while(0)
static BOOL enabled;
static void completed_witness(DWORD policy)
{
    WCHAR path[MAX_PATH+32];FILE *file=NULL;
    CHECK(GetModuleFileNameW(NULL,path,MAX_PATH));
    CHECK(!wcscat_s(path,ARRAYSIZE(path),L".policy.txt"));
    CHECK(!_wfopen_s(&file,path,L"at"));
    fprintf(file,"pid=%lu policy=%lu complete\n",GetCurrentProcessId(),policy);
    CHECK(!fclose(file));
}
static unsigned witness_count(PCWSTR executable)
{
    WCHAR path[MAX_PATH+32];FILE *file=NULL;char line[160];unsigned count=0;
    wcscpy_s(path,ARRAYSIZE(path),executable);wcscat_s(path,ARRAYSIZE(path),L".policy.txt");
    if(!_wfopen_s(&file,path,L"rt")){
        while(fgets(line,sizeof(line),file))if(strstr(line," complete"))++count;
        fclose(file);
    }
    return count;
}
static BOOL baseline_return;
static HANDLE separate_child;
static void native_trace(const char *phase)
{
    WCHAR path[MAX_PATH];FILE *file=NULL;
    DWORD length=GetEnvironmentVariableW(L"MVDM_STYLE_PROBE_LOG",path,ARRAYSIZE(path));
    if(!length)return;
    CHECK(length<ARRAYSIZE(path) && !_wfopen_s(&file,path,L"at"));
    fprintf(file,"pid=%lu tid=%lu phase=%s\n",GetCurrentProcessId(),GetCurrentThreadId(),phase);
    fclose(file);
}
/* A thread that has owned windows must service sent USER messages while it
 * waits for another GUI thread/process (activation and dialog ownership). */
static DWORD wait_gui(HANDLE target,DWORD timeout)
{
    ULONGLONG deadline=GetTickCount64()+timeout;
    for(;;){
        ULONGLONG now=GetTickCount64();MSG message;
        if(now>=deadline)return WAIT_TIMEOUT;
        DWORD result=MsgWaitForMultipleObjectsEx(1,&target,(DWORD)(deadline-now),QS_ALLINPUT,MWMO_INPUTAVAILABLE);
        if(result!=WAIT_OBJECT_0+1)return result;
        while(PeekMessageW(&message,NULL,0,0,PM_REMOVE)){
            CHECK(message.message!=WM_QUIT);TranslateMessage(&message);DispatchMessageW(&message);
        }
    }
}
static void title(HWND window,PCWSTR expected)
{ WCHAR actual[256],wanted[256];CHECK(window);GetWindowTextW(window,actual,256);swprintf_s(wanted,256,L"%ls%ls",enabled?L"(Win16 Style) ":L"",expected);if(wcscmp(actual,wanted))fwprintf(stderr,L"actual=[%ls] wanted=[%ls]\n",actual,wanted);CHECK(!wcscmp(actual,wanted)); }
static LRESULT CALLBACK proc(HWND w,UINT m,WPARAM a,LPARAM b)
{return DefWindowProcW(w,m,a,b);}
static LRESULT CALLBACK return_proc(HWND w,UINT m,WPARAM a,LPARAM b)
{
    if(m==WM_APP+0x33){DWORD code=MAXDWORD;return separate_child && GetExitCodeProcess(separate_child,&code)?code:MAXDWORD;}
    if(m==WM_APP+0x32){
        DWORD code=STILL_ACTIVE;
        CHECK(separate_child && wait_gui(separate_child,10000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeProcess(separate_child,&code) && !code);
        CloseHandle(separate_child);separate_child=NULL;return 1;
    }
    if(m==WM_DESTROY){PostQuitMessage(0);return 0;}
    if(m==WM_APP+0x31){
        native_trace("return-request");
        nthook_context context;BOOL found=FALSE;WCHAR guest[MAX_PATH],launcher[MAX_PATH],line[MAX_PATH*2+8],*slash;
        STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION child={0};DWORD code;
        if(baseline_return){
            DWORD length=GetModuleFileNameW(NULL,guest,ARRAYSIZE(guest));
            CHECK(length && length<ARRAYSIZE(guest));
        }else{
            CHECK(!nthook_context_read(&context,&found) && found);
            wcscpy_s(guest,context.launcher);
        }
        slash=wcsrchr(guest,L'\\');CHECK(slash);*slash=0;
        slash=wcsrchr(guest,L'\\');CHECK(slash);CHECK(!wcscpy_s(slash+1,ARRAYSIZE(guest)-(size_t)(slash+1-guest),L"WINMINE.EXE"));
        if(baseline_return){
            wcscpy_s(launcher,guest);slash=wcsrchr(launcher,L'\\');CHECK(slash);
            CHECK(!wcscpy_s(slash+1,ARRAYSIZE(launcher)-(size_t)(slash+1-launcher),L"system32\\run16.exe"));
            swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" \"%ls\"",launcher,guest);
        }else swprintf_s(line,ARRAYSIZE(line),L"\"%ls\"",guest);
        /* No explicit Run16/options: the inherited Hook must redirect the
         * actual Win16 image and retain the original launch policy. */
        /* The baseline control explicitly calls Run16 on both versions and
         * avoids reading the changed Hook ABI; it is not propagation proof. */
        if(a==2){
            char app_a[MAX_PATH*2],line_a[MAX_PATH*4];STARTUPINFOA start_a={sizeof(start_a)};
            CHECK(WideCharToMultiByte(CP_ACP,0,guest,-1,app_a,sizeof(app_a),NULL,NULL));
            CHECK(WideCharToMultiByte(CP_ACP,0,line,-1,line_a,sizeof(line_a),NULL,NULL));
            CHECK(CreateProcessA(app_a,line_a,NULL,NULL,FALSE,CREATE_SEPARATE_WOW_VDM,NULL,NULL,&start_a,&child));
        }else CHECK(CreateProcessW(baseline_return?launcher:guest,line,NULL,NULL,FALSE,
            a==1?CREATE_SEPARATE_WOW_VDM:0,NULL,NULL,&start,&child));
        native_trace("return-child-created");
        if(a){CHECK(!separate_child);separate_child=child.hProcess;CloseHandle(child.hThread);return 1;}
        CHECK(wait_gui(child.hProcess,45000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeProcess(child.hProcess,&code) && !code);
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
        native_trace("return-child-launcher-exited");
        title(w,L"native relay");return 1;
    }
    return DefWindowProcW(w,m,a,b);
}
static INT_PTR CALLBACK dialog(HWND w,UINT m,WPARAM,LPARAM modal)
{if(m==WM_INITDIALOG){CHECK(SetTimer(w,1,10,NULL));SetWindowLongPtrW(w,DWLP_USER,modal);return TRUE;}if(m==WM_TIMER){title(w,L"dialog");KillTimer(w,1);if(GetWindowLongPtrW(w,DWLP_USER))EndDialog(w,47);return TRUE;}return FALSE;}
static void CALLBACK message_box_timer(HWND window,UINT,UINT_PTR timer,DWORD)
{
    CHECK(KillTimer(window,timer));title(window,L"message box");
    CHECK(PostMessageW(window,WM_CLOSE,0,0));
}
static LRESULT CALLBACK message_box_created(int code,WPARAM wp,LPARAM lp)
{
    if(code==HCBT_ACTIVATE){
        WCHAR cls[32];GetClassNameW((HWND)wp,cls,ARRAYSIZE(cls));
        if(!wcscmp(cls,L"#32770"))CHECK(SetTimer((HWND)wp,0x7156,50,message_box_timer));
    }
    return CallNextHookEx(NULL,code,wp,lp);
}
static void message_boxes(void)
{
    for(unsigned variant=0;variant<6;++variant){
        HHOOK observer=SetWindowsHookExW(WH_CBT,message_box_created,NULL,GetCurrentThreadId());CHECK(observer);
        printf("MessageBox begin variant=%u enabled=%d\n",variant,enabled);fflush(stdout);
        int result=0;
        switch(variant){
        case 0:result=MessageBoxA(NULL,"text","message box",MB_OK);break;
        case 1:result=MessageBoxW(NULL,L"text",L"message box",MB_OK);break;
        case 2:result=MessageBoxExA(NULL,"text","message box",MB_OK,0);break;
        case 3:result=MessageBoxExW(NULL,L"text",L"message box",MB_OK,0);break;
        case 4:{MSGBOXPARAMSA p={sizeof(p)};p.lpszText="text";p.lpszCaption="message box";result=MessageBoxIndirectA(&p);break;}
        case 5:{MSGBOXPARAMSW p={sizeof(p)};p.lpszText=L"text";p.lpszCaption=L"message box";result=MessageBoxIndirectW(&p);break;}
        }
        CHECK(result==IDOK);
        CHECK(UnhookWindowsHookEx(observer));
        printf("PASS MessageBox variant=%u policy-enabled=%d tid=%lu\n",variant,enabled,GetCurrentThreadId());
    }
}
static DWORD WINAPI message_box_thread(void *){message_boxes();return 0;}
static DWORD WINAPI windows(void *)
{
    printf("windows begin tid=%lu tick=%llu\n",GetCurrentThreadId(),GetTickCount64());fflush(stdout);
    HWND w;WNDCLASSW cls={0};cls.lpfnWndProc=proc;cls.lpszClassName=L"NativeTitle";
    if(!RegisterClassW(&cls))CHECK(GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    w=CreateWindowExW(0,cls.lpszClassName,L"wide",WS_OVERLAPPEDWINDOW,0,0,200,100,NULL,NULL,NULL,NULL);
    title(w,L"wide");CHECK(SetWindowTextA(w,"updated"));title(w,L"updated");CHECK(DestroyWindow(w));
    w=CreateWindowExA(0,"STATIC","ansi",WS_OVERLAPPEDWINDOW,0,0,200,100,NULL,NULL,NULL,NULL);
    title(w,L"ansi");CHECK(DestroyWindow(w));
    struct {DLGTEMPLATE header;WORD menu,cls;WCHAR caption[7];} resource={};
    resource.header.style=WS_POPUP|WS_CAPTION|WS_SYSMENU;resource.header.cx=100;resource.header.cy=80;
    wcscpy_s(resource.caption,L"dialog");
    w=CreateDialogIndirectParamW(NULL,&resource.header,NULL,dialog,0);title(w,L"dialog");CHECK(DestroyWindow(w));
    w=CreateDialogIndirectParamA(NULL,&resource.header,NULL,dialog,0);title(w,L"dialog");CHECK(DestroyWindow(w));
    CHECK(DialogBoxIndirectParamW(NULL,&resource.header,NULL,dialog,1)==47);
    CHECK(DialogBoxIndirectParamA(NULL,&resource.header,NULL,dialog,1)==47);
    HINSTANCE module=GetModuleHandleW(NULL);
    printf("named dialogs tid=%lu tick=%llu\n",GetCurrentThreadId(),GetTickCount64());fflush(stdout);
    w=CreateDialogParamW(module,MAKEINTRESOURCEW(101),NULL,dialog,0);title(w,L"dialog");CHECK(DestroyWindow(w));
    w=CreateDialogParamA(module,MAKEINTRESOURCEA(101),NULL,dialog,0);title(w,L"dialog");CHECK(DestroyWindow(w));
    CHECK(DialogBoxParamW(module,MAKEINTRESOURCEW(101),NULL,dialog,1)==47);
    CHECK(DialogBoxParamA(module,MAKEINTRESOURCEA(101),NULL,dialog,1)==47);
    printf("windows end tid=%lu tick=%llu\n",GetCurrentThreadId(),GetTickCount64());fflush(stdout);
    return 0;
}
int wmain(int argc,WCHAR **argv)
{
    if(argc==2 && (!wcscmp(argv[1],L"wow-return") || !wcscmp(argv[1],L"wow-baseline-return"))){
        native_trace("entry");
        nthook_context context;BOOL found=FALSE;DWORD code;MSG message;
        baseline_return=!wcscmp(argv[1],L"wow-baseline-return");
        if(!baseline_return){
            CHECK(!nthook_context_read(&context,&found) && found);
            native_trace("context-read");
            enabled=context.window_style==WINDOW_STYLE_ALL;
        }
        WNDCLASSW cls={0};cls.lpfnWndProc=return_proc;cls.lpszClassName=L"NativeStyleReturn";
        CHECK(RegisterClassW(&cls));
        native_trace("before-window");
        HWND window=CreateWindowExW(0,cls.lpszClassName,L"native relay",WS_OVERLAPPEDWINDOW|WS_VISIBLE,0,0,250,140,NULL,NULL,NULL,NULL);
        native_trace("after-window");
        title(window,L"native relay");
        /* Reach ordinary input-idle first. The observer requests the return
         * launch after the original WinExec parent has finished. */
        native_trace("before-message-loop");
        while((code=(DWORD)GetMessageW(&message,NULL,0,0))!=0){
            CHECK(code!=(DWORD)-1);TranslateMessage(&message);DispatchMessageW(&message);
        }
        return 0;
    }
    if((argc==3 && !wcscmp(argv[1],L"leaf")) ||
       (argc==6 && !wcscmp(argv[1],L"relay-child"))){
        nthook_context context;BOOL found=FALSE;
        CHECK(!nthook_context_read(&context,&found) && found);
        CHECK(context.window_style==(DWORD)_wtoi(argv[2]));
        enabled=context.window_style==WINDOW_STYLE_ALL;
        HWND window=CreateWindowExW(0,L"STATIC",L"relay",WS_OVERLAPPEDWINDOW,0,0,200,100,NULL,NULL,NULL,NULL);
        title(window,L"relay");CHECK(SetWindowTextW(window,L"updated relay"));title(window,L"updated relay");CHECK(DestroyWindow(window));
        if(argc==6){
            DWORD expected=!wcscmp(argv[5],L"inherit")?context.window_style:(DWORD)_wtoi(argv[5]);
            PCWSTR option=!wcscmp(argv[5],L"inherit")?L"":expected==2?L"--win16-style-all ":L"--win16-style ";
            WCHAR line[MAX_PATH*2+120];STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION child={0};DWORD code;
            swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" --wait %ls\"%ls\" leaf %lu",argv[4],option,argv[3],expected);
            CHECK(CreateProcessW(argv[4],line,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&start,&child));
            if(wait_gui(child.hProcess,45000)!=WAIT_OBJECT_0){TerminateProcess(child.hProcess,ERROR_TIMEOUT);CHECK(FALSE);}
            CHECK(GetExitCodeProcess(child.hProcess,&code) && !code);CloseHandle(child.hThread);CloseHandle(child.hProcess);
            window=CreateWindowExW(0,L"STATIC",L"parent after relay",WS_OVERLAPPEDWINDOW,0,0,200,100,NULL,NULL,NULL,NULL);
            title(window,L"parent after relay");CHECK(DestroyWindow(window));
        }
        completed_witness(context.window_style);
        return 0;
    }
    BOOL message_case=argc==3 && !wcscmp(argv[1],L"messages");
    if(message_case || ((argc==3 || argc==4) && !wcscmp(argv[1],L"child"))) {
        native_trace("policy-child-entry");
        HANDLE thread;DWORD code;nthook_context context;BOOL found=FALSE;
        CHECK(!nthook_context_read(&context,&found) && found);
        CHECK(context.window_style==(DWORD)_wtoi(argv[2]));
        native_trace("policy-child-context-checked");
        enabled=context.window_style==WINDOW_STYLE_ALL;
        if(message_case)message_boxes();else windows(NULL);
        thread=CreateThread(NULL,0,message_case?message_box_thread:windows,NULL,0,NULL);CHECK(thread);
        CHECK(wait_gui(thread,30000)==WAIT_OBJECT_0 && GetExitCodeThread(thread,&code) && !code);CloseHandle(thread);
        if(argc==4){
            STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION child={0};WCHAR line[MAX_PATH+80];
            swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" child %lu",argv[3],context.window_style);
            CHECK(CreateProcessW(argv[3],line,NULL,NULL,FALSE,0,NULL,NULL,&start,&child));
            if(wait_gui(child.hProcess,45000)!=WAIT_OBJECT_0){TerminateProcess(child.hProcess,ERROR_TIMEOUT);CHECK(FALSE);}
            CHECK(GetExitCodeProcess(child.hProcess,&code) && !code);CloseHandle(child.hThread);CloseHandle(child.hProcess);
            printf("PASS actual Hook descendant inherited policy=%lu\n",context.window_style);
        }
        if(message_case)printf("PASS native policy=%lu six MessageBox APIs and later GUI thread\n",context.window_style);
        else printf("PASS native policy=%lu A/W, modeless/modal, dynamic titles, later GUI thread\n",context.window_style);
        native_trace("policy-child-success");
        /* A DOS shell's zero exit alone does not prove that it reached this
         * fixture. Keep a module-local witness independent of guest env. */
        completed_witness(context.window_style);
        return 0;
    }
    BOOL relay_case=argc==5 && !wcscmp(argv[1],L"relay");
    BOOL message_launcher=argc==5 && !wcscmp(argv[1],L"message-launcher");
    BOOL messages_only=argc==2 && !wcscmp(argv[1],L"--messages");
    BOOL dos_leaf=argc==5 && !wcscmp(argv[1],L"dos-leaf");
    BOOL dos_launcher=dos_leaf || (argc==5 && !wcscmp(argv[1],L"dos-launcher"));
    BOOL cui_launcher=dos_launcher || (argc==5 && !wcscmp(argv[1],L"cui-launcher"));
    BOOL launcher_case=argc==5 && (!wcscmp(argv[1],L"launcher") || cui_launcher || relay_case || message_launcher);
    CHECK(argc==1 || argc==2 || launcher_case);WCHAR self[MAX_PATH],desktop_name[80];HDESK desktop;
    CHECK(GetModuleFileNameW(NULL,self,MAX_PATH));
    swprintf_s(desktop_name,80,L"NativeTitle-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(desktop_name,NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desktop);
    if(launcher_case){
        for(DWORD policy=relay_case?1:0;policy<=2;++policy)for(unsigned direction=0;direction<2;++direction)
        for(unsigned variant=0;variant<(relay_case?2u:1u);++variant){
            STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION process={0};DWORD code;
            WCHAR line[MAX_PATH*4+200];start.lpDesktop=desktop_name;
            PCWSTR option=policy==2?L"--win16-style-all ":policy==1?L"--win16-style ":L"";
            PCWSTR first=argv[3+direction],second=argv[4-direction];
            unsigned first_before=dos_launcher?witness_count(first):0;
            unsigned second_before=dos_launcher?witness_count(second):0;
            if(dos_leaf)swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" --wait %lsCOMMAND.COM /c %ls leaf %lu",argv[2],option,first,policy);
            else if(dos_launcher)swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" --wait %lsCOMMAND.COM /c %ls child %lu %ls",argv[2],option,first,policy,second);
            else if(relay_case)swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" --wait %ls\"%ls\" relay-child %lu \"%ls\" \"%ls\" %ls",argv[2],option,first,policy,second,argv[2],variant?(policy==1?L"2":L"1"):L"inherit");
            else if(message_launcher)swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" --wait %ls\"%ls\" messages %lu",argv[2],option,first,policy);
            else swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" --wait %ls%ls\"%ls\" child %lu \"%ls\"",argv[2],option,dos_launcher?L"COMMAND.COM /c ":L"",first,policy,second);
            wprintf(L"Run16 case: %ls\n",line);fflush(stdout);
            /* No installer/test payload here: actual CLI and product broker /
             * native worker must originate the copied policy and Hook. */
            /* CUI roots need a real Console for frontend attachment. Keep it
             * hidden on this private desktop; GUI-only cases need no Console. */
            if(cui_launcher){start.dwFlags|=STARTF_USESHOWWINDOW;start.wShowWindow=SW_HIDE;}
            CHECK(CreateProcessW(argv[2],line,NULL,NULL,TRUE,cui_launcher?CREATE_NEW_CONSOLE:CREATE_NO_WINDOW,NULL,NULL,&start,&process));
            if(WaitForSingleObject(process.hProcess,120000)!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,ERROR_TIMEOUT);CHECK(FALSE);}
            CHECK(GetExitCodeProcess(process.hProcess,&code));
            printf("Run16 exit=%lu\n",code);fflush(stdout);CHECK(!code);
            CloseHandle(process.hThread);CloseHandle(process.hProcess);
            if(dos_launcher){
                ULONGLONG deadline=GetTickCount64()+120000;
                while(witness_count(first)<=first_before || (!dos_leaf && witness_count(second)<=second_before)){
                    CHECK(GetTickCount64()<deadline);Sleep(20);
                }
                printf("PASS DOS descendants completed policy=%lu direction=%u\n",policy,direction);fflush(stdout);
            }
        }
        CHECK(CloseDesktop(desktop));
        puts(message_launcher?"PASS actual Run16 CLI: six MessageBox APIs, three policies, both widths and later GUI threads":relay_case?"PASS actual nested Run16: inherited policy and explicit override in both cross-width directions":"PASS actual Run16 CLI: three policies, native GUI and both cross-width descendant directions");return 0;
    }
    for(DWORD policy=0;policy<=2;++policy) {
        nthook_context context;STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION process={0};DWORD code;WCHAR line[MAX_PATH*2+80];
        CHECK(!nthook_context_paths(&context));context.window_style=policy;start.lpDesktop=desktop_name;
        if(messages_only)swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" messages %lu",self,policy);
        else if(argc==2)swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" child %lu \"%ls\"",self,policy,argv[1]);
        else swprintf_s(line,ARRAYSIZE(line),L"\"%ls\" child %lu",self,policy);
        CHECK(CreateProcessW(self,line,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&start,&process));
        DWORD error=nthook_install(process.hProcess,&context,NATIVE_HOOK_INTERCEPT);
        if(error){TerminateProcess(process.hProcess,error);SetLastError(error);CHECK(!error);}
        CHECK(ResumeThread(process.hThread)!=(DWORD)-1);
        if(WaitForSingleObject(process.hProcess,90000)!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,ERROR_TIMEOUT);CHECK(FALSE);}
        CHECK(GetExitCodeProcess(process.hProcess,&code) && !code);CloseHandle(process.hThread);CloseHandle(process.hProcess);
    }
    CHECK(CloseDesktop(desktop));return 0;
}
