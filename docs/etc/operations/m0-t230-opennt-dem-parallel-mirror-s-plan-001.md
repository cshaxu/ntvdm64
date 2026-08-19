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
Superseded source-derived experiments, including the former CWD `13/18/1A`
service, belong in `bop-v1/` until final deletion; they are removed from the
formal `bx-vdm` module rather than repaired.  The imported `demdir.c` and
`demgset.c` own those services in the Direct mirror.

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

### S10 progress — Direct matrix and legacy exclusion

The complete imported-owner Direct matrix now source-builds from one fresh
Ninja graph with a 73-entry `demdisp.c` catalogue witness.  The source-derived
CWD `13/18/1A` experiment is excluded from the formal module and retained
only under `bop-v1/`; its original owner replacements are `demdir.c` and
`demgset.c`.  See [S10 matrix evidence](../evidence/t230-s10-dem-direct-matrix-and-legacy-exclusion-001.md).
The remaining S10 work is the exact reference-deletion audit and the bounded
native observation; neither is implied by this local fixture result.

## Completed package records

### S2 — `demhndl.c` direct-import local closure

The original file was copied directly to `bop/opennt/dem/demhndl.c`; the
former partial v2 file is comparison-only material under `bop/reference-v2/`.
The neutral `demhndl_shim` supplies scoped CPU, checked guest-memory and
opaque-handle mechanics, and the formal MSVC x64 `/MT` Ninja fixture runs all
eight functions. See
[S2 direct-import map](../evidence/t230-s2-demhndl-direct-import-map-001.md).
This closes only the source-shaped local Direct package: it does not claim
selector routing, Redirector/COMMAND pipe composition, `demerror.c` INT 24,
or native guest integration.

### S8 — `demerror.c` / `demlock.c` direct-import local closure

The original owner files are mirrored directly under `bop/opennt/dem/`.
Their only new composition is the fixed VHE/SYSDEV retained guest-layout seam,
the pointer-width-neutral Direct handle token, and an explicit failure for an
as-yet-unbound historical retry slot.  The formal fixture reaches all three
owned bodies; see [S8 direct-import map](../evidence/t230-s8-demerror-demlock-direct-import-map-001.md).
This is local Direct source parity, not an INT 24, full dispatcher or native
guest claim.

### S9 — `demmisc.c` direct-import local closure

The complete original `demmisc.c` is mirrored under `bop/opennt/dem/`.  Its
sole in-file change is the adjacent, documented x86/x64-safe replacement for
the original `ULONG` loader-pointer truncation; the loader loop, lifecycle,
debug gates and error ordering remain source-owned.  The formal fixture
exercises all thirteen service bodies, including a 20,000-byte real
`ntdos.sys` load and typed non-returning termination.  See the
[S9 direct-import map](../evidence/t230-s9-demmisc-direct-import-map-001.md).
This is local Direct source parity, not whole DEM or native guest closure.

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
