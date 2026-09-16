#ifndef OPENNT_BASE_VALUES_H
#define OPENNT_BASE_VALUES_H
#include "basesrv/transport/vdm_message.h"
#include "basesrv/transport/vdm_values.h"
/* BASE_API_MSG must be declared. Exact operation-selected size is required.
 * Only scalar fields change. Handles, borrowed buffers and startup storage
 * must be bound separately. Decode validates all narrowing before mutation;
 * service policy still validates the meaning of original scalar values. */
BOOL OpenNtBaseEncodeValues(const BASE_API_MSG *, uint32_t, void *, uint32_t);
BOOL OpenNtBaseDecodeValues(const void *, uint32_t, uint32_t, PBASE_API_MSG);
#endif
