#include "session/session.h"

static uint32_t teardown_order[2];
static uint32_t teardown_count;

static void record_teardown(void *context)
{
    teardown_order[teardown_count++] = *(const uint32_t *)context;
}

int main(void)
{
    session first;
    session second;
    const uint32_t one = 1u;
    const uint32_t two = 2u;
    uint8_t *text_writer;
    uint8_t text_copy[8];
    uint8_t graphics_copy[16];
    uint32_t palette[2] = { UINT32_C(0x00112233), UINT32_C(0x00445566) };
    uint32_t palette_copy[2];
    uint32_t text_columns;
    uint32_t text_rows;
    uint32_t text_bytes;

    session_initialize(&first, 101u);
    session_initialize(&second, 202u);
    if (!session_valid(&first) || !session_valid(&second) ||
        session_activate(&first) == 0 || session_activate(&second) == 0)
        return 1;
    if (session_thread_bind(&first) == 0 || session_thread_current() != &first ||
        session_thread_bind(&second) != 0 || session_thread_unbind(&second) != 0 ||
        session_dispose(&first) != 0 || session_thread_unbind(&first) == 0)
        return 2;
    if (session_presentation_text_acquire_writable(&first, 2u, 2u,
            &text_writer) == 0 || text_writer == NULL)
        return 7;
    text_writer[0] = 'N';
    text_writer[1] = 7u;
    if (session_presentation_text_snapshot(&first, text_copy,
            (uint32_t)sizeof(text_copy), &text_columns, &text_rows,
            &text_bytes) == 0 || text_columns != 2u || text_rows != 2u ||
        text_bytes != sizeof(text_copy) || text_copy[0] != 'N' ||
        text_copy[1] != 7u)
        return 8;
    session_presentation_text_clear(&first);
    if (session_presentation_text_snapshot(&first, text_copy,
            (uint32_t)sizeof(text_copy), NULL, NULL, NULL) != 0)
        return 9;
    if (session_presentation_graphics_acquire_writable(&first, 2u, 2u, 8u,
            4u, &text_writer) == 0 || text_writer == NULL)
        return 10;
    text_writer[0] = 0x5au;
    if (session_presentation_graphics_snapshot(&first, graphics_copy,
            (uint32_t)sizeof(graphics_copy), NULL, NULL, NULL, NULL,
            &text_bytes) == 0 || text_bytes != 8u || graphics_copy[0] != 0x5au)
        return 11;
    if (!session_presentation_graphics_set_palette(&first, palette, 2u) ||
        !session_presentation_graphics_palette_snapshot(&first, palette_copy,
            2u, &text_bytes) || text_bytes != 2u ||
        palette_copy[1] != palette[1]) return 12;
    session_presentation_graphics_clear(&first);
    if (session_presentation_graphics_snapshot(&first, graphics_copy,
            (uint32_t)sizeof(graphics_copy), NULL, NULL, NULL, NULL,
            NULL) != 0 || session_presentation_graphics_palette_snapshot(&first,
            palette_copy, 2u, &text_bytes) != 0)
        return 13;
    if (session_register_teardown(&first, record_teardown, (void *)&one) == 0 ||
        session_register_teardown(&first, record_teardown, (void *)&two) == 0 ||
        session_register_teardown(&first, record_teardown, (void *)&one) == 0 ||
        first.teardown_count != 2u)
        return 3;
    session_complete(&second, 37u);
    if (second.state != SESSION_STATE_COMPLETED || second.completion_code != 37u ||
        session_request_cancellation(&second, SESSION_CANCELLATION_REQUESTED) != 0)
        return 4;
    if (session_request_cancellation(&first, SESSION_CANCELLATION_REQUESTED) == 0 ||
        first.state != SESSION_STATE_CANCELLED ||
        first.cancellation_reason != SESSION_CANCELLATION_REQUESTED ||
        session_register_teardown(&first, record_teardown, (void *)&one) != 0)
        return 5;
    if (session_dispose(&first) == 0 || session_valid(&first) ||
        teardown_count != 2u || teardown_order[0] != 2u || teardown_order[1] != 1u ||
        session_dispose(&second) == 0)
        return 6;
    return 0;
}
