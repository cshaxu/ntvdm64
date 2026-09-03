# M0 T388 S7 — fixed-container interactive COMMAND observation

## Scope

This record preserves a single formal CPU40/x86 product observation after two
source-shaped local repairs:

- BOP diagnostics are written only to an explicitly supplied host report, not
  to the guest-facing Console.
- The local BaseSrv counterpart now uses one `hWaitForVDM`-shaped event for
  both record publication and a completed re-entry transition.  The earlier
  two-event seam made an ordinary no-command wait observe an initially
  signalled re-entry event.

Neither change modifies guest media, creates an app-owned command parser, or
injects keyboard text.

## Formal product and container

- Product: `build/M0-T386/S3/base-env-x86/original-softpc-process.exe` after
  the formal incremental relink.
- Fixed root: `O:\ntvdm64`.
- Container: `build/tools/console-startup-observer.exe`, owning real
  `CONIN$`/`CONOUT$`, non-debug, eight-second bound.

## Results

| Host environment | `54:0F` required paragraphs | Result | Interpretation |
| --- | ---: | --- | --- |
| inherited modern environment | `024D` | bounded timeout; Console contains no project diagnostics | The original COMMAND path is reached, but its inherited environment remains too large for the reached transient/arena layout. |
| six-variable control environment | `0047` | exit `0x00000000` before the timeout | The transient overwrite does not occur, but the no-argument interactive lifecycle still exits after original `54:01`, `54:0E`, and `54:04` handling. |

The control run is diagnostic only: it does not define a product environment
policy or authorize truncating the user environment.  `BaseCreateVDMEnvironment`
is already imported and called unchanged; its original purpose is path
normalization/copying, not an environment-size cap.

## Current owner conclusions

- The oversized-environment issue belongs to the original COMMAND/NTDOS
  arena-and-environment contract plus the host launch environment.  It is not
  a Console, BOP routing, or SoftPC instruction-fetch failure.
- The clean-environment early exit is a separate reached original
  COMMAND/BaseVDM lifecycle edge.  It remains open for S7 and must be traced
  through source-owned `54:01`/`ExitVDM` behavior before changing any
  launch-record field.
- S7 is **not closed**.  No prompt, `ver`, `dir`, editing, Ctrl+C, or `exit`
  row is claimed by this evidence.
