/*++

Copyright (c) 1991-1992  Microsoft Corporation

Module Name:

    ApiBuff.c

Abstract:

    This module contains routines for allocating and freeing API buffers.

    BUGBUG: expand description; mention RPCability, one end-user API and
    one internal.

Author:

    John Rogers (JohnRo) 25-Jan-1991

Environment:

    Portable to any flat, 32-bit environment.  (Uses Win32 typedefs.)
    Requires ANSI C extensions: slash-slash comments, long external names.

Notes:

    Parts of the commentary for this file are extracted from material written
    by Alec Barker (AlecB@Microsoft).

Revision History:

    15-Mar-91 JohnRo
        Use <netdebug.h> and netlib routines.
    25-Apr-1991 JohnRo
        Call MIDL_user_allocate and MIDL_user_free.  Delete tabs.
    03-Dec-1991 JohnRo
        Added public NetApiBufferAllocate, NetApiBufferReallocate, and
        NetApiBufferSize APIs.
        Make sure buffers are aligned for worst case use.
    10-May-1992 JohnRo
        Treat alloc and realloc of size zero as non-error (return NULL ptr).
        Use <prefix.h> equates.
        Include my own prototypes so compiler can check them.
    18-May-1992 JohnRo
        RAID 9258: return non-null pointer when allocating zero bytes.

--*/
#line 48

// DIVERGENCE(OPENNT-HOST-013): true subset of original
// ds/netapi/api/apibuff.c.  The omitted public buffer APIs require the wider
// original RPC/MIDL product shell and are not reached by the selected
// Redirector/NetLib closure.  NetapipBufferAllocate below is copied unchanged
// and preserves its original public-NetAPI failure contract.
#line 50

#include <windef.h>
#include <lmcons.h>
#include <lmapibuf.h>

#line 231
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
