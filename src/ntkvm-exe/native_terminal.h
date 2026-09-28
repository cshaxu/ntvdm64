#ifndef NTKVM_NATIVE_TERMINAL_H
#define NTKVM_NATIVE_TERMINAL_H
#include <windows.h>
/* Windows RPC's `small` macro collides with an upstream cell attribute. */
#pragma push_macro("small")
#undef small
#include <vterm.h>
#pragma pop_macro("small")
typedef struct ntkvm_terminal ntkvm_terminal;
typedef struct ntkvm_terminal_frame {
    ULONGLONG revision;
    int rows,columns;
    int history_rows,viewport_rows,viewport_columns;
    VTermPos cursor;
    BOOL cursor_visible;
    /* Original Unicode clusters, widths and attributes; colours resolved to
     * RGB. Presentation may map glyphs but never changes this terminal state. */
    VTermScreenCell *cells;
} ntkvm_terminal_frame;
DWORD ntkvm_terminal_open(int,int,ntkvm_terminal **);
void ntkvm_terminal_close(ntkvm_terminal *);
/* ConPTY output callback; never writes to the input pipe on the reader thread. */
DWORD ntkvm_terminal_feed(void *,const BYTE *,DWORD);
DWORD ntkvm_terminal_resize(ntkvm_terminal *,int,int);
/* Fresh frontend terminal only. Import canonical visible cells/history before
 * ConPTY creation; not a remote Windows-buffer write or live backend reset. */
DWORD ntkvm_terminal_seed_console(ntkvm_terminal *,const CONSOLE_SCREEN_BUFFER_INFOEX *,const CHAR_INFO *,unsigned);
/* Active frontend screen import; preserves the existing parser and ConPTY.
 * History is replaced, not appended. Not a remote Console-buffer setter. */
DWORD ntkvm_terminal_import_console(ntkvm_terminal *,const CONSOLE_SCREEN_BUFFER_INFOEX *,const CHAR_INFO *,unsigned);
/* Shared projection rule for painting and recognizing unchanged native cells. */
WORD ntkvm_terminal_console_colour(const VTermColor *,const COLORREF *);
/* Serialize an actual frontend Console operation with native output parsing. */
void ntkvm_terminal_screen_enter(ntkvm_terminal *);
void ntkvm_terminal_screen_leave(ntkvm_terminal *);
ULONGLONG ntkvm_terminal_revision(ntkvm_terminal *);
/* Presentation supplies its available scrollback capacity, not a parser or
 * hidden-Console default. History preserves each line's original width. */
DWORD ntkvm_terminal_history_limit(ntkvm_terminal *,unsigned);
DWORD ntkvm_terminal_capture(ntkvm_terminal *,ntkvm_terminal_frame *);
void ntkvm_terminal_frame_free(ntkvm_terminal_frame *);
/* Replies have exactly one owner. Caller drains them outside the parser lock,
 * separately from the output-reader callback, and frees the returned block. */
DWORD ntkvm_terminal_take_replies(ntkvm_terminal *,BYTE **,DWORD *);
BOOL ntkvm_terminal_startup_replied(ntkvm_terminal *);
#endif
