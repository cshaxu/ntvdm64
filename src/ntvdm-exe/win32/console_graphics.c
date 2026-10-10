#include "console_graphics.h"
/* DIVERGENCE(ADAPTER-WIN32-058): original ntcon bitmap/mutex service bound
 * locally, with copied presentation to run16; no Console server or KVM UI. */
#include "console_bitmap.h"
#include "console_client.h"
#include "worker-base/publication.h"
#include "conapi.h"
#undef CreateConsoleScreenBuffer
#undef SetConsoleActiveScreenBuffer
#undef CloseHandle

struct ntvdm_console_graphics {
    SRWLOCK lock;
    ntvdm_console_bitmap *bitmap;
    HANDLE identity;
    HPALETTE palette;
    DWORD bytes,width,height;
    BOOL active,dirty;
    int cursor_count;
    worker_base_publication *publisher;
};

ntvdm_console_graphics *ntvdm_console_graphics_create(void)
{
    ntvdm_console_graphics *state=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*state));
    if (state) InitializeSRWLock(&state->lock);
    else SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return state;
}

static void close_bitmap(ntvdm_console_graphics *state)
{
    /* ntcon/output.c FreeScreenBuffer owns the last installed palette. */
    if (state->palette) DeleteObject(state->palette);
    state->palette=NULL;
    ntvdm_console_bitmap_destroy(state->bitmap);state->bitmap=NULL;
    if (state->identity) CloseHandle(state->identity);
    state->identity=NULL;state->bytes=0;state->active=FALSE;state->dirty=FALSE;
    state->cursor_count=0;
}

BOOL ntvdm_console_graphics_cursor(HANDLE output,BOOL show,int *count)
{
    ntvdm_console_graphics *state=ntvdm_console_graphics_context();
    BOOL owned=FALSE;
    if (!state) return FALSE;
    AcquireSRWLockExclusive(&state->lock);
    if (output && output==state->identity) {
        /* Original ntcon/private.c SrvShowConsoleCursor keeps this counter
         * on the screen buffer, separate from its text insertion cursor. */
        *count=show ? ++state->cursor_count : --state->cursor_count;
        owned=TRUE;
    }
    ReleaseSRWLockExclusive(&state->lock);
    return owned;
}

void ntvdm_console_graphics_destroy(ntvdm_console_graphics *state)
{
    if (!state) return;
    close_bitmap(state); /* session teardown has already joined its users */
    HeapFree(GetProcessHeap(),0,state);
}

static DWORD capture(ntvdm_console_graphics *state,void **payload,SIZE_T *payload_bytes)
{
    ntvdm_bitmap_description local;
    console_video_description *copied;
    SIZE_T bytes;
    unsigned int i;
    BYTE *copy;
    if(!payload || !payload_bytes)return ERROR_INVALID_PARAMETER;
    *payload=NULL;*payload_bytes=0;
    AcquireSRWLockExclusive(&state->lock);
    /* See flush(): physical Console selection is irrelevant to a copied
     * software-VGA frame.  A live graphics identity is the source lifetime
     * guard; route admission remains exclusively in worker-base. */
    if (!state->identity || !state->dirty) {ReleaseSRWLockExclusive(&state->lock);return ERROR_SUCCESS;}
    if((SIZE_T)state->bytes>SIZE_MAX-sizeof(*copied)) {
        ReleaseSRWLockExclusive(&state->lock);return ERROR_ARITHMETIC_OVERFLOW;
    }
    bytes=sizeof(*copied)+(SIZE_T)state->bytes;
    copy=HeapAlloc(GetProcessHeap(),0,bytes);
    if(!copy) {ReleaseSRWLockExclusive(&state->lock);return ERROR_NOT_ENOUGH_MEMORY;}
    copied=(console_video_description *)copy;
    ZeroMemory(copied,sizeof(*copied));
    if (!ntvdm_console_bitmap_copy(state->bitmap,copy+sizeof(*copied),state->bytes,&local,INFINITE)) {
        DWORD error=GetLastError();HeapFree(GetProcessHeap(),0,copy);
        /* A palette-indexed VGA surface becomes active before the guest's
         * palette install.  That is a not-yet-publishable snapshot, not a
         * broken publication route.  Keep dirty set; the palette install
         * signals the existing publisher again once capture is possible. */
        if(error==ERROR_NOT_READY)error=ERROR_SUCCESS;
        ReleaseSRWLockExclusive(&state->lock);return error;
    }
    copied->width=local.width;copied->height=local.height;copied->stride=local.stride;
    copied->depth=local.depth;copied->bytes=local.bytes;
    for (i=0;i<256;++i) copied->palette[i]=((uint32_t)local.palette[i].rgbRed<<16) |
        ((uint32_t)local.palette[i].rgbGreen<<8) | local.palette[i].rgbBlue;
    state->dirty=FALSE;
    ReleaseSRWLockExclusive(&state->lock);
    *payload=copy;*payload_bytes=bytes;
    return ERROR_SUCCESS;
}

static DWORD capture_callback(void *context,void **payload,SIZE_T *payload_bytes)
{
    return capture(context,payload,payload_bytes);
}

DWORD ntvdm_console_graphics_attach_publisher(ntvdm_console_graphics *state,
    worker_base_publication *publisher)
{
    DWORD error;
    if(!state || !publisher)return ERROR_INVALID_PARAMETER;
    AcquireSRWLockExclusive(&state->lock);
    if(state->publisher) {ReleaseSRWLockExclusive(&state->lock);return ERROR_BUSY;}
    state->publisher=publisher;
    ReleaseSRWLockExclusive(&state->lock);
    error=worker_base_publication_set_capture(publisher,capture_callback,state);
    if(error) {
        AcquireSRWLockExclusive(&state->lock);state->publisher=NULL;ReleaseSRWLockExclusive(&state->lock);
    }
    return error;
}

static BOOL signal_dirty(ntvdm_console_graphics *state)
{
    DWORD error=state->publisher ? worker_base_publication_signal(state->publisher) : ERROR_NOT_READY;
    if(error) SetLastError(error);
    return !error;
}

HANDLE WINAPI MvdmCreateConsoleScreenBuffer(DWORD access,DWORD share,
    const SECURITY_ATTRIBUTES *security,DWORD flags,void *data)
{
    ntvdm_console_graphics *state=ntvdm_console_graphics_context();
    CONSOLE_GRAPHICS_BUFFER_INFO *info=data;
    HANDLE result=INVALID_HANDLE_VALUE,mutex=NULL;
    void *pixels=NULL;
    DWORD error=ERROR_SUCCESS;
    if (flags!=CONSOLE_GRAPHICS_BUFFER)
        return CreateConsoleScreenBuffer(access,share,security,flags,data);
    if (!info) { SetLastError(ERROR_INVALID_PARAMETER);return result; }
    info->hMutex=NULL;info->lpBitMap=NULL;
    if (!state) { SetLastError(ERROR_NOT_READY);return result; }
    if (security || access!=(GENERIC_READ|GENERIC_WRITE) || share!=(FILE_SHARE_READ|FILE_SHARE_WRITE)) {
        SetLastError(ERROR_NOT_SUPPORTED);return result;
    }
    AcquireSRWLockExclusive(&state->lock);
    if (state->bitmap) { error=ERROR_ALREADY_EXISTS;goto done; }
    if (!ntvdm_console_bitmap_create(info->lpBitMapInfo,info->dwBitMapInfoLength,
        info->dwUsage,&state->bitmap,&pixels,&mutex)) { error=GetLastError();goto done; }
    state->bytes=ntvdm_console_bitmap_bytes(state->bitmap);
    state->width=(DWORD)info->lpBitMapInfo->bmiHeader.biWidth;
    state->height=(DWORD)(info->lpBitMapInfo->bmiHeader.biHeight<0 ?
        -info->lpBitMapInfo->bmiHeader.biHeight : info->lpBitMapInfo->bmiHeader.biHeight);
    state->identity=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!state->identity) {
        error=GetLastError();
        close_bitmap(state);CloseHandle(mutex);goto done;
    }
    result=state->identity;info->hMutex=mutex;info->lpBitMap=pixels;
done:
    ReleaseSRWLockExclusive(&state->lock);SetLastError(error);return result;
}

BOOL WINAPI MvdmSetConsoleActiveScreenBuffer(HANDLE output)
{
    ntvdm_console_graphics *state=ntvdm_console_graphics_context();
    BOOL result,retire=FALSE;
    if (!state) return SetConsoleActiveScreenBuffer(output);
    AcquireSRWLockExclusive(&state->lock);
    if (output && output==state->identity) {
        BOOL was_active=state->active;
        state->active=TRUE;state->dirty=TRUE;
        result=signal_dirty(state);
        if(!result)state->active=was_active;
    }
    else if (output==GetStdHandle(STD_OUTPUT_HANDLE)) {
        /* The source capture callback takes this lock.  Retire outside it so
         * the publication final-drain can capture the last VGA frame. */
        retire=TRUE;result=TRUE;
    } else result=SetConsoleActiveScreenBuffer(output);
    ReleaseSRWLockExclusive(&state->lock);
    if(retire) {
        result=ntvdm_console_publish_video(NULL,NULL,0);
        if(result) {
            AcquireSRWLockExclusive(&state->lock);state->active=FALSE;
            ReleaseSRWLockExclusive(&state->lock);
        }
    }
    return result;
}

BOOL WINAPI MvdmCloseConsoleHandle(HANDLE handle)
{
    ntvdm_console_graphics *state=ntvdm_console_graphics_context();
    BOOL result,retire=FALSE,close_graphics=FALSE;
    DWORD error=ERROR_SUCCESS;
    if (!state) return CloseHandle(handle);
    AcquireSRWLockExclusive(&state->lock);
    if (handle && handle==state->identity) {
        retire=state->active;close_graphics=TRUE;result=TRUE;
    } else result=CloseHandle(handle);
    ReleaseSRWLockExclusive(&state->lock);
    if(retire) {
        result=ntvdm_console_publish_video(NULL,NULL,0);
        error=GetLastError();
    }
    if(close_graphics) {
        AcquireSRWLockExclusive(&state->lock);close_bitmap(state);
        ReleaseSRWLockExclusive(&state->lock);
    }
    if (!result) SetLastError(error);
    return result;
}

int ntvdm_console_graphics_invalidate(HANDLE output,const SMALL_RECT *rect)
{
    ntvdm_console_graphics *state=ntvdm_console_graphics_context();
    int result=0;
    if (!state) return 0;
    AcquireSRWLockExclusive(&state->lock);
    if (output && output==state->identity) {
        if (!rect || rect->Left<0 || rect->Top<0 || rect->Right<rect->Left || rect->Bottom<rect->Top ||
            (DWORD)rect->Right>=state->width || (DWORD)rect->Bottom>=state->height) {
            SetLastError(ERROR_INVALID_PARAMETER);result=-1;
        } else {state->dirty=TRUE;result=signal_dirty(state) ? 1 : -1;}
    }
    ReleaseSRWLockExclusive(&state->lock);return result;
}

int ntvdm_console_graphics_flush(void)
{
    ntvdm_console_graphics *state=ntvdm_console_graphics_context();
    int result=0;
    if (!state) return 0;
    AcquireSRWLockExclusive(&state->lock);
    if (state->identity) {
        /* `host_flush_screen` is the original completion boundary: do not
         * infer rectangles or redraw a pointer here.  The existing capture
         * callback copies the complete, already-painted DIB.  `active`
         * records the legacy physical-Console buffer selection, not whether
         * this worker's copied software-VGA source is routable.  Window mode
         * owns a valid DIB without that old selection, so gating this flush
         * on `active` silently falls back to the coarse invalidation timer. */
        state->dirty=TRUE;
        result=signal_dirty(state) ? 1 : -1;
    }
    ReleaseSRWLockExclusive(&state->lock);
    return result;
}

#ifndef NTVDM_CONSOLE_GRAPHICS_FLUSH_PROBE_EXTERNAL
void ntvdm_console_graphics_flush_probe(DWORD screen_state,DWORD mode_type)
{
    UNREFERENCED_PARAMETER(screen_state);
    UNREFERENCED_PARAMETER(mode_type);
}
#endif

int ntvdm_console_graphics_palette(HANDLE output,HPALETTE palette,DWORD flags)
{
    ntvdm_console_graphics *state=ntvdm_console_graphics_context();
    int result=0;
    if (!state) return 0;
    AcquireSRWLockExclusive(&state->lock);
    if (output && output==state->identity) {
        if (flags!=SYSPAL_STATIC) {SetLastError(ERROR_NOT_SUPPORTED);result=-1;}
        else if (!ntvdm_console_bitmap_palette(state->bitmap,palette)) result=-1;
        else {
            /* ntcon/private.c SrvSetConsolePalette transfers ownership on
             * installation, before repaint; a failed transport is not rollback. */
            if (state->palette && state->palette!=palette) DeleteObject(state->palette);
            state->palette=palette;
            state->dirty=TRUE;result=signal_dirty(state) ? 1 : -1;
        }
    }
    ReleaseSRWLockExclusive(&state->lock);return result;
}
