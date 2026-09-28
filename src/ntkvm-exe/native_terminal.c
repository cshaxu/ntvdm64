#include "native_terminal.h"
#include "native_terminal_screen.h"
#include <string.h>
#include <limits.h>
#include <stdio.h>

typedef struct history_line {
    struct history_line *older,*newer;
    int columns;
    VTermScreenCell *cells;
} history_line;

struct ntkvm_terminal {
    VTerm *parser;
    VTermScreen *screen;
    CRITICAL_SECTION lock;
    BYTE *replies;
    DWORD reply_bytes,error;
    BOOL cursor_visible;
    BOOL alternate,cursor_reply;
    ULONGLONG revision;
    unsigned history_count,history_limit;
    history_line *oldest,*newest;
};

static void history_remove(ntkvm_terminal *terminal,history_line *line)
{
    if(line->older)line->older->newer=line->newer;else terminal->oldest=line->newer;
    if(line->newer)line->newer->older=line->older;else terminal->newest=line->older;
    --terminal->history_count;HeapFree(GetProcessHeap(),0,line);
}

static int history_clear(void *context)
{
    ntkvm_terminal *terminal=context;
    while(terminal->oldest)history_remove(terminal,terminal->oldest);
    return 1;
}

static int history_push(int columns,const VTermScreenCell *cells,void *context)
{
    ntkvm_terminal *terminal=context;history_line *line;int column;
    if(!terminal->history_limit || terminal->error)return 1;
    if(columns<=0 || (SIZE_T)columns>(SIZE_MAX-sizeof(*line))/sizeof(*cells)) {
        terminal->error=ERROR_ARITHMETIC_OVERFLOW;return 0;
    }
    line=HeapAlloc(GetProcessHeap(),0,sizeof(*line)+(SIZE_T)columns*sizeof(*cells));
    if(!line) {terminal->error=ERROR_NOT_ENOUGH_MEMORY;return 0;}
    line->cells=(VTermScreenCell *)(line+1);
    line->columns=columns;memcpy(line->cells,cells,(SIZE_T)columns*sizeof(*cells));
    for(column=0;column<columns;++column) {
        vterm_screen_convert_color_to_rgb(terminal->screen,&line->cells[column].fg);
        vterm_screen_convert_color_to_rgb(terminal->screen,&line->cells[column].bg);
    }
    line->older=terminal->newest;line->newer=NULL;
    if(terminal->newest)terminal->newest->newer=line;else terminal->oldest=line;
    terminal->newest=line;++terminal->history_count;
    while(terminal->history_count>terminal->history_limit)history_remove(terminal,terminal->oldest);
    return 1;
}

static void blank_cell(ntkvm_terminal *terminal,VTermScreenCell *cell)
{
    ZeroMemory(cell,sizeof(*cell));cell->width=1;
    vterm_state_get_default_colors(vterm_obtain_state(terminal->parser),&cell->fg,&cell->bg);
    vterm_screen_convert_color_to_rgb(terminal->screen,&cell->fg);
    vterm_screen_convert_color_to_rgb(terminal->screen,&cell->bg);
}

static int history_pop(int columns,VTermScreenCell *cells,void *context)
{
    ntkvm_terminal *terminal=context;history_line *line=terminal->newest;int column;
    /* A width change must not silently discard the right half of a retained
     * line. Keep it scrollable instead of popping a truncated replacement. */
    if(!line || line->columns>columns)return 0;
    memcpy(cells,line->cells,(SIZE_T)line->columns*sizeof(*cells));
    for(column=line->columns;column<columns;++column)blank_cell(terminal,cells+column);
    history_remove(terminal,line);return 1;
}

static void reply(const char *bytes,size_t count,void *context)
{
    ntkvm_terminal *terminal=context;BYTE *grown;
    if(terminal->error)return;
    if(count>MAXDWORD-terminal->reply_bytes) {terminal->error=ERROR_ARITHMETIC_OVERFLOW;return;}
    if(!count)return;
    grown=terminal->replies ? HeapReAlloc(GetProcessHeap(),0,terminal->replies,terminal->reply_bytes+count) :
        HeapAlloc(GetProcessHeap(),0,count);
    if(!grown) {terminal->error=ERROR_NOT_ENOUGH_MEMORY;return;}
    memcpy(grown+terminal->reply_bytes,bytes,count);
    terminal->replies=grown;terminal->reply_bytes+=(DWORD)count;
    /* libvterm emits complete responses. Cursor inheritance requires CPR;
     * newer ConPTY may also ask DA1, but that is not an OS-wide prerequisite. */
    if(count>=3 && bytes[0]=='\033' && bytes[1]=='[') {
        if(bytes[count-1]=='R')terminal->cursor_reply=TRUE;
    }
}

static int property(VTermProp prop,VTermValue *value,void *context)
{
    ntkvm_terminal *terminal=context;
    if(prop==VTERM_PROP_CURSORVISIBLE) {terminal->cursor_visible=value->boolean;++terminal->revision;}
    if(prop==VTERM_PROP_ALTSCREEN) {terminal->alternate=value->boolean;++terminal->revision;}
    return 1;
}

static int damaged(VTermRect rect,void *context)
{
    UNREFERENCED_PARAMETER(rect);
    ++((ntkvm_terminal *)context)->revision;return 1;
}
static int cursor_moved(VTermPos position,VTermPos old,int visible,void *context)
{
    UNREFERENCED_PARAMETER(position);UNREFERENCED_PARAMETER(old);UNREFERENCED_PARAMETER(visible);
    ++((ntkvm_terminal *)context)->revision;return 1;
}

DWORD ntkvm_terminal_open(int rows,int columns,ntkvm_terminal **result)
{
    static const VTermScreenCallbacks callbacks={.damage=damaged,.movecursor=cursor_moved,.settermprop=property,
        .sb_pushline=history_push,.sb_popline=history_pop,.sb_clear=history_clear};
    ntkvm_terminal *terminal;
    if(!result)return ERROR_INVALID_PARAMETER;
    *result=NULL;
    if(rows<=0 || columns<=0 || rows>SHRT_MAX || columns>SHRT_MAX)return ERROR_INVALID_PARAMETER;
    terminal=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*terminal));
    if(!terminal)return ERROR_NOT_ENOUGH_MEMORY;
    InitializeCriticalSection(&terminal->lock);terminal->cursor_visible=TRUE;
    terminal->parser=vterm_new(rows,columns);
    if(!terminal->parser) {ntkvm_terminal_close(terminal);return ERROR_NOT_ENOUGH_MEMORY;}
    vterm_set_utf8(terminal->parser,1);
    terminal->screen=vterm_obtain_screen(terminal->parser);
    if(!terminal->screen) {ntkvm_terminal_close(terminal);return ERROR_NOT_ENOUGH_MEMORY;}
    vterm_screen_set_callbacks(terminal->screen,&callbacks,terminal);
    vterm_screen_enable_altscreen(terminal->screen,1);
    vterm_output_set_callback(terminal->parser,reply,terminal);
    vterm_screen_reset(terminal->screen,1);
    *result=terminal;return 0;
}

void ntkvm_terminal_close(ntkvm_terminal *terminal)
{
    if(!terminal)return;
    history_clear(terminal);
    if(terminal->parser)vterm_free(terminal->parser);
    if(terminal->replies)HeapFree(GetProcessHeap(),0,terminal->replies);
    DeleteCriticalSection(&terminal->lock);HeapFree(GetProcessHeap(),0,terminal);
}

DWORD ntkvm_terminal_feed(void *context,const BYTE *bytes,DWORD count)
{
    ntkvm_terminal *terminal=context;DWORD error;
    if(!terminal || (!bytes && count))return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&terminal->lock);
    if(!terminal->error && count && vterm_input_write(terminal->parser,(const char *)bytes,count)!=count)
        terminal->error=ERROR_INVALID_DATA;
    vterm_screen_flush_damage(terminal->screen);
    error=terminal->error;
    LeaveCriticalSection(&terminal->lock);
    return error;
}

void ntkvm_terminal_screen_enter(ntkvm_terminal *terminal)
{
    EnterCriticalSection(&terminal->lock);
}

void ntkvm_terminal_screen_leave(ntkvm_terminal *terminal)
{
    LeaveCriticalSection(&terminal->lock);
}

ULONGLONG ntkvm_terminal_revision(ntkvm_terminal *terminal)
{
    ULONGLONG revision;
    EnterCriticalSection(&terminal->lock);revision=terminal->revision;
    LeaveCriticalSection(&terminal->lock);return revision;
}

DWORD ntkvm_terminal_resize(ntkvm_terminal *terminal,int rows,int columns)
{
    DWORD error;
    if(!terminal || rows<=0 || columns<=0 || rows>SHRT_MAX || columns>SHRT_MAX)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&terminal->lock);
    vterm_set_size(terminal->parser,rows,columns);
    error=terminal->error;
    LeaveCriticalSection(&terminal->lock);
    return error;
}

static uint32_t console_character(const CHAR_INFO *cells,int column,int columns)
{
    static const WCHAR pictures[32]={0x20,0x263a,0x263b,0x2665,0x2666,0x2663,0x2660,0x2022,
        0x25d8,0x25cb,0x25d9,0x2642,0x2640,0x266a,0x266b,0x263c,
        0x25ba,0x25c4,0x2195,0x203c,0xb6,0xa7,0x25ac,0x21a8,
        0x2191,0x2193,0x2192,0x2190,0x221f,0x2194,0x25b2,0x25bc};
    uint32_t ch=cells[column].Char.UnicodeChar;
    if(ch<32)return pictures[ch];
    if(ch==127)return 0x2302;
    if(ch>=0xd800 && ch<=0xdbff && column+1<columns) {
        uint32_t low=cells[column+1].Char.UnicodeChar;
        if(low>=0xdc00 && low<=0xdfff)return 0x10000+((ch-0xd800)<<10)+(low-0xdc00);
    }
    return ch>=0xd800 && ch<=0xdfff ? 0xfffd : ch;
}

DWORD ntkvm_terminal_seed_console(ntkvm_terminal *terminal,
    const CONSOLE_SCREEN_BUFFER_INFOEX *info,const CHAR_INFO *cells,unsigned top)
{
    int rows,columns,row,column;DWORD error=0;VTermScreenCell *line=NULL;
    char bytes[160];WORD previous=0xffff;
    if(!terminal || !info || !cells)return ERROR_INVALID_PARAMETER;
    vterm_get_size(terminal->parser,&rows,&columns);
    if(info->dwSize.X!=columns || top+(unsigned)rows>(unsigned)info->dwSize.Y ||
       info->dwCursorPosition.X<0 || info->dwCursorPosition.X>=columns ||
       info->dwCursorPosition.Y<(int)top || info->dwCursorPosition.Y>=(int)top+rows)
        return ERROR_INVALID_PARAMETER;
    line=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(SIZE_T)columns*sizeof(*line));
    if(!line)return ERROR_NOT_ENOUGH_MEMORY;
    EnterCriticalSection(&terminal->lock);
    /* This is a fresh model prepared before its reader starts. History is
     * copied directly; the public parser API paints only the active viewport. */
    for(row=0;row<(int)top;++row) {
        const CHAR_INFO *source=cells+(SIZE_T)row*columns;
        for(column=0;column<columns;++column) {
            WORD attr=source[column].Attributes;
            COLORREF fg=info->ColorTable[attr&15],bg=info->ColorTable[(attr>>4)&15];
            ZeroMemory(line+column,sizeof(*line));
            line[column].chars[0]=console_character(source,column,columns);
            line[column].width=(attr&COMMON_LVB_LEADING_BYTE) && column+1<columns ? 2 : 1;
            line[column].attrs.reverse=(attr&COMMON_LVB_REVERSE_VIDEO)!=0;
            line[column].attrs.underline=(attr&COMMON_LVB_UNDERSCORE)!=0;
            vterm_color_rgb(&line[column].fg,GetRValue(fg),GetGValue(fg),GetBValue(fg));
            vterm_color_rgb(&line[column].bg,GetRValue(bg),GetGValue(bg),GetBValue(bg));
        }
        if(!history_push(columns,line,terminal)) {error=terminal->error;goto done;}
    }
    vterm_input_write(terminal->parser,"\033[?7l",5);
    for(row=0;row<rows;++row) {
        const CHAR_INFO *source=cells+(SIZE_T)(top+row)*columns;
        for(column=0;column<columns;++column) {
            WCHAR character[2];int chars=1,length;uint32_t ch;WORD attr=source[column].Attributes;
            if(attr&COMMON_LVB_TRAILING_BYTE)continue;
            if(attr!=previous) {
                COLORREF fg=info->ColorTable[attr&15],bg=info->ColorTable[(attr>>4)&15];
                length=sprintf_s(bytes,sizeof(bytes),"\033[0;38;2;%u;%u;%u;48;2;%u;%u;%u%sm",
                    GetRValue(fg),GetGValue(fg),GetBValue(fg),GetRValue(bg),GetGValue(bg),GetBValue(bg),
                    (attr&COMMON_LVB_REVERSE_VIDEO) ? ";7" : "");
                vterm_input_write(terminal->parser,bytes,length);
                if(attr&COMMON_LVB_UNDERSCORE)vterm_input_write(terminal->parser,"\033[4m",4);
                previous=attr;
            }
            length=sprintf_s(bytes,sizeof(bytes),"\033[%d;%dH",row+1,column+1);
            vterm_input_write(terminal->parser,bytes,length);
            ch=console_character(source,column,columns);
            if(ch>0xffff) {
                ch-=0x10000;character[0]=(WCHAR)(0xd800+(ch>>10));
                character[1]=(WCHAR)(0xdc00+(ch&0x3ff));chars=2;
            } else character[0]=(WCHAR)ch;
            length=WideCharToMultiByte(CP_UTF8,0,character,chars,bytes,sizeof(bytes),NULL,NULL);
            if(!length) {error=GetLastError();goto done;}
            vterm_input_write(terminal->parser,bytes,length);
            if(chars==2 && !(attr&COMMON_LVB_LEADING_BYTE))++column;
        }
    }
    {
        COLORREF fg=info->ColorTable[info->wAttributes&15],bg=info->ColorTable[(info->wAttributes>>4)&15];
        int length=sprintf_s(bytes,sizeof(bytes),"\033[?7h\033[0;38;2;%u;%u;%u;48;2;%u;%u;%um\033[%d;%dH",
            GetRValue(fg),GetGValue(fg),GetBValue(fg),GetRValue(bg),GetGValue(bg),GetBValue(bg),
            info->dwCursorPosition.Y-(int)top+1,info->dwCursorPosition.X+1);
        vterm_input_write(terminal->parser,bytes,length);
    }
    vterm_screen_flush_damage(terminal->screen);error=terminal->error;
done:
    LeaveCriticalSection(&terminal->lock);HeapFree(GetProcessHeap(),0,line);
    return error;
}

WORD ntkvm_terminal_console_colour(const VTermColor *colour,const COLORREF *palette)
{
    unsigned i,best=0;unsigned distance=~0u;
    for(i=0;i<16;++i) {
        int r=(int)colour->rgb.red-GetRValue(palette[i]);
        int g=(int)colour->rgb.green-GetGValue(palette[i]);
        int b=(int)colour->rgb.blue-GetBValue(palette[i]);
        unsigned value=(unsigned)(r*r+g*g+b*b);
        if(value<distance) {distance=value;best=i;}
    }
    return (WORD)best;
}

static BOOL same_console_projection(const VTermScreenCell *native,
    const VTermScreenCell *imported,WORD attribute,const COLORREF *palette)
{
    uint32_t ch=native->chars[0];
    WORD fg=ntkvm_terminal_console_colour(&native->fg,palette);
    WORD bg=ntkvm_terminal_console_colour(&native->bg,palette);
    if(!ch)ch=' ';
    else if(ch>0xffff && native->width!=2)ch='?';
    if(native->attrs.reverse) {WORD swap=fg;fg=bg;bg=swap;}
    return ch==imported->chars[0] && native->width==imported->width &&
        (fg|(bg<<4))==(attribute&255) &&
        !!native->attrs.underline==!!(attribute&COMMON_LVB_UNDERSCORE) &&
        !(attribute&COMMON_LVB_REVERSE_VIDEO);
}

DWORD ntkvm_terminal_import_console(ntkvm_terminal *terminal,
    const CONSOLE_SCREEN_BUFFER_INFOEX *info,const CHAR_INFO *cells,unsigned top)
{
    int rows,columns,row,column;DWORD error=0;VTermScreenCell *converted=NULL;
    ntkvm_terminal history={0};VTermPos cursor;history_line *old_line;
    int old_history;
    if(!terminal || !info || !cells)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&terminal->lock);
    vterm_get_size(terminal->parser,&rows,&columns);
    if(info->dwSize.X!=columns || top>(unsigned)SHRT_MAX ||
       top+(unsigned)rows>(unsigned)info->dwSize.Y ||
       info->dwCursorPosition.X<0 || info->dwCursorPosition.X>=columns ||
       info->dwCursorPosition.Y<(int)top || info->dwCursorPosition.Y>=(int)top+rows) {
        error=ERROR_INVALID_PARAMETER;goto done;
    }
    if(terminal->error) {error=terminal->error;goto done;}
    if((SIZE_T)rows>(SIZE_MAX/sizeof(*converted))/(SIZE_T)columns) {
        error=ERROR_ARITHMETIC_OVERFLOW;goto done;
    }
    converted=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(SIZE_T)rows*columns*sizeof(*converted));
    if(!converted) {error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    history.screen=terminal->screen;history.history_limit=terminal->history_limit;
    old_history=terminal->alternate ? 0 : (int)terminal->history_count;
    old_line=terminal->alternate ? NULL : terminal->oldest;
    for(row=0;row<(int)top+rows;++row) {
        const CHAR_INFO *source=cells+(SIZE_T)row*columns;
        VTermScreenCell *line=converted+(SIZE_T)(row<(int)top ? 0 : row-(int)top)*columns;
        for(column=0;column<columns;++column) {
            WORD attr=source[column].Attributes;
            COLORREF fg=info->ColorTable[attr&15],bg=info->ColorTable[(attr>>4)&15];
            ZeroMemory(line+column,sizeof(*line));
            line[column].chars[0]=console_character(source,column,columns);
            line[column].width=(attr&COMMON_LVB_LEADING_BYTE) && column+1<columns ? 2 : 1;
            line[column].attrs.reverse=(attr&COMMON_LVB_REVERSE_VIDEO)!=0;
            line[column].attrs.underline=(attr&COMMON_LVB_UNDERSCORE)!=0;
            vterm_color_rgb(&line[column].fg,GetRValue(fg),GetGValue(fg),GetBValue(fg));
            vterm_color_rgb(&line[column].bg,GetRValue(bg),GetGValue(bg),GetBValue(bg));
            {
                VTermScreenCell native={0};BOOL have=FALSE;
                if(row<old_history) {
                    if(old_line && column<old_line->columns) {native=old_line->cells[column];have=TRUE;}
                } else if(row-old_history<rows)
                    have=vterm_screen_get_cell(terminal->screen,(VTermPos){row-old_history,column},&native)!=0;
                if(have) {
                    vterm_screen_convert_color_to_rgb(terminal->screen,&native.fg);
                    vterm_screen_convert_color_to_rgb(terminal->screen,&native.bg);
                    /* CHAR_INFO cannot carry clusters or exact native RGB.
                     * Equal projections are not evidence of a DOS rewrite. */
                    if(same_console_projection(&native,line+column,attr,info->ColorTable))line[column]=native;
                }
            }
        }
        if(row<old_history && old_line)old_line=old_line->newer;
        if(row<(int)top && !terminal->alternate && !history_push(columns,line,&history)) {
            error=history.error;goto done;
        }
    }
    cursor.row=info->dwCursorPosition.Y-(int)top;cursor.col=info->dwCursorPosition.X;
    if(!ntkvm_vterm_replace_screen(terminal->screen,rows,columns,converted,cursor)) {
        error=ERROR_INVALID_DATA;goto done;
    }
    if(!terminal->alternate) {
        history_clear(terminal);
        terminal->oldest=history.oldest;terminal->newest=history.newest;
        terminal->history_count=history.history_count;
        history.oldest=history.newest=NULL;history.history_count=0;
    }
    vterm_screen_flush_damage(terminal->screen);
done:
    history_clear(&history);
    if(converted)HeapFree(GetProcessHeap(),0,converted);
    LeaveCriticalSection(&terminal->lock);return error;
}

DWORD ntkvm_terminal_capture(ntkvm_terminal *terminal,ntkvm_terminal_frame *frame)
{
    DWORD error=0;SIZE_T count,index;history_line *line;int row=0,column;VTermScreenCell blank;
    if(!terminal || !frame)return ERROR_INVALID_PARAMETER;
    ZeroMemory(frame,sizeof(*frame));
    EnterCriticalSection(&terminal->lock);
    if(terminal->error) {error=terminal->error;goto done;}
    frame->revision=terminal->revision;
    vterm_get_size(terminal->parser,&frame->viewport_rows,&frame->viewport_columns);
    frame->history_rows=terminal->alternate ? 0 : (int)terminal->history_count;
    frame->rows=frame->history_rows+frame->viewport_rows;frame->columns=frame->viewport_columns;
    if(!terminal->alternate)for(line=terminal->oldest;line;line=line->newer)
        if(line->columns>frame->columns)frame->columns=line->columns;
    count=(SIZE_T)frame->rows*frame->columns;
    if(count>SIZE_MAX/sizeof(*frame->cells)) {error=ERROR_ARITHMETIC_OVERFLOW;goto done;}
    frame->cells=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,count*sizeof(*frame->cells));
    if(!frame->cells) {error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    blank_cell(terminal,&blank);
    for(index=0;index<count;++index)frame->cells[index]=blank;
    if(!terminal->alternate)for(line=terminal->oldest;line;line=line->newer,++row)
        memcpy(frame->cells+(SIZE_T)row*frame->columns,line->cells,(SIZE_T)line->columns*sizeof(*frame->cells));
    for(row=0;row<frame->viewport_rows;++row)for(column=0;column<frame->viewport_columns;++column) {
        VTermPos position={row,column};
        VTermScreenCell *cell=frame->cells+(SIZE_T)(frame->history_rows+row)*frame->columns+column;
        if(!vterm_screen_get_cell(terminal->screen,position,cell)) {error=ERROR_INVALID_DATA;break;}
        vterm_screen_convert_color_to_rgb(terminal->screen,&cell->fg);
        vterm_screen_convert_color_to_rgb(terminal->screen,&cell->bg);
    }
    vterm_state_get_cursorpos(vterm_obtain_state(terminal->parser),&frame->cursor);
    frame->cursor.row+=frame->history_rows;
    frame->cursor_visible=terminal->cursor_visible;
done:
    LeaveCriticalSection(&terminal->lock);
    if(error)ntkvm_terminal_frame_free(frame);
    return error;
}

DWORD ntkvm_terminal_history_limit(ntkvm_terminal *terminal,unsigned limit)
{
    if(!terminal || limit>SHRT_MAX)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&terminal->lock);
    terminal->history_limit=limit;
    while(terminal->history_count>limit)history_remove(terminal,terminal->oldest);
    LeaveCriticalSection(&terminal->lock);return 0;
}

void ntkvm_terminal_frame_free(ntkvm_terminal_frame *frame)
{
    if(frame->cells)HeapFree(GetProcessHeap(),0,frame->cells);
    ZeroMemory(frame,sizeof(*frame));
}

DWORD ntkvm_terminal_take_replies(ntkvm_terminal *terminal,BYTE **bytes,DWORD *count)
{
    DWORD error;
    if(!terminal || !bytes || !count)return ERROR_INVALID_PARAMETER;
    *bytes=NULL;*count=0;
    EnterCriticalSection(&terminal->lock);
    error=terminal->error;
    if(!error) {
        *bytes=terminal->replies;*count=terminal->reply_bytes;
        terminal->replies=NULL;terminal->reply_bytes=0;
    }
    LeaveCriticalSection(&terminal->lock);
    return error;
}

BOOL ntkvm_terminal_startup_replied(ntkvm_terminal *terminal)
{
    BOOL ready;
    EnterCriticalSection(&terminal->lock);
    ready=terminal->cursor_reply && !terminal->reply_bytes;
    LeaveCriticalSection(&terminal->lock);return ready;
}
