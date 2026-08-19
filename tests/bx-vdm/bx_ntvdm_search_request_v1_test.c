#include <string.h>

#include "bop-v1/bx_ntvdm_search_request_v1.h"

int main(void)
{
    uint8_t path[128] = {0};
    uint8_t dta[43] = {0};
    uint8_t fcb[53] = {0};
    bx_ntvdm_search_query_v1 query;
    bx_ntvdm_search_token_v1 token;
    memcpy(path, "c:\\TMP\\*.TXT", 13u);
    if (!bx_ntvdm_search_request_v1_decode_first_path(path, 0x12u, &query) ||
        query.drive_index != 2u || query.attributes != 0x12u ||
        wcscmp(query.relative_directory, L"TMP") != 0 ||
        wcscmp(query.pattern, L"*.TXT") != 0) return 1;
    if (!bx_ntvdm_search_request_v1_decode_first_fcb(path, 0u, 0xffu, &query) ||
        query.attributes != 0u ||
        !bx_ntvdm_search_request_v1_decode_first_fcb(path, 1u, 0xffu, &query) ||
        query.attributes != 0xffu) return 2;
    memset(path, 'A', sizeof(path));
    if (bx_ntvdm_search_request_v1_decode_first_path(path, 0u, &query)) return 3;
    memset(path, 0, sizeof(path)); memcpy(path, "C:REL\\*.TXT", 11u);
    if (bx_ntvdm_search_request_v1_decode_first_path(path, 0u, &query)) return 4;
    dta[0] = 0x44u; dta[1] = 0x33u; dta[2] = 0x22u; dta[3] = 0x11u;
    dta[4] = 0x88u; dta[5] = 0x77u; dta[6] = 0x66u; dta[7] = 0x55u;
    if (!bx_ntvdm_search_request_v1_decode_next_path(dta, &token) ||
        token.cookie != 0x11223344u || token.id != 0x55667788u) return 5;
    memcpy(fcb + 32u, dta, 8u);
    if (!bx_ntvdm_search_request_v1_decode_next_fcb(fcb, &token) ||
        token.cookie != 0x11223344u || token.id != 0x55667788u) return 6;
    memset(dta, 0, sizeof(dta));
    return bx_ntvdm_search_request_v1_decode_next_path(dta, &token) ? 7 : 0;
}
