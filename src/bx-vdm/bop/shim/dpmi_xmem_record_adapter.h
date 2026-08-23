#ifndef BX_NTVDM_BOP_SHIM_DPMI_XMEM_RECORD_ADAPTER_H
#define BX_NTVDM_BOP_SHIM_DPMI_XMEM_RECORD_ADAPTER_H

/*
 * Source-derived replacement for xmem.c's private XMEM_BLOCK pointer
 * identity.  The original returned a malloc pointer in SI:DI.  That cannot
 * cross an x86/x64 Bochs guest boundary, so the session returns a nonpointer
 * 32-bit record ID while retaining the source list/owner lifetime locally.
 * This is not a DPMI ingress or an OpenNT provider body.
 */

#include <stdint.h>

#define BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAGIC UINT32_C(0x42584458)
#define BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_VERSION UINT32_C(1)
#define BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS UINT32_C(32)

enum bx_ntvdm_dpmi_xmem_record_adapter_status {
  BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK = 0u,
  BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT,
  BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_CAPACITY,
  BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_ID,
  BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE
};

struct bx_ntvdm_dpmi_xmem_record_adapter_record {
  uint32_t record_id;
  uint32_t ordinary_ram_address;
  uint32_t byte_count;
  uint32_t mantle_opaque_id;
  uint16_t owner;
  uint16_t reserved0;
};

struct bx_ntvdm_dpmi_xmem_record_adapter {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t record_count;
  uint32_t next_record_id;
  struct bx_ntvdm_dpmi_xmem_record_adapter_record
    records[BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS];
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_dpmi_xmem_record_adapter_clear(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter);
int bx_ntvdm_dpmi_xmem_record_adapter_valid(
  const struct bx_ntvdm_dpmi_xmem_record_adapter *adapter);
uint32_t bx_ntvdm_dpmi_xmem_record_adapter_allocate(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter, uint16_t owner,
  uint32_t byte_count, uint32_t *ordinary_ram_address_out,
  uint32_t *record_id_out);
uint32_t bx_ntvdm_dpmi_xmem_record_adapter_release(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter, uint32_t record_id);
uint32_t bx_ntvdm_dpmi_xmem_record_adapter_release_owner(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter, uint16_t owner);
uint32_t bx_ntvdm_dpmi_xmem_record_adapter_reset(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter);

#ifdef __cplusplus
}
#endif

#endif
