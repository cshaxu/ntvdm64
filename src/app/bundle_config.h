#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <windows.h>
#include "startup_selection.h"

/* The executable directory owns the fixed product layout: dos\ contains
 * bootstrap/configuration files and wow16\ is the separately named WOW root. */
int app_bundle_load_sibling(wchar_t product_root[MAX_PATH],
    wchar_t dos_root[MAX_PATH], wchar_t wow16_root[MAX_PATH],
    wchar_t config_source[MAX_PATH], wchar_t autoexec_source[MAX_PATH]);

/* Native entry receives the copied dos root from the application. */
int app_bundle_load_roots(const wchar_t *dos_root,
    wchar_t config_source[MAX_PATH], wchar_t autoexec_source[MAX_PATH]);

#endif
