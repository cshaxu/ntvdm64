/* Finite scalar marshalling for original basemsg.h; not a native CSR struct
 * copy. Keep pointer/resource materialization outside these fragments. */
#include "basesrv.h"
#include <base_values.h>
#include <string.h>
typedef char check_values_size[sizeof(broker_vdm_check_values)==36?1:-1];
typedef char update_values_size[sizeof(broker_vdm_update_values)==16?1:-1];
typedef char get_values_size[sizeof(broker_vdm_get_values)==32?1:-1];
#define CHECK_VALUES(X) X(task,iTask,ULONG) X(binary_type,BinaryType,ULONG) X(code_page,CodePage,ULONG) \
    X(creation_flags,dwCreationFlags,ULONG) X(drive,CurDrive,USHORT) X(state,VDMState,USHORT)
#define UPDATE_VALUES(X) X(task,iTask,ULONG) X(binary_type,BinaryType,ULONG) \
    X(entry,EntryIndex,WORD) X(creation_state,VDMCreationState,WORD)
#define GET_VALUES(X) X(task,iTask,ULONG) X(code_page,CodePage,ULONG) X(creation_flags,dwCreationFlags,ULONG) \
    X(exit_code,ExitCode,ULONG) X(drive,CurrentDrive,USHORT) X(state,VDMState,USHORT) X(from_bat,fComingFromBat,BOOLEAN)
#define ENCODE_FIELD(w,n,t) value.w=source->n;
#define ENCODE_CASE(op,type,native,member,fields) case op: { \
    type value; const native *source=&message->u.member; \
    if (bytes!=sizeof(value)) return FALSE; fields(ENCODE_FIELD) \
    memcpy(output,&value,sizeof(value)); return TRUE; }
BOOL OpenNtBaseEncodeValues(const BASE_API_MSG *message, uint32_t operation,
    void *output, uint32_t bytes)
{
    if (!message || !output) return FALSE;
    switch (operation) {
    ENCODE_CASE(BROKER_VDM_CHECK,broker_vdm_check_values,BASE_CHECKVDM_MSG,CheckVDM,CHECK_VALUES)
    ENCODE_CASE(BROKER_VDM_UPDATE,broker_vdm_update_values,BASE_UPDATE_VDM_ENTRY_MSG,UpdateVDMEntry,UPDATE_VALUES)
    case BROKER_VDM_GET_NEXT: {
        broker_vdm_get_values value;
        const BASE_GET_NEXT_VDM_COMMAND_MSG *source=&message->u.GetNextVDMCommand;
        if (bytes!=sizeof(value)) return FALSE;
        GET_VALUES(ENCODE_FIELD)
        value.standard_mask=(source->StdIn ? 1u : 0u) |
            (source->StdOut ? 2u : 0u) | (source->StdErr ? 4u : 0u);
        memcpy(output,&value,sizeof(value)); return TRUE;
    }
    default: return FALSE;
    }
}
#define DECODE_FIELD(w,n,t) target->n=(t)value.w;
#define DECODE_CASE(op,type,native,member,fields,invalid) case op: { \
    type value; native *target=&message->u.member; \
    if (bytes!=sizeof(value)) return FALSE; memcpy(&value,input,sizeof(value)); \
    if (invalid) return FALSE; fields(DECODE_FIELD) return TRUE; }
BOOL OpenNtBaseDecodeValues(const void *input, uint32_t bytes,
    uint32_t operation, PBASE_API_MSG message)
{
    if (!input || !message) return FALSE;
    switch (operation) {
    DECODE_CASE(BROKER_VDM_CHECK,broker_vdm_check_values,BASE_CHECKVDM_MSG,CheckVDM,CHECK_VALUES,
        value.drive>0xffffu || value.state>0xffffu)
    DECODE_CASE(BROKER_VDM_UPDATE,broker_vdm_update_values,BASE_UPDATE_VDM_ENTRY_MSG,UpdateVDMEntry,UPDATE_VALUES,
        value.entry>0xffffu || value.creation_state>0xffffu)
    case BROKER_VDM_GET_NEXT: {
        broker_vdm_get_values value;
        BASE_GET_NEXT_VDM_COMMAND_MSG *target=&message->u.GetNextVDMCommand;
        if (bytes!=sizeof(value)) return FALSE; memcpy(&value,input,sizeof(value));
        if (value.drive>0xffffu || value.state>0xffffu || value.from_bat>0xffu ||
            value.standard_mask>7u) return FALSE;
        GET_VALUES(DECODE_FIELD)
        /* These are Boolean placeholders until the RPC client either replaces
         * them with typed attachments or selects its already-inherited stream.
         * No sender-local HANDLE appears in this scalar wire fragment. */
        target->StdIn=(HANDLE)(ULONG_PTR)(value.standard_mask&1u);
        target->StdOut=(HANDLE)(ULONG_PTR)((value.standard_mask>>1)&1u);
        target->StdErr=(HANDLE)(ULONG_PTR)((value.standard_mask>>2)&1u);
        return TRUE;
    }
    default: return FALSE;
    }
}
