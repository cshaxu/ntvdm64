# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

The order begins with a source-function graph: complete selected MVDM-host
definitions and their reachable in-project original MVDM call closure, then
their exact direct external callees, then package-level
closure. A trace, a BOP number, a compiler failure, or a convenient modern API
cannot reprioritize this order.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Project MVDM zero-degree rebaseline | Form the exact source-graph root: every original `mvdm-host` definition plus its reachable call closure while each resolved physical definition remains in selected OpenNT `mvdm` source already mirrored by a project `mvdm-*` component. Regenerate the first/second-degree ledgers. Existing but unreachable MVDM support, tool, firmware and guest code remains outside zero; runtime eligibility is unchanged; name-only matching is forbidden. | MVDM-component provenance manifests, prior raw degree ledgers, physical identity rule and a zero-runtime-edge review. |
| 2 | Public Win32/CRT source-shaped binding and facade recovery | Consume every final public Win32/CRT leaf result from the rebaselined first-degree audit. For each unique original interface identity, retain the original declaration/call shape and prove one modern disposition: direct public binding, same-shaped `adapter-mvdm-host-out/win32` facade, or explicit source-shaped unavailable result. No private product API is silently substituted or reimplemented. | Rebaselined public-leaf resolution ledger, declaration/ABI evidence and a complete call-site-to-interface mapping matrix. |
| 3 | MVDM host first-frontier package consolidation | Group the rebaselined zero-degree and first-degree function/file results into exact original package boundaries. Give each directly reached package, source file, declaration-only boundary and adapter/reversion target one final disposition, but do not inspect a callee of a first-degree function. Any such callee is emitted only as a later second-degree candidate. No provider implementation. | Completed rebaselined degree-audit ledgers plus shared package/file/interface/build/divergence ledgers and the external package-boundary ledger. |
| 4 | Canonical original-package static closure | From the fixed file/interface selections, finish the selected `mvdm-host` union and each admitted non-MVDM original slice as exact static mirror/build manifests. It proves package membership and finite link inputs only; no provider route or adapter body is enabled. | Final first-frontier file, package, interface and build rows. |
| 5 | Shared MVDM support, ABI and adapter-contract closure | Compose only the shared original support/declaration carriers and define every named same-shaped adapter contract required by more than one selected package. This closes the interface *catalogue* and x86/x64 mapping rules before any owner package rewires a body. | Static manifests and interface/reversion rows. |
| 6 | Original monitor, SIM and SoftPC-control substrate | Recover non-executor MVDM monitor/SIM/SoftPC-control packages through `adapter-mvdm-host-out/softpc` and `adapter-bochs`; Bochs remains the sole CPU/device executor. | Prior static/contract work and machine/control package rows. |
| 7 | Base VDM service package recovery | Recover the admitted BaseSrv/client VDM protocol slice through its finite `adapter-opennt-host` boundary, and migrate a superseded command-source implementation only where the selected original producer/consumer subgraph proves equivalence. Never import CSR/CSRSS. | Prior package/contract work; accepted Base VDM slice and its interface rows. |
| 8 | Original DEM and COMMAND package interconnection | Connect complete original DEM/COMMAND packages to closed Base VDM, support, process, console and machine-control boundaries; remove only the recorded reversion targets. | Prior Base VDM/package-closure work. |
| 9 | Original XMS and DPMI package interconnection | Connect XMS, DPMI and DPMI32 after guest-memory mapping, A20, IVT, protected-context and monitor boundaries have complete source-shaped owners. | Shared contract and Base VDM work plus package interface rows. |
| 10 | Original Redirector package interconnection | Connect VDMREDIR through the already catalogued file/network/pipe and monitor/VDD adapter contracts and explicit unavailable branches; no NetAPI or CSRSS product-shell import. | First-frontier, contract and Base VDM package rows. |
| 11 | Original WOW, VDD and debugger product packages | Connect WOW32/FAX, VDD, BDE/DBG/VDMDBG/VDMEXTS after their Base VDM, monitor, UI and machine boundaries close. | Shared contract, Base VDM and package-specific rows. |
| 12 | Whole-boundary integration, reversion sweep and product verification | Verify every accepted original package/interface, remove recorded superseded autonomous routes, retain explicit exclusions, and run declared x86/x64 host/guest integration profiles. | All package/file/interface/build/divergence ledger rows. |

The external package-boundary ledger is a mandatory supporting dependency map,
not an implementation shortcut. A newly discovered OpenNT package may not be
inserted directly as implementation work: it first receives a complete row in
the degree audit ledgers and is resolved by the first-frontier package
consolidation candidate or its named successor.

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
