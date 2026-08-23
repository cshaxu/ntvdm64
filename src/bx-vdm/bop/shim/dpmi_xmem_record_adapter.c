#include "dpmi_xmem_record_adapter.h"

#include "bx-mantle/bx_ntvdm_ordinary_ram_reservation_v1.h"

#include <string.h>

static struct bx_ntvdm_dpmi_xmem_record_adapter_record *find_record(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter, uint32_t record_id)
{
  uint32_t index;
  if (!bx_ntvdm_dpmi_xmem_record_adapter_valid(adapter) || record_id == 0u)
    return NULL;
  for (index = 0u; index < BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS; ++index)
    if (adapter->records[index].record_id == record_id) return &adapter->records[index];
  return NULL;
}

void bx_ntvdm_dpmi_xmem_record_adapter_clear(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter)
{
  if (adapter == NULL) return;
  memset(adapter, 0, sizeof(*adapter));
  adapter->magic = BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAGIC;
  adapter->abi_version = BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_VERSION;
  adapter->struct_bytes = sizeof(*adapter);
  adapter->next_record_id = 1u;
}

int bx_ntvdm_dpmi_xmem_record_adapter_valid(
  const struct bx_ntvdm_dpmi_xmem_record_adapter *adapter)
{
  return adapter != NULL &&
    adapter->magic == BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAGIC &&
    adapter->abi_version == BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_VERSION &&
    adapter->struct_bytes == sizeof(*adapter) &&
    adapter->next_record_id != 0u &&
    adapter->record_count <= BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS;
}

uint32_t bx_ntvdm_dpmi_xmem_record_adapter_allocate(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter, uint16_t owner,
  uint32_t byte_count, uint32_t *ordinary_ram_address_out,
  uint32_t *record_id_out)
{
  uint32_t index;
  struct bx_ntvdm_ordinary_ram_reservation_v1 action;
  if (ordinary_ram_address_out != NULL) *ordinary_ram_address_out = 0u;
  if (record_id_out != NULL) *record_id_out = 0u;
  if (!bx_ntvdm_dpmi_xmem_record_adapter_valid(adapter) || byte_count == 0u)
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT;
  if (adapter->record_count == BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS)
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_CAPACITY;
  for (index = 0u; index < BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS; ++index)
    if (adapter->records[index].record_id == 0u) break;
  if (index == BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS)
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_CAPACITY;
  bx_ntvdm_ordinary_ram_reservation_v1_clear(&action);
  action.kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_ALLOCATE;
  action.byte_count = byte_count;
  /* xmem.c documents 64 KiB alignment; retain it at the adapter boundary. */
  action.alignment_bytes = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_MAX_ALIGNMENT;
  if (bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(&action) !=
      BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_OK)
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE;
  if (adapter->next_record_id == 0u) adapter->next_record_id = 1u;
  adapter->records[index].record_id = adapter->next_record_id++;
  adapter->records[index].ordinary_ram_address = action.address;
  adapter->records[index].byte_count = action.byte_count;
  adapter->records[index].mantle_opaque_id = action.opaque_id;
  adapter->records[index].owner = owner;
  ++adapter->record_count;
  if (ordinary_ram_address_out != NULL) *ordinary_ram_address_out = action.address;
  if (record_id_out != NULL) *record_id_out = adapter->records[index].record_id;
  return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK;
}

uint32_t bx_ntvdm_dpmi_xmem_record_adapter_release(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter, uint32_t record_id)
{
  struct bx_ntvdm_dpmi_xmem_record_adapter_record *record =
    find_record(adapter, record_id);
  struct bx_ntvdm_ordinary_ram_reservation_v1 action;
  if (record == NULL) return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_ID;
  bx_ntvdm_ordinary_ram_reservation_v1_clear(&action);
  action.kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_RELEASE;
  action.opaque_id = record->mantle_opaque_id;
  if (bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(&action) !=
      BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_OK)
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE;
  memset(record, 0, sizeof(*record));
  --adapter->record_count;
  return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK;
}

uint32_t bx_ntvdm_dpmi_xmem_record_adapter_release_owner(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter, uint16_t owner)
{
  uint32_t index;
  if (!bx_ntvdm_dpmi_xmem_record_adapter_valid(adapter))
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT;
  for (index = 0u; index < BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS; ++index) {
    if (adapter->records[index].record_id != 0u && adapter->records[index].owner == owner &&
        bx_ntvdm_dpmi_xmem_record_adapter_release(adapter,
          adapter->records[index].record_id) != BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK)
      return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE;
  }
  return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK;
}

uint32_t bx_ntvdm_dpmi_xmem_record_adapter_reset(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter)
{
  uint32_t index;
  if (!bx_ntvdm_dpmi_xmem_record_adapter_valid(adapter))
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT;
  for (index = 0u; index < BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS; ++index)
    if (adapter->records[index].record_id != 0u &&
        bx_ntvdm_dpmi_xmem_record_adapter_release(adapter,
          adapter->records[index].record_id) != BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK)
      return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE;
  bx_ntvdm_dpmi_xmem_record_adapter_clear(adapter);
  return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK;
}
