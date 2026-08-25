/////////////////////////////////////////////////////////////////////////
//
// Minimal native PIC composition.  This is Bochs-only assembly and not a
// mirror of the upstream PIC plugin lifecycle.
//
/////////////////////////////////////////////////////////////////////////

#ifndef ADAPTER_BOCHS_MINIMAL_PIC_H
#define ADAPTER_BOCHS_MINIMAL_PIC_H

class bx_pic_c;

bx_pic_c *minimal_pic_create(void);
void minimal_pic_destroy(bx_pic_c *pic);

#endif
