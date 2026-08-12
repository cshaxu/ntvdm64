#include "historical_dem_byob_profile_v1.h"

#include <stdlib.h>
#include <string.h>

extern char *pszDefaultDOSDirectory;

int __cdecl ntdos64_historical_dem_byob_profile_v1_replace_directory(const char *directory)
{
    char *replacement;
    size_t length;

    if (directory == NULL || directory[0] == '\0') return 0;
    length = strlen(directory);
    if (length > 240u) return 0;
    replacement = malloc(length + sizeof("\\ntdos.sys") + 1u);
    if (replacement == NULL) return 0;
    memcpy(replacement, directory, length + 1u);
    free(pszDefaultDOSDirectory);
    pszDefaultDOSDirectory = replacement;
    return 1;
}
