#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_terminal_observation_v1.h"
#include "bx_ntvdm_bop_sequence_observation_v1.h"
#include "bx_ntvdm_dem_open_observation_v1.h"
#include "bx_ntvdm_dem_namespace_identity_observation_v1.h"

#include <stdio.h>

static int descriptor_set(uint16_t *out_value, uint32_t maximum,
    const wchar_t *text, uint32_t *out_chars)
{
    uint32_t index = 0u;
    if (out_value == 0 || text == 0 || out_chars == 0) return 0;
    while (text[index] != L'\0') {
        if (index >= maximum || text[index] > UINT16_MAX) return 0;
        out_value[index] = (uint16_t)text[index];
        ++index;
    }
    *out_chars = index;
    return 1;
}

static int request_set(struct bx_ntvdm_engine_request_v1 *request,
    const wchar_t *profile, const wchar_t *root)
{
    bx_ntvdm_engine_request_v1_clear(request);
    if (!descriptor_set(request->profile_descriptor,
            BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS, profile,
            &request->profile_descriptor_chars) ||
        !descriptor_set(request->root_descriptor,
            BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS, root,
            &request->root_descriptor_chars) ||
        !descriptor_set(request->launch_descriptor,
            BX_NTVDM_ENGINE_V1_MAX_LAUNCH_CHARS, L"2,1,e,00",
            &request->launch_descriptor_chars)) return 0;
    request->instruction_tick_budget = UINT64_C(1);
    request->admitted_drive_mask = UINT32_C(1) << 2u;
    return bx_ntvdm_engine_request_v1_valid(request);
}

static int controlled_terminal_once(const struct bx_ntvdm_engine_request_v1 *request)
{
    struct bx_ntvdm_engine_result_v1 result;
    struct bx_ntvdm_terminal_observation_v1 stop;
    struct bx_ntvdm_bop_sequence_observation_v1 sequence;
    bx_ntvdm_dem_open_observation_v1 open;
    bx_ntvdm_dem_namespace_identity_observation_v1 identity;
    int call_result = bx_ntvdm_engine_run_v1(request, &result);
    int valid_result = bx_ntvdm_engine_result_v1_valid(&result);
    int captured = bx_ntvdm_terminal_observation_v1_copy(&stop);
    printf("t214-s3 engine call=%d valid=%d terminal=%u detail=%u\n",
        call_result, valid_result, result.terminal_kind, result.detail_code);
    if (captured) {
        printf("t215-s3 stop cs=%04x eip=%08x window=%02x%02x%02x%02x disposition=%u\n",
            stop.event.cs, stop.event.eip, stop.event.window[0], stop.event.window[1],
            stop.event.window[2], stop.event.window[3], stop.outcome.disposition);
    }
    if (!bx_ntvdm_bop_sequence_observation_v1_copy(&sequence)) return 0;
    if (!bx_ntvdm_dem_open_observation_v1_copy(&open)) return 0;
    if (!bx_ntvdm_dem_namespace_identity_observation_v1_copy(&identity)) return 0;
    printf("t215-s6 bops=%u overflow=%u\n", sequence.record_count, sequence.overflowed);
    for (uint32_t index = 0u; index < sequence.record_count; ++index) {
        const struct bx_ntvdm_bop_sequence_observation_record_v1 *entry =
            &sequence.records[index];
        printf("t215-s6 bop[%u] cs=%04x eip=%08x selector=%02x service=%02x has-service=%u disposition=%u\n",
            index, entry->cs, entry->eip, entry->selector, entry->service,
            entry->has_service, entry->disposition);
    }
    printf("t217-s4 dem-open seen=%u captured=%u state=%04x:%08x ds-si=%04x:%04x eax=%08x ebx=%08x accepted=%u disposition=%u resume=%llx gpr16-mask=%08x ax=%04x flags-mask=%08x flags-value=%08x\n",
        open.seen_open_count, open.captured, open.cs, open.eip, open.ds,
        (unsigned)(uint16_t)open.esi, open.eax, open.ebx, open.accepted,
        open.disposition, (unsigned long long)open.resume_rip,
        open.gpr16_write_mask, open.gpr16_values[0], open.eflags_write_mask,
        open.eflags_values);
    printf("t217-s6 dem-identity seen=%u captured=%u class=%u attached=%u generation=%08x drive=%u admitted=%u slot=%u ready=%u disposition=%u ax=%04x flags-mask=%08x flags-value=%08x\n",
        identity.seen_open_count, identity.captured, identity.identity_class,
        identity.namespace_attached, identity.namespace_generation, identity.drive_index,
        identity.admitted_drive, identity.declared_slot, identity.declared_bytes_ready,
        identity.disposition, identity.gpr16_values[0],
        identity.eflags_write_mask, identity.eflags_values);
    printf("t217-s10 wfp-shape bytes=%u drive-prefix=%u root-separator=%u components=%u\n",
        identity.wfp_bytes, identity.wfp_drive_prefix,
        identity.wfp_root_separator, identity.wfp_component_count);
    bx_ntvdm_bop_sequence_observation_v1_enable(0u);
    bx_ntvdm_dem_open_observation_v1_enable(0u);
    bx_ntvdm_dem_namespace_identity_observation_v1_enable(0u);
    bx_ntvdm_terminal_observation_v1_enable(0u);
    if (!call_result || !valid_result ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL ||
        result.detail_code != 1u || !captured || stop.event.window_bytes < 4u ||
        stop.outcome.disposition != BX_NTVDM_GENERIC_UD_STOP ||
        sequence.overflowed != 0u || sequence.record_count == 0u ||
        sequence.records[sequence.record_count - 1u].cs != stop.event.cs ||
        sequence.records[sequence.record_count - 1u].eip != stop.event.eip ||
        sequence.records[sequence.record_count - 1u].selector != 0x50u ||
        sequence.records[sequence.record_count - 1u].service != 0x3du ||
        sequence.records[sequence.record_count - 1u].has_service != 1u ||
        sequence.records[sequence.record_count - 1u].disposition !=
            BX_NTVDM_GENERIC_UD_STOP || identity.wfp_bytes == 0u ||
        identity.wfp_drive_prefix > BX_NTVDM_DEM_NAMESPACE_WFP_DRIVE_MALFORMED ||
        identity.wfp_root_separator > BX_NTVDM_DEM_NAMESPACE_WFP_ROOT_SLASH ||
        identity.wfp_component_count > 128u) return 0;
    return 1;
}

int wmain(int argc, wchar_t **argv)
{
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_bop_sequence_observation_v1 disabled;
    if (bx_ntvdm_bop_sequence_observation_v1_copy(&disabled)) return 1;
    bx_ntvdm_terminal_observation_v1_enable(1u);
    bx_ntvdm_bop_sequence_observation_v1_enable(1u);
    bx_ntvdm_dem_open_observation_v1_enable(1u);
    bx_ntvdm_dem_namespace_identity_observation_v1_enable(1u);
    if (argc != 3 || !request_set(&request, argv[1], argv[2]) ||
        !controlled_terminal_once(&request)) return 1;
    return 0;
}
