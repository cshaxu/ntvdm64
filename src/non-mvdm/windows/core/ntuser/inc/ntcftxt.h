/***************************** Module Header ******************************\
* Module Name: ntcftxt.h
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* Kernel call forward stubs with text arguments
*
* Each function will be created with two flavors Ansi and Unicode
*
* 06-Jan-1992 IanJa      Moved from cf.h
* 18-Mar-1995 JimA       Ported from cftxt.h
\**************************************************************************/

#ifdef UNICODE
#error Excluded unsupported Unicode class-client configuration
#line 19
#else
#define IS_ANSI TRUE
#undef _UNICODE
#line 22
#endif
#line 23
/* DIVERGENCE(OPENNT-HOST-036): retain the complete ANSI class client
 * closure: duplication, menu capture, registration and unregistration.
 * Other text APIs and the private USER precompiled environment are cropped.
 * The consuming translation unit supplies the original-shaped declarations,
 * call/capture macros and finite host bindings; no server engine is imported. */

/* DIVERGENCE(OPENNT-HOST-046): also retain the original ANSI GetClassInfo
 * client body. The private server query is supplied by its finite binding. */
#line 111
BOOL GetClassInfo(
    HINSTANCE hmod OPTIONAL,
    LPCTSTR pszClassName,
    LPWNDCLASS pwc)
{
    IN_STRING strClassName;
    LPWSTR pszMenuName;
    WNDCLASSEXW WndClass;

    /*
     * Make sure cleanup will work successfully
     */
    strClassName.fAllocated = FALSE;

    BEGINCALL()

        FIRSTCOPYLPTSTRID(&strClassName, pszClassName);

        retval = (DWORD)NtUserGetClassInfo(
                hmod,
                strClassName.pstr,
                &WndClass,
                &pszMenuName,
                IS_ANSI);

        if (retval) {

            /*
             * Move the info from the WNDCLASSEX to the WNDCLASS structure
             */
            RtlCopyMemory(pwc, &WndClass.style, sizeof(*pwc));

            /*
             * Update these pointers so they point to something real.
             * pszMenuName is actually just the pointer the app originally
             * passed to us.
             */
            pwc->lpszMenuName = (LPTSTR)pszMenuName;
            pwc->lpszClassName = pszClassName;
        }

    ERRORTRAP(0);
    CLEANUPLPTSTR(strClassName);
    ENDCALL(BOOL);
}

/**************************************************************************\
* StringDuplicate
*
* 03-25-96 GerardoB         Added Header.
\**************************************************************************/
#define StringDuplicate TEXT_FN(StringDuplicate)
#line 578
LPTSTR StringDuplicate(LPCTSTR ptszDup) {
    LPTSTR ptsz;
    ULONG cb;

    cb = (_tcslen(ptszDup) + 1) * sizeof(TCHAR);
    ptsz = (LPTSTR)LocalAlloc(NONZEROLPTR, cb);
    if (ptsz != NULL) {
        RtlCopyMemory(ptsz, ptszDup, cb);
    }
    return ptsz;
}
/**************************************************************************\
* InitClsMenuName
*
* 03-22-96 GerardoB         Created.
\**************************************************************************/
#define InitClsMenuName TEXT_FN(InitClsMenuName)
BOOL InitClsMenuName (PCLSMENUNAME pcmn, LPCTSTR lpszMenuName, PIN_STRING pstrMenuName)
{
    /*
     * We check the high-word because this may be a resource-ID.
     */
    if (HIWORD(lpszMenuName) != 0) {
#line 601
#ifdef UNICODE
#line 602
#error Excluded unsupported Unicode class-client menu-name profile
#line 609
#else
#line 610
        if ((pcmn->pszClientAnsiMenuName = StringDuplicate(lpszMenuName)) == NULL) {
            return FALSE;
        }

        if (!MBToWCS(lpszMenuName, -1, &(pcmn->pwszClientUnicodeMenuName), -1, TRUE)) {
            pcmn->pwszClientUnicodeMenuName = NULL;
        }
#endif // UNICODE
#line 618
    } else {
        /* Copy the ID */
        pcmn->pszClientAnsiMenuName = (LPSTR)lpszMenuName;
        pcmn->pwszClientUnicodeMenuName = (LPWSTR)lpszMenuName;
    }

    COPYLPTSTRID(pstrMenuName, lpszMenuName);
    pcmn->pusMenuName = pstrMenuName->pstr;

    return TRUE;

    goto errorexit; /* Keep the compiler happy */
errorexit: /* Used by COPYLPTSTRID */
    /* DIVERGENCE(OPENNT-HOST-037): capture can fail after both client
     * menu names have been allocated. No class owns them on this path. */
    LocalFree(pcmn->pszClientAnsiMenuName);
    LocalFree(pcmn->pwszClientUnicodeMenuName);
#line 631
   return FALSE;
}

/* DIVERGENCE(OPENNT-HOST-036): SetClassLong is outside this client slice. */
/**************************************************************************\
* RegisterClassExWOW
*
* 03-22-96 GerardoB      Added Header
\**************************************************************************/
#line 701
ATOM TEXT_FN(RegisterClassExWOW)(
    WNDCLASSEX *lpWndClass,
    LPDWORD pdwWOWstuff,
    PROC lpfnWorker,
    WORD fnid)
{
    WNDCLASSEX WndClass;
    IN_STRING strClassName;
    IN_STRING strMenuName;
    DWORD dwFlags, dwExpWinVer;
    CLSMENUNAME cmn;

    strClassName.fAllocated = 0;
    strMenuName.fAllocated  = 0;

    /*
     * Skip validation for our classes
     */
    if (fnid !=0) {
        /*
         * This is a hack to bypass validation for DDE classes
         * specifically, allow them to use hmodUser.
         */
         if (fnid == FNID_DDE_BIT) {
             fnid = 0;
         }
    } else {
        if (lpWndClass->cbSize != sizeof(WNDCLASSEX)) {
            RIPMSG0(RIP_WARNING, "RegisterClass: Invalid cbSize");
        }

        if (lpWndClass->cbClsExtra < 0 ||
                lpWndClass->cbWndExtra < 0) {
            RIPMSG0(RIP_WARNING, "RegisterClass: invalid cb*Extra");
            goto BadParameter;
        }

        /*
         * Validate hInstance
         * Don't allow 4.0 apps to use hmodUser
         */
         if ((lpWndClass->hInstance == hmodUser)
                && (GetClientInfo()->dwExpWinVer >= VER40)) {
             RIPMSG0(RIP_WARNING, "RegisterClass: Cannot use USER's hInstance");
             goto BadParameter;
         } else if (lpWndClass->hInstance == NULL) {
            /*
             * For 32 bit apps we need to fix up the hInstance because Win 95 does
             * this in their thunk MapHInstLS
             */

            lpWndClass->hInstance = GetModuleHandle(NULL);
            RIPMSG1(RIP_WARNING, "RegisterClass: fixing up NULL hmodule to %lX",
                    lpWndClass->hInstance);
        }

        dwExpWinVer = GETEXPWINVER(lpWndClass->hInstance);


        /*
         * Check for valid style bits and strip if appropriate
         */
        if (lpWndClass->style & ~CS_VALID40) {

            if (dwExpWinVer > VER31) {
                RIPMSG0(RIP_WARNING, "RegisterClass: Invalid class style");
                goto BadParameter;
            }

            /*
             * Old application - strip bogus bits and pass through
             */
            RIPMSG0(RIP_WARNING, "RegisterClass: Invalid class style, stripping bad styles");
            lpWndClass->style &= CS_VALID40;
        }

        /*
         * Validate hbrBackground
         */
        if ((UINT) lpWndClass->hbrBackground > COLOR_MAX
                && !GdiValidateHandle(lpWndClass->hbrBackground)) {

            RIPMSG1(RIP_WARNING, "RegisterClass: Invalid class brush:%#lx", lpWndClass->hbrBackground);
            if (dwExpWinVer > VER30) {
                goto BadParameter;
            }

            lpWndClass->hbrBackground = NULL;
        }

    } /* if (fnid !=0) */


    if (!InitClsMenuName(&cmn, lpWndClass->lpszMenuName, &strMenuName)) {
        return FALSE;
    }

    BEGINCALL()

        WndClass = *lpWndClass;

#line 802
#ifdef UNICODE
#line 803
#error Excluded unsupported Unicode class-client registration profile
#else
#line 805
        dwFlags = CSF_ANSIPROC;
#endif // UNICODE
#line 807

        COPYLPTSTRID(&strClassName, (LPTSTR)lpWndClass->lpszClassName);

        retval = NtUserRegisterClassExWOW(
                &WndClass,
                strClassName.pstr,
                &cmn,
                lpfnWorker,
                fnid,
                dwFlags,
                pdwWOWstuff);

        /*
         * Return the atom associated with this class or if earlier
         * than Win 3.1 convert it to a strict BOOL (some apps check)
         */
        if (GETEXPWINVER(lpWndClass->hInstance) < VER31)
            retval = !!retval;

    ERRORTRAP(0);
    CLEANUPLPTSTR(strMenuName);     /* Initialized by InitClsMenuName */
    CLEANUPLPTSTR(strClassName);

    if (!retval) {
        if (HIWORD(cmn.pszClientAnsiMenuName) != 0) {
            LocalFree(cmn.pszClientAnsiMenuName);
        }
        if (HIWORD(cmn.pwszClientUnicodeMenuName) != 0) {
            LocalFree(cmn.pwszClientUnicodeMenuName);
        }
    }
    ENDCALL(BOOL);

BadParameter:
    RIPERR0(ERROR_INVALID_PARAMETER, RIP_VERBOSE, "RegisterClass: Invalid Parameter");
    return FALSE;

}

/* DIVERGENCE(OPENNT-HOST-036): unrelated text APIs between class operations
 * are excluded; retain the original unregister/cleanup body. */
#line 1051
BOOL UnregisterClass(
    LPCTSTR pszClassName,
    HINSTANCE hModule)
{
    IN_STRING strClassName;
    CLSMENUNAME cmn;

    /*
     * Make sure cleanup will work successfully
     */
    strClassName.fAllocated = FALSE;

    BEGINCALL()

        FIRSTCOPYLPTSTRID(&strClassName, pszClassName);

        retval = (DWORD)NtUserUnregisterClass(
                strClassName.pstr,
                hModule,
                &cmn);

        if (retval) {
            /*
             * Free the menu strings if they are not resource IDs
             */
            if (HIWORD(cmn.pszClientAnsiMenuName) != 0) {
                LocalFree(cmn.pszClientAnsiMenuName);
            }
            if (HIWORD(cmn.pwszClientUnicodeMenuName) != 0) {
                LocalFree(cmn.pwszClientUnicodeMenuName);
            }
        }

    ERRORTRAP(0);
    CLEANUPLPTSTR(strClassName);
    ENDCALL(BOOL);
}

/* DIVERGENCE(OPENNT-HOST-036): remaining text API families are excluded. */




#line 362
#ifdef UNICODE
#line 363
#error Excluded unsupported original UNICODE branch
#line 365
#endif
#line 366


#line 206
#ifdef UNICODE
#line 207
#error Excluded unsupported original UNICODE branch
#else
#line 209
#line 210
#endif
#line 211

#line 243
#ifdef UNICODE
#line 244
#error Excluded unsupported original UNICODE branch
#else
#line 246
#line 247
#endif
#line 248

#line 353
#ifdef UNICODE
#line 354
#error Excluded unsupported original UNICODE branch
#line 356
#else
#line 357
#line 358
#endif
#line 359

#line 992
#ifdef UNICODE
#line 993
#error Excluded unsupported original UNICODE branch
#line 995
#else
#line 996
#line 1001
#endif
#line 1002

#line 1096
#ifdef UNICODE
#line 1097
#error Excluded unsupported original UNICODE branch
#else
#line 1099
#line 1100
#endif // UNICODE
#line 1101

#line 1122
#ifdef UNICODE
#line 1123
#error Excluded unsupported original UNICODE branch
#else
#line 1125
#line 1152
#endif // UNICODE
#line 1153

#line 1434
#ifdef UNICODE
#line 1435
#error Excluded unsupported original UNICODE branch
#line 1449
#else
#line 1450
#line 1468
#endif
#line 1469


#line 293
#ifdef FE_SB // GetMessage()
#line 294
#error Unsupported FE_SB original branch
#line 301
#endif // FE_SB
#line 302

#line 340
#ifdef FE_SB // GetMessage()
#line 341
#error Unsupported FE_SB original branch
#line 344
#endif // FE_SB
#line 345

#line 310
#ifndef UNICODE
#line 311
#line 315
#ifdef FE_SB // GetMessage()
#line 316
#error Excluded unsupported original text-client condition
#line 325
#else
#line 326
#line 329
#endif
#line 330
#else
#line 331
#error Unsupported UNICODE original branch
#line 338
#endif // UNICODE
#line 339

#line 1191
#ifndef UNICODE
#line 1192
#line 1207
#else
#line 1208
#error Unsupported UNICODE original branch
#line 1213
#endif
#line 1214

#line 1317
#ifndef UNICODE
#line 1318
#line 1349
#else
#line 1350
#error Unsupported UNICODE original branch
#line 1354
#endif
#line 1355

#line 38
#ifndef UNICODE
#line 39
#line 64
#endif
#line 65
#line 217
#ifndef UNICODE
#line 218
#line 228
#endif
#line 229
#line 254
#ifndef UNICODE
#line 255
#line 265
#endif
#line 266
#line 271
#ifndef UNICODE
#line 272
#line 274
#endif
#line 275
#line 369
#ifndef UNICODE
#line 370
#line 377
#endif
#line 378
#line 395
#ifndef UNICODE
#line 396
#line 402
#endif
#line 403
#line 414
#ifndef UNICODE
#line 415
#line 417
#endif
#line 418
#line 431
#ifndef UNICODE
#line 432
#line 465
#endif
#line 466
#line 514
#ifndef UNICODE
#line 515
#line 515
#ifdef FE_SB // PostMessage()
#line 516
#error Excluded unsupported original text-client condition
#line 520
#endif
#line 521
#line 523
#endif
#line 524
#line 549
#ifndef UNICODE
#line 550
#line 550
#ifdef FE_SB // PostThreadMessage()
#line 551
#error Excluded unsupported original text-client condition
#line 556
#endif
#line 557
#line 559
#endif
#line 560
#line 934
#ifndef UNICODE
#line 935
#line 935
#ifdef FE_SB // SendMessageCallBack()
#line 936
#error Excluded unsupported original text-client condition
#line 940
#endif
#line 941
#line 943
#endif
#line 944
#line 976
#ifndef UNICODE
#line 977
#line 977
#ifdef FE_SB // SendNotifyMessage()
#line 978
#error Excluded unsupported original text-client condition
#line 982
#endif
#line 983
#line 985
#endif
#line 986
#line 1111
#ifndef UNICODE
#line 1112
#line 1114
#endif
#line 1115
#line 1295
#ifndef UNICODE
#line 1296
#line 1297
#endif
#line 1298
#line 1505
#ifndef UNICODE
#line 1506
#line 1507
#ifdef FE_SB // CallMsgFilter()
#line 1508
#error Excluded unsupported original text-client condition
#line 1517
#else
#line 1518
#line 1520
#endif
#line 1521
#line 1526
#ifdef FE_IME // CallMsgFilter()
#line 1527
#error Excluded unsupported original text-client condition
#line 1529
#endif
#line 1530
#line 1534
#endif
#line 1535
