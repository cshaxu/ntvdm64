# M0 T473 S3 — CPU40 execution lifecycle closure

## Decision

The selected Win32/x86 CCPU40 execution route has one execution and teardown
owner chain.  No missing original user-mode host body, competing local CPU
engine, or duplicate lifecycle state was found.  Consequently this packet
makes **no production-source, build-selection, package, installed-image, or
guest-media change**.

## Production chain and authority

| Stage | Actual owner | Authority | Boundary result |
| --- | --- | --- | --- |
| Worker admission | `mvdm_standalone_worker_begin` | one `session`, its selected SoftPC backend and one owned thread binding | rejects duplicate/inactive setup before CPU entry |
| Original execution | original `host_start_cpu` and selected CCPU40 archive | CPU register, SAS, BOP, BIOS and return semantics | project bridge does not decode or schedule CPU work |
| Termination | `session_terminate_current` | active session completion code and its armed `jmp_buf` | controlled termination sets completion exactly once and reaches the same close cohort |
| Original close | original `host_applClose` | original host producer teardown | called on both normal CCPU return and the termination escape |
| Borrowed host memory | `mvdm_softpc_guest_memory_begin/end` | bounded session lease only | lease begins before execution and ends after original close; it is not a second SAS owner |
| Worker retirement | `mvdm_standalone_worker_cleanup` | worker-owned observer, I/O, runtime attachment, session disposal and service disconnection | later worker/process cleanup cannot become CPU execution authority |

`mvdm_softpc_execution_run_until_return` is therefore a narrow host-resource
bracket: it validates/binds the selected session, begins the bounded memory
lease, arms the session escape, invokes original `host_start_cpu`, invokes
original `host_applClose` on either return path, then disarms/releases/unbinds
in reverse order.  It contains no CPU, BOP, BIOS, timer, descriptor or
scheduler policy.  `session_terminate_current` is the sole in-session
non-local exit; it records the completion before returning through that same
bracket.

## Evidence and limits

The unchanged selected route already has direct fixture evidence in the
current formal generator:

- `ccpu-thread-lifecycle-test` creates an abnormal worker return through the
  original `host_ExitThread`, then a fresh worker return, and records
  `CCPU thread lifecycle normal-and-abnormal OK` in the retained T420 witness.
- `ccpu-halt-reset-test`, `original-external-memory-test`,
  `ccpu-high-linear-page-test`, `ccpu-stack-transition-test` and
  `session-lifecycle-test` are all explicitly wired by
  `New-T310OriginalSoftpcNinja.ps1` against the same selected CPU40 archives
  and session carriers.
- T420's retained guest-console witness records the normal/abnormal fresh TLS
  frame sequence; this packet changes none of its inputs.

This packet attempted to rerun that focused suite using the historically
successful `build/.ninja/msvc-x86-formal-release/run-ninja-parallel.cmd`.
The retained formal graph was generated on 2026-10-24 and omits the now
selected `wow_hard_error_decoder.obj` from `softpc-win32-bindings.lib`; its
link failure is a stale generated-cache composition mismatch, not CPU40
execution evidence.  The local generator invocation did not refresh that
stable tree in this session.  Per owner direction, this non-current-cache
verification is skipped rather than converted into a product failure claim.
It remains a build-cache maintenance item, not a S3 runtime defect.  No
source changed, so the prior focused evidence remains applicable; it is not
misrepresented as a fresh rerun.

## Result and handoff

CPU40's execution/lifecycle seam is source-complete for the selected profile.
CPU30/private-kernel restoration, DPMI feature expansion and device behavior
remain outside this packet.  The next finite owner packet is DOS/COMMAND/XMS:
it must establish native-child and command retirement through the selected
original command callers without treating the CPU40 bridge as a replacement
shell engine.
