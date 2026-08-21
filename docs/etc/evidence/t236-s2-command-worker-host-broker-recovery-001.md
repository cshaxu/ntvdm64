# T236 S2 COMMAND Worker And Host-Broker Recovery

## Question

Can the imported OpenNT `cmdCreateProcess` worker be compiled and used by the
single-session CLI without recreating CCPU, CSR/BaseSrv, or mutating the CLI
process's standard handles?

## Inputs

- OpenNT `base/mvdm/dos/command/cmdexec.c`, `cmdenv.c`, `cmdmisc.c`, and
  `cmdredir.c` imported under `src/bx-vdm/bop/opennt/command/`.
- OpenNT `base/win32/client/vdm.c`, `base/win32/server/srvvdm.c`, and
  `public/internal/base/inc/vdmapi.h` as host-broker and reentrancy evidence.
- The existing fixed-width COMMAND session, opaque host-handle manager and
  T234 environment/current-directory recovery.

## Procedure

1. Compared every `cmdCreateProcess` dependency with its OpenNT client/server
   origin before selecting a seam.
2. Admitted the imported worker body and retained its reentrancy record,
   drive-current-directory lookup, OEM/ANSI conversion, `cmdXformEnvironment`,
   `CREATE_SUSPENDED`, resume, wait, exit-code, cleanup and decrement order.
3. Replaced only the CCPU/private-broker transport, process-global
   `SetStdHandle`, and `ExitThread` mechanics.  `cmdExec32` now returns a
   typed `PENDING` outcome while the imported `cmdCreateProcess` body runs on
   one detached session worker; streams are decoded from fixed-width tokens
   into child-only `STARTUPINFO` endpoints.
4. Corrected the continuation initialization order: the copied double-NUL
   environment had been cleared before the worker reached the retained OpenNT
   `cmdXformEnvironment`/`CreateProcess` sequence.  Initializing before that
   copy restores the original worker's input contract without changing it.
5. Compiled changed sources under the formal MSVC x64 `/MT /W4 /WX` command
   shape.  Linked the changed objects into a source-current `bx-vdm` archive
   and ran the focused T236, retained T231 and T234 fixtures.

## Dependency Ledger

| OpenNT dependency | Source disposition | Result |
| --- | --- | --- |
| `cmdCreateProcess` worker body | Direct imported source | Compiled and invoked by imported `cmdExec32`. |
| CCPU `CreateThread(cmdCreateProcess)` | Smallest composition seam | One active `bx-vdm` session owns a detached worker and copied fixed continuation.  The imported `cmdCreateProcess` body remains the worker algorithm; Bochs remains the SoftPC-equivalent machine engine and receives no COMMAND meaning. |
| `INCREMENT/DECREMENT_REENTER_COUNT` via `GetNextVDMCommand` | Source-shaped session shim | `vdmapi.h` values and ordering retained; session records current and peak reentrancy. |
| `GetNextVDMCommand` command/environment exchange | Existing source-shaped session provider | Retained; its command/environment producer is session-owned and uses the imported COMMAND layouts. |
| `BaseSrvGetNextVDMCommand` CSR message | Explicit unavailable private protocol | Modern public Win32 has no compatible CSR client contract.  Its required local reentrancy and command-record outcomes are retained above; remote/multi-VDM broker behavior is not claimed. |
| `SetVDMCurrentDirectories` BaseSrv message | Existing source-shaped session shim | Imported `cmdSetDirectories` retains construction/order; copied multisz is published to the active session. |
| `nt_block_event_thread` / `nt_resume_event_thread` | Source-shaped session shim | Preserve the imported call order by incrementing/decrementing the session's observable event-block state; no CSRSS event thread is fabricated. |
| `nt_std_handle_notification` | Source-shaped session shim | Preserves the completion notification point as session state. Console/input injection remains deferred to its own owner. |
| `SetStdHandle` save/install/restore | Smallest child-only stream seam | Original three-stream ordering retained; opaque tokens become inherited child endpoints only.  The CLI process handles are verified unchanged. |
| `CreateProcess`, `ResumeThread`, `WaitForSingleObject`, `GetExitCodeProcess` | Direct public Win32 reuse in original body | Retained in imported worker body. |
| child containment / cancellation | New fixed-width session seam | A public Job object, worker/event IDs and cancellation intent are opaque manager entries. Dispose records intent before waiting, including the `CreateProcess`/Job-publication race. |
| `ExitThread` | Typed worker-return seam | The imported body returns to a `CreateThread` wrapper after completion; no active BOP-call pointer crosses the asynchronous boundary. |
| keyboard event helpers in `fakekbd.c` | Deferred explicit session disposition | Block/resume state remains observable in the session.  Console-input injection requires the separate console/input owner and is not claimed here. |

## Observations

- `cmdexec.c`, `command_misc_shim.c`, `command_binary_shim.c`, and the focused
  fixture compile with the formal flags.
- The T236 fixture passes `PENDING` then completion for Direct and COMSPEC
  exit; it also passes invalid opaque-stream-token rejection, anonymous stdout
  pipe, balanced reentrancy, CLI standard-handle isolation, negative
  double-completion, and dispose/cancellation with zero remaining opaque
  tokens.
- `cmdexec.c`, `command_misc_shim.c`, `command_native_session_shim.c`, the
  typed-result object, and the changed selector-blind core/mantle units compile
  under the formal MSVC x64 `/MT /W4 /WX` command shape.  A source-current
  archive containing those objects links and runs the focused fixture.
- The dedicated selector-blind machine fixture runs a real-mode `#UD` to the
  new mechanical `PENDING` terminal state, retains the machine, then performs
  another controlled execute which reaches the same faulting instruction and
  accepts a test-only generic `STOP`.  It contains no BOP/provider vocabulary.
- The native `54:08` fixture initializes an actual real-mode Bochs stage with
  `C4 C4 54 08`, its original `DS:SI`, `ES`, and `SS:BP` inputs, and the real
  adapter composition bridge.  It observes `PENDING`, waits for the imported
  worker, resumes the exact BOP, verifies `AX=0125` (`AL=37`) and the original
  CR-to-NUL command-tail mutation, then reaches the bounded guest `HLT`.
- A fresh formal Ninja graph at `build/M0-T236-S2/001` dry-runs the affected
  source closure.  Its executor retains the known `cmd.exe` stall; no full
  Ninja execution pass is claimed.

## Interpretation And Confidence

The imported worker is now exercised through the intended detached topology,
not the prior synchronous P1 bridge.  The pending/resume machine witness is
also present.  The remaining S2 work is the final source/ABI/failure-ledger
review and formal graph execution; it is not a claim of CCPU, CSR/BaseSrv,
multi-session, Redirector or WOW recovery.

## Follow-Up

Finalize the source/ABI/failure ledger and formal graph execution, then close
S2.  T236 S3 owns DEM hard-error/retry.
