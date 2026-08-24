#ifndef T255_S2_GENERIC_UD_CONTEXT_FIXTURE_BRIDGE_H
#define T255_S2_GENERIC_UD_CONTEXT_FIXTURE_BRIDGE_H

#include "adapter-bop/generic_ud_bridge.h"

#define T255_S2_GENERIC_UD_CONTEXT_FIXTURE_MAGIC 0x42584346u

struct t255_s2_generic_ud_context_fixture {
  uint32_t magic, abi_version, struct_bytes, expected_execution_mode;
  struct runtime_generic_ud_outcome_v2 outcome;
};

void t255_s2_generic_ud_context_fixture_clear(
  struct t255_s2_generic_ud_context_fixture *fixture);
int t255_s2_generic_ud_context_fixture_configure(
  const struct t255_s2_generic_ud_context_fixture *fixture);

#endif
