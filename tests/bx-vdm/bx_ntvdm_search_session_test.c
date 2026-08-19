#include <string.h>

#include "bop-v1/bx_ntvdm_search_session.h"

int main(void)
{
    bx_ntvdm_search_sessions_v1 sessions;
    bx_ntvdm_host_namespace_entry_v1 entries[3] = {0};
    bx_ntvdm_host_namespace_entry_v1 out;
    bx_ntvdm_search_token_v1 token, next_token, bad;
    wcscpy(entries[0].dos_name, L"ALPHA.TXT");
    wcscpy(entries[1].dos_name, L"BETA.TXT");
    wcscpy(entries[2].dos_name, L"GAMMA.TXT");
    bx_ntvdm_search_sessions_v1_initialize(&sessions);
    if (!bx_ntvdm_search_sessions_v1_valid(&sessions) ||
        !bx_ntvdm_search_sessions_v1_begin(&sessions, 0x1234u, entries, 3u,
            &out, &token) || wcscmp(out.dos_name, L"ALPHA.TXT") != 0 ||
        token.cookie == 0u || token.id == 0u) return 1;
    bad = token; bad.id ^= UINT32_C(1);
    if (bx_ntvdm_search_sessions_v1_next(&sessions, 0x1234u, &bad, &out,
        &next_token) || bx_ntvdm_search_sessions_v1_next(&sessions, 0x2222u,
        &token, &out, &next_token)) return 2;
    if (!bx_ntvdm_search_sessions_v1_next(&sessions, 0x1234u, &token, &out,
        &next_token) || wcscmp(out.dos_name, L"BETA.TXT") != 0 ||
        memcmp(&token, &next_token, sizeof(token)) != 0) return 3;
    if (!bx_ntvdm_search_sessions_v1_next(&sessions, 0x1234u, &token, &out,
        &next_token) || wcscmp(out.dos_name, L"GAMMA.TXT") != 0 ||
        next_token.cookie != 0u || next_token.id != 0u ||
        bx_ntvdm_search_sessions_v1_next(&sessions, 0x1234u, &token, &out,
            &next_token)) return 4;
    if (!bx_ntvdm_search_sessions_v1_begin(&sessions, 0x1234u, entries, 1u,
        &out, &token) || token.cookie != 0u || token.id != 0u) return 5;
    if (!bx_ntvdm_search_sessions_v1_begin(&sessions, 0x1234u, entries, 2u,
        &out, &token)) return 6;
    if (!bx_ntvdm_search_sessions_v1_begin(&sessions, 0x2222u, entries, 2u,
        &out, &bad) || !bx_ntvdm_search_sessions_v1_release_pdb(&sessions,
            0x1234u, 0) || bx_ntvdm_search_sessions_v1_next(&sessions,
            0x1234u, &token, &out, &next_token) || !bx_ntvdm_search_sessions_v1_next(
            &sessions, 0x2222u, &bad, &out, &next_token)) return 7;
    if (!bx_ntvdm_search_sessions_v1_release_pdb(&sessions, 0x2222u, 0)) return 8;
    bx_ntvdm_search_sessions_v1_cancel(&sessions, 0x1234u, &token);
    if (bx_ntvdm_search_sessions_v1_next(&sessions, 0x1234u, &token, &out,
        &next_token)) return 9;
    bx_ntvdm_search_sessions_v1_release(&sessions);
    return 0;
}
