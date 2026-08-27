#include "adapter-softpc/interrupt06_provider.h"

static int step(struct int06_provider_state *s, uint32_t kind, uint64_t address, uint8_t value, uint8_t read_value) {
  struct int06_provider_operation o;
  if (!int06_provider_next(s, &o) || o.kind != kind || o.address != address ||
      ((kind != INT06_PROVIDER_OPERATION_RAM_READ8 && kind != INT06_PROVIDER_OPERATION_PORT_READ8) && o.value != value)) return 0;
  return int06_provider_accept(s, &o, read_value);
}
static int software_case(void) {
  struct int06_provider_state s;
  int06_provider_initialize(&s);
  if (!int06_provider_begin(&s, 0x70u, 0x479u)) return 1;
  if (!step(&s,1,0xb79u,0,0x34u)||!step(&s,1,0xb7au,0,0x12u)||!step(&s,1,0xb7bu,0,0x70u)||!step(&s,1,0xb7cu,0,0x00u)) return 2;
  if (!step(&s,2,0xb79u,0x35u,0)||!step(&s,2,0xb7au,0x12u,0)||!step(&s,4,0x20u,0x0bu,0)||!step(&s,3,0x20u,0,0)) return 3;
  if (!step(&s,2,0x46bu,0xffu,0)||!int06_provider_complete(&s)) return 4;
  return 0;
}
static int hardware_carry_slave_case(void) {
  struct int06_provider_state s;
  int06_provider_initialize(&s);
  if (!int06_provider_begin(&s, 0x100u, 0xfffeu)) return 1;
  if (!step(&s,1,0x10ffeu,0,0xffu)||!step(&s,1,0x10fffu,0,0xffu)||!step(&s,1,0x11000u,0,0x34u)||!step(&s,1,0x11001u,0,0x12u)) return 2;
  if (!step(&s,2,0x11000u,0x35u,0)||!step(&s,2,0x11001u,0x12u,0)||!step(&s,2,0x10ffeu,0x00u,0)||!step(&s,2,0x10fffu,0x00u,0)) return 3;
  if (!step(&s,4,0x20u,0x0bu,0)||!step(&s,3,0x20u,0,0x09u)||!step(&s,3,0x21u,0,0x20u)||!step(&s,4,0xa0u,0x0bu,0)||!step(&s,3,0xa0u,0,0x80u)||!step(&s,3,0xa1u,0,0x02u)) return 4;
  if (!step(&s,4,0xa1u,0x82u,0)||!step(&s,4,0xa0u,0x20u,0)||!step(&s,4,0x21u,0x29u,0)||!step(&s,4,0x20u,0x20u,0)||!step(&s,2,0x46bu,0x09u,0)||!int06_provider_complete(&s)) return 5;
  return 0;
}
static int unexpected_suffix_case(void) {
  struct int06_provider_state s;
  int06_provider_initialize(&s);
  if (!int06_provider_begin_unexpected(&s)) return 1;
  if (!step(&s, 4, 0x20u, 0x0bu, 0) || !step(&s, 3, 0x20u, 0, 0)) return 2;
  if (!step(&s, 2, 0x46bu, 0xffu, 0) || !int06_provider_complete(&s)) return 3;
  return 0;
}
int main(void) { int x=software_case(); if(x) return x; x=hardware_carry_slave_case(); if(x) return x; return unexpected_suffix_case(); }
