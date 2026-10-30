#include <windows.h>
#include <stdio.h>
#include "wow_registry_binding.h"

static unsigned opens, closes;
static HKEY closed[4];
LONG WINAPI fixture_open(HKEY root, LPCSTR name, PHKEY out) {
    if (root != HKEY_CLASSES_ROOT || !name || !out) return ERROR_INVALID_PARAMETER;
    *out = (HKEY)(ULONG_PTR)(0x1000 + ++opens);
    return ERROR_SUCCESS;
}
LONG WINAPI fixture_close(HKEY key) {
    if (closes >= 4) return ERROR_INVALID_PARAMETER;
    closed[closes++] = key;
    return ERROR_SUCCESS;
}
LONG WINAPI fixture_unused(void) { return ERROR_CALL_NOT_IMPLEMENTED; }
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(void) {
    HKEY a = NULL, b = NULL, c = NULL;
    CHECK(wow_registry_open_key(HKEY_CLASSES_ROOT,"a",&a) == ERROR_SUCCESS);
    CHECK(wow_registry_open_key(HKEY_CLASSES_ROOT,"b",&b) == ERROR_SUCCESS);
    CHECK(a && b && a != b && opens == 2 && closes == 0);
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
