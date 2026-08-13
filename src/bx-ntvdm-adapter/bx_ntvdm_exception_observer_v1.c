#include "bx_ntvdm_exception_observer_v1.h"

#include "bx_ntvdm_bop_catalog_v1.h"

#include <stdio.h>

int bx_ntvdm_exception_observer_v1_observe(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_observer_v1_report report, void *opaque)
{
    bx_ntvdm_bop_catalog_v1_identity identity;
    char message[320];

    if (event == 0 || cpu_before == 0 || window == 0 || report == 0 ||
        !bx_ntvdm_bop_catalog_v1_observe(event, cpu_before, window,
            &identity)) return 0;
    if (!identity.is_bop) return 1;

    (void)snprintf(message, sizeof(message),
        "ntdos64 adapter bop observed cpu=%u rip=%llx selector=%02x "
        "class=%u family=%u service-state=%u service=%02x ax=%04x bx=%04x "
        "cs=%04x ds=%04x dx=%04x si=%04x di=%04x cx=%04x flags=%08x "
        "ss=%04x sp=%04x",
        (unsigned)event->cpu_id, (unsigned long long)event->fault_rip,
        (unsigned)identity.selector, (unsigned)identity.source_class,
        (unsigned)identity.family, (unsigned)identity.service_state,
        (unsigned)identity.service, (unsigned)(cpu_before->eax & 0xffffu),
        (unsigned)(cpu_before->ebx & 0xffffu), (unsigned)cpu_before->cs,
        (unsigned)cpu_before->ds, (unsigned)(cpu_before->edx & 0xffffu),
        (unsigned)(cpu_before->esi & 0xffffu),
        (unsigned)(cpu_before->edi & 0xffffu),
        (unsigned)(cpu_before->ecx & 0xffffu),
        (unsigned)cpu_before->eflags, (unsigned)cpu_before->ss,
        (unsigned)(cpu_before->esp & 0xffffu));
    report(opaque, message);
    return 1;
}
