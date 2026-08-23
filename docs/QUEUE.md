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
| 1 | Selector-blind guest async-completion delivery | Recover the mechanical interrupt-delivery contract needed by original `int5c.asm` ANR completion, without giving bx-vdm CPU/PIC/IVT ownership. | [Async delivery proposal](etc/operations/proposal-selector-blind-guest-async-completion-delivery-001.md): the existing IRQ14/PIC path is selected; the bounded selector-blind wrapper needs proof. |
| 2 | OpenNT DPMI protected-mode owner-package recovery | Recover one declared DPMI profile as a package, after its protected-mode, LDT/IDT, exception/IRET and memory prerequisites are proven. | [DPMI proposal](etc/operations/proposal-opennt-dpmi-owner-package-completion-001.md): whole-profile source/ABI/failure closure. |
| 3 | OpenNT WOW16 owner-package recovery | Recover one bounded non-GUI WOW16 profile only after the needed NE guest and DPMI/COMMAND contracts exist. | [WOW16 proposal](etc/operations/proposal-opennt-wow16-owner-package-completion-001.md): owner-specific host/guest composition plan. |
| 4 | OpenNT VDD, debugger and top-level event/console recovery | Recover debugger/VDD, console-input and top-level notification routes on top of the completed machine event/device basis; `59` is already closed and is not reopened. | [Debugger/VDD proposal](etc/operations/proposal-opennt-debugger-vdd-top-level-owner-package-completion-001.md): product-visible endpoint audit and whole-provider regression. |
| 5 | COMMAND `cmdExec32` full-capability composition closure | Consume completed Redirector, WOW16, console/event, machine and guest EXEC contracts to restore the remaining original `cmdExec32` broker/lifecycle paths. It may not recreate CSR/BaseSrv/CCPU. | [COMMAND composition proposal](etc/operations/proposal-opennt-command-cmdexec32-full-capability-closure-001.md): source-first broker and cross-owner lifecycle ledger. |
| 6 | BOP cross-family completion audit and integrated verification | After the selected owner packages are code/ABI complete, reconcile the entire tracker, provider disposition and source-recovery ledger; run declared cross-family integration profiles and classify residual gaps by original owner. This task does not create trace-led leaf patches. | [Cross-family audit proposal](etc/operations/proposal-bop-cross-family-completion-audit-001.md): cardinality/owner/route reconciliation, complete-package regression matrix and native integration evidence. |
| 7 | Complete DOS/WOW16 guest-binary source-build closure | Establish a reproducible original-source build route for every staged `dos/` and `wow16/` guest artifact, while retaining original product binaries as packaging inputs. | [DOS/WOW16 source-build proposal](etc/operations/proposal-dos-wow16-complete-source-build-closure-001.md): per-artifact original source/build/tool audit and reproducibility matrix. |

The external-compatibility overview remains a supporting dependency map; it is
not itself an admissible implementation task.  Each candidate above admits
one bounded original-owner package or one selector-blind machine prerequisite.
The live tracker determines whether a candidate has sufficient source, ABI and
failure evidence for admission; queue order does not pre-authorize a feature.

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
