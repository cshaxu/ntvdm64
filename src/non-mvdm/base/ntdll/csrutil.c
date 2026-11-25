/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    dllutil.c

Abstract:

    This module contains utility procedures for the Windows Client DLL


Author:

    Steve Wood (stevewo) 8-Oct-1990

Revision History:

--*/

/* DIVERGENCE(OPENNT-HOST-031): local capture heap; private CSR transport excluded. */
#include <nt.h>
#include <ntlpcapi.h>
#include <ntcsrmsg.h>
#include "compat/include/base_capture_heap.h"
#define MAKE_CSRPORT_TAG(tag) 0
#define CAPTURE_TAG 0
#line 244
PCSR_CAPTURE_HEADER
CsrAllocateCaptureBuffer(
    IN ULONG CountMessagePointers,
    IN ULONG Size
    )

/*++

Routine Description:

    This function allocates a buffer from the Port Memory section for
    use by the client in capture arguments into Port Memory.  In addition to
    specifying the size of the data that needs to be captured, the caller
    needs to specify how many pointers to captured data will be passed.
    Pointers can be located in either the request message itself, and/or
    the capture buffer.

Arguments:

    CountMessagePointers - Number of pointers within the request message
        that will point to locations within the allocated capture buffer.

    Size - Total size of the data that will be captured into the capture
        buffer.

Return Value:

    A pointer to the capture buffer header.

--*/

{
    PCSR_CAPTURE_HEADER CaptureBuffer;
    ULONG CountPointers;

    //
    // Calculate the total number of pointers that will be passed
    //

    CountPointers = CountMessagePointers;

    //
    // Calculate the total size of the capture buffer.  This includes the
    // header, the array of pointer offsets and the data length.  We round
    // the data length to a 32-bit boundary, assuming that each pointer
    // points to data whose length is not aligned on a 32-bit boundary.
    //

    if (Size >= MAXLONG) {
        //
        // Bail early if too big
        //
        return NULL;
        }
    Size += FIELD_OFFSET(CSR_CAPTURE_HEADER, MessagePointerOffsets) + (CountPointers * sizeof( PVOID ));
    Size = (Size + ((sizeof(PVOID)-1) * (CountPointers+1))) & ~(sizeof(PVOID)-1);

    //
    // Allocate the capture buffer from the Port Memory Heap.
    //

    CaptureBuffer = RtlAllocateHeap( CsrPortHeap, MAKE_CSRPORT_TAG( CAPTURE_TAG ), Size );
    if (CaptureBuffer == NULL) {

        //
        // FIX, FIX - need to attempt the receive lost reply messages to
        // to see if they contain CaptureBuffer pointers that can be freed.
        //

        return( NULL );
    }

    //
    // Initialize the capture buffer header
    //

    CaptureBuffer->Length = Size;
    CaptureBuffer->CountMessagePointers = 0;

    //
    // If there are pointers being passed then initialize the arrays of
    // pointer offsets to zero.  In either case set the free space pointer
    // in the capture buffer header to point to the first 32-bit aligned
    // location after the header, the arrays of pointer offsets are considered
    // part of the header.
    //

    RtlZeroMemory( CaptureBuffer->MessagePointerOffsets,
                   CountPointers * sizeof( ULONG_PTR )
                 );

    CaptureBuffer->FreeSpace = (PCHAR)
        (CaptureBuffer->MessagePointerOffsets + CountPointers);

    //
    // Returned the address of the capture buffer.
    //

    return( CaptureBuffer );
}

#line 346
VOID
CsrFreeCaptureBuffer(
    IN PCSR_CAPTURE_HEADER CaptureBuffer
    )

/*++

Routine Description:

    This function frees a capture buffer allocated by CsrAllocateCaptureBuffer.

Arguments:

    CaptureBuffer - Pointer to a capture buffer allocated by
        CsrAllocateCaptureBuffer.

Return Value:

    None.

--*/

{
    //
    // Free the capture buffer back to the Port Memory heap.
    //

    RtlFreeHeap( CsrPortHeap, 0, CaptureBuffer );
}

#line 377
ULONG
CsrAllocateMessagePointer(
    IN OUT PCSR_CAPTURE_HEADER CaptureBuffer,
    IN ULONG Length,
    OUT PVOID *Pointer
    )

/*++

Routine Description:

    This function allocates space from the capture buffer along with a
    pointer to point to it.  The pointer is presumed to be located in
    the request message structure.

Arguments:

    CaptureBuffer - Pointer to a capture buffer allocated by
        CsrAllocateCaptureBuffer.

    Length - Size of data being allocated from the capture buffer.

    Pointer - Address of the pointer within the request message that
        is to point to the space allocated out of the capture buffer.

Return Value:

    The actual length of the buffer allocated, after it has been rounded
    up to a multiple of 4.

--*/

{
    if (Length == 0) {
        *Pointer = NULL;
        Pointer = NULL;
        }

    else {

        //
        // Set the returned pointer value to point to the next free byte in
        // the capture buffer.
        //

        *Pointer = CaptureBuffer->FreeSpace;

        //
        // Round the length up to a multiple of 4
        //

        if (Length >= MAXLONG) {
            //
            // Bail early if too big
            //
            return 0;
            }

        Length = (Length + sizeof(PVOID)-1) & ~(sizeof(PVOID)-1);

        //
        // Update the free space pointer to point to the next available byte
        // in the capture buffer.
        //

        CaptureBuffer->FreeSpace += Length;
        }


    //
    // Remember the location of this pointer so that CsrClientCallServer can
    // convert it into a server pointer prior to sending the request to
    // the server.
    //

    CaptureBuffer->MessagePointerOffsets[ CaptureBuffer->CountMessagePointers++ ] =
        (ULONG_PTR)Pointer;

    //
    // Returned the actual length allocated.
    //

    return( Length );
}

#line 463
VOID
CsrCaptureMessageBuffer(
    IN OUT PCSR_CAPTURE_HEADER CaptureBuffer,
    IN PVOID Buffer OPTIONAL,
    IN ULONG Length,
    OUT PVOID *CapturedBuffer
    )

/*++

Routine Description:

    This function captures a buffer of bytes in an API request message.

Arguments:

    CaptureBuffer - Pointer to a capture buffer allocated by
        CsrAllocateCaptureBuffer.

    Buffer - Optional pointer to the buffer.  If this parameter is
        not present, then no data is copied into capture buffer.

    Length - Length of the buffer.

    CapturedBuffer - Pointer to the field in the message that will
        be filled in to point to the capture buffer.

Return Value:

    None.

--*/

{
    //
    // Set the length fields of the captured string structure and allocated
    // the Length for the string from the capture buffer.
    //

    CsrAllocateMessagePointer( CaptureBuffer,
                               Length,
                               CapturedBuffer
                             );

    //
    // If Buffer parameter is not present or the length of the data is zero,
    // return.
    //

    if (!ARGUMENT_PRESENT( Buffer ) || (Length == 0)) {
        return;
        }

    //
    // Copy the buffer data to the capture area.
    //

    RtlMoveMemory( *CapturedBuffer, Buffer, Length );

    return;
}

#line 525
VOID
CsrCaptureMessageString(
    IN OUT PCSR_CAPTURE_HEADER CaptureBuffer,
    IN PCSTR String OPTIONAL,
    IN ULONG Length,
    IN ULONG MaximumLength,
    OUT PSTRING CapturedString
    )

/*++

Routine Description:

    This function captures an ASCII string into a counted string data
    structure located in an API request message.

Arguments:

    CaptureBuffer - Pointer to a capture buffer allocated by
        CsrAllocateCaptureBuffer.

    String - Optional pointer to the ASCII string.  If this parameter is
        not present, then the counted string data structure is set to
        the null string.

    Length - Length of the ASCII string, ignored if String is NULL.

    MaximumLength - Maximum length of the string.  Different for null
        terminated strings, where Length does not include the null and
        MaximumLength does. This is always how much space is allocated
        from the capture buffer.

    CaptureString - Pointer to the counted string data structure that will
        be filled in to point to the captured ASCII string.

Return Value:

    None.

--*/

{
    ASSERT(CapturedString != NULL);
    //
    // If String parameter is not present, then set the captured string
    // to be the null string and returned.
    //

    if (!ARGUMENT_PRESENT( String )) {
        CapturedString->Length = 0;
        CapturedString->MaximumLength = (USHORT)MaximumLength;
        CsrAllocateMessagePointer( CaptureBuffer,
                                   MaximumLength,
                                   (PVOID *)&CapturedString->Buffer
                                 );
        //
        // Make it nul terminated if there is any room.
        //
        if (MaximumLength != 0) {
            CapturedString->Buffer[0] = 0;
            }
        return;
        }

    //
    // Set the length fields of the captured string structure and allocated
    // the MaximumLength for the string from the capture buffer.
    //

    CapturedString->Length = (USHORT)Length;
    CapturedString->MaximumLength = (USHORT)
        CsrAllocateMessagePointer( CaptureBuffer,
                                   MaximumLength,
                                   (PVOID *)&CapturedString->Buffer
                                 );
    //
    // If the Length of the ASCII string is non-zero then move it to the
    // capture area.
    //

    if (Length != 0) {
        RtlMoveMemory( CapturedString->Buffer, String, MaximumLength );
        }
    if (CapturedString->Length < CapturedString->MaximumLength) {
        CapturedString->Buffer[ CapturedString->Length ] = '\0';
        }

    return;
}

#line 741
PLARGE_INTEGER
CsrCaptureTimeout(
    IN ULONG MilliSeconds,
    OUT PLARGE_INTEGER Timeout
    )
{
    if (MilliSeconds == -1) {
        return( NULL );
        }
    else {
        Timeout->QuadPart = Int32x32To64( MilliSeconds, -10000 );
        return( (PLARGE_INTEGER)Timeout );
        }
}
