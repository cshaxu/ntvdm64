# T479 S1 guest-bridge research closure

The owner accepts the current research results and directs S1 closure.
This is a bounded research closure, superseding the earlier requirement to
finish executable ABI proofs within S1. It does not establish runtime
feasibility for every required behavior or waive those proofs before release.
T479 remains open; S2 is planned, not admitted by this closure.

## Delivered findings

The [call-chain record](../operations/m0-t479-s1-guest-callchain.md), delivered
through 4c9d5087c, traces both COMMAND services through the blocking cmdExec32
path and the CCPU BOP return boundary. It records singleton-state constraints,
actual SAS paging ownership, exact VM/frontend identity requirements and
the separate NIDDB lifecycle problem. Historical experiments are distinguished
from current-package evidence, including duplicate-input failures.

The candidate uses a VMM callback after unwinding the host BOP, a guest-side
semaphore wait and an emulated notification device. It excludes periodic
suspend/resume, background-thread guest memory access, hardware V86 monitor
reintroduction and blind forwarding of B1 initialization. It is a candidate
to validate, not a frozen, proven runtime ABI.

## Mandatory handoff

| Stage | Retained obligation and gate |
| --- | --- |
| S2 | Recover/pin the add-on toolchain; validate loading, VMM identity, callback entry, notification resource and event delivery, registration/retirement, system/pre-existing/reused VMs and environment transitions. Resolve NIDDB device-state requirements separately. Freeze only the ABI proved by these checks. |
| S3 | Both 54:08/0A continuations, legal semaphore wait and destruction, signal-before-wait, full caller restoration, two VMs in both completion orders and NTSRV-controlled child completion. No retained host BOP stack. |
| S4 | Full consumed Console-opcode mapping, single-consumption keyboard/input, focus/encoding, final-frame serial, geometry/graphics, cancellation and nested chains. A byte-pipe or text-only demonstration cannot close this stage. |
| S5 | Integrated DOS/native/GUI/redirection and Win3.1 enhanced/Win95 lifecycle regression, coherent cached build, publication and final evidence audit. |

The callback, event, input, presentation and lifecycle proof gates previously
listed under S1 are transferred to these named stages, not declared passed.
New semantic concessions still require approval. The full T objective and
immutable-guest boundary remain unchanged.

## Delivery scope

The guest research changes only documentation; it adds no runtime/mirror diff,
guest binary or product image. Existing published 0.0.479/protocol-67 ten-image
package remains in place. No rebuild or repeated product regression is needed
for this closure. The separate [generic-thunk side integration](m0-t479-s1-thunk-import.md)
retains its own Full 16/16 evidence; those passes do not validate the guest bridge.

Closure verification consists of documentation governance and Git whitespace/
diff review. Disposable logs are owned by this closure under build/t479-s1;
no running product build or test is interrupted. Owned hidden verifier PID
21780 exited zero: documentation governance and `git diff --check` passed.
Logs are `build/t479-s1/closure.stdout` and `.stderr`.
