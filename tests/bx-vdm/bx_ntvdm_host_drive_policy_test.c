#include "bx_ntvdm_host_drive_policy.h"

#include <stdio.h>
#include <string.h>

static uint32_t bit(unsigned index) { return UINT32_C(1) << index; }

int main(void)
{
    uint8_t types[26] = {0};
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    uint32_t include_mask, exclude_mask;
    int failed = 0;

    types[2] = 3u; types[3] = 3u; types[4] = 5u;
    failed |= !bx_ntvdm_host_drive_policy_v1_parse(L"C,D,E", &include_mask) ||
        include_mask != (bit(2u) | bit(3u) | bit(4u));
    failed |= !bx_ntvdm_host_drive_policy_v1_parse(L"e", &exclude_mask) ||
        exclude_mask != bit(4u);
    failed |= bx_ntvdm_host_drive_policy_v1_parse(L"C,,D", &include_mask) ||
        bx_ntvdm_host_drive_policy_v1_parse(L"C,C", &include_mask) ||
        bx_ntvdm_host_drive_policy_v1_parse(L"C,1", &include_mask);

    failed |= !bx_ntvdm_host_drive_snapshot_v1_apply(bit(2u) | bit(3u) | bit(4u),
        types, bit(2u) | bit(3u) | bit(4u), bit(4u), &snapshot) ||
        snapshot.admitted_mask != (bit(2u) | bit(3u)) || snapshot.types[4] != 0u;
    failed |= !bx_ntvdm_host_drive_snapshot_v1_apply(bit(2u) | bit(3u) | bit(4u),
        types, 0u, bit(3u), &snapshot) ||
        snapshot.admitted_mask != (bit(2u) | bit(4u));
    failed |= !bx_ntvdm_host_drive_snapshot_v1_apply(bit(2u) | bit(3u) | bit(4u),
        types, 0u, 0u, &snapshot) ||
        snapshot.admitted_mask != (bit(2u) | bit(3u) | bit(4u));
    types[3] = 0u;
    failed |= bx_ntvdm_host_drive_snapshot_v1_apply(bit(2u) | bit(3u), types,
        0u, 0u, &snapshot);

    if (failed) return 1;
    puts("bx-ntvdm-host-drive-policy-test: deterministic include/exclude snapshot policy verified");
    return 0;
}
