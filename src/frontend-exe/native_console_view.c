#include "native_console_view.h"
#include <string.h>

static DWORD exchange(run16_native_backend *backend,DWORD operation,void *payload,DWORD bytes,
    DWORD offset,DWORD count,run16_native_host_reply *reply,void *data,DWORD capacity)
{
    run16_native_host_request request={RUN16_NATIVE_HOST_VERSION,operation,bytes,offset,count};
    DWORD error=run16_native_backend_call(backend,&request,payload,reply,data,capacity);
    return error ? error : reply->status;
}

static DWORD visible_geometry(HANDLE output,run16_native_geometry *geometry)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    ZeroMemory(geometry,sizeof(*geometry));geometry->font.cbSize=sizeof(geometry->font);
    if(!GetConsoleScreenBufferInfo(output,&info) ||
        !GetCurrentConsoleFontEx(output,FALSE,&geometry->font))return GetLastError();
    geometry->size=info.dwSize;geometry->window=info.srWindow;
    return ERROR_SUCCESS;
}

void run16_native_view_end(run16_native_console_view *view)
{
    if(!view)return;
    if(view->input && view->input!=INVALID_HANDLE_VALUE) {
        if(view->mode_saved)SetConsoleMode(view->input,view->input_mode);
        CloseHandle(view->input);
    }
    if(view->output && view->output!=INVALID_HANDLE_VALUE)CloseHandle(view->output);
    ZeroMemory(view,sizeof(*view));
}

DWORD run16_native_view_begin(run16_native_backend *backend,run16_native_console_view *view)
{
    HANDLE input,output;
    DWORD error;
    if(!backend || !view)return ERROR_INVALID_PARAMETER;
    ZeroMemory(view,sizeof(*view));
    input=CreateFileW(L"CONIN$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(input==INVALID_HANDLE_VALUE)return GetLastError();
    output=CreateFileW(L"CONOUT$",GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    error=output==INVALID_HANDLE_VALUE ? GetLastError() :
        run16_native_view_begin_on(backend,view,input,output);
    if(output!=INVALID_HANDLE_VALUE)CloseHandle(output);
    CloseHandle(input);
    return error;
}

DWORD run16_native_view_begin_on(run16_native_backend *backend,
    run16_native_console_view *view,HANDLE input,HANDLE output)
{
    DWORD error;
    if(!backend || !view)return ERROR_INVALID_PARAMETER;
    ZeroMemory(view,sizeof(*view));
    if(!input || input==INVALID_HANDLE_VALUE || !output || output==INVALID_HANDLE_VALUE)
        return ERROR_INVALID_HANDLE;
    if(!DuplicateHandle(GetCurrentProcess(),input,GetCurrentProcess(),&view->input,0,FALSE,DUPLICATE_SAME_ACCESS) ||
        !DuplicateHandle(GetCurrentProcess(),output,GetCurrentProcess(),&view->output,0,FALSE,DUPLICATE_SAME_ACCESS) ||
        !GetConsoleMode(view->input,&view->input_mode))error=GetLastError();
    else error=run16_native_view_seed(backend,view);
    if(error)run16_native_view_end(view);
    return error;
}

DWORD run16_native_view_seed(run16_native_backend *backend,run16_native_console_view *view)
{
    run16_native_capture capture={0};
    run16_native_screen_seed seed={0};
    run16_native_host_reply reply;
    CHAR_INFO cells[RUN16_NATIVE_HOST_CELLS];
    DWORD error,offset=0,count,total;
    SMALL_RECT region;
    if(!backend || !view)return ERROR_INVALID_PARAMETER;
    error=run16_native_capture_begin_output(&capture,view->output);
    if(error)goto done;
    seed.frame.screen=capture.info;seed.frame.cursor=capture.cursor;
    seed.frame.output_mode=capture.output_mode;seed.frame.input_codepage=capture.input_codepage;
    seed.frame.output_codepage=capture.output_codepage;
    seed.font.cbSize=sizeof(seed.font);
    if(!GetCurrentConsoleFontEx(capture.buffer,FALSE,&seed.font)) { error=GetLastError();goto done; }
    error=exchange(backend,RUN16_NATIVE_SCREEN_APPLY,&seed,sizeof(seed),0,0,&reply,NULL,0);
    total=(DWORD)capture.info.dwSize.X*capture.info.dwSize.Y;
    while(!error && offset<total) {
        error=run16_native_capture_read(&capture,offset,cells,ARRAYSIZE(cells),&region,&count);
        if(!error)error=exchange(backend,RUN16_NATIVE_CELLS_WRITE,cells,count*sizeof(*cells),offset,count,&reply,NULL,0);
        if(!error)offset+=count;
    }
    if(!error) {
        /* Cooked editing belongs to the hidden Windows Console. The visible
         * side forwards records once, without echoing or interpreting them. */
        DWORD mode=(view->input_mode|ENABLE_EXTENDED_FLAGS|ENABLE_WINDOW_INPUT|ENABLE_MOUSE_INPUT)&
            ~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT|ENABLE_PROCESSED_INPUT|ENABLE_QUICK_EDIT_MODE|ENABLE_VIRTUAL_TERMINAL_INPUT);
        if(!SetConsoleMode(view->input,mode))error=GetLastError();
        else view->mode_saved=TRUE;
    }
    if(!error) {
        view->geometry.size=seed.frame.screen.dwSize;view->geometry.window=seed.frame.screen.srWindow;
        view->geometry.font=seed.font;view->geometry_saved=TRUE;
    }
done:
    run16_native_capture_end(&capture);
    return error;
}

static DWORD present_snapshot(run16_native_backend *backend,run16_native_console_view *view,BOOL window)
{
    run16_native_frame_info frame;
    run16_native_host_reply reply;
    CHAR_INFO *cells=NULL;
    DWORD error,ending,total,offset=0;
    SIZE_T bytes;
    run16_native_geometry before,after;
    if(!backend || !view)return ERROR_INVALID_PARAMETER;
    error=visible_geometry(view->output,&before);
    if(error)return error;
    /* A change since our last paint belongs to the user-facing Console, not
     * the older hidden snapshot. Resize native storage before capturing it;
     * never reseed cells/cursor over output written meanwhile by the target. */
    if(view->geometry_saved && memcmp(&before,&view->geometry,sizeof(before))) {
        error=exchange(backend,RUN16_NATIVE_GEOMETRY,&before,sizeof(before),0,0,&reply,NULL,0);
        if(error)return error;
        view->geometry=before;
    }
    error=exchange(backend,RUN16_NATIVE_FRAME_BEGIN,NULL,0,0,0,&reply,&frame,sizeof(frame));
    if(error)return error;
    if(reply.bytes!=sizeof(frame) || frame.screen.dwSize.X<=0 || frame.screen.dwSize.Y<=0) {
        error=ERROR_INVALID_DATA;goto done;
    }
    total=(DWORD)frame.screen.dwSize.X*frame.screen.dwSize.Y;
    if((uint64_t)total*sizeof(*cells)>(SIZE_T)-1) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
    bytes=(SIZE_T)total*sizeof(*cells);
    cells=HeapAlloc(GetProcessHeap(),0,bytes);
    if(!cells) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
    while(offset<total) {
        DWORD capacity=min(total-offset,RUN16_NATIVE_HOST_CELLS);
        DWORD x=offset%(DWORD)frame.screen.dwSize.X,y=offset/(DWORD)frame.screen.dwSize.X;
        error=exchange(backend,RUN16_NATIVE_FRAME_READ,NULL,0,offset,capacity,&reply,cells+offset,capacity*sizeof(*cells));
        if(error)break;
        if(!reply.count || reply.count>capacity || reply.bytes!=reply.count*sizeof(*cells) ||
            reply.region.Left!=(SHORT)x || reply.region.Top!=(SHORT)y || reply.region.Right<reply.region.Left ||
            reply.region.Bottom<reply.region.Top || reply.region.Right>=frame.screen.dwSize.X ||
            reply.region.Bottom>=frame.screen.dwSize.Y ||
            (reply.region.Bottom>reply.region.Top &&
                (reply.region.Left || reply.region.Right!=frame.screen.dwSize.X-1)) ||
            (DWORD)(reply.region.Right-reply.region.Left+1)*(DWORD)(reply.region.Bottom-reply.region.Top+1)!=reply.count) {
            error=ERROR_INVALID_DATA;break;
        }
        offset+=reply.count;
    }
    /* Do not publish half a resized snapshot. Windows remains the owner of
     * buffer storage, scrollback, Unicode cells and attributes on both sides. */
    if(!error)error=visible_geometry(view->output,&after);
    if(!error && memcmp(&before,&after,sizeof(before)))error=ERROR_RETRY;
    if(!error && window && view->window_frame)
        error=view->window_frame(view->window_context,&frame,cells,total);
    else if(!error) {
        error=run16_native_screen_apply(view->output,&frame.screen,&frame.cursor);
        if(!error)error=run16_native_cells_write(view->output,0,cells,total);
    }
    if(!error) {
        view->geometry=before;
        if(!window || !view->window_frame) {
            view->geometry.size=frame.screen.dwSize;
            view->geometry.window=frame.screen.srWindow;
        }
        view->geometry_saved=TRUE;
    }
done:
    ending=exchange(backend,RUN16_NATIVE_FRAME_END,NULL,0,0,0,&reply,NULL,0);
    if(cells)HeapFree(GetProcessHeap(),0,cells);
    return error ? error : ending;
}

DWORD run16_native_view_present(run16_native_backend *backend,run16_native_console_view *view)
{
    return present_snapshot(backend,view,TRUE);
}

DWORD run16_native_view_sync_console(run16_native_backend *backend,run16_native_console_view *view)
{
    return present_snapshot(backend,view,FALSE);
}

DWORD run16_native_view_reclaim_input(run16_native_backend *backend,
    run16_native_console_view *view,DWORD *returned)
{
    return run16_native_view_reclaim_input_to(backend,view,returned,NULL,NULL);
}

DWORD run16_native_view_reclaim_input_to(run16_native_backend *backend,
    run16_native_console_view *view,DWORD *returned,
    DWORD (*sink)(void *,const INPUT_RECORD *,DWORD),void *context)
{
    typedef BOOL (WINAPI *prepend_input)(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
    prepend_input prepend=(prepend_input)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
        "WriteConsoleInputVDMW");
    INPUT_RECORD batch[RUN16_NATIVE_HOST_INPUTS],*records=NULL;
    run16_native_host_reply reply;
    DWORD error=ERROR_SUCCESS,total=0,written=0;
    if(!backend || !view || !returned)return ERROR_INVALID_PARAMETER;
    *returned=0;
    if(!sink && !prepend)return ERROR_CALL_NOT_IMPLEMENTED;
    for(;;) {
        INPUT_RECORD *grown;
        SIZE_T bytes;
        error=exchange(backend,RUN16_NATIVE_INPUT_READ,NULL,0,0,ARRAYSIZE(batch),&reply,batch,sizeof(batch));
        if(error)break;
        if(reply.count>ARRAYSIZE(batch) || reply.bytes!=reply.count*sizeof(INPUT_RECORD)) {
            error=ERROR_INVALID_DATA;break;
        }
        if(!reply.count)break;
        if((uint64_t)total+reply.count>((SIZE_T)-1)/sizeof(INPUT_RECORD)) {
            error=ERROR_NOT_ENOUGH_MEMORY;break;
        }
        bytes=((SIZE_T)total+reply.count)*sizeof(INPUT_RECORD);
        grown=records ? HeapReAlloc(GetProcessHeap(),0,records,bytes) : HeapAlloc(GetProcessHeap(),0,bytes);
        if(!grown) { error=ERROR_NOT_ENOUGH_MEMORY;break; }
        records=grown;
        memcpy(records+total,batch,reply.bytes);total+=reply.count;
    }
    /* A single native prepend retains cross-tile order and stays ahead of
     * newer host arrivals. Appending or prepending each tile would reorder
     * typeahead. Original Console Server owns the insertion, not a local
     * keyboard parser or a second guest input queue. */
    if(!error && total) {
        if(sink)error=sink(context,records,total);
        else if(!prepend(view->input,records,total,&written))error=GetLastError();
        else if(written!=total)error=ERROR_WRITE_FAULT;
    }
    if(!error)*returned=total;
    if(records)HeapFree(GetProcessHeap(),0,records);
    return error;
}

DWORD run16_native_view_forward_input(run16_native_backend *backend,run16_native_console_view *view)
{
    INPUT_RECORD records[64];
    DWORD available=0,count=0,i,kept=0;
    if(!PeekConsoleInputW(view->input,records,ARRAYSIZE(records),&available))return GetLastError();
    if(!available)return ERROR_SUCCESS;
    if(!ReadConsoleInputW(view->input,records,available,&count))return GetLastError();
    /* The geometry exchange invokes the real native resize, which generates
     * its own mode-dependent notification. Do not inject a duplicate/stale
     * visible-buffer WINDOW_BUFFER_SIZE_EVENT into the hidden input queue. */
    for(i=0;i<count;++i) {
        BOOL keep=TRUE;DWORD error;
        if(view->console_input) {
            error=view->console_input(view->window_context,&records[i],&keep);
            if(error)return error;
        }
        if(keep && records[i].EventType!=WINDOW_BUFFER_SIZE_EVENT)records[kept++]=records[i];
    }
    count=kept;
    return run16_native_backend_input(backend,records,count);
}

DWORD run16_native_view_wait(run16_native_backend *backend,run16_native_console_view *view,
    HANDLE target,DWORD *result)
{
    DWORD error=ERROR_SUCCESS,wait;
    HANDLE waits[3];
    if(!backend || !view || !target || !result)return ERROR_INVALID_PARAMETER;
    view->presentation_error=ERROR_SUCCESS;
    waits[0]=target;waits[1]=run16_native_backend_process(backend);waits[2]=view->input;
    for(;;) {
        BOOL ended=WaitForSingleObject(target,0)==WAIT_OBJECT_0;
        error=run16_native_view_present(backend,view);
        if(error && error!=ERROR_RETRY)break;
        if(ended)break;
        wait=WaitForMultipleObjects(3,waits,FALSE,30);
        if(wait==WAIT_OBJECT_0 || wait==WAIT_TIMEOUT)continue;
        if(wait!=WAIT_OBJECT_0+2) {
            error=wait==WAIT_OBJECT_0+1 ? ERROR_BROKEN_PIPE : GetLastError();
            break;
        }
        error=run16_native_view_forward_input(backend,view);
        if(error)break;
    }
    view->presentation_error=error;
    /* Completion is authoritative even if the last frame/input exchange lost
     * its peer. Recheck here to cover completion during a failed exchange.
     * A still-running target is neither success nor forcibly terminated. */
    wait=WaitForSingleObject(target,0);
    if(wait==WAIT_OBJECT_0) {
        if(!GetExitCodeProcess(target,result))return GetLastError();
        return ERROR_SUCCESS;
    }
    if(wait==WAIT_FAILED)return GetLastError();
    return error;
}
