#include "opennt_int06_provider.h"

/* DIVERGENCE(BOP-DIV-080): source-derived state machine for OpenNT illegalp.c:79-134 and
 * unexp_nt.c:50-99.  It exposes copied operations only; execution belongs to
 * the typed machine seam and the selector belongs to bx-vdm. */
enum {
  P_IP_LO = 1, P_IP_HI, P_CS_LO, P_CS_HI, P_WRITE_CS_LO, P_WRITE_CS_HI,
  P_WRITE_IP_LO, P_WRITE_IP_HI, P_MASTER_OCW3, P_MASTER_ISR,
  P_MASTER_IMR, P_SLAVE_OCW3, P_SLAVE_ISR, P_SLAVE_IMR, P_WRITE_SLAVE_IMR,
  P_SLAVE_EOI, P_WRITE_MASTER_IMR, P_MASTER_EOI, P_BDA, P_DONE
};

static int valid(const struct int06_provider_state *s) {
  return s != 0 && s->magic == INT06_PROVIDER_MAGIC &&
    s->abi_version == INT06_PROVIDER_VERSION &&
    s->struct_bytes == sizeof(*s) &&
    s->status == INT06_PROVIDER_STATUS_PENDING;
}
static void op(struct int06_provider_operation *o, uint32_t kind,
  uint64_t address, uint8_t value) {
  memset(o, 0, sizeof(*o)); o->kind = kind; o->address = address; o->value = value;
}
void int06_provider_initialize(struct int06_provider_state *s) {
  if (s == 0) return; memset(s, 0, sizeof(*s)); s->magic = INT06_PROVIDER_MAGIC;
  s->abi_version = INT06_PROVIDER_VERSION; s->struct_bytes = sizeof(*s);
}
int int06_provider_begin(struct int06_provider_state *s,
  uint16_t ss, uint32_t esp) {
  uint64_t stack;
  if (s == 0 || s->magic != INT06_PROVIDER_MAGIC ||
      s->abi_version != INT06_PROVIDER_VERSION ||
      s->struct_bytes != sizeof(*s) || s->status != INT06_PROVIDER_STATUS_PENDING ||
      s->phase != 0u) return 0;
  stack = ((uint64_t)ss << 4) + esp;
  if (stack > UINT32_MAX - 3u) { s->status = INT06_PROVIDER_STATUS_INVALID; return 0; }
  s->stack_physical = stack; s->phase = P_IP_LO; return 1;
}
int int06_provider_begin_unexpected(
  struct int06_provider_state *s) {
  if (s == 0 || s->magic != INT06_PROVIDER_MAGIC ||
      s->abi_version != INT06_PROVIDER_VERSION ||
      s->struct_bytes != sizeof(*s) || s->status != INT06_PROVIDER_STATUS_PENDING ||
      s->phase != 0u) return 0;
  s->phase = P_MASTER_OCW3;
  return 1;
}
int int06_provider_next(const struct int06_provider_state *s,
  struct int06_provider_operation *o) {
  if (o == 0 || !valid(s)) return 0;
  switch (s->phase) {
  case P_IP_LO: op(o, INT06_PROVIDER_OPERATION_RAM_READ8, s->stack_physical, 0); return 1;
  case P_IP_HI: op(o, INT06_PROVIDER_OPERATION_RAM_READ8, s->stack_physical+1, 0); return 1;
  case P_CS_LO: op(o, INT06_PROVIDER_OPERATION_RAM_READ8, s->stack_physical+2, 0); return 1;
  case P_CS_HI: op(o, INT06_PROVIDER_OPERATION_RAM_READ8, s->stack_physical+3, 0); return 1;
  case P_WRITE_CS_LO: op(o, INT06_PROVIDER_OPERATION_RAM_WRITE8, s->stack_physical+2, (uint8_t)(s->saved_cs+1u)); return 1;
  case P_WRITE_CS_HI: op(o, INT06_PROVIDER_OPERATION_RAM_WRITE8, s->stack_physical+3, (uint8_t)((s->saved_cs+1u)>>8)); return 1;
  case P_WRITE_IP_LO: op(o, INT06_PROVIDER_OPERATION_RAM_WRITE8, s->stack_physical, (uint8_t)(s->saved_ip+1u)); return 1;
  case P_WRITE_IP_HI: op(o, INT06_PROVIDER_OPERATION_RAM_WRITE8, s->stack_physical+1, (uint8_t)((s->saved_ip+1u)>>8)); return 1;
  case P_MASTER_OCW3: op(o, INT06_PROVIDER_OPERATION_PORT_WRITE8, 0x20u, 0x0bu); return 1;
  case P_MASTER_ISR: op(o, INT06_PROVIDER_OPERATION_PORT_READ8, 0x20u, 0); return 1;
  case P_MASTER_IMR: op(o, INT06_PROVIDER_OPERATION_PORT_READ8, 0x21u, 0); return 1;
  case P_SLAVE_OCW3: op(o, INT06_PROVIDER_OPERATION_PORT_WRITE8, 0xa0u, 0x0bu); return 1;
  case P_SLAVE_ISR: op(o, INT06_PROVIDER_OPERATION_PORT_READ8, 0xa0u, 0); return 1;
  case P_SLAVE_IMR: op(o, INT06_PROVIDER_OPERATION_PORT_READ8, 0xa1u, 0); return 1;
  case P_WRITE_SLAVE_IMR: op(o, INT06_PROVIDER_OPERATION_PORT_WRITE8, 0xa1u, (uint8_t)(s->slave_imr|s->slave_isr)); return 1;
  case P_SLAVE_EOI: op(o, INT06_PROVIDER_OPERATION_PORT_WRITE8, 0xa0u, 0x20u); return 1;
  case P_WRITE_MASTER_IMR: op(o, INT06_PROVIDER_OPERATION_PORT_WRITE8, 0x21u, (uint8_t)((s->master_imr|s->master_isr)&0xfbu)); return 1;
  case P_MASTER_EOI: op(o, INT06_PROVIDER_OPERATION_PORT_WRITE8, 0x20u, 0x20u); return 1;
  case P_BDA: op(o, INT06_PROVIDER_OPERATION_RAM_WRITE8, INT06_BDA_INTERRUPT_CAUSE, s->master_isr == 0u ? 0xffu : s->master_isr); return 1;
  default: return 0;
  }
}
int int06_provider_accept(struct int06_provider_state *s,
  const struct int06_provider_operation *o, uint8_t v) {
  struct int06_provider_operation expected;
  if (!valid(s) || o == 0 || !int06_provider_next(s, &expected) ||
      o->kind != expected.kind || o->address != expected.address ||
      (o->kind != INT06_PROVIDER_OPERATION_RAM_READ8 &&
       o->kind != INT06_PROVIDER_OPERATION_PORT_READ8 && o->value != expected.value)) {
    if (s != 0) s->status = INT06_PROVIDER_STATUS_FAILED; return 0;
  }
  switch (s->phase) {
  case P_IP_LO: s->saved_ip = v; s->phase = P_IP_HI; break;
  case P_IP_HI: s->saved_ip |= (uint16_t)v << 8; s->phase = P_CS_LO; break;
  case P_CS_LO: s->saved_cs = v; s->phase = P_CS_HI; break;
  case P_CS_HI: s->saved_cs |= (uint16_t)v << 8; s->phase = s->saved_ip == 0xffffu ? P_WRITE_CS_LO : P_WRITE_IP_LO; break;
  case P_WRITE_CS_LO: s->phase = P_WRITE_CS_HI; break;
  case P_WRITE_CS_HI: s->phase = P_WRITE_IP_LO; break;
  case P_WRITE_IP_LO: s->phase = P_WRITE_IP_HI; break;
  case P_WRITE_IP_HI: s->phase = P_MASTER_OCW3; break;
  case P_MASTER_OCW3: s->phase = P_MASTER_ISR; break;
  case P_MASTER_ISR: s->master_isr = v; s->phase = v == 0 ? P_BDA : P_MASTER_IMR; break;
  case P_MASTER_IMR: s->master_imr = v; s->phase = P_SLAVE_OCW3; break;
  case P_SLAVE_OCW3: s->phase = P_SLAVE_ISR; break;
  case P_SLAVE_ISR: s->slave_isr = v; s->phase = v == 0 ? P_WRITE_MASTER_IMR : P_SLAVE_IMR; break;
  case P_SLAVE_IMR: s->slave_imr = v; s->phase = P_WRITE_SLAVE_IMR; break;
  case P_WRITE_SLAVE_IMR: s->phase = P_SLAVE_EOI; break;
  case P_SLAVE_EOI: s->phase = P_WRITE_MASTER_IMR; break;
  case P_WRITE_MASTER_IMR: s->phase = P_MASTER_EOI; break;
  case P_MASTER_EOI: s->phase = P_BDA; break;
  case P_BDA: s->phase = P_DONE; s->status = INT06_PROVIDER_STATUS_COMPLETE; break;
  default: s->status = INT06_PROVIDER_STATUS_FAILED; return 0;
  }
  return 1;
}
int int06_provider_complete(const struct int06_provider_state *s) {
  return s != 0 && s->magic == INT06_PROVIDER_MAGIC &&
    s->abi_version == INT06_PROVIDER_VERSION && s->struct_bytes == sizeof(*s) &&
    s->status == INT06_PROVIDER_STATUS_COMPLETE && s->phase == P_DONE;
}
