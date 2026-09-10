# T229 S2 P24 — DEM Whole-Package Local Code Completion

## Scope

This record closes the local code-completion gate for the 73 callable DEM
identities (`50:00..48`) in Direct and Readonly profiles.  It does **not**
claim a native guest continuous-execution result; that is a later integration
stage.

## Source corrections made during closure

- `src/opennt/base/mvdm/dos/dem/demdisp.c` maps `50:01` to `demChMod` and
  `50:31` to `demGetFileInfo`.  The whole-provider fixture had twice treated
  `50:01` as the latter.  Its query-form assertion now correctly requires the
  DOS attribute projection in `CX`.
- `src/opennt/base/mvdm/dos/dem/demsrch.c:659-687` shows that
  `demTerminatePDB` releases per-PDB search state, while the normal DOS abort
  sequence subsequently closes SFT-backed handles.  The fixture now retains a
  Direct opaque token through `50:3C`, verifies it remains usable, and closes
  it through normal `50:02`.

No Bochs, bx-mantle, guest ABI, host policy, or profile behavior changed.

## Formal local regression

All runs used the formal Ninja graph rooted at
`build/M0-T229-S2/formal-disk-reset-20260818r5` on 2026-08-18.

- Owner-group fixtures passed for error/lock, lifecycle routing, DASD/IOCTL,
  original no-op, readonly file/session, concurrent namespace, path search,
  local file backend, drive state, debug, DTA, process owner, loader/FastRead,
  direct search, FCB Direct/Readonly, and disk-reset composition.
- `t229-s2-dem-whole-provider-session-fixture.exe` passed its Direct and
  Readonly whole-provider chain.
- `t225-s19-dem-package-lifecycle-fixture.exe direct` passed.
- `t225-s19-dem-package-lifecycle-fixture.exe readonly` passed.
- The same lifecycle fixture's `direct pdb-lifecycle` and
  `readonly pdb-lifecycle` paths passed before the final all-73 run.

The all-73 fixture verifies each identity produces a valid non-pass-through
package outcome under each selected profile.  Its owner fixtures supply the
service-specific checked-copy, result ABI, source terminal, and mutation-policy
coverage; Redirector-owned pipe services `50:47/48` remain explicitly deferred
rather than receiving a filesystem substitute.

## Result and boundary

T229/S2 has a local code-complete Direct/Readonly DEM package basis: every
callable identity has a source-owned provider, source-derived rehost, original
no-op/unavailable terminal, or explicit deferred owner disposition, with no
unclassified fall-through.  Overlay remains ABI-compatible but intentionally
unimplemented.  Native trace is not used to generate the next leaf task.