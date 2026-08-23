#include "dpmi_xmem_record_adapter.h"

#include "bx-mantle/bx_ntvdm_ordinary_ram_reservation_v1.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

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

static uint32_t pages_for(uint32_t bytes)
{
  return ((bytes - 1u) / UINT32_C(0x1000)) + 1u;
}

static int copy_ordinary_ram(uint32_t from, uint32_t to, uint32_t bytes)
{
  uint8_t buffer[4096];
  uint32_t offset = 0u;
  while (offset < bytes) {
    uint32_t chunk = bytes - offset;
    if (chunk > sizeof(buffer)) chunk = sizeof(buffer);
    if (!bx_ntvdm_mantle_checked_ram_read_v1((uint64_t)from + offset,
        buffer, chunk) || !bx_ntvdm_mantle_checked_ram_write_v1(
        (uint64_t)to + offset, buffer, chunk)) return 0;
    offset += chunk;
  }
  return 1;
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

uint32_t bx_ntvdm_dpmi_xmem_record_adapter_reallocate(
  struct bx_ntvdm_dpmi_xmem_record_adapter *adapter, uint32_t record_id,
  uint32_t new_byte_count, uint32_t *ordinary_ram_address_out)
{
  struct bx_ntvdm_dpmi_xmem_record_adapter_record *record =
    find_record(adapter, record_id);
  struct bx_ntvdm_ordinary_ram_reservation_v1 allocation;
  struct bx_ntvdm_ordinary_ram_reservation_v1 release;
  uint32_t old_address, old_bytes, old_mantle_id;
  if (ordinary_ram_address_out != NULL) *ordinary_ram_address_out = 0u;
  if (record == NULL) return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_ID;
  if (new_byte_count == 0u) return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT;
  /* i386/dpmimem.c keeps the original allocation when page coverage does not
     grow. xmem.c nevertheless publishes the requested logical length. */
  if (pages_for(new_byte_count) <= pages_for(record->byte_count)) {
    record->byte_count = new_byte_count;
    if (ordinary_ram_address_out != NULL)
      *ordinary_ram_address_out = record->ordinary_ram_address;
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK;
  }
  bx_ntvdm_ordinary_ram_reservation_v1_clear(&allocation);
  allocation.kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_ALLOCATE;
  allocation.byte_count = new_byte_count;
  allocation.alignment_bytes = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_MAX_ALIGNMENT;
  if (bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(&allocation) !=
      BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_OK)
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE;
  old_address = record->ordinary_ram_address;
  old_bytes = record->byte_count;
  old_mantle_id = record->mantle_opaque_id;
  if (!copy_ordinary_ram(old_address, allocation.address, old_bytes)) {
    bx_ntvdm_ordinary_ram_reservation_v1_clear(&release);
    release.kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_RELEASE;
    release.opaque_id = allocation.opaque_id;
    (void)bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(&release);
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE;
  }
  bx_ntvdm_ordinary_ram_reservation_v1_clear(&release);
  release.kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_RELEASE;
  release.opaque_id = old_mantle_id;
  if (bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(&release) !=
      BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_OK) {
    bx_ntvdm_ordinary_ram_reservation_v1_clear(&release);
    release.kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_RELEASE;
    release.opaque_id = allocation.opaque_id;
    (void)bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(&release);
    return BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE;
  }
  record->ordinary_ram_address = allocation.address;
  record->byte_count = new_byte_count;
  record->mantle_opaque_id = allocation.opaque_id;
  if (ordinary_ram_address_out != NULL) *ordinary_ram_address_out = allocation.address;
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
