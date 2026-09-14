/* Compose copied original command fields at the unavailable CSR transport
 * boundary. No selection, retry, resource duplication or record policy. */
#include "basesrv.h"
#include <base_command.h>
#include <base_payload.h>
#include <base_values.h>
#include <base_startup.h>
#include <string.h>
typedef struct check_prefix {
    broker_vdm_message_header header;
    broker_vdm_check_values values;
    broker_vdm_startup startup;
    uint32_t console_mode;
} check_prefix;
typedef char check_prefix_size[sizeof(check_prefix)==100?1:-1];

typedef struct check_reply {
    broker_vdm_message_header header;
    uint32_t task,state;
} check_reply;
typedef char check_reply_size[sizeof(check_reply)==40?1:-1];
BOOL OpenNtBaseEncodeCheckReply(const BASE_API_MSG *message,uint32_t request,uint32_t generation,
    void *output,uint32_t capacity,uint32_t *required)
{
    check_reply wire={0};
    if (required) *required=0;
    if (!message || !request || !generation || !required) return FALSE;
    wire.header.version=BROKER_VDM_MESSAGE_VERSION;wire.header.bytes=sizeof(wire);
    wire.header.operation=BROKER_VDM_CHECK;wire.header.request_id=request;
    wire.header.generation=generation;wire.header.reply=1;
    wire.header.status=message->ReturnValue;wire.header.payload_bytes=8;
    if (NT_SUCCESS((NTSTATUS)message->ReturnValue)) {
        wire.task=message->u.CheckVDM.iTask;wire.state=message->u.CheckVDM.VDMState;
    }
    *required=sizeof(wire);
    if (!output) return TRUE;
    if (capacity<sizeof(wire)) return FALSE;
    memcpy(output,&wire,sizeof(wire));return TRUE;
}
BOOL OpenNtBaseApplyCheckReply(const void *input,uint32_t bytes,uint32_t generation,
    uint32_t request,PBASE_API_MSG message)
{
    check_reply wire;
    broker_vdm_message_header header;
    if (!message || !request || bytes!=sizeof(wire) ||
        !broker_vdm_message_read(input,bytes,generation,1,&header) ||
        header.operation!=BROKER_VDM_CHECK || header.request_id!=request) return FALSE;
    memcpy(&wire,input,sizeof(wire));
    if (wire.state>0xffffu || (!NT_SUCCESS((NTSTATUS)header.status) && (wire.task || wire.state))) return FALSE;
    if (NT_SUCCESS((NTSTATUS)header.status)) {
        message->u.CheckVDM.iTask=wire.task;message->u.CheckVDM.VDMState=(USHORT)wire.state;
    }
    message->ReturnValue=header.status;
    return TRUE;
}
typedef struct get_prefix {
    broker_vdm_message_header header;
    broker_vdm_get_values values;
    broker_vdm_startup startup;
} get_prefix;
typedef char get_prefix_size[sizeof(get_prefix)==100?1:-1];
typedef struct update_request {
    broker_vdm_message_header header;
    broker_vdm_update_values values;
} update_request;
typedef char update_request_size[sizeof(update_request)==48?1:-1];

BOOL OpenNtBaseEncodeUpdateReply(const BASE_API_MSG *message,uint32_t request,uint32_t generation,
    void *output,uint32_t capacity,uint32_t *required)
{
    broker_vdm_message_header wire={0};
    if (required) *required=0;
    if (!message || !required || !request || !generation) return FALSE;
    wire.version=BROKER_VDM_MESSAGE_VERSION;wire.bytes=sizeof(wire);
    wire.operation=BROKER_VDM_UPDATE;wire.request_id=request;wire.generation=generation;
    wire.reply=1;wire.status=message->ReturnValue;
    *required=sizeof(wire);
    if (!output) return TRUE;
    if (capacity<sizeof(wire)) return FALSE;
    memcpy(output,&wire,sizeof(wire));return TRUE;
}
BOOL OpenNtBaseApplyUpdateReply(const void *input,uint32_t bytes,uint32_t generation,
    uint32_t request,PBASE_API_MSG message)
{
    broker_vdm_message_header wire;
    if (!message || !request || bytes!=sizeof(wire) ||
        !broker_vdm_message_read(input,bytes,generation,1,&wire) ||
        wire.operation!=BROKER_VDM_UPDATE || wire.request_id!=request) return FALSE;
    message->ReturnValue=wire.status;
    return TRUE;
}

BOOL OpenNtBaseEncodeUpdateCommand(const BASE_API_MSG *message,uint32_t request,uint32_t generation,
    void *output,uint32_t capacity,uint32_t *required)
{
    update_request wire={0};
    if (required) *required=0;
    if (!message || !required || !request || !generation ||
        message->u.UpdateVDMEntry.EntryIndex>UPDATE_VDM_HOOKED_CTRLC) return FALSE;
    wire.values.entry=message->u.UpdateVDMEntry.EntryIndex;
    wire.values.binary_type=message->u.UpdateVDMEntry.BinaryType;
    wire.values.task=message->u.UpdateVDMEntry.iTask;
    if (wire.values.entry==UPDATE_VDM_UNDO_CREATION) wire.values.creation_state=message->u.UpdateVDMEntry.VDMCreationState;
    wire.header.version=BROKER_VDM_MESSAGE_VERSION;
    wire.header.bytes=sizeof(wire); wire.header.operation=BROKER_VDM_UPDATE;
    wire.header.request_id=request; wire.header.generation=generation;
    wire.header.payload_bytes=sizeof(wire.values);
    *required=sizeof(wire);
    if (!output) return TRUE;
    if (capacity<sizeof(wire)) return FALSE;
    memcpy(output,&wire,sizeof(wire));
    return TRUE;
}
BOOL OpenNtBaseDecodeUpdateCommand(const void *input,uint32_t bytes,uint32_t generation,
    PBASE_API_MSG message,uint32_t *request)
{
    update_request wire;
    broker_vdm_message_header header;
    if (!message || !request || bytes!=sizeof(wire) ||
        !broker_vdm_message_read(input,bytes,generation,0,&header) ||
        header.operation!=BROKER_VDM_UPDATE) return FALSE;
    memcpy(&wire,input,sizeof(wire));
    if (wire.values.entry>UPDATE_VDM_HOOKED_CTRLC ||
        (wire.values.entry!=UPDATE_VDM_UNDO_CREATION && wire.values.creation_state) ||
        !OpenNtBaseDecodeValues(&wire.values,sizeof(wire.values),BROKER_VDM_UPDATE,message)) return FALSE;
    *request=header.request_id;
    return TRUE;
}

BOOL OpenNtBaseEncodeCheckCommand(const BASE_API_MSG *message, uint32_t request,
    uint32_t generation, void *output, uint32_t capacity, uint32_t *required)
{
    check_prefix prefix={0};
    uint32_t payloadBytes,total;
    if (required) *required=0;
    if (!message || !required || !request || !generation ||
        !OpenNtBaseEncodeCheckPayload(&message->u.CheckVDM,NULL,0,&payloadBytes) ||
        payloadBytes>UINT32_MAX-sizeof(prefix)) return FALSE;
    total=sizeof(prefix)+payloadBytes;
    /* iTask is output-only; VDMState is initialized to FALSE by BaseCheckVDM.
     * Neither caller scratch value is a command request field. */
    prefix.values.binary_type=message->u.CheckVDM.BinaryType;
    prefix.values.code_page=message->u.CheckVDM.CodePage;
    prefix.values.creation_flags=message->u.CheckVDM.dwCreationFlags;
    prefix.values.drive=message->u.CheckVDM.CurDrive;
    if (message->u.CheckVDM.ConsoleHandle==OPENNT_BASE_CONSOLE_WOW)
        prefix.console_mode=2;
    else if (!message->u.CheckVDM.ConsoleHandle)
        prefix.console_mode=0;
    else
        prefix.console_mode=1;
    OpenNtBaseEncodeStartup(message->u.CheckVDM.StartupInfo,&prefix.startup);
    prefix.header.version=BROKER_VDM_MESSAGE_VERSION;
    prefix.header.bytes=total; prefix.header.operation=BROKER_VDM_CHECK;
    prefix.header.request_id=request; prefix.header.generation=generation;
    prefix.header.payload_bytes=total-sizeof(prefix.header);
    *required=total;
    if (!output) return TRUE;
    if (capacity<total) return FALSE;
    if (!OpenNtBaseEncodeCheckPayload(&message->u.CheckVDM,
        (unsigned char *)output+sizeof(prefix),payloadBytes,&payloadBytes)) return FALSE;
    memcpy(output,&prefix,sizeof(prefix));
    return TRUE;
}

BOOL OpenNtBaseDecodeCheckCommand(void *input, uint32_t bytes, uint32_t generation,
    PBASE_API_MSG message, STARTUPINFOA *startup, uint32_t *request)
{
    check_prefix prefix;
    broker_vdm_message_header header;
    BASE_API_MSG decoded;
    STARTUPINFOA decodedStartup;
    if (!message || !startup || !request || bytes<sizeof(prefix) ||
        !broker_vdm_message_read(input,bytes,generation,0,&header) ||
        header.operation!=BROKER_VDM_CHECK) return FALSE;
    memcpy(&prefix,input,sizeof(prefix));
    decoded=*message;
    if (prefix.values.task || prefix.values.state || prefix.console_mode>2 ||
        !OpenNtBaseDecodeValues(&prefix.values,sizeof(prefix.values),BROKER_VDM_CHECK,&decoded) ||
        !OpenNtBaseDecodeStartup(&prefix.startup,&decodedStartup) ||
        !OpenNtBaseDecodeCheckPayload((unsigned char *)input+sizeof(prefix),
            bytes-sizeof(prefix),&decoded.u.CheckVDM)) return FALSE;
    decoded.u.CheckVDM.StartupInfo=prefix.startup.present?startup:NULL;
    decoded.u.CheckVDM.ConsoleHandle=prefix.console_mode==2 ? OPENNT_BASE_CONSOLE_WOW :
        prefix.console_mode==1 ? OPENNT_BASE_CONSOLE_EXISTING : OPENNT_BASE_CONSOLE_NEW;
    *startup=decodedStartup;
    *message=decoded;
    *request=header.request_id;
    return TRUE;
}

BOOL OpenNtBaseEncodeGetCommand(const BASE_API_MSG *message,uint32_t request,uint32_t generation,
    void *output,uint32_t capacity,uint32_t *required)
{
    get_prefix prefix={0};
    uint32_t payloadBytes;
    if (required) *required=0;
    if (!message || !required || !request || !generation || !message->u.GetNextVDMCommand.StartupInfo ||
        !OpenNtBaseEncodeGetRequest(&message->u.GetNextVDMCommand,NULL,0,&payloadBytes)) return FALSE;
    /* Original vdm.c initializes only these scalar inputs. In particular,
     * StartupInfo is an output capture, not an initialized STARTUPINFO. */
    prefix.values.state=message->u.GetNextVDMCommand.VDMState;
    prefix.values.exit_code=message->u.GetNextVDMCommand.ExitCode;
    if (prefix.values.state & ASKING_FOR_PIF) prefix.values.task=message->u.GetNextVDMCommand.iTask;
    prefix.startup.present=1;
    prefix.header.version=BROKER_VDM_MESSAGE_VERSION;
    prefix.header.bytes=sizeof(prefix)+payloadBytes;
    prefix.header.operation=BROKER_VDM_GET_NEXT;
    prefix.header.request_id=request; prefix.header.generation=generation;
    prefix.header.payload_bytes=prefix.header.bytes-sizeof(prefix.header);
    *required=prefix.header.bytes;
    if (!output) return TRUE;
    if (capacity<*required) return FALSE;
    if (!OpenNtBaseEncodeGetRequest(&message->u.GetNextVDMCommand,
        (unsigned char *)output+sizeof(prefix),payloadBytes,&payloadBytes)) return FALSE;
    memcpy(output,&prefix,sizeof(prefix));
    return TRUE;
}

DWORD OpenNtBasePrepareGetCommand(const void *input,uint32_t bytes,uint32_t generation,
    PBASE_API_MSG message,OPENNT_BASE_GET_COMMAND *state)
{
    get_prefix prefix;
    broker_vdm_message_header header;
    BASE_API_MSG decoded;
    STARTUPINFOA startup;
    DWORD error;
    void *reply;
    if (!message || !state || state->reply || state->payload.bytes || bytes<sizeof(prefix) ||
        !broker_vdm_message_read(input,bytes,generation,0,&header) ||
        header.operation!=BROKER_VDM_GET_NEXT) return ERROR_INVALID_PARAMETER;
    memcpy(&prefix,input,sizeof(prefix));
    decoded=*message;
    if (prefix.startup.present!=1 || prefix.startup.x || prefix.startup.y || prefix.startup.x_size ||
        prefix.startup.y_size || prefix.startup.x_chars || prefix.startup.y_chars || prefix.startup.fill ||
        prefix.startup.flags || prefix.startup.show || prefix.values.code_page || prefix.values.creation_flags ||
        prefix.values.drive || prefix.values.from_bat ||
        (!(prefix.values.state & ASKING_FOR_PIF) && prefix.values.task) ||
        !OpenNtBaseDecodeValues(&prefix.values,sizeof(prefix.values),BROKER_VDM_GET_NEXT,&decoded) ||
        !OpenNtBaseDecodeStartup(&prefix.startup,&startup)) return ERROR_INVALID_PARAMETER;
    error=OpenNtBasePrepareGetPayload((const unsigned char *)input+sizeof(prefix),bytes-sizeof(prefix),
        &decoded.u.GetNextVDMCommand,&state->payload);
    if (error) return error;
    if (state->payload.size>UINT32_MAX-sizeof(prefix)) {
        OpenNtBaseReleaseGetPayload(&state->payload); return ERROR_ARITHMETIC_OVERFLOW;
    }
    reply=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(prefix)+state->payload.size);
    if (!reply) { OpenNtBaseReleaseGetPayload(&state->payload); return ERROR_NOT_ENOUGH_MEMORY; }
    state->reply=reply; state->reply_bytes=sizeof(prefix)+state->payload.size;
    state->request=header; state->startup=startup;
    decoded.u.GetNextVDMCommand.StartupInfo=&state->startup;
    *message=decoded;
    return 0;
}

BOOL OpenNtBaseFinishGetCommand(const BASE_API_MSG *message,OPENNT_BASE_GET_COMMAND *state)
{
    get_prefix prefix={0};
    if (!message || !state || !state->reply ||
        !OpenNtBaseFinishGetPayload(&message->u.GetNextVDMCommand,&state->payload) ||
        !OpenNtBaseEncodeValues(message,BROKER_VDM_GET_NEXT,&prefix.values,sizeof(prefix.values))) return FALSE;
    OpenNtBaseEncodeStartup(&state->startup,&prefix.startup);
    prefix.header=state->request;
    prefix.header.reply=1; prefix.header.status=message->ReturnValue;
    prefix.header.bytes=state->reply_bytes;
    prefix.header.payload_bytes=state->reply_bytes-sizeof(prefix.header);
    memcpy(state->reply,&prefix,sizeof(prefix));
    memcpy((unsigned char *)state->reply+sizeof(prefix),state->payload.bytes,state->payload.size);
    return TRUE;
}

BOOL OpenNtBaseApplyGetCommand(const void *input,uint32_t bytes,uint32_t generation,
    uint32_t request,PBASE_API_MSG message)
{
    get_prefix prefix;
    broker_vdm_message_header header;
    BASE_API_MSG decoded;
    STARTUPINFOA startup;
    if (!message || !message->u.GetNextVDMCommand.StartupInfo || bytes<sizeof(prefix) ||
        !broker_vdm_message_read(input,bytes,generation,1,&header) ||
        header.operation!=BROKER_VDM_GET_NEXT || header.request_id!=request) return FALSE;
    memcpy(&prefix,input,sizeof(prefix)); decoded=*message;
    if (prefix.startup.present!=1 ||
        !OpenNtBaseDecodeValues(&prefix.values,sizeof(prefix.values),BROKER_VDM_GET_NEXT,&decoded) ||
        !OpenNtBaseDecodeStartup(&prefix.startup,&startup) ||
        !OpenNtBaseApplyGetPayload((const unsigned char *)input+sizeof(prefix),bytes-sizeof(prefix),
            &decoded.u.GetNextVDMCommand)) return FALSE;
    /* Every validation precedes publication, including buffer capacity. */
    *decoded.u.GetNextVDMCommand.StartupInfo=startup;
    decoded.ReturnValue=header.status;
    *message=decoded;
    return TRUE;
}

void OpenNtBaseReleaseGetCommand(OPENNT_BASE_GET_COMMAND *state)
{
    if (!state) return;
    OpenNtBaseReleaseGetPayload(&state->payload);
    if (state->reply) HeapFree(GetProcessHeap(),0,state->reply);
    memset(state,0,sizeof(*state));
}
