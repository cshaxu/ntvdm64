#ifndef BYOB_IMAGE_H
#define BYOB_IMAGE_H

#include "byob_profile.h"

#include <stddef.h>
#include <stdint.h>

typedef enum byob_image_result {
    BYOB_IMAGE_OK,
    BYOB_IMAGE_INVALID_ARGUMENT,
    BYOB_IMAGE_OPEN_REJECTED,
    BYOB_IMAGE_IDENTITY_MISMATCH,
    BYOB_IMAGE_NO_MEMORY
} byob_image_result;

typedef struct byob_image {
    uint8_t *bytes;
    size_t byte_count;
} byob_image;

/* Reads a component from one non-reparse, write-share-denied file handle and
 * verifies its exact selected identity before returning the transient buffer. */
byob_image_result byob_image_load_exact(const wchar_t *root,
    const byob_component_descriptor *descriptor, byob_image *out_image);
void byob_image_release(byob_image *image);

#endif
