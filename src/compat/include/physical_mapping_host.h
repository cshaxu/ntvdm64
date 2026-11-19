#ifndef COMPAT_PHYSICAL_MAPPING_HOST_H
#define COMPAT_PHYSICAL_MAPPING_HOST_H
#include "physical_mapping_services.h"

#define mvdm_softpc_physical_mapping_initialize (compat_physical_mapping_current_services()->initialize)
#define mvdm_softpc_physical_mapping_release (compat_physical_mapping_current_services()->release)
#define VdmSetPhysRecStructs (compat_physical_mapping_current_services()->set_records)
#define VdmMapDosMemory (compat_physical_mapping_current_services()->map)
#define VdmUnmapDosMemory (compat_physical_mapping_current_services()->unmap)
#define mvdm_softpc_physical_mapping_translate (compat_physical_mapping_current_services()->translate)
#define mvdm_softpc_physical_mapping_resolve (compat_physical_mapping_current_services()->resolve)
#define mvdm_softpc_physical_mapping_span_is_aliased (compat_physical_mapping_current_services()->span_is_aliased)
#endif
