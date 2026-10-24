#ifndef RUN16_LAUNCH_OPTIONS_H
#define RUN16_LAUNCH_OPTIONS_H

#include <wchar.h>

/* Launcher-only prefix. The target command remains an untouched input slice. */
typedef struct run16_launch_options {
    const wchar_t *command;
    int wait;
} run16_launch_options;

int run16_parse_launch_options(const wchar_t *command, run16_launch_options *options);

#endif
