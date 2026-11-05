/* Product frontend identity. A locator never grants authority: NTSRV also
 * authenticates the owning process/control connection on every operation. */
#ifndef COMMON_FRONTEND_INSTANCE_H
#define COMMON_FRONTEND_INSTANCE_H
#include <stdint.h>
typedef struct common_frontend_key {
    uint64_t instance;
    uint32_t generation;
} common_frontend_key;
typedef struct common_frontend_location {
    uint64_t payload;
    uint64_t vm;
    uint64_t incarnation;
} common_frontend_location;
#endif
