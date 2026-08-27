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
