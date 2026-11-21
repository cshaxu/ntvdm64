#ifndef COMPAT_MVDM_VM_IO_H
#define COMPAT_MVDM_VM_IO_H
#include "compat/include/mvdm_vm_application.h"
#include "compat/include/mvdm_vm_input.h"
#include "compat/include/mvdm_vm_text.h"

typedef struct mvdm_vm_io mvdm_vm_io;
/* Borrow model/continuation until registry retirement joins all callbacks.
 * Title is copied ANSI handoff metadata, not a guest address. The caller owns
 * VM identity and registration; this provider performs no lifecycle RPC. */
DWORD mvdm_vm_io_create(ntvdm_vm_text *, mvdm_vm_continuation *, const char *, mvdm_vm_io **);
HANDLE mvdm_vm_io_ready(mvdm_vm_io *);
mvdm_vm_input *mvdm_vm_io_input(mvdm_vm_io *);
void mvdm_vm_io_destroy(mvdm_vm_io *); /* Only after callbacks/producers join. */
DWORD mvdm_vm_io_dispatch(mvdm_vm_io *, const HANDLE cancellation[4],
    const console_io_request *, console_io_reply *);
void mvdm_vm_io_finish(mvdm_vm_io *);
#endif
