/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    vrnmpipe.c

Abstract:

    Contains Named Pipe function handlers for Vdm Redir support. This module
    contains the following Vr (VdmRedir) routines:

    Contents:
        VrGetNamedPipeInfo
        VrGetNamedPipeHandleState
        VrSetNamedPipeHandleState
        VrPeekNamedPipe
        VrTransactNamedPipe
        VrCallNamedPipe
        VrWaitNamedPipe
        VrNetHandleGetInfo
        VrNetHandleSetInfo
        VrReadWriteAsyncNmPipe
        VrNmPipeInterrupt
        VrTerminateNamedPipes
        VrCancelPipeIo

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

    Private (Vrp) routines:

        VrpAsyncNmPipeThread
        VrpSnapshotEventList
        VrpSearchForRequestByEventHandle
        VrpCompleteAsyncRequest
        VrpQueueAsyncRequest
        VrpDequeueAsyncRequest
        VrpFindCompletedRequest
        VrpAddOpenNamedPipeInfo
        VrpGetOpenNamedPipeInfo
        VrpRemoveOpenNamedPipeInfo
        RememberPipeIo
        ForgetPipeIo

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

#include <nt.h>
#include <ntrtl.h>      // ASSERT, DbgPrint
#include <nturtl.h>
#include <windows.h>
#include <softpc.h>     // x86 virtual machine definitions
#include <vrdlctab.h>
#include <vdmredir.h>   // common Vdm Redir stuff
#include <vrinit.h>     // VrQueueCompletionHandler
#include "vrdebug.h"    // IF_DEBUG
#include "vrputil.h"    // private utility prototypes
//#include <os2def.h>
//#include <bsedos.h>     // PIPEINFO structure
#include <align.h>
#include <lmcons.h>     // LM20_PATHLEN
#include <lmerr.h>      // NERR_
#include <string.h>     // Dos still dealing with ASCII
#include <dossvc.h>     // PDEMEXTERR
#include <exterr.h>     // extended error info

//
// the following 2 #undef's required because without them, insignia.h gives
// errors (BOOL previously typedef'd) when compiled for MIPS
//

#undef BOOL
#undef NT_INCLUDED
#include <insignia.h>   // Insignia defines
#include <xt.h>         // half_word
#include <ica.h>        // ica_hw_interrupt
#include <idetect.h>    // WaitIfIdle
#include <vrica.h>      // call_ica_hw_interrupt
#include <vrnmpipe.h>   // routine prototypes
/* DIVERGENCE(MVDM-HOST-DIV-167): original async requests retained flat
 * GetVDMAddr aliases. Keep dispatch, queue and ICA control flow in the
 * mirror; enter the Redirector adapter only at the guest-memory lifetime
 * seam. */
#include "common/redirector/async.h"

//
// manifests
//

//#define NAMED_PIPE_TIMEOUT  300000  // 5 minutes
#line 146
#define NAMED_PIPE_TIMEOUT  INFINITE

//
// private data types
//

//
// OVERLAPPED_PIPE_IO - contains handle of thread issuing named pipe I/O request.
// If the app is later killed, we need to cancel any pending named pipe I/O
//

typedef struct _OVERLAPPED_PIPE_IO {
    struct _OVERLAPPED_PIPE_IO* Next;
    DWORD Thread;
    BOOL Cancelled;
    OVERLAPPED Overlapped;
} OVERLAPPED_PIPE_IO, *POVERLAPPED_PIPE_IO;


//
// private routine prototypes
//

#undef PRIVATE
#define PRIVATE /* static */            // actually, want to see routines in FREE build

PRIVATE
DWORD
VrpAsyncNmPipeThread(
    IN LPVOID Parameters
    );

PRIVATE
DWORD
VrpSnapshotEventList(
    OUT LPHANDLE pList
    );

PRIVATE
PDOS_ASYNC_NAMED_PIPE_INFO
VrpSearchForRequestByEventHandle(
    IN HANDLE EventHandle
    );

PRIVATE
VOID
VrpCompleteAsyncRequest(
    IN PDOS_ASYNC_NAMED_PIPE_INFO pAsyncInfo
    );

PRIVATE
VOID
VrpQueueAsyncRequest(
    IN PDOS_ASYNC_NAMED_PIPE_INFO pAsyncInfo
    );

PRIVATE
PDOS_ASYNC_NAMED_PIPE_INFO
VrpDequeueAsyncRequest(
    IN PDOS_ASYNC_NAMED_PIPE_INFO pAsyncInfo
    );

PRIVATE
PDOS_ASYNC_NAMED_PIPE_INFO
VrpFindCompletedRequest(
    VOID
    );

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

PRIVATE
VOID
RememberPipeIo(
    IN POVERLAPPED_PIPE_IO PipeIo
    );

PRIVATE
VOID
ForgetPipeIo(
    IN POVERLAPPED_PIPE_IO PipeIo
    );

#if DBG
#line 246
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 249

//
// global data
//

DWORD VrPeekNamedPipeTickCount;

//
// private data
//

CRITICAL_SECTION VrNamedPipeCancelCritSec;
POVERLAPPED_PIPE_IO PipeIoQueue = NULL;


//
// Vdm Redir Named Pipe support routines
//

VOID
VrGetNamedPipeInfo(
    VOID
    )

/*++

Routine Description:

    Performs GetNamedPipeInfo (DosQNmPipeInfo) request on behalf of VDM redir

Arguments:

    Function = 5F32h

    ENTRY   BP:BX = 32-bit Named Pipe handle
            CX = Buffer size
            DX = Info level
            DS:SI = Buffer

    EXIT    CF = 1
                AX = Error code

            CF = 0
                no error
                AX = undefined

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    HANDLE Handle;
    DWORD Flags, OutBufferSize, InBufferSize, MaxInstances, CurInstances, bufLen;
    PIPEINFO* PipeInfo;
    BOOL Ok;
    POPEN_NAMED_PIPE_INFO OpenNamedPipeInfo;

#if DBG
#line 309
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 319

    //
    // bp:bx is 32-bit named pipe handle. Mapped from 8-bit handle in redir
    //

    Handle = HANDLE_FROM_WORDS(getBP(), getBX());

    //
    // we have to collect the info to put in the PIPEINFO structure from
    // various sources - we stored the name (& name length) in a
    // OPEN_NAMED_PIPE_INFO structure. The other stuff we get from
    // GetNamedPipeInfo and GetNamedPipeHandleState
    //

    OpenNamedPipeInfo = VrpGetOpenNamedPipeInfo(Handle);
    if (OpenNamedPipeInfo) {
        bufLen = getCX();
        if (bufLen >= sizeof(PIPEINFO)) {
            Ok =  GetNamedPipeInfo(Handle,
                                   &Flags,
                                   &OutBufferSize,
                                   &InBufferSize,
                                   &MaxInstances
                                   );
            if (Ok) {

                //
                // we are only interested in the current # instances of the
                // named pipe from this next call
                //

                Ok = GetNamedPipeHandleState(Handle,
                                             NULL,
                                             &CurInstances,
                                             NULL,
                                             NULL,
                                             NULL,
                                             0
                                             );
                if (Ok) {
                    PipeInfo = (PIPEINFO*)POINTER_FROM_WORDS(getDS(), getSI());
                    WRITE_WORD(&PipeInfo->cbOut, (OutBufferSize > 65535 ? 65535 : OutBufferSize));
                    WRITE_WORD(&PipeInfo->cbIn, (InBufferSize > 65535 ? 65535 : InBufferSize));
                    WRITE_BYTE(&PipeInfo->cbMaxInst, (MaxInstances > 255 ? 255 : MaxInstances));
                    WRITE_BYTE(&PipeInfo->cbCurInst, (CurInstances > 255 ? 255 : CurInstances));
                    WRITE_BYTE(&PipeInfo->cbName, OpenNamedPipeInfo->NameLength);

                    //
                    // copy name if enough space
                    //

                    if (bufLen - sizeof(PIPEINFO) >= OpenNamedPipeInfo->NameLength) {
                        strcpy(PipeInfo->szName, OpenNamedPipeInfo->Name);
                    }
                    setCF(0);
                } else {
                    SET_ERROR(VrpMapLastError());
                }
            } else {
                SET_ERROR(VrpMapLastError());
            }
        } else {
            SET_ERROR(ERROR_BUFFER_OVERFLOW);
        }
    } else {

#if DBG
#line 386
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 392

        SET_ERROR(ERROR_INVALID_HANDLE);
    }

#if DBG
#line 397
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 418

}


VOID
VrGetNamedPipeHandleState(
    VOID
    )

/*++

Routine Description:

    Performs GetNamedPipeHandleState request on behalf of VDM redir

Arguments:

    Function = 5F33h

    ENTRY   BP:BX = 32-bit Named Pipe handle

    EXIT    CF = 1
                AX = Error code

            CF = 0
                AX = Pipe mode:
                        BSxxxWxRIIIIIIII

                        where:
                            B = Blocking mode. If B=1 the pipe is non blocking
                            S = Server end of pipe if 1
                            W = Pipe is written in message mode if 1 (else byte mode)
                            R = Pipe is read in message mode if 1 (else byte mode)
                            I = Pipe instances. Unlimited if 0xFF

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    HANDLE  Handle;
    DWORD   State, CurInstances, Flags, MaxInstances;
    BOOL    Ok;
    WORD    PipeHandleState;

#if DBG
#line 466
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 470

    Handle = HANDLE_FROM_WORDS(getBP(), getBX());
    Ok =  GetNamedPipeHandleState(Handle,
                                  &State,
                                  &CurInstances,
                                  NULL,
                                  NULL,
                                  NULL,
                                  0
                                  );
    if (Ok) {
        Ok = GetNamedPipeInfo(Handle, &Flags, NULL, NULL, &MaxInstances);
        if (Ok) {

            //
            // Create the Dos pipe handle state from the information gathered
            //

            PipeHandleState = (WORD)((MaxInstances > 255) ? 255 : (MaxInstances & 0xff))
                | (WORD)((State & PIPE_NOWAIT) ? NP_NBLK : 0)
                | (WORD)((State & PIPE_READMODE_MESSAGE) ? NP_RMESG : 0)

                //
                // BUGBUG - can't possibly be server end????
                //

                | (WORD)((Flags & PIPE_SERVER_END) ? NP_SERVER : 0)
                | (WORD)((Flags & PIPE_TYPE_MESSAGE) ? NP_WMESG : 0)
                ;

            setAX((WORD)PipeHandleState);
            setCF(0);
        } else {
            SET_ERROR(VrpMapLastError());
        }
    } else {
        SET_ERROR(VrpMapLastError());
    }
}


VOID
VrSetNamedPipeHandleState(
    VOID
    )

/*++

Routine Description:

    Performs SetNamedPipeHandleState request on behalf of VDM redir

Arguments:

    Function = 5F34h

    ENTRY   BP:BX = 32-bit Named Pipe handle
            CX = Pipe mode to set

    EXIT    CF = 1
                AX = Error code

            CF = 0
                AX = Pipe mode set

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    HANDLE  Handle = HANDLE_FROM_WORDS(getBP(), getBX());
    BOOL    Ok;
    WORD    DosPipeMode;
    DWORD   WinPipeMode;

#define ILLEGAL_NMP_SETMODE_BITS    ~(NP_NBLK | NP_RMESG | NP_WMESG)

#if DBG
#line 550
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 554

    //
    // Convert the Dos pipe mode bits to Win32 pipe mode bits. We can only
    // change the wait/no-wait status and the read mode of the pipe (byte
    // or message)
    //

    DosPipeMode = getCX();

    //
    // catch disallowed flags
    //

    if (DosPipeMode & ILLEGAL_NMP_SETMODE_BITS) {
        SET_ERROR(ERROR_INVALID_PARAMETER);
        return;
    }

    WinPipeMode = ((DosPipeMode & NP_NBLK)
                    ? PIPE_NOWAIT
                    : PIPE_WAIT)
                | ((DosPipeMode & NP_RMESG)
                    ? PIPE_READMODE_MESSAGE
                    : PIPE_READMODE_BYTE);
    if (!(Ok = SetNamedPipeHandleState(Handle, &WinPipeMode, NULL, NULL))) {

#if DBG
#line 581
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 587

        SET_ERROR(VrpMapLastError());
    } else {
        setCF(0);
    }
}


VOID
VrPeekNamedPipe(
    VOID
    )

/*++

Routine Description:

    Performs PeekNamedPipe request on behalf of VDM redir

Arguments:

    Function = 5F35h

    ENTRY   BP:BX = 32-bit Named Pipe handle
            CX = Size of buffer for peek
            DS:SI = Buffer address

    EXIT    CF = 1
                AX = Error code

            CF = 0
                AX = Pipe status
                BX = Number of bytes peeked into buffer
                CX = Number of bytes in pipe
                DX = Number of bytes in message
                DI = Pipe status
                DS:SI = Data peeked

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    HANDLE Handle;
    LPBYTE lpBuffer;
    DWORD nBufferSize, BytesRead, BytesAvailable, BytesLeftInMessage;
    BOOL Ok;

#if DBG
#line 638
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 647

    Handle = HANDLE_FROM_WORDS(getBP(), getBX());
    lpBuffer = (LPBYTE)POINTER_FROM_WORDS(getDS(), getSI());
    nBufferSize = (DWORD)getCX();
    Ok = PeekNamedPipe(Handle,
                       lpBuffer,
                       nBufferSize,
                       &BytesRead,
                       &BytesAvailable,
                       &BytesLeftInMessage
                       );
    if (Ok) {

        //
        // Since we gave a 16-bit quantity for the buffer size, BytesRead
        // cannot be >64K
        //

        setBX((WORD)BytesRead);
        setCX((WORD)BytesAvailable);

        //
        // if message mode pipe, return total bytes in message (as opposed to
        // NT's bytes LEFT in message)
        //

        setDX(BytesLeftInMessage ? ((WORD)BytesLeftInMessage + (WORD)BytesRead) : 0);

        //
        // Not sure what this means. According to NETPIAPI.ASM, a 3 is returned
        // on success, meaning status = connected. The named pipe statuses are
        // (according to BSEDOS.H):
        //
        //      NP_DISCONNECTED 1
        //      NP_LISTENING    2
        //      NP_CONNECTED    3
        //      NP_CLOSING      4
        //
        // Presumably, a client-side pipe can only be connected or the pipe is
        // closed
        //

        setDI(NP_CONNECTED);
        setCF(0);

#if DBG
#line 693
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 701

    } else {
        SET_ERROR(VrpMapLastError());

#if DBG
#line 706
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 710

        BytesRead = 0;
    }

    //
    // idle processing - only idle if there is nothing to return (including
    // an error occurred)
    //
    // For now, allow 10(!) peeks per second - on ALL pipe handles
    //

    if (!BytesRead) {
        if (GetTickCount() - VrPeekNamedPipeTickCount < 100) {
            WaitIfIdle();
        }
    }
    VrPeekNamedPipeTickCount = GetTickCount();
}


VOID
VrTransactNamedPipe(
    VOID
    )

/*++

Routine Description:

    Performs TransactNamedPipe request on behalf of VDM redir

Arguments:

    Function = 5F36h

    ENTRY   BP:BX = 32-bit Named Pipe handle
            CX = Transmit buffer length
            DX = Receive buffer length
            DS:SI = Transmit buffer
            ES:DI = Receive buffer

    EXIT    CF = 1
                AX = Error code

            CF = 0
                CX = Number of bytes in Receive buffer


Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    DWORD BytesRead;
    BOOL Ok;
    OVERLAPPED_PIPE_IO pipeio;
    DWORD Error;
    HANDLE Handle;

#if DBG
#line 772
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 784

    //
    // now that we are opening named pipes with FLAG_FILE_OVERLAPPED, we have
    // to perform every I/O operation with an OVERLAPPED structure. We are only
    // interested in the event handle. We create a new event for synchronous
    // operation which requires an OVERLAPPED structure. Create the event to
    // be manually reset - this way, if we wait on it & the read has already
    // completed, the wait completes immediately. If we create an auto-reset
    // event, then it may go back into the not-signalled state, causing us to
    // wait forever for an event that has already occurred
    //

    RtlZeroMemory(&pipeio, sizeof(pipeio));
    pipeio.Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (pipeio.Overlapped.hEvent != NULL) {

        //
        // collect arguments from registers and perform transact named pipe call
        //

        Handle = HANDLE_FROM_WORDS(getBP(), getBX());
        RememberPipeIo(&pipeio);
        /* DIVERGENCE(MVDM-HOST-DIV-277): modern TransactNamedPipe rejects
         * lpBytesRead when an OVERLAPPED is supplied.  The original next
         * step already retrieves the authoritative count from the completed
         * OVERLAPPED request. */
#line 806
        Ok = TransactNamedPipe(Handle,
                               (LPVOID)POINTER_FROM_WORDS(getDS(), getSI()),
                               (DWORD)getCX(),
                               (LPVOID)POINTER_FROM_WORDS(getES(), getDI()),
                               (DWORD)getDX(),
                               NULL,
                               &pipeio.Overlapped
                               );
        Error = Ok ? NO_ERROR : GetLastError();
        if (Error == ERROR_IO_PENDING) {

#if DBG
#line 818
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 822

            Error = WaitForSingleObject(pipeio.Overlapped.hEvent, NAMED_PIPE_TIMEOUT);
        }
        ForgetPipeIo(&pipeio);
        if (pipeio.Cancelled) {
            Error = WAIT_TIMEOUT;
        }
        if (Error == NO_ERROR || Error == ERROR_MORE_DATA) {
            /* A signalled OVERLAPPED event only says that the operation has
             * completed.  Modern named-pipe completion can still fail (for
             * example a peer disconnect); preserve the original result path,
             * but do not publish an uninitialised/zero byte count as success. */
            Ok = GetOverlappedResult(Handle, &pipeio.Overlapped, &BytesRead, TRUE);
            if (!Ok) {
                Error = GetLastError();
            }
        }
        if (Error == NO_ERROR || Error == ERROR_MORE_DATA) {

#if DBG
#line 833
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 837

            setCX((WORD)BytesRead);
            setAX((WORD)Error);

            //
            // if we are returning NO_ERROR then carry flag is clear, else we
            // are returning ERROR_MORE_DATA: set carry flag
            //

            setCF(Error == ERROR_MORE_DATA);
        } else {

            //
            // if we timed-out then close the pipe handle
            //

            if (Error == WAIT_TIMEOUT) {

#if DBG
#line 856
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 860
                CloseHandle(Handle);
                VrpRemoveOpenNamedPipeInfo(Handle);
            } else {
                Error = VrpMapLastError();
            }
            SET_ERROR((WORD)Error);

#if DBG
#line 868
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 872
        }

        //
        // kill the event handle
        //

        CloseHandle(pipeio.Overlapped.hEvent);
    } else {

        //
        // failed to create event handle
        //

        SET_ERROR(VrpMapLastError());
    }
}


VOID
VrCallNamedPipe(
    VOID
    )

/*++

Routine Description:

    Performs CallNamedPipe request on behalf of VDM redir

Arguments:

    Function = 5F37h

    ENTRY   DS:SI = Pointer to CallNmPipe structure:

                DWORD   Timeout;            +0
                LPWORD  lpBytesRead;        +4
                WORD    OutputBufferLen;    +8
                LPBYTE  OutputBuffer;       +10
                WORD    InputBufferLength;  +14
                LPBYTE  InputBuffer;        +16
                LPSTR   PipeName;           +20

    EXIT    CF = 1
                AX = Error code

            CF = 0
                CX = Bytes received

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    BOOL Ok;
    DWORD BytesRead;
    PDOS_CALL_NAMED_PIPE_STRUCT StructurePointer;


    StructurePointer = (PDOS_CALL_NAMED_PIPE_STRUCT)POINTER_FROM_WORDS(getDS(), getSI());

#if DBG
#line 936
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 940

    Ok = CallNamedPipe((LPSTR)READ_FAR_POINTER(&StructurePointer->lpPipeName),
                        READ_FAR_POINTER(&StructurePointer->lpInBuffer),
                        READ_WORD(&StructurePointer->nInBufferLen),
                        READ_FAR_POINTER(&StructurePointer->lpOutBuffer),
                        READ_WORD(&StructurePointer->nOutBufferLen),
                        &BytesRead,
                        READ_DWORD(&StructurePointer->Timeout)
                        );
    if (!Ok) {
        SET_ERROR(VrpMapLastError());

#if DBG
#line 953
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 957
    } else {
        WRITE_WORD(READ_FAR_POINTER(&StructurePointer->lpBytesRead), (WORD)BytesRead);
        setCX((WORD)BytesRead);
        setCF(0);

#if DBG
#line 963
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 967
    }
}


VOID
VrWaitNamedPipe(
    VOID
    )

/*++

Routine Description:

    Performs WaitNamedPipe request on behalf of VDM redir. We assume that the
    name we are getting is \\computer\pipe\name, anything else is invalid

Arguments:

    Function = 5F38h

    ENTRY   BX:CX = Timeout
            DS:DX = Pipe name

    EXIT    CF = 1
                AX = Error code

            CF = 0
                No error

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    BOOL Ok;

    //
    // BUGBUG - should really perform DosPathCanonicalization on input string -
    // DOS redir would convert eg //server/pipe\foo.bar into \\SERVER\PIPE\FOO.BAR
    // if it makes any difference
    //

#if DBG
#line 1012
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1019

    Ok = WaitNamedPipe(LPSTR_FROM_WORDS(getDS(), getDX()),
                        DWORD_FROM_WORDS(getBX(), getCX())
                        );
    if (!Ok) {
        SET_ERROR(VrpMapLastError());
    } else {

#if DBG
#line 1028
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1032
        setAX(0);
        setCF(0);
    }
}


VOID
VrNetHandleGetInfo(
    VOID
    )

/*++

Routine Description:

    Performs local NetHandleGetInfo on behalf of the Vdm client

Arguments:

    Function = 5F3Ch

    ENTRY   BP:BX = 32-bit Named Pipe handle
            CX = Buffer length
            SI = Level (1)
            DS:DX = Buffer

    EXIT    CX = size of required buffer (whether we got it or not)
            CF = 1
                AX = Error code

            CF = 0
                indicated stuff put in buffer

Return Value:

    None. Results returned via VDM registers or in VDM memory, according to
    request

--*/

{
    HANDLE  Handle;
    DWORD   Level;
    DWORD   BufLen;
    BOOL    Ok;
    DWORD   CollectCount;
    DWORD   CollectTime;
    LPVDM_HANDLE_INFO_1 StructurePointer;

#if DBG
#line 1082
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1086

    Handle = HANDLE_FROM_WORDS(getBP(), getBX());
    Level = (DWORD)getSI();
    if (Level == 1) {
        BufLen = (DWORD)getCX();
        if (BufLen >= sizeof(VDM_HANDLE_INFO_1)) {

            //
            // BUGBUG - the information we are interested in cannot be returned
            // if the client and server are on the same machine, or if this is
            // the server end of the pipe???
            //

            Ok = GetNamedPipeHandleState(Handle,
                                            NULL,   // not interested in state
                                            NULL,   // ditto curInstances
                                            &CollectCount,
                                            &CollectTime,
                                            NULL,   // not interested in client app name
                                            0
                                            );
            if (!Ok) {
                SET_ERROR(VrpMapLastError());
            } else {
                StructurePointer = (LPVDM_HANDLE_INFO_1)POINTER_FROM_WORDS(getDS(), getDX());
                StructurePointer->CharTime = CollectTime;
                StructurePointer->CharCount = (WORD)CollectCount;
                setCF(0);
            }
        } else {
            SET_ERROR(NERR_BufTooSmall);
        }
    } else {
        SET_ERROR(ERROR_INVALID_LEVEL);
    }
}


VOID
VrNetHandleSetInfo(
    VOID
    )

/*++

Routine Description:

    Performs local NetHandleSetInfo on behalf of the Vdm client

Arguments:

    Function = 5F3Bh

    ENTRY   BP:BX = 32-bit Named Pipe handle
            CX = Buffer length
            SI = Level (1)
            DI = Parmnum
            DS:DX = Buffer

    EXIT    CF = 1
                AX = Error code

            CF = 0
                Stuff from buffer set

Return Value:

    None. Results returned via VDM registers or in VDM memory, according to
    request

--*/

{
    HANDLE  Handle;
    DWORD   Level;
    DWORD   BufLen;
    BOOL    Ok;
    DWORD   Data;
    DWORD   ParmNum;
    LPBYTE  Buffer;

#if DBG
#line 1168
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1173

    Handle = HANDLE_FROM_WORDS(getBP(), getBX());
    Level = (DWORD)getSI();
    Buffer = LPBYTE_FROM_WORDS(getDS(), getDX());
    if (Level == 1) {
        BufLen = (DWORD)getCX();

        //
        // ParmNum can be 1 (CharTime) or 2 (CharCount), Can't be 0 (set
        // everything)
        //

        ParmNum = (DWORD)getDI();
        if (!--ParmNum) {
            if (BufLen < sizeof(((LPVDM_HANDLE_INFO_1)0)->CharTime)) {
                SET_ERROR(NERR_BufTooSmall);
                return ;
            }
            Data = (DWORD)*(LPDWORD)Buffer;
        } else if (!--ParmNum) {
            if (BufLen < sizeof(((LPVDM_HANDLE_INFO_1)0)->CharCount)) {
                SET_ERROR(NERR_BufTooSmall);
                return ;
            }
            Data = (DWORD)*(LPWORD)Buffer;
        } else {
            SET_ERROR(ERROR_INVALID_PARAMETER);
            return ;
        }

        //
        // BUGBUG - the information we are interested in cannot be set
        // if the client and server are on the same machine, or if this is
        // the server end of the pipe???
        //

        Ok = SetNamedPipeHandleState(Handle,
                                        NULL,   // not interested in mode
                                        (LPDWORD)((ParmNum == 1) ? &Data : NULL),
                                        (LPDWORD)((ParmNum == 2) ? &Data : NULL)
                                        );
        if (!Ok) {
            SET_ERROR(VrpMapLastError());
        } else {
            setCF(0);
        }
    } else {
        SET_ERROR(ERROR_INVALID_LEVEL);
    }
}


//
// Request Queue. This queue holds a singly linked list of async named pipe
// read/write requests. The async thread will search this list when an async
// read or write completes (the event is signalled). It then sets up the
// information for the call back to the VDM and dequeues the request info.
// Because we can have the async thread and the request thread simultaneously
// accessing the queue, it is protected by a critical section
//

CRITICAL_SECTION VrNmpRequestQueueCritSec;
PDOS_ASYNC_NAMED_PIPE_INFO RequestQueueHead = NULL;
PDOS_ASYNC_NAMED_PIPE_INFO RequestQueueTail = NULL;
HANDLE VrpNmpSomethingToDo;


VOID
VrReadWriteAsyncNmPipe(
    VOID
    )

/*++

Routine Description:

    Performs asynchronous read or write of a message mode named pipe on behalf
    of the VDM DOS application

Arguments:

    None. All arguments are extracted from DOS registers/memory.

    These calls are made through int 2fh/ax=function code, not int 21h/ah=5fh

        AX = 1186h  DosReadAsyncNmPipe
             118Fh  DosWriteAsyncNmPipe
             1190h  DosReadAsyncNmPipe2
             1191h  DosWriteAsyncNmPipe2

        BP:BX = 32-bit Named Pipe Handle

        DS:SI = DOS_ASYNC_NAMED_PIPE_STRUCT
            DD  address of returned bytes read
            DW  size of caller's buffer
            DD  address of caller's buffer
            DD  address of returned error code
            DD  address of Asynchronous Notification Routine
            DW  named pipe handle
            DD  address of caller's 'semaphore'

Return Value:

    None.

--*/

{
    HANDLE  Handle;

    //
    // Type is type of request - read or write, standard or 2 (meaning the
    // request has an associated 'semaphore' which must be cleared)
    //

    DWORD   Type;

    //
    // pAsyncInfo is a pointer to the request packet we stick on the request
    // queue
    //

#line 1302
    PDOS_ASYNC_NAMED_PIPE_INFO pAsyncInfo;

    //
    // pipeInfo is a pointer to the information we created/stored when the
    // named pipe was opened. We just need this to check the handle's valid
    //

    POPEN_NAMED_PIPE_INFO pipeInfo;

    WORD    length;
    LPBYTE  buffer;
    DWORD   error;
    BOOL    ok;
    HANDLE  hEvent;
    DWORD   bytesTransferred;

    //
    // hThread and tid: these must be kept alive so long as the async named
    // pipe (completion) thread exists. tid can be used with ResumeThread and
    // SuspendThread as we may see fit
    //

    static HANDLE hThread = NULL;
    static DWORD tid;

    //
    // get info from registers and the async named pipe structure
    //

    Handle = HANDLE_FROM_WORDS(getBP(), getBX());
    pipeInfo = VrpGetOpenNamedPipeInfo(Handle);
    Type = (DWORD)getAX() & 0xff;   // 0x86, 0x8f, 0x90 or 0x91

#if DBG
#line 1339
#error DIVERGENCE MVDM-HOST-DIV-517: DBG debug branch is not selected; see mirror register.
#endif
#line 1379

    //
    // if we can't find this handle in our list of opened named pipes, return
    // an error
    //

    if (!pipeInfo) {

#if DBG
#line 1388
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1392

        SET_ERROR(ERROR_INVALID_HANDLE);
        return;
    }

    //
    // looks like we're going to make an async read/write request. Create the
    // async thread if it doesn't already exist. Create also the "something to
    // do" event. Create this as an auto reset event which is initially in the
    // not-signalled state
    //

    if (hThread == NULL) {
        VrpNmpSomethingToDo = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (VrpNmpSomethingToDo == NULL) {

#if DBG
#line 1409
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1415

            //
            // return an out-of-resources error
            //

            SET_ERROR(ERROR_NOT_ENOUGH_MEMORY);
            return;
        }

        //
        // we have the "something to do" event. Now create the thread
        //

        hThread = CreateThread(NULL,
                               0,
                               VrpAsyncNmPipeThread,
                               NULL,
                               0,
                               &tid
                               );
        if (hThread == NULL) {

#if DBG
#line 1438
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1444

            CloseHandle(VrpNmpSomethingToDo);
            SET_ERROR(ERROR_NOT_ENOUGH_MEMORY);
            return;
        }
    }

    //
    // allocate a structure in which to store the information required to
    // complete the request (in the VDM)
    //

    pAsyncInfo = (PDOS_ASYNC_NAMED_PIPE_INFO)LocalAlloc(LMEM_FIXED, sizeof(DOS_ASYNC_NAMED_PIPE_INFO));
    if (pAsyncInfo == NULL) {

#if DBG
#line 1460
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1464

        SET_ERROR(ERROR_NOT_ENOUGH_MEMORY);
        return;
    }

    RtlZeroMemory(&pAsyncInfo->Overlapped, sizeof(pAsyncInfo->Overlapped));
    /* DIVERGENCE(MVDM-HOST-DIV-167): LocalAlloc does not promise a zeroed
     * private field. The overlay uses this member solely as ownership state. */
    pAsyncInfo->PrivateAsyncState = NULL;

    //
    // create a new event for this request - there can be multiple simultaneous
    // requests per named pipe. The event is manual reset so that if the request
    // completes before the WaitForMultipleObjects snaps the list, the event
    // will stay reset and hence the wait will complete. If we created the event
    // as auto-reset, it may get signalled, and go not-signalled before we wait
    // on it, potentially causing an infinite wait
    //

#line 1480
    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hEvent == NULL) {

#if DBG
#line 1484
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1488

        LocalFree((HLOCAL)pAsyncInfo);

        //
        // return approximation out-of-resources error
        //

        SET_ERROR(ERROR_NOT_ENOUGH_MEMORY);
        return;
    } else {
        pAsyncInfo->Overlapped.hEvent = hEvent;
    }

    //
    // set up rest of async operation info structure
    //

    pAsyncInfo->Completed = FALSE;
    pAsyncInfo->Handle = Handle;
    if (!mvdm_redirector_async_prepare(pAsyncInfo, getDS(), getSI(), Type,
            &buffer, &length)) {
        CloseHandle(hEvent);
        LocalFree(pAsyncInfo);
        SET_ERROR(ERROR_INVALID_ADDRESS);
        return;
#line 1523
    }

#if DBG
#line 1526
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1528

    //
    // add the completion info structure to the async thread's work queue
    //

    VrpQueueAsyncRequest(pAsyncInfo);

    //
    // Q: what happens if the request completes asynchronously before we finish
    // this routine?
    //

    if (Type == ANP_READ || Type == ANP_READ2) {
        ok = ReadFile(Handle,
                      buffer,
                      length,
                      &bytesTransferred,
                      &pAsyncInfo->Overlapped
                      );

#if DBG
#line 1549
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1558

    } else {
        ok = WriteFile(Handle,
                       buffer,
                       length,
                       &bytesTransferred,
                       &pAsyncInfo->Overlapped
                       );

#if DBG
#line 1568
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1577

    }
    error = ok ? NO_ERROR : GetLastError();

    //
    // if we get ERROR_MORE_DATA then treat it as an error. GetOverlappedResult
    // will give us the same error which we will return asynchronously
    //

    if (error != NO_ERROR && error != ERROR_IO_PENDING && error != ERROR_MORE_DATA) {

        //
        // we didn't get to start the I/O operation successfully, therefore
        // we won't get called back, so we dequeue and free the completion
        // structure and return the error
        //

#if DBG
#line 1595
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1599

        VrpDequeueAsyncRequest(pAsyncInfo);
        CloseHandle(hEvent);
        mvdm_redirector_async_release(pAsyncInfo);
#line 1602
        LocalFree(pAsyncInfo);
        SET_ERROR((WORD)error);
    } else {

#if DBG
#line 1607
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1613

        setCF(0);
    }

#if DBG
#line 1618
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1622
}


BOOLEAN
VrNmPipeInterrupt(
    VOID
    )

/*++

Routine Description:

    Called from hardware interrupt BOP processing to check if there are any
    async named pipe ANRs to call

Arguments:

    None.

Return Value:

    BOOLEAN
        TRUE    - there was an async named pipe operation to complete. The
                  VDM registers & data areas have been modified to indicate
                  that the named pipe ANR must be called

        FALSE   - no async named pipe processing to do. Interrupt must have
                  been generated by NetBios or DLC

--*/

{
    PDOS_ASYNC_NAMED_PIPE_INFO pAsyncInfo;

    mvdm_redirector_async_trace("interrupt-enter");

#if DBG
#line 1657
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1661


    //
    // locate the first async named pipe request packet that has completed and
    // is waiting for interrupt processing (ie its ANR to be called)
    //

    pAsyncInfo = VrpFindCompletedRequest();
    if (!pAsyncInfo) {
        mvdm_redirector_async_trace("interrupt-empty");

#if DBG
#line 1672
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1677

        //
        // returning FALSE indicates that the hardware interrupt callback was
        // not generated by async named pipe request completing
        //

#line 1684
        return FALSE;
    } else {
        mvdm_redirector_async_trace("interrupt-dispose");

        //
        // set the VDM registers to indicate a named pipe callback
        //

#line 1691
        setDS(HIWORD(pAsyncInfo->Buffer));
        setSI(LOWORD(pAsyncInfo->Buffer));
        setES(HIWORD(pAsyncInfo->Semaphore));
        setDI(LOWORD(pAsyncInfo->Semaphore));
        setCX(HIWORD(pAsyncInfo->ANR));
        setBX(LOWORD(pAsyncInfo->ANR));
        setAL((BYTE)pAsyncInfo->Type2);
        SET_CALLBACK_NAMEPIPE();

        //
        // finished with this request packet, so dequeue and deallocate it
        //

        VrpDequeueAsyncRequest(pAsyncInfo);
        CloseHandle(pAsyncInfo->Overlapped.hEvent);
        mvdm_redirector_async_release(pAsyncInfo);
#line 1706
        LocalFree(pAsyncInfo);
        mvdm_redirector_async_trace("interrupt-callback-ready");

#if DBG
#line 1709
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1719

        //
        // returning TRUE indicates that we have accepted a named pipe
        // completion request
        //

        //VrDismissInterrupt();
        return TRUE;
    }
}


VOID
VrTerminateNamedPipes(
    IN WORD DosPdb
    )

/*++

Routine Description:

    Cleans out all open named pipe info and pending async named pipe requests
    when the owning DOS app terminates

Arguments:

    DosPdb  - PDB (DOS 'process' identifier) of terminating DOS process

Return Value:

    None.

--*/

{
#if DBG
#line 1755
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1758
}


VOID
VrCancelPipeIo(
    IN DWORD Thread
    )

/*++

Routine Description:

    For all pending named pipe I/Os owned by Thread, mark them as cancelled
    and signal the event in the OVERLAPPED structure, causing the wait to
    terminate.

    This thread may not have any outstanding named pipe I/O

Arguments:

    Thread  - pseudo-handle of thread owning named pipe I/O

Return Value:

    None.

--*/

{
    POVERLAPPED_PIPE_IO ptr;

    EnterCriticalSection(&VrNamedPipeCancelCritSec);
    for (ptr = PipeIoQueue; ptr; ptr = ptr->Next) {
        if (ptr->Thread == Thread) {
            ptr->Cancelled = TRUE;
            SetEvent(ptr->Overlapped.hEvent);
        }
    }
    LeaveCriticalSection(&VrNamedPipeCancelCritSec);
}


PRIVATE
DWORD
VrpAsyncNmPipeThread(
    IN LPVOID Parameters
    )

/*++

Routine Description:

    Waits for an asynchronous named pipe read or write operation to complete.
    Loops forever, waiting on list of pending async (overlapped) named pipe
    operations. If there are no more outstanding named pipe read/writes then
    waits on VrpNmpSomethingToDo which is reset (put in not-signalled state)
    when there are no packets left on the request queue

Arguments:

    Parameters  - unused parameter block

Return Value:

    DWORD
        0

--*/

{
    DWORD numberOfHandles;
    DWORD index;
    HANDLE eventList[MAXIMUM_ASYNC_PIPES + 1];
    PDOS_ASYNC_NAMED_PIPE_INFO pRequest;

    UNREFERENCED_PARAMETER(Parameters);

#if DBG
#line 1837
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1841

    while (TRUE) {

        //
        // create an array of event handles. The first handle in the array is
        // the "something to do" event. This will only be reset when the queue
        // of requests changes from the empty set
        //

        numberOfHandles = VrpSnapshotEventList(eventList);
        index = WaitForMultipleObjects(numberOfHandles, eventList, FALSE, INFINITE);

        //
        // if the index is 0, then the "something to do" event has been signalled,
        // meaning that we have to snapshot a new event list and re-wait
        //

        if (index > 0 && index < numberOfHandles) {

#if DBG
#line 1861
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1865

            pRequest = VrpSearchForRequestByEventHandle(eventList[index]);
            if (pRequest != NULL) {
                VrpCompleteAsyncRequest(pRequest);
            }

#if DBG
#line 1872
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1880

        } else if (index) {

            //
            // an error occurred
            //

#if DBG
#line 1888
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1895

        }

#if DBG
#line 1899
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1905

    }

#if DBG
#line 1909
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1913

    return 0;   // appease the compiler-god
}


PRIVATE
DWORD
VrpSnapshotEventList(
    OUT LPHANDLE pList
    )

/*++

Routine Description:

    Builds an array of event handles for those asynchronous named pipe I/O
    requests which are still not completed (the Completed flag is FALSE).
    The first event handle is always the "something to do" event

Arguments:

    pList   - pointer to callers list to build

Return Value:

    DWORD

--*/

{
    DWORD count = 1;
    PDOS_ASYNC_NAMED_PIPE_INFO ptr;

#if DBG
#line 1947
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1951

    pList[0] = VrpNmpSomethingToDo;
    EnterCriticalSection(&VrNmpRequestQueueCritSec);
    for (ptr = RequestQueueHead; ptr; ptr = ptr->Next) {
        if (ptr->Completed == FALSE) {
            pList[count++] = ptr->Overlapped.hEvent;
        }
    }
    LeaveCriticalSection(&VrNmpRequestQueueCritSec);

#if DBG
#line 1962
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1966

    return count;
}


PRIVATE
PDOS_ASYNC_NAMED_PIPE_INFO
VrpSearchForRequestByEventHandle(
    IN HANDLE EventHandle
    )

/*++

Routine Description:

    Searches the async request queue for the structure with this event handle.
    If the structure is found, it is marked as Completed. The required structure
    may NOT be located: this might occur when an item was removed from the list
    due to an error in VrReadWriteAsyncNmPipe

Arguments:

    EventHandle - to search for

Return Value:

    PDOS_ASYNC_NAMED_PIPE_INFO
        Success - the located structure
        Failure - NULL

--*/

{
    PDOS_ASYNC_NAMED_PIPE_INFO ptr;

#if DBG
#line 2002
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2006

    EnterCriticalSection(&VrNmpRequestQueueCritSec);
    for (ptr = RequestQueueHead; ptr; ptr = ptr->Next) {
        if (ptr->Overlapped.hEvent == EventHandle) {
            ptr->Completed = TRUE;
            break;
        }
    }
    LeaveCriticalSection(&VrNmpRequestQueueCritSec);

#if DBG
#line 2017
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2034

    return ptr;
}


PRIVATE
VOID
VrpCompleteAsyncRequest(
    IN PDOS_ASYNC_NAMED_PIPE_INFO pAsyncInfo
    )

/*++

Routine Description:

    Completes the asynchronous named pipe I/O request by getting the results
    of the transfer and filling-in the error & bytes transferred fields of
    the async named pipe info structure. If there is an ANR to be called, then
    a simulated hardware interrupt request is generated to the VDM. If there
    is no ANR to call then the async named pipe info structure is cleared out.

    If there is an ANR, the request will be completed finally when it is
    dequeued & freed by VrNmPipeInterrupt

Arguments:

    pAsyncInfo  - pointer to DOS_ASYNC_NAMED_PIPE_INFO structure to complete

Return Value:

    None.

--*/

{
    BOOL ok;
    DWORD bytesTransferred;
    DWORD error;

    if (!mvdm_redirector_async_completion_begin(pAsyncInfo)) {
        VrpDequeueAsyncRequest(pAsyncInfo);
        CloseHandle(pAsyncInfo->Overlapped.hEvent);
        mvdm_redirector_async_release(pAsyncInfo);
        LocalFree(pAsyncInfo);
        return;
    }

#if DBG
#line 2074
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2080

    ok = GetOverlappedResult(pAsyncInfo->Handle,
                             &pAsyncInfo->Overlapped,
                             &bytesTransferred,
                             FALSE
                             );
    error = ok ? NO_ERROR : GetLastError();

    //
    // update the VDM variables
    //

    if (!mvdm_redirector_async_complete(pAsyncInfo, bytesTransferred, error)) {
        /* DIVERGENCE(MVDM-HOST-DIV-167): guest memory may have disappeared
         * with its session while host I/O was completing. Do not publish a
         * stale completion or keep an alias past that event. */
        VrpDequeueAsyncRequest(pAsyncInfo);
        CloseHandle(pAsyncInfo->Overlapped.hEvent);
        mvdm_redirector_async_completion_end(pAsyncInfo);
        mvdm_redirector_async_release(pAsyncInfo);
        LocalFree(pAsyncInfo);
        return;
    }

#if DBG
#line 2096
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2105

    //
    // if there is no ANR then we cannot make a call-back to DOS (error? DOS
    // app polls 'semaphore'?) so close the event handle, dequeue the request
    // packet and free it
    //

    if (!pAsyncInfo->ANR) {

#if DBG
#line 2115
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#else
#line 2124

        VrpDequeueAsyncRequest(pAsyncInfo);

#endif
#line 2128

        CloseHandle(pAsyncInfo->Overlapped.hEvent);
        mvdm_redirector_async_completion_end(pAsyncInfo);
        mvdm_redirector_async_release(pAsyncInfo);
#line 2130
        LocalFree(pAsyncInfo);
    } else {

        //
        // interrupt the VDM. It must call back to find out what asynchronous
        // processing there is to do
        //

#if DBG
#line 2139
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2143

        VrQueueCompletionHandler(VrNmPipeInterrupt);
        mvdm_redirector_async_trace("completion-queued");
#line 2145
        VrRaiseInterrupt();
        mvdm_redirector_async_trace("completion-raised");
        mvdm_redirector_async_completion_end(pAsyncInfo);
#line 2146
    }
}


PRIVATE
VOID
VrpQueueAsyncRequest(
    IN PDOS_ASYNC_NAMED_PIPE_INFO pAsyncInfo
    )

/*++

Routine Description:

    Adds a DOS_ASYNC_NAMED_PIPE_INFO structure to the end of the request queue.
    The queue is protected by a critical section

Arguments:

    pAsyncInfo  - pointer to structure to add

Return Value:

    None.

--*/

{
#if DBG
#line 2175
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2179

    EnterCriticalSection(&VrNmpRequestQueueCritSec);
    if (!RequestQueueHead) {
        RequestQueueHead = pAsyncInfo;

        //
        // the set is changing state from empty set to not empty set. Set the
        // "something to do" event. Note: it is OK to do this here (before we
        // have finished updating the queue info): because the async thread
        // must gain this critical section before it can access the request
        // queue
        //

//        PulseEvent(VrpNmpSomethingToDo);
    } else {
        RequestQueueTail->Next = pAsyncInfo;
    }
    pAsyncInfo->Next = NULL;
    RequestQueueTail = pAsyncInfo;
    SetEvent(VrpNmpSomethingToDo);
    LeaveCriticalSection(&VrNmpRequestQueueCritSec);
}


PRIVATE
PDOS_ASYNC_NAMED_PIPE_INFO
VrpDequeueAsyncRequest(
    IN PDOS_ASYNC_NAMED_PIPE_INFO pAsyncInfo
    )

/*++

Routine Description:

    Removes the DOS_ASYNC_NAMED_PIPE_INFO structure pointed at by pAsyncInfo
    from the request queue. Protected by critical section

Arguments:

    pAsyncInfo  - pointer to DOS_ASYNC_NAMED_PIPE_INFO to remove

Return Value:

    PDOS_ASYNC_NAMED_PIPE_INFO
        Success - pAsyncInfo is returned
        Failure - NULL - AsyncInfo wasn't found on the queue

--*/

{
    PDOS_ASYNC_NAMED_PIPE_INFO ptr;
    PDOS_ASYNC_NAMED_PIPE_INFO prev = (PDOS_ASYNC_NAMED_PIPE_INFO)&RequestQueueHead;

#if DBG
#line 2233
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2237

    EnterCriticalSection(&VrNmpRequestQueueCritSec);
    for (ptr = RequestQueueHead; ptr; ptr = ptr->Next) {
        if (ptr == pAsyncInfo) {
            break;
        } else {
            prev = ptr;
        }
    }
    if (ptr) {
        prev->Next = ptr->Next;
        if (RequestQueueTail == ptr) {
            RequestQueueTail = prev;
        }
    }

    //
    // if this was the last item on the queue (in the set) then the set has
    // changed state from not empty to empty set. Reset the "something to do"
    // event to stop the async thread until another request arrives. Note: it
    // is safe to reset the event here
    //

//    ResetEvent(VrpNmpSomethingToDo);
    LeaveCriticalSection(&VrNmpRequestQueueCritSec);

#if DBG
#line 2264
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2268

    return ptr;
}


PRIVATE
PDOS_ASYNC_NAMED_PIPE_INFO
VrpFindCompletedRequest(
    VOID
    )

/*++

Routine Description:

    Tries to locate the first request packet in the queue with the Completed
    field set, meaning the I/O request has completed and is waiting to generate
    a callback

Arguments:

    None.

Return Value:

    PDOS_ASYNC_NAMED_PIPE_INFO
        Success - pointer to request packet to complete
        Failure - NULL

--*/

{
    PDOS_ASYNC_NAMED_PIPE_INFO ptr;

#if DBG
#line 2303
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2307

    EnterCriticalSection(&VrNmpRequestQueueCritSec);
    for (ptr = RequestQueueHead; ptr; ptr = ptr->Next) {
        if (ptr->Completed) {
            break;
        }
    }
    LeaveCriticalSection(&VrNmpRequestQueueCritSec);

#if DBG
#line 2317
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2334

    return ptr;
}


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
#line 2374
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2378

    if (VrIsNamedPipeName(lpFileName)) {

#if DBG
#line 2382
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2386

        //
        // if we can't create the named pipe info structure, or the async
        // read/write event, return FALSE which results in an out-of-resources
        // error (not enough memory) since DOS doesn't understand about events
        //

        ok = VrpAddOpenNamedPipeInfo(Handle, lpFileName);
    } else {

#if DBG
#line 2397
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2401

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
#line 2438
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#else
#line 2454
    VrpRemoveOpenNamedPipeInfo(Handle);
#endif
#line 2456

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
    DWORD dwBytesRead;

#if DBG
#line 2500
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2504

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

    RememberPipeIo(&pipeio);
    /* DIVERGENCE(MVDM-HOST-DIV-276): NT4 accepted an output count together
     * with OVERLAPPED.  Modern ReadFile requires it to be NULL; the original
     * next step already obtains the authoritative count from
     * GetOverlappedResult. */
    success = ReadFile(Handle, Buffer, Buflen, NULL, &pipeio.Overlapped);
#line 2527
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
#line 2540
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2544

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

    ForgetPipeIo(&pipeio);
    if (pipeio.Cancelled) {
        error = WAIT_TIMEOUT;
        success = FALSE;
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
        CloseHandle(Handle);
        VrpRemoveOpenNamedPipeInfo(Handle);
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
#line 2604
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2608

        SetLastError(error);
    } else {
        *BytesRead = dwBytesRead;

#if DBG
#line 2614
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2618

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
#line 2667
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2671

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
        RememberPipeIo(&pipeio);
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
        ForgetPipeIo(&pipeio);
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
#line 2716
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2720

        SetLastError(error);
        if (error == WAIT_TIMEOUT) {
            CloseHandle(Handle);
            VrpRemoveOpenNamedPipeInfo(Handle);
        }
    }

#if DBG
#line 2729
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2737

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
#line 2790
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2792

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
#line 2810
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2814
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
#line 2829
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2833

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
#line 2848
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2852

                return TRUE;
            }
        }
    }

#if DBG
#line 2859
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2863

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
        pipeLength = strlen(pipeName);
        prefixLength = (DWORD)pipeName - (DWORD)Name;
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
            if (pipeName = strchr(Buffer, '/')) {
                *pipeName++ = '\\';
            }
        } while (pipeName);

#if DBG
#line 2995
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 2999

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

    NameLength = strlen(PipeName) + 1;
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
#line 3075
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 3079

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
#line 3104
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 3112

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
#line 3188
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 3194

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
#line 3207
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 3215

            LocalFree(ptr);
            return TRUE;
        } else {
            prev = ptr;
            ptr = ptr->Next;
        }
    }

#if DBG
#line 3225
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 3229

    return FALSE;
}


PRIVATE
VOID
RememberPipeIo(
    IN POVERLAPPED_PIPE_IO PipeIo
    )

/*++

Routine Description:

    Adds an OVERLAPPED_PIPE_IO structure to the list of in-progress named pipe
    I/Os

Arguments:

    PipeIo  - pointer to OVERLAPPED_PIPE_IO structure to add to list

Return Value:

    None.

--*/

{
    //
    // just stick this at front of list; order is not important - this is just
    // a stack of in-progress requests
    //

    PipeIo->Thread = GetCurrentThreadId();
    EnterCriticalSection(&VrNamedPipeCancelCritSec);
    PipeIo->Next = PipeIoQueue;
    PipeIoQueue = PipeIo;
    LeaveCriticalSection(&VrNamedPipeCancelCritSec);
}


PRIVATE
VOID
ForgetPipeIo(
    IN POVERLAPPED_PIPE_IO PipeIo
    )

/*++

Routine Description:

    Removes an OVERLAPPED_PIPE_IO structure from the list of in-progress named
    pipe I/Os

Arguments:

    PipeIo  - pointer to OVERLAPPED_PIPE_IO structure to remove

Return Value:

    None.

--*/

{
    POVERLAPPED_PIPE_IO prev, ptr;

    EnterCriticalSection(&VrNamedPipeCancelCritSec);
    for (ptr = PipeIoQueue, prev = (POVERLAPPED_PIPE_IO)&PipeIoQueue; ptr && ptr != PipeIo; ) {
        prev = ptr;
        ptr = ptr->Next;
    }
    if (ptr == PipeIo) {
        prev->Next = ptr->Next;
    }
    LeaveCriticalSection(&VrNamedPipeCancelCritSec);
}


#if DBG
#line 3310
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 3413
