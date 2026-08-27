# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

The order is package-topological. Recovery first closes the selected MVDM host
package union and the finite set of directly required non-MVDM OpenNT packages;
only then does it connect source-shaped interfaces within and between those
packages. A trace, a BOP number, a compiler failure, or a convenient modern
API cannot reprioritize this order.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Complete MVDM host and external OpenNT package-boundary audit | Audit all selected `mvdm-host` packages and every directly reached non-MVDM OpenNT candidate. Give each package/file/interface one final disposition, a finite outgoing closure, a stopping-boundary decision and any current implementation eligible for reversion. No provider implementation. | Shared package/file/interface/build/divergence ledgers plus the external package-boundary ledger. |
| 2 | Canonical MVDM host source-union and package static closures | Complete exact selected MVDM host package imports, package manifests and independently buildable original/static closures. Preserve packages even where runtime profiles remain unavailable. | Package/file/build dispositions from candidate 1. |
| 3 | Base VDM service package recovery | Recover the admitted BaseSrv/client VDM protocol slice through finite `adapter-opennt-host` boundaries; migrate any superseded command-source implementation. Do not import CSR/CSRSS. | Accepted Base VDM package and per-interface rows from candidate 1. |
| 4 | MVDM host common support and public-platform bindings | Compose admitted `mvdm-support`, `mvdm-platform-abi` and public Win32/NTDLL-shaped bindings required by more than one MVDM package, without absorbing private product shells. | Cross-package edge ledger from candidate 1. |
| 5 | Original monitor, SIM and SoftPC-control substrate | Recover non-executor MVDM monitor/SIM/SoftPC-control packages through `adapter-mvdm-host-out/softpc` and `adapter-bochs`; Bochs remains sole CPU/device executor. | Machine/control package edges and exclusions from candidate 1. |
| 6 | Original DEM and COMMAND package interconnection | Connect complete original DEM/COMMAND packages to admitted Base VDM, support, process, console and machine-control boundaries; remove recorded superseded project routes. | Candidate 1 reversion ledger and candidates 3–5 closures. |
| 7 | Original XMS and DPMI package interconnection | Connect XMS, DPMI and DPMI32 after guest-memory, A20, IVT, protected-context and monitor boundaries have complete source-shaped owners. | Candidates 4–5 and package interface rows. |
| 8 | Original Redirector package interconnection | Connect VDMREDIR through admitted file/network/pipe adapters and explicit unavailable branches; no NetAPI or CSRSS product-shell import. | Redirector package and external-boundary rows. |
| 9 | Original WOW, VDD and debugger product packages | Connect WOW32/FAX, VDD, BDE/DBG/VDMDBG/VDMEXTS after their Base VDM, monitor, UI and machine boundaries close. | Candidates 3, 5 and package-specific rows. |
| 10 | Whole-boundary integration, reversion sweep and product verification | Verify accepted package interfaces, remove superseded autonomous routes, retain explicit exclusions, and run declared x86/x64 host/guest integration profiles. | All package/file/interface/build/divergence ledger rows. |

The external package-boundary ledger is a mandatory supporting dependency map,
not an implementation shortcut. A newly discovered OpenNT package may not be
inserted directly as implementation work: it first receives a complete row in
that ledger and is resolved by candidate 1 or its named successor.

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
