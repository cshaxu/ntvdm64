#include "mvdm_softpc_vdd_configuration.h"

enum mvdm_softpc_vdd_configuration
mvdm_softpc_open_installable_vdd_registry(HKEY *key_out)
{
    HKEY key;
    LONG status;

    if (key_out == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return MVDM_SOFTPC_VDD_CONFIGURATION_ERROR;
    }
    *key_out = NULL;
    status = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Control\\VirtualDeviceDrivers", 0,
        KEY_QUERY_VALUE, &key);
    if (status == ERROR_SUCCESS) {
        *key_out = key;
        return MVDM_SOFTPC_VDD_CONFIGURATION_PRESENT;
    }
    if (status == ERROR_FILE_NOT_FOUND || status == ERROR_PATH_NOT_FOUND)
        return MVDM_SOFTPC_VDD_CONFIGURATION_NONE;
    SetLastError((DWORD)status);
    return MVDM_SOFTPC_VDD_CONFIGURATION_ERROR;
}
