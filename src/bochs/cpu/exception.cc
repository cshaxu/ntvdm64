/////////////////////////////////////////////////////////////////////////
// $Id: exception.cc 11330 2012-08-09 13:11:25Z sshwarts $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#ifndef BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT
#define BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT 0
#endif

#ifndef BX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC
#define BX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC 0
#endif

#ifndef BX_NTVDM_ENABLE_STARTUP_TRANSACTION
#define BX_NTVDM_ENABLE_STARTUP_TRANSACTION 0
#endif

#ifndef BX_NTVDM_ENABLE_UNMATCHED_UD_DIAGNOSTIC
#define BX_NTVDM_ENABLE_UNMATCHED_UD_DIAGNOSTIC 0
#endif

#ifndef BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER
#define BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER 0
#endif

#ifndef BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE
#define BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE 0
#endif

#ifndef BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN
#define BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN 0
#endif

#ifndef BX_NTVDM_ENABLE_MACHINE_COMPOSITION
#define BX_NTVDM_ENABLE_MACHINE_COMPOSITION 0
#endif

#if BX_NTVDM_ENABLE_MACHINE_COMPOSITION
#include "iodev/iodev.h"
#include "bx_ntvdm_machine_composition_seam.h"

static int bx_ntvdm_machine_composition_read8(void *, uint16_t port,
  uint8_t *value)
{
  if (value == 0) return 0;
  *value = (uint8_t) bx_devices.inp((Bit16u) port, 1);
  return 1;
}

static int bx_ntvdm_machine_composition_write8(void *, uint16_t port,
  uint8_t value)
{
  bx_devices.outp((Bit16u) port, (Bit32u) value, 1);
  return 1;
}

static int bx_ntvdm_machine_composition_store8(void *, uint32_t address,
  uint8_t value)
{
  Bit8u byte = (Bit8u) value;
  return BX_MEM(0)->copy_to_ordinary_ram((bx_phy_address) address, 1, &byte);
}

static int bx_ntvdm_machine_composition_load8(void *, uint32_t address,
  uint8_t *value)
{
  Bit8u byte;
  if (value == 0 || !BX_MEM(0)->copy_from_ordinary_ram(
      (bx_phy_address) address, 1, &byte)) return 0;
  *value = (uint8_t) byte;
  return 1;
}

static void bx_ntvdm_machine_composition_report(void *, uint32_t error_number,
  uint32_t options, const char *message)
{
  BX_INFO(("machine composition report error=%u options=%u %s",
    (unsigned) error_number, (unsigned) options,
    message == 0 ? "" : message));
}

static bx_ntvdm_machine_bop_v1_result bx_ntvdm_machine_composition_probe(unsigned cpu_id,
  unsigned vector, Bit16u error_code, Bit64u fault_rip,
  const bx_ntvdm_cpu_state_v1 *cpu_state,
  const bx_ntvdm_instruction_window_v1 *instruction_window)
{
  bx_ntvdm_exception_event_v1 event;
  bx_ntvdm_machine_composition_mechanics mechanics;
  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = cpu_id;
  event.vector = vector;
  event.error_code = error_code;
  event.reserved0 = 0;
  event.fault_rip = fault_rip;
  bx_ntvdm_machine_composition_initialize_mechanics(&mechanics);
  mechanics.read8 = bx_ntvdm_machine_composition_read8;
  mechanics.write8 = bx_ntvdm_machine_composition_write8;
  mechanics.store8 = bx_ntvdm_machine_composition_store8;
  mechanics.load8 = bx_ntvdm_machine_composition_load8;
  mechanics.report = bx_ntvdm_machine_composition_report;
  mechanics.execution_mode = cpu_state->execution_mode;
  mechanics.esp = cpu_state->esp;
  mechanics.ss = cpu_state->ss;
  return bx_ntvdm_machine_composition_v1_probe(&event, cpu_state,
    instruction_window, &mechanics);
}
#endif

#if BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN
#include "bx_ntvdm_adapter_runtime.h"
#include "bx_ntvdm_exception_intercept.h"

static bx_bool bx_ntvdm_deferred_startup_plan_consumer(BX_CPU_C *cpu,
  unsigned cpu_id,
  unsigned vector, Bit16u error_code, Bit64u fault_rip,
  const bx_ntvdm_cpu_state_v1 *cpu_state,
  const bx_ntvdm_instruction_window_v1 *instruction_window,
  bx_ntvdm_exception_response *response)
{
  bx_ntvdm_exception_event_v1 event;
  bx_ntvdm_startup_plan_v1 plan;
  const uint8_t *payload = NULL;
  uint64_t payload_bytes = 0;
  Bit8u preserved[BX_NTVDM_STARTUP_PLAN_MAX_PRESERVED_BYTES];
  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = cpu_id;
  event.vector = vector;
  event.error_code = error_code;
  event.reserved0 = 0;
  event.fault_rip = fault_rip;
  if (cpu_state == NULL || instruction_window == NULL ||
      !bx_ntvdm_adapter_runtime_v1_take_deferred_startup_plan(&event,
        cpu_state, instruction_window, &plan, &payload, &payload_bytes) ||
      !bx_ntvdm_startup_plan_v1_preflight(&plan, 0x100000ULL, payload_bytes) ||
      payload == NULL || !BX_MEM(0)->ordinary_ram_readable(
        (bx_phy_address)plan.preserved_state_address,
        (Bit64u)plan.preserved_state_bytes) ||
      !BX_MEM(0)->copy_from_ordinary_ram(
        (bx_phy_address)plan.preserved_state_address,
        (Bit64u)plan.preserved_state_bytes, preserved) ||
      !BX_MEM(0)->copy_to_ordinary_ram(
        (bx_phy_address)plan.payload_write.guest_physical_address,
        (Bit64u)payload_bytes, (const Bit8u *)payload) ||
      !BX_MEM(0)->copy_to_ordinary_ram(
        (bx_phy_address)plan.preserved_state_address,
        (Bit64u)plan.preserved_state_bytes, preserved)) return 0;
  cpu->apply_real_mode_entry(plan.entry_cpu.cs,
    (Bit32u)plan.entry_cpu.eip);
  response->abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
  response->disposition = BX_NTVDM_EXCEPTION_RESUME;
  response->resume_rip = plan.entry_cpu.eip;
  return 1;
}
#endif

#if BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_bop_catalog_v1.h"

static void bx_ntvdm_bop_catalog_listener(unsigned cpu_id, unsigned vector,
  Bit16u error_code, Bit64u fault_rip,
  const bx_ntvdm_cpu_state_v1 *cpu_state,
  const bx_ntvdm_instruction_window_v1 *instruction_window)
{
  bx_ntvdm_exception_event_v1 event;
  bx_ntvdm_bop_catalog_v1_identity identity;
  Bit8u stack_frame[4] = { 0, 0, 0, 0 };
  Bit8u frame_code[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  Bit32u stack_address = 0;
  Bit32u frame_address = 0;
  unsigned stack_valid = 0;
  unsigned frame_valid = 0;
  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = cpu_id;
  event.vector = vector;
  event.error_code = error_code;
  event.reserved0 = 0;
  event.fault_rip = fault_rip;
  if (cpu_state != 0 && instruction_window != 0 &&
      bx_ntvdm_bop_catalog_v1_observe(&event, cpu_state,
        instruction_window, &identity) && identity.is_bop) {
    if (cpu_state->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        (cpu_state->esp & 0xffffu) <= 0xfffcu) {
      stack_address = ((Bit32u)cpu_state->ss << 4) +
        (Bit32u)(cpu_state->esp & 0xffffu);
      if (stack_address <= 0xffffcu &&
          BX_MEM(0)->ordinary_ram_readable((bx_phy_address)stack_address, 4) &&
          BX_MEM(0)->copy_from_ordinary_ram((bx_phy_address)stack_address, 4,
            stack_frame)) {
        stack_valid = 1;
        frame_address = (((Bit32u)stack_frame[2] +
          ((Bit32u)stack_frame[3] << 8)) << 4) + (Bit32u)stack_frame[0] +
          ((Bit32u)stack_frame[1] << 8);
        if (frame_address <= 0xffff8u &&
            BX_MEM(0)->ordinary_ram_readable((bx_phy_address)frame_address, 8) &&
            BX_MEM(0)->copy_from_ordinary_ram((bx_phy_address)frame_address, 8,
              frame_code)) frame_valid = 1;
      }
    }
    BX_INFO(("ntdos64 adapter bop observed cpu=%u rip=%llx selector=%02x class=%u family=%u service-state=%u service=%02x ax=%04x bx=%04x cs=%04x ds=%04x dx=%04x si=%04x di=%04x cx=%04x flags=%08x ss=%04x sp=%04x stack-valid=%u stack=%02x%02x%02x%02x frame-valid=%u frame=%02x%02x%02x%02x%02x%02x%02x%02x",
      cpu_id, (unsigned long long)fault_rip,
      identity.selector, identity.source_class, identity.family,
      identity.service_state, identity.service, (unsigned)(cpu_state->eax & 0xffffu),
      (unsigned)(cpu_state->ebx & 0xffffu), (unsigned)cpu_state->cs,
      (unsigned)cpu_state->ds, (unsigned)(cpu_state->edx & 0xffffu),
      (unsigned)(cpu_state->esi & 0xffffu),
      (unsigned)(cpu_state->edi & 0xffffu), (unsigned)(cpu_state->ecx & 0xffffu),
      (unsigned)cpu_state->eflags, (unsigned)cpu_state->ss,
      (unsigned)(cpu_state->esp & 0xffffu), stack_valid,
      (unsigned)stack_frame[0], (unsigned)stack_frame[1],
      (unsigned)stack_frame[2], (unsigned)stack_frame[3], frame_valid,
      (unsigned)frame_code[0], (unsigned)frame_code[1],
      (unsigned)frame_code[2], (unsigned)frame_code[3],
      (unsigned)frame_code[4], (unsigned)frame_code[5],
      (unsigned)frame_code[6], (unsigned)frame_code[7]));
  }
}
#endif

#if BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE
#include "bx_ntvdm_exception_intercept.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_cpu_result_bridge_v1.h"
#include "bx_ntvdm_guest_read_action_v1.h"
#include "bx_ntvdm_startup_transaction_bridge.h"

static bx_bool bx_ntvdm_cpu_result_bridge_interceptor(
  const bx_ntvdm_exception_request *request,
  bx_ntvdm_exception_response *response)
{
  bx_ntvdm_exception_event_v1 event;
  bx_ntvdm_cpu_result_v2 result;
  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = request->cpu_id;
  event.vector = request->vector;
  event.error_code = request->error_code;
  event.reserved0 = 0;
  event.fault_rip = request->fault_rip;
  if (request->cpu_state == 0 || request->instruction_window == 0 ||
      !bx_ntvdm_cpu_result_bridge_v1_dispatch(&event, request->cpu_state,
        request->instruction_window, &result) ||
      result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
      !bx_ntvdm_cpu_result_v2_valid(&result)) return 0;
  response->abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
  response->disposition = BX_NTVDM_EXCEPTION_RESUME;
  response->resume_rip = result.resume_rip;
  response->cpu_result = result;
  return 1;
}

static bx_bool bx_ntvdm_cpu_result_bridge_guest_read_interceptor(
  const bx_ntvdm_exception_request *request,
  bx_ntvdm_exception_response *response)
{
  bx_ntvdm_exception_event_v1 event;
  bx_ntvdm_guest_read_action_v1 action;
  bx_ntvdm_cpu_result_v2 result;
  Bit8u bytes[BX_NTVDM_GUEST_READ_ACTION_V1_MAX_BYTES];
  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = request->cpu_id;
  event.vector = request->vector;
  event.error_code = request->error_code;
  event.reserved0 = 0;
  event.fault_rip = request->fault_rip;
  if (request->cpu_state == 0 || request->instruction_window == 0 ||
      !bx_ntvdm_cpu_result_bridge_v1_dispatch_guest_read(&event,
        request->cpu_state, request->instruction_window, &action) ||
      !bx_ntvdm_guest_read_action_v1_valid(&action) ||
      action.disposition != BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ ||
      action.guest_read.length > sizeof(bytes) ||
      !BX_MEM(0)->copy_from_ordinary_ram(
        (bx_phy_address) action.guest_read.address,
        (Bit64u) action.guest_read.length, bytes) ||
      !bx_ntvdm_cpu_result_bridge_v1_complete_guest_read(&event,
        request->cpu_state, &action, bytes, action.guest_read.length, &result) ||
      result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
      !bx_ntvdm_cpu_result_v2_valid(&result)) return 0;
  response->abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
  response->disposition = BX_NTVDM_EXCEPTION_RESUME;
  response->resume_rip = result.resume_rip;
  response->cpu_result = result;
  return 1;
}

static bx_bool bx_ntvdm_cpu_result_bridge_multi_write_interceptor(
  const bx_ntvdm_exception_request *request,
  bx_ntvdm_exception_response *response)
{
  bx_ntvdm_exception_event_v1 event;
  bx_ntvdm_multi_write_transaction_v1 transaction;
  const uint8_t *payload;
  uint64_t payload_bytes;
  unsigned write_index;
  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = request->cpu_id;
  event.vector = request->vector;
  event.error_code = request->error_code;
  event.reserved0 = 0;
  event.fault_rip = request->fault_rip;
  if (request->cpu_state == 0 || request->instruction_window == 0 ||
      !bx_ntvdm_startup_transaction_bridge_v1_prepare_multi_write(&event,
        request->cpu_state, request->instruction_window) ||
      !bx_ntvdm_startup_transaction_bridge_v1_take_pending_multi_write(&event,
        request->cpu_state, &transaction, &payload, &payload_bytes)) return 0;
  for (write_index = 0; write_index < transaction.writes.write_count;
      ++write_index) {
    const bx_ntvdm_guest_write_v1 *write = &transaction.writes.writes[write_index];
    if (!BX_MEM(0)->ordinary_ram_writable(
          (bx_phy_address)write->guest_physical_address,
          (Bit64u)write->byte_count)) return 0;
  }
  for (write_index = 0; write_index < transaction.writes.write_count;
      ++write_index) {
    const bx_ntvdm_guest_write_v1 *write = &transaction.writes.writes[write_index];
    if (!BX_MEM(0)->copy_to_ordinary_ram(
          (bx_phy_address)write->guest_physical_address,
          (Bit64u)write->byte_count,
          (const Bit8u *)(payload + write->payload_offset))) return 0;
  }
  response->abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
  response->disposition = BX_NTVDM_EXCEPTION_RESUME;
  response->resume_rip = transaction.result.resume_rip;
  response->cpu_result = transaction.result;
  return 1;
}
#endif

#if BX_NTVDM_ENABLE_STARTUP_TRANSACTION
#include "bx_ntvdm_exception_intercept.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_startup_transaction_bridge.h"

static bx_bool bx_ntvdm_startup_transaction_interceptor(
  const bx_ntvdm_exception_request *request,
  bx_ntvdm_exception_response *response)
{
  bx_ntvdm_exception_event_v1 event;
  bx_ntvdm_exception_result_v1 result;
  bx_ntvdm_host_transaction_v1 transaction;
  const uint8_t *payload;
  uint64_t payload_bytes;
  uint64_t resume_rip;

  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = request->cpu_id;
  event.vector = request->vector;
  event.error_code = request->error_code;
  event.reserved0 = 0;
  event.fault_rip = request->fault_rip;
  if (request->cpu_state == 0 || request->instruction_window == 0 ||
      !bx_ntvdm_startup_transaction_bridge_v1_dispatch(&event,
        request->cpu_state, request->instruction_window, &result) ||
      result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
      !bx_ntvdm_startup_transaction_bridge_v1_take_pending_write(&event,
        request->cpu_state, &transaction, &payload, &payload_bytes,
        &resume_rip) || resume_rip != result.resume_rip ||
      transaction.guest_write.byte_count != payload_bytes ||
      !BX_MEM(0)->copy_to_ordinary_ram(
        (bx_phy_address) transaction.guest_write.guest_physical_address,
        (Bit64u) payload_bytes, (const Bit8u *) payload)) return 0;
  response->abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
  response->disposition = BX_NTVDM_EXCEPTION_RESUME;
  response->resume_rip = result.resume_rip;
  if (!bx_ntvdm_cpu_result_v2_resume(&response->cpu_result,
      result.resume_rip)) return 0;
  BX_INFO(("guest transaction committed cs=%04x eip=%08x address=%llx bytes=%llx resume=%llx",
    request->cpu_state->cs, request->cpu_state->eip,
    (unsigned long long) transaction.guest_write.guest_physical_address,
    (unsigned long long) payload_bytes, (unsigned long long) result.resume_rip));
  return 1;
}
#endif

#if BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT
#include "bx_ntvdm_exception_intercept.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_adapter_runtime.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"

static bx_ntvdm_exception_interceptor_t bx_ntvdm_exception_interceptor = 0;

static bx_bool bx_ntvdm_ud_test_interceptor(
  const bx_ntvdm_exception_request *request,
  bx_ntvdm_exception_response *response)
{
  response->abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
  response->disposition = BX_NTVDM_EXCEPTION_RESUME;
  response->resume_rip = request->fault_rip + 2;
  BX_INFO(("ntdos64 #UD test interceptor resumed execution"));
  return 1;
}

static bx_bool bx_ntvdm_adapter_interceptor(
  const bx_ntvdm_exception_request *request,
  bx_ntvdm_exception_response *response)
{
  bx_ntvdm_exception_event_v1 event;
  bx_ntvdm_cpu_result_v2 result;
  bx_ntvdm_guest_read_action_v1 action;
  bx_ntvdm_guest_gather_read_action_v1 gather_action;
  bx_ntvdm_host_transaction_v1 transaction;
  bx_ntvdm_multi_write_transaction_v1 multi_write;
  bx_ntvdm_bulk_result_transaction_v1 bulk_result;
  bx_ntvdm_observation_transaction_v1 observation;
  bx_ntvdm_startup_snapshot_transaction_v1 startup_snapshot;
  const uint8_t *payload;
  uint8_t *observation_output;
  uint8_t *guest_read_bytes;
  uint64_t payload_bytes;
  uint64_t observation_output_bytes;
  uint64_t observation_digest;
  uint64_t startup_snapshot_digest;
  uint64_t resume_rip;
  uint32_t write_index;
  uint32_t range_index;
  const char *trace_mode = getenv("NTDOS64_ADAPTER_TRACE");

  event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
  event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
  event.struct_bytes = sizeof(event);
  event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
  event.cpu_id = request->cpu_id;
  event.vector = request->vector;
  event.error_code = request->error_code;
  event.reserved0 = 0;
  event.fault_rip = request->fault_rip;

  if (request->cpu_state == 0 || request->instruction_window == 0 ||
      !bx_ntvdm_adapter_runtime_v4_dispatch(&event, request->cpu_state,
        request->instruction_window, &gather_action) ||
      !bx_ntvdm_guest_gather_read_action_v1_valid(&gather_action)) return 0;
  result = gather_action.cpu_result;
  if (gather_action.disposition == BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ) {
      uint64_t gather_offset = 0;
      if (!bx_ntvdm_adapter_runtime_v4_take_pending_guest_gather_read(&event,
            request->cpu_state, &gather_action, &guest_read_bytes)) return 0;
      for (range_index = 0; range_index < gather_action.range_count;
          ++range_index) {
        const bx_ntvdm_guest_range *range = &gather_action.ranges[range_index];
        if (!BX_MEM(0)->ordinary_ram_readable((bx_phy_address)range->address,
              (Bit64u)range->length)) return 0;
      }
      for (range_index = 0; range_index < gather_action.range_count;
          ++range_index) {
        const bx_ntvdm_guest_range *range = &gather_action.ranges[range_index];
        if (!BX_MEM(0)->copy_from_ordinary_ram((bx_phy_address)range->address,
              (Bit64u)range->length, (Bit8u *)(guest_read_bytes + gather_offset))) return 0;
        gather_offset += range->length;
      }
      if (gather_offset != gather_action.total_bytes ||
          !bx_ntvdm_adapter_runtime_v4_complete_guest_gather_read(&event,
            request->cpu_state, &gather_action,
            (const uint8_t *)guest_read_bytes, gather_offset, &result)) return 0;
      BX_INFO(("ntdos64 adapter gather copied ranges=%u bytes=%u",
        (unsigned)gather_action.range_count, (unsigned)gather_action.total_bytes));
  } else if (gather_action.disposition ==
      BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_PASS_THROUGH) {
    if (!bx_ntvdm_adapter_runtime_v3_dispatch(&event, request->cpu_state,
          request->instruction_window, &action) ||
        !bx_ntvdm_guest_read_action_v1_valid(&action)) return 0;
    result = action.cpu_result;
    if (action.disposition == BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ) {
      if (!bx_ntvdm_adapter_runtime_v3_take_pending_guest_read(&event,
          request->cpu_state, &action, &guest_read_bytes) ||
          !BX_MEM(0)->copy_from_ordinary_ram((bx_phy_address)action.guest_read.address,
            (Bit64u)action.guest_read.length, (Bit8u *)guest_read_bytes) ||
          !bx_ntvdm_adapter_runtime_v3_complete_guest_read(&event,
            request->cpu_state, &action, (const uint8_t *)guest_read_bytes,
            action.guest_read.length, &result)) return 0;
    }
  } else return 0;
  if (!bx_ntvdm_cpu_result_v2_valid(&result))
    return 0;
  if (result.disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH) {
#if BX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC
      bx_ntvdm_adapter_runtime_diagnostic_state_v1 diagnostic_state;
      if (bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state(&diagnostic_state))
        BX_INFO(("ntdos64 adapter lifecycle installed=%u provider=%u pending=%u",
          (unsigned)diagnostic_state.installed,
          (unsigned)diagnostic_state.has_boot_namespace_provider,
          (unsigned)diagnostic_state.pending_kind));
#endif
      BX_INFO(("ntdos64 adapter boundary passed through cs=%04x eip=%08x rip=%llx bytes=%02x%02x%02x%02x",
        request->cpu_state->cs, request->cpu_state->eip,
        (unsigned long long)request->fault_rip,
        request->instruction_window->bytes[0], request->instruction_window->bytes[1],
        request->instruction_window->bytes[2], request->instruction_window->bytes[3]));
      return 0;
  }
  if (result.disposition == BX_NTVDM_CPU_RESULT_V2_STOP) {
    response->abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
    response->disposition = BX_NTVDM_EXCEPTION_STOP;
    response->resume_rip = 0;
    response->cpu_result = result;
    BX_INFO(("ntdos64 adapter controlled stop accepted"));
    return 1;
  }
  if (result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME)
    return 0;

  if (bx_ntvdm_adapter_runtime_v1_take_pending_write(&event, request->cpu_state,
      &transaction, &payload,
      &payload_bytes, &resume_rip)) {
    if (resume_rip != result.resume_rip ||
        transaction.guest_write.byte_count != payload_bytes ||
        !BX_MEM(0)->copy_to_ordinary_ram(
          (bx_phy_address)transaction.guest_write.guest_physical_address,
          (Bit64u)payload_bytes, (const Bit8u *)payload)) return 0;
    BX_INFO(("ntdos64 adapter transaction committed address=%llx bytes=%llx",
      (unsigned long long)transaction.guest_write.guest_physical_address,
      (unsigned long long)payload_bytes));
  }

  if (bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event,
      request->cpu_state, &multi_write, &payload, &payload_bytes)) {
    if (multi_write.result.resume_rip != result.resume_rip) return 0;
    for (write_index = 0; write_index < multi_write.writes.write_count;
        ++write_index) {
      const bx_ntvdm_guest_write_v1 *write =
        &multi_write.writes.writes[write_index];
      if (!BX_MEM(0)->ordinary_ram_writable(
          (bx_phy_address)write->guest_physical_address,
          (Bit64u)write->byte_count)) return 0;
    }
    for (write_index = 0; write_index < multi_write.writes.write_count;
        ++write_index) {
      const bx_ntvdm_guest_write_v1 *write =
        &multi_write.writes.writes[write_index];
      if (!BX_MEM(0)->copy_to_ordinary_ram(
          (bx_phy_address)write->guest_physical_address,
          (Bit64u)write->byte_count,
          (const Bit8u *)(payload + write->payload_offset))) return 0;
    }
    BX_INFO(("ntdos64 adapter multi-write transaction committed writes=%u bytes=%llx",
      (unsigned)multi_write.writes.write_count,
      (unsigned long long)payload_bytes));
  }

  if (bx_ntvdm_adapter_runtime_v1_take_pending_bulk_result(&event,
      request->cpu_state, &bulk_result, &payload, &payload_bytes)) {
    if (bulk_result.result.resume_rip != result.resume_rip ||
        bulk_result.payload_bytes != payload_bytes ||
        (payload_bytes != 0 &&
         (!BX_MEM(0)->ordinary_ram_writable(
            (bx_phy_address)bulk_result.guest_physical_address,
            (Bit64u)payload_bytes) ||
          !BX_MEM(0)->copy_to_ordinary_ram(
            (bx_phy_address)bulk_result.guest_physical_address,
            (Bit64u)payload_bytes, (const Bit8u *)payload)))) return 0;
    BX_INFO(("ntdos64 adapter bulk transaction committed bytes=%llx",
      (unsigned long long)payload_bytes));
  }

  if (bx_ntvdm_adapter_runtime_v1_take_pending_observation(&event,
      request->cpu_state, &observation, &observation_output,
      &observation_output_bytes)) {
    if (!BX_MEM(0)->copy_from_ordinary_ram(
          (bx_phy_address)observation.guest_read.address,
          (Bit64u)observation_output_bytes,
          (Bit8u *)observation_output)) return 0;
    if (!bx_ntvdm_adapter_runtime_v1_complete_profile_observation(
          &observation, (const uint8_t *)observation_output,
          observation_output_bytes, &observation_digest)) return 0;
    BX_INFO(("ntdos64 adapter observation copied digest=%llx",
      (unsigned long long)observation_digest));
  }

  if (bx_ntvdm_adapter_runtime_v1_take_pending_startup_snapshot(&event,
      request->cpu_state, &startup_snapshot, &observation_output,
      &observation_output_bytes)) {
    uint32_t range_index;
    uint64_t output_offset = 0;
    for (range_index = 0; range_index < startup_snapshot.range_count;
        ++range_index) {
      if (!BX_MEM(0)->ordinary_ram_readable(
          (bx_phy_address)startup_snapshot.ranges[range_index].guest_read.address,
          (Bit64u)startup_snapshot.ranges[range_index].guest_read.length)) return 0;
    }
    for (range_index = 0; range_index < startup_snapshot.range_count;
        ++range_index) {
      Bit64u range_bytes =
        (Bit64u)startup_snapshot.ranges[range_index].guest_read.length;
      if (!BX_MEM(0)->copy_from_ordinary_ram(
          (bx_phy_address)startup_snapshot.ranges[range_index].guest_read.address,
          range_bytes, (Bit8u *)(observation_output + output_offset))) return 0;
      output_offset += range_bytes;
    }
    if (output_offset != observation_output_bytes ||
        !bx_ntvdm_adapter_runtime_v1_complete_profile_startup_snapshot(
          &startup_snapshot, (const uint8_t *)observation_output,
          observation_output_bytes, &startup_snapshot_digest)) return 0;
    BX_INFO(("ntdos64 adapter snapshot copied digest=%llx",
      (unsigned long long)startup_snapshot_digest));
  }

  response->abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
  response->disposition = BX_NTVDM_EXCEPTION_RESUME;
  response->resume_rip = result.resume_rip;
  response->cpu_result = result;
  if (trace_mode != 0 && !strcmp(trace_mode, "1")) {
    BX_INFO(("ntdos64 adapter accepted resume cs=%04x eip=%08x ss=%04x esp=%08x ds=%04x es=%04x fs=%04x gs=%04x eax=%08x ebx=%08x ecx=%08x edx=%08x esi=%08x edi=%08x ebp=%08x flags=%08x rip=%llx next=%llx delta=%02x",
      request->cpu_state->cs, request->cpu_state->eip, request->cpu_state->ss,
      request->cpu_state->esp, request->cpu_state->ds, request->cpu_state->es,
      request->cpu_state->fs, request->cpu_state->gs, request->cpu_state->eax,
      request->cpu_state->ebx, request->cpu_state->ecx, request->cpu_state->edx,
      request->cpu_state->esi, request->cpu_state->edi, request->cpu_state->ebp,
      request->cpu_state->eflags, (unsigned long long)request->fault_rip,
      (unsigned long long)result.resume_rip,
      (unsigned int)result.cpu_delta.gpr16_write_mask));
  }
  return 1;
}

static void bx_ntvdm_maybe_install_ud_test_interceptor(void)
{
  const char *test_mode = getenv("NTDOS64_UD_TEST");
  const char *adapter_mode = getenv("NTDOS64_ADAPTER_BRIDGE");

  if (adapter_mode != 0 && !strcmp(adapter_mode, "1")) {
    bx_ntvdm_exception_interceptor = bx_ntvdm_adapter_interceptor;
    return;
  }
  if (test_mode != 0 && !strcmp(test_mode, "1"))
    bx_ntvdm_exception_interceptor = bx_ntvdm_ud_test_interceptor;
}

void bx_ntvdm_set_exception_interceptor(
  bx_ntvdm_exception_interceptor_t interceptor)
{
  bx_ntvdm_exception_interceptor = interceptor;
}

static bx_bool bx_ntvdm_try_intercept_ud(
  unsigned cpu_id, unsigned vector, Bit16u error_code, Bit64u fault_rip,
  const bx_ntvdm_cpu_state_v1 *cpu_state,
  const bx_ntvdm_instruction_window_v1 *instruction_window,
  bx_ntvdm_exception_response *response)
{
  bx_ntvdm_exception_request request;

  if (vector != BX_UD_EXCEPTION || bx_ntvdm_exception_interceptor == 0)
    bx_ntvdm_maybe_install_ud_test_interceptor();

  if (vector != BX_UD_EXCEPTION || bx_ntvdm_exception_interceptor == 0)
    return 0;

  request.abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
  request.cpu_id = cpu_id;
  request.vector = vector;
  request.error_code = error_code;
  request.fault_rip = fault_rip;
  request.cpu_state = cpu_state;
  request.instruction_window = instruction_window;

  response->abi_version = 0;
  response->disposition = BX_NTVDM_EXCEPTION_PASS_THROUGH;
  response->resume_rip = 0;
  bx_ntvdm_cpu_result_v2_pass_through(&response->cpu_result);

  if (!bx_ntvdm_exception_interceptor(&request, response))
    return 0;

  if (response->abi_version != BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION ||
      (response->disposition != BX_NTVDM_EXCEPTION_RESUME &&
       response->disposition != BX_NTVDM_EXCEPTION_STOP) ||
      !bx_ntvdm_cpu_result_v2_valid(&response->cpu_result))
    return 0;

  return 1;
}
#endif

#include "param_names.h"
#include "iodev/iodev.h"

#if BX_SUPPORT_X86_64==0
// Make life easier merging cpu64 & cpu code.
#define RIP EIP
#define RSP ESP
#endif

#if BX_SUPPORT_X86_64
void BX_CPU_C::long_mode_int(Bit8u vector, unsigned soft_int, bx_bool push_error, Bit16u error_code)
{
  bx_descriptor_t gate_descriptor, cs_descriptor;
  bx_selector_t cs_selector;

  // interrupt vector must be within IDT table limits,
  // else #GP(vector*8 + 2 + EXT)
  if ((vector*16 + 15) > BX_CPU_THIS_PTR idtr.limit) {
    BX_ERROR(("interrupt(long mode): vector must be within IDT table limits, IDT.limit = 0x%x", BX_CPU_THIS_PTR idtr.limit));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  Bit64u desctmp1 = system_read_qword(BX_CPU_THIS_PTR idtr.base + vector*16);
  Bit64u desctmp2 = system_read_qword(BX_CPU_THIS_PTR idtr.base + vector*16 + 8);

  if (desctmp2 & BX_CONST64(0x00001F0000000000)) {
    BX_ERROR(("interrupt(long mode): IDT entry extended attributes DWORD4 TYPE != 0"));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  Bit32u dword1 = GET32L(desctmp1);
  Bit32u dword2 = GET32H(desctmp1);
  Bit32u dword3 = GET32L(desctmp2);

  parse_descriptor(dword1, dword2, &gate_descriptor);

  if ((gate_descriptor.valid==0) || gate_descriptor.segment)
  {
    BX_ERROR(("interrupt(long mode): gate descriptor is not valid sys seg"));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // descriptor AR byte must indicate interrupt gate, trap gate,
  // or task gate, else #GP(vector*8 + 2 + EXT)
  if (gate_descriptor.type != BX_386_INTERRUPT_GATE &&
      gate_descriptor.type != BX_386_TRAP_GATE)
  {
    BX_ERROR(("interrupt(long mode): unsupported gate type %u",
        (unsigned) gate_descriptor.type));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // if software interrupt, then gate descripor DPL must be >= CPL,
  // else #GP(vector * 8 + 2 + EXT)
  if (soft_int && gate_descriptor.dpl < CPL)
  {
    BX_ERROR(("interrupt(long mode): soft_int && gate.dpl < CPL"));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // Gate must be present, else #NP(vector * 8 + 2 + EXT)
  if (! IS_PRESENT(gate_descriptor)) {
    BX_ERROR(("interrupt(long mode): gate.p == 0"));
    exception(BX_NP_EXCEPTION, vector*8 + 2);
  }

  Bit16u gate_dest_selector = gate_descriptor.u.gate.dest_selector;
  Bit64u gate_dest_offset   = ((Bit64u)dword3 << 32) |
                       gate_descriptor.u.gate.dest_offset;

  unsigned ist = gate_descriptor.u.gate.param_count & 0x7;

  // examine CS selector and descriptor given in gate descriptor
  // selector must be non-null else #GP(EXT)
  if ((gate_dest_selector & 0xfffc) == 0) {
    BX_ERROR(("int_trap_gate(long mode): selector null"));
    exception(BX_GP_EXCEPTION, 0);
  }

  parse_selector(gate_dest_selector, &cs_selector);

  // selector must be within its descriptor table limits
  // else #GP(selector+EXT)
  fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
  parse_descriptor(dword1, dword2, &cs_descriptor);

  // descriptor AR byte must indicate code seg
  // and code segment descriptor DPL<=CPL, else #GP(selector+EXT)
  if (cs_descriptor.valid==0 || cs_descriptor.segment==0 ||
      IS_DATA_SEGMENT(cs_descriptor.type) ||
      cs_descriptor.dpl > CPL)
  {
    BX_ERROR(("interrupt(long mode): not accessible or not code segment"));
    exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
  }

  // check that it's a 64 bit segment
  if (! IS_LONG64_SEGMENT(cs_descriptor) || cs_descriptor.u.segment.d_b)
  {
    BX_ERROR(("interrupt(long mode): must be 64 bit segment"));
    exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
  }

  // segment must be present, else #NP(selector + EXT)
  if (! IS_PRESENT(cs_descriptor)) {
    BX_ERROR(("interrupt(long mode): segment not present"));
    exception(BX_NP_EXCEPTION, cs_selector.value & 0xfffc);
  }
 
  Bit64u RSP_for_cpl_x;

  Bit64u old_CS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  Bit64u old_RIP = RIP;
  Bit64u old_SS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
  Bit64u old_RSP = RSP;

  // if code segment is non-conforming and DPL < CPL then
  // INTERRUPT TO INNER PRIVILEGE:
  if (IS_CODE_SEGMENT_NON_CONFORMING(cs_descriptor.type) && cs_descriptor.dpl < CPL)
  {
    BX_DEBUG(("interrupt(long mode): INTERRUPT TO INNER PRIVILEGE"));

    // check selector and descriptor for new stack in current TSS
    if (ist > 0) {
      BX_DEBUG(("interrupt(long mode): trap to IST, vector = %d", ist));
      RSP_for_cpl_x = get_RSP_from_TSS(ist+3);
    }
    else {
      RSP_for_cpl_x = get_RSP_from_TSS(cs_descriptor.dpl);
    }

    // align stack
    RSP_for_cpl_x &= BX_CONST64(0xfffffffffffffff0);

    // push old stack long pointer onto new stack
    write_new_stack_qword_64(RSP_for_cpl_x -  8, cs_descriptor.dpl, old_SS);
    write_new_stack_qword_64(RSP_for_cpl_x - 16, cs_descriptor.dpl, old_RSP);
    write_new_stack_qword_64(RSP_for_cpl_x - 24, cs_descriptor.dpl, read_eflags());
    // push long pointer to return address onto new stack
    write_new_stack_qword_64(RSP_for_cpl_x - 32, cs_descriptor.dpl, old_CS);
    write_new_stack_qword_64(RSP_for_cpl_x - 40, cs_descriptor.dpl, old_RIP);
    RSP_for_cpl_x -= 40;

    if (push_error) {
      RSP_for_cpl_x -= 8;
      write_new_stack_qword_64(RSP_for_cpl_x, cs_descriptor.dpl, error_code);
    }

    // load CS:RIP (guaranteed to be in 64 bit mode)
    branch_far64(&cs_selector, &cs_descriptor, gate_dest_offset, cs_descriptor.dpl);

    // set up null SS descriptor
    load_null_selector(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], cs_descriptor.dpl);
  }
  else if(IS_CODE_SEGMENT_CONFORMING(cs_descriptor.type) || cs_descriptor.dpl==CPL) 
  {
    // if code segment is conforming OR code segment DPL = CPL then
    // INTERRUPT TO SAME PRIVILEGE LEVEL:

    BX_DEBUG(("interrupt(long mode): INTERRUPT TO SAME PRIVILEGE"));

    // check selector and descriptor for new stack in current TSS
    if (ist > 0) {
      BX_DEBUG(("interrupt(long mode): trap to IST, vector = %d", ist));
      RSP_for_cpl_x = get_RSP_from_TSS(ist+3);
    }
    else {
      RSP_for_cpl_x = RSP;
    }

    // align stack
    RSP_for_cpl_x &= BX_CONST64(0xfffffffffffffff0);

    // push flags onto stack
    // push current CS selector onto stack
    // push return offset onto stack
    write_new_stack_qword_64(RSP_for_cpl_x - 8,  cs_descriptor.dpl, old_SS);
    write_new_stack_qword_64(RSP_for_cpl_x - 16, cs_descriptor.dpl, old_RSP);
    write_new_stack_qword_64(RSP_for_cpl_x - 24, cs_descriptor.dpl, read_eflags());
    // push long pointer to return address onto new stack
    write_new_stack_qword_64(RSP_for_cpl_x - 32, cs_descriptor.dpl, old_CS);
    write_new_stack_qword_64(RSP_for_cpl_x - 40, cs_descriptor.dpl, old_RIP);
    RSP_for_cpl_x -= 40;

    if (push_error) {
      RSP_for_cpl_x -= 8;
      write_new_stack_qword_64(RSP_for_cpl_x, cs_descriptor.dpl, error_code);
    }

    // set the RPL field of CS to CPL
    branch_far64(&cs_selector, &cs_descriptor, gate_dest_offset, CPL);
  }
  else {
    BX_ERROR(("interrupt(long mode): bad descriptor type %u (CS.DPL=%u CPL=%u)",
      (unsigned) cs_descriptor.type, (unsigned) cs_descriptor.dpl, (unsigned) CPL));
    exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
  }

  RSP = RSP_for_cpl_x;

  // if interrupt gate then set IF to 0
  if (!(gate_descriptor.type & 1)) // even is int-gate
    BX_CPU_THIS_PTR clear_IF();
  BX_CPU_THIS_PTR clear_TF();
//BX_CPU_THIS_PTR clear_VM(); // VM is clear in long mode
  BX_CPU_THIS_PTR clear_RF();
  BX_CPU_THIS_PTR clear_NT();
}
#endif

void BX_CPU_C::protected_mode_int(Bit8u vector, unsigned soft_int, bx_bool push_error, Bit16u error_code)
{
  bx_descriptor_t gate_descriptor, cs_descriptor;
  bx_selector_t cs_selector;

  Bit16u raw_tss_selector;
  bx_selector_t   tss_selector;
  bx_descriptor_t tss_descriptor;

  Bit16u gate_dest_selector;
  Bit32u gate_dest_offset;

  // interrupt vector must be within IDT table limits,
  // else #GP(vector*8 + 2 + EXT)
  if ((vector*8 + 7) > BX_CPU_THIS_PTR idtr.limit) {
    BX_ERROR(("interrupt(): vector must be within IDT table limits, IDT.limit = 0x%x", BX_CPU_THIS_PTR idtr.limit));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  Bit64u desctmp = system_read_qword(BX_CPU_THIS_PTR idtr.base + vector*8);

  Bit32u dword1 = GET32L(desctmp);
  Bit32u dword2 = GET32H(desctmp);

  parse_descriptor(dword1, dword2, &gate_descriptor);

  if ((gate_descriptor.valid==0) || gate_descriptor.segment) {
    BX_ERROR(("interrupt(): gate descriptor is not valid sys seg (vector=0x%02x)", vector));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // descriptor AR byte must indicate interrupt gate, trap gate,
  // or task gate, else #GP(vector*8 + 2 + EXT)
  switch (gate_descriptor.type) {
  case BX_TASK_GATE:
  case BX_286_INTERRUPT_GATE:
  case BX_286_TRAP_GATE:
  case BX_386_INTERRUPT_GATE:
  case BX_386_TRAP_GATE:
    break;
  default:
    BX_ERROR(("interrupt(): gate.type(%u) != {5,6,7,14,15}",
      (unsigned) gate_descriptor.type));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // if software interrupt, then gate descripor DPL must be >= CPL,
  // else #GP(vector * 8 + 2 + EXT)
  if (soft_int && gate_descriptor.dpl < CPL) {
    BX_ERROR(("interrupt(): soft_int && (gate.dpl < CPL)"));
    exception(BX_GP_EXCEPTION, vector*8 + 2);
  }

  // Gate must be present, else #NP(vector * 8 + 2 + EXT)
  if (! IS_PRESENT(gate_descriptor)) {
    BX_ERROR(("interrupt(): gate not present"));
    exception(BX_NP_EXCEPTION, vector*8 + 2);
  }

  switch (gate_descriptor.type) {
  case BX_TASK_GATE:
    // examine selector to TSS, given in task gate descriptor
    raw_tss_selector = gate_descriptor.u.taskgate.tss_selector;
    parse_selector(raw_tss_selector, &tss_selector);

    // must specify global in the local/global bit,
    //      else #GP(TSS selector)
    if (tss_selector.ti) {
      BX_ERROR(("interrupt(): tss_selector.ti=1 from gate descriptor - #GP(tss_selector)"));
      exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    // index must be within GDT limits, else #TS(TSS selector)
    fetch_raw_descriptor(&tss_selector, &dword1, &dword2, BX_GP_EXCEPTION);

    parse_descriptor(dword1, dword2, &tss_descriptor);

    // AR byte must specify available TSS,
    //   else #GP(TSS selector)
    if (tss_descriptor.valid==0 || tss_descriptor.segment) {
      BX_ERROR(("interrupt(): TSS selector points to invalid or bad TSS - #GP(tss_selector)"));
      exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    if (tss_descriptor.type!=BX_SYS_SEGMENT_AVAIL_286_TSS &&
        tss_descriptor.type!=BX_SYS_SEGMENT_AVAIL_386_TSS)
    {
      BX_ERROR(("interrupt(): TSS selector points to bad TSS - #GP(tss_selector)"));
      exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    // TSS must be present, else #NP(TSS selector)
    if (! IS_PRESENT(tss_descriptor)) {
      BX_ERROR(("interrupt(): TSS descriptor.p == 0"));
      exception(BX_NP_EXCEPTION, raw_tss_selector & 0xfffc);
    }

    // switch tasks with nesting to TSS
    task_switch(0, &tss_selector, &tss_descriptor,
                    BX_TASK_FROM_INT, dword1, dword2, push_error, error_code);
    return;

  case BX_286_INTERRUPT_GATE:
  case BX_286_TRAP_GATE:
  case BX_386_INTERRUPT_GATE:
  case BX_386_TRAP_GATE:
    gate_dest_selector = gate_descriptor.u.gate.dest_selector;
    gate_dest_offset   = gate_descriptor.u.gate.dest_offset;

    // examine CS selector and descriptor given in gate descriptor
    // selector must be non-null else #GP(EXT)
    if ((gate_dest_selector & 0xfffc) == 0) {
      BX_ERROR(("int_trap_gate(): selector null"));
      exception(BX_GP_EXCEPTION, 0);
    }

    parse_selector(gate_dest_selector, &cs_selector);

    // selector must be within its descriptor table limits
    // else #GP(selector+EXT)
    fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &cs_descriptor);

    // descriptor AR byte must indicate code seg
    // and code segment descriptor DPL<=CPL, else #GP(selector+EXT)
    if (cs_descriptor.valid==0 || cs_descriptor.segment==0 ||
        IS_DATA_SEGMENT(cs_descriptor.type) ||
        cs_descriptor.dpl > CPL)
    {
      BX_ERROR(("interrupt(): not accessible or not code segment cs=0x%04x", cs_selector.value));
      exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
    }

    // segment must be present, else #NP(selector + EXT)
    if (! IS_PRESENT(cs_descriptor)) {
      BX_ERROR(("interrupt(): segment not present"));
      exception(BX_NP_EXCEPTION, cs_selector.value & 0xfffc);
    }

    // if code segment is non-conforming and DPL < CPL then
    // INTERRUPT TO INNER PRIVILEGE
    if(IS_CODE_SEGMENT_NON_CONFORMING(cs_descriptor.type) && cs_descriptor.dpl < CPL)
    {
      Bit16u old_SS, old_CS, SS_for_cpl_x;
      Bit32u ESP_for_cpl_x, old_EIP, old_ESP;
      bx_descriptor_t ss_descriptor;
      bx_selector_t   ss_selector;
      int is_v8086_mode = v8086_mode();

      BX_DEBUG(("interrupt(): INTERRUPT TO INNER PRIVILEGE"));

      // check selector and descriptor for new stack in current TSS
      get_SS_ESP_from_TSS(cs_descriptor.dpl,
                              &SS_for_cpl_x, &ESP_for_cpl_x);

      if (is_v8086_mode && cs_descriptor.dpl != 0) {
        // if code segment DPL != 0 then #GP(new code segment selector)
        BX_ERROR(("interrupt(): code segment DPL(%d) != 0 in v8086 mode", cs_descriptor.dpl));
        exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
      }

      // Selector must be non-null else #TS(EXT)
      if ((SS_for_cpl_x & 0xfffc) == 0) {
        BX_ERROR(("interrupt(): SS selector null"));
        exception(BX_TS_EXCEPTION, 0); /* TS(ext) */
      }

      // selector index must be within its descriptor table limits
      // else #TS(SS selector + EXT)
      parse_selector(SS_for_cpl_x, &ss_selector);
      // fetch 2 dwords of descriptor; call handles out of limits checks
      fetch_raw_descriptor(&ss_selector, &dword1, &dword2, BX_TS_EXCEPTION);
      parse_descriptor(dword1, dword2, &ss_descriptor);

      // selector rpl must = dpl of code segment,
      // else #TS(SS selector + ext)
      if (ss_selector.rpl != cs_descriptor.dpl) {
        BX_ERROR(("interrupt(): SS.rpl != CS.dpl"));
        exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // stack seg DPL must = DPL of code segment,
      // else #TS(SS selector + ext)
      if (ss_descriptor.dpl != cs_descriptor.dpl) {
        BX_ERROR(("interrupt(): SS.dpl != CS.dpl"));
        exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // descriptor must indicate writable data segment,
      // else #TS(SS selector + EXT)
      if (ss_descriptor.valid==0 || ss_descriptor.segment==0 ||
           IS_CODE_SEGMENT(ss_descriptor.type) ||
          !IS_DATA_SEGMENT_WRITEABLE(ss_descriptor.type))
      {
        BX_ERROR(("interrupt(): SS is not writable data segment"));
        exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // seg must be present, else #SS(SS selector + ext)
      if (! IS_PRESENT(ss_descriptor)) {
        BX_ERROR(("interrupt(): SS not present"));
        exception(BX_SS_EXCEPTION, SS_for_cpl_x & 0xfffc);
      }

      // IP must be within CS segment boundaries, else #GP(0)
      if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
        BX_ERROR(("interrupt(): gate EIP > CS.limit"));
        exception(BX_GP_EXCEPTION, 0);
      }

      old_ESP = ESP;
      old_SS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
      old_EIP = EIP;
      old_CS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;

      // Prepare new stack segment
      bx_segment_reg_t new_stack;
      new_stack.selector = ss_selector;
      new_stack.cache = ss_descriptor;
      new_stack.selector.rpl = cs_descriptor.dpl;
      // add cpl to the selector value
      new_stack.selector.value = (0xfffc & new_stack.selector.value) |
        new_stack.selector.rpl;

      if (ss_descriptor.u.segment.d_b) {
        Bit32u temp_ESP = ESP_for_cpl_x;

        if (is_v8086_mode)
        {
          if (gate_descriptor.type>=14) { // 386 int/trap gate
            write_new_stack_dword_32(&new_stack, temp_ESP-4,  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_dword_32(&new_stack, temp_ESP-8,  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_dword_32(&new_stack, temp_ESP-12, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_dword_32(&new_stack, temp_ESP-16, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_ESP -= 16;
          }
          else {
            write_new_stack_word_32(&new_stack, temp_ESP-2, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_word_32(&new_stack, temp_ESP-4, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_word_32(&new_stack, temp_ESP-6, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_word_32(&new_stack, temp_ESP-8, cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_ESP -= 8;
          }
        }

        if (gate_descriptor.type>=14) { // 386 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_dword_32(&new_stack, temp_ESP-4,  cs_descriptor.dpl, old_SS);
          write_new_stack_dword_32(&new_stack, temp_ESP-8,  cs_descriptor.dpl, old_ESP);
          write_new_stack_dword_32(&new_stack, temp_ESP-12, cs_descriptor.dpl, read_eflags());
          write_new_stack_dword_32(&new_stack, temp_ESP-16, cs_descriptor.dpl, old_CS);
          write_new_stack_dword_32(&new_stack, temp_ESP-20, cs_descriptor.dpl, old_EIP);
          temp_ESP -= 20;

          if (push_error) {
            temp_ESP -= 4;
            write_new_stack_dword_32(&new_stack, temp_ESP, cs_descriptor.dpl, error_code);
          }
        }
        else {                          // 286 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_word_32(&new_stack, temp_ESP-2,  cs_descriptor.dpl, old_SS);
          write_new_stack_word_32(&new_stack, temp_ESP-4,  cs_descriptor.dpl, (Bit16u) old_ESP);
          write_new_stack_word_32(&new_stack, temp_ESP-6,  cs_descriptor.dpl, (Bit16u) read_eflags());
          write_new_stack_word_32(&new_stack, temp_ESP-8,  cs_descriptor.dpl, old_CS);
          write_new_stack_word_32(&new_stack, temp_ESP-10, cs_descriptor.dpl, (Bit16u) old_EIP);
          temp_ESP -= 10;

          if (push_error) {
            temp_ESP -= 2;
            write_new_stack_word_32(&new_stack, temp_ESP, cs_descriptor.dpl, error_code);
          }
        }

        ESP = temp_ESP;
      }
      else {
        Bit16u temp_SP = (Bit16u) ESP_for_cpl_x;

        if (is_v8086_mode)
        {
          if (gate_descriptor.type>=14) { // 386 int/trap gate
            write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-4),  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-8),  cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-12), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-16), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_SP -= 16;
          }
          else {
            write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-2), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
            write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-4), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
            write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-6), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
            write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-8), cs_descriptor.dpl,
                BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
            temp_SP -= 8;
          }
        }

        if (gate_descriptor.type>=14) { // 386 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-4),  cs_descriptor.dpl, old_SS);
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-8),  cs_descriptor.dpl, old_ESP);
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-12), cs_descriptor.dpl, read_eflags());
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-16), cs_descriptor.dpl, old_CS);
          write_new_stack_dword_32(&new_stack, (Bit16u)(temp_SP-20), cs_descriptor.dpl, old_EIP);
          temp_SP -= 20;

          if (push_error) {
            temp_SP -= 4;
            write_new_stack_dword_32(&new_stack, temp_SP, cs_descriptor.dpl, error_code);
          }
        }
        else {                          // 286 int/trap gate
          // push long pointer to old stack onto new stack
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-2),  cs_descriptor.dpl, old_SS);
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-4),  cs_descriptor.dpl, (Bit16u) old_ESP);
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-6),  cs_descriptor.dpl, (Bit16u) read_eflags());
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-8),  cs_descriptor.dpl, old_CS);
          write_new_stack_word_32(&new_stack, (Bit16u)(temp_SP-10), cs_descriptor.dpl, (Bit16u) old_EIP);
          temp_SP -= 10;

          if (push_error) {
            temp_SP -= 2;
            write_new_stack_word_32(&new_stack, temp_SP, cs_descriptor.dpl, error_code);
          }
        }

        SP = temp_SP;
      }

      // load new CS:eIP values from gate
      // set CPL to new code segment DPL
      // set RPL of CS to CPL
      load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);

      // load new SS:eSP values from TSS
      load_ss(&ss_selector, &ss_descriptor, cs_descriptor.dpl);

      if (is_v8086_mode)
      {
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].cache.valid = 0;
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value = 0;
      }
    }
    else
    {
      BX_DEBUG(("interrupt(): INTERRUPT TO SAME PRIVILEGE"));

      if (v8086_mode() && (IS_CODE_SEGMENT_CONFORMING(cs_descriptor.type) || cs_descriptor.dpl != 0)) {
        // if code segment DPL != 0 then #GP(new code segment selector)
        BX_ERROR(("interrupt(): code segment conforming or DPL(%d) != 0 in v8086 mode", cs_descriptor.dpl));
        exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc);
      }

      // EIP must be in CS limit else #GP(0)
      if (gate_dest_offset > cs_descriptor.u.segment.limit_scaled) {
        BX_ERROR(("interrupt(): IP > CS descriptor limit"));
        exception(BX_GP_EXCEPTION, 0);
      }

      // push flags onto stack
      // push current CS selector onto stack
      // push return offset onto stack
      if (gate_descriptor.type >= 14) { // 386 gate
        push_32(read_eflags());
        push_32(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
        push_32(EIP);
        if (push_error)
          push_32(error_code);
      }
      else { // 286 gate
        push_16((Bit16u) read_eflags());
        push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
        push_16(IP);
        if (push_error)
          push_16(error_code);
      }

      // load CS:IP from gate
      // load CS descriptor
      // set the RPL field of CS to CPL
      load_cs(&cs_selector, &cs_descriptor, CPL);
    }

    EIP = gate_dest_offset;

    // if interrupt gate then set IF to 0
    if (!(gate_descriptor.type & 1)) // even is int-gate
      BX_CPU_THIS_PTR clear_IF();
    BX_CPU_THIS_PTR clear_TF();
    BX_CPU_THIS_PTR clear_NT();
    BX_CPU_THIS_PTR clear_VM();
    BX_CPU_THIS_PTR clear_RF();
    return;

  default:
    BX_PANIC(("bad descriptor type in interrupt()!"));
    break;
  }
}

void BX_CPU_C::real_mode_int(Bit8u vector, bx_bool push_error, Bit16u error_code)
{
#if BX_NTVDM_ENABLE_REAL_MODE_VECTOR_DIAGNOSTIC
  Bit16u ntdos64_old_cs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  Bit16u ntdos64_old_ip = IP;
  Bit16u ntdos64_old_ss = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
  Bit16u ntdos64_old_sp = SP;
#endif
  if ((vector*4+3) > BX_CPU_THIS_PTR idtr.limit) {
    BX_ERROR(("interrupt(real mode) vector > idtr.limit"));
    exception(BX_GP_EXCEPTION, 0);
  }

  push_16((Bit16u) read_eflags());
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
  push_16(IP);

  Bit16u new_ip = system_read_word(BX_CPU_THIS_PTR idtr.base + 4 * vector);
  // CS.LIMIT can't change when in real/v8086 mode
  if (new_ip > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
    BX_ERROR(("interrupt(real mode): instruction pointer not within code segment limits"));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit16u cs_selector = system_read_word(BX_CPU_THIS_PTR idtr.base + 4 * vector + 2);
#if BX_NTVDM_ENABLE_REAL_MODE_VECTOR_DIAGNOSTIC
  BX_INFO(("ntdos64 real-mode vector vector=%02x old-cs=%04x old-ip=%04x old-ss=%04x old-sp=%04x target-cs=%04x target-ip=%04x",
      vector, ntdos64_old_cs, ntdos64_old_ip, ntdos64_old_ss, ntdos64_old_sp,
      cs_selector, new_ip));
#endif
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS], cs_selector);
  EIP = new_ip;

  /* INT affects the following flags: I,T */
  BX_CPU_THIS_PTR clear_IF();
  BX_CPU_THIS_PTR clear_TF();
#if BX_CPU_LEVEL >= 4
  BX_CPU_THIS_PTR clear_AC();
#endif
  BX_CPU_THIS_PTR clear_RF();
}

void BX_CPU_C::interrupt(Bit8u vector, unsigned type, bx_bool push_error, Bit16u error_code)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_intsig;
#if BX_DEBUG_LINUX
  if (bx_dbg.linux_syscall) {
    if (vector == 0x80) bx_dbg_linux_syscall(BX_CPU_ID);
  }
#endif
  bx_dbg_interrupt(BX_CPU_ID, vector, error_code);
#endif

  BX_INSTR_INTERRUPT(BX_CPU_ID, vector);

  invalidate_prefetch_q();

  bx_bool soft_int = 0;
  switch(type) {
    case BX_SOFTWARE_INTERRUPT:
    case BX_SOFTWARE_EXCEPTION:
      soft_int = 1;
      break;
    case BX_PRIVILEGED_SOFTWARE_INTERRUPT:
    case BX_EXTERNAL_INTERRUPT:
    case BX_NMI:
    case BX_HARDWARE_EXCEPTION:
      break;

    default:
      BX_PANIC(("interrupt(): unknown exception type %d", type));
  }

  BX_DEBUG(("interrupt(): vector = %02x, TYPE = %u, EXT = %u",
      vector, type, (unsigned) BX_CPU_THIS_PTR EXT));

  // Discard any traps and inhibits for new context; traps will
  // resume upon return.
  BX_CPU_THIS_PTR debug_trap = 0;
  BX_CPU_THIS_PTR inhibit_mask = 0;

#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  BX_CPU_THIS_PTR in_event = 1;
#endif

#if BX_SUPPORT_X86_64
  if (long_mode()) {
    long_mode_int(vector, soft_int, push_error, error_code);
  }
  else
#endif
  {
    RSP_SPECULATIVE;

    // software interrupt can be redirefcted in v8086 mode
    if (type != BX_SOFTWARE_INTERRUPT || !v8086_mode() || !v86_redirect_interrupt(vector))
    {
      if(real_mode()) {
        real_mode_int(vector, push_error, error_code);
      }
      else {
        protected_mode_int(vector, soft_int, push_error, error_code);
      }
    }

    RSP_COMMIT;
  }

#if BX_X86_DEBUGGER
  BX_CPU_THIS_PTR in_repeat = 0;
#endif

#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  BX_CPU_THIS_PTR in_event = 0;
#endif
}

/* Exception classes.  These are used as indexes into the 'is_exception_OK'
 * array below, and are stored in the 'exception' array also
 */
#define BX_ET_BENIGN       0
#define BX_ET_CONTRIBUTORY 1
#define BX_ET_PAGE_FAULT   2

#define BX_ET_DOUBLE_FAULT 10

static const bx_bool is_exception_OK[3][3] = {
    { 1, 1, 1 }, /* 1st exception is BENIGN */
    { 1, 0, 1 }, /* 1st exception is CONTRIBUTORY */
    { 1, 0, 0 }  /* 1st exception is PAGE_FAULT */
};

#define BX_EXCEPTION_CLASS_TRAP  0
#define BX_EXCEPTION_CLASS_FAULT 1
#define BX_EXCEPTION_CLASS_ABORT 2

struct BxExceptionInfo exceptions_info[BX_CPU_HANDLED_EXCEPTIONS] = {
  /* DE */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 0 },
  /* DB */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 02 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }, // NMI
  /* BP */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_TRAP,  0 },
  /* OF */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_TRAP,  0 },
  /* BR */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* UD */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* NM */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* DF */ { BX_ET_DOUBLE_FAULT, BX_EXCEPTION_CLASS_FAULT, 1 },
             // coprocessor segment overrun (286,386 only)
  /* 09 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* TS */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* NP */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* SS */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* GP */ { BX_ET_CONTRIBUTORY, BX_EXCEPTION_CLASS_FAULT, 1 },
  /* PF */ { BX_ET_PAGE_FAULT,   BX_EXCEPTION_CLASS_FAULT, 1 },
  /* 15 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }, // reserved
  /* MF */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* AC */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 1 },
  /* MC */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_ABORT, 0 },
  /* XM */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 20 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 21 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 22 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 23 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 24 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 25 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 26 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 27 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 28 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 29 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 },
  /* 30 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }, // FIXME: SVM #SF
  /* 31 */ { BX_ET_BENIGN,       BX_EXCEPTION_CLASS_FAULT, 0 }
};

// vector:     0..255: vector in IDT
// error_code: if exception generates and error, push this error code
// trap:       override exception class to TRAP
void BX_CPU_C::exception(unsigned vector, Bit16u error_code)
{
#if BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT || BX_NTVDM_ENABLE_STARTUP_TRANSACTION || BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER || BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE || BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN || BX_NTVDM_ENABLE_MACHINE_COMPOSITION
  bx_ntvdm_exception_response response;
  bx_ntvdm_cpu_state_v1 cpu_state;
  bx_ntvdm_instruction_window_v1 instruction_window;
#endif

  BX_INSTR_EXCEPTION(BX_CPU_ID, vector, error_code);

#if BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT || BX_NTVDM_ENABLE_STARTUP_TRANSACTION || BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER || BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE || BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN
  if (vector == BX_UD_EXCEPTION) {
    bx_ntvdm_cpu_state_v1_initialize(&cpu_state,
      real_mode() ? BX_NTVDM_CPU_EXECUTION_REAL :
      (v8086_mode() ? BX_NTVDM_CPU_EXECUTION_V8086 :
       BX_NTVDM_CPU_EXECUTION_PROTECTED));
    cpu_state.eax = EAX;
    cpu_state.ebx = EBX;
    cpu_state.ecx = ECX;
    cpu_state.edx = EDX;
    cpu_state.esi = ESI;
    cpu_state.edi = EDI;
    cpu_state.ebp = EBP;
    cpu_state.esp = ESP;
    cpu_state.eip = (Bit32u) BX_CPU_THIS_PTR prev_rip;
    cpu_state.eflags = read_eflags();
    cpu_state.cs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
    cpu_state.ds = BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value;
    cpu_state.es = BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value;
    cpu_state.ss = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
    cpu_state.fs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value;
    cpu_state.gs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value;
    bx_ntvdm_instruction_window_v1_capture(&instruction_window, 0, 0);
    bx_address window_offset = BX_CPU_THIS_PTR prev_rip +
      BX_CPU_THIS_PTR eipPageBias;
    if (BX_CPU_THIS_PTR eipFetchPtr != 0 &&
        window_offset < BX_CPU_THIS_PTR eipPageWindowSize) {
      unsigned window_bytes = BX_CPU_THIS_PTR eipPageWindowSize - window_offset;
      if (window_bytes > BX_NTVDM_INSTRUCTION_WINDOW_V1_MAX_BYTES)
        window_bytes = BX_NTVDM_INSTRUCTION_WINDOW_V1_MAX_BYTES;
      bx_ntvdm_instruction_window_v1_capture(&instruction_window,
        BX_CPU_THIS_PTR eipFetchPtr + window_offset, window_bytes);
    }
#if BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER
    bx_ntvdm_bop_catalog_listener(BX_CPU_ID, vector, error_code,
      BX_CPU_THIS_PTR prev_rip, &cpu_state, &instruction_window);
#endif
  }

#if BX_NTVDM_ENABLE_STARTUP_TRANSACTION
  if (vector == BX_UD_EXCEPTION) {
    bx_ntvdm_exception_request request;
    request.abi_version = BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION;
    request.cpu_id = BX_CPU_ID;
    request.vector = vector;
    request.error_code = error_code;
    request.fault_rip = BX_CPU_THIS_PTR prev_rip;
    request.cpu_state = &cpu_state;
    request.instruction_window = &instruction_window;
    if (bx_ntvdm_startup_transaction_interceptor(&request, &response)) {
      RIP = response.resume_rip;
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
    }
#if BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE
    if (bx_ntvdm_cpu_result_bridge_interceptor(&request, &response)) {
      for (unsigned reg = 0; reg < BX_NTVDM_CPU_DELTA_V1_GPR16_COUNT; ++reg) {
        if ((response.cpu_result.cpu_delta.gpr16_write_mask & (1u << reg)) != 0u)
          BX_CPU_THIS_PTR set_reg16(reg, response.cpu_result.cpu_delta.gpr16_values[reg]);
      }
      if ((response.cpu_result.eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u)
        BX_CPU_THIS_PTR set_CF((response.cpu_result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u);
      RIP = response.resume_rip;
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
    }
    if (bx_ntvdm_cpu_result_bridge_guest_read_interceptor(&request, &response)) {
      for (unsigned reg = 0; reg < BX_NTVDM_CPU_DELTA_V1_GPR16_COUNT; ++reg) {
        if ((response.cpu_result.cpu_delta.gpr16_write_mask & (1u << reg)) != 0u)
          BX_CPU_THIS_PTR set_reg16(reg, response.cpu_result.cpu_delta.gpr16_values[reg]);
      }
      if ((response.cpu_result.eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u)
        BX_CPU_THIS_PTR set_CF((response.cpu_result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u);
      RIP = response.resume_rip;
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
    }
    if (bx_ntvdm_cpu_result_bridge_multi_write_interceptor(&request, &response)) {
      for (unsigned reg = 0; reg < BX_NTVDM_CPU_DELTA_V1_GPR16_COUNT; ++reg) {
        if ((response.cpu_result.cpu_delta.gpr16_write_mask & (1u << reg)) != 0u)
          BX_CPU_THIS_PTR set_reg16(reg, response.cpu_result.cpu_delta.gpr16_values[reg]);
      }
      if ((response.cpu_result.eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u)
        BX_CPU_THIS_PTR set_CF((response.cpu_result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u);
      RIP = response.resume_rip;
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
    }
#endif

#if BX_NTVDM_ENABLE_UNMATCHED_UD_DIAGNOSTIC
    BX_INFO(("ntdos64 unmatched-ud cpu=%u cs=%04x rip=%08x",
      BX_CPU_ID, BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
      (Bit32u) BX_CPU_THIS_PTR prev_rip));
#endif
  }
#endif

#if BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN
  if (vector == BX_UD_EXCEPTION &&
      bx_ntvdm_deferred_startup_plan_consumer(BX_CPU(0), BX_CPU_ID, vector, error_code,
        BX_CPU_THIS_PTR prev_rip, &cpu_state, &instruction_window,
        &response)) {
    RIP = response.resume_rip;
    longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
  }
#endif

#if BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT
  if (bx_ntvdm_try_intercept_ud(BX_CPU_ID, vector, error_code,
      BX_CPU_THIS_PTR prev_rip,
      vector == BX_UD_EXCEPTION ? &cpu_state : 0,
      vector == BX_UD_EXCEPTION ? &instruction_window : 0, &response)) {
  if (response.disposition == BX_NTVDM_EXCEPTION_STOP) {
      SIM->quit_sim(0);
  }
    for (unsigned reg = 0; reg < BX_NTVDM_CPU_DELTA_V1_GPR16_COUNT; ++reg) {
      if ((response.cpu_result.cpu_delta.gpr16_write_mask & (1u << reg)) != 0u)
        BX_CPU_THIS_PTR set_reg16(reg, response.cpu_result.cpu_delta.gpr16_values[reg]);
    }
    if ((response.cpu_result.eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u)
      BX_CPU_THIS_PTR set_CF((response.cpu_result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u);
    RIP = response.resume_rip;
    longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
  }
#endif

#if BX_NTVDM_ENABLE_MACHINE_COMPOSITION
  if (vector == BX_UD_EXCEPTION) {
    bx_ntvdm_machine_bop_v1_result outcome = bx_ntvdm_machine_composition_probe(
      BX_CPU_ID, vector, error_code, BX_CPU_THIS_PTR prev_rip, &cpu_state,
      &instruction_window);
    if (outcome != BX_NTVDM_MACHINE_BOP_V1_DECLINED)
      BX_INFO(("ntdos64 machine composition outcome=%u", (unsigned) outcome));
    if (outcome == BX_NTVDM_MACHINE_BOP_V1_HANDLED_RESUME) {
      RIP = BX_CPU_THIS_PTR prev_rip + 3;
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
    }
  }
#endif
#endif

#if BX_DEBUGGER
  bx_dbg_exception(BX_CPU_ID, vector, error_code);
#endif

  BX_DEBUG(("exception(0x%02x): error_code=%04x", vector, error_code));

  unsigned exception_type = 0;
  unsigned exception_class = BX_EXCEPTION_CLASS_FAULT;
  bx_bool push_error = 0;

  if (vector < BX_CPU_HANDLED_EXCEPTIONS) {
     push_error = exceptions_info[vector].push_error;
     exception_class = exceptions_info[vector].exception_class;
     exception_type = exceptions_info[vector].exception_type;
  }
  else {
     BX_PANIC(("exception(%u): bad vector", vector));
  }

  if (vector != BX_PF_EXCEPTION && vector != BX_DF_EXCEPTION) {
    // Page faults have different format
    error_code = (error_code & 0xfffe) | BX_CPU_THIS_PTR EXT;
  }

#if BX_SUPPORT_VMX
  VMexit_Event(BX_HARDWARE_EXCEPTION, vector, error_code, push_error);
#endif

#if BX_SUPPORT_SVM
  SvmInterceptException(BX_HARDWARE_EXCEPTION, vector, error_code, push_error);
#endif

  if (BX_CPU_THIS_PTR errorno > 0) {
    if (BX_CPU_THIS_PTR errorno > 2 || BX_CPU_THIS_PTR curr_exception == BX_ET_DOUBLE_FAULT) {
      // restore RIP/RSP to value before error occurred
      RIP = BX_CPU_THIS_PTR prev_rip;
      if (BX_CPU_THIS_PTR speculative_rsp)
        RSP = BX_CPU_THIS_PTR prev_rsp;

      debug(BX_CPU_THIS_PTR prev_rip); // print debug information to the log
#if BX_SUPPORT_VMX
      VMexit_TripleFault();
#endif
#if BX_DEBUGGER
      // trap into debugger (similar as done when PANIC occured)
      bx_debug_break();
#endif
      if (SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT)->get()) {
        BX_ERROR(("exception(): 3rd (%d) exception with no resolution, shutdown status is %02xh, resetting", vector, DEV_cmos_get_reg(0x0f)));
        bx_pc_system.Reset(BX_RESET_HARDWARE);
      }
      else {
        BX_PANIC(("exception(): 3rd (%d) exception with no resolution", vector));
        BX_ERROR(("WARNING: Any simulation after this point is completely bogus !"));
        shutdown();
      }
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
    }
  }

  // note: fault-class exceptions _except_ #DB set RF in
  //       eflags image.
  if (exception_class == BX_EXCEPTION_CLASS_FAULT)
  {
    // restore RIP/RSP to value before error occurred
    RIP = BX_CPU_THIS_PTR prev_rip;
    if (BX_CPU_THIS_PTR speculative_rsp)
      RSP = BX_CPU_THIS_PTR prev_rsp;

    if (vector != BX_DB_EXCEPTION) BX_CPU_THIS_PTR assert_RF();
  }

  if (vector == BX_DB_EXCEPTION) {
    // Commit debug events to DR6: preserve DR5.BS and DR6.BD values,
    // only software can clear them
    BX_CPU_THIS_PTR dr6.val32 = (BX_CPU_THIS_PTR dr6.val32 & 0xffff6ff0) |
                          (BX_CPU_THIS_PTR debug_trap & 0x0000e00f);

    // clear GD flag in the DR7 prior entering debug exception handler
    BX_CPU_THIS_PTR dr7.set_GD(0);
  }

  BX_CPU_THIS_PTR EXT = 1;

  /* if we've already had 1st exception, see if 2nd causes a
   * Double Fault instead.  Otherwise, just record 1st exception
   */
  if (BX_CPU_THIS_PTR errorno > 0 && exception_type != BX_ET_DOUBLE_FAULT) {
    if (! is_exception_OK[BX_CPU_THIS_PTR curr_exception][exception_type]) {
      exception(BX_DF_EXCEPTION, 0);
    }
  }

  BX_CPU_THIS_PTR curr_exception = exception_type;
  BX_CPU_THIS_PTR errorno++;

  if (real_mode()) {
    push_error = 0; // not INT, no error code pushed
    error_code = 0;
  }

  interrupt(vector, BX_HARDWARE_EXCEPTION, push_error, error_code);
  BX_CPU_THIS_PTR errorno = 0; // error resolved
  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}
