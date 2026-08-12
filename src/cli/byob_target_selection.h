#ifndef BYOB_TARGET_SELECTION_H
#define BYOB_TARGET_SELECTION_H

#include <wchar.h>

#include "byob_profile.h"

/* Compares only normalized local names.  Content identity remains owned by
 * byob_image_load_exact in the receiving adapter process. */
int byob_target_selection_matches(const wchar_t *byob_root,
    const byob_profile_selection *selection, const wchar_t *target_path);

#endif
