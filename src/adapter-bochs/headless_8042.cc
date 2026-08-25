// Native Bochs 8042 composition.  T244 S2 keeps the object private to the
// mantle; it receives neither guest tables nor host input.

#include "bochs.h"
#include "iodev/iodev.h"
#include "bochs-core/iodev/keyboard_bridge.h"
#include "adapter-bochs/headless_8042.h"

bx_bool bx_mantle_headless_8042_create_v1(void)
{
  return bochs_core_keyboard_headless_create();
}

bx_bool bx_mantle_headless_8042_destroy_v1(void)
{
  return bochs_core_keyboard_headless_destroy();
}
