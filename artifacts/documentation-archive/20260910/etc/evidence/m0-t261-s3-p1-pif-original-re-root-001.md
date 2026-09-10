# M0 T261 S3 P1 — PIF original-source re-root

## Question

Can the three verified OpenNT PIF inputs move to their re-rooted historical
paths without changing their contents or losing the current parser compilation
closure?

## Inputs and procedure

- Original sources:
  `refs/opennt/base/mvdm/softpc.new/host/src/nt_pif.c` and
  `refs/opennt/base/mvdm/softpc.new/host/inc/{nt_pif.h,nt_uis.h}`.
- Performed content-preserving `git mv` operations into
  `src/opennt-host/softpc.new/host/{src,inc}`.
- Changed only the separately authored parser composition include to point at
  the re-rooted original body, and added the re-rooted `inc` directory to the
  formal manifest include roots.
- Compared SHA-256 for every moved original with its reference counterpart.
- Generated the formal Ninja graph in `build/M0-T261-S3/r001` and built the
  parser object target.

## Observations

- `nt_pif.c` hash: `4AE2C1D2C4A66DC63C9B2DDD673FCAE067F2FBDAB953DC8D96AECFCF03C262BC`
  for both source and reference.
- `nt_pif.h` hash: `A8069D676141812CDDC35ADB0B29EA58E58BBA0A7F29DCF287468108188B8D09`
  for both source and reference.
- `nt_uis.h` hash: `FC00F8C6FFFF9C57F5F0B203C102709DD8A456CD2644DF1DFEB5B5155B81C71E`
  for both source and reference.
- The formal graph accepts the new include root and the re-rooted parser object
  compiles successfully.
- The original `nt_pif.h:PIF_DATA` and `dos/command/cmdpif.h:PIF_DATA` are not
  identical. The retained, separately named composition bridge is therefore
  necessary; it remains a T261/S3 migration item, not a direct-source claim.

## Limitation and follow-up

The external execution transport terminated overlapping full-graph Ninja
instances before final link, leaving `r001/.ninja_log` incomplete. This is not
a source or link failure and is not passing formal-closure evidence. S3 will
use a fresh disposable `build/M0-T261-S3/r002` root for its full actual build;
the `r001` tree is not reused.
