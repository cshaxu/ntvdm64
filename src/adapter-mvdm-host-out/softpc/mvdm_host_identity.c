#include "mvdm_host_identity.h"

#include "session/session.h"

static mapping_manager *current_host_resources(void)
{
    session *instance = session_thread_current();
    return instance == NULL ? NULL : session_host_resource_mappings(instance);
}

int mvdm_host_identity_publish(uintptr_t native_value, uint32_t *identity_out)
{
    mapping_manager *resources = current_host_resources();
    if (identity_out != NULL) *identity_out = 0u;
    return resources != NULL && mapping_manager_publish(resources, native_value,
        identity_out);
}

int mvdm_host_identity_resolve(uint32_t identity,
    uintptr_t *native_value_out)
{
    mapping_manager *resources = current_host_resources();
    if (native_value_out != NULL) *native_value_out = (uintptr_t)0u;
    return resources != NULL && mapping_manager_lookup_value(resources,
        identity, native_value_out);
}

int mvdm_host_identity_lookup(uintptr_t native_value, uint32_t *identity_out)
{
    mapping_manager *resources = current_host_resources();
    if (identity_out != NULL) *identity_out = 0u;
    return resources != NULL && mapping_manager_lookup_identifier(resources,
        native_value, identity_out);
}

int mvdm_host_identity_release(uint32_t identity)
{
    mapping_manager *resources = current_host_resources();
    return resources != NULL && mapping_manager_release(resources, identity);
}

uintptr_t mvdm_host_identity_resolve_words(uint16_t high, uint16_t low)
{
    uint32_t identity = ((uint32_t)high << 16u) | (uint32_t)low;
    uintptr_t native_value = (uintptr_t)0u;

    if (identity == 0u) return (uintptr_t)0u;
    if (!mvdm_host_identity_resolve(identity, &native_value))
        return UINTPTR_MAX;
    return native_value;
}

int mvdm_host_identity_publish_words(uintptr_t native_value,
    uint16_t *high_out, uint16_t *low_out)
{
    uint32_t identity = 0u;

    if (high_out != NULL) *high_out = 0u;
    if (low_out != NULL) *low_out = 0u;
    if (!mvdm_host_identity_publish(native_value, &identity)) return 0;
    if (high_out != NULL) *high_out = (uint16_t)(identity >> 16u);
    if (low_out != NULL) *low_out = (uint16_t)identity;
    return 1;
}
