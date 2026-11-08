#include "mvdm_vm_io.h"
#include "ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.h"
#include "mvdm_vm_executor.h"
#include "mvdm_standalone_worker.h"
#include "mvdm_vm_bridge.h"
#include "mvdm_vm_wake.h"
#include "mvdm_vm_continuation.h"
#include "mvdm_guest_location.h"
#include "mvdm_softpc_guest_memory.h"
#include "mvdm_vm_command.h"
#include "ntvdm-exe/session/session.h"
#include "ntvdm-exe/command/include/mvdm_command_native_child.h"
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
extern int mvdm_command_guest_state_scs_field(uint32_t, uint16_t *, uint16_t *);
extern uint32_t c_getEAX(void), c_getEDX(void), c_getESI(void);
extern uint16_t c_getIP(void), c_getCS(void);
extern uint16_t c_getDS(void), c_getDX(void), c_getCX(void), c_getAX(void), c_getSI(void);
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
    char guest_directory[67];
} diagnostic_request;
static diagnostic_request *waiting;
static uint32_t waiting_service;
static HANDLE blocked_keyboard_ready;
static volatile LONG keyboard_consumer;
static void trace(unsigned operation, uint32_t vm, uint32_t token);
static uint32_t test_original_cds_buffer;
/* Read-only original DEM directory service observation, diagnostic link only. */
extern BOOL DemDispatch(ULONG);
BOOL test_dem_dispatch(ULONG service)
{
    BOOL result;
    if (service == 0x1b) {
        mvdm_guest_location location;
        mvdm_guest_location_lease lease;
        uint16_t ds = c_getDS();
        uint32_t field;
        test_original_cds_buffer = 0;
        if (mvdm_guest_location_set_real_mode(&location, ds, c_getSI()) &&
            mvdm_guest_location_acquire(&location, 36, GUEST_MEMORY_ACCESS_READ, &lease)) {
            memcpy(&field, lease.bytes + 8, sizeof(field));
            test_original_cds_buffer = ((uint32_t)ds << 4) + (field & 0xffffu);
            trace(0xc50, ((uint32_t)ds << 16) | c_getSI(), field);
            trace(0xc51, test_original_cds_buffer, 0);
            (void)mvdm_guest_location_release(&lease, 0);
        }
    }
    result = DemDispatch(service);
    if (service == 0x13 || service == 0x18 || service == 0x1a) {
        DWORD saved = GetLastError(), length, i;
        char path[MAX_PATH];
        trace(0xc00, service, (uint32_t)c_getCF());
        trace(0xc01, c_getCS(), c_getIP());
        length = GetCurrentDirectoryA(sizeof(path), path);
        if (length && length < sizeof(path))
            for (i = 0; i < length; ++i) trace(0xc02, i, (uint8_t)path[i]);
        SetLastError(saved);
    }
    return result;
}

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
    if (GetEnvironmentVariableA("MVDM_VIDEO_DISTINCT_FONTS", NULL, 0))
        memset(style->fonts[1], role == 'A' ? 0x5a : 0xa5, sizeof(style->fonts[1]));
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
/* Diagnostic-only pass-through for the original INT33 and enhanced mouse
 * owners. Observe their real calls; never synthesize a callback or result. */
extern void mouse_io_interrupt(void), mouse_EM_callback(void);
extern void mouse_int1(void);
extern uint8_t c_sas_hw_at(uint32_t address);
extern void virtual_device_trap(void);
extern uint32_t c_getEBX(void);
extern void c_setEDX(uint32_t);
void test_virtual_device_trap(void)
{
    uint32_t operation=c_getEAX();
    if (operation == 0x4e530001u || operation == 0x4e530002u) {
        uint16_t segment = 0, offset = 0;
        int found = mvdm_command_guest_state_scs_field(0, &segment, &offset);
        trace(0xc40, (uint32_t)found, ((uint32_t)segment << 16) | offset);
        trace(0xc52, test_original_cds_buffer, 0);
        if (operation == 0x4e530002u)
            c_setEDX(test_original_cds_buffer);
        return;
    }
    if(operation!=0x4e544d56) {
        trace(0x590,operation,c_getEBX());
        trace(0x591,GetCurrentProcessId(),c_sas_hw_at(0x4ac));
    }
    virtual_device_trap();
    if(operation!=0x4e544d56)
        trace(0x592,c_getEAX(),c_getCF());
}
void test_mouse_record(DWORD pid,const MOUSE_EVENT_RECORD *mouse,DWORD width,DWORD height)
{
    trace(0x5a0,pid,mouse->dwEventFlags);
    trace(0x5a1,(uint32_t)mouse->dwMousePosition.X,(uint32_t)mouse->dwMousePosition.Y);
    trace(0x5a2,mouse->dwButtonState,mouse->dwControlKeyState);
    trace(0x5a3,width,height);
}
void test_mouse_mode(unsigned after, unsigned mode, unsigned instance_mode, unsigned cs, unsigned ip)
{
    trace(0x570+after*3,mode,instance_mode | (GetCurrentProcessId()<<8));
    trace(0x571+after*3,cs,ip);
    trace(0x572+after*3,c_sas_hw_at(0x4ac),c_sas_hw_at(0x449));
}
void test_mouse_coordinate_state(unsigned video, unsigned instance_mode, unsigned x, unsigned y)
{
    trace(0x564,video,instance_mode | (GetCurrentProcessId()<<8));
    trace(0x565,x,y);
}
void test_mouse_coordinates(unsigned mode, int dx, int dy, int old_x, int old_y, int x, int y)
{
    {
        trace(0x560,mode,c_sas_hw_at(0x4ac));
        trace(0x561,(uint32_t)dx,(uint32_t)dy);
        trace(0x562,(uint32_t)old_x,(uint32_t)old_y);
        trace(0x563,(uint32_t)x,(uint32_t)y);
    }
}
void test_mouse_int1(void)
{
    mvdm_mouse_bridge *state=mvdm_softpc_mouse_current();
    trace(0x550,c_sas_hw_at(0x4ac),state?state->queue.count:0);
    if(state && state->queue.count) {
        mvdm_mouse_input_sample *sample=&state->queue.samples[state->queue.head];
        trace(0x551,(uint32_t)sample->dx,(uint32_t)sample->dy);
    }
    mouse_int1();
    trace(0x552,c_getCS(),c_getIP());
    trace(0x553,GetCurrentProcessId(),c_getAX());
    trace(0x554,c_getCX(),c_getDX());
    trace(0x555,c_getBX(),c_getSI());
}
void test_mouse_io_interrupt(void)
{
    uint16_t function=c_getAX();
    if(function==0 || function==4 || function==0x0c || function==0x14) {
        trace(0x516,GetCurrentProcessId(),c_sas_hw_at(0x4ac));
        trace(0x517,c_getCS(),c_getIP());
        trace(0x510,function,c_getCX());
        trace(0x511,c_getES(),c_getDX());
    }
    mouse_io_interrupt();
    if(function==0 || function==0x0c || function==0x14)
        trace(0x512,c_getAX(),c_getBX());
}
void test_mouse_EM_callback(void)
{
    trace(0x520,c_getAX(),c_getSI());
    trace(0x521,c_getBX(),c_getCX());
    trace(0x522,c_getDX(),c_getES());
    mouse_EM_callback();
}
/* Optional bios.obj link interposition; observe arguments, then call the
 * unchanged original BIOS entry. No guest register or device mutation. */
#ifdef TEST_VIDEO_GEOMETRY
extern void test_video_geometry_snapshot(unsigned, unsigned);
#else
#define test_video_geometry_snapshot(after, function) ((void)0)
#endif
static void trace_video(void (*entry)(void), unsigned operation)
{
    unsigned function = c_getAX();
    if ((function >> 8) == 0 || (function >> 8) == 0x11 || (function >> 8) == 0x12) {
        trace(operation, c_getAX(), c_getBX());
        trace(operation + 1, c_getCX(), c_getDX());
        trace(operation + 2, c_getES(), c_getBP());
    }
    if ((function >> 8) == 0 || (function >> 8) == 0x11 || (function >> 8) == 0x12) test_video_geometry_snapshot(0, function);
    entry();
    if ((function >> 8) == 0 || (function >> 8) == 0x11 || (function >> 8) == 0x12) test_video_geometry_snapshot(1, function);
}
void test_video_io(void) { trace_video(video_io, 0x320); }
void test_ega_video_io(void) { trace_video(ega_video_io, 0x324); }
void test_mouse_video_io(void)
{
    if ((c_getAX() >> 8) == 0 || c_getAX() == 0x6f05) {
        trace(0x580,GetCurrentProcessId(),c_sas_hw_at(0x4ac));
        trace(0x581,c_getAX(),c_getBX());
        trace(0x582,c_getCS(),c_getIP());
    }
    trace_video(mouse_video_io, 0x328);
}
/* Optional loader diagnostic: preserve the real DEM read and record its
 * requested extent, destination and actual DOS result. */
void test_dem_read(void)
{
    trace(0x310, ((uint32_t)c_getDS() << 16) | c_getDX(), c_getCX());
    demRead();
    trace(0x311, c_getAX(), (uint32_t)c_getCF());
}
extern int xtrn2phy(uint32_t address, uint8_t access, uint32_t *physical);
extern uint32_t c_getCR3(void);
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
static DWORD interactive_input(mvdm_vm_executor *executor, uint32_t vm, uint64_t token)
{
    const char *line = "exit\r";
    unsigned i;
    if (GetEnvironmentVariableW(L"MVDM_NESTED_COMMAND", NULL, 0))
        line = "mem\rexit\r";
    if (GetEnvironmentVariableW(L"MVDM_REENTRANT_COMMAND", NULL, 0))
        line = "command /c cmd /c mem\rexit\r";
    if (GetEnvironmentVariableW(L"MVDM_NESTED_REDIRECT", NULL, 0))
        line = "command /c cmd /c mem > result.txt\recho %ROLE%>> result.txt\rtype result.txt | find \"bytes total\"\rexit\r";
    for (i = 0; line[i]; ++i) {
        KEY_EVENT_RECORD key = {0};
        DWORD error;
        key.bKeyDown = TRUE; key.wRepeatCount = 1;
        /* Synthetic native records, not a physical guest-keyboard claim. */
        if (line[i] == '\r') key.wVirtualKeyCode = VK_RETURN;
        else {
            SHORT mapped = VkKeyScanA(line[i]);
            if (mapped == -1) return ERROR_NO_UNICODE_TRANSLATION;
            key.wVirtualKeyCode = LOBYTE(mapped);
            key.dwControlKeyState = ((mapped & 0x100) ? SHIFT_PRESSED : 0) |
                ((mapped & 0x200) ? LEFT_CTRL_PRESSED : 0) |
                ((mapped & 0x400) ? LEFT_ALT_PRESSED : 0);
        }
        key.wVirtualScanCode = (WORD)MapVirtualKeyW(key.wVirtualKeyCode, MAPVK_VK_TO_VSC);
        key.uChar.UnicodeChar = (WCHAR)line[i];
        error = mvdm_vm_executor_key(executor, vm, token, &key);
        if (error) return error;
        key.bKeyDown = FALSE;
        error = mvdm_vm_executor_key(executor, vm, token, &key);
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
            { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
    }
    if (!mvdm_command_capture_read(request->capture, &view))
        ExitProcess(ERROR_INVALID_DATA);
    {
        unsigned failed = (strcmp(view.environment, environment) ? 1 : 0) |
            (view.environment_bytes != 8 ? 2 : 0) |
            (request->service == 8 && strcmp(view.command, command) ? 4 : 0) |
            (view.service != request->route ? 8 : 0) |
            (view.standard_handles[0] != 0x11111111 ? 16 : 0) |
            (view.standard_handles[1] != 0x22222222 ? 32 : 0) |
            (view.standard_handles[2] != 0x33333333 ? 64 : 0) |
            (view.default_drive != expected_drive ? 128 : 0) |
            (!view.directory || (GetEnvironmentVariableW(L"MVDM_DIRECTORY_SNAPSHOT", NULL, 0)
             ? strcmp(view.directory, request->guest_directory)
             : !strrchr(view.directory, '\\') || strcmp(strrchr(view.directory, '\\'), directory)) ? 256 : 0);
        if (failed) { trace(0x3fe, request->role, failed); ExitProcess(ERROR_INVALID_DATA); }
    }
    mvdm_command_capture_destroy(request->capture);
    {
        void *application;
        uint32_t application_bytes;
        ntvdm_vm_text *text = request->text;
        console_io_request write = {0}; console_io_reply reply = {0};
        console_io_cell cell = {(uint16_t)(request->role + ('a' - 'A')), 0x2f};
        write.operation = CONSOLE_IO_PUBLICATION_BEGIN;
        if (ntvdm_vm_text_dispatch(text, &write, &reply)) ExitProcess(ERROR_INVALID_STATE);
        if (GetEnvironmentVariableW(L"MVDM_FILL_APPLICATION", NULL, 0)) {
            write.operation = CONSOLE_IO_FILL_CHARACTER;
            write.state.count = 1; write.state.character = cell.character;
            if (ntvdm_vm_text_dispatch(text, &write, &reply) || reply.state.count != 1)
                ExitProcess(ERROR_WRITE_FAULT);
            write.operation = CONSOLE_IO_FILL_ATTRIBUTE;
            write.state.attribute = cell.attribute;
            if (ntvdm_vm_text_dispatch(text, &write, &reply) || reply.state.count != 1)
                ExitProcess(ERROR_WRITE_FAULT);
        } else {
            write.operation = CONSOLE_IO_WRITE_CELLS_W;
            write.state.width = write.state.height = 1;
            write.bytes = sizeof(cell); write.data = (uint8_t *)&cell;
            if (ntvdm_vm_text_dispatch(text, &write, &reply)) ExitProcess(ERROR_WRITE_FAULT);
        }
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
            stage_video(text, request->role)) { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
        if (ntvdm_vm_text_copy_application(text, &application, &application_bytes)) ExitProcess(ERROR_INVALID_STATE);
        if (GetEnvironmentVariableA("MVDM_VIDEO_APPLICATION", NULL, 0)) {
            const ntvdm_vm_text_header *h = application;
            BYTE *version = (BYTE *)application + sizeof(*h) + h->columns * h->rows * 2 + 4;
            uint32_t bad = 2, good = NTVDM_VM_VIDEO_VERSION;
            memcpy(version, &bad, sizeof(bad));
            if (mvdm_vm_application_publish(request->executor, application, application_bytes, NULL) != ERROR_INVALID_DATA)
                { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
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
        (frame[7] & 0xffff0000u) != 0x54320000u) { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
    request->route = request->service == 10 ? MVDM_COMMAND_SERVICE_INTERACTIVE :
        (frame[7] & 0xff00u) ? MVDM_COMMAND_SERVICE_EXECUTE : MVDM_COMMAND_SERVICE_NONE;
    request->role = (char)frame[6];
    if (request->role != 'A' && request->role != 'B') { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
    error = mvdm_vm_command_snapshot(request->service, descriptor_address,
        descriptor_bytes, &request->capture, &request->text);
    trace(0x20a, request->service, error);
    if (error) ExitProcess(error);
    {
        console_io_request read = {0}; console_io_reply reply = {0};
        read.operation = CONSOLE_IO_SCREEN_INFO;
        if (ntvdm_vm_text_dispatch(request->text, &read, &reply)) { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
        if (!GetEnvironmentVariableW(L"MVDM_SCREEN_UNSEEDED", NULL, 0) &&
            (reply.state.x != 2 || reply.state.y != 3)) { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
        trace(0x208, reply.state.width, reply.state.height);
    }
    {
        mvdm_command_capture_view view;
        size_t i, length;
        if (!mvdm_command_capture_read(request->capture, &view)) { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
        trace(0x203, frame[7], view.service);
        trace(0x206, (uint32_t)(unsigned char)request->role, frame[6]);
        length = view.directory ? strlen(view.directory) : 0;
        for (i = 0; i < length; ++i) trace(0x204, (uint32_t)i, (uint8_t)view.directory[i]);
        mvdm_guest_location_set_real_mode(&location, (uint16_t)frame[15], (uint16_t)frame[0]);
        if (!mvdm_guest_location_acquire_vm(&location, base,
            GetEnvironmentVariableW(L"MVDM_DIRECTORY_NETCDS", NULL, 0) ? 142 : 76,
            GUEST_MEMORY_ACCESS_READ, &service_lease)) ExitProcess(ERROR_INVALID_ADDRESS);
        if (!memchr(service_lease.bytes, 0, 64)) ExitProcess(ERROR_INVALID_DATA);
        request->guest_directory[0] = (char)('A' + (frame[7] & 0xff));
        request->guest_directory[1] = ':'; request->guest_directory[2] = '\\';
        memcpy(request->guest_directory + 3, service_lease.bytes, 64);
        for (i = 0; i < 64 && service_lease.bytes[i]; ++i)
            trace(0x205, (uint32_t)i, service_lease.bytes[i]);
        for (i = 64; i < 76; i += 4) {
            uint32_t value;
            memcpy(&value, service_lease.bytes + i, sizeof(value));
            trace(0x207, (uint32_t)i, value);
        }

        if (GetEnvironmentVariableW(L"MVDM_DIRECTORY_NETCDS", NULL, 0))
            for (i = 0; i < 64 && service_lease.bytes[78+i]; ++i)
                trace(0xc20, (uint32_t)i, service_lease.bytes[78+i]);
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
    if (pair->first->role == pair->second->role) { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
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
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) ExitProcess(GetLastError());
    length = (DWORD)sprintf_s(line, sizeof(line), "%u %08lx %08lx\n",
        operation, (unsigned long)vm, (unsigned long)token);
    if (!WriteFile(file, line, length, &written, NULL) || written != length)
        ExitProcess(ERROR_WRITE_FAULT);
    CloseHandle(file);
    ReleaseSRWLockExclusive(&lock);
}

/* Optional BIOS geometry trace; the linked observer preserves the CRTC index. */
void test_video_font_mapping(unsigned address, unsigned pte, unsigned flags)
{
    trace(0xb32, address, pte);
    trace(0xb33, address, flags);
}
void test_video_font_store(unsigned address, unsigned value, unsigned readback)
{
    trace(0xb30, address, value | (readback << 8));
}
void test_video_geometry(unsigned after, unsigned function, unsigned lines, unsigned rows)
{
    trace(0xb20 + after * 2, function, lines);
    trace(0xb21 + after * 2, c_sas_hw_at(0x4ac), rows);
}

/* Diagnostic queue observations only; no key is consumed or rewritten. */
void test_vm_io_records(uint32_t owner, uint32_t operation, const void *data, uint32_t count)
{
    const console_io_input *records = data;
    uint32_t i;
    if (!data || count > CONSOLE_IO_INPUT_CAPACITY) return;
    for (i = 0; i < count; ++i) if (records[i].type == KEY_EVENT)
        trace(0xa00 + operation, owner,
            (records[i].character & 0xffffu) | (records[i].key_down ? 0x10000u : 0));
}
void test_vm_io_result(const console_io_request *request, DWORD transport, DWORD reply)
{
    unsigned operation = request ? request->operation : 0;
    trace(0x600u + operation, GetCurrentProcessId(), transport ? transport : reply);
    if (request && (transport || reply)) {
        trace(0x700u + operation, request->state.width, request->state.height);
        trace(0x800u + operation, request->state.count, request->state.has_clip);
        trace(0x900u + operation, request->state.mode, request->bytes);
    }
}

/* Read-only diagnostic of the existing vm_command descriptor and VMM client
 * frame (layout documented by mvdm_vm_command.c). Never recapture/normalize
 * the command: that would change the original CR terminator before admission. */
static void trace_command(uint32_t address, uint32_t extent, uint32_t vm)
{
    uint32_t descriptor[5], frame[18], word = 0, i;
    session *owner = session_thread_current();
    guest_memory_lease *lease;
    uint8_t *bytes;
    mvdm_guest_location location;
    mvdm_guest_location_lease command;
    if (extent != sizeof(descriptor) || !session_guest_memory_acquire(owner,
        address, sizeof(descriptor), GUEST_MEMORY_ACCESS_READ, &lease, &bytes))
        ExitProcess(ERROR_INVALID_ADDRESS);
    memcpy(descriptor, bytes, sizeof(descriptor));
    if (!session_guest_memory_release(owner, lease, 0) ||
        !session_guest_memory_acquire(owner, descriptor[0], sizeof(frame),
            GUEST_MEMORY_ACCESS_READ, &lease, &bytes)) ExitProcess(ERROR_INVALID_ADDRESS);
    memcpy(frame, bytes, sizeof(frame));
    if (!session_guest_memory_release(owner, lease, 0)) ExitProcess(ERROR_INVALID_ADDRESS);
    mvdm_guest_location_set_real_mode(&location, (uint16_t)frame[15], (uint16_t)frame[1]);
    if (!mvdm_guest_location_acquire_vm(&location, descriptor[1], 64,
        GUEST_MEMORY_ACCESS_READ, &command)) ExitProcess(ERROR_INVALID_ADDRESS);
    for (i = 0; i < 64; ++i) {
        uint8_t ch = command.bytes[i];
        word |= (uint32_t)ch << ((i % 4) * 8);
        if (i % 4 == 3 || !ch || ch == 13) { trace(0x360 + i / 4, vm, word); word = 0; }
        if (!ch || ch == 13) break;
    }
    if (!mvdm_guest_location_release(&command, 0)) ExitProcess(ERROR_INVALID_ADDRESS);
}
BOOL test_cmd_dispatch(ULONG service)
{
    uint32_t callback, target[2];
    if (service == 254) {

        if ((c_getEAX() & 0xffff0000u) == 0x4e430000u) {
            uint32_t physical = 0, address = c_getEDX();
            trace(0xc30, c_getEAX() & 0xffu, address);
            trace(0xc31, xtrn2phy(address, 2u, &physical) ? physical : 0xffffffffu,
                (uint32_t)c_getVM());
            trace(0xc32, c_getCR3(), c_getCPL());
        }
        trace(254, c_getEAX(), c_getIP());
        return TRUE;
    }
    if ((service != 8 && service != 10) ||
        (c_getEAX() & 0xffff0000u) != 0x54320000u) {
        BOOL result;
        uint16_t ds = c_getDS(), si = (uint16_t)c_getESI();
        uint8_t drive = (uint8_t)c_getAX();
        DWORD saved;
        trace(0x100 + service, 0, 0);
        if (service == 4) {
            trace(0xc12, drive, ((uint32_t)ds << 16) | si);
            trace(0xc13, c_getCS(), c_getIP());
        }
        result = CmdDispatch(service);
        if (service == 4) {
            mvdm_guest_location location;
            mvdm_guest_location_lease lease;
            size_t i;
            saved = GetLastError();
            trace(0xc10, drive, (uint32_t)c_getCF());
            if (!c_getCF() && mvdm_guest_location_set_real_mode(&location, ds, si) &&
                mvdm_guest_location_acquire(&location, 64, GUEST_MEMORY_ACCESS_READ, &lease)) {
                for (i = 0; i < 64 && lease.bytes[i]; ++i)
                    trace(0xc11, (uint32_t)i, lease.bytes[i]);
                (void)mvdm_guest_location_release(&lease, 0);
            }
            SetLastError(saved);
        }
        return result;
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

/* Diagnostic-only copy after transfer, before guest application/free.
 * Operation14 traces separately establish the later application result. */
static void trace_application(uint32_t vm, uint32_t address, uint32_t extent)
{
    uint32_t descriptor[7], count, i, cell_address, cell_bytes = 2, stride, video_height = 0;
    ntvdm_vm_text_header header;
    char path[MAX_PATH], *text;
    BYTE *cells;
    FILE *file;
    static uint32_t editors[16];
    static DWORD editor_count;
    if (extent != sizeof(descriptor) ||
        !mvdm_softpc_guest_memory_copy_from(address, (BYTE *)descriptor, sizeof(descriptor)) ||
        descriptor[6] || descriptor[5] < sizeof(header) ||
        !mvdm_softpc_guest_memory_copy_from(descriptor[4], (BYTE *)&header, sizeof(header))) return;
    if (!header.columns || !header.rows || header.columns > 256 || header.rows > 256) return;
    count = header.columns * header.rows;
    if (descriptor[5] < sizeof(header) + count * 2) return;
    cell_address = descriptor[4] + sizeof(header);
    stride = header.columns * 2;
    if (descriptor[5] >= sizeof(header) + count * 2 + sizeof(ntvdm_vm_video_application)) {
        ntvdm_vm_video_application video;
        uint32_t offset = sizeof(header) + count * 2;
        if (!mvdm_softpc_guest_memory_copy_from(descriptor[4] + offset, (BYTE *)&video, sizeof(video)))
            ExitProcess(ERROR_INVALID_ADDRESS);
        if (video.signature == NTVDM_VM_VIDEO_SIGNATURE &&
            video.description.kind == CONSOLE_VIDEO_TEXT_FRAME) {
            const console_video_description *d = &video.description;
            if (!d->width || !d->height || d->width > 256 || d->height > 256 ||
                (d->stride != d->width * 2 && d->stride != d->width * 3) ||
                d->bytes != sizeof(console_text_style) + d->height * d->stride ||
                descriptor[5] - offset - sizeof(video) < d->bytes)
                { trace(0x3ff, __LINE__, ERROR_INVALID_DATA); ExitProcess(ERROR_INVALID_DATA); }
            if (!mvdm_softpc_guest_memory_copy_from(descriptor[4] + offset + sizeof(video),
                (BYTE *)&video_height, sizeof(video_height))) ExitProcess(ERROR_INVALID_ADDRESS);
            header.columns = d->width; header.rows = d->height;
            count = header.columns * header.rows;
            stride = d->stride; cell_bytes = stride / header.columns;
            cell_address = descriptor[4] + offset + sizeof(video) + sizeof(console_text_style);
        }
    }
    trace(0xb10, header.rows, header.font_height);
    trace(0xb11, header.columns, video_height);
    if (!GetEnvironmentVariableA("MVDM_SCREEN_TRACE", path, sizeof(path))) return;
    cells = HeapAlloc(GetProcessHeap(), 0, stride * header.rows);
    text = HeapAlloc(GetProcessHeap(), 0, count + header.rows + 1);
    if (!cells || !text) ExitProcess(ERROR_NOT_ENOUGH_MEMORY);
    if (!mvdm_softpc_guest_memory_copy_from(cell_address, cells, stride * header.rows))
        ExitProcess(ERROR_INVALID_ADDRESS);
    for (i = 0; i < header.rows; ++i) {
        uint32_t x;
        for (x = 0; x < header.columns; ++x) {
            BYTE ch = cells[i * stride + x * cell_bytes];
            text[i * (header.columns + 1) + x] = ch >= 32 && ch < 127 ? (char)ch : ' ';
        }
        text[i * (header.columns + 1) + header.columns] = '\n';
    }
    text[count + header.rows] = 0;
    (void)fopen_s(&file, path, "ab");
    if (!file) ExitProcess(ERROR_OPEN_FAILED);
    fprintf(file, "VM=%08lx TOKEN=%08lx:%08lx %lux%lu BIOSFONT=%lu VIDEOFONT=%lu\n%s\n",
        (unsigned long)vm, (unsigned long)descriptor[1], (unsigned long)descriptor[0],
        (unsigned long)header.columns, (unsigned long)header.rows,
        (unsigned long)header.font_height, (unsigned long)video_height, text);
    fclose(file);
    if (strstr(text, "File") && strstr(text, "Edit") &&
        GetEnvironmentVariableA("MVDM_EDITORS_READY", path, sizeof(path))) {
        for (i = 0; i < editor_count && editors[i] != vm; ++i) {}
        if (i == editor_count) {
            if (editor_count == sizeof(editors) / sizeof(editors[0])) ExitProcess(ERROR_BUFFER_OVERFLOW);
            editors[editor_count++] = vm;
            {
                DWORD written;
                HANDLE ready = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL, CREATE_ALWAYS, 0, NULL);
                if (ready == INVALID_HANDLE_VALUE) ExitProcess(GetLastError());
                if (!WriteFile(ready, &editor_count, sizeof(editor_count), &written, NULL) ||
                    written != sizeof(editor_count)) ExitProcess(ERROR_WRITE_FAULT);
                CloseHandle(ready);
            }
        }
    }
    HeapFree(GetProcessHeap(), 0, text);
    HeapFree(GetProcessHeap(), 0, cells);
}

/* Read-only guest DOS screen at its actual VMM high mapping. Unlike host
 * application frames this also observes DOSONLY1, without admitting a job. */
static void trace_guest_screen(uint32_t vm, uint32_t base)
{
    static struct {uint32_t vm; char text[80*25+26];} prior[16];
    static FILE *stream;
    static DWORD editor_seen;
    char path[MAX_PATH], text[80*25+26];
    BYTE cells[80*25*2];
    DWORD i, written;
    HANDLE file;
    if (!GetEnvironmentVariableA("MVDM_GUEST_SCREEN", path, sizeof(path))) return;
    if (!mvdm_softpc_guest_memory_copy_from(base+0xb8000, cells, sizeof(cells))) return;
    for(i=0;i<80*25;++i) {
        text[i+i/80]=cells[2*i]>=32 && cells[2*i]<127 ? cells[2*i] : ' ';
        if(i%80==79) text[i+i/80+1]='\n';
    }
    text[80*25+25]=0;
    for(i=0;i<16 && prior[i].vm && prior[i].vm!=vm;++i) {}
    if(i==16) ExitProcess(ERROR_BUFFER_OVERFLOW);
    if(strcmp(prior[i].text,text)) {
        prior[i].vm=vm;
        strcpy_s(prior[i].text,sizeof(prior[i].text),text);
        if(!stream) (void)fopen_s(&stream,path,"ab");
        if(!stream) ExitProcess(ERROR_OPEN_FAILED);
        fprintf(stream,"VM=%08lx GUEST 80x25\n%s\n",(unsigned long)vm,text);
        fflush(stream);
    }
    if(!editor_seen && strstr(text,"File") && strstr(text,"Edit") &&
       GetEnvironmentVariableA("MVDM_EDITORS_READY",path,sizeof(path))) {
        editor_seen=1;
        file=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
        if(file==INVALID_HANDLE_VALUE) ExitProcess(GetLastError());
        if(!WriteFile(file,&editor_seen,sizeof(editor_seen),&written,NULL) || written!=sizeof(editor_seen)) ExitProcess(ERROR_WRITE_FAULT);
        CloseHandle(file);
    }
}

int mvdm_vm_bridge_bop(uint32_t signature, uint32_t operation,
    uint32_t low, uint32_t high, uint32_t vm, int ring0,
    mvdm_vm_bridge_result *result)
{
    uint64_t token = ((uint64_t)high << 32) | low;
    if (signature != MVDM_VM_BRIDGE_SIGNATURE || !ring0)
        return mvdm_vm_bridge_bop_direct(signature, operation, low, high,
            vm, ring0, result);
    /* Original diagnostic keyboard operations predate the production input
     * ingress/handback BOPs. Real-command tests must use the actual providers,
     * never acknowledge a production handback with an empty diagnostic result. */
    if (operation == 19 && GetEnvironmentVariableW(L"MVDM_REAL_COMMAND", NULL, 0)) {
        uint32_t mouse[7] = {0};
        int handled = mvdm_vm_bridge_bop_direct(signature, operation, low, high,
            vm, ring0, result);
        trace(0x530, vm, result->status);
        if (high == sizeof(mouse) && mvdm_softpc_guest_memory_copy_from(low,
                (uint8_t *)mouse, sizeof(mouse))) {
            trace(0x531, mouse[0], mouse[1]);
            trace(0x532, mouse[2], mouse[3]);
            trace(0x533, mouse[4], mouse[5]);
        }
        return handled;
    }
    if (operation >= 16 && operation <= 19 &&
        GetEnvironmentVariableW(L"MVDM_REAL_COMMAND", NULL, 0))
        return mvdm_vm_bridge_bop_direct(signature, operation, low, high,
            vm, ring0, result);
    if (operation == 255) {
        console_mouse_input mouse;
        ZeroMemory(result,sizeof(*result));
        if(high!=sizeof(mouse) || !mvdm_softpc_guest_memory_copy_from(low,(BYTE *)&mouse,sizeof(mouse))) {
            result->status=ERROR_INVALID_PARAMETER;return 1;
        }
        {
            mvdm_mouse_bridge *state=mvdm_softpc_mouse_current();
            /* The real NTCON window may already own ENTER. The probe joins
             * that route explicitly; it does not reset or double-enter it. */
            result->status=mouse.action==CONSOLE_MOUSE_ENTER && state && state->submitted
                ? ERROR_ALREADY_EXISTS : mvdm_softpc_mouse_submit(state,&mouse);
        }
        trace(255,mouse.action,result->status);
        trace(0x540,(uint32_t)mouse.dx,(uint32_t)mouse.dy);
        {
            mvdm_mouse_bridge *state=mvdm_softpc_mouse_current();
            if(state) {
                trace(0x541,state->virtual_width,state->virtual_height);
                trace(0x542,state->queue.count,state->active);
            }
        }
        return 1;
    }
    if (operation == 254) {
        trace_guest_screen(vm,low);
        ZeroMemory(result,sizeof(*result));
        return 1;
    }
    if (operation == 251) {
        static uint32_t readers[16];
        static DWORD count;
        WCHAR path[MAX_PATH];
        DWORD i, written;
        trace(251, vm, 0);
        trace_guest_screen(vm,low);
        if(GetEnvironmentVariableW(L"MVDM_READ_TOTAL",path,MAX_PATH)) {
            static DWORD total;
            HANDLE file=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
            ++total;
            if(file==INVALID_HANDLE_VALUE) ExitProcess(GetLastError());
            if(!WriteFile(file,&total,sizeof(total),&written,NULL) || written!=sizeof(total)) ExitProcess(ERROR_WRITE_FAULT);
            CloseHandle(file);
        }
        ZeroMemory(result, sizeof(*result));
        if (!GetEnvironmentVariableW(L"MVDM_PROMPT_READERS", path, MAX_PATH)) return 1;
        for (i = 0; i < count; ++i) if (readers[i] == vm) return 1;
        if (count == sizeof(readers) / sizeof(readers[0])) ExitProcess(ERROR_BUFFER_OVERFLOW);
        readers[count++] = vm;
        {
            HANDLE file = CreateFileW(path, GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
            if (file == INVALID_HANDLE_VALUE) ExitProcess(GetLastError());
            if (!WriteFile(file, &count, sizeof(count), &written, NULL) || written != sizeof(count))
                ExitProcess(ERROR_WRITE_FAULT);
            CloseHandle(file);
        }
        return 1;
    }
    if (operation == 16 || operation == 250) {
        trace(0x400 + low, vm, high);
        ZeroMemory(result, sizeof(*result));
        return 1;
    }
    if (operation == 18 || operation == 252) {
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
    if (operation == 17 || operation == 253) {
        KEY_EVENT_RECORD key = {0};
        ZeroMemory(result, sizeof(*result));
        /* Test-only stimulus through the original scan-code conversion and
         * controller. No BIOS buffer writes or fabricated interrupt/result. */
        if (!(low & 0xff) || (low & ~0x3ffu) ||
            (low & 0x300) == 0x300 || !high || high > 0x7f) {
            result->status = ERROR_INVALID_PARAMETER;
            return 1;
        }
        key.wVirtualKeyCode = (WORD)(low & 0xff);
        key.wVirtualScanCode = (WORD)high;
        key.wRepeatCount = 1;
        host_ica_lock();
        if (!(low & 0x200)) {
            key.bKeyDown = TRUE;
            nt_key_down_action(&key);
        }
        if (!(low & 0x100)) {
            key.bKeyDown = FALSE;
            nt_key_up_action(&key);
        }
        host_ica_unlock();
        trace(17, vm, low);
        if (low == 'A' && blocked_keyboard_ready && !SetEvent(blocked_keyboard_ready))
            ExitProcess(GetLastError());
        return 1;
    }
    if (operation == 5 || operation == 7 || operation == 8 ||
        operation == 11 || operation == 12) trace(operation, vm, low);
    if (operation == 13) {
        int handled = mvdm_vm_bridge_bop_direct(signature, operation, low, high, vm, ring0, result);
        if (handled && !result->status) trace_application(vm, low, high);
        return handled;
    }
    if (operation == 14) {
        uint32_t descriptor[7];
        if (high == sizeof(descriptor) &&
            mvdm_softpc_guest_memory_copy_from(low, (BYTE *)descriptor, sizeof(descriptor)))
            trace(0x370, vm, descriptor[6]);
    }
    if (operation == 12) {
        static LONG completed;
        WCHAR path[MAX_PATH];
        int handled = mvdm_vm_bridge_bop_direct(signature, operation, low, high,
            vm, ring0, result);
        if (handled && !result->status &&
            GetEnvironmentVariableW(L"MVDM_COMPLETION_COUNT", path, MAX_PATH)) {
            DWORD count = (DWORD)InterlockedIncrement(&completed), written;
            HANDLE file = CreateFileW(path, GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
            if (file == INVALID_HANDLE_VALUE) ExitProcess(GetLastError());
            if (!WriteFile(file, &count, sizeof(count), &written, NULL) || written != sizeof(count))
                ExitProcess(ERROR_WRITE_FAULT);
            CloseHandle(file);
        }
        return handled;
    }
    if (operation == 11 && GetEnvironmentVariableW(L"MVDM_REAL_COMMAND", NULL, 0)) {
        uint32_t service;
        DWORD error = mvdm_vm_command_service(low, high, &service);
        if (!error && service == 8) trace_command(low, high, vm);
        if (error) ExitProcess(error);
        mvdm_vm_bridge_bop_direct(signature, operation, low, high, vm, ring0, result);
        trace(0x351, vm, result->status);
        if (result->status) ExitProcess(result->status);
        token = ((uint64_t)result->serial_low << 32) | result->value;
        if (service == 10) {
            error = interactive_input(mvdm_standalone_worker_executor(), vm, token);
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
