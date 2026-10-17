#include "wow_edit_drawing_bindings.h"

/* ADAPTER-WOW-064. Original Microsoft kernel/init.c LW_BrushInit's
 * monochrome pattern, pinned source/hash in the S3 source ledger. Only
 * resource creation is rehosted; rtl/draw.c remains the drawing algorithm. */
HBRUSH wow_edit_system_brushes[WOW_EDIT_COLOR_COUNT];
HBRUSH wow_edit_gray_brush;
static INIT_ONCE brushes_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK initialize_brushes(PINIT_ONCE once, PVOID parameter,
    PVOID *context)
{
    static const WORD pattern[8] = {0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa};
    HBRUSH colors[WOW_EDIT_COLOR_COUNT], gray;
    HBITMAP bitmap;
    DWORD error;
    unsigned i;
    (void)once; (void)parameter; (void)context;
    for (i = 0; i < WOW_EDIT_COLOR_COUNT; ++i) {
        colors[i] = GetSysColorBrush((int)i);
        if (!colors[i]) { SetLastError(ERROR_INVALID_HANDLE); return FALSE; }
    }
    bitmap = CreateBitmap(8, 8, 1, 1, pattern);
    if (!bitmap) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    gray = CreatePatternBrush(bitmap);
    error = gray ? ERROR_SUCCESS : ERROR_NOT_ENOUGH_MEMORY;
    DeleteObject(bitmap);
    if (!gray) { SetLastError(error); return FALSE; }
    /* System brushes are borrowed, never deleted. This sole owned brush
     * lives with the process's original control pools; no per-task disposal
     * may invalidate a brush in another task's active original DrawFrame. */
    CopyMemory(wow_edit_system_brushes, colors, sizeof(colors));
    wow_edit_gray_brush = gray;
    return TRUE;
}

BOOL WINAPI wow_edit_drawing_ready(void)
{
    DWORD error = GetLastError();
    if (!InitOnceExecuteOnce(&brushes_once, initialize_brushes, NULL, NULL))
        return FALSE;
    SetLastError(error);
    return TRUE;
}
