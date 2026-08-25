/*
 * True source subset: OpenNT base/mvdm/softpc.new/host/src/nt_bop.c,
 * MS_bop_4. The remaining historical selector bodies are not COMMAND inputs.
 */

#include "nt_bop_command.h"
#include "opennt_command_composition.h"

/* DIVERGENCE(BOP-DIV-099): the original CCPU SAS closure is unavailable as a
 * standalone modern component. These same-shaped, call-scoped facades retain
 * the original byte load and CS:IP sequence; the outer typed result remains
 * the only way to resume the Bochs CPU. */
typedef UCHAR half_word;
#define getCS() runtime_command_misc_get_cs()
#define getIP() runtime_command_misc_get_ip()
#define setIP(value) runtime_command_misc_set_ip((USHORT)(value))
#define sas_load(address, target) runtime_command_misc_sas_load((address), (target))
#define CmdDispatch(service) runtime_command_misc_dispatch_source_command((service))
#define CmdDispatch(service) runtime_command_misc_dispatch_source_command((service))

// SCS BOP
/* CMD dispatcher, this BOP will only work in real mode */
void MS_bop_4(void)
{
    half_word Command;

    sas_load( ((ULONG)getCS()<<4) + getIP(), &Command);
    CmdDispatch((ULONG) Command);
    setIP((USHORT)(getIP() + 1));
}
