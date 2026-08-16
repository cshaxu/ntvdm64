# T222 S2 P2 — DEM drive-view observation route migration

## Question

Can the admitted Direct/Readonly observation and fixed-layout DEM services
route through one `dem_drive_view_provider_v1` without changing their CPU
result, guest-write, or failure contracts?

## Inputs

- OpenNT `src/opennt/base/mvdm/dos/dem/demgset.c` and `demdisp.c`.
- T222 S1 source/ABI ledger and provider design.
- Existing typed helpers for GSET, media ID, computer name, full DPB and DPB
  list.

## Change

The provider now owns the normal package-session dispatch for `50:0E`, `0F`,
`10`, `14`, `15`, `19`, `1C`, `25`, `41`, and `46`.  It returns either an
already-formed typed CPU result or a checked multi-write transaction.  The
package session remains the only owner that executes a transaction through the
mechanical guest-memory boundary.

A zero-write `50:46` transaction still copies its typed CPU result; this
preserves its BP update rather than treating absence of writes as absence of a
completion.

## Observation

Two source-built MSVC x64 `/MT` fixtures passed from disposable roots:

- `dem-package`: all existing family assertions, including media failure,
  clock-set terminal, full DPB, and DPB-list BP/layout checks.
- `dem-whole-provider-session`: normal session composition remained green.

## Interpretation

This is provider-route migration, not whole-package closure. `0D`, `13`,
`18`, `1A`, and `1B` remain with the boot/default-drive, CWD and DTA group;
raw-media/DASD remains excluded. The retained Overlay and Virtual interfaces
were not changed. Computer-name content remains the pre-existing constrained
helper result and is not asserted here as complete host identity recovery.

## Follow-up

Move the CWD/default-drive/DTA group through the same state owner, then add a
Direct/Readonly-specific full 15-identity regression before considering T222
closed.
