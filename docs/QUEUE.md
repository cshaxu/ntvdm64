# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

The ordering below is dependency-driven, using the live
[BOP tracker](etc/bop-list.md), not BOP selector or historical component
alphabetical order. A candidate must use its `BOP-…` and
`BOP-DEPENDENCY-…` rows as the implementation inventory. A newly discovered
reachable OpenNT dependency must be added to that tracker before it can enter
an admitted S.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Repository and neutral runtime foundation | Establish the thirteen-root owner/provenance/build manifest, dependency-neutral `session`, one mapping-manager implementation with three typed per-session instances, checked guest-memory leases, broker wire contract and x86/x64 neutral tests. | [Foundation proposal](etc/operations/proposal-rebootstrap-neutral-runtime-foundation-001.md): source manifest, lifecycle, mapping, broker contract and width closure. |
| 2 | Bochs machine foundation | Recover the source-audited `bochs-core` and its sole mechanical caller `adapter-bochs`, then prove minimal machine lifecycle on both host architectures without OpenNT/BOP/DOS semantics. | [Bochs foundation proposal](etc/operations/proposal-rebootstrap-bochs-machine-foundation-001.md): mirror, lifecycle and boundary closure. |
| 3 | Canonical OpenNT source supply | Select the package-scope OpenNT/OpenNT-4.5 union and establish `opennt-platform-abi`, complete load-only DOS/WOW16 mirrors and the canonical non-guest/non-tool `opennt-mvdm-host` supply. | [Source-supply proposal](etc/operations/proposal-rebootstrap-opennt-source-supply-001.md): provenance, mirror and host/guest isolation closure. |
| 4 | Historical interface adapter recovery | Recover same-shaped BOP, SoftPC, Win32 and VDM-monitor interface families plus the minimum cooperative broker implementation for selected original callers. | [Adapter recovery proposal](etc/operations/proposal-rebootstrap-historical-interface-adapter-recovery-001.md): original-caller ledger, deterministic failure and IPC boundaries. |
| 5 | Dual-architecture product composition | Generate formal x86/x64 Ninja graphs, link admitted original packages, compose `ntvdm.exe` and audit all thirteen production roots, guest isolation and `src.old` exclusion. | [Composition proposal](etc/operations/proposal-rebootstrap-dual-architecture-product-composition-001.md): build/link/CLI and whole-tree closure. |
| 6 | OpenNT DPMI protected-mode owner-package recovery | Recover one declared DPMI profile only after the rebootstrap selects its original package and all protected-mode, LDT/IDT, exception/IRET and memory prerequisites have a whole-package source/ABI/failure closure. | [DPMI proposal](etc/operations/proposal-opennt-dpmi-owner-package-completion-001.md): whole-profile source/ABI/failure closure. |
| 7 | OpenNT WOW16 owner-package recovery | Recover one bounded non-GUI WOW16 profile only after the rebootstrap imports its guest package and the needed NE guest and DPMI/COMMAND contracts exist. | [WOW16 proposal](etc/operations/proposal-opennt-wow16-owner-package-completion-001.md): owner-specific host/guest composition plan. |
| 8 | OpenNT VDD, debugger and top-level event/console recovery | Recover debugger/VDD, console-input and top-level notification routes on top of the completed machine event/device basis; `59` is already closed and is not reopened. | [Debugger/VDD proposal](etc/operations/proposal-opennt-debugger-vdd-top-level-owner-package-completion-001.md): product-visible endpoint audit and whole-provider regression. |
| 9 | COMMAND `cmdExec32` full-capability composition closure | Consume completed Redirector, WOW16, console/event, machine and guest EXEC contracts to restore the remaining original `cmdExec32` broker/lifecycle paths. It may not recreate CSR/BaseSrv/CCPU. | [COMMAND composition proposal](etc/operations/proposal-opennt-command-cmdexec32-full-capability-closure-001.md): source-first broker and cross-owner lifecycle ledger. |
| 10 | BOP cross-family completion audit and integrated verification | After the selected owner packages are code/ABI complete, reconcile the entire tracker, provider disposition and source-recovery ledger; run declared cross-family integration profiles and classify residual gaps by original owner. This task does not create trace-led leaf patches. | [Cross-family audit proposal](etc/operations/proposal-bop-cross-family-completion-audit-001.md): cardinality/owner/route reconciliation, complete-package regression matrix and native integration evidence. |
| 11 | Complete DOS/WOW16 guest-binary source-build closure | Establish a reproducible original-source build route for every staged DOS and WOW16 guest artifact, while retaining original product binaries as packaging inputs. | [DOS/WOW16 source-build proposal](etc/operations/proposal-dos-wow16-complete-source-build-closure-001.md): per-artifact original source/build/tool audit and reproducibility matrix. |

The external-compatibility overview remains a supporting dependency map; it is
not itself an admissible implementation task.  Each candidate above admits
one bounded original-owner package or one selector-blind machine prerequisite.
The live tracker determines whether a candidate has sufficient source, ABI and
failure evidence for admission; queue order does not pre-authorize a feature.

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
