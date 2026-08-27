# Queue

This is the authoritative ordered queue of **unnumbered candidate T task
packages**. A candidate gains its next global numeric `T` identifier only
when the owner selects it and admits it to [STATUS.md](STATUS.md). The active
package and its single active S are recorded only in Status; S/P planning does
not belong here.

The ordering below is dependency-driven, using the live BOP tracker together
with the package/symbol interface-closure tracker. A candidate must use both
its BOP rows and its source-package dependency rows as implementation
inventory. A newly discovered reachable OpenNT dependency must be added to the
appropriate tracker before it can enter an admitted S.

| Order | Candidate T package | Boundary | Admission evidence |
| --- | --- | --- | --- |
| 1 | Original COMMAND local lifecycle | Recover the local original COMMAND package; defer only tracker-proven brokered cross-process paths. | [Wave 3](etc/operations/mvdm-package-recovery-wave-plan-001.md#wave-3-command-local-lifecycle). |
| 2 | MVDM platform-ABI canonical union | Replace edition-named parallel ABI source roots with one selected OpenNT public/private declaration tree under `mvdm-platform-abi`; record every merged, one-sided and conflicting header selection with source/hash/rejection evidence, then update all include/build manifests. | [Platform-ABI canonical-union proposal](etc/operations/proposal-mvdm-platform-abi-canonical-union-001.md). |
| 3 | Bochs core diff and overlay minimization | Re-audit every non-upstream Bochs-core line and every Bochs-core-overlay body; transfer configuration and lifecycle composition to `adapter-bochs` or another already-owned adapter where it does not require Bochs private state, then delete redundant observers and retain only the smallest private hooks. | [Bochs diff/overlay minimization proposal](etc/operations/proposal-bochs-core-diff-overlay-minimization-001.md). |
| 4 | Original XMS owner package | Recover `xms.486` through the source-shaped A20/RAM/IVT/UMB mechanical seam. | [Wave 4](etc/operations/mvdm-package-recovery-wave-plan-001.md#wave-4-xms). |
| 5 | Original VDM monitor substrate | Recover `sim16`, `sim32`, `v86` and `ieuvddex` through the complete same-shaped monitor adapter. | [Wave 5](etc/operations/mvdm-package-recovery-wave-plan-001.md#wave-5-vdm-monitor-substrate). |
| 6 | Original DPMI owner packages | Recover `dpmi` and `dpmi32` after their complete protected-machine dependency profile closes. | [Wave 6](etc/operations/mvdm-package-recovery-wave-plan-001.md#wave-6-dpmi). |
| 7 | Original Redirector and brokered COMMAND extensions | Recover `vdmredir` and only its source-proven brokered COMMAND dependencies. | [Wave 7](etc/operations/mvdm-package-recovery-wave-plan-001.md#wave-7-redirector-and-brokered-command-extensions). |
| 8 | Original WOW owner packages | Recover `wow32` and `fax` after their monitor, COMMAND and Redirector dependencies close. | [Wave 8](etc/operations/mvdm-package-recovery-wave-plan-001.md#wave-8-wow). |
| 9 | Original VDD and debugger packages | Recover `vdd`, `bde`, `dbg`, `vdmdbg` and `vdmexts` after their machine/monitor dependencies close. | [Wave 9](etc/operations/mvdm-package-recovery-wave-plan-001.md#wave-9-vdd-and-debugger-products). |
| 10 | Dual-architecture product composition and integrated verification | Link all admitted original package closures, audit the tracker and run declared host/guest integration profiles. | [Wave 10](etc/operations/mvdm-package-recovery-wave-plan-001.md#wave-10-product-composition-and-integrated-verification). |

The external-compatibility overview remains a supporting dependency map; it is
not itself an admissible implementation task.  Each candidate above admits
one bounded original-owner package or one selector-blind machine prerequisite.
The live tracker determines whether a candidate has sufficient source, ABI and
failure evidence for admission; queue order does not pre-authorize a feature.

Queue order expresses priority only. It is neither approval nor a numeric T
reservation. Do not add an active packet, an S/P entry, a technical baseline,
or a per-BOP implementation backlog here; use `STATUS.md` for the admitted
T/S packet and `TODO.md` only for unplanned debt.
