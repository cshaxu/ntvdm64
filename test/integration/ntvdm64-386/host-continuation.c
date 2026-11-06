#include "mvdm_vm_io.h"
#include "mvdm_vm_executor.h"
#include "mvdm_standalone_worker.h"
#include "mvdm_vm_bridge.h"
#include "mvdm_vm_wake.h"
#include "mvdm_vm_continuation.h"
#include "mvdm_guest_location.h"
#include "mvdm_softpc_guest_memory.h"
#include "mvdm_vm_command.h"
#include "ntvdm-exe/command/include/mvdm_command_native_child.h"
#include "ntvdm-exe/session/session.h"
#include "common/protocol/console_io.h"
#include "ntvdm-exe/frontend/vm_text.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

/* Diagnostic link only. Interpose CmdDispatch in a separately compiled
 * nt_bop.obj, and rename the normal notification BOP entry as below. Actual
 * default marked probes receive diagnostic AL=25h; MVDM_REAL_COMMAND instead
 * uses the registered VM provider and real Run16/NTVWM execution. The
 * fixed-result mode stays diagnostic. REAL_COMMAND delegates BOP11 admission
 * to production and supplies only translated input stimulus. */
extern BOOL CmdDispatch(ULONG);
extern uint32_t c_getEAX(void), c_getEDX(void);
extern uint16_t c_getIP(void);
extern uint16_t c_getDS(void), c_getDX(void), c_getCX(void), c_getAX(void);
extern uint16_t c_getBX(void), c_getES(void), c_getBP(void);
extern void video_io(void), ega_video_io(void);
extern void mouse_video_io(void);
extern int c_getCF(void);
extern void demRead(void);
extern void nt_key_down_action(PKEY_EVENT_RECORD);
extern void nt_key_up_action(PKEY_EVENT_RECORD);
extern void host_ica_lock(void), host_ica_unlock(void);
extern void c_setIP(uint16_t);
extern void CALLF(uint32_t target[2]);
extern int mvdm_vm_bridge_bop_direct(uint32_t, uint32_t, uint32_t,
    uint32_t, uint32_t, int, mvdm_vm_bridge_result *);
typedef struct {
    mvdm_vm_continuation *executor;
    mvdm_command_capture *capture;
    uint32_t service, route, vm;
    ntvdm_vm_text *text;
    char role;
} diagnostic_request;
static diagnostic_request *waiting;
static uint32_t waiting_service;
static HANDLE blocked_keyboard_ready;
static volatile LONG keyboard_consumer;
static DWORD stage_video(ntvdm_vm_text *text, char role)
{
    ntvdm_vm_text_publication view;
    const ntvdm_vm_text_header *h;
    console_video_description description = {0};
    console_text_style *style;
    console_io_request request = {0}; console_io_reply reply = {0};
    BYTE *payload;
    uint32_t offset, i;
    DWORD error = ntvdm_vm_text_candidate(text, &view);
    if (error) return error;
    h = view.text;
    description.width = h->columns; description.height = h->rows;
    description.stride = h->columns * 2;
    description.bytes = sizeof(*style) + description.stride * h->rows;
    description.kind = CONSOLE_VIDEO_TEXT_FRAME;
    for (i = 0; i < 16; ++i) description.palette[i] = i * 0x111111;
    description.palette[7] = role == 'A' ? 0xff0000 : 0x0000ff;
    payload = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, description.bytes);
    if (!payload) return ERROR_NOT_ENOUGH_MEMORY;
    style = (console_text_style *)payload;
    style->font_height = h->font_height;
    style->attribute_font_select = 1;
    style->cursor_column = h->x; style->cursor_row = h->y;
    style->cursor_start = 6; style->cursor_height = 2; style->cursor_visible = 1;
    /* Deliberately distinct glyphs for a later pixel/readback witness. Palette
     * and cell readback below do not by themselves prove displayed glyphs. */
    memset(style->fonts, role == 'A' ? 0x3c : 0x66, sizeof(style->fonts));
    memcpy(payload + sizeof(*style), h + 1, h->columns * h->rows * 2);
    request.operation = CONSOLE_IO_VIDEO_BEGIN; request.state.mode = 1;
    request.bytes = sizeof(description); request.data = (BYTE *)&description;
    error = ntvdm_vm_text_dispatch(text, &request, &reply);
    for (offset = 0; !error && offset < description.bytes; offset += request.bytes) {
        request.operation = CONSOLE_IO_VIDEO_DATA; request.state.count = offset;
        request.bytes = min(CONSOLE_IO_TILE_BYTES, description.bytes - offset);
        request.data = payload + offset;
        error = ntvdm_vm_text_dispatch(text, &request, &reply);
    }
    HeapFree(GetProcessHeap(), 0, payload);
    return error;
}

typedef struct { diagnostic_request *first, *second; } completion_pair;
static void trace(unsigned operation, uint32_t vm, uint32_t token);
/* Optional bios.obj link interposition; observe arguments, then call the
 * unchanged original BIOS entry. No guest register or device mutation. */
static void trace_video(void (*entry)(void), unsigned operation)
{
    if ((c_getAX() >> 8) == 0x11) {
        trace(operation, c_getAX(), c_getBX());
        trace(operation + 1, c_getCX(), c_getDX());
        trace(operation + 2, c_getES(), c_getBP());
    }
    entry();
}
void test_video_io(void) { trace_video(video_io, 0x320); }
void test_ega_video_io(void) { trace_video(ega_video_io, 0x324); }
void test_mouse_video_io(void) { trace_video(mouse_video_io, 0x328); }
/* Optional loader diagnostic: preserve the real DEM read and record its
 * requested extent, destination and actual DOS result. */
void test_dem_read(void)
{
    trace(0x310, ((uint32_t)c_getDS() << 16) | c_getDX(), c_getCX());
    demRead();
    trace(0x311, c_getAX(), (uint32_t)c_getCF());
}
extern int xtrn2phy(uint32_t address, uint8_t access, uint32_t *physical);
extern uint32_t c_sas_memory_size(void);
extern void c_sas_store(uint32_t address, uint8_t value);
extern uint8_t c_sas_hw_at(uint32_t address);
extern uint32_t c_getCPL(void);
extern uint32_t c_getVM(void);

/* Optional ega_vide.obj interposition: sample each font bank's first write
 * while the original BIOS still owns its font-plane mapping. */
void test_font_store(uint32_t address, uint8_t value)
{
    c_sas_store(address, value);
    if (address == 0xa0000 || address == 0xa4000) {
        uint32_t physical = 0;
        int mapped = xtrn2phy(address, 0, &physical);
        trace(0x330, address, value);
        trace(0x331, (uint32_t)mapped, physical);
        trace(0x332, address, c_sas_hw_at(address));
        trace(0x333, c_getCPL(), c_getVM());
        physical = 0;
        mapped = xtrn2phy(address, 5, &physical);
        trace(0x334, (uint32_t)mapped, physical);
        physical = 0;
        mapped = xtrn2phy(address, 7, &physical);
        trace(0x335, (uint32_t)mapped, physical);
    }
}

/* Optional diagnostic link interposition in the lease preflight only. */
int test_xtrn2phy(uint32_t address, uint8_t access, uint32_t *physical)
{
    int result = xtrn2phy(address, access, physical);
    if (!result) {
        uint32_t relaxed = 0;
        int mapped = xtrn2phy(address, (uint8_t)(access & ~4u), &relaxed);
        trace(0x300, address, access);
        trace(0x301, (uint32_t)mapped, relaxed);
    }
    return result;
}

/* Deterministic provider-boundary stimulus, not a physical guest keyboard
 * translation claim. The interactive COMSPEC must really consume and execute
 * this line; no process completion or exit code is substituted. */
static DWORD interactive_input(mvdm_vm_executor *executor, uint64_t token)
{
    const char *line = "exit\r";
    unsigned i;
    if (GetEnvironmentVariableW(L"MVDM_NESTED_COMMAND", NULL, 0))
        line = "mem\rexit\r";
    if (GetEnvironmentVariableW(L"MVDM_REENTRANT_COMMAND", NULL, 0))
        line = "command /c cmd /c mem\rexit\r";
    for (i = 0; line[i]; ++i) {
        KEY_EVENT_RECORD key = {0};
        DWORD error;
        key.bKeyDown = TRUE; key.wRepeatCount = 1;
        /* The witness contains only ASCII letters, space, slash and Enter.
         * Map its native key records without asserting physical guest input. */
        key.wVirtualKeyCode = line[i] == '\r' ? VK_RETURN :
            line[i] == ' ' ? VK_SPACE : line[i] == '/' ? VK_OEM_2 :
            (WORD)(line[i] - 'a' + 'A');
        key.wVirtualScanCode = (WORD)MapVirtualKeyW(key.wVirtualKeyCode, MAPVK_VK_TO_VSC);
        key.uChar.UnicodeChar = (WCHAR)line[i];
        error = mvdm_vm_executor_key(executor, token, &key);
        if (error) return error;
        key.bKeyDown = FALSE;
        error = mvdm_vm_executor_key(executor, token, &key);
        if (error) return error;
    }
    return ERROR_SUCCESS;
}
static void complete_one(diagnostic_request *request)
{
    mvdm_command_capture_view view;
    const mvdm_vm_completion completion = {0, 0x25, 0};
    char environment[] = "ROLE=X", command[] = "echo X";
    char directory[] = "\\VX";
    unsigned expected_drive = GetEnvironmentVariableW(L"MVDM_CONTINUATION_LOCAL", NULL, 0) ? 2 : 14;
    environment[5] = command[5] = request->role;
    directory[2] = request->role;
    {
        console_io_request read = {0}; console_io_reply reply = {0};
        console_io_cell cell;
        read.operation = CONSOLE_IO_READ_CELLS_W;
        read.state.width = read.state.height = 1;
        reply.data = (BYTE *)&cell;
        if (ntvdm_vm_text_dispatch(request->text, &read, &reply) ||
            (!GetEnvironmentVariableW(L"MVDM_SCREEN_UNSEEDED", NULL, 0) &&
            (cell.character != (uint16_t)request->role || cell.attribute != 0x1e)))
            ExitProcess(ERROR_INVALID_DATA);
    }
    if (!mvdm_command_capture_read(request->capture, &view) ||
        strcmp(view.environment, environment) || view.environment_bytes != 8 ||
        (request->service == 8 && strcmp(view.command, command)) ||
        view.service != request->route ||
        view.standard_handles[0] != 0x11111111 ||
        view.standard_handles[1] != 0x22222222 ||
        view.standard_handles[2] != 0x33333333 ||
        view.default_drive != expected_drive || !view.directory ||
        !strrchr(view.directory, '\\') || strcmp(strrchr(view.directory, '\\'), directory))
        ExitProcess(ERROR_INVALID_DATA);
    mvdm_command_capture_destroy(request->capture);
    {
        void *application;
        uint32_t application_bytes;
        ntvdm_vm_text *text = request->text;
        console_io_request write = {0}; console_io_reply reply = {0};
        console_io_cell cell = {(uint16_t)(request->role + ('a' - 'A')), 0x2f};
        write.operation = CONSOLE_IO_PUBLICATION_BEGIN;
        if (ntvdm_vm_text_dispatch(text, &write, &reply)) ExitProcess(ERROR_INVALID_STATE);
        write.operation = CONSOLE_IO_WRITE_CELLS_W;
        write.state.width = write.state.height = 1;
        write.bytes = sizeof(cell); write.data = (uint8_t *)&cell;
        if (ntvdm_vm_text_dispatch(text, &write, &reply)) ExitProcess(ERROR_WRITE_FAULT);
        ZeroMemory(&write, sizeof(write)); write.operation = CONSOLE_IO_CURSOR_POSITION;
        write.state.x = 4; write.state.y = 5;
        if (ntvdm_vm_text_dispatch(text, &write, &reply)) ExitProcess(ERROR_INVALID_STATE);
        write.operation = CONSOLE_IO_SCREEN_INFO;
        if (ntvdm_vm_text_dispatch(text, &write, &reply)) ExitProcess(ERROR_INVALID_STATE);
        write.operation = CONSOLE_IO_WINDOW_RECT;
        write.state.mode = 1; write.state.left = write.state.top = 0;
        write.state.right = reply.state.right; write.state.bottom = reply.state.bottom;
        if (ntvdm_vm_text_dispatch(text, &write, &reply)) ExitProcess(ERROR_INVALID_STATE);
        write.operation = CONSOLE_IO_ATTRIBUTE; write.state.attribute = 0x2f;
        if (ntvdm_vm_text_dispatch(text, &write, &reply)) ExitProcess(ERROR_INVALID_STATE);
        if (GetEnvironmentVariableA("MVDM_VIDEO_APPLICATION", NULL, 0) &&
            stage_video(text, request->role)) ExitProcess(ERROR_INVALID_DATA);
        if (ntvdm_vm_text_copy_application(text, &application, &application_bytes)) ExitProcess(ERROR_INVALID_STATE);
        if (GetEnvironmentVariableA("MVDM_VIDEO_APPLICATION", NULL, 0)) {
            const ntvdm_vm_text_header *h = application;
            BYTE *version = (BYTE *)application + sizeof(*h) + h->columns * h->rows * 2 + 4;
            uint32_t bad = 2, good = NTVDM_VM_VIDEO_VERSION;
            memcpy(version, &bad, sizeof(bad));
            if (mvdm_vm_application_publish(request->executor, application, application_bytes, NULL) != ERROR_INVALID_DATA)
                ExitProcess(ERROR_INVALID_DATA);
            memcpy(version, &good, sizeof(good));
        }
        {
            DWORD error;
            ZeroMemory(&write, sizeof(write));
            write.operation = CONSOLE_IO_PUBLICATION_END;
            error = mvdm_vm_application_dispatch(request->executor, text, NULL, &write, &reply);
            if (!error) error = reply.error;
            trace(0x340, (uint32_t)(unsigned char)request->role, error);
            if (error) ExitProcess(error);
        }
        HeapFree(GetProcessHeap(), 0, application);
        ntvdm_vm_text_destroy(text);
    }
    if (mvdm_vm_continuation_complete(request->executor, &completion))
        ExitProcess(ERROR_INVALID_STATE);
    mvdm_vm_continuation_release(request->executor);
    HeapFree(GetProcessHeap(), 0, request);
}

static diagnostic_request *capture(uint32_t descriptor_address, uint32_t descriptor_bytes)
{
    uint32_t descriptor[5], frame_address, base;
    uint32_t frame[18]; /* original VMM Client_Reg_Struc through Client_GS */
    guest_memory_lease *lease;
    uint8_t *bytes;
    session *owner = session_thread_current();
    mvdm_guest_location location;
    mvdm_guest_location_lease service_lease;
    diagnostic_request *request;
    DWORD error;
    uint32_t physical = 0;
    if (descriptor_bytes != sizeof(descriptor) ||
        !session_guest_memory_acquire(owner, descriptor_address, sizeof(descriptor),
            GUEST_MEMORY_ACCESS_READ, &lease, &bytes)) ExitProcess(ERROR_INVALID_ADDRESS);
    memcpy(descriptor, bytes, sizeof(descriptor));
    if (!session_guest_memory_release(owner, lease, 0)) ExitProcess(ERROR_INVALID_ADDRESS);
    frame_address = descriptor[0]; base = descriptor[1];
    trace(0x200, frame_address, base);
    trace(0x201, c_sas_memory_size(),
        (uint32_t)xtrn2phy(frame_address, 0, &physical));
    trace(0x202, frame_address, physical);
    if (!session_guest_memory_acquire(owner, frame_address, sizeof(frame),
            GUEST_MEMORY_ACCESS_READ, &lease, &bytes)) ExitProcess(ERROR_INVALID_ADDRESS);
    memcpy(frame, bytes, sizeof(frame));
    if (!session_guest_memory_release(owner, lease, 0)) ExitProcess(ERROR_INVALID_ADDRESS);
    trace(0x209, frame[10], frame[9]);
    request = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*request));
    if (!request) ExitProcess(ERROR_NOT_ENOUGH_MEMORY);
    mvdm_guest_location_set_real_mode(&location, (uint16_t)frame[10], (uint16_t)(frame[9]-1));
    if (!mvdm_guest_location_acquire_vm(&location, base, 1, GUEST_MEMORY_ACCESS_READ,
            &service_lease)) ExitProcess(ERROR_INVALID_ADDRESS);
    request->service = service_lease.bytes[0];
    if (!mvdm_guest_location_release(&service_lease, 0)) ExitProcess(ERROR_INVALID_ADDRESS);
    if ((request->service != 8 && request->service != 10) ||
        (frame[7] & 0xffff0000u) != 0x54320000u) ExitProcess(ERROR_INVALID_DATA);
    request->route = request->service == 10 ? MVDM_COMMAND_SERVICE_INTERACTIVE :
        (frame[7] & 0xff00u) ? MVDM_COMMAND_SERVICE_EXECUTE : MVDM_COMMAND_SERVICE_NONE;
    request->role = (char)frame[6];
    if (request->role != 'A' && request->role != 'B') ExitProcess(ERROR_INVALID_DATA);
    error = mvdm_vm_command_snapshot(request->service, descriptor_address,
        descriptor_bytes, &request->capture, &request->text);
    trace(0x20a, request->service, error);
    if (error) ExitProcess(error);
    {
        console_io_request read = {0}; console_io_reply reply = {0};
        read.operation = CONSOLE_IO_SCREEN_INFO;
        if (ntvdm_vm_text_dispatch(request->text, &read, &reply)) ExitProcess(ERROR_INVALID_DATA);
        if (!GetEnvironmentVariableW(L"MVDM_SCREEN_UNSEEDED", NULL, 0) &&
            (reply.state.x != 2 || reply.state.y != 3)) ExitProcess(ERROR_INVALID_DATA);
        trace(0x208, reply.state.width, reply.state.height);
    }
    {
        mvdm_command_capture_view view;
        size_t i, length;
        if (!mvdm_command_capture_read(request->capture, &view)) ExitProcess(ERROR_INVALID_DATA);
        trace(0x203, frame[7], view.service);
        trace(0x206, (uint32_t)(unsigned char)request->role, frame[6]);
        length = view.directory ? strlen(view.directory) : 0;
        for (i = 0; i < length; ++i) trace(0x204, (uint32_t)i, (uint8_t)view.directory[i]);
        mvdm_guest_location_set_real_mode(&location, (uint16_t)frame[15], (uint16_t)frame[0]);
        if (!mvdm_guest_location_acquire_vm(&location, base, 76,
            GUEST_MEMORY_ACCESS_READ, &service_lease)) ExitProcess(ERROR_INVALID_ADDRESS);
        for (i = 0; i < 64 && service_lease.bytes[i]; ++i)
            trace(0x205, (uint32_t)i, service_lease.bytes[i]);
        for (i = 64; i < 76; i += 4) {
            uint32_t value;
            memcpy(&value, service_lease.bytes + i, sizeof(value));
            trace(0x207, (uint32_t)i, value);
        }
        if (!mvdm_guest_location_release(&service_lease, 0)) ExitProcess(ERROR_INVALID_ADDRESS);
    }
    return request;
}

static VOID CALLBACK complete_pair(PTP_CALLBACK_INSTANCE instance, void *context)
{
    completion_pair *pair = context;
    (void)instance;
    /* Second arrival completes first. Both references remain owned until the
     * publishing call returns, even if the CPU consumes/retires immediately. */
    if (pair->first->role == pair->second->role) ExitProcess(ERROR_INVALID_DATA);
    if (blocked_keyboard_ready) {
        if (WaitForSingleObject(blocked_keyboard_ready, INFINITE) != WAIT_OBJECT_0)
            ExitProcess(GetLastError());
    }
    /* Diagnostic observation interval only; never linked into the product.
     * Keep both requests pending long enough to distinguish interrupt service
     * from a completion that happens before the next guest timer tick. */
    if (GetEnvironmentVariableW(L"MVDM_INTERRUPT_OBSERVATION", NULL, 0)) Sleep(500);
    InterlockedExchange(&keyboard_consumer, 0);
    trace(19, 0, 0); /* before any host I/O/completion wake for this pair */
    complete_one(pair->second);
    complete_one(pair->first);
    HeapFree(GetProcessHeap(), 0, pair);
}

static VOID CALLBACK complete_single(PTP_CALLBACK_INSTANCE instance, void *context)
{ (void)instance; complete_one(context); }

/* Test evidence only: retain the last reached lifecycle boundary even when
 * the outer integration observer has to stop a guest that did not exit. */
static void trace(unsigned operation, uint32_t vm, uint32_t token)
{
    static SRWLOCK lock = SRWLOCK_INIT;
    char path[MAX_PATH], line[96];
    DWORD length, written;
    HANDLE file;
    length = GetEnvironmentVariableA("MVDM_CONTINUATION_TRACE", path, sizeof(path));
    if (!length || length >= sizeof(path)) return;
    AcquireSRWLockExclusive(&lock);
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) ExitProcess(GetLastError());
    length = (DWORD)sprintf_s(line, sizeof(line), "%u %08lx %08lx\n",
        operation, (unsigned long)vm, (unsigned long)token);
    if (!WriteFile(file, line, length, &written, NULL) || written != length)
        ExitProcess(ERROR_WRITE_FAULT);
    CloseHandle(file);
    ReleaseSRWLockExclusive(&lock);
}

BOOL test_cmd_dispatch(ULONG service)
{
    uint32_t callback, target[2];
    if (service == 254) {
        trace(254, c_getEAX(), c_getIP());
        return TRUE;
    }
    if ((service != 8 && service != 10) ||
        (c_getEAX() & 0xffff0000u) != 0x54320000u) {
        trace(0x100 + service, 0, 0);
        return CmdDispatch(service);
    }
    callback = c_getEDX();
    if (GetEnvironmentVariableW(L"MVDM_REAL_COMMAND", NULL, 0))
        ExitProcess(ERROR_INVALID_STATE); /* production ingress must intercept */
    c_setIP((uint16_t)(c_getIP() + 1));
    /* This diagnostic interposition returns through the original MS_bop_4
     * +1 epilogue. Production admission must instead bypass that epilogue. */
    target[0] = (uint16_t)(callback - 1);
    target[1] = callback >> 16;
    CALLF(target); /* original CCPU validation, stack push and segment load */
    return TRUE;
}

int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t low, uint32_t high, uint32_t vm, int ring0,
    mvdm_vm_bridge_result *result)
{
    uint64_t token = ((uint64_t)high << 32) | low;
    if (signature != MVDM_VM_BRIDGE_SIGNATURE || !ring0)
        return mvdm_vm_bridge_bop_direct(signature, operation, low, high,
            vm, ring0, result);
    if (operation == 16) {
        trace(0x400 + low, vm, high);
        ZeroMemory(result, sizeof(*result));
        return 1;
    }
    if (operation == 18) {
        trace(18, vm, high);
        if (GetEnvironmentVariableW(L"MVDM_CONSUME_KEYBOARD", NULL, 0))
            InterlockedExchange(&keyboard_consumer, (LONG)vm);
        ZeroMemory(result, sizeof(*result));
        return 1;
    }
    if (operation == 20) {
        trace(20, vm, low);
        trace(21, vm, high);
        ZeroMemory(result, sizeof(*result));
        if (vm == (uint32_t)InterlockedCompareExchange(&keyboard_consumer, 0, 0)) {
            trace(22, vm, low); /* diagnostic receipt, not native delivery */
            result->status = 1;
        }
        return 1;
    }
    if (operation == 17) {
        KEY_EVENT_RECORD key = {0};
        ZeroMemory(result, sizeof(*result));
        /* Test-only stimulus through the original scan-code conversion and
         * controller. No BIOS buffer writes or fabricated interrupt/result. */
        if ((low != 'A' || high != 0x1e) &&
            (low != 'B' || high != 0x30)) {
            result->status = ERROR_INVALID_PARAMETER;
            return 1;
        }
        key.wVirtualKeyCode = (WORD)low;
        key.wVirtualScanCode = (WORD)high;
        key.wRepeatCount = 1;
        host_ica_lock();
        key.bKeyDown = TRUE;
        nt_key_down_action(&key);
        key.bKeyDown = FALSE;
        nt_key_up_action(&key);
        host_ica_unlock();
        trace(17, vm, low);
        if (low == 'A' && blocked_keyboard_ready && !SetEvent(blocked_keyboard_ready))
            ExitProcess(GetLastError());
        return 1;
    }
    if (operation == 5 || operation == 7 || operation == 8 ||
        operation == 11 || operation == 12) trace(operation, vm, low);
    if (operation == 11 && GetEnvironmentVariableW(L"MVDM_REAL_COMMAND", NULL, 0)) {
        uint32_t service;
        DWORD error = mvdm_vm_command_service(low, high, &service);
        if (error) ExitProcess(error);
        mvdm_vm_bridge_bop_direct(signature, operation, low, high, vm, ring0, result);
        trace(0x351, vm, result->status);
        if (result->status) ExitProcess(result->status);
        token = ((uint64_t)result->serial_low << 32) | result->value;
        if (service == 10) {
            error = interactive_input(mvdm_standalone_worker_executor(), token);
            if (error) ExitProcess(error);
        }
        return 1;
    }
    if (operation == 11) {
        if (!blocked_keyboard_ready &&
            GetEnvironmentVariableW(L"MVDM_BLOCKED_KEYBOARD", NULL, 0)) {
            blocked_keyboard_ready = CreateEventW(NULL, TRUE, FALSE, NULL);
            if (!blocked_keyboard_ready) ExitProcess(GetLastError());
        }
        ZeroMemory(result, sizeof(*result));
        diagnostic_request *request = capture(low, high);
        request->vm = vm;
        result->status = mvdm_vm_continuation_begin(vm, &token, &request->executor);
        if (result->status) ExitProcess(result->status);
        result->value = (uint32_t)token;
        result->serial_low = (uint32_t)(token >> 32);
        if (GetEnvironmentVariableW(L"MVDM_CONTINUATION_DEFER", NULL, 0)) {
            if (!waiting) {
                waiting = request;
                waiting_service = request->route;
            } else {
                completion_pair *pair;
                if (waiting_service != request->route) ExitProcess(ERROR_INVALID_STATE);
                pair = HeapAlloc(GetProcessHeap(), 0, sizeof(*pair));
                if (!pair) ExitProcess(ERROR_NOT_ENOUGH_MEMORY);
                pair->first = waiting;
                pair->second = request;
                waiting = NULL;
                if (!TrySubmitThreadpoolCallback(complete_pair, pair, NULL))
                    ExitProcess(GetLastError());
            }
        } else {
            if (!TrySubmitThreadpoolCallback(complete_single, request, NULL))
                ExitProcess(GetLastError());
        }
        return 1;
    }
    return mvdm_vm_bridge_bop_direct(signature, operation, low, high,
        vm, ring0, result);
}
