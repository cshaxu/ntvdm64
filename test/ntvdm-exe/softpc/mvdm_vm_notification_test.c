#include "mvdm_vm_io.h"
#include "mvdm_vm_executor.h"
#include "ntvdm-exe/frontend/vm_text.h"
#include "mvdm_vm_notification.h"
#include "mvdm_vm_bridge.h"
#include "mvdm_vm_wake.h"
#include "mvdm_vm_continuation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char guest_memory[256];
static mvdm_vm_executor *input_executor;
/* Conversion itself is exercised by keycode_object_contract_fixture using
 * the actual original object. This substitute checks the ingress boundary. */
BOOL BiosKeyToInputRecord(PKEY_EVENT_RECORD key)
{
    if (key->wVirtualScanCode == 0x3b && !key->uChar.AsciiChar) {
        key->wVirtualKeyCode = VK_F1; key->wRepeatCount = 1;
        key->dwControlKeyState = 0; return TRUE;
    }
    if ((key->wVirtualScanCode != 0x1e && key->wVirtualScanCode != 0) || key->uChar.AsciiChar != 'a') return FALSE;
    key->wVirtualScanCode = 0x1e;
    key->wRepeatCount = 1; key->wVirtualKeyCode = 'A';
    key->uChar.UnicodeChar = L'a'; key->dwControlKeyState = 0;
    return TRUE;
}
HANDLE vm_io_reset_failure;
mvdm_vm_executor *mvdm_standalone_worker_executor(void) { return input_executor; }
/* Ingress selection/capture has its own tests; these isolate notification. */
DWORD mvdm_vm_ingress_register(uint32_t callback)
{ return callback ? ERROR_SUCCESS : ERROR_NOT_READY; }
void mvdm_vm_ingress_reset(void) {}
DWORD mvdm_vm_command_admit(uint32_t vm, uint32_t address, uint32_t bytes, uint64_t *token)
{ (void)vm; (void)address; (void)bytes; *token = 0; return ERROR_INVALID_DATA; }
int mvdm_softpc_guest_memory_copy_from(uint32_t address, uint8_t *bytes, uint32_t count)
{
    if (address > sizeof(guest_memory) || count > sizeof(guest_memory) - address) return 0;
    memcpy(bytes, guest_memory + address, count); return 1;
}
int mvdm_softpc_guest_memory_copy_to(uint32_t address, const uint8_t *bytes, uint32_t count)
{
    if (address > sizeof(guest_memory) || count > sizeof(guest_memory) - address) return 0;
    memcpy(guest_memory + address, bytes, count); return 1;
}

/* Unit boundary substitutes only: these do not prove real PIC/guest delivery. */
static void (*eoi)(int, int);
static HANDLE owner;
static HANDLE application_published;
static CRITICAL_SECTION ica;
static int depth, raises, cancels, busy;
static LONG assertions;
#define CHECK(x) do { InterlockedIncrement(&assertions); if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
void host_ica_lock(void) { EnterCriticalSection(&ica); ++depth; }
void host_ica_unlock(void) { CHECK(depth > 0); --depth; LeaveCriticalSection(&ica); }
WORD VDDReserveIrqLine(HANDLE h, WORD irq)
{
    CHECK(depth && irq == 0xffff && h);
    if (busy || owner) return 0xffff;
    owner = h;
    return 15;
}
BOOL VDDReleaseIrqLine(HANDLE h, WORD irq)
{
    CHECK(depth && owner == h && irq == 15);
    owner = NULL;
    return TRUE;
}
BOOL RegisterEOIHook(int irq, void (*proc)(int, int))
{
    CHECK(depth && irq == 15);
    if (eoi) return FALSE;
    eoi = proc;
    return TRUE;
}
BOOL RemoveEOIHook(int irq, void (*proc)(int, int))
{
    CHECK(depth && irq == 15 && eoi == proc);
    eoi = NULL;
    return TRUE;
}
void ica_hw_interrupt(ULONG adapter, ULONG line, LONG count)
{
    CHECK(depth && owner && adapter == 1 && line == 7 && count == 1);
    ++raises;
    if (application_published) SetEvent(application_published);
}
void ica_hw_interrupt_cancel(ULONG adapter, ULONG line)
{
    CHECK(depth && adapter == 1 && line == 7);
    ++cancels;
    eoi(15, -1);
}
static void occupied(int irq, int count) { (void)irq; (void)count; }
typedef struct producer_context {
    HANDLE published, resume;
    DWORD first_error, retired_error, reused_error;
} producer_context;
static DWORD WINAPI publish_from_host(void *argument)
{
    producer_context *p = argument;
    uint64_t serial;
    p->first_error = mvdm_vm_wake_publish(3, 30, &serial);
    SetEvent(p->published);
    if (WaitForSingleObject(p->resume, INFINITE) != WAIT_OBJECT_0)
        return ERROR_INVALID_HANDLE;
    p->retired_error = mvdm_vm_wake_publish(3, 30, &serial);
    p->reused_error = mvdm_vm_wake_publish(3, 31, &serial);
    return ERROR_SUCCESS;
}

typedef struct {
    HANDLE resume;
    mvdm_vm_continuation *request;
    DWORD error;
} completion_context;
static DWORD WINAPI complete_after_retirement(void *argument)
{
    completion_context *context = argument;
    const mvdm_vm_completion result = {0, 37, 0};
    CHECK(WaitForSingleObject(context->resume, INFINITE) == WAIT_OBJECT_0);
    context->error = mvdm_vm_continuation_complete(context->request, &result);
    mvdm_vm_continuation_release(context->request);
    return 0;
}
typedef struct io_wait_context {
    HANDLE ready;
    mvdm_vm_continuation *request;
    uint64_t ticket;
    DWORD error;
} io_wait_context;
static DWORD WINAPI finish_guest_io(void *argument)
{
    io_wait_context *context = argument;
    CHECK(WaitForSingleObject(context->ready, INFINITE) == WAIT_OBJECT_0);
    context->error = mvdm_vm_continuation_finish_io(context->request,
        context->ticket, 0);
    return 0;
}
typedef struct application_context {
    mvdm_vm_continuation *request;
    HANDLE cancellation[4];
    ntvdm_vm_text *text;
    mvdm_vm_io *io;
    DWORD error;
    BOOL implicit;
    console_io_request final_operation;
} application_context;
static DWORD WINAPI apply_from_channel(void *argument)
{
    application_context *context = argument;
    console_io_request request = {0};
    console_io_reply reply = {0};
    request.operation = context->implicit ? CONSOLE_IO_CURSOR_POSITION : CONSOLE_IO_PUBLICATION_END;
    if (context->implicit) request.state.x = 1;
    if (context->final_operation.operation) request = context->final_operation;
    ntvdm_vm_cancel cancel = {context->cancellation[0], context->cancellation[1],
        context->cancellation[2], context->cancellation[3]};
    frontend_base_result result = {0};
    context->error = mvdm_vm_io_operations.dispatch(context->io, &cancel, &request, &reply, &result);
    CHECK(result.reply && !result.terminal_error);
    if (!context->error) context->error = reply.error;
    CHECK((reply.result != 0) == !context->error && !reply.bytes);
    return 0;
}
static void provider_tests(mvdm_vm_continuation *continuation)
{
    struct { ntvdm_vm_text_header h; BYTE cells[4]; } frame = {
        {2, 1, 0, 0, 0x0607, 3, 0, 437, 16}, {'A', 7, 'B', 7}};
    ntvdm_vm_text *text;
    mvdm_vm_io *a, *b;
    KEY_EVENT_RECORD key = {0};
    console_io_request q = {0}; console_io_reply r = {0};
    frontend_base_result result = {0}; ntvdm_vm_cancel cancel = {0};
    BYTE data[CONSOLE_IO_TILE_BYTES]; BOOL pending;
    unsigned i; char title[] = "VM A";
    CHECK(!ntvdm_vm_text_create(&frame, sizeof(frame), &text));
    CHECK(!mvdm_vm_io_create(text, continuation, title, &a));
    CHECK(!mvdm_vm_io_create(text, continuation, "VM B", &b));
    title[3] = 'Z'; r.data = data;
    q.operation = CONSOLE_IO_GET_HANDOFF_TITLE_A;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
    CHECK(r.result && r.bytes == 5 && !strcmp((char *)data, "VM A"));
    q.operation = CONSOLE_IO_PUBLISH_TITLE_A; q.data = (BYTE *)title; q.bytes = sizeof(title);
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result) && r.result);
    title[3] = 'Y';
    --q.bytes; /* Missing terminator must not replace the copied title. */
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
    CHECK(!r.result && r.error == ERROR_INVALID_PARAMETER);
    q.operation = CONSOLE_IO_GET_HANDOFF_TITLE_A; q.data = NULL; q.bytes = 0;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
    CHECK(r.result && !strcmp((char *)data, "VM Z"));
    CHECK(!mvdm_vm_io_operations.dispatch(b, &cancel, &q, &r, &result));
    CHECK(r.result && !strcmp((char *)data, "VM B"));
    CHECK(WaitForSingleObject(mvdm_vm_io_ready(a), 0) == WAIT_TIMEOUT);
    key.bKeyDown = TRUE; key.wRepeatCount = 1; key.wVirtualKeyCode = 'A';
    key.wVirtualScanCode = 0x1e; key.uChar.UnicodeChar = 'a';
    CHECK(!mvdm_vm_io_key(a, &key));
    CHECK(WaitForSingleObject(mvdm_vm_io_ready(a), 0) == WAIT_OBJECT_0);
    CHECK(!mvdm_vm_io_operations.input_pending(b, &pending) && !pending);
    q.operation = CONSOLE_IO_PEEK_INPUT; q.state.count = 1;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
    CHECK(r.result && r.bytes == sizeof(console_io_input));
    CHECK(((console_io_input *)data)->character == 'a');
    CHECK(!mvdm_vm_io_operations.input_pending(a, &pending) && pending);
    q.operation = CONSOLE_IO_READ_INPUT; q.state.mode = CONSOLE_IO_INPUT_RETURNABLE + 1;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
    CHECK(!r.result && r.error == ERROR_INVALID_PARAMETER);
    CHECK(!mvdm_vm_io_operations.input_pending(a, &pending) && pending);
    q.state.mode = CONSOLE_IO_INPUT_RETURNABLE;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
    CHECK(r.result && r.state.count == 1 && !r.error);
    CHECK(!((console_io_input *)data)->receipt_low && !((console_io_input *)data)->receipt_high);
    q.state.mode = 0;
    CHECK(WaitForSingleObject(mvdm_vm_io_ready(a), 0) == WAIT_TIMEOUT);
    for (i = 0; i < CONSOLE_IO_INPUT_CAPACITY; ++i) CHECK(!mvdm_vm_io_key(a, &key));
    CHECK(mvdm_vm_io_key(a, &key) == ERROR_NOT_ENOUGH_MEMORY);
    mvdm_vm_io_operations.finish(a);
    CHECK(!mvdm_vm_io_operations.input_pending(a, &pending) && pending);
    q.state.count = CONSOLE_IO_INPUT_CAPACITY;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
    CHECK(r.result && r.state.count == CONSOLE_IO_INPUT_CAPACITY);
    CHECK(!mvdm_vm_io_operations.input_pending(a, &pending) && !pending);
    {
        console_io_input returned[2] = {0};
        returned[0].type = returned[1].type = KEY_EVENT;
        returned[0].repeat = returned[1].repeat = 1;
        returned[0].character = 'x'; returned[1].character = 'y';
        CHECK(!mvdm_vm_io_key(a, &key));
        q.operation = CONSOLE_IO_PREPEND_KEYS; q.state.count = 2;
        q.data = (BYTE *)returned; q.bytes = sizeof(returned);
        returned[1].receipt_low = 1;
        CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
        CHECK(!r.result && r.error == ERROR_INVALID_DATA);
        returned[1].receipt_low = 0;
        CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
        CHECK(r.result && r.state.count == 2);
        q.operation = CONSOLE_IO_READ_INPUT; q.state.count = 3; q.data = NULL; q.bytes = 0;
        CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
        CHECK(r.result && r.state.count == 3);
        CHECK(((console_io_input *)data)[0].character == 'x' &&
            ((console_io_input *)data)[1].character == 'y' &&
            ((console_io_input *)data)[2].character == 'a');
        CHECK(WaitForSingleObject(mvdm_vm_io_ready(a), 0) == WAIT_TIMEOUT);
    }
    q.operation = CONSOLE_IO_SET_MODE; q.state.input = 1;
    q.state.mode = ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result) && r.result);
    q.operation = CONSOLE_IO_GET_MODE;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result) && r.result);
    CHECK(r.state.mode == q.state.mode);
    q.operation = CONSOLE_IO_SET_MODE; q.state.mode = ENABLE_LINE_INPUT;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
    CHECK(!r.result && r.error == ERROR_NOT_SUPPORTED);
    q.operation = CONSOLE_IO_GET_MODE; q.state.input = 0;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result) && r.result && !r.state.mode);
    q.operation = CONSOLE_IO_SET_MODE; q.state.mode = ENABLE_PROCESSED_OUTPUT;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result));
    CHECK(!r.result && r.error == ERROR_NOT_SUPPORTED);
    q.state.mode = 0;
    CHECK(!mvdm_vm_io_operations.dispatch(a, &cancel, &q, &r, &result) && r.result);
    /* A failed readiness reset is not a successful input transfer. Keep the
     * original FIFO intact so retirement cannot silently lose these keys.
     * Inject only this provider's reset failure; all handles remain valid. */
    CHECK(!mvdm_vm_io_key(b, &key));
    vm_io_reset_failure = mvdm_vm_io_ready(b);
    ZeroMemory(&q, sizeof(q));
    q.operation = CONSOLE_IO_READ_INPUT; q.state.count = 1;
    CHECK(!mvdm_vm_io_operations.dispatch(b, &cancel, &q, &r, &result));
    CHECK(!r.result && r.error == ERROR_INVALID_HANDLE && !r.bytes);
    CHECK(!mvdm_vm_io_operations.input_pending(b, &pending) && pending);
    q.operation = CONSOLE_IO_PEEK_INPUT;
    CHECK(!mvdm_vm_io_operations.dispatch(b, &cancel, &q, &r, &result));
    CHECK(r.result && r.state.count == 1 &&
        ((console_io_input *)data)->character == 'a');
    vm_io_reset_failure = NULL;
    {
        console_io_input returned;
        uint32_t count;
        uint64_t generation = mvdm_vm_continuation_generation(continuation);
        CHECK(mvdm_vm_io_return_input(b, 1, generation + 1) == ERROR_NOT_FOUND);
        CHECK(!mvdm_vm_io_operations.input_pending(b, &pending) && pending);
        CHECK(!mvdm_vm_io_return_input(b, 1, generation));
        CHECK(!mvdm_vm_io_operations.input_pending(b, &pending) && !pending);
        CHECK(!mvdm_vm_io_return_input(b, 1, generation)); /* No duplicate. */
        mvdm_vm_io_destroy(b);
        CHECK(!mvdm_vm_wake_read_input(1, generation, &returned, 1, &count, 1));
        CHECK(count == 1 && returned.character == 'a' && returned.scan == 0x1e);
        CHECK(!mvdm_vm_wake_read_input(1, generation, &returned, 1, &count, 1) && !count);
    }
    mvdm_vm_io_destroy(a); ntvdm_vm_text_destroy(text);
}

static void application_tests(uint64_t token, mvdm_vm_continuation *request)
{
    unsigned i;
    HANDLE cancel = CreateEventW(NULL, TRUE, FALSE, NULL);
    CHECK(cancel != NULL);
    application_published = CreateEventW(NULL, TRUE, FALSE, NULL);
    CHECK(application_published != NULL);
    for (i = 0; i < 18; ++i) {
        unsigned outcome = i % 6;
        application_context context = {0};
        uint64_t serial, ticket = 0;
        uint32_t size;
        mvdm_vm_wake_identity wake;
        HANDLE thread;
        struct { ntvdm_vm_text_header header; BYTE cells[4]; } frame = {
            {2, 1, 0, 0, 0x0607, 3, 0, 437, 16}, {'A', 7, 'B', 7}};
        console_io_request operation = {0};
        console_io_reply reply = {0};
        struct { console_text_style style; BYTE cells[4]; } video_frame = {0};
        console_video_description description = {0};
        context.request = request;
        context.implicit = i >= 6;
        CHECK(!ntvdm_vm_text_create(&frame, sizeof(frame), &context.text));
        CHECK(!mvdm_vm_io_create(context.text, request, "test", &context.io));
        if (i >= 12) {
            HANDLE waits[4] = {0};
            video_frame.style.font_height = 16; video_frame.style.cursor_column = 1;
            video_frame.cells[0] = 'C'; video_frame.cells[1] = 7;
            video_frame.cells[2] = 'D'; video_frame.cells[3] = 7;
            description.kind = CONSOLE_VIDEO_TEXT_FRAME;
            description.width = 2; description.height = 1; description.stride = 4;
            description.bytes = sizeof(video_frame);
            operation.operation = CONSOLE_IO_VIDEO_BEGIN; operation.state.mode = 1;
            operation.bytes = sizeof(description); operation.data = (BYTE *)&description;
            CHECK(!mvdm_vm_application_dispatch(request, context.text, waits, &operation, &reply) && reply.result);
            operation.operation = CONSOLE_IO_VIDEO_DATA;
            operation.bytes = CONSOLE_IO_TILE_BYTES; operation.data = (BYTE *)&video_frame;
            CHECK(!mvdm_vm_application_dispatch(request, context.text, waits, &operation, &reply) && reply.result);
            operation.operation = CONSOLE_IO_BARRIER; operation.bytes = 0; operation.data = NULL;
            CHECK(!mvdm_vm_application_dispatch(request, context.text, waits, &operation, &reply));
            CHECK(reply.error == ERROR_BUSY && ntvdm_vm_text_pending(context.text) == 2);
            context.final_operation.operation = CONSOLE_IO_VIDEO_DATA;
            context.final_operation.state.mode = 1;
            context.final_operation.state.count = CONSOLE_IO_TILE_BYTES;
            context.final_operation.bytes = sizeof(video_frame) - CONSOLE_IO_TILE_BYTES;
            context.final_operation.data = (BYTE *)&video_frame + CONSOLE_IO_TILE_BYTES;
        }
        if (!context.implicit) {
            operation.operation = CONSOLE_IO_PUBLICATION_BEGIN;
            CHECK(!ntvdm_vm_text_dispatch(context.text, &operation, &reply));
            operation.operation = CONSOLE_IO_CURSOR_POSITION;
            operation.state.x = 1;
            CHECK(!ntvdm_vm_text_dispatch(context.text, &operation, &reply));
        }
        if (outcome < 4) context.cancellation[outcome] = cancel;
        if (outcome == 4) context.cancellation[0] = context.cancellation[3] = cancel;
        CHECK(!mvdm_vm_notification_snapshot(&serial));
        CHECK(!mvdm_vm_notification_acknowledge(serial));
        eoi(15, 0); /* emulate the CPU acknowledging the previous IRQ */
        CHECK(ResetEvent(application_published) && ResetEvent(cancel));
        thread = CreateThread(NULL, 0, apply_from_channel, &context, 0, NULL);
        CHECK(thread != NULL);
        CHECK(WaitForSingleObject(application_published, INFINITE) == WAIT_OBJECT_0);
        CHECK(!mvdm_vm_wake_take(&wake));
        CHECK(mvdm_vm_continuation_read_io(token, 1, &ticket, NULL, 0, &size) == ERROR_INSUFFICIENT_BUFFER);
        CHECK(size == sizeof(frame) + (i >= 12 ? sizeof(ntvdm_vm_video_application) + sizeof(video_frame) : 0) && ticket);
        if (outcome < 4) CHECK(SetEvent(cancel));
        else CHECK(!mvdm_vm_continuation_ack_io(token, 1, ticket,
            outcome == 4 ? ERROR_SUCCESS : ERROR_WRITE_FAULT));
        CHECK(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0);
        CHECK(context.error == (DWORD)(outcome < 4 ? ERROR_OPERATION_ABORTED :
            outcome == 4 ? ERROR_SUCCESS : ERROR_WRITE_FAULT));
        CHECK(mvdm_vm_continuation_read_io(token, 1, &ticket, NULL, 0, &size) == ERROR_NOT_FOUND);
        operation.operation = CONSOLE_IO_SCREEN_INFO;
        CHECK(!ntvdm_vm_text_dispatch(context.text, &operation, &reply));
        CHECK(reply.state.x == (outcome == 4 ? 1 : 0));
        CHECK(!ntvdm_vm_text_pending(context.text));
        /* Every failed publication released its candidate for the next one. */
        operation.operation = CONSOLE_IO_PUBLICATION_BEGIN;
        CHECK(!ntvdm_vm_text_dispatch(context.text, &operation, &reply));
        operation.operation = CONSOLE_IO_PUBLICATION_ABORT;
        CHECK(!ntvdm_vm_text_dispatch(context.text, &operation, &reply));
        mvdm_vm_io_operations.finish(context.io);
        mvdm_vm_io_destroy(context.io);
        ntvdm_vm_text_destroy(context.text);
        CloseHandle(thread);
    }
    {
        HANDLE stopped[4] = {cancel, NULL, NULL, NULL};
        uint64_t ticket = 0;
        uint32_t size;
        CHECK(SetEvent(cancel));
        CHECK(mvdm_vm_application_publish(request, "frame", 6, stopped) == ERROR_OPERATION_ABORTED);
        CHECK(mvdm_vm_continuation_read_io(token, 1, &ticket, NULL, 0, &size) == ERROR_NOT_FOUND);
        stopped[0] = INVALID_HANDLE_VALUE;
        CHECK(mvdm_vm_application_publish(request, "frame", 6, stopped) == ERROR_INVALID_HANDLE);
    }
    CloseHandle(application_published); application_published = NULL;
    CloseHandle(cancel);
}
static void io_tests(uint64_t token, mvdm_vm_continuation *request)
{
    unsigned char bytes[] = {1, 2, 3}, copy[3] = {0};
    uint64_t ticket, query = 0, next;
    uint32_t size;
    HANDLE ready, unused, thread;
    mvdm_vm_wake_identity wake;
    const mvdm_vm_completion terminal = {0, 1, 0};
    io_wait_context context;
    mvdm_vm_bridge_result bridge;
    uint32_t descriptor[7] = {0};
    CHECK(!mvdm_vm_continuation_post_io(request, bytes, sizeof(bytes), &ticket, &ready));
    bytes[0] = 9; /* producer storage is not the retained guest job */
    CHECK(!mvdm_vm_wake_take(&wake) && wake.vm == 1 && wake.generation == 11);
    CHECK(WaitForSingleObject(ready, 0) == WAIT_TIMEOUT);
    CHECK(mvdm_vm_continuation_complete(request, &terminal) == ERROR_BUSY);
    CHECK(mvdm_vm_continuation_post_io(request, bytes, sizeof(bytes), &next, &unused) == ERROR_BUSY);
    CHECK(!next && !unused);
    CHECK(mvdm_vm_continuation_read_io(token, 2, &query, NULL, 0, &size) == ERROR_ACCESS_DENIED);
    CHECK(!query && !size);
    CHECK(mvdm_vm_continuation_read_io(token, 1, &query, NULL, 0, &size) == ERROR_INSUFFICIENT_BUFFER);
    CHECK(query == ticket && size == sizeof(bytes));
    CHECK(!mvdm_vm_continuation_read_io(token, 1, &query, copy, sizeof(copy), &size));
    CHECK(copy[0] == 1 && copy[1] == 2 && copy[2] == 3);
    memcpy(descriptor, &token, sizeof(token));
    memcpy(guest_memory, descriptor, sizeof(descriptor));
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 13, 0, sizeof(descriptor), 1, 0, &bridge));
    CHECK(bridge.status == ERROR_ACCESS_DENIED);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 13, 0, sizeof(descriptor), 2, 1, &bridge));
    CHECK(bridge.status == ERROR_ACCESS_DENIED);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 13, 0, sizeof(descriptor), 1, 1, &bridge));
    CHECK(bridge.status == ERROR_INSUFFICIENT_BUFFER);
    memcpy(descriptor, guest_memory, sizeof(descriptor));
    CHECK(descriptor[2] == (uint32_t)ticket && descriptor[3] == (uint32_t)(ticket >> 32));
    CHECK(descriptor[5] == sizeof(bytes));
    descriptor[4] = 255; /* Invalid destination must not acknowledge application. */
    memcpy(guest_memory, descriptor, sizeof(descriptor));
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 13, 0, sizeof(descriptor), 1, 1, &bridge));
    CHECK(bridge.status == ERROR_INVALID_ADDRESS);
    descriptor[4] = 128;
    memcpy(guest_memory, descriptor, sizeof(descriptor));
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 13, 0, sizeof(descriptor), 1, 1, &bridge));
    CHECK(!bridge.status && guest_memory[128] == 1 && guest_memory[130] == 3);
    CHECK(WaitForSingleObject(ready, 0) == WAIT_TIMEOUT); /* copying is not application */
    CHECK(mvdm_vm_continuation_finish_io(request, ticket, 0) == ERROR_IO_INCOMPLETE);
    CHECK(mvdm_vm_continuation_ack_io(token, 2, ticket, 0) == ERROR_ACCESS_DENIED);
    CHECK(mvdm_vm_continuation_ack_io(token, 1, ticket + 1, 0) == ERROR_NOT_FOUND);
    descriptor[6] = ERROR_WRITE_FAULT;
    memcpy(guest_memory, descriptor, sizeof(descriptor));
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 14, 0, sizeof(descriptor), 1, 1, &bridge));
    CHECK(!bridge.status);
    CHECK(mvdm_vm_continuation_ack_io(token, 1, ticket, 0) == ERROR_INVALID_STATE);
    CHECK(mvdm_vm_continuation_finish_io(request, ticket, 0) == ERROR_WRITE_FAULT);
    CHECK(!mvdm_vm_continuation_post_io(request, bytes, sizeof(bytes), &next, &ready));
    CHECK(next > ticket);
    CHECK(mvdm_vm_continuation_ack_io(token, 1, ticket, 0) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_wake_take(&wake));
    CHECK(mvdm_vm_continuation_finish_io(request, next, ERROR_CANCELLED) == ERROR_CANCELLED);
    CHECK(mvdm_vm_continuation_ack_io(token, 1, next, 0) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_continuation_post_io(request, bytes, sizeof(bytes), &ticket, &ready));
    CHECK(!mvdm_vm_wake_take(&wake));
    context.ready = ready; context.request = request; context.ticket = ticket;
    context.error = ERROR_IO_INCOMPLETE;
    thread = CreateThread(NULL, 0, finish_guest_io, &context, 0, NULL);
    CHECK(thread != NULL);
    CHECK(!mvdm_vm_continuation_ack_io(token, 1, ticket, 0));
    CHECK(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0);
    CHECK(context.error == ERROR_SUCCESS);
    CloseHandle(thread);
    provider_tests(request);
    application_tests(token, request);
}
static void continuation_tests(void)
{
    const mvdm_vm_completion terminal = {0, 19, 0}, reentry = {0, 0, 1};
    mvdm_vm_completion result;
    mvdm_vm_continuation *a, *b;
    mvdm_vm_bridge_result bridge;
    mvdm_vm_wake_identity wake;
    completion_context context = {0};
    io_wait_context io_context = {0};
    uint64_t one, two, after_reset;
    WORD irq;
    HANDLE thread;
    CHECK(!mvdm_vm_continuation_invalidated(NULL));
    CHECK(mvdm_vm_continuation_begin(1, &one, &a) == ERROR_NOT_READY && !one && !a);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_notification_ready());
    CHECK(!mvdm_vm_wake_register(1, 11));
    CHECK(!mvdm_vm_wake_register(2, 22));
    CHECK(mvdm_vm_continuation_begin(99, &one, &a) == ERROR_NOT_FOUND && !one && !a);
    CHECK(!mvdm_vm_continuation_begin(1, &one, &a));
    CHECK(!mvdm_vm_continuation_begin(2, &two, &b) && two > one);
    CHECK(WaitForSingleObject(mvdm_vm_continuation_invalidated(a), 0) == WAIT_TIMEOUT);
    mvdm_vm_continuation_retire(1, 10); /* another generation cannot cancel */
    CHECK(WaitForSingleObject(mvdm_vm_continuation_invalidated(a), 0) == WAIT_TIMEOUT);
    io_tests(one, a);
    CHECK(mvdm_vm_continuation_consume(one, 1, &result) == ERROR_IO_INCOMPLETE);
    CHECK(mvdm_vm_continuation_consume(one, 2, &result) == ERROR_ACCESS_DENIED);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 12,
        (uint32_t)one, (uint32_t)(one >> 32), 1, 1, &bridge));
    CHECK(bridge.status == ERROR_IO_INCOMPLETE && !bridge.value && !bridge.serial_low);
    CHECK(!mvdm_vm_continuation_complete(b, &reentry));
    CHECK(!mvdm_vm_continuation_complete(a, &terminal));
    CHECK(WaitForSingleObject(mvdm_vm_continuation_invalidated(a), 0) == WAIT_TIMEOUT);
    CHECK(mvdm_vm_continuation_complete(a, &terminal) == ERROR_INVALID_STATE);
    CHECK(!mvdm_vm_wake_take(&wake) && wake.vm == 2 && wake.generation == 22);
    CHECK(!mvdm_vm_wake_take(&wake) && wake.vm == 1 && wake.generation == 11);
    mvdm_vm_continuation_release(b); /* executor can leave before consumption */
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 12,
        (uint32_t)two, (uint32_t)(two >> 32), 2, 1, &bridge));
    CHECK(!bridge.status && bridge.serial_low == 1);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 12,
        (uint32_t)one, (uint32_t)(one >> 32), 2, 1, &bridge));
    CHECK(bridge.status == ERROR_ACCESS_DENIED);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 12,
        (uint32_t)one, (uint32_t)(one >> 32), 1, 0, &bridge));
    CHECK(bridge.status == ERROR_ACCESS_DENIED);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 12,
        (uint32_t)one, (uint32_t)(one >> 32), 1, 1, &bridge));
    CHECK(!bridge.status && bridge.value == 19);
    CHECK(WaitForSingleObject(mvdm_vm_continuation_invalidated(a), 0) == WAIT_OBJECT_0);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 12,
        (uint32_t)one, (uint32_t)(one >> 32), 1, 1, &bridge));
    CHECK(bridge.status == ERROR_NOT_FOUND && !bridge.value && !bridge.serial_low);
    mvdm_vm_continuation_release(a); /* or after consumption */

    CHECK(!mvdm_vm_continuation_begin(1, &one, &context.request));
    CHECK(DuplicateHandle(GetCurrentProcess(),mvdm_vm_continuation_invalidated(context.request),
        GetCurrentProcess(),&context.resume,SYNCHRONIZE,FALSE,0));
    thread = CreateThread(NULL, 0, complete_after_retirement, &context, 0, NULL);
    CHECK(thread != NULL);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 8, 11, 0, 1, 1, &bridge));
    CHECK(!bridge.status);
    CHECK(!mvdm_vm_wake_register(1, 12));
    CHECK(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0);
    CHECK(context.error == ERROR_OPERATION_ABORTED);
    CHECK(mvdm_vm_continuation_consume(one, 1, &result) == ERROR_NOT_FOUND);
    CHECK(mvdm_vm_wake_take(&wake) == ERROR_NO_MORE_ITEMS);
    CloseHandle(thread);
    CloseHandle(context.resume);

    CHECK(!mvdm_vm_continuation_begin(1, &one, &a));
    CHECK(!mvdm_vm_continuation_begin(2, &two, &b));
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 5, 0, 0, 0, 1, &bridge));
    CHECK(!bridge.status);
    CHECK(WaitForSingleObject(mvdm_vm_continuation_invalidated(a), 0) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(mvdm_vm_continuation_invalidated(b), 0) == WAIT_OBJECT_0);
    CHECK(mvdm_vm_continuation_complete(a, &terminal) == ERROR_OPERATION_ABORTED);
    mvdm_vm_continuation_release(a);
    CHECK(mvdm_vm_continuation_complete(b, &terminal) == ERROR_OPERATION_ABORTED);
    mvdm_vm_continuation_release(b);
    CHECK(mvdm_vm_continuation_consume(two, 2, &result) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_wake_register(1, 13));
    CHECK(!mvdm_vm_continuation_begin(1, &after_reset, &a) && after_reset > two);
    io_context.request = a;
    CHECK(!mvdm_vm_continuation_post_io(a, "retirement", 11,
        &io_context.ticket, &io_context.ready));
    thread = CreateThread(NULL, 0, finish_guest_io, &io_context, 0, NULL);
    CHECK(thread != NULL);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 5, 0, 0, 0, 1, &bridge));
    CHECK(!bridge.status);
    CHECK(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0);
    CHECK(io_context.error == ERROR_OPERATION_ABORTED);
    CHECK(mvdm_vm_continuation_ack_io(after_reset, 1, io_context.ticket, 0) == ERROR_NOT_FOUND);
    CloseHandle(thread);
    mvdm_vm_continuation_release(a);
}
/* Production executor ownership with real threadpool/continuation/provider.
 * Only broker/launcher boundaries are substituted here. */
static HANDLE executor_started, executor_release, executor_retiring, executor_retire_release;
static LONG executor_freed, executor_retired;
static BOOL executor_hold_retirement;
static DWORD executor_payload_error, executor_register_error;
static void *executor_input[3]; /* Borrow only while the fixture holds execution. */
DWORD ntvdm_vm_payload_begin(ntvdm_vm_host *host, uint64_t *payload)
{ CHECK(host == (ntvdm_vm_host *)1); *payload = 0;
  if (executor_payload_error) return executor_payload_error;
  *payload = 1; return 0; }
DWORD ntvdm_vm_host_register(ntvdm_vm_host *host, uint64_t payload, uint64_t vm,
    HANDLE input, const ntvdm_vm_io *io, void *context, common_frontend_location *location)
{
    CHECK(host == (ntvdm_vm_host *)1 && payload == 1 && input && context);
    CHECK(io == &mvdm_vm_io_operations);
    CHECK(vm > 0 && vm < 3);
    if (executor_register_error) return executor_register_error;
    executor_input[vm] = context;
    location->payload = payload; location->vm = vm; location->incarnation = vm;
    return 0;
}
DWORD mvdm_vm_command_execute(mvdm_vm_continuation *request, ntvdm_command_launch *launch,
    ntvdm_vm_host *host, const common_frontend_location *location, DWORD *code)
{
    HANDLE waits[2] = {executor_release, mvdm_vm_continuation_invalidated(request)};
    DWORD status;
    CHECK(host == (ntvdm_vm_host *)1 && launch && location->incarnation);
    CHECK(ReleaseSemaphore(executor_started, 1, NULL));
    status = WaitForMultipleObjects(2, waits, FALSE, 10000);
    CHECK(status == WAIT_OBJECT_0 || status == WAIT_OBJECT_0 + 1);
    *code = (DWORD)location->vm;
    return status == WAIT_OBJECT_0 ? 0 : ERROR_OPERATION_ABORTED;
}
DWORD ntvdm_vm_host_retire(ntvdm_vm_host *host, const common_frontend_location *location)
{
    CHECK(host == (ntvdm_vm_host *)1 && location->incarnation);
    if (executor_hold_retirement) {
        CHECK(SetEvent(executor_retiring));
        CHECK(WaitForSingleObject(executor_retire_release, 10000) == WAIT_OBJECT_0);
    }
    InterlockedIncrement(&executor_retired);
    return 0;
}
void ntvdm_command_launch_destroy(ntvdm_command_launch *launch)
{ CHECK(launch == (ntvdm_command_launch *)1); InterlockedIncrement(&executor_freed); }
static DWORD WINAPI close_executor(void *context)
{ mvdm_vm_executor_close(context); return 0; }
static ntvdm_vm_text *executor_text(void)
{
    struct { ntvdm_vm_text_header h; BYTE cells[4]; } frame = {
        {2, 1, 0, 0, 0x0607, 3, 0, 437, 16}, {'A', 7, 'B', 7}};
    ntvdm_vm_text *text;
    CHECK(!ntvdm_vm_text_create(&frame, sizeof(frame), &text));
    return text;
}
static void executor_input_bop_tests(mvdm_vm_executor *executor, uint64_t token)
{
    uint32_t descriptor[8] = {(uint32_t)token, (uint32_t)(token >> 32),
        1, 3, 'A', 0x1e, 'a', SHIFT_PRESSED};
    mvdm_vm_bridge_result bridge;
    console_io_input received[4];
    console_io_request q = {0}; console_io_reply r = {0};
    frontend_base_result result = {0}; ntvdm_vm_cancel cancel = {0};
    BOOL pending;
    unsigned i;
    memcpy(guest_memory, descriptor, sizeof(descriptor));
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 16, 0, 32, 1, 1, &bridge));
    CHECK(bridge.status == ERROR_NOT_READY);
    input_executor = executor;
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 16, 0, 32, 1, 0, &bridge));
    CHECK(bridge.status == ERROR_ACCESS_DENIED);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 16, 0, 32, 2, 1, &bridge));
    CHECK(bridge.status == ERROR_NOT_FOUND);
    CHECK(mvdm_vm_executor_key_bop(executor, 1, 0, 31) == ERROR_INVALID_PARAMETER);
    CHECK(mvdm_vm_executor_key_bop(executor, 1, 250, 32) == ERROR_INVALID_ADDRESS);
    for (i = 2; i <= 6; ++i) {
        uint32_t saved = descriptor[i];
        descriptor[i] = UINT32_MAX;
        memcpy(guest_memory, descriptor, sizeof(descriptor));
        CHECK(mvdm_vm_executor_key_bop(executor, 1, 0, 32) == ERROR_INVALID_DATA);
        descriptor[i] = saved;
    }
    descriptor[3] = 0;
    memcpy(guest_memory, descriptor, sizeof(descriptor));
    CHECK(mvdm_vm_executor_key_bop(executor, 1, 0, 32) == ERROR_INVALID_DATA);
    CHECK(!mvdm_vm_io_operations.input_pending(executor_input[1], &pending) && !pending);
    CHECK(!mvdm_vm_io_operations.input_pending(executor_input[2], &pending) && !pending);
    descriptor[3] = 3;
    memcpy(guest_memory, descriptor, sizeof(descriptor));
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 16, 0, 32, 1, 1, &bridge));
    CHECK(!bridge.status);
    descriptor[2] = 0; descriptor[3] = 1;
    memcpy(guest_memory, descriptor, sizeof(descriptor));
    CHECK(!mvdm_vm_executor_key_bop(executor, 1, 0, 32));
    memset(guest_memory, 0xff, sizeof(guest_memory)); /* No retained guest alias. */
    q.operation = CONSOLE_IO_READ_INPUT; q.state.count = 2;
    r.data = (uint8_t *)received;
    CHECK(!mvdm_vm_io_operations.dispatch(executor_input[1], &cancel, &q, &r, &result));
    CHECK(r.result && r.state.count == 2 && r.bytes == 2 * sizeof(received[0]));
    CHECK(received[0].key_down == 1 && received[0].repeat == 3);
    CHECK(received[1].key_down == 0 && received[1].repeat == 1);
    for (i = 0; i < 2; ++i) {
        CHECK(received[i].type == KEY_EVENT && received[i].virtual_key == 'A');
        CHECK(received[i].scan == 0x1e && received[i].character == 'a');
        CHECK(received[i].control == SHIFT_PRESSED);
        CHECK(!received[i].receipt_low && !received[i].receipt_high);
    }
    CHECK(!mvdm_vm_io_operations.input_pending(executor_input[1], &pending) && !pending);
    CHECK(!mvdm_vm_io_operations.input_pending(executor_input[2], &pending) && !pending);
    {
        uint32_t bios[7] = {(uint32_t)token, (uint32_t)(token >> 32), 0x1e,
            1, 0x1e61, 0, 0};
        memcpy(guest_memory, bios, sizeof(bios));
        CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 27) == ERROR_INVALID_PARAMETER);
        CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 250, 28) == ERROR_INVALID_ADDRESS);
        CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 17, 0, 28, 1, 0, &bridge));
        CHECK(bridge.status == ERROR_ACCESS_DENIED);
        CHECK(mvdm_vm_executor_bios_key_bop(executor, 2, 0, 28) == ERROR_NOT_FOUND);
        for (i = 2; i < 7; ++i) {
            uint32_t saved = bios[i];
            bios[i] = UINT32_MAX; memcpy(guest_memory, bios, sizeof(bios));
            CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_INVALID_DATA);
            bios[i] = saved;
        }
        bios[4] = 0; memcpy(guest_memory, bios, sizeof(bios));
        CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_NO_UNICODE_TRANSLATION);
        CHECK(!mvdm_vm_io_operations.input_pending(executor_input[1], &pending) && !pending);
        bios[4] = 0x1e61; memcpy(guest_memory, bios, sizeof(bios));
        CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 17, 0, 28, 1, 1, &bridge));
        CHECK(!bridge.status);
        /* A release must use this accepted make, not a second translation.
         * Its current BIOS flags may differ from those of the original make. */
        bios[3] = 0; bios[4] = 0; bios[5] = 0x170; bios[6] = 8;
        memcpy(guest_memory, bios, sizeof(bios));
        CHECK(mvdm_vm_executor_bios_key_bop(executor, 2, 0, 28) == ERROR_NOT_FOUND);
        CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
        CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_NOT_FOUND);
        memset(guest_memory, 0xff, sizeof(guest_memory));
        CHECK(!mvdm_vm_io_operations.dispatch(executor_input[1], &cancel, &q, &r, &result));
        CHECK(r.result && r.state.count == 2);
        CHECK(received[0].key_down == 1 && received[0].repeat == 1);
        CHECK(received[0].virtual_key == 'A' && received[0].scan == 0x1e);
        CHECK(received[0].character == 'a' && !received[0].control);
        CHECK(!received[1].key_down && received[1].repeat == 1 && !received[1].character);
        CHECK(received[1].virtual_key == 'A' && received[1].scan == 0x1e);
        CHECK(received[1].control == (LEFT_CTRL_PRESSED | RIGHT_ALT_PRESSED |
            CAPSLOCK_ON | NUMLOCK_ON | SCROLLLOCK_ON));
        CHECK(!mvdm_vm_io_operations.input_pending(executor_input[1], &pending) && !pending);
        /* The E0 prefix is part of physical identity, even when BIOS
         * translation produces the same native scan for both keys. */
        bios[3] = 1; bios[4] = 0x1e61; bios[5] = bios[6] = 0;
        bios[2] = 0x1e; memcpy(guest_memory, bios, sizeof(bios));
        CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
        bios[2] = 0x9e; memcpy(guest_memory, bios, sizeof(bios));
        CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
        bios[3] = 0; bios[4] = 0;
        memcpy(guest_memory, bios, sizeof(bios));
        CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
        CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_NOT_FOUND);
        bios[2] = 0x1e; memcpy(guest_memory, bios, sizeof(bios));
        CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
        q.state.count = 4;
        CHECK(!mvdm_vm_io_operations.dispatch(executor_input[1], &cancel, &q, &r, &result));
        CHECK(r.result && r.state.count == 4);
        CHECK(received[0].key_down && !received[0].control);
        CHECK(received[1].key_down && received[1].control == ENHANCED_KEY);
        CHECK(!received[2].key_down && received[2].control == ENHANCED_KEY);
        CHECK(!received[3].key_down && !received[3].control);
        /* A BIOS-only character is not another make of the already released
         * Alt key. It reconstructs a complete native pair atomically. */
        bios[2] = 0; bios[3] = 2; bios[4] = 'a';
        memcpy(guest_memory, bios, sizeof(bios));
        CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
        CHECK(!mvdm_vm_io_operations.dispatch(executor_input[1], &cancel, &q, &r, &result));
        CHECK(r.result && r.state.count == 2);
        CHECK(received[0].key_down && !received[1].key_down);
        CHECK(received[0].character == 'a' && received[1].character == 'a');
        bios[2] = 0x38;
        memcpy(guest_memory, bios, sizeof(bios));
        CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_INVALID_DATA);
        bios[3] = 0; bios[4] = 0;
        memcpy(guest_memory, bios, sizeof(bios));
        CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_NOT_FOUND);
        {
            static const uint32_t modifiers[][5] = {
                {0x2a, VK_SHIFT, 2, 0, SHIFT_PRESSED},
                {0x36, VK_SHIFT, 1, 0, SHIFT_PRESSED},
                {0x1d, VK_CONTROL, 0x104, 0, LEFT_CTRL_PRESSED},
                {0x9d, VK_CONTROL, 4, 4, RIGHT_CTRL_PRESSED | ENHANCED_KEY},
                {0x38, VK_MENU, 0x208, 0, LEFT_ALT_PRESSED},
                {0xb8, VK_MENU, 8, 8, RIGHT_ALT_PRESSED | ENHANCED_KEY}
            };
            for (i = 0; i < ARRAYSIZE(modifiers); ++i) {
                bios[2] = modifiers[i][0]; bios[3] = 1;
                bios[4] = bios[5] = bios[6] = 0;
                memcpy(guest_memory, bios, sizeof(bios));
                CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
                bios[3] = 0; bios[5] = modifiers[i][2]; bios[6] = modifiers[i][3];
                memcpy(guest_memory, bios, sizeof(bios));
                CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
                CHECK(!mvdm_vm_io_operations.dispatch(executor_input[1], &cancel, &q, &r, &result));
                CHECK(r.result && r.state.count == 2);
                CHECK(received[0].key_down && received[0].virtual_key == modifiers[i][1]);
                CHECK(received[0].scan == (modifiers[i][0] & 127) && !received[0].character);
                CHECK(received[0].control == modifiers[i][4]);
                CHECK(!received[1].key_down && received[1].virtual_key == modifiers[i][1]);
                CHECK(received[1].control == (modifiers[i][4] & ENHANCED_KEY));
            }
            bios[2] = 0x1e; bios[5] = bios[6] = 0;
        }
        {
            KEY_EVENT_RECORD filler = {TRUE, 1, 'B', 0x30, {'b'}, 0};
            /* A rejected make must not invent a later release. Conversely,
             * backpressure on release must retain the accepted make. */
            for (i = 0; i < CONSOLE_IO_INPUT_CAPACITY; ++i)
                CHECK(!mvdm_vm_io_key(executor_input[1], &filler));
            bios[3] = 1; bios[4] = 0x1e61;
            memcpy(guest_memory, bios, sizeof(bios));
            CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_NOT_ENOUGH_MEMORY);
            bios[3] = 0; bios[4] = 0;
            memcpy(guest_memory, bios, sizeof(bios));
            CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_NOT_FOUND);
            q.state.count = 1;
            CHECK(!mvdm_vm_io_operations.dispatch(executor_input[1], &cancel, &q, &r, &result));
            CHECK(r.result && r.state.count == 1 && received[0].virtual_key == 'B');
            bios[2] = 0; bios[3] = 2; bios[4] = 'a';
            memcpy(guest_memory, bios, sizeof(bios));
            CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_NOT_ENOUGH_MEMORY);
            bios[2] = 0x1e;
            bios[3] = 1; bios[4] = 0x1e61;
            memcpy(guest_memory, bios, sizeof(bios));
            CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
            bios[3] = 0; bios[4] = 0;
            memcpy(guest_memory, bios, sizeof(bios));
            CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_NOT_ENOUGH_MEMORY);
            for (i = 0; i < CONSOLE_IO_INPUT_CAPACITY; ++i) {
                CHECK(!mvdm_vm_io_operations.dispatch(executor_input[1], &cancel, &q, &r, &result));
                CHECK(r.result && r.state.count == 1);
                CHECK(received[0].virtual_key == (uint32_t)(i + 1 == CONSOLE_IO_INPUT_CAPACITY ? 'A' : 'B'));
            }
            CHECK(!mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28));
            CHECK(mvdm_vm_executor_bios_key_bop(executor, 1, 0, 28) == ERROR_NOT_FOUND);
            CHECK(!mvdm_vm_io_operations.dispatch(executor_input[1], &cancel, &q, &r, &result));
            CHECK(r.result && r.state.count == 1 && !received[0].key_down);
            CHECK(received[0].virtual_key == 'A' && !received[0].character);
            CHECK(!mvdm_vm_io_operations.input_pending(executor_input[1], &pending) && !pending);
        }
    }
    input_executor = NULL;
}
static void executor_tests(void)
{
    mvdm_vm_executor *executor;
    ntvdm_vm_text *retained;
    uint64_t first, second, rejected;
    WORD irq;
    HANDLE closing;
    KEY_EVENT_RECORD key = {0};
    mvdm_vm_completion completion;
    key.wRepeatCount = 1; key.bKeyDown = TRUE; key.wVirtualKeyCode = 'A';
    executor_started = CreateSemaphoreW(NULL, 0, 2, NULL);
    executor_release = CreateEventW(NULL, TRUE, FALSE, NULL);
    executor_retiring = CreateEventW(NULL, TRUE, FALSE, NULL);
    executor_retire_release = CreateEventW(NULL, TRUE, FALSE, NULL);
    CHECK(executor_started && executor_release && executor_retiring && executor_retire_release);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_wake_register(1, 100));
    CHECK(!mvdm_vm_wake_register(2, 101));
    CHECK(!mvdm_vm_executor_create((ntvdm_vm_host *)1, &executor));
    retained = executor_text();
    CHECK(mvdm_vm_executor_submit(executor, 3, (ntvdm_command_launch *)1,
        retained, &rejected) == ERROR_NOT_FOUND && !rejected);
    CHECK(!mvdm_vm_executor_submit(executor, 1, (ntvdm_command_launch *)1,
        executor_text(), &first));
    CHECK(WaitForSingleObject(executor_started, 10000) == WAIT_OBJECT_0);
    CHECK(mvdm_vm_executor_submit(executor, 1, (ntvdm_command_launch *)1,
        retained, &rejected) == ERROR_BUSY && !rejected);
    CHECK(!mvdm_vm_executor_submit(executor, 2, (ntvdm_command_launch *)1,
        executor_text(), &second));
    CHECK(WaitForSingleObject(executor_started, 10000) == WAIT_OBJECT_0);
    executor_input_bop_tests(executor, first);
    CHECK(mvdm_vm_executor_key(executor, 0, first, &key) == ERROR_INVALID_PARAMETER);
    CHECK(mvdm_vm_executor_key(executor, 2, first, &key) == ERROR_NOT_FOUND);
    CHECK(mvdm_vm_executor_key(executor, 1, second, &key) == ERROR_NOT_FOUND);
    CHECK(first != second && !mvdm_vm_executor_key(executor, 1, first, &key));
    CHECK(!mvdm_vm_executor_key(executor, 2, second, &key));
    CHECK(mvdm_vm_executor_key(executor, 1, UINT64_MAX, &key) == ERROR_NOT_FOUND);
    /* Close cancels both commands but cannot return before broker retirement
     * joins their callbacks. No timeout/sleep drives the production protocol. */
    executor_hold_retirement = TRUE;
    closing = CreateThread(NULL, 0, close_executor, executor, 0, NULL);
    CHECK(closing && WaitForSingleObject(executor_retiring, 10000) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(closing, 0) == WAIT_TIMEOUT);
    CHECK(mvdm_vm_executor_key(executor, 1, first, &key) == ERROR_OPERATION_ABORTED);
    CHECK(mvdm_vm_executor_submit(executor, 1, (ntvdm_command_launch *)1,
        retained, &rejected) == ERROR_OPERATION_ABORTED && !rejected);
    CHECK(!executor_freed && !executor_retired);
    CHECK(SetEvent(executor_retire_release));
    CHECK(WaitForSingleObject(closing, 10000) == WAIT_OBJECT_0);
    CHECK(executor_freed == 2 && executor_retired == 2);
    CHECK(mvdm_vm_continuation_consume(first, 1, &completion) == ERROR_NOT_FOUND);
    CHECK(mvdm_vm_continuation_consume(second, 2, &completion) == ERROR_NOT_FOUND);
    mvdm_vm_executor_destroy(executor);
    ntvdm_vm_text_destroy(retained); /* failed submissions did not consume it */
    CHECK(!mvdm_vm_notification_close());
    mvdm_vm_wake_reset();
    /* Normal task completion also closes input before retirement joins.
     * This case has neither executor close nor continuation cancellation. */
    CHECK(ResetEvent(executor_retiring));
    CHECK(ResetEvent(executor_retire_release));
    CHECK(SetEvent(executor_release));
    application_published = CreateEventW(NULL, FALSE, FALSE, NULL);
    CHECK(application_published != NULL);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_notification_ready());
    CHECK(!mvdm_vm_wake_register(1, 102));
    CHECK(!mvdm_vm_executor_create((ntvdm_vm_host *)1, &executor));
    CHECK(!mvdm_vm_executor_submit(executor, 1, (ntvdm_command_launch *)1,
        executor_text(), &first));
    CHECK(WaitForSingleObject(executor_retiring, 10000) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(application_published, 0) == WAIT_TIMEOUT);
    CHECK(mvdm_vm_executor_key(executor, 1, first, &key) == ERROR_OPERATION_ABORTED);
    CHECK(mvdm_vm_executor_key(executor, 2, first, &key) == ERROR_NOT_FOUND);
    CHECK(SetEvent(executor_retire_release));
    CHECK(WaitForSingleObject(application_published, 10000) == WAIT_OBJECT_0);
    CHECK(!mvdm_vm_continuation_consume(first, 1, &completion));
    CHECK(!completion.error && completion.exit_code == 1);
    CHECK(mvdm_vm_executor_key(executor, 1, first, &key) == ERROR_NOT_FOUND);
    mvdm_vm_executor_destroy(executor);
    CHECK(executor_freed == 3 && executor_retired == 3);
    executor_hold_retirement = FALSE;
    CHECK(!mvdm_vm_notification_close());
    mvdm_vm_wake_reset();
    CloseHandle(application_published); application_published = NULL;
    CloseHandle(closing); CloseHandle(executor_started); CloseHandle(executor_release);
    CloseHandle(executor_retiring); CloseHandle(executor_retire_release);
}

static void executor_registration_failure_tests(void)
{
    mvdm_vm_executor *executor = (mvdm_vm_executor *)1;
    mvdm_vm_completion completion;
    const DWORD errors[] = {ERROR_ACCESS_DENIED, ERROR_BROKEN_PIPE};
    LONG freed = executor_freed, retired = executor_retired;
    uint64_t token;
    unsigned i;
    WORD irq;
    executor_payload_error = ERROR_ACCESS_DENIED;
    CHECK(mvdm_vm_executor_create((ntvdm_vm_host *)1, &executor) == ERROR_ACCESS_DENIED);
    CHECK(!executor); /* Failed creation cannot publish a partial owner. */
    executor_payload_error = 0;
    for (i = 0; i < _countof(errors); ++i) {
        CHECK(!mvdm_vm_notification_open(&irq));
        CHECK(!mvdm_vm_notification_ready());
        CHECK(!mvdm_vm_wake_register(1, 501 + i));
        application_published = CreateEventW(NULL, TRUE, FALSE, NULL);
        CHECK(application_published);
        CHECK(!mvdm_vm_executor_create((ntvdm_vm_host *)1, &executor));
        executor_register_error = errors[i];
        CHECK(!mvdm_vm_executor_submit(executor, 1, (ntvdm_command_launch *)1,
            executor_text(), &token) && token);
        CHECK(WaitForSingleObject(application_published, 10000) == WAIT_OBJECT_0);
        CHECK(!mvdm_vm_continuation_consume(token, 1, &completion));
        CHECK(completion.error == errors[i] && !completion.exit_code);
        mvdm_vm_executor_destroy(executor); /* Joins the completion producer. */
        CHECK(executor_freed == freed + (LONG)i + 1);
        CHECK(executor_retired == retired); /* No registered frontend to retire. */
        executor_register_error = 0;
        CHECK(!mvdm_vm_notification_close());
        mvdm_vm_wake_reset();
        CloseHandle(application_published); application_published = NULL;
    }
}

static void returned_input_tests(void)
{
    console_io_input keys[40] = {0}, output[40] = {0};
    uint32_t count, i;
    WORD irq;
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_wake_register(1, 301));
    CHECK(!mvdm_vm_wake_register(2, 302));
    for (i = 0; i < 40; ++i) {
        keys[i].type = KEY_EVENT; keys[i].key_down = !(i & 1);
        keys[i].repeat = i + 1; keys[i].character = 'a' + i;
    }
    CHECK(mvdm_vm_wake_return_input(1, 300, keys, 40) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_wake_return_input(1, 301, keys, 40));
    CHECK(!mvdm_vm_wake_read_input(2, 302, output, 40, &count, 1) && !count);
    CHECK(!mvdm_vm_wake_read_input(1, 301, output, 15, &count, 0) && count == 15);
    CHECK(!memcmp(keys, output, 15 * sizeof(*keys)));
    CHECK(!mvdm_vm_wake_read_input(1, 301, output, 15, &count, 1) && count == 15);
    CHECK(!memcmp(keys, output, 15 * sizeof(*keys)));
    CHECK(!mvdm_vm_wake_return_input(1, 301, keys, 2));
    CHECK(!mvdm_vm_wake_read_input(1, 301, output, 40, &count, 1) && count == 27);
    CHECK(!memcmp(keys + 15, output, 25 * sizeof(*keys)));
    CHECK(!memcmp(keys, output + 25, 2 * sizeof(*keys)));
    CHECK(!mvdm_vm_wake_read_input(1, 301, output, 40, &count, 1) && !count);
    CHECK(!mvdm_vm_wake_return_input(1, 301, keys, 40));
    CHECK(!mvdm_vm_wake_retire(1, 301));
    CHECK(!mvdm_vm_wake_register(1, 303));
    CHECK(mvdm_vm_wake_return_input(1, 301, keys, 1) == ERROR_NOT_FOUND);
    CHECK(mvdm_vm_wake_read_input(1, 301, output, 40, &count, 1) == ERROR_NOT_FOUND && !count);
    CHECK(!mvdm_vm_wake_read_input(1, 303, output, 40, &count, 1) && !count);
    CHECK(!mvdm_vm_wake_return_input(1, 303, keys, 40));
    mvdm_vm_wake_reset();
    CHECK(!mvdm_vm_wake_register(1, 304));
    CHECK(!mvdm_vm_wake_read_input(1, 304, output, 40, &count, 1) && !count);
    {
        uint32_t descriptor[3] = {304, 0, 437};
        mvdm_vm_bridge_result result;
        ZeroMemory(keys, sizeof(keys));
        keys[0].type = keys[1].type = KEY_EVENT;
        keys[0].key_down = 1; keys[0].repeat = 20;
        keys[0].character = 'z'; keys[0].scan = 0x2c; keys[0].virtual_key = 'Z';
        keys[1].repeat = 1; keys[1].scan = 0x2c; keys[1].virtual_key = 'Z';
        CHECK(!mvdm_vm_wake_return_input(1, 304, keys, 2));
        memcpy(guest_memory, descriptor, sizeof(descriptor));
        CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 18, 0, 12, 1, 0, &result));
        CHECK(result.status == ERROR_ACCESS_DENIED);
        CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 18, 0, 11, 1, 1, &result));
        CHECK(result.status == ERROR_INVALID_PARAMETER);
        CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 18, 0, 12, 2, 1, &result));
        CHECK(result.status == ERROR_NOT_FOUND);
        CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 18, 0, 12, 1, 1, &result));
        CHECK(!result.status && result.value == 0x2c7a && result.serial_low == 20);
        CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 18, 0, 12, 1, 1, &result));
        CHECK(result.status == ERROR_NO_MORE_ITEMS); /* Release is not another char. */
        keys[0].character = 0; keys[0].scan = 0x3b; keys[0].virtual_key = VK_F1;
        CHECK(!mvdm_vm_wake_return_input(1, 304, keys, 1));
        CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 18, 0, 12, 1, 1, &result));
        CHECK(!result.status && result.value == 0x3b00 && result.serial_low == 20);
        {
            const DWORD modifiers[] = {0, SHIFT_PRESSED, RIGHT_CTRL_PRESSED,
                RIGHT_ALT_PRESSED, SHIFT_PRESSED | RIGHT_CTRL_PRESSED,
                SHIFT_PRESSED | RIGHT_CTRL_PRESSED | RIGHT_ALT_PRESSED};
            const WORD scans[] = {0x85, 0x87, 0x89, 0x8b, 0x89, 0x8b};
            unsigned key, mode;
            for (key = 0; key != 2; ++key) {
                keys[0].scan = 0x57 + key; keys[0].virtual_key = VK_F11 + key;
                for (mode = 0; mode != sizeof(scans) / sizeof(scans[0]); ++mode) {
                    keys[0].control = modifiers[mode];
                    CHECK(!mvdm_vm_wake_return_input(1, 304, keys, 1));
                    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 18, 0, 12, 1, 1, &result));
                    CHECK(!result.status && result.value == (DWORD)((scans[mode] + key) << 8)
                        && result.serial_low == 20);
                }
            }
            keys[0].control = 0;
        }
        keys[0].character = 0x4e00;
        CHECK(!mvdm_vm_wake_return_input(1, 304, keys, 1));
        CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 18, 0, 12, 1, 1, &result));
        CHECK(result.status == ERROR_NO_UNICODE_TRANSLATION);
        CHECK(!mvdm_vm_wake_read_input(1, 304, output, 1, &count, 0) && count == 1);
        CHECK(output[0].character == 0x4e00 && output[0].repeat == 20);
    }
    mvdm_vm_wake_reset();
    CHECK(!mvdm_vm_notification_close());
}

static void executor_reuse_tests(void)
{
    mvdm_vm_executor *executor;
    uint64_t old_token, new_token;
    mvdm_vm_completion completion;
    WORD irq;
    KEY_EVENT_RECORD key = {0};
    key.wRepeatCount = 1;
    executor_freed = executor_retired = 0;
    executor_started = CreateSemaphoreW(NULL, 0, 2, NULL);
    executor_release = CreateEventW(NULL, TRUE, FALSE, NULL);
    executor_retiring = CreateEventW(NULL, TRUE, FALSE, NULL);
    executor_retire_release = CreateEventW(NULL, TRUE, FALSE, NULL);
    application_published = CreateEventW(NULL, FALSE, FALSE, NULL);
    CHECK(executor_started && executor_release && executor_retiring &&
        executor_retire_release && application_published);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_notification_ready());
    CHECK(!mvdm_vm_wake_register(1, 201));
    CHECK(!mvdm_vm_executor_create((ntvdm_vm_host *)1, &executor));
    CHECK(!mvdm_vm_executor_submit(executor, 1, (ntvdm_command_launch *)1,
        executor_text(), &old_token));
    CHECK(WaitForSingleObject(executor_started, 10000) == WAIT_OBJECT_0);
    executor_hold_retirement = TRUE;
    CHECK(!mvdm_vm_wake_retire(1, 201));
    mvdm_vm_continuation_retire(1, 201);
    CHECK(WaitForSingleObject(executor_retiring, 10000) == WAIT_OBJECT_0);
    CHECK(!mvdm_vm_wake_register(1, 202));
    CHECK(!mvdm_vm_executor_submit(executor, 1, (ntvdm_command_launch *)1,
        executor_text(), &new_token));
    CHECK(new_token != old_token);
    CHECK(mvdm_vm_executor_key(executor, 1, old_token, &key) == ERROR_OPERATION_ABORTED);
    CHECK(WaitForSingleObject(executor_started, 0) == WAIT_TIMEOUT);
    CHECK(SetEvent(executor_retire_release));
    CHECK(WaitForSingleObject(executor_started, 10000) == WAIT_OBJECT_0);
    CHECK(!mvdm_vm_executor_key(executor, 1, new_token, &key));
    CHECK(SetEvent(executor_release));
    CHECK(WaitForSingleObject(application_published, 10000) == WAIT_OBJECT_0);
    CHECK(mvdm_vm_continuation_consume(old_token, 1, &completion) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_continuation_consume(new_token, 1, &completion));
    CHECK(!completion.error && completion.exit_code == 1);
    mvdm_vm_executor_destroy(executor);
    CHECK(executor_freed == 2 && executor_retired == 2);
    CHECK(!mvdm_vm_notification_close());
    mvdm_vm_wake_reset();
    CloseHandle(application_published); application_published = NULL;
    CloseHandle(executor_started); CloseHandle(executor_release);
    CloseHandle(executor_retiring); CloseHandle(executor_retire_release);
    executor_hold_retirement = FALSE;
}

int main(void)
{
    WORD irq;
    uint64_t first, second, latest;
    mvdm_vm_bridge_result result = {123, 456, 789, 0};
    mvdm_vm_wake_identity wake;
    producer_context producer = {0};
    HANDLE thread;
    DWORD thread_result;
    InitializeCriticalSection(&ica);
    CHECK(!mvdm_vm_bridge_bop(0, 0, 0, 0, 0, 1, &result));
    CHECK(result.status == 123 && result.value == 456);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 0, 0, 0, 0, 0, &result));
    CHECK(result.status == ERROR_ACCESS_DENIED && !owner);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 99, 0, 0, 0, 1, &result));
    CHECK(result.status == ERROR_INVALID_FUNCTION && !owner);
    CHECK(mvdm_vm_notification_post(&first) == ERROR_NOT_READY && !first);
    CHECK(mvdm_vm_wake_register(1, 1) == ERROR_NOT_READY);
    busy = 1;
    CHECK(mvdm_vm_notification_open(&irq) == ERROR_BUSY && irq == 0xffff);
    busy = 0;
    eoi = occupied;
    CHECK(mvdm_vm_notification_open(&irq) == ERROR_BUSY && !owner);
    eoi = NULL;
    CHECK(!mvdm_vm_notification_open(&irq) && irq == 15);
    CHECK(mvdm_vm_notification_open(&irq) == ERROR_ALREADY_EXISTS);
    CHECK(!mvdm_vm_notification_post(&first) && raises == 0);
    CHECK(!mvdm_vm_notification_ready() && raises == 1);
    CHECK(!mvdm_vm_notification_post(&second) && second > first && raises == 1);
    eoi(15, -1);
    CHECK(raises == 2); /* reset must not lose unacknowledged work */
    CHECK(!mvdm_vm_notification_snapshot(&latest) && latest == second);
    CHECK(mvdm_vm_notification_acknowledge(second + 1) == ERROR_INVALID_DATA);
    CHECK(!mvdm_vm_notification_acknowledge(first));
    eoi(15, 0);
    CHECK(raises == 3); /* old ack cannot consume the newer post */
    CHECK(!mvdm_vm_notification_acknowledge(second));
    eoi(15, 0);
    CHECK(raises == 3);
    CHECK(mvdm_vm_notification_acknowledge(first) == ERROR_INVALID_DATA);
    CHECK(!mvdm_vm_notification_post(&latest) && raises == 4);
    CHECK(!mvdm_vm_notification_close() && raises == 4 && cancels == 1);
    CHECK(!owner && !eoi);
    CHECK(!mvdm_vm_notification_close() && cancels == 1);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(mvdm_vm_notification_acknowledge(first) == ERROR_INVALID_DATA);
    CHECK(!mvdm_vm_notification_post(&second) && second > latest);
    CHECK(!mvdm_vm_notification_close());
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 0, 0, 0, 0, 1, &result));
    CHECK(!result.status && result.value == 15);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 2, 0, 0, 0, 1, &result));
    CHECK(!result.status && result.serial_low > (uint32_t)second);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 5, 0, 0, 0, 1, &result));
    CHECK(!result.status && !owner);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_notification_ready());
    CHECK(!mvdm_vm_wake_register(1, 11));
    CHECK(!mvdm_vm_wake_register(2, 12));
    CHECK(mvdm_vm_wake_register(1, 99) == ERROR_ALREADY_EXISTS);
    CHECK(mvdm_vm_wake_publish(1, 99, &latest) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_wake_publish(1, 11, &first));
    CHECK(!mvdm_vm_wake_publish(2, 12, &second) && second > first);
    CHECK(!mvdm_vm_wake_publish(1, 11, &latest));
    CHECK(!mvdm_vm_wake_retire(1, 11));
    CHECK(!mvdm_vm_wake_take(&wake) && wake.vm == 2 && wake.generation == 12);
    CHECK(mvdm_vm_wake_take(&wake) == ERROR_NO_MORE_ITEMS);
    CHECK(!mvdm_vm_wake_publish(2, 12, &latest));
    CHECK(mvdm_vm_wake_retire(2, 11) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_wake_take(&wake) && wake.vm == 2);
    CHECK(!mvdm_vm_wake_register(1, 22));
    CHECK(mvdm_vm_wake_publish(1, 11, &latest) == ERROR_NOT_FOUND);
    CHECK(!mvdm_vm_wake_publish(1, 22, &latest));
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 5, 0, 0, 0, 1, &result));
    CHECK(!result.status && !owner);
    CHECK(mvdm_vm_wake_take(&wake) == ERROR_NO_MORE_ITEMS);
    CHECK(mvdm_vm_wake_retire(1, 22) == ERROR_NOT_FOUND);
    CHECK(mvdm_vm_wake_register(1, 23) == ERROR_NOT_READY);
    CHECK(!mvdm_vm_notification_open(&irq));
    CHECK(!mvdm_vm_notification_ready());
    CHECK(!mvdm_vm_wake_register(3, 30));
    producer.published = CreateEventW(NULL, FALSE, FALSE, NULL);
    producer.resume = CreateEventW(NULL, FALSE, FALSE, NULL);
    CHECK(producer.published && producer.resume);
    thread = CreateThread(NULL, 0, publish_from_host, &producer, 0, NULL);
    CHECK(thread != NULL);
    CHECK(WaitForSingleObject(producer.published, INFINITE) == WAIT_OBJECT_0);
    CHECK(!producer.first_error);
    /* Retire with an unread completion, then reuse the same VM handle.
     * The old producer must not publish into the new generation. */
    CHECK(!mvdm_vm_wake_retire(3, 30));
    CHECK(!mvdm_vm_wake_register(3, 31));
    CHECK(SetEvent(producer.resume));
    CHECK(WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread, &thread_result) && !thread_result);
    CHECK(producer.retired_error == ERROR_NOT_FOUND && !producer.reused_error);
    CHECK(!mvdm_vm_wake_take(&wake) && wake.vm == 3 && wake.generation == 31);
    CHECK(mvdm_vm_wake_take(&wake) == ERROR_NO_MORE_ITEMS);
    CloseHandle(thread);
    CloseHandle(producer.resume);
    CloseHandle(producer.published);
    CHECK(mvdm_vm_bridge_bop(MVDM_VM_BRIDGE_SIGNATURE, 5, 0, 0, 0, 1, &result));
    CHECK(!result.status && !owner);
    CHECK(!depth);
    continuation_tests();
    executor_tests();
    executor_reuse_tests();
    executor_registration_failure_tests();
    returned_input_tests();
    DeleteCriticalSection(&ica);
    printf("PASS %ld notification unit assertions; no real PIC claim\n", assertions);
    return 0;
}
