#ifndef BX_NTVDM_OPENNT_DEM_CCPU_SAS_FACADE_H
#define BX_NTVDM_OPENNT_DEM_CCPU_SAS_FACADE_H

/*
 * Compatibility boundary for the directly imported OpenNT file
 * src/opennt/base/mvdm/dos/dem/demhndl.c.
 *
 * This header intentionally exposes the historical helper spellings used by
 * that file.  Its implementation is a per-call adapter context; it is not a
 * provider, dispatcher, or legacy-v1 compatibility layer.
 */

#include <windows.h>
#include <stdint.h>

#include "bx_ntvdm_dem_direct_context.h"
#include "adapter-softpc/bx_ntvdm_cpu_result_v2.h"
#include "adapter-softpc/bx_ntvdm_cpu_state_abi.h"
#include "adapter-softpc/bx_ntvdm_exception_abi.h"
#include "opennt-host/vdmredir/vrnmpipe_compat.h"

typedef uint16_t USHORT;
typedef uint16_t WORD;
typedef uint8_t UCHAR;
typedef char CHAR;
typedef void *LPVOID;
typedef uint8_t *PBYTE;
typedef WORD *LPWORD;

typedef struct bx_ntvdm_demhndl_extended_error {
    WORD ExtendedError;
    UCHAR ExtendedErrorClass;
    UCHAR ExtendedErrorAction;
    UCHAR ExtendedErrorLocus;
} bx_ntvdm_demhndl_extended_error;

extern __declspec(thread) bx_ntvdm_demhndl_extended_error *pExtendedError;

enum bx_ntvdm_demhndl_service {
    BX_NTVDM_DEMHNDL_CHG_FILE_PTR = 0x00u,
    BX_NTVDM_DEMHNDL_CLOSE = 0x02u,
    BX_NTVDM_DEMHNDL_FILE_TIMES = 0x08u,
    BX_NTVDM_DEMHNDL_READ = 0x16u,
    BX_NTVDM_DEMHNDL_WRITE = 0x1eu,
    BX_NTVDM_DEMHNDL_COMMIT = 0x27u,
    BX_NTVDM_DEMHNDL_PIPE_DATA_EOF = 0x47u,
    BX_NTVDM_DEMHNDL_PIPE_EOF = 0x48u
};

/* The raw OpenNT service body executes against one scoped call context.
 * These callbacks are the modern equivalent of the CCPU/SAS boundary: they
 * transport a bounded guest buffer and an opaque session handle, but do not
 * add DEM policy or DOS behavior. */
typedef int (*bx_ntvdm_demhndl_guest_read_fn)(void *state,
    uint32_t physical_address, uint8_t *buffer, uint32_t bytes);
typedef int (*bx_ntvdm_demhndl_guest_write_fn)(void *state,
    uint32_t physical_address, const uint8_t *buffer, uint32_t bytes);
typedef int (*bx_ntvdm_demhndl_pipe_data_eof_fn)(void *state, HANDLE file,
    BOOL *eof_out);
typedef int (*bx_ntvdm_demhndl_pipe_eof_fn)(void *state, HANDLE file);
typedef struct bx_ntvdm_demhndl_guest_span {
    USHORT segment, offset;
    uint32_t bytes, write_back;
} bx_ntvdm_demhndl_guest_span;

typedef struct bx_ntvdm_demhndl_call {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t service;
    bx_ntvdm_dem_direct_context *direct;
    const bx_ntvdm_exception_event_v1 *boundary;
    const bx_ntvdm_cpu_state_v1 *cpu;
    bx_ntvdm_cpu_result_v2 *result;
    void *guest_state;
    bx_ntvdm_demhndl_guest_read_fn guest_read;
    bx_ntvdm_demhndl_guest_write_fn guest_write;
    const bx_ntvdm_demhndl_guest_span *guest_spans;
    uint32_t guest_span_count;
    void *pipe_state;
    bx_ntvdm_demhndl_pipe_data_eof_fn pipe_data_eof;
    bx_ntvdm_demhndl_pipe_eof_fn pipe_eof;
} bx_ntvdm_demhndl_call;

#define BX_NTVDM_DEMHNDL_CALL_MAGIC 0x42584448u
#define BX_NTVDM_DEMHNDL_CALL_VERSION 1u

int bx_ntvdm_demhndl_call_valid(const bx_ntvdm_demhndl_call *call);
int bx_ntvdm_demhndl_invoke(bx_ntvdm_demhndl_call *call);
/* `demdisp.c` leaves 50:42 as its historical missing worker, although the
 * original x86 DOS caller selects it and consumes the same read ABI as
 * demRead.  This narrowly named composition seam retains that caller and
 * invokes the imported demRead body; it is not a dispatcher-table edit or a
 * replacement file-I/O algorithm. */
int bx_ntvdm_demhndl_invoke_fast_read(bx_ntvdm_demhndl_call *call);
/* A second directly imported DEM owner may execute through the same scoped
 * CCPU/SAS replacement.  This remains a mechanical call boundary: the caller
 * owns service selection and the imported function owns its DEM semantics. */
int bx_ntvdm_demhndl_invoke_body(bx_ntvdm_demhndl_call *call,
    void (*body)(void));
int bx_ntvdm_demhndl_invoke_body_with_resume(bx_ntvdm_demhndl_call *call,
    void (*body)(void), uint32_t resume_bytes);

#define errLOC_Net 2u
#define errCLASS_TempSit 0x0bu
#define errACT_Retry 1u
#define STOREWORD(member, value) ((member) = (WORD)(value))

USHORT bx_ntvdm_demhndl_get_ax(void);
USHORT bx_ntvdm_demhndl_get_bx(void);
USHORT bx_ntvdm_demhndl_get_cx(void);
USHORT bx_ntvdm_demhndl_get_dx(void);
USHORT bx_ntvdm_demhndl_get_si(void);
USHORT bx_ntvdm_demhndl_get_di(void);
USHORT bx_ntvdm_demhndl_get_bp(void);
USHORT bx_ntvdm_demhndl_get_ds(void);
USHORT bx_ntvdm_demhndl_get_es(void);
USHORT bx_ntvdm_demhndl_get_cs(void);
USHORT bx_ntvdm_demhndl_get_ip(void);
USHORT bx_ntvdm_demhndl_get_al(void);
USHORT bx_ntvdm_demhndl_get_cl(void);
USHORT bx_ntvdm_demhndl_get_ch(void);
USHORT bx_ntvdm_demhndl_get_bl(void);
USHORT bx_ntvdm_demhndl_get_dh(void);
USHORT bx_ntvdm_demhndl_get_dl(void);
int bx_ntvdm_demhndl_get_zf(void);
int bx_ntvdm_demhndl_get_cf(void);
void bx_ntvdm_demhndl_set_ax(USHORT value);
void bx_ntvdm_demhndl_set_al(USHORT value);
void bx_ntvdm_demhndl_set_bx(USHORT value);
void bx_ntvdm_demhndl_set_bl(USHORT value);
void bx_ntvdm_demhndl_set_bp(USHORT value);
void bx_ntvdm_demhndl_set_ds(USHORT value);
void bx_ntvdm_demhndl_set_es(USHORT value);
/* These two accessors retain the historical CCPU helper shape for callers
 * that stage a real-mode BIOS continuation.  They update only the scoped
 * source-call copy: ordinary BOP resumption still owns its own RIP contract. */
void bx_ntvdm_demhndl_set_cs(USHORT value);
void bx_ntvdm_demhndl_set_ip(USHORT value);
void bx_ntvdm_demhndl_set_si(USHORT value);
void bx_ntvdm_demhndl_set_di(USHORT value);
void bx_ntvdm_demhndl_set_cx(USHORT value);
void bx_ntvdm_demhndl_set_cl(USHORT value);
void bx_ntvdm_demhndl_set_ch(USHORT value);
void bx_ntvdm_demhndl_set_dx(USHORT value);
void bx_ntvdm_demhndl_set_dl(USHORT value);
void bx_ntvdm_demhndl_set_dh(USHORT value);
USHORT bx_ntvdm_demhndl_get_ah(void);
void bx_ntvdm_demhndl_set_ah(USHORT value);
void bx_ntvdm_demhndl_set_cf(int value);
void bx_ntvdm_demhndl_set_zf(int value);
HANDLE bx_ntvdm_demhndl_get_handle(USHORT high, USHORT low);
LPVOID bx_ntvdm_demhndl_get_vdm_addr(USHORT segment, USHORT offset);
int bx_ntvdm_demhndl_copy_guest(USHORT segment, USHORT offset, void *buffer,
    uint32_t bytes);
int bx_ntvdm_demhndl_copy_guest_oem_string(USHORT segment, USHORT offset,
    CHAR *buffer, uint32_t capacity);
/* A directly imported owner that retains a historical guest address across
 * calls may use these bounded context operations.  They remain checked RAM
 * transport, not a DOS or BOP dispatcher. */
uint32_t bx_ntvdm_demhndl_current_service(void);
int bx_ntvdm_demhndl_write_guest(USHORT segment, USHORT offset,
    const void *buffer, uint32_t bytes);
/* demmisc.c needs only these non-return/loader mechanics formerly provided
 * by the NTVDM product shell and SAS.  They neither classify BOPs nor own
 * the OpenNT loader algorithm. */
void bx_ntvdm_demhndl_terminate(void);
int bx_ntvdm_demhndl_loader_write(const void *buffer, uint32_t bytes);
/* Optional fixed-layout completion hook implemented by the demerror shim.
 * It is a no-op unless that original owner has retained VHE state. */
/* Selector-blind completion hook for a source body that retains a bounded
 * guest layout across the call.  It is installed by BOP composition at
 * runtime; adapter-softpc never names a DEM service or provider. */
typedef void (*bx_ntvdm_demhndl_post_body_hook)(void);
void bx_ntvdm_demhndl_set_post_body_hook(bx_ntvdm_demhndl_post_body_hook hook);
void bx_ntvdm_demhndl_flush_vdm_pointer(ULONG far_pointer, USHORT bytes,
    PBYTE pointer, BOOL write_back);
void bx_ntvdm_demhndl_free_vdm_pointer(ULONG far_pointer, USHORT bytes,
    PBYTE pointer, BOOL write_back);
/* The original demerror.c owns this failure algorithm, including the saved
 * retry register image.  A bound 50:32 VHE is its historical prerequisite. */
void demClientError(HANDLE file, CHAR drive);
BOOL bx_ntvdm_demhndl_close_handle(HANDLE file);
BOOL bx_ntvdm_demhndl_publish_handle(HANDLE file);
BOOL bx_ntvdm_demhndl_publish_handle_token(HANDLE file, uint32_t *token_out);

#define getAX() bx_ntvdm_demhndl_get_ax()
#define getBX() bx_ntvdm_demhndl_get_bx()
#define getCX() bx_ntvdm_demhndl_get_cx()
#define getDX() bx_ntvdm_demhndl_get_dx()
#define getSI() bx_ntvdm_demhndl_get_si()
#define getDI() bx_ntvdm_demhndl_get_di()
#define getBP() bx_ntvdm_demhndl_get_bp()
#define getDS() bx_ntvdm_demhndl_get_ds()
#define getES() bx_ntvdm_demhndl_get_es()
#define getCS() bx_ntvdm_demhndl_get_cs()
#define getIP() bx_ntvdm_demhndl_get_ip()
#define getAL() bx_ntvdm_demhndl_get_al()
#define getCL() bx_ntvdm_demhndl_get_cl()
#define getCH() bx_ntvdm_demhndl_get_ch()
#define getBL() bx_ntvdm_demhndl_get_bl()
#define getDH() bx_ntvdm_demhndl_get_dh()
#define getDL() bx_ntvdm_demhndl_get_dl()
#define getZF() bx_ntvdm_demhndl_get_zf()
#define getCF() bx_ntvdm_demhndl_get_cf()
#define setAX(value) bx_ntvdm_demhndl_set_ax(value)
#define setAL(value) bx_ntvdm_demhndl_set_al(value)
#define setBX(value) bx_ntvdm_demhndl_set_bx(value)
#define setBL(value) bx_ntvdm_demhndl_set_bl(value)
#define setBP(value) bx_ntvdm_demhndl_set_bp(value)
#define setSI(value) bx_ntvdm_demhndl_set_si(value)
#define setDI(value) bx_ntvdm_demhndl_set_di(value)
#define setCX(value) bx_ntvdm_demhndl_set_cx(value)
#define setCL(value) bx_ntvdm_demhndl_set_cl(value)
#define setCH(value) bx_ntvdm_demhndl_set_ch(value)
#define setDX(value) bx_ntvdm_demhndl_set_dx(value)
#define setDL(value) bx_ntvdm_demhndl_set_dl(value)
#define setDH(value) bx_ntvdm_demhndl_set_dh(value)
#define setCF(value) bx_ntvdm_demhndl_set_cf(value)
#define setZF(value) bx_ntvdm_demhndl_set_zf(value)
#define setCS(value) bx_ntvdm_demhndl_set_cs(value)
#define setIP(value) bx_ntvdm_demhndl_set_ip(value)
#define GETHANDLE(high, low) bx_ntvdm_demhndl_get_handle(high, low)
#define GetVDMAddr(segment, offset) bx_ntvdm_demhndl_get_vdm_addr(segment, offset)
#define Sim32FlushVDMPointer(far_pointer, bytes, pointer, write_back) \
    bx_ntvdm_demhndl_flush_vdm_pointer(far_pointer, bytes, pointer, write_back)
#define Sim32FreeVDMPointer(far_pointer, bytes, pointer, write_back) \
    bx_ntvdm_demhndl_free_vdm_pointer(far_pointer, bytes, pointer, write_back)

int IsVdmRedirLoaded(void);
int cmdPipeFileDataEOF(HANDLE file, BOOL *eof_out);
int cmdPipeFileEOF(HANDLE file);
void demPrintMsg(ULONG message);

#define MSG_TIMEDATE 6u

#endif
