# M0 T230 — OpenNT DEM Parallel-Mirror S Plan

## Purpose

T230 recovers the complete OpenNT DEM `50:00..48` service family as a
parallel, source-shaped implementation below `src/bx-vdm/bop/opennt/dem/`. The unit
of work is an original OpenNT DEM source file, or an inseparable source-file
group—not an individual trace hit or hand-written individual BOP patch.

The authoritative upstream shape is `src/opennt/base/mvdm/dos/dem/`. For each
mirrored file, retain its original responsibility, function names, data
structures, function order, dispatch ordering and error propagation wherever
they can compose. A change is allowed only at a named host/guest ABI or
historical-host-composition boundary, with an adjacent comment naming the
original source path and the reason for divergence.

## Scope correction

The v2 mirror is **Direct-only OpenNT source parity**. It does not add
Readonly, Overlay or Virtual branches. Legacy implementations stay isolated
under `src/bx-vdm/bop-v1/` while callers migrate; they are not v2 evidence.
Shared pointer-width-safe host-token and checked guest-memory seams may be
reused, but may not replace a DEM algorithm.

## Required v2 layout

```text
src/bx-vdm/bop/
  opennt/dem/    OpenNT-shaped DEM mirror files only
  shim/          named, neutral ABI/host-composition compatibility seams only
src/bx-vdm/bop-v1/
                 retained legacy providers, policy and compatibility code
```

The `bop/` root is a namespace, not a permanent mixed implementation
directory. S1 inventories every pre-existing `bop/` file and uses `git mv`
where its contents already belong to `opennt/dem/` or `shim/`. A file that
mixes original DEM algorithm with v1 policy is split at the ownership boundary:
the source-shaped algorithm enters `opennt/dem/`; the smallest reusable,
selector-blind compatibility mechanism enters `shim/`; the old composite
provider remains in `bop-v1/` until its v2 route is replaced. No copied v1
profile, Readonly, Overlay or Virtual policy enters either new directory.

## Common acceptance rules

Every S below must identify its original file(s), functions and `demdisp.c`
services; add the source-shaped mirror file(s) below
`src/bx-vdm/bop/opennt/dem/` and any required neutral seam below
`src/bx-vdm/bop/shim/`;
preserve the source-recovery ladder; record every CCPU/SAS, guest-memory,
host-token or host-composition divergence locally; source-build the formal
Ninja target; run focused Direct tests for all owned BOPs; and update the
203-entry tracker with a v2 Direct source-parity result.

## Ordered S sequence

| S | Original owner file(s) | BOP range / services | Deliverable |
| --- | --- | --- | --- |
| S1 | `dem.c`, `demdata.c`, `demmsg.c`, `demdisp.c`; existing `bop/` v2 files | shared DEM state; 1D, 1F, 24, 26, 28, 2B, 40, 42, 43 | Establish `bop/opennt/dem/` and `bop/shim/`. Inventory, `git mv`, split or retain every existing `bop/` v2 file by owner; extract only neutral token/guest-memory/typed-result seams; add provenance headers and the exact service-to-owner map. Preserve original no-op handlers as no-ops. |
| S2 | `demhndl.c` | 00, 02, 08, 16, 1E, 27, 47, 48 | Mirror seek, close, times, read/write, commit and pipe EOF; use one width-safe host-handle seam. |
| S3 | `demfile.c`, `demlabel.c` | 01, 03, 05, 12, 17, 22, 44 | Mirror path, open/create/delete/rename, metadata and volume-label support. |
| S4 | `demdir.c` | 04, 06, 13, 18 | Mirror directory creation/deletion and host-backed current-directory semantics. |
| S5 | `demsrch.c`, `demfcb.c` | 07, 09, 0A, 0B, 0C, 20, 2C–31, 3C | Mirror pathname/FCB search and FCB lifecycle, including DTA/FCB guest layouts. |
| S6 | `demgset.c` | 0D–10, 14, 15, 19, 1A–1C, 25, 41, 46 | Mirror drive, DPB, DTA, date/time, disk-space and computer-name services. |
| S7 | `demdasd.c`, `demioctl.c` | 21, 29, 2A | Mirror IOCTL and absolute-disk operations through required host capabilities. |
| S8 | `demerror.c`, `demlock.c` | 32, 33, 3F | Mirror hard-error/retry and lock behavior with source-shaped continuations. |
| S9 | `demmisc.c` | 11, 23, 34–3E, 45 | Mirror DOS-load, reset, symbol, DOS-app/WOW, diagnostic and lifecycle helpers. |
| S10 | whole `demdisp.c` table and S1–S9 mirrors | 00–48 | Reconcile all 73 identities, remove migrated v1 calls from v2 routes, run full Direct local matrix and one bounded native integration observation. |

## Dependency order

`S1` is the sole common foundation. `S2` and `S3` share the token seam but
remain separate deliveries. `S5` depends on pathname/guest-layout facilities
from `S1` and `S3`; `S6` provides drive/DPB context for `S7`. `S8` and `S9`
consume rather than duplicate common error/session seams. `S10` is the sole
whole-family reconciliation and native-observation S.

## Explicit exclusions

T230 does not move DEM semantics into Bochs, become a DOS kernel, make trace
hits select leaf work, or revive Virtual. Mutation-profile policy outside the
Direct v2 mirror remains independent work.
