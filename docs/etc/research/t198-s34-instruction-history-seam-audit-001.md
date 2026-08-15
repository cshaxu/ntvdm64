# T198 S34 Instruction-History Seam Audit 001

## Question

Can the adopted minimal Bochs CPU5 closure reuse an existing default-off,
selector-blind instruction-history callback to identify the instruction before
the S29 generic fault?

## Active Closure

The current exact CPU5 build projects `BX_INSTRUMENTATION=0`, `BX_DEBUGGER=0`,
and uses `refs/bochs/instrument/stubs` only as the compile-time instrumentation
header path. With `BX_INSTRUMENTATION=0`, every `BX_INSTR_*` macro in the
selected stub header expands to nothing. The x64 `/MT` finite closure links no
active instrumentation module.

## Candidate Map

| Candidate | Timing/data | Required closure | Disposition |
| --- | --- | --- | --- |
| Existing CPU-loop `BX_INSTR_BEFORE_EXECUTION` | Correct timing: it occurs before RIP advance and instruction execution. | Requires rebuilding CPU with `BX_INSTRUMENTATION=1` and supplying active instrumentation objects. | Not reusable as-is. |
| `instrument/stubs` | Has the nominal callbacks but their bodies are no-ops. | Enabling them changes the core feature projection and requires new object/link closure. | Rejected as an existing history facility. |
| `instrument/example0` | Prints opcode/memory/branch diagnostics; before hook receives `bxInstruction_c*`. | Active instrumentation plus logging, CPU-internal object dependence and output behavior. | Rejected: product-style diagnostic path, not fixed-width mantle state. |
| `instrument/example2` | Counts instruction classes and allocates state/logging. | Active instrumentation and heap/log lifecycle. | Rejected: statistics facility, no address history. |
| Built-in debugger/disassembly trace | May display instruction addresses. | `BX_DEBUGGER=1` product debugger path; current build has it disabled. | Explicitly rejected. |
| Existing generic `#UD` copied event | Fault-time fixed-width CS:EIP/window/state only. | Already in the selector-blind minimal closure. | Useful for fault capture, but it has no predecessor history. |

## Source Timing

In `bx-core/cpu/cpu.cc`, the non-chaining loop calls
`BX_INSTR_BEFORE_EXECUTION` before `RIP += i->ilen()` and before `i->execute`.
After successful execution it commits `prev_rip = RIP` and calls
`BX_INSTR_AFTER_EXECUTION`. Exceptions do not reach that after-execution path.
This timing is technically appropriate for history, but the current macros
compile away and the available implementations have unsuitable product/logging
or raw `bxInstruction_c*` coupling.

## Conclusion

There is no existing minimal, fixed-width, mantle-owned instruction-history
facility to reuse. Enabling Bochs instrumentation or debugger features would
broaden the CPU build and introduce product/debug dependencies merely to
diagnose one path; it is not admitted.

If trace evidence is still required, the only owner-correct design is a new,
explicit default-off **mechanical** core-to-mantle record at the already
existing CPU-loop before-execution location. It must record a tiny fixed-width
CPU state (at most CS:EIP and a bounded copied instruction window), ring it in
mantle-owned storage, expose no raw instruction pointer/object, interpret no
selector/service/guest data, and be enabled only by the finite fixture. That
requires a separate implementation S and Bochs exception registration before
any code changes.
