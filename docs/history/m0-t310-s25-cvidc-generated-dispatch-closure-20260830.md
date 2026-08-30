# M0 T310 S25 — C-VID generated dispatch closure

## Closed scope

S25 closes `SPC-CVIDC-GENERATED-DISPATCH` for selected Win32/x86 CCPU40
SoftPC.  The original generated `VideoVector` and EVID tables are restored as
the production table owners; no per-slot typed-wrapper implementation remains.

## Evidence and result

- The [source-boundary record](../etc/evidence/m0-t310-s25-p1-cvidc-generated-dispatch-source-boundary-001.md)
  identifies all 156 `VideoVector` entries and all 692 entries across the 14
  EVID tables, their original definition/caller headers, and the two minimal
  live declaration corrections.
- The [formal-assembly record](../etc/evidence/m0-t310-s25-p2-cvidc-generated-dispatch-formal-assembly-001.md)
  records the direct original C-VID archive and complete 369-edge forced-link
  audit, with no selected C-VID table unresolved symbol.
- The x64-only generated wrapper tool and its obsolete overlay registration
  were removed rather than retained as a second production implementation.

## Explicit non-claims and successor

S25 does not claim controller behavior, physical video-memory behavior,
host display or guest-visible video execution.  S26 owns original video
controller dispatch, S27 owns host video bindings, and S49 owns integrated
machine behavior.
