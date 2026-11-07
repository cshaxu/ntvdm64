/* Diagnostic-only wrapper around the unchanged production VM I/O owner. */
#define mvdm_vm_io_operations observed_vm_io_operations
#define mvdm_vm_io_keys original_vm_io_keys
#define mvdm_vm_io_key original_vm_io_key
#include "ntvdm-exe/softpc/mvdm_vm_io.c"
#undef mvdm_vm_io_key
#undef mvdm_vm_io_keys
#undef mvdm_vm_io_operations
extern void test_vm_io_result(const console_io_request *, DWORD, DWORD);
extern void test_vm_io_records(uint32_t, uint32_t, const void *, uint32_t);
static DWORD observed_dispatch(void *context, const ntvdm_vm_cancel *cancel,
    const console_io_request *request, console_io_reply *reply, frontend_base_result *result)
{
    DWORD error = dispatch(context, cancel, request, reply, result);
    DWORD saved = GetLastError();
    test_vm_io_result(request, error,
        !error && reply ? reply->error : 0);
    if (!error && reply && !reply->error && request) {
        if (request->operation == CONSOLE_IO_READ_INPUT)
            test_vm_io_records((uint32_t)(uintptr_t)context, request->operation,
                reply->data, reply->state.count);
        else if (request->operation == CONSOLE_IO_PREPEND_KEYS)
            test_vm_io_records((uint32_t)(uintptr_t)context, request->operation,
                request->data, request->state.count);
    }
    SetLastError(saved);
    return error;
}
const ntvdm_vm_io mvdm_vm_io_operations = {observed_dispatch, input_pending, finish};

DWORD mvdm_vm_io_keys(mvdm_vm_io *io, const KEY_EVENT_RECORD *keys, uint32_t count)
{
    DWORD error = original_vm_io_keys(io, keys, count), saved = GetLastError();
    uint32_t i;
    if (!error) for (i = 0; i < count; ++i) {
        console_io_input input = {0};
        input.type = KEY_EVENT;
        input.key_down = keys[i].bKeyDown;
        input.character = keys[i].uChar.UnicodeChar;
        test_vm_io_records((uint32_t)(uintptr_t)io, 0x100, &input, 1);
    }
    SetLastError(saved);
    return error;
}
DWORD mvdm_vm_io_key(mvdm_vm_io *io, const KEY_EVENT_RECORD *key)
{ return mvdm_vm_io_keys(io, key, 1); }
