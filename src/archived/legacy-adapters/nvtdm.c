#include <windows.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "dos_namespace.h"
#include "dos_file_service.h"
#include "bootstrap_profile.h"

#define NVDTM_GUEST_BYTES (1024u * 1024u)
#define NVDTM_LOAD_SEGMENT 0x1000u
#define NVDTM_PSP_BYTES 0x100u
#define NVDTM_MODULE_SEGMENT (NVDTM_LOAD_SEGMENT + 0x10u)

typedef struct runtime_file {
    const wchar_t *name;
    int required;
} runtime_file;

typedef struct staged_image {
    uint8_t *memory;
    uint16_t cs;
    uint16_t ip;
    uint16_t ss;
    uint16_t sp;
    uint16_t ds;
    uint16_t es;
    size_t bytes;
} staged_image;

typedef struct probe_cpu {
    uint16_t ax, bx, cx, dx, sp, bp, si, di;
    uint16_t cs, ip, ss, ds, es;
    uint16_t flags;
} probe_cpu;

static const runtime_file runtime_files[] = {
    { L"NTIO.SYS", 1 }, { L"NTDOS.SYS", 1 }, { L"COMMAND.COM", 1 },
    { L"HIMEM.SYS", 1 }, { L"DOSX.EXE", 1 }, { L"VDMREDIR.DLL", 0 },
    { L"EDIT.COM", 0 }
};

static int read_file(const wchar_t *path, uint8_t **out_data, size_t *out_bytes);

static int make_runtime_path(const wchar_t *root, const wchar_t *name,
    wchar_t out_path[MAX_PATH])
{
    const size_t root_length = wcslen(root);
    const size_t name_length = wcslen(name);
    const int slash = root_length != 0u && root[root_length - 1u] != L'\\';

    if (root_length + (size_t)slash + name_length + 1u > MAX_PATH) return 0;
    memcpy(out_path, root, root_length * sizeof(*root));
    if (slash) out_path[root_length] = L'\\';
    memcpy(out_path + root_length + (size_t)slash, name,
        (name_length + 1u) * sizeof(*name));
    return 1;
}

static int fingerprint_file(const wchar_t *path, wchar_t hex[65])
{
    BCRYPT_ALG_HANDLE algorithm = NULL;
    BCRYPT_HASH_HANDLE hash = NULL;
    DWORD object_bytes = 0u;
    DWORD property_bytes = 0u;
    BYTE *object = NULL;
    BYTE digest[32];
    BYTE buffer[4096];
    HANDLE file = INVALID_HANDLE_VALUE;
    DWORD read_bytes;
    NTSTATUS status;
    size_t index;

    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    status = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM,
        NULL, 0u);
    if (file == INVALID_HANDLE_VALUE || status < 0 ||
        BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&object_bytes,
        sizeof(object_bytes), &property_bytes, 0u) < 0) goto fail;
    object = (BYTE *)HeapAlloc(GetProcessHeap(), 0, object_bytes);
    if (object == NULL || BCryptCreateHash(algorithm, &hash, object, object_bytes,
        NULL, 0u, 0u) < 0) goto fail;
    for (;;) {
        if (!ReadFile(file, buffer, sizeof(buffer), &read_bytes, NULL)) goto fail;
        if (read_bytes == 0u) break;
        if (BCryptHashData(hash, buffer, read_bytes, 0u) < 0) goto fail;
    }
    if (BCryptFinishHash(hash, digest, sizeof(digest), 0u) < 0) goto fail;
    for (index = 0u; index < ARRAYSIZE(digest); ++index) {
        swprintf(hex + index * 2u, 3u, L"%02x", digest[index]);
    }
    hex[64] = L'\0';
    BCryptDestroyHash(hash);
    HeapFree(GetProcessHeap(), 0, object);
    BCryptCloseAlgorithmProvider(algorithm, 0u);
    CloseHandle(file);
    return 1;

fail:
    if (hash != NULL) BCryptDestroyHash(hash);
    if (object != NULL) HeapFree(GetProcessHeap(), 0, object);
    if (algorithm != NULL) BCryptCloseAlgorithmProvider(algorithm, 0u);
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    return 0;
}

static int inspect_runtime(const wchar_t *root)
{
    size_t index;
    int missing = 0;

    wprintf(L"nvtdm: BYOB runtime: %ls\n", root);
    for (index = 0u; index < ARRAYSIZE(runtime_files); ++index) {
        wchar_t path[MAX_PATH];
        WIN32_FILE_ATTRIBUTE_DATA data;
        wchar_t fingerprint[65];
        const runtime_file *file = &runtime_files[index];

        if (!make_runtime_path(root, file->name, path) ||
            !GetFileAttributesExW(path, GetFileExInfoStandard, &data) ||
            (data.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
            FILE_ATTRIBUTE_REPARSE_POINT)) != 0u ||
            (data.nFileSizeHigh == 0u && data.nFileSizeLow == 0u)) {
            wprintf(L"  %ls: %ls\n", file->name,
                file->required ? L"missing or invalid (required)" :
                L"absent or invalid (optional)");
            if (file->required) missing = 1;
            continue;
        }
        if (!fingerprint_file(path, fingerprint)) {
            wprintf(L"  %ls: unreadable (required=%d)\n", file->name,
                file->required);
            if (file->required) missing = 1;
            continue;
        }
        wprintf(L"  %ls: %llu bytes sha256=%ls%s\n", file->name,
            ((unsigned long long)data.nFileSizeHigh << 32) | data.nFileSizeLow,
            fingerprint, file->required ? L"" : L" (optional)");
    }
    return missing ? 0 : 1;
}

static int write_manifest_text(HANDLE file, const char *text)
{
    const size_t length = strlen(text);
    DWORD written = 0u;

    return length <= MAXDWORD && WriteFile(file, text, (DWORD)length, &written,
        NULL) && written == (DWORD)length;
}

static int format_manifest_line(const runtime_file *file,
    const WIN32_FILE_ATTRIBUTE_DATA *data, const wchar_t fingerprint[65],
    char line[160])
{
    return sprintf_s(line, 160u, "%ls|%llu|%ls\r\n", file->name,
        ((unsigned long long)data->nFileSizeHigh << 32) | data->nFileSizeLow,
        fingerprint) >= 0;
}

static int record_runtime_manifest(const wchar_t *root, const wchar_t *output_path)
{
    HANDLE output;
    size_t index;
    int result = 1;

    if (!inspect_runtime(root)) return 3;
    output = CreateFileW(output_path, GENERIC_WRITE, 0, NULL, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (output == INVALID_HANDLE_VALUE) {
        fwprintf(stderr, L"nvtdm: cannot create manifest %ls (error %lu)\n",
            output_path, GetLastError());
        return 1;
    }
    if (!write_manifest_text(output, "nvtdm-runtime-manifest-v1\r\n")) goto done;
    for (index = 0u; index < ARRAYSIZE(runtime_files); ++index) {
        wchar_t path[MAX_PATH];
        WIN32_FILE_ATTRIBUTE_DATA data;
        wchar_t fingerprint[65];
        char line[160];
        const runtime_file *file = &runtime_files[index];

        if (!make_runtime_path(root, file->name, path) ||
            !GetFileAttributesExW(path, GetFileExInfoStandard, &data) ||
            (data.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
            FILE_ATTRIBUTE_REPARSE_POINT)) != 0u ||
            (data.nFileSizeHigh == 0u && data.nFileSizeLow == 0u)) {
            if (file->required) goto done;
            continue;
        }
        if (!fingerprint_file(path, fingerprint)) goto done;
        if (!format_manifest_line(file, &data, fingerprint, line) ||
            !write_manifest_text(output, line)) goto done;
    }
    result = 0;

done:
    CloseHandle(output);
    if (result != 0) {
        DeleteFileW(output_path);
        fwprintf(stderr, L"nvtdm: unable to record manifest %ls\n", output_path);
    } else {
        wprintf(L"nvtdm: wrote local runtime manifest %ls\n", output_path);
    }
    return result;
}

static int manifest_contains_line(const char *text, const char *line)
{
    const char *cursor = text;
    const size_t line_length = strlen(line) - 2u;

    while ((cursor = strstr(cursor, line)) != NULL) {
        if ((cursor == text || cursor[-1] == '\n') &&
            cursor[line_length] == '\r' && cursor[line_length + 1u] == '\n') {
            return 1;
        }
        ++cursor;
    }
    return 0;
}

static int verify_runtime_manifest(const wchar_t *root, const wchar_t *path)
{
    uint8_t *data;
    size_t bytes;
    char *text;
    size_t index;
    int result = 3;

    if (!read_file(path, &data, &bytes)) {
        fwprintf(stderr, L"nvtdm: unable to read manifest %ls\n", path);
        return 1;
    }
    text = (char *)HeapAlloc(GetProcessHeap(), 0, bytes + 1u);
    if (text == NULL) {
        HeapFree(GetProcessHeap(), 0, data);
        return 1;
    }
    memcpy(text, data, bytes);
    text[bytes] = '\0';
    HeapFree(GetProcessHeap(), 0, data);
    if (strncmp(text, "nvtdm-runtime-manifest-v1\r\n", 27u) != 0) goto done;
    for (index = 0u; index < ARRAYSIZE(runtime_files); ++index) {
        wchar_t runtime_path[MAX_PATH];
        WIN32_FILE_ATTRIBUTE_DATA metadata;
        wchar_t fingerprint[65];
        char line[160];
        const runtime_file *file = &runtime_files[index];

        if (!make_runtime_path(root, file->name, runtime_path) ||
            !GetFileAttributesExW(runtime_path, GetFileExInfoStandard, &metadata) ||
            (metadata.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY |
            FILE_ATTRIBUTE_REPARSE_POINT)) != 0u ||
            (metadata.nFileSizeHigh == 0u && metadata.nFileSizeLow == 0u)) {
            if (file->required) goto done;
            continue;
        }
        if (!fingerprint_file(runtime_path, fingerprint) ||
            !format_manifest_line(file, &metadata, fingerprint, line) ||
            !manifest_contains_line(text, line)) goto done;
    }
    result = 0;

done:
    HeapFree(GetProcessHeap(), 0, text);
    if (result == 0) wprintf(L"nvtdm: runtime identity matches %ls\n", path);
    else fwprintf(stderr, L"nvtdm: runtime identity does not match %ls\n", path);
    return result;
}

static int read_file(const wchar_t *path, uint8_t **out_data, size_t *out_bytes)
{
    HANDLE file;
    LARGE_INTEGER size;
    DWORD bytes_read = 0u;
    uint8_t *data;

    *out_data = NULL;
    *out_bytes = 0u;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !GetFileSizeEx(file, &size) ||
        size.QuadPart <= 0 || (uint64_t)size.QuadPart > NVDTM_GUEST_BYTES) {
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        return 0;
    }
    data = (uint8_t *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)size.QuadPart);
    if (data == NULL || !ReadFile(file, data, (DWORD)size.QuadPart, &bytes_read,
        NULL) || bytes_read != (DWORD)size.QuadPart) {
        if (data != NULL) HeapFree(GetProcessHeap(), 0, data);
        CloseHandle(file);
        return 0;
    }
    CloseHandle(file);
    *out_data = data;
    *out_bytes = (size_t)size.QuadPart;
    return 1;
}

static int stage_com(const uint8_t *data, size_t bytes, staged_image *image)
{
    const size_t base = (size_t)NVDTM_LOAD_SEGMENT << 4;

    if (bytes > NVDTM_GUEST_BYTES - base - NVDTM_PSP_BYTES) return 0;
    image->memory[base] = 0xcdu; /* PSP: INT 20h termination entry. */
    image->memory[base + 1u] = 0x20u;
    image->memory[base + 0x80u] = 0u; /* Empty command tail. */
    memcpy(image->memory + base + NVDTM_PSP_BYTES, data, bytes);
    image->cs = NVDTM_LOAD_SEGMENT;
    image->ip = 0x100u;
    image->ss = NVDTM_LOAD_SEGMENT;
    image->sp = 0xfffeu;
    image->ds = NVDTM_LOAD_SEGMENT;
    image->es = NVDTM_LOAD_SEGMENT;
    image->bytes = bytes;
    return 1;
}

static int stage_mz(const uint8_t *data, size_t bytes, staged_image *image)
{
    uint16_t fields[14];
    size_t header_bytes;
    size_t image_bytes;
    size_t module_bytes;
    size_t module_base = (size_t)NVDTM_MODULE_SEGMENT << 4;
    uint16_t relocation_count;
    uint16_t relocation_offset;
    uint16_t index;

    if (bytes < 28u || data[0] != 'M' || data[1] != 'Z') return 0;
    memcpy(fields, data, sizeof(fields));
    header_bytes = (size_t)fields[4] << 4;
    if (fields[2] == 0u || (fields[1] != 0u && fields[1] > 512u)) return 0;
    image_bytes = ((size_t)fields[2] - 1u) * 512u +
        (fields[1] == 0u ? 512u : fields[1]);
    relocation_count = fields[3];
    relocation_offset = fields[12];
    if (header_bytes > image_bytes || image_bytes > bytes) return 0;
    module_bytes = image_bytes - header_bytes;
    if (module_bytes > NVDTM_GUEST_BYTES - module_base ||
        relocation_offset > header_bytes || (size_t)relocation_count * 4u >
        header_bytes - relocation_offset) return 0;
    memcpy(image->memory + module_base, data + header_bytes, module_bytes);
    for (index = 0u; index < relocation_count; ++index) {
        uint16_t relocation[2];
        size_t address;
        uint16_t value;

        memcpy(relocation, data + relocation_offset + (size_t)index * 4u,
            sizeof(relocation));
        address = module_base + ((size_t)relocation[1] << 4) + relocation[0];
        if (address + sizeof(value) > module_base + module_bytes) return 0;
        memcpy(&value, image->memory + address, sizeof(value));
        value = (uint16_t)(value + NVDTM_MODULE_SEGMENT);
        memcpy(image->memory + address, &value, sizeof(value));
    }
    image->memory[(size_t)NVDTM_LOAD_SEGMENT << 4] = 0xcdu;
    image->memory[((size_t)NVDTM_LOAD_SEGMENT << 4) + 1u] = 0x20u;
    image->memory[((size_t)NVDTM_LOAD_SEGMENT << 4) + 0x80u] = 0u;
    image->cs = (uint16_t)(NVDTM_MODULE_SEGMENT + fields[11]);
    image->ip = fields[10];
    image->ss = (uint16_t)(NVDTM_MODULE_SEGMENT + fields[7]);
    image->sp = fields[8];
    image->ds = NVDTM_LOAD_SEGMENT;
    image->es = NVDTM_LOAD_SEGMENT;
    image->bytes = module_bytes;
    return 1;
}

static int stage_target(const wchar_t *path)
{
    uint8_t *data;
    size_t bytes;
    staged_image image = {0};
    int result;

    if (!read_file(path, &data, &bytes)) {
        fwprintf(stderr, L"nvtdm: unable to read %ls\n", path);
        return 1;
    }
    image.memory = (uint8_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        NVDTM_GUEST_BYTES);
    if (image.memory == NULL) {
        HeapFree(GetProcessHeap(), 0, data);
        return 1;
    }
    result = bytes >= 2u && data[0] == 'M' && data[1] == 'Z' ?
        stage_mz(data, bytes, &image) : stage_com(data, bytes, &image);
    if (result) {
        wprintf(L"nvtdm: staged %ls: %zu bytes CS:IP=%04X:%04X SS:SP=%04X:%04X\n",
            path, image.bytes, image.cs, image.ip, image.ss, image.sp);
    } else {
        fwprintf(stderr, L"nvtdm: invalid or oversized DOS image: %ls\n", path);
    }
    HeapFree(GetProcessHeap(), 0, image.memory);
    HeapFree(GetProcessHeap(), 0, data);
    return result ? 0 : 1;
}

static int plan_bootstrap(const wchar_t *runtime, const wchar_t *manifest)
{
    wchar_t ntio_path[MAX_PATH];
    uint8_t *ntio_bytes = NULL;
    size_t ntio_size = 0u;
    nvtdm_bootstrap_plan plan;
    int result = 3;

    if (verify_runtime_manifest(runtime, manifest) != 0) return 3;
    if (!make_runtime_path(runtime, L"NTIO.SYS", ntio_path) ||
        !read_file(ntio_path, &ntio_bytes, &ntio_size)) {
        fwprintf(stderr, L"nvtdm: unable to read verified NTIO.SYS\n");
        return 1;
    }
    if (!nvtdm_plan_opennt_ntio_bootstrap(ntio_bytes, ntio_size, &plan)) {
        fwprintf(stderr, L"nvtdm: fixed bootstrap pattern is absent from NTIO.SYS\n");
        goto done;
    }
    wprintf(L"nvtdm: private OpenNT bootstrap plan: NTIO.SYS=%zu bytes rewrites=%zu\n",
        ntio_size, plan.rewrite_count);
    nvtdm_bootstrap_plan_destroy(&plan);
    result = 0;

done:
    HeapFree(GetProcessHeap(), 0, ntio_bytes);
    return result;
}

static int cpu_address(uint16_t segment, uint16_t offset, size_t *address)
{
    const size_t candidate = ((size_t)segment << 4u) + offset;

    if (candidate >= NVDTM_GUEST_BYTES) return 0;
    *address = candidate;
    return 1;
}

static int cpu_fetch8(const staged_image *image, probe_cpu *cpu, uint8_t *value)
{
    size_t address;

    if (!cpu_address(cpu->cs, cpu->ip, &address)) return 0;
    *value = image->memory[address];
    cpu->ip = (uint16_t)(cpu->ip + 1u);
    return 1;
}

static int cpu_fetch16(const staged_image *image, probe_cpu *cpu, uint16_t *value)
{
    uint8_t low;
    uint8_t high;

    if (!cpu_fetch8(image, cpu, &low) || !cpu_fetch8(image, cpu, &high)) return 0;
    *value = (uint16_t)(low | ((uint16_t)high << 8u));
    return 1;
}

static uint8_t cpu_high8(uint16_t value)
{
    return (uint8_t)(value >> 8u);
}

static uint8_t cpu_get8(const probe_cpu *cpu, uint8_t index)
{
    const uint16_t registers[] = { cpu->ax, cpu->cx, cpu->dx, cpu->bx };

    return index < 4u ? (uint8_t)registers[index] :
        (uint8_t)(registers[index - 4u] >> 8u);
}

static void cpu_set8(probe_cpu *cpu, uint8_t index, uint8_t value)
{
    uint16_t *registers[] = { &cpu->ax, &cpu->cx, &cpu->dx, &cpu->bx };
    uint16_t *target = registers[index & 3u];

    if (index < 4u) *target = (uint16_t)((*target & 0xff00u) | value);
    else *target = (uint16_t)((*target & 0x00ffu) | ((uint16_t)value << 8u));
}

static void cpu_set_carry(probe_cpu *cpu, int value)
{
    if (value) cpu->flags |= 0x0001u;
    else cpu->flags &= (uint16_t)~0x0001u;
}

static int guest_ascii_path(const staged_image *image, uint16_t segment,
    uint16_t offset, wchar_t path[MAX_PATH])
{
    size_t address;
    size_t index;

    if (!cpu_address(segment, offset, &address)) return 0;
    for (index = 0u; index + 1u < MAX_PATH && address + index < NVDTM_GUEST_BYTES;
        ++index) {
        const uint8_t value = image->memory[address + index];
        if (value == 0u) {
            path[index] = L'\0';
            return index != 0u;
        }
        if (value > 0x7fu) return 0;
        path[index] = (wchar_t)value;
    }
    return 0;
}

static int probe_interrupt21(staged_image *image, probe_cpu *cpu,
    int *exit_code, nvtdm_dos_files *files, const wchar_t *root)
{
    size_t address;
    size_t count;

    switch (cpu_high8(cpu->ax)) {
    case 0x3d:
    {
        wchar_t path[MAX_PATH];
        uint16_t handle;
        if (root == NULL || !guest_ascii_path(image, cpu->ds, cpu->dx, path) ||
            !nvtdm_dos_open_readonly(files, root, path, &handle)) {
            cpu->ax = 2u;
            cpu_set_carry(cpu, 1);
            return 1;
        }
        cpu->ax = handle;
        cpu_set_carry(cpu, 0);
        return 1;
    }
    case 0x3e:
        if (!nvtdm_dos_close(files, cpu->bx)) {
            cpu->ax = 6u;
            cpu_set_carry(cpu, 1);
        } else {
            cpu->ax = 0u;
            cpu_set_carry(cpu, 0);
        }
        return 1;
    case 0x3f:
    {
        size_t address;
        DWORD read = 0u;
        DWORD requested = cpu->cx;
        if (!cpu_address(cpu->ds, cpu->dx, &address) ||
            requested > NVDTM_GUEST_BYTES - address ||
            !nvtdm_dos_read(files, cpu->bx, image->memory + address, requested,
                &read)) {
            cpu->ax = 5u;
            cpu_set_carry(cpu, 1);
        } else {
            cpu->ax = (uint16_t)read;
            cpu_set_carry(cpu, 0);
        }
        return 1;
    }
    case 0x02:
        putwchar((wchar_t)(cpu->dx & 0xffu));
        cpu_set_carry(cpu, 0);
        return 1;
    case 0x09:
        if (!cpu_address(cpu->ds, cpu->dx, &address)) return 0;
        for (count = 0u; count < NVDTM_GUEST_BYTES - address; ++count) {
            const uint8_t value = image->memory[address + count];
            if (value == '$') {
                cpu_set_carry(cpu, 0);
                return 1;
            }
            putwchar((wchar_t)value);
        }
        return 0;
    case 0x30:
        cpu->ax = 0x0500u; /* Probe value only; it is not an NTDOS claim. */
        cpu_set_carry(cpu, 0);
        return 1;
    case 0x4c:
        *exit_code = (int)(cpu->ax & 0xffu);
        return 2;
    default:
        return 0;
    }
}

static int run_real_mode_probe(const wchar_t *path, const wchar_t *root,
    int allow_unsimulate_bop)
{
    uint8_t *data;
    size_t bytes;
    staged_image image = {0};
    probe_cpu cpu = {0};
    unsigned long steps;
    int result;
    int exit_code = 0;
    nvtdm_dos_files files;

    if (!read_file(path, &data, &bytes)) {
        fwprintf(stderr, L"nvtdm: unable to read %ls\n", path);
        return 1;
    }
    image.memory = (uint8_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        NVDTM_GUEST_BYTES);
    if (image.memory == NULL) {
        HeapFree(GetProcessHeap(), 0, data);
        return 1;
    }
    result = bytes >= 2u && data[0] == 'M' && data[1] == 'Z' ?
        stage_mz(data, bytes, &image) : stage_com(data, bytes, &image);
    HeapFree(GetProcessHeap(), 0, data);
    if (!result) {
        fwprintf(stderr, L"nvtdm: invalid or oversized DOS image: %ls\n", path);
        HeapFree(GetProcessHeap(), 0, image.memory);
        return 1;
    }
    cpu.cs = image.cs; cpu.ip = image.ip; cpu.ss = image.ss; cpu.sp = image.sp;
    cpu.ds = image.ds; cpu.es = image.es;
    cpu.flags = 0x0200u;
    nvtdm_dos_files_init(&files);
    for (steps = 0u; steps < 1000000u; ++steps) {
        uint8_t opcode;

        if (!cpu_fetch8(&image, &cpu, &opcode)) break;
        if (opcode >= 0xb8u && opcode <= 0xbfu) {
            uint16_t value;
            uint16_t *registers[] = { &cpu.ax, &cpu.cx, &cpu.dx, &cpu.bx,
                &cpu.sp, &cpu.bp, &cpu.si, &cpu.di };
            if (!cpu_fetch16(&image, &cpu, &value)) break;
            *registers[opcode - 0xb8u] = value;
        } else if (opcode >= 0xb0u && opcode <= 0xb7u) {
            uint8_t value;
            if (!cpu_fetch8(&image, &cpu, &value)) break;
            cpu_set8(&cpu, (uint8_t)(opcode - 0xb0u), value);
        } else if (opcode == 0x8au) {
            uint8_t modrm;
            if (!cpu_fetch8(&image, &cpu, &modrm) || (modrm & 0xc0u) != 0xc0u) break;
            cpu_set8(&cpu, (uint8_t)((modrm >> 3u) & 7u),
                cpu_get8(&cpu, (uint8_t)(modrm & 7u)));
        } else if (opcode >= 0x90u && opcode <= 0x97u) {
            uint16_t *registers[] = { &cpu.ax, &cpu.cx, &cpu.dx, &cpu.bx,
                &cpu.sp, &cpu.bp, &cpu.si, &cpu.di };
            const uint16_t value = cpu.ax;
            cpu.ax = *registers[opcode - 0x90u];
            *registers[opcode - 0x90u] = value;
        } else if (opcode == 0x72u || opcode == 0x73u) {
            uint8_t displacement;
            const int carry_set = (cpu.flags & 0x0001u) != 0u;
            if (!cpu_fetch8(&image, &cpu, &displacement)) break;
            if ((opcode == 0x72u && carry_set) ||
                (opcode == 0x73u && !carry_set)) {
                cpu.ip = (uint16_t)(cpu.ip + (int16_t)(int8_t)displacement);
            }
        } else if (opcode == 0x90u) {
            continue;
        } else if (opcode == 0xeb || opcode == 0xe9) {
            uint16_t displacement;
            if (opcode == 0xeb) {
                uint8_t short_displacement;
                if (!cpu_fetch8(&image, &cpu, &short_displacement)) break;
                displacement = (uint16_t)(int16_t)(int8_t)short_displacement;
            } else if (!cpu_fetch16(&image, &cpu, &displacement)) break;
            cpu.ip = (uint16_t)(cpu.ip + (int16_t)displacement);
        } else if (opcode == 0xc4u && allow_unsimulate_bop) {
            uint8_t marker, selector;
            if (!cpu_fetch8(&image, &cpu, &marker) || !cpu_fetch8(&image, &cpu, &selector)) break;
            if (marker == 0xc4u && selector == 0xfeu) {
                wprintf(L"nvtdm: probe observed BOP_UNSIMULATE at %04X:%04X\n",
                    cpu.cs, (uint16_t)(cpu.ip - 3u));
                result = 0;
                goto done;
            }
            fwprintf(stderr, L"nvtdm: probe rejected BOP %02X %02X\n", marker, selector);
            break;
        } else if (opcode == 0xcd) {
            uint8_t interrupt;
            if (!cpu_fetch8(&image, &cpu, &interrupt)) break;
            if (interrupt == 0x20u) { result = 0; goto done; }
            if (interrupt == 0x21u) {
                const int disposition = probe_interrupt21(&image, &cpu,
                    &exit_code, &files, root);
                if (disposition == 2) { result = exit_code; goto done; }
                if (disposition == 1) continue;
            }
            break;
        } else if (opcode == 0xf4u || opcode == 0xc3u) {
            result = 0;
            goto done;
        } else {
            fwprintf(stderr, L"nvtdm: probe unsupported opcode %02X at %04X:%04X\n",
                opcode, cpu.cs, (uint16_t)(cpu.ip - 1u));
            break;
        }
    }
    fwprintf(stderr, L"nvtdm: probe stopped before normal termination\n");
    result = 4;

done:
    nvtdm_dos_files_destroy(&files);
    HeapFree(GetProcessHeap(), 0, image.memory);
    return result;
}

static int probe_dos_path(const wchar_t *root, const wchar_t *dos_path)
{
    wchar_t resolved[MAX_PATH];

    if (!nvtdm_resolve_dos_path(root, dos_path, resolved)) {
        fwprintf(stderr, L"nvtdm: rejected DOS path %ls\n", dos_path);
        return 3;
    }
    wprintf(L"nvtdm: DOS path %ls -> %ls\n", dos_path, resolved);
    return 0;
}

static int probe_rooted_read(const wchar_t *root, const wchar_t *dos_path)
{
    wchar_t resolved[MAX_PATH];
    HANDLE file = nvtdm_open_rooted_readonly(root, dos_path, resolved);
    LARGE_INTEGER size;

    if (file == INVALID_HANDLE_VALUE || !GetFileSizeEx(file, &size)) {
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        fwprintf(stderr, L"nvtdm: rejected or unreadable DOS file %ls\n", dos_path);
        return 3;
    }
    CloseHandle(file);
    wprintf(L"nvtdm: readable DOS file %ls -> %ls (%lld bytes)\n", dos_path,
        resolved, size.QuadPart);
    return 0;
}

static int probe_file_service(const wchar_t *root, const wchar_t *dos_path)
{
    nvtdm_dos_files files;
    uint16_t dos_handle;
    uint8_t buffer[64];
    DWORD read = 0u;
    int result = 3;

    nvtdm_dos_files_init(&files);
    if (nvtdm_dos_open_readonly(&files, root, dos_path, &dos_handle) &&
        nvtdm_dos_read(&files, dos_handle, buffer, sizeof(buffer), &read) &&
        nvtdm_dos_close(&files, dos_handle)) {
        wprintf(L"nvtdm: DOS file handle %u read %lu bytes from %ls\n", dos_handle,
            read, dos_path);
        result = 0;
    } else {
        fwprintf(stderr, L"nvtdm: DOS file service rejected %ls\n", dos_path);
    }
    nvtdm_dos_files_destroy(&files);
    return result;
}

int wmain(void)
{
    int argc;
    wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    const wchar_t *runtime = NULL;
    int index = 1;
    int result = 2;

    if (argv != NULL && argc >= 4 && _wcsicmp(argv[1], L"--runtime") == 0) {
        runtime = argv[2];
        index = 3;
    }
    if (argv != NULL && argc == 4 && _wcsicmp(argv[1], L"--probe-real-mode-root") == 0) {
        result = run_real_mode_probe(argv[3], argv[2], 0);
    } else if (argv != NULL && argc == 4 && _wcsicmp(argv[1], L"--probe-file-service") == 0) {
        result = probe_file_service(argv[2], argv[3]);
    } else if (argv != NULL && argc == 4 && _wcsicmp(argv[1], L"--probe-rooted-read") == 0) {
        result = probe_rooted_read(argv[2], argv[3]);
    } else if (argv != NULL && argc == 4 && _wcsicmp(argv[1], L"--probe-dos-path") == 0) {
        result = probe_dos_path(argv[2], argv[3]);
    } else if (argv != NULL && argc == 3 && _wcsicmp(argv[1], L"--probe-real-mode") == 0) {
        result = run_real_mode_probe(argv[2], NULL, 0);
    } else if (argv != NULL && argc == 3 && _wcsicmp(argv[1], L"--probe-bop-unsimulate") == 0) {
        result = run_real_mode_probe(argv[2], NULL, 1);
    } else if (runtime == NULL || index >= argc) {
        fwprintf(stderr, L"usage: nvtdm --probe-dos-path root path | --probe-rooted-read root path | --probe-file-service root path | --probe-real-mode target | --probe-real-mode-root root target | --probe-bop-unsimulate target | --runtime DIR --inspect | --stage target | --record-manifest file | --verify-manifest file | --plan-bootstrap manifest\n");
    } else if (_wcsicmp(argv[index], L"--inspect") == 0 && index + 1 == argc) {
        result = inspect_runtime(runtime) ? 0 : 3;
    } else if (_wcsicmp(argv[index], L"--stage") == 0 && index + 2 == argc) {
        result = inspect_runtime(runtime) ? stage_target(argv[index + 1]) : 3;
    } else if (_wcsicmp(argv[index], L"--record-manifest") == 0 &&
        index + 2 == argc) {
        result = record_runtime_manifest(runtime, argv[index + 1]);
    } else if (_wcsicmp(argv[index], L"--verify-manifest") == 0 &&
        index + 2 == argc) {
        result = verify_runtime_manifest(runtime, argv[index + 1]);
    } else if (_wcsicmp(argv[index], L"--plan-bootstrap") == 0 &&
        index + 2 == argc) {
        result = plan_bootstrap(runtime, argv[index + 1]);
    } else {
        fwprintf(stderr, L"nvtdm: execution engine is not implemented; use --inspect or --stage\n");
        result = 4;
    }
    if (argv != NULL) LocalFree(argv);
    return result;
}
