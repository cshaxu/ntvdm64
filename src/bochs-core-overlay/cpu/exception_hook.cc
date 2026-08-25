/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-UD-001,BX-UD-002): private copied-record exception hook.
// The adopted exception.cc retains only one selector-blind member call.
// This file has no public consumer; its records are byte contracts only.
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "iodev/iodev.h"
#include "bochs-core-overlay/cpu/opaque_callback_private.h"

#ifndef RUNTIME_ENABLE_MANTLE_UD_BRIDGE
#if defined(BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE)
#define RUNTIME_ENABLE_MANTLE_UD_BRIDGE BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE
#else
#define RUNTIME_ENABLE_MANTLE_UD_BRIDGE 0
#endif
#endif

#ifndef RUNTIME_ENABLE_MANTLE_FIRST_FAULT_OBSERVER
#if defined(BX_NTVDM_ENABLE_MANTLE_FIRST_FAULT_OBSERVER)
#define RUNTIME_ENABLE_MANTLE_FIRST_FAULT_OBSERVER BX_NTVDM_ENABLE_MANTLE_FIRST_FAULT_OBSERVER
#else
#define RUNTIME_ENABLE_MANTLE_FIRST_FAULT_OBSERVER 0
#endif
#endif

enum {
  BOCHS_CORE_OVERLAY_GENERIC_UD_EVENT_MAGIC = 0x42585544u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_EVENT_VERSION = 1u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_WINDOW_BYTES = 15u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_GPR16_COUNT = 8u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_GPR32_COUNT = 8u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_OUTCOME_V2_VERSION = 2u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_EFLAGS_WRITE_MASK = 0x003f7fd5u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_PASS_THROUGH = 0u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_RESUME = 1u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_STOP = 2u,
  BOCHS_CORE_OVERLAY_GENERIC_UD_PENDING = 3u,
  BOCHS_CORE_OVERLAY_CONTEXT_UNCHANGED = 0u,
  BOCHS_CORE_OVERLAY_CONTEXT_REAL = 1u,
  BOCHS_CORE_OVERLAY_CONTEXT_PROTECTED = 2u,
  BOCHS_CORE_OVERLAY_FIRST_FAULT_MAGIC = 0x42584631u,
  BOCHS_CORE_OVERLAY_FIRST_FAULT_VERSION = 1u
};

/* These private records intentionally retain the fixed byte layout used by
 * the composed callback.  They neither name nor include an adapter. */
struct bochs_core_overlay_generic_ud_event_v1 {
  Bit32u magic, abi_version, struct_bytes, cpu_id;
  Bit32u vector, error_code, execution_mode, reserved0;
  Bit64u fault_rip;
  Bit32u eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, eflags;
  Bit16u cs, ds, es, ss, fs, gs;
  Bit32u window_bytes;
  Bit8u window[BOCHS_CORE_OVERLAY_GENERIC_UD_WINDOW_BYTES];
  Bit8u reserved1;
};

struct bochs_core_overlay_generic_ud_outcome_v1 {
  Bit32u abi_version, disposition;
  Bit64u resume_rip;
  Bit32u gpr16_write_mask;
  Bit16u gpr16_values[BOCHS_CORE_OVERLAY_GENERIC_UD_GPR16_COUNT];
  Bit32u segment_write_mask;
  Bit16u segment_values[6];
  Bit32u eflags_write_mask, eflags_values;
};

struct bochs_core_overlay_generic_ud_outcome_v2 {
  Bit32u abi_version, disposition;
  Bit64u resume_rip;
  Bit32u gpr32_write_mask;
  Bit32u gpr32_values[BOCHS_CORE_OVERLAY_GENERIC_UD_GPR32_COUNT];
  Bit32u segment_write_mask;
  Bit16u segment_values[6];
  Bit32u eflags_write_mask, eflags_values;
  Bit32u context_mode, reserved0;
};

struct bochs_core_overlay_first_fault_v1 {
  Bit32u magic, abi_version, struct_bytes, cpu_id;
  Bit32u vector, error_code, execution_mode, reserved0;
  Bit64u fault_rip;
  Bit32u eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, eflags;
  Bit16u cs, ds, es, ss, fs, gs;
};

static void bochs_core_overlay_copy_exception_state(BX_CPU_C *cpu,
  bochs_core_overlay_generic_ud_event_v1 *event, unsigned vector,
  Bit16u error_code)
{
  memset(event, 0, sizeof(*event));
  event->magic = BOCHS_CORE_OVERLAY_GENERIC_UD_EVENT_MAGIC;
  event->abi_version = BOCHS_CORE_OVERLAY_GENERIC_UD_EVENT_VERSION;
  event->struct_bytes = sizeof(*event);
  event->cpu_id = cpu->bx_cpuid;
  event->vector = vector;
  event->error_code = error_code;
  event->execution_mode = cpu->real_mode() ? 1u : (cpu->v8086_mode() ? 3u : 2u);
  event->fault_rip = cpu->prev_rip;
  event->eax = cpu->gen_reg[BX_32BIT_REG_EAX].dword.erx;
  event->ebx = cpu->gen_reg[BX_32BIT_REG_EBX].dword.erx;
  event->ecx = cpu->gen_reg[BX_32BIT_REG_ECX].dword.erx;
  event->edx = cpu->gen_reg[BX_32BIT_REG_EDX].dword.erx;
  event->esi = cpu->gen_reg[BX_32BIT_REG_ESI].dword.erx;
  event->edi = cpu->gen_reg[BX_32BIT_REG_EDI].dword.erx;
  event->ebp = cpu->gen_reg[BX_32BIT_REG_EBP].dword.erx;
  event->esp = cpu->gen_reg[BX_32BIT_REG_ESP].dword.erx;
  event->eip = (Bit32u)cpu->prev_rip;
  event->eflags = cpu->read_eflags();
  event->cs = cpu->sregs[BX_SEG_REG_CS].selector.value;
  event->ds = cpu->sregs[BX_SEG_REG_DS].selector.value;
  event->es = cpu->sregs[BX_SEG_REG_ES].selector.value;
  event->ss = cpu->sregs[BX_SEG_REG_SS].selector.value;
  event->fs = cpu->sregs[BX_SEG_REG_FS].selector.value;
  event->gs = cpu->sregs[BX_SEG_REG_GS].selector.value;
}

int BX_CPU_C::overlay_handle_exception(unsigned vector, Bit16u error_code)
{
#if RUNTIME_ENABLE_MANTLE_FIRST_FAULT_OBSERVER
  if (vector != BX_UD_EXCEPTION) {
    bochs_core_overlay_first_fault_v1 event;
    bochs_core_overlay_generic_ud_event_v1 common;
    bochs_core_overlay_copy_exception_state(this, &common, vector, error_code);
    memcpy(&event, &common, sizeof(event));
    event.magic = BOCHS_CORE_OVERLAY_FIRST_FAULT_MAGIC;
    event.abi_version = BOCHS_CORE_OVERLAY_FIRST_FAULT_VERSION;
    event.struct_bytes = sizeof(event);
    if (bochs_core_overlay_opaque_callback_v1_invoke(&event, sizeof(event), 0, 0)) {
      bx_pc_system.kill_bochs_request = 1;
      BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
    }
  }
#endif

#if RUNTIME_ENABLE_MANTLE_UD_BRIDGE
  if (vector == BX_UD_EXCEPTION) {
    bochs_core_overlay_generic_ud_event_v1 event;
    bochs_core_overlay_generic_ud_outcome_v2 context_outcome;
    bochs_core_overlay_generic_ud_outcome_v1 outcome;
    bochs_core_overlay_copy_exception_state(this, &event, vector, error_code);
    bx_address offset = BX_CPU_THIS_PTR prev_rip + BX_CPU_THIS_PTR eipPageBias;
    if (BX_CPU_THIS_PTR eipFetchPtr != 0 && offset < BX_CPU_THIS_PTR eipPageWindowSize) {
      unsigned available = BX_CPU_THIS_PTR eipPageWindowSize - offset;
      event.window_bytes = available > BOCHS_CORE_OVERLAY_GENERIC_UD_WINDOW_BYTES ?
        BOCHS_CORE_OVERLAY_GENERIC_UD_WINDOW_BYTES : available;
      memcpy(event.window, BX_CPU_THIS_PTR eipFetchPtr + offset, event.window_bytes);
    }

    memset(&context_outcome, 0, sizeof(context_outcome));
    if (bochs_core_overlay_opaque_callback_v1_invoke(&event, sizeof(event),
          &context_outcome, sizeof(context_outcome)) &&
        context_outcome.abi_version == BOCHS_CORE_OVERLAY_GENERIC_UD_OUTCOME_V2_VERSION &&
        context_outcome.disposition == BOCHS_CORE_OVERLAY_GENERIC_UD_RESUME &&
        context_outcome.gpr32_write_mask <= 0xffu &&
        context_outcome.segment_write_mask <= 0x3fu &&
        (context_outcome.eflags_write_mask &
          ~BOCHS_CORE_OVERLAY_GENERIC_UD_EFLAGS_WRITE_MASK) == 0u &&
        (context_outcome.context_mode == BOCHS_CORE_OVERLAY_CONTEXT_UNCHANGED ||
         context_outcome.context_mode == BOCHS_CORE_OVERLAY_CONTEXT_REAL ||
         context_outcome.context_mode == BOCHS_CORE_OVERLAY_CONTEXT_PROTECTED)) {
      bx_address context_cr0 = BX_CPU_THIS_PTR read_CR0();
      if (context_outcome.context_mode == BOCHS_CORE_OVERLAY_CONTEXT_REAL)
        context_cr0 &= ~1u;
      else if (context_outcome.context_mode == BOCHS_CORE_OVERLAY_CONTEXT_PROTECTED)
        context_cr0 |= 1u;
      if (context_outcome.context_mode == BOCHS_CORE_OVERLAY_CONTEXT_UNCHANGED ||
          BX_CPU_THIS_PTR SetCR0(context_cr0)) {
        for (unsigned reg = 0; reg < BOCHS_CORE_OVERLAY_GENERIC_UD_GPR32_COUNT; ++reg)
          if ((context_outcome.gpr32_write_mask & (1u << reg)) != 0u)
            BX_CPU_THIS_PTR set_reg32(reg, context_outcome.gpr32_values[reg]);
        for (unsigned seg = 0; seg < 6u; ++seg)
          if (seg != BX_SEG_REG_CS &&
              (context_outcome.segment_write_mask & (1u << seg)) != 0u)
            BX_CPU_THIS_PTR load_seg_reg(&BX_CPU_THIS_PTR sregs[seg],
              context_outcome.segment_values[seg]);
        if (context_outcome.eflags_write_mask != 0u)
          BX_CPU_THIS_PTR writeEFlags(context_outcome.eflags_values,
            context_outcome.eflags_write_mask);
        if ((context_outcome.segment_write_mask & (1u << BX_SEG_REG_CS)) != 0u) {
          if (BX_CPU_THIS_PTR protected_mode())
            BX_CPU_THIS_PTR jump_protected(0,
              context_outcome.segment_values[BX_SEG_REG_CS], context_outcome.resume_rip);
          else
            BX_CPU_THIS_PTR load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS],
              context_outcome.segment_values[BX_SEG_REG_CS]);
        }
        RIP = context_outcome.resume_rip;
        longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
      }
    }

    memset(&outcome, 0, sizeof(outcome));
    if (bochs_core_overlay_opaque_callback_v1_invoke(&event, sizeof(event),
          &outcome, sizeof(outcome)) &&
        outcome.abi_version == BOCHS_CORE_OVERLAY_GENERIC_UD_EVENT_VERSION &&
        (outcome.disposition == BOCHS_CORE_OVERLAY_GENERIC_UD_RESUME ||
         outcome.disposition == BOCHS_CORE_OVERLAY_GENERIC_UD_STOP ||
         outcome.disposition == BOCHS_CORE_OVERLAY_GENERIC_UD_PENDING)) {
      if (outcome.disposition == BOCHS_CORE_OVERLAY_GENERIC_UD_STOP ||
          outcome.disposition == BOCHS_CORE_OVERLAY_GENERIC_UD_PENDING) {
        bx_pc_system.kill_bochs_request = 1;
        BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
        longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
      }
      for (unsigned reg = 0; reg < BOCHS_CORE_OVERLAY_GENERIC_UD_GPR16_COUNT; ++reg)
        if ((outcome.gpr16_write_mask & (1u << reg)) != 0u)
          BX_CPU_THIS_PTR set_reg16(reg, outcome.gpr16_values[reg]);
      for (unsigned seg = 0; seg < 6u; ++seg)
        if ((outcome.segment_write_mask & (1u << seg)) != 0u)
          BX_CPU_THIS_PTR load_seg_reg(&BX_CPU_THIS_PTR sregs[seg],
            outcome.segment_values[seg]);
      if ((outcome.eflags_write_mask & 1u) != 0u)
        BX_CPU_THIS_PTR set_CF((outcome.eflags_values & 1u) != 0u);
      if ((outcome.eflags_write_mask & 0x40u) != 0u)
        BX_CPU_THIS_PTR set_ZF((outcome.eflags_values & 0x40u) != 0u);
      RIP = outcome.resume_rip;
      longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1);
    }
  }
#endif
  return 0;
}
