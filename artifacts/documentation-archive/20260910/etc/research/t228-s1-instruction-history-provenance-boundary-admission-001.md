# T228 S1 Instruction-History Provenance Boundary Admission

## Decision

Repair the existing `bx_ntvdm_instruction_history` implementation so its
already registered scalar history and separately registered provenance capture
are independently compiled. This is enforcement of existing entries
`BX-CORE-071`, `BX-MANTLE-072`, `BX-CORE-074`, and `BX-MANTLE-075`; it does
not authorize another Bochs intrusion.

## Current Fault

The current mantle implementation includes `bx-core/memory/memory.h` and
calls `copy_from_ordinary_ram` whenever retained consecutive records change
CS. Therefore merely enabling the scalar history feature can read guest RAM.
That contradicts the first three entries above, which explicitly exclude
guest-memory access. The read belongs only to `BX-MANTLE-075`.

## Bounded Repair

`BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY` remains the existing default-off
CPU capture gate. A new, default-off
`BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE` gate will own the
memory header, real-address helper, provenance state, ordinary-RAM copies and
positive provenance query. With that gate absent, the public provenance query
remains ABI-stable but returns unavailable; the scalar ring and CS-transition
query continue to work without a guest-memory dependency.

No selector, BOP, OpenNT, DOS, address, opcode, service, profile, or adapter
condition is introduced. No CPU state changes and no core source changes are
admitted.

## Recovery Ledger

1. **Original source reuse:** not applicable: this is project-owned
   diagnostic storage around a Bochs CPU loop, not a recovered OpenNT owner.
2. **Smallest adapter/shim:** existing mantle owns the copied storage and is
   the narrowest component capable of separating the optional copied-RAM
   provenance from CPU capture.
3. **External-code intrusion:** no new intrusion. The pre-registered
   `BX-MANTLE-075` permits only the gated ordinary-RAM copies.
4. **New behavior:** none; the repair removes an unintended coupling and
   retains the registered copied-query contract.

## Verification

Build CPU5/Pentium-MMX MSVC x64 `/MT` fixtures in three states: default;
history enabled with provenance disabled; and history plus explicit provenance.
The first retains no records, the second proves ring and scalar transition but
no provenance, and the third alone proves the registered copied-RAM capture.
Run a source scan to ensure ordinary-RAM symbols occur only inside the explicit
provenance conditional, then rerun the formal Ninja module graph and project
documentation checks.

## Stop Conditions

Stop for re-admission if the repair needs a CPU semantic modification, a new
memory primitive, a selector/address/opcode branch, adapter/BOP linkage, or
any behavior beyond copied diagnostic observation.
