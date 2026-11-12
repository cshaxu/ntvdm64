/*++

Copyright (c) 1991 Microsoft Corporation

Module Name:

    ApiMsg.c

Abstract:

    This module contains individual API handlers for the NetMessage APIs.

    SUPPORTED - NetMessageBufferSend, NetMessageNameAdd, NetMessageNameDel,
                NetMessageNameEnum, NetMessageNameGetInfo.

Author:

    Shanku Niyogi (w-shanku)    8-Mar-1991

Revision History:

--*/

#include "xactsrv_wksta_minimal.h"

#line 36
NTSTATUS
XsNetMessageBufferSend (
    API_HANDLER_PARAMETERS
    )

/*++

Routine Description:

    This routine handles a call to NetMessageBufferSend.

Arguments:

    API_HANDLER_PARAMETERS - information about the API call. See
        XsTypes.h for details.

Return Value:

    NTSTATUS - STATUS_SUCCESS or reason for failure.

--*/

#line 58
{
    NET_API_STATUS status;

    PXS_NET_MESSAGE_BUFFER_SEND parameters = Parameters;
    LPTSTR nativeRecipient = NULL;          // Native parameters
    LPBYTE nativeBuffer = NULL;
    DWORD nativeBufLen;

    API_HANDLER_PARAMETERS_REFERENCE;       // Avoid warnings

    IF_DEBUG(MESSAGE) {
        NetpKdPrint(( "XsNetMessageBufferSend: header at %lx, params at %lx, "
                      "recipient %s\n",
                      Header, parameters,
                      SmbGetUlong( &parameters->Recipient )));
    }

    //
    // Translate parameters, check for errors.
    //

#line 79
    XsConvertTextParameter(
        nativeRecipient,
        (LPSTR)SmbGetUlong( &parameters->Recipient )
        );

    //
    // NetMessageBufferSend has an ASCII data buffer. Convert this to
    // Unicode if necessary. BUGBUG - Make sure BufLen is semantically correct.
    //

#line 89
#ifdef UNICODE

    nativeBufLen = SmbGetUshort( &parameters->BufLen ) * sizeof(WCHAR);

    if (( nativeBuffer = NetpMemoryAllocate( nativeBufLen )) == NULL ) {

        status = NERR_NoRoom;
        goto cleanup;

    } else {

        XsCopyBufToTBuf(
            (LPBYTE)nativeBuffer,
            (LPBYTE)SmbGetUlong( &parameters->Buffer ),
            (DWORD)SmbGetUshort( &parameters->BufLen )
            );

    }

#else
#line 108
#error Unsupported excluded original configuration
#endif // def UNICODE
#line 113

    status = NetMessageBufferSend(
                 NULL,
                 nativeRecipient,
                 NULL,
                 nativeBuffer,
                 nativeBufLen
                 );

#line 121

    if ( !XsApiSuccess( status )) {
        IF_DEBUG(ERRORS) {
            NetpKdPrint(( "XsNetMessageBufferSend: NetMessageBufferSend "
                          "failed: %X\n", status ));
        }
    }

#line 129
cleanup:

    NetpMemoryFree( nativeRecipient );

#line 133
#ifdef UNICODE
    NetpMemoryFree( nativeBuffer );
#endif // def UNICODE
#line 136

    //
    // Nothing to return.
    //

#line 141
    Header->Status = (WORD)status;

    return STATUS_SUCCESS;

} // XsNetMessageBufferSend
