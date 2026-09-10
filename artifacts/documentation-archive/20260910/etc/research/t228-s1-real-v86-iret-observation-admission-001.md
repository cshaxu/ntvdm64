# T228 S1 P23 — Real/V86 IRET Completion Observation Admission

## Purpose

P22 rules out a post-`EndInit` DEM file-family reload as the next repair
rung.  The remaining owner question is the original guest COMMAND/NTDOS
interrupt-return and control-state lifecycle.  P20 records software-interrupt
state only before delivery.  It cannot prove the completed IRET destination or
the post-return register/flags state that the original guest continuation
receives.

This admission permits a *mechanical diagnostic profile*, not an interrupt or
BOP repair.  It is intentionally broad enough to remain selector-blind: every
successfully completed real/V86 `IRET16` or `IRET32` may emit one fixed-width
post-return record.  A later bounded integration observation may correlate the
chronological records offline; it must not claim a one-to-one vector pairing in
the presence of nested or hardware interrupts.

## Current source and boundary audit

| Rung | Disposition | Evidence and consequence |
| --- | --- | --- |
| Original OpenNT source reuse | Not composable | `src/opennt/base/mvdm/.../command` and `.../ntdos/doskrnl/dos/alloc.asm` own the guest continuation, MCB and transient semantics, but execute as the immutable 16-bit guest rather than exposing a host-side IRET completion callback.  They remain the semantic authority. |
| Smallest adapter/shim | Not possible | `bx-vdm` obtains only the #UD/BOP bridge after a copied opcode window.  It cannot observe ordinary IRET without decoding or intercepting DOS interrupt semantics; doing so would cross the Bochs CPU boundary and violate the adapter rules. |
| Existing adopted-Bochs facility | Insufficient | `src/bx-core/cpu/ctrl_xfer16.cc:587` and `ctrl_xfer32.cc:630` already invoke `BX_INSTR_FAR_BRANCH(...BX_INSTR_IS_IRET...)`, but the minimal build retains the original `instrument/stubs/instrument.h` with `BX_INSTRUMENTATION=0`; its macros are no-ops and provide neither a mantle-owned copied record nor CPU register/flags state.  Enabling/customizing instrumentation would activate a broad product subsystem, not a bounded fixed ABI. |
| Registered external mechanical exception | Proposed first usable rung | `BX-CORE-089` may add a default-off call after the existing successful real/V86 IRET state commit.  Mantle retains the copied bounded ring and query. |
| Newly authored guest behavior | Rejected | No guest, interrupt, provider, BOP, DOS or OpenNT behavior may be authored by this profile. |

The historical `BX-TRACE-050` IRET16 TF-only probe is not reusable: it was
removed by T196 and was filtered on an already-popped FLAGS value.  Its
selector-blind successor must not revive that semantic condition.

## Proposed fixed ABI and profile boundary

`src/bx-mantle/bx_ntvdm_interrupt_return_observation_v1.*` may own a bounded,
opt-in copied ring.  Each accepted record contains only version, CPU id,
native instruction count, post-return RIP/EFLAGS, CS/SS/SP, AX/BX/CX/DX,
DS/ES, execution mode and operand width (16 or 32).  It contains no vector,
BOP selector, service, guest address predicate, memory byte, pointer or host
handle.

The only proposed adopted-code edits are guarded calls in
`src/bx-core/cpu/ctrl_xfer16.cc` and `ctrl_xfer32.cc`, after their existing
`RSP_COMMIT` and only for real/V86 completion.  Protected-mode handling uses
its existing `iret_protected` path and records nothing.  The hook must read
already-live scalar CPU state only; it must not inspect the popped stack frame,
read guest memory, alter CPU/flags/stack/exception delivery, request a stop or
call bx-vdm.

The profile groups are complete as one unit: disabled/default behavior,
IRET16 real mode, IRET32 real mode, V86 admission if reachable in the fixture,
protected-mode decline, lifecycle reset, ring ordering/wrap and retained
default graph.  Inapplicable V86 coverage must be explained in the result;
it may not be silently omitted.

## Required verification before use

1. Register `BX-CORE-089` before editing adopted code.
2. Add one explicit diagnostic build switch; the formal/default Ninja graph
   must remain unarmed.
3. Build a CPU5 MSVC x64 `/MT` fixture proving disabled behavior, IRET16 and
   IRET32 record/order, protected-mode negative behavior, reset/wrap and
   retained normal IRET delivery to the fixture's controlled stop.
4. Scan core/mantle sources for forbidden BOP/OpenNT/DOS/SoftPC/selector/
   address/filter vocabulary and verify no guest-memory helper is used.
5. Only after the profile closes, admit at most one bounded immutable
   Direct/Readonly CLI observation.  That observation may select a whole
   original owner package, never a leaf BOP or CPU repair.

## Stop conditions

Stop and re-admit if any implementation needs vector or service recognition,
a trace-address/opcode condition, stack-frame/guest-memory capture, CPU state
mutation, exception/result change, adapter callback, a device/firmware
feature, default product output, instrumentation activation, or a repair
claim.