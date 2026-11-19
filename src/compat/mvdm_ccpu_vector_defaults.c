/* DIVERGENCE(MVDM-HOST-DIV-127): unavailable fallback providers retain the
 * source-proven fail-fast behavior. */
#include <windows.h>
#include <intrin.h>

/* Selected OpenNT softpc.new/host/inc/insignia.h:64,72,78 scalar ABI. */
typedef unsigned char IU8;
typedef unsigned long IU32;
typedef unsigned int IUH;

VOID EDL_fast_bop(ULONG immed)
{
    __debugbreak();
}

IU32 c_VirtualiseInstruction(IU32 eipInRom, IUH size,
                             IU32 linearAddrOrPort, IU32 dataIn)
{
    __debugbreak();
    return 0;
}

IU8 *c_sas_touch(IU32 addr, IU32 length)
{
    __debugbreak();
    return NULL;
}
