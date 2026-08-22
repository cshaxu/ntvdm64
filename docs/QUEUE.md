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
| 1 | SoftPC BIOS conventional-memory reset/query recovery | Recover only the first source-proven native boot blocker: reset-owned conventional-memory state and the SoftPC `BIOS[12] -> memory_size` query. `bx-vdm` owns BOP recognition and copied `AX`/resume; `bx-mantle` owns only selector-blind machine-profile data. It admits no keyboard, PIC, video, FDC/DMA, disk, FPU or broad BIOS product shell. | [Conventional-memory proposal](etc/operations/proposal-softpc-bios-conventional-memory-recovery-001.md): exact source/ABI/map gate and local/native verification. |
| 2 | PC keyboard, IVT and INT 15h compatibility recovery | Recover the smallest original SoftPC keyboard/IVT/INT 15 composition needed by the already-imported XMS owner, including `52:09`, without introducing DOS/BOP policy into `bx-core` or `bx-mantle`. Any top-level keyboard selector shares the package only if the source audit proves the same owner/lifecycle. | [XMS proposal](etc/operations/proposal-opennt-xms-owner-package-completion-001.md): remaining `52:09` owner closure; [machine proposal](etc/operations/proposal-bx-machine-bios-selector-owner-package-completion-001.md): selector-blind mechanics. |
| 3 | NTDOS guest EXEC and parent-return recovery | Recover guest-owned DOS `EXEC`, PSP, arena, JFN, environment and ordinary parent return, consuming but not reimplementing the existing COMMAND host-child lifecycle. | [Guest EXEC proposal](etc/operations/proposal-ntdos-command-guest-exec-parent-return-001.md): source map, guest owner boundary and declared child-to-parent regression. |
| 4 | OpenNT Redirector owner-package recovery | Recover the complete `57:xx` family and its declared DEM/COMMAND remote-stream branches using original `VDMREDIR` ownership and explicit public-Win32 compatibility decisions. It may not recreate a second COMMAND child broker. | [Redirector proposal](etc/operations/proposal-opennt-redirector-owner-package-completion-001.md): package map, compatibility ledger and full-family regression. |
| 5 | OpenNT DPMI protected-mode owner-package recovery | Recover one declared DPMI profile as a package, after its protected-mode, LDT/IDT, exception/IRET and memory prerequisites are proven. | [DPMI proposal](etc/operations/proposal-opennt-dpmi-owner-package-completion-001.md): whole-profile source/ABI/failure closure. |
| 6 | OpenNT WOW16 owner-package recovery | Recover one bounded non-GUI WOW16 profile only after the needed NE guest and DPMI/COMMAND contracts exist. | [WOW16 proposal](etc/operations/proposal-opennt-wow16-owner-package-completion-001.md): owner-specific host/guest composition plan. |
| 7 | OpenNT VDD, debugger and top-level event/console recovery | Recover debugger/VDD, console-input and top-level notification routes on top of the completed machine event/device basis; `59` is already closed and is not reopened. | [Debugger/VDD proposal](etc/operations/proposal-opennt-debugger-vdd-top-level-owner-package-completion-001.md): product-visible endpoint audit and whole-provider regression. |
| 8 | Evidence-admitted PC machine device expansion | Admit only individually source-proven remaining PC machine components—such as video, block media, FDC/DMA, idle, unsimulate or real-mode switch—after their caller, original owner, lifecycle and negative test are frozen. This is explicitly not a full BIOS/device enablement task. | [Machine proposal](etc/operations/proposal-bx-machine-bios-selector-owner-package-completion-001.md): selector-blind feature-admission and lifecycle rules. |
| 9 | COMMAND `cmdExec32` full-capability composition closure | Consume completed Redirector, WOW16, console/event, machine and guest EXEC contracts to restore the remaining original `cmdExec32` broker/lifecycle paths. It may not recreate CSR/BaseSrv/CCPU. | [COMMAND composition proposal](etc/operations/proposal-opennt-command-cmdexec32-full-capability-closure-001.md): source-first broker and cross-owner lifecycle ledger. |
| 10 | BOP cross-family completion audit and integrated verification | After the selected owner packages are code/ABI complete, reconcile the entire tracker, provider disposition and source-recovery ledger; run declared cross-family integration profiles and classify residual gaps by original owner. This task does not create trace-led leaf patches. | [Cross-family audit proposal](etc/operations/proposal-bop-cross-family-completion-audit-001.md): cardinality/owner/route reconciliation, complete-package regression matrix and native integration evidence. |
| 11 | Complete DOS/WOW16 guest-binary source-build closure | Establish a reproducible original-source build route for every staged `dos/` and `wow16/` guest artifact, while retaining original product binaries as packaging inputs. | [DOS/WOW16 source-build proposal](etc/operations/proposal-dos-wow16-complete-source-build-closure-001.md): per-artifact original source/build/tool audit and reproducibility matrix. |

The external-compatibility overview remains a supporting dependency map; it is
not itself an admissible implementation task.  Each candidate above admits
one bounded original-owner package or one selector-blind machine prerequisite.
The live tracker determines whether a candidate has sufficient source, ABI and
failure evidence for admission; queue order does not pre-authorize a feature.

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
