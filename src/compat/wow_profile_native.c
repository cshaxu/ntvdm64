#include "include/wow_profile_native.h"

/* ADAPTER-WOW-042: original PMAP_COMPAT is machine-wide, never impersonated
 * HKCU configuration. Select the native machine view explicitly on WOW64;
 * do not create the absent key or synthesize compatibility values. */
HANDLE WINAPI wow_profile_open_key(PUNICODE_STRING user, UINT section, ACCESS_MASK access, PDWORD policy)
{
    HKEY key=NULL;
    /* Use the original XP section identifiers shared by the migrated callers.
     * No impersonation, policy search or write access belongs to this provider. */
    if (policy) {
        DWORD requested = *policy;
        *policy = 0;
        if (requested) return NULL;
    }
    if(user || access!=KEY_READ) return NULL;
    if(section==10) {
        if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Compatibility",
            0,KEY_READ|KEY_WOW64_64KEY,&key)!=ERROR_SUCCESS) return NULL;
    } else if(section==4) {
        /* Original PMAP_DESKTOP, used only by recovered EDIT preferences.
         * This worker executes as its user; no impersonation/cache is added. */
        if(RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop",
            0,KEY_READ,&key)!=ERROR_SUCCESS) return NULL;
    } else if(section==31) {
        if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"Software\\Microsoft\\Windows NT\\CurrentVersion\\IME Compatibility",
            0,KEY_READ|KEY_WOW64_64KEY,&key)!=ERROR_SUCCESS) return NULL;
    } else return NULL;
    return key;
}

