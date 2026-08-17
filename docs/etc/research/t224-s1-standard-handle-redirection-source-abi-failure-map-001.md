# T224 S1 Standard-Handle And Redirection Source/ABI/Failure Map

## Question

What complete original owner package is required for guest DOS/COMMAND
standard handles, host-child redirection, and pipe completion; what is the
current bound route; and which Direct/Readonly outcomes are admissible before
implementation begins?

## Inputs

- OpenNT COMMAND dispatcher and ABI: `base/mvdm/dos/command/cmddisp.c`,
  `cmd.h`, `cmdmisc.c`, `cmdexec.c`, `cmdredir.c`, and `inc/cmdsvc.h`.
- Guest callers: `base/mvdm/dos/v86/cmd/command/init.asm` and `tcode.asm`.
- DOS continuation and DEM endpoints:
  `base/mvdm/dos/v86/doskrnl/dos/handle.asm`,
  `base/mvdm/dos/dem/demdisp.c`, `demhndl.c`, and `inc/dossvc.h`.
- Current routes: `bx_ntvdm_command_stream_child_v1.*`,
  `bx_ntvdm_command_execution_lifecycle_v1.c`,
  `bx_ntvdm_command_package_session_v1.c`, and the DEM package session.

## Original Owner And ABI Map

| Identity / dependency | Original owner and caller | Historical copied contract and terminal behavior | Current bound route | Direct / Readonly disposition | Recovery decision |
| --- | --- | --- | --- | --- | --- |
| bootstrap redirection snapshot (not a BOP) | `cmdmisc.c:356-358` calls `cmdCheckStandardHandles`; `cmdredir.c:183-223` | Copies stdin/stdout/stderr presence into `CMDINFO.bStdHandles`, stores a 32-bit `pRdrInfo`, and notifies SoftPC redirection state. No redirection stores NULL. | No session-owned source-shaped replacement; `stream_child` captures ambient standard handles during provider initialization. | Direct: later CLI capture capability; Readonly: no host-global stream mutation/capture by default. | Replace the ambient initialization with one CLI-admitted, session-owned stream snapshot before any service binding. |
| `54:06` `SVC_GETSTDHANDLE` | `cmdredir.c:225-315`; `tcode.asm:1567-1573` | CX selects stdin/out/err; AX:BX carries `pRdrInfo`; returns a 32-bit handle in BX:CX, zero DX:AX size, CF clear. Pipe conversion failure terminates VDM and sets CF. | `command_stream_child` recognizes only this service and returns an adapter opaque token in BX:CX for Direct; unavailable/direct-denied returns CF set and zeroed AX/DX. | Direct: source-derived opaque session token only after selected stream capture; Readonly: explicit refusal, never a host handle. | Migrate current partial shim into the shared stream provider. It must keep guest-visible token/SFT contract separate from host HANDLE storage and retain the source slot/order/failure distinction. |
| `54:07` `SVC_CMDCHECKBINARY` | `cmddisp.c`, COMMAND execution caller path | Binary classification is adjacent launch preparation, not standard-handle routing. | Current lifecycle returns the declared DOS-only success branch. | Direct/Readonly: retain current non-host classifier disposition. | Keep outside the stream/pipe implementation package; test it as an adjacent no-cross-fallback regression. |
| `54:08` `SVC_CMDEXEC` | `cmdexec.c:525-587`; `tcode.asm:1342-1348` | Reads DS:SI command tail, ES environment, SS:BP packed `STD_HANDLES`; malformed tail returns CF clear/AL `ERROR_BAD_FORMAT`; `/c` COMSPEC failure returns CF clear/AL `ERROR_BAD_ENVIRONMENT`; otherwise enters `cmdExec32`. | Current Direct-only path validates a 12-byte adapter token record then uses `CreateProcessW`; absent admission falls through to copied `ERROR_NOT_SUPPORTED` with CF clear. | Direct: later complete child/stream package; Readonly: copied non-launch failure. | Existing child launch is a partial source-derived rehost; migrate only after stream provider and lifecycle are complete. Do not treat a successful host spawn as T224 closure. |
| `54:0A` `SVC_EXECCOMSPEC32` | `cmdexec.c:491-523`; `tcode.asm:1186-1191` | Reads ES environment and AL drive; absent/oversized COMSPEC gives CF clear/AL `ERROR_BAD_ENVIRONMENT`; otherwise calls `cmdExec32`. | Current execution lifecycle returns copied `ERROR_NOT_SUPPORTED`; it does not share the Direct child route. | Direct/Readonly: explicit unavailable until the same child provider admits COMSPEC launch. | Keep coupled to `54:08` in one child-execution subpackage; no singleton implementation. |
| `50:47` `SVC_DEMPIPEFILEDATAEOF` | `demhndl.c:553-583`; `handle.asm:537-553` | AX:BP is a 32-bit redirected-file handle. ZF denotes no data; CF denotes EOF; EOF returns file size in AX:BP. | Current no-pipe-SFT profiles use explicit absent-pipe terminal behavior; no host pipe is synthesized. | Direct/Readonly: unavailable until a session-owned pipe/SFT provider exists. | Defer to the pipe-continuation subpackage, after stream capture and before child-pipe execution. |
| `50:48` `SVC_DEMPIPEFILEEOF` | `demhndl.c:585-605`; `handle.asm:765-776` | AX:BP identifies redirected file; CF denotes EOF and returns final size in AX:BP. | Same explicit absent-pipe route. | Direct/Readonly: unavailable until the same pipe/SFT provider exists. | Couple to `50:47`; they share source state and must regress together. |
| redirection cleanup (not a BOP) | `cmdredir.c:20-96`, `405-663` | Maintains per-COMMAND pipe list, worker threads, temp files, EOF state, and cleanup ordering. | Current child helper closes duplicated handles after synchronous child completion; it has no complete pipe lifecycle. | Direct: later owned lifecycle; Readonly: no workers or temp files. | New provider must preserve observable EOF/disconnect ordering without importing `TerminateThread`, raw `PREDIRCOMPLETE_INFO`, or SoftPC notification. |

## Source-Recovery Ladder

1. **Original direct composition is rejected by named dependencies.** The
   original translation units use CCPU/SAS macros (`getAX`, `setCF`,
   `GetVDMAddr`), a 32-bit `pRdrInfo` host pointer in guest-visible CMDINFO,
   `VDMInfo`, SoftPC redirection notification, worker threads, and legacy VDM
   termination. They cannot link into the MSVC x64 `/MT` CLI composition
   without importing the historical host product.
2. **Smallest admissible seam is a COMMAND-owned stream/pipe provider.** It
   must preserve the service ordering, packed stdout/stderr/stdin relation,
   copied register results, EOF/size results, and documented failure terminals
   while holding host handles privately behind opaque typed tokens.
3. **No Bochs intrusion is admissible.** CPU, device, and selector mechanics
   are unrelated to this host-service package.
4. **Authored behavior is not admitted in S1.** Existing `stream_child` and
   execution-lifecycle code are classified as partial source-derived seams;
   they must be retained, migrated, replaced, or deleted by a later complete
   owner-package S—not extended from a trace hit.

## Implementation Partition Recommended By The Map

1. A shared CLI-admission and COMMAND stream-session provider: explicit
   standard stream snapshot, opaque private handles, Direct/Readonly policy,
   teardown, and `54:06` success/refusal contract.
2. A shared DOS SFT/pipe-continuation provider for `50:47/48`, including EOF,
   size, disconnect, and no-pipe terminals.
3. One child execution/redirection provider for `54:08/0A`, consuming the
   selected stream session and preserving command-tail/environment failure
   behavior; `54:07` remains a separately audited classifier edge.
4. A family regression that proves no ambient HANDLE crosses guest, adapter,
   or profile boundaries, and that Readonly cannot fall through to Direct.

## Confidence And Follow-up

High confidence for ownership and historical ABI: both dispatcher tables,
guest callers, and DOS continuation endpoints agree on the six service
identities plus their bootstrap/cleanup dependencies. The recommended next S2
is the first shared stream-session provider package, but it requires a new
admission brief before code changes. No trace was used to select this work.
