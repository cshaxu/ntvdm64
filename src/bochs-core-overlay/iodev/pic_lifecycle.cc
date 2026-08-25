/////////////////////////////////////////////////////////////////////////
//
// Private finite-machine lifecycle overlay for the upstream Bochs PIC.
// It exports no public component ABI and contains only machine mechanics.
// BX-MACH-024 supplies the body reached through bx_pic_c::fini().
//
/////////////////////////////////////////////////////////////////////////

#include "iodev.h"
#include "bochs-core/iodev/pic.h"

bx_bool bochs_core_overlay_pic_fini(bx_pic_c *pic)
{
  bx_bool ok = 1;
  ok = bx_devices.unregister_io_read_handler(pic, bx_pic_c::read_handler, 0x0020, 1) && ok;
  ok = bx_devices.unregister_io_read_handler(pic, bx_pic_c::read_handler, 0x0021, 1) && ok;
  ok = bx_devices.unregister_io_read_handler(pic, bx_pic_c::read_handler, 0x00A0, 1) && ok;
  ok = bx_devices.unregister_io_read_handler(pic, bx_pic_c::read_handler, 0x00A1, 1) && ok;
  ok = bx_devices.unregister_io_write_handler(pic, bx_pic_c::write_handler, 0x0020, 1) && ok;
  ok = bx_devices.unregister_io_write_handler(pic, bx_pic_c::write_handler, 0x0021, 1) && ok;
  ok = bx_devices.unregister_io_write_handler(pic, bx_pic_c::write_handler, 0x00A0, 1) && ok;
  ok = bx_devices.unregister_io_write_handler(pic, bx_pic_c::write_handler, 0x00A1, 1) && ok;
  return ok;
}
