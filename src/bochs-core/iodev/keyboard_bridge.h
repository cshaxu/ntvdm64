/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-MACH-027): narrow Bochs-only headless lifecycle boundary.
/////////////////////////////////////////////////////////////////////////

#ifndef BOCHS_CORE_KEYBOARD_BRIDGE_H
#define BOCHS_CORE_KEYBOARD_BRIDGE_H

bx_bool bochs_core_keyboard_headless_create(void);
bx_bool bochs_core_keyboard_headless_destroy(void);

#endif
