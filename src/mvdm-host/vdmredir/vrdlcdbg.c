/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    vrdlcdbg.c

Abstract:

    Contains functions for dumping CCBs, parameter tables; diagnostic and
    debugging functions for DOS DLC (CCB1)

    Contents:
        DbgOut
        DbgOutStr
        DumpCcb
        DumpDosDlcBufferPool
        DumpDosDlcBufferChain
        MapCcbRetcode
        (DefaultParameterTableDump)
        (DumpParameterTableHeader)
        (DumpBufferFreeParms)
        (DumpBufferGetParms)
        (DumpDirCloseAdapterParms)
        (DumpDirDefineMifEnvironmentParms)
        (DumpDirInitializeParms)
        (DumpDirModifyOpenParmsParms)
        (DumpDirOpenAdapterParms)
        (DumpDirReadLog)
        (DumpDirRestoreOpenParmsParms)
        (DumpDirSetFunctionalAddressParms)
        (DumpDirSetGroupAddressParms)
        (DumpDirSetUserAppendageParms)
        (DumpDirStatusParms)
        (DumpDirTimerCancelParms)
        (DumpDirTimerCancelGroupParms)
        (DumpDirTimerSetParms)
        (DumpDlcCloseSapParms)
        (DumpDlcCloseStationParms)
        (DumpDlcConnectStationParms)
        (DumpDlcFlowControlParms)
        (MapFlowControl)
        (DumpDlcModifyParms)
        (DumpDlcOpenSapParms)
        (MapOptionsPriority)
        (DumpDlcOpenStationParms)
        (DumpDlcReallocateParms)
        (DumpDlcResetParms)
        (DumpDlcStatisticsParms)
        (DumpPdtTraceOffParms)
        (DumpPdtTraceOnParms)
        (DumpReadParms)
        (MapReadEvent)
        (MapDlcStatus)
        (DumpReadCancelParms)
        (DumpReceiveParms)
        (DumpReceiveCancelParms)
        (DumpReceiveModifyParms)
        (DumpTransmitDirFrameParms)
        (DumpTransmitIFrameParms)
        (DumpTransmitTestCmdParms)
        (DumpTransmitUiFrameParms)
        (DumpTransmitXidCmdParms)
        (DumpTransmitXidRespFinalParms)
        (DumpTransmitXidRespNotFinalParms)
        (DumpTransmitParms)
        (DumpTransmitQueue)
        DumpReceiveDataBuffer
        (MapMessageType)
        DumpData
        IsCcbErrorCodeAllowable
        IsCcbErrorCodeValid
        IsCcbCommandValid
        MapCcbCommandToName
        DumpDosAdapter
        (MapAdapterType)

Author:

    Richard L Firth (rfirth) 30-Apr-1992

Revision History:

--*/

#line 87
#if DBG
#line 88
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 4025
