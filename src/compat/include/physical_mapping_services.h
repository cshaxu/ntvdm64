#ifndef COMPAT_PHYSICAL_MAPPING_SERVICES_H
#define COMPAT_PHYSICAL_MAPPING_SERVICES_H
#include <stdint.h>
#include <windows.h>

/* Immutable capability binding. Session mappings, aliases and teardown are
 * owned by the product; this interface owns no guest-memory state. */
typedef struct compat_physical_mapping_services {
    int (*initialize)(void *, uint32_t);
    void (*release)(void);
    void (*set_records)(uint32_t, uint32_t, uint32_t);
    int32_t (*map)(uint32_t, uint32_t, uint32_t);
    int32_t (*unmap)(uint32_t, uint32_t);
    int (*translate)(uint32_t, uint32_t *);
    int (*resolve)(uint32_t, uint8_t **);
    int (*span_is_aliased)(uint32_t, uint32_t);
} compat_physical_mapping_services;
BOOL compat_physical_mapping_install(const compat_physical_mapping_services *services);
const compat_physical_mapping_services *compat_physical_mapping_current_services(void);
#endif
