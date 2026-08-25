#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "nt_uis.h"
#include "host_rrr.h"
/* DIVERGENCE(HOST-DIV-012,HOST-DIV-014): private same-shaped NT4 product bindings. */
#include "opennt-host-overlay/softpc.new/host/src/nt_error_bindings.h"
/*
 * SoftPC Revision 2.0
 *
 * Title	: General Error Handler
 *
 * Description	: General purpose error handler.  It handles both
 *		  general SoftPC errors (error numbers 0 - 999) and
 *		  host specific errors (error numbers >= 1000)
 *
 * Author(s)	: Dave Bartlett (based on module by John Shanly)
 *
 * Parameters	: int used to index an array of error messages
 *		  held in message.c, and a bit mask indicating
 *		  the user's possible options:
 *                    Quit, Reset, Continue, Setup
 *
 */

DWORD TlsDirectError;
//
// Called directly from C or via bop. Type checked against global 'DirectError'
// to see if called already in this app. 'DirectError' cleared on VDM resume.
//
// This function is expected to be called by 16 bit threads
// which is doing the unsupported service. For DosApps this is
// the CPU thread, For WOW this is one of the individual 16 bit tasks.
//
//
VOID host_direct_access_error(ULONG type)
{
    CHAR message[EHS_MSG_LEN];
    CHAR acctype[EHS_MSG_LEN];
    CHAR dames[EHS_MSG_LEN];
    DWORD dwDirectError;


       /*
        *  Get the direct error record for the current thread
        *  if TlsGetValue returns NULL
        *     - could be invalid index (TlsAlloc failed)
        *     - actual value is 0, (no bits set)
        *  In both cases we will go ahead with the popup
        */
    /* DIVERGENCE(HOST-DIV-026): the original 32-bit TLS payload cast is
     * pointer-width unsafe on x64.  The stored value remains the original
     * DWORD category bit mask; only the transport cast is widened. */
    dwDirectError = (DWORD)(ULONG_PTR)TlsGetValue(TlsDirectError);

       // don't annoy user with repeated popups
    /* DIVERGENCE(HOST-DIV-013): the original expression has undefined shift
     * behavior for a guest-provided category >= 32. Historical 0..6 values
     * retain the exact test/write ordering; other values remain observable. */
    if (type < 32u && (dwDirectError & (1 << type)) != 0)
        return;

    if (type < 32u)
        TlsSetValue(TlsDirectError,
                    (LPVOID)(ULONG_PTR)(dwDirectError | (1 << type)));

    if (LoadString(GetModuleHandle(NULL), D_A_MESS,
                   dames, sizeof(dames)/sizeof(CHAR)) &&
        LoadString(GetModuleHandle(NULL), D_A_MESS + type + 1,
                   acctype, sizeof(acctype)/sizeof(CHAR))     )
       {
        sprintf(message, dames, acctype);
        }
    else {
        strcpy(message, szDoomMsg);
        }


    ErrorDialogBox(message, NULL, RMB_ICON_STOP | RMB_ABORT | RMB_IGNORE);
}
