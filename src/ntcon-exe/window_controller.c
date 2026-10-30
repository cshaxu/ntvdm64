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
    kvm_frame_exchange *frames;
    HANDLE wake;
    volatile LONG failure,requested_console,requested_mouse_release;
    LONG epoch;
    frontend_display_mode display;
    BOOL graphics,frame_valid,frame_dirty,route_known,route_window,route_graphics;
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
/* Console title producers retain their existing ACP wire. The imported
 * Window API is UTF-8. Owner-approved cosmetic truncation retains the longest
 * complete UTF-8 prefix; the producer title and wire remain unchanged. */
static DWORD encode_title(const char *title,char output[KVM_WINDOW_TITLE_CAPACITY])
{
    WCHAR wide[KVM_WINDOW_TITLE_CAPACITY];
    char encoded[3*KVM_WINDOW_TITLE_CAPACITY];int bytes;size_t length;
    if(!title || strlen(title)>=KVM_WINDOW_TITLE_CAPACITY)return ERROR_INVALID_PARAMETER;
    if(!MultiByteToWideChar(CP_ACP,0,title,-1,wide,ARRAYSIZE(wide)))return GetLastError();
    bytes=WideCharToMultiByte(CP_UTF8,0,wide,-1,encoded,sizeof(encoded),NULL,NULL);
    if(!bytes)return GetLastError();
    length=(size_t)bytes-1;
    if(length>=KVM_WINDOW_TITLE_CAPACITY) {
        length=KVM_WINDOW_TITLE_CAPACITY-1;
        while(((unsigned char)encoded[length]&0xc0)==0x80)--length;
    }
    memcpy(output,encoded,length);output[length]=0;
    return ERROR_SUCCESS;
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
        /* Repeated X/CAF requests are the same idempotent assignment.
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
    DWORD error=status_error(kvm_window_destroy(owner->window)),next;
    /* Every normal return has joined and consumed the library object. */
    owner->window=NULL;
    next=drain_input(owner); /* Join includes final source-retirement event. */
    if(!error)error=next;
    next=route(owner,FALSE);
    return error ? error : next;
}
static DWORD apply(frontend_window_controller *owner)
{
    DWORD error;
    BOOL want=owner->frame_valid &&
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
            'F',BASE_KEY_MODIFIER_CONTROL|BASE_KEY_MODIFIER_ALT,"console"));
        if(!error)error=status_error(kvm_hotkey_registry_register(&options.component.hotkeys,
            'M',BASE_KEY_MODIFIER_CONTROL|BASE_KEY_MODIFIER_ALT,"release-mouse"));
        if(error)return error;
        error=status_error(kvm_window_create(&owner->window,&options));
        if(error)return error;
        owner->frame_dirty=TRUE;
    }
    error=route(owner,TRUE);
    if(!error && owner->frame_dirty) {
        error=status_error(kvm_window_publish_frame(owner->window,owner->frames));
        if(!error)owner->frame_dirty=FALSE;
    }
    return error;
}
DWORD frontend_window_create(frontend_window_controller **result,
    const frontend_window_callbacks *callbacks,const char *title)
{
    frontend_window_controller *owner;
    char encoded[KVM_WINDOW_TITLE_CAPACITY];DWORD error;
    if(!result)return ERROR_INVALID_PARAMETER;
    *result=NULL;
    if(!callbacks || !callbacks->input || !callbacks->route)return ERROR_INVALID_PARAMETER;
    error=encode_title(title,encoded);
    if(error)return error;
    owner=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*owner));
    if(!owner)return ERROR_NOT_ENOUGH_MEMORY;
    owner->callbacks=*callbacks;
    if(!frontend_window_input_queue_create(&owner->input)) {
        error=GetLastError();HeapFree(GetProcessHeap(),0,owner);return error;
    }
    owner->wake=CreateEventW(NULL,TRUE,FALSE,NULL);
    error=owner->wake ? status_error(kvm_frame_exchange_create(&owner->frames,
        sizeof(kvm_window_frame_storage),kvm_window_frame_retire)) : GetLastError();
    if(error) {
        if(owner->wake)CloseHandle(owner->wake);
        frontend_window_input_queue_destroy(&owner->input);
        HeapFree(GetProcessHeap(),0,owner);return error;
    }
    strcpy_s(owner->title,sizeof(owner->title),encoded);
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
kvm_window_frame_storage *frontend_window_writer(frontend_window_controller *owner)
{
    return owner ? kvm_frame_exchange_writer(owner->frames) : NULL;
}
DWORD frontend_window_present(frontend_window_controller *owner,BOOL graphics)
{
    kvm_window_frame_storage *writer=frontend_window_writer(owner);
    if(!writer)return ERROR_INVALID_PARAMETER;
    if(graphics && !writer->frame.graphics)return ERROR_INVALID_DATA;
    if(kvm_window_frame_commit(owner->frames)!=LIB_STATUS_OK)return ERROR_INVALID_DATA;
    owner->frame_valid=TRUE;
    owner->frame_dirty=TRUE;
    owner->graphics=graphics!=FALSE;
    return frontend_window_poll(owner);
}
DWORD frontend_window_set_title(frontend_window_controller *owner,const char *title)
{
    DWORD error;char encoded[KVM_WINDOW_TITLE_CAPACITY];
    if(!owner)return ERROR_INVALID_PARAMETER;
    error=encode_title(title,encoded);
    if(error)return error;
    if(!strcmp(owner->title,encoded))return ERROR_SUCCESS;
    if(owner->window) {
        error=status_error(kvm_window_set_title(owner->window,encoded));
        if(error)return error;
    }
    strcpy_s(owner->title,sizeof(owner->title),encoded);
    return ERROR_SUCCESS;
}
DWORD frontend_window_clear(frontend_window_controller *owner)
{
    if(!owner)return ERROR_INVALID_PARAMETER;
    kvm_frame_exchange_invalidate(owner->frames);
    owner->frame_valid=FALSE;
    owner->graphics=FALSE;
    return frontend_window_poll(owner);
}
DWORD frontend_window_destroy(frontend_window_controller *owner)
{
    DWORD error;
    if(!owner)return ERROR_SUCCESS;
    error=close_window(owner);
    if(!CloseHandle(owner->wake) && !error)error=GetLastError();
    frontend_window_input_queue_destroy(&owner->input);
    kvm_frame_exchange_destroy(owner->frames);
    HeapFree(GetProcessHeap(),0,owner);
    return error;
}
BOOL frontend_window_visible(const frontend_window_controller *owner)
{
    return owner && owner->window!=NULL;
}
frontend_display_mode frontend_window_mode(const frontend_window_controller *owner)
{
    return owner ? owner->display : FRONTEND_DISPLAY_CONSOLE;
}
