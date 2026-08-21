#ifndef NTDOS64_CONFIG_H
#define NTDOS64_CONFIG_H

#include <windows.h>
#include "byob_profile.h"

/* Loads the fixed flat YAML form.  The root is always derived from the config
 * pathname; callers may compare it with a copied transport descriptor. */
int ntdos64_config_load_file(const wchar_t *config_path, wchar_t root[MAX_PATH],
    wchar_t config_source[MAX_PATH], wchar_t autoexec_source[MAX_PATH],
    byob_profile_selection *selection);

/* Runner entry: resolve only the fixed sibling ntvdmcfg.yaml. */
int ntdos64_config_load_sibling(wchar_t config_path[MAX_PATH], wchar_t root[MAX_PATH],
    wchar_t config_source[MAX_PATH], wchar_t autoexec_source[MAX_PATH],
    byob_profile_selection *selection);

#endif
