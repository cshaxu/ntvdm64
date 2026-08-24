#ifndef APP_STARTUP_SELECTION_H
#define APP_STARTUP_SELECTION_H

/* The copied startup transport is configuration output, not a JSON profile.
 * The retained layout is shared with older adapter-only evidence, so this
 * name is the product-facing spelling while that evidence ages out. */
#include "byob_profile.h"

typedef byob_profile_selection app_startup_selection;

#endif
