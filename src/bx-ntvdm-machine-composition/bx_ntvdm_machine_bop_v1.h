#ifndef BX_NTVDM_MACHINE_BOP_V1_H
#define BX_NTVDM_MACHINE_BOP_V1_H

#include <stdint.h>

#define BX_NTVDM_MACHINE_BOP_V1_MAGIC 0x424D4331u
#define BX_NTVDM_MACHINE_BOP_V1_VERSION 1u
#define BX_NTVDM_MACHINE_BOP_V1_SELECTOR_UNEXPECTED_INT 0x02u
#define BX_NTVDM_MACHINE_BOP_V1_SELECTOR_ILLEGAL_OP_INT 0x06u

/* This is an already-classified request, not an instruction decoder or a
 * replacement for the historical BIOS[] table. */
typedef enum bx_ntvdm_machine_bop_v1_classification {
    BX_NTVDM_MACHINE_BOP_V1_CLASS_NONE = 0u,
    BX_NTVDM_MACHINE_BOP_V1_CLASS_MACHINE = 1u,
    BX_NTVDM_MACHINE_BOP_V1_CLASS_HOST_SERVICE = 2u,
    BX_NTVDM_MACHINE_BOP_V1_CLASS_UNAVAILABLE = 3u
} bx_ntvdm_machine_bop_v1_classification;

typedef enum bx_ntvdm_machine_bop_v1_result {
    BX_NTVDM_MACHINE_BOP_V1_DECLINED = 0u,
    BX_NTVDM_MACHINE_BOP_V1_HANDLED_RESUME = 1u,
    BX_NTVDM_MACHINE_BOP_V1_MECHANICS_FAILURE = 2u
} bx_ntvdm_machine_bop_v1_result;

typedef struct bx_ntvdm_machine_bop_v1_request {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t classification;
    uint8_t selector;
    uint8_t reserved[3];
} bx_ntvdm_machine_bop_v1_request;

/* The context is a synchronous, same-native-island capability. It never
 * crosses the adapter ABI, cannot be retained, and does not model a device. */
typedef int (*bx_ntvdm_machine_bop_v1_read8)(void *opaque, uint16_t port,
    uint8_t *value);
typedef int (*bx_ntvdm_machine_bop_v1_write8)(void *opaque, uint16_t port,
    uint8_t value);
typedef int (*bx_ntvdm_machine_bop_v1_store8)(void *opaque, uint32_t address,
    uint8_t value);
typedef int (*bx_ntvdm_machine_bop_v1_load8)(void *opaque, uint32_t address,
    uint8_t *value);
typedef void (*bx_ntvdm_machine_bop_v1_report)(void *opaque,
    uint32_t error_number, uint32_t options, const char *message);

typedef struct bx_ntvdm_machine_bop_v1_mechanics {
    uint32_t magic, abi_version, struct_bytes;
    void *opaque;
    bx_ntvdm_machine_bop_v1_read8 read8;
    bx_ntvdm_machine_bop_v1_write8 write8;
    bx_ntvdm_machine_bop_v1_store8 store8;
    bx_ntvdm_machine_bop_v1_load8 load8;
    bx_ntvdm_machine_bop_v1_report report;
    uint32_t execution_mode;
    uint32_t esp;
    uint16_t ss;
    uint16_t reserved0;
} bx_ntvdm_machine_bop_v1_mechanics;

#define BX_NTVDM_MACHINE_BOP_V1_MECHANICS_MAGIC 0x424D4D31u
#define BX_NTVDM_MACHINE_BOP_V1_MECHANICS_VERSION 1u

typedef void (*bx_ntvdm_machine_bop_v1_handler)(void);

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_machine_bop_v1_initialize_request(
    bx_ntvdm_machine_bop_v1_request *request);
int bx_ntvdm_machine_bop_v1_request_valid(
    const bx_ntvdm_machine_bop_v1_request *request);
void bx_ntvdm_machine_bop_v1_initialize_mechanics(
    bx_ntvdm_machine_bop_v1_mechanics *mechanics);
int bx_ntvdm_machine_bop_v1_mechanics_valid(
    const bx_ntvdm_machine_bop_v1_mechanics *mechanics);

/* The handler is expected to be the separately compiled original
 * unexpected_int object. No OpenNT source is included or modified here. */
bx_ntvdm_machine_bop_v1_result bx_ntvdm_machine_bop_v1_execute_unexpected_int(
    const bx_ntvdm_machine_bop_v1_request *request,
    const bx_ntvdm_machine_bop_v1_mechanics *mechanics,
    bx_ntvdm_machine_bop_v1_handler handler);

/* The handler is the unchanged separately compiled original illegal_op_int
 * object. Its legacy imports remain usable only in this synchronous scope. */
bx_ntvdm_machine_bop_v1_result bx_ntvdm_machine_bop_v1_execute_illegal_op_int(
    const bx_ntvdm_machine_bop_v1_request *request,
    const bx_ntvdm_machine_bop_v1_mechanics *mechanics,
    bx_ntvdm_machine_bop_v1_handler handler);

/* C-linkage compatibility symbols used by the original OpenNT object. They
 * are valid only during execute_unexpected_int's synchronous dynamic scope. */
void inb(uint16_t port, uint8_t *value);
void outb(uint16_t port, uint8_t value);
void c_sas_store(uint32_t address, uint8_t value);
uint32_t c_effective_addr(uint16_t segment, uint32_t offset);
uint32_t c_getESP(void);
uint16_t c_getSS(void);
uint8_t c_sas_hw_at(uint32_t address);
short host_error(int error_number, int options, char *message);

#ifdef __cplusplus
}
#endif

#endif
