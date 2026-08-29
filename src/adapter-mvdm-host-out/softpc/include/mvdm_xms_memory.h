#ifndef MVDM_XMS_MEMORY_H
#define MVDM_XMS_MEMORY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Resolve the original XMS move descriptor at SS:BP and perform its
 * source-ordered guest-memory transfer through the current session.
 * The arguments and values remain Intel numeric addresses; no host pointer
 * is published to XMS source.
 */
int mvdm_xms_move_block(uint16_t stack_segment, uint16_t stack_offset);

#ifdef __cplusplus
}
#endif

#endif
