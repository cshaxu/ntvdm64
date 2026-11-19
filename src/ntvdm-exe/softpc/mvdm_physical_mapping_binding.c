#include "compat/include/physical_mapping_services.h"
#include "mvdm_softpc_physical_mapping.h"

BOOL ntvdm_physical_mapping_initialize(void)
{
    static const compat_physical_mapping_services services = {
        mvdm_softpc_physical_mapping_initialize,
        mvdm_softpc_physical_mapping_release,
        VdmSetPhysRecStructs,
        VdmMapDosMemory,
        VdmUnmapDosMemory,
        mvdm_softpc_physical_mapping_translate,
        mvdm_softpc_physical_mapping_resolve,
        mvdm_softpc_physical_mapping_span_is_aliased
    };
    return compat_physical_mapping_install(&services);
}
