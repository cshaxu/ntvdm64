#include <windows.h>

/* The source's final i386 host-LDT install is unavailable until a later
 * package can prove a Bochs-native descriptor-publication contract.  Keeping
 * this original imported symbol explicit preserves DpmiSetDescriptorEntry's
 * failure branch without pretending that a Windows host LDT exists. */
BOOL DpmiSetX86Descriptor(void *descriptors, USHORT selector, USHORT count)
{
    (void)descriptors;
    (void)selector;
    (void)count;
    return FALSE;
}
