#include <windows.h>
#include <stdio.h>
#include "compat/include/mvdm_shadow_registry.h"
#include "compat/include/wow_registry_binding.h"

static unsigned opens, closes;
static HKEY closed[4];
LONG WINAPI ntvdm_shadow_reg_open_key(HKEY root, LPCSTR name, PHKEY out) {
    if (root != HKEY_CLASSES_ROOT || !name || !out) return ERROR_INVALID_PARAMETER;
    *out = (HKEY)(ULONG_PTR)(0x1000 + ++opens);
    return ERROR_SUCCESS;
}
LONG WINAPI ntvdm_shadow_reg_close_key(HKEY key) {
    if (closes >= 4) return ERROR_INVALID_PARAMETER;
    closed[closes++] = key;
    return ERROR_SUCCESS;
}
LONG WINAPI ntvdm_shadow_reg_create_key(HKEY root, LPCSTR name, PHKEY out) {
    return ntvdm_shadow_reg_open_key(root,name,out);
}
LONG WINAPI ntvdm_shadow_reg_delete_key(HKEY key, LPCSTR subkey)
{
    (void)key; (void)subkey;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

LONG WINAPI ntvdm_shadow_reg_set_value(HKEY key, LPCSTR subkey, DWORD type,
    LPCSTR data, DWORD data_size)
{
    (void)key; (void)subkey; (void)type; (void)data; (void)data_size;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

LONG WINAPI ntvdm_shadow_reg_set_value_ex(HKEY key, LPCSTR value, DWORD reserved,
    DWORD type, const BYTE *data, DWORD data_size)
{
    (void)key; (void)value; (void)reserved; (void)type; (void)data; (void)data_size;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

LONG WINAPI ntvdm_shadow_reg_query_value(HKEY key, LPCSTR subkey, LPSTR data,
    PLONG data_size)
{
    (void)key; (void)subkey; (void)data; (void)data_size;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

LONG WINAPI ntvdm_shadow_reg_query_value_ex(HKEY key, LPCSTR value, LPDWORD reserved,
    LPDWORD type, LPBYTE data, LPDWORD data_size)
{
    (void)key; (void)value; (void)reserved; (void)type; (void)data; (void)data_size;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

LONG WINAPI ntvdm_shadow_reg_enum_key(HKEY key, DWORD index, LPSTR name,
    DWORD name_size)
{
    (void)key; (void)index; (void)name; (void)name_size;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

LONG WINAPI ntvdm_shadow_reg_open_key_ex(HKEY root, LPCSTR subkey, DWORD options,
    REGSAM access, PHKEY result)
{
    (void)root; (void)subkey; (void)options; (void)access; (void)result;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

LONG WINAPI ntvdm_shadow_reg_query_info_key(HKEY key, LPTSTR class_name,
    LPDWORD class_length, LPDWORD reserved, LPDWORD subkeys,
    LPDWORD maximum_subkey_length, LPDWORD maximum_class_length,
    LPDWORD values, LPDWORD maximum_value_name_length,
    LPDWORD maximum_value_data_length, LPDWORD security_descriptor,
    PFILETIME last_write_time)
{
    (void)key; (void)class_name; (void)class_length; (void)reserved; (void)subkeys; (void)maximum_subkey_length; (void)maximum_class_length; (void)values; (void)maximum_value_name_length; (void)maximum_value_data_length; (void)security_descriptor; (void)last_write_time;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

LONG WINAPI ntvdm_shadow_reg_enum_value(HKEY key, DWORD index, LPSTR value_name,
    LPDWORD value_name_size, LPDWORD reserved, LPDWORD type, LPBYTE data,
    LPDWORD data_size)
{
    (void)key; (void)index; (void)value_name; (void)value_name_size; (void)reserved; (void)type; (void)data; (void)data_size;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(void) {
    HKEY a = NULL, b = NULL, c = NULL;
    CHECK(wow_registry_open_key(HKEY_CLASSES_ROOT,"a",&a) == ERROR_SUCCESS);
    CHECK(wow_registry_open_key(HKEY_CLASSES_ROOT,"b",&b) == ERROR_SUCCESS);
    CHECK(a && b && a != b && opens == 2 && closes == 0);
    CHECK(a != (HKEY)(ULONG_PTR)1 && b != (HKEY)(ULONG_PTR)1);
    CHECK(wow_registry_close_key(a) == ERROR_SUCCESS);
    CHECK(closes == 1 && closed[0] == (HKEY)(ULONG_PTR)0x1001);
    wow_registry_binding_shutdown();
    CHECK(closes == 2 && closed[1] == (HKEY)(ULONG_PTR)0x1002);
    CHECK(wow_registry_close_key(a) == ERROR_INVALID_HANDLE);
    CHECK(wow_registry_close_key(b) == ERROR_INVALID_HANDLE);
    wow_registry_binding_shutdown();
    CHECK(closes == 2);
    CHECK(wow_registry_open_key(HKEY_CLASSES_ROOT,"c",&c) == ERROR_SUCCESS);
    CHECK(c && c != a && c != b);
    wow_registry_binding_shutdown();
    CHECK(closes == 3 && closed[2] == (HKEY)(ULONG_PTR)0x1003);
    puts("WOW_REGISTRY_POPULATED_SHUTDOWN_OK");
    return 0;
}
