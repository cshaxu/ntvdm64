#include "bop-v1/bx_ntvdm_dem_path_v1.h"

#include <wchar.h>

static int profile(bx_ntvdm_mutation_profile_v1 *value)
{
    bx_ntvdm_mutation_profile_v1_initialize(value, BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(value,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT,
        1u << (BX_NTVDM_MUTATION_MODE_V1_DIRECT - 1u));
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 mutation;
    bx_ntvdm_dem_cwd_context_v1 cwd;
    wchar_t relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint8_t drive;
    if (!profile(&mutation) || !bx_ntvdm_dem_cwd_context_v1_initialize(&cwd, &mutation))
        return 1;
    if (bx_ntvdm_dem_path_v1_resolve("C:\\DOS\\TEST.TXT", &cwd, &drive, relative) !=
            BX_NTVDM_DEM_PATH_V1_OK || drive != 2u || wcscmp(relative, L"DOS\\TEST.TXT") != 0)
        return 2;
    {
        int path_result = bx_ntvdm_dem_path_v1_resolve("FILE.TXT", &cwd, &drive, relative);
        if (path_result != BX_NTVDM_DEM_PATH_V1_OK || drive != 0u ||
            wcscmp(relative, L"FILE.TXT") != 0) {
            (void)path_result;
            return 3;
        }
    }
    if (bx_ntvdm_dem_path_v1_resolve("C:\\DOS\\..\\X.TXT", &cwd, &drive, relative) >= 1 ||
        bx_ntvdm_dem_path_v1_resolve("C:\\LONGFILENAME.TXT", &cwd, &drive, relative) >= 1)
        return 4;
    return 0;
}
