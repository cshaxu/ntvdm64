# T95 S6 Passive x87 Compatibility-Diagnostic Design 001

## Decision

The only acceptable next diagnostic candidate is a **default-off Bochs-local
snapshot at the already-existing x87 compatibility-IRQ13 branch**. It is not
an adapter feature, a guest instruction tracer, a debugger, or an execution
control mechanism. It must observe and log fixed-width native CPU/x87 fields
immediately before the existing `DEV_pic_raise_irq(13)` call, then execute that
unchanged call.

This is deliberately narrower than the earlier phrase “instruction/x87-state
diagnostic”: current source evidence supports an x87-state snapshot at the
existing branch, not a faithful arbitrary instruction trace. The snapshot
will help distinguish reset state from a prior recorded x87 state, but it must
not claim the exact triggering instruction unless its fields independently
support that conclusion.

## Existing mechanisms considered

| Mechanism | Evidence | Decision |
| --- | --- | --- |
| Existing adapter passive observation | `design/ADAPTER-PASSIVE-OBSERVATION-TRIGGER.md` reaches an adapter transaction only at profile-declared `UD2`; its range read is already restricted to an exact `#UD` boundary. | Rejected for this issue. Inserting `UD2` into NTIO would mutate the guest path; an arbitrary address/exception extension would turn the adapter into a tracer. |
| Bochs instrumentation framework | `artifacts/build/bochs-2.6-native-adapter-msvc-r2/config.h:619` sets `BX_INSTRUMENTATION 0`; `cpu/cpu.cc` invokes hooks only through that disabled framework. `instrument/example1` is a broad execution callback family. | Rejected. Enabling it changes the native build feature set and admits a general tracing surface. |
| Built-in debugger instruction trace | `cpu/cpu.cc` guards `debug_disasm_instruction` with `BX_DEBUGGER`; Bochs documentation ties instruction/register trace to debugger support. | Rejected. It adds debugger/disassembly/product facilities and emits an unbounded general trace. |
| Existing FPU compatibility branch | `fpu/fpu.cc:56-74` already tests x87 pending-summary state, chooses `#MF` or compatibility IRQ13 via `CR0.NE`, emits the observed log, then calls `DEV_pic_raise_irq(13)`. | Selected diagnostic site. It is reached by the exact symptom and requires no new dispatch point or guest trigger. |

## Candidate boundary

If a later implementation admission is made, it must first register a new
Bochs intrusion exception. The exception may alter only `refs/bochs/fpu/fpu.cc`
and only under a new named compile-time macro that defaults to disabled.

When enabled, it may emit one structured, parseable line **per existing
compatibility-IRQ13 event**, before the original info log and IRQ call. Its
record is fixed to these already-owned values:

| Field | Reason |
| --- | --- |
| CPU identifier | The existing CPU owner supplies it; no cross-island identity. |
| current RIP and CS selector | Correlates the native snapshot to the retained log without a new fetch/decode callback. |
| CR0 value and `NE` bit | Proves the branch selector already implicit in the original code. |
| x87 control word, status/partial-status word, tag word, TOP | Captures the predicate and state used by the original FPU owner. |
| x87 last-operation fields (`foo`, `fcs:fip`, `fds:fdp`) | Preserves Bochs's existing last-x87 provenance where it exists; it is not asserted to be the current instruction. |

The record must contain no guest-memory bytes, physical/linear range request,
host path, BYOB field, adapter object, BOP/DEM/DOS selector, OpenNT term,
pointer, callback, counter-based stop, or resume/result disposition. It must
not allocate memory, modify FPU/CPU/PIC state, suppress or coalesce events, or
decide whether IRQ13 is handled.

## Required negative proof before any implementation

1. The source-boundary test must prove the macro is disabled by default and
   that the original `FPU_check_pending_exceptions` predicate and
   `DEV_pic_raise_irq(13)` sequence are unchanged.
2. A same-island object/link observation must show that the default-off r2
   companion has no new object, library, adapter or CLI dependency.
3. Only then may one fresh-root runtime observation reuse the immutable NTIO
   image and current PC-reset-floor profile. It may read the new log record;
   it may not change the profile, input bytes, CPU level, ROM, device set,
   interrupt vector, VDM state, host capability, or benchmark stop policy.
4. A missing record, a watchdog, or any changed terminal behavior ends that
   single observation and is evidence, not a retry reason.

## Ownership result

Bochs retains ownership of the x87 predicate, x87 fields, CR0 branch and PIC
delivery. The adapter remains uninvolved because no typed cross-island
operation is needed to diagnose a Bochs-native event. OpenNT remains the
owner of NTIO semantics; its source is used only to correlate the immutable
guest input after the observation. This keeps the diagnostic reusable for a
future native profile without allowing either side to absorb the other's
semantics.
