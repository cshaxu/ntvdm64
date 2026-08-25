/*
 * Directly retained OpenNT VDMREDIR source body.
 *
 * Original source: OpenNT NT4 base/mvdm/vdmredir/vrmslot.c,
 * VrPeekMailslot.  The full original unit is being recovered in staged
 * owner groups; this first body deliberately supersedes the former
 * source-derived peek emulation because OpenNT explicitly declines peeking
 * NT mailslots.
 */

#include <stdlib.h>
#include <string.h>

#include "vrmslot_compat.h" /* DIVERGENCE(BOP-DIV-057): NT4 vdmredir/CCPU
                               * header closure is replaced by the declared
                               * same-shaped adapter-softpc facade. */

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
    /*
     * return not supported error instead of ERROR_INVALID_FUNCTION
     */

    SET_ERROR(ERROR_NOT_SUPPORTED);
}

/* Directly retained OpenNT VrMakeMailslot body.  Its original ordering,
 * record layout and failure convention are preserved below. */
VOID
VrMakeMailslot(
    VOID
    )
{
    PVR_MAILSLOT_INFO ptr;
    WORD Handle16;
    HANDLE Handle32;
    DWORD NameLength;
    CHAR NameBuffer[MAX_PATH + 1u];
    LPSTR lpName;
    CHAR LocalMailslot[LOCAL_MAILSLOT_NAMELEN + 1u];
    BOOL Ok;
    uint32_t token = 0u;

    /* DIVERGENCE(BOP-DIV-062): OpenNT dereferenced a flat SAS guest pointer.
     * This composition retains the source body while copying the ASCIZ name
     * through the existing bounded CCPU/SAS facade. */
    if (!runtime_demhndl_copy_guest_oem_string(getDS(), getSI(), NameBuffer,
            sizeof(NameBuffer))) { SET_ERROR(ERROR_INVALID_ADDRESS); return; }
    lpName = NameBuffer;
    NameLength = (DWORD)strlen(lpName);
    if (NameLength <= MAILSLOT_PREFIX_LENGTH) { SET_ERROR(ERROR_PATH_NOT_FOUND); return; }
    NameLength -= MAILSLOT_PREFIX_LENGTH;
    if ((ptr = VrpAllocateMailslotStructure(NameLength)) == NULL) {
        SET_ERROR(ERROR_PATH_NOT_FOUND); return;
    }
    VrpMakeLocalMailslotName(LocalMailslot, lpName);
    Handle32 = CreateMailslotA(LocalMailslot, (DWORD)getBX(), 0, NULL);
    if (Handle32 == HANDLE_FUNCTION_FAILED) {
        SET_ERROR(VrpMapLastError()); VrpFreeMailslotStructure(ptr); return;
    }
    /* DIVERGENCE(BOP-DIV-063): replace OpenNT's private pre-allocated
     * Handle16 bitmap with the sole session-owned opaque handle mapper. */
    if (!runtime_demhndl_publish_handle_token(Handle32, &token) || token > UINT16_MAX) {
        if (token != 0u) (void)VrpCloseMailslotHandle((WORD)token, Handle32);
        else CloseHandle(Handle32);
        VrpFreeMailslotStructure(ptr); SET_ERROR(ERROR_PATH_NOT_FOUND); return;
    }
    Handle16 = (WORD)token;
    ptr->DosPdb = getAX(); ptr->Handle16 = Handle16; ptr->Handle32 = Handle32;
    ptr->BufferAddress.Offset = getDI(); ptr->BufferAddress.Selector = getES();
    ptr->Selector = getDX();
    Ok = GetMailslotInfo(Handle32, &ptr->MessageSize, NULL, NULL, NULL);
    if (!Ok) ptr->MessageSize = getCX();
    ptr->NameLength = NameLength;
    strcpy_s(ptr->Name, (size_t)NameLength + 1u, lpName + MAILSLOT_PREFIX_LENGTH);
    VrpLinkMailslotStructure(ptr); setAX(Handle16); setCF(0);
}

VOID
VrReadMailslot(
    VOID
    )
{
    PVR_MAILSLOT_INFO ptr;
    HANDLE Handle;
    DWORD BytesRead, NextSize;
    BOOL Ok;
    if ((ptr = VrpMapMailslotHandle16(getBX())) == NULL) {
        SET_ERROR(ERROR_INVALID_HANDLE);
    } else {
        Handle = ptr->Handle32;
        if (!SetMailslotInfo(Handle, MAKE_DWORD(getDX(), getCX()))) {
            SET_ERROR(VrpMapLastError());
        } else {
            /* DIVERGENCE(BOP-DIV-064): original flat SAS output pointer is
             * the declared checked MessageSize lease supplied by its caller. */
            Ok = ReadFile(Handle, POINTER_FROM_WORDS(getES(), getDI()),
                ptr->MessageSize, &BytesRead, NULL);
            if (!Ok) {
                SET_ERROR(VrpMapLastError());
            } else {
                setAX((WORD)BytesRead);
                NextSize = MAILSLOT_NO_MESSAGE;
                Ok = GetMailslotInfo(Handle, NULL, &NextSize, NULL, NULL);
                if (NextSize == MAILSLOT_NO_MESSAGE) setCX(0);
                else setCX((WORD)NextSize);
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

    Performs DosWriteMailslot request on behalf of VDM redir.

Arguments:

    None. All arguments are extracted from 16-bit context descriptor.

Return Value:

    None. Returns values in VDM Ax and Flags registers.

--*/

{
    LPSTR Name;
    HANDLE Handle;
    BOOL Ok;
    DWORD BytesWritten;
    CHAR LocalMailslotName[LOCAL_MAILSLOT_NAMELEN + 1u];
    struct DosWriteMailslotStruct *StructurePointer;
    CHAR NameBuffer[MAX_PATH + 1u];

    /* DIVERGENCE(BOP-DIV-065): OpenNT obtained Name through a flat SAS
     * pointer.  Preserve the source body and its call order after copying the
     * bounded ASCIZ input through the existing CCPU/SAS facade. */
    if (!runtime_demhndl_copy_guest_oem_string(getDS(), getSI(), NameBuffer,
            sizeof(NameBuffer))) { SET_ERROR(ERROR_INVALID_ADDRESS); return; }
    Name = NameBuffer;

    if (!VrpIsMailslotName(Name)) {
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
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL
                        );
    if (Handle == HANDLE_FUNCTION_FAILED) {
        SET_ERROR(VrpMapLastError());
    } else {
        StructurePointer = (struct DosWriteMailslotStruct *)
                                POINTER_FROM_WORDS(getES(), getDI());

        Ok = SetMailslotInfo(Handle, READ_DWORD(&StructurePointer->DWMS_Timeout));

        Ok = WriteFile(Handle,
                        READ_FAR_POINTER(&StructurePointer->DWMS_Buffer),
                        (DWORD)getCX(),
                        &BytesWritten,
                        NULL
                        );
        if (!Ok) {
            SET_ERROR(VrpMapLastError());
        } else {
            setCF(0);
        }
        CloseHandle(Handle);
    }
}


VOID
VrGetMailslotInfo(
    VOID
    )

/*++

Routine Description:

    Performs DosMailslotInfo request on behalf of VDM redir.

Arguments:

    None. All arguments are extracted from 16-bit context descriptor.

Return Value:

    None. Returns values in VDM Ax and Flags registers.

--*/

{
    PVR_MAILSLOT_INFO ptr;
    DWORD MaxMessageSize, NextSize, MessageCount;
    BOOL Ok;

    if ((ptr = VrpMapMailslotHandle16(getBX())) == NULL) {
        SET_ERROR(ERROR_INVALID_HANDLE);
    } else {
        Ok = GetMailslotInfo(ptr->Handle32,
                              &MaxMessageSize,
                              &NextSize,
                              &MessageCount,
                              NULL);
        if (!Ok) {
            SET_ERROR(VrpMapLastError());
        } else {
            setAX((WORD)MaxMessageSize);
            setBX((WORD)MaxMessageSize);
            if (NextSize == MAILSLOT_NO_MESSAGE) {
                setCX(0);
            } else {
                setCX((WORD)NextSize);
            }
            setDX(0);
            setSI((WORD)MessageCount);
            setCF(0);
        }
    }
}

VOID
VrDeleteMailslot(
    VOID
    )

/*++

Routine Description:

    Performs DosDeleteMailslot request on behalf of VDM redir.

Arguments:

    None. All arguments are extracted from 16-bit context descriptor.

Return Value:

    None. Returns values in VDM Ax and Flags registers.

--*/

{
    WORD Handle16, DosPdb;
    PVR_MAILSLOT_INFO ptr;

    DosPdb = getAX();
    Handle16 = getBX();

    if (!(ptr = VrpMapMailslotHandle16(Handle16))) {
        SET_ERROR(ERROR_INVALID_HANDLE);
    } else {
        if (ptr->DosPdb != DosPdb) {
            SET_ERROR(ERROR_INVALID_HANDLE);
        } else {
            if (!VrpCloseMailslotHandle(Handle16, ptr->Handle32)) {
                SET_ERROR(VrpMapLastError());
            } else {
                VrpUnlinkMailslotStructure(Handle16);
                /* DIVERGENCE(BOP-DIV-061): CloseMailslotHandle releases the
                 * session's sole opaque token, replacing the original
                 * private VrpFreeHandle16 bitmap operation. */

                setES(ptr->BufferAddress.Selector);
                setDI(ptr->BufferAddress.Offset);
                setDX(ptr->Selector);
                VrpFreeMailslotStructure(ptr);
                setCF(0);
            }
        }
    }
}

/* The following list primitives retain the original vrmslot.c record order,
 * list traversal and process cleanup ownership.  DIVERGENCE(BOP-DIV-058):
 * original Handle16Bitmap allocation is replaced by the one session-owned
 * opaque host-handle manager; the list receives its already-range-checked
 * 16-bit token and never maps a native HANDLE itself. */

#define MAILSLOT_PREFIX "\\MAILSLOT\\"
#define MAILSLOT_PREFIX_LENGTH (sizeof(MAILSLOT_PREFIX) - 1u)

static PVR_MAILSLOT_INFO MailslotInfoList = NULL;
static PVR_MAILSLOT_INFO LastMailslotInfo = NULL;

PVR_MAILSLOT_INFO VrpAllocateMailslotStructure(DWORD name_length)
{
    size_t bytes = sizeof(VR_MAILSLOT_INFO) + (size_t)name_length;
    return (PVR_MAILSLOT_INFO)calloc(1u, bytes);
}

void VrpFreeMailslotStructure(PVR_MAILSLOT_INFO record)
{
    free(record);
}

void VrpLinkMailslotStructure(PVR_MAILSLOT_INFO record)
{
    if (record == NULL) return;
    if (!LastMailslotInfo) MailslotInfoList = record;
    else LastMailslotInfo->Next = record;
    LastMailslotInfo = record;
    record->Next = NULL;
}

PVR_MAILSLOT_INFO VrpUnlinkMailslotStructure(WORD handle16)
{
    PVR_MAILSLOT_INFO ptr, previous = NULL;
    for (ptr = MailslotInfoList; ptr; ) {
        if (ptr->Handle16 == handle16) {
            if (!previous) MailslotInfoList = ptr->Next;
            else previous->Next = ptr->Next;
            if (LastMailslotInfo == ptr) LastMailslotInfo = previous;
            break;
        }
        previous = ptr;
        ptr = ptr->Next;
    }
    return ptr;
}

PVR_MAILSLOT_INFO VrpMapMailslotHandle16(WORD handle16)
{
    PVR_MAILSLOT_INFO ptr;
    for (ptr = MailslotInfoList; ptr; ptr = ptr->Next)
        if (ptr->Handle16 == handle16) break;
    return ptr;
}

PVR_MAILSLOT_INFO VrpMapMailslotName(LPSTR name)
{
    PVR_MAILSLOT_INFO ptr;
    DWORD name_length;
    if (name == NULL || strlen(name) < MAILSLOT_PREFIX_LENGTH) return NULL;
    name_length = (DWORD)(strlen(name) - MAILSLOT_PREFIX_LENGTH);
    for (ptr = MailslotInfoList; ptr; ptr = ptr->Next)
        if (ptr->NameLength == name_length &&
            _stricmp(ptr->Name, name + MAILSLOT_PREFIX_LENGTH) == 0) break;
    return ptr;
}

void VrpMakeLocalMailslotName(LPSTR buffer, LPSTR name)
{
    if (buffer != NULL && name != NULL &&
        !_strnicmp(name, MAILSLOT_PREFIX, MAILSLOT_PREFIX_LENGTH)) {
        strcpy_s(buffer, LOCAL_MAILSLOT_NAMELEN + 1u, LOCAL_MAILSLOT_PREFIX);
        strcat_s(buffer, LOCAL_MAILSLOT_NAMELEN + 1u, name);
    }
}

BOOL
VrpIsMailslotName(
    IN LPSTR Name
    )
{
    int CharCount;

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
                return FALSE;
            }
            ++Name;
        }

        if (!_strnicmp(Name, "MAILSLOT", 8)) {
            Name += 8;
            if (IS_ASCII_PATH_SEPARATOR(*Name)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

static void VrpReleaseMailslot(PVR_MAILSLOT_INFO ptr, void *state,
    runtime_vrmslot_release_fn release)
{
    DWORD ignored_error;
    if (ptr == NULL) return;
    if (release != NULL)
        (void)release(state, (uint32_t)ptr->Handle16, &ignored_error);
    else
        (void)VrpCloseMailslotHandle(ptr->Handle16, ptr->Handle32);
    VrpFreeMailslotStructure(ptr);
}

void VrpRemoveProcessMailslotsWithRelease(WORD dos_pdb, void *state,
    runtime_vrmslot_release_fn release)
{
    PVR_MAILSLOT_INFO ptr = MailslotInfoList, previous = NULL;
    while (ptr != NULL) {
        PVR_MAILSLOT_INFO next = ptr->Next;
        if (ptr->DosPdb == dos_pdb) {
            if (!previous) MailslotInfoList = next;
            else previous->Next = next;
            if (LastMailslotInfo == ptr) LastMailslotInfo = previous;
            VrpReleaseMailslot(ptr, state, release);
        } else {
            previous = ptr;
        }
        ptr = next;
    }
}

void VrpRemoveProcessMailslots(WORD dos_pdb)
{
    VrpRemoveProcessMailslotsWithRelease(dos_pdb, NULL, NULL);
}

void VrpResetMailslots(void *state, runtime_vrmslot_release_fn release)
{
    PVR_MAILSLOT_INFO ptr = MailslotInfoList;
    MailslotInfoList = NULL;
    LastMailslotInfo = NULL;
    while (ptr != NULL) {
        PVR_MAILSLOT_INFO next = ptr->Next;
        VrpReleaseMailslot(ptr, state, release);
        ptr = next;
    }
}

VOID
VrTerminateMailslots(
    IN WORD DosPdb
    )

/*++

Routine Description:

    Removes local mailslots owned by a terminating DOS process.

Arguments:

    DosPdb - 16-bit segment identifier of the terminating DOS process.

Return Value:

    None.

--*/

{
    VrpRemoveProcessMailslots(DosPdb);
}

/* DIVERGENCE(BOP-DIV-060): the source helper receives its PDB from the
 * original NetResetEnvironment caller.  The BOP route carries that same
 * 16-bit value in AX, copied by the existing selector-owned frame bridge. */
void runtime_vrmslot_terminate_bop_body(void)
{
    VrTerminateMailslots(getAX());
}
