#include "include/console_grid.h"
#include <stdlib.h>

/* DIVERGENCE(ADAPTER-WIN32-052): OpenNT ntcon/server/output.c's
 * ResizeScreenBuffer copies rows; it never reflows paragraphs. ConPTY also
 * resizes/reflows storage through SetConsoleWindowInfo. Keep the original
 * cell-grid rule at both reached calls. Private SCREEN_INFORMATION/ROW and
 * CSR/GDI prevent whole-unit composition; no guest or persistent screen
 * state is added. The public handle must support the reached read/write
 * operations. Native failures and incomplete transfers remain failures. */
static BOOL console_grid_transfer(BOOL write, HANDLE output, CHAR_INFO *cells,
    COORD size, SMALL_RECT rect)
{
    COORD origin = {0, 0};
    SMALL_RECT actual = rect;
    BOOL ok = write ? WriteConsoleOutputW(output, cells, size, origin, &actual)
                    : ReadConsoleOutputW(output, cells, size, origin, &actual);
    if (ok && (actual.Left != rect.Left || actual.Top != rect.Top ||
        actual.Right != rect.Right || actual.Bottom != rect.Bottom)) {
        SetLastError(write ? ERROR_WRITE_FAULT : ERROR_READ_FAULT);
        return FALSE;
    }
    return ok;
}

BOOL opennt_console_resize_grid(HANDLE output, const COORD *size,
    BOOL absolute, const SMALL_RECT *window)
{
    CONSOLE_SCREEN_BUFFER_INFO before, after;
    CHAR_INFO *saved, *resized = NULL;
    SMALL_RECT rect;
    COORD cursor;
    int top, rows, columns, x, y;
    BOOL ok;
    DWORD error;

    if (!GetConsoleScreenBufferInfo(output, &before) ||
        (size && (size->X <= 0 || size->Y <= 0 ||
         (size->X == before.dwSize.X && size->Y == before.dwSize.Y))))
        return size ? SetConsoleScreenBufferSize(output, *size)
                    : SetConsoleWindowInfo(output, absolute, window);
    saved = (CHAR_INFO *)calloc((size_t)before.dwSize.X * before.dwSize.Y,
        sizeof(*saved));
    if (!saved) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    rect.Left = rect.Top = 0;
    rect.Right = before.dwSize.X - 1; rect.Bottom = before.dwSize.Y - 1;
    ok = console_grid_transfer(FALSE, output, saved, before.dwSize, rect);
    if (ok) ok = size ? SetConsoleScreenBufferSize(output, *size)
                      : SetConsoleWindowInfo(output, absolute, window);
    if (ok) ok = GetConsoleScreenBufferInfo(output, &after);
    if (ok && (before.dwSize.X != after.dwSize.X ||
               before.dwSize.Y != after.dwSize.Y)) {
        resized = (CHAR_INFO *)calloc((size_t)after.dwSize.X * after.dwSize.Y,
            sizeof(*resized));
        if (!resized) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); ok = FALSE; }
        if (ok) {
            /* Original ResizeScreenBuffer: retain the cursor-containing rows,
             * clip columns, extend each row's final attribute, blank new rows. */
            top = before.dwCursorPosition.Y >= after.dwSize.Y
                ? before.dwCursorPosition.Y - after.dwSize.Y + 1 : 0;
            rows = before.dwSize.Y - top;
            if (rows > after.dwSize.Y) rows = after.dwSize.Y;
            columns = before.dwSize.X < after.dwSize.X
                ? before.dwSize.X : after.dwSize.X;
            for (y = 0; y < after.dwSize.Y; ++y) {
                for (x = 0; x < after.dwSize.X; ++x) {
                    CHAR_INFO *cell = &resized[y * after.dwSize.X + x];
                    if (y < rows && x < columns)
                        *cell = saved[(y + top) * before.dwSize.X + x];
                    else {
                        cell->Char.UnicodeChar = L' ';
                        cell->Attributes = y < rows
                            ? saved[(y + top + 1) * before.dwSize.X - 1].Attributes
                            : before.wAttributes;
                    }
                }
            }
            rect.Right = after.dwSize.X - 1; rect.Bottom = after.dwSize.Y - 1;
            ok = console_grid_transfer(TRUE, output, resized, after.dwSize, rect);
            cursor = before.dwCursorPosition;
            if (cursor.X >= after.dwSize.X) cursor.X = 0;
            if (cursor.Y >= after.dwSize.Y) cursor.Y = after.dwSize.Y - 1;
            if (ok) ok = SetConsoleCursorPosition(output, cursor);
        }
    }
    error = GetLastError();
    free(resized); free(saved);
    SetLastError(error);
    return ok;
}
