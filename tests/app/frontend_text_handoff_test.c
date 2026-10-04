#include "ntcon-exe/frontend_session.h"
#include <stdio.h>
#include <string.h>
static unsigned checks,failures;
static FILE *log;
#define CHECK(x) do {++checks;if(!(x)){++failures;fprintf(log,"FAIL %u %s\n",__LINE__,#x);}} while(0)
int wmain(int argc,WCHAR **argv)
{
    frontend_session *frontend=NULL;frontend_video video={0};
    console_video_description description={0};console_text_style *style;
    console_text_configuration copy={0};console_io_reply reply;
    BYTE *payload;DWORD offset=0,revision=0,error;int dos,native;
    if(argc!=2 || _wfopen_s(&log,argv[1],L"wx"))return 2;
    CHECK(frontend_session_create(&frontend)==0);
    if(!frontend)goto done;
    CHECK(frontend_session_bind(frontend,&dos,TRUE)==0);
    CHECK(frontend_session_prepare_text(frontend,&dos,(COORD){80,25},NULL)==0);
    error=frontend_session_enter(frontend,&dos);CHECK(error==0);
    if(!error) {
        CHECK(!frontend_session_text_frame_required(frontend));
        CHECK(frontend_session_read_text_configuration(frontend,0,0,&reply)==ERROR_NOT_FOUND);
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
        while(offset<sizeof(copy)) {
            error=frontend_session_read_text_configuration(frontend,offset,revision,&reply);
            CHECK(error==0);if(error)break;
            CHECK(reply.state.count==sizeof(copy) && reply.state.mode && reply.bytes &&
                reply.bytes<=sizeof(copy)-offset);
            if(!reply.bytes || reply.bytes>sizeof(copy)-offset)break;
            revision=reply.state.mode;memcpy((BYTE *)&copy+offset,reply.data,reply.bytes);offset+=reply.bytes;
        }
        CHECK(offset==sizeof(copy) && copy.style.font_height==16 && copy.style.attribute_font_select==1);
        CHECK(!memcmp(copy.style.fonts,style->fonts,sizeof(style->fonts)) && copy.palette[1]==0x123456);
        CHECK(frontend_session_read_text_configuration(frontend,0,revision+1,&reply)==ERROR_RETRY);
        CHECK(frontend_session_read_text_configuration(frontend,sizeof(copy),revision,&reply)==ERROR_INVALID_PARAMETER);
        CHECK(frontend_session_read_text_configuration(frontend,1,0,&reply)==ERROR_INVALID_PARAMETER);
        frontend_session_leave(frontend);
    }
    CHECK(frontend_session_bind(frontend,&native,FALSE)==0);
    HeapFree(GetProcessHeap(),0,payload);
dispose:
    frontend_session_destroy(frontend);frontend_video_dispose(&video);
done:
    fprintf(log,"FRONTEND-TEXT-HANDOFF checks=%u failures=%u production-storage=yes runtime-guest=no\n",checks,failures);
    fclose(log);return failures ? 1 : 0;
}
