/**************************************************************************\
* Module Name: ntstubs.c
*
* Copyright (c) Microsoft Corp. 1990 All Rights Reserved
*
* client side API stubs
*
* History:
* 03-19-95 JimA             Created.
\**************************************************************************/

/* DIVERGENCE(OPENNT-HOST-041): retain original GetClassWOWWords only.
 * Other client services and the private precompiled environment are excluded.
 * A synchronous native class borrow replaces the private shared CLS view. */
#define WOW_ORIGINAL_CLASS_QUERY
#include "wow_class_query_bindings.h"
#include "../inc/ntsend.h"

LONG GetClassWOWWords(
    HINSTANCE hInstance,
    LPCTSTR pString)
{
    IN_STRING strClassName;
    PCLS pcls;

    /*
     * Make sure cleanup will work successfully
     */
    strClassName.fAllocated = FALSE;

    BEGINCALL()

        FIRSTCOPYLPSTRW(&strClassName, pString);

        pcls = NtUserGetWOWClass(hInstance, strClassName.pstr);

        if (pcls == NULL) {
            MSGERRORCODE(ERROR_CLASS_DOES_NOT_EXIST);
        }

        pcls = (PCLS)((PBYTE)pcls - GetClientInfo()->ulClientDelta);
        retval = _GetClassData(pcls, NULL, GCL_WOWWORDS, TRUE);

    ERRORTRAP(0);
    CLEANUPLPSTRW(strClassName);
    ENDCALL(LONG);
}
