#include <windows.h>

#include "doswow.h"
#include "mvdm_vdd_sft_shadow.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "mvdm_host_identity.h"
#include "session/session.h"

#define MVDM_VDD_SFT_SHADOW_MAGIC UINT32_C(0x53465453)
#define MVDM_VDD_SFT_HEADER_BYTES 6u
#define MVDM_VDD_SFT_MAXIMUM_GROUPS 256u

typedef struct mvdm_vdd_sft_registry mvdm_vdd_sft_registry;

typedef struct mvdm_vdd_sft_shadow {
    /* This must remain first: original callers receive &sft as PDOSSFT. */
    DOSSFT sft;
    uint32_t magic;
    uint32_t pointer_identity;
    uint32_t host_handle_identity;
    uint32_t sft_entry_offset;
    uint32_t jft_length;
    mvdm_guest_location sft_location;
    mvdm_guest_location jft_location;
    uint8_t *jft_bytes;
    mvdm_vdd_sft_registry *registry;
    struct mvdm_vdd_sft_shadow *next;
} mvdm_vdd_sft_shadow;

struct mvdm_vdd_sft_registry {
    session *instance;
    mvdm_vdd_sft_shadow *shadows;
    mvdm_vdd_sft_registry *next;
};

static SRWLOCK mvdm_vdd_sft_registry_lock = SRWLOCK_INIT;
static mvdm_vdd_sft_registry *mvdm_vdd_sft_registries;

static uint16_t read_u16(const uint8_t *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

static uint32_t read_u32(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
        ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

static int location_advance(mvdm_guest_location *location, uint32_t amount)
{
    uint32_t linear;

    if (location == NULL || location->valid != 1u) return 0;
    linear = ((uint32_t)location->segment << 4) + location->offset;
    if (amount > 0x000fffffu - linear) return 0;
    linear += amount;
    location->segment = (uint16_t)(linear >> 4);
    location->offset = (uint16_t)(linear & 0x000fu);
    return 1;
}

static int copy_from_guest(const mvdm_guest_location *location,
    uint32_t byte_count, uint8_t *bytes)
{
    mvdm_guest_location_lease lease;
    int result;

    if (bytes == NULL || !mvdm_guest_location_acquire(location, byte_count,
        GUEST_MEMORY_ACCESS_READ, &lease)) return 0;
    memcpy(bytes, lease.bytes, byte_count);
    result = mvdm_guest_location_release(&lease, 0);
    return result;
}

static int copy_to_guest(const mvdm_guest_location *location,
    uint32_t byte_count, const uint8_t *bytes)
{
    mvdm_guest_location_lease lease;
    int result;

    if (bytes == NULL || !mvdm_guest_location_acquire(location, byte_count,
        GUEST_MEMORY_ACCESS_READ | GUEST_MEMORY_ACCESS_WRITE, &lease)) return 0;
    memcpy(lease.bytes, bytes, byte_count);
    result = mvdm_guest_location_release(&lease, 1);
    return result;
}

static mvdm_vdd_sft_shadow *shadow_from_sft(PDOSSFT sft)
{
    uintptr_t value;
    uint32_t identity;
    mvdm_vdd_sft_shadow *shadow;

    if (sft == NULL || !mvdm_host_identity_lookup((uintptr_t)sft, &identity) ||
        !mvdm_host_identity_resolve(identity, &value) ||
        value != (uintptr_t)sft) return NULL;
    shadow = (mvdm_vdd_sft_shadow *)sft;
    if (shadow->magic != MVDM_VDD_SFT_SHADOW_MAGIC ||
        shadow->pointer_identity != identity) return NULL;
    return shadow;
}

static void release_identity(session *instance, uint32_t identity)
{
    mapping_manager *resources;

    if (instance == NULL || identity == 0u) return;
    resources = session_host_resource_mappings(instance);
    if (resources != NULL) (void)mapping_manager_release(resources, identity);
}

static void registry_remove_shadow(mvdm_vdd_sft_shadow *shadow)
{
    mvdm_vdd_sft_shadow **link;

    if (shadow == NULL || shadow->registry == NULL) return;
    AcquireSRWLockExclusive(&mvdm_vdd_sft_registry_lock);
    link = &shadow->registry->shadows;
    while (*link != NULL && *link != shadow) link = &(*link)->next;
    if (*link == shadow) *link = shadow->next;
    ReleaseSRWLockExclusive(&mvdm_vdd_sft_registry_lock);
    shadow->next = NULL;
    shadow->registry = NULL;
}

static void registry_teardown(void *context)
{
    mvdm_vdd_sft_registry *registry = (mvdm_vdd_sft_registry *)context;
    mvdm_vdd_sft_registry **link;
    mvdm_vdd_sft_shadow *shadow;

    if (registry == NULL) return;
    AcquireSRWLockExclusive(&mvdm_vdd_sft_registry_lock);
    link = &mvdm_vdd_sft_registries;
    while (*link != NULL && *link != registry) link = &(*link)->next;
    if (*link == registry) *link = registry->next;
    shadow = registry->shadows;
    registry->shadows = NULL;
    ReleaseSRWLockExclusive(&mvdm_vdd_sft_registry_lock);
    while (shadow != NULL) {
        mvdm_vdd_sft_shadow *next = shadow->next;
        release_identity(registry->instance, shadow->pointer_identity);
        release_identity(registry->instance, shadow->host_handle_identity);
        free(shadow->jft_bytes);
        memset(shadow, 0, sizeof(*shadow));
        free(shadow);
        shadow = next;
    }
    free(registry);
}

static mvdm_vdd_sft_registry *registry_current(void)
{
    session *instance = session_thread_current();
    mvdm_vdd_sft_registry *registry;

    if (instance == NULL) return NULL;
    AcquireSRWLockExclusive(&mvdm_vdd_sft_registry_lock);
    for (registry = mvdm_vdd_sft_registries; registry != NULL;
        registry = registry->next) {
        if (registry->instance == instance) break;
    }
    if (registry == NULL) {
        registry = (mvdm_vdd_sft_registry *)calloc(1u, sizeof(*registry));
        if (registry != NULL) {
            registry->instance = instance;
            registry->next = mvdm_vdd_sft_registries;
            mvdm_vdd_sft_registries = registry;
            if (!session_register_teardown(instance, registry_teardown,
                registry)) {
                mvdm_vdd_sft_registries = registry->next;
                free(registry);
                registry = NULL;
            }
        }
    }
    ReleaseSRWLockExclusive(&mvdm_vdd_sft_registry_lock);
    return registry;
}

static int registry_add_shadow(mvdm_vdd_sft_shadow *shadow)
{
    mvdm_vdd_sft_registry *registry = registry_current();

    if (shadow == NULL || registry == NULL) return 0;
    AcquireSRWLockExclusive(&mvdm_vdd_sft_registry_lock);
    shadow->registry = registry;
    shadow->next = registry->shadows;
    registry->shadows = shadow;
    ReleaseSRWLockExclusive(&mvdm_vdd_sft_registry_lock);
    return 1;
}

static void shadow_free(mvdm_vdd_sft_shadow *shadow, int release_handle)
{
    session *instance;

    if (shadow == NULL) return;
    instance = shadow->registry == NULL ? session_thread_current() :
        shadow->registry->instance;
    registry_remove_shadow(shadow);
    release_identity(instance, shadow->pointer_identity);
    if (release_handle) release_identity(instance, shadow->host_handle_identity);
    free(shadow->jft_bytes);
    memset(shadow, 0, sizeof(*shadow));
    free(shadow);
}

static int read_pdb(ULONG pdb_far, mvdm_guest_location *jft_location,
    uint16_t *jft_length)
{
    mvdm_guest_location pdb_location;
    uint8_t bytes[sizeof(DOSPDB)];

    if (jft_location != NULL) memset(jft_location, 0, sizeof(*jft_location));
    if (jft_length != NULL) *jft_length = 0u;
    if (jft_location == NULL || jft_length == NULL ||
        !mvdm_guest_location_from_far_value(&pdb_location, pdb_far) ||
        !copy_from_guest(&pdb_location, (uint32_t)sizeof(bytes), bytes) ||
        !mvdm_guest_location_from_far_value(jft_location,
            read_u32(bytes + offsetof(DOSPDB, PDB_JFN_Pointer)))) return 0;
    *jft_length = read_u16(bytes + offsetof(DOSPDB, PDB_JFN_Length));
    return *jft_length != 0u;
}

static int select_sft(const mvdm_guest_location *head, int want_free,
    uint16_t wanted_sfn, mvdm_guest_location *entry_location,
    uint32_t *entry_offset_out, DOSSFT *sft_out, uint16_t *sfn_out)
{
    mvdm_guest_location group;
    uint16_t remaining = wanted_sfn;
    uint16_t accumulated = 0u;
    uint32_t group_number;

    if (entry_location != NULL) memset(entry_location, 0,
        sizeof(*entry_location));
    if (entry_offset_out != NULL) *entry_offset_out = 0u;
    if (sft_out != NULL) memset(sft_out, 0, sizeof(*sft_out));
    if (sfn_out != NULL) *sfn_out = 0u;
    if (head == NULL || entry_location == NULL || entry_offset_out == NULL ||
        sft_out == NULL || sfn_out == NULL || head->valid != 1u) return 0;
    group = *head;
    for (group_number = 0u; group_number < MVDM_VDD_SFT_MAXIMUM_GROUPS;
        ++group_number) {
        uint8_t header[MVDM_VDD_SFT_HEADER_BYTES];
        uint8_t *entries;
        uint16_t count;
        uint16_t index;
        uint32_t byte_count;
        uint32_t link;

        if (!copy_from_guest(&group, (uint32_t)sizeof(header), header)) return 0;
        count = read_u16(header + 4u);
        if (count == 0u || count > (UINT32_MAX - MVDM_VDD_SFT_HEADER_BYTES) /
            sizeof(DOSSFT)) return 0;
        byte_count = (uint32_t)count * (uint32_t)sizeof(DOSSFT);
        entries = (uint8_t *)malloc(byte_count);
        if (entries == NULL) return 0;
        {
            mvdm_guest_location entries_location = group;
            if (!location_advance(&entries_location,
                MVDM_VDD_SFT_HEADER_BYTES) ||
                !copy_from_guest(&entries_location, byte_count, entries)) {
                free(entries);
                return 0;
            }
        }
        for (index = 0u; index < count; ++index) {
            DOSSFT current;
            memcpy(&current, entries + (uint32_t)index * sizeof(current),
                sizeof(current));
            if ((want_free && current.SFT_Ref_Count == 0u) ||
                (!want_free && remaining == 0u)) {
                *entry_location = group;
                if (!location_advance(entry_location,
                    MVDM_VDD_SFT_HEADER_BYTES + (uint32_t)index *
                    sizeof(DOSSFT))) {
                    free(entries);
                    return 0;
                }
                *entry_offset_out = 0u;
                *sft_out = current;
                *sfn_out = (uint16_t)(accumulated + index);
                free(entries);
                return 1;
            }
            if (!want_free) --remaining;
        }
        free(entries);
        accumulated = (uint16_t)(accumulated + count);
        link = read_u32(header);
        if ((link & 0xffffu) == 0xffffu ||
            !mvdm_guest_location_from_far_value(&group, link)) return 0;
    }
    return 0;
}

static mvdm_vdd_sft_shadow *shadow_create(ULONG pdb_far,
    const mvdm_guest_location *head, int want_free, uint16_t wanted_sfn,
    uint16_t *sfn_out, SHORT *dos_handle_out)
{
    mvdm_vdd_sft_shadow *shadow;
    uint16_t jft_length;
    uint16_t index;
    uint16_t sfn;
    uint32_t ignored_offset;

    if (sfn_out != NULL) *sfn_out = 0u;
    if (dos_handle_out != NULL) *dos_handle_out = (SHORT)-1;
    shadow = (mvdm_vdd_sft_shadow *)calloc(1u, sizeof(*shadow));
    if (shadow == NULL || !read_pdb(pdb_far, &shadow->jft_location,
        &jft_length)) goto failure;
    shadow->jft_bytes = (uint8_t *)malloc(jft_length);
    if (shadow->jft_bytes == NULL || !copy_from_guest(&shadow->jft_location,
        jft_length, shadow->jft_bytes)) goto failure;
    shadow->jft_length = jft_length;
    if (want_free) {
        for (index = 0u; index < jft_length; ++index)
            if (shadow->jft_bytes[index] == 0xffu) break;
        if (index == jft_length) goto failure;
        if (!select_sft(head, 1, 0u, &shadow->sft_location, &ignored_offset,
            &shadow->sft, &sfn)) goto failure;
        shadow->jft_bytes[index] = (uint8_t)sfn;
        memset(&shadow->sft, 0, sizeof(shadow->sft));
        shadow->sft.SFT_Ref_Count = 1u;
        if (dos_handle_out != NULL) *dos_handle_out = (SHORT)index;
    } else {
        if (wanted_sfn == 0xffu || !select_sft(head, 0, wanted_sfn,
            &shadow->sft_location, &ignored_offset, &shadow->sft, &sfn))
            goto failure;
        if (shadow->sft.SFT_Ref_Count == 0u) goto failure;
    }
    shadow->magic = MVDM_VDD_SFT_SHADOW_MAGIC;
    if (!mvdm_host_identity_publish((uintptr_t)&shadow->sft,
        &shadow->pointer_identity) || !registry_add_shadow(shadow)) goto failure;
    if (sfn_out != NULL) *sfn_out = sfn;
    return shadow;

failure:
    shadow_free(shadow, 0);
    return NULL;
}

SHORT mvdm_vdd_sft_shadow_allocate(ULONG pdb_far,
    const mvdm_guest_location *sft_head, PDOSSFT *sft_out, PBYTE *jft_out)
{
    mvdm_vdd_sft_shadow *shadow;
    SHORT handle;

    if (sft_out != NULL) *sft_out = NULL;
    if (jft_out != NULL) *jft_out = NULL;
    shadow = shadow_create(pdb_far, sft_head, 1, 0u, NULL, &handle);
    if (shadow == NULL) return (SHORT)-1;
    if (sft_out == NULL) {
        shadow_free(shadow, 0);
        return (SHORT)-1;
    }
    *sft_out = &shadow->sft;
    if (jft_out != NULL) *jft_out = shadow->jft_bytes;
    return handle;
}

HANDLE mvdm_vdd_sft_shadow_retrieve(ULONG pdb_far,
    const mvdm_guest_location *sft_head, SHORT dos_handle,
    PDOSSFT *sft_out, PBYTE *jft_out)
{
    mvdm_vdd_sft_shadow *shadow;
    uint32_t handle_identity;
    uintptr_t native_handle;

    if (sft_out != NULL) *sft_out = NULL;
    if (jft_out != NULL) *jft_out = NULL;
    if (dos_handle < 0) return NULL;
    /* The JFT selects the SFT number; it is not supplied by the caller. */
    {
        mvdm_guest_location jft_location;
        uint16_t jft_length;
        uint8_t *jft_bytes;
        uint16_t sfn;
        uint32_t ignored_offset;
        if (!read_pdb(pdb_far, &jft_location, &jft_length) ||
            (uint16_t)dos_handle >= jft_length) return NULL;
        jft_bytes = (uint8_t *)malloc(jft_length);
        if (jft_bytes == NULL || !copy_from_guest(&jft_location, jft_length,
            jft_bytes)) { free(jft_bytes); return NULL; }
        sfn = jft_bytes[(uint16_t)dos_handle];
        if (sfn == 0xffu) { free(jft_bytes); return NULL; }
        shadow = (mvdm_vdd_sft_shadow *)calloc(1u, sizeof(*shadow));
        if (shadow == NULL) { free(jft_bytes); return NULL; }
        shadow->jft_location = jft_location;
        shadow->jft_length = jft_length;
        shadow->jft_bytes = jft_bytes;
        if (!select_sft(sft_head, 0, sfn, &shadow->sft_location,
            &ignored_offset, &shadow->sft, &sfn) ||
            shadow->sft.SFT_Ref_Count == 0u) {
            shadow_free(shadow, 0); return NULL;
        }
        shadow->magic = MVDM_VDD_SFT_SHADOW_MAGIC;
        if (!mvdm_host_identity_publish((uintptr_t)&shadow->sft,
            &shadow->pointer_identity) || !registry_add_shadow(shadow)) {
            shadow_free(shadow, 0); return NULL;
        }
    }
    handle_identity = shadow->sft.SFT_NTHandle;
    if (handle_identity == 0u || !mvdm_host_identity_resolve(handle_identity,
        &native_handle)) { shadow_free(shadow, 0); return NULL; }
    shadow->host_handle_identity = handle_identity;
    if (sft_out == NULL) {
        shadow_free(shadow, 0);
        return (HANDLE)native_handle;
    }
    *sft_out = &shadow->sft;
    if (jft_out != NULL) *jft_out = shadow->jft_bytes;
    return (HANDLE)native_handle;
}

int mvdm_vdd_sft_shadow_associate(PDOSSFT sft, HANDLE file_handle,
    WORD access)
{
    mvdm_vdd_sft_shadow *shadow = shadow_from_sft(sft);
    uint32_t identity;

    if (shadow == NULL || !mvdm_host_identity_publish((uintptr_t)file_handle,
        &identity)) return 0;
    shadow->host_handle_identity = identity;
    shadow->sft.SFT_Mode = access & 0x7fu;
    shadow->sft.SFT_Attr = 0u;
    shadow->sft.SFT_Flags = (access & 0x80u) ? 0x1000u : 0u;
    shadow->sft.SFT_Devptr = (ULONG)-1;
    shadow->sft.SFT_NTHandle = identity;
    /* The original caller may still write SFT fields after Associate.  It
     * commits at its final source-visible write boundary. */
    return 1;
}

int mvdm_vdd_sft_shadow_commit(PDOSSFT sft)
{
    mvdm_vdd_sft_shadow *shadow = shadow_from_sft(sft);
    int release_handle;

    if (shadow == NULL || !copy_to_guest(&shadow->sft_location,
        (uint32_t)sizeof(shadow->sft), (const uint8_t *)&shadow->sft) ||
        !copy_to_guest(&shadow->jft_location, shadow->jft_length,
            shadow->jft_bytes)) return 0;
    release_handle = shadow->sft.SFT_Ref_Count == 0u;
    shadow_free(shadow, release_handle);
    return 1;
}

void mvdm_vdd_sft_shadow_discard(PDOSSFT sft)
{
    shadow_free(shadow_from_sft(sft), 0);
}
