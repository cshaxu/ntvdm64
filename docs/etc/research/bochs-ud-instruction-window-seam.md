# Bochs #UD Instruction-Window Seam Research

## Question

After the adapter identifies an admitted selector, how can it establish that
the faulting guest instruction is the exact historical boundary without making
the adapter a Bochs-memory client or re-reading guest memory during exception
delivery?

## Source findings

In this pinned Bochs 2.6 import:

- `cpu/cpu.cc` obtains an already decoded `bxInstruction_c` from the
  instruction cache, advances `RIP`, then calls its executor. A faulting
  instruction never reaches the normal post-execution commit.
- `cpu/exception.cc` receives `prev_rip`, which remains the precise faulting
  instruction address for the existing generic #UD event.
- `cpu/icache.cc` decodes from `eipFetchPtr`; it also handles page-split
  instructions through `boundaryFetch` before execution.
- `cpu/paging.cc` exposes `access_read_linear`, but that routine translates
  and may itself raise a page fault. It is therefore not safe to call from the
  exception interception path merely to inspect opcode bytes.
- `eipFetchPtr` is a private host pointer, not a bridge value. Exporting it
  would break the typed boundary and leak Bochs object semantics.

## Rejected approaches

| Approach | Rejection reason |
| --- | --- |
| Adapter rereads guest memory | Would require Bochs memory access semantics in the adapter. |
| Call `access_read_linear` in #UD handling | A read can translate/fault and change the original exception outcome. |
| Pass `eipFetchPtr` | It is an internal host pointer and not stable across page-split handling. |
| Infer bytes from selector alone | Admits non-historical #UD input and turns a selection record into a decoder. |

## Minimum viable next seam

If owner entry is separately approved, the smallest generic Bochs intrusion is
a fixed-size, copied instruction-window field prepared at the already-complete
fetch/decode point and attached to the existing #UD request. The field must
contain only bytes and their valid length, never an address, pointer, or Bochs
object. `prev_rip` remains separately represented by the existing CPU state.

This is a mechanics-only observation transport: Bochs does not recognize BOP,
selector `0x50`, DOS, or OpenNT. The adapter would be the only place comparing
the copied bytes with the historically admitted pattern and then consulting
the one-entry selector map. No implementation is admitted by this research
record; a new external-intrusion register entry and focused positive/negative
fixture are prerequisites.
