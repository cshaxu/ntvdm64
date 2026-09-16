#ifndef MVDM_BASE_VDM_ENVIRONMENT_H
#define MVDM_BASE_VDM_ENVIRONMENT_H

#include <windows.h>

typedef struct mvdm_base_vdm_environment {
    PWSTR original_environment;
    PSTR ansi_environment;
    PWSTR projected_environment;
    BOOL installed;
} mvdm_base_vdm_environment;

void mvdm_base_vdm_environment_initialize(mvdm_base_vdm_environment *environment);
BOOL mvdm_base_vdm_environment_prepare(mvdm_base_vdm_environment *environment);
void mvdm_base_vdm_environment_restore(mvdm_base_vdm_environment *environment);

#endif
