#include "historical_bios_bridge_v1.h"

#include <stddef.h>

typedef void (*ntdos64_bios_entry)(void);

extern ntdos64_bios_entry BIOS[];
extern void c_cpu_init(void);
extern void c_cpu_terminate(void);
extern void sas_init(uint32_t size);
extern void sas_term(void);
extern uint8_t *ntdos64_ccpu_sm0_ram(void);
extern void setAX(uint16_t value);
extern void setCX(uint16_t value);
extern void setDX(uint16_t value);
extern void setCS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getAX(void);
extern uint16_t getCX(void);
extern uint16_t getDX(void);
extern uint16_t getCS(void);
extern uint16_t getIP(void);

static int ntdos64_historical_bios_bridge_v1_initialized;

static void ntdos64_historical_bios_bridge_v1_set_response(
    ntdos64_historical_bios_bridge_v1_response *response,
    ntdos64_historical_bios_bridge_v1_disposition disposition)
{
    response->abi_version = NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_ABI;
    response->struct_bytes = (uint32_t)sizeof(*response);
    response->disposition = (uint32_t)disposition;
    response->event_flags = 0u;
    response->ax = 0u;
    response->cx = 0u;
    response->dx = 0u;
    response->cs = 0u;
    response->ip = 0u;
    response->reserved = 0u;
}

int __cdecl ntdos64_historical_bios_bridge_v1_initialize(void)
{
    if (ntdos64_historical_bios_bridge_v1_initialized) {
        return 1;
    }

    sas_init(2u * 1024u * 1024u);
    c_cpu_init();
    if (ntdos64_ccpu_sm0_ram() == NULL || BIOS[0x50u] == NULL) {
        c_cpu_terminate();
        sas_term();
        return 0;
    }

    ntdos64_historical_bios_bridge_v1_initialized = 1;
    return 1;
}

void __cdecl ntdos64_historical_bios_bridge_v1_terminate(void)
{
    if (!ntdos64_historical_bios_bridge_v1_initialized) {
        return;
    }

    ntdos64_historical_bios_bridge_v1_initialized = 0;
    c_cpu_terminate();
    sas_term();
}

int __cdecl ntdos64_historical_bios_bridge_v1_invoke(
    const ntdos64_historical_bios_bridge_v1_request *request,
    ntdos64_historical_bios_bridge_v1_response *response)
{
    uint32_t service_address;
    uint8_t *ram;

    if (response == NULL) {
        return 0;
    }
    ntdos64_historical_bios_bridge_v1_set_response(
        response, NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_INVALID_RECORD);
    if (request == NULL ||
        request->abi_version != NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_ABI ||
        request->struct_bytes != sizeof(*request) ||
        request->reserved != 0u || request->reserved2 != 0u) {
        return 1;
    }
    if (!ntdos64_historical_bios_bridge_v1_initialized) {
        response->disposition = NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_NOT_INITIALIZED;
        return 1;
    }
    if (request->selector != 0x50u || request->service_byte != 0x14u) {
        response->disposition = NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_UNSUPPORTED;
        return 1;
    }

    service_address = ((uint32_t)request->cs << 4) + request->ip;
    if (service_address >= 2u * 1024u * 1024u) {
        return 1;
    }
    ram = ntdos64_ccpu_sm0_ram();
    if (ram == NULL) {
        response->disposition = NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_NOT_INITIALIZED;
        return 1;
    }

    setAX(request->ax);
    setCX(request->cx);
    setDX(request->dx);
    setCS(request->cs);
    setIP(request->ip);
    ram[service_address] = request->service_byte;
    BIOS[0x50u]();

    response->disposition = NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_COMPLETED;
    response->ax = getAX();
    response->cx = getCX();
    response->dx = getDX();
    response->cs = getCS();
    response->ip = getIP();
    return 1;
}
