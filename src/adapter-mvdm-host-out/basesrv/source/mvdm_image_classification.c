#include "adapter-mvdm-host-out/basesrv/include/mvdm_image_classification.h"

#include <string.h>

static int has_extension(const char *path)
{
    const char *dot = strrchr(path, '.');
    const char *slash = strrchr(path, '\\');
    const char *forward = strrchr(path, '/');

    if (forward != NULL && (slash == NULL || forward > slash)) slash = forward;
    return dot != NULL && (slash == NULL || dot > slash);
}

static int copy_image_token(const char *text, char *image,
    size_t image_bytes)
{
    const char *cursor;
    size_t count = 0u;
    int quoted = 0;

    if (text == NULL || image == NULL || image_bytes == 0u) return 0;
    cursor = text;
    while (*cursor == ' ' || *cursor == '\t') ++cursor;
    if (*cursor == '\0') return 0;
    if (*cursor == '"') {
        quoted = 1;
        ++cursor;
    }
    while (*cursor != '\0') {
        if ((quoted && *cursor == '"') ||
            (!quoted && (*cursor == ' ' || *cursor == '\t'))) break;
        if (strchr("|&<>\r\n", *cursor) != NULL || count + 1u >= image_bytes)
            return 0;
        image[count++] = *cursor++;
    }
    if (quoted && *cursor != '"') return 0;
    if (count == 0u) return 0;
    image[count] = '\0';
    return 1;
}

mvdm_image_kind mvdm_image_classify_path(const char *path)
{
    DWORD binary_type;
    HANDLE file;
    unsigned char header[64];
    unsigned char signature[2];
    DWORD read = 0u;
    DWORD offset;

    if (path == NULL || path[0] == '\0') return MVDM_IMAGE_UNKNOWN;
    file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) goto system_type;
    if (!ReadFile(file, header, sizeof(header), &read, NULL) || read < 2u) {
        CloseHandle(file);
        goto system_type;
    }
    if (header[0] != 'M' || header[1] != 'Z') {
        CloseHandle(file);
        return has_extension(path) && _stricmp(strrchr(path, '.'), ".COM") == 0 ?
            MVDM_IMAGE_DOS : MVDM_IMAGE_UNKNOWN;
    }
    if (read < sizeof(header)) {
        CloseHandle(file);
        goto system_type;
    }
    offset = (DWORD)header[60] | ((DWORD)header[61] << 8) |
        ((DWORD)header[62] << 16) | ((DWORD)header[63] << 24);
    if (SetFilePointer(file, (LONG)offset, NULL, FILE_BEGIN) ==
            INVALID_SET_FILE_POINTER ||
        !ReadFile(file, signature, sizeof(signature), &read, NULL) || read != 2u) {
        CloseHandle(file);
        goto system_type;
    }
    CloseHandle(file);
    if (signature[0] == 'N' && signature[1] == 'E') return MVDM_IMAGE_WIN16;
    if (signature[0] == 'P' && signature[1] == 'E') return MVDM_IMAGE_NATIVE;
system_type:
    if (GetBinaryTypeA(path, &binary_type)) {
        if (binary_type == SCS_DOS_BINARY) return MVDM_IMAGE_DOS;
        if (binary_type == SCS_WOW_BINARY) return MVDM_IMAGE_WIN16;
        return MVDM_IMAGE_NATIVE;
    }
    return MVDM_IMAGE_UNKNOWN;
}

mvdm_image_kind mvdm_image_classify_command_line(const char *command_line)
{
    char image[MAX_PATH];
    char resolved[MAX_PATH];
    DWORD result;

    if (!copy_image_token(command_line, image, sizeof(image)))
        return MVDM_IMAGE_UNKNOWN;
    result = SearchPathA(NULL, image, has_extension(image) ? NULL : ".EXE",
        (DWORD)sizeof(resolved), resolved, NULL);
    if (result == 0u || result >= sizeof(resolved)) return MVDM_IMAGE_UNKNOWN;
    return mvdm_image_classify_path(resolved);
}

int mvdm_image_launch_native(char *command_line, DWORD *exit_code_out,
    DWORD *failure_out)
{
    STARTUPINFOA startup;
    PROCESS_INFORMATION process;
    DWORD wait_result;

    if (exit_code_out != NULL) *exit_code_out = 0u;
    if (failure_out != NULL) *failure_out = ERROR_SUCCESS;
    if (command_line == NULL || command_line[0] == '\0') {
        if (failure_out != NULL) *failure_out = ERROR_INVALID_PARAMETER;
        return 0;
    }
    memset(&startup, 0, sizeof(startup));
    memset(&process, 0, sizeof(process));
    startup.cb = sizeof(startup);
    if (!CreateProcessA(NULL, command_line, NULL, NULL, TRUE, 0u, NULL, NULL,
            &startup, &process)) {
        if (failure_out != NULL) *failure_out = GetLastError();
        return 0;
    }
    CloseHandle(process.hThread);
    wait_result = WaitForSingleObject(process.hProcess, INFINITE);
    if (wait_result != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(process.hProcess, exit_code_out)) {
        if (failure_out != NULL) *failure_out = wait_result == WAIT_FAILED ?
            GetLastError() : ERROR_GEN_FAILURE;
        CloseHandle(process.hProcess);
        return 0;
    }
    CloseHandle(process.hProcess);
    return 1;
}
