#include "session/session.h"

int main(void)
{
    session instance;
    mapping_manager isolated;
    mapping_manager *host;
    mapping_manager *completion;
    uint32_t first, second, other;
    uintptr_t value;

    session_initialize(&instance, 1u);
    host = session_host_resource_mappings(&instance);
    completion = session_completion_callback_mappings(&instance);
    if (host == NULL || completion == NULL || host == completion ||
        mapping_manager_publish(host, (uintptr_t)0x1010u, &first) == 0 ||
        first != 1u || mapping_manager_publish(host, (uintptr_t)0x1010u, &second) == 0 ||
        second != first || mapping_manager_lookup_value(host, first, &value) == 0 ||
        value != (uintptr_t)0x1010u ||
        mapping_manager_publish(completion, (uintptr_t)0x1010u, &other) == 0 ||
        other != 1u)
        return 1;
    if (mapping_manager_release(host, first) == 0 ||
        mapping_manager_lookup_value(host, first, &value) != 0 ||
        mapping_manager_publish(host, (uintptr_t)0x2020u, &second) == 0 ||
        second != 2u || host->tombstone_count != 1u)
        return 2;
    mapping_manager_initialize(&isolated, MAPPING_MANAGER_HOST_RESOURCE,
        MAPPING_MANAGER_RESERVE_MAXIMUM);
    if (mapping_manager_publish(&isolated, (uintptr_t)0x3030u, &other) == 0 ||
        other != 0u) return 3;
#if defined(SESSION_MAPPING_TESTING)
    mapping_manager_dispose(&isolated);
    mapping_manager_initialize(&isolated, MAPPING_MANAGER_HOST_RESOURCE,
        MAPPING_MANAGER_RESERVE_MAXIMUM);
    mapping_manager_test_set_next_identifier(&isolated, UINT32_MAX);
    if (mapping_manager_publish(&isolated, (uintptr_t)0x4040u, &other) != 0)
        return 4;
#endif
    mapping_manager_dispose(&isolated);
    if (session_dispose(&instance) == 0) return 5;
    return 0;
}
