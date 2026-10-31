/****************************** Module Header ******************************\
* Module Name: handtabl.c
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* Implements the USER handle table.
*
* 01-13-92 ScottLu      Created.
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-063): unchanged object ownership table; native
 * USER remains the allocator. Do not allocate a second handle namespace. */
#include <windows.h>
#include "../inc/user.h"
#define WOW_USER_OBJECT_FLAGS_ONLY
#include "userk.h"

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
#ifdef FE_IME
    OCF_THREADOWNED | OCF_MARKTHREAD,                   /* input context */
#endif
};

