/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    vrnmpipe.c

Abstract:

    Contains Named Pipe function handlers for Vdm Redir support. This module
    contains the following Vr (VdmRedir) routines:

    There are a couple of extra routines which must be called on open and close.
    Because these routines (in Dos Emulator) are general purpose, our open
    and close routines will be called for every file open/handle close. We
    must check that the operation is being performed on a named pipe entity.
    The routines are:

        VrAddOpenNamedPipeInfo
        VrRemoveOpenNamedPipeInfo

    Because named pipes are now opened in overlapped I/O mode, in case an app
    wishes to perform an asynchronous read or write operation, we must provide
    our own read/write routines for synchronously reading a pipe. If we just
    left this to the standard read/write routines in DEM, they would return an
    error because the handles were opened with FLAG_FILE_OVERLAPPED and the
    operations are performed with the LPOVERLAPPED parameter set to NULL

        VrReadNamedPipe
        VrWriteNamedPipe

    A couple of helper routines which are callable from outside of this module:

        VrIsNamedPipeName
        VrIsNamedPipeHandle
        VrConvertLocalNtPipeName

    Private (Vrp) routines retained by this synchronous subset:

        VrpAddOpenNamedPipeInfo
        VrpGetOpenNamedPipeInfo
        VrpRemoveOpenNamedPipeInfo

Author:

    Richard L Firth (rfirth) 10-Sep-1991

Environment:

    Any 32-bit flat address space

Notes:

    This module implements client-side named pipe support for the VDM process.
    Client-side named pipes are opened using the standard DOS open call (INT 21/
    ah=3dh) from a DOS app. The actual open is performed in the 32-bit context
    where a 32-bit handle is returned. This is put in the DOS context SFT and
    DOS returns an 8-bit J(ob) F(ile) N(umber) which the app then uses in other
    named pipe calls. The redir, which handles named pipe requests apart from
    open and close, must map the 8-bit JFN to the original 32-bit handle using
    a routine exported from DOS. The handle is then stored in BP:BX and control
    passed here.

    When an open succeeds, we add an OPEN_NAMED_PIPE_INFO structure to a list
    of structures. This maps the handle and name (for DosQNmPipeInfo). We don't
    expect to have very many of these structures at any one time, so they are
    singly linked and sequentially traversed using the handle as a key

    This code assumes that only one process at a time will be updating the list
    of structures and that any non-stack data items in this module will be
    replicated to all processes which use these functions (Ie the data is NOT
    shared)

Revision History:

    10-Sep-1991 RFirth
        Created

--*/

#include <windows.h>
#include <ntrtl.h>

/* DIVERGENCE(HOST-DIV-024,HOST-DIV-025): private static-provider/product manifests. */
#include "opennt-host-overlay/vdmredir/vrnmpipe_product_seams.h"
#include "opennt-host/inc/vrnmpipe.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#undef PRIVATE
#define PRIVATE /* static */            // actually, want to see routines in FREE build
PRIVATE
BOOL
VrpAddOpenNamedPipeInfo(
    IN HANDLE Handle,
    IN LPSTR PipeName
    );

PRIVATE
POPEN_NAMED_PIPE_INFO
VrpGetOpenNamedPipeInfo(
    IN HANDLE Handle
    );

PRIVATE
BOOL
VrpRemoveOpenNamedPipeInfo(
    IN HANDLE Handle
    );

//
// externally callable interceptors
//

BOOL
VrAddOpenNamedPipeInfo(
    IN  HANDLE  Handle,
    IN  LPSTR   lpFileName
    )

/*++

Routine Description:

    This routine is called whenever DEM (Dos Emulator) successfully opens a
    handle to a file. We check if the file just opened was a named pipe (based
    on the name) and if so create an association between name and handle

Arguments:

    Handle      - of just opened file/pipe/device
    lpFileName  - symbolic name of what was just opened

Return Value:

    BOOL
        TRUE    - created/added open named pipe structure
        FALSE   - couldn't allocate structure memory or create event

--*/

{
    BOOL ok;

#if DBG
    IF_DEBUG(NAMEPIPE) {
        DbgPrint("VrAddOpenNamedPipeInfo\n");
    }
#endif

    if (VrIsNamedPipeName(lpFileName)) {

#if DBG
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("Adding %s as named pipe\n", lpFileName);
        }
#endif

        //
        // if we can't create the named pipe info structure, or the async
        // read/write event, return FALSE which results in an out-of-resources
        // error (not enough memory) since DOS doesn't understand about events
        //

        ok = VrpAddOpenNamedPipeInfo(Handle, lpFileName);
    } else {

#if DBG
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("VrAddOpenNamedPipeInfo: Error: not named pipe: %s\n", lpFileName);
        }
#endif

        ok = FALSE;

    }

    return ok;
}


BOOL
VrRemoveOpenNamedPipeInfo(
    IN HANDLE Handle
    )

/*++

Routine Description:

    This is the companion routine to VrAddOpenNamedPipeInfo. When a handle is
    successfully closed for a DOS app, we must check if it referenced a named
    pipe, and if so remove the info structure we created when the pipe was
    opened

Arguments:

    Handle  - to file/pipe/device just closed for Dos app

Return Value:

    BOOL
        TRUE
        FALSE

--*/

{
#if DBG
    IF_DEBUG(NAMEPIPE) {
        DbgPrint("VrRemoveOpenNamedPipeInfo\n");
    }

    if (!VrpRemoveOpenNamedPipeInfo(Handle)) {
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("Handle 0x%08x is not a named pipe\n", Handle);
        }
        return FALSE;
    } else {
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("VrRemoveOpenNamedPipeInfo - Handle 0x%08x has been removed\n", Handle);
        }
        return TRUE;
    }
#else
    VrpRemoveOpenNamedPipeInfo(Handle);
#endif

    return TRUE;
}


BOOL
VrReadNamedPipe(
    IN  HANDLE  Handle,
    IN  LPBYTE  Buffer,
    IN  DWORD   Buflen,
    OUT LPDWORD BytesRead,
    OUT LPDWORD Error
    )

/*++

Routine Description:

    Performs ReadFile on a named pipe handle. All named pipes are opened in
    overlapped-IO mode because async read/writes cannot be performed otherwise

Arguments:

    Handle      - of opened NamedPipe
    Buffer      - client (VDM) data buffer
    Buflen      - size of read buffer
    BytesRead   - where actual bytes read is returned
    Error       - pointer to returned error in case of failure or more data

Return Value:

    BOOL
        TRUE    - handle was successfully written
        FALSE   - an error occurred, use GetLastError

--*/

{
    OVERLAPPED_PIPE_IO pipeio;
    BOOL success;
    DWORD error;
    /* DIVERGENCE(HOST-DIV-023): MSVC /W4 correctly diagnoses the original
     * potentially uninitialized failure observation. This value is not
     * observable after a successful GetOverlappedResult. */
    DWORD dwBytesRead = 0;

#if DBG
    IF_DEBUG(NAMEPIPE) {
        DbgPrint("VrReadNamePipe(0x%08x, %x, %d)\n", Handle, Buffer, Buflen);
    }
#endif

    //
    // create an event to wait on. This goes in the overlapped structure - it
    // is the only thing in the overlapped structure we are interested in.
    // Create the event with manual reset. This is so that if the I/O operation
    // completes immediately, we don't wait on the event. If we create the
    // event as auto-reset, it can go into the signalled state, and back to the
    // not-signalled state before we prime the wait, causing us to wait forever
    // for an event that has already occurred
    //

    RtlZeroMemory(&pipeio, sizeof(pipeio));
    if ((pipeio.Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL) {
        *Error = ERROR_NOT_ENOUGH_MEMORY;   // really want out-of-resources (71?)
        return FALSE;
    }

    //
    // event handle created ok
    //

    /* DIVERGENCE(HOST-DIV-015): VDD cancellation registration belongs to the
     * deferred asynchronous Redirector owner. This synchronous retained call
     * has no cancellable product-global record. */
    success = ReadFile(Handle, Buffer, Buflen, BytesRead, &pipeio.Overlapped);
    if (!success) {
        error = GetLastError();
        if (error == ERROR_IO_PENDING) {
            error = WaitForSingleObject(pipeio.Overlapped.hEvent, NAMED_PIPE_TIMEOUT);
            if (error == 0xffffffff) {
                error = GetLastError();
            } else {
                success = (error == WAIT_OBJECT_0);
            }

        } else {

#if DBG
            IF_DEBUG(NAMEPIPE) {
                DbgPrint("VrReadNamedPipe: ReadFile failed: %d\n", GetLastError());
            }
#endif

            //
            // if we got ERROR_MORE_DATA, then this is actually success(!). In this case
            // we don't want to SetLastError, but we do want to set the extended error
            // info in DOS data segment. This is done by demRead
            //

            if (error == ERROR_MORE_DATA) {
                success = TRUE;
            }
        }
    } else {
        error = NO_ERROR;
    }


    if (success) {

        //
        // get the real bytes read. If GetOverlappedResult returns FALSE,
        // check for ERROR_MORE_DATA
        //

        success = GetOverlappedResult(Handle, &pipeio.Overlapped, &dwBytesRead, FALSE);
        error = success ? NO_ERROR : GetLastError();

        //
        // if we got ERROR_MORE_DATA, then this is actually success(!). In this case
        // we don't want to SetLastError, but we do want to set the extended error
        // info in DOS data segment. This is done by demRead
        //

        if (error == ERROR_MORE_DATA) {
            success = TRUE;
        }
    } else if (error == WAIT_TIMEOUT) {
        /* DIVERGENCE(HOST-DIV-016): the session-owned handle manager, not this
         * OpenNT mirror, owns retirement of the opaque native handle. */
    }

    CloseHandle(pipeio.Overlapped.hEvent);

    //
    // if no bytes were read and success was returned then treat this as an
    // error - this is what the DOS Redir does
    //

    if (error == NO_ERROR && dwBytesRead == 0) {
        error = ERROR_NO_DATA;
        success = FALSE;
    }

    if (!success) {

#if DBG
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("VrReadNamePipe: Error: Returning %d\n", error);
        }
#endif

        SetLastError(error);
    } else {
        *BytesRead = dwBytesRead;

#if DBG
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("VrReadNamePipe: Ok: %d bytes read from pipe\n", *BytesRead);
        }
#endif

    }

    //
    // set the error code so that we can set the extended error code info
    // from demRead and return the success/failure indication
    //

    *Error = error;
    return success;
}


BOOL
VrWriteNamedPipe(
    IN  HANDLE  Handle,
    IN  LPBYTE  Buffer,
    IN  DWORD   Buflen,
    OUT LPDWORD BytesWritten
    )

/*++

Routine Description:

    Performs WriteFile on a named pipe handle. All named pipes are opened in
    overlapped-IO mode because async read/writes cannot be performed otherwise

Arguments:

    Handle          - of opened NamedPipe
    Buffer          - client (VDM) data buffer
    Buflen          - size of write
    BytesWritten    - where actual bytes written is returned

Return Value:

    BOOL
        TRUE    - handle was successfully written
        FALSE   - an error occurred, use GetLastError

--*/

{
    OVERLAPPED_PIPE_IO pipeio;
    BOOL success;
    DWORD error;

#if DBG
    IF_DEBUG(NAMEPIPE) {
        DbgPrint("VrWriteNamePipe(0x%08x, %x, %d)\n", Handle, Buffer, Buflen);
    }
#endif

    //
    // create an event to wait on. This goes in the overlapped structure - it
    // is the only thing in the overlapped structure we are interested in.
    // Create the event with manual reset. This is so that if the I/O operation
    // completes immediately, we don't wait on the event. If we create the
    // event as auto-reset, it can go into the signalled state, and back to the
    // not-signalled state before we prime the wait, causing us to wait forever
    // for an event that has already occurred
    //

    RtlZeroMemory(&pipeio, sizeof(pipeio));
    if ((pipeio.Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL) {
        error = ERROR_NOT_ENOUGH_MEMORY;
        success = FALSE;
    } else {
        /* DIVERGENCE(HOST-DIV-015): VDD cancellation registration belongs to the
     * deferred asynchronous Redirector owner. This synchronous retained call
     * has no cancellable product-global record. */
        success = WriteFile(Handle, Buffer, Buflen, BytesWritten, &pipeio.Overlapped);
        error = success ? NO_ERROR : GetLastError();
        if (error == ERROR_IO_PENDING) {
            error = WaitForSingleObject(pipeio.Overlapped.hEvent, NAMED_PIPE_TIMEOUT);
            if (error == 0xffffffff) {
                error = GetLastError();
            } else {
                success = (error == WAIT_OBJECT_0);
            }
        }
            if (pipeio.Cancelled) {
            error = WAIT_TIMEOUT;
            success = FALSE;
        }
    }
    if (success) {

        //
        // get the real bytes written
        //

        GetOverlappedResult(Handle, &pipeio.Overlapped, BytesWritten, FALSE);
    }
    CloseHandle(pipeio.Overlapped.hEvent);
    if (!success) {

#if DBG
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("VrWriteNamePipe: Error: Returning %d\n", error);
        }
#endif

        SetLastError(error);
        if (error == WAIT_TIMEOUT) {
            /* DIVERGENCE(HOST-DIV-016): see the matching read-path rule. */
        }
    }

#if DBG

    else {
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("VrWriteNamePipe: Ok: %d bytes written to pipe\n", *BytesWritten);
        }
    }

#endif

    return success;
}


//
// externally callable helpers
//

BOOL
VrIsNamedPipeName(
    IN LPSTR Name
    )

/*++

Routine Description:

    Checks if a string designates a named pipe. As criteria for the decision
    we use:

        \\computername\PIPE\...

    DOS (client-side) can only open a named pipe which is created at a server
    and must therefore be prefixed by a computername

    We *know* that Name has just been used to successfully open a handle to
    a named <something>, so it should at least be semi-sensible. We can
    assume the following:

        * ASCIZ string
        * an LPSTR points at a single byte (& therefore ++ will add 1)

    But we can't assume the following:

        * Canonicalized name

Arguments:

    Name    - to check for (Dos) named pipe syntax

Return Value:

    BOOL
        TRUE    - Name refers to (local or remote) named pipe
        FALSE   - Name doesn't look like name of pipe

--*/

{
    int     CharCount;

#if DBG
    LPSTR   OriginalName = Name;
#endif

    if (IS_ASCII_PATH_SEPARATOR(*Name)) {
        ++Name;
        if (IS_ASCII_PATH_SEPARATOR(*Name)) {
            ++Name;
            CharCount = 0;
            while (*Name && !IS_ASCII_PATH_SEPARATOR(*Name)) {
                ++Name;
                ++CharCount;
            }
            if (!CharCount || !*Name) {

                //
                // Name is \\ or \\\ or just \\name which I don't understand,
                // so its not a named pipe - fail it
                //

#if DBG
                IF_DEBUG(NAMEPIPE) {
                    DbgPrint("VrIsNamedPipeName - returning FALSE for %s\n", OriginalName);
                }
#endif
                return FALSE;
            }

            //
            // bump name past next path separator. Note that we don't have to
            // check CharCount for max. length of a computername, because this
            // function is called only after the (presumed) named pipe has been
            // successfully opened, therefore we know that the name has been
            // validated
            //

            ++Name;
        } else {

#if DBG
            IF_DEBUG(NAMEPIPE) {
                DbgPrint("VrIsNamedPipeName - returning FALSE for %s\n", OriginalName);
            }
#endif

            return FALSE;

        }

        //
        // We are at <something> (after \ or \\<name>\). Check if <something>
        // is [Pp][Ii][Pp][Ee][\\/]
        //

        if (!_strnicmp(Name, "PIPE", 4)) {
            Name += 4;
            if (IS_ASCII_PATH_SEPARATOR(*Name)) {

#if DBG
                IF_DEBUG(NAMEPIPE) {
                    DbgPrint("VrIsNamedPipeName - returning TRUE for %s\n", OriginalName);
                }
#endif

                return TRUE;
            }
        }
    }

#if DBG
    IF_DEBUG(NAMEPIPE) {
        DbgPrint("VrIsNamedPipeName - returning FALSE for %s\n", OriginalName);
    }
#endif

    return FALSE;
}


BOOL
VrIsNamedPipeHandle(
    IN HANDLE Handle
    )

/*++

Routine Description:

    Checks if Handle appears in the list of known named pipe handles. Callable
    from outside this module

Arguments:

    Handle  - of suspected name pipe

Return Value:

    BOOL
        TRUE    Handle refers to an open named pipe
        FALSE   Don't know what Handle refers to

--*/

{
    return VrpGetOpenNamedPipeInfo(Handle) != NULL;
}


LPSTR
VrConvertLocalNtPipeName(
    OUT LPSTR Buffer OPTIONAL,
    IN LPSTR Name
    )

/*++

Routine Description:

    Converts a pipe name of the form \\<local-machine-name>\pipe\name to
    \\.\pipe\name

    If non-NULL pointer is returned, the buffer contains a canonicalized
    name - any forward-slash characters (/) are converted to backward-slash
    characters (\). In the interest of future-proofing, the name is not
    upper-cased

    Assumes: Name points to a named pipe specification (\\Server\PIPE\name)

    Note: it is possible to supply the same input and output buffers and have
          the conversion take place in situ. However, this is a side-effect
          of the fact the input computername is replaced by effectively a
          computername of length 1. Nevertheless, it is safe

Arguments:

    Buffer  - pointer to CHAR array where name is placed. If this parameter
              is not present then this routine will allocate a buffer (using
              LocalAlloc and return that
    Name    - pointer to ASCIZ pipe name

Return Value:

    LPSTR   - pointer to buffer containing name or NULL if failed

--*/

{
    DWORD prefixLength; // length of \\computername
    DWORD pipeLength;   // length of pipe name without computername/device prefix
    LPSTR pipeName;     // \PIPE\name...
    static char ThisComputerName[MAX_COMPUTERNAME_LENGTH+1] = {0};
    static DWORD ThisComputerNameLength = 0xffffffff;
    BOOLEAN mapped = FALSE;

    ASSERT(Name);
    ASSERT(IS_ASCII_PATH_SEPARATOR(Name[0]) && IS_ASCII_PATH_SEPARATOR(Name[1]));

    //
    // first time round, get the computername. If this fails assume there is no
    // computername (i.e. no network)
    //

    if (ThisComputerNameLength == 0xffffffff) {
        ThisComputerNameLength = sizeof(ThisComputerName);
        if (!GetComputerName((LPTSTR)&ThisComputerName, &ThisComputerNameLength)) {
            ThisComputerNameLength = 0;
        }
    }

    if (!ARGUMENT_PRESENT(Buffer)) {
        Buffer = (LPSTR)LocalAlloc(LMEM_FIXED, strlen(Name)+1);
    }

    if (Buffer) {
        pipeName = strchr(Name+2, '\\');    // starts \pipe\...
        if (!pipeName) {
            pipeName = strchr(Name+2, '/');
        }
        ASSERT(pipeName);
        pipeLength = (DWORD)strlen(pipeName);
        /* DIVERGENCE(HOST-DIV-020): the original expression narrowed pointers
         * before subtraction. Preserve its byte count with defined arithmetic. */
        prefixLength = (DWORD)(pipeName - Name);
        if (ThisComputerNameLength && (prefixLength - 2 == ThisComputerNameLength)) {
            if (!_strnicmp(ThisComputerName, &Name[2], ThisComputerNameLength)) {
                strcpy(Buffer, LOCAL_DEVICE_PREFIX);
                mapped = TRUE;
            }
        }
        if (!mapped) {
            strncpy(Buffer, Name, prefixLength);
            Buffer[prefixLength] = 0;

        }
        strcat(Buffer, pipeName);

        //
        // convert any forward-slashes to backward-slashes
        //


        do {
            /* DIVERGENCE(HOST-DIV-020): parenthesize the original assignment
             * expression for the required warning-clean modern build. */
            if ((pipeName = strchr(Buffer, '/')) != NULL) {
                *pipeName++ = '\\';
            }
        } while (pipeName);
        (void)pipeLength;

#if DBG
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("VrConvertLocalNtPipeName - returning %s\n", Buffer);
        }
#endif

    }

    return Buffer;
}


//
// Private utilities
//

//
// Private list of open named pipe info structures for this VDM process, and
// associated manipulation routines
//

PRIVATE
POPEN_NAMED_PIPE_INFO   OpenNamedPipeInfoList = NULL;

PRIVATE
POPEN_NAMED_PIPE_INFO   LastOpenNamedPipeInfo = NULL;

PRIVATE
BOOL
VrpAddOpenNamedPipeInfo(
    IN HANDLE Handle,
    IN LPSTR PipeName
    )

/*++

Routine Description:

    When a named pipe is successfully opened, we call this routine to
    associate an open handle and a pipe name. This is required by
    DosQNmPipeInfo (VrGetNamedPipeInfo)

Arguments:

    Handle      - The handle returned from CreateFile (in demOpen)
    PipeName    - Name of pipe being opened

Return Value:

    BOOL
        TRUE    - created a OPEN_NAMED_PIPE_INFO structure and added to list
        FALSE   - couldn't get memory, or couldn't create event. Use GetLastError
                  if you really want to know why this failed

--*/

{
    POPEN_NAMED_PIPE_INFO PipeInfo;
    DWORD NameLength;

    //
    // grab a OPEN_NAMED_PIPE_INFO structure
    //

    /* DIVERGENCE(HOST-DIV-020): retain the original count/value with an
     * explicit x86/x64-safe narrowing conversion. */
    NameLength = (DWORD)strlen(PipeName) + 1;
    PipeInfo = (POPEN_NAMED_PIPE_INFO)
                LocalAlloc(LMEM_FIXED,
                    ROUND_UP_COUNT((sizeof(OPEN_NAMED_PIPE_INFO) + NameLength),
                        sizeof(DWORD)
                        )
                    );

    //
    // if we cannot claim memory here, we should *really* close the pipe and
    // return an insufficient memory error to the VDM. However, I don't expect
    // us to run out of memory
    //

    if (PipeInfo == NULL) {

#if DBG
        IF_DEBUG(NAMEPIPE) {
            DbgPrint("VrpAddOpenNamedPipeInfo: couldn't allocate structure - returning FALSE\n");
        }
#endif

        return FALSE;
    }

    //
    // fill it in
    //

    PipeInfo->Next = NULL;
    PipeInfo->Handle = Handle;
    PipeInfo->NameLength = NameLength;
    strcpy(PipeInfo->Name, PipeName);   // from DOS, so its old-fashioned ASCII

    //
    // put it at the end of the list
    //

    if (LastOpenNamedPipeInfo == NULL) {
        OpenNamedPipeInfoList = PipeInfo;
    } else {
        LastOpenNamedPipeInfo->Next = PipeInfo;
    }
    LastOpenNamedPipeInfo = PipeInfo;

#if DBG
    IF_DEBUG(NAMEPIPE) {
        DbgPrint("VrpAddOpenNamedPipeInfo - adding structure @ %08x, Handle=0x%08x, Name=%s\n",
            PipeInfo,
            PipeInfo->Handle,
            PipeInfo->Name
            );
    }
#endif

    return TRUE;
}


PRIVATE
POPEN_NAMED_PIPE_INFO
VrpGetOpenNamedPipeInfo(
    IN HANDLE Handle
    )

/*++

Routine Description:

    Linear search for an OPEN_NAMED_PIPE_INFO structure in OpenNamedPipeInfoList
    using the handle as search criteria

Arguments:

    Handle  - to search for

Return Value:

    POPEN_NAMED_PIPE_INFO
        Success - Pointer to located structure
        Failure - NULL

--*/

{
    POPEN_NAMED_PIPE_INFO ptr;

    for (ptr = OpenNamedPipeInfoList; ptr; ptr = ptr->Next) {
        if (ptr->Handle == Handle) {
            break;
        }
    }
    return ptr;
}


PRIVATE
BOOL
VrpRemoveOpenNamedPipeInfo(
    IN HANDLE Handle
    )

/*++

Routine Description:

    Unlinks and frees an OPEN_NAMED_PIPE_INFO structure from
    OpenNamedPipeInfoList

    Note: Assumes that the Handle is in the list (no action taken if not
    found)

Arguments:

    Handle  - defining OPEN_NAMED_PIPE_INFO structure to remove from list

Return Value:

    BOOL
        TRUE    - OPEN_NAMED_PIPE_INFO structure corresponding to Handle was
                  removed from list and freed
        FALSE   - OPEN_NAMED_PIPE_INFO structure corresponding to Handle was
                  not found

--*/

{
    POPEN_NAMED_PIPE_INFO ptr, prev = NULL;

#if DBG
    IF_DEBUG(NAMEPIPE) {
        DbgPrint("VrpRemoveOpenNamedPipeInfo(0x%08x)\n", Handle);
        DumpOpenPipeList();
        DumpRequestQueue();
    }
#endif

    for (ptr = OpenNamedPipeInfoList; ptr; ) {
        if (ptr->Handle == Handle) {
            if (!prev) {
                OpenNamedPipeInfoList = ptr->Next;
            } else {
                prev->Next = ptr->Next;
            }
            if (LastOpenNamedPipeInfo == ptr) {
                LastOpenNamedPipeInfo = prev;
            }

#if DBG
            IF_DEBUG(NAMEPIPE) {
                DbgPrint("VrpRemoveOpenNamedPipeInfo - freeing structure @ %08x, Handle=0x%08x, Name=%s\n",
                    ptr,
                    ptr->Handle,
                    ptr->Name
                    );
            }
#endif

            LocalFree(ptr);
            return TRUE;
        } else {
            prev = ptr;
            ptr = ptr->Next;
        }
    }

#if DBG
    IF_DEBUG(NAMEPIPE) {
        DbgPrint("VrpRemoveOpenNamedPipeInfo: Can't find 0x%08x in list\n", Handle);
    }
#endif

    return FALSE;
}
