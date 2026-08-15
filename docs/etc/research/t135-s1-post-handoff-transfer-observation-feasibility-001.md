# T135 S1 post-handoff transfer-observation feasibility 001

## Required fact

T134 leaves one bounded gap: the trace knows that fetch observes
`CS:IP=0BC1:10000h` and that INT 0Dh subsequently returns to it, but not the
last original instruction position before that fetch.

## Existing mechanisms audited

| Mechanism | Why it cannot close the gap |
| --- | --- |
| `BX-TRACE-049` real-mode vector record in `exception.cc` | It begins after the prefetch check has already raised #GP; it preserves the invalid `0BC1:FFFF` fault frame, not the predecessor instruction. |
| `BX-TRACE-051` immediate far-jump record in `ctrl_xfer32.cc` | It covers only the immediate far-jump implementation. The trace provides no evidence that this instruction family produced the failing EIP. |
| Existing IRET and SS/SP diagnostics | They cover TF-bearing IRET or selected segment/stack writes; neither is a general predecessor record and neither appeared as the needed transition. |
| Bochs instrumentation callbacks | The current minimal composition uses the instrumentation stubs. Enabling a debugger/instrumentation product would add a separate feature/object closure rather than reuse an admitted diagnostic. |

## Exact boundary and closure

The original `BX_CPU_C::prefetch` in `refs/bochs/cpu/cpu.cc` already checks
`EIP > CS.limit` immediately before it calls the original #GP path. At that
point the CPU already retains `PREV_RIP`, current CS, EIP, and the locally
read limit. A default-off log of only those fields is sufficient to tell
whether the fault follows an instruction at `FFFFh` or a control transfer
from another address; it neither reads a guest byte nor attributes the
instruction to any guest component.

This is not part of the retained `exception.o` seam. `cpu.cc` belongs to
`cpu/libcpu.a`; therefore a future derivative must prove a direct-object
override or an exact archive rebuild/link closure before runtime use. The
T130 build record shows that an ordinary recursive native build can rebuild
that archive, so it is not evidence that the current narrow derivative may
silently do so.

## Disposition

No existing registered diagnostic can provide the predecessor. The minimally
scoped next step is **BX-TRACE-056**, registered above, followed by a
build-closure-only task. It is generic Bochs observability, not a BOP,
adapter, DOS, firmware, reset, or device feature. No source/build/runtime
action occurred in T135.
