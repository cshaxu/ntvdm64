#ifndef BX_NTVDM_OPENNT_BOP_DPMI_DESCRIPTOR_SOURCE_SHIM_H
#define BX_NTVDM_OPENNT_BOP_DPMI_DESCRIPTOR_SOURCE_SHIM_H

/* DIVERGENCE(BOP-DIV-067): OpenNT's 486 DPMI owner installed descriptors in the
 * NT4 host process LDT.  The modern source boundary instead copies exactly
 * the bounded guest descriptor list through the existing selector-blind
 * protected-range action, then publishes it to the already source-published
 * guest LDT.  No host pointer or descriptor cache is retained. */

#include <stdint.h>

#include "dpmi_startup_session_shim.h"

typedef uint8_t UCHAR;
typedef uint16_t USHORT;
typedef uint32_t ULONG;
typedef int BOOL;
typedef void VOID;

/* DIVERGENCE(BOP-DIV-079): the historical 16-bit selector ABI requires an
 * eight-byte LDT record.  Native x64 alignment would enlarge this source
 * compatibility record and change dpmiselr.c's original selector stride. */
#pragma pack(push, 1)
typedef struct _LDT_ENTRY {
  USHORT LimitLow;
  USHORT BaseLow;
  union {
    struct {
      UCHAR BaseMid;
      UCHAR Flags1;
      UCHAR Flags2;
      UCHAR BaseHi;
    } Bytes;
    struct {
      ULONG BaseMid : 8;
      ULONG Type : 5;
      ULONG Dpl : 2;
      ULONG Pres : 1;
      ULONG LimitHi : 4;
      ULONG Sys : 1;
      ULONG Reserved_0 : 1;
      ULONG Default_Big : 1;
      ULONG Granularity : 1;
      ULONG BaseHi : 8;
    } Bits;
  } HighWord;
} LDT_ENTRY, *PLDT_ENTRY;
#pragma pack(pop)

_Static_assert(sizeof(LDT_ENTRY) == 8u,
  "OpenNT DPMI selector records retain their eight-byte ABI");
#pragma pack(pop)

_Static_assert(sizeof(LDT_ENTRY) == 8u,
  "OpenNT DPMI selector records retain their eight-byte ABI");

#define TRUE 1
#define FALSE 0
#define DBG 0
#define BX_NTVDM_DPMI_DESCRIPTOR_MAX_ENTRIES 512u

/* Keep the original source spellings inside its translation unit, but do not
 * export generic SoftPC register helper names into the combined bx-vdm link. */
#define getAX bx_ntvdm_dpmi_getAX
#define getBX bx_ntvdm_dpmi_getBX
#define getCX bx_ntvdm_dpmi_getCX
#define getES bx_ntvdm_dpmi_getES
#define setAX bx_ntvdm_dpmi_setAX
#define DpmiSetX86Descriptor bx_ntvdm_dpmi_set_x86_descriptor

#ifdef __cplusplus
extern "C" {
#endif

USHORT bx_ntvdm_dpmi_getAX(VOID);
USHORT bx_ntvdm_dpmi_getBX(VOID);
USHORT bx_ntvdm_dpmi_getCX(VOID);
USHORT bx_ntvdm_dpmi_getES(VOID);
VOID bx_ntvdm_dpmi_setAX(USHORT value);
BOOL bx_ntvdm_dpmi_set_x86_descriptor(LDT_ENTRY *descriptors, USHORT registerAX,
  USHORT registerCX);
BOOL bx_ntvdm_dpmi_descriptor_source_acquire(LDT_ENTRY *descriptors,
  USHORT registerCX);
VOID DpmiSetDescriptorEntry(VOID);

#ifdef __cplusplus
}
#endif

#endif
