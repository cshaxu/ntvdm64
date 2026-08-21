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
3. Replaced only the detached CCPU thread, process-global `SetStdHandle`, and
   `ExitThread` mechanics.  The body executes in the active one-session typed
   call; streams are decoded from fixed-width tokens into child-only
   `STARTUPINFO` endpoints.
4. Corrected the pre-existing RTL compatibility shim so the OpenNT
   non-allocating `RtlUnicodeStringToAnsiString(..., FALSE)` command-buffer
   contract succeeds instead of incorrectly returning `ERROR_INVALID_DATA`.
5. Compiled changed sources under the formal MSVC x64 `/MT /W4 /WX` command
   shape.  Linked the changed objects into a source-current `bx-vdm` archive
   and ran the focused T236, retained T231 and T234 fixtures.

## Dependency Ledger

| OpenNT dependency | Source disposition | Result |
| --- | --- | --- |
| `cmdCreateProcess` worker body | Direct imported source | Compiled and invoked by imported `cmdExec32`. |
| CCPU `CreateThread(cmdCreateProcess)` | Smallest composition seam | One active `bx-vdm` session executes the same worker body synchronously because the typed BOP call owns checked guest-copy lifetime.  Bochs remains the SoftPC-equivalent machine engine; it receives no COMMAND meaning. |
| `INCREMENT/DECREMENT_REENTER_COUNT` via `GetNextVDMCommand` | Source-shaped session shim | `vdmapi.h` values and ordering retained; session records current and peak reentrancy. |
| `GetNextVDMCommand` command/environment exchange | Existing source-shaped session provider | Retained; its command/environment producer is session-owned and uses the imported COMMAND layouts. |
| `BaseSrvGetNextVDMCommand` CSR message | Explicit unavailable private protocol | Modern public Win32 has no compatible CSR client contract.  Its required local reentrancy and command-record outcomes are retained above; remote/multi-VDM broker behavior is not claimed. |
| `SetVDMCurrentDirectories` BaseSrv message | Existing source-shaped session shim | Imported `cmdSetDirectories` retains construction/order; copied multisz is published to the active session. |
| `SetStdHandle` save/install/restore | Smallest child-only stream seam | Original three-stream ordering retained; opaque tokens become inherited child endpoints only.  The CLI process handles are verified unchanged. |
| `CreateProcess`, `ResumeThread`, `WaitForSingleObject`, `GetExitCodeProcess` | Direct public Win32 reuse in original body | Retained in imported worker body. |
| `ExitThread` | Typed-call return seam | Worker returns in the active BOP call so its checked guest-copy context remains valid. |
| keyboard event helpers in `fakekbd.c` | Deferred explicit session disposition | Block/resume state remains observable in the session.  Console-input injection requires the separate console/input owner and is not claimed here. |

## Observations

- `cmdexec.c`, `command_misc_shim.c`, `command_binary_shim.c`, and the focused
  fixture compile with the formal flags.
- The T236 fixture passes Direct exit, COMSPEC exit, invalid opaque stream
  token, anonymous stdout pipe, balanced reentrancy and CLI standard-handle
  isolation.
- Retained T231 lifecycle and T234 current-directory fixtures also pass against
  the same source-current archive.
- A fresh formal Ninja graph at `build/M0-T236-S2/001` dry-runs the affected
  source closure.  Its executor retains the known `cmd.exe` stall; no full
  Ninja execution pass is claimed.

## Interpretation And Confidence

High confidence for the declared one-session local child profile.  S2 expands
original-source reuse materially: the worker algorithm now belongs to imported
OpenNT code, while each uncomposable historical product mechanism has one
named, bounded replacement.  This is not a claim of CCPU, CSR/BaseSrv,
multi-session, Redirector or WOW recovery.

## Follow-Up

T236 S3 owns DEM hard-error/retry.  The retained [pending BOP session-lifecycle
candidate](../operations/proposal-bx-vdm-pending-bop-session-lifecycle-001.md)
may later add asynchronous guest slices only after it has a fixed typed
completion contract; it must not reopen this worker as a Bochs semantic feature.
