#include "console_graphics.h"
/* DIVERGENCE(ADAPTER-WIN32-058): original ntcon bitmap/mutex service bound
 * locally, with copied presentation to run16; no Console server or KVM UI. */
#include "console_bitmap.h"
#include "console_client.h"
#include "conapi.h"
#undef CreateConsoleScreenBuffer
#undef SetConsoleActiveScreenBuffer
#undef CloseHandle

struct ntvdm_console_graphics {
    SRWLOCK lock;
    ntvdm_console_bitmap *bitmap;
    HANDLE identity;
    HPALETTE palette;
    BYTE *snapshot;
    DWORD bytes,width,height;
    BOOL active;
    int cursor_count;
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
    if (state->snapshot) HeapFree(GetProcessHeap(),0,state->snapshot);
    state->identity=NULL;state->snapshot=NULL;state->bytes=0;state->active=FALSE;
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

static BOOL publish(ntvdm_console_graphics *state)
{
    ntvdm_bitmap_description local;
    console_video_description copied;
    unsigned int i;
    if (!state->active) return TRUE;
    if (!ntvdm_console_bitmap_copy(state->bitmap,state->snapshot,state->bytes,&local,INFINITE)) return FALSE;
    copied.width=local.width;copied.height=local.height;copied.stride=local.stride;
    copied.depth=local.depth;copied.bytes=local.bytes;
    for (i=0;i<256;++i) copied.palette[i]=((uint32_t)local.palette[i].rgbRed<<16) |
        ((uint32_t)local.palette[i].rgbGreen<<8) | local.palette[i].rgbBlue;
    /* copy has released the painter mutex before the potentially blocking IPC. */
    return ntvdm_console_publish_video(&copied,state->snapshot,state->bytes);
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
    state->snapshot=HeapAlloc(GetProcessHeap(),0,state->bytes);
    state->identity=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!state->snapshot || !state->identity) {
        error=state->identity ? ERROR_NOT_ENOUGH_MEMORY : GetLastError();
        close_bitmap(state);CloseHandle(mutex);goto done;
    }
    result=state->identity;info->hMutex=mutex;info->lpBitMap=pixels;
done:
    ReleaseSRWLockExclusive(&state->lock);SetLastError(error);return result;
}

BOOL WINAPI MvdmSetConsoleActiveScreenBuffer(HANDLE output)
{
    ntvdm_console_graphics *state=ntvdm_console_graphics_context();
    BOOL result;
    if (!state) return SetConsoleActiveScreenBuffer(output);
    AcquireSRWLockExclusive(&state->lock);
    if (output && output==state->identity) {
        BOOL was_active=state->active;
        state->active=TRUE;
        result=was_active || publish(state);
        /* Original creation selects the surface before publishing its first
         * logical palette. A later reactivation must republish existing pixels. */
        if (!result && GetLastError()==ERROR_NOT_READY) result=TRUE;
    }
    else if (output==GetStdHandle(STD_OUTPUT_HANDLE)) {
        result=ntvdm_console_publish_video(NULL,NULL,0);
        if (result) state->active=FALSE;
    } else result=SetConsoleActiveScreenBuffer(output);
    ReleaseSRWLockExclusive(&state->lock);return result;
}

BOOL WINAPI MvdmCloseConsoleHandle(HANDLE handle)
{
    ntvdm_console_graphics *state=ntvdm_console_graphics_context();
    BOOL result;
    if (!state) return CloseHandle(handle);
    AcquireSRWLockExclusive(&state->lock);
    if (handle && handle==state->identity) {
        DWORD error;
        result=!state->active || ntvdm_console_publish_video(NULL,NULL,0);
        error=GetLastError();
        close_bitmap(state);
        if (!result) SetLastError(error);
    } else result=CloseHandle(handle);
    ReleaseSRWLockExclusive(&state->lock);return result;
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
        } else result=publish(state) ? 1 : -1;
    }
    ReleaseSRWLockExclusive(&state->lock);return result;
}

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
            result=publish(state) ? 1 : -1;
        }
    }
    ReleaseSRWLockExclusive(&state->lock);return result;
}
