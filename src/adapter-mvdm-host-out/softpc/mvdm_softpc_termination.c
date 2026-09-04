#include "mvdm_softpc_termination.h"

#include "session/session.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "mvdm_guest_location.h"

/* Original CPU40 query; its nonzero result means 20-bit wrapping is active
 * (therefore A20 is off).  This observer never changes that state. */
extern int c_sas_twenty_bit_wrapping_enabled(void);
extern unsigned short c_getMSW(void);
extern unsigned int c_getDS_BASE(void);
extern unsigned int c_getDS_LIMIT(void);

static __declspec(thread) const char *mvdm_softpc_termination_origin =
    "unattributed";
static char mvdm_softpc_command_continuation_report_path[MAX_PATH];
static char mvdm_softpc_stream_io_report_path[MAX_PATH];
static volatile LONG mvdm_softpc_dos_console_line_input_seen;

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

static void mvdm_softpc_write_captured_report(const char *report_path,
    const char *message, DWORD message_bytes)
{
    HANDLE report;
    DWORD written;

    if (report_path == NULL || report_path[0] == '\0') return;
    report = CreateFileA(report_path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (report == INVALID_HANDLE_VALUE) return;
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

    mvdm_softpc_stream_io_report_path[0] = '\0';
    bytes = GetEnvironmentVariableA("MVDM_STREAM_IO_REPORT_PATH",
        mvdm_softpc_stream_io_report_path,
        (DWORD)sizeof(mvdm_softpc_stream_io_report_path));
    (void)SetEnvironmentVariableA("MVDM_STREAM_IO_REPORT_PATH", NULL);
    if (bytes == 0u || bytes >= sizeof(mvdm_softpc_stream_io_report_path))
        mvdm_softpc_stream_io_report_path[0] = '\0';

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

    /* DIVERGENCE(ADAPTER-SOFTPC-046): this is an observer-only copy of an
     * already-decoded original ingress.  It must never use the guest-facing
     * Console: original COMMAND owns that surface.  The bounded report path
     * is captured and removed before original environment processing, and an
     * absent path makes the observation a no-op. */
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
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            terminal_message, (DWORD)(sizeof(terminal_message) - 1));
        return;
    }
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH", message,
        (DWORD)(sizeof(message) - 1));
}

void mvdm_softpc_record_bop_return(unsigned int selector,
                                   unsigned int service,
                                   unsigned int guest_cs,
                                   unsigned int guest_ip,
                                   unsigned int guest_ax,
                                   unsigned int guest_cf,
                                   unsigned int guest_if)
{
    static const char hex[] = "0123456789ABCDEF";
    char message[] =
        "MVDM-BOP-RETURN 00:00 cs=0000 ip=0000 ax=0000 cf=0 if=0\r\n";
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
    message[54] = guest_if ? '1' : '0';
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH", message,
        (DWORD)(sizeof(message) - 1));
    if (selector == 0x54u && service == 0x0fu)
        mvdm_softpc_record_command_environment_return_code(guest_cs, guest_ip);
}

void mvdm_softpc_record_keyboard_waitio(void)
{
    static const char ready[] = "MVDM-COMMAND-INPUT-READY\r\n";

    mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path, ready,
        (DWORD)(sizeof(ready) - 1));
}

void mvdm_softpc_record_keyboard_poll(void)
{
    static LONG reported;
    static const char ready[] = "MVDM-COMMAND-INPUT-READY\r\n";

    /* The original AH=00h polling edge can execute many times before the
     * Console worker supplies a key.  The observer needs one edge, not a
     * diagnostic write per poll. */
    if (InterlockedCompareExchange(&reported, 1, 0) == 0) {
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            ready, (DWORD)(sizeof(ready) - 1));
    }
}

void mvdm_softpc_record_dos_console_line_input(void)
{
    static LONG reported;
    static const char ready[] = "MVDM-DOS-CON-LINE-INPUT\r\n";

    InterlockedExchange(&mvdm_softpc_dos_console_line_input_seen, 1);
    if (InterlockedCompareExchange(&reported, 1, 0) == 0) {
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            ready, (DWORD)(sizeof(ready) - 1));
    }
}

void mvdm_softpc_record_cpu_interrupt_enable(void)
{
    static LONG reports;
    static const char marker[] = "MVDM-CPU-STI\r\n";

    if (InterlockedCompareExchange(&mvdm_softpc_dos_console_line_input_seen,
            0, 0) == 0 || InterlockedIncrement(&reports) > 64)
        return;
    mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
        marker, (DWORD)(sizeof(marker) - 1));
}

void mvdm_softpc_record_idle_activity(unsigned int phase,
    unsigned int now_waiting, unsigned int event_present,
    unsigned int wait_result)
{
    static LONG reports;
    char message[112];
    int formatted;

    /* Pre-input idle/timer activity is deliberately irrelevant to the
     * second-COMMAND proof. Keep this observer bounded after the actual
     * source-owned DOS line-input edge. */
    if (InterlockedCompareExchange(&mvdm_softpc_dos_console_line_input_seen,
            0, 0) == 0 || InterlockedIncrement(&reports) > 64)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-IDLE phase=%u waiting=%u event=%u result=%08X\r\n",
        phase, now_waiting != 0u ? 1u : 0u, event_present != 0u ? 1u : 0u,
        wait_result);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_cpu_unsimulate(unsigned int guest_cs,
    unsigned int guest_ip)
{
    char message[64];
    int formatted;

    formatted = snprintf(message, sizeof(message),
        "MVDM-CPU-UNSIMULATE cs=%04X ip=%04X\r\n",
        guest_cs & 0xffffu, guest_ip & 0xffffu);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
}

void mvdm_softpc_record_cpu_simulate_return(unsigned int guest_cs,
    unsigned int guest_ip)
{
    char message[64];
    int formatted;

    formatted = snprintf(message, sizeof(message),
        "MVDM-CPU-SIMULATE-RETURN cs=%04X ip=%04X\r\n",
        guest_cs & 0xffffu, guest_ip & 0xffffu);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
}

void mvdm_softpc_record_cpu_far_return(unsigned int source_cs,
    unsigned int source_ip, unsigned int target_cs, unsigned int target_ip)
{
    static LONG reported;
    char message[112];
    int formatted;

    /* A bounded default-off observer is sufficient to attribute an original
     * guest far-return without turning CPU execution into an instruction
     * trace. */
    /* Startup performs many source-owned far returns before NTDOS transfers
     * control to the explicit child.  Keep this bounded, but retain enough
     * of one fixed 30-second observation to include that later handoff. */
    if (InterlockedIncrement(&reported) > 4096)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CPU-RETF source=%04X:%04X target=%04X:%04X\r\n",
        source_cs & 0xffffu, source_ip & 0xffffu,
        target_cs & 0xffffu, target_ip & 0xffffu);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
}

void mvdm_softpc_record_stream_io_update(const uint8_t *buffer,
    unsigned int count)
{
    char message[160];
    int formatted;
    unsigned int index;
    unsigned int printable = 0u;
    unsigned int sample_count;
    size_t used;

    for (index = 0u; buffer != NULL && index < count; ++index) {
        if (buffer[index] >= 0x20u && buffer[index] != 0x7fu) {
            printable = 1u;
            break;
        }
    }

    formatted = snprintf(message, sizeof(message),
        "MVDM-STREAM-UPDATE count=%u printable=%u", count, printable);
    used = formatted > 0 && (size_t)formatted < sizeof(message)
        ? (size_t)formatted : 0u;
    sample_count = count < 16u ? count : 16u;
    for (index = 0u; used != 0u && buffer != NULL && index < sample_count;
        ++index) {
        formatted = snprintf(message + used, sizeof(message) - used,
            "%s%02X", index == 0u ? " bytes=" : ",", buffer[index]);
        if (formatted <= 0 || (size_t)formatted >= sizeof(message) - used) {
            used = 0u;
            break;
        }
        used += (size_t)formatted;
    }
    if (used != 0u && used + 2u < sizeof(message)) {
        message[used++] = '\r';
        message[used++] = '\n';
        message[used] = '\0';
        formatted = (int)used;
    }
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_stream_io_result(unsigned int count,
    unsigned int wrote, unsigned int bytes_written, unsigned long error_code)
{
    char message[128];
    int formatted;

    formatted = snprintf(message, sizeof(message),
        "MVDM-STREAM-RESULT count=%u wrote=%u bytes=%u error=%lu\r\n",
        count, wrote ? 1u : 0u, bytes_written, error_code);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_console_key(unsigned int scan_code,
    unsigned int key_down)
{
    char message[64];
    int formatted;

    formatted = snprintf(message, sizeof(message),
        "MVDM-CONSOLE-KEY scan=%02X down=%u\r\n",
        scan_code & 0xffu, key_down != 0u ? 1u : 0u);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_keyboard_offer(unsigned int scan_code,
    unsigned int accepted, unsigned int scanning_stopped,
    unsigned int keyboard_disabled, unsigned int queue_depth)
{
    char message[128];
    int formatted;

    formatted = snprintf(message, sizeof(message),
        "MVDM-KBD-OFFER scan=%02X accepted=%u stopped=%u disabled=%u depth=%u\r\n",
        scan_code & 0xffu, accepted != 0u ? 1u : 0u,
        scanning_stopped != 0u ? 1u : 0u,
        keyboard_disabled != 0u ? 1u : 0u, queue_depth);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_keyboard_gate(unsigned int stage,
    unsigned int eoi_pending, unsigned int interrupts_enabled,
    unsigned int output_full, unsigned int keyboard_disabled)
{
    char message[128];
    int formatted;

    formatted = snprintf(message, sizeof(message),
        "MVDM-KBD-GATE stage=%u eoi=%u int=%u full=%u disabled=%u\r\n",
        stage, eoi_pending != 0u ? 1u : 0u,
        interrupts_enabled != 0u ? 1u : 0u,
        output_full != 0u ? 1u : 0u,
        keyboard_disabled != 0u ? 1u : 0u);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_keyboard_ica_irq(unsigned int adapter,
    unsigned int line)
{
    const char message[] = "MVDM-KBD-ICA-IRQ adapter=0 line=1\r\n";

    if (adapter != 0u || line != 1u)
        return;
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
        message, (DWORD)(sizeof(message) - 1));
    mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
        message, (DWORD)(sizeof(message) - 1));
}

void mvdm_softpc_record_keyboard_ica_already_high(unsigned int adapter,
    unsigned int previous_line, unsigned int requested_line)
{
    char message[96];
    int formatted;

    if (adapter != 0u || requested_line != 1u)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-KBD-ICA-ALREADY-HIGH previous=%u requested=%u\r\n",
        previous_line, requested_line);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_keyboard_ica_request(unsigned int irr,
    unsigned int isr, unsigned int imr, unsigned int scan_result)
{
    char message[112];
    int formatted = snprintf(message, sizeof(message),
        "MVDM-KBD-ICA-REQUEST irr=%02X isr=%02X imr=%02X scan=%02X\r\n",
        irr & 0xffu, isr & 0xffu, imr & 0xffu, scan_result & 0xffu);

    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

static volatile LONG mvdm_softpc_cpu_hwirq_epoch;
static volatile LONG mvdm_softpc_cpu_hwirq_deferred_epoch;

void mvdm_softpc_record_cpu_hw_interrupt(void)
{
    static LONG reported;
    char message[] = "MVDM-CPU-HW-INT ordinal=00\r\n";
    static const char hex[] = "0123456789ABCDEF";
    LONG ordinal;

    /* This remains a bounded, default-off observer.  Recording the first
     * request alone cannot distinguish a lost request from one that reached
     * CPU40 while IF was clear later in the same startup. */
    (void)InterlockedIncrement(&mvdm_softpc_cpu_hwirq_epoch);
    ordinal = InterlockedIncrement(&reported);
    if (ordinal > 64)
        return;
    message[24] = hex[((unsigned long)ordinal >> 4) & 0x0f];
    message[25] = hex[(unsigned long)ordinal & 0x0f];
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
        message, (DWORD)(sizeof(message) - 1));
}

void mvdm_softpc_record_cpu_hw_interrupt_deferred(unsigned int interrupts_enabled,
    unsigned int guest_cs, unsigned int guest_ip)
{
    LONG epoch;
    LONG observed;
    char message[160];
    int formatted;
    mvdm_guest_location int1c_vector;
    mvdm_guest_location int1c_target;
    mvdm_guest_location_lease int1c_code;
    mvdm_guest_location int16_vector;
    mvdm_guest_location int16_target;
    mvdm_guest_location_lease int16_code;
    mvdm_guest_location vdm_state_location;
    mvdm_guest_location_lease vdm_state;
    mvdm_guest_location current_code;
    mvdm_guest_location_lease current_code_lease;

    (void)interrupts_enabled;
    epoch = InterlockedCompareExchange(&mvdm_softpc_cpu_hwirq_epoch, 0, 0);
    observed = InterlockedCompareExchange(
        &mvdm_softpc_cpu_hwirq_deferred_epoch, 0, 0);
    if (epoch == 0 || observed == epoch ||
        InterlockedCompareExchange(&mvdm_softpc_cpu_hwirq_deferred_epoch,
            epoch, observed) != observed)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CPU-HW-INT-DEFERRED if=0 cs=%04X ip=%04X\r\n",
        guest_cs & 0xffffu, guest_ip & 0xffffu);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message))
        return;
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
        message, (DWORD)formatted);
    mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
        message, (DWORD)formatted);

    /* The original WOW_x86 FSTI/FCLI macros publish virtual interrupt state
     * at this fixed guest address.  Capture it read-only at the exact point
     * CPU40 defers a real IRQ; it distinguishes a missing monitor bridge from
     * an ordinary CLI-protected interval without changing guest state. */
    if (mvdm_guest_location_set_real_mode(&vdm_state_location, 0u, 0x0714u) &&
        mvdm_guest_location_acquire(&vdm_state_location, 4u,
            GUEST_MEMORY_ACCESS_READ, &vdm_state)) {
        uint32_t vdm_state_value = (uint32_t)vdm_state.bytes[0] |
            ((uint32_t)vdm_state.bytes[1] << 8) |
            ((uint32_t)vdm_state.bytes[2] << 16) |
            ((uint32_t)vdm_state.bytes[3] << 24);
        formatted = snprintf(message, sizeof(message),
            "MVDM-CPU-VDMSTATE value=%08X vif=%u pending=%u\r\n",
            (unsigned int)vdm_state_value,
            (vdm_state_value & 0x0200u) != 0u ? 1u : 0u,
            (vdm_state_value & 0x0003u) != 0u ? 1u : 0u);
        if (mvdm_guest_location_release(&vdm_state, 0) && formatted > 0 &&
            (size_t)formatted < sizeof(message)) {
            mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
                message, (DWORD)formatted);
            mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
                message, (DWORD)formatted);
        }
    }

    /* The installed INT 16h vector distinguishes the ROM handler from the
     * original NTIO resident SpcKbd handler.  Read the existing IVT only;
     * this observer neither selects nor changes the vector. */
    if (mvdm_guest_location_set_real_mode(&int16_vector, 0u, 0x0058u) &&
        mvdm_guest_location_read_far(&int16_vector, &int16_target) &&
        mvdm_guest_location_acquire(&int16_target, 8u,
            GUEST_MEMORY_ACCESS_READ, &int16_code)) {
        formatted = snprintf(message, sizeof(message),
            "MVDM-CPU-INT16 target=%04X:%04X bytes=%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
            (unsigned int)int16_target.segment,
            (unsigned int)int16_target.offset,
            int16_code.bytes[0], int16_code.bytes[1], int16_code.bytes[2],
            int16_code.bytes[3], int16_code.bytes[4], int16_code.bytes[5],
            int16_code.bytes[6], int16_code.bytes[7]);
        if (mvdm_guest_location_release(&int16_code, 0) && formatted > 0 &&
            (size_t)formatted < sizeof(message)) {
            mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
                message, (DWORD)formatted);
            mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
                message, (DWORD)formatted);
        }
    }

    if (mvdm_guest_location_set_real_mode(&current_code,
            (uint16_t)guest_cs, (uint16_t)guest_ip) &&
        mvdm_guest_location_acquire(&current_code, 8u,
            GUEST_MEMORY_ACCESS_READ, &current_code_lease)) {
        formatted = snprintf(message, sizeof(message),
            "MVDM-CPU-HW-INT-CODE cs=%04X ip=%04X bytes=%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
            guest_cs & 0xffffu, guest_ip & 0xffffu,
            current_code_lease.bytes[0], current_code_lease.bytes[1],
            current_code_lease.bytes[2], current_code_lease.bytes[3],
            current_code_lease.bytes[4], current_code_lease.bytes[5],
            current_code_lease.bytes[6], current_code_lease.bytes[7]);
        if (mvdm_guest_location_release(&current_code_lease, 0) &&
            formatted > 0 && (size_t)formatted < sizeof(message)) {
            mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
                message, (DWORD)formatted);
            mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
                message, (DWORD)formatted);
        }
    }

    /* INT 08h keeps IF clear by design while it directly invokes INT 1Ch.
     * Capture the already-installed original vector and a bounded instruction
     * window only: this distinguishes a stuck callback target from a CPU40
     * scheduling fault without modifying IVT, guest code, PIC or flags. */
    if (!mvdm_guest_location_set_real_mode(&int1c_vector, 0u, 0x0070u) ||
        !mvdm_guest_location_read_far(&int1c_vector, &int1c_target) ||
        !mvdm_guest_location_acquire(&int1c_target, 8u,
            GUEST_MEMORY_ACCESS_READ, &int1c_code))
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CPU-INT1C target=%04X:%04X bytes=%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
        (unsigned int)int1c_target.segment, (unsigned int)int1c_target.offset,
        int1c_code.bytes[0], int1c_code.bytes[1], int1c_code.bytes[2],
        int1c_code.bytes[3], int1c_code.bytes[4], int1c_code.bytes[5],
        int1c_code.bytes[6], int1c_code.bytes[7]);
    if (!mvdm_guest_location_release(&int1c_code, 0) || formatted <= 0 ||
        (size_t)formatted >= sizeof(message))
        return;
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
        message, (DWORD)formatted);
    mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
        message, (DWORD)formatted);
}

void mvdm_softpc_record_cpu_hw_interrupt_service(unsigned int vector)
{
    static LONG reported;
    char message[] = "MVDM-CPU-HW-INT-SERVICE vector=00\r\n";
    static const char hex[] = "0123456789ABCDEF";

    /* This is bounded because the fixed observer needs to distinguish the
     * pre-input device interrupts from the later keyboard IRQ. */
    if (InterlockedIncrement(&reported) > 64)
        return;
    message[31] = hex[(vector >> 4) & 0x0fu];
    message[32] = hex[vector & 0x0fu];
    mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
        message, (DWORD)(sizeof(message) - 1));
    mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
        message, (DWORD)(sizeof(message) - 1));
}

void mvdm_softpc_record_ica_eoi(unsigned int adapter, int line,
    unsigned int cpu_interrupt_pending)
{
    static LONG reports;
    char message[96];
    int formatted;

    /* The fixed observer only needs the master timer/keyboard cases.  The
     * bounded record is deliberately emitted after original EOI selection,
     * before its unchanged follow-on interrupt scan. */
    if (adapter != 0u || (line != 0 && line != 1) ||
        InterlockedIncrement(&reports) > 64)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-ICA-EOI adapter=%u line=%d pending=%u\r\n",
        adapter, line, cpu_interrupt_pending != 0u ? 1u : 0u);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_host_eoi_hook(int irq_line, int call_count,
    unsigned int registered)
{
    char message[112];
    int formatted;

    formatted = snprintf(message, sizeof(message),
        "MVDM-HOST-EOI-HOOK line=%d count=%d registered=%u\r\n",
        irq_line, call_count, registered != 0u ? 1u : 0u);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_keyboard_port_read(unsigned int value,
    unsigned int output_full)
{
    static LONG reports;
    char message[96];
    int formatted;

    if (InterlockedIncrement(&reports) > 64)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-KBD-PORT60 value=%02X full=%u\r\n", value & 0xffu,
        output_full != 0u ? 1u : 0u);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_keyboard_eoi_state(unsigned int bios_owns_hardware,
    unsigned int bios_buffer_space)
{
    static LONG reports;
    char message[96];
    int formatted;

    if (InterlockedIncrement(&reports) > 64)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-KBD-EOI bios-owns=%u buffer-space=%u\r\n",
        bios_owns_hardware != 0u ? 1u : 0u,
        bios_buffer_space != 0u ? 1u : 0u);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_keyboard_delay(unsigned int stage,
    unsigned int delay_us, unsigned int delay_mask)
{
    static LONG reports;
    char message[112];
    int formatted;

    if (InterlockedIncrement(&reports) > 64)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-KBD-DELAY stage=%u us=%u mask=%08X\r\n", stage, delay_us,
        delay_mask);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_keyboard_pump(unsigned int stage,
    unsigned int eoi_pending, unsigned int output_full,
    unsigned int pending_8042, unsigned int queue_depth,
    unsigned int keyboard_disabled, unsigned int waiting_for_upcode)
{
    static LONG reports;
    char message[160];
    int formatted;

    if (InterlockedIncrement(&reports) > 96)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-KBD-PUMP stage=%u eoi=%u full=%u pending=%u depth=%u disabled=%u up=%u\\r\\n",
        stage, eoi_pending != 0u ? 1u : 0u, output_full != 0u ? 1u : 0u,
        pending_8042 != 0u ? 1u : 0u, queue_depth,
        keyboard_disabled != 0u ? 1u : 0u,
        waiting_for_upcode != 0u ? 1u : 0u);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_optional_report("MVDM_BOP_RETURN_REPORT_PATH",
            message, (DWORD)formatted);
    if (formatted > 0 && (size_t)formatted < sizeof(message))
        mvdm_softpc_write_captured_report(mvdm_softpc_stream_io_report_path,
            message, (DWORD)formatted);
}

void mvdm_softpc_record_command_call(unsigned int service,
                                    unsigned int stage,
                                    unsigned int guest_ax,
                                    unsigned int guest_cf)
{
    static const char hex[] = "0123456789ABCDEF";
    char message[] = "MVDM-CMD-CALL svc=00 stage=0 ax=0000 cf=0\r\n";

    message[18] = hex[(service >> 4) & 0x0fu];
    message[19] = hex[service & 0x0fu];
    message[27] = hex[stage & 0x0fu];
    message[32] = hex[(guest_ax >> 12) & 0x0fu];
    message[33] = hex[(guest_ax >> 8) & 0x0fu];
    message[34] = hex[(guest_ax >> 4) & 0x0fu];
    message[35] = hex[guest_ax & 0x0fu];
    message[40] = guest_cf ? '1' : '0';
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

void mvdm_softpc_record_command_vdm_result(unsigned int stage,
    unsigned int error_code, unsigned int vdm_state, unsigned int succeeded,
    unsigned int first_call, unsigned int repeat_call)
{
    char message[176];
    int formatted;

    if (mvdm_softpc_command_continuation_report_path[0] == '\0')
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-VDMINFO stage=%u error=%04X state=%04X success=%u first=%u repeat=%u\\r\\n",
        stage, error_code & 0xffffu, vdm_state & 0xffffu, succeeded ? 1u : 0u,
        first_call ? 1u : 0u, repeat_call ? 1u : 0u);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message))
        return;
    {
        HANDLE report = CreateFileA(mvdm_softpc_command_continuation_report_path,
            FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD written;
        if (report == INVALID_HANDLE_VALUE)
            return;
        (void)WriteFile(report, message, (DWORD)formatted, &written, NULL);
        CloseHandle(report);
    }
}

void mvdm_softpc_record_command_guest_return(unsigned int return_code,
    unsigned int first_call, unsigned int repeat_call)
{
    char message[144];
    int formatted;

    if (mvdm_softpc_command_continuation_report_path[0] == '\0')
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-GUEST-RETURN code=%04X first=%u repeat=%u\\r\\n",
        return_code & 0xffffu, first_call ? 1u : 0u, repeat_call ? 1u : 0u);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message))
        return;
    {
        HANDLE report = CreateFileA(mvdm_softpc_command_continuation_report_path,
            FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD written;
        if (report == INVALID_HANDLE_VALUE)
            return;
        (void)WriteFile(report, message, (DWORD)formatted, &written, NULL);
        CloseHandle(report);
    }
}

void mvdm_softpc_record_command_environment_return_code(unsigned int guest_cs,
    unsigned int guest_ip)
{
    mvdm_guest_location location;
    mvdm_guest_location_lease lease;
    char message[224];
    int formatted;

    if (mvdm_softpc_command_continuation_report_path[0] == '\0')
        return;
    if (!mvdm_guest_location_set_real_mode(&location, (uint16_t)guest_cs,
            (uint16_t)guest_ip) ||
        !mvdm_guest_location_acquire(&location, 12u, GUEST_MEMORY_ACCESS_READ,
            &lease))
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-ENV-RETURN cs=%04X ip=%04X bytes=%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X state=copied\r\n",
        guest_cs & 0xffffu, guest_ip & 0xffffu, lease.bytes[0], lease.bytes[1],
        lease.bytes[2], lease.bytes[3], lease.bytes[4], lease.bytes[5],
        lease.bytes[6], lease.bytes[7], lease.bytes[8], lease.bytes[9],
        lease.bytes[10], lease.bytes[11]);
    if (!mvdm_guest_location_release(&lease, 0) || formatted <= 0 ||
        (size_t)formatted >= sizeof(message))
        return;
    {
        HANDLE report = CreateFileA(mvdm_softpc_command_continuation_report_path,
            FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD written;
        if (report == INVALID_HANDLE_VALUE)
            return;
        (void)WriteFile(report, message, (DWORD)formatted, &written, NULL);
        CloseHandle(report);
    }
}

void mvdm_softpc_record_command_environment(unsigned int stage,
    unsigned int guest_es, unsigned int guest_bx, unsigned int guest_ax,
    unsigned int guest_cf, unsigned int guest_ds, unsigned int guest_ss,
    unsigned int guest_sp)
{
    enum {
        mvdm_command_envsiz_offset = 0x203cu,
        mvdm_command_ressiz_offset = 0x0592u,
        mvdm_dos_mcb_size_offset = 0x0003u
    };
    mvdm_guest_location environment_size_location;
    mvdm_guest_location resident_size_location;
    mvdm_guest_location mcb_size_location;
    char message[128];
    unsigned int cpu_ds_base;
    unsigned int cpu_ds_limit;
    unsigned int cpu_msw;
    uint16_t environment_size;
    uint16_t resident_size;
    uint16_t mcb_size;
    int formatted;

    /* Reuse the captured-and-scrubbed path: unlike the generic BOP report
     * selector, this diagnostic can never be copied into the initial DOS
     * environment by original cmdenv.c. */
    if (mvdm_softpc_command_continuation_report_path[0] == '\0')
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-ENV svc=0F stage=%u es=%04X bx=%04X ax=%04X cf=%u ds=%04X ss=%04X sp=%04X\r\n",
        stage, guest_es & 0xffffu, guest_bx & 0xffffu,
        guest_ax & 0xffffu, guest_cf ? 1u : 0u, guest_ds & 0xffffu,
        guest_ss & 0xffffu, guest_sp & 0xffffu);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message))
        return;
    {
        HANDLE report = CreateFileA(mvdm_softpc_command_continuation_report_path,
            FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD written;
        if (report == INVALID_HANDLE_VALUE)
            return;
        (void)WriteFile(report, message, (DWORD)formatted, &written, NULL);
        CloseHandle(report);
    }
    /* The unchanged guest immediately compares the returned BX with this
     * selected-source scalar. Observe it only after the provider returns;
     * this does not retain a guest alias or participate in the comparison. */
    if (stage != 1u || !mvdm_guest_location_set_real_mode(
            &environment_size_location, (uint16_t)guest_ds,
            mvdm_command_envsiz_offset) ||
        !mvdm_guest_location_read_u16(&environment_size_location,
            &environment_size))
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-ENVSIZ ds=%04X value=%04X state=copied\r\n",
        guest_ds & 0xffffu, (unsigned int)environment_size);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message))
        return;
    {
        HANDLE report = CreateFileA(mvdm_softpc_command_continuation_report_path,
            FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD written;
        if (report == INVALID_HANDLE_VALUE)
            return;
        (void)WriteFile(report, message, (DWORD)formatted, &written, NULL);
        CloseHandle(report);
    }
    /* CPU40 alone owns the hidden segment cache.  Copy its state after the
     * unchanged table return so this default-off observer can establish
     * whether the session real-mode lease names the same guest word. */
    cpu_msw = (unsigned int)c_getMSW();
    cpu_ds_base = (unsigned int)c_getDS_BASE();
    cpu_ds_limit = (unsigned int)c_getDS_LIMIT();
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-ENV-DS msw=%04X selector=%04X base=%08X limit=%08X expected-base=%08X state=copied\r\n",
        cpu_msw & 0xffffu, guest_ds & 0xffffu, cpu_ds_base, cpu_ds_limit,
        ((guest_ds & 0xffffu) << 4));
    if (formatted <= 0 || (size_t)formatted >= sizeof(message))
        return;
    {
        HANDLE report = CreateFileA(mvdm_softpc_command_continuation_report_path,
            FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD written;
        if (report == INVALID_HANDLE_VALUE)
            return;
        (void)WriteFile(report, message, (DWORD)formatted, &written, NULL);
        CloseHandle(report);
    }
    /* ResSize is the original COMMAND input to AH=4Ah; the preceding MCB's
     * size is the original NTDOS result.  Read both through bounded leases
     * only, after the original environment-table return. */
    if (guest_ds == 0u || !mvdm_guest_location_set_real_mode(
            &resident_size_location, (uint16_t)guest_ds,
            mvdm_command_ressiz_offset) ||
        !mvdm_guest_location_read_u16(&resident_size_location, &resident_size) ||
        !mvdm_guest_location_set_real_mode(&mcb_size_location,
            (uint16_t)(guest_ds - 1u), mvdm_dos_mcb_size_offset) ||
        !mvdm_guest_location_read_u16(&mcb_size_location, &mcb_size)) return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-ARENA ds=%04X ressize=%04X mcb-size=%04X state=copied\r\n",
        guest_ds & 0xffffu, (unsigned int)resident_size,
        (unsigned int)mcb_size);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message))
        return;
    {
        HANDLE report = CreateFileA(mvdm_softpc_command_continuation_report_path,
            FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD written;
        if (report == INVALID_HANDLE_VALUE)
            return;
        (void)WriteFile(report, message, (DWORD)formatted, &written, NULL);
        CloseHandle(report);
    }
}

void mvdm_softpc_record_command_stub_table(uint16_t guest_cs)
{
    static const uint16_t offsets[] = { 0x011cu, 0x0120u, 0x0124u };
    static const char * const names[] = {
        "TrnLodCom1", "LodCom", "MsgRetrv"
    };
    unsigned int index;
    char message[256];
    int formatted;
    mvdm_guest_location entry;
    mvdm_guest_location target;
    mvdm_guest_location lodcom_target;
    mvdm_guest_location_lease code_lease;
    uint16_t com_in_hma_word = 0u;
    uint16_t pdb_version = 0u;
    unsigned int a20_wrap;

    if (mvdm_softpc_command_continuation_report_path[0] == '\0')
        return;
    for (index = 0u; index < sizeof(offsets) / sizeof(offsets[0]); ++index) {
        if (!mvdm_guest_location_set_real_mode(&entry, guest_cs,
                offsets[index]) || !mvdm_guest_location_read_far(&entry,
                &target))
            return;
        if (index == 1u)
            lodcom_target = target;
        formatted = snprintf(message, sizeof(message),
            "MVDM-CMD-STUB name=%s entry=%04X:%04X target=%04X:%04X state=copied\r\n",
            names[index], (unsigned int)guest_cs, (unsigned int)offsets[index],
            (unsigned int)target.segment, (unsigned int)target.offset);
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
    if (!mvdm_guest_location_set_real_mode(&entry, guest_cs, 0x0134u) ||
        !mvdm_guest_location_read_u16(&entry, &com_in_hma_word)) return;
    a20_wrap = c_sas_twenty_bit_wrapping_enabled() ? 1u : 0u;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-HMA cominhma=%u a20-wrap=%u state=copied\r\n",
        (unsigned int)(com_in_hma_word & 0xffu), a20_wrap);
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

    /* The unchanged COMMAND initialization immediately sets its PSP as the
     * current DOS PDB, then INT 21h/AH=30h reads PDB_Version at PSP:0040.
     * Record that source-defined scalar only; this observer neither creates
     * nor modifies the PDB or the DOS return frame. */
    if (!mvdm_guest_location_set_real_mode(&entry, guest_cs, 0x0040u) ||
        !mvdm_guest_location_read_u16(&entry, &pdb_version)) return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-PDB segment=%04X version=%04X expected=0005 state=copied\r\n",
        (unsigned int)guest_cs, (unsigned int)pdb_version);
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

    /* The far pointer itself is insufficient evidence: source immediately
     * transfers through LodCom_Entry.  Copy its first instruction bytes via
     * a bounded lease, so HMA mapping is verified without changing it. */
    if (!mvdm_guest_location_acquire(&lodcom_target, 8u,
            GUEST_MEMORY_ACCESS_READ,
            &code_lease)) return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-HMA-CODE target=%04X:%04X bytes=%02X %02X %02X %02X %02X %02X %02X %02X state=copied\r\n",
        (unsigned int)lodcom_target.segment, (unsigned int)lodcom_target.offset,
        code_lease.bytes[0], code_lease.bytes[1], code_lease.bytes[2],
        code_lease.bytes[3], code_lease.bytes[4], code_lease.bytes[5],
        code_lease.bytes[6], code_lease.bytes[7]);
    if (!mvdm_guest_location_release(&code_lease, 0) || formatted <= 0 ||
        (size_t)formatted >= sizeof(message)) return;
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

static void mvdm_softpc_record_dem_path(const char *environment_name,
    const char *record_name, uint16_t guest_ds, uint16_t guest_si,
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

    if (environment_name == NULL || record_name == NULL ||
        GetEnvironmentVariableA(environment_name, NULL, 0u) == 0u)
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
        "%s phase=%u ds=%04X si=%04X status=%08X ax=%04X cf=%u path=%s\r\n",
        record_name, phase, (unsigned int)guest_ds, (unsigned int)guest_si, status,
        guest_ax, guest_cf, text);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    mvdm_softpc_write_optional_report(environment_name, message, (DWORD)formatted);
}

void mvdm_softpc_record_dem_open(uint16_t guest_ds, uint16_t guest_si,
    unsigned int phase, unsigned int status, unsigned int guest_ax,
    unsigned int guest_cf)
{
    mvdm_softpc_record_dem_path("MVDM_DEM_OPEN_REPORT_PATH", "MVDM-DEM-OPEN",
        guest_ds, guest_si, phase, status, guest_ax, guest_cf);
}

void mvdm_softpc_record_dem_create(uint16_t guest_ds, uint16_t guest_si,
    unsigned int phase, unsigned int status, unsigned int guest_ax,
    unsigned int guest_cf)
{
    mvdm_softpc_record_dem_path("MVDM_DEM_CREATE_REPORT_PATH", "MVDM-DEM-CREATE",
        guest_ds, guest_si, phase, status, guest_ax, guest_cf);
}

void mvdm_softpc_record_dem_chmod(uint16_t guest_ds, uint16_t guest_dx,
    unsigned int phase, unsigned int status, unsigned int guest_ax,
    unsigned int guest_cf)
{
    mvdm_softpc_record_dem_path("MVDM_DEM_CHMOD_REPORT_PATH", "MVDM-DEM-CHMOD",
        guest_ds, guest_dx, phase, status, guest_ax, guest_cf);
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

void mvdm_softpc_record_dem_write(uint16_t guest_ds, uint16_t guest_dx,
    uint16_t requested_bytes, uint16_t file_offset_high,
    uint16_t file_offset_low, unsigned int phase, unsigned int guest_ax,
    unsigned int guest_cf)
{
    char message[224];
    int formatted;

    if (GetEnvironmentVariableA("MVDM_DEM_WRITE_REPORT_PATH", NULL, 0u) == 0u)
        return;
    formatted = snprintf(message, sizeof(message),
        "MVDM-DEM-WRITE phase=%u ds=%04X dx=%04X requested=%04X offset=%04X:%04X ax=%04X cf=%u state=copied\r\n",
        phase, (unsigned int)guest_ds, (unsigned int)guest_dx,
        (unsigned int)requested_bytes, (unsigned int)file_offset_high,
        (unsigned int)file_offset_low, guest_ax, guest_cf);
    if (formatted <= 0 || (size_t)formatted >= sizeof(message)) return;
    mvdm_softpc_write_optional_report("MVDM_DEM_WRITE_REPORT_PATH", message,
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
