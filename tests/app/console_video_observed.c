/* Test-only link substitution: all operations still execute the production
 * receiver; record completed frames and copied input for boundary diagnosis.
 * Never a product input. */
#define run16_console_video_data production_video_data
#define run16_console_video_text production_video_text
#include "../../src/ntcon-exe/console_video.c"
#undef run16_console_video_data
#undef run16_console_video_text
#include <stdio.h>
static SRWLOCK report_lock=SRWLOCK_INIT;
static void close_report(FILE *file)
{
    fclose(file);
    ReleaseSRWLockExclusive(&report_lock);
}

static FILE *report(void)
{
    static LONG announced;
    char path[MAX_PATH];
    DWORD length=GetEnvironmentVariableA("MVDM_TEST_FRAME_REPORT",path,sizeof(path));
    FILE *file=NULL;
    AcquireSRWLockExclusive(&report_lock);
    if (length && length<sizeof(path)) fopen_s(&file,path,"a");
    if(file && !InterlockedCompareExchange(&announced,1,0))
        fprintf(file,"RECEIVER pid=%lu\n",GetCurrentProcessId());
    if(!file)ReleaseSRWLockExclusive(&report_lock);
    return file;
}

/* Same production converter and sink; observe acceptance, not merely a
 * successful SendMessage to a Window which may already be retiring. */
#define frontend_keyboard_dispatch production_keyboard_dispatch
#include "../../src/ntcon-exe/window_keyboard.c"
#undef frontend_keyboard_dispatch
typedef struct observed_keyboard_sink {
    frontend_keyboard_sink sink;
    void *context;
} observed_keyboard_sink;
static DWORD observe_keyboard_sink(void *context,const INPUT_RECORD *records,DWORD count)
{
    observed_keyboard_sink *observed=context;
    DWORD error=observed->sink(observed->context,records,count),index;
    FILE *file=report();
    if(file) {
        for(index=0;index<count;++index) {
            const KEY_EVENT_RECORD *key=&records[index].Event.KeyEvent;
            fprintf(file,"WINDOW_DELIVERY tick=%llu error=%lu down=%d scan=%u char=%u\n",
                GetTickCount64(),error,key->bKeyDown,key->wVirtualScanCode,key->uChar.UnicodeChar);
        }
        close_report(file);
    }
    return error;
}
DWORD frontend_keyboard_dispatch(frontend_keyboard_delivery *delivery,
    const frontend_window_input *input,frontend_keyboard_sink sink,void *context)
{
    observed_keyboard_sink observed={sink,context};FILE *file=report();DWORD error;
    if(file) {
        fprintf(file,"WINDOW_EVENT tick=%llu source=%llu type=%u down=%u scan=%u\n",
            GetTickCount64(),input->event.source_identity,input->event.type,
            input->event.data.key.pressed,input->event.data.key.scan_code);
        close_report(file);
    }
    error=production_keyboard_dispatch(delivery,input,observe_keyboard_sink,&observed);
    return error;
}

DWORD run16_console_video_data(run16_console_video *video,uint32_t serial,
    uint32_t offset,const void *data,uint32_t bytes)
{
    console_text_style failed_style={0};
    DWORD result;
    if(video->pending && video->pending_description.kind==CONSOLE_VIDEO_TEXT_FRAME &&
        video->received>=offsetof(console_text_style,fonts))
        memcpy(&failed_style,video->pending,offsetof(console_text_style,fonts));
    result=production_video_data(video,serial,offset,data,bytes);
    if(result) {
        FILE *file=report();
        if(file) {
            fprintf(file,"FRAME_ERROR serial=%u error=%lu offset=%u bytes=%u font=%u select=%u cursor=%d,%d/%d,%d visible=%u\n",
                serial,result,offset,bytes,failed_style.font_height,failed_style.attribute_font_select,
                failed_style.cursor_start,failed_style.cursor_height,failed_style.cursor_start1,
                failed_style.cursor_height1,failed_style.cursor_visible);
            close_report(file);
        }
    }
    if (!result && video->published_serial==serial && !video->pending) {
        FILE *file=report();
        if (file) {
            if(video->description.kind==CONSOLE_VIDEO_TEXT_FRAME) {
                const console_text_style *style=(const console_text_style *)video->pixels;
                const BYTE *cells=video->pixels+sizeof(*style);
                uint32_t index,nonblank=0,font_hash=2166136261u,cell_hash=2166136261u;
                for(index=0;index<sizeof(style->fonts);++index)
                    font_hash=(font_hash^((const BYTE *)style->fonts)[index])*16777619u;
                for(index=0;index<video->description.width*video->description.height*2;++index) {
                    cell_hash=(cell_hash^cells[index])*16777619u;
                    if(!(index&1) && cells[index] && cells[index]!=' ')++nonblank;
                }
                fprintf(file,"TEXT_FRAME serial=%u width=%u height=%u font=%u banks=%u nonblank=%u fontHash=%08x cellHash=%08x cursor=%d,%d pid=%lu\n",
                    serial,video->description.width,video->description.height,style->font_height,
                    style->attribute_font_select,nonblank,font_hash,cell_hash,
                    style->cursor_column,style->cursor_row,GetCurrentProcessId());
                close_report(file);return result;
            }
            uint32_t x,y,a=0,b=0,other=0;
            for(y=0;y<video->description.height;++y)
                for(x=0;x<video->description.width;++x) {
                    BYTE pixel=video->description.depth==8 ?
                        video->pixels[y*video->description.stride+x] :
                        (BYTE)((video->pixels[y*video->description.stride+x/8]>>(7-x%8))&1);
                    if(pixel==0x12) ++a; else if(pixel==0x2a) ++b; else ++other;
                }
            fprintf(file,"FRAME serial=%u width=%u height=%u depth=%u a=%u b=%u other=%u paletteA=%06x paletteB=%06x pid=%lu\n",
                serial,video->description.width,video->description.height,
                video->description.depth,a,b,other,
                video->description.palette[0x12],video->description.palette[0x2a],GetCurrentProcessId());
            close_report(file);
        }
    }
    return result;
}

DWORD run16_console_video_text(run16_console_video *video,uint32_t serial)
{
    DWORD result=production_video_text(video,serial);
    FILE *file=report();
    if(file){fprintf(file,"TEXT serial=%u result=%lu\n",serial,result);close_report(file);}
    return result;
}

#define run16_console_dispatch production_console_dispatch
#include "../../src/ntcon-exe/console_frontend.c"
#undef run16_console_dispatch
DWORD run16_console_dispatch(run16_console_frontend *owner,
    const console_io_request *request,console_io_reply *reply)
{
    CONSOLE_SCREEN_BUFFER_INFO before={0},after={0};
    BOOL have_before=GetConsoleScreenBufferInfo(owner->output,&before);
    DWORD result=production_console_dispatch(owner,request,reply);
    BOOL have_after=GetConsoleScreenBufferInfo(owner->output,&after);
    if(request->operation==CONSOLE_IO_WRITE ||
       request->operation==CONSOLE_IO_CURSOR_POSITION ||
       request->operation==CONSOLE_IO_BUFFER_SIZE ||
       request->operation==CONSOLE_IO_WINDOW_RECT ||
       request->operation==CONSOLE_IO_SCREEN_INFO) {
        FILE *file=report();
        if(file) {
            fprintf(file,"GEOMETRY tick=%llu op=%u seq=%u handle=%p before=%d:%d,%d/%d,%d/%d,%d,%d,%d after=%d:%d,%d/%d,%d/%d,%d,%d,%d request=%d,%d,%d,%d status=%lu result=%u error=%u\n",
                GetTickCount64(),request->operation,request->sequence,owner->output,
                have_before,before.dwSize.X,before.dwSize.Y,before.dwCursorPosition.X,before.dwCursorPosition.Y,
                before.srWindow.Left,before.srWindow.Top,before.srWindow.Right,before.srWindow.Bottom,
                have_after,after.dwSize.X,after.dwSize.Y,after.dwCursorPosition.X,after.dwCursorPosition.Y,
                after.srWindow.Left,after.srWindow.Top,after.srWindow.Right,after.srWindow.Bottom,
                request->state.left,request->state.top,request->state.right,request->state.bottom,
                result,reply->result,reply->error);
            close_report(file);
        }
    }
    if (request && reply && (!reply->result || result) &&
        (request->operation==CONSOLE_IO_VIDEO_BEGIN ||
         request->operation==CONSOLE_IO_VIDEO_DATA ||
         request->operation==CONSOLE_IO_WINDOW_QUERY)) {
        FILE *file=report();
        if(file) {
            fprintf(file,"VIDEO_REQUEST_ERROR op=%u seq=%u status=%lu error=%u\n",
                request->operation,request->sequence,result,reply->error);
            close_report(file);
        }
    }
    if (request->operation==CONSOLE_IO_READ_INPUT ||
        request->operation==CONSOLE_IO_PREPEND_KEYS) {
        FILE *file=report();
        if(file) {
            uint32_t i,bytes=request->operation==CONSOLE_IO_READ_INPUT ? reply->bytes : request->bytes;
            const BYTE *data=request->operation==CONSOLE_IO_READ_INPUT ? reply->data : request->data;
            fprintf(file,"INPUT pid=%lu generation=%u op=%u seq=%u status=%lu result=%u error=%u bytes=%u tick=%llu\n",
                GetCurrentProcessId(),owner->generation,request->operation,request->sequence,
                result,reply->result,reply->error,bytes,GetTickCount64());
            for(i=0;i+sizeof(console_io_input)<=bytes;i+=sizeof(console_io_input)) {
                console_io_input wire;
                memcpy(&wire,data+i,sizeof(wire));
                fprintf(file,"RECORD type=%u down=%u repeat=%u vk=%u scan=%u char=%u control=%u\n",
                    wire.type,wire.key_down,wire.repeat,wire.virtual_key,wire.scan,wire.character,wire.control);
            }
            close_report(file);
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
            fputc('\n',file);close_report(file);
        }
    }
    return result;
}
