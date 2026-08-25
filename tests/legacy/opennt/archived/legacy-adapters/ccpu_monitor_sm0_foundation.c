/*
 * SM-0 bounded software-monitor foundation.
 *
 * This is intentionally separate from the trace-only CCPU foundation. It
 * exposes one checked RAM aperture to CCPU SAS and aborts on every unadmitted
 * host callback. It contains no BIOS, BOP, DOS, or device behavior.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint8_t *sm0_ram;
static uint32_t sm0_ram_size;
static unsigned long sm0_unexpected_calls;

#ifdef CCPU_SM0_USE_HISTORICAL_SAS
extern uint8_t *Start_of_M_area;
#endif

#ifndef CCPU_SM2_USE_HISTORICAL_BIOS
void (*BIOS[256])(void);
#endif

unsigned long runner_ccpu_sm0_unexpected_calls(void) {
    return sm0_unexpected_calls;
}

uint8_t *runner_ccpu_sm0_ram(void) {
#ifdef CCPU_SM0_USE_HISTORICAL_SAS
    return Start_of_M_area;
#else
    return sm0_ram;
#endif
}

#ifndef CCPU_SM0_USE_HISTORICAL_SAS
void *host_sas_init(uint32_t size) {
    const uint32_t max_size = 64u * 1024u * 1024u;

    if (size == 0 || size > max_size || sm0_ram != NULL) {
        return NULL;
    }

    sm0_ram = calloc(1, size);
    if (sm0_ram != NULL) {
        sm0_ram_size = size;
    }
    return sm0_ram;
}

void *host_sas_term(void) {
    void *ram = sm0_ram;
    sm0_ram = NULL;
    sm0_ram_size = 0;
    return ram;
}
#endif

uint32_t c_VirtualiseInstruction(uint32_t eip_in_rom, uint16_t size,
                                 uint32_t linear_address_or_port,
                                 uint32_t data_in) {
    (void)eip_in_rom;
    (void)size;
    (void)linear_address_or_port;
    (void)data_in;
    ++sm0_unexpected_calls;
    fputs("unadmitted CCPU virtualised instruction in SM-0\n", stderr);
    abort();
}

static void sm0_unadmitted_callback(const char *name) {
    ++sm0_unexpected_calls;
    fputs("unadmitted CCPU host callback in SM-0: ", stderr);
    fputs(name, stderr);
    fputc('\n', stderr);
    abort();
}

uint8_t *c_sas_touch(uint32_t address, uint32_t length) {
    (void)address;
    (void)length;
    sm0_unadmitted_callback("c_sas_touch");
    return NULL;
}

void EDL_fast_bop(void) { sm0_unadmitted_callback("EDL_fast_bop"); }
void LIM_b_write(void) { sm0_unadmitted_callback("LIM_b_write"); }
void LIM_w_write(void) { sm0_unadmitted_callback("LIM_w_write"); }
void host_simulate(void) { sm0_unadmitted_callback("host_simulate"); }
/* c_cpu_reset probes C_SWITCHNPX through the historical configuration
 * boundary. SM-0 deliberately admits no optional switchable-NPX profile.
 * The config-to-SAS trace instead retains original config.c as that owner. */
#ifndef CCPU_SM0_USE_ORIGINAL_CONFIG
void *config_inquire(uint8_t host_id, void *values) {
    (void)host_id;
    (void)values;
    return NULL;
}
#endif
/* rom_init() asks this historical resource boundary for optional ROM images.
 * A zero byte count is its defined "resource absent" result; SM-0 supplies
 * no ROM image and leaves every address ordinary SAS RAM. */
long host_read_resource(int type, char *name, uint8_t *address, int max_size,
                        int display_error) {
    (void)type;
    (void)name;
    (void)address;
    (void)max_size;
    (void)display_error;
    return 0;
}
#ifndef CCPU_SM2_USE_EFFECTIVE_ADDR_COMPLETION
uint32_t c_effective_addr(uint16_t selector, uint32_t offset) {
    (void)selector;
    (void)offset;
    sm0_unadmitted_callback("c_effective_addr");
    return 0;
}
#endif
void host_clear_hw_int(void) { sm0_unadmitted_callback("host_clear_hw_int"); }
#ifndef CCPU_SM2_USE_HISTORICAL_EOI
void host_EOI_hook(void) { sm0_unadmitted_callback("host_EOI_hook"); }
#endif
void host_error(void) { sm0_unadmitted_callback("host_error"); }
void host_exint_hook(void) { sm0_unadmitted_callback("host_exint_hook"); }
void host_get_jump_restart(void) { sm0_unadmitted_callback("host_get_jump_restart"); }
void host_get_q_calib_val(void) { sm0_unadmitted_callback("host_get_q_calib_val"); }
#ifndef CCPU_SM2_USE_HISTORICAL_EOI
void host_ica_lock(void) { sm0_unadmitted_callback("host_ica_lock"); }
void host_ica_unlock(void) { sm0_unadmitted_callback("host_ica_unlock"); }
#endif
void host_mouse_install1(void) { sm0_unadmitted_callback("host_mouse_install1"); }
void host_mouse_install2(void) { sm0_unadmitted_callback("host_mouse_install2"); }
void host_set_hw_int(void) { sm0_unadmitted_callback("host_set_hw_int"); }
void host_swint_hook(void) { sm0_unadmitted_callback("host_swint_hook"); }
#ifndef CCPU_SM2_USE_HISTORICAL_TIMER
void host_timer_event(void) { sm0_unadmitted_callback("host_timer_event"); }
void host_TimeStamp(void) { sm0_unadmitted_callback("host_TimeStamp"); }
#endif
#ifndef CCPU_SM2_USE_HISTORICAL_EOI
void WOWIdle(void) { sm0_unadmitted_callback("WOWIdle"); }
#endif

FILE *trace_file;
#ifndef CCPU_SM2_USE_HISTORICAL_EOI
uint32_t DelayIrqLine = 0xffffffffu;
#endif

/* This vector is consulted only for non-RAM/self-modifying SAS regions. */
#ifndef CCPU_SM2_USE_HISTORICAL_BIOS
uint8_t read_pointers[64];
#endif
