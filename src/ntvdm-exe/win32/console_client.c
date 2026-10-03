/* Worker-local transport only. No native Console presentation or guest policy. */
#include "console_client.h"
#include "console_text.h"
#include "common/console/client.h"
#include "opennt-abi/host-compat/include/console_grid.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "worker-base/connection.h"
#include "ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.h"
#include <stddef.h>
#include <limits.h>
#include <string.h>

static BOOL decode_input(const console_io_input *wire,INPUT_RECORD *record)
{
    if(wire->type==CONSOLE_INPUT_RELATIVE_MOUSE) {
        console_mouse_input mouse;
        if(wire->buttons>3 || wire->flags>UINT16_MAX)return FALSE;
        mouse.dx=wire->x;mouse.dy=wire->y;mouse.buttons=(uint16_t)wire->buttons;
        mouse.action=(uint16_t)wire->flags;mouse.width=(uint16_t)wire->control;
        mouse.height=(uint16_t)(wire->control>>16);
        if(!console_mouse_input_valid(&mouse))return FALSE;
        ZeroMemory(record,sizeof(*record));record->EventType=CONSOLE_INPUT_RELATIVE_MOUSE;
        memcpy(&record->Event,&mouse,sizeof(mouse));return TRUE;
    }
    return ntcon_worker_decode_input(wire,record);
}

typedef struct console_client {
    session *owner;
    ntcon_worker_client channel;
    HANDLE ready,wake,stop,rearm,watcher,shutdown;
    HANDLE capability,input_identity,output_identity;
    CRITICAL_SECTION lock;
    console_io_request request;
    ntvdm_console_graphics *graphics;
    PALETTEENTRY text_palette[16];
    BOOL text_palette_valid;
    console_text_configuration sent_configuration;
    BOOL configuration_sent;
    mvdm_mouse_bridge mouse;
} console_client;
static DWORD console_activate(console_client *,BOOL);
static console_client *output_client(HANDLE);

/* Same bound worker endpoint as copied keyboard input. Its existing teardown
 * runs after bound threads have joined, so no separate mouse lifetime exists. */
mvdm_mouse_bridge *mvdm_softpc_mouse_current(void)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    return client ? &client->mouse : NULL;
}

/* Worker-local identities only. DuplicateHandle aliases retain their role;
 * closed/reused handle values cannot impersonate a Console endpoint. Neither
 * object owns a Windows Console, input queue or screen buffer. */
DWORD ntvdm_console_handle_kind(HANDLE handle)
{
    typedef BOOL (WINAPI *COMPARE)(HANDLE,HANDLE);
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    COMPARE compare;
    if(!client || !handle || handle==INVALID_HANDLE_VALUE)return 0;
    compare=(COMPARE)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    if(!compare)return 0;
    if(client->input_identity && compare(handle,client->input_identity))return 1;
    if(client->output_identity && compare(handle,client->output_identity))return 2;
    return 0;
}
static HANDLE open_endpoint(BOOL input,DWORD access,LPSECURITY_ATTRIBUTES security,DWORD disposition,DWORD flags,HANDLE template_file)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    HANDLE result=NULL;
    if(!client || access!=(GENERIC_READ|GENERIC_WRITE) || disposition!=OPEN_EXISTING || flags || template_file){
        SetLastError(ERROR_INVALID_PARAMETER);return INVALID_HANDLE_VALUE;
    }
    if(!DuplicateHandle(GetCurrentProcess(),input ? client->input_identity : client->output_identity,
        GetCurrentProcess(),&result,0,security && security->bInheritHandle,DUPLICATE_SAME_ACCESS))return INVALID_HANDLE_VALUE;
    return result;
}
HANDLE WINAPI MvdmCreateFileA(LPCSTR name,DWORD access,DWORD share,LPSECURITY_ATTRIBUTES security,DWORD disposition,DWORD flags,HANDLE template_file)
{
    session *owner=session_thread_current();
    if(owner && owner->console_client && name && (!_stricmp(name,"CONIN$") || !_stricmp(name,"CONOUT$")))
        return open_endpoint(!_stricmp(name,"CONIN$"),access,security,disposition,flags,template_file);
    return CreateFileA(name,access,share,security,disposition,flags,template_file);
}
HANDLE WINAPI MvdmCreateFileW(LPCWSTR name,DWORD access,DWORD share,LPSECURITY_ATTRIBUTES security,DWORD disposition,DWORD flags,HANDLE template_file)
{
    session *owner=session_thread_current();
    if(owner && owner->console_client && name && (!_wcsicmp(name,L"CONIN$") || !_wcsicmp(name,L"CONOUT$")))
        return open_endpoint(!_wcsicmp(name,L"CONIN$"),access,security,disposition,flags,template_file);
    return CreateFileW(name,access,share,security,disposition,flags,template_file);
}
DWORD WINAPI MvdmGetFileType(HANDLE handle)
{
    return ntvdm_console_handle_kind(handle) ? FILE_TYPE_CHAR : GetFileType(handle);
}

/* DIVERGENCE(ADAPTER-WIN32-050): local mouse visibility bookkeeping only;
 * conhost/Terminal still owns the native pointer, not the text caret. */
static volatile LONG mvdm_pointer_display_count;
int WINAPI ShowConsoleCursor(HANDLE output, BOOL show)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    int count;
    if (ntvdm_console_graphics_cursor(output,show,&count)) return count;
    /* GetConsoleMode also accepts CONIN$, and rejects our graphics backing.
     * Original SrvShowConsoleCursor accepts output handles, never input. */
    if (!output_client(output) && !GetConsoleScreenBufferInfo(output, &info)) return -1;
    return (int)(show ? InterlockedIncrement(&mvdm_pointer_display_count) :
        InterlockedDecrement(&mvdm_pointer_display_count));
}

ntvdm_console_graphics *ntvdm_console_graphics_context(void)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    return client ? client->graphics : NULL;
}

void NtvdmConsoleTextColours(const PALETTEENTRY *colours)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    if (!client || !colours) return;
    EnterCriticalSection(&client->lock);
    memcpy(client->text_palette,colours,sizeof(client->text_palette));
    client->text_palette_valid=TRUE;
    LeaveCriticalSection(&client->lock);
}

BOOL ntvdm_console_text_palette(PALETTEENTRY colours[16])
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    BOOL valid;
    if (!client) { SetLastError(ERROR_NOT_READY);return FALSE; }
    EnterCriticalSection(&client->lock);
    valid=client->text_palette_valid;
    if (valid) memcpy(colours,client->text_palette,sizeof(client->text_palette));
    LeaveCriticalSection(&client->lock);
    SetLastError(valid ? ERROR_SUCCESS : ERROR_NO_DATA);
    return valid;
}

/* Original nt_event.c owns the VDM close decision/cleanup. The NT4 Console
 * Server used a callback thread and bounded wait before forced close. Our
 * authenticated broker instruction controls session retirement; no native
 * Console, PID ancestry, or guest/task policy is acquired here. */
extern BOOL CntrlHandler(ULONG type);
static DWORD WINAPI console_close_callback(void *context)
{
    console_client *client=context;
    if (!session_thread_bind(client->owner)) return ERROR_INVALID_STATE;
    (void)CntrlHandler(CTRL_CLOSE_EVENT);
    (void)session_thread_unbind(client->owner);
    return 0;
}

/* nt_event caches its wait handle once. Keep that local event stable; the
 * authenticated frontend event and process are notification sources only.
 * A signalled source is armed again after read/peek, avoiding a hot wait loop. */
static DWORD WINAPI console_input_watch(void *context)
{
    console_client *client=context;
    BOOL pending=FALSE;
    for (;;) {
        HANDLE waits[3]={client->shutdown,client->stop,pending ? client->rearm : client->ready};
        DWORD result=WaitForMultipleObjects(3,waits,FALSE,INFINITE);
        if (result==WAIT_OBJECT_0+1) return 0;
        if (result==WAIT_OBJECT_0) {
            HANDLE close=CreateThread(NULL,0,console_close_callback,client,0,NULL);
            /* The broker ordered closure: there is no remaining UI in which to
             * cancel closing this session. Bound a blocked original handler,
             * then close this worker only, as Console Server forced close did.
             * This timeout is a close grace, never a guest idle timeout. */
            if (close) { WaitForSingleObject(close,5000);CloseHandle(close); }
            TerminateProcess(GetCurrentProcess(),CONTROL_C_EXIT);
            return ERROR_PROCESS_ABORTED;
        }
        if (result!=WAIT_OBJECT_0+2) {
            SetEvent(client->wake);return result==WAIT_FAILED ? GetLastError() : ERROR_PIPE_NOT_CONNECTED;
        }
        if (!pending) SetEvent(client->wake);
        pending=!pending;
    }
}

static void console_client_end(void *context)
{
    console_client *client=context;
    client->owner->console_client=NULL;
    OpenNtBaseClientSetCommandBinding(NULL,NULL);
    if (client->stop) SetEvent(client->stop);
    if (client->watcher) {
        WaitForSingleObject(client->watcher,INFINITE);CloseHandle(client->watcher);
    }
    CloseHandle(client->channel.pipe);CloseHandle(client->channel.peer);
    ntcon_worker_client_dispose(&client->channel);
    CloseHandle(client->ready);
    if (client->capability) CloseHandle(client->capability);
    if (client->wake) CloseHandle(client->wake);
    if (client->stop) CloseHandle(client->stop);
    if (client->rearm) CloseHandle(client->rearm);
    if (client->shutdown) CloseHandle(client->shutdown);
    if (client->input_identity) CloseHandle(client->input_identity);
    if (client->output_identity) CloseHandle(client->output_identity);
    DeleteCriticalSection(&client->lock);
    ntvdm_console_graphics_destroy(client->graphics);
    HeapFree(GetProcessHeap(),0,client);
}

static DWORD console_command_ready(void *context)
{
    console_client *client=context;
    if (WaitForSingleObject(client->channel.peer,0)==WAIT_TIMEOUT) return console_activate(client,TRUE);
    /* A dead root closes this session; it cannot be rebound to a new root. */
    return ERROR_PIPE_NOT_CONNECTED;
}

DWORD ntvdm_console_client_begin(session *owner)
{
    typedef BOOL (WINAPI *compare_handles)(HANDLE,HANDLE);
    compare_handles compare;
    console_client *client;
    HANDLE root=NULL;
    DWORD error;
    if (!owner || owner->console_client) return ERROR_INVALID_STATE;
    client=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*client));
    if (!client) return ERROR_NOT_ENOUGH_MEMORY;
    error=OpenNtBaseClientWaitFrontend(&client->channel.pipe,&client->channel.peer,&client->channel.generation,&client->ready);
    if (error) { HeapFree(GetProcessHeap(),0,client);return error; }
    client->owner=owner;
    InitializeCriticalSection(&client->lock);
    client->graphics=ntvdm_console_graphics_create();
    if (!client->graphics) { error=GetLastError();console_client_end(client);return error; }
    error=OpenNtBaseClientWorkerFrontendCapability(&client->capability);
    if (error) { console_client_end(client);return error; }
    /* The route's peer and the capability must name the same authenticated
     * root. A short-lived launcher or a reused PID cannot substitute for it. */
    error=worker_base_retain_frontend_root(client->capability,&root);
    compare=(compare_handles)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    if(!error && !compare)error=ERROR_CALL_NOT_IMPLEMENTED;
    if(!error && !compare(root,client->channel.peer))error=ERROR_ACCESS_DENIED;
    if(root)CloseHandle(root);
    if(error){console_client_end(client);return error;}
    error=ntcon_worker_client_init(&client->channel,client->channel.pipe,
        client->channel.peer,NULL,client->channel.generation);
    if(error){console_client_end(client);return error;}
    client->wake=CreateEventW(NULL,TRUE,FALSE,NULL);
    client->stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    client->rearm=CreateEventW(NULL,FALSE,FALSE,NULL);
    client->input_identity=CreateEventW(NULL,TRUE,FALSE,NULL);
    client->output_identity=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!client->channel.event || !client->wake || !client->stop || !client->rearm ||
        !client->input_identity || !client->output_identity) {
        error=GetLastError();console_client_end(client);return error;
    }
    error=worker_base_shutdown_event(&client->shutdown);
    if(error){console_client_end(client);return error;}
    client->watcher=CreateThread(NULL,0,console_input_watch,client,0,NULL);
    error=client->watcher ? console_activate(client,TRUE) : GetLastError();
    if (!error && !session_register_teardown(owner,console_client_end,client))error=ERROR_NOT_ENOUGH_MEMORY;
    if (error) {
        console_client_end(client);return error;
    }
    owner->console_client=client;
    OpenNtBaseClientSetCommandBinding(console_command_ready,client);
    return ERROR_SUCCESS;
}

BOOL ntvdm_console_inherit_launch_capabilities(HANDLE *capability,HANDLE *execution)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    BOOL result=TRUE;
    HANDLE retained=NULL;
    DWORD error;
    if (!capability || !execution) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    *capability=NULL;*execution=NULL;
    if (!client) return TRUE;
    EnterCriticalSection(&client->lock);
    if (!client->capability) { SetLastError(ERROR_INVALID_STATE);result=FALSE; }
    else result=DuplicateHandle(GetCurrentProcess(),client->capability,
        GetCurrentProcess(),capability,SYNCHRONIZE,TRUE,0);
    LeaveCriticalSection(&client->lock);
    if (!result) return FALSE;
    /* A worker-created native child must receive this worker's verified
     * execution Console, never an old launcher-local handle from guest env. */
    error=OpenNtBaseClientAcquireConsoleContext(*capability,&retained);
    if (!error && !DuplicateHandle(GetCurrentProcess(),retained,GetCurrentProcess(),
        execution,SYNCHRONIZE,TRUE,0)) error=GetLastError();
    if (retained) CloseHandle(retained);
    if (error) {
        CloseHandle(*capability);*capability=NULL;
        SetLastError(error);return FALSE;
    }
    return result;
}

HANDLE ntvdm_console_input_wait_handle(void)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    return client ? client->wake : GetStdHandle(STD_INPUT_HANDLE);
}

static console_client *mode_client(HANDLE handle)
{
    session *owner=session_thread_current();
    DWORD mode;
    /* NUL is FILE_TYPE_CHAR too, but is not a Console. Preserve the native
     * error for non-Console handles instead of displaying their data remotely. */
    return owner && owner->console_client && (ntvdm_console_handle_kind(handle) || GetConsoleMode(handle,&mode)) ?
        owner->console_client : NULL;
}

static console_client *output_client(HANDLE output)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    console_client *client=mode_client(output);
    /* A CONIN$ handle also supports GetConsoleMode. It cannot authorize an
     * output operation on the frontend's unrelated CONOUT$ handle. */
    return client && (ntvdm_console_handle_kind(output)==2 ||
        (!ntvdm_console_handle_kind(output) && GetConsoleScreenBufferInfo(output,&info))) ? client : NULL;
}

static console_client *input_client(HANDLE input)
{
    DWORD count;
    console_client *client=mode_client(input);
    return client && (ntvdm_console_handle_kind(input)==1 ||
        (!ntvdm_console_handle_kind(input) && GetNumberOfConsoleInputEvents(input,&count))) ? client : NULL;
}

/* Called with the per-session lock held; all operations share one ordered
 * request sequence, including stream chunks and geometry queries. */
static DWORD exchange(console_client *client,console_io_reply *reply)
{
    return ntcon_worker_exchange(&client->channel,&client->request,reply);
}

static DWORD console_activate(console_client *client,BOOL active)
{
    DWORD error;
    EnterCriticalSection(&client->lock);
    /* NTCON owns the I/O predicate and waits for its actual binding change
     * inside this same activation request. It returns one bounded failure;
     * this worker does not sample another process's ownership on a timer. */
    error=ntcon_worker_activate(&client->channel,0,active);
    /* Unlike IRQ cancellation, successful DOS ownership handoff retires the
     * frontend's DOS mouse route and discards its copied relative records.
     * Original nt_block_event_thread has quiesced the event/timer producers
     * before this call. Match that acknowledged retirement locally; the next
     * DOS activation will receive a fresh ENTER. Never reset on failed RPC. */
    if(!error && !active)ZeroMemory(&client->mouse,sizeof(client->mouse));
    LeaveCriticalSection(&client->lock);
    return error;
}
BOOL ntvdm_console_set_active(BOOL active)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    DWORD error=client ? console_activate(client,active) : ERROR_SUCCESS;
    SetLastError(error);return error==ERROR_SUCCESS;
}

BOOL ntvdm_console_publish_video(const console_video_description *description,
    const void *pixels,size_t capacity)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    DWORD error=ERROR_SUCCESS;
    if (!client) { SetLastError(ERROR_NOT_READY);return FALSE; }
    if ((description && (!pixels || capacity<description->bytes || !description->bytes)) ||
        (!description && (pixels || capacity))) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    EnterCriticalSection(&client->lock);
    if(description && description->kind==CONSOLE_VIDEO_TEXT_CONFIGURATION &&
        description->bytes==sizeof(console_text_style) && client->configuration_sent &&
        !memcmp(pixels,&client->sent_configuration.style,sizeof(console_text_style)) &&
        !memcmp(description->palette,client->sent_configuration.palette,
            sizeof(client->sent_configuration.palette)))goto done;
    error=ntcon_worker_video(&client->channel,description,pixels);
    if(!error && description && description->kind==CONSOLE_VIDEO_TEXT_CONFIGURATION &&
        description->bytes==sizeof(console_text_style)) {
        memcpy(&client->sent_configuration.style,pixels,sizeof(console_text_style));
        memcpy(client->sent_configuration.palette,description->palette,
            sizeof(client->sent_configuration.palette));
        client->configuration_sent=TRUE;
    } else if(!error)client->configuration_sent=FALSE;
done:
    LeaveCriticalSection(&client->lock);
    SetLastError(error);
    return error==ERROR_SUCCESS;
}

static BOOL state_operation(console_client *client,DWORD operation,
    const console_io_state *state,console_io_state *result)
{
    console_io_reply reply;
    DWORD error;
    BOOL ok;
    EnterCriticalSection(&client->lock);
    ZeroMemory(&client->request,offsetof(console_io_request,data));
    client->request.operation=operation;
    if (state) client->request.state=*state;
    error=exchange(client,&reply);
    ok=!error && reply.result;
    if (!error) {
        if (result) *result=reply.state;
        if (!ok) error=reply.error;
    }
    LeaveCriticalSection(&client->lock);
    if (!ok) SetLastError(error);
    return ok;
}

static UINT console_code_page(BOOL input)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_state request={0},reply={0};
    if (!client) return input ? GetConsoleCP() : GetConsoleOutputCP();
    request.input=input;
    return state_operation(client,CONSOLE_IO_CODE_PAGE,&request,&reply) ? reply.count : 0;
}

BOOL WINAPI MvdmGetCurrentConsoleFont(HANDLE output,BOOL maximum,PCONSOLE_FONT_INFO font)
{
    console_client *client=output_client(output);
    console_io_state request={0},reply={0};
    if (!client) return GetCurrentConsoleFont(output,maximum,font);
    if (!font) { SetLastError(ERROR_INVALID_ACCESS);return FALSE; }
    request.mode=maximum!=FALSE;
    if (!state_operation(client,CONSOLE_IO_CURRENT_FONT,&request,&reply)) return FALSE;
    if (reply.x<SHRT_MIN || reply.x>SHRT_MAX || reply.y<SHRT_MIN || reply.y>SHRT_MAX) {
        SetLastError(ERROR_INVALID_DATA);return FALSE;
    }
    font->nFont=reply.count;
    font->dwFontSize.X=(SHORT)reply.x;font->dwFontSize.Y=(SHORT)reply.y;
    return TRUE;
}

COORD WINAPI MvdmGetConsoleFontSize(HANDLE output,DWORD index)
{
    console_client *client=output_client(output);
    console_io_state request={0},reply={0};
    COORD size={0,0};
    if (!client) return GetConsoleFontSize(output,index);
    request.count=index;
    if (state_operation(client,CONSOLE_IO_FONT_SIZE,&request,&reply)) {
        if (reply.x<SHRT_MIN || reply.x>SHRT_MAX || reply.y<SHRT_MIN || reply.y>SHRT_MAX)
            SetLastError(ERROR_INVALID_DATA);
        else { size.X=(SHORT)reply.x;size.Y=(SHORT)reply.y; }
    }
    return size;
}

UINT WINAPI MvdmGetConsoleCP(void) { return console_code_page(TRUE); }
UINT WINAPI MvdmGetConsoleOutputCP(void) { return console_code_page(FALSE); }

BOOL WINAPI MvdmGetConsoleDisplayMode(LPDWORD mode)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_state request={0},reply={0};
    if (!client) return GetConsoleDisplayMode(mode);
    if (!mode) { SetLastError(ERROR_INVALID_ACCESS);return FALSE; }
    if (!state_operation(client,CONSOLE_IO_GET_DISPLAY_MODE,&request,&reply)) return FALSE;
    *mode=reply.mode;
    return TRUE;
}

BOOL WINAPI MvdmSetConsoleDisplayMode(HANDLE output,DWORD flags,PCOORD size)
{
    console_client *client=output_client(output);
    console_io_state request={0},reply={0};
    if (!client) return SetConsoleDisplayMode(output,flags,size);
    if (!size) { SetLastError(ERROR_INVALID_ACCESS);return FALSE; }
    request.mode=flags;
    if (!state_operation(client,CONSOLE_IO_SET_DISPLAY_MODE,&request,&reply)) return FALSE;
    if (reply.x<SHRT_MIN || reply.x>SHRT_MAX || reply.y<SHRT_MIN || reply.y>SHRT_MAX) {
        SetLastError(ERROR_INVALID_DATA);return FALSE;
    }
    size->X=(SHORT)reply.x;size->Y=(SHORT)reply.y;
    return TRUE;
}

/* DIVERGENCE(ADAPTER-WIN32-034): retain the historical BOOL/name contract
 * through the actual Console owner. No thread-HKL fallback or locale cache. */
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_reply reply;
    DWORD error;
    if (!name) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    if (!client) {
        typedef BOOL (WINAPI *query_layout)(LPSTR);
        query_layout query=(query_layout)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
            "GetConsoleKeyboardLayoutNameA");
        if (!query) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED);return FALSE; }
        return query(name);
    }
    EnterCriticalSection(&client->lock);
    ZeroMemory(&client->request,offsetof(console_io_request,data));
    client->request.operation=CONSOLE_IO_KEYBOARD_LAYOUT;
    error=exchange(client,&reply);
    if (!error && !reply.result) error=reply.error ? reply.error : ERROR_GEN_FAILURE;
    if (!error && (reply.bytes!=KL_NAMELENGTH ||
        reply.data[KL_NAMELENGTH-1]!=0)) error=ERROR_INVALID_DATA;
    if (!error) memcpy(name,reply.data,KL_NAMELENGTH);
    LeaveCriticalSection(&client->lock);
    SetLastError(error);
    return error==ERROR_SUCCESS;
}

DWORD WINAPI MvdmGetConsoleTitleA(LPSTR title,DWORD capacity)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_reply reply;
    DWORD error,result=0;
    if (!client) return GetConsoleTitleA(title,capacity);
    if ((!title && capacity) || capacity>CONSOLE_IO_DATA_BYTES) {
        SetLastError(ERROR_INVALID_PARAMETER);return 0;
    }
    EnterCriticalSection(&client->lock);
    ZeroMemory(&client->request,offsetof(console_io_request,data));
    client->request.operation=CONSOLE_IO_GET_TITLE_A;
    client->request.state.count=capacity;
    error=exchange(client,&reply);
    if (!error && reply.bytes>capacity) error=ERROR_INVALID_DATA;
    if (!error) {
        if (reply.bytes) memcpy(title,reply.data,reply.bytes);
        result=reply.state.count;error=reply.error;
    }
    LeaveCriticalSection(&client->lock);
    SetLastError(error);return error ? 0 : result;
}

BOOL WINAPI MvdmSetConsoleTitleA(LPCSTR title)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_reply reply;
    DWORD error;
    BOOL result=FALSE;
    size_t length;
    if (!client) return SetConsoleTitleA(title);
    if (!title) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    length=strnlen_s(title,CONSOLE_IO_DATA_BYTES);
    if (length==CONSOLE_IO_DATA_BYTES) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    EnterCriticalSection(&client->lock);
    ZeroMemory(&client->request,offsetof(console_io_request,data));
    client->request.operation=CONSOLE_IO_SET_TITLE_A;
    client->request.bytes=(DWORD)length+1;
    memcpy(client->request.data,title,length+1);
    error=exchange(client,&reply);
    if (!error) { result=reply.result!=0;error=reply.error; }
    if(!error && result) {
        char copied[CONSOLE_IO_TITLE_BYTES];
        memcpy(copied,title,min(length,sizeof(copied)-1));
        copied[min(length,sizeof(copied)-1)]=0;
        /* The original Console title call has already succeeded. Window
         * caption publication is supplemental and cannot change its result. */
        (void)ntcon_worker_publish_title(&client->channel,copied);
    }
    LeaveCriticalSection(&client->lock);
    SetLastError(error);return !error && result;
}

BOOL WINAPI MvdmGetCursorPos(LPPOINT point)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_state request={0},reply={0};
    if (!client) return GetCursorPos(point);
    if (!point) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    if (!state_operation(client,CONSOLE_IO_GET_POINTER,&request,&reply)) return FALSE;
    point->x=reply.x;point->y=reply.y;return TRUE;
}

BOOL WINAPI MvdmSetCursorPos(int x,int y)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_state request={0};
    if (!client) return SetCursorPos(x,y);
    request.x=x;request.y=y;
    return state_operation(client,CONSOLE_IO_SET_POINTER,&request,NULL);
}

BOOL WINAPI MvdmGetClipCursor(LPRECT rect)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_state request={0},reply={0};
    if (!client) return GetClipCursor(rect);
    if (!rect) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    if (!state_operation(client,CONSOLE_IO_GET_POINTER_CLIP,&request,&reply)) return FALSE;
    rect->left=reply.left;rect->top=reply.top;
    rect->right=reply.right;rect->bottom=reply.bottom;return TRUE;
}

BOOL WINAPI MvdmClipCursor(const RECT *rect)
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_state request={0};
    if (!client) return ClipCursor(rect);
    if (rect) {
        request.has_clip=1;request.left=rect->left;request.top=rect->top;
        request.right=rect->right;request.bottom=rect->bottom;
    }
    return state_operation(client,CONSOLE_IO_SET_POINTER_CLIP,&request,NULL);
}

int ntvdm_console_window_query(DWORD query,LONG values[4])
{
    session *owner=session_thread_current();
    console_client *client=owner ? owner->console_client : NULL;
    console_io_state request={0},reply={0};
    if (!client) return -1;
    request.mode=query;request.x=values[0];request.y=values[1];
    if (!state_operation(client,CONSOLE_IO_WINDOW_QUERY,&request,&reply)) return 0;
    values[0]=reply.left;values[1]=reply.top;
    values[2]=reply.right;values[3]=reply.bottom;
    return 1;
}

BOOL WINAPI MvdmWriteConsoleA(HANDLE output,const VOID *buffer,DWORD length,
    LPDWORD written,LPVOID reserved)
{
    console_client *client=output_client(output);
    console_io_reply reply;
    const BYTE *cursor=buffer;
    DWORD error=ERROR_SUCCESS,total=0,chunk;
    BOOL result=TRUE;
    /* WOW and non-Console streams retain their existing source route. DOS
     * with an admitted channel never falls back after transport failure. */
    if (!client)
        return WriteConsoleA(output,buffer,length,written,reserved);
    if (written) *written=0;
    if (reserved || (!buffer && length)) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    EnterCriticalSection(&client->lock);
    if (client->channel.failure) { error=client->channel.failure;goto done; }
    do {
        chunk=length-total;
        if (chunk>CONSOLE_IO_DATA_BYTES) chunk=CONSOLE_IO_DATA_BYTES;
        ZeroMemory(&client->request,offsetof(console_io_request,data));
        client->request.operation=CONSOLE_IO_WRITE;
        client->request.bytes=chunk;
        if (chunk) memcpy(client->request.data,cursor+total,chunk);
        error=exchange(client,&reply);
        if (!error && reply.state.count>chunk)
            error=ERROR_INVALID_DATA;
        if (error) { client->channel.failure=error;break; }
        total+=reply.state.count;
        if (!reply.result) { result=FALSE;error=reply.error;break; }
        if (reply.state.count!=chunk) break;
    } while (total<length);
done:
    LeaveCriticalSection(&client->lock);
    if (written) *written=total;
    if (error || !result) SetLastError(error);
    return error==ERROR_SUCCESS && result;
}

BOOL WINAPI MvdmGetConsoleScreenBufferInfo(HANDLE output,PCONSOLE_SCREEN_BUFFER_INFO info)
{
    console_client *client=output_client(output);
    console_io_state s={0};
    if (!client) return GetConsoleScreenBufferInfo(output,info);
    if (!info) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    if (!state_operation(client,CONSOLE_IO_SCREEN_INFO,NULL,&s)) return FALSE;
    info->dwSize.X=(SHORT)s.width;info->dwSize.Y=(SHORT)s.height;
    info->dwCursorPosition.X=(SHORT)s.x;info->dwCursorPosition.Y=(SHORT)s.y;
    info->wAttributes=(WORD)s.attribute;
    info->srWindow.Left=(SHORT)s.left;info->srWindow.Top=(SHORT)s.top;
    info->srWindow.Right=(SHORT)s.right;info->srWindow.Bottom=(SHORT)s.bottom;
    info->dwMaximumWindowSize.X=(SHORT)s.max_width;
    info->dwMaximumWindowSize.Y=(SHORT)s.max_height;
    return TRUE;
}

BOOL WINAPI MvdmSetConsoleCursorPosition(HANDLE output,COORD position)
{
    console_client *client=output_client(output);
    console_io_state s={0};
    if (!client) return SetConsoleCursorPosition(output,position);
    s.x=position.X;s.y=position.Y;
    return state_operation(client,CONSOLE_IO_CURSOR_POSITION,&s,NULL);
}

BOOL WINAPI MvdmSetConsoleCursorInfo(HANDLE output,const CONSOLE_CURSOR_INFO *info)
{
    console_client *client=output_client(output);
    console_io_state s={0};
    if (!client) return SetConsoleCursorInfo(output,info);
    if (!info) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    s.cursor_size=info->dwSize;s.cursor_visible=info->bVisible!=FALSE;
    return state_operation(client,CONSOLE_IO_CURSOR_INFO,&s,NULL);
}

BOOL WINAPI MvdmFillConsoleOutputCharacterA(HANDLE output,CHAR character,DWORD length,
    COORD position,LPDWORD written)
{
    console_client *client=output_client(output);
    console_io_state s={0},r={0};
    BOOL ok;
    if (!client) return FillConsoleOutputCharacterA(output,character,length,position,written);
    if (!written) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    s.x=position.X;s.y=position.Y;s.character=(BYTE)character;s.count=length;
    ok=state_operation(client,CONSOLE_IO_FILL_CHARACTER,&s,&r);
    *written=r.count;return ok;
}

BOOL WINAPI MvdmGetConsoleCursorInfo(HANDLE output,PCONSOLE_CURSOR_INFO info)
{
    console_client *client=output_client(output);
    console_io_state result={0};
    if (!client) return GetConsoleCursorInfo(output,info);
    if (!info) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    if (!state_operation(client,CONSOLE_IO_GET_CURSOR_INFO,NULL,&result)) return FALSE;
    info->dwSize=result.cursor_size;info->bVisible=result.cursor_visible;
    return TRUE;
}

BOOL WINAPI MvdmGetConsoleMode(HANDLE handle,LPDWORD mode)
{
    console_client *client=mode_client(handle);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    console_io_state request={0},result={0};
    if (!client) return GetConsoleMode(handle,mode);
    if (!mode) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    request.input=ntvdm_console_handle_kind(handle) ? ntvdm_console_handle_kind(handle)==1 : !GetConsoleScreenBufferInfo(handle,&screen);
    if (!state_operation(client,CONSOLE_IO_GET_MODE,&request,&result)) return FALSE;
    *mode=result.mode;return TRUE;
}

BOOL WINAPI MvdmSetConsoleMode(HANDLE handle,DWORD mode)
{
    console_client *client=mode_client(handle);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    console_io_state request={0};
    if (!client) return SetConsoleMode(handle,mode);
    request.input=ntvdm_console_handle_kind(handle) ? ntvdm_console_handle_kind(handle)==1 : !GetConsoleScreenBufferInfo(handle,&screen);request.mode=mode;
    return state_operation(client,CONSOLE_IO_SET_MODE,&request,NULL);
}

BOOL WINAPI MvdmFillConsoleOutputAttribute(HANDLE output,WORD attribute,DWORD length,
    COORD position,LPDWORD written)
{
    console_client *client=output_client(output);
    console_io_state s={0},r={0};
    BOOL ok;
    if (!client) return FillConsoleOutputAttribute(output,attribute,length,position,written);
    if (!written) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    s.x=position.X;s.y=position.Y;s.attribute=attribute;s.count=length;
    ok=state_operation(client,CONSOLE_IO_FILL_ATTRIBUTE,&s,&r);
    *written=r.count;return ok;
}

BOOL WINAPI MvdmScrollConsoleScreenBufferA(HANDLE output,const SMALL_RECT *rect,
    const SMALL_RECT *clip,COORD position,const CHAR_INFO *fill)
{
    console_client *client=output_client(output);
    console_io_state s={0};
    if (!client) return ScrollConsoleScreenBufferA(output,rect,clip,position,fill);
    if (!rect || !fill) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    s.left=rect->Left;s.top=rect->Top;s.right=rect->Right;s.bottom=rect->Bottom;
    if (clip) {
        s.has_clip=1;s.clip_left=clip->Left;s.clip_top=clip->Top;
        s.clip_right=clip->Right;s.clip_bottom=clip->Bottom;
    }
    s.x=position.X;s.y=position.Y;
    s.character=(BYTE)fill->Char.AsciiChar;s.attribute=fill->Attributes;
    return state_operation(client,CONSOLE_IO_SCROLL,&s,NULL);
}

BOOL WINAPI MvdmSetConsoleTextAttribute(HANDLE output,WORD attribute)
{
    console_client *client=output_client(output);
    console_io_state s={0};
    if (!client) return SetConsoleTextAttribute(output,attribute);
    s.attribute=attribute;
    return state_operation(client,CONSOLE_IO_ATTRIBUTE,&s,NULL);
}

BOOL WINAPI MvdmSetConsoleScreenBufferSize(HANDLE output,COORD size)
{
    console_client *client=output_client(output);
    console_io_state s={0};
    if (!client) return opennt_console_resize_grid(output,&size,FALSE,NULL);
    s.width=size.X;s.height=size.Y;
    return state_operation(client,CONSOLE_IO_BUFFER_SIZE,&s,NULL);
}

BOOL WINAPI MvdmSetConsoleWindowInfo(HANDLE output,BOOL absolute,const SMALL_RECT *window)
{
    console_client *client=output_client(output);
    console_io_state s={0};
    if (!client) return opennt_console_resize_grid(output,NULL,absolute,window);
    if (!window) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    s.mode=absolute!=FALSE;s.left=window->Left;s.top=window->Top;
    s.right=window->Right;s.bottom=window->Bottom;
    return state_operation(client,CONSOLE_IO_WINDOW_RECT,&s,NULL);
}

/* Original ntcon/client/iostubs.c clips to the caller's buffer and packs
 * CopyRectangle data before CSR. Here bounded row tiles replace the unavailable
 * capture transport; the frontend retains native screen clipping/code pages.
 * Whole 80x50 frames fit one tile; larger rectangles are not rejected. */
static BOOL cells_operation(console_client *client,BOOL write,BOOL wide,CHAR_INFO *buffer,
    COORD size,COORD origin,PSMALL_RECT region)
{
    console_io_reply reply;
    SMALL_RECT actual={0,0,-1,-1};
    int width,height,row,column,tile,rows,first=1,x,y,r,start_column,start_row;
    DWORD error=ERROR_SUCCESS,operation;
    BOOL ok=TRUE;
    if (!buffer || !region || size.X<=0 || size.Y<=0 || origin.X<0 || origin.Y<0) {
        SetLastError(ERROR_INVALID_PARAMETER);return FALSE;
    }
    width=min((int)region->Right-region->Left+1,(int)size.X-origin.X);
    height=min((int)region->Bottom-region->Top+1,(int)size.Y-origin.Y);
    if (width<=0 || height<=0) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    operation=write ? (wide ? CONSOLE_IO_WRITE_CELLS_W : CONSOLE_IO_WRITE_CELLS_A) :
        (wide ? CONSOLE_IO_READ_CELLS_W : CONSOLE_IO_READ_CELLS_A);
    EnterCriticalSection(&client->lock);
    /* Clip once before tiling. Modern Console can return the unchanged request
     * rectangle for a wholly off-screen tile; unioning it would invent cells
     * that the single original call never transferred. */
    ZeroMemory(&client->request,offsetof(console_io_request,data));
    client->request.operation=CONSOLE_IO_SCREEN_INFO;
    error=exchange(client,&reply);
    if (error || !reply.result) {
        if (!error) error=reply.error;
        ok=FALSE;goto done_cells;
    }
    start_column=max(0,-(int)region->Left);start_row=max(0,-(int)region->Top);
    width=min(width,reply.state.width-region->Left);
    height=min(height,reply.state.height-region->Top);
    if (width<=start_column || height<=start_row) {
        /* Native read and write differ for a wholly off-screen rectangle.
         * Ask the real provider; there is no cell to copy or synthesize. */
        ZeroMemory(&client->request,offsetof(console_io_request,data));
        client->request.operation=operation;
        client->request.state.width=client->request.state.height=1;
        client->request.state.left=region->Left;client->request.state.right=region->Right;
        client->request.state.top=region->Top;client->request.state.bottom=region->Bottom;
        if (write) {
            client->request.bytes=sizeof(CHAR_INFO);
            memcpy(client->request.data,buffer+origin.Y*(size_t)size.X+origin.X,sizeof(CHAR_INFO));
        }
        error=exchange(client,&reply);ok=!error && reply.result;
        if (!error) {
            region->Left=(SHORT)reply.state.left;region->Top=(SHORT)reply.state.top;
            region->Right=(SHORT)reply.state.right;region->Bottom=(SHORT)reply.state.bottom;
        }
        if (!error && !ok) error=reply.error;
        goto done_cells;
    }
    for (row=start_row;row<height && ok;row+=rows) {
        rows=width-start_column<=(int)(CONSOLE_IO_DATA_BYTES/sizeof(CHAR_INFO)) ?
            min(height-row,(int)(CONSOLE_IO_DATA_BYTES/sizeof(CHAR_INFO))/(width-start_column)) : 1;
        for (column=start_column;column<width;column+=tile) {
            console_io_state *s=&client->request.state;
            tile=min(width-column,(int)(CONSOLE_IO_DATA_BYTES/sizeof(CHAR_INFO)));
            ZeroMemory(&client->request,offsetof(console_io_request,data));
            client->request.operation=operation;
            s->width=tile;s->height=rows;
            s->left=region->Left+column;s->right=s->left+tile-1;
            s->top=region->Top+row;s->bottom=s->top+rows-1;
            if (write) {
                client->request.bytes=tile*rows*sizeof(CHAR_INFO);
                for (r=0;r<rows;r++) memcpy(client->request.data+r*tile*sizeof(CHAR_INFO),
                    buffer+(origin.Y+row+r)*(size_t)size.X+origin.X+column,tile*sizeof(CHAR_INFO));
            }
            error=exchange(client,&reply);
            if (error) { ok=FALSE;break; }
            if (!reply.result) { error=reply.error;ok=FALSE;break; }
            if ((!write && reply.bytes!=tile*rows*sizeof(CHAR_INFO)) ||
                (reply.state.right>=reply.state.left && reply.state.bottom>=reply.state.top &&
                 (reply.state.left<s->left || reply.state.right>s->right ||
                  reply.state.top<s->top || reply.state.bottom>s->bottom))) {
                error=client->channel.failure=ERROR_INVALID_DATA;ok=FALSE;break;
            }
            if (reply.state.right<reply.state.left || reply.state.bottom<reply.state.top) continue;
            if (first) {
                actual.Left=(SHORT)reply.state.left;actual.Right=(SHORT)reply.state.right;
                actual.Top=(SHORT)reply.state.top;actual.Bottom=(SHORT)reply.state.bottom;first=0;
            } else {
                actual.Left=(SHORT)min(actual.Left,reply.state.left);
                actual.Right=(SHORT)max(actual.Right,reply.state.right);
                actual.Top=(SHORT)min(actual.Top,reply.state.top);
                actual.Bottom=(SHORT)max(actual.Bottom,reply.state.bottom);
            }
            if (!write) {
                x=reply.state.left-s->left;
                for (r=reply.state.top-s->top;r<=reply.state.bottom-s->top;r++) {
                    y=origin.Y+row+r;
                    memcpy(buffer+y*(size_t)size.X+origin.X+column+x,
                        reply.data+(r*tile+x)*sizeof(CHAR_INFO),
                        (reply.state.right-reply.state.left+1)*sizeof(CHAR_INFO));
                }
            }
        }
    }
done_cells:
    LeaveCriticalSection(&client->lock);
    if (ok && !first) *region=actual;
    if (!ok) SetLastError(error);
    return ok;
}

BOOL WINAPI MvdmWriteConsoleOutputA(HANDLE output,const CHAR_INFO *buffer,COORD size,
    COORD origin,PSMALL_RECT region)
{
    console_client *client=output_client(output);
    if (!client) return WriteConsoleOutputA(output,buffer,size,origin,region);
    return cells_operation(client,TRUE,FALSE,(CHAR_INFO *)buffer,size,origin,region);
}
BOOL WINAPI MvdmWriteConsoleOutputW(HANDLE output,const CHAR_INFO *buffer,COORD size,
    COORD origin,PSMALL_RECT region)
{
    console_client *client=output_client(output);
    if (!client) return WriteConsoleOutputW(output,buffer,size,origin,region);
    return cells_operation(client,TRUE,TRUE,(CHAR_INFO *)buffer,size,origin,region);
}
BOOL WINAPI MvdmReadConsoleOutputA(HANDLE output,PCHAR_INFO buffer,COORD size,
    COORD origin,PSMALL_RECT region)
{
    console_client *client=output_client(output);
    if (!client) return ReadConsoleOutputA(output,buffer,size,origin,region);
    return cells_operation(client,FALSE,FALSE,buffer,size,origin,region);
}
BOOL WINAPI MvdmReadConsoleOutputW(HANDLE output,PCHAR_INFO buffer,COORD size,
    COORD origin,PSMALL_RECT region)
{
    console_client *client=output_client(output);
    if (!client) return ReadConsoleOutputW(output,buffer,size,origin,region);
    return cells_operation(client,FALSE,TRUE,buffer,size,origin,region);
}


static BOOL input_operation(HANDLE input,INPUT_RECORD *records,DWORD length,LPDWORD read,BOOL peek)
{
    console_client *client=input_client(input);
    console_io_reply reply;
    DWORD error,i;
    BOOL ok;
    if (!client) return peek ? PeekConsoleInputW(input,records,length,read) :
        ReadConsoleInputW(input,records,length,read);
    if (!read || (!records && length)) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    *read=0;
    EnterCriticalSection(&client->lock);
    ZeroMemory(&client->request,offsetof(console_io_request,data));
    client->request.operation=peek ? CONSOLE_IO_PEEK_INPUT : CONSOLE_IO_READ_INPUT;
    client->request.state.count=min(length,CONSOLE_IO_INPUT_CAPACITY);
    error=exchange(client,&reply);ok=!error && reply.result;
    if (!error && !ok) error=reply.error;
    if (ok && (reply.state.count>client->request.state.count ||
        reply.bytes!=reply.state.count*sizeof(console_io_input))) {
        error=client->channel.failure=ERROR_INVALID_DATA;ok=FALSE;
    }
    for (i=0;ok && i<reply.state.count;i++) {
        console_io_input wire;
        memcpy(&wire,reply.data+i*sizeof(wire),sizeof(wire));
        if (!decode_input(&wire,&records[i])) { error=client->channel.failure=ERROR_INVALID_DATA;ok=FALSE; }
    }
    if (ok) *read=reply.state.count;
    if (ok) {
        ResetEvent(client->wake);
        SetEvent(client->rearm);
    }
    LeaveCriticalSection(&client->lock);
    if (!ok) SetLastError(error);
    return ok;
}
BOOL WINAPI MvdmReadConsoleInputW(HANDLE input,PINPUT_RECORD records,DWORD length,LPDWORD read)
{
    return input_operation(input,records,length,read,FALSE);
}
BOOL WINAPI MvdmPeekConsoleInputW(HANDLE input,PINPUT_RECORD records,DWORD length,LPDWORD read)
{
    return input_operation(input,records,length,read,TRUE);
}

BOOL ntvdm_console_prepend_keys(HANDLE input,PINPUT_RECORD records,DWORD length,LPDWORD written)
{
    console_client *client=input_client(input);
    console_io_reply reply;
    DWORD error=ERROR_SUCCESS;
    BOOL ok=FALSE;
    if (!client) {
        typedef BOOL (WINAPI *write_vdm_input)(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
        write_vdm_input prepend=(write_vdm_input)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
            "WriteConsoleInputVDMW");
        if (!prepend) { if (written) *written=0;SetLastError(ERROR_CALL_NOT_IMPLEMENTED);return FALSE; }
        return prepend(input,records,length,written);
    }
    if (written) *written=0;
    /* Original ReturnBiosBufferKeys uses 32 records; ReturnUnusedKeyEvents
     * uses MAX_KEY_EVENTS=100. One atomic batch fits the existing wire bound. */
    if (!written || (!records && length) || length>CONSOLE_IO_INPUT_CAPACITY) {
        SetLastError(ERROR_INVALID_PARAMETER);return FALSE;
    }
    EnterCriticalSection(&client->lock);
    error=ntcon_worker_prepend_keys(&client->channel,records,length,&reply);
    if (!error) {
        *written=reply.state.count;ok=reply.result;if (!ok) error=reply.error;
    }
    LeaveCriticalSection(&client->lock);
    if (error) SetLastError(error);
    return ok;
}
