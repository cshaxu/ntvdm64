#include "include/mvdm_firmware.h"
#include <windows.h>
#include <string.h>

long mvdm_softpc_firmware_read_embedded_rom(const char *name, void *bytes_out,
    uint32_t bytes_out_capacity)
{
    const char *resource_name;
    HMODULE module;
    HRSRC resource;
    HGLOBAL loaded;
    DWORD size;
    const void *bytes;

    if (name == NULL || bytes_out == NULL) return 0;
    if (strcmp(name, "bios1.rom") == 0) resource_name = "SOFTPC_BIOS1";
    else if (strcmp(name, "bios4.rom") == 0) resource_name = "SOFTPC_BIOS4";
    else if (strcmp(name, "v7vga.rom") == 0) resource_name = "SOFTPC_V7VGA";
    else return 0;
    module = GetModuleHandleA(NULL);
    resource = module == NULL ? NULL : FindResourceA(module, resource_name,
        RT_RCDATA);
    if (resource == NULL) return 0;
    size = SizeofResource(module, resource);
    if (size == 0u || size > bytes_out_capacity) return 0;
    loaded = LoadResource(module, resource);
    bytes = loaded == NULL ? NULL : LockResource(loaded);
    if (bytes == NULL) return 0;
    memcpy(bytes_out, bytes, size);
    return (long)size;
}
