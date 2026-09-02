#include "mvdm_softpc_termination.h"

#include "session/session.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "mvdm_guest_location.h"

static __declspec(thread) const char *mvdm_softpc_termination_origin =
    "unattributed";
static char mvdm_softpc_command_continuation_report_path[MAX_PATH];

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

void mvdm_softpc_capture_command_continuation_report_path(void)
{
    DWORD bytes;

    /* DIVERGENCE(APP-DIV-015): original cmdenv.c copies inherited variables
     * into the guest DOS environment.  A host-only observation selector must
     * be captured before original startup and then removed, rather than
     * becoming a new guest/environment allocation input. */
    mvdm_softpc_command_continuation_report_path[0] = '\0';
    bytes = GetEnvironmentVariableA("MVDM_COMMAND_CONTINUATION_REPORT_PATH",
        mvdm_softpc_command_continuation_report_path,
        (DWORD)sizeof(mvdm_softpc_command_continuation_report_path));
    (void)SetEnvironmentVariableA("MVDM_COMMAND_CONTINUATION_REPORT_PATH",
        NULL);
    if (bytes == 0u || bytes >= sizeof(mvdm_softpc_command_continuation_report_path))
        mvdm_softpc_command_continuation_report_path[0] = '\0';
}

static void mvdm_softpc_write_exception_report(const char *message,
    DWORD message_bytes)
{
    mvdm_softpc_write_optional_report("MVDM_EXCEPTION_REPORT_PATH", message,
        message_bytes);
}

typedef struct mvdm_softpc_direct_ram_observation {
    LONG initialized;
    LONG enabled;
    LONG observed;
    uint32_t requested_physical_address;
    uint32_t direct_physical_address;
    uint32_t wrap_mask;
    uint32_t m_area_length;
    uintptr_t m_area_base;
} mvdm_softpc_direct_ram_observation;

static mvdm_softpc_direct_ram_observation mvdm_direct_ram_observation;

void mvdm_softpc_record_direct_ram_access(uint32_t requested_physical_address,
    uint32_t direct_physical_address, uint32_t wrap_mask,
    uint32_t m_area_length, uintptr_t m_area_base)
{
    if (InterlockedCompareExchange(&mvdm_direct_ram_observation.initialized,
            1, 0) == 0 &&
        GetEnvironmentVariableA("MVDM_SAS_DIRECT_RAM_REPORT_PATH", NULL,
            0u) != 0u) {
        InterlockedExchange(&mvdm_direct_ram_observation.enabled, 1);
    }
    if (mvdm_direct_ram_observation.enabled == 0)
        return;
    mvdm_direct_ram_observation.requested_physical_address =
        requested_physical_address;
    mvdm_direct_ram_observation.direct_physical_address =
        direct_physical_address;
    mvdm_direct_ram_observation.wrap_mask = wrap_mask;
    mvdm_direct_ram_observation.m_area_length = m_area_length;
    mvdm_direct_ram_observation.m_area_base = m_area_base;
    InterlockedExchange(&mvdm_direct_ram_observation.observed, 1);
}

static void mvdm_softpc_write_direct_ram_observation(void)
{
    char message[256];
    int formatted;

    if (mvdm_direct_ram_observation.enabled == 0 ||
        mvdm_direct_ram_observation.observed == 0)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-SAS-DIRECT-RAM requested=%08lX direct=%08lX wrap=%08lX length=%08lX base=%0*llX resolver=miss state=copied\r\n",
        (unsigned long)mvdm_direct_ram_observation.requested_physical_address,
        (unsigned long)mvdm_direct_ram_observation.direct_physical_address,
        (unsigned long)mvdm_direct_ram_observation.wrap_mask,
        (unsigned long)mvdm_direct_ram_observation.m_area_length,
        (int)(sizeof(uintptr_t) * 2u),
        (unsigned long long)mvdm_direct_ram_observation.m_area_base);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message))
        return;
    mvdm_softpc_write_optional_report("MVDM_SAS_DIRECT_RAM_REPORT_PATH",
        message, (DWORD)formatted);
}

void mvdm_softpc_set_termination_origin(const char *origin)
{
    mvdm_softpc_termination_origin = origin != NULL ? origin : "unattributed";
}

int mvdm_softpc_terminate_current_session(uint32_t vdm_for_wow,
    uint32_t completion_code)
{
    char message[192];
    int formatted;

    (void)vdm_for_wow;
    formatted = snprintf(message, sizeof(message),
        "MVDM-SESSION-TERMINATION origin=%s code=0x%08lX\r\n",
        mvdm_softpc_termination_origin, (unsigned long)completion_code);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_SESSION_TERMINATION_REPORT_PATH",
            message, (DWORD)formatted);
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
    mvdm_softpc_write_direct_ram_observation();
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

void mvdm_softpc_record_command_continuation(unsigned int stage,
    unsigned int guest_cs, unsigned int guest_ip, unsigned int guest_ax,
    unsigned int guest_bx, unsigned int guest_cf, unsigned int first_call,
    unsigned int repeat_call, uint32_t dos_record_state)
{
    char message[192];
    int formatted;

    /* This is deliberately a separate default-off channel.  Its path was
     * captured and removed before original cmdenv.c saw the environment, so
     * an enabled observer cannot alter original guest input or allocation. */
    if (mvdm_softpc_command_continuation_report_path[0] == '\0')
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-CONT svc=01 stage=%u cs=%04X ip=%04X ax=%04X bx=%04X cf=%u first=%u repeat=%u dos-state=%08lX\\r\\n",
        stage, guest_cs & 0xffffu, guest_ip & 0xffffu,
        guest_ax & 0xffffu, guest_bx & 0xffffu, guest_cf ? 1u : 0u,
        first_call ? 1u : 0u, repeat_call ? 1u : 0u,
        (unsigned long)dos_record_state);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    {
        HANDLE report = CreateFileA(mvdm_softpc_command_continuation_report_path,
            FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD written;
        if (report == INVALID_HANDLE_VALUE) return;
        (void)WriteFile(report, message, (DWORD)formatted, &written, NULL);
        CloseHandle(report);
    }
}

void mvdm_softpc_record_dem_open(uint16_t guest_ds, uint16_t guest_si,
    unsigned int phase, unsigned int status, unsigned int guest_ax,
    unsigned int guest_cf)
{
    mvdm_guest_location location;
    uint8_t copied[260];
    char text[sizeof(copied)];
    char message[512];
    uint32_t copied_bytes = 0u;
    uint32_t index;
    int formatted;

    if (GetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH", NULL, 0u) == 0u)
        return;
    memset(copied, 0, sizeof(copied));
    if (!mvdm_guest_location_set_real_mode(&location, guest_ds, guest_si) ||
        !mvdm_guest_location_copy_c_string(&location, copied,
            (uint32_t)sizeof(copied), &copied_bytes)) {
        memcpy(text, "<unavailable>", sizeof("<unavailable>"));
    } else {
        for (index = 0u; index + 1u < copied_bytes && index + 1u < sizeof(text);
            ++index) {
            text[index] = isprint(copied[index]) ? (char)copied[index] : '?';
        }
        text[index] = '\0';
    }
    formatted = snprintf(message, sizeof(message),
        "MVDM-DEM-OPEN phase=%u ds=%04X si=%04X status=%08X ax=%04X cf=%u path=%s\r\n",
        phase, (unsigned int)guest_ds, (unsigned int)guest_si, status,
        guest_ax, guest_cf, text);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    mvdm_softpc_write_optional_report("MVDM_DEM_OPEN_REPORT_PATH", message,
        (DWORD)formatted);
}

void mvdm_softpc_record_dem_read(uint16_t guest_ds, uint16_t guest_dx,
    uint16_t requested_bytes, uint16_t file_offset_high,
    uint16_t file_offset_low, uint16_t completed_bytes,
    unsigned int phase, unsigned int guest_ax, unsigned int guest_cf)
{
    char message[224];
    int formatted;

    if (GetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH", NULL, 0u) == 0u)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-DEM-READ phase=%u ds=%04X dx=%04X requested=%04X offset=%04X:%04X completed=%04X ax=%04X cf=%u state=copied\r\n",
        phase, (unsigned int)guest_ds, (unsigned int)guest_dx,
        (unsigned int)requested_bytes, (unsigned int)file_offset_high,
        (unsigned int)file_offset_low, (unsigned int)completed_bytes,
        guest_ax, guest_cf);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    mvdm_softpc_write_optional_report("MVDM_DEM_READ_REPORT_PATH", message,
        (DWORD)formatted);
}

void mvdm_softpc_record_dem_seek(uint16_t requested_high,
    uint16_t requested_low, unsigned int origin, unsigned int phase,
    uint16_t result_high, uint16_t result_low, unsigned int guest_ax,
    unsigned int guest_cf)
{
    char message[192];
    int formatted;

    if (GetEnvironmentVariableA("MVDM_DEM_SEEK_REPORT_PATH", NULL, 0u) == 0u)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-DEM-SEEK phase=%u requested=%04X:%04X origin=%02X result=%04X:%04X ax=%04X cf=%u state=copied\r\n",
        phase, (unsigned int)requested_high, (unsigned int)requested_low,
        origin, (unsigned int)result_high, (unsigned int)result_low,
        guest_ax, guest_cf);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    mvdm_softpc_write_optional_report("MVDM_DEM_SEEK_REPORT_PATH", message,
        (DWORD)formatted);
}

void mvdm_softpc_record_dem_ioctl(unsigned int subfunction,
    unsigned int drive, unsigned int phase, unsigned int guest_ax,
    unsigned int guest_dx, unsigned int guest_cf)
{
    char message[160];
    int formatted;

    if (GetEnvironmentVariableA("MVDM_DEM_IOCTL_REPORT_PATH", NULL, 0u) == 0u)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-DEM-IOCTL phase=%u subfunction=%02X drive=%02X ax=%04X dx=%04X cf=%u state=copied\r\n",
        phase, subfunction, drive, guest_ax, guest_dx, guest_cf);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    mvdm_softpc_write_optional_report("MVDM_DEM_IOCTL_REPORT_PATH", message,
        (DWORD)formatted);
}

void mvdm_softpc_record_config_done(uint16_t guest_cs)
{
    /* These are offsets in the exact selected NTIO.SYS map, not a general
     * DOS ABI. The map is file-relative; caller-provided CS is the original
     * loaded SYSINIT segment at this BOP. */
    enum {
        mvdm_config_done_pass_offset = 0x03d8u,
        mvdm_config_done_command_offset = 0x3466u,
        mvdm_config_done_command_bytes = 64u
    };
    mvdm_guest_location pass_location;
    mvdm_guest_location command_location;
    mvdm_guest_location_lease lease;
    uint8_t pass_value = 0u;
    uint8_t command[mvdm_config_done_command_bytes];
    char text[mvdm_config_done_command_bytes + 1u];
    char message[256];
    uint32_t index;
    int command_terminated = 0;
    int pass_available = 0;
    int command_available = 0;
    int formatted;

    if (GetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH", NULL, 0u) == 0u)
        return;
    memset(command, 0, sizeof(command));
    memset(text, 0, sizeof(text));
    if (mvdm_guest_location_set_real_mode(&pass_location, guest_cs,
        mvdm_config_done_pass_offset) &&
        mvdm_guest_location_acquire(&pass_location, 1u,
            GUEST_MEMORY_ACCESS_READ, &lease)) {
        pass_value = lease.bytes[0];
        pass_available = mvdm_guest_location_release(&lease, 0);
    }
    if (mvdm_guest_location_set_real_mode(&command_location, guest_cs,
        mvdm_config_done_command_offset) &&
        mvdm_guest_location_acquire(&command_location,
            mvdm_config_done_command_bytes, GUEST_MEMORY_ACCESS_READ, &lease)) {
        memcpy(command, lease.bytes, sizeof(command));
        command_available = mvdm_guest_location_release(&lease, 0);
    }
    for (index = 0u; index < sizeof(command); ++index) {
        if (command[index] == 0u) {
            command_terminated = 1;
            break;
        }
        text[index] = isprint(command[index]) ? (char)command[index] : '?';
    }
    if (command_terminated)
        text[index] = '\0';
    else
        memcpy(text, "<unterminated>", sizeof("<unterminated>"));
    formatted = snprintf(message, sizeof(message),
        "MVDM-CONFIG-DONE al=00 cs=%04X pass=%s%02X command=%s command-state=%s\r\n",
        (unsigned int)guest_cs, pass_available ? "" : "?", pass_value, text,
        command_available ? "copied" : "unavailable");
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    mvdm_softpc_write_optional_report("MVDM_CONFIG_DONE_REPORT_PATH", message,
        (DWORD)formatted);
}

void mvdm_softpc_record_sas_store(uint32_t guest_linear_address,
    uint32_t byte_count, uint32_t value)
{
    static LONG initialized;
    static uint32_t watched_address;
    static uint32_t watched_length;
    static int enabled;
    static unsigned int record_count;
    char configured_address[16];
    char configured_length[16];
    char message[128];
    char *end;
    unsigned long parsed_address;
    uint64_t write_end;
    uint64_t watched_end;
    int formatted;

    if (initialized == 0) {
        if (GetEnvironmentVariableA("MVDM_SAS_STORE_REPORT_PATH",
                NULL, 0u) != 0u &&
            GetEnvironmentVariableA("MVDM_SAS_STORE_LINEAR",
                configured_address, (DWORD)sizeof(configured_address)) != 0u) {
            parsed_address = strtoul(configured_address, &end, 0);
            if (end != configured_address && *end == '\0' &&
                parsed_address <= UINT32_MAX) {
                watched_address = (uint32_t)parsed_address;
                watched_length = 1u;
                if (GetEnvironmentVariableA("MVDM_SAS_STORE_LENGTH",
                        configured_length, (DWORD)sizeof(configured_length)) != 0u) {
                    parsed_address = strtoul(configured_length, &end, 0);
                    if (end == configured_length || *end != '\0' ||
                        parsed_address == 0u || parsed_address > UINT32_MAX)
                        return;
                    watched_length = (uint32_t)parsed_address;
                }
                enabled = 1;
                InterlockedExchange(&initialized, 1);
            }
        }
    }
    write_end = (uint64_t)guest_linear_address + (uint64_t)byte_count;
    watched_end = (uint64_t)watched_address + (uint64_t)watched_length;
    if (!enabled || byte_count == 0u ||
        write_end <= watched_address || watched_end <= guest_linear_address ||
        record_count == 8u)
        return;
    ++record_count;
    formatted = snprintf(message, sizeof(message),
        "MVDM-SAS-STORE ordinal=%u linear=%05lX width=%lu value=%08lX state=copied\r\n",
        record_count, (unsigned long)guest_linear_address,
        (unsigned long)byte_count, (unsigned long)value);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    mvdm_softpc_write_optional_report("MVDM_SAS_STORE_REPORT_PATH",
        message, (DWORD)formatted);
}
