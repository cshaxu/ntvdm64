#ifndef NTDOS64_CONFIG_H
#define NTDOS64_CONFIG_H

#include <windows.h>
#include "byob_profile.h"

/* Loads only the fixed, sibling ntvdmcfg.yaml file.  Values are flat scalar
 * YAML entries; no ambient root, schema, or version selector is accepted. */
int ntdos64_config_load_sibling(wchar_t config_path[MAX_PATH], wchar_t root[MAX_PATH],
    wchar_t config_source[MAX_PATH], wchar_t autoexec_source[MAX_PATH],
    byob_profile_selection *selection);

#endif
