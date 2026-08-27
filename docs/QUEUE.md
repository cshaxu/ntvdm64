# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

The order begins with a source-function graph: complete selected MVDM-host
definitions, then their exact direct external callees, then package-level
closure. A trace, a BOP number, a compiler failure, or a convenient modern API
cannot reprioritize this order.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | MVDM host zero-degree function definition and disposition audit | Enumerate every selected `mvdm-host` function definition with a unique definition identity; resolve same-name functions by source path, line, linkage and source form rather than merging them. For each definition, record outgoing calls, leaf status, final recovery disposition and whether its retained form is original, binding-only, adapter-backed, overlay-required or not-host-runtime. Emit only external/non-zero callees to the mutually exclusive first-degree candidate ledger. | Prior one-degree function/file frontier, selected MVDM file ledger and the zero/first-degree audit plan. |
| 2 | MVDM host first-degree function source and disposition audit | Resolve every candidate emitted by candidate 1 to its exact definition, declaration-only hard boundary, public modern API leaf, or finite shallow replacement. Record its source file/hash, final mirror/binding/adapter/overlay/exclusion disposition and its own outgoing calls without silently expanding a second degree. | Candidate 1 zero-degree and first-degree ledgers. |
| 3 | Complete MVDM host and external OpenNT package-boundary audit | Audit all selected `mvdm-host` packages and every directly reached non-MVDM OpenNT candidate. Give each package/file/interface one final disposition, a finite outgoing closure, a stopping-boundary decision and any current implementation eligible for reversion. No provider implementation. | Candidates 1–2 plus shared package/file/interface/build/divergence ledgers and the external package-boundary ledger. |
| 4 | Canonical original-package static closure | From candidate 3's fixed file/interface selections, finish the selected `mvdm-host` union and each admitted non-MVDM original slice as exact static mirror/build manifests. It proves package membership and finite link inputs only; no provider route or adapter body is enabled. | Candidate 3 final file, package, interface and build rows. |
| 5 | Shared MVDM support, ABI and adapter-contract closure | Compose only the shared original support/declaration carriers and define every named same-shaped adapter contract required by more than one selected package. This closes the interface *catalogue* and x86/x64 mapping rules before any owner package rewires a body. | Candidate 4 static manifests and candidate 3 interface/reversion rows. |
| 6 | Base VDM service package recovery | Recover the admitted BaseSrv/client VDM protocol slice through its finite `adapter-opennt-host` boundary, and migrate a superseded command-source implementation only where the selected original producer/consumer subgraph proves equivalence. Never import CSR/CSRSS. | Candidates 3–5; accepted Base VDM slice and its interface rows. |
| 7 | Original monitor, SIM and SoftPC-control substrate | Recover non-executor MVDM monitor/SIM/SoftPC-control packages through `adapter-mvdm-host-out/softpc` and `adapter-bochs`; Bochs remains the sole CPU/device executor. | Candidates 3–5 and machine/control package rows. |
| 8 | Original DEM and COMMAND package interconnection | Connect complete original DEM/COMMAND packages to closed Base VDM, support, process, console and machine-control boundaries; remove only the reversion targets recorded by candidate 3. | Candidates 3–7. |
| 9 | Original XMS and DPMI package interconnection | Connect XMS, DPMI and DPMI32 after guest-memory mapping, A20, IVT, protected-context and monitor boundaries have complete source-shaped owners. | Candidates 5 and 7 plus package interface rows. |
| 10 | Original Redirector package interconnection | Connect VDMREDIR through the already catalogued file/network/pipe adapter contracts and explicit unavailable branches; no NetAPI or CSRSS product-shell import. | Candidates 3–5 and Redirector package rows. |
| 11 | Original WOW, VDD and debugger product packages | Connect WOW32/FAX, VDD, BDE/DBG/VDMDBG/VDMEXTS after their Base VDM, monitor, UI and machine boundaries close. | Candidates 5–7 and package-specific rows. |
| 12 | Whole-boundary integration, reversion sweep and product verification | Verify every accepted original package/interface, remove recorded superseded autonomous routes, retain explicit exclusions, and run declared x86/x64 host/guest integration profiles. | All package/file/interface/build/divergence ledger rows. |

The external package-boundary ledger is a mandatory supporting dependency map,
not an implementation shortcut. A newly discovered OpenNT package may not be
inserted directly as implementation work: it first receives a complete row in
that ledger and is resolved by candidate 1 or its named successor.

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
