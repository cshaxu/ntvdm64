#include <windows.h>
#include <WinHvPlatform.h>
#include <WinHvEmulation.h>
#include <stdio.h>

typedef struct emulator_context {
    WHV_PARTITION_HANDLE partition;
    int io_seen;
} emulator_context;

static WHV_X64_SEGMENT_REGISTER real_segment(UINT16 attributes)
{
    WHV_X64_SEGMENT_REGISTER segment = {0};
    segment.Limit = 0xffffu;
    segment.Attributes = attributes;
    return segment;
}

static HRESULT CALLBACK io_callback(VOID *context,
    WHV_EMULATOR_IO_ACCESS_INFO *access)
{
    emulator_context *state = (emulator_context *)context;

    if (access->Direction != 1u || access->Port != 0xe9u ||
        access->AccessSize != 1u || (access->Data & 0xffu) != 0x42u) {
        return E_INVALIDARG;
    }
    state->io_seen = 1;
    return S_OK;
}

static HRESULT CALLBACK get_registers_callback(VOID *context,
    const WHV_REGISTER_NAME *names, UINT32 count, WHV_REGISTER_VALUE *values)
{
    emulator_context *state = (emulator_context *)context;
    return WHvGetVirtualProcessorRegisters(state->partition, 0u, names, count,
        values);
}

static HRESULT CALLBACK set_registers_callback(VOID *context,
    const WHV_REGISTER_NAME *names, UINT32 count,
    const WHV_REGISTER_VALUE *values)
{
    emulator_context *state = (emulator_context *)context;
    return WHvSetVirtualProcessorRegisters(state->partition, 0u, names, count,
        values);
}

static HRESULT CALLBACK unsupported_memory_callback(VOID *context,
    WHV_EMULATOR_MEMORY_ACCESS_INFO *access)
{
    (void)context;
    (void)access;
    return E_NOTIMPL;
}

static HRESULT CALLBACK unsupported_translate_callback(VOID *context,
    WHV_GUEST_VIRTUAL_ADDRESS address, WHV_TRANSLATE_GVA_FLAGS flags,
    WHV_TRANSLATE_GVA_RESULT_CODE *result,
    WHV_GUEST_PHYSICAL_ADDRESS *gpa)
{
    (void)context;
    (void)address;
    (void)flags;
    (void)result;
    (void)gpa;
    return E_NOTIMPL;
}

int wmain(void)
{
    WHV_PARTITION_HANDLE partition = NULL;
    BYTE *memory = NULL;
    UINT32 processor_count = 1u;
    WHV_REGISTER_NAME names[] = { WHvX64RegisterRax, WHvX64RegisterRip,
        WHvX64RegisterRflags, WHvX64RegisterCs, WHvX64RegisterDs,
        WHvX64RegisterEs, WHvX64RegisterSs, WHvX64RegisterCr0 };
    WHV_REGISTER_VALUE values[ARRAYSIZE(names)] = {0};
    WHV_RUN_VP_EXIT_CONTEXT exit_context = {0};
    WHV_EMULATOR_CALLBACKS callbacks = {0};
    WHV_EMULATOR_HANDLE emulator = NULL;
    WHV_EMULATOR_STATUS emulator_status = {0};
    emulator_context emulator_context = {0};
    HRESULT result = E_FAIL;
    int vp_created = 0;
    int mapped = 0;
    int exit_code = 3;

    memory = (BYTE *)VirtualAlloc(NULL, 4096u, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    if (memory == NULL) goto done;
    memory[0] = 0xe6u; /* OUT imm8, AL */
    memory[1] = 0xe9u;
    memory[2] = 0xf4u; /* HLT after the explicitly handled I/O fixture. */
    result = WHvCreatePartition(&partition);
    if (FAILED(result)) goto done;
    result = WHvSetPartitionProperty(partition,
        WHvPartitionPropertyCodeProcessorCount, &processor_count,
        sizeof(processor_count));
    if (FAILED(result)) goto done;
    result = WHvSetupPartition(partition);
    if (FAILED(result)) goto done;
    result = WHvMapGpaRange(partition, memory, 0u, 4096u,
        WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagWrite |
        WHvMapGpaRangeFlagExecute);
    if (FAILED(result)) goto done;
    mapped = 1;
    result = WHvCreateVirtualProcessor(partition, 0u, 0u);
    if (FAILED(result)) goto done;
    vp_created = 1;
    emulator_context.partition = partition;
    callbacks.Size = sizeof(callbacks);
    callbacks.WHvEmulatorIoPortCallback = io_callback;
    callbacks.WHvEmulatorMemoryCallback = unsupported_memory_callback;
    callbacks.WHvEmulatorGetVirtualProcessorRegisters = get_registers_callback;
    callbacks.WHvEmulatorSetVirtualProcessorRegisters = set_registers_callback;
    callbacks.WHvEmulatorTranslateGvaPage = unsupported_translate_callback;
    result = WHvEmulatorCreateEmulator(&callbacks, &emulator);
    if (FAILED(result)) goto done;
    values[0].Reg64 = 0x42u;
    values[1].Reg64 = 0u;
    values[2].Reg64 = 0x2u;
    values[3].Segment = real_segment(0x9bu);
    values[4].Segment = real_segment(0x93u);
    values[5].Segment = real_segment(0x93u);
    values[6].Segment = real_segment(0x93u);
    values[7].Reg64 = 0x10u;
    result = WHvSetVirtualProcessorRegisters(partition, 0u, names,
        ARRAYSIZE(names), values);
    if (FAILED(result)) goto done;
    result = WHvRunVirtualProcessor(partition, 0u, &exit_context,
        sizeof(exit_context));
    if (FAILED(result)) goto done;
    if (exit_context.ExitReason != WHvRunVpExitReasonX64IoPortAccess ||
        !exit_context.IoPortAccess.AccessInfo.IsWrite ||
        exit_context.IoPortAccess.PortNumber != 0xe9u ||
        (exit_context.IoPortAccess.Rax & 0xffu) != 0x42u) {
        fwprintf(stderr, L"whpx-io: unexpected I/O exit\n");
        goto done;
    }
    result = WHvEmulatorTryIoEmulation(emulator, &emulator_context,
        &exit_context.VpContext, &exit_context.IoPortAccess, &emulator_status);
    if (FAILED(result) || !emulator_status.EmulationSuccessful ||
        !emulator_context.io_seen) goto done;
    ZeroMemory(&exit_context, sizeof(exit_context));
    result = WHvRunVirtualProcessor(partition, 0u, &exit_context,
        sizeof(exit_context));
    if (FAILED(result) || exit_context.ExitReason != WHvRunVpExitReasonX64Halt) {
        fwprintf(stderr, L"whpx-io: did not resume to HLT (reason=%u rip=%llX length=%u)\n",
            (unsigned)exit_context.ExitReason,
            (unsigned long long)exit_context.VpContext.Rip,
            (unsigned)exit_context.VpContext.InstructionLength);
        goto done;
    }
    wprintf(L"whpx-io: OUT 00E9 value 42 and resume-to-HLT verified\n");
    exit_code = 0;

done:
    if (exit_code != 0) fwprintf(stderr, L"whpx-io: failed: 0x%08lX\n",
        (unsigned long)result);
    if (emulator != NULL) WHvEmulatorDestroyEmulator(emulator);
    if (vp_created) WHvDeleteVirtualProcessor(partition, 0u);
    if (mapped) WHvUnmapGpaRange(partition, 0u, 4096u);
    if (partition != NULL) WHvDeletePartition(partition);
    if (memory != NULL) VirtualFree(memory, 0u, MEM_RELEASE);
    return exit_code;
}
