#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "run16-exe/console_frontend.h"
#define REQUIRE(x) do { if (!(x)) {printf("FAIL line=%d\n",__LINE__);return 1;} } while(0)

static DWORD dispatch_frame(run16_console_frontend *owner, console_io_request *request,
    console_io_reply *reply, DWORD operation, DWORD serial)
{
    request->version=CONSOLE_IO_VERSION; request->generation=owner->generation;
    request->sequence=owner->sequence+1; request->operation=operation;
    request->state.mode=serial;
    return run16_console_dispatch(owner,request,reply);
}

int main(void)
{
    run16_console_frontend owner={0};
    console_io_request request={0};
    console_io_reply reply;
    console_video_description description={0};
    uint32_t offset,count;
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
        REQUIRE(!owner.video.pending && !owner.video.pixels && !owner.video.published_serial);
        request.bytes=1; request.state.count=1;
        REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_DATA,3)==0 && !reply.result);
        REQUIRE(reply.error==ERROR_INVALID_DATA && !owner.video.pending && !owner.video.pixels);
        run16_console_video_dispose(&owner.video);
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
    REQUIRE(!owner.video.pixels && !owner.video.pending && !owner.video.published_serial);
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
    REQUIRE(run16_console_dispatch(&owner,&request,&reply)==ERROR_ACCESS_DENIED);
    REQUIRE(dispatch_frame(&owner,&request,&reply,CONSOLE_IO_VIDEO_BEGIN,4)==0 && reply.result);
    run16_console_video_dispose(&owner.video);
    REQUIRE(!owner.video.pending && !owner.video.pixels);
    puts("PASS production dispatcher: copied frame chunks, atomic publication, stale/text retirement, bounds, identity and teardown; no Window rendering claim");
    return 0;
}
