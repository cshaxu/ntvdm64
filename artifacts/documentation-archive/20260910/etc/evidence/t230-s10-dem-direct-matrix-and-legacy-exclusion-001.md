# T230 S10 — DEM Direct Matrix And Legacy Exclusion

## Result

The formal `bx-vdm` module now builds the complete directly imported OpenNT
DEM owner set without compiling the superseded source-derived CWD experiment.
The nine focused Direct fixtures source-build and return zero from a fresh
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

## Dispatcher correction

The earlier S10 catalogue was a source-derived owner ledger, not the original
dispatcher translation unit.  After the direct dispatcher passed its focused
fixture and reference audit, that obsolete catalogue was deleted.  The active
`src/bx-vdm/bop/opennt/dem/demdisp.c` directly retains OpenNT's 73-entry
`apfnSVC[]`, `DemDispatch`, `CurrentISVC`, hard-error reset, and the original
`demNotYetImplemented` / `demSetV86KernelAddr` bodies.  Its only in-file
change is the documented replacement of the historical product include
closure by `demdisp_shim.h`.

The shim does not select a BOP family.  After a higher-level route has already
chosen DEM, its typed bounded-call wrapper invokes the original `DemDispatch`
table.  The S2 fixture now proves this path for original slot `50:1F`, including
`CurrentISVC`, checked resume, and the original CF-clear no-op result.

## Common-foundation correction

The active `dem.c`, `demdata.c`, and `demmsg.c` are also now direct OpenNT
translation-unit mirrors.  Their earlier source-derived forms were deleted
after the direct replacements passed the formal matrix and reference audit.
The only newly admitted seam correction is exact OpenNT DTA state typing:
`pulDTALocation` is `ULONG UNALIGNED *`, a pointer to a 32-bit guest FAR
address, rather than an incorrectly widened host-pointer type.  The fresh S6
fixture compiles and runs with that original state declaration.

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

Current build root:

```text
build/M0-T230-S10/demdisp-direct-r4
```

The graph was generated from `tools/build/t225-s7-full-module-manifest.json`.
It compiled `bx-vdm.lib` with the direct dispatcher and without the legacy CWD
source, then built and ran:

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

## Fresh r5 revalidation

On 2026-08-19, the same nine fixture executables were rebuilt from a new,
disposable `build/M0-T230-S10/direct-matrix-r5` Ninja graph generated from
the current formal manifest.  All nine returned zero again.  This confirms
that the active `bx-vdm` library still compiles the sixteen direct OpenNT DEM
translation units and neutral shims, without making `bop-v1/` an input to the
DEM fixture routes.  As with the original r4 witness, it is local Direct
source-parity evidence only; it does not prove a native selector ingress or
guest execution.

## Bounded Direct DEM ingress

The standalone CLI cannot link historical `nt_bop.c`: that translation unit
combines selector decoding with CCPU/SAS, DLL, VDD, GUI and other unavailable
NTVDM-product composition.  The Direct replacement is deliberately smaller:
`bop/shim/dem_ingress_shim.c` accepts only a copied four-byte
`C4 C4 50 <service>` window, requires the supplied checked call to name that
same service in the original `0..48h` range, and invokes the original
`DemDispatch` table through `demdisp_shim`.  It has no route for any other
selector and no DEM service body.

The fresh formal `dem-ingress-r6` graph source-builds this shim with the full
direct DEM library.  `t230-s10-dem-ingress-direct-fixture` verifies the
original `50:1F` ordinary-return slot and rejects a COMMAND selector, a
truncated BOP, and the `49h` non-callable endpoint.  This is the first
v1-free bridge from a bounded BOP observation to the original DEM dispatcher.
It still is not a native guest-execution result: binding a real machine event
to a checked DEM call context is separate composition work.

## Remaining S10 work

This witness does not authorize deletion of all `bop-v1` material: other BOP
families and retained historical probes remain outside the DEM mirror.  The
remaining S10 work is the exact superseded-DEM reference deletion audit and
the separately bounded native observation required by `STATUS.md`.

## Current shared-closure revalidation

The later `formal-r11` full-module graph source-builds the current DEM and
COMMAND owners together, then has no remaining Ninja action. Its ten DEM
fixtures all return zero after the shared-owner merge. This revalidation
corrected one retained DEM type mismatch: `pusCurrentPDB` is OpenNT's
`PUSHORT`, not a signed pointer. DEM pipe checks now call directly imported
`cmdredir.c` helpers; no DEM test-local pipe provider remains in the product
library.
