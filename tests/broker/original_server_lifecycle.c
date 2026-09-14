/* Test host mechanics only. All VDM record policy is linked from srvvdm.c. */
#include "basesrv.h"
#include <base_interactive.h>
#include <base_capture.h>
#include <base_config.h>
#include <base_process.h>
#include <base_dispatch.h>
#include <base_startup.h>
#include <base_payload.h>
#include <base_values.h>
#include <base_command.h>
#include "broker/vdm_receipt.h"
#include "broker/vdm_delivery.h"
#include <stdio.h>
#include <string.h>

static CSR_PROCESS caller;
static CSR_THREAD thread;
static OPENNT_BASE_PROCESS_REGISTRY processRegistry;
typedef struct stream_rollback_test {
    broker_vdm_receipts receipts;
    broker_vdm_delivery delivery;
    ULONG attempts;
} stream_rollback_test;
static DWORD revoke_pending_stream(void *context,uint32_t generation,uint32_t receipt)
{
    stream_rollback_test *test=context;
    return broker_vdm_receipt_revoke(&test->receipts,generation,receipt);
}
static NTSTATUS deliver_stream_then_fail(void *context,HANDLE sourceProcess,HANDLE source,HANDLE targetProcess,
    PHANDLE target,ACCESS_MASK access,ULONG attributes,ULONG options)
{
    stream_rollback_test *test=context;
    broker_vdm_delivery_item *ticket;
    uint32_t receipt;
    DWORD error;
    (void)sourceProcess; (void)targetProcess; (void)access; (void)attributes; (void)options;
    error=broker_vdm_delivery_prepare(&test->delivery,test,revoke_pending_stream,&ticket);
    if (error) return (NTSTATUS)0xc0000017L;
    if (++test->attempts==2) {
        broker_vdm_delivery_forget(&test->delivery,ticket); /* Explicit non-delivery. */
        return (NTSTATUS)0xc0000022L;
    }
    error=broker_vdm_receipt_accept(&test->receipts,BROKER_VDM_STDIN,source,&receipt);
    if (error) {
        broker_vdm_delivery_forget(&test->delivery,ticket);
        return (NTSTATUS)0xc0000008L;
    }
    if (broker_vdm_delivery_acknowledge(ticket,7,receipt)) return (NTSTATUS)0xc000000dL;
    *target=(HANDLE)receipt;
    return STATUS_SUCCESS;
}
typedef struct resource_failure_test {
    HANDLE event;
    ULONG duplicated,closed;
} resource_failure_test;
static NTSTATUS reject_delivery(void *context,HANDLE sourceProcess,HANDLE source,HANDLE targetProcess,
    PHANDLE target,ACCESS_MASK access,ULONG attributes,ULONG options)
{
    resource_failure_test *test=(resource_failure_test *)context;
    (void)sourceProcess; (void)targetProcess; (void)target; (void)access; (void)attributes;
    if (options!=DUPLICATE_SAME_ACCESS) return (NTSTATUS)0xc000000dL;
    test->event=source; ++test->duplicated;
    return (NTSTATUS)0xc0000022L;
}
static NTSTATUS close_failed_delivery(void *context,HANDLE event)
{
    resource_failure_test *test=(resource_failure_test *)context;
    if (event!=test->event || test->duplicated!=1) return (NTSTATUS)0xc000000dL;
    ++test->closed;
    return CloseHandle(event)?0:(NTSTATUS)0xc0000008L;
}
static NTSTATUS partial_stream_delivery(void *context,HANDLE sourceProcess,HANDLE source,HANDLE targetProcess,
    PHANDLE target,ACCESS_MASK access,ULONG attributes,ULONG options)
{
    resource_failure_test *test=(resource_failure_test *)context;
    (void)sourceProcess; (void)targetProcess; (void)access; (void)attributes; (void)options;
    ++test->duplicated;
    if (test->duplicated==1 && source==(HANDLE)10) {
        *target=(HANDLE)101; /* Test-only distinct destination identity; no OS resource. */
        return 0;
    }
    return (NTSTATUS)0xc0000022L;
}
typedef struct stream_identity_test {
    ULONG copied, revoked;
    HANDLE closed[3];
} stream_identity_test;
static NTSTATUS stream_identity_delivery(void *context,HANDLE sourceProcess,HANDLE source,HANDLE targetProcess,
    PHANDLE target,ACCESS_MASK access,ULONG attributes,ULONG options)
{
    stream_identity_test *test=(stream_identity_test *)context;
    (void)sourceProcess; (void)access;
    if (options==DUPLICATE_CLOSE_SOURCE) {
        if (targetProcess || target || test->revoked>=3) return (NTSTATUS)0xc000000dL;
        test->closed[test->revoked++]=source;
        return 0;
    }
    if (options!=DUPLICATE_SAME_ACCESS || attributes!=OBJ_INHERIT || !target)
        return (NTSTATUS)0xc000000dL;
    *target=(HANDLE)(100+ ++test->copied); /* Distinct test receipt identities. */
    return 0;
}
typedef struct registry_remove_test {
    HANDLE started;
    HANDLE finished;
    BOOL removed;
} registry_remove_test;
static DWORD WINAPI remove_registered_peer(LPVOID context)
{
    registry_remove_test *test=(registry_remove_test *)context;
    SetEvent(test->started);
    test->removed=OpenNtBaseRemoveProcess(&processRegistry,&caller);
    SetEvent(test->finished);
    return test->removed?0:1;
}
extern HANDLE hwndWowExec;
extern ULONG ulWowExecProcessSequenceNumber;
extern NTSTATUS (*UserTestTokenForInteractive)(HANDLE, PLUID);
extern PWOWHEAD WOWHead;
extern LUID WowAuthId;
BOOL BaseUpdateVDMEntry(ULONG, HANDLE *, ULONG, ULONG);
BOOL BaseCheckForVDM(HANDLE, LPDWORD);
BOOL BaseCheckVDM(ULONG, PCWCH, PCWCH, PCWCH, ANSI_STRING *, PBASE_API_MSG, PULONG, DWORD, LPSTARTUPINFOW);
PFNNOTIFYPROCESSCREATE UserNotifyProcessCreate = NULL;

/* Test dispatch only; original capture routines use a real private heap. */
PVOID CsrPortHeap;
#define captures capture_blocks()
static ULONG launchCalls;
static HANDLE enqueueGate, queuedParent;
static ULONG retryCalls, retryExit;
static ULONG enqueueStatus;
static ULONG capture_blocks(void)
{
    PROCESS_HEAP_ENTRY entry={0};
    ULONG count=0;
    if(!HeapLock(CsrPortHeap)) return MAXULONG;
    while(HeapWalk(CsrPortHeap,&entry)) if(entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) ++count;
    HeapUnlock(CsrPortHeap);
    return count;
}
static BOOL scalar_roundtrip(PBASE_API_MSG message, uint32_t operation)
{
    unsigned char values[sizeof(broker_vdm_get_values)], zero[sizeof(values)]={0};
    uint32_t size=operation==BROKER_VDM_CHECK?sizeof(broker_vdm_check_values):
        operation==BROKER_VDM_UPDATE?sizeof(broker_vdm_update_values):sizeof(broker_vdm_get_values);
    BASE_API_MSG copy=*message;
    if (operation!=BROKER_VDM_CHECK && operation!=BROKER_VDM_UPDATE && operation!=BROKER_VDM_GET_NEXT) return TRUE;
    return OpenNtBaseEncodeValues(message,operation,values,size) &&
        OpenNtBaseDecodeValues(zero,size,operation,&copy) &&
        OpenNtBaseDecodeValues(values,size,operation,&copy) && !memcmp(message,&copy,sizeof(copy));
}

static BOOL scalar_negatives(void)
{
    BASE_API_MSG original, target;
    uint32_t op,i,size,values[7],expected[7]={1,2,3,4,5,6,7};
    memset(&original,0xa5,sizeof(original));
    for (op=BROKER_VDM_CHECK;op<=BROKER_VDM_GET_NEXT;++op) {
        size=op==BROKER_VDM_CHECK?24:op==BROKER_VDM_UPDATE?16:28;
        if (op==BROKER_VDM_CHECK) {
            original.u.CheckVDM.iTask=1; original.u.CheckVDM.BinaryType=2;
            original.u.CheckVDM.CodePage=3; original.u.CheckVDM.dwCreationFlags=4;
            original.u.CheckVDM.CurDrive=5; original.u.CheckVDM.VDMState=6;
        } else if (op==BROKER_VDM_UPDATE) {
            original.u.UpdateVDMEntry.iTask=1; original.u.UpdateVDMEntry.BinaryType=2;
            original.u.UpdateVDMEntry.EntryIndex=3; original.u.UpdateVDMEntry.VDMCreationState=4;
        } else {
            original.u.GetNextVDMCommand.iTask=1; original.u.GetNextVDMCommand.CodePage=2;
            original.u.GetNextVDMCommand.dwCreationFlags=3; original.u.GetNextVDMCommand.ExitCode=4;
            original.u.GetNextVDMCommand.CurrentDrive=5; original.u.GetNextVDMCommand.VDMState=6;
            original.u.GetNextVDMCommand.fComingFromBat=7;
        }
        if (!scalar_roundtrip(&original,op) || !OpenNtBaseEncodeValues(&original,op,values,size) ||
            memcmp(values,expected,size)) return FALSE;
        for (i=0;i<size;++i) {
            target=original;
            if (OpenNtBaseDecodeValues(values,i,op,&target) || memcmp(&target,&original,sizeof(target))) return FALSE;
        }
        /* Each narrowing field independently rejects overflow, without
         * changing any preceding scalar, pointer or resource field. */
        for (i=op==BROKER_VDM_UPDATE?2:4;i<size/4;++i) {
            uint32_t saved=values[i];
            values[i]=(op==BROKER_VDM_GET_NEXT && i==6)?0x100u:0x10000u;
            target=original;
            if (OpenNtBaseDecodeValues(values,size,op,&target) || memcmp(&target,&original,sizeof(target))) return FALSE;
            values[i]=saved;
        }
    }
    target=original;
    return !OpenNtBaseDecodeValues(values,28,0,&target) && !memcmp(&target,&original,sizeof(target));
}

static BOOL command_negatives(void *payload,uint32_t bytes,const BASE_API_MSG *original)
{
    BASE_API_MSG copy;
    STARTUPINFOA startup, saved;
    uint32_t cut,id,invalid=2,old;
    unsigned char *presence=(unsigned char *)payload+sizeof(broker_vdm_message_header)+sizeof(broker_vdm_check_values);
    memset(&saved,0xa5,sizeof(saved));
    for (cut=0;cut<=bytes;++cut) {
        copy=*original; startup=saved; id=0xabcdu;
        if (OpenNtBaseDecodeCheckCommand(payload,cut,cut==bytes?2:1,&copy,&startup,&id) ||
            memcmp(&copy,original,sizeof(copy)) || memcmp(&startup,&saved,sizeof(saved)) || id!=0xabcdu) return FALSE;
    }
    memcpy(&old,presence,sizeof(old)); memcpy(presence,&invalid,sizeof(invalid));
    copy=*original; startup=saved; id=0xabcdu;
    invalid=OpenNtBaseDecodeCheckCommand(payload,bytes,1,&copy,&startup,&id);
    memcpy(presence,&old,sizeof(old));
    return !invalid && !memcmp(&copy,original,sizeof(copy)) &&
        !memcmp(&startup,&saved,sizeof(saved)) && id==0xabcdu;
}

NTSTATUS NTAPI CsrClientCallServer(PCSR_API_MSG message, PCSR_CAPTURE_HEADER capture,
    CSR_API_NUMBER number, ULONG length)
{
    NTSTATUS result;
    STARTUPINFOA decoded;
    LPSTARTUPINFOA savedStartup=NULL;
    BASE_CHECKVDM_MSG savedCheck;
    void *payload=NULL;
    uint32_t payloadBytes,requestId;
    BASE_GET_NEXT_VDM_COMMAND_MSG savedGet;
    OPENNT_BASE_GET_COMMAND getCommand={0};
    unsigned char getRequest[100+16*BROKER_VDM_PAYLOAD_FIELDS];
    (void)capture;
    if (!scalar_negatives()) return STATUS_INVALID_PARAMETER;
    if (number==CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepUpdateVDMEntry)) {
        unsigned char update[48];
        if (!OpenNtBaseEncodeUpdateCommand((PBASE_API_MSG)message,3,1,update,sizeof(update),&payloadBytes) ||
            !OpenNtBaseDecodeUpdateCommand(update,payloadBytes,1,(PBASE_API_MSG)message,&requestId) || requestId!=3)
            return STATUS_INVALID_PARAMETER;
    }
    if (number == CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCheckVDM)) ++launchCalls;
    if (enqueueGate && number == CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand)) {
        PBASE_GET_NEXT_VDM_COMMAND_MSG request = &((PBASE_API_MSG)message)->u.GetNextVDMCommand;
        ++retryCalls;
        if (retryCalls == 2) retryExit = request->ExitCode;
    }
    if (number == CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCheckVDM)) {
        PBASE_CHECKVDM_MSG request=&((PBASE_API_MSG)message)->u.CheckVDM;
        savedCheck=*request;
        if (!OpenNtBaseEncodeCheckCommand((PBASE_API_MSG)message,1,1,NULL,0,&payloadBytes)) return STATUS_INVALID_PARAMETER;
        payload=HeapAlloc(GetProcessHeap(),0,payloadBytes);
        if (!payload) return STATUS_NO_MEMORY;
        if (!OpenNtBaseEncodeCheckCommand((PBASE_API_MSG)message,1,1,payload,payloadBytes,&payloadBytes)) {
            HeapFree(GetProcessHeap(),0,payload); return STATUS_INVALID_PARAMETER;
        }
        if (!command_negatives(payload,payloadBytes,(PBASE_API_MSG)message)) {
            HeapFree(GetProcessHeap(),0,payload); return STATUS_INVALID_PARAMETER;
        }
        savedStartup=request->StartupInfo;
        if (!OpenNtBaseDecodeCheckCommand(payload,payloadBytes,1,(PBASE_API_MSG)message,&decoded,&requestId) || requestId!=1) {
            *request=savedCheck; HeapFree(GetProcessHeap(),0,payload); return STATUS_INVALID_PARAMETER;
        }
    }
    if (number == CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand)) {
        PBASE_GET_NEXT_VDM_COMMAND_MSG request=&((PBASE_API_MSG)message)->u.GetNextVDMCommand;
        savedGet=*request;
        if (!OpenNtBaseEncodeGetCommand((PBASE_API_MSG)message,2,1,getRequest,sizeof(getRequest),&payloadBytes)) return STATUS_INVALID_PARAMETER;
        if (OpenNtBasePrepareGetCommand(getRequest,payloadBytes,1,(PBASE_API_MSG)message,&getCommand)) return STATUS_NO_MEMORY;
    }
    result = OpenNtBaseDispatchOperation(message,OpenNtBaseVdmOperation(number),length);
    if (!scalar_roundtrip((PBASE_API_MSG)message,OpenNtBaseVdmOperation(number))) result=STATUS_INVALID_PARAMETER;
    if (number == CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand)) {
        PBASE_GET_NEXT_VDM_COMMAND_MSG response=&((PBASE_API_MSG)message)->u.GetNextVDMCommand;
        BOOL prepared=OpenNtBaseFinishGetCommand((PBASE_API_MSG)message,&getCommand);
#define RESTORE_GET(p,n) response->p=savedGet.p; response->n=savedGet.n;
        RESTORE_GET(CmdLine,CmdLen) RESTORE_GET(AppName,AppLen)
        RESTORE_GET(PifFile,PifLen) RESTORE_GET(CurDirectory,CurDirectoryLen)
        RESTORE_GET(Env,EnvLen) RESTORE_GET(Desktop,DesktopLen)
        RESTORE_GET(Title,TitleLen) RESTORE_GET(Reserved,ReservedLen)
#undef RESTORE_GET
        response->StartupInfo=savedGet.StartupInfo;
        if (!prepared || OpenNtBaseApplyGetCommand(getCommand.reply,getCommand.reply_bytes,2,2,(PBASE_API_MSG)message) ||
            OpenNtBaseApplyGetCommand(getCommand.reply,getCommand.reply_bytes,1,3,(PBASE_API_MSG)message) ||
            !OpenNtBaseApplyGetCommand(getCommand.reply,getCommand.reply_bytes,1,2,(PBASE_API_MSG)message)) result=STATUS_INVALID_PARAMETER;
        OpenNtBaseReleaseGetCommand(&getCommand);
    }
    if (number == CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCheckVDM)) {
        PBASE_CHECKVDM_MSG request=&((PBASE_API_MSG)message)->u.CheckVDM;
        request->StartupInfo=savedStartup;
        request->CmdLine=savedCheck.CmdLine; request->AppName=savedCheck.AppName;
        request->PifFile=savedCheck.PifFile; request->CurDirectory=savedCheck.CurDirectory;
        request->Env=savedCheck.Env; request->Desktop=savedCheck.Desktop;
        request->Title=savedCheck.Title; request->Reserved=savedCheck.Reserved;
        {
            unsigned char wire[40];
            uint32_t size,cut;
            BASE_API_MSG original=*(PBASE_API_MSG)message,copy;
            if (!OpenNtBaseEncodeCheckReply(&original,1,1,wire,sizeof(wire),&size) || size!=sizeof(wire))
                result=STATUS_INVALID_PARAMETER;
            else {
                for (cut=0;cut<sizeof(wire);++cut) {
                    copy=original;
                    if (OpenNtBaseApplyCheckReply(wire,cut,1,1,&copy) || memcmp(&copy,&original,sizeof(copy)))
                        result=STATUS_INVALID_PARAMETER;
                }
                copy=original;
                if (OpenNtBaseApplyCheckReply(wire,size,2,1,&copy) ||
                    OpenNtBaseApplyCheckReply(wire,size,1,2,&copy) || memcmp(&copy,&original,sizeof(copy)))
                    result=STATUS_INVALID_PARAMETER;
                if (NT_SUCCESS((NTSTATUS)original.ReturnValue)) {
                    copy.u.CheckVDM.iTask=0xdeadbeef;copy.u.CheckVDM.VDMState=0xffff;
                }
                if (!OpenNtBaseApplyCheckReply(wire,size,1,1,&copy) || memcmp(&copy,&original,sizeof(copy)))
                    result=STATUS_INVALID_PARAMETER;
                else *(PBASE_API_MSG)message=copy;
            }
        }
        HeapFree(GetProcessHeap(),0,payload);
    }
    if (enqueueGate && number == CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand) &&
        ((PBASE_API_MSG)message)->u.GetNextVDMCommand.WaitObjectForVDM) SetEvent(enqueueGate);
    return result;
}

static DWORD WINAPI enqueue_after_wait(LPVOID unused)
{
    BASE_API_MSG message = {0};
    CSR_REPLY_STATUS reply = 0;
    STARTUPINFOA startup = {sizeof(startup)};
    char command[] = "NEXT.COM\r\n";
    (void)unused;
    CSR_THREAD localThread = thread;
    if (OpenNtBaseServerRequestThread()!=NULL) return 3;
    if (WaitForSingleObject(enqueueGate,5000) != WAIT_OBJECT_0) return 2;
    localThread.ThreadHandle=GetCurrentThread();
    localThread.ClientId.UniqueThread=(HANDLE)GetCurrentThreadId();
    if (OpenNtBaseBindServerRequestThread(&localThread)!=NULL) return 4;
    if (OpenNtBaseBindProcessRegistry(&processRegistry)!=NULL) return 6;
    message.u.CheckVDM.ConsoleHandle = (HANDLE)1;
    message.u.CheckVDM.BinaryType = BINARY_TYPE_DOS;
    message.u.CheckVDM.CmdLine = command;
    message.u.CheckVDM.CmdLen = sizeof(command);
    message.u.CheckVDM.StartupInfo = &startup;
    enqueueStatus = BaseSrvCheckVDM((PCSR_API_MSG)&message,&reply);
    queuedParent = message.u.CheckVDM.WaitObjectForParent;
    if (OpenNtBaseBindProcessRegistry(NULL)!=&processRegistry) return 7;
    if (OpenNtBaseServerRequestThread()!=&localThread ||
        OpenNtBaseBindServerRequestThread(NULL)!=&localThread ||
        OpenNtBaseServerRequestThread()!=NULL) return 5;
    return 0;
}

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(0)

int main(int argc, char **argv)
{
    BASE_API_MSG m;
    CSR_REPLY_STATUS reply = 0;
    PCONSOLERECORD record = NULL;
    char command[] = "MEM.EXE\r\n", dirs[] = "C:\\TEST\0", output[32];
    STARTUPINFOA startup = {sizeof(startup)};
    ULONG status;
    HANDLE parentWait, workerWait;
    VDMINFO clientInfo;
    HWND registrationWindow;
    HANDLE sender;
    DWORD senderExit;
    if (argc==2 && !strcmp(argv[1],"--registry-child")) return 0;
    BaseSrvHeap = GetProcessHeap();
    {
        BASE_API_MSG input={0},target={0},saved;
        STARTUPINFOA startup={0};
        unsigned char check[234],update[48];
        uint32_t size,id,cut,index;
        broker_vdm_check_values checkValues;
        broker_vdm_update_values updateValues;
        input.u.CheckVDM.CmdLine="X.COM"; input.u.CheckVDM.CmdLen=6;
        input.u.CheckVDM.iTask=0xccccccccu; input.u.CheckVDM.VDMState=0xcccc;
        CHECK(OpenNtBaseEncodeCheckCommand(&input,1,1,check,sizeof(check),&size));
        memcpy(&checkValues,check+32,sizeof(checkValues));
        CHECK(!checkValues.task && !checkValues.state);
        CHECK(OpenNtBaseDecodeCheckCommand(check,size,1,&target,&startup,&id));
        CHECK(!target.u.CheckVDM.iTask && !target.u.CheckVDM.VDMState);
        for(index=UPDATE_VDM_UNDO_CREATION;index<=UPDATE_VDM_HOOKED_CTRLC;++index) {
            memset(&input,0,sizeof(input)); memset(&target,0,sizeof(target));
            input.u.UpdateVDMEntry.EntryIndex=(WORD)index;
            input.u.UpdateVDMEntry.iTask=42;
            input.u.UpdateVDMEntry.BinaryType=BINARY_TYPE_DOS;
            input.u.UpdateVDMEntry.VDMCreationState=VDM_PARTIALLY_CREATED;
            target.u.UpdateVDMEntry.VDMProcessHandle=(HANDLE)0x1234;
            target.u.UpdateVDMEntry.WaitObjectForParent=(HANDLE)0x5678;
            saved=target;
            CHECK(OpenNtBaseEncodeUpdateCommand(&input,8,9,update,sizeof(update),&size));
            memcpy(&updateValues,update+32,sizeof(updateValues));
            CHECK(updateValues.task==42 && updateValues.entry==index &&
                updateValues.creation_state==(index==UPDATE_VDM_UNDO_CREATION?VDM_PARTIALLY_CREATED:0));
            for(cut=0;cut<size;++cut) {
                id=99;
                CHECK(!OpenNtBaseDecodeUpdateCommand(update,cut,9,&target,&id));
                CHECK(id==99 && !memcmp(&target,&saved,sizeof(saved)));
            }
            CHECK(OpenNtBaseDecodeUpdateCommand(update,size,9,&target,&id) && id==8);
            CHECK(target.u.UpdateVDMEntry.VDMProcessHandle==(HANDLE)0x1234 &&
                target.u.UpdateVDMEntry.WaitObjectForParent==(HANDLE)0x5678);
        }
        puts("PASS: Check output-only fields omitted; Update original scalar inputs composed without resource values");
    }
    {
        BASE_API_MSG input={0},decodedInput={0},untouched={0};
        OPENNT_BASE_GET_COMMAND state={0};
        unsigned char request[228],shortBuffer[228];
        broker_vdm_get_values values;
        uint32_t bytes,cut;
        /* Only presence is input: encoding must not dereference this output
         * capture or read output-only scalar fields from the original call. */
        input.u.GetNextVDMCommand.StartupInfo=(LPSTARTUPINFOA)1;
        input.u.GetNextVDMCommand.iTask=0xccccccccu;
        input.u.GetNextVDMCommand.CodePage=0xccccccccu;
        input.u.GetNextVDMCommand.CurrentDrive=0xccccu;
        input.u.GetNextVDMCommand.dwCreationFlags=0xccccccccu;
        input.u.GetNextVDMCommand.fComingFromBat=0xcc;
        input.u.GetNextVDMCommand.VDMState=ASKING_FOR_FIRST_COMMAND;
        input.u.GetNextVDMCommand.ExitCode=37;
        memset(shortBuffer,0xa5,sizeof(shortBuffer));
        CHECK(!OpenNtBaseEncodeGetCommand(&input,4,7,shortBuffer,227,&bytes) && bytes==228);
        for (cut=0;cut<sizeof(shortBuffer);++cut) CHECK(shortBuffer[cut]==0xa5);
        CHECK(OpenNtBaseEncodeGetCommand(&input,4,7,request,sizeof(request),&bytes));
        memcpy(&values,request+sizeof(broker_vdm_message_header),sizeof(values));
        CHECK(values.task==0 && values.code_page==0 && values.drive==0 && values.creation_flags==0 && values.from_bat==0);
        CHECK(values.state==ASKING_FOR_FIRST_COMMAND && values.exit_code==37);
        for (cut=0;cut<bytes;++cut) {
            CHECK(OpenNtBasePrepareGetCommand(request,cut,7,&decodedInput,&state)==ERROR_INVALID_PARAMETER);
            CHECK(!memcmp(&decodedInput,&untouched,sizeof(untouched)) && !state.reply && !state.payload.bytes);
        }
        CHECK(OpenNtBasePrepareGetCommand(request,bytes,8,&decodedInput,&state)==ERROR_INVALID_PARAMETER);
        CHECK(!OpenNtBasePrepareGetCommand(request,bytes,7,&decodedInput,&state));
        CHECK(decodedInput.u.GetNextVDMCommand.StartupInfo==&state.startup);
        CHECK(state.startup.dwFlags==0 && state.startup.lpReserved==NULL);
        memset(&decodedInput,0,sizeof(decodedInput));
        OpenNtBaseReleaseGetCommand(&state); OpenNtBaseReleaseGetCommand(&state);
        CHECK(!state.reply && !state.payload.bytes);
        puts("PASS: GetNext envelope excludes uninitialized output fields, validates truncation/generation and preallocates reply");
    }
    {
        OPENNT_BASE_VDM_CONFIG config;
        UNICODE_STRING commandLine={0};
        ULONG reserve=0;
        WCHAR shortKernel[MAX_PATH];
        CHAR tooLong[MAX_VDM_CFG_LINE+1];
        memset(tooLong,'x',sizeof(tooLong)-1); tooLong[sizeof(tooLong)-1]=0;
        CHECK(!OpenNtBaseInitializeVdmConfig(&config,tooLong,"kernel"));
        CHECK(!OpenNtBaseInitializeVdmConfig(&config,"bad\"image","kernel"));
        CHECK(!BaseGetVdmConfigInfo(NULL,0,BINARY_TYPE_DOS,&commandLine,&reserve));
        CHECK(commandLine.Buffer==NULL);
        CHECK(OpenNtBaseInitializeVdmConfig(&config,"O:\\package with spaces\\ntvdm.exe",
            "O:\\ntvdm64\\system32\\KRNL386"));
        CHECK(OpenNtBaseBindVdmConfig(&config)==NULL);
        {
            CHAR small[2]={'x',0};
            DWORD capacity=sizeof(small), size=0xfeed;
            CHECK(!BaseGetVDMKeyword(CMDLINE,small,&capacity,DOSSIZE,&size));
            CHECK(small[0]=='x' && capacity==sizeof(small) && size==0xfeed);
            CHECK(!BaseGetVDMKeyword(CMDLINE,small,&capacity,WOWSIZE,&size));
        }
        CHECK(BaseGetVdmConfigInfo(NULL,0,BINARY_TYPE_DOS,&commandLine,&reserve));
        CHECK(!wcscmp(commandLine.Buffer,L"\"O:\\package with spaces\\ntvdm.exe\" -f"));
        CHECK(reserve==16*1024*1024);
        CHECK(HeapFree(GetProcessHeap(),0,commandLine.Buffer));
        CHECK(BaseGetVdmConfigInfo(NULL,0x12ab,BINARY_TYPE_DOS,&commandLine,&reserve));
        CHECK(!wcscmp(commandLine.Buffer,L"\"O:\\package with spaces\\ntvdm.exe\" -f -i12ab"));
        CHECK(HeapFree(GetProcessHeap(),0,commandLine.Buffer));
        CHECK(GetShortPathNameW(L"O:\\ntvdm64\\system32\\KRNL386.exe",shortKernel,MAX_PATH)>0);
        CHECK(BaseGetVdmConfigInfo(NULL,0,BINARY_TYPE_WIN16,&commandLine,&reserve));
        CHECK(wcsstr(commandLine.Buffer,L"\" -f -w -a ")!=NULL);
        CHECK(!wcscmp(wcsstr(commandLine.Buffer,L" -a ")+4,shortKernel));
        CHECK(HeapFree(GetProcessHeap(),0,commandLine.Buffer));
        CHECK(OpenNtBaseBindVdmConfig(NULL)==&config);
        puts("PASS: original DOS/WOW worker command construction, package path spaces, session ID and config rejection");
    }
    CsrPortHeap=HeapCreate(0,0,0);
    CHECK(CsrPortHeap!=NULL && captures==0);
    {
        PCSR_CAPTURE_HEADER capture=CsrAllocateCaptureBuffer(3,0,5);
        PVOID first=NULL,empty=(PVOID)1,last=NULL;
        CHECK(capture!=NULL && captures==1);
        CHECK(CsrAllocateMessagePointer(capture,1,&first)==4);
        CHECK(CsrAllocateMessagePointer(capture,0,&empty)==0 && empty==NULL);
        CHECK(CsrAllocateMessagePointer(capture,3,&last)==4 && (PCHAR)last==(PCHAR)first+4);
        CHECK(capture->CountMessagePointers==3 && capture->CountCapturePointers==0);
        CHECK(capture->MessagePointerOffsets[0]==(ULONG)&first && capture->MessagePointerOffsets[1]==0);
        CHECK(capture->MessagePointerOffsets[2]==(ULONG)&last);
        CHECK(capture->FreeSpace<=(PCHAR)capture+capture->Length);
        CsrFreeCaptureBuffer(capture);
        CHECK(captures==0 && CsrAllocateCaptureBuffer(1,0,MAXLONG)==NULL && captures==0);
    }
    CHECK(NtCurrentPeb()->ImageBaseAddress==GetModuleHandleW(NULL));
    CHECK(RtlProcessHeap()==GetProcessHeap());
    CHECK(opennt_support_current_teb()->ClientId.UniqueProcess==(HANDLE)GetCurrentProcessId());
    /* Local fixture Console association only. A stdout HANDLE is not the
     * authenticated cross-process Console ID required by product dispatch. */
    NtCurrentPeb()->ProcessParameters->ConsoleHandle = (HANDLE)1;
    CHECK(OpenNtBaseInitializeProcessRegistry(&processRegistry));
    CHECK(OpenNtBaseBindProcessRegistry(&processRegistry)==NULL);
    CHECK(OpenNtBaseRegisterProcess(&processRegistry,&caller,GetCurrentProcess()));
    CHECK(caller.SequenceNumber==1);
    {
        HANDLE retained=NULL;
        CHECK(!OpenNtBaseRetainRegisteredProcess(&processRegistry,GetCurrentProcessId(),2,&retained));
        CHECK(GetLastError()==ERROR_ACCESS_DENIED && retained==NULL);
        CHECK(OpenNtBaseRetainRegisteredProcess(&processRegistry,GetCurrentProcessId(),1,&retained));
        CHECK(GetProcessId(retained)==GetCurrentProcessId());
        CHECK(processRegistry.Pins==0 && CloseHandle(retained));
    }
    {
        CSR_PROCESS duplicate={0};
        PCSR_PROCESS found=(PCSR_PROCESS)1;
        CHECK(!OpenNtBaseRegisterProcess(&processRegistry,&duplicate,GetCurrentProcess()));
        CHECK(GetLastError()==ERROR_ALREADY_EXISTS);
        CHECK(!OpenNtBaseDestroyProcessRegistry(&processRegistry));
        CHECK(GetLastError()==ERROR_BUSY);
        CHECK(CsrLockProcessByClientId(NULL,&found)<0 && found==NULL);
        CHECK(CsrLockProcessByClientId((HANDLE)GetCurrentProcessId(),&found)==0 && found==&caller);
        CHECK(!OpenNtBaseRemoveProcess(&processRegistry,&caller) && GetLastError()==ERROR_BUSY);
        CHECK(CsrUnlockProcess(found)==0);
    }
    thread.Process = &caller;
    thread.ThreadHandle = GetCurrentThread();
    thread.ClientId.UniqueProcess = (HANDLE)GetCurrentProcessId();
    thread.ClientId.UniqueThread = (HANDLE)GetCurrentThreadId();
    CHECK(OpenNtBaseServerRequestThread()==NULL);
    CHECK(OpenNtBaseBindServerRequestThread(&thread)==NULL);
    {
        CSR_THREAD nested=thread;
        PCSR_THREAD previous=OpenNtBaseBindServerRequestThread(&nested);
        CHECK(previous==&thread && OpenNtBaseServerRequestThread()==&nested);
        CHECK(OpenNtBaseBindServerRequestThread(previous)==&nested);
        CHECK(OpenNtBaseServerRequestThread()==&thread);
    }
    BaseSrvVDMInit();
    {
        broker_vdm_payload_input fields[BROKER_VDM_PAYLOAD_FIELDS]={0};
        BASE_CHECKVDM_MSG message={0},saved;
        char commandText[2]={'X',0},emptyEnv[1]={0},badEnv[2]={'A',0};
        unsigned char wire[256]; uint32_t bytes;
        fields[0].present=1; fields[0].length=2; fields[0].data_bytes=2; fields[0].data=commandText;
        fields[4].present=1; fields[4].length=1; fields[4].data_bytes=1; fields[4].data=emptyEnv;
        CHECK(broker_vdm_payload_encode(fields,wire,sizeof(wire),&bytes));
        CHECK(OpenNtBaseDecodeCheckPayload(wire,bytes,&message));
        CHECK(message.EnvLen==1 && message.Env[0]==0 && !strcmp(message.CmdLine,"X"));
        memset(&message,0xa5,sizeof(message)); saved=message;
        commandText[1]='Y';
        CHECK(broker_vdm_payload_encode(fields,wire,sizeof(wire),&bytes));
        CHECK(!OpenNtBaseDecodeCheckPayload(wire,bytes,&message) && !memcmp(&message,&saved,sizeof(message)));
        commandText[1]=0; fields[4].length=2; fields[4].data_bytes=2; fields[4].data=badEnv;
        CHECK(broker_vdm_payload_encode(fields,wire,sizeof(wire),&bytes));
        CHECK(!OpenNtBaseDecodeCheckPayload(wire,bytes,&message) && !memcmp(&message,&saved,sizeof(message)));
        fields[4].length=1; fields[4].data_bytes=1; fields[4].data=emptyEnv;
        fields[0].length=0; fields[0].data_bytes=0;
        CHECK(broker_vdm_payload_encode(fields,wire,sizeof(wire),&bytes));
        CHECK(!OpenNtBaseDecodeCheckPayload(wire,bytes,&message));
        fields[0].present=0;
        CHECK(broker_vdm_payload_encode(fields,wire,sizeof(wire),&bytes));
        CHECK(!OpenNtBaseDecodeCheckPayload(wire,bytes,&message));
        puts("PASS: CheckVDM rejects unterminated text/environment and empty present buffers; original single-NUL empty environment accepted");
    }
    {
        broker_vdm_payload_input fields[BROKER_VDM_PAYLOAD_FIELDS]={0};
        unsigned char wire[128];
        uint32_t bytes;
        BASE_GET_NEXT_VDM_COMMAND_MSG request={0},saved;
        OPENNT_BASE_GET_PAYLOAD storage={0};
        fields[BROKER_VDM_ENVIRONMENT].present=1;
        fields[BROKER_VDM_ENVIRONMENT].length=UINT32_MAX;
        CHECK(broker_vdm_payload_encode(fields,wire,sizeof(wire),&bytes));
        saved=request;
        CHECK(OpenNtBasePrepareGetPayload(wire,bytes,&request,&storage)==ERROR_ARITHMETIC_OVERFLOW);
        CHECK(!storage.bytes && !memcmp(&request,&saved,sizeof(saved)));
        memset(fields,0,sizeof(fields)); fields[0].present=1; fields[0].length=4;
        CHECK(broker_vdm_payload_encode(fields,wire,sizeof(wire),&bytes));
        CHECK(OpenNtBasePrepareGetPayload(wire,bytes,&request,&storage)==0);
        CHECK(request.CmdLine && request.CmdLen==4 && !memcmp(request.CmdLine,"\0\0\0\0",4));
        CHECK(OpenNtBasePrepareGetPayload(wire,bytes,&request,&storage)==ERROR_INVALID_PARAMETER);
        request.CmdLine[0]='A'; request.CmdLen=100;
        CHECK(OpenNtBaseFinishGetPayload(&request,&storage));
        CHECK(broker_vdm_payload_validate(storage.bytes,storage.size));
        request.CmdLine=NULL;
        OpenNtBaseReleaseGetPayload(&storage); OpenNtBaseReleaseGetPayload(&storage);
        CHECK(!storage.bytes && !storage.size);
        puts("PASS: GetNext capacity overflow refused before mutation, storage zeroed, reply preallocated and release repeat-safe");
    }
    {
        VDMINFO info={0};
        BASE_GET_NEXT_VDM_COMMAND_MSG query={0};
        char pif[2]={0x7b,0x7b},title[2]={0x7b,0x7b},directory[2]={0x7b,0x7b};
        broker_vdm_payload_input fields[BROKER_VDM_PAYLOAD_FIELDS]={0};
        unsigned char wire[131];
        broker_vdm_payload_span span;
        uint32_t bytes;
        query.PifFile=pif; query.PifLen=sizeof(pif);
        query.Title=title; query.TitleLen=sizeof(title);
        query.CurDirectory=directory; query.CurDirectoryLen=sizeof(directory);
        CHECK(BaseSrvFillPifInfo(&info,&query)==0);
        CHECK(!query.PifLen && !query.TitleLen && !query.CurDirectoryLen);
        CHECK(!pif[0] && !title[0] && !directory[0]);
        CHECK(pif[1]==0x7b && title[1]==0x7b && directory[1]==0x7b);
        fields[BROKER_VDM_PIF].present=1; fields[BROKER_VDM_PIF].data_bytes=1; fields[BROKER_VDM_PIF].data=pif;
        fields[BROKER_VDM_TITLE].present=1; fields[BROKER_VDM_TITLE].data_bytes=1; fields[BROKER_VDM_TITLE].data=title;
        fields[BROKER_VDM_DIRECTORY].present=1; fields[BROKER_VDM_DIRECTORY].data_bytes=1; fields[BROKER_VDM_DIRECTORY].data=directory;
        CHECK(broker_vdm_payload_encode(fields,wire,sizeof(wire),&bytes));
        CHECK(bytes==sizeof(wire) && broker_vdm_payload_validate(wire,bytes));
        memcpy(&span,wire+16*BROKER_VDM_TITLE,sizeof(span));
        CHECK(span.length==0 && span.data_bytes==1 && wire[span.offset]==0);
        {
            BASE_GET_NEXT_VDM_COMMAND_MSG destination={0},saved;
            char returnedPif[2]={0x7b,0x7b},returnedTitle[2]={0x7b,0x7b},returnedDirectory[2]={0x7b,0x7b};
            destination.PifFile=returnedPif; destination.PifLen=2;
            destination.Title=returnedTitle; destination.TitleLen=0;
            destination.CurDirectory=returnedDirectory; destination.CurDirectoryLen=2;
            saved=destination;
            CHECK(!OpenNtBaseApplyGetPayload(wire,bytes,&destination));
            CHECK(!memcmp(&saved,&destination,sizeof(saved)));
            CHECK(returnedPif[0]==0x7b && returnedDirectory[0]==0x7b && returnedTitle[0]==0x7b);
            destination.TitleLen=2;
            CHECK(OpenNtBaseApplyGetPayload(wire,bytes,&destination));
            CHECK(!destination.PifLen && !destination.TitleLen && !destination.CurDirectoryLen);
            CHECK(!returnedPif[0] && !returnedDirectory[0] && !returnedTitle[0]);
            CHECK(returnedPif[1]==0x7b && returnedDirectory[1]==0x7b && returnedTitle[1]==0x7b);
        }
        puts("PASS: original PIF writes terminators despite zero returned lengths; copied payload preserves them");
    }
    {
        broker_vdm_payload_input fields[BROKER_VDM_PAYLOAD_FIELDS]={0};
        BASE_CHECKVDM_MSG decoded,saved;
        uint32_t bytes;
        void *large=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,65536);
        void *wire;
        CHECK(large!=NULL);
        fields[0].present=1; fields[0].length=65536; fields[0].data_bytes=65536; fields[0].data=large;
        CHECK(broker_vdm_payload_encode(fields,NULL,0,&bytes));
        wire=HeapAlloc(GetProcessHeap(),0,bytes); CHECK(wire!=NULL);
        CHECK(broker_vdm_payload_encode(fields,wire,bytes,&bytes));
        memset(&decoded,0xa5,sizeof(decoded)); saved=decoded;
        CHECK(!OpenNtBaseDecodeCheckPayload(wire,bytes,&decoded));
        CHECK(!memcmp(&decoded,&saved,sizeof(saved)));
        fields[0].length=10; fields[0].data_bytes=0;
        CHECK(broker_vdm_payload_encode(fields,wire,bytes,&bytes));
        CHECK(!OpenNtBaseDecodeCheckPayload(wire,bytes,&decoded));
        CHECK(!memcmp(&decoded,&saved,sizeof(saved)));
        CHECK(HeapFree(GetProcessHeap(),0,wire) && HeapFree(GetProcessHeap(),0,large));
        puts("PASS: native CheckVDM payload rejects USHORT overflow and capacity-only input without mutation");
    }
    {
        STARTUPINFOA original,decoded,saved;
        broker_vdm_startup wire;
        memset(&original,0xa5,sizeof(original));
        original.dwX=1; original.dwY=2; original.dwXSize=3; original.dwYSize=4;
        original.dwXCountChars=5; original.dwYCountChars=6;
        original.dwFillAttribute=7; original.dwFlags=8; original.wShowWindow=0xffff;
        OpenNtBaseEncodeStartup(&original,&wire);
        CHECK(OpenNtBaseDecodeStartup(&wire,&decoded));
        CHECK(decoded.cb==sizeof(decoded) && decoded.dwX==1 && decoded.dwY==2);
        CHECK(decoded.dwXSize==3 && decoded.dwYSize==4 && decoded.dwXCountChars==5 && decoded.dwYCountChars==6);
        CHECK(decoded.dwFillAttribute==7 && decoded.dwFlags==8 && decoded.wShowWindow==0xffff);
        CHECK(!decoded.lpReserved && !decoded.lpDesktop && !decoded.lpTitle && !decoded.lpReserved2 && !decoded.cbReserved2);
        CHECK(!decoded.hStdInput && !decoded.hStdOutput && !decoded.hStdError);
        saved=decoded; wire.show=0x10000;
        CHECK(!OpenNtBaseDecodeStartup(&wire,&decoded) && !memcmp(&saved,&decoded,sizeof(saved)));
        OpenNtBaseEncodeStartup(NULL,&wire);
        CHECK(!wire.present && OpenNtBaseDecodeStartup(&wire,&decoded) && decoded.cb==0);
        wire.flags=1;
        CHECK(!OpenNtBaseDecodeStartup(&wire,&decoded));
        wire.flags=0; wire.present=2;
        CHECK(!OpenNtBaseDecodeStartup(&wire,&decoded));
        puts("PASS: nine source startup scalars roundtrip without native pointers/handles; absent and invalid forms checked");
    }
    {
        resource_failure_test test={0};
        OPENNT_BASE_RESOURCE_BINDING binding={&test,reject_delivery,close_failed_delivery};
        HANDLE server=NULL,client=NULL;
        DWORD flags;
        CHECK(OpenNtBaseBindResources(&binding)==NULL);
        CHECK(BaseSrvCreatePairWaitHandles(&server,&client)==(ULONG)STATUS_ACCESS_DENIED);
        CHECK(OpenNtBaseBindResources(NULL)==&binding);
        CHECK(test.duplicated==1 && test.closed==1 && client==NULL);
        CHECK(!GetHandleInformation(test.event,&flags) && GetLastError()==ERROR_INVALID_HANDLE);
        puts("PASS: original pair-wait delivery failure closes the created server event synchronously");
    }
    {
        resource_failure_test test={0};
        OPENNT_BASE_RESOURCE_BINDING binding={&test,partial_stream_delivery,close_failed_delivery};
        VDMINFO info={0};
        DOSRECORD record={0};
        info.StdIn=(HANDLE)10; info.StdOut=(HANDLE)20; info.StdErr=(HANDLE)30;
        record.lpVDMInfo=&info;
        CHECK(OpenNtBaseBindResources(&binding)==NULL);
        CHECK(BaseSrvDupStandardHandles(GetCurrentProcess(),&record)==(ULONG)STATUS_ACCESS_DENIED);
        CHECK(OpenNtBaseBindResources(NULL)==&binding);
        CHECK(test.duplicated==2 && test.closed==0);
        CHECK(info.StdIn==(HANDLE)101 && info.StdOut==(HANDLE)20 && info.StdErr==(HANDLE)30);
        puts("PASS: original partial stream failure retains mixed source/destination fields; no helper rollback");
    }
    {
        ULONG alias;
        for (alias=0;alias<2;++alias) {
            stream_identity_test test={0};
            OPENNT_BASE_RESOURCE_BINDING binding={&test,stream_identity_delivery,NULL};
            VDMINFO info={0};
            DOSRECORD record={0};
            info.StdIn=(HANDLE)10; info.StdOut=(HANDLE)20; info.StdErr=(HANDLE)(alias?20:30);
            record.lpVDMInfo=&info;
            CHECK(OpenNtBaseBindResources(&binding)==NULL);
            CHECK(BaseSrvDupStandardHandles(GetCurrentProcess(),&record)==STATUS_SUCCESS);
            CHECK(test.copied==(alias?2:3));
            CHECK(info.StdIn==(HANDLE)101 && info.StdOut==(HANDLE)102);
            CHECK(info.StdErr==(HANDLE)(alias?102:103));
            BaseSrvCloseStandardHandles(GetCurrentProcess(),&record);
            CHECK(test.revoked==3 && test.closed[0]==(HANDLE)101 && test.closed[1]==(HANDLE)102);
            CHECK(test.closed[2]==(HANDLE)(alias?102:103));
            CHECK(!info.StdIn && !info.StdOut && !info.StdErr);
            BaseSrvCloseStandardHandles(GetCurrentProcess(),&record);
            CHECK(test.revoked==3);
            CHECK(OpenNtBaseBindResources(NULL)==&binding);
        }
        puts("PASS: original distinct/aliased stream delivery, duplicate alias close and cleared repeat cleanup");
    }
    {
        stream_rollback_test test={0};
        OPENNT_BASE_RESOURCE_BINDING binding={&test,deliver_stream_then_fail,NULL};
        VDMINFO info={0};
        DOSRECORD record={0};
        WCHAR name[80];
        HANDLE source,received;
        DWORD written,handleFlags;
        CHECK(_snwprintf(name,80,L"stream-rollback-%lu.tmp",GetCurrentProcessId())>0);
        source=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_TEMPORARY,NULL);
        CHECK(source!=INVALID_HANDLE_VALUE);
        CHECK(WriteFile(source,"A",1,&written,NULL) && written==1);
        CHECK(!broker_vdm_receipts_initialize(&test.receipts,7));
        info.StdIn=source; info.StdOut=source; info.StdErr=source; record.lpVDMInfo=&info;
        CHECK(OpenNtBaseBindResources(&binding)==NULL);
        CHECK(BaseSrvDupStandardHandles(GetCurrentProcess(),&record)==(ULONG)STATUS_ACCESS_DENIED);
        CHECK(OpenNtBaseBindResources(NULL)==&binding);
        CHECK(test.attempts==2 && info.StdOut==source && info.StdErr==source);
        CHECK(!broker_vdm_receipt_resolve(&test.receipts,7,(uint32_t)info.StdIn,BROKER_VDM_STDIN,&received));
        CHECK(GetFileSize(received,NULL)==1);
        CHECK(GetHandleInformation(received,&handleFlags) && (handleFlags&HANDLE_FLAG_INHERIT));
        CHECK(!broker_vdm_delivery_rollback(&test.delivery));
        CHECK(!test.delivery.pending && !test.receipts.entries);
        CHECK(broker_vdm_receipt_resolve(&test.receipts,7,(uint32_t)info.StdIn,BROKER_VDM_STDIN,&received)==ERROR_NOT_FOUND);
        CHECK(WriteFile(source,"B",1,&written,NULL) && written==1 && GetFileSize(source,NULL)==2);
        broker_vdm_receipts_drain(&test.receipts);
        CHECK(CloseHandle(source) && DeleteFileW(name));
        puts("PASS: original partial stream failure rolls back actual receipt without closing sender file");
    }
    { LUID negative=RtlConvertLongToLuid(-1), positive=RtlConvertLongToLuid(0x7fffffff);
      CHECK(negative.LowPart==0xffffffff && negative.HighPart==-1);
      CHECK(positive.LowPart==0x7fffffff && positive.HighPart==0); }

    ZeroMemory(&m,sizeof(m));
    {
        CSR_API_NUMBER api=CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepIsFirstVDM);
        CHECK(OpenNtBaseBindServerRequestThread(NULL)==&thread);
        CHECK(OpenNtBaseDispatch((PCSR_API_MSG)&m,api,sizeof(m.u.IsFirstVDM))==STATUS_ACCESS_DENIED);
        CHECK(OpenNtBaseBindServerRequestThread(&thread)==NULL);
        CHECK(OpenNtBaseDispatch((PCSR_API_MSG)&m,api,sizeof(m.u.IsFirstVDM)-1)==STATUS_INVALID_PARAMETER);
        CHECK(OpenNtBaseDispatch((PCSR_API_MSG)&m,api,sizeof(m.u.IsFirstVDM)+1)==STATUS_INVALID_PARAMETER);
        CHECK(OpenNtBaseDispatch((PCSR_API_MSG)&m,CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCreateProcess),0)==STATUS_INVALID_PARAMETER);
        m.h.ClientId.UniqueProcess=(HANDLE)0x1234;
        CHECK(OpenNtBaseDispatch((PCSR_API_MSG)&m,api,sizeof(m.u.IsFirstVDM))==0 && m.u.IsFirstVDM.FirstVDM);
        CHECK(m.h.ClientId.UniqueProcess==thread.ClientId.UniqueProcess);
        CHECK(m.h.ClientId.UniqueThread==thread.ClientId.UniqueThread);
        puts("PASS: production dispatch rejects unbound/short/long/unknown requests without consuming first-VDM state; trusted identity replaces input");
    }
    CHECK(BaseSrvIsFirstVDM((PCSR_API_MSG)&m,&reply) == 0 && !m.u.IsFirstVDM.FirstVDM);
    CHECK(GetNextVDMCommand(NULL) == FALSE);

    ZeroMemory(&m,sizeof(m));
    m.u.CheckVDM.ConsoleHandle = (HANDLE)1; /* Local fixture record key, not IPC/native handle. */
    m.u.CheckVDM.BinaryType = BINARY_TYPE_DOS;
    m.u.CheckVDM.CmdLine = command;
    m.u.CheckVDM.CmdLen = sizeof(command);
    m.u.CheckVDM.StartupInfo = &startup;
    CHECK(BaseSrvCheckVDM((PCSR_API_MSG)&m,&reply) == 0);
    CHECK(m.u.CheckVDM.VDMState == VDM_NOT_PRESENT);
    CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record) == 0 && record != NULL);
    CHECK(record->DOSRecord->VDMState == VDM_TO_TAKE_A_COMMAND);
    CHECK(!memcmp(record->DOSRecord->lpVDMInfo->CmdLine,command,sizeof(command)));

    ZeroMemory(&m,sizeof(m));
    m.u.GetSetVDMCurDirs.ConsoleHandle = (HANDLE)1;
    m.u.GetSetVDMCurDirs.lpszzCurDirs = dirs;
    m.u.GetSetVDMCurDirs.cchCurDirs = sizeof(dirs);
    CHECK(BaseSrvSetVDMCurDirs((PCSR_API_MSG)&m,&reply) == 0);
    m.u.GetSetVDMCurDirs.lpszzCurDirs = output;
    m.u.GetSetVDMCurDirs.cchCurDirs = 1;
    status = BaseSrvGetVDMCurDirs((PCSR_API_MSG)&m,&reply);
    CHECK(status == (ULONG)STATUS_INVALID_PARAMETER);
    CHECK(m.u.GetSetVDMCurDirs.cchCurDirs == sizeof(dirs));
    CHECK(record->lpszzCurDirs != NULL);
    CHECK(BaseSrvGetVDMCurDirs((PCSR_API_MSG)&m,&reply) == 0);
    CHECK(!memcmp(output,dirs,sizeof(dirs)) && record->lpszzCurDirs == NULL);
    CHECK(BaseSrvGetVDMCurDirs((PCSR_API_MSG)&m,&reply) == 0);
    CHECK(m.u.GetSetVDMCurDirs.cchCurDirs == 0);
    CHECK(SetVDMCurrentDirectories(sizeof(dirs),dirs) && captures == 0);
    CHECK(GetVDMCurrentDirectories(0,NULL) == sizeof(dirs) && captures == 0);
    CHECK(record->lpszzCurDirs != NULL);
    CHECK(GetVDMCurrentDirectories(sizeof(output),output) == sizeof(dirs));
    CHECK(!memcmp(output,dirs,sizeof(dirs)) && captures == 0);
    CHECK(GetVDMCurrentDirectories(sizeof(output),output) == 0 && captures == 0);
    CmdBatNotification(CMD_BAT_OPERATION_STARTING);
    CHECK(BaseSrvGetBatRecord((HANDLE)1) != NULL);
    CmdBatNotification(CMD_BAT_OPERATION_TERMINATING);
    CHECK(BaseSrvGetBatRecord((HANDLE)1) == NULL);

    /* Registration-only test: an owned message-only window, not guest UI. */
    registrationWindow = CreateWindowExA(0,"STATIC","broker-registration-fixture",
        0,0,0,1,1,HWND_MESSAGE,NULL,GetModuleHandleW(NULL),NULL);
    CHECK(registrationWindow != NULL);
    RegisterWowExec(registrationWindow);
    CHECK(hwndWowExec == registrationWindow && ulWowExecProcessSequenceNumber == 1);
    CHECK(DestroyWindow(registrationWindow));
    RegisterWowExec(NULL);
    CHECK(hwndWowExec == NULL);

    ZeroMemory(&m,sizeof(m));
    m.u.UpdateVDMEntry.ConsoleHandle = (HANDLE)1;
    m.u.UpdateVDMEntry.BinaryType = BINARY_TYPE_DOS;
    m.u.UpdateVDMEntry.EntryIndex = UPDATE_VDM_PROCESS_HANDLE;
    m.u.UpdateVDMEntry.VDMProcessHandle = GetCurrentProcess();
    parentWait = GetCurrentProcess();
    CHECK(BaseUpdateVDMEntry(UPDATE_VDM_PROCESS_HANDLE,&parentWait,0,BINARY_TYPE_DOS));
    CHECK(parentWait && WaitForSingleObject(parentWait,0) == WAIT_TIMEOUT);
    {
        DWORD taskExit=0xfeed;
        CHECK(!BaseCheckForVDM(GetCurrentProcess(),&taskExit) && taskExit==0xfeed);
        CHECK(!BaseCheckForVDM(NULL,&taskExit) && taskExit==0xfeed);
        CHECK(BaseCheckForVDM(parentWait,&taskExit) && taskExit==STILL_ACTIVE);
        CHECK(record->DOSRecord->VDMState==VDM_TO_TAKE_A_COMMAND);
    }

    ZeroMemory(&m,sizeof(m));
    m.u.GetNextVDMCommand.ConsoleHandle = (HANDLE)1;
    m.u.GetNextVDMCommand.VDMState = ASKING_FOR_FIRST_COMMAND;
    m.u.GetNextVDMCommand.CmdLine = output;
    m.u.GetNextVDMCommand.CmdLen = 1;
    m.u.GetNextVDMCommand.StartupInfo = &startup;
    CHECK(BaseSrvGetNextVDMCommand((PCSR_API_MSG)&m,&reply) == (ULONG)STATUS_INVALID_PARAMETER);
    CHECK(m.u.GetNextVDMCommand.CmdLen == sizeof(command));
    CHECK(record->DOSRecord->VDMState == VDM_TO_TAKE_A_COMMAND);
    ZeroMemory(&clientInfo,sizeof(clientInfo));
    clientInfo.VDMState = ASKING_FOR_FIRST_COMMAND;
    clientInfo.CmdLine = output;
    clientInfo.CmdSize = 1;
    CHECK(!GetNextVDMCommand(&clientInfo));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(clientInfo.CmdSize == sizeof(command) && captures == 0);
    CHECK(record->DOSRecord->VDMState == VDM_TO_TAKE_A_COMMAND);
    CHECK(GetNextVDMCommand(&clientInfo) && captures == 0);
    CHECK(!memcmp(output,command,sizeof(command)));
    CHECK(record->DOSRecord->VDMState == VDM_BUSY);
    CHECK(WaitForSingleObject(parentWait,0) == WAIT_TIMEOUT);

    ZeroMemory(&m,sizeof(m));
    m.u.GetNextVDMCommand.ConsoleHandle = (HANDLE)1;
    m.u.GetNextVDMCommand.ExitCode = 7;
    CHECK(BaseSrvGetNextVDMCommand((PCSR_API_MSG)&m,&reply) == 0);
    workerWait = m.u.GetNextVDMCommand.WaitObjectForVDM;
    CHECK(workerWait && WaitForSingleObject(workerWait,0) == WAIT_TIMEOUT);
    CHECK(WaitForSingleObject(parentWait,0) == WAIT_OBJECT_0);
    CHECK(record->DOSRecord->ErrorCode == 7);
    CHECK(record->DOSRecord->VDMState == VDM_HAS_RETURNED_ERROR_CODE);
    {
        DWORD taskExit=0xfeed;
        CHECK(BaseCheckForVDM(parentWait,&taskExit) && taskExit==7);
        CHECK(record->DOSRecord->VDMState==VDM_READY && !record->DOSRecord->hWaitForParent);
        CHECK(BaseCheckForVDM(parentWait,&taskExit) && taskExit==0);
    }
    CloseHandle(parentWait);

    ZeroMemory(&m,sizeof(m));
    m.u.SetReenterCount.ConsoleHandle = (HANDLE)1;
    m.u.SetReenterCount.fIncDec = INCREMENT_REENTER_COUNT;
    CHECK(BaseSrvSetReenterCount((PCSR_API_MSG)&m,&reply) == TRUE);
    CHECK(BaseSrvSetReenterCount((PCSR_API_MSG)&m,&reply) == TRUE);
    m.u.SetReenterCount.fIncDec = DECREMENT_REENTER_COUNT;
    CHECK(BaseSrvSetReenterCount((PCSR_API_MSG)&m,&reply) == TRUE);
    CHECK(record->nReEntrancy == 1 && WaitForSingleObject(workerWait,0) == WAIT_OBJECT_0);
    CHECK(BaseSrvSetReenterCount((PCSR_API_MSG)&m,&reply) == TRUE);

    enqueueGate = CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(enqueueGate != NULL);
    sender = CreateThread(NULL,0,enqueue_after_wait,NULL,0,NULL);
    CHECK(sender != NULL);
    ZeroMemory(&clientInfo,sizeof(clientInfo));
    clientInfo.VDMState = NO_PARENT_TO_WAKE;
    clientInfo.ErrorCode = 99;
    clientInfo.CmdLine = output;
    clientInfo.CmdSize = sizeof(output);
    CHECK(GetNextVDMCommand(&clientInfo));
    CHECK(WaitForSingleObject(sender,5000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(sender,&senderExit) && senderExit == 0 && enqueueStatus == 0);
    CHECK(retryCalls == 2 && retryExit == 0 && captures == 0);
    CHECK(!strcmp(output,"NEXT.COM\r\n") && queuedParent != NULL);
    CHECK(WaitForSingleObject(queuedParent,0) == WAIT_TIMEOUT);
    CloseHandle(sender); CloseHandle(enqueueGate); enqueueGate = NULL;

    ZeroMemory(&m,sizeof(m));
    m.u.ExitVDM.ConsoleHandle = (HANDLE)1;
    ExitVDM(FALSE,0);
    CHECK(WaitForSingleObject(queuedParent,0) == WAIT_OBJECT_0);
    CloseHandle(queuedParent);
    { DWORD handleFlags;
      CHECK(!GetHandleInformation(workerWait,&handleFlags) && GetLastError() == ERROR_INVALID_HANDLE); }
    CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record) == (ULONG)STATUS_INVALID_PARAMETER);
    CHECK(BaseSrvExitDOSTask(&m.u.ExitVDM) == (ULONG)STATUS_INVALID_PARAMETER);
    CHECK(BaseSrvIsFirstVDM((PCSR_API_MSG)&m,&reply) == 0 && !m.u.IsFirstVDM.FirstVDM);
    ZeroMemory(&m,sizeof(m));
    m.u.GetNextVDMCommand.ConsoleHandle = (HANDLE)-1;
    m.u.GetNextVDMCommand.CmdLen = 1;
    m.u.GetNextVDMCommand.WaitObjectForVDM = (HANDLE)1;
    CHECK(BaseSrvGetNextVDMCommand((PCSR_API_MSG)&m,&reply) == 0);
    CHECK(!m.u.GetNextVDMCommand.WaitObjectForVDM && !m.u.GetNextVDMCommand.CmdLen);
    ZeroMemory(&clientInfo,sizeof(clientInfo));
    clientInfo.VDMState = ASKING_FOR_WOW_BINARY;
    clientInfo.CmdLine = output;
    clientInfo.CmdSize = sizeof(output);
    CHECK(GetNextVDMCommand(&clientInfo));
    CHECK(!clientInfo.CmdSize && captures == 0);
    /* Original launcher construction -> actual original server deep copy. */
    {
        STARTUPINFOW launch={sizeof(launch)};
        CHAR envBytes[]="PATH=O:\\ntvdm64\0";
        ANSI_STRING env={sizeof(envBytes),sizeof(envBytes),envBytes};
        ULONG task=0, before;
        launch.lpTitle=L"original launch test";
        launch.lpDesktop=L"default";
        launch.lpReserved=L"test reserved";
        launch.dwFlags=STARTF_USESTDHANDLES;
        ZeroMemory(&m,sizeof(m));
        CHECK(BaseCheckVDM(BINARY_TYPE_DOS|BINARY_TYPE_DOS_EXE,L"O:\\ntvdm64\\MEM.EXE",
            L"\"O:\\ntvdm64\\MEM.EXE\"  /?",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(launchCalls==1 && captures==0 && m.u.CheckVDM.VDMState==VDM_NOT_PRESENT);
        CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record)==0 && record!=NULL);
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->CmdLine,"/?\r\n"));
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->AppName,"O:\\ntvdm64\\MEM.EXE"));
        CHECK(record->DOSRecord->lpVDMInfo->EnviornmentSize==sizeof(envBytes));
        CHECK(!memcmp(record->DOSRecord->lpVDMInfo->Enviornment,envBytes,sizeof(envBytes)));
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->Title,"original launch test"));
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->Desktop,"default"));
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->Reserved,"test reserved"));
        ExitVDM(FALSE,0);
        CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record)==(ULONG)STATUS_INVALID_PARAMETER);
        launch.dwFlags=STARTF_USEHOTKEY;
        launch.hStdInput=(HANDLE)42;
        ZeroMemory(&m,sizeof(m));
        CHECK(BaseCheckVDM(BINARY_TYPE_DOS,L"O:\\ntvdm64\\MEM.EXE",L"MEM.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record)==0 && record!=NULL);
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->Reserved,"hotkey.42 test reserved"));
        CHECK(!(launch.dwFlags & STARTF_USEHOTKEY) && launch.hStdInput==NULL && captures==0);
        /* Original routine frees its replacement reserved string on return. */
        launch.lpReserved=NULL;
        ExitVDM(FALSE,0);
        before=launchCalls;
        HANDLE normalHeap=CsrPortHeap;
        HANDLE exhaustedHeap=HeapCreate(0,4096,4096);
        ULONG allocations=0;
        CHECK(exhaustedHeap!=NULL);
        while(allocations<4096 && HeapAlloc(exhaustedHeap,0,128)) ++allocations;
        CHECK(allocations<4096);
        CsrPortHeap=exhaustedHeap;
        ZeroMemory(&m,sizeof(m));
        CHECK(!BaseCheckVDM(BINARY_TYPE_DOS,L"O:\\ntvdm64\\MEM.EXE",L"MEM.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_NOT_ENOUGH_MEMORY && launchCalls==before);
        CsrPortHeap=normalHeap;
        CHECK(captures==0 && HeapDestroy(exhaustedHeap));
        CHECK(!BaseCheckVDM(BINARY_TYPE_DOS,L"O:\\ntvdm64\\MEM.EXE",L"MEM.EXE",L"O:\\ntvdm64",NULL,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_INVALID_PARAMETER && captures==0 && launchCalls==before);
        puts("PASS: original BaseCheckVDM command/environment/startup capture, server ownership and allocation failure");
    }
    {
        OPENNT_BASE_INTERACTIVE_SCOPE scope,wrong;
        STARTUPINFOW launch={sizeof(launch)};
        CHAR envBytes[]="PATH=O:\\ntvdm64\0";
        ANSI_STRING env={sizeof(envBytes),sizeof(envBytes),envBytes};
        ULONG task=0;
        HANDLE undo;
        LUID invalidOutput={0x12345678,0x12345678};
        CHECK(!OpenNtBaseInitializeInteractiveScope(NULL) && GetLastError()==ERROR_INVALID_PARAMETER);
        CHECK(!NT_SUCCESS(_UserTestTokenForInteractive(NULL,&invalidOutput)));
        CHECK(invalidOutput.LowPart==0x12345678 && invalidOutput.HighPart==0x12345678);
        CHECK(OpenNtBaseInitializeInteractiveScope(&scope));
        wrong=scope; wrong.AuthenticationId.LowPart=0x3e7; wrong.AuthenticationId.HighPart=0;
        CHECK(OpenNtBaseBindInteractiveScope(&wrong)==NULL);
        CHECK(!NT_SUCCESS(NtUserTestForInteractiveUser(&wrong.AuthenticationId)));
        CHECK(OpenNtBaseBindInteractiveScope(NULL)==&wrong);
        UserTestTokenForInteractive=_UserTestTokenForInteractive;
        ZeroMemory(&m,sizeof(m));
        CHECK(!BaseCheckVDM(BINARY_TYPE_WIN16,L"O:\\ntvdm64\\system32\\WRITE.EXE",L"WRITE.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_ACCESS_DENIED && WOWHead==NULL && captures==0);
        wrong=scope; wrong.AuthenticationId.LowPart^=1;
        CHECK(OpenNtBaseBindInteractiveScope(&wrong)==NULL);
        ZeroMemory(&m,sizeof(m));
        CHECK(!BaseCheckVDM(BINARY_TYPE_WIN16,L"O:\\ntvdm64\\system32\\WRITE.EXE",L"WRITE.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_ACCESS_DENIED && WOWHead==NULL && captures==0);
        CHECK(OpenNtBaseBindInteractiveScope(&scope)==&wrong);
        ZeroMemory(&m,sizeof(m));
        CHECK(BaseCheckVDM(BINARY_TYPE_WIN16,L"O:\\ntvdm64\\system32\\WRITE.EXE",L"WRITE.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(WOWHead!=NULL && task!=0 && m.u.CheckVDM.VDMState==VDM_NOT_PRESENT && captures==0);
        CHECK(WOWHead->WOWRecord->iTask==task);
        CHECK(WowAuthId.LowPart==scope.AuthenticationId.LowPart && WowAuthId.HighPart==scope.AuthenticationId.HighPart);
        CHECK(!strcmp(WOWHead->WOWRecord->lpVDMInfo->AppName,"O:\\ntvdm64\\system32\\WRITE.EXE"));
        undo=(HANDLE)task;
        CHECK(BaseUpdateVDMEntry(UPDATE_VDM_UNDO_CREATION,&undo,VDM_PARTIALLY_CREATED,BINARY_TYPE_WIN16));
        CHECK(WOWHead==NULL && OpenNtBaseBindInteractiveScope(NULL)==&scope);
        CHECK(WowAuthId.LowPart==0xffffffff && WowAuthId.HighPart==-1);
        puts("PASS: original shared-WOW admission rejects absent/wrong scope, accepts matching logon and undoes launch");
    }
    {
        OPENNT_BASE_INTERACTIVE_SCOPE scope;
        STARTUPINFOW launch={sizeof(launch)};
        CHAR envBytes[]="PATH=O:\\ntvdm64\0";
        ANSI_STRING env={sizeof(envBytes),sizeof(envBytes),envBytes};
        ULONG types[2]={BINARY_TYPE_DOS,BINARY_TYPE_WIN16},i;
        CHECK(OpenNtBaseInitializeInteractiveScope(&scope));
        CHECK(OpenNtBaseBindInteractiveScope(&scope)==NULL);
        for(i=0;i<2;++i) {
            CSR_PROCESS worker={0};
            ULONG task=0, sequence=0x100+i;
            HANDLE console=i?(HANDLE)-1:(HANDLE)1;
            HANDLE wait=GetCurrentProcess();
            LPCWSTR app=i?L"O:\\ntvdm64\\system32\\WRITE.EXE":L"O:\\ntvdm64\\MEM.EXE";
            ZeroMemory(&m,sizeof(m));
            CHECK(BaseCheckVDM(types[i],app,app,L"O:\\ntvdm64",&env,&m,&task,0,&launch));
            CHECK(BaseUpdateVDMEntry(UPDATE_VDM_PROCESS_HANDLE,&wait,task,types[i]));
            CHECK(WaitForSingleObject(wait,0)==WAIT_TIMEOUT);
            BaseSrvUpdateVDMSequenceNumber(console,sequence,task);
            BaseSrvUpdateVDMSequenceNumber(console,sequence+10,task);
            if(i) CHECK(WOWHead && WOWHead->SequenceNumber==sequence);
            else {
                CHECK(BaseSrvGetConsoleRecord(console,&record)==0);
                CHECK(record->SequenceNumber==sequence);
            }
            worker.fVDM=TRUE; worker.SequenceNumber=sequence+10;
            BaseSrvCleanupVDMResources(&worker);
            CHECK(WaitForSingleObject(wait,0)==WAIT_TIMEOUT);
            worker.fVDM=FALSE; worker.SequenceNumber=sequence;
            BaseSrvCleanupVDMResources(&worker);
            CHECK(WaitForSingleObject(wait,0)==WAIT_TIMEOUT);
            worker.fVDM=TRUE;
            BaseSrvCleanupVDMResources(&worker);
            CHECK(WaitForSingleObject(wait,0)==WAIT_OBJECT_0);
            if(i) CHECK(WOWHead==NULL && hwndWowExec==NULL && ulWowExecProcessSequenceNumber==0);
            else CHECK(BaseSrvGetConsoleRecord(console,&record)==(ULONG)STATUS_INVALID_PARAMETER);
            BaseSrvCleanupVDMResources(&worker);
            CHECK(WaitForSingleObject(wait,0)==WAIT_OBJECT_0);
            CHECK(CloseHandle(wait) && captures==0);
        }
        CHECK(OpenNtBaseBindInteractiveScope(NULL)==&scope);
        puts("PASS: original DOS/WOW generation registration and termination cleanup, wrong generation/role rejection and repeat cleanup");
    }
    CHECK(OpenNtBaseServerRequestThread()==&thread);
    CHECK(OpenNtBaseBindServerRequestThread(NULL)==&thread);
    CHECK(OpenNtBaseServerRequestThread()==NULL);
    puts("PASS: request-thread isolation, nested restoration and explicit unbind");
    CHECK(OpenNtBaseRemoveProcess(&processRegistry,&caller));
    CHECK(!OpenNtBaseRemoveProcess(&processRegistry,&caller) && GetLastError()==ERROR_NOT_FOUND);
    CHECK(!OpenNtBaseRegisterProcess(&processRegistry,&caller,NULL));
    CHECK(OpenNtBaseRegisterProcess(&processRegistry,&caller,GetCurrentProcess()));
    CHECK(caller.SequenceNumber==2);
    {
        HANDLE retained=NULL,stale=NULL;
        CHECK(!OpenNtBaseRetainRegisteredProcess(&processRegistry,GetCurrentProcessId(),1,&stale));
        CHECK(GetLastError()==ERROR_ACCESS_DENIED && stale==NULL);
        CHECK(OpenNtBaseRetainRegisteredProcess(&processRegistry,GetCurrentProcessId(),2,&retained));
        CHECK(OpenNtBaseRemoveProcess(&processRegistry,&caller));
        CHECK(WaitForSingleObject(retained,0)==WAIT_TIMEOUT);
        CHECK(!OpenNtBaseRetainRegisteredProcess(&processRegistry,GetCurrentProcessId(),2,&stale));
        CHECK(GetLastError()==ERROR_NOT_FOUND && stale==NULL);
        CHECK(CloseHandle(retained));
        CHECK(OpenNtBaseRegisterProcess(&processRegistry,&caller,GetCurrentProcess()));
    }
    CHECK(OpenNtBaseRemoveProcess(&processRegistry,&caller));
    {
        HANDLE limited=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
        HANDLE unexpected=NULL;
        PCSR_PROCESS found=NULL;
        CHECK(limited!=NULL);
        CHECK(OpenNtBaseRegisterProcess(&processRegistry,&caller,limited));
        CHECK(CloseHandle(limited));
        CHECK(CsrLockProcessByClientId((HANDLE)GetCurrentProcessId(),&found)==0);
        CHECK(GetProcessId(found->ProcessHandle)==GetCurrentProcessId());
        CHECK(WaitForSingleObject(found->ProcessHandle,0)==WAIT_TIMEOUT);
        CHECK(!DuplicateHandle(found->ProcessHandle,GetCurrentProcess(),GetCurrentProcess(),
            &unexpected,0,FALSE,DUPLICATE_SAME_ACCESS));
        CHECK(GetLastError()==ERROR_ACCESS_DENIED && unexpected==NULL);
        CHECK(CsrUnlockProcess(found)==0);
        CHECK(OpenNtBaseRemoveProcess(&processRegistry,&caller));
    }
    {
        registry_remove_test test={0};
        HANDLE remover;
        PCSR_PROCESS pinned=NULL;
        DWORD exitCode;
        CHECK(OpenNtBaseRegisterProcess(&processRegistry,&caller,GetCurrentProcess()));
        CHECK(CsrLockProcessByClientId((HANDLE)GetCurrentProcessId(),&pinned)==0);
        test.started=CreateEventW(NULL,TRUE,FALSE,NULL);
        test.finished=CreateEventW(NULL,TRUE,FALSE,NULL);
        CHECK(test.started && test.finished);
        remover=CreateThread(NULL,0,remove_registered_peer,&test,0,NULL);
        CHECK(remover!=NULL);
        CHECK(WaitForSingleObject(test.started,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(test.finished,50)==WAIT_TIMEOUT);
        CHECK(GetProcessId(pinned->ProcessHandle)==GetCurrentProcessId());
        CHECK(CsrUnlockProcess(pinned)==0);
        CHECK(WaitForSingleObject(test.finished,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(remover,5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(remover,&exitCode) && exitCode==0);
        CHECK(test.removed && caller.ProcessHandle==NULL);
        CHECK(CloseHandle(remover) && CloseHandle(test.started) && CloseHandle(test.finished));
        puts("PASS: concurrent removal waits for original service lookup unlock");
    }
    {
        WCHAR image[MAX_PATH], command[MAX_PATH+32];
        STARTUPINFOW startupInfo={sizeof(startupInfo)};
        PROCESS_INFORMATION child={0};
        CSR_PROCESS childRecord={0};
        PCSR_PROCESS found=NULL;
        DWORD size=GetModuleFileNameW(NULL,image,MAX_PATH), code;
        CHECK(size>0 && size<MAX_PATH);
        CHECK(_snwprintf(command,MAX_PATH+32,L"\"%ls\" --registry-child",image)>0);
        CHECK(CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
            NULL,NULL,&startupInfo,&child));
        CHECK(OpenNtBaseRegisterProcess(&processRegistry,&childRecord,child.hProcess));
        CHECK(ResumeThread(child.hThread)!=MAXULONG);
        CHECK(CloseHandle(child.hThread));
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeProcess(child.hProcess,&code) && code==0);
        {
            HANDLE unexpected=NULL;
            CHECK(OpenNtBaseDuplicateObject(GetCurrentProcess(),GetCurrentProcess(),child.hProcess,
                &unexpected,0,0,DUPLICATE_SAME_ACCESS)==(NTSTATUS)0xc00000bbL);
            CHECK(unexpected==NULL);
        }
        CHECK(CloseHandle(child.hProcess));
        CHECK(CsrLockProcessByClientId((HANDLE)child.dwProcessId,&found)==0 && found==&childRecord);
        CHECK(WaitForSingleObject(found->ProcessHandle,0)==WAIT_OBJECT_0);
        CHECK(GetProcessId(found->ProcessHandle)==child.dwProcessId);
        CHECK(CsrUnlockProcess(found)==0);
        CHECK(OpenNtBaseRemoveProcess(&processRegistry,&childRecord));
        CHECK(CsrLockProcessByClientId((HANDLE)child.dwProcessId,&found)<0 && found==NULL);
        puts("PASS: real exited process remains referenced until explicit registered cleanup");
    }
    processRegistry.NextSequence=MAXULONG;
    CHECK(!OpenNtBaseRegisterProcess(&processRegistry,&caller,GetCurrentProcess()));
    CHECK(GetLastError()==ERROR_ARITHMETIC_OVERFLOW);
    CHECK(OpenNtBaseBindProcessRegistry(NULL)==&processRegistry);
    CHECK(OpenNtBaseDestroyProcessRegistry(&processRegistry));
    puts("PASS: registered process ownership, duplicate/missing/pinned removal, empty teardown and sequence exhaustion");
    CHECK(captures==0 && HeapDestroy(CsrPortHeap));
    puts("PASS: original first-VDM, record/command/directory capacity, dispatch/completion, parent/worker events, reentry, empty-WOW, cleanup");
    return 0;
}
