# T177 S9 — Illegal-Opcode Vector Owner Audit 001

## Question

Does the first post-termination loop in the retained normal-return observation
indicate a failed adapter BOP service, a Bochs CPU defect, or a missing owner
in the historical machine composition?

## Exact retained observation

The sole S7 observation log, `artifacts/analysis/t176-s19-share-normal-return-
lifecycle-observation-001/bochs.log`, repeatedly records the same facts:

- `C4 C4 06` at `073B:0740` is seen by the adapter as selector `06`, with no
  host-service family or service identity.
- The adapter declines it unchanged.
- Bochs raises real-mode vector `06`; the IVT target is again `073B:0740`.
- Each retry reduces `SS:SP` by six bytes, from `A0C9` to `0001`, then the
  stack fault/triple-fault path stops execution.

The trace proves a self-referential guest vector target and Bochs's normal
real-mode fault-frame push. It does not identify the earlier guest instruction
or write that installed that target.

## Original ownership

The symbolic distinction matters:

| Selector | Original `BIOS[]` owner | Meaning |
| --- | --- | --- |
| `06h` | `illegal_op_int` | Invalid-opcode vector bridge. |
| `56h` | `MS_bop_6` | Separate debugger dispatcher. |

`src/opennt/base/mvdm/softpc.new/base/bios/bios.c` binds `06h` to
`illegal_op_int` and separately binds `56h` to `MS_bop_6`. The observed
three-byte selector is `06h`; it is therefore not a DEM/COMMAND/debugger
host-service request and must not be routed through the adapter host-service
plane.

`base/system/illegalp.c` shows the intended historical semantics:

1. read the faulting IP:CS from the real-mode exception frame at `SS:ESP`;
2. report the illegal instruction under the old NT host policy;
3. advance the saved IP by one byte (including `FFFFh` segment carry); and
4. invoke original `unexpected_int` so the guest's subsequent IRET sees the
   modified frame.

The pre-existing `src/bx-ntvdm-machine-composition/` component was deliberately
limited to BOP `02h` and the independently isolated `unexpected_int` object.
It has no selector-`06h` route, no saved-frame access contract, and no source
closure for `illegal_op_int`. This is intentional, not an adapter bypass.

## Disposition

The first unmet owner is the **historical SoftPC illegal-opcode machine
composition**, not a DEM/COMMAND provider and not an adapter semantic error.
Bochs correctly executes its generic #UD/vector/stack mechanics from the
guest IVT it observes. The immediate loop occurs because the minimal
composition has no admitted original `illegal_op_int` handler to consume the
`06h` BOP and alter the saved frame before guest continuation.

This is not yet proof that adding that handler produces a normal COMMAND
return: the source handler has a separate historical ABI and host-error policy,
and the trace lacks the predecessor that selected vector `06h`.

## Next bounded work

T177 S10 may perform a read-only, whole-handler source/ABI closure audit for
`illegal_op_int`: all imports, exception-frame access assumptions,
`unexpected_int` relation, and the minimum separately owned Bochs mechanics
needed to preserve its original behavior. It must not connect a handler,
change the adapter, enable a device, retry the guest run, or treat `06h` as a
host-service selector.
