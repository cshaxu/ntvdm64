#include "window_controller.h"
#include <limits.h>
#include <string.h>

/* Recover the old presentation/window_controller.c's source-local close,
 * route and checked destruction contracts, not its worker-owned providers,
 * input reader or polling thread. One frontend I/O owner drives this leaf.
 * OpenNT has no standalone display-policy equivalent; this is the explicitly
 * admitted product presentation policy, not another guest scheduler. */
struct frontend_window_controller {
    frontend_window_callbacks callbacks;
    frontend_window_input_queue input;
    kvm_window *window;
    kvm_window_frame *frame;
    HANDLE wake;
    volatile LONG failure,requested_console,requested_mouse_release;
    LONG epoch;
    frontend_display_mode display;
    BOOL graphics,frame_dirty,route_known,route_window,route_graphics;
    char title[KVM_WINDOW_TITLE_CAPACITY];
};

static DWORD status_error(lib_status status)
{
    if(status==LIB_STATUS_OK)return ERROR_SUCCESS;
    if(status==LIB_STATUS_NO_MEMORY)return ERROR_NOT_ENOUGH_MEMORY;
    if(status==LIB_STATUS_INVALID_ARGUMENT)return ERROR_INVALID_PARAMETER;
    if(status==LIB_STATUS_UNSUPPORTED)return ERROR_NOT_SUPPORTED;
    return ERROR_IO_DEVICE;
}
static void fail(frontend_window_controller *owner,DWORD error)
{
    InterlockedCompareExchange(&owner->failure,(LONG)(error ? error : ERROR_GEN_FAILURE),0);
    SetEvent(owner->wake);
}
static void window_failure(void *context,lib_u64 source,lib_status status)
{
    (void)source;
    fail(context,status_error(status));
}
static lib_bool window_input(void *context,const kvm_input_event *event)
{
    frontend_window_controller *owner=context;
    if(event->type==KVM_EVENT_HOTKEY &&
        !strcmp((const char *)event->data.hotkey.identifier,"release-mouse")) {
        InterlockedExchange(&owner->requested_mouse_release,owner->epoch);
        if(!SetEvent(owner->wake)) { fail(owner,GetLastError());return LIB_FALSE; }
        return LIB_TRUE;
    }
    if(event->type==KVM_EVENT_WINDOW_CLOSE ||
        (event->type==KVM_EVENT_HOTKEY &&
         !strcmp((const char *)event->data.hotkey.identifier,"console"))) {
        /* Repeated X/CAF/AE requests are the same idempotent assignment.
         * Tag it so an old window's queued close cannot close a new instance. */
        InterlockedExchange(&owner->requested_console,owner->epoch);
        if(!SetEvent(owner->wake)) { fail(owner,GetLastError());return LIB_FALSE; }
        return LIB_TRUE;
    }
    if(!frontend_window_input_queue_push(&owner->input,event)) {
        fail(owner,GetLastError());return LIB_FALSE;
    }
    if(!SetEvent(owner->wake)) { fail(owner,GetLastError());return LIB_FALSE; }
    return LIB_TRUE;
}
static DWORD drain_input(frontend_window_controller *owner)
{
    frontend_window_input event;
    while(frontend_window_input_queue_pop(&owner->input,&event))
        if(!owner->callbacks.input(owner->callbacks.context,&event))return ERROR_IO_DEVICE;
    return GetLastError()==ERROR_NO_DATA ? ERROR_SUCCESS : GetLastError();
}
static DWORD route(frontend_window_controller *owner,BOOL window)
{
    DWORD error;
    if(owner->route_known && owner->route_window==window &&
        owner->route_graphics==owner->graphics)return ERROR_SUCCESS;
    error=owner->callbacks.route(owner->callbacks.context,window,owner->graphics);
    if(!error) {
        owner->route_known=TRUE;owner->route_window=window;
        owner->route_graphics=owner->graphics;
    }
    return error;
}
static DWORD close_window(frontend_window_controller *owner)
{
    DWORD error=status_error(kvm_window_destroy(owner->window));
    if(error)return error; /* Keep live callback storage on failed join. */
    owner->window=NULL;
    error=drain_input(owner); /* Join includes final source-retirement event. */
    if(error)return error;
    return route(owner,FALSE);
}
static DWORD apply(frontend_window_controller *owner)
{
    DWORD error;
    BOOL want=owner->frame->valid &&
        (owner->display==FRONTEND_DISPLAY_WINDOW || owner->graphics);
    if(!want)return close_window(owner);
    if(!owner->window) {
        kvm_window_options options={0};
        if(owner->epoch==LONG_MAX)return ERROR_ARITHMETIC_OVERFLOW;
        ++owner->epoch;
        options.component.input_context=owner;
        options.component.input_sink=window_input;
        options.component.failure_context=owner;
        options.component.failure_sink=window_failure;
        options.initial_title=owner->title;
        kvm_hotkey_registry_initialize(&options.component.hotkeys);
        error=status_error(kvm_hotkey_registry_register(&options.component.hotkeys,
            'F',KVM_KEY_MODIFIER_CONTROL|KVM_KEY_MODIFIER_ALT,"console"));
        if(!error)error=status_error(kvm_hotkey_registry_register(&options.component.hotkeys,
            KVM_KEY_ENTER,KVM_KEY_MODIFIER_ALT,"console"));
        if(!error)error=status_error(kvm_hotkey_registry_register(&options.component.hotkeys,
            'M',KVM_KEY_MODIFIER_CONTROL|KVM_KEY_MODIFIER_ALT,"release-mouse"));
        if(error)return error;
        error=status_error(kvm_window_create(&owner->window,&options));
        if(error)return error;
        owner->frame_dirty=TRUE;
    }
    error=route(owner,TRUE);
    if(!error && owner->frame_dirty) {
        error=status_error(kvm_window_publish_frame(owner->window,owner->frame));
        if(!error)owner->frame_dirty=FALSE;
    }
    return error;
}
DWORD frontend_window_create(frontend_window_controller **result,
    const frontend_window_callbacks *callbacks,const char *title)
{
    frontend_window_controller *owner;
    if(!result)return ERROR_INVALID_PARAMETER;
    *result=NULL;
    if(!callbacks || !callbacks->input || !callbacks->route || !title ||
        strlen(title)>=KVM_WINDOW_TITLE_CAPACITY)return ERROR_INVALID_PARAMETER;
    owner=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*owner));
    if(!owner)return ERROR_NOT_ENOUGH_MEMORY;
    owner->callbacks=*callbacks;
    if(!frontend_window_input_queue_create(&owner->input)) {
        DWORD error=GetLastError();HeapFree(GetProcessHeap(),0,owner);return error;
    }
    owner->wake=CreateEventW(NULL,TRUE,FALSE,NULL);
    owner->frame=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*owner->frame));
    if(!owner->wake || !owner->frame) {
        DWORD error=owner->frame ? GetLastError() : ERROR_NOT_ENOUGH_MEMORY;
        if(owner->wake)CloseHandle(owner->wake);
        if(owner->frame)HeapFree(GetProcessHeap(),0,owner->frame);
        frontend_window_input_queue_destroy(&owner->input);
        HeapFree(GetProcessHeap(),0,owner);return error;
    }
    strcpy_s(owner->title,sizeof(owner->title),title);
    *result=owner;return ERROR_SUCCESS;
}
HANDLE frontend_window_wake(frontend_window_controller *owner)
{
    return owner ? owner->wake : NULL;
}
DWORD frontend_window_poll(frontend_window_controller *owner)
{
    LONG requested,release,failure;
    DWORD error;
    if(!owner)return ERROR_INVALID_PARAMETER;
    if(!ResetEvent(owner->wake))return GetLastError();
    requested=InterlockedExchange(&owner->requested_console,0);
    release=InterlockedExchange(&owner->requested_mouse_release,0);
    failure=InterlockedCompareExchange(&owner->failure,0,0);
    if(failure)return (DWORD)failure;
    error=drain_input(owner);
    if(error)return error;
    if(release && release==owner->epoch && owner->window) {
        error=status_error(kvm_window_release_mouse(owner->window));
        if(error)return error;
    }
    if(requested && requested==owner->epoch)owner->display=FRONTEND_DISPLAY_CONSOLE;
    return apply(owner);
}
DWORD frontend_window_select(frontend_window_controller *owner,frontend_display_mode display)
{
    DWORD error;
    if(!owner || (display!=FRONTEND_DISPLAY_CONSOLE && display!=FRONTEND_DISPLAY_WINDOW))
        return ERROR_INVALID_PARAMETER;
    error=frontend_window_poll(owner);
    if(error)return error;
    owner->display=display;
    return apply(owner);
}
DWORD frontend_window_present(frontend_window_controller *owner,const kvm_window_frame *frame,BOOL graphics)
{
    if(!owner || !frame)return ERROR_INVALID_PARAMETER;
    if(graphics && !frame->graphics)return ERROR_INVALID_DATA;
    if(!kvm_window_frame_copy(owner->frame,frame))return ERROR_INVALID_DATA;
    owner->frame_dirty=TRUE;
    owner->graphics=graphics!=FALSE;
    return frontend_window_poll(owner);
}
DWORD frontend_window_set_title(frontend_window_controller *owner,const char *title)
{
    DWORD error;
    if(!owner || !title || strlen(title)>=sizeof(owner->title))return ERROR_INVALID_PARAMETER;
    if(!strcmp(owner->title,title))return ERROR_SUCCESS;
    if(owner->window) {
        error=status_error(kvm_window_set_title(owner->window,title));
        if(error)return error;
    }
    strcpy_s(owner->title,sizeof(owner->title),title);
    return ERROR_SUCCESS;
}
DWORD frontend_window_clear(frontend_window_controller *owner)
{
    if(!owner)return ERROR_INVALID_PARAMETER;
    owner->frame->valid=LIB_FALSE;
    owner->graphics=FALSE;
    return frontend_window_poll(owner);
}
DWORD frontend_window_destroy(frontend_window_controller *owner)
{
    DWORD error;
    if(!owner)return ERROR_SUCCESS;
    error=close_window(owner);
    if(error)return error;
    CloseHandle(owner->wake);
    frontend_window_input_queue_destroy(&owner->input);
    HeapFree(GetProcessHeap(),0,owner->frame);
    HeapFree(GetProcessHeap(),0,owner);
    return ERROR_SUCCESS;
}
BOOL frontend_window_visible(const frontend_window_controller *owner)
{
    return owner && owner->window!=NULL;
}
frontend_display_mode frontend_window_mode(const frontend_window_controller *owner)
{
    return owner ? owner->display : FRONTEND_DISPLAY_CONSOLE;
}
