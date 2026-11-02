/*++

Copyright (c) 1991-92  Microsoft Corporation

Module Name:

    packstr.c

Abstract:

    Contains utilities for allocating and/or packing buffers that contain
    a fixed section and a variable (string) section.  The following
    functions are available:

        NetpPackString
        NetpCopyStringToBuffer
        NetpCopyDataToBuffer
        NetpAllocateEnumBuffer

Author:

    various

Environment:

    User Mode -Win32

Revision History:

    30-Apr-1991     danl
        NetpAllocateEnumBuffer:  Removed use of NetApiBufferFree.  It has
        been changed to use MIDL_user_allocate and MIDL_user_free.
        Also added NT-style headers where needed.
    16-Apr-1991 JohnRo
        Clarify UNICODE handling of pack and copy routines.  Got rid of tabs
        in source file.
    21-Nov-1991 JohnRo
        Removed NT dependencies to reduce recompiles.
    15-Apr-1992 JohnRo
        FORMAT_POINTER is obsolete.

--*/

// These must be included first:

#include <windef.h>     // IN, OUT, etc.
#include <lmcons.h>     // Needed by <netlib.h>.
#include <lmerr.h>      // NERR_*
#include <rpc.h>        // MIDL_user_allocate & MIDL_user_free

// These may be included in any order:

#include <align.h>      // ROUND_UP_COUNT().
#include <debuglib.h>   // IF_DEBUG().
#include <netdebug.h>   // NetpKdPrint(), FORMAT_ equates.
#include <netlib.h>     // My prototype.
#include <tstring.h>    // STRCPY(), STRLEN(), STRNCPY().


#if !defined(MVDM_XACTSRV_PACKSTR_COPY_ONLY)
#error Unsupported excluded original configuration
#endif
#line 158


BOOL
NetpCopyStringToBuffer (
    IN LPTSTR String OPTIONAL,
    IN DWORD CharacterCount,
    IN LPBYTE FixedDataEnd,
    IN OUT LPTSTR *EndOfVariableData,
    OUT LPTSTR *VariableDataPointer
    )

/*++

Routine Description:

    This routine puts a single variable-length string into an output buffer.
    The string is not written if it would overwrite the last fixed structure
    in the buffer.

    The code is swiped from svcsupp.c written by DavidTr.

    Sample usage:

            LPBYTE FixedDataEnd = OutputBuffer + sizeof(WKSTA_INFO_202);
            LPTSTR EndOfVariableData = OutputBuffer + OutputBufferSize;

            //
            // Copy user name
            //

            NetpCopyStringToBuffer(
                UserInfo->UserName.Buffer;
                UserInfo->UserName.Length;
                FixedDataEnd,
                &EndOfVariableData,
                &WkstaInfo->wki202_username
                );

Arguments:

    String - Supplies a pointer to the source string to copy into the
        output buffer.  If String is null then a pointer to a zero terminator
        is inserted into output buffer.

    CharacterCount - Supplies the length of String, not including zero
        terminator.

    FixedDataEnd - Supplies a pointer to just after the end of the last
        fixed structure in the buffer.

    EndOfVariableData - Supplies an address to a pointer to just after the
        last position in the output buffer that variable data can occupy.
        Returns a pointer to the string written in the output buffer.

    VariableDataPointer - Supplies a pointer to the place in the fixed
        portion of the output buffer where a pointer to the variable data
        should be written.

Return Value:

    Returns TRUE if string fits into output buffer, FALSE otherwise.

--*/
{
    DWORD BytesNeeded = (CharacterCount + 1) * sizeof(TCHAR);

    IF_DEBUG(PACKSTR) {
        NetpKdPrint(("NetpStringToBuffer: String at " FORMAT_LPVOID
                ", CharacterCount=" FORMAT_DWORD
                ",\n  FixedDataEnd at " FORMAT_LPVOID
                ", *EndOfVariableData at " FORMAT_LPVOID
                ",\n  VariableDataPointer at " FORMAT_LPVOID
                ", BytesNeeded=" FORMAT_DWORD ".\n",
                (LPVOID) String, CharacterCount, FixedDataEnd,
                (LPVOID) *EndOfVariableData,
                (LPVOID) VariableDataPointer, BytesNeeded));
    }

    //
    // Determine if string will fit, allowing for a zero terminator.  If no,
    // just set the pointer to NULL.
    //

    if ((*EndOfVariableData - (CharacterCount+1)) >= (LPTSTR)FixedDataEnd) {

        //
        // It fits.  Move EndOfVariableData pointer up to the location where
        // we will write the string.
        //

        *EndOfVariableData -= (CharacterCount+1);

        //
        // Copy the string to the buffer if it is not null.
        //

        if (CharacterCount > 0 && String != NULL) {

            STRNCPY(*EndOfVariableData, String, CharacterCount);
        }

        //
        // Set the zero terminator.
        //

        *(*EndOfVariableData + CharacterCount) = TCHAR_EOS;

        //
        // Set up the pointer in the fixed data portion to point to where the
        // string is written.
        //

        *VariableDataPointer = *EndOfVariableData;

        return TRUE;

    }
    else {

        //
        // It doesn't fit.  Set the offset to NULL.
        //

        *VariableDataPointer = NULL;

        return FALSE;
    }
} // NetpCopyStringToBuffer


#if !defined(MVDM_XACTSRV_PACKSTR_COPY_ONLY)
#error Unsupported excluded original configuration
#endif
#line 411


#if !defined(MVDM_XACTSRV_PACKSTR_COPY_ONLY)
#error Unsupported excluded original configuration
#endif
#line 675
