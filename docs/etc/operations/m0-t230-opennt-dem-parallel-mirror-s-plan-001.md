# M0 T230 — OpenNT DEM Parallel-Mirror S Plan

## Purpose

T230 recovers the complete OpenNT DEM `50:00..48` service family by directly
importing each original OpenNT DEM translation unit beneath
`src/bx-vdm/bop/opennt/dem/`. The unit of work is an original OpenNT DEM source
file, or an inseparable source-file group—not an individual trace hit or
hand-written individual BOP patch.

The authoritative upstream shape is `src/opennt/base/mvdm/dos/dem/`. For each
mirrored file, retain its original responsibility, function names, data
structures, function order, dispatch ordering and error propagation wherever
they can compose. A change is allowed only at a named host/guest ABI or
historical-host-composition boundary, with an adjacent comment naming the
original source path and the reason for divergence.

## Scope correction

The v2 mirror is **Direct-only OpenNT source parity**. It does not add
Readonly, Overlay or Virtual branches. Legacy v1 and the earlier partial v2
implementations are read-only comparison material only: T230 neither changes
them for compatibility nor requires them to build or run.  Each is retained
only until its directly imported OpenNT-file replacement is accepted, then is
deleted as a deliberate cleanup. Shared pointer-width-safe host-token and
checked guest-memory seams may be reused, but may not replace a DEM algorithm.

## Required v2 layout

```text
src/bx-vdm/bop/
  opennt/dem/    directly imported OpenNT DEM files, locally adapted in place
  shim/          named, neutral ABI/host-composition compatibility seams only
src/bx-vdm/bop-v1/
                 legacy comparison material only; not a T230 runtime input
```

The `bop/` root is a namespace, not a permanent mixed implementation
directory. Before adapting an original file, T230 preserves the current
partial implementation beside the mirror as comparison material, then directly
copies the OpenNT file into `opennt/dem/`.  The imported translation unit is
the implementation being adapted—not a sketch from which to rewrite selected
handlers. Every in-place change has an adjacent comment naming the original
path, the unavailable historical dependency, and the shim that replaces it.
No copied v1 profile, Readonly, Overlay or Virtual policy enters either new
directory. No T230 acceptance depends on v1 callers reaching the new file.

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
| S2 | `demhndl.c` | 00, 02, 08, 16, 1E, 27, 47, 48 | Preserve the partial file only as comparison material; directly copy original `demhndl.c`, then adapt it in place through neutral host-handle, guest-memory and typed-result shims. |
| S3 | `demfile.c`, `demlabel.c` | 01, 03, 05, 12, 17, 22, 44 | Preserve partial file only as comparison material; directly copy original source files, then adapt their required host/path/guest-memory dependencies in place. |
| S4 | `demdir.c` | 04, 06, 13, 18 | Mirror directory creation/deletion and host-backed current-directory semantics. |
| S5 | `demsrch.c`, `demfcb.c` | 07, 09, 0A, 0B, 0C, 20, 2C–31, 3C | Mirror pathname/FCB search and FCB lifecycle, including DTA/FCB guest layouts. |
| S6 | `demgset.c` | 0D–10, 14, 15, 19, 1A–1C, 25, 41, 46 | Mirror drive, DPB, DTA, date/time, disk-space and computer-name services. |
| S7 | `demdasd.c`, `demioctl.c` | 21, 29, 2A | Mirror IOCTL and absolute-disk operations through required host capabilities. |
| S8 | `demerror.c`, `demlock.c` | 32, 33, 3F | Mirror hard-error/retry and lock behavior with source-shaped continuations. |
| S9 | `demmisc.c` | 11, 23, 34–3E, 45 | Mirror DOS-load, reset, symbol, DOS-app/WOW, diagnostic and lifecycle helpers. |
| S10 | whole `demdisp.c` table and S1–S9 mirrors | 00–48 | Reconcile all 73 identities, delete superseded partial v2 and no-longer-needed v1 comparison material, run full Direct local matrix and one bounded native integration observation. |

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
