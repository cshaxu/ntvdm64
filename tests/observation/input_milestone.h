#ifndef OBSERVER_INPUT_MILESTONE_H
#define OBSERVER_INPUT_MILESTONE_H

/* Test-only observation of the actual rendered text. Window uses the same
 * selected context prefix already checked by window_text_geometry_probe.c.
 * No production hook, acknowledgement, focus change or injected marker. */
#include <stddef.h>
#include "window_frame_observation.h"

/* Observation buffer bound follows current wire acceptance, not lib capacity. */
enum { OBSERVER_TEXT_COLUMNS=160, OBSERVER_TEXT_ROWS=96 };

typedef struct observer_text_view {
    unsigned columns, rows;
    int cursor_column, cursor_row;
    char cells[OBSERVER_TEXT_COLUMNS * OBSERVER_TEXT_ROWS];
} observer_text_view;

static BOOL observer_text_read(HANDLE output, HWND window, DWORD expected_pid,
                               observer_text_view *view)
{
    unsigned x, y;
    if (window) {
        DWORD pid = 0;
        HANDLE process;
        observed_window_frame first,second;
        kvm_text_cell *cells=NULL,*check;
        lib_size bytes;
        BOOL ok = FALSE;
        GetWindowThreadProcessId(window, &pid);
        if (pid != expected_pid) return FALSE;
        process = OpenProcess(PROCESS_VM_READ | SYNCHRONIZE, FALSE, pid);
        if (!process) return FALSE;
        /* Reject torn geometry/content rather than treating a transport ACK
         * or old Console mirror as proof of consumption. Blink phase is not
         * an input/result milestone. Every other copied field must agree. */
        if (WaitForSingleObject(process,0)==WAIT_TIMEOUT &&
            observer_window_frame_read(process,window,&first) && first.frame.text &&
            first.text.text_columns<=OBSERVER_TEXT_COLUMNS && first.text.text_rows<=OBSERVER_TEXT_ROWS &&
            first.text.stride<=OBSERVER_TEXT_COLUMNS &&
            kvm_frame_extent(first.text.text_columns,first.text.text_rows,first.text.stride,
                sizeof(*cells),&bytes)==LIB_STATUS_OK &&
            first.text.capacity>=bytes/sizeof(*cells) && (cells=malloc(bytes*2))!=NULL) {
            check=(kvm_text_cell *)((BYTE *)cells+bytes);
            if(observer_read_bytes(process,first.text.cells,cells,bytes) &&
                observer_read_bytes(process,first.text.cells,check,bytes) &&
                !memcmp(cells,check,bytes) && observer_window_frame_read(process,window,&second) &&
                !memcmp(&first,&second,sizeof(first))) {
                first.text.cells=cells;
                if(kvm_text_frame_validate(&first.text)!=LIB_STATUS_OK)goto done;
                view->columns = first.text.text_columns;
                view->rows = first.text.text_rows;
                view->cursor_column = first.text.cursor_column;
                view->cursor_row = first.text.cursor_row;
                for (y = 0; y < view->rows; ++y)
                    for (x = 0; x < view->columns; ++x)
                        view->cells[y * view->columns + x] =
                            (char)cells[y * first.text.stride + x].glyph_index;
                ok = TRUE;
            }
        }
done:
        free(cells); CloseHandle(process); return ok;
    } else {
        CONSOLE_SCREEN_BUFFER_INFO info;
        char *cells = NULL;
        DWORD count = 0;
        if (observer_console_snapshot(output, &info, &cells, &count)) return FALSE;
        view->columns = (unsigned)(info.srWindow.Right - info.srWindow.Left + 1);
        view->rows = (unsigned)(info.srWindow.Bottom - info.srWindow.Top + 1);
        if (view->columns > OBSERVER_TEXT_COLUMNS || view->rows > OBSERVER_TEXT_ROWS) {
            free(cells); return FALSE;
        }
        view->cursor_column = info.dwCursorPosition.X - info.srWindow.Left;
        view->cursor_row = info.dwCursorPosition.Y - info.srWindow.Top;
        for (y = 0; y < view->rows; ++y)
            memcpy(view->cells + y * view->columns,
                cells + (y + info.srWindow.Top) * info.dwSize.X + info.srWindow.Left,
                view->columns);
        free(cells); return TRUE;
    }
}

/* Only the current cursor row counts: a retained historical prompt or result
 * elsewhere on the screen cannot acknowledge this line. */
static BOOL observer_prompt_echo(const observer_text_view *view, const char *echo)
{
    const char *row;
    unsigned x, start;
    size_t length = strlen(echo);
    if (view->cursor_row < 0 || (unsigned)view->cursor_row >= view->rows ||
        view->cursor_column < 0 || (unsigned)view->cursor_column > view->columns)
        return FALSE;
    row = view->cells + view->cursor_row * view->columns;
    if (view->columns < 3 || row[1] != ':') return FALSE;
    for (start = 2; start < view->columns && row[start] != '>'; ++start) {}
    if (++start > view->columns || length > view->columns - start ||
        view->cursor_column != (int)(start + length) ||
        memcmp(row + start, echo, length)) return FALSE;
    for (x = start + (unsigned)length; x < view->columns; ++x)
        if (row[x] != ' ' && row[x] != '\0') return FALSE;
    return TRUE;
}

static BOOL observer_view_contains(const observer_text_view *view, const char *text)
{
    unsigned y;
    char row[OBSERVER_TEXT_COLUMNS + 1];
    for (y = 0; y < view->rows; ++y) {
        memcpy(row, view->cells + y * view->columns, view->columns);
        row[view->columns] = 0;
        if (strstr(row, text)) return TRUE;
    }
    return FALSE;
}
#endif
