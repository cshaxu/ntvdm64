#ifndef MVDM_SOFTPC_VDD_CONFIGURATION_H
#define MVDM_SOFTPC_VDD_CONFIGURATION_H

#include <windows.h>

enum mvdm_softpc_vdd_configuration {
    MVDM_SOFTPC_VDD_CONFIGURATION_ERROR = 0,
    MVDM_SOFTPC_VDD_CONFIGURATION_NONE = 1,
    MVDM_SOFTPC_VDD_CONFIGURATION_PRESENT = 2
};

/* Product policy: never access the system registry.  NTVDM.REG beside the
 * worker is the sole configuration source; the HKEY argument remains only to
 * keep the original SetupInstallableVDD caller shape narrow. */
enum mvdm_softpc_vdd_configuration
mvdm_softpc_open_installable_vdd_registry(HKEY *key_out);

/* Reads the `VDD` REG_MULTI_SZ value from the package-local NTVDM.REG shadow.
 * The caller owns the malloc buffer on success. */
BOOL mvdm_softpc_read_installable_vdd_shadow(char **value_out);

/* The worker loads its immutable package-local shadow at startup.  Absence is
 * valid and means that no optional product configuration is selected. */
BOOL ntvdm_shadow_registry_initialize(VOID);
VOID ntvdm_shadow_registry_shutdown(VOID);

/* Same-shaped bounded registry ABI for the one original VDD caller.  These
 * never receive or return a Windows registry handle. */
LONG WINAPI mvdm_shadow_vdd_query_info_key(HKEY, LPTSTR, LPDWORD, LPDWORD,
    LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PFILETIME);
LONG WINAPI mvdm_shadow_vdd_query_value_ex(HKEY, LPCTSTR, LPDWORD, LPDWORD,
    LPBYTE, LPDWORD);
LONG WINAPI mvdm_shadow_vdd_close_key(HKEY);

#undef RegQueryInfoKey
#undef RegQueryValueEx
#undef RegCloseKey
#define RegQueryInfoKey mvdm_shadow_vdd_query_info_key
#define RegQueryValueEx mvdm_shadow_vdd_query_value_ex
#define RegCloseKey mvdm_shadow_vdd_close_key

#endif
