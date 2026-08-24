/*
 * Default-disabled SH-15 host foundation for the historical CCPU fixture.
 * It owns only private backing storage. Every other currently unresolved host
 * callback is an unadmitted path: it records its name and stops the fixture.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void *ccpu_sas_storage;
static unsigned long ccpu_unexpected_calls;

/* CCPU references this historical dispatch-table symbol even though the reset
 * fixture never executes a BOP. The zero table supplies no handler or policy. */
void (*BIOS[256])(void);

unsigned long ntdos64_ccpu_host_unexpected_calls(void) {
    return ccpu_unexpected_calls;
}

void *host_sas_init(uint32_t size) {
    const uint32_t max_size = 64u * 1024u * 1024u;

    if (size == 0 || size > max_size || ccpu_sas_storage != NULL) {
        return NULL;
    }

    ccpu_sas_storage = calloc(1, size);
    return ccpu_sas_storage;
}

void *host_sas_term(void) {
    void *storage = ccpu_sas_storage;
    ccpu_sas_storage = NULL;
    return storage;
}

static void ccpu_unadmitted_callback(const char *name) {
    ++ccpu_unexpected_calls;
    fputs("unadmitted historical CCPU host callback: ", stderr);
    fputs(name, stderr);
    fputc('\n', stderr);
    abort();
}

void EDL_fast_bop(void) { ccpu_unadmitted_callback("EDL_fast_bop"); }
void host_clear_hw_int(void) { ccpu_unadmitted_callback("host_clear_hw_int"); }
#ifndef CCPU_USE_HISTORICAL_EOI_TIMER_OWNERS
void host_EOI_hook(void) { ccpu_unadmitted_callback("host_EOI_hook"); }
#endif
void host_error(void) { ccpu_unadmitted_callback("host_error"); }
void host_exint_hook(void) { ccpu_unadmitted_callback("host_exint_hook"); }
void host_get_jump_restart(void) { ccpu_unadmitted_callback("host_get_jump_restart"); }
void host_get_q_calib_val(void) { ccpu_unadmitted_callback("host_get_q_calib_val"); }
#ifndef CCPU_USE_HISTORICAL_EOI_TIMER_OWNERS
void host_ica_lock(void) { ccpu_unadmitted_callback("host_ica_lock"); }
void host_ica_unlock(void) { ccpu_unadmitted_callback("host_ica_unlock"); }
#endif
#ifndef CCPU_USE_HISTORICAL_MOUSE_OWNER
void host_mouse_install1(void) { ccpu_unadmitted_callback("host_mouse_install1"); }
void host_mouse_install2(void) { ccpu_unadmitted_callback("host_mouse_install2"); }
#endif
void host_set_hw_int(void) { ccpu_unadmitted_callback("host_set_hw_int"); }
void host_swint_hook(void) { ccpu_unadmitted_callback("host_swint_hook"); }
#ifndef CCPU_USE_HISTORICAL_EOI_TIMER_OWNERS
void host_timer_event(void) { ccpu_unadmitted_callback("host_timer_event"); }
void host_TimeStamp(void) { ccpu_unadmitted_callback("host_TimeStamp"); }
void WOWIdle(void) { ccpu_unadmitted_callback("WOWIdle"); }
#endif

FILE *trace_file;
#ifndef CCPU_USE_HISTORICAL_EOI_TIMER_OWNERS
uint32_t DelayIrqLine = 0xffffffffu;
#endif
