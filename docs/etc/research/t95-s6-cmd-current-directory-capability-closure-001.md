# T95 S6 CMD current-directory capability closure

## Decision

`C4 C4 54 04` is the first reached BOP that requires the already planned host
capability composition.  It must not be implemented by adding a fixed-string
result to the startup bridge, importing `adapter_runtime`, or making Bochs
inspect CMD semantics.

The next implementation packet therefore owns one cohesive capability:

```text
CLI include/exclude policy
  -> one adapter-session Win32 drive snapshot
  -> one adapter-owned read-only namespace-root set
  -> source-shaped CMD current-directory state/result
  -> generic bounded guest RAM write + checked CPU result
```

Each arrow is adapter-local.  Bochs remains limited to its existing generic
atomic write and CPU-result mechanics; it neither enumerates nor opens host
drives, retains a path, identifies a BOP, or owns a DOS current directory.

## Inputs already available

| Existing component | Proven responsibility | Limitation that prevents direct r27 use |
| --- | --- | --- |
| `src/cli/ntdos64_run.c` | Normalizes `--include-drives` / `--exclude-drives` and passes only child environment values. | The direct r27 observer does not invoke this CLI handoff. |
| `bx_ntvdm_host_drive_snapshot_v1_capture_from_environment` | Exactly one `GetLogicalDrives`/`GetDriveTypeW` capture, with exclusion precedence and immutable copied result. | The narrow bridge deliberately does not include this host capability. |
| `bx_ntvdm_host_namespace_v1_initialize` | Opens only roots already admitted by the snapshot; retains opaque handles and an available mask. | It is not linked into r27's 31-object startup closure. |
| `bx_ntvdm_startup_transaction_bridge_v1` | Startup images, static profile inventory, registrations, source-derived result/multi-write handoff. | It excludes `adapter_runtime`, `host_session` and all host paths by design. |
| generic `bx_ntvdm_multi_write_transaction_v1` | All-or-nothing ordinary-RAM ranges plus checked CPU result. | It has no current-directory service producer. |

## Source-shaped CMD contract

`cmdmisc.c:517-568` requires, for one call:

1. AL supplies a zero-based drive number; DS:SI identifies output storage.
2. An unavailable/non-admitted root produces the original failure form
   `CF=1, AX=0`; no guest directory bytes are written.
3. An admitted root whose adapter session has no later current-directory state
   starts at that drive's DOS root (`X:\\` plus NUL), writes only that bounded
   byte sequence atomically to DS:SI, and returns `CF=0`.
4. The session retains only DOS-visible per-drive current-directory text,
   never a raw host path.  Later directory-changing services need their own
   source contracts before they may alter it.

The historic `=?X:` process environment is not imported: its purpose is to
retain per-drive current-directory state.  The adapter session replaces that
state with a bounded, private value so that it cannot mutate the host process
or leak a host path.  `macro.asm:394-421` supplies the consumer evidence: DOS
uses the output to synchronize CDS/NetCDS and checks CF.

## Required new closure, before any fresh trace

- A small adapter-only capability-session object may install the CLI-filtered
  snapshot exactly once alongside the existing startup session and initialize
  namespace roots from it. It must release them at session shutdown.
- A CMD current-directory producer needs a fixed-width request (copied event,
  copied CPU state, bounded output physical address), typed result and opaque
  multi-write payload. It cannot call Bochs or access raw guest pointers.
- The current generic Bochs multi-write mechanic may consume that transaction
  only after all ranges are preflighted. No new Bochs intrusion is needed.
- The builder must name this new capability closure explicitly. It must not
  pull `adapter_runtime`, `host_session`, search, file I/O, or an unbounded
  namespace object graph merely because they exist elsewhere in the tree.
- A real trace must be launched through `ntdos64-run` (or an equivalent
  child-environment fixture) so the CLI's policy is actually the session
  input. The retained r27 all-zero BYOB inventory fixture is not evidence for
  this capability.

## Explicitly rejected shortcuts

- Treating `C:` from `SVC_DEMGETBOOTDRIVE` as proof that host `C:\\` is
  admitted.
- Returning `C:\\` for a drive that the CLI excluded or the snapshot could
  not open.
- Using the BYOB artifact directory as a DOS root.
- Calling `GetCurrentDirectory`, reading/writing `=?X:`, or changing the host
  process environment on a guest request.
- Giving a `54:04` selector branch to Bochs.
