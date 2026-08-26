#ifndef APP_OPENNT_BOP_XMS_COMPAT_H
#define APP_OPENNT_BOP_XMS_COMPAT_H

/* Compatibility surface for the directly mirrored OpenNT XMS sources.
 * DIVERGENCE(BOP-DIV-024..031): private NT4 product headers are unavailable;
 * reached SoftPC/CCPU/SAS calls retain their source-shaped spelling through
 * adapter-softpc.  BOP route, dispatch and package lifecycle remain here. */

#include <windows.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "adapter-softpc/exception_abi.h"
#include "adapter-softpc/opennt_xms_softpc_facade.h"

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

typedef struct runtime_xms_call {
    uint32_t magic, abi_version, struct_bytes, service;
    const runtime_exception_event *boundary;
    const runtime_cpu_state *cpu;
    runtime_cpu_result *result;
    void *guest_state;
    runtime_xms_guest_read_fn guest_read;
    runtime_xms_guest_write_fn guest_write;
} runtime_xms_call;

#define RUNTIME_XMS_CALL_MAGIC 0x4258584du
#define RUNTIME_XMS_CALL_VERSION 1u

int runtime_xms_call_valid(const runtime_xms_call *call);
int runtime_xms_initialize(void);
int runtime_xms_invoke(runtime_xms_call *call);

int runtime_xms_configure_memory_kib(ULONG kib);
void runtime_xms_reset(void);
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

/* The original suballocator remains the policy implementation. */
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif
#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#endif
PVOID SAInitialize(ULONG base, ULONG bytes, runtime_xms_commit_fn commit,
    runtime_xms_commit_fn decommit, runtime_xms_move_fn move);
BOOL SAAllocate(PVOID allocation, ULONG bytes, PULONG address);
BOOL SAFree(PVOID allocation, ULONG bytes, ULONG address);
BOOL SAReallocate(PVOID allocation, ULONG old_bytes, ULONG old_address,
    ULONG new_bytes, PULONG new_address);
BOOL SAQueryFree(PVOID allocation, PULONG total, PULONG largest);
#define getAX() runtime_xms_get_ax()
#define getBX() runtime_xms_get_bx()
#define getCX() runtime_xms_get_cx()
#define getDX() runtime_xms_get_dx()
#define getBP() runtime_xms_get_bp()
#define getSS() runtime_xms_get_ss()
#define getCS() runtime_xms_get_cs()
#define setAX(value) runtime_xms_set_ax(value)
#define setBX(value) runtime_xms_set_bx(value)
#define setBL(value) runtime_xms_set_bl(value)
#define setCX(value) runtime_xms_set_cx(value)
#define setDX(value) runtime_xms_set_dx(value)
#define setCF(value) runtime_xms_set_cf(value)
#define GetVDMAddr(segment, offset) runtime_xms_get_vdm_addr(segment, offset)
#define DbgPrint(...) ((void)0)
#define ASSERT(value) ((void)0)

#endif
