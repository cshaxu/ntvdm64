/* Retained comparison-only pre-import dispatcher catalogue.
 *
 * This file was the former src/bx-vdm/bop/opennt/dem/demdisp.c.  It is not
 * compiled by the formal bx-vdm module.  The real OpenNT demdisp.c now owns
 * the active table and DemDispatch implementation. */
#include "demdisp_catalog_v2.h"

uint32_t fShowSVCMsg;
uint32_t CurrentISVC;

static const bx_ntvdm_opennt_dem_service_entry apfnSVC[] = {
 {0x00u,"demChgFilePtr","demhndl.c"},{0x01u,"demChMod","demfile.c"},{0x02u,"demClose","demhndl.c"},{0x03u,"demCreate","demfile.c"},
 {0x04u,"demCreateDir","demdir.c"},{0x05u,"demDelete","demfile.c"},{0x06u,"demDeleteDir","demdir.c"},{0x07u,"demDeleteFCB","demfcb.c"},
 {0x08u,"demFileTimes","demhndl.c"},{0x09u,"demFindFirst","demsrch.c"},{0x0au,"demFindFirstFCB","demsrch.c"},{0x0bu,"demFindNext","demsrch.c"},
 {0x0cu,"demFindNextFCB","demsrch.c"},{0x0du,"demGetBootDrive","demgset.c"},{0x0eu,"demGetDriveFreeSpace","demgset.c"},{0x0fu,"demGetDrives","demgset.c"},
 {0x10u,"demGSetMediaID","demgset.c"},{0x11u,"demLoadDos","demmisc.c"},{0x12u,"demOpen","demfile.c"},{0x13u,"demQueryCurrentDir","demdir.c"},
 {0x14u,"demQueryDate","demgset.c"},{0x15u,"demQueryTime","demgset.c"},{0x16u,"demRead","demhndl.c"},{0x17u,"demRename","demfile.c"},
 {0x18u,"demSetCurrentDir","demdir.c"},{0x19u,"demSetDate","demgset.c"},{0x1au,"demSetDefaultDrive","demgset.c"},{0x1bu,"demSetDTALocation","demgset.c"},
 {0x1cu,"demSetTime","demgset.c"},{0x1du,"demSetV86KernelAddr","demdisp.c"},{0x1eu,"demWrite","demhndl.c"},{0x1fu,"demNotYetImplemented","demdisp.c"},
 {0x20u,"demRenameFCB","demfcb.c"},{0x21u,"demIOCTL","demioctl.c"},{0x22u,"demCreateNew","demfile.c"},{0x23u,"demDiskReset","demmisc.c"},
 {0x24u,"demNotYetImplemented","demdisp.c"},{0x25u,"demGetDPB","demgset.c"},{0x26u,"demNotYetImplemented","demdisp.c"},{0x27u,"demCommit","demhndl.c"},
 {0x28u,"demNotYetImplemented","demdisp.c"},{0x29u,"demAbsRead","demdasd.c"},{0x2au,"demAbsWrite","demdasd.c"},{0x2bu,"demNotYetImplemented","demdisp.c"},
 {0x2cu,"demCreateFCB","demfcb.c"},{0x2du,"demOpenFCB","demfcb.c"},{0x2eu,"demCloseFCB","demfcb.c"},{0x2fu,"demFCBIO","demfcb.c"},
 {0x30u,"demDate16","demfcb.c"},{0x31u,"demGetFileInfo","demfcb.c"},{0x32u,"demSetHardErrorInfo","demerror.c"},{0x33u,"demRetry","demerror.c"},
 {0x34u,"demLoadDosAppSym","demmisc.c"},{0x35u,"demFreeDosAppSym","demmisc.c"},{0x36u,"demEntryDosApp","demmisc.c"},{0x37u,"demDOSDispCall","demmisc.c"},
 {0x38u,"demDOSDispRet","demmisc.c"},{0x39u,"demOutputString","demmisc.c"},{0x3au,"demInputString","demmisc.c"},{0x3bu,"demIsDebug","demmisc.c"},
 {0x3cu,"demTerminatePDB","demsrch.c"},{0x3du,"demExitVDM","demmisc.c"},{0x3eu,"demWOWFiles","demmisc.c"},{0x3fu,"demLockOper","demlock.c"},
 {0x40u,"demNotYetImplemented","demdisp.c"},{0x41u,"demGetComputerName","demgset.c"},{0x42u,"demNotYetImplemented","demdisp.c"},{0x43u,"demNotYetImplemented","demdisp.c"},
 {0x44u,"demCheckPath","demfile.c"},{0x45u,"demSystemSymbolOp","demmisc.c"},{0x46u,"demGetDPBList","demgset.c"},{0x47u,"demPipeFileDataEOF","demhndl.c"},{0x48u,"demPipeFileEOF","demhndl.c"}
};

size_t bx_ntvdm_opennt_dem_service_count(void)
{ return sizeof(apfnSVC) / sizeof(apfnSVC[0]); }

const bx_ntvdm_opennt_dem_service_entry *bx_ntvdm_opennt_dem_service_at(uint8_t service)
{ return service < bx_ntvdm_opennt_dem_service_count() ? &apfnSVC[service] : 0; }

int bx_ntvdm_opennt_dem_not_yet_implemented(const bx_ntvdm_exception_event_v1 *boundary, bx_ntvdm_cpu_result_v2 *result)
{ return boundary != 0 && result != 0 && bx_ntvdm_exception_event_v1_valid(boundary) && boundary->fault_rip <= UINT64_MAX - 4u && bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) && bx_ntvdm_cpu_result_v2_set_cf(result, 0); }

int bx_ntvdm_opennt_dem_set_v86_kernel_addr(const bx_ntvdm_exception_event_v1 *boundary, bx_ntvdm_cpu_result_v2 *result)
{ return boundary != 0 && result != 0 && bx_ntvdm_exception_event_v1_valid(boundary) && boundary->fault_rip <= UINT64_MAX - 4u && bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u); }
