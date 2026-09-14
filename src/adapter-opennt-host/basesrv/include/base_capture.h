/* Private local-heap binding for original BaseClient capture routines.
 * The heap and all captured pointers are local; none is a broker wire value.
 * Initialize before requests and destroy only after every capture is freed. */
#ifndef OPENNT_BASE_CAPTURE_H
#define OPENNT_BASE_CAPTURE_H
#include <nt.h>
#include <ntlpcapi.h>
#include <ntcsrmsg.h>
PCSR_CAPTURE_HEADER NTAPI CsrAllocateCaptureBuffer(ULONG, ULONG, ULONG);
ULONG NTAPI CsrAllocateMessagePointer(PCSR_CAPTURE_HEADER, ULONG, PVOID *);
VOID NTAPI CsrFreeCaptureBuffer(PCSR_CAPTURE_HEADER);
extern PVOID CsrPortHeap;
/* Original port-heap tags are metadata, not modern heap allocation flags. */
#define MAKE_CSRPORT_TAG(tag) 0
#define CAPTURE_TAG 0
#endif
