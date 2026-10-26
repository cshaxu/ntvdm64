# M0 T473 S1 — package composition map

## Observed product graph

The canonical x86 Ninja graphs are the authority for this map. They compile 368 unique current MVDM paths in the worker graph and 78 in the WOW32 graph; these graph counts include fixture edges and do not themselves prove a product-target link.

The worker link edge names these complete selected original archive inputs: `original-ccpu386`, `original-softpc-{host-roots,support,bios,keymouse,system,disks,video,cvidc,comms,dos,base-trace}`, `original-mvdm-{dem,command,xms,dpmi32,host-suballoc,host-oemuni}`, and `original-opennt-base-vdm` / RTL support. `VDMREDIR.dll` retains its distinct original DLL boundary and imports the worker's generated `ntvdm.lib`. The WOW graph links its 78 original WOW32 objects directly into `wow32.dll`.

The selected archive names prove source selection and final link input, not archive extraction, initialization, dispatch, teardown, or behavioral reach. S1 keeps those evidence columns separate.

## Dispositions from the current union

| Package family | Current disposition | Why it is not silently treated as a missing host library | S1 follow-through |
| --- | --- | --- | --- |
| CCPU40; selected SoftPC devices; DEM, COMMAND, XMS, DPMI32, SubAlloc, OEMUNI; VDMREDIR; WOW32 | Selected original owner package | Actual compilation and the product link edges above select these packages. | Reconcile archive extraction, entry/teardown and runtime witnesses by package; do not replace them with an adapter. |
| `dbg`, `vdd` | Partial host boundary | Bindings exist, but the historic audit did not establish complete original owner composition. | Keep as explicit partial rows; recover only after a finite caller/provider and dependency audit. |
| `dos/v86`, `wow16`, `dpmi`, `bin86`, ROM/firmware assembly and build carriers | Immutable guest, load/build, or media role | They are original inputs, but not host CCPU40 link inputs; importing them into the host violates immutable-guest and selected-executor boundaries. | Retain path-level inventory and prove required guest/media activation separately. |
| `v86` CPU30 monitor/scaffold | Prohibited retired executor | It assumes the unavailable kernel-VDM monitor and is not an alternate CCPU40 provider. | Exclude with this hard boundary; no recovery admission without a new architecture decision. |
| `bde`, `dirs`, `fax`, `ieuvddex`, `vdmdbg`, `vdmexts`, `vdmutils`, `sim16`, `sim32`, top-level declaration/build roots | Unselected product-profile capability or tooling | Existence in the source union does not prove current need or a safe dependency closure. | Each becomes an S2 source-admission row only after caller, host boundary, normal/failure/teardown and duplicate-provider evidence. |

## Verified limits

The worker was rebuilt in the recorded x86 `VsDevCmd` environment after this audit cleanup; `wow32.dll` was already current, and a subsequent Ninja dry run reported no work for either target. The ordinary-shell attempt that could not locate `cl.exe` is not source or product evidence. No package was installed or released for these audit-only changes.
