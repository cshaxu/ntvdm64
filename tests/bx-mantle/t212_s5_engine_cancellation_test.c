#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_composition_runtime_v1.h"
#include "bx_ntvdm_cancellation_controller_v1.h"

static int install_calls;
static int reset_calls;
static int cancel_during_install;

int bx_ntvdm_composition_runtime_v1_install_from_copied_input(
    const uint16_t *profile, uint32_t profile_chars, const uint16_t *root,
    uint32_t root_chars, const uint16_t *launch, uint32_t launch_chars,
    uint32_t include_mask, uint32_t exclude_mask)
{
    if (profile == 0 || root == 0 || launch == 0 || profile_chars == 0u ||
        root_chars == 0u || launch_chars == 0u || include_mask != 0u ||
        exclude_mask != 0u) return -1;
    ++install_calls;
    if (cancel_during_install && !bx_ntvdm_engine_request_cancellation_v1(
        BX_NTVDM_CANCELLATION_V1_USER_REQUEST)) return -1;
    return 1;
}

void bx_ntvdm_composition_runtime_v1_reset(void) { ++reset_calls; }
int bx_ntvdm_composition_runtime_v1_prepare_machine_stage_request(
    struct bx_ntvdm_machine_stage_v1_request *request)
{ (void)request; return 1; }
int bx_ntvdm_composition_runtime_v1_prepare_machine_stage_entry(
    struct bx_ntvdm_machine_stage_v1_entry *entry)
{ (void)entry; return 1; }
uint32_t bx_ntvdm_machine_stage_v1_begin(
    const struct bx_ntvdm_machine_stage_v1_request *request)
{ (void)request; return BX_NTVDM_MACHINE_STAGE_V1_OK; }
uint32_t bx_ntvdm_machine_stage_v1_arm_real_mode_entry(
    const struct bx_ntvdm_machine_stage_v1_entry *entry)
{ (void)entry; return BX_NTVDM_MACHINE_STAGE_V1_OK; }
void bx_ntvdm_machine_stage_v1_execution_request_clear(
    struct bx_ntvdm_machine_stage_v1_execution_request *request)
{
    if (request != 0) {
        request->magic = BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_MAGIC;
        request->abi_version = BX_NTVDM_MACHINE_STAGE_V1_VERSION;
        request->struct_bytes = (uint32_t)sizeof(*request);
        request->ips = 0u;
        request->instruction_tick_budget = 0u;
    }
}
uint32_t bx_ntvdm_machine_stage_v1_execute(
    const struct bx_ntvdm_machine_stage_v1_execution_request *request)
{ (void)request; return BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET; }
uint32_t bx_ntvdm_machine_stage_v1_reset(void)
{ return BX_NTVDM_MACHINE_STAGE_V1_OK; }

static void descriptor_set(uint16_t *out_value, const char *text,
    uint32_t *out_chars)
{
    uint32_t index = 0u;
    while (text[index] != '\0') {
        out_value[index] = (uint16_t)(unsigned char)text[index];
        ++index;
    }
    *out_chars = index;
}

static void request_set(struct bx_ntvdm_engine_request_v1 *request)
{
    bx_ntvdm_engine_request_v1_clear(request);
    descriptor_set(request->profile_descriptor, "profile.json",
        &request->profile_descriptor_chars);
    descriptor_set(request->root_descriptor, "bundle", &request->root_descriptor_chars);
    descriptor_set(request->launch_descriptor, "2,1,e,00",
        &request->launch_descriptor_chars);
    request->instruction_tick_budget = UINT64_C(10);
}

int main(void)
{
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;
    request_set(&request);
    cancel_during_install = 1;
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION ||
        result.detail_code != BX_NTVDM_CANCELLATION_V1_USER_REQUEST ||
        install_calls != 1 || reset_calls != 1 ||
        bx_ntvdm_engine_request_cancellation_v1(
            BX_NTVDM_CANCELLATION_V1_USER_REQUEST)) return 1;
    cancel_during_install = 0;
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET ||
        install_calls != 2 || reset_calls != 2) return 2;
    return 0;
}
