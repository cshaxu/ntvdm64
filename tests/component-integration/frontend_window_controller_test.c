#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "window_controller.h"
#include "window_keyboard.h"

#define CHECK(x) do { if(!(x)) { fprintf(stderr,"FAIL line %d error %lu: %s\n",__LINE__,GetLastError(),#x); return __LINE__; } } while(0)
typedef struct observations {
    LONG retired; DWORD routes; BOOL window,graphics; DWORD consumer_thread;
    HANDLE native_input;frontend_keyboard_delivery keyboard;
} observations;
static DWORD deliver_console(void *context,const INPUT_RECORD *records,DWORD count)
{
    observations *seen=context;DWORD written;
    if(!WriteConsoleInputW(seen->native_input,records,count,&written))return GetLastError();
    return written==count ? ERROR_SUCCESS : ERROR_WRITE_FAULT;
}
typedef struct window_find { const char *title; HWND window; } window_find;
static lib_bool input(void *context,const frontend_window_input *copied)
{
    observations *seen=context;
    const kvm_input_event *event=&copied->event;
    if(!seen->consumer_thread)seen->consumer_thread=GetCurrentThreadId();
    if(seen->consumer_thread!=GetCurrentThreadId() || event->source)return LIB_FALSE;
    if(event->type==KVM_EVENT_SOURCE_RETIRED)InterlockedIncrement(&seen->retired);
    if(seen->native_input && frontend_keyboard_dispatch(&seen->keyboard,copied,TRUE,deliver_console,seen))
        return LIB_FALSE;
    return LIB_TRUE;
}
static DWORD route(void *context,BOOL window,BOOL graphics)
{
    observations *seen=context;
    ++seen->routes;seen->window=window;seen->graphics=graphics;
    return ERROR_SUCCESS;
}
static BOOL CALLBACK find_window(HWND window,LPARAM parameter)
{
    window_find *found=(window_find *)parameter;
    DWORD pid;char title[128];
    GetWindowThreadProcessId(window,&pid);
    if(pid==GetCurrentProcessId() && GetWindowTextA(window,title,sizeof(title)) &&
        !strcmp(title,found->title)) { found->window=window;return FALSE; }
    return TRUE;
}
static HWND lookup(const char *title)
{
    window_find found={title,NULL};
    EnumWindows(find_window,(LPARAM)&found);return found.window;
}
static BOOL close_request(HWND window)
{
    DWORD_PTR result;
    return SendMessageTimeoutW(window,WM_CLOSE,0,0,SMTO_ABORTIFHUNG,3000,&result)!=0;
}
static HWND captured_window(HWND window)
{
    GUITHREADINFO info={sizeof(info)};
    DWORD thread=GetWindowThreadProcessId(window,NULL);
    return GetGUIThreadInfo(thread,&info) ? info.hwndCapture : NULL;
}
/* Test-only keyboard state injection on this process's private Window thread.
 * No SendInput, focus switch, production injection option or owner input. */
static HHOOK keyboard_hook;
static BYTE chord_control;
static BYTE chord_alt=1;
static LRESULT CALLBACK set_chord_state(int code,WPARAM wparam,LPARAM lparam)
{
    if(code>=0) {
        const CWPSTRUCT *message=(const CWPSTRUCT *)lparam;
        if(message->message==WM_KEYDOWN || message->message==WM_SYSKEYDOWN ||
            message->message==WM_KEYUP || message->message==WM_SYSKEYUP) {
            BYTE state[256]={0};
            if(chord_alt)state[VK_MENU]=0x80;
            if(chord_control)state[VK_CONTROL]=0x80;
            SetKeyboardState(state);
        }
    }
    return CallNextHookEx(keyboard_hook,code,wparam,lparam);
}
static BOOL chord_request(HWND window,BOOL caf)
{
    DWORD_PTR result;DWORD thread=GetWindowThreadProcessId(window,NULL);
    BOOL sent;
    chord_control=(BYTE)caf;
    chord_alt=1;
    keyboard_hook=SetWindowsHookExW(WH_CALLWNDPROC,set_chord_state,NULL,thread);
    if(!keyboard_hook)return FALSE;
    sent=SendMessageTimeoutW(window,WM_SYSKEYDOWN,caf ? 'F' : VK_RETURN,
        1|((LPARAM)(caf ? 0x21 : 0x1c)<<16)|(1L<<29),SMTO_ABORTIFHUNG,3000,&result)!=0;
    if(sent)sent=SendMessageTimeoutW(window,WM_SYSKEYUP,caf ? 'F' : VK_RETURN,
        1|((LPARAM)(caf ? 0x21 : 0x1c)<<16)|(1L<<29)|(1L<<30)|(1L<<31),
        SMTO_ABORTIFHUNG,3000,&result)!=0;
    if(!UnhookWindowsHookEx(keyboard_hook))sent=FALSE;
    keyboard_hook=NULL;return sent;
}
static BOOL text_key(HWND window,WPARAM key,unsigned scan)
{
    DWORD_PTR result;BOOL sent;DWORD thread=GetWindowThreadProcessId(window,NULL);
    chord_control=chord_alt=0;
    keyboard_hook=SetWindowsHookExW(WH_CALLWNDPROC,set_chord_state,NULL,thread);
    if(!keyboard_hook)return FALSE;
    sent=SendMessageTimeoutW(window,WM_KEYDOWN,key,1|((LPARAM)scan<<16),SMTO_ABORTIFHUNG,3000,&result)!=0;
    if(sent)sent=SendMessageTimeoutW(window,WM_KEYUP,key,
        (LPARAM)(1u|(scan<<16)|(1u<<30)|(1u<<31)),SMTO_ABORTIFHUNG,3000,&result)!=0;
    if(!UnhookWindowsHookEx(keyboard_hook))sent=FALSE;
    keyboard_hook=NULL;return sent;
}
static int run_on_private_desktop(void)
{
    char name[64],image[MAX_PATH],command[MAX_PATH+3];
    STARTUPINFOA start={sizeof(start)};
    PROCESS_INFORMATION child={0};
    HDESK desktop;
    DWORD code=ERROR_GEN_FAILURE;
    sprintf_s(name,sizeof(name),"NTVDMConsoleTest-%lu",GetCurrentProcessId());
    desktop=CreateDesktopA(name,NULL,NULL,0,GENERIC_ALL,NULL);
    if(!desktop)return (int)GetLastError();
    if(!GetModuleFileNameA(NULL,image,sizeof(image))) { CloseDesktop(desktop);return (int)GetLastError(); }
    sprintf_s(command,sizeof(command),"\"%s\"",image);
    start.lpDesktop=name;
    if(CreateProcessA(NULL,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&start,&child)) {
        if(WaitForSingleObject(child.hProcess,120000)==WAIT_OBJECT_0)
            GetExitCodeProcess(child.hProcess,&code);
        else { TerminateProcess(child.hProcess,ERROR_TIMEOUT);code=ERROR_TIMEOUT; }
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
    } else code=GetLastError();
    CloseDesktop(desktop);
    fprintf(stderr,"private Window controller test exit=%lu\n",code);
    return (int)code;
}
int main(int argc,char **argv)
{
    char desktop[96];DWORD size;
    frontend_window_controller *a=NULL,*b=NULL;
    observations first={0},second={0};
    frontend_window_callbacks ca={&first,input,route},cb={&second,input,route};
    kvm_window_frame *frame=calloc(1,sizeof(*frame));
    HWND wa,wb;DWORD routes,input_mode=0;
    (void)argv;CHECK(argc==1);
    /* Never create a test Window on the user's desktop. */
    if(!GetUserObjectInformationA(GetThreadDesktop(GetCurrentThreadId()),UOI_NAME,
        desktop,sizeof(desktop),&size) || strncmp(desktop,"NTVDMConsoleTest-",17))
        return run_on_private_desktop();
    CHECK(GetUserObjectInformationA(GetThreadDesktop(GetCurrentThreadId()),UOI_NAME,
        desktop,sizeof(desktop),&size));
    CHECK(!strncmp(desktop,"NTVDMConsoleTest-",17));
    {
        frontend_window_input_queue queue;
        frontend_window_input copy;kvm_input_event event={0};unsigned i;
        CHECK(frontend_window_input_queue_create(&queue));
        event.type=KVM_EVENT_KEY;event.source=&queue;event.source_identity=77;
        for(i=0;i<FRONTEND_WINDOW_INPUT_CAPACITY;++i) {
            event.data.key.scan_code=(lib_u16)i;
            CHECK(frontend_window_input_queue_push(&queue,&event));
        }
        CHECK(!frontend_window_input_queue_push(&queue,&event) && GetLastError()==ERROR_BUFFER_OVERFLOW);
        CHECK(WaitForSingleObject(queue.ready,0)==WAIT_OBJECT_0);
        frontend_window_input_queue_close(&queue);
        CHECK(!frontend_window_input_queue_push(&queue,&event) && GetLastError()==ERROR_OPERATION_ABORTED);
        for(i=0;i<FRONTEND_WINDOW_INPUT_CAPACITY;++i) {
            CHECK(frontend_window_input_queue_pop(&queue,&copy));
            CHECK(copy.event.source==NULL && copy.event.source_identity==77 && copy.event.data.key.scan_code==i);
            CHECK(copy.keyboard_layout && copy.ui_thread_id==GetCurrentThreadId());
        }
        CHECK(!frontend_window_input_queue_pop(&queue,&copy) && GetLastError()==ERROR_OPERATION_ABORTED);
        frontend_window_input_queue_destroy(&queue);
        puts("PASS copied FIFO order, UI-time layout, overflow rejection, closed queue drain and no borrowed source pointer");
    }
    CHECK(frame);
    frame->valid=LIB_TRUE;frame->text.base.text_columns=80;
    frame->text.base.text_rows=50;frame->text.base.font_height=8;
    CHECK(!frontend_window_create(&a,&ca,"Frontend controller A"));
    CHECK(!frontend_window_create(&b,&cb,"Frontend controller B"));
    CHECK(frontend_window_mode(a)==FRONTEND_DISPLAY_CONSOLE);
    CHECK(!frontend_window_present(a,frame,FALSE) && !frontend_window_visible(a));
    CHECK(!frontend_window_select(a,FRONTEND_DISPLAY_WINDOW));
    {
        unsigned attempt;
        for(attempt=0;attempt<100;++attempt) {
            wa=lookup("Frontend controller A");
            if(wa && IsWindowVisible(wa) && first.window)break;
            Sleep(10);
        }
        CHECK(attempt<100);
    }
    {
        char actual[KVM_WINDOW_TITLE_CAPACITY];
        char too_long[KVM_WINDOW_TITLE_CAPACITY+1];
        unsigned attempt;
        CHECK(!frontend_window_set_title(a,"Console title changed"));
        for(attempt=0;attempt<100;++attempt) {
            GetWindowTextA(wa,actual,sizeof(actual));
            if(!strcmp(actual,"Console title changed"))break;
            Sleep(10);
        }
        CHECK(attempt<100 && IsWindow(wa));
        CHECK(!frontend_window_set_title(a,"") && !frontend_window_set_title(a,"Frontend controller A"));
        for(attempt=0;attempt<100;++attempt) {
            GetWindowTextA(wa,actual,sizeof(actual));
            if(!strcmp(actual,"Frontend controller A"))break;
            Sleep(10);
        }
        CHECK(attempt<100 && IsWindow(wa));
        memset(too_long,'x',sizeof(too_long));too_long[sizeof(too_long)-1]=0;
        CHECK(frontend_window_set_title(a,too_long)==ERROR_INVALID_PARAMETER);
        puts("PASS live Window caption update, empty title, capacity guard and unchanged HWND");
    }
    {
        DWORD_PTR result;unsigned attempt;
        chord_control=chord_alt=0;
        CHECK(SendMessageTimeoutW(wa,WM_LBUTTONDOWN,MK_LBUTTON,
            MAKELPARAM(16,16),SMTO_ABORTIFHUNG,3000,&result));
        CHECK(captured_window(wa)==wa);
        chord_control=chord_alt=1;
        keyboard_hook=SetWindowsHookExW(WH_CALLWNDPROC,set_chord_state,NULL,
            GetWindowThreadProcessId(wa,NULL));
        CHECK(keyboard_hook);
        CHECK(SendMessageTimeoutW(wa,WM_SYSKEYDOWN,'M',
            1|(0x32L<<16)|(1L<<29),SMTO_ABORTIFHUNG,3000,&result));
        CHECK(UnhookWindowsHookEx(keyboard_hook));keyboard_hook=NULL;
        CHECK(WaitForSingleObject(frontend_window_wake(a),3000)==WAIT_OBJECT_0);
        CHECK(!frontend_window_poll(a));
        for(attempt=0;attempt<100 && captured_window(wa)==wa;++attempt)Sleep(10);
        CHECK(captured_window(wa)!=wa && frontend_window_visible(a) &&
            frontend_window_mode(a)==FRONTEND_DISPLAY_WINDOW);
        puts("PASS Ctrl+Alt+M releases Window pointer without changing display or closing Window");
    }
    first.native_input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    CHECK(first.native_input!=INVALID_HANDLE_VALUE && GetConsoleMode(first.native_input,&input_mode));
    CHECK(SetConsoleMode(first.native_input,ENABLE_LINE_INPUT|ENABLE_PROCESSED_INPUT));
    CHECK(FlushConsoleInputBuffer(first.native_input));
    {
        DWORD_PTR result;DWORD count;INPUT_RECORD records[8];lib_u64 identity;
        CHECK(SendMessageTimeoutW(wa,WM_KEYDOWN,'A',1|(0x1eL<<16),SMTO_ABORTIFHUNG,3000,&result));
        CHECK(!frontend_window_poll(a) && first.keyboard.physical.held_count==1);
        identity=first.keyboard.physical.source_identity;CHECK(identity);
        CHECK(SendMessageTimeoutW(wa,WM_KILLFOCUS,0,0,SMTO_ABORTIFHUNG,3000,&result));
        CHECK(SendMessageTimeoutW(wa,WM_ACTIVATEAPP,FALSE,0,SMTO_ABORTIFHUNG,3000,&result));
        CHECK(!frontend_window_poll(a) && !first.keyboard.physical.held_count &&
            first.keyboard.physical.source_identity==identity && IsWindow(wa));
        CHECK(ReadConsoleInputW(first.native_input,records,ARRAYSIZE(records),&count));
        CHECK(count==2 && records[0].EventType==KEY_EVENT && records[0].Event.KeyEvent.bKeyDown &&
            records[1].EventType==KEY_EVENT && !records[1].Event.KeyEvent.bKeyDown &&
            records[0].Event.KeyEvent.wVirtualScanCode==0x1e && records[1].Event.KeyEvent.wVirtualScanCode==0x1e);
        puts("PASS actual Window loss messages -> FIFO -> frontend reset -> one delivered-key release; live source retained");
    }
    CHECK(text_key(wa,'A',0x1e) && text_key(wa,'B',0x30) && text_key(wa,VK_RETURN,0x1c));
    CHECK(!frontend_window_poll(a));
    {
        WCHAR line[16];DWORD count;
        CHECK(ReadConsoleW(first.native_input,line,16,&count,NULL));
        CHECK(count==4 && !memcmp(line,L"ab\r\n",4*sizeof(WCHAR)));
        CHECK(first.keyboard.physical.held_count==0);
        puts("PASS real Window key messages -> copied FIFO -> native conversion -> cooked Console exact ab CR LF");
    }
    CHECK(!frontend_window_present(b,frame,FALSE));
    CHECK(!frontend_window_select(b,FRONTEND_DISPLAY_WINDOW));
    wb=lookup("Frontend controller B");CHECK(wb && IsWindowVisible(wb));
    CHECK(close_request(wa));
    CHECK(WaitForSingleObject(frontend_window_wake(a),3000)==WAIT_OBJECT_0);
    CHECK(!frontend_window_poll(a));
    CHECK(!frontend_window_visible(a) && !IsWindow(wa) && !first.window);
    CHECK(frontend_window_visible(b) && IsWindow(wb));
    CHECK(frontend_window_mode(a)==FRONTEND_DISPLAY_CONSOLE && first.retired==1);
    CHECK(first.keyboard.physical.source_identity==0 && !first.keyboard.native.layout);
    CHECK(SetConsoleMode(first.native_input,input_mode));CloseHandle(first.native_input);first.native_input=NULL;
    {
        unsigned chord;
        for(chord=0;chord<2;++chord) {
            CHECK(!frontend_window_select(a,FRONTEND_DISPLAY_WINDOW));
            wa=lookup("Frontend controller A");CHECK(wa);
            CHECK(chord_request(wa,chord!=0));
            CHECK(WaitForSingleObject(frontend_window_wake(a),3000)==WAIT_OBJECT_0);
            CHECK(!frontend_window_poll(a));
            CHECK(!frontend_window_visible(a) && frontend_window_mode(a)==FRONTEND_DISPLAY_CONSOLE);
        }
    }
    /* Native raster is text: only the explicit policy, not frame.graphics,
     * selects Window. Static DOS graphics continues without new frames. */
    memset(frame,0,sizeof(*frame));frame->valid=frame->graphics=LIB_TRUE;
    frame->image.width=frame->image.stride=2;frame->image.height=2;
    CHECK(!frontend_window_present(a,frame,FALSE) && !frontend_window_visible(a));
    CHECK(!frontend_window_present(a,frame,TRUE) && frontend_window_visible(a));
    wa=lookup("Frontend controller A");CHECK(wa && first.graphics);
    routes=first.routes;
    CHECK(close_request(wa));
    CHECK(WaitForSingleObject(frontend_window_wake(a),3000)==WAIT_OBJECT_0);
    CHECK(!frontend_window_poll(a) && frontend_window_visible(a));
    CHECK(lookup("Frontend controller A")==wa && first.routes==routes);
    CHECK(frontend_window_mode(a)==FRONTEND_DISPLAY_CONSOLE);
    CHECK(!frontend_window_poll(a) && frontend_window_visible(a));
    CHECK(!frontend_window_present(a,frame,FALSE) && !frontend_window_visible(a));
    CHECK(first.retired==4);
    CHECK(!frontend_window_select(a,FRONTEND_DISPLAY_WINDOW));
    CHECK(frontend_window_visible(a));
    CHECK(!frontend_window_clear(a) && !frontend_window_visible(a));
    CHECK(frontend_window_mode(a)==FRONTEND_DISPLAY_WINDOW);
    CHECK(!frontend_window_present(a,frame,FALSE) && frontend_window_visible(a));
    CHECK(!frontend_window_destroy(a));CHECK(!frontend_window_destroy(b));
    CHECK(first.retired==6 && second.retired==1);
    CHECK(first.consumer_thread==GetCurrentThreadId() && second.consumer_thread==GetCurrentThreadId());
    CHECK(!lookup("Frontend controller A") && !lookup("Frontend controller B"));
    free(frame);
    puts("PASS real private Window: policy, X/CAF/AltEnter, static graphics, native raster classification, owner handoff, independent instances, joined retirement");
    return 0;
}
