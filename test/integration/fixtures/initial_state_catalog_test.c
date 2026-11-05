#include "initial_state_catalog.h"
int main(void) {
  static const uint16_t good[] = L"168219f4c7d73abee06b62b1423298a2f111685e1bf36da23b2c16a642310195";
  runtime_initial_state v;
  if (!runtime_initial_state_catalog_select(good,64u,&v) || v.range_count!=3u || v.payload_bytes!=53u) return 1;
  good[0];
  if (runtime_initial_state_catalog_select(good,63u,&v) || v.disposition!=RUNTIME_INITIAL_STATE_ABSENT) return 2;
  return 0;
}
