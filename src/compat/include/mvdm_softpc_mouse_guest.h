#ifndef MVDM_SOFTPC_MOUSE_GUEST_H
#define MVDM_SOFTPC_MOUSE_GUEST_H
#include "mvdm_softpc_mouse_bridge.h"
int16_t mvdm_mouse_coordinate_delta(int16_t position,int32_t delta);
DWORD mvdm_softpc_mouse_receive(const INPUT_RECORD *);
#endif
