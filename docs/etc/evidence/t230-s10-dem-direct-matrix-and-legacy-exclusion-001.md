# T230 S10 — DEM Direct Matrix And Legacy Exclusion

## Result

The formal `bx-vdm` module now builds the complete directly imported OpenNT
DEM owner set without compiling the superseded source-derived CWD experiment.
The nine focused Direct fixtures source-build and return zero from one fresh
MSVC x64 `/MT` Ninja graph.  This is local Direct source-parity evidence only;
it is not a BOP-ingress or native guest-execution claim.

## OpenNT source baseline

The route catalogue is the directly imported
`src/opennt/base/mvdm/dos/dem/demdisp.c` (repository revision
`d450bfc1b7711234072eebfdfa23735ea509e7c5`).  Its contiguous `50:00..48`
table has 73 entries.  The formal module imports these original owner files
under `src/bx-vdm/bop/opennt/dem/`:

| Owner group | Source-shaped owner files | Services |
| --- | --- | --- |
| handle | `demhndl.c` | 8 |
| file and label | `demfile.c`, `demlabel.c` | 7 |
| directory | `demdir.c` | 4 |
| search and FCB | `demsrch.c`, `demfcb.c` | 13 |
| drive/general state | `demgset.c` | 13 |
| DASD and IOCTL | `demdasd.c`, `demioctl.c` | 3 |
| error and lock | `demerror.c`, `demlock.c` | 3 |
| lifecycle/miscellaneous | `demmisc.c` | 13 |
| dispatcher-owned ordinary returns | `demdisp.c` | 9 |

All imported-file divergences remain adjacent to their original source site
and route only through `src/bx-vdm/bop/shim/`.

## Deliberate legacy exclusion

The former `bx_ntvdm_dem_cwd_service_v2.c` / `.h` is a source-derived,
profile-aware experiment for `50:13`, `50:18`, and `50:1A`.  It is not an
OpenNT translation unit and imports the old CWD/profile composition.  It has
therefore moved to `src/bx-vdm/bop-v1/` as comparison material and is removed
from the formal `bx-vdm` module source list.  It was not repaired or tested.

The Direct mirror owns the same historical routes in their original files:
`demdir.c` owns `50:13` and `50:18`; `demgset.c` owns `50:1A`.  This is a
source-owner replacement, not an attempt to preserve the old implementation.

## Fresh formal witness

Build root:

```text
build/M0-T230-S10/dem-matrix-r3
```

The graph was generated from `tools/build/t225-s7-full-module-manifest.json`.
It compiled `bx-vdm.lib` without the legacy CWD source, then built and ran:

```text
t230-s2-demhndl-direct-import-fixture
t230-s3-demfile-direct-import-fixture
t230-s4-demdir-direct-import-fixture
t230-s5-demsrch-fcb-direct-import-fixture
t230-s6-demgset-direct-import-fixture
t230-s7-demdasd-ioctl-direct-import-fixture
t230-s8-demerror-lock-direct-import-fixture
t230-s9-demmisc-direct-import-fixture
t230-s10-demdisp-direct-import-fixture
```

Every executable returned zero.  The S10 fixture additionally verifies all
73 catalogue identities and the nine original dispatcher-owned ordinary
returns.  A transient locked S8 fixture object inside this disposable build
root was removed and regenerated; no source file or evidence input was
changed to address it.

## Remaining S10 work

This witness does not authorize deletion of all `bop-v1` material: other BOP
families and retained historical probes remain outside the DEM mirror.  The
remaining S10 work is the exact superseded-DEM reference deletion audit and
the separately bounded native observation required by `STATUS.md`.
