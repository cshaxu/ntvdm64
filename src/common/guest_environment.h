#ifndef COMMON_GUEST_ENVIRONMENT_H
#define COMMON_GUEST_ENVIRONMENT_H
#include <windows.h>

/* ANSI guest task/boot-block projection only. Input is a bounded MULTI_SZ; output is
 * owned by the caller (process heap), including a spare terminator byte.
 * SYSTEMROOT is a directory prefix without a trailing separator because the
 * original Win16 kernel appends its own \SYSTEM suffix; WIN16DIR is retained.
 * Never modify the Unicode host worker environment or process environment. */
DWORD common_guest_environment_root(PCSTR input,DWORD bytes,PCSTR root,
    PSTR *output,DWORD *output_bytes);
#endif
