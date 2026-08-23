#include "bx_ntvdm_initial_state_catalog_v1.h"
#include <string.h>

static const char evidence[] = "168219f4c7d73abee06b62b1423298a2f111685e1bf36da23b2c16a642310195";
static const uint8_t bytes[] = {
  0x4a,0x01,0x00,0xc0,0x4d,0xf8,0x00,0xf0,0x41,0xf8,0x00,0xf0,0xfe,0xe3,0x00,0xf0,
  0x39,0xe7,0x00,0xf0,0x59,0xf8,0x00,0xf0,0x2e,0xe8,0x00,0xf0,0xd2,0xef,0x00,0xf0,
  0x95,0x96,0x00,0xf0,0xf2,0xe6,0x00,0xf0,0x6e,0xfe,0x00,0xf0,0x53,0xff,0x00,0xf0,
  0x10,0x00,0x00,0x00,0xc0 };
static uint8_t nibble(char c) { return (uint8_t)(c <= '9' ? c - '0' : c - 'a' + 10); }
int bx_ntvdm_initial_state_catalog_v1_select(const uint16_t *digest,
    uint32_t chars, bx_ntvdm_initial_state_v1 *out)
{
  uint32_t i;
  if (!out) return 0;
  bx_ntvdm_initial_state_v1_clear(out);
  if (!digest || chars != 64u) return 0;
  for (i=0; i<64u; ++i) if (digest[i] != (uint16_t)(uint8_t)evidence[i]) return 0;
  out->disposition=BX_NTVDM_INITIAL_STATE_V1_PRESENT; out->range_count=3u; out->payload_bytes=(uint32_t)sizeof(bytes);
  for(i=0;i<32u;++i) out->evidence_sha256[i]=(uint8_t)((nibble(evidence[i*2])<<4)|nibble(evidence[i*2+1]));
  memcpy(out->payload,bytes,sizeof(bytes));
  bx_ntvdm_guest_write_v1_initialize(&out->ranges[0],0x40u,48u,0u);
  bx_ntvdm_guest_write_v1_initialize(&out->ranges[1],0x496u,1u,48u);
  bx_ntvdm_guest_write_v1_initialize(&out->ranges[2],0x714u,4u,49u);
  return bx_ntvdm_initial_state_v1_admitted(out,0x100000u);
}
