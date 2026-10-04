#ifndef OBSERVER_INPUT_MILESTONE_H
#define OBSERVER_INPUT_MILESTONE_H

/* Test-only observation of the actual rendered text. Window uses the same
 * selected context prefix already checked by window_text_geometry_probe.c.
 * No production hook, acknowledgement, focus change or injected marker. */
#include <stddef.h>
#include "lib/kvm-window/frame_interface.h"

typedef struct observer_text_view {
    unsigned columns, rows;
    int cursor_column, cursor_row;
    char cells[KVM_TEXT_COLUMNS * KVM_TEXT_ROWS];
} observer_text_view;

typedef struct observer_window_prefix {
    void *owner;
    kvm_window_frame frame;
} observer_window_prefix;

static BOOL observer_text_read(HANDLE output, HWND window, DWORD expected_pid,
                               observer_text_view *view)
{
    unsigned x, y;
    if (window) {
        DWORD pid = 0;
        SIZE_T copied;
        HANDLE process;
        ULONG_PTR context, address;
        kvm_text_frame *first, *second;
        lib_bool flags[2], after_flags[2];
        BOOL ok = FALSE;
        GetWindowThreadProcessId(window, &pid);
        if (pid != expected_pid) return FALSE;
        process = OpenProcess(PROCESS_VM_READ | SYNCHRONIZE, FALSE, pid);
        if (!process) return FALSE;
        context = (ULONG_PTR)GetWindowLongPtrW(window, GWLP_USERDATA);
        address = context + offsetof(observer_window_prefix, frame);
        first = (kvm_text_frame *)malloc(sizeof(*first) * 2);
        if (!first) { CloseHandle(process); return FALSE; }
        second = first + 1;
        /* Reject torn geometry/content rather than treating a transport ACK
         * or old Console mirror as proof of consumption. Blink phase is not
         * an input/result milestone. Every other copied field must agree. */
        if (context && WaitForSingleObject(process, 0) == WAIT_TIMEOUT &&
            ReadProcessMemory(process, (void *)address, flags, sizeof(flags), &copied) &&
            copied == sizeof(flags) && flags[0] && !flags[1] &&
            ReadProcessMemory(process, (void *)(address + offsetof(kvm_window_frame, text)),
                first, sizeof(*first), &copied) && copied == sizeof(*first) &&
            ReadProcessMemory(process, (void *)(address + offsetof(kvm_window_frame, text)),
                second, sizeof(*second), &copied) && copied == sizeof(*second) &&
            ReadProcessMemory(process, (void *)address, after_flags, sizeof(after_flags), &copied) &&
            copied == sizeof(after_flags) && !memcmp(flags, after_flags, sizeof(flags)) &&
            context == (ULONG_PTR)GetWindowLongPtrW(window, GWLP_USERDATA)) {
            first->cursor_phase = second->cursor_phase = 0;
            if (!memcmp(first, second, sizeof(*first)) &&
                kvm_text_frame_validate(first) == LIB_STATUS_OK) {
                view->columns = first->text_columns;
                view->rows = first->text_rows;
                view->cursor_column = first->cursor_column;
                view->cursor_row = first->cursor_row;
                for (y = 0; y < view->rows; ++y)
                    for (x = 0; x < view->columns; ++x)
                        view->cells[y * view->columns + x] =
                            (char)first->cells[y * KVM_TEXT_COLUMNS + x].glyph_index;
                ok = TRUE;
            }
        }
        free(first); CloseHandle(process); return ok;
    } else {
        CONSOLE_SCREEN_BUFFER_INFO info;
        char *cells = NULL;
        DWORD count = 0;
        if (observer_console_snapshot(output, &info, &cells, &count)) return FALSE;
        view->columns = (unsigned)(info.srWindow.Right - info.srWindow.Left + 1);
        view->rows = (unsigned)(info.srWindow.Bottom - info.srWindow.Top + 1);
        if (view->columns > KVM_TEXT_COLUMNS || view->rows > KVM_TEXT_ROWS) {
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
    char row[KVM_TEXT_COLUMNS + 1];
    for (y = 0; y < view->rows; ++y) {
        memcpy(row, view->cells + y * view->columns, view->columns);
        row[view->columns] = 0;
        if (strstr(row, text)) return TRUE;
    }
    return FALSE;
}
#endif
