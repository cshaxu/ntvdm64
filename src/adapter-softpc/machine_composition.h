#ifndef RUNTIME_MACHINE_COMPOSITION_V2_H
#define RUNTIME_MACHINE_COMPOSITION_V2_H

#include <stdint.h>

#include "cpu_result.h"
#include "cpu_state_abi.h"
#include "exception_abi.h"
#include "instruction_window_abi.h"

#define RUNTIME_MACHINE_MECHANICS_V1_MAGIC 0x424D4D32u
#define RUNTIME_MACHINE_MECHANICS_V1_VERSION 1u

/* A synchronous native capability supplied by the engine.  It is opaque to
 * the adapter, valid only for one call, and neither selects a provider nor
 * crosses into a persisted guest/host ABI. */
typedef int (*runtime_machine_mechanics_v1_read8)(void *opaque,
    uint16_t port, uint8_t *value);
typedef int (*runtime_machine_mechanics_v1_write8)(void *opaque,
    uint16_t port, uint8_t value);
typedef int (*runtime_machine_mechanics_v1_store8)(void *opaque,
    uint32_t address, uint8_t value);
typedef int (*runtime_machine_mechanics_v1_load8)(void *opaque,
    uint32_t address, uint8_t *value);
typedef void (*runtime_machine_mechanics_v1_report)(void *opaque,
    uint32_t error_number, uint32_t options, const char *message);

typedef struct runtime_machine_mechanics_v1 {
    uint32_t magic, abi_version, struct_bytes;
    void *opaque;
    runtime_machine_mechanics_v1_read8 read8;
    runtime_machine_mechanics_v1_write8 write8;
    runtime_machine_mechanics_v1_store8 store8;
    runtime_machine_mechanics_v1_load8 load8;
    runtime_machine_mechanics_v1_report report;
    uint32_t execution_mode;
    uint32_t esp;
    uint16_t ss;
    uint16_t reserved0;
} runtime_machine_mechanics_v1;

#ifdef __cplusplus
extern "C" {
#endif

void runtime_machine_mechanics_v1_initialize(
    runtime_machine_mechanics_v1 *mechanics);
int runtime_machine_mechanics_v1_valid(
    const runtime_machine_mechanics_v1 *mechanics);

/* Adapter-owned classification and original-handler invocation.  It returns
 * a generic result only: pass-through when this plane declines, resume when a
 * selected original handler completes, and controlled stop on no admitted
 * typed recovery. */
int runtime_machine_composition_v2_dispatch(
    const runtime_exception_event_v1 *event,
    const runtime_cpu_state_v1 *cpu_before,
    const runtime_instruction_window_v1 *window,
    const runtime_machine_mechanics_v1 *mechanics,
    runtime_cpu_result_v2 *result);

/* Compatibility imports consumed only by the separately compiled original
 * handler islands during the synchronous mechanics transaction. */
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
