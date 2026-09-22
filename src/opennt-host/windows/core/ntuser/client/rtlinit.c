/****************************** Module Header ******************************\
* Module Name: rtlinit.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* This module contains all the init code for the USERRTL.DLL.  When the DLL is
* dynlinked its initialization procedure (UserRtlDllInitialize) is called by
* the loader.
*
* History:
* 14-Jan-1991 mikeke
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-040): retain original RtlCaptureAnsiString.
 * Other USER initialization/capture routines and private precompiled headers
 * are excluded. The existing worker TLS/heap boundary supplies resources. */
#include "wow_user_capture_bindings.h"

BOOL RtlCaptureAnsiString(
    PIN_STRING pstr,
    LPCSTR psz,
    BOOL fForceAlloc)
{
    int cbSrc;
    int cbDst;

    pstr->fAllocated = FALSE;
    if (psz) {
        cbSrc = strlen(psz) + 1;
        if (cbSrc > MAXUSHORT) {
            RIPMSG0(RIP_WARNING, "String too long for standard string");
            return FALSE;
        }

        /*
         * If the allocation is forced or if the string is
         * too long to fit in the TEB, allocate a buffer.
         * Otherwise, store the result in the TEB.
         */
        if (fForceAlloc ||
                cbSrc > (STATIC_UNICODE_BUFFER_LENGTH / sizeof(WCHAR))) {
            pstr->strCapture.Buffer = RtlAllocateHeap(RtlProcessHeap(),
                    0, cbSrc * sizeof(WCHAR));
            if (pstr->strCapture.Buffer == NULL)
                return FALSE;
            pstr->fAllocated = TRUE;
            pstr->pstr = &pstr->strCapture;
            pstr->strCapture.MaximumLength = (USHORT)(cbSrc * sizeof(WCHAR));
        } else {
            pstr->pstr = &NtCurrentTeb()->StaticUnicodeString;
        }

        /*
         * Convert the string to Unicode
         */
        if (RtlMultiByteToUnicodeN(pstr->pstr->Buffer,
                (ULONG)pstr->pstr->MaximumLength, &cbDst,
                (LPSTR)psz, cbSrc)) {
            RIPMSG0(RIP_WARNING, "Unicode conversion failed");
            if (pstr->fAllocated) {
                RtlFreeHeap(RtlProcessHeap(), 0, pstr->strCapture.Buffer);
                pstr->fAllocated = FALSE;
            }
            return FALSE;
        }
        pstr->pstr->Length = (USHORT)cbDst - sizeof(WCHAR);
    } else {
        pstr->pstr = &pstr->strCapture;
        pstr->strCapture.Length = pstr->strCapture.MaximumLength = 0;
        pstr->strCapture.Buffer = NULL;
    }
    return TRUE;
}

