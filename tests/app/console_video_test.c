#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "ntcon-exe/console_frontend.h"
#define REQUIRE(x) do { if (!(x)) {printf("FAIL line=%d\n",__LINE__);return 1;} } while(0)

static DWORD dispatch_frame(frontend_console *owner, console_io_request *request,
    console_io_reply *reply, DWORD operation, DWORD serial)
{
    request->version=CONSOLE_IO_VERSION; request->generation=owner->generation;
    request->sequence=owner->sequence+1; request->operation=operation;
    request->state.mode=serial;
    return frontend_console_dispatch(owner,request,reply);
}

static int staged_publication(void)
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
    puts("PASS production frame staging: explicit commit, cancellation, style rejection, configuration isolation and serial high-water preservation");
    return 0;
}

int main(void)
{
    frontend_console owner={0};
    console_io_request request={0};
    console_io_reply reply;
    console_video_description description={0};
    uint32_t offset,count;
    REQUIRE(!staged_publication());
    owner.generation=71;
    description.width=320; description.height=200; description.depth=8;
    description.stride=320; description.bytes=64000; description.palette[1]=0x00123456;
    memcpy(request.data,&description,sizeof(description)); request.bytes=sizeof(description);
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,1)==0 && reply.result);
    REQUIRE(!owner.video.pixels && owner.video.pending);
    for(offset=0;offset<64000;offset+=count) {
        count=64000-offset; if(count>CONSOLE_IO_DATA_BYTES) count=CONSOLE_IO_DATA_BYTES;
        request.bytes=count; request.state.count=offset;
        memset(request.data,(int)(offset/CONSOLE_IO_DATA_BYTES+1),count);
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_DATA,1)==0 && reply.result);
        if(offset+count<64000) REQUIRE(!owner.video.pixels);
    }
    REQUIRE(owner.video.published_serial==1 && !owner.video.pending);
    REQUIRE(owner.video.pixels[0]==1 && owner.video.pixels[63999]==4);
    REQUIRE(owner.video.description.palette[1]==0x00123456);
    /* A different complete format must remain private until the last byte.
     * Width 9 exercises packed bits plus DWORD row padding, not width/8. */
    {
        console_video_description mono={0};
        const BYTE packed[8]={0x80,0x80,0xa5,0x5a,0x55,0,0x33,0xcc};
        mono.width=9; mono.height=2; mono.depth=1;
        mono.stride=4; mono.bytes=8; mono.palette[1]=0x00ffffff;
        memcpy(request.data,&mono,sizeof(mono)); request.bytes=sizeof(mono);
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,2)==0 && reply.result);
        request.bytes=3; request.state.count=0;
        memcpy(request.data,packed,3);
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_DATA,2)==0 && reply.result);
        REQUIRE(owner.video.description.depth==8 && owner.video.published_serial==1);
        request.bytes=5; request.state.count=3;
        memcpy(request.data,packed+3,5);
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_DATA,2)==0 && reply.result);
        REQUIRE(owner.video.description.depth==1 && owner.video.description.stride==4);
        REQUIRE(owner.video.published_serial==2 && !owner.video.pending);
        REQUIRE(memcmp(owner.video.pixels,packed,sizeof(packed))==0);
        REQUIRE(owner.video.description.palette[1]==0x00ffffff);

        /* TEXT must retire both the last complete frame and an unfinished
         * replacement. Late data must not resurrect either of them. */
        memcpy(request.data,&description,sizeof(description)); request.bytes=sizeof(description);
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,3)==0 && reply.result);
        request.bytes=1; request.state.count=0; request.data[0]=0x12;
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_DATA,3)==0 && reply.result);
        request.bytes=0;
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_TEXT,4)==0 && reply.result);
        REQUIRE(!owner.video.pending && !owner.video.pixels && owner.video.published_serial==4);
        request.bytes=1; request.state.count=1;
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_DATA,3)==0 && !reply.result);
        REQUIRE(reply.error==ERROR_INVALID_DATA && !owner.video.pending && !owner.video.pixels);
        frontend_video_dispose(&owner.video);
    }
    /* Re-establish the first complete frame for the independent negative
     * sequence below; disposal also starts a fresh frame serial lifetime. */
    memcpy(request.data,&description,sizeof(description)); request.bytes=sizeof(description);
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,1)==0 && reply.result);
    for(offset=0;offset<64000;offset+=count) {
        count=64000-offset; if(count>CONSOLE_IO_DATA_BYTES) count=CONSOLE_IO_DATA_BYTES;
        request.bytes=count; request.state.count=offset;
        memset(request.data,(int)(offset/CONSOLE_IO_DATA_BYTES+1),count);
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_DATA,1)==0 && reply.result);
    }
    request.state.count=0;
    memcpy(request.data,&description,sizeof(description)); request.bytes=sizeof(description);
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,2)==0 && reply.result);
    REQUIRE(owner.video.published_serial==1 && owner.video.pixels[0]==1);
    request.bytes=1; request.state.count=1;
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_DATA,2)==0 && !reply.result);
    REQUIRE(reply.error==ERROR_INVALID_DATA && !owner.video.pending && owner.video.published_serial==1);
    request.bytes=0; request.state.count=0;
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_TEXT,3)==0 && reply.result);
    REQUIRE(!owner.video.pixels && !owner.video.pending && owner.video.published_serial==3);
    request.bytes=1;
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_DATA,2)==0 && !reply.result);
    description.width=UINT32_MAX;
    memcpy(request.data,&description,sizeof(description)); request.bytes=sizeof(description);
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,4)==0 && !reply.result);
    description.width=320; description.stride=321;
    memcpy(request.data,&description,sizeof(description));
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,4)==0 && !reply.result);
    description.stride=320; description.palette[0]=0xff000000;
    memcpy(request.data,&description,sizeof(description));
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,4)==0 && !reply.result);
    description.palette[0]=0;
    memcpy(request.data,&description,sizeof(description));
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,3)==0 && !reply.result);
    request.bytes=sizeof(description)-1;
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,4)==ERROR_INVALID_DATA);
    request.bytes=sizeof(description); request.generation=70;
    REQUIRE(frontend_console_dispatch(&owner,&request,&reply)==ERROR_ACCESS_DENIED);
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,4)==0 && reply.result);
    frontend_video_dispose(&owner.video);
    REQUIRE(!owner.video.pending && !owner.video.pixels);
    puts("PASS production dispatcher: copied frame chunks, atomic publication, stale/text retirement, bounds, identity and teardown; no Window rendering claim");
    return 0;
}
