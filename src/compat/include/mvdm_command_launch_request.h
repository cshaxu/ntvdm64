#ifndef COMPAT_MVDM_COMMAND_LAUNCH_REQUEST_H
#define COMPAT_MVDM_COMMAND_LAUNCH_REQUEST_H

#include "mvdm_command_launch.h"

/* Owned by compat from prepare to destroy; the product alone claims start. */
struct ntvdm_command_launch {
    char *command,*environment,*directory;
    HANDLE streams[3];
    unsigned int service;
    DWORD console_mask;
    LONG started;
};

#endif
