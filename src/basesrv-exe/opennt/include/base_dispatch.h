#ifndef OPENNT_BASE_DISPATCH_H
#define OPENNT_BASE_DISPATCH_H
#include "basesrv-exe/transport/vdm_message.h"

/* Local materialized message only. The transport must authenticate, validate
 * all variable payloads/resources and bind the registered caller first.
 * This is not a wire decoder or an authentication API. */
NTSTATUS OpenNtBaseDispatch(PCSR_API_MSG, CSR_API_NUMBER, ULONG);
uint32_t OpenNtBaseVdmOperation(CSR_API_NUMBER);
NTSTATUS OpenNtBaseDispatchOperation(PCSR_API_MSG, uint32_t, ULONG);

#endif
