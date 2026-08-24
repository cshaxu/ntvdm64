#ifndef RUNTIME_OPENNT_XMS_SOFTPC_FACADE_H
#define RUNTIME_OPENNT_XMS_SOFTPC_FACADE_H

/*
 * Same-shaped SoftPC/CCPU/SAS boundary used by the imported OpenNT XMS
 * package.  This component owns only bounded machine-facing operations.  It
 * neither identifies a #UD/BOP selector nor dispatches an XMS service.
 */

#include <windows.h>
#include <stdint.h>

#include "adapter-softpc/cpu_result.h"
#include "adapter-softpc/cpu_state_abi.h"

typedef int (*runtime_xms_guest_read_fn)(void *state, uint32_t address,
    uint8_t *buffer, uint32_t bytes);
typedef int (*runtime_xms_guest_write_fn)(void *state, uint32_t address,
    const uint8_t *buffer, uint32_t bytes);

typedef struct runtime_xms_softpc_context {
    const runtime_cpu_state_v1 *cpu;
    runtime_cpu_result_v2 *result;
    void *guest_state;
    runtime_xms_guest_read_fn guest_read;
    runtime_xms_guest_write_fn guest_write;
} runtime_xms_softpc_context;

int runtime_xms_softpc_context_valid(const runtime_xms_softpc_context *context);
int runtime_xms_softpc_context_begin(const runtime_xms_softpc_context *context);
void runtime_xms_softpc_context_end(void);

USHORT runtime_xms_get_ax(void); USHORT runtime_xms_get_bx(void);
USHORT runtime_xms_get_cx(void); USHORT runtime_xms_get_dx(void);
USHORT runtime_xms_get_bp(void); USHORT runtime_xms_get_ss(void);
USHORT runtime_xms_get_cs(void);
void runtime_xms_set_ax(USHORT value); void runtime_xms_set_bx(USHORT value);
void runtime_xms_set_bl(USHORT value); void runtime_xms_set_cx(USHORT value);
void runtime_xms_set_dx(USHORT value); void runtime_xms_set_cf(int value);

PVOID runtime_xms_get_vdm_addr(USHORT segment, USHORT offset);
int runtime_xms_bind_himem_a20_state(USHORT segment, USHORT offset);
void runtime_xms_write_himem_a20_state(BYTE value);
void runtime_xms_clear_himem_a20_state(void);
int runtime_xms_copy_physical(uint32_t source, uint32_t destination,
    uint32_t bytes);
int runtime_xms_move_block_from_guest(USHORT segment, USHORT offset);
WORD runtime_xms_linear_to_segment(PVOID address);
void runtime_xms_a20_set(int enabled);
int runtime_xms_a20_enabled(void);
int runtime_xms_a20_available(void);
void runtime_xms_softpc_reset(void);

void sas_enable_20_bit_wrapping(void);
void sas_disable_20_bit_wrapping(void);
BOOL sas_twenty_bit_wrapping_enabled(void);

typedef NTSTATUS (*runtime_xms_commit_fn)(ULONG base, ULONG bytes);
typedef VOID (*runtime_xms_move_fn)(ULONG destination, ULONG source, ULONG bytes);
typedef runtime_xms_commit_fn PSACOMMITROUTINE;
typedef runtime_xms_move_fn PSAMEMORYMOVEROUTINE;

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif
#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#endif

NTSTATUS xmsCommitBlock(ULONG base, ULONG bytes);
NTSTATUS xmsDecommitBlock(ULONG base, ULONG bytes);
VOID xmsMoveMemory(ULONG destination, ULONG source, ULONG count);
BOOL ReserveUMB(ULONG owner, PVOID *address, PULONG bytes);

#endif
