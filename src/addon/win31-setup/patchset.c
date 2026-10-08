/*
 * PATCHSET is an installation-time finalizer for the Windows 1.01/3.1
 * add-on packages.  It is not a product component and has no process
 * relationship with run16, NTSRV, or a worker.  Batch files invoke it once
 * after original Setup exits so shipped PATCH directories need not expose
 * PowerShell sources, PIF templates, or generated metadata.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define PIF_FIXED_BYTES 0x171u
#define PIF_EXTENSION_START 0x171u
#define PIF_NEXT_OFFSET 16u
#define PIF_DATA_OFFSET 18u
#define PIF_LENGTH_OFFSET 20u

#pragma comment(lib, "bcrypt.lib")

#define WIN386_RETAIL_SHA256 "6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5"
#define WIN386_PATCHED_SHA256 "C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8"
#define KRNL386_RETAIL_SHA256 "FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980"
#define KRNL386_PATCHED_SHA256 "88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181"

static int fail(const wchar_t *message)
{
    fwprintf(stderr, L"PATCHSET: %ls (error %lu)\n", message, GetLastError());
    return 1;
}

static BOOL join_path(wchar_t *output, DWORD capacity, const wchar_t *left,
                      const wchar_t *right)
{
    size_t a = wcslen(left), b = wcslen(right);
    if (a + 1u + b + 1u > capacity) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    wcscpy_s(output, capacity, left);
    if (a && output[a - 1u] != L'\\') wcscat_s(output, capacity, L"\\");
    wcscat_s(output, capacity, right);
    return TRUE;
}

static BOOL make_directory(const wchar_t *path)
{
    if (CreateDirectoryW(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
        return TRUE;
    return FALSE;
}

static BOOL require_file(const wchar_t *root, const wchar_t *name)
{
    wchar_t path[MAX_PATH];
    DWORD attributes;
    if (!join_path(path, ARRAYSIZE(path), root, name)) return FALSE;
    attributes = GetFileAttributesW(path);
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        SetLastError(ERROR_FILE_NOT_FOUND);
        return FALSE;
    }
    return TRUE;
}

static BOOL copy_named_file(const wchar_t *source_root, const wchar_t *name,
                            const wchar_t *destination_root)
{
    wchar_t source[MAX_PATH], destination[MAX_PATH];
    if (!join_path(source, ARRAYSIZE(source), source_root, name) ||
        !join_path(destination, ARRAYSIZE(destination), destination_root, name))
        return FALSE;
    return CopyFileW(source, destination, FALSE);
}

static BOOL sha256_file(const wchar_t *path, char output[65])
{
    BCRYPT_ALG_HANDLE algorithm = NULL;
    BCRYPT_HASH_HANDLE hash = NULL;
    BYTE digest[32], buffer[8192];
    DWORD object_bytes = 0, returned = 0, read = 0, i;
    BYTE *object = NULL;
    HANDLE file = INVALID_HANDLE_VALUE;
    NTSTATUS status;
    BOOL ok = FALSE;

    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) goto done;
    status = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    if (status < 0) { SetLastError(ERROR_INVALID_DATA); goto done; }
    status = BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&object_bytes,
                               sizeof(object_bytes), &returned, 0);
    if (status < 0 || returned != sizeof(object_bytes)) { SetLastError(ERROR_INVALID_DATA); goto done; }
    object = HeapAlloc(GetProcessHeap(), 0, object_bytes);
    if (!object) goto done;
    status = BCryptCreateHash(algorithm, &hash, object, object_bytes, NULL, 0, 0);
    if (status < 0) { SetLastError(ERROR_INVALID_DATA); goto done; }
    for (;;) {
        if (!ReadFile(file, buffer, ARRAYSIZE(buffer), &read, NULL)) goto done;
        if (read == 0) break;
        if (BCryptHashData(hash, buffer, read, 0) < 0) { SetLastError(ERROR_INVALID_DATA); goto done; }
    }
    if (BCryptFinishHash(hash, digest, ARRAYSIZE(digest), 0) < 0) {
        SetLastError(ERROR_INVALID_DATA); goto done;
    }
    for (i = 0; i < ARRAYSIZE(digest); ++i) sprintf_s(output + i * 2u, 65u - i * 2u, "%02X", digest[i]);
    ok = TRUE;
done:
    if (hash) BCryptDestroyHash(hash);
    if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    if (object) HeapFree(GetProcessHeap(), 0, object);
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    return ok;
}

static BOOL copy_if_missing(const wchar_t *source, const wchar_t *destination)
{
    DWORD attributes = GetFileAttributesW(destination);
    if (attributes != INVALID_FILE_ATTRIBUTES) return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    return CopyFileW(source, destination, TRUE);
}

static BOOL ensure_recovery_copy(const wchar_t *source, const wchar_t *destination,
                                 const char *expected_hash)
{
    char actual[65];
    DWORD attributes = GetFileAttributesW(destination);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        if (!CopyFileW(source, destination, TRUE)) return FALSE;
    } else if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        SetLastError(ERROR_ALREADY_EXISTS);
        return FALSE;
    }
    if (!sha256_file(destination, actual) || _stricmp(actual, expected_hash) != 0) {
        SetLastError(ERROR_INVALID_DATA);
        return FALSE;
    }
    return TRUE;
}

static BOOL read_resource_template(BYTE **bytes, DWORD *length)
{
    HRSRC resource;
    HGLOBAL data;
    void *source;
    BYTE *copy;

    *bytes = NULL;
    *length = 0;
    resource = FindResourceW(NULL, L"PIF_TEMPLATE", RT_RCDATA);
    if (resource == NULL) return FALSE;
    *length = SizeofResource(NULL, resource);
    data = LoadResource(NULL, resource);
    source = data ? LockResource(data) : NULL;
    if (source == NULL || *length < PIF_FIXED_BYTES) {
        SetLastError(ERROR_INVALID_DATA);
        return FALSE;
    }
    copy = HeapAlloc(GetProcessHeap(), 0, *length);
    if (copy == NULL) return FALSE;
    CopyMemory(copy, source, *length);
    *bytes = copy;
    return TRUE;
}

static BOOL pif_put(BYTE *pif, DWORD bytes, DWORD offset, DWORD capacity,
                    const wchar_t *text)
{
    DWORD length;
    if (offset > bytes || capacity > bytes - offset) {
        SetLastError(ERROR_INVALID_DATA);
        return FALSE;
    }
    if (WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, text, -1,
                            (char *)pif + offset, capacity, NULL, NULL) == 0)
        return FALSE;
    length = (DWORD)strlen((char *)pif + offset);
    if (length + 1u >= capacity || offset + capacity > bytes) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    ZeroMemory(pif + offset + length + 1u, capacity - length - 1u);
    return TRUE;
}

static WORD pif_word(const BYTE *pif, DWORD offset)
{
    return (WORD)(pif[offset] | ((WORD)pif[offset + 1u] << 8));
}

static BOOL pif_make(BYTE *pif, DWORD bytes, const wchar_t *title,
                     const wchar_t *program, const wchar_t *directory,
                     const wchar_t *arguments, const wchar_t *config,
                     const wchar_t *autoexec)
{
    DWORD position = PIF_EXTENSION_START;
    BOOL seen_nt = FALSE, seen_386 = FALSE;
    BYTE sum = 0;
    DWORD i;

    if (bytes < PIF_FIXED_BYTES ||
        !pif_put(pif, bytes, 2u, 30u, title) ||
        !pif_put(pif, bytes, 0x24u, 63u, program) ||
        !pif_put(pif, bytes, 0x65u, 64u, directory) ||
        !pif_put(pif, bytes, 0xa5u, 64u, arguments))
        return FALSE;
    for (i = 0; position != 0xffffu; ++i) {
        char signature[17] = {0};
        WORD next, data, length;
        if (i > 255u || position + 22u > bytes) {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }
        CopyMemory(signature, pif + position, 16u);
        next = pif_word(pif, position + PIF_NEXT_OFFSET);
        data = pif_word(pif, position + PIF_DATA_OFFSET);
        length = pif_word(pif, position + PIF_LENGTH_OFFSET);
        if ((DWORD)data + length > bytes) {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }
        if (!strncmp(signature, "WINDOWS 386 3.0", 15u)) {
            if (length < 104u || !pif_put(pif, bytes, (DWORD)data + 40u, 64u, arguments))
                return FALSE;
            seen_386 = TRUE;
        }
        if (!strncmp(signature, "WINDOWS NT  3.1", 15u)) {
            if (length < 140u ||
                !pif_put(pif, bytes, (DWORD)data + 12u, 64u, config) ||
                !pif_put(pif, bytes, (DWORD)data + 76u, 64u, autoexec))
                return FALSE;
            seen_nt = TRUE;
        }
        if (next == position) {
            SetLastError(ERROR_INVALID_DATA);
            return FALSE;
        }
        position = next;
    }
    if (!seen_nt || !seen_386) {
        SetLastError(ERROR_INVALID_DATA);
        return FALSE;
    }
    pif[1] = 0;
    for (i = 2u; i < PIF_FIXED_BYTES; ++i) sum = (BYTE)(sum + pif[i]);
    pif[1] = sum;
    return TRUE;
}

static BOOL write_bytes(const wchar_t *path, const BYTE *bytes, DWORD length)
{
    HANDLE file;
    DWORD written;
    file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    if (!WriteFile(file, bytes, length, &written, NULL) || written != length) {
        DWORD error = GetLastError();
        CloseHandle(file);
        SetLastError(error ? error : ERROR_WRITE_FAULT);
        return FALSE;
    }
    return CloseHandle(file);
}

static BOOL write_text(const wchar_t *path, const wchar_t *text)
{
    int bytes;
    BYTE *buffer;
    BOOL result;
    bytes = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, text, -1,
                                NULL, 0, NULL, NULL);
    if (!bytes) return FALSE;
    buffer = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)bytes);
    if (!buffer) return FALSE;
    if (!WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, text, -1,
                             (char *)buffer, bytes, NULL, NULL)) {
        HeapFree(GetProcessHeap(), 0, buffer);
        return FALSE;
    }
    result = write_bytes(path, buffer, (DWORD)bytes - 1u);
    HeapFree(GetProcessHeap(), 0, buffer);
    return result;
}

static BOOL patch_system_ini(const wchar_t *root, const wchar_t *driver)
{
    wchar_t path[MAX_PATH], temporary[MAX_PATH];
    HANDLE file;
    DWORD size, read, written;
    char *source = NULL, *result = NULL, *line, *end;
    size_t replacement;
    BOOL ok = FALSE;

    if (!join_path(path, ARRAYSIZE(path), root, L"SYSTEM.INI") ||
        !join_path(temporary, ARRAYSIZE(temporary), root, L"SYSTEM.INI.PATCHSET"))
        return FALSE;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    size = GetFileSize(file, NULL);
    if (size == INVALID_FILE_SIZE || size > 1024u * 1024u) goto done;
    source = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)size + 1u);
    if (!source || !ReadFile(file, source, size, &read, NULL) || read != size) goto done;
    CloseHandle(file); file = INVALID_HANDLE_VALUE;
    line = strstr(source, "[boot]");
    if (!line) { SetLastError(ERROR_INVALID_DATA); goto done; }
    end = strstr(line + 6, "\n[");
    if (!end) end = source + size;
    for (; line < end; ++line) {
        if ((line == source || line[-1] == '\n') &&
            !_strnicmp(line, "mouse.drv=", 10u)) break;
    }
    if (line >= end) { SetLastError(ERROR_INVALID_DATA); goto done; }
    {
        char ansi[MAX_PATH];
        DWORD characters = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, driver,
                                               -1, ansi, ARRAYSIZE(ansi), NULL, NULL);
        char *line_end = strchr(line, '\n');
        if (!characters || !line_end) { SetLastError(ERROR_INVALID_DATA); goto done; }
        replacement = strlen("mouse.drv=") + strlen(ansi) + 2u;
        result = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)size + replacement + 1u);
        if (!result) goto done;
        CopyMemory(result, source, (SIZE_T)(line - source));
        sprintf_s(result + (line - source), (size_t)size + replacement + 1u - (size_t)(line - source),
                  "mouse.drv=%s\r\n%s", ansi, line_end + 1);
    }
    file = CreateFileW(temporary, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE ||
        !WriteFile(file, result, (DWORD)strlen(result), &written, NULL) ||
        written != strlen(result) || !CloseHandle(file)) goto done;
    file = INVALID_HANDLE_VALUE;
    if (!MoveFileExW(temporary, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) goto done;
    ok = TRUE;
done:
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (!ok) DeleteFileW(temporary);
    if (result) HeapFree(GetProcessHeap(), 0, result);
    if (source) HeapFree(GetProcessHeap(), 0, source);
    return ok;
}

/* The owner-approved S1/S3 experiments are strictly identity-bound.  The
 * first untouched retail file is retained beside the installed patch, and no
 * other KRNL386 or WIN386 image is accepted or rewritten. */
static BOOL install_binary_adaptation(const wchar_t *root, const wchar_t *patch,
                                       const wchar_t *package, const wchar_t *target_name,
                                       const wchar_t *candidate_name, const wchar_t *backup_name,
                                       const wchar_t *retail_name,
                                       const char *retail_hash, const char *candidate_hash)
{
    wchar_t target[MAX_PATH], candidate[MAX_PATH], backup[MAX_PATH], retail[MAX_PATH];
    char current[65], supplied[65], supplied_retail[65];

    if (!join_path(target, ARRAYSIZE(target), root, target_name) ||
        !join_path(candidate, ARRAYSIZE(candidate), package, candidate_name) ||
        !join_path(backup, ARRAYSIZE(backup), patch, backup_name) ||
        !join_path(retail, ARRAYSIZE(retail), package, retail_name) ||
        !sha256_file(candidate, supplied) ||
        _stricmp(supplied, candidate_hash) != 0 ||
        !sha256_file(target, current)) {
        SetLastError(ERROR_INVALID_DATA);
        return FALSE;
    }
    if (_stricmp(current, candidate_hash) == 0) {
        /* A derived Setup source can legitimately copy the approved candidate
         * before PATCHSET first runs.  Establish recovery only from the
         * separately identity-checked retail package file, never from the
         * candidate or an arbitrary installed image. */
        if (!sha256_file(retail, supplied_retail) ||
            _stricmp(supplied_retail, retail_hash) != 0 ||
            !ensure_recovery_copy(retail, backup, retail_hash)) {
            fwprintf(stderr, L"PATCHSET: missing or invalid recovery copy %ls\n", backup);
            return FALSE;
        }
        return TRUE;
    }
    if (_stricmp(current, retail_hash) != 0) {
        SetLastError(ERROR_INVALID_DATA);
        return FALSE;
    }
    if (!ensure_recovery_copy(target, backup, retail_hash)) {
        fwprintf(stderr, L"PATCHSET: could not preserve %ls as %ls\n", target, backup);
        return FALSE;
    }
    if (!CopyFileW(candidate, target, FALSE)) {
        fwprintf(stderr, L"PATCHSET: could not install %ls as %ls\n", candidate, target);
        return FALSE;
    }
    return TRUE;
}

static BOOL finalize_win31(const wchar_t *root, const wchar_t *package)
{
    wchar_t patch[MAX_PATH], path[MAX_PATH], config[MAX_PATH], autoexec[MAX_PATH];
    wchar_t program[MAX_PATH], driver[MAX_PATH], ini[MAX_PATH], ini_backup[MAX_PATH];
    BYTE *template = NULL;
    DWORD template_bytes = 0;
    static const wchar_t config_text[] =
        L"REM Windows 3.1 private DOS profile\r\n"
        L"dos=high, umb\r\n"
        L"device=%SystemRoot%\\system32\\himem.sys\r\n"
        L"files=128\r\n"
        L"dosonly\r\n";
    wchar_t auto_text[2 * MAX_PATH + 256];
    wchar_t command[2 * MAX_PATH + 256];
    const wchar_t *stale[] = {
        L"configure-launch.ps1", L"run-setup.ps1", L"addon-files.json",
        L"WIN31-TEMPLATE.PIF", L"winstd.cmd.template", L"win386.cmd.template",
        L"setup-result.txt"
    };
    DWORD i;

    if (!require_file(root, L"WIN.COM") || !require_file(root, L"SYSTEM\\DOSX.EXE") ||
        !require_file(root, L"SYSTEM\\KRNL386.EXE") || !require_file(root, L"SYSTEM\\WIN386.EXE") ||
        !require_file(root, L"SYSTEM.INI") || !require_file(package, L"MOUSE31.DRV") ||
        !require_file(package, L"WIN386-ADAPTED.EXE") ||
        !require_file(package, L"KRNL386-ADAPTED.EXE") ||
        !require_file(package, L"WIN386-RETAIL.EXE") ||
        !require_file(package, L"KRNL386-RETAIL.EXE")) return FALSE;
    if (!join_path(patch, ARRAYSIZE(patch), root, L"PATCH") || !make_directory(patch) ||
        !copy_named_file(package, L"MOUSE31.DRV", patch) ||
        !join_path(driver, ARRAYSIZE(driver), patch, L"MOUSE31.DRV") ||
        !install_binary_adaptation(root, patch, package, L"SYSTEM\\KRNL386.EXE",
                                   L"KRNL386-ADAPTED.EXE", L"KRNL386.ORIG",
                                   L"KRNL386-RETAIL.EXE",
                                   KRNL386_RETAIL_SHA256, KRNL386_PATCHED_SHA256) ||
        !install_binary_adaptation(root, patch, package, L"SYSTEM\\WIN386.EXE",
                                   L"WIN386-ADAPTED.EXE", L"WIN386.ORIG",
                                   L"WIN386-RETAIL.EXE",
                                   WIN386_RETAIL_SHA256, WIN386_PATCHED_SHA256) ||
        !join_path(ini, ARRAYSIZE(ini), root, L"SYSTEM.INI") ||
        !join_path(ini_backup, ARRAYSIZE(ini_backup), patch, L"SYSTEM.INI.ORIG") ||
        !copy_if_missing(ini, ini_backup) ||
        !patch_system_ini(root, driver)) return FALSE;
    if (!join_path(config, ARRAYSIZE(config), patch, L"CONFIG.NT") ||
        !join_path(autoexec, ARRAYSIZE(autoexec), patch, L"AUTOEXEC.NT") ||
        !write_text(config, config_text) ||
        swprintf_s(auto_text, ARRAYSIZE(auto_text),
                   L"@echo off\r\nSET TEMP=%%SystemRoot%%\\Temp\r\nSET TMP=%%SystemRoot%%\\Temp\r\n"
                   L"SET PATH=%ls;%ls\\SYSTEM;%%SystemRoot%%\\system32\r\n", root, root) < 0 ||
        !write_text(autoexec, auto_text) ||
        !join_path(program, ARRAYSIZE(program), root, L"WIN.COM") ||
        !read_resource_template(&template, &template_bytes)) return FALSE;
    if (!pif_make(template, template_bytes, L"Windows 3.1 Standard", program, root, L"/S", config, autoexec) ||
        !join_path(path, ARRAYSIZE(path), patch, L"WINSTD.PIF") ||
        !write_bytes(path, template, template_bytes)) goto done;
    if (!pif_make(template, template_bytes, L"Windows 3.1 386 Enhanced", program, root, L"/3", config, autoexec) ||
        !join_path(path, ARRAYSIZE(path), patch, L"WIN386.PIF") ||
        !write_bytes(path, template, template_bytes)) goto done;
    if (swprintf_s(command, ARRAYSIZE(command),
                   L"@echo off\r\nsetlocal\r\nwhere run16 >nul 2>nul || exit /b 2\r\n"
                   L"pushd \"%%~dp0..\" || exit /b 3\r\ncall run16 \"%%~dp0WINSTD.PIF\"\r\n"
                   L"set result=%%errorlevel%%\r\npopd\r\nexit /b %%result%%\r\n") < 0 ||
        !join_path(path, ARRAYSIZE(path), patch, L"WINSTD.CMD") || !write_text(path, command)) goto done;
    if (swprintf_s(command, ARRAYSIZE(command),
                   L"@echo off\r\nsetlocal\r\nwhere run16 >nul 2>nul || exit /b 2\r\n"
                   L"pushd \"%%~dp0..\" || exit /b 3\r\ncall run16 \"%%~dp0WIN386.PIF\"\r\n"
                   L"set result=%%errorlevel%%\r\npopd\r\nexit /b %%result%%\r\n") < 0 ||
        !join_path(path, ARRAYSIZE(path), patch, L"WIN386.CMD") || !write_text(path, command)) goto done;
    for (i = 0; i < ARRAYSIZE(stale); ++i) {
        if (join_path(path, ARRAYSIZE(path), patch, stale[i])) DeleteFileW(path);
    }
    HeapFree(GetProcessHeap(), 0, template);
    wprintf(L"PATCHSET: configured %ls\\PATCH\n", root);
    return TRUE;
done:
    if (template) HeapFree(GetProcessHeap(), 0, template);
    return FALSE;
}

int wmain(int argc, wchar_t **argv)
{
    wchar_t package[MAX_PATH], module[MAX_PATH], *slash;
    if (argc != 3 || _wcsicmp(argv[1], L"--win31")) {
        fwprintf(stderr, L"Usage: PATCHSET.EXE --win31 <installed-directory>\n");
        return 64;
    }
    if (!GetModuleFileNameW(NULL, module, ARRAYSIZE(module))) return fail(L"locate package");
    slash = wcsrchr(module, L'\\');
    if (slash == NULL) { SetLastError(ERROR_INVALID_DATA); return fail(L"locate package"); }
    *slash = L'\0';
    if (!GetFullPathNameW(module, ARRAYSIZE(package), package, NULL)) return fail(L"normalize package");
    if (!finalize_win31(argv[2], package)) return fail(L"configure Windows 3.1");
    return 0;
}
