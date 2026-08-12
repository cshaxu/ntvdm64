#include "bx_ntvdm_search_session.h"

#include <bcrypt.h>
#include <string.h>

static void bx_ntvdm_search_sessions_release_slot(bx_ntvdm_search_slot_v1 *slot)
{
    if (slot == 0) return;
    if (slot->entries != 0) HeapFree(GetProcessHeap(), 0u, slot->entries);
    memset(slot, 0, sizeof(*slot));
}

static int bx_ntvdm_search_sessions_token_unique(
    const bx_ntvdm_search_sessions_v1 *value,
    const bx_ntvdm_search_token_v1 *token)
{
    uint32_t index;
    if (token->cookie == 0u || token->id == 0u) return 0;
    for (index = 0u; index < BX_NTVDM_SEARCH_SESSION_V1_MAX_SESSIONS; ++index) {
        const bx_ntvdm_search_slot_v1 *slot = &value->slots[index];
        if (slot->entries != 0 && slot->token.cookie == token->cookie &&
            slot->token.id == token->id) return 0;
    }
    return 1;
}

static int bx_ntvdm_search_sessions_random_token(
    const bx_ntvdm_search_sessions_v1 *value, bx_ntvdm_search_token_v1 *token)
{
    uint32_t attempts;
    if (value == 0 || token == 0) return 0;
    for (attempts = 0u; attempts < 8u; ++attempts) {
        if (BCryptGenRandom(0, (PUCHAR)token, sizeof(*token),
                BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0) return 0;
        if (bx_ntvdm_search_sessions_token_unique(value, token)) return 1;
    }
    return 0;
}

static bx_ntvdm_search_slot_v1 *bx_ntvdm_search_sessions_find(
    bx_ntvdm_search_sessions_v1 *value, uint32_t owner_pdb,
    const bx_ntvdm_search_token_v1 *presented)
{
    uint32_t index;
    if (!bx_ntvdm_search_sessions_v1_valid(value) || presented == 0 ||
        presented->cookie == 0u || presented->id == 0u) return 0;
    for (index = 0u; index < BX_NTVDM_SEARCH_SESSION_V1_MAX_SESSIONS; ++index) {
        bx_ntvdm_search_slot_v1 *slot = &value->slots[index];
        if (slot->entries != 0 && slot->owner_pdb == owner_pdb &&
            slot->token.cookie == presented->cookie &&
            slot->token.id == presented->id) return slot;
    }
    return 0;
}

void bx_ntvdm_search_sessions_v1_initialize(bx_ntvdm_search_sessions_v1 *value)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_SEARCH_SESSION_V1_MAGIC;
    value->version = BX_NTVDM_SEARCH_SESSION_V1_VERSION;
}

void bx_ntvdm_search_sessions_v1_release(bx_ntvdm_search_sessions_v1 *value)
{
    uint32_t index;
    if (value == 0) return;
    for (index = 0u; index < BX_NTVDM_SEARCH_SESSION_V1_MAX_SESSIONS; ++index)
        bx_ntvdm_search_sessions_release_slot(&value->slots[index]);
    memset(value, 0, sizeof(*value));
}

int bx_ntvdm_search_sessions_v1_valid(const bx_ntvdm_search_sessions_v1 *value)
{
    uint32_t index;
    if (value == 0 || value->magic != BX_NTVDM_SEARCH_SESSION_V1_MAGIC ||
        value->version != BX_NTVDM_SEARCH_SESSION_V1_VERSION) return 0;
    for (index = 0u; index < BX_NTVDM_SEARCH_SESSION_V1_MAX_SESSIONS; ++index) {
        const bx_ntvdm_search_slot_v1 *slot = &value->slots[index];
        if (slot->entries == 0) {
            if (slot->entry_count != 0u || slot->next_index != 0u ||
                slot->token.cookie != 0u || slot->token.id != 0u) return 0;
        } else if (slot->entry_count < 2u || slot->next_index == 0u ||
            slot->next_index >= slot->entry_count || slot->token.cookie == 0u ||
            slot->token.id == 0u) return 0;
    }
    return 1;
}

int bx_ntvdm_search_sessions_v1_begin(bx_ntvdm_search_sessions_v1 *value,
    uint32_t owner_pdb, const bx_ntvdm_host_namespace_entry_v1 *entries,
    uint32_t entry_count, bx_ntvdm_host_namespace_entry_v1 *first,
    bx_ntvdm_search_token_v1 *continuation)
{
    uint32_t index;
    bx_ntvdm_search_slot_v1 *slot = 0;
    if (first == 0 || continuation == 0 || entries == 0 || entry_count == 0u ||
        entry_count > BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES ||
        !bx_ntvdm_search_sessions_v1_valid(value)) return 0;
    memset(continuation, 0, sizeof(*continuation));
    *first = entries[0];
    if (entry_count == 1u) return 1;
    for (index = 0u; index < BX_NTVDM_SEARCH_SESSION_V1_MAX_SESSIONS; ++index) {
        if (value->slots[index].entries == 0) { slot = &value->slots[index]; break; }
    }
    if (slot == 0 || !bx_ntvdm_search_sessions_random_token(value, continuation)) return 0;
    slot->entries = (bx_ntvdm_host_namespace_entry_v1 *)HeapAlloc(GetProcessHeap(),
        HEAP_ZERO_MEMORY, (SIZE_T)entry_count * sizeof(*slot->entries));
    if (slot->entries == 0) { memset(continuation, 0, sizeof(*continuation)); return 0; }
    memcpy(slot->entries, entries, (size_t)entry_count * sizeof(*entries));
    slot->token = *continuation;
    slot->owner_pdb = owner_pdb;
    slot->entry_count = entry_count;
    slot->next_index = 1u;
    return 1;
}

int bx_ntvdm_search_sessions_v1_next(bx_ntvdm_search_sessions_v1 *value,
    uint32_t owner_pdb, const bx_ntvdm_search_token_v1 *presented,
    bx_ntvdm_host_namespace_entry_v1 *next,
    bx_ntvdm_search_token_v1 *continuation)
{
    bx_ntvdm_search_slot_v1 *slot;
    if (next == 0 || continuation == 0) return 0;
    memset(continuation, 0, sizeof(*continuation));
    slot = bx_ntvdm_search_sessions_find(value, owner_pdb, presented);
    if (slot == 0) return 0;
    *next = slot->entries[slot->next_index++];
    if (slot->next_index == slot->entry_count) bx_ntvdm_search_sessions_release_slot(slot);
    else *continuation = slot->token;
    return 1;
}

void bx_ntvdm_search_sessions_v1_cancel(bx_ntvdm_search_sessions_v1 *value,
    uint32_t owner_pdb, const bx_ntvdm_search_token_v1 *presented)
{
    bx_ntvdm_search_slot_v1 *slot = bx_ntvdm_search_sessions_find(value,
        owner_pdb, presented);
    if (slot != 0) bx_ntvdm_search_sessions_release_slot(slot);
}

int bx_ntvdm_search_sessions_v1_release_pdb(bx_ntvdm_search_sessions_v1 *value,
    uint16_t owner_pdb, uint32_t *released_slots)
{
    uint32_t index, released = 0u;
    if (released_slots != 0) *released_slots = 0u;
    if (!bx_ntvdm_search_sessions_v1_valid(value)) return 0;
    for (index = 0u; index < BX_NTVDM_SEARCH_SESSION_V1_MAX_SESSIONS; ++index) {
        bx_ntvdm_search_slot_v1 *slot = &value->slots[index];
        if (slot->entries != 0 && slot->owner_pdb == (uint32_t)owner_pdb) {
            bx_ntvdm_search_sessions_release_slot(slot);
            ++released;
        }
    }
    if (!bx_ntvdm_search_sessions_v1_valid(value)) return 0;
    if (released_slots != 0) *released_slots = released;
    return 1;
}
