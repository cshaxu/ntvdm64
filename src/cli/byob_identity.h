#ifndef BYOB_IDENTITY_H
#define BYOB_IDENTITY_H

#include <stdint.h>
#include <wchar.h>

typedef enum byob_identity_result {
    BYOB_IDENTITY_OK,
    BYOB_IDENTITY_NAME_UNSAFE,
    BYOB_IDENTITY_OPEN_REJECTED,
    BYOB_IDENTITY_MISMATCH
} byob_identity_result;

int byob_component_name_safe(const wchar_t *name);
byob_identity_result byob_verify_component(const wchar_t *root,
    const wchar_t *name, uint64_t expected_bytes, const wchar_t expected_sha256[65]);

#endif
