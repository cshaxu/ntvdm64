# T177 S1 Vector And Stack Predecessor Attribution 001

## Question

What owns the reached transition from the post-COMMAND path toward the
real-mode `0000:019d` stack fault, and which components are excluded before
any repair package is considered?

## Inputs

- the retained vector records in `t158-s1-real-mode-vector-observation-001`;
- the T176 S24 reproduction of the same terminal stack state;
- `dos/v86/doskrnl/bios/spckbd.asm`; and
- the T156 Bochs stack-mechanics audit.

## Transition Map

| Edge | Evidence | Owner/disposition |
| --- | --- | --- |
| Post-COMMAND state to vector 29 | T158 observes `0032:379b -> 0070:0254`. | Reached guest interrupt delivery; no adapter result is active at this edge. |
| Guest vector 29 to vector 10 | T158 observes `0070:0260 -> 073b:089d`. | Reached guest control flow; exact guest source offset is not asserted. |
| Fullscreen INT10 path | `spckbd.asm` `int10h_vector` branches to `nativebios`; `jmp_native` is an `EA` far jump through `host_int10`. | Original guest BIOS glue, not Bochs selector decoding or an adapter service. |
| `host_int10` target capture | `InstSpcKbd` saves IVT `10h` offset/segment at IVT words `40h/42h` into `host_int10`. | Historical contract requires an initialized machine INT10 vector before the guest captures it. |
| Captured target | T158 and its cited same-family direct-entry record establish target `0000:0000` under the direct pre-POST composition. | Missing native POST/IVT precondition, high confidence. |
| Low-memory execution to vector 1 and stack failure | T158 observes repeated vector 1 delivery as CS=0000 advances; T156 proves the later `#SS`/triple-fault mechanics are native and architecturally enforced. | Exact byte/source label remains unproven; Bochs correctly delivers the fault rather than owning its cause. |

## Excluded Owners

The chain occurs after the observed adapter transactions and before any
observed selector `06h`. It contains no adapter result transition, no adapter
BOP provider outcome, and no optional-device admission. The current evidence
therefore excludes adapter BOP planes, `illegal_op_int`, and optional devices
as the owner of the zero INT10 target or the later stack fault.

## Interpretation

The first repair-relevant edge is the guest's capture of a zero machine INT10
vector in the direct pre-POST composition. T177 S1 does not authorize a
repair: it establishes the required owner split. A following cross-check must
show whether the already adopted native POST/deferred handoff composition
actually provides the non-zero precondition in the current source-built
derivative before any new lifecycle work can be admitted.
