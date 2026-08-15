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
    WHV_REGISTER_NAME names[] = { WHvX64RegisterRip, WHvX64RegisterRflags,
        WHvX64RegisterCs, WHvX64RegisterDs, WHvX64RegisterEs,
        WHvX64RegisterSs, WHvX64RegisterCr0 };
    WHV_REGISTER_VALUE values[ARRAYSIZE(names)] = {0};
    WHV_RUN_VP_EXIT_CONTEXT exit_context = {0};
    HRESULT result = E_FAIL;
    int vp_created = 0;
    int mapped = 0;
    int exit_code = 3;

    memory = (BYTE *)VirtualAlloc(NULL, 4096u, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    if (memory == NULL) goto done;
    memory[0] = 0xf4u; /* HLT */
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
    values[1].Reg64 = 0x2u;
    values[2].Segment = real_segment(0x9bu);
    values[3].Segment = real_segment(0x93u);
    values[4].Segment = real_segment(0x93u);
    values[5].Segment = real_segment(0x93u);
    values[6].Reg64 = 0x10u;
    result = WHvSetVirtualProcessorRegisters(partition, 0u, names,
        ARRAYSIZE(names), values);
    if (FAILED(result)) goto done;
    result = WHvRunVirtualProcessor(partition, 0u, &exit_context,
        sizeof(exit_context));
    if (FAILED(result)) goto done;
    if (exit_context.ExitReason != WHvRunVpExitReasonX64Halt) {
        fwprintf(stderr, L"whpx-hlt: unexpected exit reason %u\n",
            (unsigned)exit_context.ExitReason);
        goto done;
    }
    wprintf(L"whpx-hlt: real-mode HLT exit verified\n");
    exit_code = 0;

done:
    if (exit_code != 0) {
        fwprintf(stderr, L"whpx-hlt: failed: 0x%08lX\n", (unsigned long)result);
    }
    if (vp_created) WHvDeleteVirtualProcessor(partition, 0u);
    if (mapped) WHvUnmapGpaRange(partition, 0u, 4096u);
    if (partition != NULL) WHvDeletePartition(partition);
    if (memory != NULL) VirtualFree(memory, 0u, MEM_RELEASE);
    return exit_code;
}
