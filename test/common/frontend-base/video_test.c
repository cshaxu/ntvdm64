#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "common/frontend-base/video.h"
#define REQUIRE(x) do { if (!(x)) {printf("FAIL line=%d\n",__LINE__);return 1;} } while(0)

int main(void)
{
    frontend_video video={0};
    console_video_description description={0},configuration={0};
    struct {
        console_text_style style;
        BYTE cells[6];
    } payload={0};
    BYTE *previous;
    payload.style.font_height=16;
    payload.style.cursor_visible=1;
    payload.style.cursor_height=2;
    payload.cells[0]='A';payload.cells[1]=7;
    payload.cells[2]=CONSOLE_TEXT_UNDERLINE;
    payload.cells[3]='B';payload.cells[4]=7;
    description.kind=CONSOLE_VIDEO_TEXT_FRAME;
    description.width=2;description.height=1;description.stride=6;
    description.bytes=sizeof(payload.style)+sizeof(payload.cells);
    REQUIRE(!frontend_video_begin(&video,1,&description));
    REQUIRE(!frontend_video_data(&video,1,0,&payload,description.bytes));
    previous=video.pixels;
    REQUIRE(previous && video.published_serial==1 && !video.pending_validated);

    /* A complete, validated replacement remains private until its owner has
     * prepared the dependent grid. Neither partial nor complete staging can
     * retire the old renderer's borrowed storage. */
    payload.cells[0]='C';
    REQUIRE(!frontend_video_begin(&video,2,&description));
    REQUIRE(frontend_video_commit_pending(&video)==ERROR_INVALID_STATE);
    REQUIRE(!frontend_video_stage_data(&video,2,0,&payload,1));
    REQUIRE(frontend_video_commit_pending(&video)==ERROR_INVALID_STATE);
    REQUIRE(video.pixels==previous && video.published_serial==1);
    REQUIRE(!frontend_video_stage_data(&video,2,1,(BYTE *)&payload+1,description.bytes-1));
    REQUIRE(video.pending_validated && video.pixels==previous && video.published_serial==1);
    REQUIRE(video.pixels[sizeof(payload.style)]=='A');
    frontend_video_abort_pending(&video);
    REQUIRE(!video.pending && !video.pending_validated && video.pixels==previous);
    REQUIRE(video.serial==2 && video.published_serial==1);
    REQUIRE(frontend_video_begin(&video,2,&description)==ERROR_INVALID_DATA);

    REQUIRE(!frontend_video_begin(&video,3,&description));
    REQUIRE(!frontend_video_stage_data(&video,3,0,&payload,description.bytes));
    REQUIRE(!frontend_video_commit_pending(&video));
    REQUIRE(video.pixels!=previous && video.published_serial==3 && !video.pending);
    REQUIRE(video.pixels[sizeof(payload.style)]=='C' && !video.pending_validated);
    previous=video.pixels;

    /* Malformed optional styles invalidate staging, not the committed frame. */
    payload.cells[2]=0xff;
    REQUIRE(!frontend_video_begin(&video,4,&description));
    REQUIRE(frontend_video_stage_data(&video,4,0,&payload,description.bytes)==ERROR_INVALID_DATA);
    REQUIRE(!video.pending && !video.pending_validated && video.pixels==previous);
    REQUIRE(video.published_serial==3 && video.serial==4);
    REQUIRE(frontend_video_commit_pending(&video)==ERROR_INVALID_STATE);

    configuration.kind=CONSOLE_VIDEO_TEXT_CONFIGURATION;
    configuration.bytes=sizeof(payload.style);
    configuration.palette[7]=0x00123456;
    REQUIRE(!frontend_video_begin(&video,5,&configuration));
    payload.style.font_height=8;
    REQUIRE(!frontend_video_stage_data(&video,5,0,&payload.style,sizeof(payload.style)));
    REQUIRE(!video.configuration_serial && video.pixels==previous && video.pending_validated);
    REQUIRE(!frontend_video_commit_pending(&video));
    REQUIRE(video.configuration_serial==5 && video.configuration.style.font_height==8);
    REQUIRE(video.configuration.palette[7]==0x00123456 && video.pixels==previous);
    REQUIRE(video.published_serial==3 && !video.pending && !video.pending_validated);

    REQUIRE(!frontend_video_begin(&video,6,&configuration));
    REQUIRE(!frontend_video_stage_data(&video,6,0,&payload.style,1));
    frontend_video_abort_pending(&video); /* EOF/cancel by the owner. */
    REQUIRE(video.configuration_serial==5 && video.pixels==previous && video.serial==6);
    REQUIRE(frontend_video_commit_pending(NULL)==ERROR_INVALID_STATE);
    frontend_video_abort_pending(NULL);
    frontend_video_dispose(&video);
    REQUIRE(!video.pending && !video.pixels && !video.pending_validated);
    puts("PASS frontend-base video: explicit commit, cancellation, style rejection, configuration isolation and serial high-water preservation");
    return 0;
}
