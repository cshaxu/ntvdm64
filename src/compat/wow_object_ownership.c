/* Copyright (c) 1985-95, Microsoft Corporation.
 * Derived from OpenNT windows/core/ntuser/kernel/handtabl.c, lines 39-59,
 * revision 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * T483 S6 P23 owner-approved extraction of the selected non-IME ownership
 * table. Original values and object type/flag definitions are unchanged.
 */
#include <windows.h>
#include "compat/include/opennt_user_multipass.h"
#define WOW_USER_OBJECT_FLAGS_ONLY
#include "non-mvdm/windows/core/ntuser/kernel/userk.h"

CONST BYTE gabObjectCreateFlags[TYPE_CTYPES] = {
    0,                                                  /* free */
    OCF_THREADOWNED | OCF_MARKTHREAD | OCF_USEQUOTA,    /* window */
    OCF_PROCESSOWNED,                                   /* menu */
    OCF_PROCESSOWNED | OCF_USEQUOTA,                    /* cursor/icon */
    OCF_THREADOWNED | OCF_USEQUOTA,                     /* hswpi (SetWindowPos Information) */
    OCF_THREADOWNED | OCF_MARKTHREAD,                   /* hook */
    OCF_THREADOWNED | OCF_USEQUOTA,                     /* thread info object (internal) */
    0,                                                  /* clipboard data (internal) */
    OCF_THREADOWNED,                                    /* CALLPROCDATA */
    OCF_PROCESSOWNED | OCF_USEQUOTA,                    /* accel table */
    OCF_THREADOWNED | OCF_USEQUOTA,                     /* dde access */
    OCF_THREADOWNED | OCF_MARKTHREAD | OCF_USEQUOTA,    /* dde conversation */
    OCF_THREADOWNED | OCF_MARKTHREAD | OCF_USEQUOTA,    /* ddex */
    OCF_PROCESSOWNED,                                   /* zombie */
    OCF_PROCESSOWNED,                                   /* keyboard layout */
    OCF_PROCESSOWNED,                                   /* keyboard file */
};
