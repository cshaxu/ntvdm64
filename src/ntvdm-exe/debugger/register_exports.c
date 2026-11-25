/* XP's public x86 ABI is callee-pop. Internal SoftPC calls remain cdecl. */
#include <windows.h>

#define EXPORT_REGISTER_SETTER(name, type)         \
    extern void __cdecl name(type value);          \
    void WINAPI ntvdm_debug_##name(type value)     \
    {                                              \
        name(value);                               \
    }

EXPORT_REGISTER_SETTER(setAX, USHORT)
EXPORT_REGISTER_SETTER(setEAX, ULONG)
EXPORT_REGISTER_SETTER(setEBX, ULONG)
EXPORT_REGISTER_SETTER(setECX, ULONG)
EXPORT_REGISTER_SETTER(setEDX, ULONG)
EXPORT_REGISTER_SETTER(setESI, ULONG)
EXPORT_REGISTER_SETTER(setEDI, ULONG)
EXPORT_REGISTER_SETTER(setESP, ULONG)
EXPORT_REGISTER_SETTER(setEBP, ULONG)
EXPORT_REGISTER_SETTER(setEIP, ULONG)
EXPORT_REGISTER_SETTER(setEFLAGS, ULONG)

extern unsigned int __cdecl setES(USHORT value);

unsigned int WINAPI ntvdm_debug_setES(USHORT value)
{
    return setES(value);
}
