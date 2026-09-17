# MVDM runtime package-completion audit

## Question

Which original MVDM packages are fully connected, partially connected, or not
connected in the current Win32/x86 product; and how should recovery avoid
trace-led partial implementation?

## Inputs and method

The audit reads the canonical 23-root tracker, its 1,689 selected path rows,
the current formal x86 generator, current architecture rules and package
proposals. “Complete” means complete selected original manifest, finite
external-interface closure, original lifecycle/failure ordering, local package
matrix and integration proof. Compiled, linked, staged or once-loaded alone is
not completion.

## Classification

| Class | Package roots or units | Result |
| --- | --- | --- |
| Complete | None | No runtime package has all required manifest/interface/lifecycle/local-matrix evidence. The previous 19 count is build selection, not package closure. |
| Partial: formal runtime units | CCPU386; SoftPC BIOS, keymouse, system, disks, support, video, C-VID, comms, DOS, base trace, host roots; DEM; COMMAND; XMS; DPMI32; suballoc; OEMUNI; VDMREDIR | The formal graph selects these 19 units, but each has an outstanding package-local binding, coverage or lifecycle closure. `VDMREDIR` is separately built as a DLL. |
| Partial: top-level runtime owners not in the formal product closure | `dbg`, `vdd`, `wow32`; load-only `dpmi` and `wow16` guest activation | `dbg`/`vdd` have bounded bindings but no complete original owner. WOW32 was historically built but never proven loaded by the current worker. DPMI/WOW16 media is present, but required protected-mode/WOW activation is not package-complete. |
| Not connected by current product profile | `bde`, `dirs`, `fax`, `ieuvddex`, `inc`, `makefil0`, `sim16`, `sim32`, `v86`, `vdmdbg`, `vdmexts`, `vdmutils` | These are tool, declaration, build-control, extension or prohibited CPU30-monitor roots. They require an explicit product-profile admission before any runtime recovery; they are not missing libraries. |

## Priority

Priority follows whole-package dependencies: CPU/host foundations, DOS host
services, memory/DPMI, Redirector, then WOW32/WOW16 and debugger. Every future
S closes exactly one named original package unit; no S may stop at a successful
trace or add a narrow substitute for an unreviewed remainder.

## Conclusion

The old WOW/debugger candidate was too narrow: it could have repaired selected
callbacks while leaving the surrounding package manifests partial. It is
replaced by the package-completion candidate. WRITE becomes an integration
acceptance workload only after prerequisite units are complete.
