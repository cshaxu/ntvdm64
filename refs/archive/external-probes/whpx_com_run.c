#include <windows.h>
#include <WinHvPlatform.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define GUEST_BYTES (1024u * 1024u)
#define PSP_SEGMENT 0x1000u
#define PSP_BASE ((size_t)PSP_SEGMENT << 4u)
#define MODULE_SEGMENT (PSP_SEGMENT + 0x10u)
#define INT21_HANDLER 0x0800u
#define DOS_TRAP_PORT 0x00e9u

static WHV_X64_SEGMENT_REGISTER real_segment(UINT16 selector, UINT16 attributes)
{
    WHV_X64_SEGMENT_REGISTER segment = {0};
    segment.Base = (UINT64)selector << 4u;
    segment.Limit = 0xffffu;
    segment.Selector = selector;
    segment.Attributes = attributes;
    return segment;
}

static UINT16 read_u16(const BYTE *memory, size_t offset)
{
    return (UINT16)(memory[offset] | ((UINT16)memory[offset + 1u] << 8u));
}

/* Consume the owned INT 21h trap by restoring its verified real-mode frame. */
static HRESULT return_from_service_trap(WHV_PARTITION_HANDLE partition,
    const BYTE *memory)
{
    const WHV_REGISTER_NAME source_names[] = { WHvX64RegisterRsp,
        WHvX64RegisterSs };
    WHV_REGISTER_VALUE source_values[ARRAYSIZE(source_names)] = {0};
    WHV_REGISTER_NAME target_names[] = { WHvX64RegisterRip,
        WHvX64RegisterRsp, WHvX64RegisterRflags, WHvX64RegisterCs };
    WHV_REGISTER_VALUE target_values[ARRAYSIZE(target_names)] = {0};
    size_t frame;
    HRESULT result = WHvGetVirtualProcessorRegisters(partition, 0u,
        source_names, ARRAYSIZE(source_names), source_values);

    if (FAILED(result)) return result;
    frame = (size_t)source_values[1].Segment.Base +
        (size_t)(source_values[0].Reg64 & 0xffffu);
    if (frame > GUEST_BYTES - 6u) return E_FAIL;
    target_values[0].Reg64 = read_u16(memory, frame);
    target_values[1].Reg64 = (source_values[0].Reg64 + 6u) & 0xffffu;
    target_values[2].Reg64 = read_u16(memory, frame + 4u);
    target_values[3].Segment = real_segment(read_u16(memory, frame + 2u), 0x9bu);
    return WHvSetVirtualProcessorRegisters(partition, 0u, target_names,
        ARRAYSIZE(target_names), target_values);
}

static HRESULT write_dollar_string(WHV_PARTITION_HANDLE partition,
    const BYTE *memory)
{
    const WHV_REGISTER_NAME names[] = { WHvX64RegisterDs, WHvX64RegisterRdx };
    WHV_REGISTER_VALUE values[ARRAYSIZE(names)] = {0};
    size_t offset;
    HRESULT result = WHvGetVirtualProcessorRegisters(partition, 0u, names,
        ARRAYSIZE(names), values);

    if (FAILED(result)) return result;
    offset = (size_t)values[0].Segment.Base +
        (size_t)(values[1].Reg64 & 0xffffu);
    while (offset < GUEST_BYTES && memory[offset] != '$') {
        putchar((int)memory[offset]);
        ++offset;
    }
    if (offset == GUEST_BYTES) return E_FAIL;
    fflush(stdout);
    return S_OK;
}

typedef struct guest_image {
    UINT16 cs, ip, ss, sp, ds, es;
} guest_image;

static int read_target(const wchar_t *path, BYTE **out_data, size_t *out_bytes)
{
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    LARGE_INTEGER size;
    DWORD read = 0u;

    BYTE *data = NULL;

    *out_data = NULL;
    *out_bytes = 0u;
    if (file == INVALID_HANDLE_VALUE || !GetFileSizeEx(file, &size) ||
        size.QuadPart <= 0 || (UINT64)size.QuadPart > GUEST_BYTES ||
        (data = (BYTE *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)size.QuadPart)) == NULL ||
        !ReadFile(file, data, (DWORD)size.QuadPart, &read, NULL) ||
        read != (DWORD)size.QuadPart) {
        if (data != NULL) HeapFree(GetProcessHeap(), 0, data);
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        return 0;
    }
    CloseHandle(file);
    *out_data = data;
    *out_bytes = (size_t)size.QuadPart;
    return 1;
}

static int load_target(const wchar_t *path, BYTE *memory, guest_image *image)
{
    BYTE *data = NULL;
    size_t bytes = 0u;
    size_t module_base = (size_t)MODULE_SEGMENT << 4u;
    int result = 0;

    if (!read_target(path, &data, &bytes)) return 0;
    memory[PSP_BASE] = 0xcdu;
    memory[PSP_BASE + 1u] = 0x20u;
    memory[PSP_BASE + 0x80u] = 0u;
    if (bytes >= 2u && data[0] == 'M' && data[1] == 'Z') {
        UINT16 fields[14];
        size_t header_bytes, image_bytes, module_bytes;
        UINT16 relocations, relocation_offset, index;

        if (bytes < sizeof(fields)) goto done;
        memcpy(fields, data, sizeof(fields));
        header_bytes = (size_t)fields[4] << 4u;
        image_bytes = ((size_t)fields[2] - 1u) * 512u +
            (fields[1] == 0u ? 512u : fields[1]);
        relocations = fields[3];
        relocation_offset = fields[12];
        if (fields[2] == 0u || (fields[1] != 0u && fields[1] > 512u) ||
            header_bytes > image_bytes || image_bytes > bytes ||
            relocation_offset > header_bytes ||
            (size_t)relocations * 4u > header_bytes - relocation_offset) goto done;
        module_bytes = image_bytes - header_bytes;
        if (module_bytes > GUEST_BYTES - module_base) goto done;
        memcpy(memory + module_base, data + header_bytes, module_bytes);
        for (index = 0u; index < relocations; ++index) {
            UINT16 item[2], value;
            size_t address;
            memcpy(item, data + relocation_offset + (size_t)index * 4u, sizeof(item));
            address = module_base + ((size_t)item[1] << 4u) + item[0];
            if (address + sizeof(value) > module_base + module_bytes) goto done;
            memcpy(&value, memory + address, sizeof(value));
            value = (UINT16)(value + MODULE_SEGMENT);
            memcpy(memory + address, &value, sizeof(value));
        }
        image->cs = (UINT16)(MODULE_SEGMENT + fields[11]); image->ip = fields[10];
        image->ss = (UINT16)(MODULE_SEGMENT + fields[7]); image->sp = fields[8];
    } else {
        if (bytes > GUEST_BYTES - PSP_BASE - 0x100u) goto done;
        memcpy(memory + PSP_BASE + 0x100u, data, bytes);
        image->cs = PSP_SEGMENT; image->ip = 0x100u;
        image->ss = PSP_SEGMENT; image->sp = 0xfffeu;
    }
    image->ds = PSP_SEGMENT; image->es = PSP_SEGMENT;
    result = 1;
done:
    HeapFree(GetProcessHeap(), 0, data);
    return result;
}

int wmain(int argc, wchar_t **argv)
{
    WHV_PARTITION_HANDLE partition = NULL;
    BYTE *memory = NULL;
    UINT32 processor_count = 1u;
    WHV_REGISTER_NAME names[] = { WHvX64RegisterRip, WHvX64RegisterRsp,
        WHvX64RegisterRflags, WHvX64RegisterCs, WHvX64RegisterDs,
        WHvX64RegisterEs, WHvX64RegisterSs, WHvX64RegisterCr0 };
    WHV_REGISTER_VALUE values[ARRAYSIZE(names)] = {0};
    WHV_RUN_VP_EXIT_CONTEXT exit_context = {0};
    HRESULT result = E_FAIL;
    int vp_created = 0;
    int mapped = 0;
    int exit_code = 4;
    const wchar_t *target = NULL;
    guest_image image = {0};

    if (argc == 2) {
        target = argv[1];
    } else if (argc == 3 && wcscmp(argv[1], L"--") == 0) {
        target = argv[2];
    } else {
        fwprintf(stderr, L"usage: owned-dos-engine [--] target.com\n");
        return 2;
    }
    memory = (BYTE *)VirtualAlloc(NULL, GUEST_BYTES, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    if (memory == NULL || !load_target(target, memory, &image)) {
        fwprintf(stderr, L"whpx-com-run: invalid or oversized DOS image\n");
        exit_code = 1;
        goto done;
    }
    memory[0x21u * 4u] = (BYTE)(INT21_HANDLER & 0xffu);
    memory[0x21u * 4u + 1u] = (BYTE)(INT21_HANDLER >> 8u);
    memory[0x21u * 4u + 2u] = 0u;
    memory[0x21u * 4u + 3u] = 0u;
    memory[INT21_HANDLER] = 0xe6u; /* OUT imm8, AL: host service trap */
    memory[INT21_HANDLER + 1u] = (BYTE)DOS_TRAP_PORT;

    result = WHvCreatePartition(&partition);
    if (FAILED(result)) goto done;
    result = WHvSetPartitionProperty(partition,
        WHvPartitionPropertyCodeProcessorCount, &processor_count,
        sizeof(processor_count));
    if (FAILED(result)) goto done;
    result = WHvSetupPartition(partition);
    if (FAILED(result)) goto done;
    result = WHvMapGpaRange(partition, memory, 0u, GUEST_BYTES,
        WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagWrite |
        WHvMapGpaRangeFlagExecute);
    if (FAILED(result)) goto done;
    mapped = 1;
    result = WHvCreateVirtualProcessor(partition, 0u, 0u);
    if (FAILED(result)) goto done;
    vp_created = 1;
    values[0].Reg64 = image.ip;
    values[1].Reg64 = image.sp;
    values[2].Reg64 = 0x2u;
    values[3].Segment = real_segment(image.cs, 0x9bu);
    values[4].Segment = real_segment(image.ds, 0x93u);
    values[5].Segment = real_segment(image.es, 0x93u);
    values[6].Segment = real_segment(image.ss, 0x93u);
    values[7].Reg64 = 0x10u;
    result = WHvSetVirtualProcessorRegisters(partition, 0u, names,
        ARRAYSIZE(names), values);
    if (FAILED(result)) goto done;
    for (;;) {
        unsigned function;

        ZeroMemory(&exit_context, sizeof(exit_context));
        result = WHvRunVirtualProcessor(partition, 0u, &exit_context,
            sizeof(exit_context));
        if (FAILED(result) || exit_context.ExitReason != WHvRunVpExitReasonX64IoPortAccess ||
            !exit_context.IoPortAccess.AccessInfo.IsWrite ||
            exit_context.IoPortAccess.PortNumber != DOS_TRAP_PORT) goto done;
        function = (unsigned)((exit_context.IoPortAccess.Rax >> 8u) & 0xffu);
        if (function == 0x4cu) {
            exit_code = (int)(exit_context.IoPortAccess.Rax & 0xffu);
            wprintf(L"whpx-com-run: guest terminated with %d\n", exit_code);
            break;
        }
        if (function == 0x09u) {
            result = write_dollar_string(partition, memory);
            if (FAILED(result)) goto done;
        } else if (function == 0x02u) {
            const WHV_REGISTER_NAME rdx_name = WHvX64RegisterRdx;
            WHV_REGISTER_VALUE rdx_value = {0};
            result = WHvGetVirtualProcessorRegisters(partition, 0u, &rdx_name,
                1u, &rdx_value);
            if (FAILED(result)) goto done;
            putchar((int)(rdx_value.Reg64 & 0xffu));
            fflush(stdout);
        } else {
            fwprintf(stderr, L"whpx-com-run: unsupported INT 21h AH=%02X\n", function);
            goto done;
        }
        result = return_from_service_trap(partition, memory);
        if (FAILED(result)) goto done;
    }

done:
    if (exit_code == 4) fwprintf(stderr, L"whpx-com-run: execution failed: 0x%08lX\n",
        (unsigned long)result);
    if (vp_created) WHvDeleteVirtualProcessor(partition, 0u);
    if (mapped) WHvUnmapGpaRange(partition, 0u, GUEST_BYTES);
    if (partition != NULL) WHvDeletePartition(partition);
    if (memory != NULL) VirtualFree(memory, 0u, MEM_RELEASE);
    return exit_code;
}
