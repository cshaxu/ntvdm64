#include "mvdm_softpc_termination.h"

#include "session/session.h"

#include <stdio.h>
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

static void mvdm_softpc_write_optional_report(const char *environment_name,
    const char *message, DWORD message_bytes)
{
    char report_path[MAX_PATH];
    DWORD report_path_bytes;
    HANDLE report;
    DWORD written;

    /* An observer may supply a child-only diagnostic path.  Absence or a
     * file failure is deliberately non-fatal to the original code path. */
    report_path_bytes = GetEnvironmentVariableA(environment_name,
        report_path, (DWORD)sizeof(report_path));
    if (report_path_bytes == 0 || report_path_bytes >= sizeof(report_path))
        return;
    report = CreateFileA(report_path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (report == INVALID_HANDLE_VALUE)
        return;
    (void)WriteFile(report, message, message_bytes, &written, NULL);
    CloseHandle(report);
}

static void mvdm_softpc_write_exception_report(const char *message,
    DWORD message_bytes)
{
    mvdm_softpc_write_optional_report("MVDM_EXCEPTION_REPORT_PATH", message,
        message_bytes);
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
    ULONG_PTR frame_pointer;
    ULONG_PTR return_address;
    ULONG_PTR image_base;
    ULONG_PTR access_kind;
    ULONG_PTR access_target;
    ULONG_PTR stack_words[8];
    DWORD code_segment;
    unsigned int stack_word_count;
    unsigned int stack_index;
    char message[512];
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
    frame_pointer = 0;
    return_address = 0;
    access_kind = 0;
    access_target = 0;
    code_segment = 0;
    stack_word_count = 0;
    memset(stack_words, 0, sizeof(stack_words));
    image_base = (ULONG_PTR)GetModuleHandle(NULL);
    if (context != NULL) {
#if defined(_M_IX86)
        instruction_pointer = context->Eip;
        stack_pointer = context->Esp;
        frame_pointer = context->Ebp;
        code_segment = context->SegCs;
#elif defined(_M_X64)
        instruction_pointer = context->Rip;
        stack_pointer = context->Rsp;
        frame_pointer = context->Rbp;
        code_segment = context->SegCs;
#endif
    }
    if (record->NumberParameters >= 2u) {
        access_kind = record->ExceptionInformation[0];
        access_target = record->ExceptionInformation[1];
    }
    if (stack_pointer != 0) {
        (void)ReadProcessMemory(GetCurrentProcess(), (const void *)stack_pointer,
            &return_address, sizeof(return_address), &copied);
        if (ReadProcessMemory(GetCurrentProcess(), (const void *)stack_pointer,
                stack_words, sizeof(stack_words), &copied))
            stack_word_count = (unsigned int)(copied / sizeof(stack_words[0]));
    }
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
    memcpy(cursor, " sp=0x", 6);
    cursor += 6;
    cursor = mvdm_softpc_append_hex(cursor, stack_pointer,
        sizeof(ULONG_PTR) * 2);
    memcpy(cursor, " fp=0x", 6);
    cursor += 6;
    cursor = mvdm_softpc_append_hex(cursor, frame_pointer,
        sizeof(ULONG_PTR) * 2);
    memcpy(cursor, " cs=0x", 6);
    cursor += 6;
    cursor = mvdm_softpc_append_hex(cursor, code_segment, 4);
    memcpy(cursor, " access=0x", 10);
    cursor += 10;
    cursor = mvdm_softpc_append_hex(cursor, access_kind,
        sizeof(ULONG_PTR) * 2);
    memcpy(cursor, " target=0x", 10);
    cursor += 10;
    cursor = mvdm_softpc_append_hex(cursor, access_target,
        sizeof(ULONG_PTR) * 2);
    memcpy(cursor, " return=0x", 10);
    cursor += 10;
    cursor = mvdm_softpc_append_hex(cursor, return_address,
        sizeof(ULONG_PTR) * 2);
    memcpy(cursor, " base=0x", 8);
    cursor += 8;
    cursor = mvdm_softpc_append_hex(cursor, image_base,
        sizeof(ULONG_PTR) * 2);
    memcpy(cursor, " stack=", 7);
    cursor += 7;
    for (stack_index = 0; stack_index != stack_word_count; ++stack_index) {
        if (stack_index != 0)
            *cursor++ = ',';
        cursor = mvdm_softpc_append_hex(cursor, stack_words[stack_index],
            sizeof(ULONG_PTR) * 2);
    }
    *cursor++ = '\r';
    *cursor++ = '\n';
    output = GetStdHandle(STD_ERROR_HANDLE);
    if (output != NULL && output != INVALID_HANDLE_VALUE)
        (void)WriteFile(output, message, (DWORD)(cursor - message), &written, NULL);
    mvdm_softpc_write_exception_report(message, (DWORD)(cursor - message));
}

void mvdm_softpc_record_main_return(int result)
{
    char report_path[MAX_PATH];
    char message[64];
    DWORD report_path_bytes;
    HANDLE report;
    DWORD written;
    int formatted;

    report_path_bytes = GetEnvironmentVariableA(
        "MVDM_MAIN_RETURN_REPORT_PATH", report_path, (DWORD)sizeof(report_path));
    if (report_path_bytes == 0 || report_path_bytes >= sizeof(report_path))
        return;
    report = CreateFileA(report_path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (report == INVALID_HANDLE_VALUE)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-ORIGINAL-MAIN-RETURN code=0x%08lx\r\n",
        (unsigned long)(uint32_t)result);
    if (formatted > 0 && (size_t)formatted < sizeof(message)) {
        DWORD message_bytes = (DWORD)formatted;
        (void)WriteFile(report, message, message_bytes, &written, NULL);
    }
    CloseHandle(report);
}

void mvdm_softpc_record_bop_dispatch(unsigned int selector,
                                     unsigned int service,
                                     unsigned int guest_cs,
                                     unsigned int guest_ip,
                                     unsigned int guest_ds,
                                     unsigned int guest_dx)
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
    if (selector == 0x50u && service == 0x3du) {
        char terminal_message[] =
            "MVDM-BOP-DISPATCH 50:3D cs=0000 ip=0000 ds=0000 dx=0000\r\n";
        terminal_message[27] = hex[(guest_cs >> 12) & 0x0fu];
        terminal_message[28] = hex[(guest_cs >> 8) & 0x0fu];
        terminal_message[29] = hex[(guest_cs >> 4) & 0x0fu];
        terminal_message[30] = hex[guest_cs & 0x0fu];
        terminal_message[35] = hex[(guest_ip >> 12) & 0x0fu];
        terminal_message[36] = hex[(guest_ip >> 8) & 0x0fu];
        terminal_message[37] = hex[(guest_ip >> 4) & 0x0fu];
        terminal_message[38] = hex[guest_ip & 0x0fu];
        terminal_message[43] = hex[(guest_ds >> 12) & 0x0fu];
        terminal_message[44] = hex[(guest_ds >> 8) & 0x0fu];
        terminal_message[45] = hex[(guest_ds >> 4) & 0x0fu];
        terminal_message[46] = hex[guest_ds & 0x0fu];
        terminal_message[51] = hex[(guest_dx >> 12) & 0x0fu];
        terminal_message[52] = hex[(guest_dx >> 8) & 0x0fu];
        terminal_message[53] = hex[(guest_dx >> 4) & 0x0fu];
        terminal_message[54] = hex[guest_dx & 0x0fu];
        (void)WriteFile(output, terminal_message,
            (DWORD)(sizeof(terminal_message) - 1), &written, NULL);
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)(sizeof(message) - 1));
        return;
    }
    (void)WriteFile(output, message, (DWORD)(sizeof(message) - 1),
                    &written, NULL);
    /* DIVERGENCE(MVDM-HOST-DIV-164): the original ingress observer is
     * default-off.  A fixed-container child may request the same already
     * decoded scalar record through its durable report, so a later console
     * repaint cannot hide which original BOP followed a completed return. */
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH", message,
        (DWORD)(sizeof(message) - 1));
}

void mvdm_softpc_record_bop_return(unsigned int selector,
                                   unsigned int service,
                                   unsigned int guest_cs,
                                   unsigned int guest_ip,
                                   unsigned int guest_ax,
                                   unsigned int guest_cf)
{
    static const char hex[] = "0123456789ABCDEF";
    char message[] =
        "MVDM-BOP-RETURN 00:00 cs=0000 ip=0000 ax=0000 cf=0\r\n";
    HANDLE output;
    DWORD written;

    output = GetStdHandle(STD_ERROR_HANDLE);
    if (output == NULL || output == INVALID_HANDLE_VALUE) return;
    message[16] = hex[(selector >> 4) & 0x0fu];
    message[17] = hex[selector & 0x0fu];
    message[19] = hex[(service >> 4) & 0x0fu];
    message[20] = hex[service & 0x0fu];
    message[25] = hex[(guest_cs >> 12) & 0x0fu];
    message[26] = hex[(guest_cs >> 8) & 0x0fu];
    message[27] = hex[(guest_cs >> 4) & 0x0fu];
    message[28] = hex[guest_cs & 0x0fu];
    message[33] = hex[(guest_ip >> 12) & 0x0fu];
    message[34] = hex[(guest_ip >> 8) & 0x0fu];
    message[35] = hex[(guest_ip >> 4) & 0x0fu];
    message[36] = hex[guest_ip & 0x0fu];
    message[41] = hex[(guest_ax >> 12) & 0x0fu];
    message[42] = hex[(guest_ax >> 8) & 0x0fu];
    message[43] = hex[(guest_ax >> 4) & 0x0fu];
    message[44] = hex[guest_ax & 0x0fu];
    message[49] = guest_cf ? '1' : '0';
    (void)WriteFile(output, message, (DWORD)(sizeof(message) - 1),
                    &written, NULL);
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH", message,
        (DWORD)(sizeof(message) - 1));
}

void mvdm_softpc_record_command_call(unsigned int service,
                                    unsigned int stage,
                                    unsigned int guest_ax,
                                    unsigned int guest_cf)
{
    static const char hex[] = "0123456789ABCDEF";
    char message[] = "MVDM-CMD-CALL svc=00 stage=0 ax=0000 cf=0\r\n";
    HANDLE output;
    DWORD written;

    message[18] = hex[(service >> 4) & 0x0fu];
    message[19] = hex[service & 0x0fu];
    message[27] = hex[stage & 0x0fu];
    message[32] = hex[(guest_ax >> 12) & 0x0fu];
    message[33] = hex[(guest_ax >> 8) & 0x0fu];
    message[34] = hex[(guest_ax >> 4) & 0x0fu];
    message[35] = hex[guest_ax & 0x0fu];
    message[40] = guest_cf ? '1' : '0';
    output = GetStdHandle(STD_ERROR_HANDLE);
    if (output != NULL && output != INVALID_HANDLE_VALUE)
        (void)WriteFile(output, message, (DWORD)(sizeof(message) - 1),
                        &written, NULL);
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH", message,
        (DWORD)(sizeof(message) - 1));
}
