# M0 T310 S24 — host communications bindings closure

## Closed scope

S24 closes `SPC-HOST-COMMS-BINDINGS` for selected Win32/x86 CCPU40 SoftPC:
the original serial endpoint lifecycle and its fast IOCTL/wait helper binding,
with exact disposition of WOW and printer/MONITOR adjacent paths.

## Evidence and result

- The [source boundary](../etc/evidence/m0-t310-s24-p1-host-comms-source-boundary-001.md)
  proves the original `nt_com` / `nt_ntfun` serial state and public-API
  boundary, private handle ownership, WOW callback ownership and unselected
  LPT conditions.
- The [formal assembly evidence](../etc/evidence/m0-t310-s24-p2-host-comms-formal-assembly-001.md)
  records a fresh 369-edge x86 CCPU40 build, source archives and no S24-owner
  unresolved forced-link symbol.
- The tracked impossible-COM fixture proves only the safe public host-open
  failure result.  It never opens a configured COM device, changes host DCB
  state, starts an original worker or performs I/O.

## Explicit non-claims and successor

S24 does not claim live COM/LPT traffic, WOW callback delivery, MONITOR
direct-printer access or guest communication execution.  WOW-specific handle
callbacks remain S39; printer/MONITOR work remains unselected unless a later
profile admission changes that state.  S49 remains responsible for integrated
machine behavior.  The next sequential packet, S25, owns C-VID generated
dispatch ABI recovery.
