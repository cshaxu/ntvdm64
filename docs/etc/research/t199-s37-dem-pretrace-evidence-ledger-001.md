# T199 S37 DEM Pre-Trace Evidence Ledger 001

## Scope

This is the package-level evidence ledger for the 73 callable `50:00..48`
DEM identities.  The authority for the identity universe is OpenNT
`src/opennt/base/mvdm/dos/dem/demdisp.c`'s `apfnSVC[]`; `50:49` is its
non-callable sentinel.  It does not derive scope from a native trace.

Each family below is an adapter-owned provider family whose source/ABI/failure
contract is recorded in its linked evidence.  “Terminal” remains an explicit,
source-derived CLI disposition; it is not a hidden host call or a generic raw
pass-through.

| Evidence family | Callable identities | Primary contract/evidence |
| --- | --- | --- |
| N1 readonly namespace | `00-06,08,12-13,16-18,1E,22,27,44,47-48` | Immutable handles/path operations, checked guest copies, `\\DEV\\` branch, and explicit mutation/pipe terminals; [N1 baseline](t199-s37-dem-n1-readonly-namespace-family-regression-001.md). |
| S1 search/FCB | `07,09-0C,20,2C-31` | DTA/SRCHBUF and per-PDB continuation state; absent writable FCB backend is an explicit terminal; [S1 baseline](t199-s37-dem-s1-fcb-search-family-baseline-001.md). |
| G1 immutable drive state | `0D-10,14-15,19,1A-1C,25,41,46` | Immutable drive/volume snapshots, DPB/CDS writes, and AL-only clock-set failure; [GSET regression](t199-s37-dem-gset-family-regression-002.md). |
| M1 lifecycle/MISC | `11,1D,23,34-3E,45` | NTDOS load, no-debug/no-VDD/no-WOW outcomes, PDB search cleanup, and controlled exit; [MISC record](t199-s37-dem-misc-family-routing-correction-001.md). |
| E1 error/lock | `32-33,3F` | Bounded real-mode hard-error locators, no-retry result, and no-projected-handle lock terminal; [hard-error boundary](t199-s37-dem-package-family-regression-001.md). |
| D1 DASD/IOCTL | `21,29-2A` | Immutable IOCTL metadata and explicit no-device raw-I/O terminals; [DASD/IOCTL record](t199-s37-dem-dasd-ioctl-family-regression-001.md). |
| O1 original no-op/FastRead | `1F,24,26,28,2B,40,42-43` | Original no-op returns plus separately bounded FastRead compatibility; [package regression](t199-s37-dem-package-family-regression-001.md). |

## Coverage calculation

The listed numeric identity sets were mechanically expanded and checked
against the closed interval `0..72`: total entries = 73, distinct entries =
73, missing entries = none, duplicate entries = none.  This guards against a
provider-family summary silently omitting an OpenNT dispatcher member.

## Current package evidence

`artifacts/build/t199-s37-dem-package-r87-gset-boundaries/` is a fresh MSVC
x64 `/MT` build, link and execution of the complete all-DEM fixture.  It
proves that every canonical identity reaches a typed result through the one
DEM package session; no identity reaches raw BOP pass-through.  The fixture
also contains family assertions for N1/S1/G1/M1/E1/D1/O1 behavior.

## Boundary

This ledger closes the static pre-trace coverage audit only.  It does not say
that the historical NT host product shell, any unadmitted filesystem/device,
or continuous guest execution has been reproduced.  A native trace, if
admitted by the active packet, is integration evidence only and may not create
a one-service implementation task.
