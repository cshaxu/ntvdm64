#include "include/console_services.h"
#include "include/console_endpoint.h"
#include "include/host_context.h"
#include <string.h>

static compat_console_endpoint *current_endpoint(void)
{
    compat_host_context *context = compat_host_context_current();
    return context ? context->console_endpoint : NULL;
}

BOOL compat_console_state_operation(compat_console_endpoint *client,DWORD operation,
    const console_io_state *state,console_io_state *result)
{
    console_io_reply reply;
    DWORD error;
    BOOL ok;
    EnterCriticalSection(&client->lock);
    ZeroMemory(&client->request,offsetof(console_io_request,data));
    client->request.operation=operation;
    if (state) client->request.state=*state;
    error=ntcon_worker_exchange(&client->channel,&client->request,&reply);
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
    compat_console_endpoint *client=current_endpoint();
    console_io_state request={0},reply={0};
    if (!client) return input ? GetConsoleCP() : GetConsoleOutputCP();
    request.input=input;
    return compat_console_state_operation(client,CONSOLE_IO_CODE_PAGE,&request,&reply) ? reply.count : 0;
}

UINT WINAPI MvdmGetConsoleCP(void) { return console_code_page(TRUE); }
UINT WINAPI MvdmGetConsoleOutputCP(void) { return console_code_page(FALSE); }

/* DIVERGENCE(ADAPTER-WIN32-034): retain the historical BOOL/name contract
 * through the actual Console owner. No thread-HKL fallback or locale cache. */
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name)
{
    compat_console_endpoint *client=current_endpoint();
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
    error=ntcon_worker_exchange(&client->channel,&client->request,&reply);
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
    compat_console_endpoint *client=current_endpoint();
    console_io_reply reply;
    DWORD error,result=0;
    if (!client) return GetConsoleTitleA(title,capacity);
    if ((!title && capacity) || capacity>CONSOLE_IO_TILE_BYTES) {
        SetLastError(ERROR_INVALID_PARAMETER);return 0;
    }
    EnterCriticalSection(&client->lock);
    ZeroMemory(&client->request,offsetof(console_io_request,data));
    client->request.operation=CONSOLE_IO_GET_TITLE_A;
    client->request.state.count=capacity;
    error=ntcon_worker_exchange(&client->channel,&client->request,&reply);
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
    compat_console_endpoint *client=current_endpoint();
    console_io_reply reply;
    DWORD error;
    BOOL result=FALSE;
    size_t length;
    if (!client) return SetConsoleTitleA(title);
    if (!title) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    length=strnlen_s(title,CONSOLE_IO_TILE_BYTES);
    if (length==CONSOLE_IO_TILE_BYTES) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    EnterCriticalSection(&client->lock);
    ZeroMemory(&client->request,offsetof(console_io_request,data));
    client->request.operation=CONSOLE_IO_SET_TITLE_A;
    client->request.bytes=(DWORD)length+1;
    client->request.data=(BYTE *)title;
    error=ntcon_worker_exchange(&client->channel,&client->request,&reply);
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
    compat_console_endpoint *client=current_endpoint();
    console_io_state request={0},reply={0};
    if (!client) return GetCursorPos(point);
    if (!point) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    if (!compat_console_state_operation(client,CONSOLE_IO_GET_POINTER,&request,&reply)) return FALSE;
    point->x=reply.x;point->y=reply.y;return TRUE;
}

BOOL WINAPI MvdmSetCursorPos(int x,int y)
{
    compat_console_endpoint *client=current_endpoint();
    console_io_state request={0};
    if (!client) return SetCursorPos(x,y);
    request.x=x;request.y=y;
    return compat_console_state_operation(client,CONSOLE_IO_SET_POINTER,&request,NULL);
}

BOOL WINAPI MvdmGetClipCursor(LPRECT rect)
{
    compat_console_endpoint *client=current_endpoint();
    console_io_state request={0},reply={0};
    if (!client) return GetClipCursor(rect);
    if (!rect) { SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    if (!compat_console_state_operation(client,CONSOLE_IO_GET_POINTER_CLIP,&request,&reply)) return FALSE;
    rect->left=reply.left;rect->top=reply.top;
    rect->right=reply.right;rect->bottom=reply.bottom;return TRUE;
}

BOOL WINAPI MvdmClipCursor(const RECT *rect)
{
    compat_console_endpoint *client=current_endpoint();
    console_io_state request={0};
    if (!client) return ClipCursor(rect);
    if (rect) {
        request.has_clip=1;request.left=rect->left;request.top=rect->top;
        request.right=rect->right;request.bottom=rect->bottom;
    }
    return compat_console_state_operation(client,CONSOLE_IO_SET_POINTER_CLIP,&request,NULL);
}
