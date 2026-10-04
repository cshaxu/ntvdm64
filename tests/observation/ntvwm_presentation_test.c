#include "ntvwm-exe/presentation.h"
#include "ntvwm-exe/console_state.h"
#include "ntcon-exe/console_video.h"
#include "common/console/client.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
static FILE *log;
static unsigned checks,failures;
#define CHECK(x) do { ++checks; if(!(x)){++failures;fprintf(log,"FAIL %d %s\n",__LINE__,#x);} } while(0)
typedef struct peer_state {
    HANDLE pipe;
    unsigned mode,calls;
    unsigned activations,releases;
    unsigned snapshot_begins,snapshot_ends,screen_reads;
    unsigned publication_begins,publication_ends,publication_aborts;
    unsigned titles,cell_writes;
    console_io_state screen;
    frontend_video video;
    console_io_input returned[2*CONSOLE_IO_INPUT_CAPACITY+3];
    DWORD returned_count;
    LONG page_shift;
} peer_state;
static BOOL transfer(HANDLE pipe,BOOL write,void *data,DWORD bytes)
{
    BYTE *cursor=data;DWORD done;
    while(bytes) {
        if(!(write ? WriteFile(pipe,cursor,bytes,&done,NULL) : ReadFile(pipe,cursor,bytes,&done,NULL)) || !done)
            return FALSE;
        cursor+=done;bytes-=done;
    }
    return TRUE;
}
static DWORD WINAPI peer(void *context)
{
    peer_state *state=context;DWORD sequence=0;
    for(;;) {
        console_io_request request;console_io_reply reply={0};DWORD error=0;
        if(!transfer(state->pipe,FALSE,&request,offsetof(console_io_request,data)))break;
        if(request.bytes>CONSOLE_IO_DATA_BYTES || request.version!=CONSOLE_IO_VERSION ||
            request.generation!=17 || request.sequence!=++sequence)return ERROR_INVALID_DATA;
        if(!transfer(state->pipe,FALSE,request.data,request.bytes))return ERROR_BROKEN_PIPE;
        ++state->calls;
        if(request.operation==CONSOLE_IO_WRITE_CELLS_W)++state->cell_writes;
        /* The sender must never acquire the frontend implicitly. */
        if(request.operation==CONSOLE_IO_ACTIVATE)return ERROR_INVALID_FUNCTION;
        if(state->mode==0 || (state->mode==6 && sequence<=5)) {
            if(sequence==1)error=ERROR_NOT_READY;
            else if(request.operation==CONSOLE_IO_VIDEO_BEGIN) {
                console_video_description description;
                if(request.bytes!=sizeof(description))return ERROR_INVALID_DATA;
                memcpy(&description,request.data,sizeof(description));
                error=frontend_video_begin(&state->video,request.state.mode,&description);
            } else if(request.operation==CONSOLE_IO_VIDEO_DATA)
                error=frontend_video_data(&state->video,request.state.mode,request.state.count,
                    request.data,request.bytes);
            else if(request.operation!=CONSOLE_IO_BARRIER && state->mode!=6)return ERROR_INVALID_FUNCTION;
        }
        reply.version=CONSOLE_IO_VERSION;reply.generation=17;reply.sequence=sequence;
        reply.result=error==0;reply.error=error;
        if(state->mode==7 || state->mode==8 || state->mode==19) {
            console_io_input events[3]={0};
            if(request.operation!=CONSOLE_IO_READ_INPUT)return ERROR_INVALID_FUNCTION;
            events[0].type=KEY_EVENT;events[0].key_down=1;events[0].repeat=1;
            events[0].virtual_key='A';events[0].scan=30;events[0].character='a';
            events[1]=events[0];events[1].key_down=0;
            events[2].type=MOUSE_EVENT;events[2].x=3;events[2].y=2;
            events[2].flags=MOUSE_MOVED;
            if(state->mode==8)events[2].type=0xffffffffu;
            if(state->mode==19) {
                ZeroMemory(events,sizeof(events));
                events[0].type=events[1].type=events[2].type=CONSOLE_INPUT_FRAME_MOUSE;
                events[0].menu=events[1].menu=640;
                events[0].focus=events[1].focus=400;
                events[0].flags=CONSOLE_MOUSE_ENTER;
                events[1].flags=CONSOLE_MOUSE_MOVE;events[1].x=INT_MAX;events[1].y=INT_MAX;
                events[1].buttons=1;events[1].control=SHIFT_PRESSED;
                events[2].flags=CONSOLE_MOUSE_LEAVE;
            }
            reply.state.count=3;reply.bytes=sizeof(events);
            memcpy(reply.data,events,sizeof(events));
        }
        if((state->mode>=9 && state->mode<=16) || state->mode==20 ||
            (state->mode==6 && sequence>5)) {
            if(request.operation==CONSOLE_IO_PUBLICATION_BEGIN) {
                if(state->publication_begins!=state->publication_ends+state->publication_aborts)error=ERROR_BUSY;
                else ++state->publication_begins;
            } else if(request.operation==CONSOLE_IO_PUBLICATION_END || request.operation==CONSOLE_IO_PUBLICATION_ABORT) {
                if(state->publication_begins!=state->publication_ends+state->publication_aborts+1)error=ERROR_INVALID_STATE;
                else if(request.operation==CONSOLE_IO_PUBLICATION_END)++state->publication_ends;
                else ++state->publication_aborts;
            } else if(request.operation==CONSOLE_IO_SNAPSHOT_BEGIN) {
                if(state->snapshot_begins!=state->snapshot_ends)error=ERROR_BUSY;
                else ++state->snapshot_begins;
            } else if(request.operation==CONSOLE_IO_SNAPSHOT_END) {
                if(state->snapshot_begins!=state->snapshot_ends+1)error=ERROR_INVALID_STATE;
                else ++state->snapshot_ends;
            } else if(request.operation==CONSOLE_IO_ACTIVATE) {
                if(request.state.input)++state->activations;else ++state->releases;
            } else if(request.operation==CONSOLE_IO_PUBLISH_TITLE_A) {
                if(!request.bytes || request.bytes>CONSOLE_IO_TITLE_BYTES ||
                    request.data[request.bytes-1] ||
                    strlen((const char *)request.data)+1!=request.bytes)return ERROR_INVALID_DATA;
                ++state->titles;
            } else if(request.operation==CONSOLE_IO_SCREEN_INFO) {
                ++state->screen_reads;
                reply.state=state->screen;
                if(state->mode==10 && state->screen_reads>1)reply.state.x=5;
            } else if(request.operation==CONSOLE_IO_BUFFER_SIZE) {
                /* Reject the exact regression: shrink before the previous
                 * viewport fits. This peer models only copied geometry;
                 * the real NTMon integration separately verifies conhost. */
                if(request.state.width<=state->screen.right || request.state.height<=state->screen.bottom)
                    error=ERROR_INVALID_PARAMETER;
                else {state->screen.width=request.state.width;state->screen.height=request.state.height;}
            } else if(request.operation==CONSOLE_IO_WINDOW_RECT) {
                if(request.state.right>=state->screen.width || request.state.bottom>=state->screen.height)
                    error=ERROR_INVALID_PARAMETER;
                else {
                    state->screen.left=request.state.left;state->screen.right=request.state.right;
                    state->screen.top=request.state.top;state->screen.bottom=request.state.bottom;
                }
            } else if(request.operation==CONSOLE_IO_GET_CURSOR_INFO) {
                reply.state.cursor_size=25;reply.state.cursor_visible=1;
                if(state->mode==12)reply.state.cursor_size=0;
            } else if(request.operation==CONSOLE_IO_READ_TEXT_CONFIGURATION) {
                if(state->mode<13)error=ERROR_NOT_FOUND;
                else {
                    console_text_configuration configuration={0};DWORD offset=request.state.count;
                    configuration.style.font_height=state->mode==15 ? 33 : 16;
                    memset(configuration.style.fonts,0xa5,sizeof(configuration.style.fonts));
                    configuration.palette[1]=0x123456;
                    if(offset>=sizeof(configuration))return ERROR_INVALID_DATA;
                    reply.state.mode=state->mode==14 && offset ? 8 : 7;
                    reply.state.count=sizeof(configuration);
                    reply.bytes=min(sizeof(configuration)-offset,CONSOLE_IO_DATA_BYTES);
                    memcpy(reply.data,(BYTE *)&configuration+offset,reply.bytes);
                }
            } else if(request.operation==CONSOLE_IO_READ_CELLS_W) {
                DWORD i;
                if(request.state.width!=20 || request.state.height<1 || request.state.top<0 ||
                    request.state.top+request.state.height>8)return ERROR_INVALID_DATA;
                reply.state=request.state;reply.bytes=20*request.state.height*sizeof(console_io_cell);
                for(i=0;i<20*(DWORD)request.state.height;++i) {
                    console_io_cell cell={(uint16_t)('A'+request.state.top+i/20+
                        InterlockedCompareExchange(&state->page_shift,0,0)),7};
                    memcpy(reply.data+i*sizeof(cell),&cell,sizeof(cell));
                }
            } else if(request.operation==CONSOLE_IO_PREPEND_KEYS && state->mode==16) {
                DWORD count=request.state.count;
                if(!count || count>CONSOLE_IO_INPUT_CAPACITY ||
                    request.bytes!=count*sizeof(console_io_input) ||
                    count>ARRAYSIZE(state->returned)-state->returned_count)return ERROR_INVALID_DATA;
                memmove(state->returned+count,state->returned,state->returned_count*sizeof(console_io_input));
                memcpy(state->returned,request.data,request.bytes);state->returned_count+=count;
                reply.state.count=count;
            } else if(request.operation==CONSOLE_IO_VIDEO_BEGIN) {
                console_video_description description;
                if(request.bytes!=sizeof(description))return ERROR_INVALID_DATA;
                memcpy(&description,request.data,sizeof(description));
                error=frontend_video_begin(&state->video,request.state.mode,&description);
            } else if(request.operation==CONSOLE_IO_VIDEO_DATA)
                error=frontend_video_data(&state->video,request.state.mode,request.state.count,request.data,request.bytes);
            else if(request.operation!=CONSOLE_IO_BARRIER && request.operation!=CONSOLE_IO_SET_MODE &&
                request.operation!=CONSOLE_IO_WRITE_CELLS_W && request.operation!=CONSOLE_IO_CURSOR_POSITION &&
                request.operation!=CONSOLE_IO_CURSOR_INFO && request.operation!=CONSOLE_IO_ATTRIBUTE)
                return ERROR_INVALID_FUNCTION;
            reply.result=error==0;reply.error=error;
        }
        if(state->mode==17 || state->mode==18) {
            reply.bytes=KL_NAMELENGTH;
            memcpy(reply.data,"00000409",KL_NAMELENGTH);
        }
        if(state->mode==1)++reply.generation;
        if(state->mode==2)++reply.version;
        if(state->mode==3)reply.error=ERROR_ACCESS_DENIED;
        if(state->mode==4) {
            transfer(state->pipe,TRUE,&reply,8);break;
        }
        if(!transfer(state->pipe,TRUE,&reply,offsetof(console_io_reply,data)+reply.bytes))break;
    }
    DisconnectNamedPipe(state->pipe);return 0;
}
static void run_case(unsigned mode)
{
    WCHAR name[120];HANDLE pipe=NULL,client=INVALID_HANDLE_VALUE,thread=NULL,stop=NULL,process=NULL;
    ntvwm_presentation *endpoint=NULL;peer_state state={0};DWORD error,exit_code=0;
    console_io_request request={0};console_io_reply reply;
    swprintf_s(name,120,L"\\\\.\\pipe\\ntvwm-presentation-test-%lu-%u",GetCurrentProcessId(),mode);
    pipe=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,65536,65536,0,NULL);
    CHECK(pipe!=INVALID_HANDLE_VALUE);if(pipe==INVALID_HANDLE_VALUE)return;
    client=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
    CHECK(client!=INVALID_HANDLE_VALUE);if(client==INVALID_HANDLE_VALUE)goto done;
    CHECK(ConnectNamedPipe(pipe,NULL) || GetLastError()==ERROR_PIPE_CONNECTED);
    stop=CreateEventW(NULL,TRUE,mode==5,NULL);CHECK(stop!=NULL);if(!stop)goto done;
    state.pipe=pipe;state.mode=mode;
    state.screen.width=20;state.screen.height=8;state.screen.right=19;state.screen.bottom=7;
    state.screen.x=4;state.screen.y=2;state.screen.attribute=7;
    if(mode==6) {
        state.screen.width=120;state.screen.height=9001;
        state.screen.right=119;state.screen.bottom=29;
    }
    thread=CreateThread(NULL,0,peer,&state,0,NULL);CHECK(thread!=NULL);if(!thread)goto done;
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
        &process,SYNCHRONIZE,FALSE,0));
    CHECK(ntvwm_presentation_open(client,process,stop,17,&endpoint)==0);
    if(!endpoint)goto done;
    if(mode==17 || mode==18) {
        request.operation=mode==17 ? CONSOLE_IO_KEYBOARD_LAYOUT : CONSOLE_IO_BARRIER;
        error=ntvwm_presentation_call(endpoint,&request,&reply);
        CHECK(error==(mode==17 ? ERROR_SUCCESS : ERROR_INVALID_DATA));
        if(!error)CHECK(reply.bytes==KL_NAMELENGTH && !memcmp(reply.data,"00000409",KL_NAMELENGTH));
        if(mode==18)CHECK(ntvwm_presentation_call(endpoint,&request,&reply)==ERROR_INVALID_DATA);
        goto done;
    }
    if(mode==7 || mode==8 || mode==19) {
        HANDLE input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        INPUT_RECORD records[4];DWORD accepted=99,count=0,original_mode=0;
        CHECK(input!=INVALID_HANDLE_VALUE);
        if(input!=INVALID_HANDLE_VALUE) {
            CHECK(GetConsoleMode(input,&original_mode));
            CHECK(SetConsoleMode(input,ENABLE_MOUSE_INPUT|ENABLE_EXTENDED_FLAGS));
            CHECK(FlushConsoleInputBuffer(input));
            error=ntvwm_presentation_input(endpoint,input,&accepted);
            CHECK(error==(mode==8 ? ERROR_INVALID_DATA : ERROR_SUCCESS));
            CHECK(accepted==(mode==8 ? 0u : 3u));
            CHECK(PeekConsoleInputW(input,records,4,&count));
            fprintf(log,"input case=%u accepted=%lu queued=%lu\n",mode,accepted,count);
            for(DWORD index=0;index<count;++index)
                fprintf(log,"input record=%lu type=%u flags=%lu\n",index,records[index].EventType,
                    records[index].EventType==MOUSE_EVENT ? records[index].Event.MouseEvent.dwEventFlags : 0);
            CHECK(count==(mode==8 ? 0u : 3u));
            if(mode==19 && count==3) {
                ntvwm_capture capture={0};CHECK(!ntvwm_capture_begin(&capture));
                CHECK(records[0].EventType==MOUSE_EVENT && records[0].Event.MouseEvent.dwEventFlags==MOUSE_MOVED);
                CHECK(records[0].Event.MouseEvent.dwMousePosition.X==capture.info.srWindow.Right &&
                    records[0].Event.MouseEvent.dwMousePosition.Y==capture.info.srWindow.Bottom);
                CHECK(!records[0].Event.MouseEvent.dwButtonState);
                CHECK(records[1].Event.MouseEvent.dwButtonState==FROM_LEFT_1ST_BUTTON_PRESSED &&
                    records[1].Event.MouseEvent.dwControlKeyState==SHIFT_PRESSED);
                CHECK(records[2].EventType==MOUSE_EVENT && !records[2].Event.MouseEvent.dwButtonState);
                ntvwm_capture_end(&capture);
            }
            if(mode==7 && count==3) {
                CHECK(records[0].EventType==KEY_EVENT && records[0].Event.KeyEvent.bKeyDown &&
                    records[0].Event.KeyEvent.uChar.UnicodeChar=='a');
                CHECK(records[1].EventType==KEY_EVENT && !records[1].Event.KeyEvent.bKeyDown);
                CHECK(records[2].EventType==MOUSE_EVENT && !records[2].Event.MouseEvent.dwButtonState &&
                    records[2].Event.MouseEvent.dwMousePosition.X==3 && records[2].Event.MouseEvent.dwMousePosition.Y==2);
            }
            if(mode==8)CHECK(ntvwm_presentation_input(endpoint,input,&accepted)==ERROR_INVALID_DATA && !accepted);
            CHECK(FlushConsoleInputBuffer(input));CHECK(SetConsoleMode(input,original_mode));
            CloseHandle(input);
        }
        goto done;
    }
    if(mode>=9 && mode!=17 && mode!=18 && mode!=19) {
        HANDLE buffer=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
        CONSOLE_SCREEN_BUFFER_INFO before,after;COORD origin={0,0};WCHAR cell=0;DWORD count;
        CHECK(buffer!=INVALID_HANDLE_VALUE);
        if(buffer!=INVALID_HANDLE_VALUE) {
            CHECK(GetConsoleScreenBufferInfo(buffer,&before));
            CHECK(WriteConsoleOutputCharacterW(buffer,L"Z",1,origin,&count) && count==1);
            error=mode>=11 ? ntvwm_presentation_begin(endpoint,buffer) : ntvwm_presentation_seed(endpoint,buffer);
            CHECK(error==(DWORD)(mode==10 ? ERROR_RETRY : mode==12 || mode==14 || mode==15 ? ERROR_INVALID_DATA : ERROR_SUCCESS));
            CHECK(GetConsoleScreenBufferInfo(buffer,&after));
            CHECK(ReadConsoleOutputCharacterW(buffer,&cell,1,origin,&count) && count==1);
            if(mode==9 || mode==11 || mode==13 || mode==16 || mode==20) {
                COORD last={19,7};
                CHECK(cell=='A' && after.dwSize.X==20 && after.dwSize.Y==8);
                CHECK(after.dwCursorPosition.X==4 && after.dwCursorPosition.Y==2);
                CHECK(ReadConsoleOutputCharacterW(buffer,&cell,1,last,&count) && count==1 && cell=='H');
                if(mode!=20) {
                    CHECK(WriteConsoleW(buffer,L"N",1,&count,NULL) && count==1);
                    CHECK(ReadConsoleOutputCharacterW(buffer,&cell,1,after.dwCursorPosition,&count) && cell=='N');
                }
            } else CHECK(cell=='Z' && before.dwSize.X==after.dwSize.X && before.dwSize.Y==after.dwSize.Y);
            if(mode==9) {
                CONSOLE_SCREEN_BUFFER_INFOEX history={sizeof(history)};
                CONSOLE_CURSOR_INFO cursor={25,TRUE};
                COORD tail={19,39},page={0,7};
                unsigned repeat;
                CHECK(GetConsoleScreenBufferInfoEx(buffer,&history));
                history.dwSize.Y=40;history.srWindow.Top=10;history.srWindow.Bottom=17;
                history.dwCursorPosition.Y=12;
                CHECK(ntvwm_screen_apply(buffer,&history,&cursor)==0);
                CHECK(WriteConsoleOutputCharacterW(buffer,L"P",1,origin,&count) && count==1);
                CHECK(WriteConsoleOutputCharacterW(buffer,L"T",1,tail,&count) && count==1);
                for(repeat=0;repeat<2;++repeat) {
                    CHECK(ntvwm_presentation_seed(endpoint,buffer)==0);
                    CHECK(GetConsoleScreenBufferInfo(buffer,&after));
                    CHECK(after.dwSize.X==20 && after.dwSize.Y==8 &&
                        after.srWindow.Top==0 && after.srWindow.Bottom==7 &&
                        after.dwCursorPosition.X==4 && after.dwCursorPosition.Y==2);
                    CHECK(ReadConsoleOutputCharacterW(buffer,&cell,1,origin,&count) && count==1 && cell=='A');
                    CHECK(ReadConsoleOutputCharacterW(buffer,&cell,1,tail,&count) && count==0);
                    CHECK(ReadConsoleOutputCharacterW(buffer,&cell,1,page,&count) && count==1 && cell=='H');
                }
            }
            if(mode==20) {
                CONSOLE_SCREEN_BUFFER_INFOEX history={sizeof(history)};
                CONSOLE_CURSOR_INFO cursor={25,TRUE};
                COORD first={0,0},second={0,1},shifted={0,2},last={0,7};
                WCHAR value=0;
                CHECK(GetConsoleScreenBufferInfoEx(buffer,&history));
                history.dwSize.Y=40;history.srWindow.Top=0;history.srWindow.Bottom=7;
                CHECK(ntvwm_screen_apply(buffer,&history,&cursor)==0);
                InterlockedExchange(&state.page_shift,2);
                CHECK(ntvwm_presentation_seed(endpoint,buffer)==0);
                CHECK(GetConsoleScreenBufferInfo(buffer,&after));
                CHECK(after.dwSize.Y==8 && after.srWindow.Top==0 &&
                    after.dwCursorPosition.Y==2);
                CHECK(ReadConsoleOutputCharacterW(buffer,&value,1,first,&count) && value=='C');
                CHECK(ReadConsoleOutputCharacterW(buffer,&value,1,second,&count) && value=='D');
                CHECK(ReadConsoleOutputCharacterW(buffer,&value,1,shifted,&count) && value=='E');
                CHECK(ReadConsoleOutputCharacterW(buffer,&value,1,last,&count) && value=='J');
            }
            if(mode==11 || mode==13 || mode==16) {
                HANDLE original=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
                    FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
                console_text_style font={0};font.font_height=14;
                CHECK(original!=INVALID_HANDLE_VALUE);
                if(original!=INVALID_HANDLE_VALUE) {
                    if(mode==16) {
                        INPUT_RECORD keys[2*CONSOLE_IO_INPUT_CAPACITY+3]={0};DWORD i,written;
                        HANDLE input=GetStdHandle(STD_INPUT_HANDLE);
                        CHECK(FlushConsoleInputBuffer(input));
                        for(i=0;i<ARRAYSIZE(keys);++i) {
                            keys[i].EventType=KEY_EVENT;keys[i].Event.KeyEvent.bKeyDown=TRUE;
                            keys[i].Event.KeyEvent.wRepeatCount=1;
                            keys[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0x100+i);
                        }
                        CHECK(WriteConsoleInputW(input,keys,ARRAYSIZE(keys),&written) && written==ARRAYSIZE(keys));
                    }
                    CHECK(SetConsoleActiveScreenBuffer(buffer));
                    CHECK(ntvwm_presentation_end(endpoint,&font)==0);
                    if(mode==16) {
                        DWORD remaining=MAXDWORD;
                        CHECK(GetNumberOfConsoleInputEvents(GetStdHandle(STD_INPUT_HANDLE),&remaining) && !remaining);
                    }
                    CHECK(SetConsoleActiveScreenBuffer(original));CloseHandle(original);
                }
            }
            CloseHandle(buffer);
        }
        goto done;
    }
    request.operation=CONSOLE_IO_BARRIER;
    error=ntvwm_presentation_call(endpoint,&request,&reply);
    fprintf(log,"case=%u first_error=%lu reply_error=%lu\n",mode,(unsigned long)error,(unsigned long)reply.error);
    if(mode==0 || mode==6) {
        console_video_description description={0};console_text_style *style;
        BYTE *payload;
        CHECK(error==ERROR_NOT_READY);
        description.kind=CONSOLE_VIDEO_TEXT_FRAME;description.width=80;description.height=25;
        description.stride=160;description.bytes=sizeof(console_text_style)+4000;
        payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,description.bytes);CHECK(payload!=NULL);
        if(!payload)goto done;
        style=(console_text_style *)payload;style->font_height=14;
        memset(style->fonts,0x5a,sizeof(style->fonts));
        memset(payload+sizeof(*style),'A',4000);
        CHECK(ntvwm_presentation_text(endpoint,&description,payload,description.bytes)==0);
        CHECK(ntvwm_presentation_call(endpoint,&request,&reply)==0);
        if(mode==6) {
            unsigned calls=state.calls;
            /* Sampling decides whether to generate an update. Explicit
             * worker frame sends must never be filtered by the transport. */
            CHECK(ntvwm_presentation_text(endpoint,&description,payload,description.bytes)==0);
            CHECK(state.calls==calls+3);
        }
        description.kind=CONSOLE_VIDEO_DIB;
        CHECK(ntvwm_presentation_text(endpoint,&description,payload,description.bytes)==ERROR_INVALID_PARAMETER);
        description.kind=CONSOLE_VIDEO_TEXT_FRAME;
        CHECK(ntvwm_presentation_text(endpoint,&description,payload,description.bytes-1)==ERROR_INVALID_PARAMETER);
        if(mode==6) {
            HANDLE original=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
                FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
            HANDLE buffer=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
                FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
            COORD size={80,25},position={3,2},origin={0,0};SMALL_RECT window={0,0,79,24};DWORD written;
            CONSOLE_CURSOR_INFO cursor={25,TRUE};
            CHECK(original!=INVALID_HANDLE_VALUE && buffer!=INVALID_HANDLE_VALUE);
            if(original!=INVALID_HANDLE_VALUE && buffer!=INVALID_HANDLE_VALUE) {
                CONSOLE_SCREEN_BUFFER_INFO info;COORD capacity;
                CHECK(GetConsoleScreenBufferInfo(buffer,&info));
                /* A narrow RDP desktop may permit only 53x14 at the current
                 * host font. This is a capture test, not a desktop resize. */
                size.X=min(size.X,info.dwMaximumWindowSize.X);
                size.Y=min(size.Y,info.dwMaximumWindowSize.Y);
                CHECK(size.X>position.X && size.Y>position.Y);
                window.Right=size.X-1;window.Bottom=size.Y-1;
                capacity.X=max(info.dwSize.X,size.X);capacity.Y=max(info.dwSize.Y,size.Y);
                CHECK(SetConsoleScreenBufferSize(buffer,capacity));
                CHECK(SetConsoleActiveScreenBuffer(buffer));
                {
                    BOOL ok=SetConsoleWindowInfo(buffer,TRUE,&window);
                    fprintf(log,"capture_window ok=%d error=%lu previous=%dx%d max=%dx%d\n",
                        ok,ok ? 0 : GetLastError(),info.dwSize.X,info.dwSize.Y,
                        info.dwMaximumWindowSize.X,info.dwMaximumWindowSize.Y);
                    CHECK(ok);
                }
                CHECK(SetConsoleScreenBufferSize(buffer,size));
                CHECK(WriteConsoleOutputCharacterW(buffer,L"Z",1,origin,&written) && written==1);
                CHECK(SetConsoleCursorPosition(buffer,position));
                CHECK(SetConsoleCursorInfo(buffer,&cursor));
                CHECK(SetConsoleActiveScreenBuffer(buffer));
                CHECK(ntvwm_presentation_capture(endpoint,style)==0);
                {
                    unsigned calls=state.calls;
                    unsigned publications=state.publication_begins;
                    unsigned writes=state.cell_writes;
                    /* A real unchanged Console capture must not emit another
                     * publication and thereby repaint the visible Console. */
                    CHECK(ntvwm_presentation_capture(endpoint,style)==0);
                    CHECK(state.calls==calls);
                    CHECK(state.publication_begins==publications);
                    CHECK(SetConsoleCursorPosition(buffer,(COORD){4,2}));
                    CHECK(!ntvwm_presentation_capture(endpoint,style));
                    CHECK(state.publication_begins==publications+1);
                    CHECK(((console_text_style *)state.video.pixels)->cursor_column==4);
                    CHECK(state.cell_writes==writes);
                    calls=state.calls;
                    CHECK(!ntvwm_presentation_capture(endpoint,style));
                    CHECK(state.calls==calls);
                    CHECK(SetConsoleCursorPosition(buffer,position));
                    CHECK(!ntvwm_presentation_capture(endpoint,style));
                    CHECK(state.cell_writes==writes);
                    CHECK(WriteConsoleOutputCharacterW(buffer,L"Q",1,origin,&written) && written==1);
                    CHECK(!ntvwm_presentation_capture(endpoint,style));
                    CHECK(state.video.pixels[sizeof(console_text_style)]=='Q');
                    CHECK(state.cell_writes>writes);writes=state.cell_writes;
                    calls=state.calls;
                    CHECK(!ntvwm_presentation_capture(endpoint,style));
                    CHECK(state.calls==calls);
                    CHECK(state.cell_writes==writes);
                    CHECK(WriteConsoleOutputCharacterW(buffer,L"Z",1,origin,&written) && written==1);
                    CHECK(!ntvwm_presentation_capture(endpoint,style));
                }
                CHECK(SetConsoleActiveScreenBuffer(original));
            }
            if(buffer!=INVALID_HANDLE_VALUE)CloseHandle(buffer);
            if(original!=INVALID_HANDLE_VALUE)CloseHandle(original);
        }
        HeapFree(GetProcessHeap(),0,payload);
    } else {
        CHECK(error==(DWORD)(mode==4 ? ERROR_PIPE_NOT_CONNECTED : mode==5 ? ERROR_OPERATION_ABORTED : ERROR_INVALID_DATA));
        /* Framing/cancellation failure cannot reuse a desynchronized stream. */
        CHECK(ntvwm_presentation_call(endpoint,&request,&reply)==error);
    }
done:
    ntvwm_presentation_close(endpoint);
    if(client!=INVALID_HANDLE_VALUE)CloseHandle(client);
    if(thread) {
        DWORD wait=WaitForSingleObject(thread,3000);
        if(wait!=WAIT_OBJECT_0) {
            CancelSynchronousIo(thread);wait=WaitForSingleObject(thread,3000);
        }
        CHECK(wait==WAIT_OBJECT_0);
        /* Do not unwind stack storage while a failed peer still owns it. */
        if(wait!=WAIT_OBJECT_0)ExitProcess(3);
        CHECK(GetExitCodeThread(thread,&exit_code) && exit_code==0);
        CloseHandle(thread);
    }
    fprintf(log,"mode=%u calls=%u activations=%u releases=%u\n",mode,state.calls,
        state.activations,state.releases);
    if(mode==0 || mode==6) {
        CHECK(mode==6 ? state.calls>8u : state.calls==5u);
        if(mode==6)CHECK(state.screen.width<=80 && state.screen.height<=25 &&
            state.screen.right<state.screen.width && state.screen.bottom<state.screen.height);
        if(mode==6)CHECK(state.titles==1);
        CHECK(state.video.published_serial==(mode==6 ? 7u : 1u) && state.video.description.kind==CONSOLE_VIDEO_TEXT_FRAME);
        CHECK(state.video.pixels && state.video.pixels[sizeof(console_text_style)]==(mode==6 ? 'Z' : 'A'));
        if(mode==6 && state.video.pixels) {
            const console_text_style *style=(const console_text_style *)state.video.pixels;
            CHECK(style->cursor_column==3 && style->cursor_row==2 && style->cursor_visible);
            CHECK(style->fonts[0][0][0]==0x5a);
        }
    } else if(mode>=17 && mode!=20)CHECK(state.calls==1);
    else CHECK(mode==11 || mode>=13 ? state.calls>=6u :
        state.calls==(mode==5 ? 0u : mode==12 ? 4u : mode==10 ? 6u : mode==9 ? 21u : 1u));
    if((mode>=9 && mode<=16) || mode==20)CHECK(state.snapshot_begins==state.snapshot_ends &&
        state.snapshot_begins==(mode==9 ? 3u : mode==20 ? 2u : 1u));
    /* Ownership is admitted/released over NTSRV RPC, never by presentation.
     * Reject ACTIVATE above, while retaining final paint/input/barrier checks. */
    CHECK(state.activations==0 && state.releases==0);
    if(mode==16) {
        DWORD i;CHECK(state.returned_count==ARRAYSIZE(state.returned));
        for(i=0;i<state.returned_count;++i)
            CHECK(state.returned[i].type==KEY_EVENT && state.returned[i].key_down==1 &&
                state.returned[i].repeat==1 && state.returned[i].character==0x100+i);
    }
    if(mode==11)CHECK(state.video.published_serial==1 && state.video.pixels &&
        state.video.pixels[sizeof(console_text_style)+2*(2*20+4)]=='N');
    if(mode==13) {
        CHECK(state.video.published_serial==1 && state.video.pixels);
        if(state.video.pixels) {
            const console_text_style *style=(const console_text_style *)state.video.pixels;
            CHECK(style->font_height==16 && style->fonts[0][0][0]==0xa5 &&
                style->fonts[1][255][31]==0xa5 && state.video.description.palette[1]==0x123456);
        }
    }
    frontend_video_dispose(&state.video);
    if(stop)CloseHandle(stop);
    if(process)CloseHandle(process);
    CloseHandle(pipe);
}
int wmain(int argc,WCHAR **argv)
{
    unsigned mode;
    if((argc!=2 && argc!=3) || _wfopen_s(&log,argv[1],L"wx"))return 2;
    {
        console_io_input wire={0},saved;INPUT_RECORD record;
        console_frame_mouse_input frame;
        wire.type=CONSOLE_INPUT_FRAME_MOUSE;wire.flags=CONSOLE_MOUSE_MOVE;
        wire.menu=640;wire.focus=400;wire.control=SHIFT_PRESSED|LEFT_ALT_PRESSED;
        wire.x=INT_MIN;wire.y=INT_MAX;wire.buttons=3;saved=wire;
        CHECK(ntcon_worker_decode_input(&wire,&record));
        memcpy(&frame,&record.Event,sizeof(frame));
        CHECK(record.EventType==CONSOLE_INPUT_FRAME_MOUSE && frame.dx==INT_MIN &&
            frame.dy==INT_MAX && frame.width==640 && frame.height==400 &&
            frame.buttons==3 && frame.control==wire.control);
        wire.focus=65536;CHECK(!ntcon_worker_decode_input(&wire,&record));
        wire=saved;wire.control=0x200;CHECK(!ntcon_worker_decode_input(&wire,&record));
        wire=saved;wire.menu=0;CHECK(!ntcon_worker_decode_input(&wire,&record));
        wire=saved;wire.repeat=1;CHECK(!ntcon_worker_decode_input(&wire,&record));
        wire=saved;wire.flags=CONSOLE_MOUSE_ENTER;CHECK(!ntcon_worker_decode_input(&wire,&record));
        wire.x=wire.y=wire.buttons=0;CHECK(ntcon_worker_decode_input(&wire,&record));
        wire.flags=CONSOLE_MOUSE_LEAVE;CHECK(!ntcon_worker_decode_input(&wire,&record));
        wire.menu=wire.focus=0;CHECK(ntcon_worker_decode_input(&wire,&record));
        wire.type=0x8002;CHECK(!ntcon_worker_decode_input(&wire,&record));
    }
    {
        ntcon_worker_client client={0};DWORD flags;
        HANDLE borrowed=CreateEventW(NULL,TRUE,FALSE,NULL),event;
        CHECK(borrowed!=NULL);
        CHECK(ntcon_worker_client_init(&client,NULL,GetCurrentProcess(),NULL,1)==ERROR_INVALID_PARAMETER);
        CHECK(!client.event);
        CHECK(!ntcon_worker_client_init(&client,borrowed,GetCurrentProcess(),borrowed,17));
        event=client.event;
        CHECK(event && client.pipe==borrowed && client.cancel==borrowed && client.generation==17);
        ntcon_worker_client_dispose(&client);
        CHECK(!client.event && !client.pipe && !client.peer && !client.cancel);
        CHECK(!GetHandleInformation(event,&flags) && GetLastError()==ERROR_INVALID_HANDLE);
        CHECK(GetHandleInformation(borrowed,&flags));
        ntcon_worker_client_dispose(&client);
        CHECK(GetHandleInformation(borrowed,&flags));
        CloseHandle(borrowed);
    }
    if(argc==3) {
        HANDLE input;
        /* Run on an observer-owned private desktop. A real, otherwise empty
         * Console is required: the production member guard must not be mocked. */
        if(wcscmp(argv[2],L"--input-return"))return 2;
        CHECK(FreeConsole());CHECK(AllocConsole());
        ShowWindow(GetConsoleWindow(),SW_HIDE);
        /* AllocConsole can retain inherited STARTF_USESTDHANDLES values. */
        input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
        CHECK(input!=INVALID_HANDLE_VALUE);
        CHECK(SetStdHandle(STD_INPUT_HANDLE,input));
        run_case(16);
        if(input!=INVALID_HANDLE_VALUE)CloseHandle(input);
    } else {
        /* The geometry cases generate asynchronous WINDOW_BUFFER_SIZE_EVENTs.
         * Verify exact injected-input order before changing Console geometry;
         * do not weaken the queue-count assertions or wait out the event. */
        run_case(7);run_case(8);run_case(19);
        for(mode=0;mode<=20;++mode)
            if(mode!=7 && mode!=8 && mode!=16 && mode!=19)run_case(mode);
    }
    fprintf(log,"NTVWM-PRESENTATION checks=%u failures=%u named-pipe=yes production-activation=no\n",checks,failures);
    fclose(log);return failures ? 1 : 0;
}
