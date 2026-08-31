#ifndef APP_LAUNCH_DECLARATION_H
#define APP_LAUNCH_DECLARATION_H

#include <stdint.h>

#include "base_vdm_local.h"

typedef struct app_launch_declaration {
    base_vdm_local base_vdm;
    uint32_t bound;
    uint32_t ordinary_child;
    char command[MAXIMUM_VDM_COMMAND_LENGTH];
    char application[MAX_PATH];
    char environment[MAXIMUM_VDM_ENVIORNMENT];
    char current_directory[MAXIMUM_VDM_PATH_STRING];
} app_launch_declaration;

void app_launch_declaration_initialize(app_launch_declaration *declaration);
int app_launch_declaration_consume_options(app_launch_declaration *declaration,
    int *argc, char **argv);
int app_launch_declaration_bind(app_launch_declaration *declaration,
    session *owner);
int app_launch_declaration_publish(app_launch_declaration *declaration,
    const session *owner);

#endif
