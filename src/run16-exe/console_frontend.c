/* Presentation only: original SoftPC produces the operations; public Console
 * owns host cells/scrollback. No guest state, command selection or scheduler. */
#include "console_frontend.h"
#include "opennt-abi/host-compat/include/console_grid.h"
#include <limits.h>
#include <stddef.h>
#include <string.h>
typedef char console_cell_layout_check[(sizeof(CHAR_INFO)==sizeof(console_io_cell) &&
    offsetof(CHAR_INFO,Attributes)==offsetof(console_io_cell,attribute)) ? 1 : -1];

static BOOL coordinate(int32_t value)
{
    return value>=SHRT_MIN && value<=SHRT_MAX;
}

static BOOL encode_input(const INPUT_RECORD *record,console_io_input *wire)
{
    ZeroMemory(wire,sizeof(*wire));wire->type=record->EventType;
    switch (record->EventType) {
    case KEY_EVENT:
        wire->key_down=record->Event.KeyEvent.bKeyDown!=FALSE;
        wire->repeat=record->Event.KeyEvent.wRepeatCount;
        wire->virtual_key=record->Event.KeyEvent.wVirtualKeyCode;
        wire->scan=record->Event.KeyEvent.wVirtualScanCode;
        wire->character=record->Event.KeyEvent.uChar.UnicodeChar;
        wire->control=record->Event.KeyEvent.dwControlKeyState;break;
    case MOUSE_EVENT:
        wire->x=record->Event.MouseEvent.dwMousePosition.X;
        wire->y=record->Event.MouseEvent.dwMousePosition.Y;
        wire->buttons=record->Event.MouseEvent.dwButtonState;
        wire->flags=record->Event.MouseEvent.dwEventFlags;
        wire->control=record->Event.MouseEvent.dwControlKeyState;break;
    case WINDOW_BUFFER_SIZE_EVENT:
        wire->x=record->Event.WindowBufferSizeEvent.dwSize.X;
        wire->y=record->Event.WindowBufferSizeEvent.dwSize.Y;break;
    case MENU_EVENT: wire->menu=record->Event.MenuEvent.dwCommandId;break;
    case FOCUS_EVENT: wire->focus=record->Event.FocusEvent.bSetFocus!=FALSE;break;
    default:return FALSE;
    }
    return TRUE;
}

DWORD run16_console_dispatch(run16_console_frontend *owner,const console_io_request *request,
    console_io_reply *reply)
{
    const console_io_state *s;
    COORD position;
    DWORD count=0,mode=0;
    BOOL ok=FALSE;
    BOOL cells,write_cells;
    if (!owner || !request || !reply) return ERROR_INVALID_PARAMETER;
    ZeroMemory(reply,sizeof(*reply));
    if (request->version!=CONSOLE_IO_VERSION) return ERROR_REVISION_MISMATCH;
    if (!owner->generation || request->generation!=owner->generation) return ERROR_ACCESS_DENIED;
    if (!request->sequence || owner->sequence==UINT32_MAX ||
        request->sequence!=owner->sequence+1 || request->bytes>CONSOLE_IO_DATA_BYTES ||
        request->operation<CONSOLE_IO_WRITE || request->operation>CONSOLE_IO_SET_DISPLAY_MODE)
        return ERROR_INVALID_DATA;
    cells=request->operation>=CONSOLE_IO_READ_CELLS_A && request->operation<=CONSOLE_IO_WRITE_CELLS_W;
    write_cells=request->operation>=CONSOLE_IO_WRITE_CELLS_A && request->operation<=CONSOLE_IO_WRITE_CELLS_W;
    if (request->operation!=CONSOLE_IO_WRITE && !write_cells &&
        request->operation!=CONSOLE_IO_PREPEND_KEYS &&
        request->operation!=CONSOLE_IO_SET_TITLE_A &&
        request->operation!=CONSOLE_IO_VIDEO_BEGIN &&
        request->operation!=CONSOLE_IO_VIDEO_DATA && request->bytes)
        return ERROR_INVALID_DATA;
    s=&request->state;
    if (request->operation==CONSOLE_IO_VIDEO_BEGIN &&
        request->bytes!=sizeof(console_video_description)) return ERROR_INVALID_DATA;
    if (request->operation==CONSOLE_IO_VIDEO_DATA && !request->bytes) return ERROR_INVALID_DATA;
    if (request->operation==CONSOLE_IO_GET_TITLE_A && s->count>CONSOLE_IO_DATA_BYTES)
        return ERROR_INVALID_DATA;
    if (request->operation==CONSOLE_IO_SET_TITLE_A && (!request->bytes ||
        memchr(request->data,0,request->bytes)!=request->data+request->bytes-1))
        return ERROR_INVALID_DATA;
    if (request->operation==CONSOLE_IO_PREPEND_KEYS &&
        (s->count>CONSOLE_IO_INPUT_CAPACITY || request->bytes!=s->count*sizeof(console_io_input)))
        return ERROR_INVALID_DATA;
    if ((request->operation==CONSOLE_IO_READ_INPUT || request->operation==CONSOLE_IO_PEEK_INPUT) &&
        s->count>CONSOLE_IO_INPUT_CAPACITY)
        return ERROR_INVALID_DATA;
    if (request->operation==CONSOLE_IO_WINDOW_RECT && s->mode>1) return ERROR_INVALID_DATA;
    if (request->operation==CONSOLE_IO_CURRENT_FONT && s->mode>1) return ERROR_INVALID_DATA;
    if (cells && (s->width<=0 || s->height<=0 || s->width>SHRT_MAX || s->height>SHRT_MAX ||
        (uint64_t)s->width*s->height>CONSOLE_IO_DATA_BYTES/sizeof(CHAR_INFO) ||
        (write_cells && request->bytes!=(uint32_t)(s->width*s->height*sizeof(CHAR_INFO)))))
        return ERROR_INVALID_DATA;
    if ((request->operation!=CONSOLE_IO_WINDOW_QUERY &&
        request->operation<CONSOLE_IO_GET_POINTER &&
        (!coordinate(s->width) || !coordinate(s->height) ||
        !coordinate(s->x) || !coordinate(s->y) || !coordinate(s->left) ||
        !coordinate(s->top) || !coordinate(s->right) || !coordinate(s->bottom) ||
        !coordinate(s->clip_left) || !coordinate(s->clip_top) ||
        !coordinate(s->clip_right) || !coordinate(s->clip_bottom))) ||
        s->attribute>UINT16_MAX || s->character>UINT8_MAX ||
        s->input>1 || s->has_clip>1 || s->cursor_visible>1) return ERROR_INVALID_DATA;
    owner->sequence=request->sequence;
    reply->version=CONSOLE_IO_VERSION;
    reply->generation=owner->generation;
    reply->sequence=request->sequence;
    position.X=(SHORT)s->x; position.Y=(SHORT)s->y;
    SetLastError(ERROR_SUCCESS);
    switch (request->operation) {
    case CONSOLE_IO_VIDEO_BEGIN: {
        console_video_description description;
        memcpy(&description,request->data,sizeof(description));
        mode=run16_console_video_begin(&owner->video,s->mode,&description);
        ok=mode==ERROR_SUCCESS;SetLastError(mode);break;
    }
    case CONSOLE_IO_VIDEO_DATA:
        mode=run16_console_video_data(&owner->video,s->mode,s->count,request->data,request->bytes);
        ok=mode==ERROR_SUCCESS;SetLastError(mode);break;
    case CONSOLE_IO_VIDEO_TEXT:
        mode=run16_console_video_text(&owner->video,s->mode);
        /* Text-mode publication is transported without a worker-side call
         * to activate the user's screen buffer. Headless protocol fixtures
         * may intentionally omit a Console handle. */
        ok=mode==ERROR_SUCCESS;SetLastError(mode);break;
    case CONSOLE_IO_GET_DISPLAY_MODE:
        ok=GetConsoleDisplayMode(&reply->state.mode);break;
    case CONSOLE_IO_SET_DISPLAY_MODE: {
        COORD size;
        ok=SetConsoleDisplayMode(owner->output,s->mode,&size);
        if (ok) { reply->state.x=size.X;reply->state.y=size.Y; }
        break;
    }
    case CONSOLE_IO_CURRENT_FONT: {
        CONSOLE_FONT_INFO font;
        ok=GetCurrentConsoleFont(owner->output,s->mode,&font);
        if (ok) {
            reply->state.count=font.nFont;
            reply->state.x=font.dwFontSize.X;reply->state.y=font.dwFontSize.Y;
        }
        break;
    }
    case CONSOLE_IO_FONT_SIZE: {
        COORD size=GetConsoleFontSize(owner->output,s->count);
        ok=size.X!=0 || size.Y!=0;
        if (ok) { reply->state.x=size.X;reply->state.y=size.Y; }
        break;
    }
    case CONSOLE_IO_GET_POINTER: {
        POINT point;
        ok=GetCursorPos(&point);
        if (ok) { reply->state.x=point.x;reply->state.y=point.y; }
        break;
    }
    case CONSOLE_IO_SET_POINTER:
        ok=SetCursorPos(s->x,s->y);break;
    case CONSOLE_IO_GET_POINTER_CLIP: {
        RECT rect;
        ok=GetClipCursor(&rect);
        if (ok) {
            reply->state.left=rect.left;reply->state.top=rect.top;
            reply->state.right=rect.right;reply->state.bottom=rect.bottom;
        }
        break;
    }
    case CONSOLE_IO_SET_POINTER_CLIP: {
        RECT rect={s->left,s->top,s->right,s->bottom};
        ok=ClipCursor(s->has_clip ? &rect : NULL);break;
    }
    case CONSOLE_IO_GET_TITLE_A:
        /* Empty titles can leave the caller's buffer untouched. Return only
         * bytes actually supplied by the native API, not a fabricated NUL. */
        memset(reply->data,0x55,s->count);
        count=GetConsoleTitleA((LPSTR)reply->data,s->count);
        mode=GetLastError();
        reply->state.count=count;
        if (s->count && (count || reply->data[0]==0))
            reply->bytes=count<s->count ? count+1 : s->count;
        ok=count!=0 || mode==ERROR_SUCCESS;
        SetLastError(mode);
        break;
    case CONSOLE_IO_SET_TITLE_A:
        ok=SetConsoleTitleA((LPCSTR)request->data);
        break;
    case CONSOLE_IO_WINDOW_QUERY: {
        HWND window=GetConsoleWindow();
        if (!window) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED);break; }
        switch (s->mode) {
        case CONSOLE_WINDOW_ICONIC:
            reply->state.left=IsIconic(window);ok=TRUE;break;
        case CONSOLE_WINDOW_CLIENT_RECT: {
            RECT rect;
            ok=GetClientRect(window,&rect);
            if (ok) {
                reply->state.left=rect.left;reply->state.top=rect.top;
                reply->state.right=rect.right;reply->state.bottom=rect.bottom;
            }
            break;
        }
        case CONSOLE_WINDOW_CLIENT_TO_SCREEN: {
            POINT point={s->x,s->y};
            ok=ClientToScreen(window,&point);
            if (ok) { reply->state.left=point.x;reply->state.top=point.y; }
            break;
        }
        default: SetLastError(ERROR_CALL_NOT_IMPLEMENTED);break;
        }
        break;
    }
    case CONSOLE_IO_CODE_PAGE:
        reply->state.count=s->input ? GetConsoleCP() : GetConsoleOutputCP();
        ok=reply->state.count!=0;
        break;
    case CONSOLE_IO_WRITE:
        ok=WriteConsoleA(owner->output,request->data,request->bytes,&count,NULL);
        reply->state.count=count;
        break;
    case CONSOLE_IO_SCREEN_INFO: {
        CONSOLE_SCREEN_BUFFER_INFO info;
        ok=GetConsoleScreenBufferInfo(owner->output,&info);
        if (ok) {
            reply->state.width=info.dwSize.X; reply->state.height=info.dwSize.Y;
            reply->state.x=info.dwCursorPosition.X; reply->state.y=info.dwCursorPosition.Y;
            reply->state.left=info.srWindow.Left; reply->state.top=info.srWindow.Top;
            reply->state.right=info.srWindow.Right; reply->state.bottom=info.srWindow.Bottom;
            reply->state.max_width=info.dwMaximumWindowSize.X;
            reply->state.max_height=info.dwMaximumWindowSize.Y;
            reply->state.attribute=info.wAttributes;
        }
        break;
    }
    case CONSOLE_IO_CURSOR_POSITION:
        ok=SetConsoleCursorPosition(owner->output,position);
        break;
    case CONSOLE_IO_CURSOR_INFO: {
        CONSOLE_CURSOR_INFO cursor;
        cursor.dwSize=s->cursor_size; cursor.bVisible=s->cursor_visible;
        ok=SetConsoleCursorInfo(owner->output,&cursor);
        break;
    }
    case CONSOLE_IO_GET_CURSOR_INFO: {
        CONSOLE_CURSOR_INFO cursor;
        ok=GetConsoleCursorInfo(owner->output,&cursor);
        if (ok) {
            reply->state.cursor_size=cursor.dwSize;
            reply->state.cursor_visible=cursor.bVisible!=FALSE;
        }
        break;
    }
    case CONSOLE_IO_FILL_CHARACTER:
        ok=FillConsoleOutputCharacterA(owner->output,(CHAR)s->character,s->count,position,&count);
        reply->state.count=count;
        break;
    case CONSOLE_IO_FILL_ATTRIBUTE:
        ok=FillConsoleOutputAttribute(owner->output,(WORD)s->attribute,s->count,position,&count);
        reply->state.count=count;
        break;
    case CONSOLE_IO_SCROLL: {
        SMALL_RECT rect={(SHORT)s->left,(SHORT)s->top,(SHORT)s->right,(SHORT)s->bottom};
        SMALL_RECT clip={(SHORT)s->clip_left,(SHORT)s->clip_top,(SHORT)s->clip_right,(SHORT)s->clip_bottom};
        CHAR_INFO fill;
        ZeroMemory(&fill,sizeof(fill));
        fill.Char.AsciiChar=(CHAR)s->character; fill.Attributes=(WORD)s->attribute;
        ok=ScrollConsoleScreenBufferA(owner->output,&rect,s->has_clip ? &clip : NULL,position,&fill);
        break;
    }
    case CONSOLE_IO_ATTRIBUTE:
        ok=SetConsoleTextAttribute(owner->output,(WORD)s->attribute);
        break;
    case CONSOLE_IO_GET_MODE:
        ok=GetConsoleMode(s->input ? owner->input : owner->output,&mode);
        reply->state.mode=mode;
        break;
    case CONSOLE_IO_SET_MODE:
        ok=SetConsoleMode(s->input ? owner->input : owner->output,s->mode);
        break;
    case CONSOLE_IO_BARRIER:
        ok=TRUE;
        break;
    case CONSOLE_IO_PREPEND_KEYS: {
        typedef BOOL (WINAPI *write_vdm_input)(HANDLE,PINPUT_RECORD,DWORD,LPDWORD);
        write_vdm_input prepend=(write_vdm_input)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
            "WriteConsoleInputVDMW");
        INPUT_RECORD records[CONSOLE_IO_INPUT_CAPACITY];
        DWORD i;
        if (!prepend) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED);break; }
        ZeroMemory(records,sizeof(records));
        for (i=0;i<s->count;i++) {
            console_io_input wire;
            memcpy(&wire,request->data+i*sizeof(wire),sizeof(wire));
            if (wire.type!=KEY_EVENT || wire.key_down>1 || wire.repeat>UINT16_MAX ||
                wire.virtual_key>UINT16_MAX || wire.scan>UINT16_MAX || wire.character>UINT16_MAX) {
                SetLastError(ERROR_INVALID_DATA);break;
            }
            records[i].EventType=KEY_EVENT;
            records[i].Event.KeyEvent.bKeyDown=wire.key_down;
            records[i].Event.KeyEvent.wRepeatCount=(WORD)wire.repeat;
            records[i].Event.KeyEvent.wVirtualKeyCode=(WORD)wire.virtual_key;
            records[i].Event.KeyEvent.wVirtualScanCode=(WORD)wire.scan;
            records[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)wire.character;
            records[i].Event.KeyEvent.dwControlKeyState=wire.control;
        }
        /* Validate the whole original return batch before mutating CONIN$.
         * The native Console owner performs the atomic front insertion. */
        if (i==s->count) ok=prepend(owner->input,records,s->count,&count);
        reply->state.count=count;
        break;
    }
    case CONSOLE_IO_READ_INPUT:
    case CONSOLE_IO_PEEK_INPUT: {
        INPUT_RECORD records[CONSOLE_IO_INPUT_CAPACITY];
        DWORD i;
        ok=request->operation==CONSOLE_IO_PEEK_INPUT ?
            PeekConsoleInputW(owner->input,records,s->count,&count) :
            ReadConsoleInputW(owner->input,records,s->count,&count);
        if (ok) {
            for (i=0;i<count;i++) {
                  console_io_input wire;
                  if (!encode_input(&records[i],&wire)) { ok=FALSE;SetLastError(ERROR_INVALID_DATA);break; }
                memcpy(reply->data+i*sizeof(wire),&wire,sizeof(wire));
            }
            if (ok) { reply->state.count=count;reply->bytes=count*sizeof(console_io_input); }
        }
        break;
    }
    case CONSOLE_IO_BUFFER_SIZE: {
        COORD size={(SHORT)s->width,(SHORT)s->height};
        ok=opennt_console_resize_grid(owner->output,&size,FALSE,NULL);
        break;
    }
    case CONSOLE_IO_WINDOW_RECT: {
        SMALL_RECT rect={(SHORT)s->left,(SHORT)s->top,(SHORT)s->right,(SHORT)s->bottom};
        ok=opennt_console_resize_grid(owner->output,NULL,s->mode!=0,&rect);
        break;
    }
    case CONSOLE_IO_READ_CELLS_A:
    case CONSOLE_IO_READ_CELLS_W:
    case CONSOLE_IO_WRITE_CELLS_A:
    case CONSOLE_IO_WRITE_CELLS_W: {
        COORD size={(SHORT)s->width,(SHORT)s->height},origin={0,0};
        SMALL_RECT rect={(SHORT)s->left,(SHORT)s->top,(SHORT)s->right,(SHORT)s->bottom};
        CHAR_INFO *buffer=(CHAR_INFO *)reply->data;
        /* CHAR_INFO is two 16-bit scalar fields on the selected Windows ABI.
         * Copy, never interpret a native address received from the peer. */
        if (write_cells) memcpy(buffer,request->data,request->bytes);
        if (request->operation==CONSOLE_IO_WRITE_CELLS_A)
            ok=WriteConsoleOutputA(owner->output,buffer,size,origin,&rect);
        else if (request->operation==CONSOLE_IO_WRITE_CELLS_W)
            ok=WriteConsoleOutputW(owner->output,buffer,size,origin,&rect);
        else if (request->operation==CONSOLE_IO_READ_CELLS_A)
            ok=ReadConsoleOutputA(owner->output,buffer,size,origin,&rect);
        else ok=ReadConsoleOutputW(owner->output,buffer,size,origin,&rect);
        reply->state.left=rect.Left;reply->state.top=rect.Top;
        reply->state.right=rect.Right;reply->state.bottom=rect.Bottom;
        if (ok && !write_cells) reply->bytes=(uint32_t)(s->width*s->height*sizeof(CHAR_INFO));
        break;
    }
    }
    reply->result=ok!=FALSE;
    reply->error=ok ? ERROR_SUCCESS : GetLastError();
    return ERROR_SUCCESS;
}
