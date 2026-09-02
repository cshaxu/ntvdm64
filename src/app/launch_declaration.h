#ifndef APP_LAUNCH_DECLARATION_H
#define APP_LAUNCH_DECLARATION_H

#include <stdint.h>

#include "base_vdm_broker.h"

typedef struct app_launch_declaration {
    base_vdm_local base_vdm;
    base_vdm_broker broker;
    uint32_t bound;
    uint32_t command_declared;
    char requested_command[MAXIMUM_VDM_COMMAND_LENGTH];
    char command[MAXIMUM_VDM_COMMAND_LENGTH];
    char application[MAX_PATH];
    char environment[MAXIMUM_VDM_ENVIORNMENT];
    /* The original BaseClient current-directory carrier is a host path, not a
     * guest drive-relative component.  It must match the MAX_PATH + 1 buffer
     * advertised by original COMMAND's cmdGetNextCmd. */
    char current_directory[MAX_PATH + 1u];
} app_launch_declaration;

void app_launch_declaration_initialize(app_launch_declaration *declaration);
int app_launch_declaration_consume_options(app_launch_declaration *declaration,
    int *argc, char **argv);
int app_launch_declaration_bind(app_launch_declaration *declaration,
    session *owner);
int app_launch_declaration_publish(app_launch_declaration *declaration,
    const session *owner);

#endif
