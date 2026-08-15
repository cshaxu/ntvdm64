#include "bx_ntvdm_dem_file_session_v1.h"

#include <string.h>

static uint32_t token_for(uint32_t index, uint32_t generation)
{
    return (generation << 16) | (index + 1u);
}

static int decode(uint32_t token, uint32_t *index_out, uint32_t *generation_out)
{
    uint32_t index;
    if (token == 0u || index_out == 0 || generation_out == 0) return 0;
    index = (token & 0xffffu) - 1u;
    if (index >= BX_NTVDM_DEM_FILE_SESSION_V1_MAX_TOKENS) return 0;
    *index_out = index;
    *generation_out = token >> 16;
    return *generation_out != 0u;
}

int bx_ntvdm_dem_file_session_v1_valid(
    const bx_ntvdm_dem_file_session_v1 *session)
{
    uint32_t index;
    if (session == 0 || session->magic != BX_NTVDM_DEM_FILE_SESSION_V1_MAGIC ||
        session->abi_version != BX_NTVDM_DEM_FILE_SESSION_V1_VERSION ||
        session->struct_bytes != sizeof(*session) || session->reserved0 != 0u ||
        !bx_ntvdm_dem_profile_consumer_v1_valid(&session->profile)) return 0;
    for (index = 0u; index < BX_NTVDM_DEM_FILE_SESSION_V1_MAX_TOKENS; ++index) {
        const bx_ntvdm_dem_file_token_slot_v1 *slot = &session->slots[index];
        if (slot->generation == 0u || slot->in_use > 1u || slot->reserved0 != 0u ||
            (slot->in_use != 0u &&
             (slot->handle == 0 || slot->handle == INVALID_HANDLE_VALUE)) ||
            (slot->in_use == 0u && slot->pdb_owner != 0u)) return 0;
    }
    return 1;
}

int bx_ntvdm_dem_file_session_v1_initialize(
    bx_ntvdm_dem_file_session_v1 *session,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    uint32_t index;
    if (session == 0 || profile == 0 || !bx_ntvdm_mutation_profile_v1_valid(profile))
        return 0;
    memset(session, 0, sizeof(*session));
    session->magic = BX_NTVDM_DEM_FILE_SESSION_V1_MAGIC;
    session->abi_version = BX_NTVDM_DEM_FILE_SESSION_V1_VERSION;
    session->struct_bytes = sizeof(*session);
    if (!bx_ntvdm_dem_profile_consumer_v1_initialize(&session->profile, profile)) {
        memset(session, 0, sizeof(*session));
        return 0;
    }
    for (index = 0u; index < BX_NTVDM_DEM_FILE_SESSION_V1_MAX_TOKENS; ++index)
        session->slots[index].generation = 1u;
    return bx_ntvdm_dem_file_session_v1_valid(session);
}

int bx_ntvdm_dem_file_session_v1_adopt(
    bx_ntvdm_dem_file_session_v1 *session, HANDLE handle, uint32_t *token_out)
{
    return bx_ntvdm_dem_file_session_v1_adopt_owned(session, handle, 0u,
        token_out);
}

int bx_ntvdm_dem_file_session_v1_adopt_owned(
    bx_ntvdm_dem_file_session_v1 *session, HANDLE handle, uint16_t pdb_owner,
    uint32_t *token_out)
{
    uint32_t index;
    if (token_out != 0) *token_out = 0u;
    if (!bx_ntvdm_dem_file_session_v1_valid(session) || token_out == 0 ||
        handle == 0 || handle == INVALID_HANDLE_VALUE) return 0;
    for (index = 0u; index < BX_NTVDM_DEM_FILE_SESSION_V1_MAX_TOKENS; ++index) {
        bx_ntvdm_dem_file_token_slot_v1 *slot = &session->slots[index];
        if (slot->in_use == 0u) {
            slot->handle = handle;
            slot->in_use = 1u;
            slot->pdb_owner = pdb_owner;
            *token_out = token_for(index, slot->generation);
            return bx_ntvdm_dem_file_session_v1_valid(session);
        }
    }
    return 0;
}

int bx_ntvdm_dem_file_session_v1_lookup(
    const bx_ntvdm_dem_file_session_v1 *session, uint32_t token,
    HANDLE *handle_out)
{
    uint32_t index, generation;
    const bx_ntvdm_dem_file_token_slot_v1 *slot;
    if (handle_out != 0) *handle_out = INVALID_HANDLE_VALUE;
    if (!bx_ntvdm_dem_file_session_v1_valid(session) || handle_out == 0 ||
        !decode(token, &index, &generation)) return 0;
    slot = &session->slots[index];
    if (slot->in_use == 0u || slot->generation != generation) return 0;
    *handle_out = slot->handle;
    return 1;
}

int bx_ntvdm_dem_file_session_v1_release(
    bx_ntvdm_dem_file_session_v1 *session, uint32_t token)
{
    uint32_t index, generation;
    bx_ntvdm_dem_file_token_slot_v1 *slot;
    if (!bx_ntvdm_dem_file_session_v1_valid(session) ||
        !decode(token, &index, &generation)) return 0;
    slot = &session->slots[index];
    if (slot->in_use == 0u || slot->generation != generation ||
        !CloseHandle(slot->handle)) return 0;
    slot->handle = INVALID_HANDLE_VALUE;
    slot->in_use = 0u;
    slot->pdb_owner = 0u;
    ++slot->generation;
    if (slot->generation == 0u) slot->generation = 1u;
    return bx_ntvdm_dem_file_session_v1_valid(session);
}

int bx_ntvdm_dem_file_session_v1_release_owner(
    bx_ntvdm_dem_file_session_v1 *session, uint16_t pdb_owner,
    uint32_t *released_out)
{
    uint32_t index, released = 0u;
    if (released_out != 0) *released_out = 0u;
    if (!bx_ntvdm_dem_file_session_v1_valid(session) || pdb_owner == 0u)
        return 0;
    for (index = 0u; index < BX_NTVDM_DEM_FILE_SESSION_V1_MAX_TOKENS; ++index) {
        bx_ntvdm_dem_file_token_slot_v1 *slot = &session->slots[index];
        if (slot->in_use == 0u || slot->pdb_owner != pdb_owner) continue;
        if (!CloseHandle(slot->handle)) return 0;
        slot->handle = INVALID_HANDLE_VALUE;
        slot->in_use = 0u;
        slot->pdb_owner = 0u;
        ++slot->generation;
        if (slot->generation == 0u) slot->generation = 1u;
        ++released;
    }
    if (released_out != 0) *released_out = released;
    return bx_ntvdm_dem_file_session_v1_valid(session);
}

void bx_ntvdm_dem_file_session_v1_teardown(
    bx_ntvdm_dem_file_session_v1 *session)
{
    uint32_t index;
    if (session == 0) return;
    if (session->magic == BX_NTVDM_DEM_FILE_SESSION_V1_MAGIC) {
        for (index = 0u; index < BX_NTVDM_DEM_FILE_SESSION_V1_MAX_TOKENS; ++index) {
            bx_ntvdm_dem_file_token_slot_v1 *slot = &session->slots[index];
            if (slot->in_use != 0u && slot->handle != 0 &&
                slot->handle != INVALID_HANDLE_VALUE) CloseHandle(slot->handle);
        }
    }
    memset(session, 0, sizeof(*session));
}
