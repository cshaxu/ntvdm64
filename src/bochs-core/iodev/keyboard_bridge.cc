/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-MACH-027): narrow core-owned bridge to the private headless
// keyboard lifecycle overlay.
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "iodev/iodev.h"
#include "bochs-core/iodev/keyboard.h"
#include "bochs-core/iodev/keyboard_bridge.h"

extern bx_bool bochs_core_overlay_keyboard_headless_create(void);
extern bx_bool bochs_core_overlay_keyboard_headless_destroy(void);

bx_bool bochs_core_keyboard_headless_create(void)
{
  return bochs_core_overlay_keyboard_headless_create();
}

bx_bool bochs_core_keyboard_headless_destroy(void)
{
  return bochs_core_overlay_keyboard_headless_destroy();
}
