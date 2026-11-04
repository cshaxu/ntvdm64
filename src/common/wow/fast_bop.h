#ifndef COMMON_WOW_FAST_BOP_H
#define COMMON_WOW_FAST_BOP_H
#include <stdint.h>
/* Guest-visible offsets into the worker's existing FastBOP carrier. */
uint32_t mvdm_softpc_fast_bop_dispatch_offset(void);
uint32_t mvdm_softpc_fast_bop_callback_offset(void);
#endif
