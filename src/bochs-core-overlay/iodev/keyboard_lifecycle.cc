/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-MACH-027): private non-product 8042 lifecycle body.
/////////////////////////////////////////////////////////////////////////

#include "iodev.h"
#include "bochs-core/iodev/keyboard.h"

extern bx_keyb_c *theKeyboard;

bx_bool bochs_core_overlay_keyboard_headless_destroy(void)
{
  if (theKeyboard == NULL || bx_devices.pluginKeyboard != theKeyboard) return 0;
  if (!theKeyboard->fini()) return 0;
  delete theKeyboard;
  theKeyboard = NULL;
  bx_devices.pluginKeyboard = &bx_devices.stubKeyboard;
  return 1;
}

bx_bool bx_keyb_c::fini(void)
{
  bx_bool ok = 1;
  if (BX_KEY_THIS timer_handle != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer((unsigned) BX_KEY_THIS timer_handle);
    ok = bx_pc_system.unregisterTimer((unsigned) BX_KEY_THIS timer_handle) && ok;
    BX_KEY_THIS timer_handle = BX_NULL_TIMER_HANDLE;
  }
  ok = bx_devices.unregister_io_read_handler(this, read_handler, 0x0060, 1) && ok;
  ok = bx_devices.unregister_io_read_handler(this, read_handler, 0x0064, 1) && ok;
  ok = bx_devices.unregister_io_write_handler(this, write_handler, 0x0060, 1) && ok;
  ok = bx_devices.unregister_io_write_handler(this, write_handler, 0x0064, 1) && ok;
  return ok;
}
