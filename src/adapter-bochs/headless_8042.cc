// Native Bochs 8042 composition.  T244 S2 keeps the object private to the
// mantle; it receives neither guest tables nor host input.

#include "bochs.h"
#include "iodev/iodev.h"
#include "adapter-bochs/headless_8042.h"

extern int libkeyboard_LTX_plugin_init(plugin_t *plugin, plugintype_t type,
  int argc, char *argv[]);
extern void libkeyboard_LTX_plugin_fini(void);

bx_bool bx_mantle_headless_8042_create_v1(void)
{
  return libkeyboard_LTX_plugin_init(0, (plugintype_t)0, 0, 0) == 0;
}

bx_bool bx_mantle_headless_8042_destroy_v1(void)
{
  libkeyboard_LTX_plugin_fini();
  return bx_devices.pluginKeyboard == &bx_devices.stubKeyboard;
}
