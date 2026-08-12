/*
 * DPMI32 was built against the native NT user-mode declaration umbrella.
 * Use the preserved OpenNT order so TEB/VDM declarations stay coherent with
 * each other. This header supplies declarations only and no host behavior.
 */
#ifndef NTDOS64_OPENNT_DPMI32_NATIVE_PREAMBLE_H
#define NTDOS64_OPENNT_DPMI32_NATIVE_PREAMBLE_H

#include "../../public/sdk/inc/nt.h"
#include "../../public/sdk/inc/ntrtl.h"
#include "../../public/sdk/inc/nturtl.h"

/* Avoid the generic staging wrappers reintroducing a modern Windows umbrella
 * after the historical OpenNT NT declarations are established. */
#define OPENNT_STAGE_NT_H
#define OPENNT_STAGE_NTRTL_H
#define OPENNT_STAGE_NTURTL_H

#endif
