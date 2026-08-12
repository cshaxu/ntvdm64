#include "bx_ntvdm_machine_bop_v1.h"
#include "bx_ntvdm_machine_composition_seam.h"

extern void unexpected_int(void);
extern void illegal_op_int(void);

#if defined(_MSC_VER)
#define BX_NTVDM_MACHINE_BOP_TLS __declspec(thread)
#else
#define BX_NTVDM_MACHINE_BOP_TLS _Thread_local
#endif

static BX_NTVDM_MACHINE_BOP_TLS const bx_ntvdm_machine_bop_v1_mechanics
    *bx_ntvdm_machine_bop_v1_active_mechanics;
static BX_NTVDM_MACHINE_BOP_TLS int bx_ntvdm_machine_bop_v1_failed;

void bx_ntvdm_machine_bop_v1_initialize_request(
    bx_ntvdm_machine_bop_v1_request *request)
{
    if (request == 0) return;
    request->magic = BX_NTVDM_MACHINE_BOP_V1_MAGIC;
    request->abi_version = BX_NTVDM_MACHINE_BOP_V1_VERSION;
    request->struct_bytes = sizeof(*request);
    request->classification = BX_NTVDM_MACHINE_BOP_V1_CLASS_NONE;
    request->selector = 0u;
    request->reserved[0] = request->reserved[1] = request->reserved[2] = 0u;
}

int bx_ntvdm_machine_bop_v1_request_valid(
    const bx_ntvdm_machine_bop_v1_request *request)
{
    return request != 0 && request->magic == BX_NTVDM_MACHINE_BOP_V1_MAGIC &&
        request->abi_version == BX_NTVDM_MACHINE_BOP_V1_VERSION &&
        request->struct_bytes == sizeof(*request) &&
        request->classification <= BX_NTVDM_MACHINE_BOP_V1_CLASS_UNAVAILABLE &&
        request->reserved[0] == 0u && request->reserved[1] == 0u &&
        request->reserved[2] == 0u;
}

void bx_ntvdm_machine_bop_v1_initialize_mechanics(
    bx_ntvdm_machine_bop_v1_mechanics *mechanics)
{
    if (mechanics == 0) return;
    mechanics->magic = BX_NTVDM_MACHINE_BOP_V1_MECHANICS_MAGIC;
    mechanics->abi_version = BX_NTVDM_MACHINE_BOP_V1_MECHANICS_VERSION;
    mechanics->struct_bytes = sizeof(*mechanics);
    mechanics->opaque = 0;
    mechanics->read8 = 0;
    mechanics->write8 = 0;
    mechanics->store8 = 0;
    mechanics->load8 = 0;
    mechanics->report = 0;
    mechanics->execution_mode = 0u;
    mechanics->esp = 0u;
    mechanics->ss = 0u;
    mechanics->reserved0 = 0u;
}

int bx_ntvdm_machine_bop_v1_mechanics_valid(
    const bx_ntvdm_machine_composition_mechanics *mechanics)
{
    return mechanics != 0 &&
        mechanics->magic == BX_NTVDM_MACHINE_BOP_V1_MECHANICS_MAGIC &&
        mechanics->abi_version == BX_NTVDM_MACHINE_BOP_V1_MECHANICS_VERSION &&
        mechanics->struct_bytes == sizeof(*mechanics) && mechanics->read8 != 0 &&
        mechanics->write8 != 0 && mechanics->store8 != 0 &&
        mechanics->reserved0 == 0u;
}

static int bx_ntvdm_machine_bop_v1_illegal_op_mechanics_valid(
    const bx_ntvdm_machine_bop_v1_mechanics *mechanics)
{
    return bx_ntvdm_machine_bop_v1_mechanics_valid(mechanics) &&
        mechanics->load8 != 0 && mechanics->report != 0 &&
        mechanics->execution_mode == 1u;
}

void inb(uint16_t port, uint8_t *value)
{
    if (value == 0 || bx_ntvdm_machine_bop_v1_active_mechanics == 0 ||
        !bx_ntvdm_machine_bop_v1_active_mechanics->read8(
            bx_ntvdm_machine_bop_v1_active_mechanics->opaque, port, value)) {
        if (value != 0) *value = 0xffu;
        bx_ntvdm_machine_bop_v1_failed = 1;
    }
}

void outb(uint16_t port, uint8_t value)
{
    if (bx_ntvdm_machine_bop_v1_active_mechanics == 0 ||
        !bx_ntvdm_machine_bop_v1_active_mechanics->write8(
            bx_ntvdm_machine_bop_v1_active_mechanics->opaque, port, value))
        bx_ntvdm_machine_bop_v1_failed = 1;
}

void c_sas_store(uint32_t address, uint8_t value)
{
    if (bx_ntvdm_machine_bop_v1_active_mechanics == 0 ||
        !bx_ntvdm_machine_bop_v1_active_mechanics->store8(
            bx_ntvdm_machine_bop_v1_active_mechanics->opaque, address, value))
        bx_ntvdm_machine_bop_v1_failed = 1;
}

uint32_t c_effective_addr(uint16_t segment, uint32_t offset)
{
    return ((uint32_t)segment << 4) + offset;
}

uint32_t c_getESP(void)
{
    if (bx_ntvdm_machine_bop_v1_active_mechanics == 0) {
        bx_ntvdm_machine_bop_v1_failed = 1;
        return 0u;
    }
    return bx_ntvdm_machine_bop_v1_active_mechanics->esp;
}

uint16_t c_getSS(void)
{
    if (bx_ntvdm_machine_bop_v1_active_mechanics == 0) {
        bx_ntvdm_machine_bop_v1_failed = 1;
        return 0u;
    }
    return bx_ntvdm_machine_bop_v1_active_mechanics->ss;
}

uint8_t c_sas_hw_at(uint32_t address)
{
    uint8_t value = 0u;
    if (bx_ntvdm_machine_bop_v1_active_mechanics == 0 ||
        !bx_ntvdm_machine_bop_v1_active_mechanics->load8(
            bx_ntvdm_machine_bop_v1_active_mechanics->opaque, address, &value))
        bx_ntvdm_machine_bop_v1_failed = 1;
    return value;
}

short host_error(int error_number, int options, char *message)
{
    if (bx_ntvdm_machine_bop_v1_active_mechanics == 0 ||
        bx_ntvdm_machine_bop_v1_active_mechanics->report == 0)
        bx_ntvdm_machine_bop_v1_failed = 1;
    else
        bx_ntvdm_machine_bop_v1_active_mechanics->report(
            bx_ntvdm_machine_bop_v1_active_mechanics->opaque,
            (uint32_t)error_number, (uint32_t)options, message);
    return 2; /* OpenNT host/src/nt_error.c returns ERR_CONT unconditionally. */
}

bx_ntvdm_machine_bop_v1_result bx_ntvdm_machine_bop_v1_execute_unexpected_int(
    const bx_ntvdm_machine_bop_v1_request *request,
    const bx_ntvdm_machine_bop_v1_mechanics *mechanics,
    bx_ntvdm_machine_bop_v1_handler handler)
{
    if (!bx_ntvdm_machine_bop_v1_request_valid(request) ||
        request->classification != BX_NTVDM_MACHINE_BOP_V1_CLASS_MACHINE ||
        request->selector != BX_NTVDM_MACHINE_BOP_V1_SELECTOR_UNEXPECTED_INT ||
        !bx_ntvdm_machine_bop_v1_mechanics_valid(mechanics) || handler == 0 ||
        bx_ntvdm_machine_bop_v1_active_mechanics != 0)
        return BX_NTVDM_MACHINE_BOP_V1_DECLINED;

    bx_ntvdm_machine_bop_v1_active_mechanics = mechanics;
    bx_ntvdm_machine_bop_v1_failed = 0;
    handler();
    bx_ntvdm_machine_bop_v1_active_mechanics = 0;
    return bx_ntvdm_machine_bop_v1_failed ?
        BX_NTVDM_MACHINE_BOP_V1_MECHANICS_FAILURE :
        BX_NTVDM_MACHINE_BOP_V1_HANDLED_RESUME;
}

bx_ntvdm_machine_bop_v1_result bx_ntvdm_machine_bop_v1_execute_illegal_op_int(
    const bx_ntvdm_machine_bop_v1_request *request,
    const bx_ntvdm_machine_bop_v1_mechanics *mechanics,
    bx_ntvdm_machine_bop_v1_handler handler)
{
    if (!bx_ntvdm_machine_bop_v1_request_valid(request) ||
        request->classification != BX_NTVDM_MACHINE_BOP_V1_CLASS_MACHINE ||
        request->selector != BX_NTVDM_MACHINE_BOP_V1_SELECTOR_ILLEGAL_OP_INT ||
        !bx_ntvdm_machine_bop_v1_illegal_op_mechanics_valid(mechanics) ||
        handler == 0 || bx_ntvdm_machine_bop_v1_active_mechanics != 0)
        return BX_NTVDM_MACHINE_BOP_V1_DECLINED;

    bx_ntvdm_machine_bop_v1_active_mechanics = mechanics;
    bx_ntvdm_machine_bop_v1_failed = 0;
    handler();
    bx_ntvdm_machine_bop_v1_active_mechanics = 0;
    return bx_ntvdm_machine_bop_v1_failed ?
        BX_NTVDM_MACHINE_BOP_V1_MECHANICS_FAILURE :
        BX_NTVDM_MACHINE_BOP_V1_HANDLED_RESUME;
}

void bx_ntvdm_machine_composition_initialize_mechanics(
    bx_ntvdm_machine_composition_mechanics *mechanics)
{
    bx_ntvdm_machine_bop_v1_initialize_mechanics(mechanics);
}

bx_ntvdm_machine_bop_v1_result bx_ntvdm_machine_composition_v1_probe(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_state,
    const bx_ntvdm_instruction_window_v1 *instruction_window,
    const bx_ntvdm_machine_bop_v1_mechanics *mechanics)
{
    bx_ntvdm_machine_bop_v1_request request;
    if (event == 0 || cpu_state == 0 || instruction_window == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_state) ||
        !bx_ntvdm_instruction_window_v1_valid(instruction_window) ||
        event->vector != 6u || instruction_window->valid_bytes < 3u ||
        instruction_window->bytes[0] != 0xc4u ||
        instruction_window->bytes[1] != 0xc4u ||
        !bx_ntvdm_machine_bop_v1_mechanics_valid(mechanics))
        return BX_NTVDM_MACHINE_BOP_V1_DECLINED;
    bx_ntvdm_machine_bop_v1_initialize_request(&request);
    request.classification = BX_NTVDM_MACHINE_BOP_V1_CLASS_MACHINE;
    request.selector = instruction_window->bytes[2];
    if (request.selector == BX_NTVDM_MACHINE_BOP_V1_SELECTOR_UNEXPECTED_INT)
        return bx_ntvdm_machine_bop_v1_execute_unexpected_int(&request,
            mechanics, unexpected_int);
    if (request.selector == BX_NTVDM_MACHINE_BOP_V1_SELECTOR_ILLEGAL_OP_INT)
        return bx_ntvdm_machine_bop_v1_execute_illegal_op_int(&request,
            mechanics, illegal_op_int);
    return BX_NTVDM_MACHINE_BOP_V1_DECLINED;
}
