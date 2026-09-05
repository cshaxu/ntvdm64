/* Same-shaped Win32 binding for the selected original SoftPC media contract.
 * ROM and default configuration bytes are immutable RCDATA resources in the
 * product image.  The historical mutable resource write path is represented
 * by a per-user registry value, never an adjacent deployment file. */

#include <windows.h>
#include <string.h>

#define MVDM_SOFTPC_MEDIA_KEY "Software\\NTVDM64\\SoftPC\\Media"

static const char *resource_name(const char *name)
{
    return strcmp(name, ".spcprofile") == 0 ? "profile.spc" : name;
}

static long read_registry_value(const char *name, unsigned char *address, int maximum)
{
    HKEY key;
    DWORD type = 0;
    DWORD size = (DWORD)maximum;
    LONG status;

    status = RegOpenKeyExA(HKEY_CURRENT_USER, MVDM_SOFTPC_MEDIA_KEY, 0, KEY_QUERY_VALUE, &key);
    if (status != ERROR_SUCCESS) return 0;
    status = RegQueryValueExA(key, name, NULL, &type, address, &size);
    RegCloseKey(key);
    if (status != ERROR_SUCCESS || type != REG_BINARY || size > (DWORD)maximum) return 0;
    return (long)size;
}

long mvdm_embedded_resource_read(int type, char *name, unsigned char *address, int maximum, int display_error)
{
    HMODULE module;
    HRSRC resource;
    HGLOBAL loaded;
    const void *bytes;
    DWORD size;
    long persisted;

    (void)display_error;
    if (name == NULL || address == NULL || maximum < 0) return 0;

    name = (char *)resource_name(name);
    if (type == 0) {
        persisted = read_registry_value(name, address, maximum);
        if (persisted != 0) return persisted;
    }

    module = GetModuleHandleA(NULL);
    resource = FindResourceA(module, name, RT_RCDATA);
    if (resource == NULL) return 0;
    size = SizeofResource(module, resource);
    if (size > (DWORD)maximum) return 0;
    loaded = LoadResource(module, resource);
    bytes = loaded == NULL ? NULL : LockResource(loaded);
    if (bytes == NULL) return 0;
    memcpy(address, bytes, size);
    return (long)size;
}

int mvdm_embedded_resource_write(int type, char *name, unsigned char *address, long size)
{
    HKEY key;
    DWORD byte_count;
    LONG status;

    if (name == NULL || address == NULL || size < 0 || (unsigned long)size > MAXDWORD) return 0;
    if (type != 0) return 0;
    name = (char *)resource_name(name);
    byte_count = (DWORD)size;
    status = RegCreateKeyExA(HKEY_CURRENT_USER, MVDM_SOFTPC_MEDIA_KEY, 0, NULL, 0,
                             KEY_SET_VALUE, NULL, &key, NULL);
    if (status != ERROR_SUCCESS) return 0;
    status = RegSetValueExA(key, name, 0, REG_BINARY, address, byte_count);
    RegCloseKey(key);
    return status == ERROR_SUCCESS;
}
