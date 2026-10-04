#ifndef RUN16_GUEST_ENVIRONMENT_H
#define RUN16_GUEST_ENVIRONMENT_H
#include <windows.h>

/* ANSI guest-record projection only. Input is a bounded MULTI_SZ; output is
 * owned by the caller (process heap), including a spare terminator byte.
 * Never modify the Unicode host worker environment or process environment. */
DWORD run16_guest_environment_root(PCSTR input,DWORD bytes,PCSTR root,
    PSTR *output,DWORD *output_bytes);
#endif
