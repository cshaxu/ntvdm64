/* Copied observation facts, never DOS execution or completion commands. */
#ifndef COMMON_PROTOCOL_DOS_OBSERVATION_H
#define COMMON_PROTOCOL_DOS_OBSERVATION_H
#include <stdint.h>
#include <wchar.h>
#define DOS_OBSERVATION_ENTER 1u
#define DOS_OBSERVATION_EXIT 2u
#define DOS_OBSERVATION_GAP 3u
typedef struct common_dos_delivery {
    uint64_t direct;
} common_dos_delivery;
typedef struct common_dos_observation {
    uint64_t occurrence,parent,direct;
    uint32_t event,psp,parent_psp,flags;
    wchar_t image[260];
} common_dos_observation;
#endif
