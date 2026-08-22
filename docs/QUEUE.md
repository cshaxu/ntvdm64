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
| 1 | External compatibility, machine and legacy-composition BOP closure | Conduct the hard recovery: selector-blind bx machine/BIOS capabilities, historical/private/obsolete Win32/NT4 seams, DPMI, Redirector, WOW16, VDD/debugger and guest EXEC/parent-return. Each workstream uses original source first and the tracker’s explicit compatibility disposition; it may not recreate private NT subsystems. | [External-compatibility proposal](etc/operations/proposal-bop-external-compatibility-machine-closure-001.md): machine/API decision ledger, owner workstreams and full recovery gates. |
| 2 | BOP cross-family completion audit and integrated verification | After the two dependency phases are code/ABI complete, reconcile the entire tracker, provider disposition and source-recovery ledger; run declared cross-family integration profiles and classify residual gaps by original owner. This task does not create trace-led leaf patches. | [Cross-family audit proposal](etc/operations/proposal-bop-cross-family-completion-audit-001.md): cardinality/owner/route reconciliation, complete-package regression matrix and native integration evidence. |
| 3 | Complete DOS/WOW16 guest-binary source-build closure | Establish a reproducible original-source build route for every staged `dos/` and `wow16/` guest artifact, while retaining original product binaries as packaging inputs. | [DOS/WOW16 source-build proposal](etc/operations/proposal-dos-wow16-complete-source-build-closure-001.md): per-artifact original source/build/tool audit and reproducibility matrix. |

The owner-package proposals for XMS, DPMI, Redirector, WOW16,
debugger/VDD/top-level, bx machine/BIOS, NTDOS/COMMAND guest EXEC and
post-code lifecycle remain supporting workstream plans. They are no longer
independent queue positions: their scope is admitted in the first of the
two BOP implementation candidates for which the tracker proves the stated dependencies.

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
