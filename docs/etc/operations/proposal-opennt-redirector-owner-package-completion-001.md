# Proposal: OpenNT Redirector Owner-Package Completion

## Purpose

Recover the complete OpenNT Redirector family (`57:00..31`) for a declared
CLI capability profile.  Redirector is a product capability, not a permanent
unavailable fallback.

## Queue relationship

This is a supporting owner-workstream plan, not an independent queue position.
The dependency-driven queue admits it only inside candidate 2,
**External compatibility, machine and legacy-composition BOP closure**, after
its applicable BOP-… / BOP-DEPENDENCY-… rows have been frozen. It must
follow the tracker’s Win32/x64 compatibility decision rule and may not use its
former family position to bypass lower-dependency work.
## Boundary

The original `VDMREDIR`/`VrDispatch` tables and layouts remain the authority.
The CLI may use ordinary user-mode Win32 file, network, named-pipe and IPC
capabilities under normal permissions without modifying the host.  No raw
host handle, callback, thread identity or Win32 structure crosses guest RAM.

## COMMAND Lifecycle Dependency

T236 owns the prerequisite COMMAND child record for the bounded local,
non-pipe `54:08`/`54:0A`/`54:0B` lifecycle. Redirector must not recreate a
second child broker or change that record's command, environment, completion,
exit-code or cleanup ordering. Its later admission may extend the declared
record only with opaque stream/redirection state needed by original
`50:47`/`50:48`, `54:06`, and `57:xx` protocol paths. Those extensions must
remain session-owned and carry opaque IDs only; host `HANDLE`s, callbacks and
thread identities remain host-local. Until this package closes those remote
or named-pipe branches retain their explicit unavailable outcome; they are
not inferred from the local child path.

## Admission Plan

1. **S1 — audit:** map all `57:00..31` services by lifecycle, pipes,
   mailslots, Net API, async completion, NetBIOS/DLC and scheduler/window
   subfamilies; record source/ABI/failure and compatibility disposition.
2. **S2 — code complete:** implement the complete selected capability profile
   through original source or source-derived provider seams, with explicit
   unavailable behavior only for capabilities excluded by that profile.
3. **S3 — verification:** run whole-family declared-capability local regressions
   including denial/disconnect/completion cases, then one bounded native run.

## Exit Rule

Local DEM host-drive access is not a Redirector substitute, and an unselected
network capability cannot be reported as a successful redirector operation.
