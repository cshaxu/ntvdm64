/*++

Copyright (c) 1991 Microsoft Corporation

Module Name:

    ApiMsg.c

Abstract:

    Selected original XACTSRV local handler: NetMessageBufferSend.

--*/

#include "xactsrv_wksta_minimal.h"

NTSTATUS
XsNetMessageBufferSend (
    API_HANDLER_PARAMETERS
    )
{
    NET_API_STATUS status;

    PXS_NET_MESSAGE_BUFFER_SEND parameters = Parameters;
    LPTSTR nativeRecipient = NULL;
    LPBYTE nativeBuffer = NULL;
    DWORD nativeBufLen;

    API_HANDLER_PARAMETERS_REFERENCE;

    XsConvertTextParameter(
        nativeRecipient,
        (LPSTR)SmbGetUlong( &parameters->Recipient )
        );

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

    nativeBuffer = (LPBYTE)SmbGetUlong( &parameters->Buffer );
    nativeBufLen = (DWORD)SmbGetUshort( &parameters->BufLen );

#endif // def UNICODE

    status = NetMessageBufferSend(
                 NULL,
                 nativeRecipient,
                 NULL,
                 nativeBuffer,
                 nativeBufLen
                 );

cleanup:

    NetpMemoryFree( nativeRecipient );

#ifdef UNICODE
    NetpMemoryFree( nativeBuffer );
#endif // def UNICODE

    Header->Status = (WORD)status;

    return STATUS_SUCCESS;

} // XsNetMessageBufferSend
