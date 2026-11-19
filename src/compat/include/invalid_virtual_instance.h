#ifndef COMPAT_INVALID_VIRTUAL_INSTANCE_H
#define COMPAT_INVALID_VIRTUAL_INSTANCE_H

/* A stale NIDDB identity cannot safely select a device instance. */
__declspec(noreturn) void mvdm_softpc_invalid_virtual_instance(unsigned long id);

#endif
