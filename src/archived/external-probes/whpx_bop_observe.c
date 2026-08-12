#include <windows.h>
#include <WinHvPlatform.h>
#include <stdio.h>
#include <string.h>

#include "transition_registry.h"

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
    UINT64 exception_bitmap = 1ull << WHvX64ExceptionTypeInvalidOpcodeFault;
    WHV_REGISTER_NAME names[] = { WHvX64RegisterRip, WHvX64RegisterRax, WHvX64RegisterRdi,
        WHvX64RegisterRflags, WHvX64RegisterCs, WHvX64RegisterDs,
        WHvX64RegisterEs, WHvX64RegisterSs, WHvX64RegisterCr0 };
    WHV_REGISTER_VALUE values[ARRAYSIZE(names)] = {0};
    WHV_REGISTER_NAME observed_names[] = { WHvX64RegisterRip, WHvX64RegisterRax,
        WHvX64RegisterRdi };
    WHV_REGISTER_VALUE observed_values[ARRAYSIZE(observed_names)] = {0};
    WHV_RUN_VP_EXIT_CONTEXT exit_context = {0};
    const uint8_t bootstrap_bop[] = { 0xc4u, 0xc4u, 0x50u, 0x11u };
    const uint8_t owned_payload[] = { 'D', 'O', 'S', '!' };
    transition_registry registry;
    bop_rewrite_entry rewrite = {0};
    size_t rewrite_count = 0u;
    HRESULT result = E_FAIL;
    int vp_created = 0, mapped = 0, exit_code = 3;

    memory = (BYTE *)VirtualAlloc(NULL, 4096u, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    if (memory == NULL) goto done;
    transition_registry_init(&registry);
    if (!transition_registry_register(&registry, bootstrap_bop,
        sizeof(bootstrap_bop), 0xe9u) || !transition_registry_freeze(&registry)) goto done;
    memory[0] = bootstrap_bop[0]; memory[1] = bootstrap_bop[1];
    memory[2] = bootstrap_bop[2]; memory[3] = bootstrap_bop[3];
    memory[4] = 0xf4u; /* HLT reached only after the host consumes the pattern. */
    result = WHvCreatePartition(&partition);
    if (FAILED(result)) goto done;
    result = WHvSetPartitionProperty(partition,
        WHvPartitionPropertyCodeProcessorCount, &processor_count, sizeof(processor_count));
    if (FAILED(result)) goto done;
    result = WHvSetPartitionProperty(partition,
        WHvPartitionPropertyCodeExceptionExitBitmap, &exception_bitmap,
        sizeof(exception_bitmap));
    if (FAILED(result)) goto done;
    result = WHvSetupPartition(partition);
    if (FAILED(result)) goto done;
    /* Deliberately omit execute: WHPX must exit before the BOP bytes execute. */
    result = WHvMapGpaRange(partition, memory, 0u, 4096u,
        WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagWrite);
    if (FAILED(result)) goto done;
    mapped = 1;
    result = WHvCreateVirtualProcessor(partition, 0u, 0u);
    if (FAILED(result)) goto done;
    vp_created = 1;
    values[0].Reg64 = 0u; values[1].Reg64 = 0x1234u; values[2].Reg64 = 0x20u;
    values[3].Reg64 = 0x2u;
    values[4].Segment = real_segment(0x9bu); values[5].Segment = real_segment(0x93u);
    values[6].Segment = real_segment(0x93u); values[7].Segment = real_segment(0x93u);
    values[8].Reg64 = 0x10u;
    result = WHvSetVirtualProcessorRegisters(partition, 0u, names, ARRAYSIZE(names), values);
    if (FAILED(result)) goto done;
    result = WHvRunVirtualProcessor(partition, 0u, &exit_context, sizeof(exit_context));
    /* This is a page-granular fetch observation, not a #UD interception. */
    if (FAILED(result) || exit_context.ExitReason != WHvRunVpExitReasonMemoryAccess ||
        exit_context.VpContext.Rip != 0u || memory[0] != 0xc4u ||
        memory[1] != 0xc4u || memory[2] != 0x50u || memory[3] != 0x11u) goto done;
    result = WHvGetVirtualProcessorRegisters(partition, 0u, observed_names,
        ARRAYSIZE(observed_names), observed_values);
    if (FAILED(result) || observed_values[0].Reg64 != 0u ||
        observed_values[1].Reg64 != 0x1234u || observed_values[2].Reg64 != 0x20u) goto done;
    /* Rewrite only private guest RAM through the generic registered-pattern table. */
    if (!transition_registry_apply(&registry, memory, 4096u, &rewrite,
        1u, &rewrite_count) || rewrite_count != 1u || rewrite.offset != 0u ||
        rewrite.length != sizeof(bootstrap_bop)) goto done;
    result = WHvUnmapGpaRange(partition, 0u, 4096u);
    if (FAILED(result)) goto done;
    mapped = 0;
    result = WHvMapGpaRange(partition, memory, 0u, 4096u,
        WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagWrite | WHvMapGpaRangeFlagExecute);
    if (FAILED(result)) goto done;
    mapped = 1;
    observed_values[0].Reg64 = 0u;
    result = WHvSetVirtualProcessorRegisters(partition, 0u, observed_names, 1u,
        observed_values);
    if (FAILED(result)) goto done;
    ZeroMemory(&exit_context, sizeof(exit_context));
    result = WHvRunVirtualProcessor(partition, 0u, &exit_context, sizeof(exit_context));
    if (FAILED(result) || exit_context.ExitReason != WHvRunVpExitReasonX64IoPortAccess ||
        !exit_context.IoPortAccess.AccessInfo.IsWrite ||
        exit_context.IoPortAccess.PortNumber != 0xe9u ||
        (exit_context.IoPortAccess.Rax & 0xffu) != 0x34u) goto done;
    result = WHvGetVirtualProcessorRegisters(partition, 0u, observed_names,
        ARRAYSIZE(observed_names), observed_values);
    if (FAILED(result) || observed_values[2].Reg64 != 0x20u ||
        ((observed_values[2].Reg64 & 0xffffu) << 4u) > 4096u - sizeof(owned_payload)) goto done;
    memcpy(memory + ((observed_values[2].Reg64 & 0xffffu) << 4u), owned_payload,
        sizeof(owned_payload));
    /* The registered pattern consumes its full original four-byte extent. */
    observed_values[0].Reg64 = 4u;
    result = WHvSetVirtualProcessorRegisters(partition, 0u, observed_names, 1u,
        observed_values);
    if (FAILED(result)) goto done;
    ZeroMemory(&exit_context, sizeof(exit_context));
    result = WHvRunVirtualProcessor(partition, 0u, &exit_context, sizeof(exit_context));
    if (FAILED(result) || exit_context.ExitReason != WHvRunVpExitReasonX64Halt ||
        exit_context.VpContext.Rip != 5u) goto done;
    result = WHvGetVirtualProcessorRegisters(partition, 0u, observed_names,
        ARRAYSIZE(observed_names), observed_values);
    if (FAILED(result) || observed_values[0].Reg64 != 5u ||
        observed_values[1].Reg64 != 0x1234u || observed_values[2].Reg64 != 0x20u ||
        memcmp(memory + 0x200u, owned_payload, sizeof(owned_payload)) != 0) goto done;
    wprintf(L"whpx-bop-observe: private rewrite, load dispatch, and handled-resume verified\n");
    exit_code = 0;
done:
    if (exit_code != 0) fwprintf(stderr, L"whpx-bop-observe: failed: 0x%08lX reason=%u rip=%llX\n",
        (unsigned long)result, (unsigned)exit_context.ExitReason,
        (unsigned long long)exit_context.VpContext.Rip);
    if (vp_created) WHvDeleteVirtualProcessor(partition, 0u);
    if (mapped) WHvUnmapGpaRange(partition, 0u, 4096u);
    if (partition != NULL) WHvDeletePartition(partition);
    if (memory != NULL) VirtualFree(memory, 0u, MEM_RELEASE);
    return exit_code;
}
