/* Native transport/ownership fixture, not DOS acceptance. Run with the private
 * desktop observer's normal Console: CREATE_NO_WINDOW rejects buffer shrink
 * even through the native API. Only broker delivery and session binding are
 * stubbed; both I/O implementations and Console operations are real. */
#include "ntvdm-exe/win32/console_client.h"
#include "run16-exe/console_frontend.h"
static BOOL native_write_cells(HANDLE output,const CHAR_INFO *buffer,COORD size,
    COORD origin,PSMALL_RECT region) { return WriteConsoleOutputW(output,buffer,size,origin,region); }
static BOOL native_read_cells(HANDLE output,PCHAR_INFO buffer,COORD size,
    COORD origin,PSMALL_RECT region) { return ReadConsoleOutputW(output,buffer,size,origin,region); }
static BOOL native_set_size(HANDLE output,COORD size) { return SetConsoleScreenBufferSize(output,size); }
static BOOL native_write(HANDLE output,const char *text,DWORD *written)
{ return WriteConsoleA(output,text,1,written,NULL); }
static BOOL native_get_mode(HANDLE handle,DWORD *mode) { return GetConsoleMode(handle,mode); }
static BOOL native_set_mode(HANDLE handle,DWORD mode) { return SetConsoleMode(handle,mode); }
static BOOL native_cursor(HANDLE handle,PCONSOLE_CURSOR_INFO info) { return GetConsoleCursorInfo(handle,info); }
static BOOL native_peek(HANDLE handle,INPUT_RECORD *record,DWORD *read)
{ return PeekConsoleInputW(handle,record,1,read); }
static BOOL native_read_input(HANDLE handle,INPUT_RECORD *record,DWORD *read)
{ return ReadConsoleInputW(handle,record,1,read); }
static UINT native_input_cp(void) { return GetConsoleCP(); }
static UINT native_output_cp(void) { return GetConsoleOutputCP(); }
static DWORD native_title(char *buffer,DWORD size) { return GetConsoleTitleA(buffer,size); }
static BOOL native_set_title(const char *title) { return SetConsoleTitleA(title); }
static BOOL native_pointer(POINT *point) { return GetCursorPos(point); }
static BOOL native_clip(RECT *rect) { return GetClipCursor(rect); }
static BOOL native_font(HANDLE h,BOOL maximum,PCONSOLE_FONT_INFO font)
{ return GetCurrentConsoleFont(h,maximum,font); }
static COORD native_font_size(HANDLE h,DWORD index) { return GetConsoleFontSize(h,index); }
static BOOL native_display(DWORD *mode) { return GetConsoleDisplayMode(mode); }
static BOOL native_set_display(HANDLE h,DWORD flags,COORD *size)
{ return SetConsoleDisplayMode(h,flags,size); }
#include "opennt-abi/host-compat/include/conapi.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d error=%lu\n",__LINE__,GetLastError()); ExitProcess(1); } } while (0)
static session owner;
static __declspec(thread) session *bound;
static HANDLE delivery,peer,stop,readiness,frontend_process;
static session_teardown_fn cleanup;
static void *cleanup_context;
static run16_console_frontend frontend;
static BOOL hang_close;
int session_thread_bind(session *instance) { bound=instance;return 1; }
int session_thread_unbind(session *instance) { CHECK(bound==instance);bound=NULL;return 1; }
BOOL CntrlHandler(ULONG type)
{
    CHECK(type==CTRL_CLOSE_EVENT && bound==&owner);
    puts("PASS authenticated root death dispatches original-shape close callback");
    fflush(stdout);
    if (hang_close) Sleep(INFINITE);
    ExitProcess(73); /* Test substitute for original VDM close, not guest proof. */
}
void OpenNtBaseClientSetCommandBinding(DWORD (*ready)(void *),void *context)
{ (void)ready;(void)context; }
DWORD OpenNtBaseClientWorkerFrontendCapability(HANDLE *capability)
{
    /* Broker identity is exercised in the service/RPC tests, not this I/O fixture. */
    *capability=CreateEventW(NULL,TRUE,FALSE,NULL);
    return *capability ? ERROR_SUCCESS : GetLastError();
}

session *__cdecl session_thread_current(void) { return bound; }
int session_register_teardown(session *instance,session_teardown_fn function,void *context)
{
    CHECK(instance==&owner && !cleanup);
    cleanup=function;cleanup_context=context;return 1;
}
DWORD OpenNtBaseClientWaitFrontend(HANDLE *pipe,HANDLE *process,DWORD *generation,HANDLE *ready)
{
    readiness=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(readiness);
    CHECK(DuplicateHandle(GetCurrentProcess(),readiness,GetCurrentProcess(),ready,
        SYNCHRONIZE,FALSE,0));
    *pipe=delivery;delivery=NULL;*generation=17;
    CHECK(DuplicateHandle(GetCurrentProcess(),frontend_process,GetCurrentProcess(),process,
        SYNCHRONIZE,FALSE,0));
    return ERROR_SUCCESS;
}
static BOOL transfer(BOOL write,void *buffer,DWORD size)
{
    BYTE *cursor=buffer;
    while (size) {
        DWORD done=0;
        BOOL ok=write ? WriteFile(peer,cursor,size,&done,NULL) : ReadFile(peer,cursor,size,&done,NULL);
        if (!ok || !done || done>size) return FALSE;
        cursor+=done;size-=done;
    }
    return TRUE;
}
static DWORD WINAPI serve(void *unused)
{
    console_io_request request;
    console_io_reply reply;
    (void)unused;
    while (transfer(FALSE,&request,(DWORD)offsetof(console_io_request,data))) {
        if (WaitForSingleObject(stop,0)==WAIT_OBJECT_0) break;
        CHECK(request.bytes<=CONSOLE_IO_DATA_BYTES);
        CHECK(transfer(FALSE,request.data,request.bytes));
        CHECK(!run16_console_dispatch(&frontend,&request,&reply));
        CHECK(transfer(TRUE,&reply,(DWORD)offsetof(console_io_reply,data)+reply.bytes));
    }
    CloseHandle(peer);return 0;
}
int main(int argc,char **argv)
{
    WCHAR name[96];
    HANDLE local,thread;
    CONSOLE_SCREEN_BUFFER_INFO info;
    CONSOLE_CURSOR_INFO cursor={20,FALSE},actual;
    COORD p={2,1};
    SMALL_RECT rect={2,1,4,1};
    CHAR_INFO fill;
    DWORD count;
    char cells[4]={0},untouched;
    WORD attributes[3];
    CHAR_INFO grid[12],readback[12];
    COORD grid_size={4,3},grid_origin={1,1};
    SMALL_RECT grid_rect={-1,3,1,4};
    int i;
    HANDLE stable_wait;
    BOOL broken_pipe=argc==2 && !strcmp(argv[1],"--broken-pipe");
    hang_close=argc==2 && !strcmp(argv[1],"--close-hang");
    if (argc==2 && !strcmp(argv[1],"--readiness-peer")) { Sleep(INFINITE);return 0; }
    if (broken_pipe) {
        /* Keep the authenticated presenter process alive while its channel
         * closes. Channel loss must not kill this client or its presenter. */
        CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
            &frontend_process,SYNCHRONIZE,FALSE,0));
    } else {
        WCHAR image[MAX_PATH],command[MAX_PATH+40];
        STARTUPINFOW startup={sizeof(startup)};
        PROCESS_INFORMATION process={0};
        CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
        swprintf_s(command,MAX_PATH+40,L"\"%s\" --readiness-peer",image);
        CHECK(CreateProcessW(NULL,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
        frontend_process=process.hProcess;CloseHandle(process.hThread);
    }
    local=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    frontend.output=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    frontend.input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,OPEN_EXISTING,0,NULL);
    frontend.generation=17;
    CHECK(local!=INVALID_HANDLE_VALUE && frontend.output!=INVALID_HANDLE_VALUE &&
        frontend.input!=INVALID_HANDLE_VALUE);
    CHECK(SetConsoleActiveScreenBuffer(frontend.output));
    swprintf_s(name,96,L"\\\\.\\pipe\\ntvdm-console-fixture-%lu",GetCurrentProcessId());
    delivery=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,32768,32768,0,NULL);
    CHECK(delivery!=INVALID_HANDLE_VALUE);
    peer=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    CHECK(peer!=INVALID_HANDLE_VALUE);
    stop=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(stop);
    thread=CreateThread(NULL,0,serve,NULL,0,NULL);CHECK(thread);
    CHECK(!ntvdm_console_client_begin(&owner));bound=&owner;
    {
        typedef BOOL (WINAPI *query_layout)(LPSTR);
        query_layout query=(query_layout)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
            "GetConsoleKeyboardLayoutNameA");
        char expected[KL_NAMELENGTH]={0},layout[KL_NAMELENGTH+1];
        DWORD sequence=frontend.sequence,error;
        BOOL result;
        CHECK(query!=NULL);
        memset(layout,0x55,sizeof(layout));
        SetLastError(0);result=query(expected);error=GetLastError();
        CHECK(GetConsoleKeyboardLayoutNameA(layout)==result && frontend.sequence==sequence+1);
        if (result) CHECK(!memcmp(layout,expected,KL_NAMELENGTH));
        else CHECK(GetLastError()==error && layout[0]==0x55);
        CHECK(layout[KL_NAMELENGTH]==0x55);
        sequence=frontend.sequence;
        CHECK(!GetConsoleKeyboardLayoutNameA(NULL) && GetLastError()==ERROR_INVALID_PARAMETER &&
            frontend.sequence==sequence);
    }
    {
        POINT expected={123,456},actual={123,456};
        RECT expected_clip={1,2,3,4},actual_clip={1,2,3,4};
        DWORD sequence=frontend.sequence,error;
        BOOL result;
        SetLastError(0);result=native_pointer(&expected);error=GetLastError();
        SetLastError(0);CHECK(GetCursorPos(&actual)==result && frontend.sequence==sequence+1);
        if (!result) CHECK(GetLastError()==error && actual.x==123 && actual.y==456);
        /* A moving desktop pointer need not have identical samples. The
         * private-desktop failure must leave the caller's output untouched. */
        sequence=frontend.sequence;
        SetLastError(0);result=native_clip(&expected_clip);error=GetLastError();
        SetLastError(0);CHECK(GetClipCursor(&actual_clip)==result && frontend.sequence==sequence+1);
        if (result) CHECK(!memcmp(&expected_clip,&actual_clip,sizeof(actual_clip)));
        else CHECK(GetLastError()==error && actual_clip.left==1 && actual_clip.bottom==4);
        sequence=frontend.sequence;
        CHECK(!GetCursorPos(NULL) && GetLastError()==ERROR_INVALID_PARAMETER);
        CHECK(!GetClipCursor(NULL) && GetLastError()==ERROR_INVALID_PARAMETER && frontend.sequence==sequence);
        bound=NULL;
        CHECK(GetCursorPos(&actual)==native_pointer(&expected) && frontend.sequence==sequence);
        bound=&owner;
    }
    {
        char saved[1024]={0},actual_title[32],expected_title[32];
        const char *titles[]={"TITLE-TRANSPORT","","AB","\x82-title"};
        DWORD capacities[]={0,1,4,32},t,c,native_count,native_error,sequence;
        native_title(saved,sizeof(saved));
        for (t=0;t<sizeof(titles)/sizeof(titles[0]);++t) {
            sequence=frontend.sequence;
            CHECK(SetConsoleTitleA(titles[t]) && frontend.sequence==sequence+1);
            for (c=0;c<4;++c) {
                memset(expected_title,0x7e,sizeof(expected_title));
                memset(actual_title,0x7e,sizeof(actual_title));
                SetLastError(0);
                native_count=native_title(expected_title,capacities[c]);native_error=GetLastError();
                sequence=frontend.sequence;SetLastError(0);
                CHECK(GetConsoleTitleA(actual_title,capacities[c])==native_count);
                CHECK(GetLastError()==native_error && frontend.sequence==sequence+1);
                CHECK(!memcmp(actual_title,expected_title,sizeof(actual_title)));
            }
        }
        sequence=frontend.sequence;
        CHECK(!SetConsoleTitleA(NULL) && GetLastError()==ERROR_INVALID_PARAMETER);
        CHECK(!GetConsoleTitleA(actual_title,CONSOLE_IO_DATA_BYTES+1) &&
            GetLastError()==ERROR_INVALID_PARAMETER && frontend.sequence==sequence);
        bound=NULL;
        CHECK(SetConsoleTitleA("NATIVE-TITLE"));
        CHECK(GetConsoleTitleA(actual_title,sizeof(actual_title))==12 &&
            !strcmp(actual_title,"NATIVE-TITLE") && frontend.sequence==sequence);
        bound=&owner;CHECK(native_set_title(saved));
    }
    {
        HWND window=GetConsoleWindow();
        RECT client_rect,after;
        POINT point={40000,-40000},expected=point;
        LONG values[4]={0};
        DWORD sequence=frontend.sequence;
        DWORD attempts;
        CHECK(window && GetClientRect(window,&client_rect));
        CHECK(ntvdm_console_window_query(CONSOLE_WINDOW_ICONIC,values)==1 &&
            values[0]==IsIconic(window));
        /* A new conhost can resize asynchronously. Compare only a query
         * bracketed by an unchanged native rectangle, never stale startup data. */
        for (attempts=0;attempts<10;++attempts) {
            CHECK(GetClientRect(window,&client_rect));
            CHECK(ntvdm_console_window_query(CONSOLE_WINDOW_CLIENT_RECT,values)==1);
            CHECK(GetClientRect(window,&after));
            if (memcmp(&client_rect,&after,sizeof(client_rect))) continue;
            if (values[0]!=client_rect.left || values[1]!=client_rect.top ||
                values[2]!=client_rect.right || values[3]!=client_rect.bottom)
                fprintf(stderr,"rect native=%ld,%ld,%ld,%ld remote=%ld,%ld,%ld,%ld\n",
                    client_rect.left,client_rect.top,client_rect.right,client_rect.bottom,
                    values[0],values[1],values[2],values[3]);
            CHECK(values[0]==client_rect.left && values[1]==client_rect.top &&
                values[2]==client_rect.right && values[3]==client_rect.bottom);
            break;
        }
        CHECK(attempts<10);
        CHECK(ClientToScreen(window,&expected));
        values[0]=point.x;values[1]=point.y;
        CHECK(ntvdm_console_window_query(CONSOLE_WINDOW_CLIENT_TO_SCREEN,values)==1 &&
            values[0]==expected.x && values[1]==expected.y);
        CHECK(frontend.sequence==sequence+3+attempts);
        CHECK(ntvdm_console_window_query(0xffffffffu,values)==0 &&
            GetLastError()==ERROR_CALL_NOT_IMPLEMENTED);
        CHECK(ntvdm_console_window_query(CONSOLE_WINDOW_ICONIC,values)==1);
        bound=NULL;
        CHECK(ntvdm_console_window_query(CONSOLE_WINDOW_ICONIC,values)==-1);
        CHECK(frontend.sequence==sequence+5+attempts);
        bound=&owner;
    }
    {
        DWORD sequence=frontend.sequence;
        UINT input_cp=native_input_cp(),output_cp=native_output_cp();
        CHECK(input_cp && output_cp);
        CHECK(SetConsoleCP(437) && SetConsoleOutputCP(850));
        CHECK(GetConsoleCP()==437 && frontend.sequence==sequence+1);
        CHECK(GetConsoleOutputCP()==850 && frontend.sequence==sequence+2);
        bound=NULL;
        CHECK(GetConsoleCP()==437 && GetConsoleOutputCP()==850);
        CHECK(frontend.sequence==sequence+2);
        CHECK(SetConsoleCP(input_cp) && SetConsoleOutputCP(output_cp));
        bound=&owner;
    }
    stable_wait=ntvdm_console_input_wait_handle();
    {
        CONSOLE_FONT_INFO expected,font;
        COORD size,expected_size;
        DWORD sequence=frontend.sequence,error;
        BOOL maximum;
        for (maximum=FALSE;maximum<=TRUE;++maximum) {
            CHECK(native_font(frontend.output,maximum,&expected));
            CHECK(GetCurrentConsoleFont(local,maximum,&font));
            CHECK(!memcmp(&font,&expected,sizeof(font)));
            expected_size=native_font_size(frontend.output,font.nFont);
            size=GetConsoleFontSize(local,font.nFont);
            CHECK(size.X==expected_size.X && size.Y==expected_size.Y);
        }
        CHECK(frontend.sequence==sequence+4);
        expected_size=native_font_size(frontend.output,0xffffffffu);error=GetLastError();
        size=GetConsoleFontSize(local,0xffffffffu);
        CHECK(size.X==expected_size.X && size.Y==expected_size.Y);
        if (!size.X && !size.Y) CHECK(GetLastError()==error);
        CHECK(frontend.sequence==sequence+5);
        bound=NULL;
        CHECK(native_font(local,FALSE,&expected));
        CHECK(GetCurrentConsoleFont(local,FALSE,&font) && !memcmp(&font,&expected,sizeof(font)));
        expected_size=native_font_size(local,font.nFont);size=GetConsoleFontSize(local,font.nFont);
        CHECK(size.X==expected_size.X && size.Y==expected_size.Y && frontend.sequence==sequence+5);
        bound=&owner;
    }
    {
        DWORD expected=0xdeadbeef,actual_mode=0xdeadbeef,error,sequence=frontend.sequence;
        BOOL native_result,result;
        COORD expected_size={-123,-456},actual_size;
        native_result=native_display(&expected);error=GetLastError();
        result=GetConsoleDisplayMode(&actual_mode);
        CHECK(result==native_result && actual_mode==expected);
        if (!result) CHECK(GetLastError()==error);
        CHECK(frontend.sequence==sequence+1);
        native_result=native_set_display(frontend.output,CONSOLE_WINDOWED_MODE,&expected_size);error=GetLastError();
        actual_size.X=-123;actual_size.Y=-456;
        result=SetConsoleDisplayMode(local,CONSOLE_WINDOWED_MODE,&actual_size);
        CHECK(result==native_result && !memcmp(&actual_size,&expected_size,sizeof(actual_size)));
        if (!result) CHECK(GetLastError()==error);
        CHECK(frontend.sequence==sequence+2);
        {
            HANDLE saved_output=frontend.output;
            frontend.output=INVALID_HANDLE_VALUE;
            expected_size.X=actual_size.X=-123;expected_size.Y=actual_size.Y=-456;
            native_result=native_set_display(frontend.output,CONSOLE_WINDOWED_MODE,&expected_size);error=GetLastError();
            result=SetConsoleDisplayMode(local,CONSOLE_WINDOWED_MODE,&actual_size);
            CHECK(!native_result && !result && GetLastError()==error);
            CHECK(!memcmp(&actual_size,&expected_size,sizeof(actual_size)));
            frontend.output=saved_output;
        }
        CHECK(!GetConsoleDisplayMode(NULL) && GetLastError()==ERROR_INVALID_ACCESS);
        CHECK(!SetConsoleDisplayMode(local,CONSOLE_WINDOWED_MODE,NULL) && GetLastError()==ERROR_INVALID_ACCESS);
        CHECK(frontend.sequence==sequence+3);
        bound=NULL;
        expected=actual_mode=0xdeadbeef;
        native_result=native_display(&expected);error=GetLastError();
        result=GetConsoleDisplayMode(&actual_mode);
        CHECK(result==native_result && actual_mode==expected);
        if (!result) CHECK(GetLastError()==error);
        CHECK(frontend.sequence==sequence+3);
        bound=&owner;
    }
    CHECK(stable_wait && WaitForSingleObject(stable_wait,0)==WAIT_TIMEOUT);
    CHECK(SetEvent(readiness) && WaitForSingleObject(stable_wait,5000)==WAIT_OBJECT_0);
    CHECK(ResetEvent(readiness));
    {
        INPUT_RECORD record;
        DWORD expected_error;
        CHECK(!native_write(frontend.input,"!",&count));expected_error=GetLastError();
        CHECK(!WriteConsoleA(frontend.input,"!",1,&count,NULL) && GetLastError()==expected_error);
        CHECK(!native_cursor(frontend.input,&actual));expected_error=GetLastError();
        CHECK(!GetConsoleCursorInfo(frontend.input,&actual) && GetLastError()==expected_error);
        CHECK(!native_peek(local,&record,&count));expected_error=GetLastError();
        CHECK(!PeekConsoleInputW(local,&record,1,&count) && GetLastError()==expected_error);
        CHECK(!native_read_input(local,&record,&count));expected_error=GetLastError();
        CHECK(!ReadConsoleInputW(local,&record,1,&count) && GetLastError()==expected_error);
    }
    {
        HANDLE null_output=CreateFileW(L"NUL",GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        DWORD expected_error;
        CHECK(null_output!=INVALID_HANDLE_VALUE && GetFileType(null_output)==FILE_TYPE_CHAR);
        CHECK(!native_write(null_output,"N",&count));expected_error=GetLastError();
        CHECK(!WriteConsoleA(null_output,"N",1,&count,NULL) && GetLastError()==expected_error);
        CloseHandle(null_output);
    }
    {
        INPUT_RECORD injected[4]={0},received[4];
        CHECK(FlushConsoleInputBuffer(frontend.input));
        injected[0].EventType=KEY_EVENT;
        injected[0].Event.KeyEvent.bKeyDown=TRUE;
        injected[0].Event.KeyEvent.wRepeatCount=2;
        injected[0].Event.KeyEvent.wVirtualKeyCode='A';
        injected[0].Event.KeyEvent.wVirtualScanCode=30;
        injected[0].Event.KeyEvent.uChar.UnicodeChar=L'A';
        injected[0].Event.KeyEvent.dwControlKeyState=SHIFT_PRESSED;
        injected[1].EventType=MOUSE_EVENT;
        injected[1].Event.MouseEvent.dwMousePosition.X=10;
        injected[1].Event.MouseEvent.dwMousePosition.Y=5;
        injected[1].Event.MouseEvent.dwButtonState=FROM_LEFT_1ST_BUTTON_PRESSED;
        injected[1].Event.MouseEvent.dwEventFlags=MOUSE_MOVED;
        injected[2]=injected[0];injected[2].Event.KeyEvent.bKeyDown=FALSE;
        injected[3]=injected[1];injected[3].Event.MouseEvent.dwButtonState=0;
        injected[3].Event.MouseEvent.dwEventFlags=0;
        CHECK(WriteConsoleInputW(frontend.input,injected,4,&count) && count==4);
        CHECK(PeekConsoleInputW(frontend.input,received,4,&count) && count==4);
        CHECK(!memcmp(injected,received,sizeof(injected)));
        CHECK(ReadConsoleInputW(frontend.input,received,4,&count) && count==4);
        CHECK(!memcmp(injected,received,sizeof(injected)));
        CHECK(PeekConsoleInputW(frontend.input,received,4,&count) && count==0);
        {
            INPUT_RECORD returned[2],native_record;
            returned[0]=injected[0];returned[0].Event.KeyEvent.uChar.UnicodeChar=L'X';
            returned[1]=injected[2];returned[1].Event.KeyEvent.uChar.UnicodeChar=L'X';
            CHECK(WriteConsoleInputW(frontend.input,injected,4,&count) && count==4);
            CHECK(ntvdm_console_prepend_keys(frontend.input,returned,2,&count) && count==2);
            returned[0].Event.KeyEvent.uChar.UnicodeChar=L'Y';
            CHECK(ntvdm_console_prepend_keys(frontend.input,returned,1,&count) && count==1);
            CHECK(native_read_input(frontend.input,&native_record,&count) && count==1 &&
                !memcmp(&native_record,&returned[0],sizeof(native_record)));
            returned[0].Event.KeyEvent.uChar.UnicodeChar=L'X';
            for (i=0;i<2;++i) {
                CHECK(native_read_input(frontend.input,&native_record,&count) && count==1 &&
                    !memcmp(&native_record,&returned[i],sizeof(native_record)));
            }
            for (i=0;i<4;++i) {
                CHECK(native_read_input(frontend.input,&native_record,&count) && count==1 &&
                    !memcmp(&native_record,&injected[i],sizeof(native_record)));
            }
            CHECK(!ntvdm_console_prepend_keys(frontend.input,&injected[1],1,&count) &&
                GetLastError()==ERROR_INVALID_DATA && count==0);
            CHECK(!ntvdm_console_prepend_keys(frontend.input,returned,CONSOLE_IO_INPUT_CAPACITY+1,&count) &&
                GetLastError()==ERROR_INVALID_PARAMETER && count==0);
            CHECK(native_peek(frontend.input,&native_record,&count) && count==0);
            CHECK(ntvdm_console_prepend_keys(frontend.input,NULL,0,&count) && count==0);
        }
    }
    CHECK(SetConsoleCursorPosition(local,p));
    {
        /* Copied records, not physical focus acceptance: preserve ordering
         * through focus loss, releases, resize/menu and focus restoration. */
        INPUT_RECORD injected[7]={0},received[7]={0};
        DWORD before_sequence=frontend.sequence;
        CHECK(FlushConsoleInputBuffer(frontend.input));
        injected[0].EventType=MOUSE_EVENT;
        injected[0].Event.MouseEvent.dwMousePosition.X=79;
        injected[0].Event.MouseEvent.dwMousePosition.Y=49;
        injected[0].Event.MouseEvent.dwButtonState=FROM_LEFT_1ST_BUTTON_PRESSED;
        injected[0].Event.MouseEvent.dwControlKeyState=LEFT_CTRL_PRESSED;
        injected[1].EventType=FOCUS_EVENT;
        injected[1].Event.FocusEvent.bSetFocus=FALSE;
        injected[2].EventType=KEY_EVENT;
        injected[2].Event.KeyEvent.wRepeatCount=1;
        injected[2].Event.KeyEvent.wVirtualKeyCode=VK_CONTROL;
        injected[2].Event.KeyEvent.wVirtualScanCode=0x1d;
        injected[3]=injected[0];
        injected[3].Event.MouseEvent.dwButtonState=0;
        injected[3].Event.MouseEvent.dwControlKeyState=0;
        injected[4].EventType=WINDOW_BUFFER_SIZE_EVENT;
        injected[4].Event.WindowBufferSizeEvent.dwSize.X=80;
        injected[4].Event.WindowBufferSizeEvent.dwSize.Y=50;
        injected[5].EventType=MENU_EVENT;
        injected[5].Event.MenuEvent.dwCommandId=0x12345678;
        injected[6].EventType=FOCUS_EVENT;
        injected[6].Event.FocusEvent.bSetFocus=TRUE;
        CHECK(WriteConsoleInputW(frontend.input,injected,7,&count) && count==7);
        CHECK(PeekConsoleInputW(frontend.input,received,7,&count) && count==7);
        CHECK(!memcmp(injected,received,sizeof(injected)));
        for (i=0;i<7;++i) {
            CHECK(ReadConsoleInputW(frontend.input,received,1,&count) && count==1);
            CHECK(!memcmp(&injected[i],received,sizeof(INPUT_RECORD)));
        }
        CHECK(PeekConsoleInputW(frontend.input,received,7,&count) && count==0);
        CHECK(frontend.sequence==before_sequence+9);
    }
    CHECK(SetConsoleTextAttribute(local,0x1f));
    CHECK(WriteConsoleA(local,"ABC",3,&count,NULL) && count==3);
    CHECK(ReadConsoleOutputCharacterA(frontend.output,cells,3,p,&count) && !memcmp(cells,"ABC",3));
    CHECK(ReadConsoleOutputCharacterA(local,&untouched,1,p,&count) && untouched==' ');
    CHECK(GetConsoleScreenBufferInfo(local,&info) && info.dwCursorPosition.X==5 &&
        info.dwCursorPosition.Y==1 && info.wAttributes==0x1f);
    CHECK(SetConsoleCursorInfo(local,&cursor));
    CHECK(native_cursor(frontend.output,&actual) && actual.dwSize==20 && !actual.bVisible);
    CHECK(GetConsoleCursorInfo(local,&actual) && actual.dwSize==20 && !actual.bVisible);
    {
        DWORD local_mode,remote_mode,observed,input_mode;
        CHECK(native_get_mode(local,&local_mode) && native_get_mode(frontend.output,&remote_mode));
        CHECK(native_set_mode(local,local_mode ^ ENABLE_WRAP_AT_EOL_OUTPUT));
        CHECK(GetConsoleMode(local,&observed) && observed==remote_mode);
        CHECK(SetConsoleMode(local,remote_mode ^ ENABLE_WRAP_AT_EOL_OUTPUT));
        CHECK(native_get_mode(frontend.output,&observed) && observed==(remote_mode ^ ENABLE_WRAP_AT_EOL_OUTPUT));
        CHECK(native_get_mode(local,&observed) && observed==(local_mode ^ ENABLE_WRAP_AT_EOL_OUTPUT));
        CHECK(native_set_mode(local,local_mode) && SetConsoleMode(local,remote_mode));
        CHECK(native_get_mode(frontend.input,&input_mode));
        CHECK(SetConsoleMode(frontend.input,input_mode ^ ENABLE_MOUSE_INPUT));
        CHECK(GetConsoleMode(frontend.input,&observed) && observed==(input_mode ^ ENABLE_MOUSE_INPUT));
        CHECK(native_get_mode(frontend.input,&observed) && observed==(input_mode ^ ENABLE_MOUSE_INPUT));
        CHECK(SetConsoleMode(frontend.input,input_mode));
    }
    CHECK(FillConsoleOutputCharacterA(local,'#',3,p,&count) && count==3);
    CHECK(FillConsoleOutputAttribute(local,0x2e,3,p,&count) && count==3);
    ZeroMemory(&fill,sizeof(fill));fill.Char.AsciiChar=' ';fill.Attributes=7;
    p.Y=2;
    CHECK(ScrollConsoleScreenBufferA(local,&rect,NULL,p,&fill));
    CHECK(ReadConsoleOutputCharacterA(frontend.output,cells,3,p,&count) && !memcmp(cells,"###",3));
    CHECK(ReadConsoleOutputAttribute(frontend.output,attributes,3,p,&count) &&
        attributes[0]==0x2e && attributes[1]==0x2e && attributes[2]==0x2e);
    for (i=0;i<12;i++) {
        grid[i].Char.UnicodeChar=(WCHAR)(0x2500+i);grid[i].Attributes=(WORD)(0x10+i);
    }
    CHECK(WriteConsoleOutputW(local,grid,grid_size,grid_origin,&grid_rect));
    CHECK(grid_rect.Left==0 && grid_rect.Right==1 && grid_rect.Top==3 && grid_rect.Bottom==4);
    memset(readback,0x55,sizeof(readback));
    grid_rect.Left=-1;
    CHECK(ReadConsoleOutputW(local,readback,grid_size,grid_origin,&grid_rect));
    CHECK(grid_rect.Left==0 && grid_rect.Right==1 && grid_rect.Top==3 && grid_rect.Bottom==4);
    CHECK(readback[5].Char.UnicodeChar==0x5555 && readback[9].Char.UnicodeChar==0x5555);
    CHECK(!memcmp(&readback[6],&grid[6],2*sizeof(CHAR_INFO)) &&
        !memcmp(&readback[10],&grid[10],2*sizeof(CHAR_INFO)));
    grid[0].Char.UnicodeChar='Z';grid[0].Attributes=0x3f;
    grid_size.X=1;grid_size.Y=1;grid_origin.X=0;grid_origin.Y=0;
    grid_rect.Left=0;grid_rect.Right=0;grid_rect.Top=5;grid_rect.Bottom=5;
    CHECK(WriteConsoleOutputA(local,grid,grid_size,grid_origin,&grid_rect));
    CHECK(ReadConsoleOutputA(local,readback,grid_size,grid_origin,&grid_rect));
    CHECK(readback[0].Char.AsciiChar=='Z' && readback[0].Attributes==0x3f);
    {
        CHAR_INFO *large=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,5000*sizeof(CHAR_INFO));
        COORD large_size={5000,1},zero={0,0};
        SMALL_RECT large_rect={0,6,4999,6};
        DWORD sequence=frontend.sequence;
        SMALL_RECT native_rect=large_rect;
        CHECK(large);
        for (i=0;i<5000;i++) { large[i].Char.UnicodeChar='T';large[i].Attributes=0x4e; }
        CHECK(native_write_cells(frontend.output,large,large_size,zero,&native_rect));
        CHECK(WriteConsoleOutputW(local,large,large_size,zero,&large_rect));
        CHECK(frontend.sequence==sequence+2 && large_rect.Left==0 &&
            large_rect.Top==6 && large_rect.Right==info.dwSize.X-1);
        CHECK(!memcmp(&large_rect,&native_rect,sizeof(native_rect)));
        memset(large,0x55,5000*sizeof(CHAR_INFO));large_rect.Right=4999;
        CHECK(ReadConsoleOutputW(local,large,large_size,zero,&large_rect));
        CHECK(large[0].Char.UnicodeChar=='T' && large[large_rect.Right].Attributes==0x4e &&
            large[large_rect.Right+1].Char.UnicodeChar==0x5555);
        large_rect.Left=4096;large_rect.Right=4999;native_rect=large_rect;
        CHECK(!native_read_cells(frontend.output,large,large_size,zero,&native_rect) &&
            GetLastError()==ERROR_INVALID_PARAMETER);
        CHECK(!ReadConsoleOutputW(local,large,large_size,zero,&large_rect) &&
            GetLastError()==ERROR_INVALID_PARAMETER);
        CHECK(!memcmp(&large_rect,&native_rect,sizeof(native_rect)));
        HeapFree(GetProcessHeap(),0,large);
    }
    p.X=-1;
    CHECK(!SetConsoleCursorPosition(local,p) && GetLastError()==ERROR_INVALID_PARAMETER);
    p.X=1;
    CHECK(SetConsoleCursorPosition(local,p)); /* A native error must not poison the channel. */
    {
        CHAR_INFO resize_cells[80*25],crop[40*12];
        COORD full={80,25},cropped_size={40,12},zero={0,0},cursor_position={70,20};
        SMALL_RECT window={0,0,19,9},rectangle={0,0,79,24};
        CONSOLE_SCREEN_BUFFER_INFO local_before,local_after;
        int x,y;
        bound=NULL;CHECK(GetConsoleScreenBufferInfo(local,&local_before));bound=&owner;
        CHECK(MvdmSetConsoleWindowInfo(local,TRUE,&window));
        CHECK(GetConsoleScreenBufferInfo(local,&info));
        if (!MvdmSetConsoleScreenBufferSize(local,full)) {
            DWORD remote_error=GetLastError();BOOL native_ok=native_set_size(frontend.output,full);
            fprintf(stderr,"resize remote=%lu native=%d error=%lu\n",remote_error,native_ok,GetLastError());
            CHECK(FALSE);
        }
        for(y=0;y<25;y++) for(x=0;x<80;x++) {
            resize_cells[y*80+x].Char.UnicodeChar=(WCHAR)('A'+y);
            resize_cells[y*80+x].Attributes=(WORD)(x==79 ? 0x1e : 7);
        }
        CHECK(WriteConsoleOutputW(local,resize_cells,full,zero,&rectangle));
        CHECK(SetConsoleCursorPosition(local,cursor_position));
        CHECK(MvdmSetConsoleScreenBufferSize(local,cropped_size));
        CHECK(GetConsoleScreenBufferInfo(local,&info) && info.dwSize.X==40 && info.dwSize.Y==12 &&
            info.dwCursorPosition.X==0 && info.dwCursorPosition.Y==11);
        rectangle.Right=39;rectangle.Bottom=11;
        CHECK(ReadConsoleOutputW(local,crop,cropped_size,zero,&rectangle));
        for(y=0;y<12;y++) for(x=0;x<40;x++) CHECK(crop[y*40+x].Char.UnicodeChar=='A'+y+9);
        CHECK(MvdmSetConsoleScreenBufferSize(local,full));
        bound=NULL;CHECK(GetConsoleScreenBufferInfo(local,&local_after));bound=&owner;
        CHECK(local_after.dwSize.X==local_before.dwSize.X && local_after.dwSize.Y==local_before.dwSize.Y);
    }
    if (broken_pipe) {
        SetEvent(stop);
        goto disconnected;
    }
    CHECK(ReadConsoleOutputCharacterA(local,&untouched,1,p,&count) && untouched==' ');
    CHECK(ResetEvent(stable_wait) && WaitForSingleObject(stable_wait,0)==WAIT_TIMEOUT);
    {
        console_video_description description={0};
        BYTE *pixels=HeapAlloc(GetProcessHeap(),0,64000);
        CHECK(pixels);
        description.width=320;description.height=200;description.stride=320;
        description.depth=8;description.bytes=64000;description.palette[1]=0x123456;
        memset(pixels,0x81,64000);
        CHECK(!ntvdm_console_publish_video(&description,pixels,63999));
        CHECK(GetLastError()==ERROR_INVALID_PARAMETER);
        CHECK(ntvdm_console_publish_video(&description,pixels,64000));
        CHECK(frontend.video.published_serial==1 && frontend.video.description.bytes==64000);
        CHECK(frontend.video.pixels[0]==0x81 && frontend.video.pixels[63999]==0x81);
        CHECK(frontend.video.description.palette[1]==0x123456);
        CHECK(ntvdm_console_publish_video(NULL,NULL,0));
        CHECK(!frontend.video.pixels && !frontend.video.pending && frontend.video.serial==2);
        HeapFree(GetProcessHeap(),0,pixels);
    }
    {
        struct {BITMAPINFOHEADER h;WORD indices[256];} bitmap={0};
        struct {WORD version,count;PALETTEENTRY entries[256];} logical={0};
        CONSOLE_GRAPHICS_BUFFER_INFO graphics={0};
        SMALL_RECT dirty={0,0,7,1},invalid={0,0,8,1};
        HANDLE surface;
        HPALETTE palette,replacement;
        bitmap.h.biSize=sizeof(bitmap.h);bitmap.h.biWidth=8;bitmap.h.biHeight=-2;
        bitmap.h.biPlanes=1;bitmap.h.biBitCount=8;bitmap.h.biSizeImage=16;
        logical.version=0x300;logical.count=256;
        for(i=0;i<256;++i){bitmap.indices[i]=(WORD)i;logical.entries[i].peGreen=(BYTE)i;}
        graphics.lpBitMapInfo=(BITMAPINFO *)&bitmap;
        graphics.dwBitMapInfoLength=sizeof(bitmap);graphics.dwUsage=DIB_PAL_COLORS;
        surface=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,CONSOLE_GRAPHICS_BUFFER,&graphics);
        CHECK(surface!=INVALID_HANDLE_VALUE && graphics.hMutex && graphics.lpBitMap);
        /* Original SrvShowConsoleCursor accepts graphics output too. Keep
         * this bounded: the original show-until-nonnegative loop would hang. */
        CHECK(ShowConsoleCursor(surface,FALSE)==-1);
        CHECK(ShowConsoleCursor(surface,TRUE)==0);
        CHECK(ShowConsoleCursor(surface,FALSE)==-1);
        CHECK(ShowConsoleCursor(local,TRUE)==1); /* independent text counter */
        CHECK(ShowConsoleCursor(frontend.input,TRUE)==-1);
        CHECK(ShowConsoleCursor(local,FALSE)==0); /* rejected input changed nothing */
        CHECK(ShowConsoleCursor(surface,FALSE)==-2);
        CHECK(ShowConsoleCursor(surface,TRUE)==-1);
        CHECK(ShowConsoleCursor(surface,TRUE)==0);
        CHECK(SetConsoleActiveScreenBuffer(surface));
        palette=CreatePalette((LOGPALETTE *)&logical);CHECK(palette);
        CHECK(ntvdm_console_graphics_palette(surface,palette,SYSPAL_STATIC)==1);
        CHECK(WaitForSingleObject(graphics.hMutex,0)==WAIT_OBJECT_0);
        memset(graphics.lpBitMap,17,16);CHECK(ReleaseMutex(graphics.hMutex));
        CHECK(ntvdm_console_graphics_invalidate(surface,&dirty)==1);
        CHECK(frontend.video.pixels && frontend.video.pixels[15]==17);
        CHECK(frontend.video.description.palette[17]==0x001100);
        CHECK(ntvdm_console_graphics_palette(surface,palette,SYSPAL_STATIC)==1);
        CHECK(GetObjectType(palette)==OBJ_PAL);
        logical.entries[17].peRed=99;
        replacement=CreatePalette((LOGPALETTE *)&logical);CHECK(replacement && replacement!=palette);
        CHECK(ntvdm_console_graphics_palette(surface,replacement,SYSPAL_STATIC)==1);
        CHECK(GetPaletteEntries(palette,0,1,logical.entries)==0);
        CHECK(GetObjectType(replacement)==OBJ_PAL);
        CHECK(ntvdm_console_graphics_invalidate(surface,&invalid)==-1 && GetLastError()==ERROR_INVALID_PARAMETER);
        CHECK(SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE)));
        CHECK(!frontend.video.pixels && !frontend.video.pending);
        CHECK(SetConsoleActiveScreenBuffer(surface));
        CHECK(frontend.video.pixels && frontend.video.pixels[15]==17);
        CHECK(SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE)));
        CHECK(ShowConsoleCursor(surface,FALSE)==-1);
        CHECK(CloseHandle(surface));
        CHECK(ShowConsoleCursor(surface,TRUE)==-1); /* retired output */
        CHECK(WaitForSingleObject(graphics.hMutex,0)==WAIT_OBJECT_0);
        CHECK(ReleaseMutex(graphics.hMutex));CHECK(CloseHandle(graphics.hMutex));
        CHECK(GetPaletteEntries(replacement,0,1,logical.entries)==0);
        surface=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,CONSOLE_GRAPHICS_BUFFER,&graphics);
        CHECK(surface!=INVALID_HANDLE_VALUE);
        CHECK(ShowConsoleCursor(surface,FALSE)==-1); /* fresh surface reset */
        CHECK(ShowConsoleCursor(surface,TRUE)==0);
        CHECK(CloseHandle(surface));CHECK(CloseHandle(graphics.hMutex));
    }
    SetEvent(stop);
    CHECK(TerminateProcess(frontend_process,23));
    CHECK(WaitForSingleObject(frontend_process,5000)==WAIT_OBJECT_0);
    Sleep(10000);
    CHECK(!"root close did not terminate fixture");
disconnected:
    /* An I/O failure is not execution termination. Repeated calls must return
     * the same explicit error, without falling back to the local Console. */
    for (i=0;i<3;++i) {
        CHECK(!WriteConsoleA(local,"!",1,&count,NULL) && GetLastError()==ERROR_PIPE_NOT_CONNECTED);
        CHECK(owner.console_client);
    }
    {
        INPUT_RECORD record;
        CHECK(!ReadConsoleInputW(frontend.input,&record,1,&count) && GetLastError()==ERROR_PIPE_NOT_CONNECTED);
        CHECK(!GetConsoleScreenBufferInfo(local,&info) && GetLastError()==ERROR_PIPE_NOT_CONNECTED);
        CHECK(!SetConsoleMode(frontend.input,ENABLE_MOUSE_INPUT) && GetLastError()==ERROR_PIPE_NOT_CONNECTED);
    }
    cleanup(cleanup_context);bound=NULL;
    CHECK(!owner.console_client);
    CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
    CloseHandle(thread);CloseHandle(stop);CloseHandle(local);CloseHandle(frontend.output);CloseHandle(frontend.input);
    CloseHandle(readiness);CloseHandle(frontend_process);
    puts("PASS client transport, distinct frontend ownership, native error and idle frontend loss");
    return 0;
}
