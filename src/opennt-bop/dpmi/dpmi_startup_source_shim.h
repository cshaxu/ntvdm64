#ifndef RUNTIME_OPENNT_BOP_DPMI_STARTUP_SOURCE_SHIM_H
#define RUNTIME_OPENNT_BOP_DPMI_STARTUP_SOURCE_SHIM_H

/* DIVERGENCE(BOP-DIV-068): this is the smallest modern compilation closure for
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

void runtime_dpmi_startup_source_deferred(void);
void runtime_dpmi_startup_source_note_illegal(uint32_t index);

/* Function bodies outside the admitted S3 startup/table group remain a
 * source-shaped deferred terminal until their owner subpackage is composed. */
#define DpmiSetDescriptorEntry runtime_dpmi_startup_source_deferred
#define switch_to_protected_mode runtime_dpmi_startup_source_deferred
#define DpmiSetProtectedmodeInterrupt runtime_dpmi_startup_source_deferred
#define DpmiGetFastBopEntry runtime_dpmi_startup_source_deferred
#define DpmiXlatInt21Call runtime_dpmi_startup_source_deferred
#define DpmiAllocateXmem runtime_dpmi_startup_source_deferred
#define DpmiFreeXmem runtime_dpmi_startup_source_deferred
#define DpmiReallocateXmem runtime_dpmi_startup_source_deferred
#define DpmiSetFaultHandler runtime_dpmi_startup_source_deferred
#define DpmiGetMemoryInfo runtime_dpmi_startup_source_deferred
#define DpmiDpmiInUse runtime_dpmi_startup_source_deferred
#define DpmiDpmiNoLongerInUse runtime_dpmi_startup_source_deferred
#define DpmiSetDebugRegisters runtime_dpmi_startup_source_deferred
#define DpmiFreeAppXmem runtime_dpmi_startup_source_deferred
#define DpmiPassPmStackInfo runtime_dpmi_startup_source_deferred
#define DpmiVcdPmSvcCall32 runtime_dpmi_startup_source_deferred
#define DpmiFreeAllXmem runtime_dpmi_startup_source_deferred
#define DpmiIntHandlerIret16 runtime_dpmi_startup_source_deferred
#define DpmiIntHandlerIret32 runtime_dpmi_startup_source_deferred
#define DpmiFaultHandlerIret16 runtime_dpmi_startup_source_deferred
#define DpmiFaultHandlerIret32 runtime_dpmi_startup_source_deferred
#define DpmiUnhandledExceptionHandler runtime_dpmi_startup_source_deferred

#ifdef __cplusplus
extern "C" {
#endif

VOID DpmiInitDosx(VOID);
VOID DpmiInitApp(VOID);
VOID DpmiPassTableAddress(VOID);

int runtime_dpmi_startup_source_take_dispatch(ULONG *index);
void runtime_dpmi_startup_source_advance_ip(uint32_t bytes);
void runtime_dpmi_startup_source_initialize_dosx(void);
void runtime_dpmi_startup_source_initialize_app(void);
void runtime_dpmi_startup_source_publish_selector_table(void);

#ifdef __cplusplus
}
#endif

#endif
