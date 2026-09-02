#include "mvdm_command_native_child.h"

#include <stdio.h>
#include <string.h>

#include "mvdm_guest_location.h"
#include "session/session.h"

#define MVDM_COMMAND_NATIVE_CHILD_COMMAND_BYTES 128u
#define MVDM_COMMAND_NATIVE_CHILD_ENVIRONMENT_BYTES (32u * 1024u)
#define MVDM_COMMAND_NATIVE_CHILD_STANDARD_BYTES 12u

static char native_child_report_path[MAX_PATH];

static void record_command(const char *origin, const char *command)
{
    char message[320];
    char text[257];
    size_t index;
    int formatted;
    HANDLE report;
    DWORD written;

    if (native_child_report_path[0] == '\0' || origin == NULL || command == NULL)
        return;
    for (index = 0u; index + 1u < sizeof(text) && command[index] != '\0'; ++index)
        text[index] = (command[index] >= ' ' && command[index] <= '~') ?
            command[index] : '?';
    text[index] = '\0';
    formatted = snprintf(message, sizeof(message),
        "MVDM-CMD-PAYLOAD origin=%s bytes=%lu command=%s%s\\r\\n", origin,
        (unsigned long)strlen(command), text,
        command[index] == '\0' ? "" : "<truncated>");
    if (formatted < 0) return;
    report = CreateFileA(native_child_report_path, FILE_APPEND_DATA,
        FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (report == INVALID_HANDLE_VALUE) return;
    (void)WriteFile(report, message, (DWORD)formatted, &written, NULL);
    CloseHandle(report);
}

void mvdm_command_native_child_capture_report_path(void)
{
    DWORD bytes;

    native_child_report_path[0] = '\0';
    bytes = GetEnvironmentVariableA("MVDM_NATIVE_CHILD_REPORT_PATH",
        native_child_report_path, (DWORD)sizeof(native_child_report_path));
    (void)SetEnvironmentVariableA("MVDM_NATIVE_CHILD_REPORT_PATH", NULL);
    if (bytes == 0u || bytes >= sizeof(native_child_report_path))
        native_child_report_path[0] = '\0';
}

typedef struct mvdm_command_native_child_state {
    session *owner;
    uint32_t owner_epoch;
    uint32_t active;
    uint32_t command_bytes;
    uint32_t environment_bytes;
    ULONG standard_handles[3];
    char command[MVDM_COMMAND_NATIVE_CHILD_COMMAND_BYTES];
    char environment[MVDM_COMMAND_NATIVE_CHILD_ENVIRONMENT_BYTES];
} mvdm_command_native_child_state;

static int state_current(mvdm_command_native_child_state **state_out)
{
    session *owner = session_thread_current();
    mvdm_command_native_child_state *state;

    if (state_out != NULL) *state_out = NULL;
    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE) return 0;
    state = (mvdm_command_native_child_state *)owner->mvdm_command_native_child;
    if (state == NULL || state->owner != owner || state->owner_epoch != owner->epoch)
        return 0;
    if (state_out != NULL) *state_out = state;
    return 1;
}

static void state_teardown(void *context)
{
    mvdm_command_native_child_state *state =
        (mvdm_command_native_child_state *)context;
    if (state == NULL) return;
    if (state->owner != NULL && state->owner->mvdm_command_native_child == state)
        state->owner->mvdm_command_native_child = NULL;
    SecureZeroMemory(state, sizeof(*state));
    HeapFree(GetProcessHeap(), 0u, state);
}

static int state_ensure(mvdm_command_native_child_state **state_out)
{
    session *owner = session_thread_current();
    mvdm_command_native_child_state *state;

    if (state_out != NULL) *state_out = NULL;
    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE) return 0;
    state = (mvdm_command_native_child_state *)owner->mvdm_command_native_child;
    if (state != NULL) {
        if (state->owner != owner || state->owner_epoch != owner->epoch) return 0;
        if (state_out != NULL) *state_out = state;
        return 1;
    }
    state = (mvdm_command_native_child_state *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(*state));
    if (state == NULL) return 0;
    state->owner = owner;
    state->owner_epoch = owner->epoch;
    owner->mvdm_command_native_child = state;
    if (!session_register_teardown(owner, state_teardown, state)) {
        owner->mvdm_command_native_child = NULL;
        SecureZeroMemory(state, sizeof(*state));
        HeapFree(GetProcessHeap(), 0u, state);
        return 0;
    }
    if (state_out != NULL) *state_out = state;
    return 1;
}

static int copy_guest_command(const mvdm_guest_location *location,
    char destination[MVDM_COMMAND_NATIVE_CHILD_COMMAND_BYTES], uint32_t *bytes_out)
{
    mvdm_guest_location probe;
    mvdm_guest_location_lease lease;
    uint32_t index;

    if (bytes_out != NULL) *bytes_out = 0u;
    if (location == NULL || bytes_out == NULL) return 0;
    probe = *location;
    for (index = 0u; index + 1u < MVDM_COMMAND_NATIVE_CHILD_COMMAND_BYTES; ++index) {
        if (!mvdm_guest_location_acquire(&probe, 1u,
                GUEST_MEMORY_ACCESS_WRITE, &lease)) return 0;
        destination[index] = (char)lease.bytes[0];
        if (lease.bytes[0] == 0x0du) {
            /* Original cmdExec changes the CR terminator in place before it
             * starts cmdExec32.  Preserve that observable guest write while
             * keeping the copied worker input self-contained. */
            lease.bytes[0] = 0u;
            if (!mvdm_guest_location_release(&lease, 1)) return 0;
            destination[index] = '\0';
            *bytes_out = index + 1u;
            return 1;
        }
        if (!mvdm_guest_location_release(&lease, 0)) return 0;
        if (destination[index] == '\0') {
            *bytes_out = index + 1u;
            return 1;
        }
        if (probe.offset == UINT16_MAX) return 0;
        ++probe.offset;
    }
    return 0;
}

static int copy_guest_environment(const mvdm_guest_location *location,
    char destination[MVDM_COMMAND_NATIVE_CHILD_ENVIRONMENT_BYTES], uint32_t *bytes_out)
{
    mvdm_guest_location probe;
    mvdm_guest_location_lease lease;
    uint32_t index;
    uint32_t zero_run = 0u;

    if (bytes_out != NULL) *bytes_out = 0u;
    if (location == NULL || bytes_out == NULL) return 0;
    probe = *location;
    for (index = 0u; index < MVDM_COMMAND_NATIVE_CHILD_ENVIRONMENT_BYTES; ++index) {
        if (!mvdm_guest_location_acquire(&probe, 1u,
                GUEST_MEMORY_ACCESS_READ, &lease)) return 0;
        destination[index] = (char)lease.bytes[0];
        if (!mvdm_guest_location_release(&lease, 0)) return 0;
        if (destination[index] == '\0') {
            if (++zero_run == 2u) {
                *bytes_out = index + 1u;
                return 1;
            }
        } else {
            zero_run = 0u;
        }
        if (probe.offset == UINT16_MAX) return 0;
        ++probe.offset;
    }
    return 0;
}

static int copy_standard_handles(const mvdm_guest_location *location,
    ULONG handles_out[3])
{
    mvdm_guest_location_lease lease;
    uint32_t index;

    if (location == NULL || handles_out == NULL ||
        !mvdm_guest_location_acquire(location,
            MVDM_COMMAND_NATIVE_CHILD_STANDARD_BYTES,
            GUEST_MEMORY_ACCESS_READ, &lease)) return 0;
    for (index = 0u; index < 3u; ++index) {
        uint32_t offset = index * 4u;
        handles_out[index] = (ULONG)((uint32_t)lease.bytes[offset] |
            ((uint32_t)lease.bytes[offset + 1u] << 8u) |
            ((uint32_t)lease.bytes[offset + 2u] << 16u) |
            ((uint32_t)lease.bytes[offset + 3u] << 24u));
    }
    return mvdm_guest_location_release(&lease, 0);
}

static int capture_locations(mvdm_command_native_child_state *state,
    const char *host_command, const mvdm_guest_location *command_location,
    const mvdm_guest_location *environment_location,
    const mvdm_guest_location *standard_location)
{
    size_t bytes;

    if (state == NULL || environment_location == NULL ||
        standard_location == NULL || state->active != 0u) return 0;
    SecureZeroMemory(state->command, sizeof(state->command));
    SecureZeroMemory(state->environment, sizeof(state->environment));
    SecureZeroMemory(state->standard_handles, sizeof(state->standard_handles));
    if (host_command != NULL) {
        for (bytes = 0u; bytes < MVDM_COMMAND_NATIVE_CHILD_COMMAND_BYTES &&
                host_command[bytes] != '\0'; ++bytes) {
        }
        if (bytes == MVDM_COMMAND_NATIVE_CHILD_COMMAND_BYTES) return 0;
        memcpy(state->command, host_command, bytes + 1u);
        state->command_bytes = (uint32_t)bytes + 1u;
    } else if (!copy_guest_command(command_location, state->command,
            &state->command_bytes)) {
        return 0;
    }
    if (!copy_guest_environment(environment_location, state->environment,
            &state->environment_bytes) || !copy_standard_handles(standard_location,
            state->standard_handles)) {
        SecureZeroMemory(state->command, sizeof(state->command));
        SecureZeroMemory(state->environment, sizeof(state->environment));
        SecureZeroMemory(state->standard_handles, sizeof(state->standard_handles));
        state->command_bytes = state->environment_bytes = 0u;
        return 0;
    }
    state->active = 1u;
    record_command(host_command != NULL ? "comspec" : "guest-tail", state->command);
    return 1;
}

int mvdm_command_native_child_capture_guest(uint16_t command_segment,
    uint16_t command_offset, uint16_t environment_segment,
    uint16_t environment_offset, uint16_t standard_segment,
    uint16_t standard_offset)
{
    mvdm_command_native_child_state *state;
    mvdm_guest_location command;
    mvdm_guest_location environment;
    mvdm_guest_location standard;

    return state_ensure(&state) &&
        mvdm_guest_location_set_real_mode(&command, command_segment, command_offset) &&
        mvdm_guest_location_set_real_mode(&environment, environment_segment,
            environment_offset) &&
        mvdm_guest_location_set_real_mode(&standard, standard_segment,
            standard_offset) &&
        capture_locations(state, NULL, &command, &environment, &standard);
}

int mvdm_command_native_child_capture_host_command(const char *command,
    uint16_t environment_segment, uint16_t environment_offset,
    uint16_t standard_segment, uint16_t standard_offset)
{
    mvdm_command_native_child_state *state;
    mvdm_guest_location environment;
    mvdm_guest_location standard;

    return command != NULL && state_ensure(&state) &&
        mvdm_guest_location_set_real_mode(&environment, environment_segment,
            environment_offset) &&
        mvdm_guest_location_set_real_mode(&standard, standard_segment,
            standard_offset) &&
        capture_locations(state, command, NULL, &environment, &standard);
}

int mvdm_command_native_child_activate(const char *source_command,
    const char *source_environment, const char **command_out,
    const char **environment_out)
{
    mvdm_command_native_child_state *state;

    if (command_out != NULL) *command_out = NULL;
    if (environment_out != NULL) *environment_out = NULL;
    if (source_command == NULL || source_environment == NULL ||
        command_out == NULL || environment_out == NULL || !state_current(&state) ||
        state->active != 1u) return 0;
    *command_out = state->command;
    *environment_out = state->environment;
    return 1;
}

int mvdm_command_native_child_std_handles(ULONG handles_out[3])
{
    mvdm_command_native_child_state *state;
    if (handles_out != NULL) SecureZeroMemory(handles_out, sizeof(ULONG) * 3u);
    if (handles_out == NULL || !state_current(&state) || state->active != 1u)
        return 0;
    memcpy(handles_out, state->standard_handles, sizeof(state->standard_handles));
    return 1;
}

const char *mvdm_command_native_child_command(void)
{
    mvdm_command_native_child_state *state;
    return state_current(&state) && state->active == 1u ? state->command : NULL;
}

const char *mvdm_command_native_child_environment(void)
{
    mvdm_command_native_child_state *state;
    return state_current(&state) && state->active == 1u ? state->environment : NULL;
}

int mvdm_command_native_child_replace_command(const char *command)
{
    mvdm_command_native_child_state *state;
    size_t bytes;

    if (command == NULL || !state_current(&state) || state->active != 1u)
        return 0;
    for (bytes = 0u; bytes < sizeof(state->command) && command[bytes] != '\0';
            ++bytes) {
    }
    if (bytes == sizeof(state->command)) return 0;
    SecureZeroMemory(state->command, sizeof(state->command));
    memcpy(state->command, command, bytes + 1u);
    state->command_bytes = (uint32_t)bytes + 1u;
    record_command("worker", state->command);
    return 1;
}

void mvdm_command_native_child_finish(void)
{
    mvdm_command_native_child_state *state;
    if (!state_current(&state)) return;
    SecureZeroMemory(state->command, sizeof(state->command));
    SecureZeroMemory(state->environment, sizeof(state->environment));
    SecureZeroMemory(state->standard_handles, sizeof(state->standard_handles));
    state->command_bytes = state->environment_bytes = state->active = 0u;
}

void mvdm_command_native_child_abort(void)
{
    mvdm_command_native_child_finish();
}
