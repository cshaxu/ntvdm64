// Native Bochs 8042 composition.  T244 S2 keeps the object private to the
// mantle; it receives neither guest tables nor host input.

#include "bochs.h"
#include "iodev/iodev.h"
#include "keyboard.h"
#include "adapter-bochs/headless_8042.h"

extern bx_keyb_c *theKeyboard;

bx_bool bx_mantle_headless_8042_create_v1(void)
{
  if (theKeyboard != NULL || bx_devices.pluginKeyboard != &bx_devices.stubKeyboard)
    return 0;
  theKeyboard = new bx_keyb_c();
  if (theKeyboard == NULL) return 0;
  bx_devices.pluginKeyboard = theKeyboard;
  theKeyboard->init();
  return 1;
}

bx_bool bx_mantle_headless_8042_destroy_v1(void)
{
  if (theKeyboard == NULL || bx_devices.pluginKeyboard != theKeyboard) return 0;
  if (!theKeyboard->fini()) return 0;
  delete theKeyboard;
  theKeyboard = NULL;
  bx_devices.pluginKeyboard = &bx_devices.stubKeyboard;
  return 1;
}
