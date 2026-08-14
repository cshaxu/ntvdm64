#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_composition_runtime_v1.h"

int bx_ntvdm_engine_run_v1(const struct bx_ntvdm_engine_request_v1 *request,
    struct bx_ntvdm_engine_result_v1 *result)
{
    int install_status;
    if (result == 0) return 0;
    if (!bx_ntvdm_engine_request_v1_valid(request))
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST, 1u);
    install_status = bx_ntvdm_composition_runtime_v1_install_from_copied_input(
        request->profile_descriptor, request->profile_descriptor_chars,
        request->root_descriptor, request->root_descriptor_chars,
        request->launch_descriptor, request->launch_descriptor_chars,
        request->admitted_drive_mask, request->excluded_drive_mask);
    bx_ntvdm_composition_runtime_v1_reset();
    if (install_status != 1)
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION,
            install_status < 0 ? 1u : 2u);
    return bx_ntvdm_engine_result_v1_set(result,
        BX_NTVDM_ENGINE_TERMINAL_V1_NONE, 0u);
}
