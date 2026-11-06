/*++

Copyright (c) 1991 Microsoft Corporation

Module Name:

    ApiMsg.c

Abstract:

    Selected original XACTSRV local handler: NetMessageBufferSend.

--*/

#include "xactsrv_wksta_minimal.h"

#line 36
NTSTATUS
XsNetMessageBufferSend (
    API_HANDLER_PARAMETERS
    )
#line 58
{
    NET_API_STATUS status;

    PXS_NET_MESSAGE_BUFFER_SEND parameters = Parameters;
    LPTSTR nativeRecipient = NULL;          // Native parameters
    LPBYTE nativeBuffer = NULL;
    DWORD nativeBufLen;

    API_HANDLER_PARAMETERS_REFERENCE;       // Avoid warnings

#line 79
    XsConvertTextParameter(
        nativeRecipient,
        (LPSTR)SmbGetUlong( &parameters->Recipient )
        );

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

#line 129
cleanup:

    NetpMemoryFree( nativeRecipient );

#line 133
#ifdef UNICODE
    NetpMemoryFree( nativeBuffer );
#endif // def UNICODE
#line 136

#line 141
    Header->Status = (WORD)status;

    return STATUS_SUCCESS;

} // XsNetMessageBufferSend
