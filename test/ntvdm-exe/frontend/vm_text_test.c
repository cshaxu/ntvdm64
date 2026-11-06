#include "ntvdm-exe/frontend/vm_text.h"
#include <stdio.h>
#include <string.h>

static unsigned checks;
#define CHECK(x) do { ++checks; if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static int publication_video(void)
{
    struct { ntvdm_vm_text_header h; uint8_t cells[2]; } initial = {
        {1, 1, 0, 0, 0x0607, 3, 0, 437, 16}, {'A', 7}
    };
    struct { console_text_style style; uint8_t cells[2]; } frame = {0};
    console_video_description description = {0};
    console_io_request q = {0};
    console_io_reply r = {0};
    ntvdm_vm_text_publication view;
    void *application = NULL;
    uint32_t application_bytes;
    ntvdm_vm_video_application trailer;
    console_text_configuration configuration;
    BYTE tile[CONSOLE_IO_TILE_BYTES];
    ntvdm_vm_text *text = NULL;
    frame.style.font_height = 16;
    frame.style.fonts[0][65][0] = 0x7e;
    frame.cells[0] = 'A'; frame.cells[1] = 7;
    description.kind = CONSOLE_VIDEO_TEXT_FRAME;
    description.width = description.height = 1; description.stride = 2;
    description.bytes = sizeof(console_text_style) + 2;
    description.palette[7] = 0xaaaaaa;
    CHECK(!ntvdm_vm_text_create(&initial, sizeof(initial.h) + 2, &text));
    q.operation = CONSOLE_IO_READ_TEXT_CONFIGURATION; r.data = tile;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_NOT_FOUND);
    q.operation = CONSOLE_IO_VIDEO_BEGIN; q.state.mode = 1;
    q.bytes = sizeof(description); q.data = (uint8_t *)&description;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_INVALID_STATE);
    q.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    q.operation = CONSOLE_IO_VIDEO_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    CHECK(ntvdm_vm_text_candidate(text, &view) == ERROR_INVALID_STATE);
    CHECK(ntvdm_vm_text_finish(text, 0) == ERROR_INVALID_STATE);
    CHECK(ntvdm_vm_text_copy_application(text, &application, &application_bytes) == ERROR_INVALID_STATE);
    CHECK(!application && !application_bytes);
    q.operation = CONSOLE_IO_VIDEO_DATA; q.bytes = sizeof(console_text_style);
    q.data = (uint8_t *)&frame;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    CHECK(ntvdm_vm_text_candidate(text, &view) == ERROR_INVALID_STATE);
    q.state.count = sizeof(console_text_style); q.bytes = 2; q.data = frame.cells;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    CHECK(!ntvdm_vm_text_candidate(text, &view));
    CHECK(view.video_serial == 1 && view.video->palette[7] == 0xaaaaaa &&
        ((const console_text_style *)view.video_bytes)->fonts[0][65][0] == 0x7e);
    CHECK(!ntvdm_vm_text_copy_application(text, &application, &application_bytes));
    CHECK(application_bytes == sizeof(initial.h) + 2 + sizeof(trailer) + description.bytes);
    memcpy(&trailer, (uint8_t *)application + sizeof(initial.h) + 2, sizeof(trailer));
    CHECK(trailer.signature == NTVDM_VM_VIDEO_SIGNATURE &&
        trailer.version == NTVDM_VM_VIDEO_VERSION &&
        !memcmp(&trailer.description, &description, sizeof(description)));
    CHECK(!memcmp((uint8_t *)application + sizeof(initial.h) + 2 + sizeof(trailer),
        &frame, description.bytes));
    frame.style.fonts[0][65][0] = 0; description.palette[7] = 0;
    CHECK(((const console_text_style *)view.video_bytes)->fonts[0][65][0] == 0x7e &&
        view.video->palette[7] == 0xaaaaaa); /* Owned copies, not publisher buffers. */
    q.operation = CONSOLE_IO_PRESENT_BARRIER;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_BUSY);
    CHECK(ntvdm_vm_text_finish(text, ERROR_WRITE_FAULT) == ERROR_WRITE_FAULT);
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_NOT_FOUND);
    CHECK(((uint8_t *)application)[sizeof(initial.h)] == 'A');
    CHECK(((uint8_t *)application)[sizeof(initial.h) + 2 + sizeof(trailer) +
        offsetof(console_text_style, fonts) + 65 * 32] == 0x7e);
    HeapFree(GetProcessHeap(), 0, application); application = NULL;
    q.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    q.operation = CONSOLE_IO_VIDEO_BEGIN; q.state.mode = 2;
    q.data = (uint8_t *)&description; q.bytes = sizeof(description);
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    q.operation = CONSOLE_IO_VIDEO_DATA; q.state.count = 0;
    q.data = (uint8_t *)&frame; q.bytes = description.bytes;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    CHECK(!ntvdm_vm_text_candidate(text, &view));
    CHECK(!ntvdm_vm_text_finish(text, 0)); /* Application owner's success only. */
    {
        uint32_t offset = 0, revision = 0;
        q.operation = CONSOLE_IO_SNAPSHOT_BEGIN;
        CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
        while (offset < sizeof(configuration)) {
            q.operation = CONSOLE_IO_READ_TEXT_CONFIGURATION;
            q.state.count = offset; q.state.mode = revision;
            CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
            CHECK(r.state.mode == 2 && r.state.count == sizeof(configuration));
            CHECK(r.bytes == min(sizeof(configuration) - offset, CONSOLE_IO_TILE_BYTES));
            memcpy((BYTE *)&configuration + offset, tile, r.bytes);
            offset += r.bytes; revision = r.state.mode;
        }
        CHECK(!memcmp(&configuration.style, &frame.style, sizeof(frame.style)));
        CHECK(!memcmp(configuration.palette, description.palette, sizeof(configuration.palette)));
        q.state.count = 0; q.state.mode = 1;
        CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_RETRY);
        q.state.count = sizeof(configuration); q.state.mode = 2;
        CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_INVALID_PARAMETER);
        q.state.count = 1; q.state.mode = 0;
        CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_INVALID_PARAMETER);
        q.operation = CONSOLE_IO_SNAPSHOT_END;
        CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
        q.state.mode = 2; q.state.count = 0;
    }
    q.operation = CONSOLE_IO_PRESENT_BARRIER;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    q.state.mode = 1;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_NOT_FOUND);
    q.state.mode = 3;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_NOT_FOUND);
    q.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    q.operation = CONSOLE_IO_VIDEO_BEGIN; q.data = (uint8_t *)&description;
    q.bytes = sizeof(description);
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    q.operation = CONSOLE_IO_VIDEO_DATA; q.state.count = 1;
    q.data = (uint8_t *)&frame; q.bytes = description.bytes;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_INVALID_DATA);
    CHECK(ntvdm_vm_text_candidate(text, &view) == ERROR_INVALID_STATE);
    CHECK(ntvdm_vm_text_finish(text, 0) == ERROR_INVALID_STATE);
    q.operation = CONSOLE_IO_PUBLICATION_ABORT;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    q.operation = CONSOLE_IO_PRESENT_BARRIER; q.state.mode = 2;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r)); /* Failed batch preserves last applied serial. */
    ntvdm_vm_text_disconnect(text);
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_NOT_FOUND);
    q.operation = CONSOLE_IO_READ_TEXT_CONFIGURATION; q.state.count = 0; q.state.mode = 2;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r) && r.state.mode == 2);
    CHECK(!ntvdm_vm_text_begin(text, TRUE) && ntvdm_vm_text_pending(text) == 2);
    frame.cells[0] = 'Z';
    q.operation = CONSOLE_IO_VIDEO_BEGIN; q.state.mode = 1;
    q.data = (BYTE *)&description; q.bytes = sizeof(description);
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    CHECK(!ntvdm_vm_text_ready(text));
    q.operation = CONSOLE_IO_VIDEO_DATA; q.state.count = 0;
    q.data = (BYTE *)&frame; q.bytes = description.bytes;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r) && ntvdm_vm_text_ready(text));
    CHECK(!ntvdm_vm_text_finish(text, 0) && !ntvdm_vm_text_pending(text));
    q.operation = CONSOLE_IO_PRESENT_BARRIER;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    q.operation = CONSOLE_IO_READ_TEXT_CONFIGURATION; q.state.mode = 2;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_RETRY);
    q.state.mode = 0;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r) && r.state.mode == 3);
    ZeroMemory(&q, sizeof(q));
    q.operation = CONSOLE_IO_READ_CELLS_A; q.state.width = q.state.height = 1;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r));
    CHECK(((const console_io_cell *)r.data)->character == 'Z');
    ntvdm_vm_text_destroy(text);
    return 0;
}

static int fill_transaction(void)
{
    struct { ntvdm_vm_text_header h; uint8_t cells[12]; } initial = {
        {3, 2, 1, 1, 0x0607, 3, 0, 437, 16},
        {'A', 1, 'B', 2, 'C', 3, 'D', 4, 'E', 5, 'F', 6}
    };
    const uint8_t expected[] = {'A', 1, 'B', 2, 'Z', 3, 'Z', 4, 'Z', 5, 'F', 6};
    ntvdm_vm_text *text = NULL;
    ntvdm_vm_text_publication view;
    console_io_request q = {0};
    console_io_reply r = {0};
    const ntvdm_vm_text_header *h;
    const uint8_t *cells;
    CHECK(!ntvdm_vm_text_create(&initial, sizeof(initial), &text));
    q.operation = CONSOLE_IO_FILL_CHARACTER;
    q.state.x = 2; q.state.count = 3; q.state.character = 'Z';
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_INVALID_STATE);
    CHECK(!ntvdm_vm_text_begin(text, FALSE));
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r) && r.state.count == 3);
    CHECK(!ntvdm_vm_text_candidate(text, &view));
    h = view.text; cells = (const uint8_t *)(h + 1);
    CHECK(!memcmp(cells, expected, sizeof(expected)));
    CHECK(h->x == 1 && h->y == 1);
    CHECK(ntvdm_vm_text_finish(text, ERROR_OPERATION_ABORTED) == ERROR_OPERATION_ABORTED);
    CHECK(!ntvdm_vm_text_begin(text, FALSE));
    CHECK(!ntvdm_vm_text_candidate(text, &view));
    CHECK(!memcmp((const uint8_t *)view.text + sizeof(initial.h), initial.cells, sizeof(initial.cells)));
    q.operation = CONSOLE_IO_FILL_ATTRIBUTE;
    q.state.x = 1; q.state.y = 1; q.state.count = UINT32_MAX; q.state.attribute = 0x1e;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r) && r.state.count == 2);
    CHECK(!ntvdm_vm_text_candidate(text, &view));
    cells = (const uint8_t *)view.text + sizeof(initial.h);
    CHECK(!memcmp(cells, initial.cells, 8));
    CHECK(cells[8] == 'E' && cells[9] == 0x1e && cells[10] == 'F' && cells[11] == 0x1e);
    q.state.count = 0; q.state.attribute = 7;
    CHECK(!ntvdm_vm_text_dispatch(text, &q, &r) && !r.state.count);
    q.state.count = 1; q.state.attribute = 0x100;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_NOT_SUPPORTED);
    q.state.attribute = 7; q.state.x = 3;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_INVALID_PARAMETER);
    q.state.x = -1;
    CHECK(ntvdm_vm_text_dispatch(text, &q, &r) == ERROR_INVALID_PARAMETER);
    CHECK(!ntvdm_vm_text_finish(text, 0));
    CHECK(!ntvdm_vm_text_begin(text, FALSE));
    CHECK(!ntvdm_vm_text_candidate(text, &view));
    cells = (const uint8_t *)view.text + sizeof(initial.h);
    CHECK(cells[9] == 0x1e && cells[11] == 0x1e);
    ntvdm_vm_text_destroy(text);
    return 0;
}

int main(void)
{
    struct { ntvdm_vm_text_header h; uint8_t cells[8]; } frame = {
        {2, 2, 0, 0, 0x0607, 3, 0, 437, 16},
        {'A', 7, 'B', 7, 'C', 7, 'D', 7}
    };
    ntvdm_vm_text *text = NULL;
    console_io_cell cell = {'Z', 0x2f}, result[4];
    console_io_request request = {0};
    console_io_reply reply = {0};
    ntvdm_vm_text_publication candidate;
    void *application;
    uint32_t application_bytes;
    CHECK(!publication_video());
    CHECK(!fill_transaction());
    CHECK(ntvdm_vm_text_create(&frame, sizeof(frame) - 1, &text) == ERROR_INVALID_DATA);
    CHECK(!text);
    CHECK(!ntvdm_vm_text_create(&frame, sizeof(frame), &text));
    frame.cells[0] = 'X'; /* Creation owns its copy. */
    request.operation = CONSOLE_IO_READ_CELLS_W;
    request.state.width = request.state.height = 1;
    reply.data = (uint8_t *)result;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(result[0].character == 'A' && result[0].attribute == 7);
    request.operation = CONSOLE_IO_SNAPSHOT_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_BUSY);
    request.operation = CONSOLE_IO_SNAPSHOT_END;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_BUSY);
    request.operation = CONSOLE_IO_WRITE_CELLS_W;
    request.data = (uint8_t *)&cell; request.bytes = sizeof(cell);
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(!ntvdm_vm_text_candidate(text, &candidate));
    CHECK(candidate.text_bytes == sizeof(frame) && !candidate.video && ((const uint8_t *)candidate.text)[sizeof(frame.h)] == 'Z');
    CHECK(!ntvdm_vm_text_copy_application(text, &application, &application_bytes));
    CHECK(application_bytes == candidate.text_bytes && !memcmp(application, candidate.text, application_bytes));
    HeapFree(GetProcessHeap(), 0, application);
    CHECK(ntvdm_vm_text_finish(text, ERROR_OPERATION_ABORTED) == ERROR_OPERATION_ABORTED);
    request.operation = CONSOLE_IO_READ_CELLS_W;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(result[0].character == 'A'); /* Failed guest application cannot commit. */
    request.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_WRITE_CELLS_W;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(!ntvdm_vm_text_finish(text, ERROR_SUCCESS));
    CHECK(ntvdm_vm_text_finish(text, ERROR_SUCCESS) == ERROR_INVALID_STATE);
    request.operation = CONSOLE_IO_READ_CELLS_W;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(result[0].character == 'Z' && result[0].attribute == 0x2f);
    /* Follow NTVWM's publication order: cell writes, cursor, viewport,
     * cursor shape, then default attribute. Only guest application commits. */
    request.operation = CONSOLE_IO_SCREEN_INFO;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(reply.state.attribute == 7); /* A cell write is not a default change. */
    request.operation = CONSOLE_IO_ATTRIBUTE; request.state.attribute = 0x1e;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_INVALID_STATE);
    request.operation = CONSOLE_IO_WINDOW_RECT;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_INVALID_STATE);
    request.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_CURSOR_POSITION; request.state.x = 1;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_WINDOW_RECT;
    request.state.mode = 1; request.state.right = request.state.bottom = 1;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.state.mode = 0; request.state.right = request.state.bottom = 0;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.state.right = 1;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_NOT_SUPPORTED);
    request.state.mode = 1; request.state.right = 0;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_NOT_SUPPORTED);
    request.state.mode = 2;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_INVALID_PARAMETER);
    request.operation = CONSOLE_IO_CURSOR_INFO;
    request.state.cursor_visible = 1; request.state.cursor_size = 25;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_ATTRIBUTE;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.state.attribute = 256;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_NOT_SUPPORTED);
    request.operation = CONSOLE_IO_SCREEN_INFO;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(reply.state.attribute == 0x1e && reply.state.x == 1);
    CHECK(ntvdm_vm_text_finish(text, ERROR_OPERATION_ABORTED) == ERROR_OPERATION_ABORTED);
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(reply.state.attribute == 7 && reply.state.x == 0);
    request.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_ATTRIBUTE; request.state.attribute = 0x1e;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(!ntvdm_vm_text_finish(text, ERROR_SUCCESS));
    request.operation = CONSOLE_IO_SCREEN_INFO;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(reply.state.attribute == 0x1e);
    request.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_ATTRIBUTE; request.state.attribute = 0x70;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_PUBLICATION_ABORT;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_SCREEN_INFO;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(reply.state.attribute == 0x1e);
    request.operation = CONSOLE_IO_READ_CELLS_W;
    request.state.bottom = 0;
    request.state.right = 2; request.state.width = 3;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_INVALID_PARAMETER);
    request.operation = CONSOLE_IO_BARRIER;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_PREPARE_TEXT_REGION;
    request.state.width = frame.h.columns; request.state.height = frame.h.rows;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    ++request.state.width;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_NOT_SUPPORTED);
    request.state.width = 1; request.state.height = 1;
    request.operation = CONSOLE_IO_CODE_PAGE;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply) && reply.state.count == 437);
    request.operation = CONSOLE_IO_WINDOW_QUERY;
    request.state.mode = CONSOLE_WINDOW_TEXT_FRAME_REQUIRED;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply) && reply.state.left);
    request.state.mode = 0;
    request.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_BARRIER;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_BUSY);
    request.operation = CONSOLE_IO_WRITE_CELLS_W;
    request.state.right = 0; request.state.width = 1;
    cell.character = 0x4e00;
    CHECK(ntvdm_vm_text_dispatch(text, &request, &reply) == ERROR_NO_UNICODE_TRANSLATION);
    request.operation = CONSOLE_IO_PUBLICATION_ABORT;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_ATTRIBUTE; request.state.attribute = 0x70;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    ntvdm_vm_text_disconnect(text);
    request.operation = CONSOLE_IO_SCREEN_INFO;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    CHECK(reply.state.attribute == 0x1e);
    request.operation = CONSOLE_IO_SNAPSHOT_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    ntvdm_vm_text_disconnect(text);
    ntvdm_vm_text_disconnect(text);
    request.operation = CONSOLE_IO_PUBLICATION_BEGIN;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    request.operation = CONSOLE_IO_PUBLICATION_ABORT;
    CHECK(!ntvdm_vm_text_dispatch(text, &request, &reply));
    ntvdm_vm_text_destroy(text);
    frame.h.codepage = CP_OEMCP;
    CHECK(ntvdm_vm_text_create(&frame, sizeof(frame), &text) == ERROR_INVALID_DATA);
    CHECK(!text);
    frame.h.codepage = 932;
    CHECK(ntvdm_vm_text_create(&frame, sizeof(frame), &text) == ERROR_NOT_SUPPORTED);
    CHECK(!text);
    printf("PASS %u VM text ownership/publication assertions\n", checks);
    return 0;
}
