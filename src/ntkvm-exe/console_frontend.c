/* Presentation only: original SoftPC produces the operations; public Console
 * owns host cells/scrollback. No guest state, command selection or scheduler. */
#include "console_frontend.h"
#include "interface/console_io.h"
#include <limits.h>
#include <string.h>
static BOOL encode_input(const INPUT_RECORD *record,console_io_input *wire)
{
    ZeroMemory(wire,sizeof(*wire));wire->type=record->EventType;
    switch (record->EventType) {
    case CONSOLE_INPUT_POINTER: {
        console_pointer_input mouse;
        memcpy(&mouse,&record->Event,sizeof(mouse));
        wire->x=mouse.dx;wire->y=mouse.dy;wire->buttons=mouse.buttons;
        wire->flags=mouse.action;wire->control=mouse.control;
        return mouse.buttons<=3 && mouse.action>=CONSOLE_MOUSE_ENTER &&
            mouse.action<=CONSOLE_MOUSE_POSITION;
    }
    case CONSOLE_INPUT_RELATIVE_MOUSE: {
        console_mouse_input mouse;
        memcpy(&mouse,&record->Event,sizeof(mouse));
        if(!console_mouse_input_valid(&mouse))return FALSE;
        wire->x=mouse.dx;wire->y=mouse.dy;wire->buttons=mouse.buttons;
        wire->flags=mouse.action;wire->control=mouse.width|((uint32_t)mouse.height<<16);
        break;
    }
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

BOOL run16_console_dos_size(COORD size)
{
    /* Original calcScreenParams/DoFullScreenResume, not VGA's full mode set. */
    return size.X==80 && (size.Y==22 || size.Y==25 || size.Y==28 || size.Y==43 || size.Y==50);
}

/* Match OpenNT nt_fulsc.c::calcScreenParams and its integer MID_VAL macro.
 * The active Console viewport, not the previous DOS mode, selects the rows. */
static SHORT opennt_dos_return_height(SHORT height)
{
    if(height<=22+(25-22)/2)return 22;
    if(height<=25+(28-25)/2)return 25;
    if(height<=28+(43-28)/2)return 28;
    if(height<=43+(50-43)/2)return 43;
    return 50;
}

DWORD run16_console_prepare_dos(HANDLE output,SMALL_RECT *window)
{
    CONSOLE_SCREEN_BUFFER_INFO before,after;
    SMALL_RECT physical={0,0,0,0};
    COORD size,cursor;
    if(!window)return ERROR_INVALID_PARAMETER;
    size.X=80;
    size.Y=opennt_dos_return_height(window->Bottom-window->Top+1);
    if(!GetConsoleScreenBufferInfo(output,&before))return GetLastError();
    if(before.dwSize.X==size.X && before.dwSize.Y==size.Y &&
        !window->Left && !window->Top && window->Right==size.X-1 && window->Bottom==size.Y-1)
        return ERROR_SUCCESS;
    /* ConPTY treats a one-cell viewport as a one-cell backing page. Reduce
     * only to the target's representable viewport before resizing storage;
     * otherwise the ordinary 30->28 DOS handoff destroys the shared grid. */
    physical.Right=min(size.X,min(before.dwSize.X,before.dwMaximumWindowSize.X))-1;
    physical.Bottom=min(size.Y,min(before.dwSize.Y,before.dwMaximumWindowSize.Y))-1;
    /* Reuse OpenNT ResizeScreenBuffer's no-reflow row retention. Original
     * DoFullScreenResume subsequently reads from origin and changes real VGA
     * state; never acknowledge a frame-header-only DOS mode conversion. */
    if(!opennt_console_resize_grid(output,NULL,TRUE,&physical) ||
        !opennt_console_resize_grid(output,&size,FALSE,NULL) ||
        !GetConsoleScreenBufferInfo(output,&after))return GetLastError();
    if(after.dwSize.X!=size.X || after.dwSize.Y!=size.Y)return ERROR_RETRY;
    cursor.X=min(before.dwCursorPosition.X,size.X-1);
    cursor.Y=min(before.dwCursorPosition.Y,size.Y-1);
    physical.Right=min(size.X,after.dwMaximumWindowSize.X)-1;
    physical.Bottom=min(size.Y,after.dwMaximumWindowSize.Y)-1;
    if(!SetConsoleCursorPosition(output,cursor) ||
        !opennt_console_resize_grid(output,NULL,TRUE,&physical))return GetLastError();
    window->Left=window->Top=0;window->Right=size.X-1;window->Bottom=size.Y-1;
    return ERROR_SUCCESS;
}


DWORD run16_console_dispatch(run16_console_frontend *owner,const console_io_request *request,
    console_io_reply *reply)
{
    const console_io_state *s;
    COORD position;
    DWORD count=0,mode=0;
    BOOL ok=FALSE;
    BOOL cells,write_cells,screen_operation,screen_write;
    if (!owner || !request || !reply) return ERROR_INVALID_PARAMETER;
    if (!!owner->screen_begin != !!owner->screen_end) return ERROR_INVALID_PARAMETER;
    ZeroMemory(reply,sizeof(*reply));
    if (request->version!=CONSOLE_IO_VERSION) return ERROR_REVISION_MISMATCH;
    if (!owner->generation || request->generation!=owner->generation) return ERROR_ACCESS_DENIED;
    if (!request->sequence || owner->sequence==UINT32_MAX ||
        request->sequence!=owner->sequence+1 || request->bytes>CONSOLE_IO_DATA_BYTES ||
        request->operation<CONSOLE_IO_WRITE || request->operation>CONSOLE_IO_SNAPSHOT_END)
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
    if (request->operation==CONSOLE_IO_DOS_ACTIVE && s->mode>CONSOLE_IO_WORKER_NATIVE) return ERROR_INVALID_DATA;
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
    if(request->operation==CONSOLE_IO_DOS_ACTIVE) {
        reply->error=owner->activate ? owner->activate(owner->io_context,s->input!=0,s->mode) : ERROR_INVALID_FUNCTION;
        reply->result=reply->error==ERROR_SUCCESS;
        return ERROR_SUCCESS;
    }
    if(owner->enter) {
        reply->error=owner->enter(owner->io_context);
        if(reply->error)return ERROR_SUCCESS;
    }
    screen_operation=(request->operation>=CONSOLE_IO_WRITE && request->operation<=CONSOLE_IO_ATTRIBUTE) ||
        cells || request->operation==CONSOLE_IO_BUFFER_SIZE || request->operation==CONSOLE_IO_WINDOW_RECT ||
        request->operation==CONSOLE_IO_GET_CURSOR_INFO;
    screen_write=screen_operation && request->operation!=CONSOLE_IO_SCREEN_INFO &&
        request->operation!=CONSOLE_IO_GET_CURSOR_INFO && (!cells || write_cells);
    if(screen_operation && owner->screen_begin) {
        reply->error=owner->screen_begin(owner->io_context);
        if(reply->error) {
            if(owner->leave)owner->leave(owner->io_context);
            return ERROR_SUCCESS;
        }
    }
    position.X=(SHORT)s->x; position.Y=(SHORT)s->y;
    SetLastError(ERROR_SUCCESS);
    switch (request->operation) {
    case CONSOLE_IO_SNAPSHOT_BEGIN:
        mode=owner->snapshot_begin ? owner->snapshot_begin(owner->io_context) : ERROR_INVALID_FUNCTION;
        ok=mode==ERROR_SUCCESS;SetLastError(mode);break;
    case CONSOLE_IO_SNAPSHOT_END:
        mode=owner->snapshot_end ? owner->snapshot_end(owner->io_context) : ERROR_INVALID_FUNCTION;
        ok=mode==ERROR_SUCCESS;SetLastError(mode);break;
    case CONSOLE_IO_READ_TEXT_CONFIGURATION:
        mode=owner->read_text_configuration ?
            owner->read_text_configuration(owner->io_context,s->count,s->mode,reply) : ERROR_NOT_FOUND;
        ok=mode==ERROR_SUCCESS;SetLastError(mode);break;
    case CONSOLE_IO_KEYBOARD_LAYOUT: {
        typedef BOOL (WINAPI *query_layout)(LPSTR);
        query_layout query=(query_layout)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
            "GetConsoleKeyboardLayoutNameA");
        /* Preserve the Console owner's result, not this thread's HKL.
         * Original cmdkeyb owns NoInstallkb16 when the query fails. */
        if (!query) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED);break; }
        ok=query((LPSTR)reply->data);
        if (ok) reply->bytes=KL_NAMELENGTH;
        break;
    }
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
        /* Original DOS menu detach may release its clip while the frontend's
         * Window capture is active. It does not own that host clip. */
        ok=owner->window_clip_owned && owner->window_clip_owned(owner->io_context) ?
            TRUE : ClipCursor(s->has_clip ? &rect : NULL);break;
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
        if(ok && owner->title_changed)owner->title_changed(owner->io_context);
        break;
    case CONSOLE_IO_WINDOW_QUERY: {
        if (s->mode==CONSOLE_WINDOW_TEXT_FRAME_REQUIRED) {
            reply->state.left=owner->text_frame_required &&
                owner->text_frame_required(owner->io_context);
            ok=TRUE;break;
        }
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
            if(owner->logical_window) {
                info.srWindow=*owner->logical_window;
                /* Transport capacity, not host pixel/font-derived limits. */
                info.dwMaximumWindowSize.X=160;info.dwMaximumWindowSize.Y=96;
            }
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
    case CONSOLE_IO_CURSOR_POSITION: {
        CONSOLE_SCREEN_BUFFER_INFO current;
        ok=GetConsoleScreenBufferInfo(owner->output,&current);
        if(ok && (current.dwCursorPosition.X!=position.X ||
            current.dwCursorPosition.Y!=position.Y))
            ok=SetConsoleCursorPosition(owner->output,position);
        break;
    }
    case CONSOLE_IO_CURSOR_INFO: {
        CONSOLE_CURSOR_INFO cursor,current;
        cursor.dwSize=s->cursor_size; cursor.bVisible=s->cursor_visible;
        ok=GetConsoleCursorInfo(owner->output,&current);
        if(ok && (current.dwSize!=cursor.dwSize || current.bVisible!=cursor.bVisible))
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
    case CONSOLE_IO_ATTRIBUTE: {
        CONSOLE_SCREEN_BUFFER_INFO current;
        ok=GetConsoleScreenBufferInfo(owner->output,&current);
        if(ok && current.wAttributes!=(WORD)s->attribute)
            ok=SetConsoleTextAttribute(owner->output,(WORD)s->attribute);
        break;
    }
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
        if (!prepend && !owner->prepend_input) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED);break; }
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
        /* Validate the whole original return batch before front insertion.
         * The selected frontend queue or native Console owns the mutation. */
        if (i==s->count) {
            if(owner->prepend_input) {
                DWORD error=owner->prepend_input(owner->io_context,records,s->count);
                ok=!error;SetLastError(error);if(ok)count=s->count;
            } else ok=prepend(owner->input,records,s->count,&count);
        }
        reply->state.count=count;
        break;
    }
    case CONSOLE_IO_READ_INPUT:
    case CONSOLE_IO_PEEK_INPUT: {
        INPUT_RECORD records[CONSOLE_IO_INPUT_CAPACITY];
        DWORD i;
        if(owner->read_input) {
            DWORD error=owner->read_input(owner->io_context,request->operation==CONSOLE_IO_PEEK_INPUT,
                records,s->count,&count);
            ok=!error;SetLastError(error);
        } else if(request->operation==CONSOLE_IO_PEEK_INPUT)
            ok=PeekConsoleInputW(owner->input,records,s->count,&count);
        else if(owner->enter) {
            typedef BOOL (WINAPI *read_input_ex)(HANDLE,PINPUT_RECORD,DWORD,LPDWORD,USHORT);
            read_input_ex read_nowait=(read_input_ex)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"ReadConsoleInputExW");
            if(read_nowait)ok=read_nowait(owner->input,records,s->count,&count,2);
            else SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        } else ok=ReadConsoleInputW(owner->input,records,s->count,&count);
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
        if(ok && owner->logical_window) {
            SMALL_RECT *r=owner->logical_window;
            r->Right=min(r->Right,size.X-1);r->Bottom=min(r->Bottom,size.Y-1);
            r->Left=min(r->Left,r->Right);r->Top=min(r->Top,r->Bottom);
        }
        break;
    }
    case CONSOLE_IO_WINDOW_RECT: {
        SMALL_RECT rect={(SHORT)s->left,(SHORT)s->top,(SHORT)s->right,(SHORT)s->bottom};
        if(owner->logical_window) {
            CONSOLE_SCREEN_BUFFER_INFO info;
            LONG left=rect.Left,top=rect.Top,right=rect.Right,bottom=rect.Bottom;
            if(!s->mode) {
                left+=owner->logical_window->Left;top+=owner->logical_window->Top;
                right+=owner->logical_window->Right;bottom+=owner->logical_window->Bottom;
            }
            ok=GetConsoleScreenBufferInfo(owner->output,&info);
            if(ok && (left<0 || top<0 || right<left || bottom<top ||
                right>=info.dwSize.X || bottom>=info.dwSize.Y ||
                right-left>=160 || bottom-top>=96)) {ok=FALSE;SetLastError(ERROR_INVALID_PARAMETER);}
            if(ok) {
                SMALL_RECT physical;
                rect.Left=(SHORT)left;rect.Top=(SHORT)top;rect.Right=(SHORT)right;rect.Bottom=(SHORT)bottom;
                if(owner->logical_window->Left==rect.Left &&
                    owner->logical_window->Top==rect.Top &&
                    owner->logical_window->Right==rect.Right &&
                    owner->logical_window->Bottom==rect.Bottom)break;
                /* Only the visible presenter uses pixel-derived constraints.
                 * Preserve the complete logical region independently. */
                physical=rect;
                physical.Right=(SHORT)(left+min(right-left+1,info.dwMaximumWindowSize.X)-1);
                physical.Bottom=(SHORT)(top+min(bottom-top+1,info.dwMaximumWindowSize.Y)-1);
                ok=opennt_console_resize_grid(owner->output,NULL,TRUE,&physical);
                if(ok)*owner->logical_window=rect;
            }
        } else ok=opennt_console_resize_grid(owner->output,NULL,s->mode!=0,&rect);
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
    if(screen_operation && owner->screen_end) {
        DWORD error=owner->screen_end(owner->io_context,screen_write);
        if(error && ok) {reply->result=FALSE;reply->error=error;}
    }
    if(owner->leave)owner->leave(owner->io_context);
    return ERROR_SUCCESS;
}
