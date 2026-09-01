#include "mvdm_softpc_termination.h"

#include <stdio.h>
#include <string.h>

#include <windows.h>

#include "session/session.h"

enum {
    fixture_memory_bytes = 0x5000u,
    fixture_ntio_sysinit_segment = 0x010du,
    fixture_pass_address = (fixture_ntio_sysinit_segment << 4) + 0x03d8u,
    fixture_command_address = (fixture_ntio_sysinit_segment << 4) + 0x3466u
};

typedef struct fixture_memory {
    uint8_t bytes[fixture_memory_bytes];
    uint32_t reads;
    uint32_t writes;
} fixture_memory;

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;

    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    ++memory->reads;
    return 1;
}

static int write_memory(void *context, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;

    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    ++memory->writes;
    return 1;
}

int main(void)
{
    session owner;
    fixture_memory memory;
    char temp_path[MAX_PATH];
    char report_path[MAX_PATH];
    char report[256];
    FILE *file;
    size_t bytes_read;

    memset(&memory, 0, sizeof(memory));
    if (GetTempPathA((DWORD)sizeof(temp_path), temp_path) == 0u ||
        GetTempFileNameA(temp_path, "t35", 0u, report_path) == 0u ||
        !DeleteFileA(report_path)) return 10;
    session_initialize(&owner, 355u);
    if (!session_activate(&owner) ||
        !session_guest_memory_begin(&owner, &memory, read_memory, write_memory) ||
        !session_thread_bind(&owner)) return 11;

    /* Default-off is a negative proof: without its child-only report path, the
     * observer does not read guest memory or create a report. */
    mvdm_softpc_record_config_done(fixture_ntio_sysinit_segment);
    if (memory.reads != 0u || memory.writes != 0u ||
        GetFileAttributesA(report_path) != INVALID_FILE_ATTRIBUTES) return 12;

    memory.bytes[fixture_pass_address] = 1u;
    memcpy(memory.bytes + fixture_command_address, "O:\\ntvdm\\mvdm\\System32\\command.com",
        sizeof("O:\\ntvdm\\mvdm\\System32\\command.com"));
    if (!SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH", report_path))
        return 13;
    mvdm_softpc_record_config_done(fixture_ntio_sysinit_segment);
    SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH", NULL);
    if (memory.reads != 2u || memory.writes != 0u) return 14;
    if (fopen_s(&file, report_path, "rb") != 0 || file == NULL) return 15;
    bytes_read = fread(report, 1u, sizeof(report) - 1u, file);
    fclose(file);
    report[bytes_read] = '\0';
    DeleteFileA(report_path);
    if (strstr(report, "cs=010D") == NULL || strstr(report, "pass=01") == NULL ||
        strstr(report, "command=O:\\ntvdm\\mvdm\\System32\\command.com") == NULL ||
        strstr(report, "command-state=copied") == NULL) return 16;

    if (!session_thread_unbind(&owner)) return 17;
    session_guest_memory_end(&owner);
    return session_dispose(&owner) ? 0 : 18;
}
