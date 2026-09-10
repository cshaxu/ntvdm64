# T228 S1 P11 - scalar CS-transition ring admission

## Purpose

P9 and P10 prove that source-built COMMAND transient code is intact at 300,000
ticks and that execution is already zero-filled by 1,000,000 ticks. The
existing BX-MANTLE-072/BX-MANTLE-075 facilities retain only the latest CS
transition, so they cannot show the ordered predecessor chain into the first
known zero region.

## Proposed boundary

A new default-off, mantle-only diagnostic retains a fixed ring of at most
sixteen existing copied scalar CS transitions. Each entry consists only of the
already admitted previous/current instruction-history records: sequence, CS,
RIP, SS, SP, and BP. It is populated only when two consecutive already copied
history records have different CS values.

The diagnostic has no guest-RAM read, no instruction-byte capture or decode,
no selector/address/service/BOP/OpenNT/DOS/SoftPC recognition, no CPU or guest
state mutation, no callback or pointer, no device/firmware action, and no
adapter dependency. The default graph and any scalar-history graph that does
not explicitly enable the new terminal observation expose no ring output.

## Source/ABI/failure ledger

| Rung | Disposition |
| --- | --- |
| Original source reuse | No OpenNT historical component owns Bochs execution-history retention. |
| Smallest existing seam | Extend project-owned mantle history, which already receives the scalar copied records from registered BX-CORE-071 and owns the latest-transition record. |
| External-code intrusion | None: bx-core record shape and Bochs execution behavior remain unchanged. |
| Newly authored behavior | A fixed copied ring/query and terminal wrapper are necessary because the existing seam preserves exactly one transition. |

The public diagnostic ABI is a versioned fixed-size C structure. It returns
unavailable when default-off, disabled, non-watchdog termination, malformed
caller storage, or zero captured transitions. It has no failure path that
alters native execution.

## Verification

1. Focused x64 /MT CPU5 fixture: disabled output unavailable; two or more
   neutral far transfers retain ordered scalar records; ring wrap preserves
   chronological order; reset clears output.
2. Source scan: no forbidden vocabulary or memory-copy primitive appears in
   the new ring paths.
3. Fresh explicit history/provenance Ninja derivative and existing formal
   default graph.
4. One paired immutable Direct/Readonly native observation, printed only by an
   explicit CLI option, with byte-identical output.
5. Documentation governance, diff check, review, commit and push.

## Stop conditions

Stop rather than expand this P if diagnosis requires instruction bytes,
guest-RAM capture, a selector/address filter, opcode/BOP interpretation, a
CPU result, a host capability, an adapter route, a device, or a source-built
guest modification.