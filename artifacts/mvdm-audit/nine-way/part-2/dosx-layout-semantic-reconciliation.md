# Part 2: DOSX end labels, profile definition and generated dependency input

Four owned original paths are individually identity-bound in the part-2 DPMI observer. Original `dpmi/makefile:60-80,133-137` and `dosxi.lnk:15,18` name dxend and dxendpm objects. `486/dxboot.asm:212,411,521,1050,1382,1411,1551,1636,1647` refers to CodeEndPM and CodeEnd for sizing, copying and boundary checks. The labels are segment-layout inputs, despite their names: they are not task/worker termination callbacks. The `dxend.asm` comment says it must be the last module in its segment, whereas the observed response lists other object names later; assembler segment ordering must be checked rather than interpreting simple textual order as a demonstrated linker error.

| Owned path | Evidence-supported role / remaining constraint |
| --- | --- |
| `dpmi/dxend.asm` | Original real-mode segment-end symbol/code reserve, an explicit historical 486 object; not a task cleanup routine. Conditional VCPI reserve remains profile-specific. |
| `dpmi/dxendpm.asm` | Original protected-mode CodeEndPM marker, explicit historical object; guest boundary and link layout must be preserved. |
| `dpmi/dosx.def` | Original DOSX Windows/ROMWIN segment and import/export definition; makefile names it as prerequisite, while the specific selected response-file linker consumption remains unproved. Classify as original definition input, not a proven live interface. |
| `dpmi/program.sed` | Original dependency generation transformation referenced by makefile, not runtime code; do not execute the historic generation/cleanup command or regenerate guest. |

These are sufficiently grounded file-role recommendations only. Task exit, DPMI memory release, exception recovery and source-to-DOSX binary provenance remain independent; do not infer cleanup from `dxend` or guest binary function from a build prerequisite. Historical T404 FD continuation evidence cited in `dosx-transition-semantic-reconciliation.md` is not a layout proof. S11 should validate live transition, segment bounds and owned state on a hash-identified DOSX plus teardown, while S19 associates the exact image and release; no guest reconstruction is admitted. No tests executed or guest/product/shared ledger changed.

Replay `rg -n "CodeEndPM|CodeEnd" src/mvdm/dpmi`, inspect dosxi.lnk/makefile, dosx.def and program.sed; rerun the part-2 DPMI observer with Python bytecode disabled.
