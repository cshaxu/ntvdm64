/*++

Copyright (c) 1991-1992  Microsoft Corporation

Module Name:

    ApiSrv.c

Abstract:

    This module contains individual API handlers for the NetServer APIs.

    SUPPORTED : NetServerDiskEnum, NetServerEnum2, NetServerGetInfo,
                NetServerSetInfo.

    SEE ALSO : NetServerAuthenticate, NetServerPasswordSet,
               NetServerReqChallenge - in ApiLogon.c.

Author:

    Shanku Niyogi (w-shanku) 25-Feb-1991

Revision History:

--*/

/* DIVERGENCE(OPENNT-HOST-296): use the existing local NetAPI declarations
 * and the private-browser import boundary, not the NT4 server aggregate. */
#include "xactsrv_wksta_minimal.h"
#include "vdmredir-dll/include/remote_browser.h"


//
// Declaration of descriptor strings.
//

STATIC const LPDESC Desc16_server_info_0 = REM16_server_info_0;
STATIC const LPDESC Desc32_server_info_0 = REM32_server_info_0;
STATIC const LPDESC Desc16_server_info_1 = REM16_server_info_1;
STATIC const LPDESC Desc32_server_info_1 = REM32_server_info_1;


#line 171
NTSTATUS
XsNetServerEnum2 (
    API_HANDLER_PARAMETERS
    )

/*++

Routine Description:

    This routine handles a call to NetServerEnum.

Arguments:

    API_HANDLER_PARAMETERS - information about the API call. See
        XsTypes.h for details.

    Transport - The name of the transport provided to the API.

Return Value:

    NTSTATUS - STATUS_SUCCESS or reason for failure.

--*/

{
    NET_API_STATUS status = NERR_Success;

    PXS_NET_SERVER_ENUM_2 parameters = Parameters;
    LPTSTR nativeDomain = NULL;             // Native parameters
    LPVOID outBuffer= NULL;
    DWORD entriesRead;
    DWORD totalEntries;
    LPTSTR clientTransportName = NULL;
    LPTSTR clientName = NULL;

    /* DIVERGENCE(OPENNT-HOST-296): failure cleanup still passes this value
     * to XsSetDataCount when native enumeration was not reached. */
    DWORD entriesFilled = 0;                // Conversion variables

    API_HANDLER_PARAMETERS_REFERENCE;       // Avoid warnings
    /* DIVERGENCE(OPENNT-HOST-296): no packed response exists on early failure. */
    Header->Converter = 0;

    IF_DEBUG(SERVER) {
        NetpKdPrint(( "XsNetServerEnum2: header at %lx, params at %lx, "
                      "level %ld, buf size %ld\n",
                      Header, parameters, SmbGetUshort( &parameters->Level ),
                      SmbGetUshort( &parameters->BufLen )));
    }

    //
    // Translate parameters, check for errors.
    //

    if ( XsWordParamOutOfRange( parameters->Level, 0, 1 )) {

        Header->Status = ERROR_INVALID_LEVEL;
        goto cleanup;
    }

    XsConvertTextParameter(
        nativeDomain,
        (LPSTR)SmbGetUlong( &parameters->Domain )
        );

    clientTransportName = Header->ClientTransportName;

    clientName = Header->ClientMachineName;

    //
    // Get the actual server information from the local 32-bit call. The
    // native level is 100 or 101.
    //

    if (clientTransportName == NULL) {
        status = NetServerEnum(
                        NULL,
                        100 + (DWORD)SmbGetUshort( &parameters->Level ),
                        (LPBYTE *)&outBuffer,
                        XsNativeBufferSize( SmbGetUshort( &parameters->BufLen )),
                        &entriesRead,
                        &totalEntries,
                        SmbGetUlong( &parameters->ServerType ),
                        nativeDomain,
                        NULL
                        );

        if ( !XsApiSuccess( status ) ) {
            IF_DEBUG(API_ERRORS) {
                NetpKdPrint(( "XsNetServerEnum2: NetServerEnum failed: %X\n",
                              status ));
            }
            Header->Status = (WORD)status;
            goto cleanup;
        }

        Header->Status = XsConvertServerEnumBuffer(
                              outBuffer,
                              entriesRead,
                              &totalEntries,
                              SmbGetUshort( &parameters->Level ),
                              (LPVOID)SmbGetUlong( &parameters->Buffer ),
                              SmbGetUshort( &parameters->BufLen ),
                              &entriesFilled,
                              &Header->Converter);

    } else {

        Header->Status = I_BrowserServerEnumForXactsrv(
                 clientTransportName,
                 clientName,

                 100 + SmbGetUshort( &parameters->Level ),
                 SmbGetUshort( &parameters->Level ),

                 (PVOID)SmbGetUlong( &parameters->Buffer ),
                 SmbGetUshort( &parameters->BufLen ),
                 XsNativeBufferSize( SmbGetUshort( &parameters->BufLen ) ),

                 &entriesFilled,
                 &totalEntries,

                 SmbGetUlong( &parameters->ServerType ),
                 nativeDomain,
                 NULL,

                 &Header->Converter

                 );


        if (!XsApiSuccess( Header->Status )) {
            IF_DEBUG(API_ERRORS) {
                NetpKdPrint(( "XsNetServerEnum2: I_BrowserServerEnum failed: %d\n", Header->Status));
            }
            goto cleanup;
        }
    }

    if ( entriesFilled == 0 ) {
        SmbPutUshort( &parameters->BufLen, 0 );
    }

    //
    // Set up the response parameters.
    //

    SmbPutUshort( &parameters->EntriesRead, (WORD)entriesFilled );
    SmbPutUshort( &parameters->TotalAvail, (WORD)totalEntries );

cleanup:

    if ( outBuffer != NULL ) {
        NetApiBufferFree( outBuffer );
    }
    NetpMemoryFree( nativeDomain );

    //
    // Determine return buffer size.
    //

    XsSetDataCount(
        &parameters->BufLen,
        SmbGetUshort( &parameters->Level ) == 0 ?
                    Desc16_server_info_0 :
                    Desc16_server_info_1,
        Header->Converter,
        entriesFilled,
        Header->Status
        );

    return STATUS_SUCCESS;

} // XsNetServerEnum2

#line 522
USHORT
XsConvertServerEnumBuffer(
    IN LPVOID ServerEnumBuffer,
    IN DWORD EntriesRead,
    IN OUT PDWORD TotalEntries,
    IN USHORT Level,
    IN LPBYTE ClientBuffer,
    IN USHORT BufferLength,
    OUT PDWORD EntriesFilled,
    OUT PUSHORT Converter
    )
/*++

Routine Description:

    This routine converts an NT server info array into a down level RAP
    server info buffer.

Arguments:

    IN LPVOID ServerEnumBuffer - Buffer with NT server info.
    IN DWORD EntriesRead -       Number of entries in buffer.
    IN OUT PDWORD TotalEntries - Total Number of entries.
    IN USHORT Level - Downlevel information Level (0 or 1).
    IN LPBYTE ClientBuffer - Pointer to 16 bit client side buffer.
    IN USHORT BufferLength - Size of client buffer.
    OUT PDWORD EntriesFilled - Number of entries converted into client buffer.
    OUT PUSHORT Converter - Converter used by client side to convert back.

Return Value:

    USHORT - NERR_Success or reason for failure (16 bit DOS error).

--*/

{
    USHORT status = NERR_Success;
    DWORD invalidEntries;
    LPDESC nativeStructureDesc;
    DWORD bytesRequired = 0;
    PCHAR StructureDesc;

    IF_DEBUG(SERVER) {
        NetpKdPrint(( "XsConvertServerEnumBuffer: received %ld entries at %lx\n",
                      EntriesRead, ServerEnumBuffer ));
    }

    //
    // Use the requested level to determine the format of the
    // data structure.
    //

    switch ( Level ) {

    case 0:

        StructureDesc = Desc16_server_info_0;
        nativeStructureDesc = Desc32_server_info_0;
        break;

    case 1:

        StructureDesc = Desc16_server_info_1;
        nativeStructureDesc = Desc32_server_info_1;
        break;

    }

    //
    // Do the actual conversion from the 32-bit structures to 16-bit
    // structures.
    //

    XsFillEnumBuffer(
        ServerEnumBuffer,
        EntriesRead,
        nativeStructureDesc,
        ClientBuffer,
        ClientBuffer,
        BufferLength,
        StructureDesc,
        NULL,  // verify function
        &bytesRequired,
        EntriesFilled,
        &invalidEntries
        );

    IF_DEBUG(SERVER) {
        NetpKdPrint(( "32-bit data at %lx, 16-bit data at %lx, %ld BR,"
                      " Entries %ld of %ld\n",
                      ServerEnumBuffer, ClientBuffer ,
                      bytesRequired, *EntriesFilled, *TotalEntries ));
    }

    //
    // If there are any invalid entries, subtract this from the
    // number of total entries to avoid the case where the client
    // keeps bugging us for more data.
    //

    if ( invalidEntries > 0) {
        (*TotalEntries) -= invalidEntries;
#if DBG
#error OPENNT-HOST-296: DBG is outside the selected release profile.
#endif
#line 631
    }

    //
    // If all the entries could not be filled, return ERROR_MORE_DATA,
    // The data needs to be packed so that we don't send too much
    // useless data.
    //

    if ( (*EntriesFilled < *TotalEntries) ||
         (bytesRequired > BufferLength) ) {

        status = ERROR_MORE_DATA;
    }

    *Converter = XsPackReturnData(
                            ClientBuffer,
                            BufferLength,
                            StructureDesc,
                            *EntriesFilled
                            );


    return status;
}   //  XsConvertServerEnumBuffer
