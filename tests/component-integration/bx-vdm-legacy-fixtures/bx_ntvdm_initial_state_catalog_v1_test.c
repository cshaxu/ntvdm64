#include "initial_state_catalog_v1.h"
int main(void) {
  static const uint16_t good[] = L"168219f4c7d73abee06b62b1423298a2f111685e1bf36da23b2c16a642310195";
  bx_ntvdm_initial_state_v1 v;
  if (!bx_ntvdm_initial_state_catalog_v1_select(good,64u,&v) || v.range_count!=3u || v.payload_bytes!=53u) return 1;
  good[0];
  if (bx_ntvdm_initial_state_catalog_v1_select(good,63u,&v) || v.disposition!=BX_NTVDM_INITIAL_STATE_V1_ABSENT) return 2;
  return 0;
}
