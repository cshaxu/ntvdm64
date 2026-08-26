#include "ccpu_frame_context.h"

#include <string.h>

static __declspec(thread) runtime_ccpu_frame_context *g_context;

static runtime_ccpu_frame_context *active_context(void)
{
    return g_context != NULL && runtime_ccpu_frame_context_valid(g_context) ?
        g_context : NULL;
}

static uint16_t low16(uint32_t value) { return (uint16_t)value; }

static void set_gpr(uint32_t index, uint16_t value)
{
    runtime_ccpu_frame_context *context = active_context();
    if (context != NULL)
        (void)runtime_cpu_delta_set_gpr16(&context->result->cpu_delta, index, value);
}

int runtime_ccpu_frame_context_valid(const runtime_ccpu_frame_context *context)
{
    return context != NULL && context->magic == RUNTIME_CCPU_FRAME_CONTEXT_MAGIC &&
        context->abi_version == RUNTIME_CCPU_FRAME_CONTEXT_VERSION &&
        context->struct_bytes == sizeof(*context) && context->reserved0 == 0u &&
        context->cpu != NULL && runtime_cpu_state_valid(context->cpu) &&
        context->result != NULL && runtime_cpu_result_valid(context->result) &&
        context->guest_read != NULL && context->guest_write != NULL;
}

int runtime_ccpu_frame_context_begin(const runtime_ccpu_frame_context *context)
{
    if (!runtime_ccpu_frame_context_valid(context) || g_context != NULL) return 0;
    g_context = (runtime_ccpu_frame_context *)context;
    return 1;
}

void runtime_ccpu_frame_context_end(void) { g_context = NULL; }

uint16_t runtime_ccpu_get_ax(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : low16(c->cpu->eax); }
uint16_t runtime_ccpu_get_bx(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : low16(c->cpu->ebx); }
uint16_t runtime_ccpu_get_cx(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : low16(c->cpu->ecx); }
uint16_t runtime_ccpu_get_dx(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : low16(c->cpu->edx); }
uint16_t runtime_ccpu_get_si(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : low16(c->cpu->esi); }
uint16_t runtime_ccpu_get_bp(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : low16(c->cpu->ebp); }
uint16_t runtime_ccpu_get_cs(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : c->cpu->cs; }
uint16_t runtime_ccpu_get_ds(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : c->cpu->ds; }
uint16_t runtime_ccpu_get_es(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : c->cpu->es; }
uint16_t runtime_ccpu_get_ss(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : c->cpu->ss; }
uint16_t runtime_ccpu_get_ip(void) { runtime_ccpu_frame_context *c = active_context(); return c == NULL ? 0u : low16(c->cpu->eip); }
uint8_t runtime_ccpu_get_al(void) { return (uint8_t)(runtime_ccpu_get_ax() & 0xffu); }
uint8_t runtime_ccpu_get_ah(void) { return (uint8_t)(runtime_ccpu_get_ax() >> 8u); }
void runtime_ccpu_set_ax(uint16_t value) { set_gpr(0u, value); }
void runtime_ccpu_set_al(uint16_t value) { runtime_ccpu_set_ax((uint16_t)((runtime_ccpu_get_ax() & 0xff00u) | (value & 0xffu))); }
void runtime_ccpu_set_bx(uint16_t value) { set_gpr(3u, value); }
void runtime_ccpu_set_cx(uint16_t value) { set_gpr(1u, value); }
void runtime_ccpu_set_dx(uint16_t value) { set_gpr(2u, value); }
void runtime_ccpu_set_ds(uint16_t value) { runtime_ccpu_frame_context *c = active_context(); if (c != NULL) (void)runtime_cpu_delta_set_segment(&c->result->cpu_delta, 3u, value); }
void runtime_ccpu_set_es(uint16_t value) { runtime_ccpu_frame_context *c = active_context(); if (c != NULL) (void)runtime_cpu_delta_set_segment(&c->result->cpu_delta, 0u, value); }
void runtime_ccpu_set_ip(uint16_t value) { runtime_ccpu_frame_context *c = active_context(); if (c != NULL) c->cpu->eip = value; }
void runtime_ccpu_set_cf(int value) { runtime_ccpu_frame_context *c = active_context(); if (c != NULL) (void)runtime_cpu_result_set_cf(c->result, value); }
void runtime_ccpu_sas_load(uint32_t address, uint8_t *target)
{
    runtime_ccpu_frame_context *c = active_context();
    if (target == NULL) return;
    if (c == NULL || !c->guest_read(c->guest_state, address, target, 1u)) *target = 0xffu;
}
int runtime_ccpu_set_pending(void)
{
    runtime_ccpu_frame_context *c = active_context();
    return c != NULL && runtime_cpu_result_pending(c->result);
}

int runtime_ccpu_set_controlled_stop(void)
{
    runtime_ccpu_frame_context *c = active_context();
    return c != NULL && runtime_cpu_result_stop(c->result);
}
