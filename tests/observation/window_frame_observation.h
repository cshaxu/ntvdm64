#ifndef WINDOW_FRAME_OBSERVATION_H
#define WINDOW_FRAME_OBSERVATION_H
#include "lib/kvm-window/frame_interface.h"

/* Test-only, same-bitness observation of the pinned native leaf prefix.
 * Never a product IPC or a lock: callers reject changing samples, and actual
 * pixel/input checks remain necessary. Remote pointers are read, not followed
 * by local library functions. */
typedef struct observed_window_frame {
    const void *context;
    const kvm_window_frame *address;
    kvm_window_frame frame;
    kvm_text_frame text;
    kvm_window_graphics_frame graphics;
} observed_window_frame;

static BOOL observer_read_bytes(HANDLE process,const void *address,void *output,SIZE_T size)
{
    SIZE_T copied;
    return address && ReadProcessMemory(process,address,output,size,&copied) && copied==size;
}
static BOOL observer_window_frame_read(HANDLE process,HWND window,observed_window_frame *out)
{
    struct { void *owner; const kvm_window_frame *frame; } prefix,after;
    kvm_window_frame check;
    memset(out,0,sizeof(*out));
    out->context=(void *)GetWindowLongPtrW(window,GWLP_USERDATA);
    if(!observer_read_bytes(process,out->context,&prefix,sizeof(prefix)) ||
        !observer_read_bytes(process,prefix.frame,&out->frame,sizeof(out->frame)))return FALSE;
    out->address=prefix.frame;
    if((out->frame.text==NULL)==(out->frame.graphics==NULL))return FALSE;
    if(out->frame.text) {
        if(!observer_read_bytes(process,out->frame.text,&out->text,sizeof(out->text)))return FALSE;
    } else if(!observer_read_bytes(process,out->frame.graphics,&out->graphics,sizeof(out->graphics)))return FALSE;
    return observer_read_bytes(process,prefix.frame,&check,sizeof(check)) &&
        !memcmp(&out->frame,&check,sizeof(check)) &&
        observer_read_bytes(process,out->context,&after,sizeof(after)) &&
        !memcmp(&prefix,&after,sizeof(prefix)) &&
        out->context==(void *)GetWindowLongPtrW(window,GWLP_USERDATA);
}
#endif
