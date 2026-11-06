#include "vm_text.h"
#include "common/frontend-base/video.h"
#include <string.h>

struct ntvdm_vm_text {
    uint8_t *committed, *staged;
    uint32_t bytes;
    uint16_t attribute, staged_attribute;
    BOOL snapshot, video_started, implicit;
    uint32_t configuration_revision;
    frontend_video video;
};

static ntvdm_vm_text_header *header(ntvdm_vm_text *text)
{ return (ntvdm_vm_text_header *)(text->staged ? text->staged : text->committed); }

static void cursor(const ntvdm_vm_text_header *h, console_io_state *state)
{
    uint32_t start = (h->shape >> 8) & 31, end = h->shape & 31;
    uint32_t lines = end >= start ? end - start + 1 : h->font_height - min(start, h->font_height) + end + 1;
    state->cursor_visible = !(h->shape & 0x2000);
    state->cursor_size = max(1u, min(100u, lines * 100 / h->font_height));
}

DWORD ntvdm_vm_text_begin(ntvdm_vm_text *text, BOOL implicit)
{
    if (!text) return ERROR_INVALID_PARAMETER;
    if (text->snapshot || text->staged) return ERROR_BUSY;
    text->staged = HeapAlloc(GetProcessHeap(), 0, text->bytes);
    if (!text->staged) return ERROR_NOT_ENOUGH_MEMORY;
    text->video_started = FALSE;
    text->implicit = implicit;
    text->staged_attribute = text->attribute;
    memcpy(text->staged, text->committed, text->bytes);
    return ERROR_SUCCESS;
}
unsigned ntvdm_vm_text_pending(const ntvdm_vm_text *text)
{ return text && text->staged ? (text->implicit ? 2u : 1u) : 0u; }
BOOL ntvdm_vm_text_ready(const ntvdm_vm_text *text)
{ return text && text->staged && (!text->video_started || text->video.pending_validated); }

DWORD ntvdm_vm_text_create(const void *bytes, uint32_t count, ntvdm_vm_text **output)
{
    ntvdm_vm_text_header h;
    ntvdm_vm_text *text;
    CPINFO cp;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output = NULL;
    if (!bytes || count < sizeof(h)) return ERROR_INVALID_DATA;
    memcpy(&h, bytes, sizeof(h));
    if (!h.columns || h.columns > 255 || !h.rows || h.rows > 256 ||
        h.x >= h.columns || h.y >= h.rows || h.page > 7 ||
        (h.mode > 3 && h.mode != 7) || !h.font_height || h.font_height > 32 ||
        h.shape > UINT16_MAX || h.codepage <= CP_THREAD_ACP ||
        count != sizeof(h) + h.columns * h.rows * 2)
        return ERROR_INVALID_DATA;
    if (!GetCPInfo(h.codepage, &cp)) return GetLastError();
    /* Multi-byte cell pairing needs its guest DBCS provider, not independent
     * byte conversion. Do not silently substitute the host OEM page. */
    if (cp.MaxCharSize != 1) return ERROR_NOT_SUPPORTED;
    text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*text));
    if (!text) return ERROR_NOT_ENOUGH_MEMORY;
    text->committed = HeapAlloc(GetProcessHeap(), 0, count);
    if (!text->committed) { HeapFree(GetProcessHeap(), 0, text); return ERROR_NOT_ENOUGH_MEMORY; }
    memcpy(text->committed, bytes, count); text->bytes = count;
    /* BIOS cells carry attributes, but there is no BIOS-wide default for a
     * native Console writer. Seed that frontend-owned value once; subsequent
     * cursor movement must not replace SetConsoleTextAttribute's value. */
    text->attribute = text->committed[sizeof(h) + 2 * (h.y * h.columns + h.x) + 1];
    *output = text; return ERROR_SUCCESS;
}

void ntvdm_vm_text_destroy(ntvdm_vm_text *text)
{
    if (!text) return;
    if (text->staged) HeapFree(GetProcessHeap(), 0, text->staged);
    frontend_video_dispose(&text->video);
    HeapFree(GetProcessHeap(), 0, text->committed);
    HeapFree(GetProcessHeap(), 0, text);
}

static DWORD cells(ntvdm_vm_text *text, const console_io_request *request, console_io_reply *reply)
{
    ntvdm_vm_text_header *h = header(text);
    const console_io_state *s = &request->state;
    BOOL write = request->operation == CONSOLE_IO_WRITE_CELLS_W || request->operation == CONSOLE_IO_WRITE_CELLS_A;
    BOOL wide = request->operation == CONSOLE_IO_WRITE_CELLS_W || request->operation == CONSOLE_IO_READ_CELLS_W;
    uint32_t count, row, column, index = 0;
    uint8_t *data = (uint8_t *)(h + 1);
    if (s->left < 0 || s->top < 0 || s->right < s->left || s->bottom < s->top ||
        (uint32_t)s->right >= h->columns || (uint32_t)s->bottom >= h->rows ||
        s->width != s->right - s->left + 1 || s->height != s->bottom - s->top + 1)
        return ERROR_INVALID_PARAMETER;
    count = (uint32_t)s->width * (uint32_t)s->height;
    if (count > CONSOLE_IO_TILE_BYTES / sizeof(console_io_cell)) return ERROR_INVALID_PARAMETER;
    if (write && (!text->staged || request->bytes != count * sizeof(console_io_cell) || !request->data))
        return ERROR_INVALID_STATE;
    if (!write && !reply->data) return ERROR_INVALID_PARAMETER;
    for (row = (uint32_t)s->top; row <= (uint32_t)s->bottom; ++row)
        for (column = (uint32_t)s->left; column <= (uint32_t)s->right; ++column, ++index) {
            uint8_t *cell = data + 2 * (row * h->columns + column);
            console_io_cell wire;
            WCHAR character;
            if (write) {
                BOOL substituted = FALSE;
                memcpy(&wire, request->data + index * sizeof(wire), sizeof(wire));
                if (wire.attribute > 255) return ERROR_INVALID_DATA;
                character = (WCHAR)wire.character;
                if (!wide) cell[0] = (BYTE)wire.character;
                else if (WideCharToMultiByte(h->codepage, WC_NO_BEST_FIT_CHARS,
                        &character, 1, (char *)cell, 1, NULL, &substituted) != 1 || substituted)
                    return ERROR_NO_UNICODE_TRANSLATION;
                cell[1] = (uint8_t)wire.attribute;
            } else {
                character = *cell;
                if (wide && MultiByteToWideChar(h->codepage, 0, (const char *)cell, 1, &character, 1) != 1)
                    return ERROR_NO_UNICODE_TRANSLATION;
                wire.character = (uint16_t)character; wire.attribute = cell[1];
                memcpy(reply->data + index * sizeof(wire), &wire, sizeof(wire));
            }
        }
    reply->state = request->state;
    reply->bytes = write ? 0 : count * sizeof(console_io_cell);
    return ERROR_SUCCESS;
}

DWORD ntvdm_vm_text_dispatch(ntvdm_vm_text *text, const console_io_request *request, console_io_reply *reply)
{
    ntvdm_vm_text_header *h;
    if (!text || !request || !reply) return ERROR_INVALID_PARAMETER;
    h = header(text);
    switch (request->operation) {
    case CONSOLE_IO_PREPARE_TEXT_REGION:
        if (text->snapshot || text->staged) return ERROR_BUSY;
        if (request->state.width <= 0 || request->state.height <= 0)
            return ERROR_INVALID_PARAMETER;
        /* The copied BIOS page already has a zero-origin full-grid viewport.
         * A different size needs the guest mode owner, not a host-only resize. */
        return request->state.width == (int32_t)h->columns && request->state.height == (int32_t)h->rows ?
            ERROR_SUCCESS : ERROR_NOT_SUPPORTED;
    case CONSOLE_IO_BARRIER:
        /* Every completed application is synchronous. An unfinished explicit
         * batch or split VIDEO packet is not an acknowledged publication. */
        return text->staged ? ERROR_BUSY : ERROR_SUCCESS;
    case CONSOLE_IO_CODE_PAGE:
        reply->state.count = h->codepage; return ERROR_SUCCESS;
    case CONSOLE_IO_WINDOW_QUERY:
        if (request->state.mode != CONSOLE_WINDOW_TEXT_FRAME_REQUIRED) return ERROR_NOT_SUPPORTED;
        reply->state.left = TRUE; return ERROR_SUCCESS;
    case CONSOLE_IO_SNAPSHOT_BEGIN:
        if (text->snapshot || text->staged) return ERROR_BUSY;
        text->snapshot = TRUE; return ERROR_SUCCESS;
    case CONSOLE_IO_SNAPSHOT_END:
        if (!text->snapshot) return ERROR_INVALID_STATE;
        text->snapshot = FALSE; return ERROR_SUCCESS;
    case CONSOLE_IO_PUBLICATION_BEGIN:
        return ntvdm_vm_text_begin(text, FALSE);
    case CONSOLE_IO_PUBLICATION_ABORT:
        if (!text->staged) return ERROR_INVALID_STATE;
        return ntvdm_vm_text_finish(text, ERROR_OPERATION_ABORTED) == ERROR_OPERATION_ABORTED ?
            ERROR_SUCCESS : ERROR_INVALID_STATE;
    case CONSOLE_IO_VIDEO_BEGIN: {
        console_video_description description;
        DWORD error;
        if (!text->staged) return ERROR_INVALID_STATE;
        if (text->configuration_revision == UINT32_MAX) return ERROR_ARITHMETIC_OVERFLOW;
        if (!request->data || request->bytes != sizeof(description)) return ERROR_INVALID_DATA;
        memcpy(&description, request->data, sizeof(description));
        /* A text publication cannot switch this VM into a graphics mode.
         * Geometry changes require the guest geometry owner, not this cache. */
        if (description.kind != CONSOLE_VIDEO_TEXT_FRAME ||
            description.width != h->columns || description.height != h->rows)
            return ERROR_NOT_SUPPORTED;
        error = frontend_video_begin(&text->video, request->state.mode, &description);
        if (!error) text->video_started = TRUE;
        return error;
    }
    case CONSOLE_IO_VIDEO_DATA:
        if (!text->staged || !text->video_started) return ERROR_INVALID_STATE;
        return frontend_video_stage_data(&text->video, request->state.mode,
            request->state.count, request->data, request->bytes);
    case CONSOLE_IO_READ_TEXT_CONFIGURATION: {
        console_text_configuration configuration;
        uint32_t offset = request->state.count, revision = request->state.mode;
        uint32_t count;
        if (offset >= sizeof(configuration) || (!revision && offset) || !reply->data)
            return ERROR_INVALID_PARAMETER;
        if (text->staged) return ERROR_BUSY;
        if (!text->configuration_revision || !text->video.pixels ||
            text->video.description.kind != CONSOLE_VIDEO_TEXT_FRAME)
            return ERROR_NOT_FOUND;
        if (revision && revision != text->configuration_revision) return ERROR_RETRY;
        /* Derive the wire view from the guest-acknowledged frame. No second
         * independently maintained font/palette state is introduced. */
        memcpy(&configuration.style, text->video.pixels, sizeof(configuration.style));
        memcpy(configuration.palette, text->video.description.palette, sizeof(configuration.palette));
        count = min((uint32_t)sizeof(configuration) - offset, CONSOLE_IO_TILE_BYTES);
        memcpy(reply->data, (const BYTE *)&configuration + offset, count);
        reply->bytes = count;
        reply->state.count = sizeof(configuration);
        reply->state.mode = text->configuration_revision;
        return ERROR_SUCCESS;
    }
    case CONSOLE_IO_PRESENT_BARRIER:
        if (!request->state.mode) return ERROR_INVALID_DATA;
        if (text->staged) return ERROR_BUSY;
        return text->video.published_serial == request->state.mode ?
            ERROR_SUCCESS : ERROR_NOT_FOUND;
    case CONSOLE_IO_SCREEN_INFO:
        ZeroMemory(&reply->state, sizeof(reply->state));
        reply->state.width = h->columns; reply->state.height = h->rows;
        reply->state.max_width = h->columns; reply->state.max_height = h->rows;
        reply->state.x = h->x; reply->state.y = h->y;
        reply->state.right = h->columns - 1; reply->state.bottom = h->rows - 1;
        reply->state.attribute = text->staged ? text->staged_attribute : text->attribute;
        return ERROR_SUCCESS;
    case CONSOLE_IO_ATTRIBUTE:
        if (!text->staged) return ERROR_INVALID_STATE;
        if (request->state.attribute > UINT8_MAX) return ERROR_NOT_SUPPORTED;
        text->staged_attribute = (uint16_t)request->state.attribute;
        return ERROR_SUCCESS;
    case CONSOLE_IO_WINDOW_RECT: {
        const console_io_state *s = &request->state;
        if (!text->staged) return ERROR_INVALID_STATE;
        if (s->mode > 1) return ERROR_INVALID_PARAMETER;
        /* This guest snapshot has one physical viewport and no scrollback.
         * Absolute full-grid and relative zero-delta requests preserve it;
         * other viewports require an actual guest geometry provider. */
        if (s->left || s->top ||
            s->right != (s->mode ? (int32_t)h->columns - 1 : 0) ||
            s->bottom != (s->mode ? (int32_t)h->rows - 1 : 0))
            return ERROR_NOT_SUPPORTED;
        return ERROR_SUCCESS;
    }
    case CONSOLE_IO_GET_CURSOR_INFO:
        cursor(h, &reply->state); return ERROR_SUCCESS;
    case CONSOLE_IO_CURSOR_POSITION:
        if (!text->staged) return ERROR_INVALID_STATE;
        if (request->state.x < 0 || request->state.y < 0 ||
            (uint32_t)request->state.x >= h->columns || (uint32_t)request->state.y >= h->rows)
            return ERROR_INVALID_PARAMETER;
        h->x = request->state.x; h->y = request->state.y; return ERROR_SUCCESS;
    case CONSOLE_IO_CURSOR_INFO: {
        console_io_state previous = {0};
        uint32_t lines;
        if (!text->staged) return ERROR_INVALID_STATE;
        if (!request->state.cursor_size || request->state.cursor_size > 100 || request->state.cursor_visible > 1)
            return ERROR_INVALID_PARAMETER;
        cursor(h, &previous);
        if (previous.cursor_size == request->state.cursor_size &&
            previous.cursor_visible == request->state.cursor_visible) return ERROR_SUCCESS;
        lines = (request->state.cursor_size * h->font_height + 99) / 100;
        h->shape = ((h->font_height - lines) << 8) | (h->font_height - 1);
        if (!request->state.cursor_visible) h->shape |= 0x2000;
        return ERROR_SUCCESS;
    }
    case CONSOLE_IO_READ_CELLS_W: case CONSOLE_IO_WRITE_CELLS_W:
    case CONSOLE_IO_READ_CELLS_A: case CONSOLE_IO_WRITE_CELLS_A:
        return cells(text, request, reply);
    default: return ERROR_NOT_SUPPORTED;
    }
}

DWORD ntvdm_vm_text_candidate(ntvdm_vm_text *text, ntvdm_vm_text_publication *view)
{
    if (!text || !view) return ERROR_INVALID_PARAMETER;
    ZeroMemory(view, sizeof(*view));
    if (!text->staged) return ERROR_INVALID_STATE;
    if (text->video_started) {
        if (!text->video.pending || !text->video.pending_validated)
            return ERROR_INVALID_STATE;
        view->video = &text->video.pending_description;
        view->video_bytes = text->video.pending;
        view->video_serial = text->video.pending_serial;
    }
    view->text = text->staged; view->text_bytes = text->bytes;
    return ERROR_SUCCESS;
}

DWORD ntvdm_vm_text_copy_application(ntvdm_vm_text *text, void **bytes, uint32_t *size)
{
    ntvdm_vm_text_publication view;
    ntvdm_vm_video_application trailer;
    uint32_t total;
    BYTE *copy;
    DWORD error;
    if (bytes) *bytes = NULL;
    if (size) *size = 0;
    if (!bytes || !size) return ERROR_INVALID_PARAMETER;
    error = ntvdm_vm_text_candidate(text, &view);
    if (error) return error;
    total = view.text_bytes;
    if (view.video) {
        if (total > CONSOLE_IO_MAX_DATA_BYTES - sizeof(trailer) ||
            view.video->bytes > CONSOLE_IO_MAX_DATA_BYTES - sizeof(trailer) - total)
            return ERROR_BUFFER_OVERFLOW;
        total += sizeof(trailer) + view.video->bytes;
    }
    copy = HeapAlloc(GetProcessHeap(), 0, total);
    if (!copy) return ERROR_NOT_ENOUGH_MEMORY;
    memcpy(copy, view.text, view.text_bytes);
    if (view.video) {
        trailer.signature = NTVDM_VM_VIDEO_SIGNATURE;
        trailer.version = NTVDM_VM_VIDEO_VERSION;
        trailer.description = *view.video;
        memcpy(copy + view.text_bytes, &trailer, sizeof(trailer));
        memcpy(copy + view.text_bytes + sizeof(trailer), view.video_bytes, view.video->bytes);
    }
    *bytes = copy; *size = total;
    return ERROR_SUCCESS;
}

DWORD ntvdm_vm_text_finish(ntvdm_vm_text *text, DWORD application_error)
{
    if (!text || !text->staged) return ERROR_INVALID_STATE;
    if (!application_error && text->video_started) {
        DWORD error = frontend_video_commit_pending(&text->video);
        if (error) return error; /* Incomplete or rejected frame cannot commit cells. */
        {
            ntvdm_vm_text_header *h = (ntvdm_vm_text_header *)text->staged;
            const console_text_style *style = (const console_text_style *)text->video.pixels;
            const BYTE *cells = text->video.pixels + sizeof(*style);
            BYTE *destination = (BYTE *)(h + 1);
            uint32_t i, stride = text->video.description.stride / h->columns;
            /* The guest application uses VIDEO cells/cursor when present.
             * Project those acknowledged values, not the older BIOS snapshot,
             * into the next carrier's screen seed. */
            for (i = 0; i < h->columns * h->rows; ++i)
                memcpy(destination + i * 2, cells + i * stride, 2);
            h->x = style->cursor_column; h->y = style->cursor_row;
            h->shape = style->cursor_visible && style->cursor_height ?
                ((uint32_t)style->cursor_start << 8) |
                (uint32_t)(style->cursor_start + style->cursor_height - 1) : 0x2000;
            text->configuration_revision = max(text->configuration_revision + 1,
                text->video.published_serial);
        }
    }
    if (!application_error) {
        HeapFree(GetProcessHeap(), 0, text->committed);
        text->committed = text->staged;
        text->attribute = text->staged_attribute;
    } else HeapFree(GetProcessHeap(), 0, text->staged);
    frontend_video_abort_pending(&text->video);
    text->video_started = FALSE;
    text->implicit = FALSE;
    text->staged = NULL;
    return application_error;
}

void ntvdm_vm_text_disconnect(ntvdm_vm_text *text)
{
    if (!text) return;
    if (text->staged) ntvdm_vm_text_finish(text, ERROR_OPERATION_ABORTED);
    text->snapshot = FALSE;
    /* Frame serials belong to one channel, while acknowledged font/cells
     * survive handoff. A new carrier may legitimately start again at 1. */
    text->video.serial = text->video.published_serial = 0;
}
