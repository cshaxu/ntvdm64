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

/*
 * Use this macro if you don't need to access shared memory.
 */
#line 27
#define BEGINCALL()       \
    {                     \
    DWORD retval;         \
    {

#line 35
#define ERRORTRAP(error) \
       goto cleanup;        \
    }                       \
    goto errorexit;         \
errorexit:                  \
    retval = (DWORD)error;  \
cleanup:

#line 47
#define ENDCALL(type)     \
    return (type)retval;  \
    }

#line 55
#define MSGERROR() goto errorexit

#line 56
#define MSGERRORCODE(code) { \
    RIPERR0(code, RIP_WARNING, "Unspecified error"); \
    goto errorexit; }

#line 91
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

#line 136
#define FIRSTCOPYLPSTRW(pinstr, psz) \
    if (!RtlCaptureAnsiString((pinstr), (LPCSTR)(psz), FALSE))    \
        MSGERROR();

#line 184
#define CLEANUPLPSTRW(instr) \
    if (instr.fAllocated)                     \
        RtlFreeHeap(RtlProcessHeap(), 0, instr.strCapture.Buffer);

/* DIVERGENCE(OPENNT-HOST-140): retain the original text-width selection and
 * only the capture aliases used by the admitted client closures. */
#line 196
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
#line 249
#define CLEANUPLPWSTR(instr)

/*
 * Type-neutral macros
 */
#line 254
#ifdef UNICODE
#line 257
#define COPYLPTSTRID                COPYLPWSTRID
#line 261
#define FIRSTCOPYLPTSTRID           COPYLPWSTRID
#line 268
#define CLEANUPLPTSTR               CLEANUPLPWSTR
#line 270
#else
#line 273
#define COPYLPTSTRID                COPYLPSTRIDW
#line 277
#define FIRSTCOPYLPTSTRID           COPYLPSTRIDW
#line 284
#define CLEANUPLPTSTR               CLEANUPLPSTRW
#line 286
#endif
#line 287

