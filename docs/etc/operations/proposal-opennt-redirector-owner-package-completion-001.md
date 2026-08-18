# Proposal: OpenNT Redirector Owner-Package Completion

## Purpose

Recover the complete OpenNT Redirector family (`57:00..31`) for a declared
CLI capability profile.  Redirector is a product capability, not a permanent
unavailable fallback.

## Boundary

The original `VDMREDIR`/`VrDispatch` tables and layouts remain the authority.
The CLI may use ordinary user-mode Win32 file, network, named-pipe and IPC
capabilities under normal permissions without modifying the host.  No raw
host handle, callback, thread identity or Win32 structure crosses guest RAM.

## Admission Plan

1. **S1 — audit:** map all `57:00..31` services by lifecycle, pipes,
   mailslots, Net API, async completion, NetBIOS/DLC and scheduler/window
   subfamilies; record source/ABI/failure and Direct/Readonly disposition.
2. **S2 — code complete:** implement the complete selected capability profile
   through original source or source-derived provider seams, with explicit
   unavailable behavior only for capabilities excluded by that profile.
3. **S3 — verification:** run whole-family local Direct/Readonly regressions
   including denial/disconnect/completion cases, then one bounded native run.

## Exit Rule

Local DEM host-drive access is not a Redirector substitute, and an unselected
network capability cannot be reported as a successful redirector operation.
