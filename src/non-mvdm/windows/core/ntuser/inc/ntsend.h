/**************************************************************************\
* Module Name: ntsend.h
*
* Copyright (c) Microsoft Corp.  1995 All Rights Reserved
*
* client-side macros for kernel-mode
*
* 03-21-95 JimA             Created.
\**************************************************************************/

/* DIVERGENCE(OPENNT-HOST-042): retain only original call/error and ANSI
 * capture/cleanup macros reached by GetClassWOWWords and ANSI class clients. Other call families
 * and thread-connection macros are excluded. */

#define BEGINCALL()       \
    {                     \
    DWORD retval;         \
    {

#define ERRORTRAP(error) \
       goto cleanup;        \
    }                       \
    goto errorexit;         \
errorexit:                  \
    retval = (DWORD)error;  \
cleanup:

#define ENDCALL(type)     \
    return (type)retval;  \
    }

#define MSGERROR() goto errorexit

#define MSGERRORCODE(code) { \
    RIPERR0(code, RIP_WARNING, "Unspecified error"); \
    goto errorexit; }

#define COPYLPSTRIDW(pinstr, psz) \
    if (HIWORD(psz)) {                                      \
        if (!RtlCaptureAnsiString((pinstr), (LPCSTR)(psz), TRUE))   \
            MSGERROR();                                     \
    }                                                       \
    else {                                                  \
        (pinstr)->fAllocated = FALSE;                       \
        (pinstr)->pstr = &(pinstr)->strCapture;             \
        (pinstr)->strCapture.Length =                       \
                (pinstr)->strCapture.MaximumLength = 0;     \
        (pinstr)->strCapture.Buffer = (LPWSTR)(psz);        \
    }

/* DIVERGENCE(OPENNT-HOST-140): retain the original text-width selection and
 * only the capture aliases used by the admitted client closures. */
#define COPYLPWSTRID(pinstr, psz) \
    (pinstr)->fAllocated = FALSE;                           \
    (pinstr)->pstr = &(pinstr)->strCapture;                 \
    if (HIWORD(psz))                                        \
        RtlInitUnicodeString(&(pinstr)->strCapture, (psz)); \
    else {                                                  \
        (pinstr)->strCapture.Length =                       \
                (pinstr)->strCapture.MaximumLength = 0;     \
        (pinstr)->strCapture.Buffer = (LPWSTR)(psz);        \
    }
#define CLEANUPLPWSTR(instr)

#ifdef UNICODE
#line 52
#define COPYLPTSTRID                COPYLPWSTRID
#define FIRSTCOPYLPTSTRID           COPYLPWSTRID
#define CLEANUPLPTSTR               CLEANUPLPWSTR
#else
#line 52
#define COPYLPTSTRID                COPYLPSTRIDW
#define FIRSTCOPYLPTSTRID           COPYLPSTRIDW
#define CLEANUPLPTSTR               CLEANUPLPSTRW
#endif
#line 55

#define FIRSTCOPYLPSTRW(pinstr, psz) \
    if (!RtlCaptureAnsiString((pinstr), (LPCSTR)(psz), FALSE))    \
        MSGERROR();

#define CLEANUPLPSTRW(instr) \
    if (instr.fAllocated)                     \
        RtlFreeHeap(RtlProcessHeap(), 0, instr.strCapture.Buffer);
