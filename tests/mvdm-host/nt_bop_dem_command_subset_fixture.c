#include <stdint.h>
#include <stdio.h>

#include <nt.h>

static uint16_t fixture_cs;
static uint16_t fixture_ip;
static uint8_t fixture_service;
static uint32_t dispatcher_service;
static uint32_t idle_calls;
static uint32_t order;
static uint32_t dispatcher_order;
static uint32_t ip_order;

void MS_bop_0(void);
void MS_bop_4(void);

uint16_t getCS(void) { return fixture_cs; }
uint16_t getIP(void) { return fixture_ip; }
void setIP(uint16_t value) { fixture_ip = value; ip_order = ++order; }
void sas_load(uint32_t address, uint8_t *value)
{
    if (address != ((uint32_t)fixture_cs << 4) + fixture_ip || value == NULL) {
        fixture_service = 0u;
        return;
    }
    *value = fixture_service;
}
BOOL DemDispatch(ULONG service) { dispatcher_service = service; dispatcher_order = ++order; return TRUE; }
BOOL CmdDispatch(ULONG service) { dispatcher_service = service; dispatcher_order = ++order; return TRUE; }
void HostIdleNoActivity(void) { ++idle_calls; }

static int run_dem(uint8_t service, uint32_t expected_idle)
{
    fixture_cs = 0x1234u;
    fixture_ip = 0x0080u;
    fixture_service = service;
    dispatcher_service = idle_calls = order = dispatcher_order = ip_order = 0u;
    MS_bop_0();
    return dispatcher_service == service && fixture_ip == 0x0081u &&
        dispatcher_order == 1u && ip_order == 2u && idle_calls == expected_idle;
}

int main(void)
{
    if (!run_dem(0x1fu, 1u)) return 1;
    if (!run_dem(0x14u, 0u)) return 2;
    fixture_cs = 0x2345u;
    fixture_ip = 0x00f0u;
    fixture_service = 0x08u;
    dispatcher_service = idle_calls = order = dispatcher_order = ip_order = 0u;
    MS_bop_4();
    if (dispatcher_service != 0x08u || fixture_ip != 0x00f1u ||
        dispatcher_order != 1u || ip_order != 2u || idle_calls != 0u) return 3;
    puts("PASS: source-shaped DEM/COMMAND BOP ingress subset");
    return 0;
}
