#include "ntcon-exe/frontend_session.h"
#include <stdio.h>
#include <string.h>
static unsigned checks,failures;
static FILE *report_file;
#define CHECK(x) do {++checks;if(!(x)){++failures;fprintf(report_file,"FAIL %u %s\n",__LINE__,#x);}} while(0)
int main(int argc,char **argv)
{
    frontend_session *frontend=NULL;frontend_video video={0},palette_video={0};
    console_video_description description={0};console_text_style *style;
    console_text_configuration *copy=NULL;console_io_reply *reply=NULL;
    BYTE *payload,*palette_payload=NULL;DWORD offset=0,revision=0,error;
    HANDLE input=NULL,output=NULL;
    CONSOLE_SCREEN_BUFFER_INFOEX original={sizeof(original)},current={sizeof(current)};
    CONSOLE_SCREEN_BUFFER_INFOEX before_palette={sizeof(before_palette)};
    int dos,native;
    if(argc!=2 || fopen_s(&report_file,argv[1],"wx"))return 2;
    copy=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*copy));
    CHECK(copy!=NULL);
    if(!copy)goto done;
    reply=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*reply));
    CHECK(reply!=NULL);
    if(!reply)goto done;
    reply->data=HeapAlloc(GetProcessHeap(),0,CONSOLE_IO_TILE_BYTES);
    CHECK(reply->data!=NULL);
    if(!reply->data)goto done;
    CHECK(frontend_session_create(&frontend)==0);
    if(!frontend)goto done;
    CHECK(frontend_session_console(frontend,&input,&output)==0);
    if(input){CloseHandle(input);input=NULL;}
    CHECK(output && GetConsoleScreenBufferInfoEx(output,&original));
    CHECK(frontend_session_bind(frontend,&dos,TRUE)==0);
    CHECK(frontend_session_prepare_text(frontend,&dos,(COORD){80,25},NULL)==0);
    error=frontend_session_enter(frontend,&dos);CHECK(error==0);
    if(!error) {
        CHECK(!frontend_session_text_frame_required(frontend));
        CHECK(frontend_session_read_text_configuration(frontend,0,0,reply)==ERROR_NOT_FOUND);
        frontend_session_leave(frontend);
    }
    description.kind=CONSOLE_VIDEO_TEXT_CONFIGURATION;
    description.bytes=sizeof(*style);description.palette[1]=0x123456;
    payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,description.bytes);CHECK(payload!=NULL);
    if(!payload)goto dispose;
    style=(console_text_style *)payload;style->font_height=16;style->attribute_font_select=1;
    memset(style->fonts,0xa5,sizeof(style->fonts));
    CHECK(frontend_video_begin(&video,1,&description)==0);
    CHECK(frontend_video_data(&video,1,0,payload,description.bytes)==0);
    CHECK(!video.pixels && !video.published_serial && video.configuration_serial==1);
    /* Configuration must neither publish partial data nor erase pixels.
     * These owned bytes stand for an already acknowledged render surface. */
    video.pixels=HeapAlloc(GetProcessHeap(),0,1);CHECK(video.pixels!=NULL);
    if(video.pixels) {
        BYTE *retained=video.pixels;retained[0]=0x37;
        CHECK(frontend_video_begin(&video,2,&description)==0);
        CHECK(frontend_video_data(&video,2,0,payload,4)==0);
        CHECK(video.configuration_serial==1 && video.pixels==retained && retained[0]==0x37);
        CHECK(frontend_video_data(&video,2,4,payload+4,description.bytes-4)==0);
        CHECK(video.configuration_serial==2 && !video.published_serial && video.pixels==retained);
        style->font_height=33;
        CHECK(frontend_video_begin(&video,3,&description)==0);
        CHECK(frontend_video_data(&video,3,0,payload,description.bytes)==ERROR_INVALID_DATA);
        CHECK(video.configuration_serial==2 && video.configuration.style.font_height==16 &&
            video.pixels==retained && retained[0]==0x37);
        style->font_height=16;
        HeapFree(GetProcessHeap(),0,video.pixels);video.pixels=NULL;
    }
    CHECK(frontend_session_video(frontend,&dos,&video,TRUE)==0);
    CHECK(frontend_session_bind(frontend,&dos,FALSE)==0);
    /* Destroy the channel storage: retained configuration must be copied. */
    frontend_video_dispose(&video);
    CHECK(frontend_session_bind(frontend,&native,TRUE)==0);
    error=frontend_session_enter(frontend,&native);CHECK(error==0);
    if(!error) {
        while(offset<sizeof(*copy)) {
            error=frontend_session_read_text_configuration(frontend,offset,revision,reply);
            CHECK(error==0);if(error)break;
            CHECK(reply->state.count==sizeof(*copy) && reply->state.mode && reply->bytes &&
                reply->bytes<=sizeof(*copy)-offset);
            if(!reply->bytes || reply->bytes>sizeof(*copy)-offset)break;
            revision=reply->state.mode;memcpy((BYTE *)copy+offset,reply->data,reply->bytes);offset+=reply->bytes;
        }
        CHECK(offset==sizeof(*copy) && copy->style.font_height==16 && copy->style.attribute_font_select==1);
        CHECK(!memcmp(copy->style.fonts,style->fonts,sizeof(style->fonts)) && copy->palette[1]==0x123456);
        CHECK(frontend_session_read_text_configuration(frontend,0,revision+1,reply)==ERROR_RETRY);
        CHECK(frontend_session_read_text_configuration(frontend,sizeof(*copy),revision,reply)==ERROR_INVALID_PARAMETER);
        CHECK(frontend_session_read_text_configuration(frontend,1,0,reply)==ERROR_INVALID_PARAMETER);
        frontend_session_leave(frontend);
    }
    /* One text frame owns both indexed cells and its actual 16-entry RGB
     * table. A later configuration-only palette update must recolour the
     * same logical cells, and teardown must return the caller's palette. */
    description=(console_video_description){0};
    description.kind=CONSOLE_VIDEO_TEXT_FRAME;description.width=80;description.height=25;
    description.stride=160;description.bytes=sizeof(console_text_style)+160*25;
    description.palette[1]=0x123456;
    palette_payload=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,description.bytes);
    CHECK(palette_payload!=NULL);
    if(palette_payload) {
        style=(console_text_style *)palette_payload;style->font_height=16;
        palette_payload[sizeof(*style)]='X';palette_payload[sizeof(*style)+1]=0x1f;
        CHECK(frontend_video_begin(&palette_video,1,&description)==0);
        CHECK(frontend_video_data(&palette_video,1,0,palette_payload,description.bytes)==0);
        CHECK(frontend_session_video(frontend,&native,&palette_video,TRUE)==0);
        current.cbSize=sizeof(current);
        CHECK(GetConsoleScreenBufferInfoEx(output,&current) &&
            current.ColorTable[1]==RGB(0x12,0x34,0x56));
        before_palette=current;
        description=(console_video_description){0};
        description.kind=CONSOLE_VIDEO_TEXT_CONFIGURATION;
        description.bytes=sizeof(*style);description.palette[1]=0x654321;
        CHECK(frontend_video_begin(&palette_video,2,&description)==0);
        CHECK(frontend_video_data(&palette_video,2,0,palette_payload,sizeof(*style))==0);
        CHECK(frontend_session_video(frontend,&native,&palette_video,TRUE)==0);
        current.cbSize=sizeof(current);
        CHECK(GetConsoleScreenBufferInfoEx(output,&current) &&
            current.ColorTable[1]==RGB(0x65,0x43,0x21));
        CHECK(!memcmp(&current.dwSize,&before_palette.dwSize,sizeof(current.dwSize)) &&
            !memcmp(&current.srWindow,&before_palette.srWindow,sizeof(current.srWindow)) &&
            !memcmp(&current.dwCursorPosition,&before_palette.dwCursorPosition,sizeof(current.dwCursorPosition)));
    }
    CHECK(frontend_session_bind(frontend,&native,FALSE)==0);
    HeapFree(GetProcessHeap(),0,payload);
dispose:
    if(palette_payload)HeapFree(GetProcessHeap(),0,palette_payload);
    frontend_session_destroy(frontend);frontend_video_dispose(&video);
    frontend_video_dispose(&palette_video);
    if(output) {
        current.cbSize=sizeof(current);
        CHECK(GetConsoleScreenBufferInfoEx(output,&current) &&
            !memcmp(current.ColorTable,original.ColorTable,sizeof(original.ColorTable)));
        CloseHandle(output);
    }
done:
    if(copy)HeapFree(GetProcessHeap(),0,copy);
    if(reply) {
        if(reply->data)HeapFree(GetProcessHeap(),0,reply->data);
        HeapFree(GetProcessHeap(),0,reply);
    }
    fprintf(report_file,"FRONTEND-TEXT-HANDOFF checks=%u failures=%u production-storage=yes runtime-guest=no\n",checks,failures);
    fclose(report_file);return failures ? 1 : 0;
}
