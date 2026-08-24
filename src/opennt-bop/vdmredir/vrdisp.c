/* Re-rooted from OpenNT base/mvdm/vdmredir/vrdisp.c. */
#include <windows.h>
#include "vrdisp_compat.h"
static __declspec(thread) const struct runtime_generic_ud_event_v1 *VrEvent;
static __declspec(thread) struct runtime_generic_ud_outcome_v1 *VrOutcome;
static __declspec(thread) runtime_vr_dispatch_body_fn VrBody;
/* DIVERGENCE(BOP-DIV-056): original dispatch uses the product-global CCPU
 * frame; the standalone call binds one copied frame callback synchronously. */
static VOID VrpDispatchService(uint8_t service) { (void)VrBody(service, VrEvent, VrOutcome); }
#define VR_SERVICE(name, code) static VOID name(VOID) { VrpDispatchService(code); }
VR_SERVICE(VrInitialize,0) VR_SERVICE(VrUninitialize,1) VR_SERVICE(VrGetNamedPipeInfo,2) VR_SERVICE(VrGetNamedPipeHandleState,3)
VR_SERVICE(VrSetNamedPipeHandleState,4) VR_SERVICE(VrPeekNamedPipe,5) VR_SERVICE(VrTransactNamedPipe,6) VR_SERVICE(VrCallNamedPipe,7)
VR_SERVICE(VrWaitNamedPipe,8) VR_SERVICE(VrDeleteMailslot,9) VR_SERVICE(VrGetMailslotInfo,10) VR_SERVICE(VrMakeMailslot,11)
VR_SERVICE(VrPeekMailslot,12) VR_SERVICE(VrReadMailslot,13) VR_SERVICE(VrWriteMailslot,14) VR_SERVICE(VrTerminateDosProcess,15)
VR_SERVICE(VrNetTransactApi,16) VR_SERVICE(VrNetRemoteApi,17) VR_SERVICE(VrNetNullTransactApi,18) VR_SERVICE(VrNetServerEnum,19)
VR_SERVICE(VrNetUseAdd,20) VR_SERVICE(VrNetUseDel,21) VR_SERVICE(VrNetUseEnum,22) VR_SERVICE(VrNetUseGetInfo,23)
VR_SERVICE(VrNetWkstaGetInfo,24) VR_SERVICE(VrNetWkstaSetInfo,25) VR_SERVICE(VrNetMessageBufferSend,26) VR_SERVICE(VrGetCDNames,27)
VR_SERVICE(VrGetComputerName,28) VR_SERVICE(VrGetUserName,29) VR_SERVICE(VrGetDomainName,30) VR_SERVICE(VrGetLogonServer,31)
VR_SERVICE(VrNetHandleGetInfo,32) VR_SERVICE(VrNetHandleSetInfo,33) VR_SERVICE(VrNetGetDCName,34) VR_SERVICE(VrReadWriteAsyncNmPipe,35)
VR_SERVICE(VrReadWriteAsyncNmPipeWrite,36) VR_SERVICE(VrNetbios5c,37) VR_SERVICE(VrHandleAsyncCompletion,38) VR_SERVICE(VrDlc5cHandler,39)
VR_SERVICE(VrVdmWindowInit,40) VR_SERVICE(VrReturnAssignMode,41) VR_SERVICE(VrSetAssignMode,42) VR_SERVICE(VrGetAssignListEntry,43)
VR_SERVICE(VrDefineMacro,44) VR_SERVICE(VrBreakMacro,45) VR_SERVICE(VrNetServiceControl,46) VR_SERVICE(VrDismissInterrupt,47)
VR_SERVICE(VrEoiAndDismissInterrupt,48) VR_SERVICE(VrCheckPmNetbiosAnr,49)
VOID (*VrDispatchTable[])(VOID)={VrInitialize,VrUninitialize,VrGetNamedPipeInfo,VrGetNamedPipeHandleState,VrSetNamedPipeHandleState,VrPeekNamedPipe,VrTransactNamedPipe,VrCallNamedPipe,VrWaitNamedPipe,VrDeleteMailslot,VrGetMailslotInfo,VrMakeMailslot,VrPeekMailslot,VrReadMailslot,VrWriteMailslot,VrTerminateDosProcess,VrNetTransactApi,VrNetRemoteApi,VrNetNullTransactApi,VrNetServerEnum,VrNetUseAdd,VrNetUseDel,VrNetUseEnum,VrNetUseGetInfo,VrNetWkstaGetInfo,VrNetWkstaSetInfo,VrNetMessageBufferSend,VrGetCDNames,VrGetComputerName,VrGetUserName,VrGetDomainName,VrGetLogonServer,VrNetHandleGetInfo,VrNetHandleSetInfo,VrNetGetDCName,VrReadWriteAsyncNmPipe,VrReadWriteAsyncNmPipeWrite,VrNetbios5c,VrHandleAsyncCompletion,VrDlc5cHandler,VrVdmWindowInit,VrReturnAssignMode,VrSetAssignMode,VrGetAssignListEntry,VrDefineMacro,VrBreakMacro,VrNetServiceControl,VrDismissInterrupt,VrEoiAndDismissInterrupt,VrCheckPmNetbiosAnr};
BOOL VrDispatch(ULONG SvcCode) { if (SvcCode >= sizeof(VrDispatchTable)/sizeof(VrDispatchTable[0])) return FALSE; VrDispatchTable[SvcCode](); return TRUE; }
int runtime_vr_dispatch_with_frame(uint8_t service,const struct runtime_generic_ud_event_v1 *event,struct runtime_generic_ud_outcome_v1 *outcome,runtime_vr_dispatch_body_fn body) { int handled; if (!event || !outcome || !body || VrBody) return 0; VrEvent=event; VrOutcome=outcome; VrBody=body; handled=VrDispatch(service); VrBody=NULL; VrEvent=NULL; VrOutcome=NULL; return handled; }
