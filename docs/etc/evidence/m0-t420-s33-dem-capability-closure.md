# M0 T420 S33 — DEM file capability closure

## Question

Does the selected original `dos/dem` package perform its normal DOS file
operation and error paths through real guest calls, with bounded guest-memory
binding rather than a replacement file system or retained guest pointers?

## Source disposition

The original DEM manifest has 16 selected C units.  Nine retain registered
finite differences.  They are not an autonomous DOS file implementation:

| Difference family | Units | Disposition |
| --- | --- | --- |
| Immutable product media root | `dem.c` | The original caller still owns all paths; only its initial system directory comes from the worker's fixed package root. |
| Numeric guest locations and synchronous leases | `demdata`, `demfcb`, `demfile`, `demgset`, `demhndl`, `demsrch` | Replace historical durable `GetVDMAddr` aliases only at the exact original call boundary; source operation/order/error handling remain intact. |
| Original kernel fast-I/O unavailable result | `demdisp` | `SVC_DEMFASTREAD` and `SVC_DEMFASTWRITE` return carry, selecting NTDOS's original normal `DEMREAD`/`DEMWRITE` fallback. |
| Fixed-width pointer spelling / worker state binding | `demgset`, `demmisc` | Preserve original guest data semantics on modern x86 and the selected worker-local SCS binding. |

The remaining seven sources are byte-identical.  Earlier S28/S30 real-guest
no-attached-media coverage remains the source-shaped disposition for the
absolute disk and IOCTL families (`demdasd`, `demioctl`); S33 does not invent
a disk controller or media provider merely to turn that profile boundary into
a success.

## Real guest witness

`tests/observation/dem_capability.asm`, built only by
`tools/build/Build-T420S33DemGuestTest.ps1`, is a DOS COM fixture.  It creates
and deletes only `D33*.TMP` and `D33DIR` in the active disposable test
directory.  It runs the following original INT 21h services:

- create, write, commit, close, open, read and seek;
- deny-all open followed by a rejected second open; range lock/unlock;
- find-first/find-next, rename, FCB open/read/close, attribute get/set/get,
  directory create/remove and delete;
- a missing-file open with the exact DOS `AX=2` result; and the host's
  source-shaped sharing rejection with exact DOS `AX=5`.

The first assumption that the latter must be `AX=32` was rejected by a real
run: unchanged `CreateFile` returns `ERROR_ACCESS_DENIED` for this sharing
combination, and unchanged original DEM maps it to DOS 5.  The fixture now
asserts that observed original mapping rather than changing product code.

The final transcript is
`O:\winnt\logs\m0-t420-s33-dem-r13.raw.console.txt`:

```text
S33_FILE_COMMIT_SHARE_LOCK_FIND_RENAME_FCB_ATTR_DIR_ERROR_MAP_OK
```

After the run, all `D33.TMP`, `D33B.TMP`, `D33C.TMP` and `D33DIR` checks were
absent.  The matching observer-created `O:\winnt` broker/worker were then
terminated; user-owned processes outside that package were not touched.

## Build and regression

The active product source set is unchanged since S32's formal x86 graph;
that graph produced the current deployed package and passed normal/abnormal
worker lifecycle plus all 17 transcript-gated COMMAND/MEM/EDIT routes.  S33
adds only the independently built test COM fixture, whose manifest is in
`build/M0-T420/S33/dem-guest-r12/manifest.json`; it has no product build,
link or runtime dependency.

## Result

S33 is closed for its admitted normal DOS file capability scope.  The selected
original DEM owner performs the real guest-to-host operation family, source-
shaped error mapping and normal cleanup.  Raw media/IOCTL retain the existing
explicit no-attached-media profile proof; no replacement provider, overlay or
product implementation was added.
