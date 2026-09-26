/* Test-only link substitution: all operations still execute the production
 * receiver; record completed frames and copied input for boundary diagnosis.
 * Never a product input. */
#define run16_console_video_data production_video_data
#define run16_console_video_text production_video_text
#include "../../src/run16-exe/console_video.c"
#undef run16_console_video_data
#undef run16_console_video_text
#include <stdio.h>

static FILE *report(void)
{
    char path[MAX_PATH];
    DWORD length=GetEnvironmentVariableA("MVDM_TEST_FRAME_REPORT",path,sizeof(path));
    FILE *file=NULL;
    if (length && length<sizeof(path)) fopen_s(&file,path,"a");
    return file;
}

DWORD run16_console_video_data(run16_console_video *video,uint32_t serial,
    uint32_t offset,const void *data,uint32_t bytes)
{
    DWORD result=production_video_data(video,serial,offset,data,bytes);
    if (!result && video->published_serial==serial && !video->pending) {
        FILE *file=report();
        if (file) {
            uint32_t x,y,a=0,b=0,other=0;
            for(y=0;y<video->description.height;++y)
                for(x=0;x<video->description.width;++x) {
                    BYTE pixel=video->description.depth==8 ?
                        video->pixels[y*video->description.stride+x] :
                        (BYTE)((video->pixels[y*video->description.stride+x/8]>>(7-x%8))&1);
                    if(pixel==0x12) ++a; else if(pixel==0x2a) ++b; else ++other;
                }
            fprintf(file,"FRAME serial=%u width=%u height=%u depth=%u a=%u b=%u other=%u paletteA=%06x paletteB=%06x\n",
                serial,video->description.width,video->description.height,
                video->description.depth,a,b,other,
                video->description.palette[0x12],video->description.palette[0x2a]);
            fclose(file);
        }
    }
    return result;
}

DWORD run16_console_video_text(run16_console_video *video,uint32_t serial)
{
    DWORD result=production_video_text(video,serial);
    FILE *file=report();
    if(file){fprintf(file,"TEXT serial=%u result=%lu\n",serial,result);fclose(file);}
    return result;
}

#define run16_console_dispatch production_console_dispatch
#include "../../src/run16-exe/console_frontend.c"
#undef run16_console_dispatch
DWORD run16_console_dispatch(run16_console_frontend *owner,
    const console_io_request *request,console_io_reply *reply)
{
    DWORD result=production_console_dispatch(owner,request,reply);
    if (request->operation==CONSOLE_IO_READ_INPUT ||
        request->operation==CONSOLE_IO_PREPEND_KEYS) {
        FILE *file=report();
        if(file) {
            uint32_t i,bytes=request->operation==CONSOLE_IO_READ_INPUT ? reply->bytes : request->bytes;
            const BYTE *data=request->operation==CONSOLE_IO_READ_INPUT ? reply->data : request->data;
            fprintf(file,"INPUT pid=%lu generation=%u op=%u seq=%u status=%lu result=%u error=%u bytes=%u\n",
                GetCurrentProcessId(),owner->generation,request->operation,request->sequence,
                result,reply->result,reply->error,bytes);
            for(i=0;i+sizeof(console_io_input)<=bytes;i+=sizeof(console_io_input)) {
                console_io_input wire;
                memcpy(&wire,data+i,sizeof(wire));
                fprintf(file,"RECORD type=%u down=%u repeat=%u vk=%u scan=%u char=%u control=%u\n",
                    wire.type,wire.key_down,wire.repeat,wire.virtual_key,wire.scan,wire.character,wire.control);
            }
            fclose(file);
        }
    }
    if(request->operation==CONSOLE_IO_WRITE ||
       request->operation==CONSOLE_IO_WRITE_CELLS_A ||
       request->operation==CONSOLE_IO_WRITE_CELLS_W) {
        FILE *file=report();
        if(file){
            uint32_t i,step=request->operation==CONSOLE_IO_WRITE ? 1 : sizeof(console_io_cell);
            fprintf(file,"WRITE op=%u seq=%u status=%lu result=%u error=%u rect=%d,%d,%d,%d bytes=%u text=",
                request->operation,request->sequence,result,reply->result,reply->error,
                request->state.left,request->state.top,request->state.right,request->state.bottom,request->bytes);
            for(i=0;i<request->bytes;i+=step){BYTE ch=request->data[i];if(ch>32 && ch<127)fputc(ch,file);}
            fputc('\n',file);fclose(file);
        }
    }
    return result;
}
