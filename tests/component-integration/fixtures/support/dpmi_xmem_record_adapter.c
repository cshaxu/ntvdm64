#include "dpmi_xmem_record_adapter.h"

#include "adapter-softpc/ordinary_ram_reservation.h"
#include "adapter-softpc/mechanical_action.h"

#include <string.h>

static struct runtime_dpmi_xmem_record_adapter_record *find_record(
  struct runtime_dpmi_xmem_record_adapter *adapter, uint32_t record_id)
{
  uint32_t index;
  if (!runtime_dpmi_xmem_record_adapter_valid(adapter) || record_id == 0u)
    return NULL;
  for (index = 0u; index < RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS; ++index)
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
    if (!runtime_machine_checked_ram_read((uint64_t)from + offset,
        buffer, chunk) || !runtime_machine_checked_ram_write(
        (uint64_t)to + offset, buffer, chunk)) return 0;
    offset += chunk;
  }
  return 1;
}

void runtime_dpmi_xmem_record_adapter_clear(
  struct runtime_dpmi_xmem_record_adapter *adapter)
{
  if (adapter == NULL) return;
  memset(adapter, 0, sizeof(*adapter));
  adapter->magic = RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAGIC;
  adapter->abi_version = RUNTIME_DPMI_XMEM_RECORD_ADAPTER_VERSION;
  adapter->struct_bytes = sizeof(*adapter);
  adapter->next_record_id = 1u;
}

int runtime_dpmi_xmem_record_adapter_valid(
  const struct runtime_dpmi_xmem_record_adapter *adapter)
{
  return adapter != NULL &&
    adapter->magic == RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAGIC &&
    adapter->abi_version == RUNTIME_DPMI_XMEM_RECORD_ADAPTER_VERSION &&
    adapter->struct_bytes == sizeof(*adapter) &&
    adapter->next_record_id != 0u &&
    adapter->record_count <= RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS;
}

uint32_t runtime_dpmi_xmem_record_adapter_allocate(
  struct runtime_dpmi_xmem_record_adapter *adapter, uint16_t owner,
  uint32_t byte_count, uint32_t *ordinary_ram_address_out,
  uint32_t *record_id_out)
{
  uint32_t index;
  struct runtime_ordinary_ram_reservation action;
  if (ordinary_ram_address_out != NULL) *ordinary_ram_address_out = 0u;
  if (record_id_out != NULL) *record_id_out = 0u;
  if (!runtime_dpmi_xmem_record_adapter_valid(adapter) || byte_count == 0u)
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT;
  if (adapter->record_count == RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS)
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_CAPACITY;
  for (index = 0u; index < RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS; ++index)
    if (adapter->records[index].record_id == 0u) break;
  if (index == RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS)
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_CAPACITY;
  runtime_ordinary_ram_reservation_clear(&action);
  action.kind = RUNTIME_ORDINARY_RAM_RESERVATION_ALLOCATE;
  action.byte_count = byte_count;
  /* xmem.c documents 64 KiB alignment; retain it at the adapter boundary. */
  action.alignment_bytes = RUNTIME_ORDINARY_RAM_RESERVATION_MAX_ALIGNMENT;
  if (runtime_machine_execute_ordinary_ram_reservation(&action) !=
      RUNTIME_ORDINARY_RAM_RESERVATION_OK)
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MACHINE_FAILURE;
  if (adapter->next_record_id == 0u) adapter->next_record_id = 1u;
  adapter->records[index].record_id = adapter->next_record_id++;
  adapter->records[index].ordinary_ram_address = action.address;
  adapter->records[index].byte_count = action.byte_count;
  adapter->records[index].machine_opaque_id = action.opaque_id;
  adapter->records[index].owner = owner;
  ++adapter->record_count;
  if (ordinary_ram_address_out != NULL) *ordinary_ram_address_out = action.address;
  if (record_id_out != NULL) *record_id_out = adapter->records[index].record_id;
  return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK;
}

uint32_t runtime_dpmi_xmem_record_adapter_release(
  struct runtime_dpmi_xmem_record_adapter *adapter, uint32_t record_id)
{
  struct runtime_dpmi_xmem_record_adapter_record *record =
    find_record(adapter, record_id);
  struct runtime_ordinary_ram_reservation action;
  if (record == NULL) return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_ID;
  runtime_ordinary_ram_reservation_clear(&action);
  action.kind = RUNTIME_ORDINARY_RAM_RESERVATION_RELEASE;
  action.opaque_id = record->machine_opaque_id;
  if (runtime_machine_execute_ordinary_ram_reservation(&action) !=
      RUNTIME_ORDINARY_RAM_RESERVATION_OK)
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MACHINE_FAILURE;
  memset(record, 0, sizeof(*record));
  --adapter->record_count;
  return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK;
}

uint32_t runtime_dpmi_xmem_record_adapter_reallocate(
  struct runtime_dpmi_xmem_record_adapter *adapter, uint32_t record_id,
  uint32_t new_byte_count, uint32_t *ordinary_ram_address_out)
{
  struct runtime_dpmi_xmem_record_adapter_record *record =
    find_record(adapter, record_id);
  struct runtime_ordinary_ram_reservation allocation;
  struct runtime_ordinary_ram_reservation release;
  uint32_t old_address, old_bytes, old_machine_id;
  if (ordinary_ram_address_out != NULL) *ordinary_ram_address_out = 0u;
  if (record == NULL) return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_ID;
  if (new_byte_count == 0u) return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT;
  /* i386/dpmimem.c keeps the original allocation when page coverage does not
     grow. xmem.c nevertheless publishes the requested logical length. */
  if (pages_for(new_byte_count) <= pages_for(record->byte_count)) {
    record->byte_count = new_byte_count;
    if (ordinary_ram_address_out != NULL)
      *ordinary_ram_address_out = record->ordinary_ram_address;
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK;
  }
  runtime_ordinary_ram_reservation_clear(&allocation);
  allocation.kind = RUNTIME_ORDINARY_RAM_RESERVATION_ALLOCATE;
  allocation.byte_count = new_byte_count;
  allocation.alignment_bytes = RUNTIME_ORDINARY_RAM_RESERVATION_MAX_ALIGNMENT;
  if (runtime_machine_execute_ordinary_ram_reservation(&allocation) !=
      RUNTIME_ORDINARY_RAM_RESERVATION_OK)
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MACHINE_FAILURE;
  old_address = record->ordinary_ram_address;
  old_bytes = record->byte_count;
  old_machine_id = record->machine_opaque_id;
  if (!copy_ordinary_ram(old_address, allocation.address, old_bytes)) {
    runtime_ordinary_ram_reservation_clear(&release);
    release.kind = RUNTIME_ORDINARY_RAM_RESERVATION_RELEASE;
    release.opaque_id = allocation.opaque_id;
    (void)runtime_machine_execute_ordinary_ram_reservation(&release);
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MACHINE_FAILURE;
  }
  runtime_ordinary_ram_reservation_clear(&release);
  release.kind = RUNTIME_ORDINARY_RAM_RESERVATION_RELEASE;
  release.opaque_id = old_machine_id;
  if (runtime_machine_execute_ordinary_ram_reservation(&release) !=
      RUNTIME_ORDINARY_RAM_RESERVATION_OK) {
    runtime_ordinary_ram_reservation_clear(&release);
    release.kind = RUNTIME_ORDINARY_RAM_RESERVATION_RELEASE;
    release.opaque_id = allocation.opaque_id;
    (void)runtime_machine_execute_ordinary_ram_reservation(&release);
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MACHINE_FAILURE;
  }
  record->ordinary_ram_address = allocation.address;
  record->byte_count = new_byte_count;
  record->machine_opaque_id = allocation.opaque_id;
  if (ordinary_ram_address_out != NULL) *ordinary_ram_address_out = allocation.address;
  return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK;
}

uint32_t runtime_dpmi_xmem_record_adapter_release_owner(
  struct runtime_dpmi_xmem_record_adapter *adapter, uint16_t owner)
{
  uint32_t index;
  if (!runtime_dpmi_xmem_record_adapter_valid(adapter))
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT;
  for (index = 0u; index < RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS; ++index) {
    if (adapter->records[index].record_id != 0u && adapter->records[index].owner == owner &&
        runtime_dpmi_xmem_record_adapter_release(adapter,
          adapter->records[index].record_id) != RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK)
      return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MACHINE_FAILURE;
  }
  return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK;
}

uint32_t runtime_dpmi_xmem_record_adapter_reset(
  struct runtime_dpmi_xmem_record_adapter *adapter)
{
  uint32_t index;
  if (!runtime_dpmi_xmem_record_adapter_valid(adapter))
    return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_INPUT;
  for (index = 0u; index < RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MAX_RECORDS; ++index)
    if (adapter->records[index].record_id != 0u &&
        runtime_dpmi_xmem_record_adapter_release(adapter,
          adapter->records[index].record_id) != RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK)
      return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MACHINE_FAILURE;
  runtime_dpmi_xmem_record_adapter_clear(adapter);
  return RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK;
}
