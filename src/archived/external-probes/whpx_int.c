#include <windows.h>
#include <WinHvPlatform.h>
#include <stdio.h>

static WHV_X64_SEGMENT_REGISTER real_segment(UINT16 attributes)
{
    WHV_X64_SEGMENT_REGISTER segment = {0};
    segment.Limit = 0xffffu;
    segment.Attributes = attributes;
    return segment;
}

int wmain(void)
{
    WHV_PARTITION_HANDLE partition = NULL;
    BYTE *memory = NULL;
    UINT32 processor_count = 1u;
    WHV_REGISTER_NAME names[] = { WHvX64RegisterRip, WHvX64RegisterRsp,
        WHvX64RegisterRflags, WHvX64RegisterCs, WHvX64RegisterDs,
        WHvX64RegisterEs, WHvX64RegisterSs, WHvX64RegisterCr0 };
    WHV_REGISTER_VALUE values[ARRAYSIZE(names)] = {0};
    WHV_REGISTER_NAME return_names[] = { WHvX64RegisterRsp,
        WHvX64RegisterRflags };
    WHV_REGISTER_VALUE return_values[ARRAYSIZE(return_names)] = {0};
    WHV_RUN_VP_EXIT_CONTEXT exit_context = {0};
    HRESULT result = E_FAIL;
    int vp_created = 0;
    int mapped = 0;
    int exit_code = 3;

    memory = (BYTE *)VirtualAlloc(NULL, 4096u, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    if (memory == NULL) goto done;
    memory[0] = 0xcdu; /* INT 60h */
    memory[1] = 0x60u;
    memory[2] = 0xf4u; /* HLT after IRET returns to the caller. */
    memory[0x180u] = 0x00u; /* IVT[60h] = 0000:0200 */
    memory[0x181u] = 0x02u;
    memory[0x182u] = 0x00u;
    memory[0x183u] = 0x00u;
    memory[0x200u] = 0xcfu; /* IRET */
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
    values[0].Reg64 = 0u;
    values[1].Reg64 = 0x800u;
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
    if (FAILED(result) || exit_context.ExitReason != WHvRunVpExitReasonX64Halt ||
        exit_context.VpContext.Rip != 0x3u) goto done;
    result = WHvGetVirtualProcessorRegisters(partition, 0u, return_names,
        ARRAYSIZE(return_names), return_values);
    if (FAILED(result) || return_values[0].Reg64 != 0x800u ||
        return_values[1].Reg64 != 0x2u ||
        memory[0x7fau] != 0x02u || memory[0x7fbu] != 0x00u ||
        memory[0x7fcu] != 0x00u || memory[0x7fdu] != 0x00u) goto done;
    wprintf(L"whpx-int: real-mode IVT, IRET, and stack restoration verified\n");
    exit_code = 0;

done:
    if (exit_code != 0) fwprintf(stderr, L"whpx-int: failed: 0x%08lX reason=%u rip=%llX\n",
        (unsigned long)result, (unsigned)exit_context.ExitReason,
        (unsigned long long)exit_context.VpContext.Rip);
    if (vp_created) WHvDeleteVirtualProcessor(partition, 0u);
    if (mapped) WHvUnmapGpaRange(partition, 0u, 4096u);
    if (partition != NULL) WHvDeletePartition(partition);
    if (memory != NULL) VirtualFree(memory, 0u, MEM_RELEASE);
    return exit_code;
}
