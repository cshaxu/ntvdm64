/////////////////////////////////////////////////////////////////////////
//
// Minimal native PIC composition.  It intentionally avoids Bochs plugin
// discovery and registry ownership; all behaviour remains machine-local.
//
/////////////////////////////////////////////////////////////////////////

#include "iodev.h"
#include "bochs-core/iodev/pic.h"
#include "adapter-bochs/minimal_pic.h"

extern bx_pic_c *thePic;

bx_pic_c *minimal_pic_create(void)
{
  if (thePic != NULL) return NULL;
  thePic = new bx_pic_c();
  return thePic;
}

void minimal_pic_destroy(bx_pic_c *pic)
{
  if (pic != thePic) return;
  delete thePic;
  thePic = NULL;
}
