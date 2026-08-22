#ifndef BX_NTVDM_BOP_SHIM_XMS_SHIM_H
#define BX_NTVDM_BOP_SHIM_XMS_SHIM_H

/* Compatibility surface for the directly mirrored OpenNT XMS sources.
 * It replaces their CCPU/SAS/private-NT product shell with one scoped,
 * fixed-width call.  It is not an XMS dispatcher, allocator, UMB manager or
 * BIOS implementation. */

#include <windows.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "../../bx_ntvdm_cpu_result_v2.h"
#include "../../bx_ntvdm_cpu_state_abi.h"
#include "../../bx_ntvdm_exception_abi.h"

typedef uint8_t BYTE;
typedef uint16_t USHORT;
typedef uint16_t WORD;
typedef void (*PFNSVC)(VOID);

#define XMS_LASTSVC 12u
#define UMB_OWNER_RAM 1u
#define UMB_OWNER_XMS 2u
#define XMSUMB_THRESHOLD (3u * 16u)
#define i386 1
#define FETCHDWORD(value) (value)
#define DBG 0

typedef struct _XMSUMB_ {
    WORD Segment, Size, Owner;
    struct _XMSUMB_ *Next;
} XMSUMB, *PXMSUMB;

typedef int (*bx_ntvdm_xms_guest_read_fn)(void *state, uint32_t address,
    uint8_t *buffer, uint32_t bytes);
typedef int (*bx_ntvdm_xms_guest_write_fn)(void *state, uint32_t address,
    const uint8_t *buffer, uint32_t bytes);

typedef struct bx_ntvdm_xms_call {
    uint32_t magic, abi_version, struct_bytes, service;
    const bx_ntvdm_exception_event_v1 *boundary;
    const bx_ntvdm_cpu_state_v1 *cpu;
    bx_ntvdm_cpu_result_v2 *result;
    void *guest_state;
    bx_ntvdm_xms_guest_read_fn guest_read;
    bx_ntvdm_xms_guest_write_fn guest_write;
} bx_ntvdm_xms_call;

#define BX_NTVDM_XMS_CALL_MAGIC 0x4258584du
#define BX_NTVDM_XMS_CALL_VERSION 1u

int bx_ntvdm_xms_call_valid(const bx_ntvdm_xms_call *call);
int bx_ntvdm_xms_initialize(void);
int bx_ntvdm_xms_invoke(bx_ntvdm_xms_call *call);

USHORT bx_ntvdm_xms_get_ax(void); USHORT bx_ntvdm_xms_get_bx(void);
USHORT bx_ntvdm_xms_get_cx(void); USHORT bx_ntvdm_xms_get_dx(void);
USHORT bx_ntvdm_xms_get_bp(void); USHORT bx_ntvdm_xms_get_ss(void);
USHORT bx_ntvdm_xms_get_cs(void);
void bx_ntvdm_xms_set_ax(USHORT value); void bx_ntvdm_xms_set_bx(USHORT value);
void bx_ntvdm_xms_set_bl(USHORT value); void bx_ntvdm_xms_set_cx(USHORT value);
void bx_ntvdm_xms_set_dx(USHORT value); void bx_ntvdm_xms_set_cf(int value);
PVOID bx_ntvdm_xms_get_vdm_addr(USHORT segment, USHORT offset);
/* The historical source retained a host pointer returned by GetVDMAddr.
 * The modern boundary retains only a checked, fixed-width guest address. */
int bx_ntvdm_xms_bind_himem_a20_state(USHORT segment, USHORT offset);
void bx_ntvdm_xms_write_himem_a20_state(BYTE value);
void bx_ntvdm_xms_clear_himem_a20_state(void);
int bx_ntvdm_xms_copy_physical(uint32_t source, uint32_t destination,
    uint32_t bytes);
int bx_ntvdm_xms_move_block_from_guest(USHORT segment, USHORT offset);
WORD bx_ntvdm_xms_linear_to_segment(PVOID address);
int bx_ntvdm_xms_configure_memory_kib(ULONG kib);
void bx_ntvdm_xms_reset(void);
void bx_ntvdm_xms_a20_set(int enabled);
int bx_ntvdm_xms_a20_enabled(void);
int bx_ntvdm_xms_a20_available(void);
void bx_ntvdm_xms_update_kbd_int15(WORD segment, WORD offset);
void sas_enable_20_bit_wrapping(void);
void sas_disable_20_bit_wrapping(void);
BOOL sas_twenty_bit_wrapping_enabled(void);
void UpdateKbdInt15(WORD segment, WORD offset);

/* Original services and package initializer. */
BOOL XMSDispatch(ULONG service); BOOL XMSInit(int argc, char *argv[]);
VOID xmsA20(VOID); VOID xmsMoveBlock(VOID); VOID xmsAllocBlock(VOID);
VOID xmsFreeBlock(VOID); VOID xmsSysPageSize(VOID); VOID xmsQueryExtMem(VOID);
VOID xmsInitUMB(VOID); VOID xmsRequestUMB(VOID); VOID xmsReleaseUMB(VOID);
VOID xmsNotifyHookI15(VOID); VOID xmsQueryFreeExtMem(VOID);
VOID xmsReallocBlock(VOID); VOID xmsEnableA20Wrapping(VOID);
VOID xmsDisableA20Wrapping(VOID);
VOID xmsReleaseUMBNotify(PVOID address, DWORD bytes);
VOID xmsInsertUMB(PVOID address, ULONG bytes);

extern ULONG xmsMemorySize;
extern PVOID ExtMemSA;

/* The original suballocator remains the policy implementation.  These
 * declarations are deliberately source-shaped; its backing callbacks are
 * supplied by a later named mantle seam. */
typedef NTSTATUS (*bx_ntvdm_xms_commit_fn)(ULONG base, ULONG bytes);
typedef VOID (*bx_ntvdm_xms_move_fn)(ULONG destination, ULONG source, ULONG bytes);
typedef bx_ntvdm_xms_commit_fn PSACOMMITROUTINE;
typedef bx_ntvdm_xms_move_fn PSAMEMORYMOVEROUTINE;
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif
#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#endif
PVOID SAInitialize(ULONG base, ULONG bytes, bx_ntvdm_xms_commit_fn commit,
    bx_ntvdm_xms_commit_fn decommit, bx_ntvdm_xms_move_fn move);
BOOL SAAllocate(PVOID allocation, ULONG bytes, PULONG address);
BOOL SAFree(PVOID allocation, ULONG bytes, ULONG address);
BOOL SAReallocate(PVOID allocation, ULONG old_bytes, ULONG old_address,
    ULONG new_bytes, PULONG new_address);
BOOL SAQueryFree(PVOID allocation, PULONG total, PULONG largest);
NTSTATUS xmsCommitBlock(ULONG base, ULONG bytes);
NTSTATUS xmsDecommitBlock(ULONG base, ULONG bytes);
VOID xmsMoveMemory(ULONG destination, ULONG source, ULONG count);

BOOL ReserveUMB(ULONG owner, PVOID *address, PULONG bytes);

#define getAX() bx_ntvdm_xms_get_ax()
#define getBX() bx_ntvdm_xms_get_bx()
#define getCX() bx_ntvdm_xms_get_cx()
#define getDX() bx_ntvdm_xms_get_dx()
#define getBP() bx_ntvdm_xms_get_bp()
#define getSS() bx_ntvdm_xms_get_ss()
#define getCS() bx_ntvdm_xms_get_cs()
#define setAX(value) bx_ntvdm_xms_set_ax(value)
#define setBX(value) bx_ntvdm_xms_set_bx(value)
#define setBL(value) bx_ntvdm_xms_set_bl(value)
#define setCX(value) bx_ntvdm_xms_set_cx(value)
#define setDX(value) bx_ntvdm_xms_set_dx(value)
#define setCF(value) bx_ntvdm_xms_set_cf(value)
#define GetVDMAddr(segment, offset) bx_ntvdm_xms_get_vdm_addr(segment, offset)
#define DbgPrint(...) ((void)0)
#define ASSERT(value) ((void)0)

#endif
