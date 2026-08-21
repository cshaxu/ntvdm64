# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | OpenNT XMS owner-package completion | Complete the whole `52:xx` XMS family with its native A20, UMB and INT 15 prerequisites. | [XMS proposal](etc/operations/proposal-opennt-xms-owner-package-completion-001.md): source/machine-owner audit, full package recovery and regression. |
| 2 | OpenNT DPMI owner-package completion | Complete one declared `53:xx` DPMI guest profile without exporting protected-mode internals through the adapter. | [DPMI proposal](etc/operations/proposal-opennt-dpmi-owner-package-completion-001.md): whole-session source map, recovery ladder and protected-mode profile verification. |
| 3 | OpenNT Redirector owner-package completion | Complete the `57:00..31` Redirector family through original OpenNT ownership and the smallest necessary host-composition shim. | [Redirector proposal](etc/operations/proposal-opennt-redirector-owner-package-completion-001.md): full `VrDispatch` map, provider recovery and family regression. |
| 4 | OpenNT WOW16 owner-package completion | Complete one bounded non-GUI WOW16 NE/DPMI guest and host-composition profile. | [WOW16 proposal](etc/operations/proposal-opennt-wow16-owner-package-completion-001.md): artifact inventory, WOW host map, source-first recovery and native profile verification. |
| 5 | OpenNT debugger, VDD and top-level owner-package completion | Complete product-required debugger/VDD/notification/top-level routes and explicitly isolate truly diagnostic-only facilities. | [Debugger/VDD/top-level proposal](etc/operations/proposal-opennt-debugger-vdd-top-level-owner-package-completion-001.md): route audit, source-derived provider recovery and family regression. |
| 6 | bx machine, BIOS and selector owner-package completion | Complete BOP-reachable SoftPC/BIOS machine mechanics in bx-core/bx-mantle, never as adapter or Bochs BOP policy. | [Machine/BIOS proposal](etc/operations/proposal-bx-machine-bios-selector-owner-package-completion-001.md): native-owner audit, smallest closure and machine-family verification. |
| 7 | BOP cross-family completion audit and integrated verification | After all owner packages close, reconcile the complete BOP catalogue, provider disposition and source-recovery ledger; then run declared cross-family integration tests after each owner package has completed its own bounded native observation. This task audits and classifies gaps—it does not create trace-led leaf implementations. | [Cross-family audit proposal](etc/operations/proposal-bop-cross-family-completion-audit-001.md): cardinality/owner/route reconciliation, complete-package regression matrix, native integration evidence and explicit transfer ledger. |
| 8 | Complete DOS/WOW16 guest-binary source-build closure | Establish a reproducible original-source build route for every one of the 115 staged `dos/` and `wow16/` guest artifacts, retaining original product binaries as packaging inputs but eliminating “prebuilt-only” provenance from the source-build ledger. | [DOS/WOW16 source-build closure proposal](etc/operations/proposal-dos-wow16-complete-source-build-closure-001.md): per-artifact original source/build/tool audit, bounded recovery of missing rules and complete reproducibility matrix. |
Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
