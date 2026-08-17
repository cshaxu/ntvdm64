#include <stdint.h>
#include <wchar.h>

#include "bx_ntvdm_composition_runtime_v1.h"
#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_mutation_profile_v1.h"

static int copy_wide_input(uint16_t *output, uint32_t capacity,
                           const wchar_t *input, uint32_t *count_out)
{
    uint32_t index = 0u;

    while (input[index] != L'\0') {
        if (index >= capacity || input[index] > UINT16_MAX) return 0;
        output[index] = (uint16_t)input[index];
        ++index;
    }
    *count_out = index;
    return index != 0u;
}

static int install_one_profile(const wchar_t *profile, const wchar_t *root,
                               uint32_t mutation_mode)
{
    uint16_t profile_input[261];
    uint16_t root_input[261];
    uint16_t launch_input[257];
    uint32_t profile_chars;
    uint32_t root_chars;
    uint32_t launch_chars;
    bx_ntvdm_startup_plan_v1 plan;
    const uint8_t *ntio = 0;
    uint64_t ntio_bytes = 0u;

    if (!copy_wide_input(profile_input, 260u, profile, &profile_chars) ||
        !copy_wide_input(root_input, 260u, root, &root_chars) ||
        !copy_wide_input(launch_input, 256u, L"2,1,e,00", &launch_chars)) return 0;
    if (bx_ntvdm_composition_runtime_v1_install_from_copied_input_with_mode(
            profile_input, profile_chars, root_input, root_chars,
            launch_input, launch_chars, 4u, 0u, mutation_mode) != 1) return 0;
    if (!bx_ntvdm_composition_runtime_v1_prepare_startup_plan(
            &plan, &ntio, &ntio_bytes) || ntio == 0 || ntio_bytes != 33792u) {
        bx_ntvdm_composition_runtime_v1_reset();
        return 0;
    }
    bx_ntvdm_composition_runtime_v1_reset();
    return 1;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc != 3) return 1;
    if (!install_one_profile(argv[1], argv[2],
                             BX_NTVDM_MUTATION_MODE_V1_DIRECT) ||
        !install_one_profile(argv[1], argv[2],
                             BX_NTVDM_MUTATION_MODE_V1_READONLY)) return 2;

    /* Overlay is deferred and Virtual is retired: neither may be admitted. */
    if (install_one_profile(argv[1], argv[2],
                            BX_NTVDM_MUTATION_MODE_V1_OVERLAY) ||
        install_one_profile(argv[1], argv[2],
                            BX_NTVDM_MUTATION_MODE_V1_VIRTUAL)) return 3;
    return 0;
}
