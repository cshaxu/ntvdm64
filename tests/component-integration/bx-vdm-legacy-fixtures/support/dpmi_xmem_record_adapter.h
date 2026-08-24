#ifndef RUNTIME_BOP_SHIM_DPMI_XMEM_RECORD_ADAPTER_H
#define RUNTIME_BOP_SHIM_DPMI_XMEM_RECORD_ADAPTER_H

/*
 * Source-derived replacement for xmem.c's private XMEM_BLOCK pointer
 * identity.  The original returned a malloc pointer in SI:DI.  That cannot
 * cross an x86/x64 Bochs guest boundary, so the session returns a nonpointer
 * 32-bit record ID while retaining the source list/owner lifetime locally.
 * This is not a DPMI ingress or an OpenNT provider body.
 */

#include <stdint.h>

#define RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAGIC UINT32_C(0x42584458)
#define RUNTIME_DPMI_XMEM_RECORD_ADAPTER_VERSION UINT32_C(1)
#define RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS UINT32_C(32)

enum runtime_dpmi_xmem_record_adapter_status {
  RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK = 0u,
  RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT,
  RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_CAPACITY,
  RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_ID,
  RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE
};

struct runtime_dpmi_xmem_record_adapter_record {
  uint32_t record_id;
  uint32_t ordinary_ram_address;
  uint32_t byte_count;
  uint32_t mantle_opaque_id;
  uint16_t owner;
  uint16_t reserved0;
};

struct runtime_dpmi_xmem_record_adapter {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t record_count;
  uint32_t next_record_id;
  struct runtime_dpmi_xmem_record_adapter_record
    records[RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS];
};

#ifdef __cplusplus
extern "C" {
#endif

void runtime_dpmi_xmem_record_adapter_clear(
  struct runtime_dpmi_xmem_record_adapter *adapter);
int runtime_dpmi_xmem_record_adapter_valid(
  const struct runtime_dpmi_xmem_record_adapter *adapter);
uint32_t runtime_dpmi_xmem_record_adapter_allocate(
  struct runtime_dpmi_xmem_record_adapter *adapter, uint16_t owner,
  uint32_t byte_count, uint32_t *ordinary_ram_address_out,
  uint32_t *record_id_out);
uint32_t runtime_dpmi_xmem_record_adapter_release(
  struct runtime_dpmi_xmem_record_adapter *adapter, uint32_t record_id);
uint32_t runtime_dpmi_xmem_record_adapter_reallocate(
  struct runtime_dpmi_xmem_record_adapter *adapter, uint32_t record_id,
  uint32_t new_byte_count, uint32_t *ordinary_ram_address_out);
uint32_t runtime_dpmi_xmem_record_adapter_release_owner(
  struct runtime_dpmi_xmem_record_adapter *adapter, uint16_t owner);
uint32_t runtime_dpmi_xmem_record_adapter_reset(
  struct runtime_dpmi_xmem_record_adapter *adapter);

#ifdef __cplusplus
}
#endif

#endif
