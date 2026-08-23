#ifndef BX_NTVDM_BOP_SHIM_DPMI_STARTUP_SOURCE_SHIM_H
#define BX_NTVDM_BOP_SHIM_DPMI_STARTUP_SOURCE_SHIM_H

/* DIVERGENCE (T257 S3): this is the smallest modern compilation closure for
 * the exact-copy OpenNT dpmi32.c startup/table subset.  It supplies types and
 * named session operations only; it neither provides a process LDT nor makes
 * a DPMI BOP ingress active. */

#include "dpmi_startup_session_shim.h"

#include <stdint.h>

/* Use the public Win32 scalar spellings also used by the original source;
 * do not recreate the old private NT4 base headers. */
#include <windows.h>
typedef WORD *PWORD16;
typedef DWORD *PDWORD16;

#define TRUE 1
#define FALSE 0
#define DBG 0
#define ASSERT(expression) ((void)(expression))
#define LDT_SIZE 0x1fffu
#define DPMI_32BIT 0x1u
#define MAX_DPMI_BOP_FUNC 25u
#define DPMI_DISPATCH_ENTRY 12u
#define DBGTRACE(type, first, second, third) ((void)0)

void bx_ntvdm_dpmi_startup_source_deferred(void);
void bx_ntvdm_dpmi_startup_source_note_illegal(uint32_t index);

/* Function bodies outside the admitted S3 startup/table group remain a
 * source-shaped deferred terminal until their owner subpackage is composed. */
#define DpmiSetDescriptorEntry bx_ntvdm_dpmi_startup_source_deferred
#define switch_to_protected_mode bx_ntvdm_dpmi_startup_source_deferred
#define DpmiSetProtectedmodeInterrupt bx_ntvdm_dpmi_startup_source_deferred
#define DpmiGetFastBopEntry bx_ntvdm_dpmi_startup_source_deferred
#define DpmiXlatInt21Call bx_ntvdm_dpmi_startup_source_deferred
#define DpmiAllocateXmem bx_ntvdm_dpmi_startup_source_deferred
#define DpmiFreeXmem bx_ntvdm_dpmi_startup_source_deferred
#define DpmiReallocateXmem bx_ntvdm_dpmi_startup_source_deferred
#define DpmiSetFaultHandler bx_ntvdm_dpmi_startup_source_deferred
#define DpmiGetMemoryInfo bx_ntvdm_dpmi_startup_source_deferred
#define DpmiDpmiInUse bx_ntvdm_dpmi_startup_source_deferred
#define DpmiDpmiNoLongerInUse bx_ntvdm_dpmi_startup_source_deferred
#define DpmiSetDebugRegisters bx_ntvdm_dpmi_startup_source_deferred
#define DpmiFreeAppXmem bx_ntvdm_dpmi_startup_source_deferred
#define DpmiPassPmStackInfo bx_ntvdm_dpmi_startup_source_deferred
#define DpmiVcdPmSvcCall32 bx_ntvdm_dpmi_startup_source_deferred
#define DpmiFreeAllXmem bx_ntvdm_dpmi_startup_source_deferred
#define DpmiIntHandlerIret16 bx_ntvdm_dpmi_startup_source_deferred
#define DpmiIntHandlerIret32 bx_ntvdm_dpmi_startup_source_deferred
#define DpmiFaultHandlerIret16 bx_ntvdm_dpmi_startup_source_deferred
#define DpmiFaultHandlerIret32 bx_ntvdm_dpmi_startup_source_deferred
#define DpmiUnhandledExceptionHandler bx_ntvdm_dpmi_startup_source_deferred

#ifdef __cplusplus
extern "C" {
#endif

VOID DpmiInitDosx(VOID);
VOID DpmiInitApp(VOID);
VOID DpmiPassTableAddress(VOID);

int bx_ntvdm_dpmi_startup_source_take_dispatch(ULONG *index);
void bx_ntvdm_dpmi_startup_source_advance_ip(uint32_t bytes);
void bx_ntvdm_dpmi_startup_source_initialize_dosx(void);
void bx_ntvdm_dpmi_startup_source_initialize_app(void);
void bx_ntvdm_dpmi_startup_source_publish_selector_table(void);

#ifdef __cplusplus
}
#endif

#endif
