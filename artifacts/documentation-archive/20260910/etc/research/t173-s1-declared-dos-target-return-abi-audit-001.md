# T173 S1 declared DOS guest-target return ABI audit 001

## Question and inputs

Can the existing source-shaped `54:01` declared-target launch half gain a contained `54:11` return half without rehosting the historical NT command broker, redirection machinery, or host process model? The evidence is `cmddisp.c`, `cmdexec.c`, `cmddata.c`, the `retcode32` and `d16_return32` callers in `tcode.asm`, and the current typed COMMAND-plane/CPU-result ABI.

## Procedure

The two guest callers and the complete `cmdReturnExitCode` arm were compared field by field with the existing copied adapter result ABI.

## Observations

| Item | Original source fact | First-profile treatment |
| --- | --- | --- |
| `DX` | `retcode` (DOS child termination code) | Copy its low 16 bits only into adapter lifecycle state. |
| `AL` | `int 21h`, AH=`19h`, returns current drive | Do not consume or synchronize directories. |
| `BX:CX` | `RES_RDRINFO` redirection-completion address | Do not dereference. |
| Stack/segments | No BOP-owned stack or segment mutation | Preserve unchanged. |
| carry set | Historical broker has supplied another command | Not admitted: no queue exists in the first profile. |
| carry clear | `retcode32` enters `reent_exit`; `d16_return32` returns to prompt loop | Resume at `fault_rip + 4`, CF clear; guest owns each continuation. |

The guest clears resident `retcode` after the BOP returns. The adapter must not write that field or substitute `FE`; `BOP_UNSIMULATE` is a distinct terminal action.

`cmdReturnExitCode` builds `VDMINFO`, passes `DX` as `ErrorCode`, blocks an NT event thread, updates host directories from `AL`, dereferences `BX:CX` for redirection copying, then calls `GetNextVDMCommand`. It sets CF for a next command. With no next command it clears CF, sets only `AL` from the global `dwExitCode32`, resumes the event thread, and restores standard-handle notification.

`dwExitCode32` is zero-initialized in `cmddata.c`, but meaningful assignments here are `CreateProcess` error/exit outcomes in `cmdExec32`. It is not evidence that `AL` should equal guest `DX`; mapping `DX` to `AL` would invent semantics. The current bridge can instead express a checked four-byte resume, CF clear, and no GPR update without a pointer, handle, thread or guest-memory transaction.

## Bounded contract and interpretation

The provider may accept only real-mode vector-6 `C4 C4 54 11`, with a checked four-byte resume and an already-delivered `54:01` target state. It reads no guest memory, does not consume `AL` or `BX:CX`, retains copied `DX` as the target's fixed-width DOS status, resumes with CF clear, leaves AX/stack/segments/devices untouched, and completes once only.

Host-directory inheritance, redirection completion, event-thread control, host handles, `GetNextVDMCommand`, CF-set re-entry, and the historical `dwExitCode32`/AL behavior are explicitly unavailable. A later CLI-owned result ABI may expose the copied 16-bit status; it is not an AL emulation and does not authorize a host command queue.

Confidence is high: the original dispatcher and both guest continuations agree on the inputs and carry branch. T174 may implement this paired provider through the existing ingress/registry/COMMAND-plane route, proving positive, wrong-service, wrong-mode, repeated-return and AX-unchanged cases. It must not add a scheduler, redirection backend or FE substitution.
