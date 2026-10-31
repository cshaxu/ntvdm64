/*++

Copyright (c) 1991-1992  Microsoft Corporation

Module Name:

    ApiBuff.c

Abstract:

    This selected true subset retains the one historical internal buffer
    entry directly reached by the admitted NetLib allocation unit.

--*/

// DIVERGENCE(OPENNT-HOST-013): true subset of original
// ds/netapi/api/apibuff.c.  The omitted public buffer APIs require the wider
// original RPC/MIDL product shell and are not reached by the selected
// Redirector/NetLib closure.  NetapipBufferAllocate below is copied unchanged
// and preserves its original public-NetAPI failure contract.

#include <windef.h>
#include <lmcons.h>
#include <lmapibuf.h>

NET_API_STATUS NET_API_FUNCTION
NetapipBufferAllocate (
    IN DWORD ByteCount,
    OUT LPVOID * Buffer
    )

/*++

Routine Description:

    NetapipBufferAllocate is an old internal function that allocates buffers
    which the APIs will return to the application.  All calls to this routine
    should eventually be replaced by calls to NetApiBufferAllocate.

Arguments:

    (Same as NetApiBufferAllocate.)

Return Value:

    (Same as NetApiBufferAllocate.)

--*/

{
    return (NetApiBufferAllocate( ByteCount, Buffer ));

} // NetapipBufferAllocate
