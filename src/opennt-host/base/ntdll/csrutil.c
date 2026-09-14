/* Copyright (c) 1989  Microsoft Corporation
 * DIVERGENCE(OPENNT-HOST-031): true subset containing only the three local
 * capture-memory routines required by BaseClient. Port connection, LPC/CSR
 * transport, pointer rebasing and every other runtime function are excluded.
 * The owning process supplies a local capture heap; no shared port section. */
#include <base_capture.h>

PCSR_CAPTURE_HEADER
CsrAllocateCaptureBuffer(
    IN ULONG CountMessagePointers,
    IN ULONG CountCapturePointers,
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

    CountCapturePointers - Number of pointers within the capture buffer
        that will point to other locations within the allocated capture
        buffer.

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

    CountPointers = CountMessagePointers + CountCapturePointers;

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
    Size += sizeof( CSR_CAPTURE_HEADER ) + (CountPointers * sizeof( PVOID ));
    Size = (Size + (3 * (CountPointers+1))) & ~3;

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
    CaptureBuffer->CountCapturePointers = 0;

    //
    // If there are pointers being passed then initialize the arrays of
    // pointer offsets to zero.  In either case set the free space pointer
    // in the capture buffer header to point to the first 32-bit aligned
    // location after the header, the arrays of pointer offsets are considered
    // part of the header.
    //

    if (CountPointers != 0) {
        CaptureBuffer->MessagePointerOffsets = (PULONG)(CaptureBuffer + 1);

        CaptureBuffer->CapturePointerOffsets =
            CaptureBuffer->MessagePointerOffsets + CountMessagePointers;

        RtlZeroMemory( CaptureBuffer->MessagePointerOffsets,
                       CountPointers * sizeof( ULONG )
                     );

        CaptureBuffer->FreeSpace = (PCHAR)
            (CaptureBuffer->CapturePointerOffsets + CountCapturePointers);
        }
    else {
        CaptureBuffer->MessagePointerOffsets = NULL;
        CaptureBuffer->CapturePointerOffsets = NULL;
        CaptureBuffer->FreeSpace = (PCHAR)(CaptureBuffer + 1);
        }

    //
    // Returned the address of the capture buffer.
    //

    return( CaptureBuffer );
}


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

        Length = (Length + 3) & ~3;

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
        (ULONG)Pointer;

    //
    // Returned the actual length allocated.
    //

    return( Length );
}
