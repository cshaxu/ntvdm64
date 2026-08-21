# M0 T225 S58 — BOUND instruction-history diagnostic admission 001

## Question

Can the current post-S56 `BOUND` observation be attributed to an actual guest
instruction without introducing a CPU semantic change or selecting an
opcode-level repair?

## Inputs

- The immutable S56 Direct/Readonly inputs and completed real/V86
  segment-limit profile.
- The paired S58 first-fault observation: both modes report vector 5 at
  `0C41:1949`, `EAX=00000D6F`, mode 1, and the original Bochs
  `BOUND_GdMa: fails bounds test` marker.
- The verified ordinary COM relation, `runtime IP = COMMAND.COM file offset +
  0100h`, from the same source-built COMMAND image.
- `src/bx-core/cpu/instr.h`, `src/bx-core/cpu/cpu.cc`, and
  `src/bx-mantle/bx_ntvdm_instruction_history.*`.
- Registered exception `BX-CORE-071` in
  `adapter-external-intrusion-exceptions.md` and its T198 S36 witness.

## Current limitation

The first-fault record is an exception-delivery state, not an instruction
boundary witness.  Applying the proven COM relation places `0C41:1949` at
file offset `1849h`; those bytes are the surrounding `CMP AX,3D02h` sequence,
not a `BOUND` opcode.  It is therefore invalid to map that EIP directly to a
source line or repair it.

## Admitted diagnostic boundary

`BX-CORE-071` already provides a selector-blind, default-off, fixed-width
ring of execution positions before each native instruction.  Its record has
only version, CPU id, instruction count, CS:RIP and SS:SP; it has no decoded
instruction, BOP, OpenNT, DOS, provider, host, guest-memory, or mutation-mode
field.  T198 S36 proves that the per-step capture, unlike the rejected trace
entry hook, identifies the direct predecessor under handler chaining.

S58 may enable this already registered compile-time diagnostic only in a
separate disposable MSVC x64 `/MT`, CPU5/P-MMX diagnostic graph.  The CLI may
offer a default-off observation switch which configures a bounded capacity,
copies the retained records after the existing first-fault controlled stop,
and prints them.  It must not retain pointers, decode instructions, read or
write guest memory, alter CPU state, select BOP services, or change the normal
runtime graph.  The normal S56 build remains without this diagnostic macro.

## Required verification

1. The diagnostic graph rebuilds the complete affected CPU closure with only
   `BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1` added to its recorded
   configuration.
2. A focused fixture proves disabled/default state, bounded capacity, ordered
   wrap and exact pre-execution positions.
3. One Direct and one Readonly invocation of the immutable S56 inputs use the
   new diagnostic jointly with the existing first-fault control.  Their copied
   record sequences, first-fault record, exit/terminal, and stderr marker must
   match byte-for-byte.
4. Only after static disassembly of the last retained position identifies the
   actual `BOUND` encoding may S58 classify its owner.  That classification
   still selects a complete compatibility profile or guest-layout domain, not
   a leaf patch.

## Rejected alternatives

- Treat the copied first-fault EIP as an instruction boundary: contradicted by
  the immutable COMMAND bytes.
- Add an opcode-specific BOUND observer or change `BOUND_GdMa`: prohibited by
  the active packet and unnecessary because the existing generic history
  mechanic supplies the missing fact.
- Infer a BOP or provider cause from the trace: prohibited; no selector or
  service data participates in this diagnostic.
