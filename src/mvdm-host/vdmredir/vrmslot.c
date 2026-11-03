/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    vrmslot.c

Abstract:

    Contains Mailslot function handlers for Vdm Redir (Vr) support. This module
    contains the following Vr routines:

        VrDeleteMailslot
        VrGetMailslotInfo
        VrMakeMailslot
        VrPeekMailslot
        VrReadMailslot
        VrWriteMailslot
        VrTerminateMailslots

    Private (Vrp) routines:

        VrpIsMailslotName
        VrpMakeLocalMailslotName
        VrpLinkMailslotStructure
        VrpUnlinkMailslotStructure
        VrpMapMailslotHandle16
        VrpMapMailslotName
        VrpRemoveProcessMailslots
        VrpAllocateHandle16
        VrpFreeHandle16


Author:

    Richard L Firth (rfirth) 16-Sep-1991

Notes:

    Although once created, we must read and write local mailslots using a
    32-bit handle, we use a 16-bit handle to identify the mailslot.  Hence
    we must map the 16-bit mailslot handle to an open 32-bit mailslot
    handle on reads.  The DosWriteMailslot function always supplies the
    symbolic name of a mailslot even if it is local.  In this case we must
    map the name to the open 32-bit local mailslot handle.  We need to
    keep all 3 pieces of information around and map the 16-bit handles
    (ordinal and symbolic) to 32-bit mailslot handles.  Hence the need to
    keep mailslot info structures which are identified mainly by the
    16-bit handle value which we must generate.

    Note that in the DOS world, mailslot handles are traditionally handled
    only by the redirector TSR and DOS has no knowledge of their existence
    or meaning.  Therefore, the 32-bit handle cannot be kept in an SFT and
    DOS would not know what to do with a mailslot handle if given one,
    except where it was numerically equivalent to an open file handle,
    which would probably cause some grief.

    It is assumed that this code is shared between multiple NTVDM processes
    but that each process has its own copy of the data. Hence, none of the
    data items declared in this module are shared - each process has its
    own copy

Environment:

    32-bit flat address space

Revision History:

    16-Sep-1991 rfirth
        Created

--*/

#include <nt.h>
#include <ntrtl.h>      // ASSERT, DbgPrint
#include <nturtl.h>
#include <windows.h>
#include <softpc.h>     // x86 virtual machine definitions
#include <vrdlctab.h>
#include <vdmredir.h>   // common Vdm Redir stuff
#include <vrmslot.h>
#include <string.h>     // Dos still dealing with ASCII
#include <lmcons.h>     // LM20_PATHLEN
#include <lmerr.h>      // NERR_???
#include "vrputil.h"    // private utilities
#include "apistruc.h"   // DosWriteMailslotStruct
#include "vrdebug.h"    // IF_DEBUG

//
// local manifests
//

#define MAILSLOT_PREFIX                 "\\MAILSLOT\\"
#define MAILSLOT_PREFIX_LENGTH          (sizeof(MAILSLOT_PREFIX) - 1)
#define LOCAL_MAILSLOT_PREFIX           "\\\\."
#define LOCAL_MAILSLOT_NAMELEN          LM20_PATHLEN

//
// MAX_16BIT_HANDLES is used as the array allocator count for Handle16Bitmap
// which is stored as DWORDs. Hence, this value should be a multiple of 32,
// or BITSIN(DWORD)
//

#define MAX_16BIT_HANDLES               (1 * BITSIN(DWORD))

#define HANDLE_FUNCTION_FAILED          ((HANDLE)0xffffffff)

//
// local macros
//

#define VrpAllocateMailslotStructure(n) ((PVR_MAILSLOT_INFO)LocalAlloc(LMEM_FIXED, sizeof(VR_MAILSLOT_INFO) + (n)))
#define VrpFreeMailslotStructure(ptr)   ((void)LocalFree(ptr))
#ifdef VR_BREAK
#error Unsupported excluded original configuration
#else
#line 118
#define VR_BREAKPOINT()
#endif
#line 120


//
// private routine prototypes
//

PRIVATE
BOOL
VrpIsMailslotName(
    IN LPSTR Name
    );

PRIVATE
VOID
VrpMakeLocalMailslotName(
    IN LPSTR lpBuffer,
    IN LPSTR lpName
    );

PRIVATE
VOID
VrpLinkMailslotStructure(
    IN PVR_MAILSLOT_INFO MailslotInfo
    );

PRIVATE
PVR_MAILSLOT_INFO
VrpUnlinkMailslotStructure(
    IN WORD Handle16
    );

PRIVATE
PVR_MAILSLOT_INFO
VrpMapMailslotHandle16(
    IN WORD Handle16
    );

PRIVATE
PVR_MAILSLOT_INFO
VrpMapMailslotName(
    IN LPSTR Name
    );

PRIVATE
VOID
VrpRemoveProcessMailslots(
    IN WORD DosPdb
    );

PRIVATE
WORD
VrpAllocateHandle16(
    VOID
    );

PRIVATE
VOID
VrpFreeHandle16(
    IN WORD Handle16
    );


//
// VdmRedir Mailslot support routines
//

VOID
VrDeleteMailslot(
    VOID
    )

/*++

Routine Description:

    Performs DosDeleteMailslot request on behalf of VDM redir. Locates
    VR_MAILSLOT_INFO structure given 16-bit handle, unlinks structure from
    list, frees it and de-allocates the handle

    Notes:

        Only the owner of the mailslot can delete it. That means the PDB
        of this process must equal the PDB of the process which created
        the mailslot (DosMakeMailslot)

Arguments:

    None. All arguments are extracted from 16-bit context descriptor

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    WORD    Handle16, DosPdb;
    PVR_MAILSLOT_INFO   ptr;

    //
    // The redir passes us the CurrentPDB in ax
    //

    DosPdb = getAX();
    Handle16 = getBX();

#line 226
#if DBG
#line 227
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 232

    if (!(ptr = VrpMapMailslotHandle16(Handle16))) {
        SET_ERROR(ERROR_INVALID_HANDLE);
    } else {
        if (ptr->DosPdb != DosPdb) {
            SET_ERROR(ERROR_INVALID_HANDLE);
        } else {
            if (!CloseHandle(ptr->Handle32)) {
                SET_ERROR(VrpMapLastError());
            } else {

                //
                // phew! succeeded in deleting the mailslot. Unlink and free
                // the VR_MAILSLOT_INFO structure and de-allocate the 16-bit
                // handle
                //

                VrpUnlinkMailslotStructure(Handle16);
                VrpFreeHandle16(Handle16);

                //
                // Return some info in various registers for DOS
                //

                setES(ptr->BufferAddress.Selector);
                setDI(ptr->BufferAddress.Offset);
                setDX(ptr->Selector);

                //
                // now repatriate the structure
                //

                VrpFreeMailslotStructure(ptr);

                //
                // 'return' success indication
                //

                setCF(0);
            }
        }
    }
}


VOID
VrGetMailslotInfo(
    VOID
    )

/*++

Routine Description:

    Performs DosMailslotInfo request on behalf of VDM redir

Arguments:

    None. All arguments are extracted from 16-bit context descriptor

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    PVR_MAILSLOT_INFO   ptr;
    DWORD   MaxMessageSize, NextSize, MessageCount;
    BOOL    Ok;

#line 303
#if DBG
#line 304
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 309

    if ((ptr = VrpMapMailslotHandle16(getBX())) == NULL) {
        SET_ERROR(ERROR_INVALID_HANDLE);
    } else {
        Ok = GetMailslotInfo(ptr->Handle32,
                                &MaxMessageSize,
                                &NextSize,
                                &MessageCount,
                                NULL            // lpReadTimeout
                                );
        if (!Ok) {
            SET_ERROR(VrpMapLastError());
        } else {

            //
            // fill in the VDM registers with the required info
            //

            setAX((WORD)MaxMessageSize);
            setBX((WORD)MaxMessageSize);
            if (NextSize == MAILSLOT_NO_MESSAGE) {
                setCX(0);
            } else {
                setCX((WORD)NextSize);
            }

            //
            // we don't support priorities, just return 0
            //

            setDX(0);
            setSI((WORD)MessageCount);
            setCF(0);
        }
    }
}


VOID
VrMakeMailslot(
    VOID
    )

/*++

Routine Description:

    Performs DosMakeMailslot request on behalf of VDM redir. This routine
    creates a local mailslot. If the mailslot name argument designates a
    remote mailslot name then this call will fail

Arguments:

    None. All arguments are extracted from 16-bit context descriptor

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    PVR_MAILSLOT_INFO   ptr;
    WORD    Handle16;
    HANDLE  Handle32;
    DWORD   NameLength;
    LPSTR   lpName;
    CHAR    LocalMailslot[LOCAL_MAILSLOT_NAMELEN+1];
    BOOL    Ok;


#line 380
#if DBG
#line 381
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 386

    //
    // grab the next 16-bit handle value. This pre-allocates the handle. If we
    // cannot allocate a handle return a path not found error. If we should
    // fail anywhere along the line after this we must free up the handle
    //

    if ((Handle16 = VrpAllocateHandle16()) == 0) {
        SET_ERROR(ERROR_PATH_NOT_FOUND);    // all handles used!
        return;
    }

    //
    // get the pointer to the mailslot name from the VDM registers then
    // compute the significant length for the name
    //

    lpName = LPSTR_FROM_WORDS(getDS(), getSI());
    NameLength = strlen(lpName);

#line 406
#if DBG
#line 407
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 411

    //
    // if the name length is less than the prefix length (\MAILSLOT\) may as
    // well return an invalid name error here - can't be proper mailslot name
    //

    if (NameLength <= MAILSLOT_PREFIX_LENGTH) {
        SET_ERROR(ERROR_PATH_NOT_FOUND);
        VrpFreeHandle16(Handle16);
        return;
    }

    //
    // NameLength is length of local mailslot name after \MAILSLOT\. We
    // only store this info if the mailslot actually turns out to be
    // local
    //

    NameLength -= MAILSLOT_PREFIX_LENGTH;

    //
    // grab a structure in which to store the info. If we can't get one(!)
    // return a path not found error (Do we have a better one that the app
    // might be expecting?). We need a structure large enough to hold the
    // significant part of the mailslot name too
    //

    if ((ptr = VrpAllocateMailslotStructure(NameLength)) == NULL) {
        SET_ERROR(ERROR_PATH_NOT_FOUND);    // mon dieu! sacre fromage! etc...
        VrpFreeHandle16(Handle16);
        return;
    }

    //
    // convert the DOS namespace mailslot name to a local mailslot name
    // (\MAILSLOT\name => \\.\MAILSLOT\name)
    //

    VrpMakeLocalMailslotName(LocalMailslot, lpName);

    //
    // create the mailslot. If this fails free up the structure and handle
    // already allocated. Note: at this point we may have a proper mailslot
    // name or we could have any old garbage. We trust that CreateMailslot
    // will sort the wheat from the oatbran
    //

#line 458
#if DBG
#line 459
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 467

    Handle32 = CreateMailslot(LocalMailslot,
                                (DWORD)getBX(),     // nMaxMessageSize
                                0,                  // lReadTimeout
                                NULL                // security descriptor
                                );
    if (Handle32 == HANDLE_FUNCTION_FAILED) {
        SET_ERROR(VrpMapLastError());

#line 476
#if DBG
#line 477
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 483

        VrpFreeMailslotStructure(ptr);
        VrpFreeHandle16(Handle16);
    } else {

#line 488
#if DBG
#line 489
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 493

        //
        // mailslot created - fill in the VR_MAILSLOT_INFO structure -
        // containing mailslot info for Dos app - and link it into the
        // list of structures. Return an arbitrary (but unique!) 16-bit
        // handle
        //

        ptr->DosPdb = getAX();
        ptr->Handle16 = Handle16;
        ptr->Handle32 = Handle32;
        ptr->BufferAddress.Offset = getDI();
        ptr->BufferAddress.Selector = getES();
        ptr->Selector = getDX();    // prot mode selector for Win3

        //
        // find the true message size from the info API
        //

        Ok = GetMailslotInfo(Handle32,
                                &ptr->MessageSize,
                                NULL,           // lpNextSize
                                NULL,           // lpMessageCount
                                NULL            // lpReadTimeout
                                );
        if (!Ok) {

#line 520
#if DBG
#line 521
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 527

            ptr->MessageSize = getCX();
        }

        //
        // copy the name of the mailslot after \MAILSLOT\ to the structure.
        // We compare this when a mailslot write is requested (because
        // DosWriteMailslot passes in a name; we have to write locally
        // using a handle, so we must convert the name of a local mailslot
        // to an already open handle). Check NameLength first before doing
        // strcmp
        //

        ptr->NameLength = NameLength;
        strcpy(ptr->Name, lpName + MAILSLOT_PREFIX_LENGTH);
        VrpLinkMailslotStructure(ptr);
        setAX(Handle16);
        setCF(0);
    }
}


VOID
VrPeekMailslot(
    VOID
    )

/*++

Routine Description:

    Performs DosPeekMailslot request on behalf of VDM redir.

    Note: we are not supporting Peeks of NT mailslots (the Win32 Mailslot API
    does not support mailslot peek). This routine is left here as a place
    holder should we want to descend to the NT level to implement mailslots
    (which do allow peeks)

Arguments:

    None. All arguments are extracted from 16-bit context descriptor

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
#line 576
#if DBG
#line 577
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 584

    //
    // return not supported error instead of ERROR_INVALID_FUNCTION
    //

    SET_ERROR(ERROR_NOT_SUPPORTED);
}


VOID
VrReadMailslot(
    VOID
    )

/*++

Routine Description:

    Performs DosReadMailslot request on behalf of VDM redir

Arguments:

    None. All arguments are extracted from 16-bit context descriptor

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    PVR_MAILSLOT_INFO   ptr;
    HANDLE  Handle;
    DWORD   BytesRead;
    DWORD   NextSize;
    BOOL    Ok;

#line 621
#if DBG
#line 622
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 627

    if ((ptr = VrpMapMailslotHandle16(getBX())) == NULL) {
        SET_ERROR(ERROR_INVALID_HANDLE);
    } else {

        //
        // the NT API won't allow us to specify the read timeout on each read
        // call, so we have to change it with SetMailslotInfo before we can
        // do the read
        //

        Handle = ptr->Handle32;
        if (!SetMailslotInfo(Handle, MAKE_DWORD(getDX(), getCX()))) {
            SET_ERROR(VrpMapLastError());
        } else {

#line 643
#if DBG
#line 644
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 648

            Ok = ReadFile(Handle,
                            POINTER_FROM_WORDS(getES(), getDI()),
                            ptr->MessageSize,
                            &BytesRead,
                            NULL            // not overlapped
                            );
            if (!Ok) {
                SET_ERROR(VrpMapLastError());
            } else {

#line 659
#if DBG
#line 660
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 668

                setAX((WORD)BytesRead);

                //
                // we need to return also the NextSize and NextPriority info
                //

                NextSize = MAILSLOT_NO_MESSAGE;
                Ok = GetMailslotInfo(Handle,
                                        NULL,           // lpMaxMessageSize
                                        &NextSize,
                                        NULL,           // lpMessageCount
                                        NULL            // lpReadTimeout
                                        );
                if (NextSize == MAILSLOT_NO_MESSAGE) {
                    setCX(0);
                } else {
                    setCX((WORD)NextSize);
                }

#line 688
#if DBG
#line 689
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 693

                //
                // we don't support priorities, just return 0
                //

                setDX(0);
                setCF(0);
            }
        }
    }
}


VOID
VrWriteMailslot(
    VOID
    )

/*++

Routine Description:

    Performs DosWriteMailslot request on behalf of VDM redir

Arguments:

    None. All arguments are extracted from 16-bit context descriptor

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
    LPSTR   Name;
    HANDLE  Handle;
    BOOL    Ok;
    DWORD   BytesWritten;
    CHAR    LocalMailslotName[LOCAL_MAILSLOT_NAMELEN+1];
    struct  DosWriteMailslotStruct* StructurePointer;

    //
    // search for the local mailslot based on the name. If not found assume
    // it is a remote handle and try to open it. Return failure if cannot
    // open
    //

    Name = LPSTR_FROM_WORDS(getDS(), getSI());

#line 743
#if DBG
#line 744
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 749

    if (!VrpIsMailslotName(Name)) {

#line 752
#if DBG
#line 753
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 757

        SET_ERROR(ERROR_PATH_NOT_FOUND);
    }
    if (!IS_ASCII_PATH_SEPARATOR(Name[1])) {
        strcpy(LocalMailslotName, LOCAL_MAILSLOT_PREFIX);
        strcat(LocalMailslotName, Name);
        Name = LocalMailslotName;
    }

    Handle = CreateFile(Name,
                        GENERIC_WRITE,
                        FILE_SHARE_WRITE | FILE_SHARE_READ,
                        NULL,               // lpSecurityAttributes
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL                // hTemplateFile
                        );
    if (Handle == HANDLE_FUNCTION_FAILED) {
        SET_ERROR(VrpMapLastError());

#line 777
#if DBG
#line 778
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 782

    } else {

        //
        // we have a handle to an open mailslot - either local or remote. Get
        // the caller's timeout and buffer pointer from the
        // DosWriteMailslotStruct at es:di
        //

        StructurePointer = (struct DosWriteMailslotStruct*)
                                POINTER_FROM_WORDS(getES(), getDI());

        Ok = SetMailslotInfo(Handle, READ_DWORD(&StructurePointer->DWMS_Timeout));

#line 796
#if DBG
#line 797
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 807

        Ok = WriteFile(Handle,
                        READ_FAR_POINTER(&StructurePointer->DWMS_Buffer),
                        (DWORD)getCX(),
                        &BytesWritten,
                        NULL            // lpOverlapped
                        );
        if (!Ok) {
            SET_ERROR(VrpMapLastError());

#line 817
#if DBG
#line 818
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 822

        } else {

#line 825
#if DBG
#line 826
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 833

            setCF(0);
        }
        CloseHandle(Handle);
    }
}


VOID
VrTerminateMailslots(
    IN WORD DosPdb
    )

/*++

Routine Description:

    If a Dos app created some mailslots and then terminates, then we need to
    delete the mailslots on its behalf. The main reason is that Dos process
    termination cleanup is limited mainly to file handles. Mailslot handles
    are not part of the file handle set so don't get closed for a terminating
    app. Control is passed here via the redir receiving a NetResetEnvironment
    call when Dos decides the app is closing. The redir BOPs here and we
    clean up the mailslot mess

    Assumes single-threadedness

Arguments:

    DosPdb  - 16-bit (segment) identifier of terminating DOS process

Return Value:

    None. Returns values in VDM Ax and Flags registers

--*/

{
#line 871
#if DBG
#line 872
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 876

    VrpRemoveProcessMailslots(DosPdb);
}


//
// private utilities
//

PRIVATE
BOOL
VrpIsMailslotName(
    IN LPSTR Name
    )

/*++

Routine Description:

    Checks if a string designates a mailslot. As criteria for the decision
    we use:

        \\computername\MAILSLOT\...
        \MAILSLOT\...

Arguments:

    Name    - to check for (Dos) mailslot syntax

Return Value:

    BOOL
        TRUE    - Name refers to (local or remote) mailslot
        FALSE   - Name doesn't look like mailslot name

--*/

{
    int     CharCount;

#line 916
#if DBG
#line 917
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 919

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
                // Name is \\ or \\\ or just \\name, none of which I understand,
                // so its not a valid mailslot name - fail it
                //

#line 936
#if DBG
#line 937
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 941

                return FALSE;
            }
            ++Name;
        }

        //
        // We are at <something> (after \ or \\<name>\). Check if <something>
        // is [Mm][Aa][Ii][Ll][Ss][Ll][Oo][Tt][\\/]
        //

        if (!_strnicmp(Name, "MAILSLOT", 8)) {
            Name += 8;
            if (IS_ASCII_PATH_SEPARATOR(*Name)) {

#line 956
#if DBG
#line 957
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 961

                return TRUE;
            }
        }
    }

#line 967
#if DBG
#line 968
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 972

    return FALSE;
}


PRIVATE
VOID
VrpMakeLocalMailslotName(
    IN LPSTR lpBuffer,
    IN LPSTR lpName
    )

/*++

Routine Description:

    Converts a local DOS mailslot name of the form \MAILSLOT\<name> to a local
    NT/Win32 mailslot name of the form \\.\MAILSLOT\<name>

Arguments:

    lpBuffer    - pointer to ASCIZ buffer where local NT mailslot name will
                  be returned
    lpName      - pointer to ASCIZ Dos mailslot name

    NOTE: It is assumed that the buffer @ lpBuffer is large enough to hold the
    composite name and that Unicode support (or conversion) is NOT REQUIRED
    since we are supporting Dos which will only use ASCIZ (or at worst DBCS)
    strings

Return Value:

    return-value - Description of conditions needed to return value. - or -
    None.

--*/

{
    if (!_strnicmp(lpName, MAILSLOT_PREFIX, MAILSLOT_PREFIX_LENGTH)) {
        strcpy(lpBuffer, LOCAL_MAILSLOT_PREFIX);
        strcat(lpBuffer, lpName);
    }

#line 1015
#if DBG
#line 1016
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1020

}


//
// private mailslot list and list manipulators
//

PRIVATE
PVR_MAILSLOT_INFO   MailslotInfoList    = NULL;

PRIVATE
PVR_MAILSLOT_INFO   LastMailslotInfo    = NULL;



PRIVATE
VOID
VrpLinkMailslotStructure(
    IN PVR_MAILSLOT_INFO MailslotInfo
    )

/*++

Routine Description:

    Adds a VR_MAILSLOT_INFO structure to the end of MailslotInfoList. Points
    LastMailslotInfo at this structure

    Notes:

        Assumes that if LastMailslotInfo is NULL then there is nothing in
        the list (ie MailslotInfoList is also NULL)

Arguments:

    MailslotInfo    - pointer to VR_MAILSLOT_INFO stucture to add

Return Value:

    None.

--*/

{
    if (!LastMailslotInfo) {
        MailslotInfoList = MailslotInfo;
    } else {
        LastMailslotInfo->Next = MailslotInfo;
    }
    LastMailslotInfo = MailslotInfo;
    MailslotInfo->Next = NULL;
}


PRIVATE
PVR_MAILSLOT_INFO
VrpUnlinkMailslotStructure(
    IN WORD Handle16
    )

/*++

Routine Description:

    Removes a VR_MAILSLOT_INFO structure from the list at MailslotInfoList.
    The structure to remove is identified by the 32-bit handle

Arguments:

    Handle16    - 16-bit handle of open mailslot to search for

Return Value:

    PVR_MAILSLOT_INFO
        Success - pointer to removed VR_MAILSLOT_INFO structure
        Failure - NULL

--*/

{
    PVR_MAILSLOT_INFO   ptr, previous = NULL;

    for (ptr = MailslotInfoList; ptr; ) {
        if (ptr->Handle16 == Handle16) {
            if (!previous) {
                MailslotInfoList = ptr->Next;
            } else {
                previous->Next = ptr->Next;
            }
            if (LastMailslotInfo == ptr) {
                LastMailslotInfo = previous;
            }
            break;
        } else {
            previous = ptr;
            ptr = ptr->Next;
        }
    }

#line 1120
#if DBG
#line 1121
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1134

    return ptr;
}


PRIVATE
PVR_MAILSLOT_INFO
VrpMapMailslotHandle16(
    IN WORD Handle16
    )

/*++

Routine Description:

    Searches the list of VR_MAILSLOT_INFO structures looking for the one
    containing Handle16. If found, returns pointer to structure else NULL

    Notes:

        This routine assumes that Handle16 is unique and >1 mailslot structure
        cannot simultaneously exist with this handle

Arguments:

    Handle16    - Unique 16-bit handle to search for

Return Value:

    PVR_MAILSLOT_INFO
        Success - pointer to located structure
        Failure - NULL
--*/

{
    PVR_MAILSLOT_INFO   ptr;

    for (ptr = MailslotInfoList; ptr; ptr = ptr->Next) {
        if (ptr->Handle16 == Handle16) {
            break;
        }
    }

#line 1177
#if DBG
#line 1178
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1191

    return ptr;
}


PRIVATE
PVR_MAILSLOT_INFO
VrpMapMailslotName(
    IN LPSTR Name
    )

/*++

Routine Description:

    Searches for a VR_MAILSLOT_INFO structure in MailslotInfoList by name

Arguments:

    Name    - of mailslot to search for. Full name, including \MAILSLOT\

Return Value:

    PVR_MAILSLOT_INFO
        Success - pointer to structure containing Name
        Failure - NULL

--*/

{
    PVR_MAILSLOT_INFO   ptr;
    DWORD   NameLength;

    NameLength = strlen(Name) - MAILSLOT_PREFIX_LENGTH;
    for (ptr = MailslotInfoList; ptr; ptr = ptr->Next) {
        if (ptr->NameLength == NameLength) {
            if (!_stricmp(ptr->Name, Name)) {
                break;
            }
        }
    }

#line 1233
#if DBG
#line 1234
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1244

    return ptr;
}


PRIVATE
VOID
VrpRemoveProcessMailslots(
    IN WORD DosPdb
    )

/*++

Routine Description:

    Searches for a VR_MAILSLOT_INFO structure in MailslotInfoList by PDB
    then deletes it if found.

    Unfortunately, this routine is munged from a couple others

Arguments:

    DosPdb  - PID of terminating Dos app. Kill all mailslots belonging to
              this app

Return Value:

    None.

--*/

{
    PVR_MAILSLOT_INFO   ptr, previous = NULL, next;

#line 1278
#if DBG
#line 1279
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1285

    //
    // usual type of thing - grovel through list of mailslot structures, if
    // one belongs to our dos process then remove it from the list, close
    // the mailslot and free the structure
    //

    for (ptr = MailslotInfoList; ptr; ) {
        if (ptr->DosPdb == DosPdb) {

#line 1295
#if DBG
#line 1296
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1306

            CloseHandle(ptr->Handle32);

#line 1309
#if DBG
#line 1310
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1320

            //
            // remove mailslot structure from list
            //

            if (!previous) {
                MailslotInfoList = ptr->Next;
            } else {
                previous->Next = ptr->Next;
            }
            if (LastMailslotInfo == ptr) {
                LastMailslotInfo = previous;
            }

            //
            // free up the 16-bit handle allocation
            //

            VrpFreeHandle16(ptr->Handle16);

            //
            // and repatriate the structure
            //

            next = ptr->Next;
            VrpFreeMailslotStructure(ptr);
            ptr = next;
        } else {
            previous = ptr;
            ptr = ptr->Next;
        }
    }
}


//
// 16-bit handle allocators
//

PRIVATE
DWORD   Handle16Bitmap[MAX_16BIT_HANDLES/BITSIN(DWORD)];

PRIVATE
WORD
VrpAllocateHandle16(
    VOID
    )

/*++

Routine Description:

    Allocates the next free 16-bit handle. This is based on a bitmap: the
    ordinal number of the next available 0 bit in the map indicates the next
    16-bit handle value.

    Notes:

        The 16-bit handle is an arbitrary but unique number. We don't expect
        there to be too many TSR mailslots and 1 or 2 DWORDs should suffice
        even the most demanding local mailslot user.

        The handles are returned starting at 1. Therefore bit 0 in the map
        corresponds to handle 1; bit 0 in Handle16Bitmap[1] corresponds to
        handle 33, etc.

        Nothing assumed about byte order, only bits in DWORD (which is
        universal, methinks)

Arguments:

    None.

Return Value:

    WORD
        Success - 16-bit handle value in range 1 <= Handle <= 32
        Failure - 0

--*/

{
    int     i;
    DWORD   map;
    WORD    Handle16 = 1;

    //
    // this 'kind of' assumes that the bitmap is stored as DWORDs. Its
    // actually more explicit, so don't change the type or MAX_16BIT_HANDLES
    // without checking this code first
    //

    for (i=0; i<sizeof(Handle16Bitmap)/sizeof(Handle16Bitmap[0]); ++i) {
        map = Handle16Bitmap[i];

        //
        // if this entry in the bitmap is already full, skip to the next one
        // (if there is one, that is)
        //

        if (map == -1) {
            Handle16 += BITSIN(DWORD);
            continue;
        } else {
            int j;

            //
            // use BFI method to find next available slot
            //

            for (j=1, Handle16=1; map & j; ++Handle16, j <<= 1);
            Handle16Bitmap[i] |= j;

#line 1433
#if DBG
#line 1434
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1442

            return Handle16;
        }
    }

    //
    // no free handles found. Since handles start at 1, use 0 to indicate error
    //

#line 1451
#if DBG
#line 1452
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1457

    return 0;
}


PRIVATE
VOID
VrpFreeHandle16(
    IN WORD Handle16
    )

/*++

Routine Description:

    Free a 16-bit handle. Reset the corresponding bit in the bitmap

    Notes:

        This routine assumes that the Handle16 parameter is a valid 16-bit
        Handle value, as generated by VrpAllocate16BitHandle

Arguments:

    Handle16    - number of bit to reset

Return Value:

    None.

--*/

{
    //
    // remember: we allocated the handle value as the next free bit + 1, so
    // we started the handles at 1, not 0
    //

    --Handle16;

#line 1497
#if DBG
#line 1498
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1505

    Handle16Bitmap[Handle16/BITSIN(DWORD)] &= ~(1 << Handle16 % BITSIN(DWORD));

#line 1508
#if DBG
#line 1509
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 1513

}
