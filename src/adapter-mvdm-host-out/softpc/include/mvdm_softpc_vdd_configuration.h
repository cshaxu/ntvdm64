#ifndef MVDM_SOFTPC_VDD_CONFIGURATION_H
#define MVDM_SOFTPC_VDD_CONFIGURATION_H

#include <windows.h>

enum mvdm_softpc_vdd_configuration {
    MVDM_SOFTPC_VDD_CONFIGURATION_ERROR = 0,
    MVDM_SOFTPC_VDD_CONFIGURATION_NONE = 1,
    MVDM_SOFTPC_VDD_CONFIGURATION_PRESENT = 2
};

/* Preserve the original VDD registry source and HKEY calling shape. A modern
 * system without the retired NT4 product registration has no VDD package
 * configured for this session; other registry failures remain errors. */
enum mvdm_softpc_vdd_configuration
mvdm_softpc_open_installable_vdd_registry(HKEY *key_out);

#endif
