#include "historical_bios_bridge_v2.h"

#include <string.h>

typedef void (*runner_bios_entry)(void);

extern runner_bios_entry BIOS[];
extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern void c_setAX(uint16_t value);
extern void c_setCX(uint16_t value);
extern void c_setDX(uint16_t value);
extern void c_setCS(uint16_t value);
extern void c_setIP(uint16_t value);
extern uint16_t c_getAX(void);
extern uint16_t c_getCX(void);
extern uint16_t c_getDX(void);
extern uint16_t c_getCS(void);
extern uint16_t c_getIP(void);

static int runner_historical_bios_bridge_v2_initialized;

static void runner_historical_bios_bridge_v2_set_response(
    runner_historical_transport_v2_response *response,
    runner_historical_transport_v2_disposition disposition)
{
    memset(response, 0, sizeof(*response));
    response->magic = RUNNER_HISTORICAL_TRANSPORT_V2_MAGIC;
    response->abi_version = RUNNER_HISTORICAL_TRANSPORT_V2_ABI;
    response->struct_bytes = (uint32_t)sizeof(*response);
    response->disposition = (uint32_t)disposition;
}

static void runner_historical_bios_bridge_v2_set_state(
    const runner_historical_transport_v2_state *state)
{
    c_setAX((uint16_t)state->eax);
    c_setCX((uint16_t)state->ecx);
    c_setDX((uint16_t)state->edx);
    c_setCS(state->cs);
    c_setIP(state->ip);
}

static void runner_historical_bios_bridge_v2_get_state(
    runner_historical_transport_v2_state *state,
    const runner_historical_transport_v2_state *request_state)
{
    *state = *request_state;
    state->eax = (state->eax & 0xffff0000u) | c_getAX();
    state->ecx = (state->ecx & 0xffff0000u) | c_getCX();
    state->edx = (state->edx & 0xffff0000u) | c_getDX();
    state->cs = c_getCS();
    state->ip = c_getIP();
}

int __cdecl runner_historical_bios_bridge_v2_initialize(void)
{
    if (runner_historical_bios_bridge_v2_initialized) return 1;
    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    if (runner_ccpu_sm0_ram() == NULL || BIOS[0x50u] == NULL) {
        c_cpu_terminate();
        sas_term();
        return 0;
    }
    runner_historical_bios_bridge_v2_initialized = 1;
    return 1;
}

void __cdecl runner_historical_bios_bridge_v2_terminate(void)
{
    if (!runner_historical_bios_bridge_v2_initialized) return;
    runner_historical_bios_bridge_v2_initialized = 0;
    c_cpu_terminate();
    sas_term();
}

int __cdecl runner_historical_bios_bridge_v2_invoke(
    const runner_historical_transport_v2_request *request,
    runner_historical_transport_v2_response *response,
    uint32_t expected_session_token)
{
    uint32_t service_address;
    uint8_t *ram;

    if (response == NULL) return 0;
    runner_historical_bios_bridge_v2_set_response(
        response, RUNNER_HISTORICAL_TRANSPORT_V2_INVALID_RECORD);
    if (request == NULL || request->magic != RUNNER_HISTORICAL_TRANSPORT_V2_MAGIC ||
        request->abi_version != RUNNER_HISTORICAL_TRANSPORT_V2_ABI ||
        request->struct_bytes != sizeof(*request) || request->reserved != 0u) {
        return 1;
    }
    response->session_token = request->session_token;
    response->sequence = request->sequence;
    if (request->session_token != expected_session_token) {
        response->disposition = RUNNER_HISTORICAL_TRANSPORT_V2_SESSION_ERROR;
        return 1;
    }
    if (!runner_historical_bios_bridge_v2_initialized) {
        response->disposition = RUNNER_HISTORICAL_TRANSPORT_V2_NOT_INITIALIZED;
        return 1;
    }
    if (request->selector != 0x50u || request->service_byte != 0x14u) {
        response->disposition = RUNNER_HISTORICAL_TRANSPORT_V2_UNSUPPORTED;
        return 1;
    }
    service_address = ((uint32_t)request->state.cs << 4) + request->state.ip;
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL || service_address >= 2u * 1024u * 1024u) {
        response->disposition = RUNNER_HISTORICAL_TRANSPORT_V2_HISTORICAL_FAILURE;
        return 1;
    }
    runner_historical_bios_bridge_v2_set_state(&request->state);
    ram[service_address] = request->service_byte;
    BIOS[0x50u]();
    runner_historical_bios_bridge_v2_get_state(&response->state, &request->state);
    response->disposition = RUNNER_HISTORICAL_TRANSPORT_V2_COMPLETED;
    return 1;
}
