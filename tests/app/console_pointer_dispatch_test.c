/* Test-only USER boundary substitution: never clips or moves a real pointer.
 * Compile the production dispatcher, not a duplicate protocol implementation. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
static POINT pointer_position;
static RECT border;
static BOOL release_clip;
static DWORD calls,failure;
static DWORD layout_calls,layout_error;
static BOOL WINAPI test_layout(LPSTR name)
{
    ++layout_calls;
    if (layout_error) { SetLastError(layout_error);return FALSE; }
    memcpy(name,"00000409",KL_NAMELENGTH);return TRUE;
}
static FARPROC WINAPI test_proc(HMODULE module,LPCSTR name)
{
    if (!strcmp(name,"GetConsoleKeyboardLayoutNameA")) return (FARPROC)test_layout;
    return GetProcAddress(module,name);
}
static BOOL WINAPI test_get_pointer(LPPOINT point)
{ ++calls;if (failure) { SetLastError(failure);return FALSE; } *point=pointer_position;return TRUE; }
static BOOL WINAPI test_set_pointer(int x,int y)
{ ++calls;if (failure) { SetLastError(failure);return FALSE; } pointer_position.x=x;pointer_position.y=y;return TRUE; }
static BOOL WINAPI test_get_clip(LPRECT rect)
{ ++calls;if (failure) { SetLastError(failure);return FALSE; } *rect=border;return TRUE; }
static BOOL WINAPI test_set_clip(const RECT *rect)
{
    ++calls;if (failure) { SetLastError(failure);return FALSE; }
    release_clip=rect==NULL;if (rect) border=*rect;return TRUE;
}
#define GetCursorPos test_get_pointer
#define SetCursorPos test_set_pointer
#define GetClipCursor test_get_clip
#define ClipCursor test_set_clip
#define GetProcAddress test_proc
#include "../../src/run16-exe/console_frontend.c"
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d\n",__LINE__);return 1; } } while (0)
int main(void)
{
    run16_console_frontend owner={0};
    console_io_request request={0};
    console_io_reply reply;
    DWORD operation;
    owner.generation=17;request.version=CONSOLE_IO_VERSION;request.generation=17;
    request.sequence=1;request.operation=CONSOLE_IO_SET_POINTER;
    request.state.x=40000;request.state.y=-40000;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && calls==1);
    CHECK(pointer_position.x==40000 && pointer_position.y==-40000);
    ++request.sequence;request.operation=CONSOLE_IO_GET_POINTER;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && calls==2);
    CHECK(reply.state.x==40000 && reply.state.y==-40000);
    ++request.sequence;request.operation=CONSOLE_IO_SET_POINTER_CLIP;
    request.state.has_clip=1;request.state.left=-50000;request.state.top=-40000;
    request.state.right=50000;request.state.bottom=40000;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && !release_clip);
    ++request.sequence;request.operation=CONSOLE_IO_GET_POINTER_CLIP;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result);
    CHECK(reply.state.left==-50000 && reply.state.top==-40000 &&
        reply.state.right==50000 && reply.state.bottom==40000);
    ++request.sequence;request.operation=CONSOLE_IO_SET_POINTER_CLIP;request.state.has_clip=0;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result && release_clip);
    failure=ERROR_ACCESS_DENIED;
    for (operation=CONSOLE_IO_GET_POINTER;operation<=CONSOLE_IO_SET_POINTER_CLIP;++operation) {
        DWORD before=calls;
        ++request.sequence;request.operation=operation;
        CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result &&
            reply.error==failure && calls==before+1);
        CHECK(!reply.state.x && !reply.state.left && !reply.bytes);
    }
    ++request.sequence;request.operation=CONSOLE_IO_KEYBOARD_LAYOUT;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && reply.result &&
        reply.bytes==KL_NAMELENGTH && !memcmp(reply.data,"00000409",KL_NAMELENGTH) && layout_calls==1);
    ++request.sequence;layout_error=ERROR_INVALID_HANDLE;
    CHECK(!run16_console_dispatch(&owner,&request,&reply) && !reply.result &&
        reply.error==layout_error && !reply.bytes && layout_calls==2);
    puts("PASS: production pointer/layout dispatch, signed LONG, null release, native errors (mock host only)");
    return 0;
}
