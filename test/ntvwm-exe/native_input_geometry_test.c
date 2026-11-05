/* Extracted production input function; fake boundaries force a changing output
 * snapshot. This is not a real Console/lifecycle acceptance replacement. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "common/protocol/console_io.h"
#include "ntvwm-exe/console_state.h"
#include "ntvwm-exe/text_frame.h"
typedef struct ntvwm_presentation {
    struct {DWORD failure;} channel;
    CRITICAL_SECTION lock;
    ntvwm_mouse mouse;
    BOOL has_handoff_font;
    console_text_style handoff_font;
} ntvwm_presentation;
static console_io_input events[3];
static unsigned captures,geometry,opens,closes,writes,failed;
static DWORD read_error,write_error;
#define CHECK(x) do {if(!(x)){++failed;printf("FAIL %d: %s\n",__LINE__,#x);}}while(0)
static DWORD exchange(ntvwm_presentation *client,console_io_request *request,console_io_reply *reply)
{
    (void)client;CHECK(request->operation==CONSOLE_IO_READ_INPUT);
    memset(reply,0,sizeof(*reply));reply->state.count=3;
    reply->bytes=sizeof(events);reply->data=(BYTE *)events;return 0;
}
DWORD ntvwm_capture_begin(ntvwm_capture *capture)
{(void)capture;++captures;return ERROR_RETRY;}
void ntvwm_capture_end(ntvwm_capture *capture){(void)capture;}
static HANDLE fake_open(LPCWSTR path,DWORD access,DWORD share,LPSECURITY_ATTRIBUTES security,DWORD creation,DWORD flags,HANDLE template_file)
{
    (void)access;(void)share;(void)security;(void)creation;(void)flags;(void)template_file;
    CHECK(!wcscmp(path,L"CONOUT$"));++opens;return (HANDLE)17;
}
static BOOL fake_info(HANDLE output,PCONSOLE_SCREEN_BUFFER_INFO info)
{
    CHECK(output==(HANDLE)17);++geometry;
    if(read_error){SetLastError(read_error);return FALSE;}
    memset(info,0,sizeof(*info));info->srWindow=(SMALL_RECT){0,(SHORT)geometry,79,(SHORT)(geometry+24)};return TRUE;
}
static BOOL fake_close(HANDLE output){CHECK(output==(HANDLE)17);++closes;return TRUE;}
DWORD ntvwm_mouse_geometry(ntvwm_mouse *mouse,SMALL_RECT view,unsigned height)
{mouse->viewport=view;mouse->font_height=height;mouse->ready=TRUE;return 0;}
DWORD ntvwm_mouse_input(ntvwm_mouse *mouse,const console_frame_mouse_input *frame,INPUT_RECORD records[2],DWORD *count)
{(void)mouse;(void)frame;memset(records,0,sizeof(*records));records[0].EventType=MOUSE_EVENT;*count=1;return 0;}
static BOOL ntcon_worker_decode_input(const console_io_input *wire,INPUT_RECORD *record)
{
    memset(record,0,sizeof(*record));record->EventType=(WORD)wire->type;
    record->Event.KeyEvent.uChar.UnicodeChar=(WCHAR)wire->character;
    return wire->type==KEY_EVENT || wire->type==MOUSE_EVENT || wire->type==CONSOLE_INPUT_FRAME_MOUSE;
}
DWORD ntvwm_input_write(HANDLE input,const INPUT_RECORD *records,DWORD count,DWORD *accepted)
{
    CHECK(input==(HANDLE)19);CHECK(count==3);++writes;
    CHECK(records[0].EventType==KEY_EVENT && records[0].Event.KeyEvent.uChar.UnicodeChar=='a');
    *accepted=write_error ? 1 : count;return write_error;
}
#define CreateFileW fake_open
#define GetConsoleScreenBufferInfo fake_info
#define CloseHandle fake_close
#include "native_input_geometry.inc"
int main(void)
{
    ntvwm_presentation client={0};DWORD accepted,error;unsigned i;
    InitializeCriticalSection(&client.lock);
    for(i=0;i<3;++i){events[i].type=KEY_EVENT;events[i].character='a'+i;}
    error=ntvwm_presentation_input(&client,(HANDLE)19,&accepted);
    CHECK(!error && accepted==3 && writes==1);
    CHECK(!captures && !geometry && !opens && !closes && !client.channel.failure);
    events[1].type=MOUSE_EVENT;events[2].type=CONSOLE_INPUT_FRAME_MOUSE;
    error=ntvwm_presentation_input(&client,(HANDLE)19,&accepted);
    CHECK(!error && accepted==3 && writes==2);
    CHECK(!captures && geometry==1 && opens==1 && closes==1);
    read_error=ERROR_INVALID_HANDLE;
    CHECK(ntvwm_presentation_input(&client,(HANDLE)19,&accepted)==read_error && !accepted);
    CHECK(client.channel.failure==read_error && writes==2 && closes==2);
    read_error=0;client.channel.failure=0;write_error=ERROR_WRITE_FAULT;
    CHECK(ntvwm_presentation_input(&client,(HANDLE)19,&accepted)==write_error && accepted==1);
    CHECK(client.channel.failure==write_error && writes==3 && !captures);
    DeleteCriticalSection(&client.lock);
    printf("native input geometry failures=%u captures=%u viewport-reads=%u\n",failed,captures,geometry);
    return failed ? 1 : 0;
}
