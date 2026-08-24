#include <stdint.h>
#include <stdio.h>
#include "bx_ntvdm_machine_composition_v2.h"

typedef struct { uint8_t isr, imr, bda; } fixture;
static int read8(void *o, uint16_t p, uint8_t *v) {
    fixture *f = o; if (p == 0x20u) *v = f->isr;
    else if (p == 0x21u) *v = f->imr; else return 0; return 1;
}
static int write8(void *o, uint16_t p, uint8_t v) {
    fixture *f = o; if (p == 0x21u) f->imr = v;
    else if (p != 0x20u || (v != 0x0bu && v != 0x20u)) return 0; return 1;
}
static int store8(void *o, uint32_t a, uint8_t v) {
    fixture *f = o; if (a != 0x46bu) return 0; f->bda = v; return 1;
}
static int load8(void *o, uint32_t a, uint8_t *v) { (void)o; (void)a; *v = 0; return 1; }
static void report(void *o, uint32_t e, uint32_t p, const char *m) { (void)o; (void)e; (void)p; (void)m; }
void unexpected_int(void) { uint8_t isr, imr; outb(0x20u, 0x0bu); inb(0x20u, &isr); inb(0x21u, &imr); outb(0x21u, (uint8_t)(imr | isr)); outb(0x20u, 0x20u); c_sas_store(0x46bu, isr); }
void illegal_op_int(void) { (void)c_sas_hw_at(c_effective_addr(c_getSS(), c_getESP())); (void)host_error(0, 0, 0); unexpected_int(); }
int main(void) {
    bx_ntvdm_exception_event_v1 e = { BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(e), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0, 6, 0, 0, 0x100 };
    bx_ntvdm_cpu_state_v1 c; bx_ntvdm_instruction_window_v1 w; bx_ntvdm_machine_mechanics_v1 m; bx_ntvdm_cpu_result_v2 r; fixture f = { 4, 0x10, 0 }; uint8_t b[3] = { 0xc4, 0xc4, 2 };
    bx_ntvdm_cpu_state_v1_initialize(&c, BX_NTVDM_CPU_EXECUTION_REAL); bx_ntvdm_instruction_window_v1_capture(&w, b, 3); bx_ntvdm_machine_mechanics_v1_initialize(&m);
    m.opaque=&f; m.read8=read8; m.write8=write8; m.store8=store8; m.load8=load8; m.report=report; m.execution_mode=BX_NTVDM_CPU_EXECUTION_REAL;
    if (!bx_ntvdm_machine_composition_v2_dispatch(&e,&c,&w,&m,&r) || r.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || r.resume_rip != 0x103 || f.bda != 4) return 1;
    b[2]=6; bx_ntvdm_instruction_window_v1_capture(&w,b,3);
    if (!bx_ntvdm_machine_composition_v2_dispatch(&e,&c,&w,&m,&r) || r.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) return 1;
    b[2]=0x50; bx_ntvdm_instruction_window_v1_capture(&w,b,3);
    return bx_ntvdm_machine_composition_v2_dispatch(&e,&c,&w,&m,&r) && r.disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH ? 0 : 1;
}
