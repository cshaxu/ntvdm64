/*
 * Overlay reconstruction: preserve the original SoftPC header and replace
 * only its x86 V86 low-address GetVDMAddr assumption for the CCPU/SAS profile.
 */
#include_next "softpc.h"

#ifdef NTDOS64_CCPU_SAS_GETVDMADDR
#undef GetVDMAddr
extern unsigned char *c_GetLinAdd(unsigned long lin_addr);
#define GetVDMAddr(usSeg, usOff) \
    c_GetLinAdd((((unsigned long)(usSeg) << 4) + (unsigned long)(usOff)))
#endif
