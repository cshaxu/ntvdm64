# M0 T281 S3 — original DEM provider source matrix evidence

All twelve remaining `dos/dem` provider translation units were syntax-compiled
with external MSVC Build Tools 2022 on 2026-08-26 for both x86 and x64.
They are archived only; no `DemDispatch`, BOP selector, adapter control body,
or executable is linked.

## Outcome

- `11/12` units compiled immediately on both architectures through the S2
  declaration form and existing adapter declarations.
- `demsrch.c` initially lacked the historical `VDMQUERYDIRINFO` and
  `NtVdmControl(VdmQueryDir, ...)` declarations.
- The exact platform `vdm.h` cannot expose those declarations in this build
  because its original `_NTDEF_` gate does not match the modern declaration
  carrier. The already-existing `adapter-vdm-monitor/include/vdm.h` preserves
  the same enum, structure, function spelling and status signature without
  enabling its implementation.
- With that declaration carrier forced into the static graph, `demsrch.c`
  compiles on both architectures. This is an existing binding selection, not
  a new adapter or a change to OpenNT mirror source.

The observed warnings are original source diagnostics (`sprintf`/string CRT
security notices, unused locals and one shadowed local); they are not source
composition blockers and no source suppression or rewrite was added.

The formal graph produces only `original-dem-providers.lib`, and its surface
test rejects executable/run rules, BOP ingress, Bochs sources, `nt_bop.c`, and
the `adapter-vdm-monitor` implementation body.
