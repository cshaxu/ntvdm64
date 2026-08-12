# T177 S10 — Illegal-Opcode Handler-Island Closure 001

## Inputs and method

This is a read-only closure audit of the existing compiled original object:

`artifacts/build/current/t119-unexp-mt-projection-r2/CMakeFiles/ntdos64-
opennt-system-provider-objects.dir/overlay/base/mvdm/softpc.new/base/system/
illegalp.c.obj`.

`llvm-nm --undefined-only` reports its semantic imports as:

```text
_c_effective_addr  _c_getESP  _c_getSS  _c_sas_hw_at  _c_sas_store
_host_error  _unexpected_int
```

The remaining cookie and `__stdio_common_vsprintf` imports are modern CRT
support for the original `sprintf` formatting path. The object exports
`_illegal_op_int` (and unrelated sibling routines retained in the same source
unit); this is object evidence only, not a runnable link result.

## Complete behavior and ownership

| Original dependency | Exact use in `illegal_op_int` | Correct owner/disposition |
| --- | --- | --- |
| `c_getSS`, `c_getESP`, `c_effective_addr` | Locate the CPU-created real-mode fault frame. | A bounded native CPU-state/frame context could provide it; never adapter state. |
| `c_sas_hw_at`, `c_sas_store` | Read four frame bytes plus diagnostic opcode bytes; modify saved CS:IP for the historical continue branch. | Checked Bochs ordinary-RAM reads/writes in a dynamically scoped machine context. |
| `unexpected_int` | Preserve original PIC/BDA acknowledgement before the guest `IRET`. | Existing selector-02 isolated handler island. |
| `host_error`, `sprintf` | Historical NT host error presentation before the continue path. | The original NT host implementation is a finite CLI-rehost candidate: it emits an error dialog and returns `ERR_CONT`. |

The existing selector-02 context has only PIC-port reads/writes and ordinary
RAM write. It intentionally lacks CPU-state/frame reads, ordinary-RAM reads,
and an error-policy callback. Adding generic SAS or CCPU objects would violate
the hard boundary. A finite extension must instead add only checked RAM read,
copied real-mode `SS:ESP`, and a bounded error-report callback.

## Decision

The original handler object has a finite mechanical *shape*. The initially
suspected policy blocker is disproved by the actual OpenNT NT implementation:
`host/src/nt_error.c:182-201` calls `ErrorDialogBox` but unconditionally
returns `ERR_CONT`; `error.h:163-175` confirms `ERR_CONT == 2` and
`ERR_QU_CO_RE` exposes quit/continue/reset choices. `illegal_op_int` ignores
the return value and follows its documented continue path.

The CLI equivalent must make the fault visible through a bounded report and
then return the same `ERR_CONT` value. This is a source-derived rehost of the
historical host presentation, not an adapter service and not an automatic
semantic invention. It cannot claim that the guest fault is harmless; it only
preserves the source-selected NT continuation behavior.

## T177 consequence

The current T176 S21 may now admit one bounded machine-composition contract
for the original handler island. It must retain the source object unchanged,
keep selection outside Bochs and adapter, and prove default decline for all
non-`06h` machine selectors before any runtime observation.
