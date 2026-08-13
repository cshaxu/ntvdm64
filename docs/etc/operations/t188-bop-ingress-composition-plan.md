# T188 BOP ingress composition plan

Status: admitted work plan, 2026-08-12.

## Medium-sized outcome

T188 recovers one coherent, generic BOP ingress path from Bochs #UD through the existing fixed-width adapter transaction boundary and back to checked CPU resume. It is not a `54:0C` repair: selectors and services remain in the host-service plane, while Bochs owns #UD, memory writes and CPU state.

| S | Result | Gate before next S |
| --- | --- | --- |
| S1 | Source/build closure map for #UD consumers, macros, objects, transactions and negative paths. | One generic consumer selected; no source change. |
| S2 | No-runtime adapter-plane predicate audit for the selected ingress. | One source-preserving fixture identifies or proves inability to identify the declining predicate. |
| S3 | Audit whether the accumulated adapter session state has an existing side-effect-free observation contract. | Either select a state observation seam or reject intrusive condition instrumentation. |
| S4 | Define and verify an adapter-owned copied state snapshot only because S3 proves no existing observer; then build its narrowest derivative and make one watchdog-bounded observation. | No retry; classify state/result/continuation separately. |
| S5 | Decide whether one default-off Bochs trace consumer of the copied state is warranted; register or reject the intrusion before source change. | No completion, host-drive, device or terminal-result claim. |
| S6 | Implement and statically verify the registered default-off copied-state trace. | No build or runtime action. |
| S7 | Audit and, only if closed, build the narrow derivative with the minimum compatible adapter object replacement. | No archive/full build; no guest run before a separate one-shot admission. |
| S8 | Make one watchdog-bounded observation only from the S7 frozen derivative. | No retry; classify only copied lifecycle state and continuation. |
| S9 | Closure audit and next-boundary disposition. | No completion, host-drive, device or terminal-result claim. |

## Invariants

- Bochs exports copied exception state and applies checked results only.
- The adapter selects existing plane/provider transactions but owns no CPU, BIOS, PIC, DOS kernel or device semantics.
- The ingress is selector-blind and default-off; a listener is never promoted into a dispatcher.
