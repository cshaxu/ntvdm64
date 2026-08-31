#include "mvdm_softpc_termination.h"

#include "session/session.h"

#include <string.h>
#include <windows.h>

static char *mvdm_softpc_append_hex(char *output, ULONG_PTR value,
    unsigned int digits)
{
    static const char hex[] = "0123456789abcdef";
    unsigned int index;

    for (index = digits; index != 0; --index)
        *output++ = hex[(value >> ((index - 1) * 4)) & 0x0f];
    return output;
}

int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code)
{
    (void)vdm_for_wow;
    return session_terminate_current(completion_code);
}

void mvdm_softpc_record_unhandled_exception(
    const struct _EXCEPTION_POINTERS *exception_info)
{
    const EXCEPTION_RECORD *record;
    const CONTEXT *context;
    ULONG_PTR instruction_pointer;
    ULONG_PTR stack_pointer;
    ULONG_PTR return_address;
    ULONG_PTR image_base;
    char message[128];
    char *cursor;
    HANDLE output;
    DWORD written;
    SIZE_T copied;

    if (exception_info == NULL || exception_info->ExceptionRecord == NULL)
        return;
    record = exception_info->ExceptionRecord;
    context = exception_info->ContextRecord;
    instruction_pointer = 0;
    stack_pointer = 0;
    return_address = 0;
    image_base = (ULONG_PTR)GetModuleHandle(NULL);
    if (context != NULL) {
#if defined(_M_IX86)
        instruction_pointer = context->Eip;
        stack_pointer = context->Esp;
#elif defined(_M_X64)
        instruction_pointer = context->Rip;
        stack_pointer = context->Rsp;
#endif
    }
    if (stack_pointer != 0)
        (void)ReadProcessMemory(GetCurrentProcess(), (const void *)stack_pointer,
            &return_address, sizeof(return_address), &copied);
    /* The fixed observer owns the standard console and snapshots it after
     * process exit. Do not allocate, translate, resume, or otherwise alter
     * the original exception path here. */
    cursor = message;
    memcpy(cursor, "MVDM-ORIGINAL-EXCEPTION code=0x", 31);
    cursor += 31;
    cursor = mvdm_softpc_append_hex(cursor, record->ExceptionCode, 8);
    memcpy(cursor, " address=0x", 11);
    cursor += 11;
    cursor = mvdm_softpc_append_hex(cursor,
        (ULONG_PTR)record->ExceptionAddress, sizeof(ULONG_PTR) * 2);
    memcpy(cursor, " ip=0x", 6);
    cursor += 6;
    cursor = mvdm_softpc_append_hex(cursor, instruction_pointer,
        sizeof(ULONG_PTR) * 2);
    memcpy(cursor, " return=0x", 10);
    cursor += 10;
    cursor = mvdm_softpc_append_hex(cursor, return_address,
        sizeof(ULONG_PTR) * 2);
    memcpy(cursor, " base=0x", 8);
    cursor += 8;
    cursor = mvdm_softpc_append_hex(cursor, image_base,
        sizeof(ULONG_PTR) * 2);
    *cursor++ = '\r';
    *cursor++ = '\n';
    output = GetStdHandle(STD_ERROR_HANDLE);
    if (output == NULL || output == INVALID_HANDLE_VALUE) return;
    (void)WriteFile(output, message, (DWORD)(cursor - message), &written, NULL);
}

void mvdm_softpc_record_startup_milestone(const char *name)
{
    static const char prefix[] = "MVDM-STARTUP-MILESTONE ";
    HANDLE output;
    DWORD written;

    if (name == NULL) return;
    output = GetStdHandle(STD_ERROR_HANDLE);
    if (output == NULL || output == INVALID_HANDLE_VALUE) return;
    (void)WriteFile(output, prefix, (DWORD)(sizeof(prefix) - 1), &written, NULL);
    (void)WriteFile(output, name, (DWORD)strlen(name), &written, NULL);
    (void)WriteFile(output, "\r\n", 2, &written, NULL);
}

void mvdm_softpc_record_bop_dispatch(unsigned int selector,
                                     unsigned int service)
{
    static const char hex[] = "0123456789ABCDEF";
    char message[] = "MVDM-BOP-DISPATCH 00:00\r\n";
    HANDLE output;
    DWORD written;

    output = GetStdHandle(STD_ERROR_HANDLE);
    if (output == NULL || output == INVALID_HANDLE_VALUE) return;
    message[18] = hex[(selector >> 4) & 0x0fu];
    message[19] = hex[selector & 0x0fu];
    message[21] = hex[(service >> 4) & 0x0fu];
    message[22] = hex[service & 0x0fu];
    (void)WriteFile(output, message, (DWORD)(sizeof(message) - 1),
                    &written, NULL);
}

void mvdm_softpc_record_cpu_state(unsigned int segment,
                                  unsigned long instruction_pointer,
                                  unsigned int machine_status,
                                  unsigned int default_operand_size)
{
    static const char hex[] = "0123456789ABCDEF";
    char message[] = "MVDM-CPU-STATE 0000:00000000 MSW=0000 CSX=0\r\n";
    HANDLE output;
    DWORD written;
    unsigned int index;

    output = GetStdHandle(STD_ERROR_HANDLE);
    if (output == NULL || output == INVALID_HANDLE_VALUE) return;
    for (index = 0; index != 4; ++index) {
        unsigned int shift = (3u - index) * 4u;
        message[15 + index] = hex[(segment >> shift) & 0x0fu];
    }
    for (index = 0; index != 8; ++index) {
        unsigned int shift = (7u - index) * 4u;
        message[20 + index] = hex[(instruction_pointer >> shift) & 0x0fu];
    }
    for (index = 0; index != 4; ++index) {
        unsigned int shift = (3u - index) * 4u;
        message[33 + index] = hex[(machine_status >> shift) & 0x0fu];
    }
    message[42] = hex[default_operand_size & 0x0fu];
    (void)WriteFile(output, message, (DWORD)(sizeof(message) - 1),
                    &written, NULL);
}
