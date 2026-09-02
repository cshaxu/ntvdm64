# Project Status

## Current Work

### M0 T363 S1 — Closed SoftPC conventional-device owner ledger

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T363 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted from T362's exact source-owner transfer under the standing direction to execute the ordered queue. |
| Objective | Establish the complete source-shaped ownership and immediate closure of the reached CPU40 `c_IOVirtualised -> inb -> printer_io` conventional-device path. |
| Non-goals | No printer leaf patch, virtual printer, guest/media mutation, BOP fabrication, DOS EXEC, cmdExec32, BaseSrv/CSRSS, CPU/device semantic change before ledger closure, CPU30, Bochs, x64 runtime, WOW, Redirector or graphics feature. |
| Reference Baseline | T362 S3 [corrected same-run observation](etc/evidence/m0-t362-s3-frozen-command-observation-correction-002.md); selected original SoftPC CPU40 sources, existing LPT/IOS/timer/ICA bindings, fixed `/C EXIT` short-root container and formal original SoftPC graph. |
| Files And ABI Surface | Original SoftPC `ccpusas4`, `ios`, BIOS printer entry, `printer_`, `nt_lpt`, timer/ICA headers and existing adapter/session bindings; no production source change in S1. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture, coding and documentation rules. |
| Verification | Definition/caller/data-flow review, original-versus-mirror inspection, formal graph membership review, owner/failure ledger and governance checks. No product run. |
| Expected Markers | A finite ledger names BIOS/selector/ordinary-I/O entry paths, BDA values, IOS vector owner, LPT backend, timer/ICA relationship and one complete recovery cohort or source terminal. |
| Asset Needs | Existing original mirrors, source indexes, selected CPU40 graph and T362 evidence only; no guest or host-system mutation. |
| Reporting Requirements | Separate source fact, current binding and T362 runtime fact; identify any formerly implemented shim that original source can supersede. |
| Stop Conditions | Need to change a port result, add an ad-hoc printer policy, rerun T362, vary workload/container, enter another owner family, or infer a repair from timeout requires a new packet. |
| Exit Criteria | Every immediate reached device edge has one owner/disposition; the selected S2 cohort is complete and source-shaped, or an exact earlier source terminal is recorded. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | INT 17, selector 17, `printer_io`, `host_lpt_*`, BDA LPT ports/timeouts, IOS/CCPU I/O virtualisation, ICA/PIT timer and app/session device policy. |

**Plan:** [M0 T363 SoftPC conventional-device and DOS workload matrix](etc/operations/m0-t363-softpc-conventional-device-matrix-001.md).

**S1 closure:** [SoftPC conventional-device owner ledger](etc/evidence/m0-t363-s1-conventional-device-owner-ledger-001.md)
proves that the reached INT 17 path remains within the selected original
CPU40/IOS/LPT cohort.  The `MONITOR`-only printer transport is compiled out;
no missing printer binding, virtual printer, or leaf BOP repair is admitted.

### M0 T363 S2 — Closed original conventional-device cohort verification

**S2 closure:** [original conventional-device cohort verification](etc/evidence/m0-t363-s2-conventional-device-cohort-verification-001.md)
proves the source-shaped reset/BDA/IOS/LPT status and timeout cohort with the
selected `NTVDM`/non-`MONITOR` profile. No new device behavior was needed.

### M0 T363 S3 — Closed conventional-device formal closure

**S3 closure:** [conventional-device formal closure](etc/evidence/m0-t363-s3-conventional-device-formal-closure-001.md)
proves the source units and existing bindings are in the selected CPU40/x86
product. It makes no physical-printer claim.

### M0 T363 S4 — Closed frozen conventional-device workload observation

**S4 closure:** [frozen conventional-device workload observation](etc/evidence/m0-t363-s4-frozen-conventional-device-observation-001.md)
records the one permitted run. It passes the reached device cohort, returns a
first original `54:01`, and transfers to the later original COMMAND command
delivery/continuation cohort.

### M0 T364 S1 — Active original COMMAND command-delivery owner ledger

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T364 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted from the T363 source-owned workload transfer under the owner’s standing direction to execute the ordered queue. |
| Objective | Establish the full original `54:01` first/repeat command-delivery, app declaration, BaseClient-shaped transport and built-in-dispatch ownership chain; select one whole S2 cohort or exact source terminal. |
| Non-goals | No BOP leaf repair, guest/media mutation, virtual command source, DOS EXEC, cmdExec32, BaseSrv/CSRSS recreation, CPU/device change, CPU30, Bochs, x64 runtime, WOW, Redirector, printer or graphics work. |
| Reference Baseline | T363 S4 frozen observation; original COMMAND `cmddisp.c`, `cmdmisc.c`, `cmddata.c`, `cmdexec.c`; existing app launch declaration, BaseClient adapter and session command record. |
| Files And ABI Surface | Original COMMAND `CMDINFO`/`VDMINFO` state, BaseClient `GetNextVDMCommand`, app launch declaration and session-owned route; no source or ABI change in S1. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture, coding and documentation rules. |
| Verification | Original declaration/definition/caller/data-flow review; mirror-versus-source comparison; selected formal graph membership; source/adapter boundary ledger and governance checks. No product run. |
| Expected Markers | One ledger distinguishes first, repeat, command consumed/available, `/C` delivery, built-in handoff, block/resume and original error/exit paths, with every immediate edge owned. |
| Asset Needs | Existing original mirrors, formal CPU40/x86 graph, T363 sidecar and source indexes only; no guest or host-system mutation. |
| Reporting Requirements | Separate original source facts, current binding facts and frozen runtime facts; identify any self-authored route that an original body or same-shaped adapter can supersede. |
| Stop Conditions | Need to invent a command parser, change a BOP result, retain a guest pointer, alter app/declaration behavior, rerun T363, enter DOS EXEC/native child, or infer a repair from timeout requires a new packet. |
| Exit Criteria | Every immediate command-delivery edge has an original/source-shaped owner and disposition; S2 is a finite whole cohort or the exact earlier source terminal is recorded. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `cmdGetNextCmd`, `cmdGetNextCmdForSeparateWow`, `GetNextVDMCommand`, `VDMINFO`, `CMDINFO`, `IsFirstCall`, `IsRepeatCall`, app launch declaration, BaseClient capture/copy, `fBlock`, built-in dispatch and `cmdReturnExitCode`. |

**Plan:** [M0 T364 COMMAND command delivery and built-in dispatch](etc/operations/m0-t364-command-delivery-and-built-in-dispatch-001.md).

**S1 closure:** [original COMMAND command-delivery owner ledger](etc/evidence/m0-t364-s1-command-delivery-owner-ledger-001.md)
proves that the later `54:01` is the original ordinary DOS command
reacquisition after the one bootstrap record has been consumed.  Its pending
wait is a source-shaped BaseClient/BaseSrv contract, not a COMMAND BOP or
machine defect.

### M0 T364 S2 — Closed bounded original command-source recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T364 S2; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted from the closed T364 S1 owner ledger under the owner’s standing direction to execute the ordered queue. |
| Objective | Recover the complete source-shaped app/session command-input, copied Base VDM record, DOS wake/retry and original COMMAND handoff cohort selected by the later `54:01`. |
| Non-goals | No command parser, fabricated BOP result, guest/media mutation, virtual command source, DOS EXEC, cmdExec32, BaseSrv/CSRSS recreation, CPU/device change, CPU30, Bochs, x64 runtime, WOW, Redirector, printer, graphics or runtime observation. |
| Reference Baseline | T364 S1 ledger; T363 S4 frozen observation; original COMMAND `cmdGetNextCmd`; existing source-shaped BaseClient/Base VDM local/broker and app launch declaration. |
| Files And ABI Surface | Original `CMDINFO`/`VDMINFO` callers; `app` declaration/input boundary; copied Base VDM command/broker records; session wake/teardown. No guest-pointer, raw-handle or BOP ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture, coding and documentation rules. |
| Verification | Original caller/data-flow review; focused command-record lifecycle tests covering first delivery, consumed/empty, publication/wake/retry and source failure; formal CPU40/x86 link. No product run. |
| Expected Markers | One whole command-input contract has original owner/disposition; no app command parser; copied records and session wake lifecycle are bounded; original COMMAND return/failure behavior is locally evidenced. |
| Asset Needs | Existing original mirrors, formal CPU40/x86 graph, app/session/adapters and source indexes only; no host-system or guest-media mutation. |
| Reporting Requirements | Record original source versus BaseSrv/CSR-unavailable boundary, record lifecycle state before/after consume and teardown, and name the next exact owner without claiming DOS EXEC. |
| Stop Conditions | Need to parse commands in app, synthesize an additional command without an admitted input source, alter a BOP result, retain guest/native pointers, change CPU/device behavior, enter child/WOW/Redirector semantics, or run the product requires new admission. |
| Exit Criteria | The selected ordinary DOS command-source cohort is source-shaped and locally tested, or an exact earlier source terminal is recorded with every immediate edge owned. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `GetNextVDMCommand`, `BaseSrvGetNextVDMCommand`, BaseClient capture/retry, `VDMINFO` state flags, command-record consume/wake, app declaration, session teardown and COMMAND.COM `/C` handoff. |

**S2 closure:** [COMMAND source lifecycle verification](etc/evidence/m0-t364-s2-command-source-lifecycle-verification-001.md)
compares the imported original BaseClient/BaseSrv lifecycle with the selected
same-shaped local slice and records an executed formal x86 fixture.  It proves
the one bootstrap record and later no-next-record wait are not a COMMAND
provider defect.

### M0 T364 S3 — Closed COMMAND delivery formal closure

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T364 S3; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted from the closed T364 S2 source/lifecycle verification under the owner’s standing ordered-queue authority. |
| Objective | Prove the exact original COMMAND delivery package, BaseClient-shaped adapter, app declaration, session and broker inputs are formally linked into the selected CPU40/x86 product. |
| Non-goals | No source/ABI behavior change, parser, BOP repair, guest/media mutation, DOS EXEC, cmdExec32, BaseSrv/CSRSS recreation, CPU/device change, CPU30, Bochs, x64 runtime, WOW, Redirector, graphics, printer or product run. |
| Reference Baseline | T364 S1 owner ledger; T364 S2 x86 fixture result; selected `original-softpc-process.exe` formal graph. |
| Files And ABI Surface | Ninja manifest/product link closure only: original `mvdm-command`, `basesrv-bindings`, `session`, `broker`, app and selected CPU40 machine roots. |
| Applicable Rules | Execution, source policy, CPU40-only, mirror/overlay, build/evidence, architecture, coding and documentation rules. |
| Verification | `ninja -t query` product-input review, selected target build without reconfiguration, source-unit/archive membership review and governance checks. No product run. |
| Expected Markers | Each source-shaped command-delivery owner appears exactly once in the final product path; no retired Bochs/CPU30 or standalone command-provider substitute appears. |
| Asset Needs | Existing formal x86 build root, selected source manifests and T364 evidence; no new asset. |
| Reporting Requirements | Record direct product inputs and any archive-level proof; distinguish link closure from native guest runtime. |
| Stop Conditions | Missing selected owner, source replacement, required behavior change, graph reconfiguration, unrelated package repair or product execution requires new admission. |
| Exit Criteria | Formal graph and target build prove the selected package is linked, with source owner and adapter disposition recorded. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `original-mvdm-command.lib`, `basesrv-bindings.lib`, `session.lib`, `broker.lib`, `app-machine-shell.lib`, CPU30/Bochs exclusions and final process link inputs. |

**S3 closure:** [COMMAND delivery formal closure](etc/evidence/m0-t364-s3-command-delivery-formal-closure-001.md)
proves the original COMMAND archive, BaseClient-shaped binding, session and
broker are final CPU40/x86 process inputs.  This is formal link proof only.

### M0 T364 S4 — Active frozen COMMAND delivery observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T364 S4; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted from the closed T364 S3 formal closure under the owner’s standing ordered-queue authority. |
| Objective | Make one fixed CPU40/x86 product observation of the selected `/C EXIT` COMMAND delivery workload and classify its first source-owned successor. |
| Non-goals | No workload/container variation or retry; no source/ABI change; no BOP repair, command parser, guest/media mutation, DOS EXEC, cmdExec32, BaseSrv/CSRSS recreation, CPU/device change, CPU30, Bochs, x64 runtime, WOW, Redirector, printer or graphics work. |
| Reference Baseline | T363 S4 fixed package observation; T364 S1 source ledger; T364 S2 lifecycle fixture; T364 S3 formal link closure. |
| Files And ABI Surface | Existing product, fixed app declaration, fixed observer and child-only diagnostic report only. No production file change. |
| Applicable Rules | Execution, source policy, source-first recovery, CPU40-only, fixed-observation, architecture, coding and documentation rules. |
| Verification | One eight-second controlled observer invocation with unchanged product/root/`--command EXIT`; retain only concise result evidence. |
| Expected Markers | Original first command delivery, later ordinary reacquisition/wait or one earlier original terminal, with no changed runtime variable. |
| Asset Needs | Existing `O:\\ntvdm64` fixed package root and T362 CPU40/x86 product; no guest or host-system mutation. |
| Reporting Requirements | Record exact command, product/root identity, timeout/outcome and earliest source-owned transfer; distinguish a controlled wait from completion. |
| Stop Conditions | Any need for a retry, different command, changed root/console/observer, leaf patch, source change, guest execution claim or successor-family implementation ends this S. |
| Exit Criteria | One valid frozen observation is recorded and transfers to the correct next owner package or source terminal. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | First/repeat `54:01`, `54:04`, `54:0E`, Base VDM record state, app declaration bytes, original COMMAND dispatch return and earliest next original BOP/host owner. |

**Active: M0 T364 S4.**

### M0 T354 S1 — Closed permanent COMMAND bootstrap owner admission

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T354 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Owner permanently authorizes ordinary queue/proposal governance and directs ordered queue execution. P06D is admitted after T353's closed source-attributed `comerr -> badfil -> 50:3D` terminal. |
| Objective | Reconstruct the entire original permanent `COMMAND.COM` bootstrap cohort from `sysinit1.asm::commnd` through DOS-name resolution, INT 21h open/seek/read/close/handle operations, DEM host boundary, staged image identity and original handoff/error continuation; select one bounded recovery seam before changing production code. |
| Non-goals | No BOP leaf repair or fabricated `50:12/50:3D` result; no guest rewrite/rebuild, media mutation, virtual boot volume, CPU/CCPU/SAS/BIOS semantic change, new mapper, CPU30, Bochs, x64 runtime, BaseSrv/CSRSS, WOW, interactive COMMAND execution, child/parent EXEC, device/INSTALL or graphics claim. |
| Reference Baseline | T353 closure and S4 fixed durable observation; P06D; original NTDOS `softpc.new/bios/{sysinit1.asm,sysinit2.asm}`; selected CPU40/x86 product and its 39-character staged system-root container. |
| Files And ABI Surface | Original NTDOS `sysinit1.asm`, `sysinit2.asm` and reached DOS INT 21h file/handle dispatch; original DEM open/read/seek/close provider and host namespace/root bindings; app staging manifest and session mapping boundary. No ABI change in S1. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original declaration/definition/caller walk from `commnd`; original-versus-mirror and staged-media identity review; selected formal graph/import review; documentation governance/export and diff review. No build or runtime observation in S1. |
| Expected Markers | A source-owned ledger names every immediate permanent-command bootstrap edge, its owner/disposition, exact DOS pathname contract, original handle/error behavior and one bounded S2 recovery target (or an exact source terminal). |
| Asset Needs | Existing original mirrors, source indexes, T353 evidence, CPU40/x86 formal graph and staged media; no new source import, guest rebuild, media mutation or host-system mutation. |
| Reporting Requirements | Separate original-source, current-binding and runtime facts; identify why the staged `COMMAND.COM` is or is not reachable from original `\\COMMAND.COM`; preserve `comerr/badfil/demExitVDM` failure direction. |
| Stop Conditions | Any need to change guest bytes/media, synthesize a BOP success, alter CPU/machine semantics, introduce family-private mapping, infer a repair from a timeout, or broaden into interactive EXEC pauses for a new admission. |
| Exit Criteria | The selected original cohort has no unowned immediate edge; every rejected recovery rung is recorded; one recovery S is bounded without smuggling a DEM leaf, media rewrite or machine change into scope. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `COMSPEC`/`commnd` pathname construction, app system-root staging, DOS `3D/42/3F/3E` flow, JFN/SFT/DEM file bindings, Direct namespace root semantics, `dfil`/EXEC handoff, `badfil` and `SVC_DEMEXITVDM` terminals. |

**Plan:** [CPU40/NTDOS permanent COMMAND bootstrap recovery](etc/operations/proposal-cpu40-ntdos-permanent-command-bootstrap-001.md).

**S1 closure:** [permanent COMMAND bootstrap owner admission](etc/evidence/m0-t354-s1-permanent-command-bootstrap-owner-admission-001.md)
and its [bootstrap ledger](etc/operations/m0-t354-s1-permanent-command-bootstrap-ledger.tsv)
prove that a source-shaped canonical filename observation—not a `50:12` leaf
repair—is the next bounded seam.

### M0 T354 S2 — Closed permanent COMMAND canonical-name attribution

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T354 S2; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T354 S1's complete permanent-command owner ledger under the owner-approved ordered queue execution. |
| Objective | Add one default-off, state-neutral observation at original `SVC_DEMOPEN` sufficient to distinguish the canonical pathname and original scalar outcome for the permanent-command open in the fixed CPU40/x86 container. |
| Non-goals | No BOP result/path rewrite, file-service special case, guest/media mutation, virtual boot volume, new mapper, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or interactive COMMAND claim. |
| Reference Baseline | T353 fixed post-configuration terminal; T354 S1 bootstrap ledger; original NTDOS file route, original DEM `demOpen`, selected package root and staged root/system32 COMMAND media. |
| Files And ABI Surface | Existing DEM `demfile.c`/dispatcher boundary, shared session mapping-manager copied-read lease, existing default-off durable observer only.  No guest pointer, host handle or routing ABI escapes the call. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Source-order and lease review; focused positive/negative copied-string proof; formal CPU40/x86 link; exactly one fixed-container durable observation; governance/export and diff review. |
| Expected Markers | A bounded canonical path plus `CF`/`AX` result associated with the original permanent `SVC_DEMOPEN`, followed by the existing original terminal or a source-owned successor. |
| Asset Needs | Existing mirrors, session mapping manager, formal Ninja graph and unchanged staged media; no new asset or host mutation. |
| Reporting Requirements | Record exact byte bound, lease lifetime, selector ownership, source path, product/media identity and one run; explicitly distinguish default, source-generated and other parser-derived pathname outcomes. |
| Stop Conditions | Any need to retain a guest pointer, mutate state, inspect unrelated DEM calls, vary the container, add a second run, or modify source behavior pauses for a new admission. |
| Exit Criteria | One valid copied observation or an explicit source-backed inability result identifies the live path/outcome and selects exactly one whole recovery cohort for S3 without a leaf BOP implementation. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `SVC_DEMOPEN` callers, WFP path lifetime, `demOpen` normal/error paths, 16/32-bit host-handle return, source-generated `shell=`, staged root/system32 image identity and existing BOP-return reporting. |

**S2 closure:** [permanent COMMAND canonical-open observation](etc/evidence/m0-t354-s2-permanent-command-open-observation-001.md)
proves that the reached permanent path is the original default
`C:\\COMMAND.COM`, not the staged `mvdm\\system32\\COMMAND.COM`; original DEM
returns `ERROR_FILE_NOT_FOUND` unchanged.  The bounded next cohort is the
configuration-to-`commnd` source path, not `50:12`.

### M0 T354 S3 — Closed configuration-to-`commnd` cohort attribution

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T354 S3; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T354 S2's single valid canonical-open observation under the owner-approved ordered queue execution. |
| Objective | Audit the complete original `cmdGetConfigSys`/`cmdconf.c` transformation, NTDOS `doconf` parser, default `commnd` fallback, and app SystemRoot binding to select the minimal source-shaped recovery of the permanent COMMAND pathname. |
| Non-goals | No `demOpen` rewrite, BOP leaf result fabrication, guest/media mutation, real-host C-drive write, virtual boot volume, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or interactive COMMAND claim. |
| Reference Baseline | T353 post-configuration source terminal; T354 S1 owner ledger; T354 S2 proof that `C:\\COMMAND.COM` is the original live failing path despite the unchanged staged `mvdm\\system32\\COMMAND.COM`. |
| Files And ABI Surface | Original `cmdconf.c` configuration text transformation, original NTDOS configuration parser and `commnd` storage, app-selected SystemRoot/profile inputs, plus any already-owned adapter binding selected by source. No new guest-memory mapping or file-provider ABI. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Definition/caller/data-flow audit across original MVDM and app inputs; original-versus-mirror comparison; exact config-text identity inspection; formal graph/import review; governance/export and diff review. Runtime evidence is admitted only after a complete cohort seam is selected. |
| Expected Markers | A source-owned explanation of why `shell=` does or does not reach `doconf`, an exact pathname/result contract, and one bounded recovery seam that preserves the original DOS/DEM error direction. |
| Asset Needs | Existing original mirrors, T354 stage manifest, source index and selected CPU40/x86 graph; no new component or host mutation. |
| Reporting Requirements | Separate source behavior, staged file identity, current app binding and observed path; record every configuration parser branch required to explain `C:\\COMMAND.COM`. |
| Stop Conditions | Any conclusion based only on timeout; need to alter DEM/BOP result, write guest media to real C, invent a parser, change CPU/machine behavior, or broaden into interactive COMMAND work pauses for new admission. |
| Exit Criteria | The entire configuration-to-default pathname cohort is source-owned and selects one minimal original-compatible recovery implementation, or identifies an exact further source terminal. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `config.nt` source/staged identity, `cmdGetConfigSys`, `ExpandConfigFiles`, `doconf`, `SHELL=` grammar, `sysinit1.asm::commnd`, `sysinit2.asm` default, SystemRoot and `system32\\COMMAND.COM` selection. |

**S3 closure:** [configuration-to-`commnd` cohort attribution](etc/evidence/m0-t354-s3-config-to-commnd-cohort-attribution-001.md)
proves that the 40-character package root generates a 114-character original
`shell=` value for the 64-byte firmware `commnd` buffer.  This is an invalid
historical SystemRoot input, not a DEM leaf failure.  A session-owned short
MVDM SystemRoot projection was rejected: the next cohort is strict app-owned
package-path admission that preserves the original host and guest code.

### M0 T354 S4 — Closed original COMMAND package-path admission enforcement

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T354 S4; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T354 S3's source-defined 64-byte `commnd` contract under the owner-approved ordered queue execution. |
| Objective | Enforce the exact original NTDOS `commnd` 64-byte `shell=` input contract at app package admission; show a user-visible explanation and stop before original host startup when the executable-relative MVDM root cannot satisfy it. |
| Non-goals | No DOS-device/drive-letter projection, host path alias, real-host C-drive media copy or mutation, virtual boot volume, `demOpen`/BOP special case, guest/firmware byte change, new mapper, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or interactive COMMAND claim. |
| Reference Baseline | T354 S1 bootstrap ledger; T354 S2 canonical `C:\\COMMAND.COM` failure proof; T354 S3 source proof that the current 40-character package root violates the original 64-byte `commnd` input bound. |
| Files And ABI Surface | App package-layout validation and app-owned user-visible reporting; retained original `cmdconf.c`/NTDOS parser and existing Direct host-drive namespace. No session namespace, direct DEM provider or guest ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Source-call and exact length review, focused package-layout build/fixture, formal graph/import review, governance/export and diff review. No runtime observation is required: a long package root must not enter original host startup. |
| Expected Markers | The unchanged `cmdconf.c` format is measured against `sysconf.asm::commnd`; a root yielding more than 63 visible `shell=` value bytes produces the app-owned explanation and `APP_STARTUP_MEDIA_REJECTED`. |
| Asset Needs | Existing staged immutable media, app code and original MVDM code; no source import, host installation, host namespace or filesystem mutation. |
| Reporting Requirements | State the exact shell-value arithmetic, original source storage bound, selected app failure status and user-visible rejection text; distinguish host admission from a guest/DEM error. |
| Stop Conditions | Any drive alias, `subst`/junction, real C media copy, second mapping manager, changed guest bytes, DEM/BOP special case, or host namespace expansion pauses for new admission. |
| Exit Criteria | The invalid package-root contract is rejected before original host startup with an exact user-visible requirement, while a valid short spelling retains unchanged original `cmdconf.c`/NTDOS behavior. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `achSysRoot`, `commnd`, `pathstring`, `shell=` value parsing, root short-name availability, Direct host C and all-drive semantics, package media identity, session teardown and app launch root selection. |

**S4 closure:** [original COMMAND package-path admission enforcement](etc/evidence/m0-t354-s4-command-package-path-admission-enforcement-001.md)
preserves `cmdconf.c` and NTDOS unchanged. It rejects the current long package
root before original host startup and states the exact 63-visible-character
installation constraint to the user.

**S4 closure:** [original COMMAND package-path admission enforcement](etc/evidence/m0-t354-s4-command-package-path-admission-enforcement-001.md)
preserves `cmdconf.c` and NTDOS unchanged. It rejects the current long package
root before original host startup and states the exact 63-visible-character
installation constraint to the user.

**S4 closure:** [original COMMAND package-path admission enforcement](etc/evidence/m0-t354-s4-command-package-path-admission-enforcement-001.md)
preserves `cmdconf.c` and NTDOS unchanged. It rejects the current long package
root before original host startup and states the exact 63-visible-character
installation constraint to the user.

### M0 T353 S1 — Closed post-`SVC_CMDGETCONFIGSYS` owner admission

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T353 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Owner directs ordered queue execution.  P06C is admitted after T352's closed durable proof that original `54:05` and the reached `54:0C` both return. |
| Objective | Reconstruct the complete original `54:0C -> cmdGetConfigSys -> cmdconf` configuration cohort and the returned NTDOS `ProcessConfig -> doconf` continuation, then select one complete next owner cohort before implementation. |
| Non-goals | No leaf BOP repair, guest/media rewrite, configuration semantic change, CPU/CCPU/SAS/BIOS change, new mapper, CPU30, Bochs, x64 runtime, kernel VDM, CSRSS/BaseSrv, WOW, workload, `EXEC`, or runtime retry. |
| Reference Baseline | T352 closure; P06C; selected original MVDM/firmware mirrors; current CPU40/x86 formal product and fixed 39-character system-root container. |
| Files And ABI Surface | Original `dos/command/{cmddisp.c,cmdconf.c,cmd.h}`, `softpc.new/host/src/nt_pif.c`, original `nt_bop.c`, NTDOS `softpc.new/bios/sysinit1.asm`, reached DEM file-service and existing system-root/guest-address adapter declarations. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original-versus-mirror declaration/definition/caller/continuation walk; current binding/formal-graph review; documentation governance and diff review. No build or runtime observation in S1. |
| Expected Markers | One source call/return ledger that names `54:0C`'s complete host configuration subgraph, its exact guest continuation and every immediate dependency's owner/disposition; one bounded S2 or an explicit source terminal boundary. |
| Asset Needs | Existing mirrors, source indexes, previous immutable report and formal CPU40/x86 inputs; no new source/media import or host-system mutation. |
| Reporting Requirements | Separate historical source fact, current binding fact and runtime fact; record the configuration filename buffer contract, fatal path, `setIP` consumption and NTDOS caller continuation. |
| Stop Conditions | Any need to change original configuration result, guest bytes/media, CPU state, machine semantics, mapping ownership, or infer repair from a timeout pauses for a new S admission. |
| Exit Criteria | A complete source-backed owner ledger has no unowned selected edge, selects a whole recovery cohort (or source terminal), and bounds S2 before any production change. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `54:0C/0D` configuration pair, `ExpandConfigFiles`, PIF/default media selection, short-root and temporary-file constraints, `GetVDMAddr` guest-buffer lifetime, `MS_bop_4` IP advance, NTDOS `doconf`, CONFIG device/load sequence and original fatal paths. |

**Plan:** [CPU40/NTDOS post-`54:0C` startup continuity](etc/operations/proposal-cpu40-ntdos-post-cmdgetconfigsys-continuity-001.md).

**S1 closure:** [post-`SVC_CMDGETCONFIGSYS` owner admission](etc/evidence/m0-t353-s1-post-cmdgetconfigsys-owner-admission-001.md)
and its [owner ledger](etc/operations/m0-t353-s1-post-cmdgetconfigsys-owner-ledger.tsv)
select original NTDOS `ProcessConfig -> doconf`, not a leaf BOP repair.

### M0 T353 S2 — Closed NTDOS `ProcessConfig` continuation boundary audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T353 S2; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T353 S1's source-backed complete post-`54:0C` owner ledger under the owner-approved ordered queue execution. |
| Objective | Audit the complete original `ProcessConfig -> doconf` immediate execution and DOS file/parser dependency cohort, determine its first observable/repairable owner boundary, and select a single bounded source-shaped next step. |
| Non-goals | No `54:0C` provider rewrite, BOP leaf repair, guest/media/configuration change, synthetic CONFIG state, new mapper, CPU/CCPU/SAS/BIOS alteration, CPU30, Bochs, x64 runtime, BaseSrv/CSRSS, WOW, workload, `EXEC`, or runtime run. |
| Reference Baseline | T353 S1 owner ledger; T352 fixed CPU40/x86 durable return observation; selected original MVDM/firmware mirrors and existing configuration/media bindings. |
| Files And ABI Surface | Original `softpc.new/bios/{sysinit1.asm,sysconf.asm}`, referenced DOS parser/INT21 declarations, original DEM file-service declarations, CPU40/SAS execution and the existing session mapping interface; no production ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original declaration/definition/caller walk, original-versus-mirror comparison, selected x86 graph/import review, documentation governance and diff review. No build or runtime observation in S2. |
| Expected Markers | One immediate NTDOS continuation ledger that separates `doconf` parser logic, DOS INT21/DEM file operation contracts, CPU40/SAS prerequisites and later device/load phases; one fully bounded S3. |
| Asset Needs | Existing source mirrors and indexes; no asset import, media change or host-system mutation. |
| Reporting Requirements | Name the exact post-`CMDSVC` register/stack restoration, all source-defined first calls, every retained original failure direction, and the reason each later phase is inside or outside the selected next cohort. |
| Stop Conditions | Any need to infer control flow from a timeout, modify guest/config files, add a file/BOP/machine shim, change a mapping lease or touch runtime execution pauses for renewed admission. |
| Exit Criteria | The first post-`54:0C` original NTDOS cohort and all its immediate source edges have an owner/disposition, no machine or BOP leaf is smuggled into scope, and S3 is bounded before a production change. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `ProcessConfig`, `doconf`, pre-scan and multi-pass CONFIG flow, DOS open/read/close/seek/memory allocation, DEM file adapter bindings, `CMDSVC` return register/stack restoration, CPU40/SAS continuation and original CONFIG terminal errors. |

**S2 closure:** [NTDOS `ProcessConfig` continuation boundary audit](etc/evidence/m0-t353-s2-processconfig-continuation-boundary-audit-001.md)
and its [continuation ledger](etc/operations/m0-t353-s2-processconfig-continuation-ledger.tsv)
select the original `doconf` pre-scan through first parser handoff; it excludes
later device/INSTALL/UMB work and makes no runtime claim.

### M0 T353 S3 — Closed original CONFIG pre-scan durable attribution

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T353 S3; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T353 S2's complete immediate continuation ledger under the owner-approved ordered queue execution. |
| Objective | Add at most one state-neutral, source-ordered durable observation only if needed to distinguish original NTDOS `doconf` pre-scan progress from its first CPU40/DOS file owner boundary, then perform one frozen-container observation. |
| Non-goals | No BOP/provider, configuration/media, guest-byte, file-service, mapping, CPU/CCPU/SAS/BIOS, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, device/INSTALL/EXEC or workload behavior change. |
| Reference Baseline | T352 durable `54:0C` return proof; T353 S1–S2 source ledgers; selected immutable 39-character system-root CPU40/x86 container. |
| Files And ABI Surface | Existing original `sysinit1.asm`/`sysconf.asm`, current CPU40/SAS diagnostics, observer report path and selected original DOS/DEM bindings.  A marker is eligible only at a source-defined entry/return boundary and only with copied scalar state. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Source-order review and focused absent-path test if a recorder is needed; fresh formal CPU40/x86 build/link; exactly one fixed-container durable observation; governance and diff review. |
| Expected Markers | Either existing evidence is sufficient, or fixed-width records identify `doconf` entry/return and the earliest next original owner boundary without guest-memory access or state mutation. |
| Asset Needs | Existing original mirrors, recorder/observer, formal Ninja graph and immutable staged media; no new media or host-system mutation. |
| Reporting Requirements | Record marker ordering/ownership, product and media identity, exactly one run outcome, and a source-based next disposition; distinguish execution continuity from CONFIG semantic completion. |
| Stop Conditions | Any marker needing guest-memory access, pointer retention, register/IP/flag mutation, BOP result change, source-provider rewrite, container variation or a second runtime matrix pauses for renewed admission. |
| Exit Criteria | One unchanged source cohort is formally linked and observed once (or source proves current durable records sufficient); its earliest unproven successor is assigned to one complete owner package without a leaf repair. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | COMMAND BOP return report, NTDOS configuration pre-scan entry/return, DOS open/seek/read/close dispatch, DEM file bridge, CPU40 interrupt return, error/carry paths and observer report lifetime. |

**S3 closure:** [CONFIG pre-scan observation admission result](etc/evidence/m0-t353-s3-config-prescan-observation-admission-001.md)
records the three-action formal link and one invalid observation: its report
directory was absent, so it produces no runtime attribution.  It does not
license a retry inside S3.

### M0 T353 S4 — Closed fixed CONFIG pre-scan observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T353 S4; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted after T353 S3's formally linked but non-evidentiary one-run result; owner approval covers the ordered recovery sequence. |
| Objective | Pre-create a new observer report directory and make exactly one durable, non-debug observation of the already-linked, unchanged CPU40/x86 product in its already-staged 39-character system-root container. |
| Non-goals | No source, BOP/provider, guest/media/configuration, file-service, mapping, CPU/CCPU/SAS/BIOS, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, device/INSTALL/EXEC or workload change; no build relink and no second S4 observation. |
| Reference Baseline | T353 S1–S3; selected already-linked CPU40/x86 product; existing `build/T353S` immutable stage; existing console observer and 39-character system-root invariant. |
| Files And ABI Surface | No production source file.  Existing observer report paths and default-off `MVDM-HOST-DIV-164` records only. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Confirm product/staged media identity; pre-create only disposable report directory; exactly one observer launch; preserve report/sidecars and review marker order; governance/diff review. |
| Expected Markers | A durable `54:0C` return followed by either an original DEM/COMMAND ingress or a source-owned terminal/exception record, all without state mutation. |
| Asset Needs | Existing staged product/media and existing observer; a new disposable report directory only. |
| Reporting Requirements | Record why S3 was invalid, prove S4 container identity is unchanged, quote one durable result, and select one next source owner rather than a leaf repair. |
| Stop Conditions | Missing directory/report, changed product/media/root length/arguments, failed observer launch, or any need for source/semantic modification ends S4 without a retry and requires a new admission. |
| Exit Criteria | One valid fixed observation is preserved and source-attributed, or an explicit non-evidentiary stop is recorded with no retry; the next owner cohort is bounded. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | Observer report-parent creation, child-only environment/report ownership, staged media identity, 54:0C return, post-return ingress, exception/main-return records and timeout classification. |

**S4 closure:** [fixed CONFIG pre-scan observation](etc/evidence/m0-t353-s4-fixed-config-prescan-observation-001.md)
proves the original post-configuration NTDOS pre-scan reaches the exact
permanent-COMMAND `comerr -> stall -> 50:3D` terminal. **T353 closure:**
[CPU40/NTDOS post-`54:0C` startup continuity](history/m0-t353-cpu40-ntdos-post-cmdgetconfigsys-continuity-closure-20260901.md).

### M0 T352 S1 — Closed CPU40/COMMAND post-`SVC_CMDSETINFO` owner admission

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T352 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Owner goal directs ordered queue execution; Queue item 1 / P06B is admitted after T351's closed source/binding/formal/one-observation record. |
| Objective | Trace the complete original `cmdSetInfo` return through `MS_bop_4`, CCPU40/SAS continuation, guest `msinit.asm`, BIOS/stack exchange and every reached immediate machine predecessor, then select one coherent earliest owner cohort. |
| Non-goals | No BOP leaf provider, guest rewrite/rebuild, loader replacement, media change, new mapping manager, synthetic DOS state, alternate executor, CPU30, Bochs, x64 runtime, kernel VDM, CSRSS/BaseSrv or WOW global alias. |
| Reference Baseline | T351 S1–S4 closures; P06B proposal; current original MVDM/SoftPC/NTDOS mirrors; selected x86 CPU40 formal product and fixed-container report. |
| Files And ABI Surface | Original `nt_bop.c`, COMMAND `cmddisp.c`/`cmdmisc.c`, CCPU40/SAS return and execution forms, NTDOS `msinit.asm`, immediate BIOS/FDC/INT15/heartbeat/PIT/ICA callers, and existing session mapping boundary only. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Source declaration/definition/caller/continuation walk; original-versus-mirror comparison; current formal graph/build selection review; documentation governance and diff review. No new runtime retry in S1. |
| Expected Markers | One complete source call/return chain, every direct dependency assigned an original owner and disposition, one earliest complete recovery cohort or explicit source-owned terminal boundary. |
| Asset Needs | Existing selected mirrors, prior fixed report, source indexes and formal CPU40/x86 build inputs; no source import, guest asset or host-system change. |
| Reporting Requirements | Separate static source facts from runtime facts; name exact return/register/stack expectations; identify every sibling dependency inspected and why it is inside/outside the selected cohort. |
| Stop Conditions | Any need to alter BOP result, guest bytes, media, CPU semantics, add a mapper or infer a repair from the timeout pauses for a new S admission. |
| Exit Criteria | A source-backed owner ledger identifies the first recovery cohort and its full immediate dependency/failure boundary, with no unowned selected edge; S2 is bounded before any code change. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | COMMAND service return, BOP IP advance, CCPU40 frame/unsimulate, SAS fetch/store, real-mode segment/stack setup, NTDOS BIOS exchange, FDC/INT15, timer/PIT/ICA and session lease teardown. |

**Plan:** [CPU40/NTDOS post-`54:05` startup continuity](etc/operations/proposal-cpu40-ntdos-post-cmdsetinfo-continuity-001.md).

**S1 closure:** [post-`SVC_CMDSETINFO` owner admission](etc/evidence/m0-t352-s1-post-cmdsetinfo-owner-admission-001.md)
and its [owner ledger](etc/operations/m0-t352-s1-post-cmdsetinfo-owner-ledger.tsv)
select the source-shaped post-dispatch observation seam.  It makes no runtime
continuation claim.

### M0 T352 S2 — original `MS_bop_4` return attribution

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T352 S2; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T352 S1's source-backed owner ledger under the owner-approved ordered queue execution. |
| Objective | Add the smallest source-shaped, state-neutral observation that distinguishes original `CmdDispatch/cmdSetInfo` return from a later CPU40/SAS/guest continuation boundary. |
| Non-goals | No BOP provider change, guest/media change, synthetic state, new mapping manager, CPU semantics change, CPU30, Bochs, x64 runtime, kernel VDM, CSRSS/BaseSrv, WOW global alias, or trace-selected repair. |
| Reference Baseline | T351 closures and fixed observation; T352 S1 owner ledger/evidence; selected CPU40/x86 source and formal graph. |
| Files And ABI Surface | Original `softpc.new/host/src/nt_bop.c`; existing `adapter-mvdm-host-out/softpc` diagnostic recorder and `MVDM-HOST-DIV-164`; no production BOP ABI. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, mapping-manager, CPU40-only, architecture and coding rules. |
| Verification | Compare original/mirror source and register order; focused recorder test if present; fresh selected CPU40/x86 formal build/link; exactly one fixed-container observation after formal identity is frozen; governance/diff review. |
| Expected Markers | Existing ingress record plus one post-dispatch record with already-live fixed-width selector/service/CS:IP/AX/CF values; no altered guest or CPU result. |
| Asset Needs | Existing selected original mirrors, adapter recorder, formal Ninja graph, fixed container and immutable staged media; no new import or host-system mutation. |
| Reporting Requirements | Record source ordering, exact copied values, original/facade/intrusion/new-behavior disposition, build identity, one run result, and every still-unproven successor edge. |
| Stop Conditions | Any need to read guest memory, retain a guest pointer, change `CmdDispatch`, IP, flags, AX, CPU execution, guest bytes, media, container or introduce an alternate executor pauses for new admission. |
| Exit Criteria | The marker is source-ordered and registered, focused/formal checks pass, one fixed observation classifies return versus the next source owner, and no semantic BOP change is present. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | Original DEM `MS_bop_0` return, COMMAND `MS_bop_4` return, CPU40 BOP decode/resume, setIP service consumption, scalar facade failure direction, SAS fetch/store and first `msinit` post-SVC stack operations. |

**S2 closure:** [original `MS_bop_4` return attribution](etc/evidence/m0-t352-s2-post-cmdsetinfo-return-attribution-001.md)
records the fresh 425-action CPU40/x86 link and one fixed observation.  Its
final console surface cannot prove a missing return, so it selects no repair.

### M0 T352 S3 — Closed COMMAND return/observer durability audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T352 S3; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T352 S2's source-ordered return attribution under the owner-approved ordered queue execution. |
| Objective | Audit the complete original `CmdDispatch(5) -> cmdSetInfo` indirect-call cohort and establish whether the final-console observer can prove its return. |
| Non-goals | No BOP leaf change, guest/media change, new mapper, synthetic state, CCPU/SAS/BIOS repair, CPU30, Bochs, x64 runtime, kernel VDM, BaseSrv/CSRSS, WOW global alias, build, or runtime retry. |
| Reference Baseline | T351 scalar-binding closure, T352 S1 ledger and T352 S2 formal/one-observation evidence; selected original COMMAND/SoftPC mirrors. |
| Files And ABI Surface | Original `dos/command/{cmddisp.c,cmdmisc.c,cmd.h}` and reached declaration carriers; existing scalar state facade/session lease; selected CPU40 call/register declarations; no new public ABI. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original-versus-mirror declaration/definition/table/call walk; selected x86 object/map/import inspection; scalar fixture evidence review; documentation governance and diff review. No build or runtime run in S3. |
| Expected Markers | One source ledger with exact calling convention, function-pointer type, table slot and return/failure ownership; one bounded durable S4 observer cohort. |
| Asset Needs | Current formal build map/object files and source mirrors; no new guest asset, provider or host-system modification. |
| Reporting Requirements | Separate static proof from the S2 timeout; name every inspected sibling table/handler and each reason it is selected, retained, or excluded. |
| Stop Conditions | Any proposed semantic command change, guest-memory read/write, durable pointer, new mapper, CPU/guest/media change, or diagnosis inferred solely from timeout pauses for renewed admission. |
| Exit Criteria | The original indirect-call/handler/scalar-facade audit finds no unowned edge and demonstrates that final-console output is non-durable; only then may S4 add the bounded report path and run a new observation. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | COMMAND SVC table slots 00–10, original `PFNSVC` calling forms, CPU40 register accessors, scalar state owner/epoch validity, function-table storage, and DEBUG/non-DEBUG range behavior. |

**S3 closure:** [COMMAND return/observer durability audit](etc/evidence/m0-t352-s3-command-return-durability-audit-001.md)
finds the original COMMAND cohort structurally complete and selects a durable
child-only observer report rather than a trace-derived behavior change.

### M0 T352 S4 — durable original COMMAND return observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T352 S4; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T352 S3's complete source and observer-durability audit under the owner-approved ordered queue execution. |
| Objective | Preserve the existing source-ordered `MS_bop_4` return observation while recording it through a child-only durable report, then make one fixed-container observation that conclusively classifies the reached return boundary. |
| Non-goals | No BOP/provider semantic change, guest/media change, synthetic state, new mapper, CPU/CCPU/SAS/BIOS repair, CPU30, Bochs, x64 runtime, kernel VDM, CSRSS/BaseSrv or second observation matrix. |
| Reference Baseline | T352 S1 owner ledger, S2 formal link/one observation and S3 COMMAND/observer audit. |
| Files And ABI Surface | Existing `MVDM-HOST-DIV-164` observer in `nt_bop.c`, its adapter recorder, and `tests/observation/console_startup_observer.c`; an observer-only environment variable and fixed-width report text. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, mapping-manager, CPU40-only, architecture and coding rules. |
| Verification | Source-order review; focused report-path positive/absent-path negative check; fresh formal CPU40/x86 build/link; one staged fixed-container observation; governance/diff review. |
| Expected Markers | A durable `MVDM-BOP-RETURN 54:05` record or its unambiguous absence, alongside the existing ingress/report identities; no guest-visible state change. |
| Asset Needs | Existing observer, selected original sources, formal graph and immutable media; no new source package or host mutation. |
| Reporting Requirements | Record report-path lifetime/ownership, original record ordering, build/media hashes, one result, and exact successor owner disposition. |
| Stop Conditions | Any guest-memory access, guest handle/object, BOP return/result change, CPU state write, media/container variation, or more than one run pauses for new admission. |
| Exit Criteria | The durable record is source-ordered and absent-path safe, focused/formal checks pass, one observation is preserved and accurately classified, and the next owner package is selected without a leaf repair. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | Main-return/exception report paths, BOP ingress/return diagnostics, observer environment restoration, report handle ownership, console-snapshot limitations and child-only watchdog behavior. |

**S4 closure:** [durable original COMMAND return observation](etc/evidence/m0-t352-s4-durable-command-return-observation-001.md)
records a fresh linked/staged product and one child-only durable report.  Its
absence proves the original COMMAND indirect call has not returned.

### M0 T352 S5 — Closed complete COMMAND indirect-call attribution

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T352 S5; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T352 S4's durable return boundary under the owner-approved ordered queue execution. |
| Objective | Attribute the complete reached original COMMAND indirect-call cohort—`CmdDispatch` table call, `cmdSetInfo`, and scalar registration—to its earliest non-returning source edge without changing COMMAND or BOP semantics. |
| Non-goals | No BOP/provider behavior, guest/media/mapping change, synthetic state, CCPU/SAS/BIOS/NTDOS repair, CPU30, Bochs, x64 runtime, BaseSrv/CSRSS/WOW, or speculative unrelated service marker. |
| Reference Baseline | T351 scalar binding closure and T352 S1–S4 source/formal/durable observation records. |
| Files And ABI Surface | Original `dos/command/{cmddisp.c,cmdmisc.c}`; existing scalar facade and return recorder; fixed observer. Only state-neutral, fixed-width diagnostic report records are eligible. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Complete source ordering/edge review; focused durable marker ordering/absent-path check; fresh CPU40/x86 formal link; exactly one fixed-container observation; governance/diff review. |
| Expected Markers | Durable ordered records around the original table call and selected `cmdSetInfo` scalar registration, sufficient to identify one next source owner edge. |
| Asset Needs | Existing selected sources, adapter recorder, observer, formal graph and immutable media; no new import or host-system mutation. |
| Reporting Requirements | Record each marker's original call order and copied values, prove no marker changes a result, and name the exact next owner cohort rather than a leaf service repair. |
| Stop Conditions | Any marker requiring guest-memory access, raw host alias, result/register/IP change, source/provider rewrite, altered guest/media/container, or a second observation matrix pauses for renewed admission. |
| Exit Criteria | One bounded original COMMAND call cohort is source-ordered, durable and one-run observed; the earliest non-returning edge is assigned to a complete source owner package with no unowned selected sibling. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | All `PFNSVC` COMMAND table slots, function-pointer conventions, `cmdSetInfo` entry/failure/return, scalar facade validation, observer report ownership and BOP post-return ordering. |

**S5 closure:** [complete COMMAND indirect-call attribution](etc/evidence/m0-t352-s5-command-call-cohort-attribution-001.md)
proves original `54:05` and reached `54:0C` returns in the fixed CPU40/x86
container.  **T352 closure:** [CPU40/COMMAND post-`SVC_CMDSETINFO` continuity](history/m0-t352-cpu40-command-post-cmdsetinfo-continuity-closure-20260901.md).

### M0 T351 S2 — original SCS scalar-lifecycle binding recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T351 S2; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | T351 S1 selects the complete original SCS scalar lifecycle under the standing ordered-package owner direction. |
| Objective | Recover and formally close original SCS scalar registration and all reached COMMAND/DEM/selected SoftPC consumers through the existing one-session mapping lease. |
| Non-goals | No `54:05` leaf provider, graphics workload, guest rewrite/rebuild, loader replacement, altered media, new mapper, CPU30, Bochs, x64 runtime, host installation, kernel VDM, CSRSS/BaseSrv or WOW global alias. |
| Reference Baseline | T351 S1 owner ledger/evidence, T350 closure, current selected original COMMAND/DEM/SoftPC mirrors and session mapping manager. |
| Files And ABI Surface | Existing original `cmdmisc.c`, `cmdexec.c`, `cmdpif.c`, `demmisc.c`, `nt_fdisk.c`, `nt_rflop.c`, CPU40/session binding and command guest-state facade; no new public production ABI. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Full original declaration/definition/caller comparison for all selected alias consumers; existing scalar lifecycle fixture plus fresh CPU40/x86 formal product link; documentation governance and diff review. |
| Expected Markers | Each original direct pointer dereference maps to a same-width short lease with a source-shaped success and stale/absent-lease result; no raw alias remains in selected runtime code. |
| Asset Needs | Existing original mirrors, adapter/session fixture and formal Ninja generator; no guest assets or host-system change. |
| Reporting Requirements | Record original-to-binding mapping for every selected consumer; distinguish the selected one-session profile from deferred WOW semantics. |
| Stop Conditions | Any need for a durable native pointer, global session, new mapper, guest mutation, alternate executor or unreviewed source import pauses for re-admission. |
| Exit Criteria | The selected direct-alias cohort has one coherent binding, its source behavior/failure direction is reviewed, focused fixture/formal link pass, and S3 scope is frozen. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | COMMAND scalar registration, later COMMAND/DEM/device consumers, session lease epoch/teardown, CPU40 return/re-entry, SAS access, BIOS exchange and deferred WOW alias consumers. |

**Plan:** [COMMAND/NTDOS bootstrap runtime continuity](etc/operations/proposal-command-ntdos-bootstrap-runtime-continuity-001.md).

**S1 closure:** [owner admission](etc/evidence/m0-t351-s1-command-ntdos-bootstrap-owner-admission-001.md)
and its [owner ledger](etc/operations/m0-t351-s1-command-ntdos-bootstrap-owner-ledger.tsv)
select the whole SCS scalar lifecycle and explicitly transfer WOW/graphics work.

**S2 closure:** [SCS scalar-lifecycle binding closure](etc/evidence/m0-t351-s2-scs-scalar-lifecycle-binding-closure-001.md)
records the complete selected consumer sweep, existing x86 lifecycle fixture,
and fresh 425-action CPU40/x86 formal link.  It does not claim post-`54:05`
guest continuation.

### M0 T351 S3 — original bootstrap local-contract and formal closure

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T351 S3; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T351 S2's complete selected scalar-binding closure under the standing ordered-package owner direction. |
| Objective | Freeze the complete local original `MS_bop_4 -> CmdDispatch -> cmdSetInfo -> CPU40/SAS` contract and its selected failure boundaries before the one runtime observation. |
| Non-goals | No new provider, guest rewrite, runtime trace repair, alternate executor, mapper, CPU30, Bochs, x64 runtime, kernel/CSRSS/BaseSrv or WOW global alias. |
| Reference Baseline | T351 S1 owner ledger, T351 S2 scalar-lifecycle closure, current original COMMAND/DEM/SoftPC mirrors, selected CPU40/x86 formal graph and fixed T350 observation classification. |
| Files And ABI Surface | Existing original COMMAND dispatcher/register bridge, scalar state facade, CPU40/SAS binding and existing source-shaped register fixture. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, architecture, coding and CPU40-only rules. |
| Verification | Rebuild/run the existing source-shaped register-boundary fixture; recheck all selected source/adapter ownership and S2 formal-link identity; documentation governance and diff review. |
| Expected Markers | One original dispatcher/registration path, one bounded scalar-state route, no durable selected host alias, and no alternative CPU or BOP result path. |
| Asset Needs | Existing original mirrors, existing fixture, selected formal graph and current immutable staged media; no new guest artifact or host-system change. |
| Reporting Requirements | State the exact contract proven, every retained failure boundary, and the boundary S4 may observe without treating it as a next leaf implementation request. |
| Exit Criteria | The selected original control/registration contract and all explicit lease/failure boundaries are recorded as one local CPU40 package; no unowned selected alias remains; S4 observation input is frozen. |
| Stop Conditions | Any need to alter a BOP return, synthesize DOS state, create a durable pointer, add a mapper, change media or select a leaf repair stops and transfers to a separately admitted owner package. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | COMMAND register facade, CMDINFO setup, scalar-state consumers, DEM/device FD access, CPU40 outer binding, SAS lease lifetime and deferred WOW synchronization. |

**S3 closure:** [original bootstrap local-contract closure](etc/evidence/m0-t351-s3-original-bootstrap-local-contract-closure-001.md)
records a fresh current-source x86 scalar-lifecycle pass, freezes the one
selected original route, and accurately rejects a stale pre-consolidation
register fixture as evidence.

### M0 T351 S4 — fixed-container COMMAND/NTDOS bootstrap observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T351 S4; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted after the T351 S3 frozen original/bootstrap local contract under the standing ordered-package owner direction. |
| Objective | Make one non-debug, console-owning observation of the unchanged formally linked CPU40/x86 product and accurately classify its first source-defined boundary. |
| Non-goals | No retry matrix, trace-selected repair, BOP/provider implementation, guest/media change, loader replacement, alternate executor, CPU30, Bochs, x64 runtime, kernel/CSRSS/BaseSrv or WOW global alias. |
| Reference Baseline | T351 S1 owner ledger; T351 S2/S3 closures; existing eight-second observer and immutable stage manifest; prior T350 `54:05` observation. |
| Files And ABI Surface | Staged `original-softpc-process.exe`, immutable `mvdm` media, observer only; no production source or ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, CPU40-only, fixed-container observation, architecture and coding rules. |
| Verification | One fresh formal-stage manifest, one observer launch with `-f -o --ordinary-child`, preserved report excerpt/hash, documentation governance and diff review. |
| Expected Markers | Exactly one child process, owned `CONIN$`/`CONOUT$`, eight-second watchdog, unchanged media hash set and one accurately classified exit/timeout/return marker. |
| Asset Needs | Existing formal CPU40/x86 product, Node staging script, existing observer, original immutable DOS/firmware media. |
| Reporting Requirements | Record the exact command/container, product and media identities, output, result and confidence; explicitly state what remains unproven. |
| Stop Conditions | Any need to alter arguments, media, timeout, observer, source, guest state or select a repair from the trace stops this S. |
| Exit Criteria | One compliant observation is recorded and classified without a trace-derived implementation; T351 completion is then assessed against its documented source/binding/formal/observation scope. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | Fixed container identity, console ownership, child-only watchdog, media staging, original BOP ingress observation, outer CPU40 return and post-`54:05` guest continuation. |

**S4 closure:** [fixed-container bootstrap observation](etc/evidence/m0-t351-s4-fixed-container-bootstrap-observation-001.md)
records the sole staged product/media identity, observer result and scope
limit.  **T351 closure:** [COMMAND/NTDOS bootstrap runtime continuity](history/m0-t351-command-ntdos-bootstrap-runtime-continuity-closure-20260901.md).

### M0 T350 — Closed CPU40/NTDOS guest-bootstrap continuity

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T350; closed ordinary-mode packet. |
| Delivered Result | Original COMMAND-to-CPU40/SAS/bootstrap source cohort is source/formal closed; one frozen non-debug console-owning run is classified. |
| Verification | Passed existing CPU40/SAS lifecycle fixture, fresh 425-action CPU40/x86 formal link, and one immutable-media eight-second observation. |
| Runtime Boundary | Observation reaches original `54:05` ingress then times out; it neither proves `cmdSetInfo` return nor NTDOS/COMMAND child execution. |
| Successor | Queue item 4: SoftPC graphics-workload integration continuation. |

**Plan:** [CPU40/NTDOS guest-bootstrap continuity](etc/operations/proposal-cpu40-ntdos-guest-bootstrap-continuity-002.md).
**Closure:** [T350 history](history/m0-t350-cpu40-ntdos-guest-bootstrap-continuity-closure-20260901.md).

### M0 T349 — Closed package-path capacity record

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T349; closed ordinary-mode packet. |
| Delivered Result | One app-owned early rejection for roots outside original COMMAND short-path/caller capacity; original mirror algorithms remain unchanged. |
| Verification | Focused accepted/rejected x86 contract plus fresh 425-action formal CPU40/x86 link. |
| Successor | Queue item 3: CPU40/NTDOS post-`SVC_CMDSETINFO` guest-bootstrap continuity. |

**Plan:** [Executable-relative MVDM package-path capacity disposition](etc/operations/proposal-executable-relative-mvdm-path-capacity-001.md).

**S1 closure:** [original COMMAND capacity audit](etc/evidence/m0-t349-s1-original-command-package-path-capacity-audit-001.md)
and its ledger identify the unchanged 64-byte short-root form and every
reached original caller buffer.  **S2 closure:** [package admission](etc/evidence/m0-t349-s2-command-package-path-admission-001.md)
proves one app-only accepted/rejected preflight with a staged product root.
**S3 closure:** [formal capacity closure](etc/evidence/m0-t349-s3-command-package-path-capacity-closure-001.md)
records the selected formal link without claiming execution continuity.

### M0 T348 — Closed guest artifact matrix record

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T348; closed ordinary-mode packet. |
| Delivered Result | 2,719 guest/firmware paths have a reproducible final disposition; the 16 selected stage destinations have exact hash and refusal evidence. |
| Boundary | Load-only media; no guest source rewrite/rebuild or host execution-continuity claim. |
| Successor | Queue item 2: executable-relative MVDM package-path capacity disposition. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Runtime destination list, original `COMMAND.COM` dual placement, source-built cross-check non-substitution, ROM/profile/CMOS adjacency and stage refusal behavior. |

**Plan:** [Guest DOS/WOW16 artifact matrix](etc/operations/proposal-guest-dos-wow16-artifact-matrix-001.md).

**S1 closure:** [guest media provenance inventory](etc/evidence/m0-t348-s1-guest-media-provenance-inventory-001.md)
records 2,719 current media paths and their unique dispositions.  **S2
closure:** [staging reconciliation](etc/evidence/m0-t348-s2-executable-relative-staging-reconciliation-001.md)
proves the exact 16-destination selection, hash parity, and refusal behavior.
**S3 closure:** [matrix closure](etc/evidence/m0-t348-s3-guest-artifact-matrix-closure-001.md)
records byte-stable regeneration and complete unique-disposition coverage.
Neither changes guest content or claims guest runtime behavior.
T342 is closed: the only startup-reached debugger form
is the original `DBGInit` shape through its bounded adapter; BDE, VDMDBG,
VDMEXTS and FAX remain their original tool/product packages. T341 is closed:
the complete original VDD host-service surface remains selected in
the formal CPU40/x86 link, while the current profile explicitly selects no
external installable VDD. T340 is
closed: initial resident DOS-BIOS headers are
the original function-00 no-op-success cohort, and the selected original
HIMEM/SoftPC XMS-A20-INT15-HMA successor is source/formal-build closed. Its
single unchanged fixed observation did not reach HIMEM. See the
[T342 closure](history/m0-t342-debugger-bde-fax-owner-package-recovery-closure-20260831.md)
and
[T341 closure](history/m0-t341-vdd-owner-package-recovery-closure-20260831.md).

## Active Packet

### M0 T357 S1 — Closed staged runtime identity and owner admission

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T357 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Owner directed execution in the newly ordered component-activation sequence: “按照这个顺序执行.” |
| Objective | Produce and verify one new task-owned executable-relative CPU40/x86 runtime stage containing the formal product and immutable MVDM media, ready for one later frozen observation. |
| Non-goals | No runtime observation in S1; no source/guest/media mutation, BOP repair, CPU/device change, CPU30, Bochs, x64, CSRSS/kernel VDM, WOW, native child or graphics work. |
| Reference Baseline | T356 S3's manifest-identified formal CPU40/x86 product; T356 S4's app-owned rejection; current component activation registry and runtime recovery program. |
| Files And ABI Surface | `tools/build/Stage-OriginalSoftpcRuntime.mjs`, staged manifest/output and existing app executable-relative package contract. No production source or ABI change. |
| Applicable Rules | Execution, source policy, component activation registry, source-first recovery, CPU40-only, build-output hygiene, architecture and coding rules. |
| Verification | Product/hash/source-manifest review; new stage manifest media-hash review; tool output review; documentation governance and diff review. |
| Expected Markers | A new stage holds `original-softpc-process.exe`, sibling `mvdm` media and a format-3 manifest whose immutable media hashes match selected inputs. |
| Asset Needs | Existing T356 formal product, Node 22, selected guest/firmware media and task-owned `build/M0-T357/S1/` output. |
| Reporting Requirements | Record product and media identities separately; state whether reuse is identity-proven; do not call staging or app admission proof a guest-execution result. |
| Stop Conditions | Formal product identity cannot be proven; required immutable asset is missing; the stage tool needs source changes; or any observation/source repair is proposed before S2. |
| Exit Criteria | New stage and manifest are complete, immutable inputs are hash-verified, all app-visible paths are adjacent to the executable, and S2's single fixed observation command is frozen. |
| Original Owner Request | “非常好，按照这个顺序执行。” |
| Similar-Issue Sweep | EXE-relative system/firmware roots, stage manifest format, COMMAND path bound, media source identity, observer staging behavior and prior invalid external-EXE observation. |

**Plan:** [M0 T357 staged boot and COMMAND-ready boundary](etc/operations/m0-t357-cpu40-staged-boot-command-ready-plan-001.md).

**Result:** [staged runtime identity](etc/evidence/m0-t357-s1-staged-runtime-identity-001.md).

### M0 T357 S2 — Closed frozen staged boot observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T357 S2; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | T357 S1 closed under the owner-directed dependency-first activation sequence. |
| Objective | Execute exactly one fixed console-owning observation of the T357 task-owned stage and classify its first original boundary or terminal result. |
| Non-goals | No source/media mutation, alternate package root, BOP repair, CPU/device semantic change, CPU30, Bochs, x64, CSRSS/kernel VDM, WOW, external child or graphics work. |
| Reference Baseline | T357 S1 stage manifest and T356 S4's invalid external-product observation diagnosis. |
| Files And ABI Surface | Existing `tools/observation/ObserveSoftpcStartup.mjs`, console-owning observer, staged product and observation report. No production ABI change. |
| Applicable Rules | Execution, source policy, component activation registry, fixed-container observation discipline, CPU40-only and build-output hygiene. |
| Verification | One observer command against the stage; report/exit review; source attribution only after a concrete marker is observed. |
| Expected Markers | A source-defined original startup marker, or one exact terminal/rejection marker that is not the previously invalid external-EXE package rejection. |
| Asset Needs | T357 S1 stage, T356 formal product identity and the existing console-owning observer. |
| Reporting Requirements | Preserve exact command/output/exit evidence; identify the first original owner boundary; do not infer guest execution from staging or formal linking. |
| Stop Conditions | Observer/stage identity mismatch; product/media mutation; more than one observation; or a proposed repair before marker attribution. |
| Exit Criteria | One fixed staged observation is recorded and classified; S3 is either admitted for its named missing owner or T357 reaches its source-defined COMMAND-ready boundary. |
| Original Owner Request | “非常好，按照这个顺序执行。” |
| Similar-Issue Sweep | Stage adjacency, observer product copy, console ownership, media identity and app startup markers. |

**Plan:** [M0 T357 staged boot and COMMAND-ready boundary](etc/operations/m0-t357-cpu40-staged-boot-command-ready-plan-001.md).

**Result:** The one fixed observation reached the app's original-command-path
admission dialog and timed out there; its 58-character stage root cannot fit
the existing 64-byte generated shell contract.  This is an app package-stage
terminal, not a SoftPC, BOP or guest result.  S3 owns the one permitted
short-path re-stage using the same immutable inputs.

### M0 T357 S3 — Closed capacity-valid stage correction and resumed observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T357 S3; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | T357 S2 identified a source-defined app package-path-capacity terminal under the approved dependency-first sequence. |
| Objective | Create one new short-path executable-relative stage from the same formal product and immutable media, then resume the identical fixed observation once. |
| Non-goals | No product/guest/media mutation, host alias, BOP repair, CPU/device semantic change, CPU30, Bochs, x64, CSRSS/kernel VDM, WOW, native child or graphics work. |
| Reference Baseline | T357 S1 manifest identity, T357 S2 timeout report and `app_package_layout_command_value_fits`'s existing 64-byte original COMMAND contract. |
| Files And ABI Surface | Existing staging/observation tools, disposable short-path output and observation report only. No production ABI change. |
| Applicable Rules | Execution, source policy, component activation registry, fixed-container observation discipline, output hygiene and CPU40-only. |
| Verification | New manifest product/media hashes equal T357 S1; one resumed fixed observer command; exact first original boundary or terminal attribution. |
| Expected Markers | The app reaches original SoftPC entry rather than the path-capacity dialog, then records either an original startup marker, a named missing owner, or a classified terminal. |
| Asset Needs | T356 formal product, selected immutable media, Node 22, fixed console-owning observer and new `O:\\t357` stage output. |
| Reporting Requirements | Record both stage paths, identity equality and exactly one resumed command; do not claim guest execution unless its marker is written. |
| Stop Conditions | Short output already exists; input hashes change; stage tool/source change is required; or a repair is proposed before source attribution. |
| Exit Criteria | One capacity-valid stage and one resumed observation are recorded; T357 either reaches a source-defined COMMAND-ready boundary or identifies its single exact immediate missing owner. |
| Original Owner Request | “非常好，按照这个顺序执行。” |
| Similar-Issue Sweep | Original 64-byte shell buffer, executable-relative roots, product/media hashes, short-path identity and observer invariants. |

**Plan:** [M0 T357 staged boot and COMMAND-ready boundary](etc/operations/m0-t357-cpu40-staged-boot-command-ready-plan-001.md).

**Result:** [staged COMMAND-ready observation](etc/evidence/m0-t357-s2-s3-staged-command-ready-observation-001.md).

**T357 closure:** The capacity-valid, immutable staged CPU40 product reached
the original COMMAND-ready boundary.  It did not prove a completed COMMAND
`/C` workload, guest child, native child or Win16 execution.  The next
candidate is therefore the already ordered COMMAND built-in and batch
lifecycle package; no immediate missing owner component was proven.

### M0 T358 S1 — Closed original COMMAND workload and return topology

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Owner approved ordered execution; T357 reached the required COMMAND-ready boundary. |
| Objective | Establish a source-defined original COMMAND `/C` built-in/batch completion-and-return path, select the lowest-dependency immutable workload and identify any immediate missing original owner. |
| Non-goals | No runtime observation, source mutation, external DOS EXEC, cmdExec32, WOW, CPU/device change, guest mutation, CPU30, Bochs, x64, CSRSS/kernel VDM, Redirector or graphics work. |
| Reference Baseline | T357 final staged COMMAND-ready evidence; original `dos/command/{cmdmisc.c,cmddisp.c,cmdexec.c,cmdconf.c}` and app Base-VDM declaration route. |
| Files And ABI Surface | Original COMMAND route, Base-VDM `VDMINFO`, COMMAND BOP services, original guest completion markers and formal graph selection. No new ABI. |
| Applicable Rules | Execution, source policy, package-first recovery, component activation registry, CPU40-only, mirror/divergence and fixed-observation rules. |
| Verification | Static source/graph ownership map; selected workload identity; explicit proof whether batch stays inside COMMAND or crosses to DOS EXEC. |
| Expected Markers | One exact built-in route/return marker and a classified batch disposition; no runtime claim in S1. |
| Asset Needs | T357 source-identical stage evidence, OpenNT COMMAND sources, formal source manifest and Node 22 search tooling. |
| Reporting Requirements | Separate COMMAND-internal completion from DOS child/native child/WOW paths; name a missing original owner only with source and formal-graph evidence. |
| Stop Conditions | Selected route crosses external DOS EXEC or missing owner without a bounded package; runtime execution proposed before topology is closed. |
| Exit Criteria | Immutable built-in workload and return contract are source-defined; batch is either included with a complete owner path or explicitly handed to T359; S2 is admitted only for an exact missing owner. |
| Original Owner Request | “非常好，按照这个顺序执行。” |
| Similar-Issue Sweep | `/C` declaration, `cmdGetNextCmd`, Base-VDM command handoff, COMMAND BOPs, batch-file path, `cmdReturnExitCode` and DOS EXEC boundary. |

**Plan:** [M0 T358 COMMAND built-in and batch lifecycle](etc/operations/m0-t358-command-built-in-batch-lifecycle-plan-001.md).

**Result:** [COMMAND built-in and batch topology](etc/evidence/m0-t358-s1-command-built-in-batch-topology-001.md).

### M0 T358 S2 — Closed immediate owner recovery disposition

The formal source manifest selects the full original COMMAND cohort, Base-VDM
binding and same-shaped termination binding needed by `/C EXIT`. No immediate
original owner is missing, so this recovery subtask has no implementation
work. The batch boundary is explicitly transferred to T359 rather than
inventing a COMMAND-local replacement.

### M0 T358 S3 — Closed fixed `/C EXIT` control-flow boundary observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S3; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | T358 S1/S2 established that the selected original `/C EXIT` route is complete in the formal graph. |
| Objective | Run a fixed console-owning observation from a fresh capacity-valid stage and prove or precisely classify original COMMAND `/C EXIT` delivery and completion. |
| Non-goals | No batch execution, DOS child/EXEC, cmdExec32, WOW, CPU/device change, guest mutation, CPU30, Bochs, x64, CSRSS/kernel VDM, Redirector or graphics work. |
| Reference Baseline | T357 capacity-valid package contract and T358 S1 source topology. |
| Files And ABI Surface | Existing stage/observation tools, original COMMAND guest and host dispatcher, termination binding and disposable evidence output. No new ABI. |
| Applicable Rules | Execution, source policy, package-first recovery, fixed-container observation, CPU40-only, mirror/divergence and output-hygiene rules. |
| Verification | New manifest identity/hash review; fixed `--command EXIT` observer sequence, with one observer-only identical rerun to copy original modal-error text; inspect `54:01`, `54:00`, BOP return and controlled-stop reports. |
| Expected Markers | Original `54:01` followed by `54:00`/controlled return; if not reached, record the exact preceding original marker without repair. |
| Asset Needs | Formal T356 product, immutable media, new short-path stage, existing observer and Node 22. |
| Reporting Requirements | Distinguish command-ready from delivered/complete EXIT; do not infer DOS EXEC, batch, native child or WOW execution. |
| Stop Conditions | Stage identity/capacity failure, output mutation, more than one valid observation, or repair proposed before source attribution. |
| Exit Criteria | A capacity-valid observation sequence is recorded with source attribution; T358 either proves built-in completion or pauses at its exact next original owner boundary. |
| Original Owner Request | “非常好，按照这个顺序执行。” |
| Similar-Issue Sweep | `54:01`, `54:00`, Base-VDM declaration, `SINGLECOM`, controlled stop, NTDOS EXEC and batch external-program boundary. |

**Plan:** [M0 T358 COMMAND built-in and batch lifecycle](etc/operations/m0-t358-command-built-in-batch-lifecycle-plan-001.md).

**S3 closure:** [COMMAND `/C EXIT` control-flow boundary](etc/evidence/m0-t358-s3-command-exit-control-flow-boundary-001.md)
proves that selected original COMMAND and DEM code continue past `54:0F`, then
the selected CPU40 path attempts to execute immutable `COMMAND.COM` data at
`03F4:2171`.  T358 is paused, not closed, until T359 resolves that predecessor.

### M0 T359 S1 — Closed CPU40 COMMAND-initialization source and image map

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T359 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted under the ordered dependency-first execution rule after T358 S3 proved the exact selected CPU40 boundary. |
| Objective | Reconstruct the original CPU40/SAS/COMMAND-image control-transfer path from the last reached BOP return to `illegal_op_int`, and select at most one complete recovery cohort. |
| Non-goals | No COMMAND or DEM provider rewrite, BOP leaf repair, DOS EXEC, batch, guest/firmware mutation, CPU30, Bochs, x64 runtime, CSRSS/kernel VDM, WOW, Redirector, graphics or broad CPU rewrite. |
| Reference Baseline | T358 S3's fixed stage, product SHA-256, `03F4:2171` fault record, immutable COMMAND.COM and selected CPU40 formal manifest. |
| Files And ABI Surface | Original `c_main.c`, instruction/interrupt/return owners, SAS `.COM` image layout, `illegalp.c`, selected CCPU40 vectors and any formally selected same-shaped support binding. No new guest ABI. |
| Applicable Rules | Execution, source policy, package-first recovery, component activation registry, CPU40-only, mirror/divergence, mapping-manager and fixed-observation rules. |
| Verification | Original definition/caller/image map; mirror-versus-source comparison; formal graph/import review; no runtime retry or source change in S1. |
| Expected Markers | One source ledger classifies the first data control transfer as source-defined input/state, a mirror divergence, an omitted original provider, or a bounded same-shaped adapter seam. |
| Asset Needs | Existing original mirrors, immutable T358 stage evidence, source map and Node 22 tools; no media rebuild or host-system mutation. |
| Reporting Requirements | Separate the loaded immutable COMMAND bytes, faulting CPU state, prior BOP route and every candidate control-transfer writer; do not infer a repair from the timeout. |
| Stop Conditions | Any need to change guest bytes, manufacture a COMMAND return, add an unbounded CPU trace, alter a BOP result or proceed to a later workload pauses for renewed admission. |
| Exit Criteria | The first bad control-transfer owner and all immediate selected edges have a source disposition, and one S2 recovery cohort (or exact terminal) is bounded. |
| Original Owner Request | “非常好，按照这个顺序执行。” |
| Similar-Issue Sweep | `.COM` PSP/IP convention, CCPU `CALL`/`RET`/`IRET`/interrupt delivery, BOP IP advancement, CS:IP state publication, SAS reads and original COMMAND data/code map. |

**Plan:** [M0 T359 CPU40 COMMAND-initialization control-flow recovery](etc/operations/m0-t359-cpu40-command-initialization-control-flow-plan-001.md).

**S1 closure:** [original COMMAND transient control-flow map](etc/evidence/m0-t359-s1-command-transient-control-flow-map-001.md) establishes the whole original COMMAND resident-to-transient allocation/move/reload and NTDOS prerequisite cohort as the owner.  CCPU transfer files, immutable image identity, and the BOP result are not the repair seam.

### M0 T359 S2 — Closed original COMMAND transient lifecycle recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T359 S2; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed T359 S1 under ordered dependency-first execution. |
| Objective | Restore the complete original `EndInit`/`LodCom`/`HavCom` transient lifecycle and its normal NTDOS allocation and `50:12/00/16/02` file prerequisites without a guest-address seed or BOP-specific success. |
| Non-goals | No CPU instruction rewrite, BOP leaf repair, guest-media mutation, direct transient copy, DOS EXEC, batch semantics, CPU30, Bochs, x64 runtime, CSRSS/kernel VDM, WOW, Redirector or graphics work. |
| Files And ABI Surface | Original COMMAND `rdata.asm`, `command2.asm`, `tcode.asm`, `tspc.asm`, original NTDOS allocation/INT 21h flow, selected DEM file bindings and existing mapping-manager leases. |
| Verification | Whole source definition/caller/error map; original-versus-mirror and formal selection review; bounded owner-package local tests; formal link.  A renewed frozen observation is S4 only. |
| Exit Criteria | Every owner edge has a selected original body or a same-shaped binding, local lifecycle evidence preserves normal and failure paths, and S3 has a bounded formal-link input list. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S2 P1:** [COMMAND transient SAS-write binding](etc/evidence/m0-t359-s2-command-transient-sas-write-binding-001.md) expands the existing default-off, selector-blind CCPU SAS observer from one CONFIG byte to the one original byte/word/dword write contract needed to observe `TrnSeg` and `HavCom` control data.  A fresh CPU40/x86 formal graph linked all 428 actions; no renewed runtime observation or lifecycle-completion claim is made.

**S2 closure:** [COMMAND transient owner/binding closure](etc/evidence/m0-t359-s2-command-transient-owner-binding-closure-001.md) proves that the selected original `MS_bop_0 -> DemDispatch -> 50:00/02/12/16` cohort has no missing immediate same-shaped NTDOS/DEM binding.  Its normal and failure directions remain original; the remaining question is runtime state, not a permitted synthetic repair.

### M0 T359 S3 — Closed local SAS-store conformance and formal link

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T359 S3; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed T359 S2 under ordered dependency-first execution. |
| Objective | Prove the bounded SAS write observer's local default-off and overlap contract, and formally link the selected original COMMAND/NTDOS/DEM/CPU40 cohort before one renewed frozen observation. |
| Non-goals | No guest execution, COMMAND/NTDOS/DEM semantic change, CPU rewrite, BOP leaf repair, media mutation, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, Redirector, EXEC or graphics work. |
| Verification | Reproducible x86 Ninja fixture plus fresh CPU40/x86 formal product link; governance/export and diff review. |
| Exit Criteria | The observer proves no hidden session/guest-memory dependency and the selected product links without converting a local result into a lifecycle claim. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S3 closure:** [SAS-store observer local conformance](etc/evidence/m0-t359-s3-sas-store-observer-local-conformance-001.md) passes its 5-action x86 Ninja graph.  The formal product remains linked at 428 actions.  Neither result is guest-runtime evidence.

### M0 T359 S4 — Closed invalid long-root observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T359 S4; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed T359 S3 under ordered dependency-first execution. |
| Objective | Make exactly one immutable CPU40/x86 resumed `/C EXIT` observation, watching only the source-mapped `TrnSeg` control-word range, and record whether original execution reaches `54:01` or a narrower source-owned boundary. |
| Non-goals | No retry, no second workload, no COMMAND/NTDOS/DEM/CPU/BOP repair, no guest or media mutation, no new mapping, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, Redirector, EXEC or graphics work. |
| Verification | Fresh staged capacity-valid package; product and media identity; one bounded run; copied observer report and terminal report; governance/export and diff review. |
| Exit Criteria | One immutable run records the configured post-store controls and either reaches original `54:01` or names one exact preceding original owner boundary; a capacity-invalid stage closes with no semantic result. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S4 closure:** [invalid long-root observation disposition](etc/evidence/m0-t359-s4-invalid-long-root-observation-001.md) records the one attempted stage as capacity-invalid (74-character `mvdm` root versus the original 63-byte contract).  Its timeout and absent reports are not used as lifecycle evidence.

### M0 T359 S5 — Closed invalid suffixed-root observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T359 S5; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T359 S4's capacity-invalid-stage disposition under ordered dependency-first execution and the previously approved `O:\ntvdm64` short output root. |
| Objective | Make one replacement immutable CPU40/x86 resumed `/C EXIT` observation from a new non-overwriting short-root stage, watching only original `TrnSeg` two-byte writes and recording the next source-owned boundary. |
| Non-goals | No reuse or interpretation of the invalid stage; no retry, source change, media mutation, BOP/DEM/COMMAND/CPU repair, new mapping, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, Redirector, EXEC or graphics work. |
| Verification | Short-root capacity check; fresh product/media manifest; exactly one bounded console-owning run; copied observer report and terminal report. |
| Exit Criteria | The complete original generated `shell=` value is at most 63 visible characters and its single run either reaches original `54:01` or identifies one exact source-owned predecessor. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S5 closure:** [invalid suffixed-root observation disposition](etc/evidence/m0-t359-s5-invalid-suffixed-root-observation-001.md) corrects the capacity metric.  The 65-character full `shell=` value made the new subdirectory stage invalid; mapped frames prove app-owned path rejection, not a COMMAND result.

### M0 T359 S6 — Closed verified-root app-layout boundary

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T359 S6; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T359 S5's capacity disposition under ordered dependency-first execution and the user-designated `O:\ntvdm64` format-3 runtime container. |
| Objective | Replace only the explicitly mutable product in the hash-verified `O:\ntvdm64` fixed-media container, then make exactly one CPU40/x86 resumed `/C EXIT` observation with the original `TrnSeg` two-byte watch. |
| Non-goals | No media replacement, restaging, deletion, retry, source or BOP/DEM/COMMAND/CPU repair, new mapping, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, Redirector, EXEC or graphics work. |
| Verification | Verify all 16 fixed media hashes; one product replacement; exactly one bounded console-owning run; copied observer and terminal reports. The complete original `shell=` format is evaluated with both SystemRoot occurrences. |
| Exit Criteria | Closed: the verified root still maps to app-owned layout rejection before SoftPC/COMMAND execution; an app-owned package-layout predecessor is admitted. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S6 closure:** [verified-root app-layout boundary](etc/evidence/m0-t359-s6-verified-root-app-layout-boundary-001.md) proves that the 15-byte `O:\ntvdm64\mvdm` root produces a 65-byte complete original `shell=` value. The app rejects it before SoftPC/COMMAND execution. T359 is paused behind M0 T360.

### M0 T360 S1 — Closed original SystemRoot package-layout source map

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T360 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T359 S6 under the dependency-first execution rule. |
| Objective | Map every immediate original MVDM loader, configuration and firmware pathname that consumes SystemRoot or firmware root; compare it with current app staging; select one original-shaped package layout before code or media changes. |
| Non-goals | No original MVDM, guest or firmware edit; no BOP/DEM/COMMAND/CPU repair; no host drive alias; no media byte mutation; no CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, EXEC or graphics claim. |
| Files And ABI Surface | Original `cmdconf.c`, NTDOS configuration/loader callers, original SoftPC file-location callers, app `package_layout.c`, launch declarations and `Stage-OriginalSoftpcRuntime.mjs`. |
| Verification | Source declaration/caller/path-format ledger; current staged-media topology and manifest review; documentation governance/export and diff review. No product build or runtime run in S1. |
| Exit Criteria | Every reached immediate root consumer has an owner and exact pathname contract; S2 is limited to an app/staging layout change that restores the original directory shape without changing media bytes or original source. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S1 closure:** [original SystemRoot root-consumer map](etc/evidence/m0-t360-s1-original-systemroot-root-consumer-map-001.md) closes the reached root-consumer inventory.  It selects an executable-directory SystemRoot and its `softpc` firmware child; the prior `mvdm` child has no original owner.

### M0 T360 S2 — Closed app/staging original-layout binding

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T360 S2; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed T360 S1 under ordered dependency-first execution. |
| Objective | Restore the source-defined package shape by binding session SystemRoot to the executable directory, firmware to its `softpc` child, and staging the unchanged media at those relative paths. |
| Non-goals | No original MVDM/guest/firmware source or byte modification; no path alias, drive mapping, BOP/DEM/COMMAND/CPU repair, CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, EXEC or graphics work. |
| Verification | Static source/stage destination review, focused package-layout fixture, formal graph/import review, governance/export and diff review. No runtime observation in S2. |
| Exit Criteria | App and stager agree on every S1 media path; `cmdconf.c`, guest `commnd`, DEM NTDOS loading and original `host_find_file` require no new pathname divergence. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S2 closure:** [app/staging original-layout binding](etc/evidence/m0-t360-s2-app-staging-original-layout-binding-001.md) moves no media bytes semantically: the same sixteen source assets now stage at the original SystemRoot-relative and `softpc`-relative destinations. App selects the matching roots.

### M0 T360 S3 — Closed package-layout conformance

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T360 S3; ordinary mode (single-person dual-role implementation). |
| Objective | Prove the app package-layout contract and staging manifest agree with each S1 root consumer and the original `commnd` capacity rule. |
| Verification | x86 Ninja package-layout fixture; fixed `O:\ntvdm64` root arithmetic; fresh non-runtime staging destination inspection. |
| Exit Criteria | Closed: the fixture passes, `O:\ntvdm64` produces a 54-byte complete `shell=` value, and all sixteen required immutable assets occur exactly at their selected root-relative paths. |

**S3 closure:** [original SystemRoot package-layout conformance](etc/evidence/m0-t360-s3-original-systemroot-package-layout-conformance-001.md) records the 5-action x86 fixture pass, 54-byte root result and 16/16 staged-media path verification.

### M0 T360 S4 — Closed formal product/package closure

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T360 S4; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed T360 S2/S3 under ordered dependency-first execution. |
| Objective | Formally rebuild the original CPU40/x86 product with the app-layout change and stage one fresh immutable SystemRoot-shaped package with recorded product/media identity. |
| Non-goals | No runtime observation, original source/media mutation, BOP/DEM/COMMAND/CPU repair, CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, EXEC or graphics work. |
| Verification | Formal Ninja target, staged manifest hash review, documentation governance/export and diff review. |
| Exit Criteria | A formally linked product and a new format-3 package carry exactly the original SystemRoot directory shape; S5 alone may execute the frozen workload. |

**S4 closure:** [formal original-SystemRoot package closure](etc/evidence/m0-t360-s4-formal-original-systemroot-package-closure-001.md) records the 4-action formal x86 re-link and a fresh format-3 package with all sixteen immutable hashes verified under root/System32/softpc paths.

### M0 T360 S5 — Closed invalid observer-output attempt

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T360 S5; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed T360 S4 under ordered dependency-first execution. |
| Objective | Make one console-owned CPU40/x86 `/C EXIT` observation from the new hash-verified original-SystemRoot package, preserving T359's bounded SAS-store watch. |
| Non-goals | No retry, source/media mutation, BOP/DEM/COMMAND/CPU repair, new mapping, CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, EXEC or graphics work. |
| Verification | One bounded product run; product/media identity; terminal, exception and SAS-store reports; frame mapping only if required. |
| Exit Criteria | Closed invalid: observer output-parent absence prevents its required report/JSON records. No product inference is retained; one replacement observation is admitted only after its report directory is pre-created. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S5 closure:** [invalid observer-output disposition](etc/evidence/m0-t360-s5-invalid-observer-output-disposition-001.md) records that the observer executed but its required report parent did not exist, so it produced no durable evidence. It makes no product claim.

### M0 T360 S6 — Closed invalid long-stage-root observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T360 S6; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed invalid T360 S5 under ordered dependency-first execution. |
| Objective | Make exactly one replacement observation with the same frozen formal product and immutable package, after pre-creating the observer report directory. |
| Non-goals | No product or media change, retry with changed workload, BOP/DEM/COMMAND/CPU repair, new mapping, CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, EXEC or graphics work. |
| Verification | Report-parent existence check before launch; one console-owned run; required terminal/exception/SAS-store report collection and product/media identity review. |
| Exit Criteria | Closed invalid: although relative paths were correct, the repository stage root was 67 bytes and violated the original `commnd` contract before product execution. One replacement may use the owner-designated short root. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S6 closure:** [invalid long-stage-root observation disposition](etc/evidence/m0-t360-s6-invalid-long-stage-root-observation-001.md) retains the complete observer record but makes no SoftPC/COMMAND inference from the app-owned capacity rejection.

### M0 T360 S7 — Closed owner-root original-layout observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T360 S7; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed invalid T360 S6 under ordered dependency-first execution and the owner-designated `O:\ntvdm64` short output root. |
| Objective | Materialize the already verified root/System32/softpc media layout directly at `O:\ntvdm64` without deleting its prior `mvdm` subtree, then make exactly one console-owned CPU40 `/C EXIT` observation. |
| Non-goals | No MVDM/guest/firmware source or byte change, no deletion or reinterpretation of the pre-existing `mvdm` subtree, no BOP/DEM/COMMAND/CPU repair, new mapping, CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, EXEC or graphics work. |
| Verification | Explicit 54-byte complete `shell=` preflight; hash verification of all sixteen selected root-layout media entries; one durable observation report. |
| Exit Criteria | Closed: the selected short-root observation reaches original `54:01` after the complete transient/configuration path; its current wait is a separate Base VDM command-delivery owner boundary. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S7 closure:** [owner-root original-layout observation](etc/evidence/m0-t360-s7-owner-root-original-layout-observation-001.md) verifies the root package and reaches original `54:01`, `54:0E` and a second `54:01` entry. The active wait maps to original `base_vdm_local_wait_for_command`, not app admission or SoftPC failure.

**M0 T360 closure:** Original SystemRoot package-layout recovery is closed. The user-designated `O:\ntvdm64` now contains the source-defined root/System32/softpc package layout, preserving its previous unused `mvdm` subtree. All original root consumers reached by this workload resolve without path rewriting.

**M0 T359 closure:** The valid T360 S7 observation proves the selected original transient lifecycle no longer transfers into immutable COMMAND data and reaches `54:01`. T359 is closed; no additional CPU or COMMAND repair is admitted by its result.

### M0 T358 S4 — Closed Base VDM `/C` command-record lifecycle audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S4; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | T360 S7 closed T359's predecessor and resumed T358's unchanged `/C EXIT` workload. |
| Objective | Reconstruct the original Base VDM command-delivery path from reached `54:01` through `GetNextVDMCommand` and `base_vdm_local_wait_for_command`, determine whether the declared `/C EXIT` record is consumed, and select one whole source-owned recovery cohort. |
| Non-goals | No BOP leaf repair, retry, DOS EXEC, cmdExec32, guest/media mutation, CPU/SoftPC semantic change, CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, Redirector or graphics work. |
| Files And ABI Surface | Original COMMAND `cmdGetNextCmd`, imported Base VDM contract, current app launch declaration, Base VDM broker and session command-source binding. No ABI change in S4. |
| Verification | Original definition/caller/queue-state audit; current record lifecycle review; formal graph/import review; governance/export and diff review. No new runtime run in S4. |
| Exit Criteria | Closed: one exact source-owned command-record lifecycle ledger proves delivery complete and bounds the next whole recovery S. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S4 closure:** [Base VDM command-record lifecycle audit](etc/evidence/m0-t358-s4-base-vdm-command-record-lifecycle-audit-001.md) proves that the first `54:01` consumes the unique declared `/C EXIT` record.  The later wait is the original empty-queue path; it is not a Base VDM defect.

### M0 T358 S5 — Closed guest COMMAND `/C` exit-path topology audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S5; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed S4 under the existing T358 package objective and ordered dependency-first execution. |
| Objective | Map the selected original guest `COMMAND.COM` `/C EXIT` parser, built-in exit, DOS/host return and second command-acquisition paths after the first successful `54:01`, then select one complete earliest owner cohort. |
| Non-goals | No Base VDM broker change, BOP leaf repair, retry, DOS external EXEC, cmdExec32, guest/media mutation, CPU/SoftPC semantic change, CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, Redirector or graphics work. |
| Files And ABI Surface | Original guest `mvdm-guest/dos/v86/cmd/command` source/map/binary; original COMMAND host `cmdmisc.c`, `cmdexit.c`, `cmdexec.c`; NTDOS execution/return sources as evidence only. No ABI change in S5. |
| Verification | Original guest/host control-flow and image/map audit; selected-source/provenance and formal graph review; governance/export and diff review. No new runtime run or production change in S5. |
| Exit Criteria | Closed: one source-owned ledger distinguishes the guest `/C` route from the child-return Base VDM lifecycle and bounds one whole recovery cohort. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S5 closure:** [guest COMMAND `/C EXIT` topology audit](etc/evidence/m0-t358-s5-command-c-exit-topology-audit-001.md) proves that the current wait belongs to the missing BaseSrv DOS parent/child record lifecycle after the correct first delivery, not to `/C` parsing or a COMMAND BOP leaf.

### M0 T358 S6 — Closed single-session Base VDM DOS record-lifecycle recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S6; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed S5 under the existing T358 package objective and ordered dependency-first execution. |
| Objective | Recover the complete reached single-session DOS record lifecycle from original BaseSrv/BaseClient: initial command, busy child, return exit code, parent/no-command disposition, wake/retry and record cleanup; then formally link and locally test it. |
| Non-goals | No CSRSS/CSR transport, cross-process or multi-session broker, WOW notification, raw handle transfer, generic scheduler, BOP leaf repair, guest/media mutation, DOS external EXEC, cmdExec32, CPU/SoftPC semantic change, CPU30, Bochs, x64, Redirector or graphics work. |
| Files And ABI Surface | Original `opennt-host/base/win32/{server/srvvdm.c,client/vdm.c}` as control-flow authority; `adapter-mvdm-host-out/basesrv` local broker/client; session event lifecycle and existing app declaration. Guest and MVDM mirror function bodies remain unchanged. |
| Verification | State/caller/failure ledger; focused local Base VDM record-lifecycle fixture; formal CPU40/x86 link; governance/export and diff review. Runtime observation is separately admitted only after local/formal closure. |
| Exit Criteria | Closed: the selected one-session state machine preserves all reached initial/return/retry outcomes with no retained guest pointer or raw native handle, its fixture covers record transitions and cleanup, and the formal product links. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S6 closure:** [single-session Base VDM DOS-record lifecycle recovery](etc/evidence/m0-t358-s6-single-session-base-vdm-dos-record-lifecycle-recovery-001.md) records the complete reached local state transition, focused x86 fixture pass and formal CPU40/x86 product link.

### M0 T358 S7 — Closed fixed `/C EXIT` lifecycle observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S7; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed S6 under the existing T358 package objective and ordered dependency-first execution. |
| Objective | Run one console-owning, bounded `/C EXIT` observation using the S6 formal product and the fixed `O:\ntvdm64` original-layout package; classify the first source-owned successor after the Base VDM child-return repair. |
| Non-goals | No product/media change, retry with altered workload, BOP leaf repair, guest mutation, DOS external EXEC, cmdExec32, CPU/SoftPC semantic change, CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, Redirector or graphics work. |
| Verification | Product/media hash check; one eight-second console-owning observation; existing source-neutral BOP/COMMAND/termination reports; source frame attribution only if a new terminal occurs. |
| Exit Criteria | One frozen observation either reaches a source-owned built-in completion/controlled session termination marker, or identifies one exact earliest source-owned predecessor for the next whole recovery cohort. |
| Original Owner Request | “非常好，按照这个顺序执行。” |

**S7 closure:** [fixed `/C EXIT` lifecycle observation](etc/evidence/m0-t358-s7-fixed-exit-lifecycle-observation-001.md)
proves the root/SystemRoot package is accepted and that two original `54:0F`
environment requests return before the first `54:01`.  The fixed run then
reaches original guest `Alloc_error`; it does not reach the S6 Base VDM
child-return state or COMMAND completion.

### M0 T358 S8 — Closed original initial-environment allocation cohort audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S8; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted under the owner-approved ordered queue execution after S7's fixed observation selected this immediate original predecessor; no product behavior is changed in this audit. |
| Objective | Reconstruct the complete original `54:0F` initial-environment delivery, guest two-call allocation/reallocation, NTDOS arena allocation and `Alloc_error` continuation; select one bounded source-shaped recovery or exact terminal. |
| Non-goals | No new runtime observation, provider/BOP leaf rewrite, guest/media change, BaseSrv change, DOS external EXEC, cmdExec32, CPU/SoftPC semantic change, CPU30, Bochs, x64, CSRSS/BaseSrv, WOW, Redirector or graphics work. |
| Reference Baseline | S7 fixed root/media/product evidence: two successful original `54:0F` returns, no first `54:01`, and stopped `03F4:1F8F` at the mapped `Alloc_error` entry. |
| Files And ABI Surface | Original `dos/command/{cmdenv.c,cmddata.c,cmdmisc.c}`, guest `command/{rdata.asm,init.asm,envdata.asm}` and reached NTDOS allocation/arena source; current same-shaped command length and guest-memory bindings as comparison only. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original definition/caller/register/data-flow map; mirror-versus-source diff; selected formal graph/import review; documentation governance/export and diff review. No build or runtime execution in S8. |
| Expected Markers | A source ledger states both `54:0F` calls' `ES:0`/`BX` contract, all allocation/error branches, current binding and the earliest recoverable owner; it distinguishes a paragraph-count mismatch, allocator failure and resident-memory integrity failure. |
| Asset Needs | Existing source mirrors, selected external OpenNT source, S7 report/map, formal CPU40/x86 graph and Node 22; no new guest artifact or host-system mutation. |
| Reporting Requirements | Separate source fact, current binding and S7 observation; record all ABI values required in a later state-neutral observation and retain original NTDOS/COMMAND failure behavior. |
| Stop Conditions | Any conclusion from timeout alone, need to alter guest bytes, fabricate `54:01`, introduce an environment policy, change CPU/machine semantics or broaden into a runtime repair pauses for a new S admission. |
| Exit Criteria | Closed: the complete environment/allocation cohort has no unowned immediate edge. The later fixed root-layout observation proves its source continuation reaches `54:01`, so no recovery is selected. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `cmdGetInitEnvironment`, `cmdSetInfo`, `lpszComSpec`, host environment enumeration, `ES:0`/`BX`, INT 21h allocation/deallocation, arena/SFT state, `rdata.asm` second-call comparison, `Alloc_error` and resident image integrity. |

**S8 closure:** [original initial-environment allocation cohort audit](etc/evidence/m0-t358-s8-initial-environment-allocation-cohort-audit-001.md) proves that `54:0F` preserves its original two-call allocation contract and is no longer the live blocker in the valid root-layout package. The earlier `Alloc_error` observation remains historical evidence only.

**M0 T358 disposition:** The environment/allocation predecessor is closed, but the package has not yet observed the required original `/C EXIT` completion or return marker. The next S must source-map the reached first-command-to-later-`54:01` continuation before another workload observation.

### M0 T358 S9 — Closed `/C EXIT` completion-versus-reacquisition audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S9; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed S8 under the owner-approved ordered queue execution. |
| Objective | Reconstruct the exact original guest/host continuation from the first delivered `/C EXIT` record through the later `54:01`, distinguishing source-owned built-in completion, intended command reacquisition, Base VDM lifecycle, and any missing return marker before another workload observation. |
| Non-goals | No runtime observation, guest/media change, BOP leaf/provider rewrite, BaseSrv/CSRSS expansion, DOS external EXEC, cmdExec32, CPU/SoftPC semantic change, CPU30, Bochs, x64, WOW, Redirector, graphics, or new mapper. |
| Reference Baseline | T358 S4--S8 and T360 S7: the valid root-layout run delivers the first record, reaches `54:0E` and `54:04`, then reaches a later `54:01`; S8 proves `54:0F` allocation is not the active predecessor. |
| Files And ABI Surface | Original guest `dos/v86/cmd/command/{init.asm,tcode.asm,rdata.asm}` and maps; original host `dos/command/{cmdmisc.c,cmdexit.c,cmddisp.c}`; original BaseClient/BaseSrv VDM record contract and current bounded local binding as comparison only. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original definition/caller/register/control-flow map; original-versus-mirror review; current record-state/binding review; formal graph/import review; documentation governance/export and diff review. No build or runtime execution in S9. |
| Expected Markers | A source-owned ledger explains every transition between the two `54:01` calls, names the exact completion/return marker required for T358, and selects one bounded recovery or observation packet without conflating command wait with completion. |
| Asset Needs | Existing selected sources, guest maps/binaries, T358/T360 durable reports and formal CPU40/x86 graph; no new artifact or host mutation. |
| Reporting Requirements | Separate original source facts, current binding facts and frozen observation facts; state what a later observer must record to prove (or disprove) `/C EXIT` completion. |
| Stop Conditions | Any attempt to infer completion from BOP ingress or timeout, alter command records, add a producer, mutate guest media, synthesize termination, or broaden into DOS EXEC pauses for a new S admission. |
| Exit Criteria | Closed: the complete reached continuation has no unowned immediate edge and selects one state-neutral observation packet that can prove T358's required completion/return marker. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `SingleCom`, `/C` parser, `$EXIT`, permanent versus transient COMMAND, `SVC_CMDEXITVDM`, `cmdExitVDM`, `cmdReturnExitCode`, `RETURN_ON_NO_COMMAND`, `ASKING_FOR_SECOND_TIME`, Base VDM record transitions, and controlled session stop. |

**S9 closure:** [COMMAND `/C EXIT` completion-versus-reacquisition audit](etc/evidence/m0-t358-s9-command-exit-reacquisition-audit-001.md) proves that `54:00` is not the selected `/C` terminal and that the later `54:01` may be the source-defined parent reacquisition. It selects an observer-only discriminator.

### M0 T358 S10 — Closed original COMMAND continuation discriminator

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S10; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed S9 under the owner-approved ordered queue execution. |
| Objective | Extend the existing default-off COMMAND observer with fixed scalar provenance sufficient to distinguish first delivery, parent reacquisition and return behavior; formally link it without running a workload. |
| Non-goals | No guest/media mutation, command-text capture, pointer retention, Base VDM state change, producer, BOP/provider rewrite, BaseSrv/CSRSS, DOS EXEC, cmdExec32, CPU/SoftPC semantic change, CPU30, Bochs, x64, WOW, Redirector or graphics work. |
| Files And ABI Surface | Existing `cmddisp.c` diagnostic call boundary and default-off `mvdm_softpc_termination` observer interface only. New fields are copied scalars and not a product ABI. |
| Verification | Source ownership/order review; focused observer format proof; formal CPU40/x86 link; governance/export and diff review. No product workload in S10. |
| Expected Markers | Default-off entry/return records identify `CS:IP`, `AX`, `BX`, `CF`, first/repeat flags and bounded local DOS-record state for service `54:01`. |
| Stop Conditions | Any command string, guest/native pointer, BOP result, record state, session behavior, worker, guest byte or media mutation pauses for a new admission. |
| Exit Criteria | Closed: a formally linked default-off observer captures only the admitted scalars and leaves disabled output byte-for-byte behavior unchanged. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |

**S10 closure:** [original COMMAND continuation observer](etc/evidence/m0-t358-s10-command-continuation-observer-001.md) proves the source-shaped observer records entry/return provenance without altering the original dispatcher or Base VDM record.

### M0 T358 S11 — Closed invalid continuation-observation isolation audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S11; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed S10 under the owner-approved ordered queue execution. |
| Objective | Run exactly one frozen valid-root CPU40/x86 container with the new default-off continuation report and classify whether the second original `54:01` is parent reacquisition after the selected `/C EXIT`, a different source-owned continuation, or an earlier selected owner boundary. |
| Non-goals | No source, guest/media, package-layout, command-record, BOP/provider, BaseSrv/CSRSS, DOS EXEC, cmdExec32, CPU/SoftPC, CPU30, Bochs, x64, WOW, Redirector or graphics change. |
| Reference Baseline | T360 S7 valid root-layout report; closed T358 S8--S10; formal selected CPU40/x86 product. |
| Files And ABI Surface | No production file changes.  Existing app fixed-container observer and `MVDM_COMMAND_CONTINUATION_REPORT_PATH` only. |
| Verification | One fixed executable, root media, command declaration, console-owning container and timeout; durable report review; documentation governance/export and diff review. |
| Expected Markers | Entry/return `MVDM-CMD-CONT svc=01` records contain the admitted scalar provenance; their order and state classify the continuation without inferring a result from timeout alone. |
| Stop Conditions | A second run, any changed runtime input, or any production/source behavior change pauses for a new packet. |
| Exit Criteria | Closed invalid: the sole run proved that the report environment variable was an unadmitted original `cmdenv.c` input. It establishes no runtime classification and selects one diagnostic-isolation correction before any replacement observation. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |

**S11 closure:** [invalid continuation observation and diagnostic isolation](etc/evidence/m0-t358-s11-diagnostic-environment-isolation-001.md) records the invalid run without attributing it to product behavior.

### M0 T358 S12 — Closed observer environment-isolation correction

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S12; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed S11 under the owner-approved ordered queue execution. |
| Objective | Capture the optional host-only continuation-report path before original MVDM reads inherited variables, remove it from that process environment, and retain only a bounded adapter-private report path; then formally link and prove the default-off/enabled format. |
| Non-goals | No `cmdenv.c`/guest/media/command-record/BOP/provider/BaseSrv/CPU/SoftPC semantic change, command filtering, mapper, CPU30, Bochs, x64, WOW, Redirector or product workload. |
| Files And ABI Surface | App startup assembly and existing continuation observer only. The report path is adapter-private, not a guest or product ABI. |
| Verification | Focused default-off/capture/removal/format proof, formal CPU40/x86 link, governance/export and diff review. No product workload in S12. |
| Exit Criteria | Closed: the disabled observer produces no record; the enabled observer removes only its host-only selector before original startup and writes the same fixed scalar record; S13 may make one valid frozen run. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |

**S12 closure:** [continuation observer environment isolation](etc/evidence/m0-t358-s12-command-continuation-environment-isolation-001.md) proves capture/removal/format in the focused x86 fixture and a formal CPU40/x86 product link.

### M0 T358 S13 — Closed valid frozen continuation observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T358 S13; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by closed S12 under the owner-approved ordered queue execution. |
| Objective | Make exactly one fixed-root CPU40/x86 `/C EXIT` observation with the default-off continuation report captured outside original MVDM environment; classify the first source-owned successor after the two original `54:0F` returns. |
| Non-goals | No source, guest/media, package-layout, command-record, BOP/provider, BaseSrv/CSRSS, DOS EXEC, cmdExec32, CPU/SoftPC, CPU30, Bochs, x64, WOW, Redirector or graphics change. |
| Verification | One fixed executable, root media, command declaration, console-owning container and timeout; durable BOP/continuation report review; documentation governance and diff review. |
| Exit Criteria | Closed: one valid run reached original `54:05` but not `54:01`, with no diagnostic environment contamination; it transfers the earlier control-flow boundary without attributing a cause from timeout. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |

**S13 closure:** [valid continuation observation](etc/evidence/m0-t358-s13-valid-command-continuation-observation-001.md) records the one admissible run and its bounded, non-completion result.

**M0 T358 closure:** The COMMAND built-in/batch continuation packet is closed
as a correctly isolated observation boundary.  It makes no claim that `/C
EXIT` completed; its valid `54:05`-before-`54:01` result transfers to the
already queued CPU40/COMMAND initialization-control-flow recovery.


### M0 T355 S31 — Closed original `0x78` entry-result attribution

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S31; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted after S30's verified source-owned heartbeat cleanup under the standing owner direction for ordered source-first CPU40/x86 recovery. |
| Objective | Reconstruct the complete original source route that selects the returned `0x78` result, from all reached `VdmExitCode`/original-entry result writers through `host_terminate` or normal return, and select at most one source-owned successor. |
| Non-goals | No BOP/DEM/COMMAND result fabrication, guest/media/firmware mutation, CPU/CCPU/SAS/device semantic change, new worker, force-unbind, mapper change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC, graphics or retry. |
| Reference Baseline | S30 proves session disposal succeeds: the fixed CPU40/x86 container returns `0x78`, not app-owned disposal `0x48`. The result is therefore an original-entry outcome rather than a retained original worker binding. |
| Files And ABI Surface | Original `softpc.new/{obj.vdm,host/src,base}` result/termination callers, existing bounded execution bridge and app result propagation. No new public or guest ABI. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original definition/caller/control-flow audit; selected formal graph/import review; documentation governance/export and diff review. No runtime observation or production change in S31. |
| Expected Markers | One source ledger gives every reached writer/return path a physical identity, condition, selected-build status and owner disposition; it either selects one bounded source-shaped recovery S32 or records an exact original terminal. |
| Asset Needs | Existing original mirrors, S30 report/evidence, selected CPU40/x86 graph and source index only; no asset import, guest rebuild or host-system mutation. |
| Reporting Requirements | Distinguish raw `VdmExitCode`, original `main` return, app result propagation and observer exit; do not infer a failure cause from the number alone. |
| Stop Conditions | Any need to alter a guest byte, manufacture a success, add a diagnostic/fixture or run a second container observation pauses for a separately admitted successor. |
| Exit Criteria | The selected original `0x78` route has no unowned immediate source edge and names only one minimal recovery successor, or proves a source-owned terminal with no permitted recovery. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `VdmExitCode`, `VDMForWOW`, `host_terminate`, `TerminateVDM`, `terminate`, `host_main`, original `main`, application result propagation and all selected direct reset/error terminals. |

**S31 closure:** [original `0x78` entry-result attribution](etc/evidence/m0-t355-s31-original-0x78-entry-result-attribution-001.md)
proves that the observed result is a selected session controlled-stop rather
than normal `main` return or S30's disposal failure.  It deliberately does
not guess which linked `ERROR_CALL_NOT_IMPLEMENTED` entry was reached.

### M0 T355 S35 — Closed original COMMAND command-acquisition/wait cohort audit

**S35 closure:** [original COMMAND command-acquisition/wait cohort audit](etc/evidence/m0-t355-s35-original-command-command-acquisition-wait-cohort-audit-001.md)
establishes that the selected local Base VDM path already preserves the
original consumer-side `GetNextVDMCommand` capture/wait/retry contract, but
that `--ordinary-child` is only an observation producer—not the normal
BaseSrv-like command producer.  The next packet is limited to that app-to-
Base-VDM declaration seam; it may not repair a BOP leaf or recreate CSRSS.

### M0 T355 S36 — Closed source-shaped normal DOS command declaration recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S36; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S35's complete original COMMAND/BaseClient/BaseSrv cohort audit under the standing owner direction to continue the active packet. |
| Objective | Replace only the fixed-observation `--ordinary-child` producer with one explicit app-owned normal DOS launch declaration that publishes a source-shaped copied Base VDM DOS record to the existing local BaseClient/BaseSrv recovery. |
| Non-goals | No interactive console command reader, fabricated BOP result, guest/media mutation, DEM/FastRead change, CPU/CCPU/BIOS change, CPU30, Bochs, x64 runtime, CSRSS/BaseSrv reconstruction, WOW notification, PIF, redirection/standard-handle duplication, child/parent EXEC, device or graphics claim. |
| Reference Baseline | S35 source ledger; unchanged original `cmdGetNextCmd`, selected BaseClient/BaseSrv mirror evidence, existing one-session copied local record and fixed package-media admission. |
| Files And ABI Surface | `src/app/launch_declaration.[ch]`, `src/app/entry.c`, existing `base_vdm_command`/broker/local declarations and their local fixture.  No MVDM mirror function body or guest ABI changes. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original command/capture shape review; positive/negative app declaration fixture; existing Base VDM local contract fixture; formal CPU40/x86 link; one fixed-container observation only after local composition closes; governance/export and diff review. |
| Expected Markers | A declared ordinary DOS command arrives through the existing copied record with original CR/LF, application, environment, directory and DOS wait/retry semantics; invalid declaration is rejected before original host startup. |
| Asset Needs | Existing original mirrors, app, adapter, session, formal Ninja graph and immutable staged media; no new import or host-system mutation. |
| Reporting Requirements | Separate app declaration, original BaseClient/COMMAND contract and runtime observation; state every intentionally unavailable BaseSrv product-shell capability. |
| Stop Conditions | Any need for an ambient console reader, raw host/guest pointer, direct `cmdGetNextCmd` edit, CSRSS/CSR API, guest rewrite, media mutation, BOP leaf repair or runtime retry stops for new admission. |
| Exit Criteria | One explicit product launch declaration reaches the existing local Base VDM producer with source-shaped record contents; normal and rejected cases have focused evidence and the formal product links.  A later runtime observation may only classify its first successor. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `BaseSrvUpdateVDMEntry`, `VDM_TO_TAKE_A_COMMAND`, `GetNextVDMCommand`, `ASKING_FOR_SECOND_TIME`, `CmdLine` CR/LF, `VDMINFO` capacity, DOS versus WOW queue ownership, app argument parsing and immutable package root. |

**S36 closure:** [source-shaped normal DOS command declaration recovery](etc/evidence/m0-t355-s36-source-shaped-normal-dos-command-declaration-recovery-001.md)
adds one explicit copied `--command <text>` producer to the existing local Base
VDM contract. The positive and rejected forms pass its forced x86 fixture; the
formal CPU40/x86 product links; one fixed observation times out without a
trace-selected repair or guest-execution claim.

### M0 T356 S1 — Closed minimal DOS guest execution / controlled-return topology audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T356 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Owner admitted the next queue task with “准入.”  The graphics candidate remains source-gated by workload reachability, so this admissible P07 predecessor is selected first. |
| Objective | Establish the complete original source/ABI/failure topology from one copied Base VDM DOS command record through guest load, NTDOS `EXEC`, PSP/arena/JFN/environment, parent return, SoftPC re-entry and COMMAND completion; select the earliest complete recoverable owner cohort. |
| Non-goals | No BOP leaf repair, fabricated command or result, ambient console reader, guest/media rewrite, CPU/CCPU/BIOS change, graphics/presentation work, CPU30, Bochs, x64 runtime gate, CSRSS/kernel VDM recreation, WOW/PIF/cross-process-broker completion or runtime retry. |
| Reference Baseline | T354 permanent-command bootstrap closure; T355 S35/S36 source-shaped Base VDM consumer/producer evidence; P07 in the package recovery program; fixed CPU40/x86 package. |
| Files And ABI Surface | Original `mvdm-host/dos/command`, selected NTDOS/DOS guest execution sources and maps, original SoftPC entry/re-entry interfaces, Base VDM client/local/broker boundary, session mapping/lifecycle surfaces and exact accepted non-MVDM host evidence. No production ABI change in S1. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Definition/caller/data-layout/failure audit of all named owners; physical selected-source/provenance check; current source/binding review; documentation inventory/governance and diff review. No build or runtime execution in S1. |
| Expected Markers | A finite ordered ledger distinguishes record consumption, image load, `EXEC`, parent/child state, guest return, Base VDM completion and their exact unsupported product-shell edges; every immediate edge has one owner/disposition. |
| Asset Needs | Existing MVDM/OpenNT mirrors, guest maps/products as read-only source evidence, formal graph and T355 records; no new source import, guest rebuild, host mutation or runtime output. |
| Reporting Requirements | Separate source behavior from current composition; identify whether the first recovery belongs to original guest/DOS, original MVDM host, a same-shaped adapter, or an unavailable NT4 product shell; do not treat reachability inference as execution evidence. |
| Stop Conditions | Need for a new command semantic, guest change, raw pointer/HANDLE transfer, family-private mapping, alternate executor, CSRSS/kernel recreation, source import or trace-derived leaf repair pauses for renewed S admission. |
| Exit Criteria | One auditable package ledger covers every immediate edge and selects exactly one earliest complete source-owned recovery cohort (or an exact source-shaped unavailable terminal) without claiming a DOS workload ran. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `GetNextVDMCommand`, `cmdGetNextCmd`, `cmdExec`, `cmdReturnExitCode`, `demLoadDos`, DOS `EXEC`, PSP, MCB/arena, JFN/SFT, environment/COMSPEC, `host_simulate`, CPU40 re-entry, `ExitVDM`, `BaseSrvUpdateVDMEntry`, session completion and controlled stop. |

**S1 closure:** [Topology audit](etc/evidence/m0-t356-s1-minimal-dos-execution-controlled-return-topology-audit-001.md) proves the source-defined split between the Base-VDM command record, guest COMMAND/NTDOS execution and non-DOS `cmdExec32`.  It selects the complete original vertical guest cohort and records no S1 runtime claim.

### M0 T356 S2 — Closed selected original DOS command / controlled-return recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T356 S2; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | T356 was owner-admitted as one source-first package. S1 selected this exact cohort; S2 remains within its recorded plan and does not expand product scope. |
| Objective | Make one already packaged, immutable local DOS `.COM` command route source-complete from the declared Base VDM record through guest COMMAND, original NTDOS `$Exec`, original parent return and `cmdReturnExitCode`, using the smallest existing source-shaped composition seam. |
| Non-goals | No substitute executor, host child-process surrogate, guest binary/source mutation, ambient command input, BOP leaf repair, new media format, CPU/CCPU/BIOS change, CPU30, Bochs, CSRSS/kernel recreation, WOW/PIF/cross-process completion or speculative runtime workaround. |
| Reference Baseline | [S1 topology audit](etc/evidence/m0-t356-s1-minimal-dos-execution-controlled-return-topology-audit-001.md); T355 declared command contract; original selected `COMMAND.COM`, `NTIO.SYS` and `NTDOS.SYS` package media. |
| Files And ABI Surface | Original COMMAND/NTDOS/DEM paths and their existing Base VDM, session mapping and CPU40 bindings. A changed file, if any, must preserve original name, argument/layout/order and failure contract with registered divergence. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Static original-caller/data-layout/failure review; focused original-owner positive/negative fixture only if an existing seam changes; formal CPU40/x86 link only after source recovery; no integration observation until S4. |
| Expected Markers | One immutable selected command representation, one owner/disposition for every record/guest/return edge, and no `cmdExec32` or fabricated completion in the DOS path. |
| Asset Needs | Existing staged `mvdm/system32/COMMAND.COM`, `NTIO.SYS` and `NTDOS.SYS`, plus the formal CPU40/x86 product graph; no copied or rewritten guest payload. |
| Reporting Requirements | State whether a source change was necessary; identify exact remaining historical product-shell exclusions; distinguish code/source closure from runtime evidence. |
| Stop Conditions | Need to alter guest media/source, create an alternate command protocol/executor, make a family-private mapper, depend on ambient input, recreate CSRSS/kernel behavior, or address a non-DOS host-child path pauses for new S admission. |
| Exit Criteria | The selected DOS command cohort has one source-attributed, locally testable normal and failure direction or an exact original unavailable terminal; all touched mirror divergences and adapter rows are reviewed. |
| Original Owner Request | “准入” — execute the next queue task in single-person dual-role mode while keeping mirror changes minimal and adapter logic outside mirrors. |
| Similar-Issue Sweep | Base VDM delivery/consume, command record capacities, `CMDINFO` layout, `$Exec`, PSP/arena/JFN/environment, DEM entry notification, redirection result, command return/re-entry, SoftPC outer return and controlled termination. |

**S2 closure:** [Selected-command recovery disposition](etc/evidence/m0-t356-s2-selected-command-recovery-001.md) selects the already packaged `COMMAND.COM /C EXIT` DOS `.COM` workload.  The four-rung review found no missing source binding and introduced no shim or mirror change; code/source closure remains distinct from S3/S4 verification.

### M0 T356 S3 — Closed local and formal selected-cohort closure

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T356 S3; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | T356's approved plan admits the selected-cohort closure after S2's no-new-shim disposition. |
| Objective | Verify the selected existing Base VDM → guest COMMAND/NTDOS contract in a focused x86 local fixture, then produce a fresh formal CPU40/x86 link containing its source owners. |
| Non-goals | No product behavior change, integration observation, source/media alteration, BOP repair, host child execution, guest execution claim, CPU/device change, CPU30, Bochs, x64 gate, CSRSS/kernel/WOW recreation or unrelated test cleanup. |
| Reference Baseline | S1 topology audit; S2 selected-command disposition; current formal CPU40/x86 graph and existing Base VDM fixture source. |
| Files And ABI Surface | Existing focused Base VDM fixture/build graph and formal CPU40/x86 build graph. No production source/ABI modification is authorized by S3. |
| Applicable Rules | Execution, source-first recovery, CPU40-only, mirror/overlay, build-output hygiene, architecture and coding rules. |
| Verification | A fresh task-owned x86 fixture build/run for record normal/reject/capacity/wait directions; a fresh task-owned formal CPU40/x86 Ninja link; inspect selected source manifest and actual outputs. |
| Expected Markers | Fixture prints its pass marker; formal graph selects `cmdmisc.c`, `cmdexec.c`, `demmisc.c`, Base VDM and original SoftPC composition and links the product. |
| Asset Needs | MSVC x86, Ninja and existing selected source/media files. No runtime container is used in S3. |
| Reporting Requirements | Report fixtures and formal link separately; neither may be presented as a successful guest execution or controlled return. |
| Stop Conditions | A fixture needs synthetic guest CPU/PSP result, a build needs product-source changes, an unrelated compile error, or an execution observation beyond local fixture scope pauses for new admission. |
| Exit Criteria | Fresh focused fixture and formal CPU40/x86 product link pass, their source selection is reviewed, and no unsupported runtime claim is made. |
| Original Owner Request | “准入” — execute the next queue task in single-person dual-role mode while keeping mirror changes minimal and adapter logic outside mirrors. |
| Similar-Issue Sweep | Base VDM record normal/reject/capacity/wait, current-directory and re-entry records, command application/tail layout, formal selected owner object set and CPU40-only profile selection. |

**S3 closure:** [Local and formal closure evidence](etc/evidence/m0-t356-s3-local-and-formal-closure-001.md) records a passing task-owned x86 Base VDM fixture and a fresh 428-action CPU40/x86 formal link.  Neither is guest-execution evidence.

### M0 T356 S4 — Closed fixed-container selected DOS command observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T356 S4; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | This is the final observation step explicitly contained in the admitted T356 plan, following S3's positive local and formal closure. |
| Objective | Make exactly one fixed, non-debug, console-owning observation of the fresh CPU40/x86 product with the already selected immutable `--command EXIT` Base VDM declaration. |
| Non-goals | No retry with changed inputs, no debugger, breakpoint, guest-memory read, command/media/source mutation, trace-selected repair, host-child surrogate, BOP/CPU/device change, CPU30, Bochs, x64, CSRSS/kernel/WOW recreation or graphics work. |
| Reference Baseline | S1/S2 source disposition; S3 fresh linked `original-softpc-process.exe`; fixed staged root `O:\ntvdm` containing the selected MVDM media. |
| Files And ABI Surface | Existing `tests/observation/console_startup_observer.c` and its task-owned output only. The observer has no production ABI or guest-state write surface. |
| Applicable Rules | Execution, source-first recovery, CPU40-only, build-output hygiene, fixed-container observation and architecture rules. |
| Verification | Compile the existing observer in x86, invoke it once with fresh product path, fixed root and task-owned report base; retain only concise reviewed evidence. |
| Expected Markers | One exit or bounded timeout report, console snapshot and any existing default-off source reports; one source-defined subsequent owner disposition only. |
| Asset Needs | Fresh S3 product executable, unchanged `O:\ntvdm\mvdm` media and existing observer source; no new runtime asset. |
| Reporting Requirements | Report exact command, SHA-256/product identity, result and reached source marker if any. A timeout must remain a limitation, never a repair selector or execution claim. |
| Stop Conditions | Need to vary root/media/command/timeout/observer, attach a debugger, add a new report hook, or repair a trace-selected leaf pauses this task for renewed admission. |
| Exit Criteria | One fixed observation is recorded with its exact result and next source-owner disposition; T356 closes only if it proves the controlled route or records an exact source-shaped transfer/terminal. |
| Original Owner Request | “准入” — execute the next queue task in single-person dual-role mode while keeping mirror changes minimal and adapter logic outside mirrors. |
| Similar-Issue Sweep | Product identity, command declaration, package root, console ownership, watchdog disposition, Base VDM wait, command record consumption, NTDOS EXEC/parent return and source report markers. |

**Plan:** [M0 T356 minimal DOS guest execution and controlled return](etc/operations/m0-t356-minimal-dos-execution-controlled-return-plan-001.md).

**S4 closure:** [fixed command observation](etc/evidence/m0-t356-s4-fixed-command-observation-001.md)
records one non-timeout `0x41` exit.  It source-attributes that exit to the
app's image-relative package-media admission, before original SoftPC or guest
execution.  It does not claim that `COMMAND.COM`, NTDOS `EXEC`, or `EXIT` ran.

### M0 T356 — Closed by exact package-stage transfer

T356 is source and formal-build closed for its selected original COMMAND/NTDOS
cohort, but it did not establish its requested guest execution/controlled
return. Its single fixed observation instead proved a prior app package-stage
terminal: the fresh formal executable was not placed beside its immutable
`mvdm` media. The functional objective is retained, in dependency order, by
the [runtime recovery program](etc/operations/proposal-ntvdm-runtime-recovery-program-20260901.md),
beginning with staged CPU40 boot. No trace-selected source repair is
authorized by this closure.

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S35; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S34's successful debugger-boundary continuation under the standing owner direction for ordered source-first CPU40/x86 recovery. |
| Objective | Audit the complete original COMMAND command-acquisition, wait, resume and lifecycle cohort reached after S34, including `cmdGetNextCmd`, `GetNextVDMCommand`, Base VDM client/broker bindings, console/worker state and original ordinary failure/continuation results. Select a whole source-owner recovery or an exact unchanged terminal. |
| Non-goals | No individual trace-hit BOP patch, command fabrication, guest/media/firmware mutation, CPU/CCPU/SAS/device/timer change, mapper redesign, CPU30, Bochs, x64, BaseSrv/CSRSS reconstruction, WOW, EXEC, graphics or retry. |
| Reference Baseline | S34 fixed observation continues beyond `DBGDispatch` and reaches repeated original COMMAND `54:01` activity before the fixed watchdog. |
| Files And ABI Surface | Original `mvdm-host/dos/command`, Base VDM client/broker adapter and session command source/broker as evidence only. No product ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original definition/caller/order audit, selected graph/import review, documentation governance/export and diff review. No runtime execution or production behavior change. |
| Expected Markers | One package ledger distinguishes original local command acquisition from BaseSrv/CSR broker requirements, explains the observed continuation/wait shape, and selects one bounded owner package or exact terminal. |
| Asset Needs | Existing original mirrors, source indexes and S34 observation only; no new source import, guest rebuild, host-system mutation or new fixture. |
| Reporting Requirements | Treat the trace as ordering evidence only; record all member entrypoints and lifecycle calls before selecting a recovery. |
| Stop Conditions | A need to fabricate a command, change guest COMMAND/DOS semantics, create a BaseSrv/CSR broker, or vary the fixed container pauses for a new admission. |
| Exit Criteria | One source ledger covers the full reached COMMAND acquisition/wait cohort and names one source-shaped successor without claiming end-to-end command execution. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `cmdGetNextCmd`, `cmdGetNextCmdInfo`, `GetNextVDMCommand`, `BaseCheckVDM`, `ExitVDM`, Base VDM broker/client, `cmdExec32`, command re-entry, console worker and session command source. |

### M0 T355 S34 — Closed original ordinary-debugger dispatch recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S34; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S33's complete source package audit under the standing owner direction for ordered source-first CPU40/x86 recovery. |
| Objective | Recover the complete original ordinary non-debug `DBGDispatch` disposition: shared debug-state, bounded source-order `SS:SP` frame read, `DBG_TOOLHELP` registration, no-event paths and the original unknown-mode `AX=0` result; leave the actual NT4 debugger/CSR event transport unavailable. |
| Non-goals | No debugger broker, `STATUS_VDM_EVENT` substitute, synthetic debugger reply, new BOP-specific success, guest/media/firmware mutation beyond the original ToolHelp state read, CPU/CCPU/SAS/device/timer change, mapper redesign, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC, graphics or retry. |
| Reference Baseline | S32 exact `debugger:DBGDispatch` terminal and S33's original-mode/source/caller audit. |
| Files And ABI Surface | Existing debugger adapter sources, shared adapter-local state and existing adapter-softpc guest-memory/effective-address interfaces. Original BOP selector, CCPU registers and guest frame ABI remain unchanged. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original branch/order review; formal CPU40/x86 link; one unchanged fixed-container observation; documentation governance/export and diff review. |
| Expected Markers | Met: `DBG_TOOLHELP` no longer reaches `ERROR_CALL_NOT_IMPLEMENTED`; unavailable private debugger transport remains distinguishable and does not falsely succeed. |
| Asset Needs | Existing original mirrors, formal graph, fixed `O:\\ntvdm` stage and observer; no new source import, guest rebuild or host-system mutation. |
| Reporting Requirements | Record original mode/caller evidence, guest-frame bounds and lease lifetime, non-debug versus debugged outcomes, and the explicit remaining transport boundary. |
| Stop Conditions | A need to create a debugger broker, recover CSRSS/CSR, alter guest frame layout, change CPU semantics, or vary the execution container beyond the one fixed observation pauses for a new admission. |
| Exit Criteria | Met: original ordinary non-debug dispatch outcome is source-shaped and formally verified; exactly one fixed observation passes the former `DBGDispatch` terminal and reaches later COMMAND/DEM activity; debugger/CSR transport remains explicitly unavailable. See [S34 evidence](etc/evidence/m0-t355-s34-original-ordinary-debugger-dispatch-recovery-001.md). |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `MS_bop_6`, `DBGDispatch`, `DBG_TOOLHELP`, `DBG_ATTACH`, unknown modes, `ModuleLoad`, `ModuleSegmentMove`, `ModuleFree`, `DbgPrompt`, `DBGNotifyDebugged`, `SendVDMEvent`, guest-memory lease and CCPU `SS:SP` conversion. |

### M0 T355 S33 — Closed original debugger dispatch package audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S33; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S32's exact source attribution under the standing owner direction for ordered source-first CPU40/x86 recovery. |
| Objective | Audit the complete original `mvdm-host/dbg` dispatch package reached through `MS_bop_6 -> DBGDispatch`: source ownership, selected functions, VDM-frame/SoftPC dependencies, debugger/CSR transport boundary, and original success/failure directions. Produce one whole-package recovery disposition before changing any debugger behavior. |
| Non-goals | No debugger provider, synthetic reply, BOP-specific success, guest/firmware/media mutation, CPU/CCPU/SAS/device/timer change, mapper redesign, CPU30, Bochs, x64, BaseSrv/CSRSS reconstruction, WOW, EXEC, graphics or retry. |
| Reference Baseline | S32 fixed observation: `0x00000078` is `debugger:DBGDispatch`, reached from original `MS_bop_6`. |
| Files And ABI Surface | Original `mvdm-host/dbg` and its selected OpenNT declarations/callers; existing debugger, adapter-softpc, adapter-win32 and session boundaries as evidence only. No product ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Definition/caller/import audit across selected original source baselines; formal graph membership review; documentation governance/export and diff review. No runtime execution and no production behavior change. |
| Expected Markers | Met: the package ledger separates source-shaped non-debug branches from private debugger/CSR transport and selects S34's bounded recovery. |
| Asset Needs | Existing original mirrors and source indexes only; no new source import, guest rebuild, host-system mutation or new fixture. |
| Reporting Requirements | Distinguish the original `DBGDispatch` frame contract from its individual debugger modes; identify static dependencies rather than inferring behavior from the single runtime terminal. |
| Stop Conditions | A need to change `DBGDispatch`, add any debugger transport, revive CSRSS/CSR, alter a BOP result, or vary the fixed execution container pauses for a new admission. |
| Exit Criteria | Met: one source ledger covers the reached debugger package, establishes the ordinary non-debug contract and admits S34 without claiming debugger-product recovery. See [S33 evidence](etc/evidence/m0-t355-s33-original-debugger-dispatch-package-audit-001.md). |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `MS_bop_6`, `DBGDispatch`, `ModuleLoad`, `ModuleSegmentMove`, `ModuleFree`, `DbgPrompt`, VDM-stack frame access, `DbgInit`, `NtRaiseHardError`, debugger/CSR imports and session controlled-stop provenance. |

### M0 T355 S32 — Closed controlled-stop provenance observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S32; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S31's complete static attribution ledger under the standing owner direction for ordered source-first CPU40/x86 recovery. |
| Objective | Add a default-off, source-entry provenance record at the existing controlled session-termination boundary and use one unchanged fixed-container observation to identify the actual `0x78` caller. |
| Non-goals | No BOP/DEM/COMMAND result fabrication; no guest/media/firmware mutation; no CPU/CCPU/SAS/device/timer behavior change; no source-provider replacement, mapper change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC, graphics or retry. |
| Reference Baseline | S30 successful disposal result `0x78`; S31 proves it is a non-local controlled stop with several static `ERROR_CALL_NOT_IMPLEMENTED` candidates. |
| Files And ABI Surface | Existing session termination bridge and the selected adapter terminal callers only; the record carries copied source ID and completion code and introduces no guest or public ABI. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Source/ordering review; default-off report-path inspection; formal CPU40/x86 link; one unchanged fixed-container observation; documentation governance/export and diff review. |
| Expected Markers | Met: the fixed result names `debugger:DBGDispatch` plus `120`; without the explicit report-path environment variable, the terminal writes no record and preserves the prior completion path. |
| Asset Needs | Existing original mirrors, formal graph, fixed `O:\\ntvdm` stage and observer; no new source import, guest rebuild or host-system mutation. |
| Reporting Requirements | Distinguish selected source-entry identity from the generic completion code; prove that writing the record happens before the existing `longjmp` without modifying the selected terminal. |
| Stop Conditions | A need to change the caller's source result, to recover the identified provider before attribution, to add a second diagnostic class, or to alter guest/machine state pauses for a new admission. |
| Exit Criteria | Met: exactly one fixed observation attributes `0x78` to original `MS_bop_6 -> DBGDispatch` and selects the whole debugger dispatch cohort; no inferred repair is implemented in this S. See [S32 evidence](etc/evidence/m0-t355-s32-controlled-stop-provenance-observation-001.md). |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `VdmExitCode`, original `host_terminate`, all `session_terminate_current(120)` callers, unavailable adapters, observer report isolation and ordinary-return/exception reporters. |

### M0 T355 S29 — Closed original heartbeat shutdown reachability audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S29; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S28's source-attributed `HeartBeatThread` successor under the standing owner direction for ordered source-first CPU40/x86 recovery. |
| Objective | Reconstruct the complete original `host_start_cpu → host_applClose → TerminateHeartBeat → NtAlertThread → alertable wait → ExitThread` route and identify the smallest source-shaped reason the observed heartbeat binding remains live. |
| Non-goals | No new timer model, BOP/DEM/COMMAND/Redirector business change, guest/media/firmware mutation, CCPU execution change, mapper redesign, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW or graphics work. |
| Reference Baseline | S28 proves the sole live worker is original `HeartBeatThread`; `main.c` nominally invokes `host_applClose` after `host_start_cpu`, and the selected timer source already has same-shaped public alert/wait bindings. |
| Files And ABI Surface | Original `main.c`, `nt_reset.c`, `nt_timer.c`, selected thread/alert/wait adapters, existing session report and app composition. No new guest or public product ABI. |
| Applicable Rules | Execution, source policy, source-first recovery, CPU40-only, mirror/overlay, mapping-manager, architecture and coding rules. |
| Verification | Original definition/caller/return-path audit; selected build-preprocessor/link inspection; one fixed-container observation. |
| Expected Markers | A source-owned explanation whether original `host_applClose` is reached, whether `TerminateHeartBeat` reaches its existing alert binding, and one bounded implementation successor or an exact terminal. |
| Asset Needs | Existing selected mirrors, S28 product/link evidence and source indexes only; no asset import, rebuild or host mutation. |
| Reporting Requirements | Separate original-source control flow, selected build binding and runtime fact. State explicitly that static reachability does not prove an alert was delivered. |
| Stop Conditions | Any need to insert generic timer cancellation, force-unbind, add a parallel worker, retry/vary the container, alter guest/media, or change a CPU/BOP/machine semantic pauses for new admission. |
| Exit Criteria | Met: the direct `ExitProcess(0)` replacement is source-recovered but was not the observed exit route. Original `base/bios/reset.c` can call `host_terminate()` directly; NT4 relied on process exit for worker destruction. The next complete cohort is idempotent `host_applClose` before every selected `host_terminate`. See `etc/evidence/m0-t355-s29-original-heartbeat-shutdown-reachability-audit-001.md`. |

### M0 T355 S30 — Closed source-owned direct-termination cleanup recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S30; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S29's complete original source reachability audit under the standing owner direction for ordered source-first CPU40/x86 recovery. |
| Objective | Recover the original process-exit cleanup contract for every selected direct `host_terminate()` route by making original `host_applClose()` a once-per-host-start cohort and invoking it before session termination. |
| Non-goals | No new timer model, force-unbind, parallel worker, BOP/DEM/COMMAND/Redirector business change, guest/media/firmware mutation, CCPU execution change, mapper redesign, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW or graphics work. |
| Reference Baseline | S28 attributes the live worker to original `HeartBeatThread`; S29 proves direct reset can reach `host_terminate()` without `terminate()` while NT4's `ExitProcess` implicitly destroyed workers. |
| Files And ABI Surface | Original `softpc.new/host/src/nt_reset.c`, existing timer/alert/wait adapters, session termination boundary and fixed-container observer. No guest or public product ABI changes. |
| Applicable Rules | Execution, source policy, source-first recovery, CPU40-only, mirror/overlay, architecture and coding rules. |
| Verification | Formal CPU40/x86 Ninja link and one fixed-container disposal observation. The app returns `0x48` only when disposal finds a retained binding; the observed original-entry result was `0x78` and no disposal report was emitted, proving the binding count reached zero. |
| Stop Conditions | Any need to force-unbind, add a generic cancellation scheme, vary the container, alter guest/media, or change CPU/BOP/machine semantics pauses for new admission. |
| Exit Criteria | Met: the source-owned closure is called at most once per original host start on both `terminate()` and direct `host_terminate()` routes; formal build passed; one fixed observation returned the original-entry `0x78`, not app disposal `0x48`, and emitted no disposal failure report; mirror divergences are registered. See [S30 evidence](etc/evidence/m0-t355-s30-source-owned-direct-termination-cleanup-recovery-001.md). |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `host_main`, `host_applClose`, `TerminateHeartBeat`, `NtAlertThread`, `WaitForSingleObjectEx`, `DelayHeartBeat`, `ExitThread`, outer SEH and app disposal ordering. |

### M0 T355 S28 — Closed original-worker source attribution

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S28; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S27's source-owned successor under the standing owner direction for ordered source-first CPU40/x86 recovery. |
| Objective | Attribute the live original-worker session binding to its exact source worker before selecting a source-owned termination recovery. |
| Non-goals | No BOP, DEM, COMMAND, Redirector request/pipe, guest/media/firmware, CCPU/SoftPC, mapping-manager, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, graphics or host-system change. |
| Reference Baseline | S26 attributes the sole disposal binding to an original worker; S27 proves its `ExitThread` path is not reached in the fixed startup observation.  S28's formal link audit disproves the preliminary Redirector hypothesis: `vrnmpipe.obj` is not linked by the formal product, so its worker cannot be the observed binding. |
| Files And ABI Surface | Existing generic original-worker thread facade, session-local diagnostic snapshot and app default-off disposal report. The observation carries a bounded source spelling only; no guest register, PDB, native worker handle or pointer crosses a boundary. |
| Applicable Rules | Execution, source policy, source-first recovery, CPU40-only, mirror/overlay, mapping-manager, architecture and coding rules. |
| Verification | Formal CPU40/x86 final-link selection review; focused source-to-facade label review; one fixed `O:\\ntvdm` observation with the default-off disposal report; governance/export and diff review. |
| Expected Markers | The prior `worker=1` report includes the exact source worker spelling, or records an exact attribution impossibility without changing worker lifetime. |
| Asset Needs | Existing selected source mirrors, formal CPU40/x86 graph, immutable `O:\\ntvdm` stage and fixed observer; no new source import, guest/media mutation or host-system mutation. |
| Reporting Requirements | Separate formal link-selection fact, source/facade binding fact and single runtime result. Explicitly state that source attribution is not worker termination or broader guest continuity. |
| Stop Conditions | Any need to fabricate a guest PDB/register, force-unbind a live worker, change worker/Redirector business behavior, alter guest/media/firmware, retry/vary the fixed container, or modify a machine/BOP semantics pauses for new admission. |
| Exit Criteria | The observed live worker is source-attributed without a lifetime change, or an exact bounded attribution impossibility is recorded and transfers a complete owner cohort. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `cmdCreateProcess`, COMMAND pipe workers, Redirector workers, generic thread facade, session binding categories, app disposal report and final-link object selection. |

**S28 closure:** [original-worker source attribution](etc/evidence/m0-t355-s28-original-worker-source-attribution-001.md)
disproves the unlinked Redirector hypothesis and attributes the existing live
worker binding to original SoftPC `HeartBeatThread`.  It selects the complete
timer termination cohort without changing that worker's lifetime.

### M0 T355 S1 — Closed short-path permanent COMMAND fixed-container verification

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S1; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Owner explicitly admitted the queue head with “准入” after T354's closed original package-path admission enforcement. |
| Objective | Create one owner-approved disposable output package at `O:\ntvdm`, validate its unchanged formal product/media identity against the original NTDOS `commnd` contract, then make exactly one fixed-container observation of the original permanent `COMMAND.COM` bootstrap. |
| Non-goals | No use of an existing host directory, drive alias, `subst`, junction, media mutation after staging, guest/firmware modification, DEM/BOP special case, mapper, CPU/machine alteration, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T354 closure; unchanged original `cmdconf.c` and NTDOS `commnd` contract; formal CPU40/x86 product; current long workspace path is expressly inadmissible. |
| Files And ABI Surface | No production source change is expected. The selected package's executable/media manifest, existing app package-layout admission, original COMMAND/DEM bootstrap and fixed observer report are evidence surfaces only. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Record the output path and hashes; use app's exact path admission; verify unchanged executable/media identity; formal graph manifest review; run one non-debug fixed-container observation only after admission succeeds; governance/export and diff review. |
| Expected Markers | Admission succeeds; original generated `shell=` value is within 63 visible bytes; one durable report attributes the permanent command image open/handoff or one further original terminal. |
| Asset Needs | The owner-approved empty `O:\ntvdm` output root, existing formally linked CPU40/x86 executable and selected immutable staged media. The standard staging script creates the disposable package once and refuses to overwrite it. |
| Reporting Requirements | Record the exact output path, public short spelling, computed original shell length, executable/media hashes, one run outcome and any source-owned successor; distinguish environment admission from guest execution. |
| Stop Conditions | An occupied output root, a path exceeding the original limit, any request to mutate media after staging, a second run, timeout-driven repair, or a need to alter a source/ABI pauses for a new admission. |
| Exit Criteria | One valid immutable package has passed app admission and produced exactly one fixed-container observation, or the supplied asset is rejected with exact source-compatible evidence. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `GetShortPathNameA`, app media-root derivation, original `cmdconf.c` format, NTDOS `commnd`, package staging manifest, COMMAND/DEM open result and observer-container identity. |

**S1 closure:** [short-path permanent COMMAND observation](etc/evidence/m0-t355-s1-short-path-permanent-command-observation-001.md)
proves that the package-path admission succeeds at `O:\ntvdm`, but the one
unchanged observation still opens original default `C:\COMMAND.COM`. This
selects the original NTDOS multi-pass `SHELL` persistence cohort, not a path
projection or DEM repair.

### M0 T355 S2 — Closed original CONFIG multi-pass `SHELL` persistence audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S2; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S1's single valid short-path observation under the owner-approved ordered queue execution. |
| Objective | Reconstruct the complete original `doconf -> organize -> multi_pass -> trys -> commnd -> dfil` cohort and identify why a valid source-generated `shell=` value did not replace the default permanent command pathname. |
| Non-goals | No second runtime observation, path projection, app/package change, guest/firmware change, DEM/BOP repair, mapper, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T354 source path-contract closure; T355 S1 valid `O:\ntvdm` package evidence with a 60-character generated shell value, valid transformed config file, and unchanged observed `C:\COMMAND.COM` failure. |
| Files And ABI Surface | Original `sysinit1.asm`, `sysinit2.asm`, `sysconf.asm`, original `cmdconf.c` and exact selected guest binaries/maps only. No production ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original definition/caller/data-lifetime audit; original-versus-mirror and source-versus-staged identity review; bounded binary/map symbol review; existing S1 evidence review; governance/export and diff review. No build or runtime execution. |
| Expected Markers | An owner ledger identifies each pass, `multi_pass_id` state, configuration-buffer lifetime, `SHELL` parser reachability, `commnd` write/read locations, and one bounded source-shaped next disposition. |
| Asset Needs | Existing original mirrors, maps, staged manifest and S1 durable reports; no new media, source import or host mutation. |
| Reporting Requirements | Separate source control-flow facts, selected binary identity and runtime facts; explain all plausible conditions by which `trys` is skipped, overwritten or not reached; do not infer a repair solely from the exception code. |
| Stop Conditions | Need for a new observation, guest memory/state recorder, source/ABI change, third-party media, DEM/BOP result adjustment or an unowned machine semantic pauses for new admission. |
| Exit Criteria | The full reached multi-pass cohort has owner/disposition evidence and selects exactly one minimal source-shaped next S or records an exact source terminal; no unowned branch remains in the selected explanation. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `config`, `confbot`, `org_count`, `multi_pass_id`, `organize`, `trys`, `commnd`, `command_line`, `dfil`, NTDOS load/map identity and source-defined configuration pass ordering. |

**S2 closure:** [CONFIG multi-pass `SHELL` persistence audit](etc/evidence/m0-t355-s2-config-multipass-shell-persistence-audit-001.md)
proves that source-defined pass one must reach `trys` and replace `commnd`.
It selects a single source-positioned, state-neutral `ConfigDone` observation;
it does not authorize a path, DEM, parser, or machine repair.

### M0 T355 S3 — Closed `ConfigDone` state-neutral `commnd` observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S3; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S2's complete original control/data-flow audit under the owner-approved ordered queue execution. |
| Objective | Add one default-off observation at original NTDOS `ConfigDone` / `BOP_NOTIFICATION(5Eh, AL=0)` that copies the selected `NTIO.SYS` `multi_pass_id` and bounded `commnd` state after CONFIG multi-pass processing but before permanent-command `dfil`. |
| Non-goals | No parser/`trys`/`dfil` rewrite, guest or firmware mutation, DEM/BOP result adjustment, pathname projection, new mapper, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S1's sole valid short-root observation; T355 S2's original `doconf -> organize -> multi_pass -> trys -> commnd -> dfil` source ledger; exact selected NTIO binary/map identity. |
| Files And ABI Surface | Existing original `MS_bop_E` notification path, exact NTIO map symbols, existing session checked guest-memory lease and existing default-off durable observer records. No guest pointer, host handle or public BOP ABI escapes the call. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Source/map-to-binary identity review; lease lifetime review; focused positive/negative copied-record proof; formal CPU40/x86 link; exactly one fixed-container observation; governance/export and diff review. |
| Expected Markers | A copied `AL=0` notification record containing `multi_pass_id`, a bounded NUL-terminated `commnd` snapshot and selected NTIO identity, followed by unchanged original notification handling. |
| Asset Needs | Existing original mirrors, maps, CPU40/x86 formal graph and the immutable `O:\ntvdm` staged package; no new source/media import or host-system mutation. |
| Reporting Requirements | Separate source boundary, binary/map identity and observed bytes; record maximum copied bytes, lease scope, notification code and whether original `MS_bop_E` ran; do not interpret an exception as a repair. |
| Stop Conditions | Any need for a second observer input variant, retained guest pointer, guest/CPU/BOP/DEM state change, map inference without exact selected identity, or a second result recorder pauses for a new admission. |
| Exit Criteria | One valid copied notification observation or a source-backed inability result classifies `commnd` as unwritten, incorrectly written, or correctly written before later loss/context; it selects exactly one next whole recovery cohort. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `BOP_NOTIFICATION`, `MS_bop_E`, `UMBNotify`, `demDasdInit`, `ConfigDone`, `multi_pass_id`, `commnd`, `command_line`, selected `NTIO.SYS` map addresses, checked lease and durable observation lifetime. |

**S3 closure:** [ConfigDone observation relocation attribution](etc/evidence/m0-t355-s3-configdone-observation-relocation-attribution-001.md)
proves that the first recorder read map-file locations without the original
`NTIO_LOAD_SEGMENT` base. Its `commnd` bytes are therefore non-evidence; no
guest conclusion, BOP repair or parser change is authorized.

### M0 T355 S4 — Closed loaded-NTIO `ConfigDone` state observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S4; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S3's source-backed map-to-loader attribution under the owner-approved ordered queue execution. |
| Objective | Correct only the default-off `ConfigDone` recorder's selected-map-to-loaded-NTIO address translation by using the already-live original `MS_bop_E` `CS`, then obtain one copied `multi_pass_id`/`commnd` observation in the fixed container. |
| Non-goals | No `doconf`/`organize`/`multi_pass`/`trys`/`dfil` change; no guest or firmware mutation, DEM/BOP result adjustment, path projection, mapper, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S2 source ledger; T355 S3 proof that map `009D:*` is a file-layout identity while original `nt_msscs.c` loads the file at `0070:0000`; unchanged selected NTIO/media identity. |
| Files And ABI Surface | Existing original `MS_bop_E` `getCS()` state, selected map offsets, existing session checked guest-memory leases and default-off durable observer. No public BOP or guest pointer ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original loader/map/current-CS arithmetic review; focused default-off and copied-read fixture; formal CPU40/x86 link; exactly one new fixed-container observation; governance/export and diff review. |
| Expected Markers | A copied `AL=0` record that includes the live `CS`, map offsets, `multi_pass_id` and a bounded `commnd` value from the loaded selected NTIO image, followed by unchanged original notification handling. |
| Asset Needs | Existing mirrors, maps, CPU40/x86 formal graph and immutable `O:\ntvdm` staged media; no source/media import or host-system mutation. |
| Reporting Requirements | State map-file versus loaded-image arithmetic, exact segment/offsets and lease scope; distinguish an unwritten shell from a recorder inability; do not infer a repair from timeout. |
| Stop Conditions | Any need for a second input variant, retained guest pointer, changed guest/CPU/BOP/DEM state, generic map resolver, or additional state recorder pauses for a new admission. |
| Exit Criteria | One corrected copied observation classifies whether `trys` wrote a valid `commnd` before `dfil` and selects exactly one next whole recovery cohort. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `NTIO_LOAD_SEGMENT`, `getCS`, `ConfigDone`, `BOP_NOTIFICATION`, map-file offsets, `multi_pass_id`, `commnd`, synchronous read leases, durable observer and staged binary identity. |

**S4 closure:** [loaded-NTIO ConfigDone state observation](etc/evidence/m0-t355-s4-loaded-ntio-configdone-state-observation-001.md)
proves that valid generated configuration text has not replaced the original
default `commnd` by the source-defined end of CONFIG processing. The next
owner is NTDOS/CPU execution of the `organize -> multi_pass -> trys` cohort,
not DEM or package layout.

### M0 T355 S5 — Active NTDOS `SHELL` pass-one execution cohort audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S5; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S4's valid loaded-state observation under the owner-approved ordered queue execution. |
| Objective | Reconstruct the original in-guest `organize -> confbot/org_count -> multi_pass_id=1 -> ordinary dispatch -> trys` execution cohort and the selected CPU40/SoftPC instruction/data contracts required to carry a generated `SHELL` record to `commnd`. |
| Non-goals | No guest/parser/firmware modification or rebuild, no new runtime observation, DEM/BOP result adjustment, pathname projection, mapper, CPU/machine semantic patch, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S2 source ledger; T355 S4 proof that original loaded `commnd` remains `\\COMMAND.COM` at `ConfigDone` after pass two; selected CPU40/x86 binary/map identity. |
| Files And ABI Surface | Original `sysinit1.asm`, `sysinit2.asm`, `sysconf.asm`, selected NTIO map/binary and reached CPU40 instruction/memory primitives only. No production ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original definition/caller/branch/data-lifetime walk; selected source-versus-binary/map review; reached CPU40 instruction/segment/string/stack contract review; governance/export and diff review. No build or runtime execution. |
| Expected Markers | A source-owned ledger names every required `S` record byte/layout, the pass-one branch sequence, the CCPU mechanical primitives that preserve it, and one bounded next owner/disposition. |
| Asset Needs | Existing original mirrors, selected binaries/maps, T355 durable records and CPU40 source/formal graph; no source import, media mutation or host-system mutation. |
| Reporting Requirements | Separate original guest source facts, selected binary facts, CPU40 mechanical facts and runtime facts; distinguish a parser skip from a memory/segment/string execution mismatch without inferring from timeout. |
| Stop Conditions | Any need to alter guest bytes, add a general instruction tracer, change CPU semantics, rerun runtime, fabricate a host result, or cross into unrelated BOP owners pauses for new admission. |
| Exit Criteria | The complete pass-one `SHELL` execution cohort has no unowned immediate edge and selects exactly one source-shaped next S or a precise source terminal. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `comtab`, `cmd_indicator`, `org_file`, `org_count`, `confbot`, `getchr`, `chrptr`, `count`, `multi_pass_id`, `tryi`, `tryb`, `trys`, `getCS`, `DS/ES`, string operations and selected CPU40 memory/segment primitives. |

**S5 closure:** [NTDOS `SHELL` pass-one execution cohort audit](etc/evidence/m0-t355-s5-ntdos-shell-pass-one-execution-cohort-audit-001.md)
proves the original source route is complete through `trys`, but static evidence
cannot distinguish its live input/reorganization/CPU execution predecessor.

### M0 T355 S6 — Active original NTDOS `SHELL` pass-one execution-position observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S6; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S5's source-complete static cohort audit under the owner-approved ordered queue execution. |
| Objective | Add the narrowest default-off, selector-blind CPU40 observation sufficient to classify original NTDOS `organize`, ordinary pass-one dispatch and `trys` reachability in the fixed `O:\\ntvdm` container. |
| Non-goals | No guest/parser/firmware change or rebuild, CPU semantic repair, DEM/BOP result adjustment, pathname projection, mapper, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S4 copied loaded-state result and T355 S5 original source/map/CPU40 cohort audit. |
| Files And ABI Surface | Selected CPU40 instruction-position facility, original NTDOS map/binary identity, existing synchronous copied guest-memory leases and durable observer only. No guest pointer, host handle or public BOP ABI escapes. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Source/map/binary address attribution; focused default-off and copied-record tests; formal CPU40/x86 link; exactly one fixed-container observation; governance/export and diff review. |
| Expected Markers | Bounded copied markers distinguish `organize` entry, pass-one ordinary dispatch, `trys` entry and any `commnd` write before ConfigDone without changing original control flow. |
| Asset Needs | Existing selected mirrors, map/binary, CPU40/x86 formal graph and immutable `O:\\ntvdm` staged media; no source/media import or host-system mutation. |
| Reporting Requirements | Record exact map/binary address attribution, live CS/IP basis, copied-byte bounds, lease lifetime and the one observation outcome; distinguish absent marker from a parser repair. |
| Stop Conditions | Any need for general tracing, retained guest pointer, guest/CPU state change, second input/container, instruction semantic modification, additional runtime run or cross-owner repair pauses for a new admission. |
| Exit Criteria | One valid source-positioned observation identifies the earliest absent/present configuration transition and selects exactly one source-owned recovery cohort or exact terminal. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `organize`, `comtab`, `org_file`, `multi_pass`, `tryi`, `tryb`, `trys`, `commnd`, CPU40 CS/IP, DS/ES, SAS reads/writes and durable observer lifetime. |

**S6 closure:** [CONFIG command first-store observation](etc/evidence/m0-t355-s6-config-command-first-store-observation-001.md)
proves the selected `commnd` location receives an original zero store, but the
first-write-only diagnostic cannot determine whether a later `trys` store
occurred. It selects only the bounded same-address sequence correction.

### M0 T355 S7 — Active bounded original NTDOS `commnd` write-sequence observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S7; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S6's single valid first-store result under the owner-approved ordered queue execution. |
| Objective | Correct the default-off selected-address observer to retain at most eight post-store address/value records, then make one fixed-container observation to determine whether original `trys` writes `S` to `commnd`. |
| Non-goals | No guest/parser/firmware modification or rebuild, general instruction tracing, CPU semantic repair, DEM/BOP result adjustment, pathname projection, mapper, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S5 original pass-one cohort audit; T355 S6 first store at the exact selected `commnd` linear address. |
| Files And ABI Surface | Existing `c_sas_store` one-call observer seam, existing adapter report helper, fixed observer environment and existing formal CPU40/x86 product only. No guest pointer, mapping, CPU-control or public ABI change. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Default-off and bounded positive fixture; formal CPU40/x86 link; exactly one fixed-container observation; source-order review; governance/export and diff review. |
| Expected Markers | Ordered, bounded copied values at `0x914e6`; an `S` entry proves `trys` reached its original first filename store, while its absence selects a preceding original branch/data owner. |
| Asset Needs | Existing selected source/map/binary, formal graph and immutable `O:\\ntvdm` container; no import, media or host-system change. |
| Reporting Requirements | Record fixed address derivation, maximum records, original-write ordering, fixture bounds and the single observation; distinguish observer correction from any guest repair. |
| Stop Conditions | More than eight records, a generic trace facility, guest/CPU state change, address inference beyond the selected identity, second input/container, or a second runtime observation pauses for a new admission. |
| Exit Criteria | One valid bounded sequence determines whether the original `trys` first store occurred and selects exactly one source-owned recovery cohort or exact predecessor terminal. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `commnd` initialization, `trys` first filename store, `command_line`, `c_sas_store`, selected CPU40 linear address, report gating and sequence bounds. |

**S7 closure:** [CONFIG command write-sequence observation](etc/evidence/m0-t355-s7-config-command-write-sequence-observation-001.md)
proves that no original `trys` store occurred. Its first unresolved predecessor
is the original `doconf` full-read equality branch.

### M0 T355 S8 — Active original CONFIG temporary-file DEM read-result observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S8; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S7's source-backed absence of a `trys` store under the owner-approved ordered queue execution. |
| Objective | Add one default-off observation at the original DEM `demRead` success/failure boundary and make one fixed-container observation sufficient to establish the original `doconf` `CX == AX` full-read precondition for the configuration temporary file. |
| Non-goals | No DEM result/path rewrite, parser/firmware change or rebuild, CPU semantic repair, guest/media mutation, mapper, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S7 source proof that original `trys` did not execute; original `sysconf.asm::doconf` open/seek/read/`AX == CX -> getcom` contract; existing temporary-file open records. |
| Files And ABI Surface | Original `dos/dem/demhndl.c::demRead`, existing scalar CPU accessors and default-off durable report helper only. No guest pointer, host handle, read buffer or BOP routing ABI escapes. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original source-order review; focused default-off/scalar report fixture; formal CPU40/x86 link; exactly one fixed-container observation; governance/export and diff review. |
| Expected Markers | Existing `DS:DX`, requested `CX`, file-position `BX:SI`, completed count, carry/error state and original success/failure phase distinguish a complete config-file read from an earlier source terminal. |
| Asset Needs | Existing selected mirrors, formal graph and immutable `O:\\ntvdm` container; no new import, guest asset or host-system mutation. |
| Reporting Requirements | Record only copied scalar results after the original operation; identify the temporary-file transaction by source-order correlation with existing open records; never copy file content or retain guest/host pointers. |
| Stop Conditions | Any need to alter a result, inspect/copy guest payload, special-case a pathname, add another observer family, use a second input/container, or make a second runtime observation pauses for a new admission. |
| Exit Criteria | One valid read-result observation classifies the original `AX == CX` precondition and selects exactly one source-owned predecessor/repair cohort or exact source terminal. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `doconf`, `open/lseek/read/close`, `demRead`, `ReadFile`, `AX/BP`, `CX`, `BX/SI`, `DS/DX`, carry/error, `getcom`, `conferr` and durable report lifetime. |

**S8 closure:** [CONFIG temporary-file DEM-read observation](etc/evidence/m0-t355-s8-config-temporary-dem-read-observation-001.md)
proves that the original temporary-file `demOpen` succeeds but the selected
execution does not reach `demRead`.  It does not establish a short read or
authorize a DEM repair; the remaining original predecessor is the post-open
seek/allocation/pre-read cohort.

### M0 T355 S9 — Active original CONFIG post-open seek/pre-read cohort attribution

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S9; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S8's single fixed-container negative `demRead` result under the owner-approved ordered queue execution. |
| Objective | Reconstruct the original `doconf` sequence after successful temporary CONFIG open through seek-to-end, `confbot` allocation, seek-to-start and the call boundary to DOS read; select one source-owned next seam without changing a result. |
| Non-goals | No DEM/seek/read result rewrite, parser or firmware change/rebuild, CPU semantic repair, guest/media mutation, mapper, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S7's absent `trys` store; T355 S8's successful temporary `demOpen` and absent `demRead`; original `sysconf.asm::doconf` source order. |
| Files And ABI Surface | Original `sysconf.asm`, DOS file-pointer service and original DEM `demChgFilePtr` boundary; no ABI change in this attribution S. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original source/caller/register audit and existing evidence review only.  A new observer, build or runtime run requires a separately bounded S10 admission. |
| Expected Markers | One source-owned immediate predecessor or exact source terminal between the successful open and `demRead`, including its registers, carry/error continuation and owner. |
| Asset Needs | Existing mirrors and T355 durable reports; no new source import, asset, build or host-system mutation. |
| Reporting Requirements | Separate source facts from S8 runtime facts; retain the conclusion that no `demRead` was reached and do not infer a repair from the timeout. |
| Stop Conditions | Any need to add an observer, execute a second runtime observation, alter a result, copy guest content, special-case a pathname or change CPU/machine semantics pauses for a new admission. |
| Exit Criteria | The whole original post-open/pre-read cohort has one bounded successor seam or an exact source terminal, with no unowned immediate edge. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `doconf`, DOS `42h` seek, `demChgFilePtr`, SFT/JFN handle binding, `confbot`, DOS allocation, `CX/AX`, carry/error and `conferr`. |

**S9 closure:** [CONFIG post-open seek/pre-read cohort attribution](etc/evidence/m0-t355-s9-config-post-open-seek-cohort-attribution-001.md)
proves that the first `AH=42h/AL=2` EOF seek is the unique host-service
predecessor of the absent read; the reset seek is guest-local.  The next
bounded seam is the original `demChgFilePtr` result, not a parser or DEM-read
repair.

### M0 T355 S10 — Active original CONFIG EOF-seek result observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S10; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S9's complete original post-open source attribution under the owner-approved ordered queue execution. |
| Objective | Add one default-off, scalar-only observer at original `demChgFilePtr` and make one fixed-container observation sufficient to classify the `doconf` EOF-seek success/failure before CONFIG read. |
| Non-goals | No file-pointer/DEM/parser result rewrite, guest or media mutation, mapper, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S8 successful temporary open with absent read; T355 S9 original `AH=42h/AL=2 -> LSeekEOF -> SVC_DEMCHGFILEPTR` proof. |
| Files And ABI Surface | Original `dos/dem/demhndl.c::demChgFilePtr`, existing scalar CPU accessors and default-off durable report helper only. No guest pointer, host handle or file payload escapes. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original source-order review; focused default-off scalar report fixture; formal CPU40/x86 link; exactly one fixed-container observation; governance/export and diff review. |
| Expected Markers | Original `CX:DX`, `BL`, `CF`, `AX:DX` and phase identify whether the EOF seek reaches the read/guest-local reset cohort or its exact error continuation. |
| Asset Needs | Existing selected mirrors, formal graph and immutable `O:\ntvdm` container; no new import, guest asset or host-system mutation. |
| Reporting Requirements | Record only copied scalar results after the original operation; correlate the transaction to S8's temporary-file open by source order; never copy file content or retain guest/host pointers. |
| Stop Conditions | Any need to alter a result, inspect/copy guest payload, special-case a pathname, add another observer family, use a second input/container or make a second runtime observation pauses for a new admission. |
| Exit Criteria | One valid seek-result observation selects exactly one source-owned successor or source terminal without a DEM leaf repair. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `doconf`, DOS `42h`, `LSeekEOF`, `SVC_DEMCHGFILEPTR`, `SetFilePointer`, `AX/BP`, `CX/DX`, `BL`, `CF`, `AX:DX`, `confbot` and `conferr`. |

**S10 closure:** [CONFIG EOF-seek observation](etc/evidence/m0-t355-s10-config-eof-seek-observation-001.md)
proves original `SVC_DEMCHGFILEPTR` succeeds and returns `0x0A47`.  The
remaining pre-read interval is original guest-only `count/config_size`,
`pararound`, `confbot` and `tempcds` preparation; no host file-service repair
is selected.

### M0 T355 S11 — Active original CONFIG post-seek guest preparation audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S11; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S10's successful original EOF-seek observation under the owner-approved ordered queue execution. |
| Objective | Reconstruct the complete original guest-only `doconf` sequence from successful EOF seek through `count/config_size`, `pararound`, `confbot` relocation and `tempcds` to the DOS read call; identify one exact predecessor or source terminal. |
| Non-goals | No DEM/parser/firmware result rewrite or rebuild, guest/media mutation, mapper, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S8 successful temporary open/absent read; T355 S10 successful `SVC_DEMCHGFILEPTR` EOF seek; original `sysconf.asm::doconf`. |
| Files And ABI Surface | Original NTDOS CONFIG code and its immediate original helpers only. No adapter, DEM or BOP ABI change in S11. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original declaration/definition/caller/register/memory-span audit and existing evidence review only. A runtime observer or product change requires a separately bounded successor S. |
| Expected Markers | A complete source-owned account of every post-seek instruction group, its memory preconditions and its direct transfer to read or original terminal. |
| Asset Needs | Existing original mirrors and T355 durable evidence; no new source import, asset, build or host-system mutation. |
| Reporting Requirements | Keep S10's proved success separate from unobserved guest state; do not infer a repair from the timeout or add a diagnostic during the audit. |
| Stop Conditions | Any need to alter guest memory, add an observer, run a second fixed-container execution, change a file-service result or CPU/machine semantics pauses for a new admission. |
| Exit Criteria | The entire post-seek/pre-read guest cohort has no unowned immediate edge and selects one bounded successor seam or exact original terminal. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `doconf`, `count`, `config_size`, `confbot`, `pararound`, `tempcds`, CDS relocation, memory bounds, `DS/ES`, `AH=3Fh`, `CX`, `AX` and `conferr`. |

**S11 closure:** [CONFIG post-seek guest-preparation audit](etc/evidence/m0-t355-s11-config-post-seek-guest-preparation-audit-001.md)
proves that `pararound` and relocation are guest-local, but original
`tempcds` calls `AH=44h/AL=08` removable-media IOCTL before the read.  The next
source-owned seam is the whole existing `SVC_DEMIOCTL` owner path.

### M0 T355 S12 — Active original CONFIG temporary-CDS IOCTL observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S12; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S11's original `tempcds` source-boundary audit under the owner-approved ordered queue execution. |
| Objective | Add one default-off scalar observation at original `demIOCTL`/`demIoctlChangeable` and make one fixed-container observation sufficient to classify the CONFIG temporary-CDS removable-media request before read. |
| Non-goals | No IOCTL/drive-policy result rewrite, parser/firmware change, guest/media mutation, mapper, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S10 successful EOF seek; T355 S11 original `tempcds -> AH=4408 -> SVC_DEMIOCTL` route proof; T355 S8 absent read. |
| Files And ABI Surface | Original `dos/dem/demioctl.c`, existing scalar CPU accessors and a default-off durable report helper only. No guest pointer, host drive path or data payload escapes. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original source-order review; focused default-off scalar report fixture; formal CPU40/x86 link; exactly one fixed-container observation; governance/export and diff review. |
| Expected Markers | Original subfunction, drive number, result `AX/DX` and carry classify whether temporary-CDS preparation reaches its guest-local continuation or exact error path. |
| Asset Needs | Existing selected mirrors, formal graph and immutable `O:\ntvdm` container; no new import, guest asset or host-system mutation. |
| Reporting Requirements | Record only copied scalar results at original owner boundaries; do not infer a repair from timeout or inspect host-drive contents. |
| Stop Conditions | Any need to alter a result, special-case a drive/path, inspect guest payload, use another container or make a second runtime observation pauses for a new admission. |
| Exit Criteria | One valid IOCTL observation selects exactly one source-owned successor or terminal without a fabricated host capability result. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `tempcds`, `AH=44h`, `AL=08`, `ioctl_removable_media`, `SVC_DEMIOCTL`, `demIOCTL`, `demIoctlChangeable`, `BL`, `AX`, `DX`, carry and fixed-drive selection. |

**S12 closure:** [CONFIG temporary-CDS IOCTL observation](etc/evidence/m0-t355-s12-config-temporary-cds-ioctl-observation-001.md)
proves three original removable-media requests for drive `C:` return fixed
(`AX=1`, `CF=0`).  DEM open, EOF seek and temporary-CDS IOCTL are all source
and runtime-successful; no host-drive workaround is selected.

### M0 T355 S13 — Closed original CONFIG post-IOCTL guest-continuation audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S13; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T355 S12's successful whole original IOCTL owner result under the owner-approved ordered queue execution. |
| Objective | Reconstruct original `tempcds` return, temporary CDS write loop, `doconf` `DS/ES/confbot/CX` setup and exact DOS-read transfer; identify the exact original read owner after the preceding successful host services. |
| Non-goals | No new host capability, DEM/IOCTL/parser result rewrite, guest/media mutation, mapper, CPU/machine change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | T355 S8 successful open/absent slow read; T355 S10 successful EOF seek; T355 S12 successful original removable-media IOCTL results. |
| Files And ABI Surface | Original `sysinit1.asm::tempcds`, `sysconf.asm::doconf`, original DOS INT21 read dispatcher and selected CPU40 execution semantics. No code change in S13. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original source/caller/register/memory-span audit and existing evidence review only. A new diagnostic, build or runtime run requires a separately bounded S14 admission. |
| Expected Markers | A complete source-owned continuation from successful IOCTL through `tempcds` return and the `AH=3Fh` transfer, with one exact next seam or original terminal. |
| Asset Needs | Existing original mirrors and T355 durable evidence; no new source import, asset, build or host-system mutation. |
| Reporting Requirements | Treat the three S12 successes as bounded facts only; do not infer that `tempcds` returned or that DOS read was dispatched without source evidence. |
| Stop Conditions | Any need to add an observer, execute a second run, alter guest state, rewrite host results or change CPU/machine semantics pauses for a new admission. |
| Exit Criteria | The post-IOCTL/pre-read continuation has no unowned immediate edge and selects one bounded successor or exact original terminal. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `tempcds`, `foogo`, `fixed_drv`, `EndProc TempCDS`, `confbot`, `alloclim`, `DS/ES`, `CX`, `AH=3Fh`, DOS read dispatcher, `demRead` and CPU40 return/control state. |

**S13 closure:** [CONFIG fast-read route correction](etc/evidence/m0-t355-s13-config-fastread-route-correction-001.md)
supersedes the predecessor inference that no `demRead` meant no DOS read.
The observed `50:42` is the reached original kernel-fast-I/O seam.

### M0 T355 S14 — Active original kernel FastRead composition recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S14; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S13's complete source correction under the standing owner direction to execute the active packet. |
| Objective | Restore the original `SVC_DEMFASTREAD` normal-file result contract for the CPU40/x86 user-mode composition from `base/ntos/vdm/x86/rdwr.c::NTFastDOSIO`, without importing NT4 kernel VDM. |
| Non-goals | No kernel VDM/CSRSS/IRQL/PVDM_TIB recreation, guest/media mutation, parser/IOCTL/seek rewrite, generic BOP work, CPU/machine semantic change, CPU30, Bochs, x64 runtime, pipe/console fast I/O, EXEC or graphics claim. |
| Reference Baseline | S13 source correction; unchanged original `handle.asm::$READ`, `demRead` fallback and session mapping-manager contracts; fixed `O:\ntvdm` media. |
| Files And ABI Surface | Original `demdisp.c` dispatch selection, a bounded adapter-owned kernel-fast-I/O compatibility seam if required, existing host-resource mapping and synchronous guest-memory lease. No guest ABI shape change. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original kernel/user-source comparison; focused normal-file, EOF and fallback/error contract fixtures; formal CPU40/x86 link and one unchanged fixed-container observation only after the provider proof passes; governance/export and diff review. |
| Expected Markers | `50:42` transfers bytes and returns their count/CF contract for a normal mapped file, or returns CF set for a case delegated to original `50:16`; no raw host handle/pointer crosses the boundary. |
| Asset Needs | Existing mirrors, session mapping instances, formal graph and immutable stage; no new source import, kernel component or host-system mutation. |
| Reporting Requirements | Separate kernel-source contract, user-mode replacement boundary and observed result; record every deliberate non-kernel disposition, especially console/std-handle, pipe and seek behavior. |
| Stop Conditions | Any need for a raw native handle/pointer in DOS state, retained guest pointer beyond a checked lease, kernel API/IRQL/TEB recreation, a family-private mapper, guest rewrite or a second run pauses for new admission. |
| Exit Criteria | The selected normal-file FastRead contract is source-attributed, mapping-safe and formally linked; if its one fixed-container observation is preempted before `50:42`, it transfers the exact earlier owner without claiming runtime completion. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `SVC_DEMFASTREAD`, `SVC_DEMFASTWRITE`, `$READ`, `FastOrSlow`, `demRead`, mapped host resource identity, guest-memory lease, EOF, console/std handles, pipes, seek/ZF and `MS_bop_0` dispatch. |

**S14 P1:** [FastRead user-mode recovery and reachability limit](etc/evidence/m0-t355-s14-fastread-recovery-p1-001.md)
is retained as an auditable superseded composition attempt: direct selection of
`demRead` does not preserve `NTFastDOSIO`'s fast-failure-to-slow-read contract.

**S14 P2:** the active correction restores that contract in a mirror-relative
overlay: safe regular-file reads complete through one bounded mapping lease;
every decline or fast-read failure sets only `CF=1` for unchanged DOS `$READ`
to continue through `50:16`. The existing one fixed-container run remains
preempted before `50:42` by the separately owned C-VID SAS-vector condition;
it is not a FastRead result claim.

**S14 closure:** the corrected `50:42` provider is source-attributed,
mapping-safe and formally linked. The sole admitted runtime observation was
preempted before this service at the exact C-VID null-vector owner, so S14
transfers that machine prerequisite without a FastRead runtime claim.

### M0 T355 S15 — Active original CCPU/C-VID SAS-vector composition recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S15; ordinary mode (single-person dual-role implementation). |
| Objective | Bind only the reached C-VID `Sas.Sas_overwrite_memory` vector entry to its existing original CPU40 provider, then formally verify the product closure. |
| Non-goals | No other vector completion, cache behavior, guest-memory change, BOP/DEM result change, CPU profile change, guest/media mutation or new runtime observation. |
| Reference Baseline | S14 prerequisite audit; original `sascdef.c`, `ccpusas4.c`, `sas4gen.h` and `accessfn.c`; selected CPU40/x86 graph. |
| Files And ABI Surface | Existing C-VID overlay vector-composition point and original fixed-width `TYPE_sas_overwrite_memory` contract only. |
| Verification | Declaration/provider/initializer/caller review; formal CPU40/x86 product link; documentation governance and diff review. |
| Exit Criteria | The sole reached slot is non-null and points to the original CCPU40 provider; all other unavailable slots are unchanged and the formal product links. |

**S15 closure:** [C-VID SAS overwrite-vector binding](etc/evidence/m0-t355-s15-cvidc-sas-overwrite-vector-binding-001.md)
binds only the reached `Sas.Sas_overwrite_memory` member to the exact original
CPU40 no-cache-invalidation provider.  The formal CPU40/x86 product remains
linked; no new runtime observation or unrelated vector completion is claimed.

### M0 T355 S16 — Active fixed-container C-VID prerequisite observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S16; ordinary mode (single-person dual-role implementation). |
| Objective | Replace only the formal product executable in the immutable `O:\\ntvdm` package and perform one console-owning, non-debug observation to establish whether the earlier reached C-VID null-vector exception is gone. |
| Non-goals | No second execution, BOP leaf repair, guest/media/firmware mutation, vector completion beyond S15, mapper or CPU semantic change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC, graphics or continuity claim. |
| Reference Baseline | S14's sole pre-`50:42` C-VID null-slot exception, S15's one-slot original-provider binding, unchanged `O:\\ntvdm` media manifest and existing console observer. |
| Files And ABI Surface | Formal `original-softpc-process.exe`, immutable stage manifest/media, existing observer and durable report only. No production source/ABI change. |
| Verification | Verify the staged media manifest before launch; one bounded observer run; preserve its report and copied exception/BOP markers; governance and diff review. |
| Exit Criteria | One valid fixed-container result distinguishes removal of the exact earlier null-vector terminal from a successor owner or bounded timeout. It neither retries nor repairs the successor. |

**S16 closure:** [fixed-container C-VID prerequisite observation](etc/evidence/m0-t355-s16-cvidc-prerequisite-observation-001.md)
proves the S15 null-vector terminal is gone and that original `50:42` is now
reached.  The one allowed run transfers the succeeding access violation to the
original CPU40 `ccpusas4.c::_phyR` physical-read owner; it does not authorize a
retry or a repair in this packet.

### M0 T355 S17 — Active original CCPU SAS physical-memory lifecycle audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S17; ordinary mode (single-person dual-role implementation). |
| Objective | Reconstruct the reached original `config -> sas_term -> sas_init -> host_sas_init -> InitIntelMemory -> c_GetPhyAdd -> phyR` lifecycle and identify the exact source-owned reason that a physical read targeted the formally allocated M-area base. |
| Non-goals | No BOP/DEM repair, synthetic memory, unconditional slow path, guest/firmware mutation, mapping-manager redesign, CPU profile change, vector completion, new runtime run, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | S16's one fixed-container exception at link address `0x00409651`; original CPU40 `ccpusas4.c`, `sim32.c`, `nt_mem.c`, `config.c` and selected startup composition. |
| Files And ABI Surface | Original SAS/CCPU memory lifecycle and existing physical-mapping adapter only. S17 is a source/definition/caller audit and does not change production ABI. |
| Verification | Original definition/initializer/caller/lifetime walk, source-versus-mirror/overlay review, formal-map attribution and documentation governance/diff review. No runtime execution. |
| Exit Criteria | Every selected lifecycle handoff has an owner and an original, binding-only, adapter-backed, overlay-required or exact unavailable disposition; S17 selects at most one source-shaped repair cohort or records an exact terminal. |

**S17 closure:** [CCPU SAS physical-memory lifecycle audit](etc/evidence/m0-t355-s17-ccpu-sas-physical-memory-lifecycle-audit-001.md)
proves that the S16 `_phyR` access is the original `Start_of_M_area + addr`
RAM fallback, not an external mapping or an EMS alias.  Static source cannot
identify the Intel address/lifecycle state at the faulting access; it admits
the bounded S18 scalar observation only.

### M0 T355 S18 — Closed original CCPU SAS direct-RAM state observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S18; ordinary mode (single-person dual-role implementation). |
| Objective | Add one default-off, state-neutral record at the existing original CCPU `c_GetPhyAdd` normal-RAM seam, then make one fixed-container observation sufficient to distinguish invalid Intel address, uncommitted selected span, or invalidated SAS backing. |
| Non-goals | No BOP/DEM repair, synthetic memory, guest/firmware mutation, memory commit/decommit change, mapping-manager redesign, CPU semantics/profile change, vector completion, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | S16 exact `_phyR` exception and S17 source proof that no external physical mapping is active on the selected startup path. |
| Files And ABI Surface | Existing original `ccpusas4.c::c_GetPhyAdd`, one default-off observer binding and existing child-only fixed-container reporter. The record contains copied scalar values only; no guest pointer or host HANDLE crosses an ABI. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Original call/source order, focused observer positive/negative review, formal CPU40/x86 link, exactly one fixed-container observation, documentation governance/export and diff review. |
| Expected Markers | The first direct-RAM access that reaches the observed fault target reports Intel address, M-area base, span, wrap mask, and alias/resolver dispositions before the existing source exception or a source-owned successor. |
| Asset Needs | Existing immutable `O:\\ntvdm` package, formal CPU40/x86 product and fixed observer only. No source/media import or host-system mutation. |
| Reporting Requirements | Distinguish source fact from runtime scalar fact; state whether the direct address is in range and whether the target derives from the current M-area base. Do not infer a repair from an unclassified exception. |
| Stop Conditions | Any need to change memory allocation/commit behavior, provide a synthetic physical page, alter CPU/BOP result, record a pointer, retry, or broaden to another device pauses for a separate admission. |
| Exit Criteria | One valid scalar observation attributes the physical-access branch and selects at most one original-compatible repair cohort or exact terminal. |

**S18 closure:** [CCPU SAS direct-RAM state observation](etc/evidence/m0-t355-s18-ccpu-sas-direct-ram-state-observation-001.md)
proves that the original normal-RAM fallback requested physical address
`0x00110000`, inside the reserved 16 MiB M-area, and derived the fault target
`0x039e0000` from its current base.  The reached address is just beyond the
initial conventional/A20 commit range; the sole successor owner is original
XMS allocation/commit lifecycle, not CCPU, C-VID, mapping, EMS or `50:42`.

### M0 T355 S19 — Closed original XMS allocation/commit lifecycle audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S19; ordinary mode (single-person dual-role implementation). |
| Objective | Reconstruct the complete original CPU40 startup path from `scs_init -> XMSInit -> SAInitialize -> commit callback` and its selected `InitIntelMemory` commit contract; determine the smallest original-compatible binding that makes allocated XMS address `0x00110000` accessible without changing CCPU fallback semantics. |
| Non-goals | No CCPU/`c_GetPhyAdd` workaround, eager full-span commit, synthetic physical mapping, BOP/DEM result change, guest/firmware mutation, mapper redesign, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or runtime run. |
| Reference Baseline | S18 scalar evidence; original `softpc.new/host/src/{nt_msscs.c,nt_mem.c,sim32.c}`, `xms.486/{xms.c,xmsblock.c}`, `suballoc/suballoc.c`, the session lease contract, and selected CPU40/x86 formal graph. |
| Files And ABI Surface | Original `XMSInit`, `SAInitialize` callback shape, original `VdmAllocateVirtualMemory`/`VdmCommitVirtualMemory`, and the selected adapter `xmsMoveMemory` transfer seam.  S19 starts as source/caller/build-selection audit only. |
| Verification | Original definition/caller/conditional-compilation walk; original-versus-mirror/overlay comparison; formal graph/source selection review; documentation governance and diff review. No runtime execution in S19 admission. |
| Exit Criteria | Every lifecycle edge has a source owner and final disposition; the selected successor is an original interface binding or a documented exact terminal.  No commitment behavior changes until a separately bounded recovery S is admitted. |

**S19 closure:** [XMS allocation/commit lifecycle audit](etc/evidence/m0-t355-s19-xms-allocation-commit-lifecycle-audit-001.md)
corrects S18's callback attribution.  The selected session `xmsCommitBlock`
acquires a read-before-write lease and reaches CCPU before the requested XMS
backing can be cleared.  The original non-`i386` `XMSInit` allocation/commit
branch is the sole S20 recovery candidate.

### M0 T355 S20 — Closed original XMS backing-lifecycle selection recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S20; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted from S19's source-defined original binding selection under the standing owner direction to continue the active SoftPC package. |
| Objective | Restore the already mirrored original CPU40/x86 non-`i386` `XMSInit -> VdmAllocateVirtualMemory -> VdmCommitVirtualMemory` backing lifecycle by removing only the formal `MVDM_XMS_SESSION_BACKEND` build selection. |
| Non-goals | No CCPU fallback change, eager M-area commit, synthetic physical mapping, XMS body rewrite, mapping-manager redesign, BOP/DEM `50:42` behavior change, guest/firmware mutation, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or runtime execution. |
| Reference Baseline | S17/S18 direct-RAM evidence and S19 source/graph audit; original `xms.c`, `suballoc.c`, and `nt_mem.c`; formal CPU40/x86 Ninja graph. |
| Files And ABI Surface | `tools/build/New-T310OriginalSoftpcNinja.ps1` build selection only. Original XMS and SoftPC source bodies remain unchanged; `xmsMoveMemory` remains the existing mapping-manager-bound transfer seam. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, mapping-manager, CPU40-only, architecture and coding rules. |
| Verification | Regenerate the formal x86 graph; verify no selected compile command defines `MVDM_XMS_SESSION_BACKEND`; verify original `xms.c` non-`i386` source branch and `nt_mem.c` commit callers; run formal Ninja compile/link; documentation governance and diff review. No runtime invocation. |
| Expected Markers | Generated x86 graph has no session-backend macro; original `VdmAllocateVirtualMemory`, `VdmCommitVirtualMemory`, and `VdmDeCommitVirtualMemory` are the selected XMS allocation callbacks; `xmsMoveMemory` remains mapped through the current adapter; final link passes without `/FORCE`. |
| Asset Needs | Existing mirrors, source-shaped adapter, formal Ninja generator, MSVC x86 toolchain and disposable `build/M0-T355/S20/formal-x86` root. |
| Reporting Requirements | Record macro/branch selection separately from compile/link output; state explicitly that no runtime continuity or `50:42` behavior claim follows from this S. |
| Stop Conditions | Any need to edit an original XMS/SoftPC function body, alter lease semantics, introduce raw host pointers, modify guest/firmware, add CCPU fallback behavior, or run a container observation pauses for owner re-admission. |
| Exit Criteria | One regenerated CPU40/x86 graph selects the original allocation/commit branch, the formal product compiles and links, and source/diff/governance evidence proves no unintended ownership change. |
| Original Owner Request | “正确修复不是‘强迫所有读都走慢速路径’，而是让 `50:42` 在现代用户态中恢复原始合同：能真正读则读；无法安全处理则返回 `CF=1`，让原始 DOS 自己走 `50:16`。” |
| Similar-Issue Sweep | `MVDM_XMS_SESSION_BACKEND`, `xmsCommitBlock`, `xmsDecommitBlock`, `xmsMoveMemory`, `VdmAllocateVirtualMemory`, `VdmCommitVirtualMemory`, `VdmDeCommitVirtualMemory`, `SAAllocate`, lease read-before-write order and `50:42` fast-read behavior. |

**S20 P1:** [original XMS backing-selection build](etc/evidence/m0-t355-s20-p1-original-xms-backing-selection-build-001.md)
regenerated the formal CPU40/x86 graph without
`MVDM_XMS_SESSION_BACKEND` and linked all 426 actions successfully.  The
original `VdmAllocateVirtualMemory`/`VdmCommitVirtualMemory` lifecycle is
selected; no runtime observation or `50:42` behavior claim is made.

**S20 closure:** the source-selected original XMS backing lifecycle is formally
compiled and linked.  Its next evidence is one fixed-container observation;
that observation may classify a new source owner but may not authorize an
in-run repair.

### M0 T355 S21 — Closed fixed-container original XMS allocation observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S21; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted from S20's formally linked source-selection closure under the standing owner direction to continue the active SoftPC package. |
| Objective | Make exactly one console-owning, non-debug fixed-container observation of the S20 x86 product and classify whether the first original XMS allocation now progresses beyond the prior uncommitted-direct-RAM terminal. |
| Non-goals | No source or ABI change, retry, alternate observer, timeout change, media/firmware change, guest mutation, `50:42` semantic change, CCPU workaround, eager allocation, mapper change, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics claim. |
| Reference Baseline | S16/S18 fixed-container reports, S19 source audit and S20 formal x86 linked product. The product executable is the sole mutable fixed-container member; media and observer contract remain fixed. |
| Files And ABI Surface | Existing `ObserveSoftpcStartup.mjs`, existing `console-startup-observer.exe`, existing `O:\ntvdm` media manifest, and S20 formal `original-softpc-process.exe`. No production source is changed. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Verify fixed media identity before launch; replace only the staged product through the existing observation script; make one 8-second fixed-container observation; retain exact report/json/exception excerpts; source/map classify only the reached terminal; governance/export and diff review. |
| Expected Markers | Either the prior `0x00110000` direct-RAM fault is absent and a later original owner is reached, or the exact terminal remains with current product identity. |
| Asset Needs | S20 x86 executable, current fixed `O:\ntvdm` package, existing observer and Node 22. |
| Reporting Requirements | Record product and fixed-media hashes, command/timeout identity, result and exact successor ownership. Distinguish an observed later boundary from continuous guest execution or `50:42` completion. |
| Stop Conditions | Any need for a second run, changed observer/container/media/timeout, source change, host mutation, raw guest/host pointer capture, or repair selected from the trace stops this S for separate admission. |
| Exit Criteria | One valid fixed-container observation has either disproved the former XMS-backing terminal or reproduced it with exact current identity and has transferred at most one source-owned successor. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | Fixed media hashes, observer command/timeout, staged product identity, `XMSInit`, `SAAllocate`, `VdmCommitVirtualMemory`, `c_GetPhyAdd`, first direct-RAM report, original exception report and reached BOP records. |

**S21 closure:** one immutable-container observation proves that the selected
original XMS allocation/commit lifecycle has progressed beyond the former
`0x00110000` direct-RAM terminal.  The later `0x48` process result is exactly
the app/session disposal-failure result, not an XMS or `50:42` result.  Its
specific invariant remains unobserved and transfers only to a separately
admitted app/session lifecycle audit: [S21 fixed-container evidence](etc/evidence/m0-t355-s21-original-xms-backing-fixed-container-observation-001.md).

### M0 T355 S22 — Active original return and session-disposal lifecycle audit

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S22; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted from the S21 source-classified successor under the standing owner direction to continue the active SoftPC package. |
| Objective | Audit the complete source-shaped transition from original `ntvdm.c -> host_main` return or original `ExitVDM` through the current execution bridge, app shell teardown, thread binding, termination escape and `session_dispose`; identify the sole owner of the observed `APP_STARTUP_DISPOSE_FAILURE` predicate. |
| Non-goals | No runtime execution, retry, trace-selected patch, BOP/DEM/COMMAND/XMS behavior change, CCPU workaround, guest/firmware mutation, mapper change, new asynchronous worker, CPU30, Bochs, x64, BaseSrv/CSRSS recreation, WOW, EXEC or graphics work. |
| Reference Baseline | S21 fixed-container evidence; original `softpc.new/obj.vdm/ntvdm.c`, `base/support/main.c`, DEM/COMMAND exit callers, imported Base VDM client/server forms; current app/session and SoftPC execution bridge. |
| Files And ABI Surface | Original `main`, `host_main`, `ExitVDM`, `demExitVDM`, `cmdExitVDM`; current `mvdm_softpc_execution_run_original_entry`, `session_thread_bind/unbind`, termination escape, `session_complete`, app shell teardown and `session_dispose`. |
| Verification | Definition/caller/return-path and static predicate walk; original-versus-current lifecycle comparison; build-selection review; documentation governance and diff review. No runtime invocation. |
| Expected Markers | Either one source-shaped lifecycle repair cohort is selected with original ordering preserved, or the exact unobservable predicate is recorded as an explicit later owner. |
| Asset Needs | Existing source mirrors, current formal graph and the immutable S21 evidence only. |
| Reporting Requirements | Distinguish original exit semantics, current app/session mechanics and runtime facts. Do not infer the individual disposal predicate from exit code 72 alone. |
| Stop Conditions | Any source change, new observer, retry, changed media/container, new generic lifecycle API, BaseSrv/CSRSS reconstruction or unreviewed source import requires a separate recovery admission. |
| Exit Criteria | Every original and current exit edge has an owner and ordering disposition; S22 transfers at most one source-defined recovery cohort or one explicit deferred boundary. |
| Similar-Issue Sweep | `host_main`, `ExitVDM`, `BaseSrvExitVDM`, `demExitVDM`, `cmdExitVDM`, `session_complete`, thread binding count, teardown order, termination escape and app process return. |

**S22 corrected closure:** [original return/session-disposal lifecycle audit](etc/evidence/m0-t355-s22-original-return-session-disposal-lifecycle-audit-001.md)
proves that exit status 72 alone cannot identify the disposal predicate and
that the original event/heartbeat workers do not bind the session.  It selects
no source change.

### M0 T355 S23 — Closed rejected console-event worker cohort

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S23; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted from S22's single original cleanup cohort under the standing owner direction to continue the active SoftPC package. |
| Objective | Test whether the original console-event worker is a source-owned cause of the app disposal terminal, and retain no change if its session-binding premise is false. |
| Non-goals | No event interpretation, console API redesign, BOP/DEM/COMMAND/XMS/CCPU behavior change, guest/firmware mutation, mapper/session-ABI redesign, generic worker framework, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW or EXEC work. |
| Reference Baseline | Original `softpc.new/host/src/{nt_event.c,nt_reset.c,nt_timer.c}`, S22 lifecycle audit, current same-shaped thread-start compatibility binding and S21 fixed-container evidence. |
| Files And ABI Surface | Original `nt_start_event_thread`, `nt_remove_event_thread`, `ConsoleEventThread`, original `CreateThread`, existing thread-start compatibility source and `session_dispose` predicate only. |
| Verification | Compare original creation/callback declarations and current include path; formal CPU40/x86 link; one unchanged-container observation of the temporary experiment; remove the experiment; documentation governance and diff review. |
| Expected Markers | Either the original event worker demonstrably increments `binding_count`, or source proves that it does not and the original mirror is restored. |
| Stop Conditions | Any need to alter event semantics, add a new worker, change timeout/container/media, touch guest or BOP state, change BaseSrv/CCPU behavior or retry a failed observation pauses for a new admission. |
| Exit Criteria | The source premise is accepted or rejected, every temporary change is either registered or removed, and the next diagnostic requirement is stated without claiming a lifecycle repair. |
| Similar-Issue Sweep | `nt_start_event_thread`, `nt_remove_event_thread`, `ConsoleEventThread`, `host_applClose`, `TerminateHeartBeat`, COMMAND/Redirector thread-start binding and `session_dispose`. |

**S23 closure:** [console-worker cohort rejection](etc/evidence/m0-t355-s23-console-worker-cohort-rejection-001.md)
proves that the direct original event worker does not bind `session`; its
temporary cleanup change was removed.  A disposal-predicate observer needs a
new admission before any further lifecycle work.

### M0 T355 S24 — Active session-disposal predicate observation

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S24; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted after S22/S23 proved that source reading and exit status `72` cannot distinguish the three existing `session_dispose` rejection predicates. The standing owner direction authorizes ordered recovery, source-first minimal adapters and one fixed-container observation. |
| Objective | Add one default-off, fixed-width app/session diagnostic which atomically records the already-existing reason that `session_dispose` rejects, then make exactly one unchanged-container observation. |
| Non-goals | No change to the return value, teardown order, session state, worker scheduling, BOP/DEM/COMMAND/XMS/CCPU behavior, guest/firmware/media, mapping-manager semantics, host mutation, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics work. |
| Reference Baseline | S21 proves original XMS lifecycle progress and records only app result `72`; S22/S23 prove event/heartbeat workers cannot explain binding count. `session_dispose` currently has exactly invalid-instance, nonzero-binding-count and armed-termination predicates. |
| Files And ABI Surface | `session/session.{h,c}` adds a source-owned fixed-width diagnostic result to the existing dispose operation; `app/entry.c` consumes it only after the unchanged failure result and writes an optional child-only report. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, CPU40-only, architecture and coding rules. |
| Verification | Review predicate ordering and original app result preservation; focused session/app build; formal CPU40/x86 graph/link; one fixed-container observation with only a report-path environment addition; documentation governance/export and diff review. |
| Expected Markers | An optional record identifies exactly one of `invalid`, `binding-count`, or `termination-armed` before app returns `72`; absence of the report path leaves output and behavior unchanged. |
| Asset Needs | Existing session/app source, selected S20 formal product, fixed `O:\\ntvdm` media/observer and Node 22; no new media or source import. |
| Reporting Requirements | Record result-code format, predicate order, source file/line ownership, product/media identity and one observation. Distinguish a diagnosis from a repair or guest-continuity claim. |
| Stop Conditions | Any need to change a predicate, add a lifecycle worker, take a second runtime observation, alter container/media/timeout, inspect guest data, or select a repair from an unclassified report pauses for a new admission. |
| Exit Criteria | One report or an explicit report-unavailable result identifies the existing predicate; no original MVDM mirror changes are retained; S24 transfers at most one complete original/source-owned lifecycle cohort. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | `session_valid`, `binding_count`, `termination_armed`, execution bridge normal/nonlocal return, COMMAND/Redirector thread wrappers, app finish path and optional diagnostic writers. |

**S24 closure:** [session-disposal predicate observation](etc/evidence/m0-t355-s24-session-disposal-predicate-observation-001.md)
adds the default-off fixed-width reason without changing app result `72` or
SoftPC/MVDM behavior.  Its one admitted container run is report-unavailable:
the existing observer ignores unknown `--environment` arguments and did not
pass the requested child variable.  No retry occurs in S24.

### M0 T355 S25 — Closed fixed-container diagnostic environment forwarding

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S25; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S24's source-proven report-unavailable result under the standing owner direction to continue the selected CPU40/x86 recovery without changing original MVDM behavior. |
| Objective | Add one explicit, allowlisted child-environment entry to the fixed-container observer so a default-off diagnostic report can reach the already-linked product without changing its arguments, media, current directory, console ownership or inherited environment otherwise. |
| Non-goals | No product source/ABI change, generic environment injection, second product configuration, BOP/DEM/COMMAND/XMS/CCPU behavior change, guest/firmware/media mutation, mapping change, host mutation, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW, EXEC or graphics work. |
| Reference Baseline | S24 proves the observer currently ignores unknown options and therefore invokes its child with no diagnostic variable. The formal CPU40/x86 product already contains the default-off app/session observer. |
| Files And ABI Surface | `tools/observation/ObserveSoftpcStartup.mjs` only; one allowlisted `MVDM_SESSION_DISPOSE_REPORT_PATH=<absolute path>` option is copied into the child environment and durable JSON metadata. |
| Applicable Rules | Execution, source policy, CPU40-only, build/output hygiene, architecture and coding rules. |
| Verification | Option-parser/allowlist and unchanged-command review; Node syntax/run review; one fixed-container observation with the selected report variable; documentation governance/export and diff review. |
| Expected Markers | Durable JSON names the allowlisted variable; the child returns unchanged `72` and writes one fixed-width disposal reason, or an explicit child report failure is recorded. |
| Asset Needs | Existing Node 22, observer, selected S24-linked product and unchanged `O:\\ntvdm` media/stage. |
| Reporting Requirements | Record the exact allowlist, child command/environment identity, product/media hashes, one result and report content. Do not call this a lifecycle repair. |
| Stop Conditions | Any request for an arbitrary variable, multiple entries, a changed product/media/container/timeout, a second run, source repair selected from the result, or guest/host-state inspection pauses for a new admission. |
| Exit Criteria | The allowlisted forwarder is source-reviewed and one newly admitted observation either yields a reason or records report-unavailable without retry; at most one whole source-owned successor is selected. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | Observer option parser, spawned child command, inherited environment, durable JSON, result report path and app's default-off report writer. |

**S25 closure:** [fixed-container diagnostic environment forwarding](etc/evidence/m0-t355-s25-fixed-container-diagnostic-environment-forwarding-001.md)
proves that the existing exit `72` is specifically the nonzero
`binding_count` predicate.  It selects a binding-lifetime ledger before any
worker or original-host lifecycle repair.

### M0 T355 S26 — Closed session binding-lifetime attribution

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S26; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted under the standing owner direction for ordered, source-first CPU40/x86 recovery after S25 identified the existing binding-count predicate. |
| Objective | Attribute the existing session binding count to its two source-owned entrances—outer SoftPC entry and original-worker thread facade—without changing lifecycle behavior, then make one fixed-container observation. |
| Non-goals | No worker scheduling, BOP/DEM/COMMAND/XMS behavior, guest/media/firmware, original SoftPC control-flow, mapping, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW or graphics change. |
| Reference Baseline | S25 fixed container reports `binding-count`; direct source audit identifies only `mvdm_softpc_execution` and `thread_start_compat` as bind entrances. |
| Files And ABI Surface | Project-owned `session` diagnostic state, the two existing adapter call sites, and app's default-off disposal report. No MVDM mirror body changes. |
| Applicable Rules | Execution, source policy, CPU40-only, mirror/overlay, mapping-manager, architecture and coding rules. |
| Verification | Definition/caller review, focused session contract, formal CPU40/x86 link, one unchanged-container observation, governance and diff checks. |
| Expected Markers | A fixed-width default-off report identifies the outstanding binding owner class or proves both categories balanced. |
| Asset Needs | Existing formal product, Node observer and immutable `O:\\ntvdm` stage; no new source/media import. |
| Reporting Requirements | Preserve the existing `72` and record only count/category values; distinguish attribution from repair. |
| Stop Conditions | A request to repair a proven owner, alter original worker exit or add a third binding entrance requires a new S admission. |
| Exit Criteria | One source-reviewed ledger and one observation attribute the remaining count to a complete owner cohort (or prove the diagnostic premise false) without a behavior repair. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | outer original-entry bridge, all `thread_start_compat` thunks, BaseVDM hook, original timer/event workers, and app disposal order. |

**S26 closure:** [session binding-lifetime attribution](etc/evidence/m0-t355-s26-session-binding-lifetime-attribution-001.md)
proves the residual count belongs exclusively to the original-worker thread
facade (`entry=0`, `worker=1`).  It transfers only the complete worker
termination cohort to a separately admitted repair S.

### M0 T355 S27 — Closed source-shaped worker binding release

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T355 S27; ordinary mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by S26's exclusive original-worker attribution under the standing owner direction for ordered source-first CPU40/x86 recovery. |
| Objective | Restore balanced session lifetime for every original worker routed through the existing thread facade, including source `ExitThread` paths, while preserving the original worker termination result. |
| Non-goals | No worker scheduling, BOP/DEM/COMMAND/Redirector business semantics, guest/media/firmware, original SoftPC execution, mapping, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW or graphics change. |
| Reference Baseline | S26 proves `worker=1` and `entry=0`; `cmdredir.c` source explicitly calls `ExitThread`, which bypasses an ordinary C thunk return. |
| Files And ABI Surface | Existing adapter-owned `thread_start_compat.{h,c}` plus affected mirror include binding only. No new thread ABI and no original algorithm rewrite. |
| Applicable Rules | Execution, source policy, CPU40-only, mirror/overlay, mapping-manager, architecture and coding rules. |
| Verification | Original worker exit/caller audit, focused adapter lifetime review, formal CPU40/x86 link, one fixed-container observation, governance and diff checks. |
| Expected Markers | A source `ExitThread` releases the current facade binding then retains its original thread exit code; fixed-container disposal no longer reports a worker binding. |
| Asset Needs | Existing formal graph and immutable `O:\\ntvdm` stage; no source/media import. |
| Reporting Requirements | Record every facade translation unit, original exit form, adapter release ordering and one observation; distinguish cleanup closure from guest-continuity. |
| Stop Conditions | Any need to alter a source worker's business ordering, introduce a new worker family, or repair a later guest/host terminal requires a new S. |
| Exit Criteria | All reached facade exit forms have one source-shaped release path, formal link passes, one observation removes the proven worker binding or names a new source-owned terminal. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。执行过程中，注意要保持镜像组件的最小修改复通，保持overlay最小，自主逻辑尽量放入adapter-*。” |
| Similar-Issue Sweep | COMMAND exec/pipe worker exits, Redirector DLC/named-pipe workers, wrapper return paths, `ExitThread` paths, BaseVDM hook and session dispose. |

**S27 closure:** [source-shaped worker binding release](etc/evidence/m0-t355-s27-source-shaped-worker-binding-release-001.md)
recovers every facade `ExitThread` release path without changing original
worker termination semantics.  The one observation proves the remaining
binding is a still-live source-shaped Redirector async worker, not an exit-path
leak.

### Indexed predecessor record — M0 T345 host capability expansion

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T345 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 6, admitted under the standing direction to execute ordered owner packages after T344 closure. |
| Objective | Establish the source/ABI/caller/failure inventory for original MVDM drive, filesystem, console, registry and device capabilities before selecting any one recoverable owner cohort. |
| Non-goals | No generic capability dispatcher, virtual filesystem, drive-policy change, host installation, registry mutation merely to run the product, CSRSS/kernel VDM recreation, guest/BOP change, CPU30, Bochs route or speculative device behavior. |
| Reference Baseline | T344 closure; current original MVDM DOS/SoftPC host mirrors; existing adapter-mvdm-host-out Win32, Redirector, monitor and SoftPC bindings; session mapping-manager and paired CPU40 formal graphs. |
| Files And ABI Surface | Original host capability source forms, public declarations, direct callers, existing adapters, public Win32 candidates, and source-defined permission/unavailable results. |
| Applicable Rules | Execution, source policy, source-first recovery, mapping-manager, mirror/overlay, architecture, coding and CPU40-only rules. |
| Verification | Complete selected capability source/declaration/definition/caller/failure walk, current-binding comparison, documentation governance and diff checks; no runtime behavior change in S1. |
| Expected Markers | Each reached capability edge has an original owner, a current disposition and an exact success/permission/unavailable contract; S1 selects at most one coherent lowest-dependency cohort. |
| Asset Needs | Existing original mirrors, formal graph, session and adapter sources; no host installation, guest/media import or system configuration change. |
| Reporting Requirements | Separate original source facts from current binding facts; preserve original owner terminology and record all deferred/hard-boundary transfers. |
| Stop Conditions | Any new generic provider, virtual drive, new mapper, raw host identity transport, guest change, host mutation or unreviewed source import requires S2 admission. |
| Exit Criteria | Every reached candidate capability has an owner/disposition and source-defined result; one bounded source-shaped cohort is selected or all items are transferred to named later owners. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | DEM/COMMAND file and drive paths, host COM/console, PIF/configuration, current-directory/registry, printer/serial/device endpoints, permission results, Redirector transfer, session mapping and teardown. |

**Plan:** [M0 T345 host capability expansion](etc/operations/m0-t345-host-capability-expansion-plan-001.md).

**S1 P1:** the owner ledger partitions configuration expansion, ordinary DEM
filesystem/drive/search, console/keyboard, registry, serial/printer, raw
media, Redirector and child-lifecycle forms. It keeps raw media, fullscreen,
Redirector and child/parent execution with their existing owners, leaving the
ordinary DEM filesystem/drive/search owner package as the only coherent
low-dependency candidate: [T345 capability ledger](etc/operations/m0-t345-s1-host-capability-owner-ledger.tsv).

**S1 P2:** the selected ordinary DEM cohort is confirmed as live original
mirrors, not a leftover self-written provider. Three files are byte-identical
to the OpenNT baseline; each remaining difference is registered and limited to
the immutable media root, the sole session mapping manager or a required
same-width ABI correction: [DEM mirror/binding ledger](etc/operations/m0-t345-s1-dem-host-capability-mirror-binding-ledger.tsv).

**S1 closure:** the inventory proves that `cmdconf.c` handles the selected
`CONFIG.NT` input rather than providing a missing-file fallback, retains DEM's
original result paths and transfers non-ordinary-device capability families to
their named owners: [host capability inventory closure](etc/evidence/m0-t345-s1-host-capability-inventory-closure-001.md).

### M0 T345 S2 — Original ordinary DEM host-capability recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T345 S2; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T345/S1's source-defined cohort selection under the standing direction to execute ordered owner packages. |
| Objective | Recover and formally close one complete original DEM cohort: ordinary host filesystem/handle, directory/drive/volume and search/FCB operations, through existing same-shaped Win32 and session-mapping bindings. |
| Non-goals | No generic provider, virtual filesystem/drive, readonly policy, raw DASD/IOCTL/media, serial/printer/pipe device, Redirector lifecycle, child/PSP lifecycle, new mapper, guest/BOP change, CPU30, Bochs route, host installation or system mutation. |
| Verification | Recheck original body and adapter seams package-wide; verify the selected formal CPU40 x86/x64 graphs and only current local contract coverage. No trace-selected leaf repair or broad runtime claim. |
| Exit Criteria | All selected files have one source-shaped binding and no parallel v1/self-written provider; local contract evidence covers the original success and failure ownership boundaries; paired formal graphs link the same cohort. |
| Stop Conditions | A needed new filesystem policy, raw identity transport, new mapping manager, non-ordinary device behavior, guest behavior or unreviewed source import requires re-admission. |

**S2 closure:** the complete ordinary DEM filesystem/handle, directory/drive
and search/FCB package retains its original bodies, has exactly one selected
mapping/Win32 binding route and is present in both formal CPU40 graphs:
[ordinary DEM recovery](etc/evidence/m0-t345-s2-ordinary-dem-host-capability-recovery-001.md).

### M0 T345 S3 — Ordinary DEM local-contract and paired formal closure (closed)

**Evidence:** [ordinary DEM formal closure](etc/evidence/m0-t345-s3-ordinary-dem-formal-closure-001.md).
**Closure:** [T345 host capability expansion](history/m0-t345-host-capability-expansion-closure-20260901.md).

### M0 T346 S1 — Original SoftPC graphics-presentation source / ABI / failure inventory

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T346 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 7, admitted after the T345 closure under the standing direction to execute ordered owner packages. |
| Objective | Establish the original SoftPC graphics-presentation boundary: `nt_det`, `nt_graph`, `nt_fulsc`, console events, Alt+Enter and their public/obsolete host dependencies before selecting any bounded source-shaped recovery cohort. |
| Non-goals | No GUI redesign, generic event loop, synthetic video device, guest/BOP change, new mapper, raw pointer/HANDLE transport, CPU30, Bochs route, private console-server/CSRSS recreation or runtime trace-driven repair. |
| Verification | Complete source/declaration/definition/caller/failure walk, current-binding comparison, documentation governance and diff checks. No presentation implementation or native observation in S1. |
| Exit Criteria | Each reached presentation edge has one original owner/disposition and one bounded source-shaped cohort is selected or every form is transferred to a named later owner. |
| Stop Conditions | A new window framework, private Console Server contract, video-device behavior, guest change or unreviewed import requires a further S admission. |

**Plan:** [M0 T346 SoftPC graphics presentation](etc/operations/m0-t346-softpc-graphics-presentation-plan-001.md).

**S1 P1:** the presentation ledger separates source-owned display controllers,
public-console input and title forms, session video-event handoff, and the
unavailable private Console-Server fullscreen/controller protocol. It also
corrects three stale mvdm-host divergence registrations so current mirror
state—not prior experiments—drives the cohort decision:
[graphics-presentation owner ledger](etc/operations/m0-t346-s1-graphics-presentation-owner-ledger.tsv).

**S1 P2:** the session video event boundary has no registered production app
sink. Consequently the existing Console API façade is only an unconsumed
same-shaped event translation—not a graphics window, repaint, fullscreen or
Alt+Enter implementation. T346 must design that explicit app composition
boundary before selecting any recovery cohort.

**S1 P3:** the event ABI also lacks the registered VDM text/graphics buffer:
it carries only host handles, rectangle and flags while `RegisterConsoleVDM`
retains the allocated buffer privately. A minimal app sink alone therefore
cannot render source-owned output. Any S2 cohort must first select a bounded,
session-owned presentation-buffer capability or a retained public-console
buffer route; it may not expose private pointers or treat handles as frame
buffers.

**S1 P4:** original source separates two payloads that must remain separate in
any recovery design: `nt_cga.c` writes a bounded 80x50 character/attribute
`textBuffer`, while `nt_graph.c` owns a Console-Server graphics DIB/screen
buffer. Neither is recoverable from the current event handles. T346 will not
invent a universal framebuffer or reinterpret a handle as pixels.

**S1 P5:** the paired formal CPU40 graphs do not define `X86GFX`; the original
hardware fullscreen/detect-thread route is condition-unselected. Per P19, the
selected Alt+Enter behavior is instead an app-owned, session-scoped modern
window transition, without enabling X86GFX or private Console Server behavior.

**S1 closure:** the selected cohort assigns source-facing bindings to
adapter-mvdm-host-out, bounded surface lifetime to session and presentation/
input/window ownership to app, leaving original SoftPC video code unchanged.

### M0 T346 S2 — Session presentation-surface contract

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T346 S2; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T346/S1's P19-aligned source boundary under the standing direction to execute ordered owner packages. |
| Objective | Establish the bounded session-owned text/graphics presentation-surface contract required by an app-owned window, while retaining original SoftPC video/controller source code and adapter source-facing call shapes. |
| Non-goals | No X86GFX, Console Server/CSRSS, direct host VGA, generic framebuffer, raw internal pointer exposure, guest/controller rewrite, new mapper, CPU30 or Bochs route. |
| Verification | Contract tests prove dimensions, lifetime, bounded snapshot/lease access and deterministic unavailable result without a window. |
| Exit Criteria | A single session-owned surface route exists for the selected presentation cohort; no adapter-private pointer or host HANDLE is the data ABI; original SoftPC paths remain unchanged. |

**S2 P1:** `RegisterConsoleVDM` now obtains its original host-local 80x50
text write buffer from session rather than allocating adapter-private storage.
The source-facing pointer remains internal to the adapter/SoftPC call path;
future app code receives only a bounded snapshot. The existing session
lifecycle test now proves write, snapshot dimensions/content and post-clear
failure on both x86 and x64; both paired formal CPU40 graphs rebuild the
affected session/adapter libraries successfully. No graphics framebuffer or
window was introduced.

**S2 P2 / S2 closed:** the original graphics output has now been separated from its
retired Console-Server carrier. `nt_graph.c` still owns DIB construction,
palette/update order and every original CGA/EGA/VGA writer; its sole private
graphics-buffer request binds that DIB descriptor to bounded session storage
through adapter-mvdm-host-out. The app-facing side remains snapshot-only. The
x86/x64 local contract verifies descriptor validation, writable original
pointer, snapshot content and deterministic cleanup. Both formal CPU40 graphs
rebuild the affected objects and relink `softpc-win32-bindings.lib`. See the
[S2 closure evidence](etc/evidence/m0-t346-s2-p2-graphics-session-surface-closure-001.md).

**S2 P3 correction:** S2's graphics surface also copies the public RGB values
from the original `SetConsolePalette` call into session storage. This removes
the last implied app dependency on an `HPALETTE`; the selected x86 session and
Console/graphics fixtures pass, and the formal `session.lib` plus
`softpc-win32-bindings.lib` relink. The public fixture generator now links
`gdi32.lib` for the corresponding documented public palette APIs.

### M0 T346 S3 — App window and session-scoped Alt+Enter lifecycle

**S3 P1:** [the app window lifecycle contract](etc/operations/m0-t346-s3-window-lifecycle-contract-001.md)
confirms that every selected original controller invalidates only after its
existing DIB mutex is released, preserves original console-input consumption,
and assigns public-window lifecycle exclusively to app. It excludes X86GFX,
Console Server fullscreen and `nt_fulsc.c` from the modern Alt+Enter route.

**S3 P2:** [app presentation-window closure](etc/evidence/m0-t346-s3-app-presentation-window-closure-001.md)
adds the app-owned public window, keyboard forwarding, public Alt+Enter style
transition and deterministic normal/user-close separation.  Graphics repaint
copies under the original source mutex, resolved only inside the Win32 adapter
through the existing session host-resource mapping manager.  The selected x86
fixture and the formal `original-softpc-process.exe` link pass.  This is not a
DOS graphics workload, a native guest-continuity claim, or an x64 claim.

**S3 closure:** the app/session/adapter ownership and the selected public
window lifecycle are code-complete with x86 local and formal-link evidence.
S4 now owns only real selected-profile startup and graphics-workload evidence.

**S4 P1:** [fixed presentation observation](etc/evidence/m0-t346-s4-fixed-presentation-observation-001.md)
uses the unchanged console-owning x86 runtime container once. It again reaches
only the pre-graphics `50:11/3B/0F/1B`, `54:05` boundary and then the bounded
timeout. It neither exercises nor regresses the new presentation route; an
actual DOS graphics workload remains gated by an earlier original startup
continuity boundary. `54:05` is the `cmdSetInfo` ingress record; it does not
by itself prove that the body failed or completed. `CONFIG.NT` preprocessing
is the later `54:0C` service and was not reached.

**S4 P2:** [bounded presentation noninterference fixture](etc/evidence/m0-t346-s4-p2-presentation-noninterference-fixture-001.md)
selects SoftPC before the app window opens and proves that the window lifecycle
leaves that backend selection and the active session state unchanged. This is
local composition evidence only; it does not meet the still-blocked guest
graphics-workload criterion.

**S4 P3:** [COMMAND/DEM scalar-alias owner transfer](etc/evidence/m0-t346-s4-p3-command-scalar-alias-owner-transfer-001.md)
keeps the observed `54:05` ingress out of T346 implementation. It establishes
that the original registration of `SCS_ToSync`, `SCS_Is_Dos_Binary` and
`SCS_FDACCESS` is one future mapping-manager lifecycle cohort, not a graphics
or CONFIG.NT leaf; it does not attribute the current timeout to that cohort.

**S4 closure:** T346 has completed its admitted source inventory, bounded
session surface, app-owned presentation window, Alt+Enter lifecycle and local
noninterference verification. The selected DOS graphics-workload criterion is
not met because the fixed original startup observation remains before graphics.
That prerequisite is explicitly transferred to T347's scalar-alias owner
cohort; it is not repaired as a T346 trace leaf.

**T344 closure:** [in-process multi-session reentrancy audit](history/m0-t344-in-process-multi-session-reentrancy-audit-closure-20260901.md) and its [formal closure evidence](etc/evidence/m0-t344-s3-single-session-formal-closure-001.md).

**T343 closure:** [broker/process coordination closure](history/m0-t343-broker-process-cross-process-coordination-closure-20260901.md).

### M0 T342 S1 — Original Debugger / BDE / FAX source / ABI / failure admission (closed)

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T342 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 3, admitted under the standing direction to execute ordered queue packages after T341 closure. |
| Objective | Establish complete original debugger, BDE and FAX owner boundaries: source forms, public entrypoints, direct callers, callbacks, selected build input, failure results and external owner transfers. |
| Non-goals | No debugger/FAX activation, VDD sample loading, generic debugger broker, synthetic event, GUI redesign, kernel/CSRSS/monitor recreation, guest change, BOP leaf, new mapper, alternate executor, Bochs route or CPU30 route. |
| Files And ABI Surface | Original `dbg`, `bde`, `vdmdbg`, `vdmexts`, `fax`, directly reached host debugger sources and their declarations; current adapters and formal CPU40 x86/x64 manifests. |
| Verification | Complete source/declaration/definition/caller/failure walk, build-selection review, documentation governance and diff checks; no runtime observation in S1. |
| Exit Criteria | Every reached source form has one owner/disposition; samples/tools and monitor/kernel/CSRSS forms have explicit transfers; S1 selects at most one bounded original local cohort. |
| Stop Conditions | Any need to enable an arbitrary debugger/FAX DLL, create an event broker, expose native pointer/HANDLE identity, alter guest behavior, recreate private NT4 infrastructure or add an unreviewed source package requires re-admission. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | `DBGInit`, debug notifications, BDE/VDD contracts, FAX samples, WOW/Redirector debugger callers, monitor/kernel events, console/UI, thread lifecycle and teardown. |

**Plan:** [M0 T342 Debugger/BDE/FAX owner-package recovery](etc/operations/m0-t342-debugger-bde-fax-owner-package-recovery-plan-001.md).

### M0 T342 S2 — Selected original debugger startup formal closure (closed)

**Evidence:** [S2 debugger startup closure](etc/evidence/m0-t342-s2-debugger-startup-formal-closure-001.md). **Closure:** [T342 debugger/BDE/FAX package](history/m0-t342-debugger-bde-fax-owner-package-recovery-closure-20260831.md).

### M0 T341 S1 — Original VDD owner source / ABI / failure admission (closed)

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T341 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 2, admitted under the standing owner direction to execute the ordered queue after T340 closure. |
| Objective | Establish the complete original VDD owner boundary: public VDD entrypoints, state, callback tables, callers, machine/monitor/public-host dependencies, result/failure contracts and runtime/tool dispositions. |
| Non-goals | No VDD sample loading, generic VDD dispatcher, kernel/CSRSS/VDM monitor recreation, device or interrupt synthesis, guest-media change, BOP leaf enablement, new mapper, raw host identity, Bochs route, CPU30 route, GUI redesign or runtime trace repair. |
| Reference Baseline | T333 Redirector VDD transfer, T340 closure, selected CPU40 formal graph, original `nt_vdd.c`/headers and existing adapter-mvdm-host-out VDD declarations. |
| Files And ABI Surface | Original `softpc.new/host/src/nt_vdd.c`, `nt_vddp.h`, public VDD declarations, direct callers, `vdd/` and `ieuvddex/` source classification, existing SoftPC/monitor/session adapters. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, architecture, coding, mapping-manager and CPU40-only rules. |
| Verification | Complete source/declaration/definition/caller/failure walk, selected build-input review, documentation governance and diff checks; no runtime observation in S1. |
| Expected Markers | Every VDD entry and direct caller has one unique owner/disposition; samples and debugger extensions are explicitly excluded from runtime selection; hard monitor/kernel boundaries are named. |
| Asset Needs | Existing original MVDM host mirror, formal source manifests and adapters; no new VDD binary, host installation, guest media or private system component. |
| Reporting Requirements | Preserve original API names/layout/order; distinguish source facts from current binding facts; record all unavailable and later-owner transfers without inventing service behavior. |
| Stop Conditions | A need to create a generic VDD provider, load a sample DLL, expose native pointers/HANDLEs, recreate private monitor/kernel/CSRSS behavior, or alter the guest requires re-admission. |
| Exit Criteria | A full original VDD source/ABI/failure ledger selects at most one bounded source-shaped recovery cohort or transfers all nonlocal forms to named owners. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | IO hooks, memory hooks, IRQ/DMA, timers, VDD handles, printer/device binding, BaseSrv/monitor linkage, VDD samples, debugger extensions, WOW/FAX/Redirector callers and teardown. |

**Plan:** [M0 T341 VDD owner-package recovery](etc/operations/m0-t341-vdd-owner-package-recovery-plan-001.md).

### M0 T341 S2 — Selected original VDD no-provider formal closure (closed)

| Field | Record |
| --- | --- |
| Objective | Verify the source-selected, no-installable-VDD profile without loading a sample, creating a provider, or recreating a monitor/kernel path. |
| Result | The fixed formal CPU40/x86 target retains original `nt_vdd.c` and `nt_msscs.c` VDD surfaces plus the narrow configuration/SFT boundaries; Ninja reported `no work to do`. |
| Explicit non-claim | No VDD DLL/device lifecycle, sample, kernel fault route or monitor behavior was enabled or inferred. |

**Evidence:** [S2 VDD formal closure](etc/evidence/m0-t341-s2-vdd-no-provider-formal-closure-001.md). **Closure:** [T341 VDD owner package](history/m0-t341-vdd-owner-package-recovery-closure-20260831.md).

### M0 T339 S1 — Original `50:3D` guest-predecessor contract (closed)

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T339 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 1, admitted under the standing owner direction to execute the ordered queue after T338's closure. |
| Objective | Map both original guest predecessors of reached `50:3D -> demExitVDM` and identify the smallest source-shaped owner cohort that can distinguish them. |
| Non-goals | No BOP/provider patch, guest or media rewrite, device/FDC/INT15 work, guest loader, guessed drive policy, alternate CPU/executor, Bochs route, kernel/CSRSS recreation, raw identity or new child lifecycle. |
| Reference Baseline | T337 package composition, T338 S1–S3, current original `demExitVDM` source attribution, selected CPU40 graph and short-root immutable stage. |
| Files And ABI Surface | Original guest `sysinit1.asm`/COMMAND `init.asm`, DEM `demExitVDM` and dispatch, original file/version/EXEC contracts, existing session mapping and media bindings. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, architecture, coding, mapping-manager and paired-build rules. |
| Verification | Complete definition/caller/failure walk, source-identity and current-build review, governance and diff checks; no runtime retry in S1. |
| Expected Markers | Two caller contracts, their discriminating inputs, one earliest owner/cohort or explicit unobservable boundary. |
| Asset Needs | Selected original MVDM mirrors, current formal graph and recorded short-root observation; no new assets. |
| Reporting Requirements | Separate source fact from runtime inference; retain original failure and ordering; record every rejected recovery rung. |
| Stop Conditions | A need for synthetic guest/device success, media change, a new mapper/executor, unreviewed import, host-drive policy or a different owner package requires re-admission. |
| Exit Criteria | Met: direct callsites, file/EXEC, version, `mem_err`, BIOS BOP 12h and selected configuration-device dependencies have unique original owners/dispositions. The staged `NTDOS.SYS` and `COMMAND.COM` match retained source-built hashes, rejecting a mixed version pair. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | `sysinit1` command bootstrap, COMMAND initialization/version checks, DEM terminal service, file/drive/COMSPEC paths, configuration and Base VDM inputs. |

**Evidence:** [original guest-predecessor contract](etc/evidence/m0-t339-s1-original-guest-predecessor-contract-001.md) and [row ledger](etc/operations/m0-t339-s1-guest-predecessor-contract-ledger.tsv).

### M0 T340 S1 — Resident DOS-BIOS device-initialization contract

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T340 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 1, admitted under the standing owner direction to execute the ordered queue after T339's source-defined transfer. |
| Objective | Build the complete original `charinit -> DEVIOCALL2 -> strategy/interrupt` package map, identify every resident device and original machine/firmware dependency, and classify the first smallest source-shaped recovery cohort without implementing it. |
| Non-goals | No BOP/provider patch, guest/media/configuration change, device behavior change, FDC/INT15 repair, CPU semantic change, host-drive policy, alternate executor, Bochs route, kernel/CSRSS recreation, GUI/presentation work, or additional runtime observation. |
| Reference Baseline | T337 package composition; T338 configuration/root evidence; T339 closure; selected original CPU40 graph and fixed short-root stage. |
| Files And ABI Surface | Original DOS `msinit.asm`/`dev.asm`, BIOS `msinit.asm`/`msbio1.asm`, resident device headers and request packets, original SoftPC firmware/device interfaces, and existing machine/session mappings. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, architecture, coding, mapping-manager and CPU40-only rules. |
| Verification | Complete definition/caller/failure walk, resident-header and firmware ownership ledger, existing composition review, documentation governance and diff checks; no runtime retry. |
| Expected Markers | Every reached resident header has a unique strategy/interrupt owner, every direct machine dependency has a disposition, and one complete earliest cohort or explicit hard boundary. |
| Asset Needs | Existing selected MVDM guest/host mirrors, staged firmware/media manifests, formal CPU40 graph and T339 evidence; no new imports or media. |
| Reporting Requirements | Separate original guest source facts, current composition facts and runtime inference; retain original device order and failure behavior; record rejected recovery rungs. |
| Stop Conditions | A need to alter guest media/configuration, enable a device, change CPU behavior, create a new mapper, import unreviewed source, or cross into a different owner package requires re-admission. |
| Exit Criteria | Met: the complete package ledger identifies all eleven current resident headers, their strategy/interrupt flow, direct function-00 result, configuration-linked successor and the source-backed HIMEM/SoftPC next cohort. No runtime device success is claimed. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | CON/AUX/PRN/CLOCK/COM/LPT headers, BIOS request dispatch, A20, timer/PIC/keyboard/video/serial/parallel interfaces, configured device chain and first post-device DEM BOP. |

**Evidence:** [resident device-initialization contract](etc/evidence/m0-t340-s1-resident-device-initialization-contract-001.md) and [row ledger](etc/operations/m0-t340-s1-resident-device-initialization-contract-ledger.tsv). **Plan:** [M0 T340 resident device-initialization recovery](etc/operations/m0-t340-resident-device-initialization-recovery-plan-001.md).

### M0 T340 S2 — Original HIMEM / SoftPC XMS-A20-INT15-HMA cohort recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T340 S2; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Admitted by T340/S1's source-defined disposition under the standing direction to execute the ordered queue. |
| Objective | Recover the complete reached original `HIMEM.SYS` initialization contract together with its direct original SoftPC XMS, A20, INT 15, HMA and UMB dependency interfaces, using the selected guest and host sources before any new replacement logic. |
| Non-goals | No configuration/media rewrite, host-drive policy, guest loader, standalone XMS/BOP leaf patch, synthetic memory success, CPU semantic change, alternate executor, Bochs route, kernel/CSRSS recreation, GUI work or trace-driven individual repair. |
| Reference Baseline | T340/S1 resident-device ledger and evidence; T337 packaged MVDM root; selected CPU40 graph; original guest HIMEM source and binary. |
| Files And ABI Surface | Original guest `dev/himem/*`, `sysconf.asm`/`sysinit1.asm`, original MVDM XMS provider and SoftPC INT 15/A20/physical-memory interfaces, existing mapping-manager boundary only where an original host/guest pointer seam actually exists. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, mapping-manager, CPU40-only, architecture and coding rules. |
| Verification | Complete definition/caller/failure walk; original package/binary selection review; focused formal CPU40 build and source-contract tests; at most one unchanged fixed-container observation after the whole cohort is recovered. |
| Expected Markers | Every HIMEM init branch and direct machine dependency has one original owner/disposition; any recovery preserves original request/status/failure behavior; no raw native identity crosses the guest boundary. |
| Asset Needs | Existing selected MVDM guest/host mirrors, staged immutable `HIMEM.SYS`, selected firmware/media and formal CPU40 graph; no new assets. |
| Reporting Requirements | Separate source facts, current composition facts and runtime inference; record each rejected replacement rung and any required same-shaped adapter boundary. |
| Stop Conditions | A need for changed guest media/configuration, a second mapper, raw host pointer, new BOP provider, unreviewed import, unrelated device family or synthetic success requires re-admission. |
| Exit Criteria | Complete original HIMEM and SoftPC machine cohort is source-mapped and either recovered with focused evidence or transferred at one explicit hard boundary; only then may S3 perform its single fixed observation. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | XMS allocation/move/UMB, A20 gate, INT 15 extended-memory behavior, HMA relocation, DOS high/low selection, SoftPC mouse/EMM post-HIMEM paths and configuration-device failure returns. |

**Closed:** [S2 original HIMEM/SoftPC recovery](etc/evidence/m0-t340-s2-himem-softpc-xms-a20-int15-hma-recovery-001.md) and [row ledger](etc/operations/m0-t340-s2-himem-softpc-xms-a20-int15-hma-contract-ledger.tsv) establish the source/build closure. [S3 fixed observation](etc/evidence/m0-t340-s3-fixed-himem-cohort-observation-001.md) records no HIMEM reach and no XMS attribution. See the [T340 closure](history/m0-t340-resident-device-initialization-recovery-closure-20260831.md).

### M0 T339 S2 — Passive predecessor discriminator and cohort selection (closed)

| Field | Record |
| --- | --- |
| Objective | Use one passive, source-shaped observation to distinguish the S1 file/EXEC, COMMAND-version, or `mem_err` predecessor family; only then select the smallest original owner cohort. |
| Scope | Existing fixed-width CPU/BOP observation boundary, original `sysinit1.asm`/`sysinit2.asm`/`sysconf.asm`/`command/init.asm`, and S1 ledger source locations. |
| Non-goals | No BOP provider, changed guest media, changed machine memory, host-drive policy, synthetic EXEC success, or alternate CPU executor. |
| Required result | A recorded original callsite/family discriminator and a single source-backed S3 recovery disposition, or an evidence-backed admission blocker. |
| Verification | One unchanged short-root container observation, compared only to the S1 source map; no product behavior change. |

**Evidence:** [passive terminal discriminator observation](etc/evidence/m0-t339-s2-passive-terminal-discriminator-001.md) and [T339 closure](history/m0-t339-minimal-vertical-slice-integration-closure-20260831.md). The sole observation was a fixed-container timeout after `54:05`, not a `50:3D` callsite record; it selected no `demExitVDM` repair and transferred the exact original resident-device owner to T340.

**Plan:** [M0 T339 plan](etc/operations/m0-t339-minimal-vertical-slice-integration-closure-plan-001.md). T339 starts from T338's closed short-root `50:3D` transfer and does not reopen configuration capacity, FDC/INT15, or BOP leaf work.

### M0 T338 — Minimal DOS guest execution and controlled return

**Closed: M0 T338.** S1 preserved the original child/parent-return contract;
S2 attributed the fixed state to original COMMAND configuration expansion; S3
proved the exact original 64-byte configuration-root condition and crossed it
with a byte-identical short-root stage. The reached successor remains
original `50:3D -> demExitVDM`, not a new BOP or loader gap. See the
[S1 contract](etc/evidence/m0-t338-s1-original-child-parent-return-precondition-contract-001.md),
[S2 attribution](etc/evidence/m0-t338-s2-post-configuration-continuation-attribution-001.md),
[S3 transfer](etc/evidence/m0-t338-s3-original-configuration-path-capacity-transfer-001.md),
and [closure](history/m0-t338-minimal-dos-guest-execution-controlled-return-closure-20260831.md).

### M0 T337 — Packaged MVDM system-root composition

**Closed: M0 T337.**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T337 S2/S3/S4; Ordinary Mode. |
| Admission And Approval | The owner admitted T337, directed preservation of original guest binaries beside the executable, confirmed a unified MVDM root for DOS/Win16 media, and selected `mvdm\\softpc` rather than a separate ROM directory. |
| Objective | Make original MVDM `SystemRoot`/`SystemDirectory` consumers see one executable-relative `mvdm` root, retain `mvdm\\system32` system media and `mvdm\\softpc` external SoftPC firmware, then locally prove the selected/missing/unrelated-path contract. |
| Non-goals | No virtual volume, whole-drive `C:` substitution, guest-media or guest-binary mutation, loader/BOP/COMMAND/NTDOS rewrite, CPU/BIOS/FDC repair, new mapping manager, Bochs route, host installation, or runtime retry. |
| Reference Baseline | [T337 S1 audit](etc/evidence/m0-t337-s1-packaged-system-file-root-contract-audit-001.md), [S1 ledger](etc/operations/m0-t337-s1-packaged-system-file-root-contract-ledger.tsv), [S2 rebaseline](etc/evidence/m0-t337-s2-original-system-root-binding-rebaseline-001.md), and T336/S4 terminal attribution. |
| Files And ABI Surface | Original `dos/command/cmdconf.c`, `dos/dem/dem.c`, `nt_pif.c`, `nt_msscs.c`; session/app system-root binding; `mvdm_softpc_system_*`; and `Stage-OriginalSoftpcRuntime.mjs`. |
| Applicable Rules | Execution, source policy, original-source-first recovery, mirror/overlay, architecture, coding and documentation rules. |
| Verification | Package manifest inspection, source sweep, paired formal x86/x64 links, then one unchanged fixed x86 observation. |
| Expected Markers | One executable-relative MVDM system-root identity, original DEM ownership retained, and explicit rejection of full-drive virtualization. |
| Asset Needs | Existing immutable DOS/Win16 package media, selected MVDM mirrors, app/session sources and current formal graphs; no new assets. |
| Reporting Requirements | Separate host package layout from guest drive semantics; distinguish original facts from product inference; name every rejected virtual/root policy. |
| Stop Conditions | Any guest-media/binary alteration, whole-drive virtualization, altered original DEM ordering or unreviewed import requires re-admission. |
| Exit Criteria | Met: one session MVDM system root replaces split DOS/Win16 roots; original MVDM system-file consumers retain their original algorithms through same-shaped binding; package staging preserves `mvdm/system32` and `mvdm/softpc`; paired formal links and one fixed observation are recorded. |
| Original Owner Request | “DOS和win16的guest内容其实都已经在 build/output/dos, win16目录里面了” and “既然如此，那就还是放在mvdm/softpc下吧，不用加rom目录了”. |
| Similar-Issue Sweep | `NTIO.SYS`/`NTDOS.SYS` media load, `config.nt`/`autoexec.nt`, `COMMAND.COM`/COMSPEC, `COUNTRY.SYS`, boot-drive registry/default behavior, canonical guest path conversion, app package layout and missing-file terminal paths. |

**Plan:** [packaged guest system-file-root / DOS drive-root composition](etc/operations/proposal-packaged-guest-system-file-root-drive-root-composition-001.md). **S1 closed:** [source/ABI/failure audit](etc/evidence/m0-t337-s1-packaged-system-file-root-contract-audit-001.md) and its [ledger](etc/operations/m0-t337-s1-packaged-system-file-root-contract-ledger.tsv) retain the initial path scan. **S2 closed:** [original system-root binding rebaseline](etc/evidence/m0-t337-s2-original-system-root-binding-rebaseline-001.md) corrects S1's omitted `cmdconf.c` producer. **S3 closed:** [unified MVDM system-root composition](etc/evidence/m0-t337-s3-unified-mvdm-system-root-composition-001.md). **S4 closed:** [fixed observation](etc/evidence/m0-t337-s4-fixed-mvdm-system-root-observation-001.md). [T337 closure](history/m0-t337-packaged-mvdm-system-root-composition-closure-20260831.md).

### M0 T336 — CPU40/NTDOS post-`54:05` startup continuity

**Closed: M0 T336.**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T336 S4; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | S3 closed the complete original FDC/INT15/ICA/heartbeat source cohort and ruled out a guest-loader repair. Its fixed observation left a source-owned wait attribution, not a BOP leaf, as the exact remaining question. |
| Objective | Identify the owner and contract of the current formal product's post-vector host wait state, including the main CCPU thread and original timer/event/comms companion workers, before selecting any further original source cohort. |
| Non-goals | No BOP repair, guest loader, synthetic result, DOS/NTDOS rewrite, new mapping manager, alternate executor, Bochs route, GUI/WOW/CSRSS/kernel-VDM recreation, `EXEC`/PSP work, arbitrary runtime retry, or product instrumentation. |
| Reference Baseline | T336 S1 [source/cohort admission](etc/evidence/m0-t336-s1-post-cmdsetinfo-source-cohort-admission-001.md), S2 [recursive-vector recovery](etc/evidence/m0-t336-s2-cpu40-recursive-vector-recovery-001.md), and S3 [FDC/INT15 completion cohort](etc/evidence/m0-t336-s3-fdc-int15-completion-cohort-audit-001.md) with its [ledger](etc/operations/m0-t336-s3-fdc-int15-completion-cohort-ledger.tsv). |
| Files And ABI Surface | Original `nt_cpu.c`, CCPU40 execution, `nt_timer.c`, `nt_event.c`, `nt_com.c`, their original waits/events, and the external fixed observer's read-only process-context snapshot. |
| Applicable Rules | Execution, source policy, original-source-first recovery, mirror/overlay, session mapping, CCPU40-only, architecture, coding and documentation rules. |
| Verification | Original definition/caller/wait-object walk; one unchanged fixed-container observation only if it can discriminate an owner; paired formal x86/x64 manifest review; focused negative proof; governance and diff checks. |
| Expected Markers | One exact wait owner or source-shaped terminal result, each reached sibling has one owner/disposition, and no trace-selected BOP or loader route is selected. |
| Asset Needs | Existing selected MVDM/DOS mirrors, T335 evidence, current fixed container and paired formal CCPU40 product graphs; no new guest media or host installation. |
| Reporting Requirements | Separate observations from source inference; name the exact wait/terminal edge, every reached sibling prerequisite and the recovery rung selected or rejected. |
| Stop Conditions | Any need for a new BOP provider, guest loader, second mapper/executor, raw host pointer, guest-media rewrite, unreviewed source import, or a diagnosis that reaches a different owner package requires re-admission. |
| Exit Criteria | Met: the committed evidence/ledger names the original `50:3D -> demExitVDM` terminal owner, its two guest caller possibilities, and the next host-composition boundary; no generic DOS completion claim is made. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | `cmdSetInfo` service-byte/IP return, NTDOS register/segment restoration, DOS/BIOS exchange fields, SAS span and stack conversion, CPU40 faults, disk stack, Win386/DOSWOW fields, timer/PIT/ICA prerequisites and stale Bochs-era routes. |

**Plan:** [CPU40/NTDOS post-`54:05` startup continuity](etc/operations/proposal-cpu40-ntdos-post-cmdsetinfo-continuity-001.md). **S1 closed:** [source/cohort admission](etc/evidence/m0-t336-s1-post-cmdsetinfo-source-cohort-admission-001.md) and [ten-row ledger](etc/operations/m0-t336-s1-post-cmdsetinfo-cohort-ledger.tsv) establish that guest media is already loaded. **S2 closed:** [recursive-vector recovery](etc/evidence/m0-t336-s2-cpu40-recursive-vector-recovery-001.md) and its [six-row ledger](etc/operations/m0-t336-s2-cpu40-recursive-vector-recovery-ledger.tsv) restore the original CCPU40 `host_simulate_func` vector and turn the former null-IP crash into a bounded live-process timeout. **S3 closed:** [FDC/INT15 completion-cohort audit](etc/evidence/m0-t336-s3-fdc-int15-completion-cohort-audit-001.md) and [ledger](etc/operations/m0-t336-s3-fdc-int15-completion-cohort-ledger.tsv) select the complete original source chain, reject a speculative physical-floppy or loader repair, and record the external all-thread wait snapshot as attribution evidence only. **S4 closed:** [post-vector terminal attribution](etc/evidence/m0-t336-s4-post-vector-terminal-attribution-001.md) and its [ledger](etc/operations/m0-t336-s4-post-vector-terminal-attribution-ledger.tsv) prove the current state is original `50:3D -> demExitVDM`, not an FDC wait, and transfer the portable guest drive-root decision without selecting an unreviewed implementation. [T336 closure](history/m0-t336-cpu40-ntdos-post-cmdsetinfo-continuity-closure-20260831.md).

### M0 T335 — Original WOW32/Win16 owner-package recovery

**Closed: M0 T335.**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T335; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | T335/S1 source/profile closure under the owner’s standing direction to execute ordered queue packages. |
| Objective | Reconcile the selected original WOW32 non-GUI package, admitted bindings and every remaining owner disposition on fresh paired formal graphs. |
| Non-goals | No generic Win16 API, GUI/USER/GDI, WOWEXEC/CSRSS/kernel-VDM recreation, BOP leaf patch, new mapper, guest-media change, arbitrary guest load, raw native identity or Bochs route. |
| Reference Baseline | T335/S1 source/profile rebaseline and T335/S2 `CallBack16` contract ledger; original `wow32/wcall16.c`; CCPU40 SoftPC/monitor/session mapping contracts. |
| Files And ABI Surface | Original `wow32.h` temporary-pointer macro family, scoped-frame bridge, `CBVDMFRAME`/`VDMFRAME`, CPU40 `host_simulate`, task-frame binding, the existing same-shaped BaseVDM `VDMINFO` command route and source-contract fixtures. |
| Applicable Rules | Execution, source policy, mirror/overlay, architecture, coding, documentation, session mapping and CCPU40-only machine rules. |
| Verification | Fresh selected-source x86/x64 formal graphs, focused source-contract checks, one fixed-container observation at most, and final disposition reconciliation. |
| Expected Markers | Every selected original file/interface has one disposition; the declared profile remains distinct from unavailable product shells. |
| Asset Needs | Existing source mirrors, adapter/session code and current CCPU40 formal graph; no new guest media or private host service. |
| Reporting Requirements | Distinguish the declared non-GUI transaction from generic Win16/WOW product claims; name every unresolved monitor/broker/guest prerequisite. |
| Stop Conditions | The required profile would need a generic adapter, raw host pointer/HANDLE, second mapping manager, changed CCPU semantics, invented callback scheduler or private product-shell recreation. |
| Exit Criteria | Met: both fresh formal architectures build the selected closure; every selected row, divergence and owner transfer is reconciled without claiming generic WOW/Win16 execution. |
| Original Owner Request | Execute the queue in single-person dual-role mode, use original source first, minimize mirror divergence and keep adapters narrow. |
| Similar-Issue Sweep | WOW32/FAX hosts, selected manifest versus inactive forms, callbacks, frame layouts, CCPU/SAS/monitor calls, command broker calls, guest/resource identity, public Win32, private USER/GDI, VDD, cross-process and guest-media boundaries. |

**Plan:** [M0 T335 WOW32/Win16 owner package recovery](etc/operations/m0-t335-wow32-win16-owner-package-recovery-plan-001.md). **S1 closed:** [current source/ABI/profile rebaseline](etc/evidence/m0-t335-s1-wow-source-abi-profile-rebaseline-001.md) and its [315-row mirror/profile ledger](etc/operations/m0-t335-s1-wow-source-profile-ledger.tsv). **S2 closed:** [callback/frame contract](etc/evidence/m0-t335-s2-wow-callback-frame-contract-001.md) and its [twelve-element ledger](etc/operations/m0-t335-s2-wow-callback-frame-contract-ledger.tsv). **S3 P1 closed:** [scoped-frame lease](etc/evidence/m0-t335-s3-p1-wow-scoped-frame-lease-001.md) passes focused x86/x64 tests. **S3 P2 closed:** [pointer-access family audit](etc/evidence/m0-t335-s3-p2-wow-pointer-access-family-audit-001.md) rejects a callback-only macro change. **S3 P3 closed:** [current-task TEB binding](etc/evidence/m0-t335-s3-p3-wow-current-task-teb-binding-001.md) restores the original `CURRENTPTD()` storage precondition without recreating a real NT4 TEB. **S3 P4 closed:** [SoftPC guest-pointer seam correction](etc/evidence/m0-t335-s3-p4-wow-softpc-pointer-seam-correction-001.md) removes the direct session bypass. **S3 P5 closed:** [source-wide pointer-plane audit](etc/evidence/m0-t335-s3-p5-wow-source-wide-pointer-plane-audit-001.md) and its generated [77-unit usage ledger](etc/operations/m0-t335-s3-p5-wow-pointer-plane-source-usage-ledger.tsv) replace the earlier incomplete sample. **S3 P6 closed:** [CPU40 selector-resolution recovery](etc/evidence/m0-t335-s3-p6-wow-cpu40-selector-resolution-001.md) restores the original CCPU40 GDT/LDT descriptor path for numeric WOW/SIM32 conversion without a CPU30 monitor table. **S3 P7 closed:** [two-frame lease shape](etc/evidence/m0-t335-s3-p7-wow-two-frame-lease-shape-001.md) proves the original `CallBack16` caller/callback-frame lifetime through one session lease context on x86 and x64. **S3 closed:** source-shaped frame hook and CCPU40 recursive-return proof are recorded in P8–P12.

**S3 P8 closed:** [original callback source binding](etc/evidence/m0-t335-s3-p8-wow-callback-source-binding-001.md) compiles the selected `wcall16.c` body on x86/x64 with original declaration and CCPU40 stack selection. **S3 P9 closed:** [guest-return boundary](etc/evidence/m0-t335-s3-p9-wow-guest-return-boundary-001.md) proves that a real callback return requires the original `WOW16_From_CallBack16` guest trampoline; Win16 media is already carried and app transparently forwards the original `-w/-a` activation contract, while BOP `51h` retains the original dynamic provider boundary. **S3 P10 closed:** [BaseSrv WOW command-route binding](etc/evidence/m0-t335-s3-p10-basesrv-wow-command-route-001.md) restores the original distinct shared-WOW nonblocking record behavior through the existing same-shaped local BaseVDM boundary; paired Ninja fixtures pass. **S3 P11 closed:** [original WOW startup activation observation](etc/evidence/m0-t335-s3-p11-original-wow-startup-activation-observation-001.md) links the formal x86/x64 products and proves original `-f/-w/-a` startup reaches DOS BOP dispatch after the bounded non-GUI presentation disposition; it records, but does not repair, the later `0xC0000005` continuity failure. **S3 P12 closed:** [original `CallBack16`/CCPU40 bounded return](etc/evidence/m0-t335-s3-p12-original-callback-ccpu40-return-001.md) runs the original `wcall16.c` transaction through original CCPU40 return ordering on x86; it deliberately does not claim execution of the Win16 callback trampoline. **S4 closed:** [package reconciliation](etc/evidence/m0-t335-s4-wow-package-reconciliation-001.md) records fresh 419-action x86/x64 formal product builds and the one fixed-container observation.

T335's [closure record](history/m0-t335-wow32-win16-owner-package-closure-20260831.md) preserves its declared non-GUI profile and the named CPU40/guest-continuity successor boundary.

### M0 T334 — NetAPI/RAP Redirector network-boundary recovery

**Closed: M0 T334**

| Field | Record |
| --- | --- |
| Result | All eighteen reached physical edges have a local, exact-unavailable, or named owner-transfer disposition. |
| Source Fidelity | Three bounded public-local contracts retain original provider order, allocation/free and error behavior; `MVDM-HOST-DIV-171/172` are the only selected local binding changes. |
| Verification | Fresh selected-source `original-softpc-candidate` builds pass on x86 (147 actions) and x64 (440 actions); source-negative and x86/x64 guest-copy fixtures pass. |
| Limit | No RAP/SMB, NetBIOS/DLC/VDD, remote guest-network or end-to-end guest execution success is claimed. |
| Evidence | [S4 package reconciliation](etc/evidence/m0-t334-s4-netapi-rap-package-reconciliation-001.md) and [closure record](history/m0-t334-netapi-rap-redirector-network-boundary-closure-20260831.md). |

**Plan:** [M0 T334 NetAPI/RAP Redirector recovery](etc/operations/m0-t334-netapi-rap-redirector-network-boundary-plan-001.md). **S1 closed:** [physical boundary rebaseline](etc/evidence/m0-t334-s1-netapi-rap-physical-boundary-rebaseline-001.md) and its [18-row ledger](etc/operations/m0-t334-s1-netapi-rap-physical-edge-ledger.tsv). **S2 closed:** [public workstation-query contract verification](etc/evidence/m0-t334-s2-public-workstation-query-contract-001.md). **S3 closed:** [negative-contract reconciliation](etc/evidence/m0-t334-s3-redirector-negative-contract-reconciliation-001.md). **S4 closed:** [package reconciliation](etc/evidence/m0-t334-s4-netapi-rap-package-reconciliation-001.md).

### M0 T333 — Redirector owner package closure

**Closed: M0 T333**

| Field | Record |
| --- | --- |
| Result | Complete original package selection and dispatch disposition; formal x86/x64 static source graphs; source-owned local cohorts; exact unsupported branches; named owner transfers for BaseSrv/RAP, NetBIOS, DLC and VDD. |
| DLL Decision | No false DllMain bridge: original attach would immediately enter unresolved NetBIOS/DLC/VDD initialization. The original deferred `LoadVdmRedir` failure remains the correct current product behavior. |
| Evidence | [S6 package reconciliation](etc/evidence/m0-t333-s6-redirector-package-reconciliation-001.md), with S1/S2/S3/S4/S5 ledgers and evidence retained below. |

**Plan:** [M0 T333 original Redirector recovery](etc/operations/m0-t333-redirector-owner-package-recovery-plan-001.md). **S1 closed:** [source/ABI admission evidence](etc/evidence/m0-t333-s1-original-redirector-source-abi-admission-001.md) and its [row ledger](etc/operations/m0-t333-s1-redirector-source-abi-admission-ledger.tsv). **S2 closed:** [external-form admission](etc/evidence/m0-t333-s2-redirector-external-form-admission-001.md) and its [disposition ledger](etc/operations/m0-t333-s2-redirector-external-form-disposition-ledger.tsv). **S3 closed:** complete source selection/build and the original DLL dependency boundary are recorded below. **S4 closed:** [mailslot and asynchronous adapter composition](etc/evidence/m0-t333-s4-redirector-async-adapter-composition-001.md) moves the durable copied-span seam to the Redirector adapter without changing the original queue/ICA owner. **S5 closed:** [network and assignment disposition](etc/evidence/m0-t333-s5-redirector-network-assignment-disposition-001.md) closes every remaining network row as direct, exact original unavailable, or a named owner-package transfer. **S6 closed:** [package reconciliation](etc/evidence/m0-t333-s6-redirector-package-reconciliation-001.md) retains original dynamic-load failure until the named network/VDD owner packages close.

**S3 P1:** [the paired original synchronous-source build](etc/evidence/m0-t333-s3-p1-redirector-synchronous-source-build-001.md) retains the initial narrow-cohort evidence. **S3 P2:** [the complete original-package build and lifecycle correction](etc/evidence/m0-t333-s3-p2-complete-redirector-package-build-001.md) supersedes its package-selection conclusion: every original `vdmredir/sources` unit now compiles into `original-mvdm-redir.lib` on x86 and x64. `VDDInstallUserHook` itself is an already-selected original SoftPC body; the remaining lifecycle predecessor is the original `VDMREDIR.DLL` entry/load composition, with NetBIOS/DLC behavior still not enabled.

**S3 P3:** [the original DLL dependency boundary](etc/evidence/m0-t333-s3-p3-original-dll-dependency-boundary-001.md) records the direct non-`/FORCE` x86 link probe. It establishes that `VrDllInitialize` is not independently linkable before CPU40, mailslot/async, NetAPI/RAP, NetBIOS/DLC and original host/VDD cohorts receive their own dispositions; source-shaped `LoadVdmRedir` failure remains the current behavior. Final DLL packaging is therefore S6 work, not a Redirector-local shim.

| Field | Record |
| --- | --- |
### M0 T332 — Original DPMI/DPMI32 owner-package recovery

**Closed: M0 T332.** [S4 package disposition](etc/evidence/m0-t332-s4-original-dpmi-package-disposition-001.md) records the exact selected-source, monitor/debugger/VDD and DOSX runtime boundaries. [Closure record](history/m0-t332-original-dpmi-owner-package-closure-20260831.md).

**S1 closed:** [source/ABI admission evidence](etc/evidence/m0-t332-s1-original-dpmi-source-abi-admission-001.md) and its [complete table/import ledger](etc/operations/m0-t332-s1-dpmi-source-abi-admission-ledger.tsv) establish the original package boundary. **S2 closed:** [the original SoftPC-span, identity and monitor contract](etc/evidence/m0-t332-s2-dpmi-span-identity-monitor-contract-001.md) distinguishes source-internal RAM aliases from true host-boundary identities. **S3 closed:** [the original source-cohort build](etc/evidence/m0-t332-s3-original-dpmi-source-cohort-build-001.md) proves both final product graphs select and link the cohort.

### M0 T331 — Original XMS owner-package closure

**Closed: M0 T331.** [S4 formal product closure](etc/evidence/m0-t331-s4-original-xms-formal-product-closure-001.md) records the paired non-`/FORCE` product links and the one unchanged fixed-container observation. [Closure record](history/m0-t331-original-xms-owner-package-closure-20260831.md).

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T331 S4; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Closed under T331’s persistent owner-approved whole-package plan. |
| Objective | Formally link the selected original SoftPC/XMS product on x86 and x64, then perform at most one fixed-container x86 observation if the changed XMS binding reaches that path. |
| Non-goals | No new XMS policy, trace-selected service work, guest/device success shortcut, new allocator, CPU/FDC/ROM/vector repair, MONITOR/kernel-VDM/Bochs route, raw host pointer ABI, or unrelated BOP work. |
| Reference Baseline | T331 S1 owner/ABI ledger; T331 S2 CPU40/session binding; T331 S3 package matrix; original `xms.486` source set. |
| Files And ABI Surface | Original `xms.c`, `xmsblock.c`, `xmsa20.c`, `xmsumb.c`, original SubAlloc and dispatch; CPU40/session lease provider; original SAS/ReserveUMB/UpdateKbdInt15 declarations. |
| Applicable Rules | Execution, source policy, original-source-first recovery, mirror/overlay, architecture, mapping-manager, session and paired-host build rules. |
| Verification | Regenerate and link the same selected product target on x86 and x64 without `/FORCE`; record one bounded fixed-container observation only if it is applicable. |
| Expected Markers | Original XMS and SubAlloc bodies remain policy owners; no duplicate CRT symbol is suppressed; any observation result is attributed without selecting a new repair. |
| Asset Needs | Current original MVDM mirrors, adapter/session source, existing formal graph and source-selected x86/x64 products; no new guest media or host installation. |
| Reporting Requirements | Record both link results, any source-shaped product binding required for them, and the bounded observation result if one is performed. |
| Stop Conditions | Need for a new XMS policy, synthetic success, a new mapping manager, a Bochs resurrection, kernel/CSRSS reconstruction, a broad source import, or a behavior change outside the complete XMS package pauses for re-admission. |
| Exit Criteria | Both product links pass, no duplicate CRT symbol is suppressed, and any observation result is recorded without selecting a new repair. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Callback parameter types, callback registration order, guest-memory lease acquire/release, move overlap, commit/decommit observability, A20 state-byte write, UMB numeric carrier and INT15 provider availability. |

**S3 closed:** [the original XMS package contract matrix](etc/evidence/m0-t331-s3-original-xms-package-contract-matrix-001.md) records original allocation/move/A20/UMB/INT15 ownership and the paired adapter-boundary witness. **S4 closed:** paired product links and the one allowed fixed-container observation are recorded above.

### M0 T331 S2 — Original XMS body binding recovery

**Closed: M0 T331 S2.** [The CPU40/session binding evidence](etc/evidence/m0-t331-s2-original-xms-cpu40-session-binding-001.md) records the same-shaped `c_sas_memory_size`/`c_sas_loads`/`c_sas_stores` provider, exact lease lifetime and paired formal x86/x64 candidate builds. The original XMS/SubAlloc source remains the policy owner; S3 now owns package-wide contract exercise.

### M0 T331 S1 — Current original XMS ownership and ABI rebaseline

**Closed: M0 T331 S1.**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T331 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 1, admitted under the persistent owner request to execute the ordered queue in single-person dual-role mode. |
| Objective | Establish one current, complete original XMS source/ABI/machine-owner baseline before changing any XMS provider behavior. |
| Non-goals | No trace-selected `52:xx` repair, guest loader, new XMS allocator, synthetic service result, CPU/FDC/ROM/vector repair, MONITOR/kernel-VDM/Bochs route, raw pointer ABI, or unrelated BOP work. |
| Reference Baseline | T310 XMS static ledger/closure; T327–T330 fixed CPU40 guest-path evidence; selected `CPU_40_STYLE` original SoftPC product and current mapping manager. |
| Files And ABI Surface | `xms.486/{xms,xmsa20,xmsblock,xmsdisp,xmsmisc,xmsumb}.c`, `xms.h`, `i386/xmsmem86.c`, `mvdm-support/suballoc`, `nt_msscs.c`, `nt_bop.c`, the mapping-manager backend and A20/UMB/INT15 machine declarations. |
| Applicable Rules | Execution, source policy, original-source-first recovery, mirror/overlay, architecture, mapping-manager, session and paired-host build rules. |
| Verification | Original declaration/definition/table/caller walk; retired-route scan; current formal source-manifest review; exact adapter/overlay disposition ledger; governance and diff review. |
| Expected Markers | Every selected XMS entry has one original body and one current machine/adapter disposition; direct host-pointer backend is not selected; all former Bochs routes are either absent or named historical evidence. |
| Asset Needs | Current MVDM host/support mirrors, current adapter/session code, prior XMS ledgers and selected CPU40 formal build graph; no new guest media or host installation. |
| Reporting Requirements | Record source identity, table slot, caller, original policy owner, callback ABI, mapping use, current composition status, retained divergence and exact successor owner for every unavailable effect. |
| Stop Conditions | A need for new XMS policy, guest/device success shortcut, a new mapping manager, a Bochs resurrection, kernel/CSRSS reconstruction, unreviewed source import, or a behavior change outside the complete XMS package pauses for re-admission. |
| Exit Criteria | A current package baseline supports S2 original-body binding recovery without carrying a stale Bochs-era route or an unclassified XMS callback. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Startup `XMSInit`, BOP `XMSDispatch`, all dispatch slots, SubAlloc callbacks, A20 state/query, move descriptors, UMB lifecycle, INT15 hook, direct-pointer body and all source/build routes mentioning retired Bochs ownership. |

**S1 evidence:** [the current original XMS owner/ABI rebaseline](etc/evidence/m0-t331-s1-current-original-xms-owner-abi-rebaseline-001.md) and its [complete ledger](etc/evidence/m0-t331-s1-xms-owner-abi-rebaseline-001.tsv) establish the only admitted S2 binding cohort. The selected `original-mvdm-xms.lib` target was reproducibly up to date on both current formal x86/x64 graphs.

### M0 T330 S1 — Original exception-filter context contract

**Closed: M0 T330 S1–S3 and M0 T330.**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T330 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 1, admitted under the standing owner direction to execute the ordered queue after T329 closure. |
| Objective | Establish the original unhandled-exception filter and optional-report contract, then attribute the fixed-container exception using copied actual context. |
| Non-goals | No BOP/guest/FDC/ROM/vector/CPU behavior repair, synthetic device result, new executor, Bochs/CPU30/MONITOR/kernel-VDM/CSRSS route, new mapper, raw native identity, WOW/VDD/debugger or product-shell redesign. |
| Reference Baseline | T327 fixed configuration transfer; T328 FDC/INT15 closure; T329 CPU40 recursive-frame closure. |
| Files And ABI Surface | Original `VdmUnhandledExceptionFilter`, existing report-path diagnostic seam, fixed-width copied `EXCEPTION_RECORD`/`CONTEXT` fields, selected product entry and observer. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, architecture, mapping-manager, session and paired-host build rules. |
| Verification | Original filter/definition/caller walk; report-seam review; paired x86/x64 formal product links; one immutable short-root x86 observation with the report gate proven behavior-neutral. |
| Expected Markers | One exact exception context and mapped original source owner, or proof that the selected original filter is unavailable before it receives an exception record. |
| Asset Needs | Existing selected MVDM mirror/media, CPU40 formal graph, short-root fixed observer and existing exception report path; no new guest or host installation. |
| Reporting Requirements | Record copied fields, report gate, original exception disposition, timer effects, source map result, changed locations and successor owner. |
| Stop Conditions | Need for a behavior-changing catch/continue, FDC/ROM/vector/BOP or CPU fix, kernel/CSRSS reconstruction, persistent guest/native pointer, unreviewed source import or change outside the exception cohort pauses for re-admission. |
| Exit Criteria | An exact fixed-container original exception owner transfer with paired formal links; no x64 runtime claim. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Original main/thread filter callers, timer suspend/resume, report gate/lifetime, exception record/context copying, source-map arithmetic and bounded observer behavior. |

**S1–S3 evidence:** [the original exception-context attribution](etc/evidence/m0-t330-s1-s3-original-exception-context-attribution-001.md) records the original filter semantics, the opt-in copied report boundary, paired formal links and the fixed-container result.

**T330 closure:** the fixed observation did not traverse the selected original filter, so no exact `EXCEPTION_POINTERS` exists for this package to map. The task closes at that source-defined unavailable boundary; it does not authorize an exception catch, CPU-frame, FDC, ROM, vector or BOP workaround. [Closure record](history/m0-t330-exception-context-attribution-closure-20260831.md).

### M0 T329 S1 — Original CCPU recursive-frame lifecycle contract

**Closed: M0 T329 S1 and M0 T329.**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T329 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 1, admitted under the standing owner direction to execute the ordered queue after T328 closure. |
| Objective | Establish the complete original CPU40 recursive `setjmp`/`longjmp` frame, TLS and guest re-entry contract that T328 identified as the exact successor owner. |
| Non-goals | No FDC/ROM/vector/BOP leaf repair, guest-image rewrite, synthetic device result, new executor, Bochs/CPU30/MONITOR/kernel-VDM/CSRSS route, new mapper, raw native identity, WOW/VDD/debugger or product-shell redesign. |
| Reference Baseline | T319 ROM/termination attribution; T327 configuration transfer; T328 complete FDC/INT15 owner map and paired formal links. |
| Files And ABI Surface | Original CPU40 `c_cpu_init`, `c_cpu_simulate`, `c_cpu_unsimulate`, `ccpu386InitThreadStuff`, `ccpu386SimulatePtr`, `ccpu386Unsimulate`, exception frame helpers and direct original re-entry callers. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, architecture, mapping-manager, session and paired-host build rules. |
| Verification | Ordered original source/definition/caller walk; selected source-manifest review; paired x86/x64 formal product links after a complete owner recovery; one short-root immutable-media x86 observation only after a behavior change. |
| Expected Markers | One complete recursive-frame owner/lifetime map and either one source-shaped recovery cohort or one narrower named original owner transfer. |
| Asset Needs | Existing selected MVDM mirror/media, CPU40 formal graph, short-root fixed observer, session and adapter families; no new guest or host installation. |
| Reporting Requirements | Record initial frame setup, TLS owner/lifetime, entry/return depth, BOP-FE route, null/base-level behavior, changed mirror locations and successor owner. |
| Stop Conditions | Need for an invented device/BOP result, FDC/ROM/vector rework, kernel/CSRSS reconstruction, persistent guest/native pointer, unreviewed source import or change outside the selected complete owner cohort pauses for re-admission. |
| Exit Criteria | A fixed-container original guest-result boundary or one narrow source-defined CPU40 recursive-frame/guest-re-entry owner transfer, with paired formal links. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | CPU40 initialization, TLS allocation and teardown, frame acquire/release, BOP-FE immediate and extended paths, exception frame return, nested FDC/keyboard/WOW re-entry callers, and the first post-frame return. |

**S1 evidence:** [the original CPU40 recursive-frame lifecycle contract](etc/evidence/m0-t329-s1-original-cpu40-recursive-frame-lifecycle-contract-001.md) records the selected initialization, TLS/frame ownership, all direct re-entry callers, historical underflow disposition and current x86/x64 bounded recursive execution recheck.

**T329 closure:** no CCPU binding was missing and the bounded original recursive route passes, so S2 had no admissible behavior change. The only remaining question is actual fixed-container exception context; T330 owns that diagnostic-only attribution and may not add a frame guard.

### M0 T328 S1 — Original FDC/INT15 wait-continuation contract

**Closed: M0 T328 S1 and M0 T328.**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T328 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 1, admitted under the standing owner direction to execute the ordered queue. |
| Objective | Establish the complete original CPU40 FDC/INT15 wait-continuation and callback contract that follows T327's completed COMMAND configuration package. |
| Non-goals | No BOP leaf implementation, guest-image rewrite, synthetic device result, new executor, Bochs/CPU30/MONITOR/kernel-VDM/CSRSS route, new mapper, raw native identity, WOW/VDD/debugger or product-shell redesign. |
| Reference Baseline | T319 ROM/termination attribution; T326 fixed pre-BOP cut; T327 source-defined COMMAND configuration transfer and paired formal links. |
| Files And ABI Surface | Original CPU40 `wait_int`, FDC/INT15 callbacks and function-pointer slots, `c_cpu_unsimulate`/CCPU return, selected original machine bindings, fixed observer. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, architecture, mapping-manager, session and paired-host build rules. |
| Verification | Ordered original source/definition/caller walk; selected source-manifest review; paired x86/x64 formal product links after a complete owner recovery; one short-root immutable-media x86 observation only then. |
| Expected Markers | One complete callback-slot/owner map and either one source-shaped recovery cohort or one narrower named original owner transfer. |
| Asset Needs | Existing selected MVDM mirror/media, CPU40 formal graph, short-root fixed observer, session and adapter families; no new guest or host installation. |
| Reporting Requirements | Record original callback registration, ownership, lifetime, null/failure direction, changed mirror locations and successor owner. |
| Stop Conditions | Need for invented guest/device success, a trace-selected BOP change, kernel/CSRSS reconstruction, persistent guest/native pointer, unreviewed source import or change outside the selected complete owner cohort pauses for re-admission. |
| Exit Criteria | A fixed-container original guest-result boundary or one narrow source-defined FDC/INT15/CPU40 owner transfer, with paired formal links. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | CPU40 wait/unsimulate, FDC and INT15 callback installation, PIC/ICA completion, ROM wait vector, source-owned null callback result, and the first post-configuration return. |

**S1 evidence:** [the original FDC/INT15 wait-continuation contract](etc/evidence/m0-t328-s1-original-fdc-int15-wait-continuation-contract-001.md) resolves the complete FDC, ICA, ROM, vector and BOP-FE chain as already sourced from original owners. It establishes a narrower successor: CPU40 recursive-frame and guest re-entry integrity. Both unchanged formal x86/x64 selected-source product links pass; no x64 runtime claim is made.

**T328 closure:** no original FDC/INT15 binding was missing, therefore S2 had no admissible implementation and S3 had no changed product to observe. T328 closes through its exact source-defined owner-transfer exit. The next package is limited to the original CPU40 recursive frame contract; it may not reopen FDC/ROM/vector or BOP work.

### M0 T327 — Minimal DOS guest execution and controlled return

**Closed: M0 T327.** The fixed short-root container crossed original `54:0C`
and its CONFIG.NT DEM file-operation cohort, but it did not consume the
declared command. The next failure is the already classified original
CPU40/FDC `wait_int` continuation. [S3 evidence](etc/evidence/m0-t327-s3-command-configuration-owner-transfer-001.md) and [closure record](history/m0-t327-minimal-dos-guest-execution-controlled-return-closure-20260831.md) preserve the exact transfer.

### M0 T326 S1 — Fixed SoftPC pre-BOP startup cut

**Closed: M0 T326 S1**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T326 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 1, admitted under the standing owner direction to execute the ordered queue. |
| Objective | Establish one fixed original-SoftPC CPU40 pre-BOP startup cut and identify the exact original owner of the first continuity stop or first BOP ingress. |
| Non-goals | No BOP service, DOS/guest, controller-result, WOW, VDD, debugger, Bochs, CPU30/MONITOR, kernel-VDM, new mapper, raw pointer, synthetic device success or product-shell redesign. |
| Reference Baseline | T313 SoftPC source closure; T318 NTDOS owner transfer; T319 ROM/termination attribution; selected CPU40 source graph and staged immutable media. |
| Files And ABI Surface | Original product entry, SoftPC startup, media/firmware staging, CPU40 entry, timer/event/error sources and directly created raw workers; fixed observer and formal Ninja source selection. |
| Applicable Rules | Execution, source policy, original-source-first recovery, mirror/overlay, architecture, mapping-manager, session and paired-host build rules. |
| Verification | Ordered original source walk; fixed-container manifest review; paired x86/x64 compile graphs; one bounded x86 observation; governance and diff review. |
| Expected Markers | One named source owner or one first original BOP ingress, with no changed observer variable, no synthetic success and no inferred trace repair. |
| Asset Needs | Existing selected MVDM mirror/media, CPU40 formal graph, fixed observer and session/adapter boundaries; no new guest or host installation. |
| Reporting Requirements | Record original call order, worker creation/lifetime, terminal route, exact media/firmware inputs, architecture result and the named next owner transfer. |
| Stop Conditions | Need for a second executor, kernel/CSRSS reconstruction, invented device success, persistent native pointer, unreviewed source import or change outside the selected complete owner cluster pauses for re-admission. |
| Exit Criteria | The fixed pre-BOP cut and one observed result identify a bounded owner-recovery S or an exact source-owned transfer; no BOP/business-runtime claim is made from startup evidence alone. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Product entry, media/firmware discovery, console/error initialization, timer/event/raw-worker creation, CPU40 start/return, termination and pre-BOP exception paths. |

**S1 plan:** [the SoftPC pre-BOP startup-continuity plan](etc/operations/m0-t326-softpc-pre-bop-startup-continuity-plan-001.md)
fixes the container and original-source cut before selecting any implementation.

**S1 closed:** [the fixed SoftPC startup-cut evidence](etc/evidence/m0-t326-s1-fixed-softpc-pre-bop-startup-cut-001.md)
records the original `50:11 → MS_bop_0 → DemDispatch → demLoadDos` crossing,
the unchanged fixed container, paired successful product links and the
post-crossing successor exception.

**T326 closed:** no pre-BOP owner recovery was admissible after S1 reached the
original boundary. The post-crossing ordinary guest control flow transfers to
queue candidate 2, Minimal DOS guest execution and controlled return.

### M0 T325 S1 — NetAPI/RAP Redirector original-owner baseline

**Closed: M0 T325 S1**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T325 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 6, admitted after T324 closure under the standing owner direction to execute the ordered queue. |
| Objective | Establish one complete source/ABI/lifetime/failure baseline for the original NetAPI/RAP Redirector network cohort before selecting any public modern binding or source change. |
| Non-goals | No remote guest-network success, SMB/RAP/NetBIOS/DLC/VDD/RPC/CSRSS recreation, 57:xx leaf enablement, generic adapter, new mapping manager, native identity exposure, guest edit or unrelated local Redirector rework. |
| Reference Baseline | T324/S2 local Redirector closure; `proposal-netapi-rap-redirector-network-boundary-recovery-001.md`; T306 NetAPI cohort audit; selected original `vrnetapi.c`, `vrremote.c`, `cmdredir.c`, `vdmredir.h` and byte-identical `opennt-host/netapi/netlib/ntstatus.c`. |
| Files And ABI Surface | Original `vrnetapi.c`/`vrremote.c` callers, NetAPI/RAP declarations and layouts, `ntstatus.c` status conversion, `cmdredir.c` caller forms, session host-resource mapping and existing Redirector/Win32 facades. |
| Applicable Rules | Execution, source policy, source-first recovery, mirror/overlay, architecture, session mapping-manager and host-capability rules. |
| Verification | Original declaration/definition/caller review; physical NetAPI/RAP edge ledger; exact status/lifetime/failure disposition; source-selection/build-manifest review; governance and diff review. |
| Expected Markers | Every reached network-management call has an original caller, layout, identity/buffer lifetime, selected public binding or source-shaped unavailable result, and named later owner where needed. |
| Asset Needs | Existing MVDM and `ntstatus.c` mirrors, T306 ledgers, current session and adapter families; no network service, kernel facility, VDD or guest-media change. |
| Reporting Requirements | Record original call order, request/result fields, allocation/release, public API equivalence or exact failure, mapping ownership and any transfer to NetBIOS/DLC/VDD/WOW/broker packages. |
| Stop Conditions | Any need for an invented network provider, project-authored RAP/SMB stack, private RPC/CSRSS/kernel recreation, raw pointer/handle identity, new mapping manager or unreviewed source import pauses for re-admission. |
| Exit Criteria | A complete selected NetAPI/RAP owner baseline permits one bounded binding-recovery S without treating a host-side link as guest network success. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Workstation/user/computer queries, downlevel `XsNet*` and `Rx*` paths, remote request handling, status maps, fixed guest buffers, NetAPI allocations, mapping leases, feature-disabled outcomes and transfers to NetBIOS/DLC/VDD. |

**S1 plan:** [the NetAPI/RAP Redirector network-boundary proposal](etc/operations/proposal-netapi-rap-redirector-network-boundary-recovery-001.md) is the binding package plan. S1 first turns its complete original cohort into a physical caller/ABI/lifetime/failure ledger; no implementation is selected by a trace or one convenient API.

**S1 P1:** [the NetAPI/RAP physical-boundary baseline](etc/evidence/m0-t325-s1-netapi-rap-physical-boundary-baseline-001.md)
and its [18-row physical-edge ledger](etc/operations/m0-t325-s1-netapi-rap-physical-edge-ledger.tsv)
separate the bounded public `NetWksta*`/`NetApiBufferFree` candidates and the
selected original `ntstatus.c` mapper from every Xactsrv/Rx/SMB-RAP protocol
edge. No host-side API is yet claimed guest-network equivalent.

**S1 closed:** the complete reached cohort now has one physical source-first
disposition per call group. The only next recovery is the bounded public
workstation-query subset; its predecessor fixture route is historical-only
because its private pointer scope has been retired.

### M0 T325 S2 — Public workstation-query lease recovery

**Closed: M0 T325 S2**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T325 S2; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | T325/S1 closure under the standing owner direction to execute the ordered queue. |
| Objective | Restore the original `VrGetComputerName`, bounded `VrGetUserName(BX=1)` and `VrRemoteApi` local-computer fallback through the existing synchronous session guest-memory lease, preserving original calls, register results, sizes and status branches. |
| Non-goals | No `BX=0` unbounded username form, RAP/SMB/Xs/Rx success, remote API, NetBIOS/DLC/VDD/RPC/CSRSS recreation, new mapper/pointer scope, raw host identity, guest edit or other local Redirector redesign. |
| Reference Baseline | T325/S1 physical ledger; T324 local closure; original `vrnetapi.c`/`vrremote.c`; historical T290 evidence retained only as archival comparison; existing `mvdm_guest_location` API. |
| Files And ABI Surface | The two original `ES:DI` copies and `vrremote.c` local-computer fallback; current SoftPC location/lease facade; a minimal Redirector overlay or registered mirror divergence; focused x86/x64 fixture and Ninja source selection. |
| Applicable Rules | Execution, source-first recovery, mirror/overlay, source policy, architecture and session mapping-manager rules. |
| Verification | Original-call/order/diff review; x86/x64 compile/link; focused positive/negative lease fixture; stale-session negative check; governance and diff review. |
| Expected Markers | One acquire/copy/release transaction at each selected original guest write; no lease crosses asynchronous work; original insufficient-buffer and error registers remain observable. |
| Asset Needs | Existing selected mirrors, session guest-memory lease, public `netapi32` APIs and formal Ninja; no external network service. |
| Reporting Requirements | State every retained original branch, lease access/size/commit, public buffer free, error mapping and explicit non-admitted sibling path. |
| Stop Conditions | A need for a private guest mapper, durable native pointer, source-wide `LPSTR_FROM_WORDS` rewrite, changed DOS ABI, public-API semantic mismatch or remote/RAP enablement pauses for re-admission. |
| Exit Criteria | All three selected public branches compile and pass focused positive/negative tests on x86/x64 through the existing lease; no historical pointer-scope production reference remains. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | All selected `ES:DI` string writes, `NetApiBufferFree` pairing, explicit capacity gates, register/carry result paths, session teardown and source-selection inputs. |

**S2 plan:** [the public workstation-query lease recovery plan](etc/operations/m0-t325-s2-public-workstation-query-lease-recovery-plan-001.md) retains the original code body and confines each required change to a registered, copied synchronous guest-memory crossing.

**S2 closed:** [the workstation-query lease recovery evidence](etc/evidence/m0-t325-s2-public-workstation-query-lease-recovery-001.md)
records the two selected `ES:DI` result writes, the local RAP fallback's
fixed-capacity OEM conversion, the declaration-only `apiworke.h` import and
formal x86/x64 build-plus-fixture evidence. The later RAP/SMB request is still
an explicit unavailable boundary; no remote-network claim is made.

**T325 closed:** its complete selected public workstation-query subset is now
source-shaped, bounded and verified on both target architectures. The package
does not include the unselected RAP/SMB/Xs/Rx/NetBIOS/DLC/VDD provider families.

### M0 T324 S1 — Original Redirector owner and boundary baseline

**Closed: M0 T324 S1**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T324 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 5, admitted under the standing owner request to execute the ordered queue after T323 closure. |
| Objective | Establish the complete original VDMREDIR/Redirector package boundary, including local file, pipe, NetAPI/RAP, lifecycle, handle and selected `opennt-host/netapi/netlib/ntstatus.c` dependencies, with one source-shaped disposition per reached external edge. |
| Non-goals | No trace-selected 57:xx provider, network redirector rewrite, private NetAPI/RAP/CSRSS reconstruction, host global handle mutation, new mapping manager, guest change, or synthetic remote success. |
| Reference Baseline | T323 closure; queue candidate 5; selected original `mvdm-host/vdmredir` package, `dos/command/cmdredir.c`, `inc/vdmredir.h`, selected `opennt-host/netapi/netlib/ntstatus.c` mirror slice, session mappings and existing host-out adapter families. |
| Files And ABI Surface | Original VDMREDIR dispatcher, init, local completion, named-pipe, mailslot, NetBIOS, remote and buffer sources; BOP/COMMAND callers; original/modern file, pipe, module, NetAPI and status declarations. |
| Applicable Rules | Execution, source-first recovery, mirror/overlay, architecture, source-policy, session mapping-manager and host-capability rules. |
| Verification | Complete source/definition/caller review, original source-selection and build-manifest review, reached external declaration comparison, focused source disposition checks, formal x86/x64 graph review, governance and diff review. |
| Expected Markers | A bounded ledger separates original local file/pipe paths, public-modern bindings, selected `ntstatus.c` mirror calls, explicit remote/RAP/device unavailability and all host/guest identity lifetimes. |
| Asset Needs | Existing selected mirrors, current adapter/session components and formal CPU40 product graph; no new guest media, host installation or kernel facility. |
| Reporting Requirements | Record original caller, package/file, request/response layout, handle or pointer lifetime, direct public API candidate, selected original mirror dependency, failure disposition and later owner for every reached edge. |
| Stop Conditions | A need for a second redirector implementation, a generic adapter, unbounded guest/native pointer alias, kernel/CSRSS recreation, unreviewed external package import or invented network success pauses for re-admission. |
| Exit Criteria | A complete source/ABI/failure ledger covers the selected Redirector owner family and permits one bounded recovery S without an invented backend. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | File/pipe/mailslot/NetBIOS/remote branches, `cmdredir.c` completion records, token lifetime, local versus remote state, module loading, legacy host APIs, x86/x64 width and source-shaped unavailable paths. |

**S1 P1 baseline:** [the original Redirector owner/binding baseline](etc/evidence/m0-t324-s1-original-redirector-owner-binding-baseline-001.md)
preserves all 16 original VDMREDIR source units as one owner family, separates
the local file/pipe cohort from VDD/device/RAP boundaries, and identifies the
already-selected byte-identical `ntstatus.c` slice, now complete in
`opennt-host`.

**S1 P2:** [the Redirector source-unit disposition ledger](etc/evidence/m0-t324-s1-redirector-unit-disposition-ledger-001.md)
now records every original unit, the COMMAND caller and the selected external
status source. It distinguishes the bounded local cohort from explicit
VDD/DLC/NetBIOS/RAP transfers without enabling a 57:xx leaf.

**S1 closed:** the selected Redirector package has a complete source/ABI/failure
ledger and a byte-identical `ntstatus.c` mirror. Its only admitted next step is
the complete local source cohort; it may not enable a standalone service or
invent a remote/device backend.

### M0 T324 S2 — Original local Redirector binding recovery

**Closed: M0 T324 S2**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T324 S2; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | T324/S1 closure and the standing owner request to execute the ordered queue. |
| Objective | Recover the complete original local Redirector cohort—dispatcher, utilities, mailslots, named-pipe control flow, COMMAND redirection caller and selected status conversion—only through original source and same-shaped bounded bindings. |
| Non-goals | No trace-selected 57:xx leaf, RAP/remote/NetBIOS/DLC provider, VDD hook success, generic adapter, new mapping manager, raw host identity, guest edit, or synthetic interrupt/network result. |
| Reference Baseline | T324/S1 ledgers; `m0-t324-s2-local-redirector-binding-recovery-plan-001.md`; original `vdmredir`/`cmdredir.c`; selected `ntstatus.c`; existing session, SoftPC and Redirector identity bindings. |
| Files And ABI Surface | `vrdisp.c`, `vrmisc.c`, `vrputil.c`, `vrmslot.c`, `vrnmpipe.c`, `cmdredir.c`, `ntstatus.c`, `vdmredir.h`, original async records, current mapping identity facade and SoftPC/ICA completion boundary. |
| Applicable Rules | Execution, source-first recovery, mirror/overlay, source-policy, session mapping-manager and host-capability rules. |
| Verification | Source/declaration/definition/caller comparison, source-shaped completion-contract record, selected x86/x64 graph review, focused local positive/negative checks, governance and diff review. |
| Expected Markers | Original dispatch and request layouts remain authoritative; every external identity uses the existing session mapping; asynchronous completion has one explicit source-shaped owner/teardown result. |
| Asset Needs | Existing selected mirrors, current session and same-shaped adapters, formal CPU40 product graph; no new guest media, network service, VDD or kernel facility. |
| Reporting Requirements | For every local source edge, record original call order, request/result fields, host/guest identity and span lifetime, pending/resume/cancel order, public API/failure disposition and later owner. |
| Stop Conditions | Need for a second Redirector provider, private Network/RAP/CSRSS/VDD recreation, unbounded pointer/handle alias, invented interrupt completion or unreviewed external import pauses for re-admission. |
| Exit Criteria | The complete local cohort is bound through original source or transferred with exact source-shaped evidence; focused checks and formal graph review distinguish link closure from any guest runtime claim. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | All local dispatcher, file/pipe/mailslot, COMMAND redirection, identity, mapping lease, worker, completion, cancel and teardown forms; reject all remote, VDD, DLC and NetBIOS shortcuts. |

**S2 plan:** [the original local Redirector binding recovery plan](etc/operations/m0-t324-s2-local-redirector-binding-recovery-plan-001.md)
fixes the whole selected local cohort before any source change, and reserves
all remote/device families for their named owners.

**S2 P1:** [the original named-pipe completion contract](etc/evidence/m0-t324-s2-named-pipe-completion-contract-001.md)
proves that a cdecl thread thunk alone cannot enable `VrReadWriteAsyncNmPipe`:
the original completion persists guest locations and requires its SoftPC/ICA
interrupt route. The existing unavailable worker remains the exact safe result
until a source-shaped copied completion transaction is admitted.

**S2 P2:** [the local Redirector guest-span and ICA binding](etc/evidence/m0-t324-s2-local-redirector-guest-span-ica-binding-001.md)
proves that the selected original SoftPC ICA route already exists, while
`GetVDMAddr`'s historical durable-pointer assumption does not. The only
admitted recovery is a narrow Redirector-adapter transaction that uses existing
session leases at every guest-memory crossing; it preserves the original
VDMREDIR queue and interrupt sequence without creating a provider.

**S2 P3:** [the local Redirector source closure](etc/evidence/m0-t324-s2-local-redirector-source-closure-001.md)
selects and compiles the complete admitted local cohort on x86 and x64, links
and executes its one staging/lease/termination contract fixture, and records
the source-owned worker teardown and existing ICA boundary. It is strictly a
local source closure: it neither enables a 57:xx BOP nor claims remote,
device, VDD or guest-runtime completion.

**S2 P4:** retired the stale T290 pointer-scope generators and fixtures into
`docs/etc/legacy_code/retired-redirector-pointer-scope/`.  They referenced a
deliberately retired, unselected facade; the active local cohort uses the
source-shaped T324/S2 overlay transaction instead.  The active T310 lease
ledger now records that disposition, so no current build or test input is
allowed to revive the obsolete path.

**S2 closed / T324 closed:** the entire admitted local Redirector cohort now
has one source-owned local binding route, checked asynchronous guest-span
transactions, source-shaped ICA completion and teardown, selected x86/x64
source-graph closure, a focused contract fixture, and no live dependency on
the rejected pointer-scope experiment.  Remote/RAP/NetBIOS/DLC/VDD and a
native guest 57:xx execution claim remain explicitly outside this closure.

### M0 T323 S1 — Original DPMI/DPMI32 owner and boundary baseline

**Closed: M0 T323 S1**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T323 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Ordered queue candidate 4, admitted under the standing owner request to execute the queue. |
| Objective | Establish the complete original DPMI and DPMI32 source/package boundary, its protected-context, monitor, mapping and SoftPC interfaces, and the smallest source-shaped disposition for every reached external edge. |
| Non-goals | No protected-mode execution rewrite, BOP trace repair, new CPU/monitor, synthetic interrupt, new guest-pointer mapper, or direct kernel-VDM/CSRSS recreation. |
| Reference Baseline | T322 closure; `mvdm-host/dpmi` original Win16/DOSX source and `mvdm-host/dpmi32` original host provider source; selected CPU40 product graph. |
| Files And ABI Surface | Original `dpmi32` source manifest plus `dpmimemr.c`/`dpmimscr.c`, original `dpmi` assembly carrier, OpenNT `VDM_TIB` declarations, existing monitor/SoftPC/session binding surfaces, and formal product source selection. |
| Applicable Rules | Execution, architecture, coding, mirror-divergence, source-policy and shared mapping-manager rules. |
| Verification | Source/definition/caller review, original source-selection review, x86/x64 formal DPMI/product graph review, documentation governance and diff review. |
| Expected Markers | One owner ledger classifies protected context, LDT, interrupt/exception, memory, VxD and DOSX/guest edges as direct, same-shaped adapter, source-shaped unavailable or later-owner transfer. |
| Asset Needs | Existing OpenNT mirrors, OpenNT ABI mirror, selected CPU40 product graph, session mapping manager and current adapters; no guest or external machine change. |
| Reporting Requirements | Separate DPMI32 host provider from the DOSX/guest assembly carrier; record each raw pointer/address conversion, monitor dependency, architecture conditional, external owner and failure disposition. |
| Stop Conditions | Need for a DPMI-private mapper, kernel VDM recreation, CPU semantics change, invented protected-mode exception success, or a second executor pauses for re-admission. |
| Exit Criteria | A bounded source/ABI/failure ledger covers the complete selected DPMI/DPMI32 family and permits the next recovery S without an invented backend. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | DPMI allocation/free, descriptor/LDT operations, interrupt/fault registration, VxD controls, address/handle lifetimes, stale session, x86/x64 architecture gates and DOSX guest/host separation. |

**S1 closed:** [the original DPMI owner/binding baseline](etc/evidence/m0-t323-s1-original-dpmi-owner-binding-baseline-001.md)
separates the host DPMI32 provider from the DOSX guest carrier, preserves the
original 25-entry table, and identifies the real next boundary: the complete
shared guest-span contract—not a trace-selected DPMI service.

### M0 T323 S2 — DPMI shared guest-span and monitor binding recovery

**Closed: M0 T323 S2**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T323 S2; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | T323/S1 source baseline and standing ordered-queue authority. |
| Objective | Recover one complete source-shaped DPMI guest-span and monitor-binding cohort for the original provider without turning `Sim32GetVDMPointer` into a durable raw host alias. |
| Non-goals | No per-BOP provider, second mapper, raw guest pointer publication, synthetic protected interrupt/fault delivery, DOSX guest change, or kernel VDM recreation. |
| Reference Baseline | T323/S1 baseline; existing session mapping manager, monitor TIB binding, original `dpmi32` source and formal CPU40 product graph. |
| Files And ABI Surface | Reached `Sim32GetVDMPointer` call sites in `buffer.c`, `dpmi32.c`, `dpmiint.c`, `dpmiselr.c`, `int21map.c`, `modesw.c`, `stack.c`, `dpmimemr.c` and x86 comparison bodies; monitor `VdmTib`/`NtVdmControl` interfaces and original source headers. |
| Applicable Rules | Execution, source-first recovery, mirror/overlay, architecture and shared mapping-manager rules. |
| Verification | Call-site lifetime classification, original/adapter declaration comparison, focused positive/negative scoped-span proof, formal x86/x64 selected source links, governance and diff review. |
| Expected Markers | Every reached raw pointer form has a direct scoped binding, a minimal mirror overlay with registered divergence, or exact named unavailable transfer; no native pointer crosses a DPMI/guest ABI. |
| Asset Needs | Existing mirrors and adapter/session components only; no new guest media, kernel facility or emulator. |
| Reporting Requirements | Record address form, length, read/write direction, pointer arithmetic lifetime, flush/free behavior, TIB relation and exact continuation/failure rule for each classified call site. |
| Stop Conditions | A requirement for a generic durable alias, new DPMI-private mapping manager, implicit cross-thread lease, changed CCPU execution semantics, or invented `NtVdmControl` success pauses for re-admission. |
| Exit Criteria | One complete source-shaped guest-span cohort is bound or transferred with focused evidence; it permits later dispatcher/mode integration without a hidden raw-pointer backend. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | All ten `Sim32GetVDMPointer` provider files, direct pointer arithmetic, stack/IVT/DTA/FCB buffers, VDM_TIB fields, descriptor references, cancellation/teardown and x86/x64 link forms. |

**S2 P1 baseline:** [the DPMI guest-span lifetime ledger](etc/evidence/m0-t323-s2-dpmi-guest-span-lifetime-ledger-001.md)
separates immediate SoftPC-private pointer use from persistent DPMI state,
the `IntelBase`/`FlatAddress` width contract, the unavailable kernel-monitor
operation, and later DOS re-entry.  The next bounded recovery is the complete
`IntelBase`/`FlatAddress` contract; no generic DPMI mapper or synthetic
monitor success is admitted.

**S2 P2:** `MVDM-HOST-DIV-140` restores the complete private
`IntelBase`/`FlatAddress` host-width contract in the original DPMI32 mirror.
The selected original DPMI library and product link pass on x86 and x64.  The
same evidence retains `VdmSetInt21Handler` and DOS/DEM re-entry as exact
later-owner transfers.  The source/lifetime ledger plus formal x86/x64 build
matrix meet this bounded S's closure evidence; they do not claim DPMI guest
runtime, protected interrupt delivery or DOSX/DEM re-entry completion.

**Closed: M0 T323.** The original DPMI/DPMI32 provider now has a complete
selected-source boundary ledger and its one shared guest-span/host-address
cohort is source-shaped, registered and formally linked.  The next work must
admit an owner package for one of the named monitor or DOS-runtime transfers,
not reopen this closure as a per-BOP repair.

### M0 T322 S1 — Original XMS ownership and memory-binding baseline

**Closed: M0 T322 S1**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T322 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 3, **XMS owner package**, admitted under the standing owner request to execute the ordered queue. |
| Objective | Establish the complete original XMS owner/package boundary—dispatcher, allocator, A20, UMB, INT15 and guest-memory access—and identify the smallest same-shaped mapping/SoftPC binding for each reached external interface. |
| Non-goals | No trace-selected BOP implementation, synthetic UMB/A20/INT15 success, guest edit, CPU40 recursion repair, Bochs route, or new family-private mapper. |
| Reference Baseline | Queue candidate 3; original `mvdm-host/xms` mirror; shared session mapping-manager architecture; T321 closed vertical-slice matrix. |
| Files And ABI Surface | Original `xms.c`, `xmsdisp.c`, `xmsblock.c`, `xmsumb.c`, `xmsmem86.c`, reached headers and the existing adapter-mvdm-host-out SoftPC mapping interface. |
| Applicable Rules | Execution, architecture, coding and source policy; mapping-manager and mirror-divergence requirements. |
| Verification | Source/definition/caller review, formal x86/x64 XMS/product graph review, focused positive/negative package checks, governance verification and diff review. |
| Expected Markers | One source-first ledger gives each XMS external edge a direct, same-shaped adapter, explicit unavailable, or later-owner disposition. |
| Asset Needs | Current MVDM mirrors, existing mapping manager and formal build generators; no new guest image or external machine. |
| Reporting Requirements | Separate original XMS algorithm from historic raw host-pointer backend; record address/span/direction/lease lifetime for every mapping edge. |
| Stop Conditions | A new mapper, changed XMS allocator semantics, synthetic hardware completion, or broad host capability expansion pauses for re-admission. |
| Exit Criteria | A bounded source/ABI disposition permits the next XMS recovery S without an invented backend. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Review allocation, lock/unlock, move, handle lifetime, A20, UMB and INT15 as one owner package. |

**S1 closed:** [the original XMS owner/binding baseline](etc/evidence/m0-t322-s1-original-xms-owner-binding-baseline-001.md)
keeps the XMS algorithms in their mirror and identifies only the historic raw
host-pointer backend as replaced by the existing same-shaped shared mapping
binding.  It does not claim UMB/INT15 runtime completion.

### M0 T322 S2 — XMS dispatcher and mapped-memory package checks

**Closed: M0 T322 S2**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T322 S2; Ordinary Mode. |
| Admission And Approval | T322/S1 baseline; queue candidate 3. |
| Objective | Verify original XMS dispatcher routing and the selected same-shaped mapped-memory callbacks across supported and negative inputs. |
| Non-goals | No new XMS provider, allocator rewrite, UMB/INT15 synthetic completion, or trace-selected repair. |
| Reference Baseline | T322/S1 evidence and formal `MVDM_XMS_SESSION_BACKEND` graph. |
| Files And ABI Surface | Original XMS sources, existing XMS memory/A20/UMB bindings, shared session mapping manager and focused tests only. |
| Verification | Formal x86/x64 XMS/product links and focused dispatcher/callback tests including invalid span/descriptor cases. |
| Expected Markers | Original table routing; bounded lease success and rejection; original register failure behavior. |
| Stop Conditions | Any need for an XMS-private mapper or invented machine interrupt behavior. |
| Exit Criteria | Checked source-shaped XMS memory/dispatcher closure or an exact UMB/INT15 owner transfer. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Move overlap/direction, invalid descriptor, stale session, A20 state, UMB no-space and INT15 hook edges. |

**S2 P1:** [the original XMS static build](etc/evidence/m0-t322-s2-original-xms-static-build-001.md)
compiled all six original common units together with the selected same-shaped
memory callback binding. It did not claim a dispatcher or runtime result.

**S2 P2 / S2 closed:** [the dispatcher and mapped-memory checks](etc/evidence/m0-t322-s2-xms-dispatcher-mapped-memory-checks-001.md)
retain the original dispatcher table and callback order, prove the shared
session lease binding on x86 and x64 (including a cross-lease overlap move and
negative descriptor), and relink both selected products. UMB/INT15 hardware
delivery remains the pre-existing named CPU40/SAS/ICA owner transfer; no
synthetic completion was added.

**Closed: M0 T322.** The original XMS owner package has its common source,
dispatcher, and only required guest-address adapter closure. This closure is
local source/ABI evidence, not a claim of guest-wide XMS runtime completion.
### M0 T321 S1 — Vertical-slice ownership and bypass baseline

**Closed: M0 T321 S1**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T321 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 2, **Minimal vertical-slice integration closure**, admitted under the standing owner request to execute the ordered queue after T320 closed. |
| Objective | Establish one current, source-defined ownership/bypass baseline for the selected first slice: original CPU40, Base VDM, DEM/COMMAND, immutable guest media, and the declared return contract. |
| Non-goals | No new BOP/provider, guest edit, synthetic FDC/PIC completion, debugger, alternate CPU/machine, or source-wide interface sweep. |
| Reference Baseline | T318 frozen guest return; T319 ROM/termination evidence; T320 CPU40 recursive/device-event owner transfer; Queue candidate 2. |
| Files And ABI Surface | Current selected formal Ninja manifests, original CPU40/BOP/Base VDM/DEM/COMMAND call surfaces, session mapping leases, staged firmware/media manifest, and fixed observer only. |
| Applicable Rules | `docs/rules/EXECUTION.md`, source policy, architecture/coding rules, and mirror exception registers. |
| Verification | Static source/build-edge review, x86/x64 formal normal-link checks, fixed x86 console-owning observation, documentation governance gate, and `git diff --check`. |
| Expected Markers | One ledger distinguishes original completed edges from the still-open CPU40 recursive execution/device-event edge, and proves no app/adapter bypass has replaced it. |
| Asset Needs | Existing mirrored source, staged immutable firmware/DOS media, T318--T320 evidence, and current formal build scripts. |
| Reporting Requirements | Report every slice edge as direct original, same-shaped binding, explicit unavailable path, or remaining owner transfer; distinguish link closure from runtime continuity. |
| Stop Conditions | A required new provider, synthetic device result, changed guest media/container, or broadened external package pauses for re-admission. |
| Exit Criteria | A complete first-slice owner/bypass ledger and fixed integration matrix are recorded; any remaining failure is one exact original owner transfer. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Check CPU entry/recursive re-entry, BOP ingress, Base VDM command source, DEM/COMMAND media use, guest artifact provenance, result return, and app/session ownership together. |

**S1 closed:** [the vertical-slice owner/bypass baseline](etc/evidence/m0-t321-s1-vertical-slice-owner-bypass-baseline-001.md)
confirms that the selected formal product retains one original CPU40-to-guest
path and no Bochs, `src.old`, app-owned BOP/DEM/COMMAND, or guest-loader
bypass.  T321/S2 is limited to the fixed matrix on that same graph.

### M0 T321 S2 — Fixed vertical-slice integration matrix

**Closed: M0 T321.**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T321 S2; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | T321/S1 closed the selected source/bypass baseline; this S executes only its stated fixed matrix. |
| Objective | Rebuild/link the selected CPU40 product under x86 and x64, then run one unchanged fixed x86 console-owning observation to classify current first-slice continuity. |
| Non-goals | No source repair, BOP/provider change, guest/media/container/debugger change, synthetic device event, or alternate machine route. |
| Reference Baseline | T318--T320 evidence and T321/S1 baseline. |
| Files And ABI Surface | Formal Ninja generator, immutable staging manifest, existing console observer, and the selected original process link only. |
| Verification | Fresh x86/x64 normal links; fixed staged x86 observation; manifest/output review; documentation governance and `git diff --check`. |
| Expected Markers | Two normal link rows and one bounded observation result attributable to the existing CPU40 owner transfer or an advance beyond it. |
| Stop Conditions | Any required product behavior change or altered observation input pauses S2. |
| Exit Criteria | Exact fixed-matrix evidence, with no overclaim beyond observed continuity. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Confirm x86/x64 graph identity, media manifest identity, observer arguments, and absence of retired inputs. |

**S2 closed:** [the fixed vertical-slice matrix](etc/evidence/m0-t321-s2-fixed-vertical-slice-integration-matrix-001.md)
records both normal x86/x64 links and the unchanged x86 `0xc0000005` result.
It confirms the T320 CPU40 recursive-execution/device-event owner transfer;
it does not claim DOS execution or controlled return.  This meets the
candidate's exact-owner-transfer exit without introducing a bypass.

### M0 T320 — closure record

**Closed: M0 T320.**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T320 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | Queue candidate 1, **Minimal DOS guest execution and controlled return**, follows T318's frozen guest-return contract and T319's source-defined CPU40 raw-worker transfer. |
| Objective | Establish the exact original CPU40 machine/worker prerequisite between the reached startup markers and the frozen first DOS guest load/return boundary, before any behavior change. |
| Non-goals | No BOP/DEM/COMMAND/XMS provider patch, guest source change, app-owned execution replacement, synthetic FDC/PIC/device success, debugger attachment, changed fixed observation container, or speculative worker rewrite. |
| Reference Baseline | [T318 EXEC contract](etc/evidence/m0-t318-s1-ntdos-exec-parent-return-contract-freeze-001.md); [T319 closure](history/m0-t319-softpc-rom-residency-and-termination-closure-20260831.md); [T320 proposal](etc/operations/proposal-minimal-dos-guest-execution-controlled-return-001.md). |
| Machine Profile | Selected original `CPU_40_STYLE`/CCPU40 only; CPU30 and Bochs are retired evidence, never an implementation or fallback route. |
| Files And ABI Surface | Original CPU40 entry/recursive execution, original raw `CreateThread` users, original worker filters/lifetimes, ROM/BIOS callback paths, and frozen NTDOS/COMMAND return source contracts. |
| Applicable Rules | `docs/rules/EXECUTION.md`, `docs/rules/ARCHITECTURE.md`, `docs/rules/CODING.md`, source policy and mirror exception registration. |
| Verification | Source call/definition review, selected build-manifest review, reusable current fixed observation evidence, documentation governance verification and `git diff --check`. |
| Expected Markers | One source map distinguishes current-thread CPU40 execution from each reachable raw-worker route and names the first prerequisite before guest load; it does not claim execution from BOP markers alone. |
| Asset Needs | Existing original MVDM/guest mirrors, CPU40 graph, staged immutable media, T318/T319 evidence. |
| Reporting Requirements | Record every reachable path's owner/disposition and name a single earliest recovery cohort or exact owner transfer. |
| Stop Conditions | Need for a debugger, changed observer/media/arguments, guest/BOP/device behavior change, or a second machine backend pauses S1 for explicit re-admission. |
| Exit Criteria | A source-defined first prerequisite and its complete immediate caller/callee contract are recorded, with no new runtime behavior. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Compare original main-thread CPU40 execution, host-created filtered threads, raw event/heartbeat/floppy/detection/comms threads, and guest-entry call sites as one machine-to-guest gate. |

**S1 closed:** [the CPU40-to-guest prerequisite contract](etc/evidence/m0-t320-s1-cpu40-to-guest-prerequisite-contract-001.md)
proves NTIO load/CS:IP assignment precedes the original CCPU run and narrows
the remaining immediate recovery scope to the original FDC/INT15 recursive
CPU40 cohort.  It does not claim declared-DOS-program execution.

**S2 closed:** [the FDC/INT15 disposition](etc/evidence/m0-t320-s2-fdc-vector-prerequisite-disposition-001.md)
proves that ROM residency and the original default `wait_int`/INT15 vector are
already present.  It records the remaining raw CPU40 recursive-execution and
device-event owner transfer without introducing synthetic FDC, PIC, BOP, or
guest behavior.  This satisfies T320 exit 2; no declared-DOS-program execution
is claimed.

## Historical retained T318 delivery sequence

T318 is closed by its declared owner-transfer exit.  Its compact closure is
in [history](history/m0-t318-ntdos-ordinary-child-owner-transfer-closure-20260831.md);
the retained P records below are indexed delivery evidence, not an active
packet.

### M0 T318 S1 closure record

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T318 S1; Ordinary Mode (single-person dual-role implementation). |
| Admission And Approval | The owner's standing objective authorized queue item 1 after T317 closure. |
| Objective | Freeze the original guest-owned `EXEC -> PSP/arena/JFN/environment -> parent restore -> 54:0B` chain and its exact SoftPC/host prerequisites before any recovery edit. |
| Non-goals | No guest behavior change, host COMMAND provider change, BOP-by-trace patch, app/session lifecycle invention, device expansion, Redirector/WOW/DPMI work, or x64 recovery. |
| Reference Baseline | [T317 closure](history/m0-t317-dem-command-softpc-vertical-slice-closure-20260830.md); [T318 proposal](etc/operations/proposal-ntdos-command-guest-exec-parent-return-001.md); selected x86 `CPU_40_STYLE`/CCPU40 SoftPC path. |
| Files And ABI Surface | `src/mvdm-guest/dos/v86` original NTDOS/COMMAND source and staged images; `src/mvdm-host/dos/command/{cmdexec.c,cmdexit.c}`; the existing `54:0B` return/result ABI; resulting evidence only unless a source-identity correction is needed. |
| Applicable Rules | `docs/rules/EXECUTION.md`, `docs/rules/ARCHITECTURE.md`, `docs/rules/CODING.md`, source policy, and mirror-component exception rules. |
| Verification | Source call-chain and artifact provenance review; cross-check against the selected x86 CPU40 graph; documentation governance verification and `git diff --check`. |
| Expected Markers | One original owner map identifies each guest routine, state structure, return transition, reached host result input, and each non-guest prerequisite with a named owner/disposition. |
| Asset Needs | Existing complete DOS guest mirror and staged guest artifacts; original MVDM mirror; no new external source import. |
| Reporting Requirements | Record direct/composable/deferred dispositions and distinguish a frozen source contract from runtime continuity. |
| Stop Conditions | Discovery that the declared profile requires a new owner package, different guest image, or non-CPU40 machine route pauses recovery for owner re-admission. |
| Exit Criteria | Evidence identifies a bounded original recovery cohort and all prerequisites; no invented guest logic or broadened boundary. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务。” |
| Similar-Issue Sweep | Inspect ordinary and failure returns, COM/EXE paths, parent/child JFN and environment restoration, and the host `54:0B` input handoff as one lifecycle contract. |

**S1 closed:** the original NTDOS `INT 21h/4Bh` and parent-return cohort is
frozen in [the contract map](etc/evidence/m0-t318-s1-ntdos-exec-parent-return-contract-freeze-001.md).
It proves that PSP/arena/JFN/environment and parent restoration remain in the
unmodified guest mirror, verifies the exact staged NTDOS/COMMAND artifact
identities, and identifies original DEM `0x11`/`0x36`/`0x3C`, COMMAND `54:0B`,
and CPU40 SoftPC transitions. No guest execution claim is made.

**S2 P1:** selected CPU40 CCPU recursive simulation now preserves the original
external `host_simulate` and BIOS `host_unsimulate` interfaces, while retaining
the in-header CCPU vector.  The fresh formal archive completed 408 actions and
the complete-member forced-link audit removed five reached CPU interfaces
without inventing a loader or BOP provider.  The ten remaining unresolved
whole-archive symbols are explicit nonlocal owner transfers; no NTDOS or child
runtime claim is made.  See [P1 evidence](etc/evidence/m0-t318-s2-p1-cpu40-recursive-simulation-linkage-001.md).

**S2 P2:** original DEM now receives the same session-selected immutable DOS
media root as the existing NTIO loader, without changing its original NTDOS
filename, file-I/O, or guest-write sequence.  Formal CPU40 archives and the
existing bounded media-resource verification both pass; this is a loader-root
closure, not a `50:11` or NTDOS execution claim.  See [P2 evidence](etc/evidence/m0-t318-s2-p2-original-dem-media-root-001.md).

**S2 P3:** the current selected CPU40 source graph archives under both x86 and
x64 through the generated one-time-MSVC, Ninja-`-j 8` runner. Three historic
host-local RTL spellings and the original CCPU descriptor decoder now have one
same-shaped binding each, and the non-`/FORCE` product now links in both rows.
Private debugger/WOW/VDD/generated-CCPU boundaries retain their original
callable ABI but controlled-stop only a session when actually reached; no empty
provider or runtime-completeness claim was introduced. See [P3 evidence](etc/evidence/m0-t318-s2-p3-dual-width-normal-link-boundary-001.md).

**S2 P4:** the first original product-entry observation found an omitted
original SoftPC string resource, not a CPU or DOS failure. The unchanged
`obj.vdm/resource.rc` now compiles and links per host width; both normal product
links pass. The original `-f` direct-launch contract is confirmed, and a
watchdog-bounded x86 `-f` run stays alive without a crash. It is liveness only,
not NTIO/NTDOS or child-return evidence. See [P4 evidence](etc/evidence/m0-t318-s2-p4-original-resource-and-startup-contract-001.md).

**S2 P5:** pipe-captured and hidden-console `-f -o` observations, including a
single missing-`NTIO.SYS` counterfactual, do not validly test the DOS-media
loader. The original retired installable-VDD registry key is absent on the
modern host; its selected original query/enumeration body now accepts an empty
set only for that absence, while other failures retain the original error path.
Both product links pass, but bounded liveness remains only liveness; the next
owner boundary is original `scs_init` / Base VDM command acquisition. See
[P5 evidence](etc/evidence/m0-t318-s2-p5-console-observation-admission-001.md).

**S2 P6:** app now binds the existing source-shaped Base VDM adapter before
original `scs_init`; explicit `--ordinary-child` declares the selected
`dos/COMMAND.COM /C VER` through its copied `VDMINFO` record and is stripped
before original argument parsing. The selected CPU40 product links in both
widths and both graphs are incrementally clean. The bounded x86 run is still
liveness only: it does not prove NTIO, NTDOS, EXEC or parent return. See
[P6 evidence](etc/evidence/m0-t318-s2-p6-base-vdm-ordinary-child-declaration-001.md).

**S2 P7:** the selected original non-V86 CCPU40 carrier now retains its
`V7VGA` configuration, selecting the byte-exact retained `v7vga.rom` rather
than requesting unavailable `vga.rom`. Fresh x86/x64 formal graphs link and
are incrementally clean. An isolated x86 run passed that former firmware
block, then exposed the later original `The handle is invalid` startup error.
This is not an NTIO/NTDOS/COMMAND/EXEC claim. See [P7 evidence](etc/evidence/m0-t318-s2-p7-v7vga-firmware-configuration-recovery-001.md).

**S2 P8:** the later `The handle is invalid` dialog is now attributed to the
pipe-owned observer supplying no usable standard console handles to original
`InitScreenDesc`/console-mode setup; it is not a product-side blocker. No
source change was admitted. Further continuity observation requires a genuine
console-owning harness or interactive console. See [P8 evidence](etc/evidence/m0-t318-s2-p8-headless-console-observation-attribution-001.md).

**S2 P9:** a disposable console-owning x86 observation launcher supplied
original `CONIN$`/`CONOUT$` handles to the CPU40/V7 VGA product. The original
process remained alive for the bounded five-second interval (`STILL_ACTIVE`)
with no error dialog. This confirms P8's headless attribution, but is still
not an NTIO/NTDOS/COMMAND/EXEC/parent-return claim. See [P9 evidence](etc/evidence/m0-t318-s2-p9-real-console-startup-liveness-001.md).

**S2 P10:** real-console counterfactuals now prove the selected process reaches
the original `ntio.sys` load decision, and that removing `COMMAND.COM` affects
the process only when the admitted `--ordinary-child` declaration is present.
No `/C VER` completion was observed, so guest EXEC/parent return remains open.
See [P10 evidence](etc/evidence/m0-t318-s2-p10-original-input-counterfactuals-001.md).

**S2 P11:** the intact real-console CPU40 profile accumulated approximately
2703 ms of CPU time within a five-second window while remaining alive. Together
with the original `InitialiseDosEmulation → host_start_cpu → cpu_simulate`
sequence and P10 counterfactual, this proves entry into original CCPU execution,
not COMMAND completion. See [P11 evidence](etc/evidence/m0-t318-s2-p11-original-cpu-execution-admission-001.md).

**S2 P12:** the selected CPU40 source/composition path from BOP `0x50` through
original `MS_bop_0`, `SVC_DEMLOADDOS (0x11)`, `DemDispatch`, and `demLoadDos`
is now fixed in evidence. Both formal product graphs include the original
CCPU, BIOS, host BOP, and DEM units. This proves no replacement loader/router
is required; it does not yet prove guest arrival or NTDOS execution. See
[P12 evidence](etc/evidence/m0-t318-s2-p12-original-ntio-demload-dos-dispatch-map-001.md).

**S2 P13:** formal x86/x64 Ninja runners now initialize MSVC once and default
to bounded parallel scheduling (`min(12, processor count)`), with an explicit
`MVDM_BUILD_JOBS` override. The generated graphs and compiler/source choices
are unchanged; both runners accept the product dry run. See [P13
evidence](etc/evidence/m0-t318-s2-p13-formal-ninja-parallel-runner-001.md).

**S2 P14:** two external `demLoadDos`/NTDOS observation forms produced only
non-attributable negatives: debugger attachment changes the historical host
contract, while the inherited console carried unrelated background traffic.
They do not establish a guest blocker or weaken the P12 source route. Future
runtime observation must be independently console-owned and debugger-neutral.
See [P14 evidence](etc/evidence/m0-t318-s2-p14-external-observer-negative-result-001.md).

**S2 P15:** the current formal r4 product now has a reproducible immutable
sidecar-media staging tool matching the app-owned executable-relative package
contract. With current firmware and DOS media present, the previous missing
resource path is absent; a debugger-neutral bounded observation remains alive
but does not yet establish guest load. The current startup wait is retained as
an unclassified original-host execution issue rather than a DEM/COMMAND/BOP
failure. See [P15 evidence](etc/evidence/m0-t318-s2-p15-current-package-staging-and-startup-observation-001.md).

**S2 P16:** the selected x86 CCPU/CVIDC runtime again retains its original
contiguous 64 KiB GDP carrier; the native-width GDP-slot form is restricted to
the required x64 compile/link carrier. Both formal product links pass. The
next x86 observation exposes a separate original VGA-initialization null
transfer, without admitting a dummy video shim or changing guest/BOP logic.
See [P16 evidence](etc/evidence/m0-t318-s2-p16-original-x86-gdp-carrier-restoration-001.md).

**S2 P17:** the GDP-carrier generator now preserves its generated header's
timestamp when its byte content is unchanged. This prevents a no-op formal
graph configure from invalidating every dependent translation unit. A fresh
x86 parallel formal product rebuild completed 215 actions and a second
configure plus dry-run reported `ninja: no work to do`; source selection and
compiler flags are unchanged. See [P17 evidence](etc/evidence/m0-t318-s2-p17-incremental-gdp-generator-001.md).

**S2 P18:** the selected CCPU carrier now binds its original `Cpu` state to
the complete generated original C-VID table at C-VID's existing setup point.
The formal x86/x64 product links pass. The earlier debugger-attached observation
is superseded as runtime evidence by P20/P21; P18 is source/link closure only,
not an NTDOS/EXEC or execution-reachability claim. See [P18
evidence](etc/evidence/m0-t318-s2-p18-cvidc-vector-assembly-closure-001.md).

**S2 P19:** original `nt_det.c::initTextSection` now retains its
`RegisterConsoleVDM` call through the same-shaped adapter.  The selected
windowed CPU40 profile receives only the original caller's required,
session-owned text buffer; no NT4 Console Server fullscreen/controller
protocol is claimed. Both formal product links pass, and the fresh bounded
x86 debugger-attached real-console run no longer shows the former 120 error dialog.
That debugger observation is superseded as runtime evidence by P20/P21; P19 remains
source/link closure, not NTIO/NTDOS/EXEC evidence. See [P19
evidence](etc/evidence/m0-t318-s2-p19-windowed-console-registration-closure-001.md).

**S2 P20:** the existing non-debug `plain` observer changes console ownership
as well as debugger attachment, then exits before CPU execution with
`0xC0000005`. It is therefore non-attributable and does not validate P19.
The required next row is a non-debug, console-owning bounded observation;
no product instrumentation or new BOP route is admitted. See [P20
evidence](etc/evidence/m0-t318-s2-p20-nondebug-console-observation-attribution-001.md).

**S2 P21:** a tracked, non-debug, console-owning fixed observation container
now holds media, firmware, standard handles, command line and an eight-second
timeout constant. Its first baseline exits with `0x00000001`, before any
NTIO claim. The static source scan groups the startup prerequisites into
console/display, error termination, Base VDM command acquisition,
machine/media and complete CCPU-table assembly; follow-up work proceeds by
those owner clusters, not by changing observers. See [P21
evidence](etc/evidence/m0-t318-s2-p21-fixed-console-startup-baseline-001.md).

**S2 P22:** the P21 stage path itself exceeded the original 64-byte VDM current
directory contract, so its app-side command declaration correctly returned
`69`. A same-media, short-path formal stage now keeps the P21 observer,
arguments, console ownership and timeout fixed; it reaches the original host
path and reproducibly exits `0xC0000005` without debugging. CCPU/C-VID vector
composition is the next complete owner-contract audit; no NTIO, NTDOS, EXEC or
parent-return claim is made. See [P22
evidence](etc/evidence/m0-t318-s2-p22-fixed-container-admission-and-ccpu-fault-001.md).

**S2 P23:** the omitted historical CCPU-to-C-VID generated vector assembly is
now recovered from original `c2cpusad.h`: all 154 public and 55 private slots
receive a typed original-provider or explicit unavailable disposition. The
formal CPU40 x86 and x64 products link; the immutable real-console container
still exits `0xC0000005`, without a new runtime attribution or NTIO claim. See
[P23 evidence](etc/evidence/m0-t318-s2-p23-cvidc-generated-vector-contract-001.md).

**S2 P24:** runtime observation is now fixed to one non-debug,
console-owning, eight-second CPU40 container.  A source scan freezes the
pre-NTIO owner order—console/display, common error termination, Base VDM
command acquisition, machine/media and CCPU/C-VID assembly—so recovery is by
complete original call cluster rather than one observer-dependent symptom at a
time.  The current `0xC0000005` is still unassigned. See [P24
evidence](etc/evidence/m0-t318-s2-p24-fixed-container-startup-owner-clusters-001.md).

**S2 P25:** a repeat in that exact container and its P22 short stage now times
out after eight seconds; the observer's `0x53504354` is not a product crash.
P22/P24's `0xC0000005` remains historical evidence but is superseded as the
current baseline. The product is not yet proven to reach NTIO, NTDOS, EXEC or
parent return. See [P25 evidence](etc/evidence/m0-t318-s2-p25-fixed-container-baseline-correction-001.md).

**S2 P26:** the source-defined gate from live SoftPC startup to guest command
consumption is fixed. `scs_init` only queries first-VDM state; the declared
ordinary child is consumed later by original guest COMMAND `54:01` through
`cmdGetNextCmd` and the existing Base VDM contract. The next observation is a
passive original-dispatch reach record, not another changed startup container
or BOP implementation. See [P26 evidence](etc/evidence/m0-t318-s2-p26-startup-to-guest-observation-gate-001.md).

**S2 P27:** retired an unselected hand-maintained `MS_bop_0`/`MS_bop_4`
subset from the production overlay. The formal CPU40 graph already selects the
complete original `nt_bop.c`; there is now one selected DEM/COMMAND ingress,
not a parallel reconstructed route. See [P27 evidence](etc/evidence/m0-t318-s2-p27-retire-unselected-bop-ingress-subset-001.md).

**S2 P28:** the one opt-in passive original-dispatch recorder was rejected and
removed after its otherwise unchanged fixed-container product exited
`0xC0000005` with no record, while the normal P25 product timed out.  The
instrumented binary therefore changes an observation variable and cannot prove
pre-BOP reachability.  Recovery returns to P24/P26's static original startup
owner clusters; no BOP or guest-execution claim is made. See [P28
evidence](etc/evidence/m0-t318-s2-p28-passive-dispatch-observer-rejection-001.md).

**S2 P29:** the original startup predecessor scan now fixes the complete
`ntvdm.c → host_main → host_applInit → InitialiseDosEmulation →
host_start_cpu` owner order.  It separates console/display, CCPU/VGA,
DOS-media/Base-VDM and CPU handoff clusters, and proves no selected
startup-side `120` return or BOP service lies between `host_start_cpu` and the
CPU engine.  Subsequent recovery must select one unresolved original owner
cluster before another unchanged-container observation. See [P29
evidence](etc/evidence/m0-t318-s2-p29-original-startup-prerequisite-owner-scan-001.md).

**S2 P30:** P25's mutable-r4 product identity was no longer reproducible, so
it is historical evidence only. Fresh current-source CPU40 x86/x64 products
both link, and the x86 product plus hash-verified media are now staged once in
immutable `runtime-r30`. The sole current real-console baseline exits
`0xC0000005` before any NTIO/BOP claim; P29's original owner clusters remain
the recovery order. See [P30
evidence](etc/evidence/m0-t318-s2-p30-current-source-immutable-runtime-baseline-001.md).

**S2 P31:** original process/thread exception wrappers now establish that the
fixed r30 `0xC0000005` identifies only an escaped access violation, not its
source. The selected CPU40 initialization still creates CCPU's main-thread TLS
simulation state before `c_cpu_simulate`, while the C-VID binding remains
ordered before DOS initialization. No r30 crash frame is available from the
non-invasive event channel, so the next work is one static CPU40
execution-start owner cluster—not a BOP patch or observer variation. See [P31
evidence](etc/evidence/m0-t318-s2-p31-original-unhandled-exception-boundary-audit-001.md).

**S2 P32:** original CPU40 SAS/RAM initialization is now separated from later
external physical-page mapping: `InitIntelMemory` supplies the contiguous
`Start_of_M_area`/`CCPU_M` backing used by the first normal CCPU fetch, while
the session mapping manager is only an external-page alias seam. No RAM/token
attribution or source change is admitted. The next static unit is original
`c_cpu_simulate → ccpu(FALSE)` first dispatch; see [P32
evidence](etc/evidence/m0-t318-s2-p32-cpu40-memory-and-first-dispatch-boundary-audit-001.md).

**S2 P33:** the selected CCPU40 direct first-fetch path is now source-closed:
`host_start_cpu → c_cpu_simulate → ccpu → SETUP_HOST_IP` reads the loaded NTIO
address from the original contiguous `CCPU_M` allocation. It neither requires
the mapping manager nor the historical register-vector initializer. CCPU TLS,
exception-frame, timer and C-VID branches have defined later ownership, with
no current evidence that one caused r30's escaped access violation. The next
action is one unchanged formal r30 observation; see [P33
evidence](etc/evidence/m0-t318-s2-p33-cpu40-first-fetch-and-dispatch-owner-audit-001.md).

**S2 P34:** the one permitted unchanged r30 repeat again exits
`0xC0000005`. The source-defined pre-NTIO console cluster is now mapped as
`host_applInit → init_host_uis → nt_start_event_thread`: the fixed real-console
container is its normal prerequisite, while no `DisplayErrorTerm`, return-120
or private Console/CSR failure branch is yet proven reached. Next is a whole
`host_applInit` worker/locking compatibility audit, not a new observer or BOP
probe; see [P34
evidence](etc/evidence/m0-t318-s2-p34-fixed-container-repeat-and-console-prerequisite-audit-001.md).

**S2 P35:** the complete original `host_applInit` worker/ICA-lock cohort is
source-audited through the first `scs_init` call. The event worker remains
suspended at that point; the heartbeat is the sole later concurrent worker,
and its selected cdecl-to-WINAPI/session bridge preserves the original worker
contract. No speculative source change is admitted. See [P35
evidence](etc/evidence/m0-t318-s2-p35-host-application-worker-and-scs-init-prerequisite-audit-001.md).

**S2 P36:** the complete original `scs_init` provider cohort is source-audited:
Base VDM, first-session boot-file policy, COMMAND, DEM, XMS and debugger have
one selected owner each. The audit found the formal graph omitted the existing
XMS session-backend selection; it is a build-selection defect, not a request
for a replacement XMS provider. See [P36
evidence](etc/evidence/m0-t318-s2-p36-scs-init-provider-cohort-audit-001.md).

**S2 P37:** the formal CPU40 graph now selects that existing XMS
mapping-manager backend and fresh x86/x64 product links pass. A new product
EXE was staged with byte-identical DOS media and firmware into the otherwise
unchanged fixed non-debug console-owning container; it still exits
`0xC0000005`. XMS selection is therefore no longer a current-startup cause,
and no NTIO/NTDOS/EXEC claim is made. See [P37
evidence](etc/evidence/m0-t318-s2-p37-xms-session-backend-selection-and-fixed-run-001.md).

**S2 P38:** the fixed `-o` profile is source-proven to select original
windowed `ConsoleInit`, not stream I/O or an X86GFX/Console-Server route. Its
real-console prerequisite and P19 text registration are already satisfied;
heartbeat starts only after that prefix and transfers the next static cohort to
original timer/PIC mechanics. No observation was repeated. See [P38
evidence](etc/evidence/m0-t318-s2-p38-reset-console-heartbeat-owner-audit-001.md).

**S2 P39:** the complete original heartbeat machine cohort is source-proven:
BIOS reset initializes `SWTMR_time_tick`; NTVDM/CPU40 retains direct original
PIC delivery; and `CPU_TIMER_TICK` enters original CCPU40
`c_cpu_interrupt`. No BaseSrv/CSR/console or new adapter boundary occurs in
this path, so no source change or repeated observation is justified. See [P39
evidence](etc/evidence/m0-t318-s2-p39-timer-pic-cpu40-prerequisite-audit-001.md).

**S2 P40:** the complete original pre-decode CPU40 cohort is source-proven:
`cpu_init` establishes CCPU TLS/simulation state before DOS initialization,
and `host_start_cpu` enters only original `c_cpu_simulate`. The fixed
`0xC0000005` remains an unlocated original top-level exception—not an NTIO,
DEM, COMMAND or EXEC result—and no source mutation or repeat observation is
admitted. See [P40
evidence](etc/evidence/m0-t318-s2-p40-cpu40-entry-prerequisite-audit-001.md).

**S2 P41:** one fixed non-debug, console-owning container and a complete
original startup-precondition cohort now govern every remaining observation.
The static order from `ntvdm.c` through `scs_init`, NTIO loading and
`host_start_cpu` separates timer, NLS/error, console, Base VDM/provider and
SAS/CPU ownership. Formal Ninja rebuilds only changed libraries and final
link; one fixed short observation follows a changed product; NTDOS/EXEC
evidence is prohibited before NTIO reachability. See [P41
evidence](etc/evidence/m0-t318-s2-p41-fixed-container-startup-prerequisite-cohort-closure-001.md).

**S2 P42:** the source-built guest's exact first host-service boundary is now
fixed: `NTIO.SYS` maps its `0000:0370` entry to original `msinit.asm::init`,
whose IVT/stack/DOS-data prefix first crosses owners only at original
`SVC_DEMLOADDOS`. It reaches the selected original `demLoadDos` route—never a
replacement loader, COMMAND acquisition or EXEC path. See [P42
evidence](etc/evidence/m0-t318-s2-p42-ntio-first-service-boundary-audit-001.md).

**S2 P43:** the complete original `NTIO.SYS` entry prefix is now checked
against the selected CPU40 decoder.  Its `FCLI`/`FSTI` virtual-interrupt
macros, IVT/stack operations and ordinary BOP encoding all have original
CPU40/SAS support; no opcode, mapping or adapter repair is justified before
the existing `0x50:0x11 -> demLoadDos` crossing.  See [P43
evidence](etc/evidence/m0-t318-s2-p43-ntio-entry-cpu40-instruction-contract-audit-001.md).

**S2 P44:** the first post-load `NTIO -> NTDOS sysinit` cohort is source/build
closed: original BIOS `0x12` and `0x15/AH=88h` memory paths plus DEM `0x0D`
boot-drive service are selected and linked with their original ownership.
The next continuity cohort is guest-owned NTDOS `DOSINIT`/configuration, not
a missing app, mapper or BOP provider.  See [P44
evidence](etc/evidence/m0-t318-s2-p44-ntio-ntdos-postload-owner-cohort-001.md).

**S2 P45:** the first NTDOS configuration-to-COMMAND crossing is source/build
closed: original `CMDSVC SVC_GETCONFIGSYS` reaches `54h:0Ch`, original
`MS_bop_4`, `CmdDispatch`, and `cmdGetConfigSys` in the selected graphs.  Its
temporary-config and console-init behavior remains a single original COMMAND
owner cohort; no new BOP route or app loader is admissible.  See [P45
evidence](etc/evidence/m0-t318-s2-p45-ntdos-configuration-command-owner-cohort-001.md).

**S2 P46:** the original PIF configuration selector's obsolete system-directory
default is now bound to the already-selected immutable DOS media root; PIF
overrides and the original failure path remain unchanged.  The fixed stage now
carries byte-identical `config.nt`/`autoexec.nt`; the one new-product
observation changed from immediate exit to the bounded timeout, without
claiming NTIO reachability.  See [P46
evidence](etc/evidence/m0-t318-s2-p46-original-configuration-media-binding-closure-001.md).

**S2 P47:** the complete source-shaped CONFIG/AUTOEXEC media cohort is now
classified before another run: original `cmdconf` also requires a bounded
DOS-visible system-directory projection and its selected country, HIMEM,
COMMAND and optional AUTOEXEC payloads.  This is one media/path owner repair,
not a BOP series; no second observation of the P46 EXE is admissible.  See
[P47 evidence](etc/evidence/m0-t318-s2-p47-original-configuration-media-dependency-cohort-001.md).

**S2 P48:** the whole selected CONFIG/AUTOEXEC system-media cohort is now
source/build closed: original `cmdconf` uses the existing selected DOS-media
root binding, the immutable stage carries the original COUNTRY/HIMEM/COMMAND
payload plus established REDIR/DOSX artifacts, and x86/x64 products link.
One new x86 stage times out rather than reproducing the old immediate terminal;
it remains liveness only.  See [P48
evidence](etc/evidence/m0-t318-s2-p48-original-configuration-system-media-closure-001.md).

**S2 P49:** the fixed `config.nt` directives `DOS=HIGH,UMB`,
`DEVICE=HIMEM.SYS`, and `FILES=20` now have one source-shaped original owner
audit: guest `sysconf.asm`, the original XMS/HIMEM service table and host
`XMSInit`, plus the original DEM/COMMAND dispatch endpoints.  The selected
x86 graph already links that cohort and its session mapping backend; no local
shim or BOP implementation is admitted, and no unchanged product is rerun.
See [P49
evidence](etc/evidence/m0-t318-s2-p49-selected-configuration-directive-owner-cohort-001.md).

**S2 P50:** the complete direct early-console/display cohort from original
`ntvdm.c` through `host_main`, `host_applInit`, console/event setup and CPU
entry is now source-audited.  The fixed real-console container satisfies its
original public Win32 prerequisites; no executable CSR/BaseSrv call occurs
there, so no new adapter or source edit is admitted.  The next static boundary
is original SCS command acquisition plus DBG/VDD setup, before any newly
linked product receives one fixed-container observation.  See [P50
evidence](etc/evidence/m0-t318-s2-p50-fixed-container-console-startup-owner-cohort-001.md).

**S2 P51:** original SCS/Base VDM/COMMAND/DEM/XMS/DBG/VDD pre-NTIO startup
paths are now audited as one cohort.  Each has an original or already-registered
same-shaped binding and no remaining unclassified local terminal branch for
the declared profile.  The product is unchanged, so the fixed container is not
rerun; the next source-first boundary is the NTIO load/guest-entry transaction.
See [P51
evidence](etc/evidence/m0-t318-s2-p51-original-scs-pre-ntio-owner-cohort-001.md).

**S2 P52:** original `config()` assigns and floors `xmsMemorySize` before
`scs_init`, so the apparent zero-XMS `XMSInit` terminal is unreachable for
both ordinary DOS and WOW configurations.  No mapper/profile/source change or
unchanged-product observation is admitted.  See [P52
evidence](etc/evidence/m0-t318-s2-p52-original-xms-configuration-prerequisite-001.md).

**S2 P53:** the whole original `config()` cohort—Base VDM PIF request,
no-PIF defaults, PIF console policy, SAS/CPU40 state, ROM/BIOS mapping, UMB
list and runtime setup—is source-audited for the fixed ordinary-child profile.
It introduces no local terminal or private-transport gap before NTIO entry;
no unchanged product is rerun.  See [P53
evidence](etc/evidence/m0-t318-s2-p53-original-configuration-owner-cohort-001.md).

**S2 P54:** the selected first NTIO host transaction is source-closed as one
original owner cohort: `50h:11h → MS_bop_0 → DemDispatch → demLoadDos` retains
the original SAS destination, NTDOS filename and read-loop contract, while the
already-selected DOS-media binding supplies its sole modern path seam. Formal
dependency records confirm the original SoftPC/Sim32 headers, not a redirector
pointer-scope header. This is not runtime reachability evidence; the fixed
container may run only after a separately justified new product repair. See
[P54 evidence](etc/evidence/m0-t318-s2-p54-ntio-demload-dos-owner-closure-001.md).

**S2 P55:** the unselected Redirector pointer-scope experiment is removed from
the production adapter and test tree.  It had no production call site, formal
source selection or selected-object dependency; both x86/x64 final-link rows
remain clean.  This removes an alternate `GetVDMAddr` contract rather than
changing the original DEM/SoftPC route, so no fixed-container run is admitted.
See [P55 evidence](etc/evidence/m0-t318-s2-p55-retire-unselected-redirector-pointer-scope-001.md).

**S2 P56:** the Base VDM host-path carrier now preserves original `MAX_PATH`
application and path-string current-directory capacities; original COMMAND's
matching `MAX_PATH + 1` receiver is corrected as registered
`MVDM-HOST-DIV-160`.  The focused fixed-package declaration fixture and final
CPU40 x86/x64 links pass.  The retained p56--p59 runs are not runtime closure;
the receiving-buffer mismatch is source-safe but not a proven crash root. See
[P56 evidence](etc/evidence/m0-t318-s2-p56-base-vdm-host-path-contract-001.md).

**S2 P57:** the approved unchanged-container p60 observation of the P56
product still exits `0xC0000005` with no console output.  The COMMAND buffer
repair remains source/link valid but is not runtime-attributed; recovery
returns to the next complete original pre-NTIO owner cohort rather than a
trace-selected BOP patch. See [P57 evidence](etc/evidence/m0-t318-s2-p57-fixed-container-command-buffer-result-001.md).

**S2 P58:** the selected original top-level exception filter preserves an
escaped primary exception code through `NtTerminateProcess`; `0xC0000005` is
therefore not an app/BaseVDM synthetic exit.  It remains an unlocated original
host/CPU branch failure and requires owner-contract audit, not BOP repair. See
[P58 evidence](etc/evidence/m0-t318-s2-p58-top-level-exception-result-contract-001.md).

**S2 P59:** the complete selected CPU40 CCPU/C-VID startup contract confirms
that original main-thread simulation TLS and C-VID binding both precede
`host_start_cpu`.  The generated generic C-VID vector tail has 26 explicit
CPU40/SPC386-unavailable slots; it is a bounded future caller-capability
cohort, not a justified first-fetch crash repair.  No BOP, mapping or observer
path changes. See [P59 evidence](etc/evidence/m0-t318-s2-p59-cpu40-cvidc-vector-contract-audit-001.md).

**S2 P60:** P57's retained `0xC0000005` came from the distinct
`original-softpc-process-p60.exe` copy, not the current manifest-selected
formal product.  The current fixed container's normal record is a bounded
timeout.  P57's source repair remains valid, but its runtime result is
historical only; no NTIO/NTDOS/EXEC claim is made. See [P60
evidence](etc/evidence/m0-t318-s2-p60-runtime-product-identity-reconciliation-001.md).

**S2 P61:** the one-child composition now honors the original Base VDM
first-command contract: separate `AppName`, CR/LF-terminated `CmdLine`, and
an optional host environment capture buffer.  The original BaseSrv server
source is the direct evidence; the local x86 regression passes and the x64
row links.  A changed-product r33 run still exposes `0xC0000005`, so this is
source/ABI closure only, not guest-continuity evidence. See [P61
evidence](etc/evidence/m0-t318-s2-p61-basesrv-first-command-contract-closure-001.md).

**S2 P62:** the fixed startup container now launches the stage-resident product
beside the manifest-verified DOS and SoftPC media, rather than the separately
located formal build output.  The product EXE is the container's sole mutable
input; every media asset is hash-verified before launch and the exact staged
product hash is recorded.  The corrected, non-debug eight-second observation
still exits `0xC0000005`; missing adjacent media is therefore excluded, while
the original escaped access violation remains unlocated. See [P62
evidence](etc/evidence/m0-t318-s2-p62-fixed-runtime-product-adjacency-closure-001.md).

**S2 P63:** static CPU40 source order now separates the pre-NTIO synchronous
host UI/console cluster from later reset/heartbeat execution.  `TimerInit`
does not start the heartbeat thread, the selected `/DPROD` image excludes the
checked-build `DbgBreakPoint` access-violation branch, and the direct VDD
memory-fault raise is post-lookup rather than an early startup candidate.  The
next bounded owner unit is original `host_applInit → init_host_uis →
InitScreenDesc/SetupConsoleMode → nt_start_event_thread`; no observer or
container variation is admitted.  See [P63
evidence](etc/evidence/m0-t318-s2-p63-pre-ntio-host-ui-timer-owner-audit-001.md).

**S2 P64:** an exception-context diagnostic localized the fixed product's
escaped `0xC0000005` to the first indirect C-VID video-vector call during
original EGA configuration.  A video-only, source-shaped pre-configuration
binder now supplies the original `C_Video` table without prematurely binding
the CPU vector.  See [P64
evidence](etc/evidence/m0-t318-s2-p64-cvidc-first-vector-fault-001.md).

**S2 P65:** the retained C-VID metadata exposed three missing generated
quick-event timing slots.  Their original forwarding shims had been bound back
to themselves, causing the next stack overflow.  The registered narrow timing
provider restores the original initial value and setter contract.  Formal x86
and x64 links pass, and the unchanged x86 fixed container now remains alive to
its eight-second timeout.  This is only startup liveness—not NTIO, NTDOS,
EXEC, or child-completion evidence. See [P65
evidence](etc/evidence/m0-t318-s2-p65-cvidc-timing-provider-and-startup-liveness-001.md).

**S2 P66:** the unchanged fixed CPU40 runtime container now proves that the
original `InitialiseDosEmulation` completed its NTIO setup/load path, followed
by original `c_cpu_simulate` and CCPU's first `SETUP_HOST_IP` fetch. This is
guest-execution reachability only: it makes no BOP, NTDOS, `EXEC`, or
parent-return claim. See [P66
evidence](etc/evidence/m0-t318-s2-p66-ntio-first-fetch-milestones-001.md).

**S2 P67:** the original CPU40 BOP carrier is now observed at its existing
`MS_bop_0` boundary without introducing a new dispatcher. The same fixed
eight-second x86 container reaches NTIO and CCPU first fetch but not the first
NTIO `0x50:0x11` BOP. This assigns the next audit to pre-BOP NTIO/SoftPC
execution prerequisites, not DEM or a single BOP service. See [P67
evidence](etc/evidence/m0-t318-s2-p67-original-bop-ingress-observation-001.md).

**S2 P68:** the verified NTIO first fetch is `0070:00000000`, real-mode with a
16-bit code-segment default (`MSW=0010`, `CSX=0`).  The non-reached first BOP
cannot be attributed to a wrong CS:EIP, protected-mode, or default-operand-size
entry state. It makes no BOP reachability conclusion: the state reporter was
then removed because extra hot-path console writes perturb the fixed-container
trace. See [P68
evidence](etc/evidence/m0-t318-s2-p68-ntio-first-fetch-state-001.md).

**S2 P69:** the first original NTIO `E9` is a CCPU control transfer that forces
the original inter-instruction event check before the `0070:0370` `msinit`
fetch.  The complete original heartbeat cohort is the first conditional
cross-owner candidate after that transfer; it is an audit target, not a proven
fault and not a BOP change. See [P69
evidence](etc/evidence/m0-t318-s2-p69-first-control-transfer-heartbeat-owner-audit-001.md).

**S2 P70:** the original heartbeat cohort has a complete selected provider
binding before CPU start: `host_applInit` binds `working_video_funcs` to the
original `nt_video_funcs`, and the worker is created suspended then resumed
only by the original post-reset order. No missing vector, replacement provider,
or initialisation inversion supports a repair. The fixed-container trace must
not be perturbed with new hot-path console probes. See [P70
evidence](etc/evidence/m0-t318-s2-p70-heartbeat-provider-binding-audit-001.md).

**S2 P71:** the three completed startup console milestones were removed from
the product because they perturb the fixed-container execution observation.
The freshly linked, uninstrumented x86 CPU40 product then reaches original DEM
`50:11`, `50:3B`, `50:0F`, and `50:1B` dispatches in one fixed eight-second
run. P67's former non-observation of `50:11` is thereby superseded; this is
bootstrap progress, not yet ordinary-child `EXEC` / parent-return evidence.
See [P71
evidence](etc/evidence/m0-t318-s2-p71-remove-startup-observation-perturbation-001.md).

**S2 P72:** the local Base VDM record and app declaration now preserve the
original `MAX_PATH + 1` host-current-directory carrier, rather than rejecting
the fixed package root as a guest component. The fixed non-debug observation
therefore continues past DEM bootstrap and reaches original COMMAND `54:05`
and `54:0C`. This proves original COMMAND bootstrap ingress for the declared
ordinary-child profile, not successful CONFIG preprocessing, guest `EXEC`, or
parent-return continuity. See [P72
evidence](etc/evidence/m0-t318-s2-p72-command-ingress-and-host-path-carrier-001.md).

**S2 P73:** paired short-root fixed-container observations with and without
the admitted `--ordinary-child` declaration both exit `0xC0000005` after the
same original DEM/COMMAND bootstrap markers.  The failure therefore cannot be
assigned to the declared `/C VER` input, `EXEC`, or parent return.  The next
unit is the complete original post-`cmdSetInfo` / early guest-bootstrap cohort;
no trace-driven COMMAND/BOP repair is admitted. See [P73
evidence](etc/evidence/m0-t318-s2-p73-short-root-command-attribution-001.md).

**S2 P74:** the original `cmdconf.c` 64-byte CONFIG root carrier is distinct
from P72's corrected `MAX_PATH + 1` Base VDM transport.  The prior long-root
watchdog can block in original error UI and is not CONFIG-continuity evidence;
only a source-valid short root may exercise CONFIG/AUTOEXEC preprocessing.
The P73 short-root access violation remains unattributed. See [P74
evidence](etc/evidence/m0-t318-s2-p74-command-config-root-admission-correction-001.md).

**S2 P75:** the complete original post-`54:05 cmdSetInfo` cohort is now
bounded: NTDOS executes its own stack/device strategy-and-interrupt far-call
initialization before the next explicit `50:48 demGetDPBList` crossing; the
later `54:0C` CONFIG crossing is separate. P73's partial capture cannot assign
the unlocated failure to either later service, HIMEM/XMS, or EXEC. The next
owner unit is CPU40 guest device-call/stack semantics, not a BOP patch. See
[P75 evidence](etc/evidence/m0-t318-s2-p75-post-cmdsetinfo-guest-device-bootstrap-owner-cohort-001.md).

**S2 P76:** a single fixed-container exception-only observation now attributes
the `0xC0000005` to an attempted zero transfer on the original
`floppy.c::wait_int → host_simulate` recursive CPU40 path. The marker prefix
still excludes the declared child/EXEC lifecycle; the remaining blocker is the
original SoftPC FDC/INT15 guest-vector and interrupt-return owner package.
Because S2 excludes device expansion, it transfers to the queued minimal
vertical-slice integration package without a BOP, DEM, COMMAND or XMS patch.
See [P76 evidence](etc/evidence/m0-t318-s2-p76-floppy-recursive-simulation-owner-transfer-001.md).

### M0 T317 S2 closure record

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T317 S2; Ordinary Mode (single-person dual-role implementation). |
| Result | Closed: the seven original COMMAND shared-state/Base VDM bootstrap units formally compose under the selected x86 CCPU40 profile; the same-shaped local Base VDM request/capacity/retry/re-entry contract has focused evidence. |
| Closure | [S2 closure](etc/evidence/m0-t317-s2-command-bootstrap-cohort-closure-001.md) |
| Transfer | `cmdexec`/`cmdexit` and local `cmdredir`/`cmdpif` endpoints become T317 S3; no guest execution or CSRSS product-shell claim transfers as completed work. |

### M0 T317 S1 closure record

| Field | Record |
| --- | --- |
| Result | Closed: complete original DEM/COMMAND source cohorts, direct boundaries, selected adapters and ordered recovery groups are recorded in the T317 S1 boundary ledger. |
| Closure | [S1 boundary ledger](etc/evidence/m0-t317-s1-dem-command-softpc-boundary-ledger-001.md) |

### M0 T316 closure record

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T316; Ordinary Mode (single-person dual-role implementation). |
| Result | Closed: original monitor ABI import, bounded `VdmQueryDir` binding and CPU40-only source correction verified. |
| Closure | [T316 closure](history/m0-t316-monitor-user-mode-contract-closure-20260830.md) |
| Next Admission | Queue item 1: DEM/COMMAND vertical slice and child lifecycle. |

**T316 closed:** all reached `NtVdmControl` and `VDM_TIB` users have one
source/binding/unavailable disposition. The duplicate adapter `vdm.h` was
removed; the original ABI mirror is authoritative and `monitor_context.h`
owns only session TLS. A formal x86 CCPU40 graph and both focused monitor
fixtures pass. Kernel VDM/V86, graphics, DPMI and VDD/debugger services remain
explicitly deferred to their named owner packages.

**T313 S1 closed:** the eleven-cohort selected-machine ledger is complete,
with no live retired route in `mvdm-host` or its overlay.  Every non-original
form reached later now has a mandatory remove/replace/retain decision rather
than generic cleanup debt.  See the [closure](etc/evidence/m0-t313-s1-softpc-capability-purity-closure-001.md).

**T313 S2 P1:** the formal x86 CCPU40 graph now selects the original
`obj.vdm/ntvdm.c` body as the source-shaped process lifecycle.  App no longer
mistakes the original empty `host_cpu_init()` compatibility hook for machine
initialization; declaration-only NT4 registry bindings preserve the entry's
own control flow.  This is compile evidence only; CPU/SAS/firmware/reset and
bounded lifecycle behavior remain S2 work.  See the [P1 record](etc/evidence/m0-t313-s2-p1-original-entry-binding-001.md).

**T313 S2 P2:** the selected original SoftPC host-root archive now forms
without competing UCRT `printf`/`fprintf` definitions.  The correction is a
graph-wide CRT declaration mode that retains `fprt.c` as the sole original
interposer; it does not replace source bodies or hide warnings.  The fresh
external x86 candidate completed all 54 archive inputs.  CPU/SAS/firmware/reset
behavior is still S2 work.  See the [P2 record](etc/evidence/m0-t313-s2-p2-original-host-root-link-closure-001.md).

**T313 S2 P3:** the exact original CCPU40 instruction/SAS cohort completed its
fresh external x86 archive (133 inputs) without compiler or linker error.  No
warning was suppressed and no second executor participates.  This is formal
package evidence only; create-to-reset and bounded guest behavior remain S2
work.  See the [P3 record](etc/evidence/m0-t313-s2-p3-original-ccpu40-archive-001.md).

**T313 S2 P4:** the refreshed selected x86 CCPU40 formal candidate completed
all 189 archive inputs without error.  An encountered retired backend fixture
was deleted and the live session backend test now asserts the actual
SoftPC-only contract.  This is still formal-composition evidence; bounded
original lifecycle behavior remains S2 work.  See the [P4 record](etc/evidence/m0-t313-s2-p4-formal-candidate-and-retired-test-cleanup-001.md).

**T313 S2 P5:** an encountered `nt_msscs.c` startup definition has returned to
its original implicit-result spelling; only the necessary support-call
declaration remains.  The affected selected host-root archive rebuilt cleanly
with the original warning still visible.  See the [P5 record](etc/evidence/m0-t313-s2-p5-mvdm-host-startup-mirror-cleanup-001.md).

**T313 S2 P6:** the selected original CRT call shapes are now bound without a
project-owned replacement: graph-wide no-inline mode preserves the original
`fprt.c` provider, while the MSVC compatibility library supplies historic
`_sprintf`/`_sscanf`.  The forced-link audit leaves its duplicate-provider
diagnostic and all genuine historical boundary symbols visible.  See the [P6
record](etc/evidence/m0-t313-s2-p6-original-crt-binding-audit-001.md).

**T313 S2 P7:** the original process-to-reset source chain is now recorded,
and a fresh current x86 CCPU40 graph re-ran the bounded original
CPU/SAS/FPU/`host_start_cpu` witness successfully.  A separate fresh full
selected formal candidate completed 403/403 edges.  The witness deliberately
stops before BIOS BOP/device I/O; it does not claim a DOS boot.  See the [P7
record](etc/evidence/m0-t313-s2-p7-original-lifecycle-witness-001.md).

**T313 S2 P8:** the refreshed current x86 witness now proves the original SAS
owner through the XMS-shaped A20 interface and the existing mapping-manager
physical-page/DOS-alias positive and negative cases.  Original EMS/XMS/UMB
allocation bodies remain selected; no fixture-created allocator is claimed.
See the [P8 record](etc/evidence/m0-t313-s2-p8-memory-a20-and-umb-boundary-001.md).

**T313 S2 P9:** two non-original warning-suppression assignments were removed
from the reached original firmware-resource mirror `nt_rez.c`.  The original
source warnings are visible again and the selected host-root archive rebuilt
cleanly.  See the [P9 record](etc/evidence/m0-t313-s2-p9-firmware-resource-mirror-cleanup-001.md).

**T313 S2 closed:** original CPU/SAS/FPU, mapping/A20, firmware-resource and
reset source contracts have bounded evidence plus fresh formal x86 candidate
and forced-link audit. Controller endpoint behavior transfers only to S3. See
the [closure](history/m0-t313-s2-cpu-memory-firmware-reset-closure-20260830.md).

**T313 S3 P1:** the original controller cohort is now re-established under
the selected x86 CCPU40 build profile: a fresh 403-edge formal candidate, the
original heartbeat suspend/resume witness and original ICA-lock witness all
pass.  The reached historical fixture graphs no longer define CPU30; their
only additions are current GDP generation and already-registered overlay
include/link bindings.  Timer-to-PIC/DMA/input delivery remains S3 work. See
the [P1 record](etc/evidence/m0-t313-s3-p1-controller-source-assembly-001.md).

**T313 S3 P2:** the original reset → ICA/PIT/DMA/input chain is now explicitly
recorded and the public console-input binding witness passes on x86.  A clean
external 403/403 CCPU40 candidate rebuild confirms that no parallel
controller was introduced.  The reached `nt_reset.c` host-local `strlen`
width correction is now registered rather than left as an undocumented mirror
difference.  Live timer-to-PIC/CCPU and DMA delivery remain S3 work. See the
[P2 record](etc/evidence/m0-t313-s3-p2-controller-chain-and-input-binding-001.md).

**T313 S3 P3:** original `ios.c` and `at_dma.c` now run inside the existing
bounded x86 CCPU40 fixture.  Original DMA controller/page-register port
registration and `inb`/`outb` address/page-register round trips pass; fixture
I/O-table stand-ins were deleted.  Timer-to-PIC/CCPU remains the only positive
controller endpoint not yet witnessed. See the
[P3 record](etc/evidence/m0-t313-s3-p3-original-dma-ios-witness-001.md).

**T313 S3 P4:** original `ica.c` now runs in the same bounded x86 CCPU40
fixture: BIOS-order master/slave initialization, IRQ0 request and original
`ica_intack` return vector `08h` pass.  The run records the reached original
`WOWIdle(FALSE)` notification without claiming a local WOW scheduler or EOI
product substitute.  The remaining positive endpoint is the original
`nt_timer -> time_tick -> TimerGenerateMultipleInterrupts -> PIC` chain. See
the [P4 record](etc/evidence/m0-t313-s3-p4-original-pic-irq-witness-001.md).

**T313 S3 P5:** fresh external formal graph `r2` completes all 403 selected
x86 CCPU40 edges and contains both original timer translation units:
`nt_timer.c` and `base/system/timer.c`.  The original heartbeat-to-PIC call
order is source/build closed; full PIT/BDA/EOI period behavior remains a
separate cohesive original-device endpoint and is not inferred from the
archive. See the [P5 record](etc/evidence/m0-t313-s3-p5-original-timer-package-formal-closure-001.md).

**T313 S3 P6:** a controlled original `timer.c` link probe confirms that
`TimerGenerateMultipleInterrupts` belongs to one PIT/EOI/device cohort, with
waveform, BDA, floppy, EOI-delay and PIF dependencies.  The probe introduced
no shim and was fully reverted; complete live-period behavior must be handled
only with that original cohort. See the [P6 record](etc/evidence/m0-t313-s3-p6-original-timer-device-boundary-001.md).

**T313 S3 closed:** original heartbeat suspension/resume, PIC IRQ0,
DMA/I/O and input have bounded evidence, and the selected original timer
translation units formally compose.  The whole PIT/EOI device package
transfers to S4 without a fixture-local timer claim. See the
[closure](history/m0-t313-s3-controller-lifecycle-closure-20260830.md).

**T313 S4 P1:** the formal selected graph directly archives the complete
original `system`, `video`/`cvidc`, `disks`, and `comms` packages.  A fresh
forced-link audit finds no new unresolved device-owner symbol; it leaves only
pre-existing EMS/DEM/debug/private-NT boundaries and records CCPU/C-video
duplicate exports as a non-runnable audit result, not a `/FORCE` runtime
solution. See the [P1 record](etc/evidence/m0-t313-s4-p1-device-package-assembly-001.md).

**T313 S4 P2:** original BIOS `reset()` remains the sole device-package
startup owner: ICA, timer, video, serial, DMA, floppy and condition-selected
printer initialization retain their source order.  A reached `reset.c`
x64-carrier/extra-declaration diff was removed, returning those locations to
the original mirror; a fresh selected x86 CCPU40 candidate completed 403/403.
See the [P2 record](etc/evidence/m0-t313-s4-p2-device-reset-order-and-mirror-cleanup-001.md).

**T313 S4 P3:** original video/C-video, storage and communications packages
retain their previously bounded direct or explicit-unavailable endpoints after
the fresh S4 build; the PIT/EOI relationship remains whole-package source
evidence, not a fabricated live timer loop.  No device shim was introduced.
See the [P3 record](etc/evidence/m0-t313-s4-p3-device-endpoint-reconciliation-001.md).

**T313 S4 closed:** original reset order, device package source selection,
bounded endpoints and immediate reached-mirror cleanup are recorded in the
[closure](history/m0-t313-s4-device-lifecycle-closure-20260830.md).  S5 now
owns finite execution, controlled stop/teardown and the final reached-path
purity reread.

**T313 S5 P1:** the selected original `TerminateVDM -> terminate ->
host_terminate` chain now transfers its original exit code through an
app-owned session escape instead of calling `ExitProcess`.  The bounded
original CCPU40 witness proves a completion code of 37 without process death;
a fresh selected formal graph completes 404/404 with the reached `nt_reset.c`
body.  See the [P1 record](etc/evidence/m0-t313-s5-p1-source-shaped-controlled-termination-001.md).

**T313 S5 P2:** all reached direct `ExitProcess` forms in the selected x86
CCPU40 host path—argument rejection, `host_terminate`, console close and SAS
allocation failure—now share the P1 session terminal boundary.  Unselected
and console-product-shell forms remain exact source material.  The changed
host archive rebuilds cleanly; see the [P2 record](etc/evidence/m0-t313-s5-p2-selected-direct-exit-sweep-001.md).

**T313 S5 P3:** the original startup's independent `DBGInit` slice now enters
the selected formal graph through a same-shaped debugger-boundary binding;
full NT4 debugger BOP/event product code remains explicitly unselected.  A
fresh 407/407 candidate and forced-link audit remove `DBGInit` from the true
unresolved set.  The remaining EMS `LIM_b_write`/`LIM_w_write` pair is an
original macro/configuration contradiction, not a missing adapter.  See the
[P3 record](etc/evidence/m0-t313-s5-p3-debugger-startup-binding-001.md).

**T313 S5 P4:** that original EMS/CCPU configuration contradiction is now
closed by selecting the already-present original `emm_mngr.c` LIM writeback
block in the CCPU40 graph.  A fresh 407/407 candidate exports all three
original `LIM_*_write` forms and removes the two CCPU unresolved edges without
an EMS shim.  No live EMS caller is claimed; see the [P4
record](etc/evidence/m0-t313-s5-p4-original-ems-writeback-closure-001.md).

**T313 S5 P5:** original `host_start_cpu()` bounded execution and the typed
session terminal have been reread with all reached CCPU/SAS/FPU, terminal and
EMS divergences either source-selected or registered.  The full original
debugger product was attempted, then correctly left unselected at its NT4
private Debug Subsystem boundary; the accepted selected CCPU40 graph remains
407/407. See the [final reread](etc/evidence/m0-t313-s5-p5-bounded-execution-and-purity-reread-001.md).

### M0 T313 closure record

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T313; Ordinary Mode (single-person dual-role implementation). |
| Result | Closed for selected Win32/x86 CCPU40 SoftPC capability and mirror-purity acceptance. |
| Closure | [T313 closure](history/m0-t313-softpc-capability-mirror-purity-closure-20260830.md) |
| Next Admission | Queue item 1: Base VDM source-shaped broker recovery. |

**T313 closed:** the selected original SoftPC package has finite CCPU
execution, a session-scoped terminal result, formal source composition and a
completed reached-path purity reread.  No DOS/WOW workload or private NT4
product shell is claimed. See the
[closure](history/m0-t313-softpc-capability-mirror-purity-closure-20260830.md).

### M0 T312 closure record

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T312; Ordinary Mode (single-person dual-role implementation). |
| Result | Closed. The selected original SoftPC/CCPU40/SAS route is the only production machine route. |
| Closure | [T312 closure](history/m0-t312-retired-bochs-removal-closure-20260830.md) |
| Next Admission | Queue item 1: Complete SoftPC capability and mirror-purity acceptance. |

**T312 closed:** retired Bochs source, build, fixture, and former #UD ingress
paths are no longer product inputs. A fresh selected x86 CCPU40 formal graph
completed its candidate and forced-link targets. See
[`closure`](history/m0-t312-retired-bochs-removal-closure-20260830.md).

**T310 S29 P1:** the original two-file OEM/Unicode support package is selected
directly, preserving PIF `ExpandEnvironmentStringsOem` and related host-file
contracts. Private console shortcut/menu APIs retain their call shapes and
explicit unavailable results. Fresh 380-edge x86 CCPU40 assembly evidence is
in [P1](etc/evidence/m0-t310-s29-p1-host-configuration-source-assembly-001.md).

**T310 S29 closed:** original OEM/PIF configuration contracts are selected and
private Console Server-only configuration calls now have exact unavailable
dispositions. See [closure](history/m0-t310-s29-host-configuration-bindings-closure-20260830.md).

**T310 S30 closed:** the original `nt_vdd.c` selected I/O-hook, IRQ and DMA
contracts formally link for x86 CCPU40.  The reached SFT/JFT shadow uses the
session's only host-resource mapping-manager instance; no VDD product shell or
machine-runtime claim is made. See [closure](history/m0-t310-s30-host-vdd-binding-closure-20260830.md).

**T310 S31 closed:** original host-BOP carriers remain selected and every
cross-owner selector has a named package disposition.  The host table stays
source-defined empty, with no project dispatcher introduced. See
[closure](history/m0-t310-s31-host-bop-binding-closure-20260830.md).

**T310 S32 closed:** the 110-record residual machine bucket contains no
independent runtime package.  Its original declaration carriers have been
returned to their true controller, video, platform, communications or
unselected-tool owner without a new wrapper. See
[closure](history/m0-t310-s32-other-machine-source-closure-20260830.md).

**T310 S33 closed:** original `ios.c` supplies the selected SoftPC port-routing
controller directly.  Historical process-level `terminat.c` exit is explicitly
transferred to S49 session lifecycle validation, not hidden behind a callback
shim. See [closure](history/m0-t310-s33-support-controller-callbacks-closure-20260830.md).

**T310 S34 closed:** original `suballoc.c` and `oemuni/{file,process}.c` are
direct selected x86 CCPU40 support-library bodies. Their historical `UMTEST`
programs remain outside the product graph; XMS/DPMI and DEM/PIF consumers keep
their named owner and S49 integration obligations. See
[closure](history/m0-t310-s34-mvdm-support-library-closure-20260830.md).

**T310 S36 closed:** selected original SIM32 bodies and the exact original
SAS invalidation facade are formally composed for x86 CCPU40. The existing
same-shaped `VdmQueryDir` monitor binding now enters the formal graph and
retains original slow directory-reset fallback when unbound. See
[closure](history/m0-t310-s36-mvdm-sim32-owner-package-closure-20260830.md).

**T310 S35 closed:** all selected original DEM and COMMAND provider bodies and
dispatch tables are directly composed. The reached private keyboard-layout call
is bound through a same-shaped public Win32 adapter; monitor/debugger residuals
have explicit S36/S42 transfers. See
[closure](history/m0-t310-s35-mvdm-dos-owner-package-closure-20260830.md).

**T310 S28 P1:** `RegisterConsoleVDM` now has its complete original ABI and
an explicit unavailable direction instead of an unresolved private Console
Server edge; the already source-shaped Base-client `ExitVDM` pair enters the
formal graph.  A fresh selected Win32/x86 CCPU40 candidate completed 374 Ninja
edges without a command-line CPU30 profile define.  S28 remains active; see
[P1 boundary record](etc/evidence/m0-t310-s28-p1-host-platform-source-boundary-001.md).

**T310 S28 P2:** the original SoftPC `base/debug/trace.c` is now the selected
trace provider; the unrelated MVDM debugger product is excluded.  The retained
NTVDMx64 `ActivityCheckAfterTimeSlice` no-op is linked only as its documented
patch contract, not as an idle-policy recovery.  Fresh x86 CCPU40 candidate
and forced-link evidence are recorded in
[P2 assembly record](etc/evidence/m0-t310-s28-p2-host-platform-trace-and-idle-assembly-001.md).

**T310 S28 closed:** all selected platform edges now have a direct,
binding-only, documented patch, explicit-unavailable, or later-owner
disposition.  See [closure](history/m0-t310-s28-host-platform-bindings-closure-20260830.md).

**T310 S22 P1:** the selected host-storage surface is now separated into the
original no-fixed-disk CCPU callbacks, DEM direct DASD, local floppy and the
unimplemented historical remote-floppy service. Existing guest-location and
private-buffer divergences remain the only identified mirror changes. See the
[S22 source boundary](etc/evidence/m0-t310-s22-p1-host-storage-source-boundary-001.md).

**T310 S22 closed:** the original no-fixed-disk callbacks, direct DASD and
local-floppy bodies retain their source ordering and existing same-shaped
bindings; the `SLAVEPC` remote protocol remains exact condition-unselected
unavailable. A fresh 369-edge x86 CCPU40 graph and safe unavailable-name
fixture pass. Real media behavior remains S49 work. See the
[S22 closure record](history/m0-t310-s22-host-storage-bindings-closure-20260830.md).

**T310 S23 P1:** the original UART controller and INT 14 front end remain
separate from their later host endpoint bindings; serial reset ordering stays
in the original BIOS and controller sources.  Printer/parallel/screen-print
sources remain present but are exact `PRINTER`-unselected paths in the selected
CCPU40 configuration, with no fabricated device result.  See the
[S23 source boundary](etc/evidence/m0-t310-s23-p1-comms-controller-source-boundary-001.md).

**T310 S23 P2:** a fresh 369-edge selected x86 CCPU40 Ninja graph archived all
five original communications sources and produced a forced-link ownership
audit with no communications-owner unresolved symbol.  The retained printer
result is exact `PRINTER`-unselected unavailability; host endpoints remain
S24 and integrated behavior remains S49.  See the [S23 formal assembly
evidence](etc/evidence/m0-t310-s23-p2-comms-controller-formal-assembly-001.md).

**T310 S24 P1:** the original `nt_com` serial endpoint lifecycle and its
`nt_ntfun` IOCTL/wait helpers have a finite direct public-API closure; their
native handles/events remain private source state.  WOW callbacks remain S39,
and the `PRINTER`/MONITOR LPT source remains condition-unselected without a
synthetic provider.  See the [S24 source boundary](etc/evidence/m0-t310-s24-p1-host-comms-source-boundary-001.md).

**T310 S24 P2:** a fresh 369-edge x86 CCPU40 graph archives the original host
communications roots and communications package with no S24-owner unresolved
forced-link edge.  A safe impossible-COM-name fixture passes without touching
a real endpoint; live serial behavior remains S49.  See the [S24 formal
assembly evidence](etc/evidence/m0-t310-s24-p2-host-comms-formal-assembly-001.md).

**T310 S25 closed:** original `vglfunc.c` and `evidfunc.c` again own the
selected C-VID generated `VideoVector` and EVID tables directly: 156 vector
slots, 14 tables and 692 generated entries.  A fresh 369-edge x86 CCPU40
formal graph archives those original sources and has no selected table
unresolved symbol.  The x64-only per-slot wrapper generator was removed;
video-controller behavior remains S26 and integrated behavior S49.  See the
[closure record](history/m0-t310-s25-cvidc-generated-dispatch-closure-20260830.md).

**T310 S26 P1:** all nineteen selected original `base/video` controller
translation units now compile directly for the x86 CCPU40 product; the former
generated five-file wrapper carrier was removed.  Two registered declaration
corrections preserve the original void marking and EGA writer-table contracts.
Fresh direct archive evidence is recorded in the [S26 source boundary](etc/evidence/m0-t310-s26-p1-video-controller-direct-source-boundary-001.md).

**T310 S26 closed:** original `base/video` controller sources, selected CCPU
state carrier and EGA writer-table selection now compose under CCPU40 without
an unresolved controller/C-VID/effective-address edge.  The remaining host
invalidation endpoint is explicitly S27; integrated display behavior remains
S49.  See the [closure record](history/m0-t310-s26-video-controller-dispatch-closure-20260830.md).

**T310 S27 P1:** the original host-video callers divide into direct public GDI
palette forms and retired NT4 Console Server graphics/event forms.  The latter
require one session-owned presenter capability or an exact unavailable result;
they do not authorize a replacement controller or GUI.  See the [S27 source
boundary](etc/evidence/m0-t310-s27-p1-host-video-source-boundary-001.md).

**T310 S27 closed:** public GDI palette calls now link directly, while retired
Console Server invalidation/palette/activity forms retain their original ABI
through a session-owned presenter notification.  Unbound presentation fails
explicitly; no GUI or controller replacement was introduced.  See the
[closure record](history/m0-t310-s27-host-video-bindings-closure-20260830.md).

**T310 S17 P1:** the complete selected system-controller source set is now
split from adjacent ROM, platform, endpoint and BOP owners. The original
controller initialization/heartbeat order stays in its source bodies; every
source unit has a named current or later owner rather than a new generic
scheduler. See the [S17 controller ledger](etc/evidence/m0-t310-s17-p1-system-controller-owner-ledger-001.tsv)
and [source boundary](etc/evidence/m0-t310-s17-p1-system-controller-source-boundary-001.md).

**T310 S17 P2 / closure:** the original PIT, PIC/ICA, CMOS/RTC, DMA,
quick-event and selected host callback forms remain original bodies or an
already registered same-shaped call binding. The formal x86 CCPU40 archive
remains complete; live interrupt delivery awaits later endpoint and host
binding packages, then S49. See the [S17 closure record](history/m0-t310-s17-system-controller-callbacks-closure-20260830.md).

**T310 S18 closure:** original heartbeat creation, event initialization,
alertable teardown and timer/controller call order remain source-shaped. The
sole private `NtAlertThread` call keeps its registered same-shaped APC wake
binding; local CCPU interrupt-handler tables remain local rather than becoming
a BaseSrv route. The formal selected x86 CCPU40 graph remains complete; actual
IRQ delivery and reset lifecycle stay mandatory S49 integration work. See the
[S18 closure record](history/m0-t310-s18-host-system-interrupt-bindings-closure-20260830.md).

**T310 S19 closure:** the original keyboard callback assignments, IVT-checked
INT 15h dispatch and range-checked 53-entry mouse table are source-composed
under the selected x86 CCPU40 profile. The two retained mirror corrections
remain minimal type/private-backing repairs; host console input is now S20 and
integrated input delivery remains S49. See the [S19 closure record](history/m0-t310-s19-input-controller-callbacks-closure-20260830.md).

**T310 S20 closure:** original keyboard conversion remains byte-exact, while
the five reached NT4 Console Server call shapes now have a narrowly scoped,
same-shaped public Win32 binding. Menu/fullscreen console behavior remains
explicitly unavailable for S27. Formal x86 CCPU40 archive and forced-link
evidence, plus a focused negative console-contract test, pass; live input
delivery remains S49 integration work. See the [S20 closure record](history/m0-t310-s20-host-input-bindings-closure-20260830.md).

**T310 S21 closure:** original INT 13, WD2010, FDC, remote-floppy-client and
EMS controller bodies retain their table/callback order. The one recovered
`host_delays` timing global is original controller state, not a new adapter;
fresh x86 CCPU40 formal assembly has no unresolved `original-softpc-disks.lib`
edge. Host media semantics are now S22 and integrated guest media behavior
remains S49. See the [S21 closure record](history/m0-t310-s21-storage-controller-callbacks-closure-20260830.md).

**T310 S16 P1:** the firmware owner audit separates immutable original
`softpc.new/{bios,roms,data}` inputs from executable original BIOS, system-ROM,
CMOS and host-resource bodies. It preserves the existing narrow app/session
location binding while retaining original `host_find_file -> host_read_resource
-> read_rom` control flow; writable historical resource handling is explicitly
pending policy disposition. See the [S16 firmware audit](etc/evidence/m0-t310-s16-p1-bios-firmware-owner-audit-001.md).

**T310 S16 P2:** a fresh formal Win32/x86 CCPU40 graph completed 369/369
edges with the complete original BIOS manifest, original ROM/CMOS and host
resource bodies, immutable firmware input mirror and current narrow location
binding. Its forced-link audit has no unresolved `host_find_file`,
`host_read_resource`, `read_rom` or CMOS resource edge; remaining forms are
later owner packages. See the [S16 formal assembly evidence](etc/evidence/m0-t310-s16-p2-bios-firmware-formal-assembly-001.md).

**T310 S16 P3:** all fourteen members of the original `base/bios` manifest
now have an initial source-unit owner disposition. The unchanged `BIOS[]` and
`reset` control forms remain original and point to named later controller/BOP
owners rather than a replacement dispatcher or reset shim. See the
[S16 manifest ledger](etc/evidence/m0-t310-s16-p3-bios-manifest-owner-ledger-001.tsv)
and [reset/dispatch boundary](etc/evidence/m0-t310-s16-p3-bios-reset-dispatch-boundary-001.md).

**T310 S16 P4:** original CMOS/profile persistence retains its source function
and ordering, but the one retry after `host_find_file` now remains in the
already selected session firmware root rather than escaping to process CWD.
ROM/BIOS inputs remain immutable; no new resource provider exists. The changed
original member rebuilt in the formal x86 CCPU40 graph. See the
[S16 resource-write evidence](etc/evidence/m0-t310-s16-p4-firmware-resource-write-root-001.md).

**T310 S16 P5:** the original first-session `scs_init → AddSystemFiles` order
and void contract remain intact, but the selected CCPU40 product explicitly
does not create Brief-installer placeholder `C:\\MSDOS.SYS` or `C:\\IO.SYS`
on an admitted real host drive. The complete original helper remains in the
mirror's unselected branch; the named adapter records the narrow no-write
disposition. A fresh formal graph completed 369/369 selected x86 CCPU40
edges. This is a source/binding closure only; startup's COMMAND/DEM/XMS/DBG,
reset and controller behavior remain later owner work. See the
[S16 startup-system-file evidence](etc/evidence/m0-t310-s16-p5-startup-system-file-disposition-001.md).

**T310 S16 closed:** all selected BIOS-manifest, ROM/CMOS, resource-location
and first-session system-file source edges now have one disposition, with a
fresh selected x86 CCPU40 candidate and forced-link ownership audit. Reset and
BIOS dispatch remain original, deferred controller integration work rather
than a fabricated firmware claim. See the
[S16 closure record](history/m0-t310-s16-bios-firmware-bindings-closure-20260830.md).

**T310 S14 closed:** the original `VdmAddVirtualMemory` / remove lifecycle
and source-shaped DOS page alias now use the one session mapping-manager
identity boundary. Fresh x86/x64 runs proved publication, binding, alias
write-through, explicit unmap failure and release before reaching
`returned-recursive`; no raw pointer entered the historical 32-bit ABI. See
the [S14 memory-mapping review](etc/evidence/m0-t310-s14-memory-mapping-bindings-contract-001.md).

**T310 S15 P1:** the original six-unit XMS common package and the selected
same-shaped guest-memory backend compile in formal archives. The current
recovery gate is Win32/x86; x64 evidence is retained only as deferred
compatibility information.
The direct `i386/xmsmem86.c` host-pointer backend remains source evidence only;
runtime contract recovery is still active. See the [S15 source-closure
baseline](etc/evidence/m0-t310-s15-p1-xms-source-closure-001.md).

**T310 S15 P2:** the x86 profile audit confirms that a CPU30/MONITOR machine
row is distinct from the selected original CCPU40 graph. The product now
selects only CCPU40; the historical CPU30/MONITOR row is explicitly out of
scope. The original CCPU graph retains `CPU_30_STYLE` as a compatibility
preprocessor carrier, not as a second supported profile. See the
[S15 x86 profile evidence](etc/evidence/m0-t310-s15-p2-x86-profile-boundary-001.md).

**T310 S15 P3:** the complete original `mvdm-support/suballoc` member now
links with the original XMS package, its matching private A20 overlay, the
same-shaped historical SAS A20 facade, and the existing session lease owner in
the formal x86 CCPU40 candidate. The forced-link audit has no unresolved XMS
allocator/A20/session edge; this is source-assembly closure, not runtime
closure. See the [S15 original-owner assembly evidence](etc/evidence/m0-t310-s15-p3-xms-original-owner-assembly-001.md).

**T310 S15 P4:** the XMS declaration/definition/caller ledger gives every
selected provider one original, binding-only, adapter-backed or explicit
unavailable disposition. `nt_msscs → XMSInit` and `nt_bop → XMSDispatch` are
the original runtime entries; unsupported historical declarations remain
unfabricated. The remaining gate is reachable SoftPC lifecycle behavior, not
an unresolved XMS source edge. See the [S15 contract disposition](etc/evidence/m0-t310-s15-p4-xms-contract-disposition-001.md).

**T310 S15 closed:** all 25 selected XMS declaration, definition, caller and
repair rows have a single source-shaped disposition, and the formal Win32/x86
CCPU40 original-owner graph completes 369 edges with no unresolved XMS,
SubAlloc, A20 or session-lease edge. The original reachable callers require
the subsequent firmware/system machine lifecycle; historical Bochs fixtures
are not accepted as SoftPC runtime proof. This is XMS package/source closure
with deferred integrated behavior verification at S49, not a claim that the
guest has run XMS. See the [closure record](history/m0-t310-s15-xms-owner-package-closure-20260830.md).

**T310 S13 closed:** the selected original CPU_40 SAS-memory owner retains
its ordinary RAM vector, type map, scalar/string/move/fill algorithms and
private native backing without exporting a host identity. Fresh x86/x64 runs
proved that complete ordinary-RAM surface and then reached
`returned-recursive`; A20/XMS, EMS mapping and controller consequences remain
with their named later packets. See the [S13 SAS-memory review](etc/evidence/m0-t310-s13-sas-memory-contract-001.md).

**T310 S12 closed:** the selected CCPU extended-BOP default now links the
registered original NTVDMx64 patch's one-symbol `EDL_fast_bop(ULONG)`
debug-break disposition, rather than a fixture-local no-op. Fresh formal
x86/x64 graphs observed the expected breakpoint and then reached
`returned-recursive`; no selector/service semantics were enabled. See the
[S12 extended-BOP-default review](etc/evidence/m0-t310-s12-ccpu-extended-bop-default-contract-001.md).

**T310 S11 closed:** the selected `effective_addr -> c_effective_addr`
contract remains fixed-width guest-linear arithmetic, not the excluded
MONITOR process-pointer alias. Fresh 146-node x86/x64 selected graphs linked
the same provider and the bounded executions reached `returned-recursive`; see
the [S11 effective-address review](etc/evidence/m0-t310-s11-ccpu-effective-address-contract-001.md).

**T310 S10 closed:** selected `sas_init -> SasSetPointers -> cSasPtrs -> Sas`
keeps the original generated vector layout, provider order and unavailable
overwrite slot. Fresh x86/x64 bounded execution verified vector-mediated RAM
access and reached `returned-recursive`; see the
[S10 SAS-vector review](etc/evidence/m0-t310-s10-ccpu-sas-monitor-vector-contract-001.md).

**T310 S9 closed:** selected `CPU_30_STYLE + CCPU` access-table declarations,
storage and initializer assignments now use the source-generated fixed-width
contract. Fresh x86/x64 bounded execution checked AX, CF and simulation slots
through the original table and reached `returned-recursive`. The incremental
whole-candidate forced-link retry was stopped at the unrelated later-owner
`base/support/time_day.c` compiler stall; it is not hidden or repaired by S9.
See the [S9 access-table review](etc/evidence/m0-t310-s9-ccpu-access-table-contract-001.md).

**T310 subdivision:** S8--S49 are 42 sequential owner-contract packets, one
per cluster in the current coverage ledger. A packet first re-reads the
complete original callable contract, then makes the smallest source-shaped
repair, then proves selected x86 CCPU40 compilation and any locally meaningful behavior.
The full-tree 42-cluster warning ledger is a cross-cluster audit and planning
baseline, not an exit gate for the active S: S8 considers only evidence whose
owner is `SPC-CCPU-EXECUTOR-DISPATCH`; other clusters remain visible for their
own future packet. The final cross-cluster packet performs the machine-profile
verification; neither a compiler warning nor a trace hit creates an extra packet. Bochs
production-closure removal, `i386` semantic audit and complete SoftPC
capability/mirror-purity acceptance remain separate queued T packages. See the
amended [S8 plan](etc/operations/m0-t310-s8-original-machine-composition-plan-001.md)
and [package-recovery program](etc/operations/package-recovery-program-20260827.md).

**T310 S8 closed:** the selected real-FPU CCPU profile excludes the alternate
`ntstubs.c` carrier, while retaining it byte-for-byte as mirror evidence.
Fresh 362-node x86/x64 candidate graphs and forced-link audits passed; the
only remaining CCPU-attributed unresolved edges are the later debug trace and
EMS/LIM owners.  Fresh bounded x86/x64 execution both reached
`returned-recursive`.  See the [P6 formal closure](etc/evidence/m0-t310-s8-p6-ccpu-executor-formal-closure-001.md).

**T310 S8 P4:** a new isolated cold formal MSVC `/MT` baseline reaches the
same `362/362` selected-source graph on x86 and x64 with zero errors. Its
complete compiler output was reclassified only for `mvdm-host` and
`mvdm-support`: 19,440 cross-architecture source-diagnostic identities and
1,009 x86/x64 review records remain visible. The live ledger is rebuilt from
that output, so a warning still emitted today cannot inherit an old
`closed-*` result. This is an audit baseline, not an S8 closure claim; see the
[current full-tree baseline](etc/evidence/m0-t310-s8-p4-whole-tree-current-baseline-001.md).

**T310 S8 P4:** the source-missing CCPU `EDL_fast_bop(ULONG)` default now
links only the imported NTVDMx64 patch's original debug-break disposition, as
a one-symbol archive. Refreshed `359/359` x86/x64 forced links contain no
unresolved `EDL_fast_bop`; this is not BOP semantic recovery. The repair is
explicitly retained in the CCPU executor owner-contract cluster; see the
[EDL default-contract closure](etc/evidence/m0-t310-s8-p4-ccpu-edl-fast-bop-default-contract-closure-001.md).

**T310 S8 P4:** the selected CPU_40 SAS direct overwrite contract and the
original `nt_mem` private-pointer diagnostic now compile with their actual
fixed-width/native-width ABIs on both targets. The kernel-VDM-only EMS
`VdmMapDosMemory`/`VdmUnmapDosMemory` semantics remain separately pending;
they were not hidden behind declarations. See the [SAS/native-pointer
closure](etc/evidence/m0-t310-s8-p4-sas-native-pointer-call-contract-closure-001.md).

**T310 S8 P4:** original host-only console-title and retained XMS-stub
diagnostics now pass native pointers through `%p` on both targets; scalar
arguments retain their original fixed-width roles. Non-width source warnings
remain visible. See the [host native-format closure](etc/evidence/m0-t310-s8-p4-host-native-format-closure-001.md).

**T310 S8 P4:** the complete generated C-video assertion-provider contract
now retains the existing original `ev_glue.c` `void(char *)` ABI. All 31
selected `SEVID` units rebuilt from clean x86/x64 outputs without an implicit
`CrulesRuntimeError` call; the remaining C-video table/callback and CRT
clusters remain visible and pending. See the [C-video runtime-error closure](etc/evidence/m0-t310-s8-p4-cvidc-runtime-error-call-contract-closure-001.md).

**T310 S8 P4:** all six selected original C `printf`/`sprintf` implicit-call
records now use their native variadic declarations. The actual CCPU, C-video
and host-root Ninja targets rebuilt on both architectures without those C4013
records or warning suppression; see the [native C varargs closure](etc/evidence/m0-t310-s8-p4-native-c-varargs-call-contract-closure-001.md).

**T310 S8 P4:** the selected CCPU facade and exception paths now consume their
existing private CCPU/Yoda contracts without implicit calls or a conflicting
CRT-environment redeclaration. Formal x86/x64 CCPU archive rebuilds are clean
of those four reviewed ABI diagnostics; see the [CCPU private-call closure](etc/evidence/m0-t310-s8-p4-ccpu-private-call-contract-closure-001.md).

**T310 S8 P4:** the original SoftPC startup sequence now has exact CCPU,
`nt_msscs`, and mouse provider contracts on both host widths. Its formal
support/host-root archives rebuild without those three implicit calls; see the
[startup call-contract closure](etc/evidence/m0-t310-s8-p4-softpc-startup-call-contract-closure-001.md).

**T310 S8 P4:** `nt_msscs` now binds its original PIF and EMS page-frame
providers through their exact contracts; x86/x64 host-root and DOS archives
rebuild without those implicit calls. See the [PIF/LIM call closure](etc/evidence/m0-t310-s8-p4-pif-lim-call-contract-closure-001.md).

**T310 S8 P4:** the selected keyboard and BIOS tape paths now consume the
original XMS A20 interface header at their top-level boundaries. Both x86/x64
units rebuild without the two former implicit A20 calls; see the [XMS A20 call
closure](etc/evidence/m0-t310-s8-p4-xms-a20-call-contract-closure-001.md).

**T310 S8 P4:** BIOS reset now binds the original NT CMOS shutdown-byte helper
through its exact contract; x86/x64 BIOS and system archives rebuild without
that implicit call. See the [CMOS reset call closure](etc/evidence/m0-t310-s8-p4-cmos-reset-call-contract-closure-001.md).

**T310 S8 P4:** source-list recovery proved that the 31 `ga_glue.c` x86/x64
warnings came from a historical C-video variant deliberately commented out by
the original `base/video/sources` manifest. The Ninja manifest reader now
honors that comment, retaining the file as byte-identical evidence but not
compiling it into the selected CPU_40/EVID machine. No pointer cast,
mapping-manager route or controller shim was added; see the
[profile source-selection closure](etc/evidence/m0-t310-s8-p4-video-profile-source-selection-closure-001.md).

**T310 S8 P4:** the four selected `cvidc/ev_glue.c` function-pointer,
generated-carrier and GDP-width records are now confirmed closed by the
existing minimal `MVDM-HOST-DIV-062` and `...048` source changes. Fresh
formal x86/x64 compilation emits none of their C4113/C4133/C4312 diagnostics;
unrelated generated-source warnings remain visible. See the
[selected cvidc glue closure](etc/evidence/m0-t310-s8-p4-cvidc-selected-glue-abi-closure-001.md).

**T310 S8 P4:** the seventeen generated C-video mark-rule declarations now
match their existing four-`IUH` definitions.  The source-derived typed EVID
table carrier remains the sole table adaptation; forced formal x86/x64 carrier
rebuilds contain no `evidfunc.h` C4431 diagnostics. See the [mark-rule
declaration closure](etc/evidence/m0-t310-s8-p4-cvidc-mark-rule-declaration-closure-001.md).

**T310 S8 P4:** all five selected C-video native-`IUH` diagnostic varargs
calls now use `PRIuPTR` rather than the x86-only `%d` assumption. Formal
x86/x64 object rebuilds close their C4477 mismatch without affecting any
generated rule or display behavior; see the [native-word varargs
closure](etc/evidence/m0-t310-s8-p4-cvidc-native-word-varargs-closure-001.md).

**T310 S8 P4:** the two selected C-video generated native-word bit tests now
make their literal operand `IUH`, matching the original `IUH` carrier and its
existing `sizeof(IUH)` bound on both architectures. Formal x86/x64 object
rebuilds contain no C4334 for either source; generated control-flow warnings
remain visible. See the [native-word shift
closure](etc/evidence/m0-t310-s8-p4-cvidc-native-word-shift-closure-001.md).

**T310 S8 P4:** all four remaining selected C-video `ev_glue.c` C4244 records
are proven fixed-width byte/word video scalar conversions on both hosts, not
native-width ABI transitions. They remain visible and source-unchanged; see
the [fixed-width BIOS-write disposition](etc/evidence/m0-t310-s8-p4-cvidc-fixed-width-bios-write-disposition-001.md).

**T310 S8 P4:** all 42 generated `SINIT012/013` C-video `crules` C4033
records are a shared source-generated control-result form on both targets,
not a host-width or callable-ABI transition. They remain visible and await
C-video execution coverage; see the [generated control disposition](etc/evidence/m0-t310-s8-p4-cvidc-generated-crules-control-disposition-001.md).

**T310 S8 P4:** the complete 36-record original floppy C4431 cluster is a
shared K&R default-int source form; its declaration/definition contracts are
identical on x86 and x64 and no native identity crosses. The mirror remains
unchanged; see the [floppy source-form disposition](etc/evidence/m0-t310-s8-p4-floppy-default-int-disposition-001.md).

**T310 S8 P4:** all reviewed `stubs.c`/`nt_fulsc.c` default-int records and
the private `nt_unix.c` byte/character buffer views are source forms, not
x86/x64 ABI transitions. They remain compiler-visible and unchanged; see the
[host source-form disposition](etc/evidence/m0-t310-s8-p4-host-stub-source-form-disposition-001.md).

**T310 S8 P4:** `c_main.c` now preserves the private host decode-cursor
difference as an explicit bounded instruction-byte count before its original
EIP update forms.  Focused formal x86/x64 rebuilds close the native-width
`__int64` narrowing while retaining unrelated fixed-width Intel warnings; see
the [CCPU instruction-delta closure](etc/evidence/m0-t310-s8-p4-ccpu-instruction-delta-width-closure-001.md).

**T310 S8 P4:** the reached original `POPFD` diagnostic now declares its
same-package `void note_486_instruction(char *)` contract rather than relying
on modern C's implicit `int`. The x86/x64 formal CCPU objects rebuild without
that C4013; the separate source-missing `EDL_fast_bop` operation remains an
unclosed CCPU-to-BOP contract. See the [note-486 call-contract
closure](etc/evidence/m0-t310-s8-p4-ccpu-note486-call-contract-closure-001.md).

**T310 S8 P4:** the imported NTVDMx64 `fmstubs.c` patch now uniformly expresses
its retained original debugger-break behavior with `__debugbreak()`; no
x86-only inline `int 3` remains. Its formal patch object rebuilds on both
x86 and x64. This does not enable its placeholder paths or change their
default debug-break disposition.

**T310 S8 P4:** owner-contract review now includes prior work rather than only
the pending warning rows.  The generated
[coverage ledger](etc/operations/m0-t310-s8-p4-owner-contract-coverage-ledger.tsv)
maps every current P4 evidence file to an owner cluster and requires its
mirror diff, overlay or adapter binding to be reread with the full original
declaration/definition/caller contract.

**T310 S8 P4:** `c_effective_addr(IU16, IU32)` now has a source-shaped numeric
guest-linear recovery using the selected CCPU segment cache and descriptor
reader, not the excluded MONITOR `Sim32GetVDMPointer` alias. The dedicated
adapter archive compiles on x86 and x64. It remains subject to the parent
CCPU/SAS vector reread with `c_sas_touch`, `c_VirtualiseInstruction`, their
vector initializer and all original callers; see the [effective-address
recovery](etc/evidence/m0-t310-s8-p4-ccpu-effective-address-recovery-001.md).

**T310 S8 P4:** a new complete formal Ninja run reached `[357/357]` on both
x86 and x64 after the CCPU decode-cursor repair. Its freshly reconciled
compiler ledger contains 19,443 unique `mvdm-host`/`mvdm-support` diagnostics
and 910 source-context worklist records; prior P4 evidence remains subject to
the same owner-contract reread. See the updated [current full-tree
baseline](etc/evidence/m0-t310-s8-p4-whole-tree-current-baseline-001.md).

**T310 S8 P4:** source review classifies all 100 CCPU `c_main.c` C4057
operand calls as original fixed 32-bit Intel state, not host-width ABI
transitions. They remain compiler-visible but leave the x86/x64 repair
worklist without casts, suppression or a new adapter; see the [fixed-width
operand disposition](etc/evidence/m0-t310-s8-p4-ccpu-fixed-width-operand-call-disposition-001.md).

**T310 S8 P4:** the two `WOWpSysErrorBox` pointer truncations are now
classified as an NT4 kernel/CSRSS `NtRaiseHardError` hard boundary. They stay
visible as mirror evidence and do not authorize an invented user-mode ABI;
see the [hard-error boundary
disposition](etc/evidence/m0-t310-s8-p4-nt-hard-error-boundary-disposition-001.md).

**T310 S8 P4:** `nt_sec` now preserves its original null pseudo-handle error
value without truncating a native `HANDLE`. The selected non-MONITOR build
does not thereby claim fullscreen regeneration complete; that future path
remains a source-shaped physical-mapping recovery. See the [pseudo-handle
width closure](etc/evidence/m0-t310-s8-p4-nt-sec-pseudohandle-width-closure-001.md).

**T310 S8 P4:** the two selected C-video generated native-word bit tests now
make their literal operand `IUH`, matching the original `IUH` carrier and its
existing `sizeof(IUH)` bound on both architectures. Formal x86/x64 object
rebuilds contain no C4334 for either source; generated control-flow warnings
remain visible. See the [native-word shift
closure](etc/evidence/m0-t310-s8-p4-cvidc-native-word-shift-closure-001.md).

**T310 S8 P4:** the formal full x64 graph exposed one additional selected
base/video declaration mismatch: `ega_writ.c` declared the word-fill EVID glue
with `IU8` even though its original body and handler slot use `IU16`.
`MVDM-HOST-DIV-074` corrects that declaration only; focused formal x86/x64
rebuilds are clean. See the [EGA word-fill ABI closure](etc/evidence/m0-t310-s8-p4-ega-word-fill-abi-closure-001.md).

**T310 S8 P4:** original `config_inquire` scalar consumers now retain the
private pointer-sized configuration carrier until their original enumerated
destination. This closes the selected BIOS/ROM C4311 pointer truncations on
both targets without mapping tokens or warning suppression; see the
[configuration-scalar closure](etc/evidence/m0-t310-s8-p4-config-scalar-native-width-closure-001.md).

**T310 S8 P4:** `display_string` now holds the original ROM scratch-pad
message length in native-width temporaries until its existing bounded guard
proves the final `sys_addr` result. The x86/x64 ROM rebuild is free of the two
former C4267 entries; see the [ROM scratch-length closure](etc/evidence/m0-t310-s8-p4-rom-scratch-length-closure-001.md).

**T310 S8 P4:** the selected original XMS move and UMB paths no longer cast
guest numeric addresses through host pointers. `xmsMoveBlock` retains its
original descriptor order, forward-copy and AX success contract through the
session's bounded guest-memory lease; UMB conversion reuses the existing
numeric physical-address codec. Focused formal `/MT` x86/x64 XMS and adapter
compiles close the former C4213/C4311/C4312 records without suppressing the
remaining XMS call-contract warnings; see the [XMS guest-address closure](etc/evidence/m0-t310-s8-p4-xms-guest-address-abi-closure-001.md).

**T310 S8 P4:** original COMMAND environment collection now keeps native
host-string lengths until the existing `UNICODE_STRING`, DOS, and `VDMENVBLK`
boundaries. The source-owned failure directions remain intact; focused formal
`/MT` x86/x64 rebuilds contain none of this cluster's C4244/C4267 diagnostics,
while non-width warnings remain visible. See the [COMMAND environment
closure](etc/evidence/m0-t310-s8-p4-command-environment-native-width-closure-001.md).

**T310 S8 P4:** COMMAND's packed guest SCS/standard-handle records no longer
pack host-private pipe, thread, critical-section, or redirection records.
The original guest ABI and redirection algorithm remain unchanged; focused
formal x86/x64 rebuilds contain none of the former x64 C4366 diagnostics. See
the [COMMAND host-record alignment closure](etc/evidence/m0-t310-s8-p4-command-host-record-alignment-closure-001.md).

**T310 S8 P4:** the two original COMMAND pipe workers now use the same
session-aware, cdecl-to-WINAPI thread boundary as the recovered child worker.
Their source bodies and launch ordering remain original; formal x86/x64
rebuilds prove the callback contract without hiding the retained K&R source
diagnostics. See the [COMMAND pipe-worker callback
closure](etc/evidence/m0-t310-s8-p4-command-pipe-worker-callback-closure-001.md).

**T310 S8 P4:** the formal full x64 graph exposed one additional selected
base/video declaration mismatch: `ega_writ.c` declared the word-fill EVID glue
with `IU8` even though its original body and handler slot use `IU16`.
`MVDM-HOST-DIV-074` corrects that declaration only; focused formal x86/x64
rebuilds are clean. See the [EGA word-fill ABI closure](etc/evidence/m0-t310-s8-p4-ega-word-fill-abi-closure-001.md).

**T310 S8 P4:** five residual selected video call warnings were read and
classified as original 32-bit scalar/signedness issues, not x86/x64 ABI
transitions. They stay visible and unmodified rather than being hidden by
casts or warning policy; see the [non-width disposition](etc/evidence/m0-t310-s8-p4-video-nonwidth-warning-disposition-001.md).

**T310 S8 P4:** `c_main.c` now consumes its existing address, segment, PIC,
timer, debug and NPX call contracts with exact declarations, and passes its
existing TLS `jmp_buf` frames at the correct pointer level. Formal x86/x64
builds close those twelve ABI records without suppressing the remaining
fixed-width guest-scalar warnings; `EDL_fast_bop` remains a distinct undefined
boundary. See the [executor-call closure](etc/evidence/m0-t310-s8-p4-ccpu-executor-call-contract-closure-001.md).

**T310 S8 P4:** all 111 `c_main.c` C4244 narrowing records have now been
read as fixed-width Intel instruction state, not native host-width transfers.
They remain compiler-visible but leave the x86/x64 repair queue without casts
or suppression; see the [fixed-width disposition](etc/evidence/m0-t310-s8-p4-ccpu-fixed-width-scalar-disposition-001.md).

**T310 S8 P4:** the 40 CCPU signed/unsigned and unary-minus diagnostics were
also read across the executor tree. They concern fixed-width Intel arithmetic,
not host-width transport; they remain visible and source-unchanged. See the
[signedness disposition](etc/evidence/m0-t310-s8-p4-ccpu-fixed-width-signedness-disposition-001.md).

**T310 S1 closure:** `6a46ee748` establishes the exact original `ccpu386`,
`host/src` and `obj.vdm` roots, hashes, historical source roles and a
per-session one-backend contract. It reverses the previous planning assumption
that CCPU must remain a permanent Bochs replacement exclusion. Its only
pre-approved mirror-change class is source-shaped conversion of a historical
32-bit host pointer/handle/VDM alias into the existing session mapping-manager
and bounded-lease contract.

**T310 S2 progress:** the formal isolated candidate now archives all 130 exact
original `ccpu386` translation units plus ten selected original SoftPC host
roots on both MSVC `/MT` x86 and x64. The owner-directed NTVDMx64
`fmstubs.c` is imported with one registered syntax-only divergence and
separately compiled as x86/x64 patch evidence; both use `__debugbreak()` with
the same immediate debugging-break contract. Its four hooks remain
unimplemented stop evidence. The selected
two-site `callconv.patch` correction permits the original error-dialog worker
to enter the modern x86 `CreateThread` ABI without rewriting its behavior.
Raw `nt_mem`/`sim32` address conversions and all unresolved link forms remain
S2 source-first interface work, not a runnable-backend claim. See the
[candidate build evidence](etc/evidence/m0-t310-s2-original-softpc-candidate-build-001.md).

**T310 S2 forced-link audit:** formal MSVC `/MT` x86 and x64 Ninja graphs now
archive all selected original bodies and generate deliberate non-runnable
whole-archive DLLs.  The x86/x64 physical ledgers leave 84/83 records,
respectively, each with an original-source or named-boundary disposition.  The
owner-directed `fmstubs.c` is evidence-only on both architectures; the four
hooks remain explicit `debugbreak-patch-default` unresolved forms on both
candidate ledgers, and no hook has been silently omitted or treated as
runnable. See the
[forced-link audit](etc/evidence/m0-t310-s2-original-softpc-forced-link-audit-001.md).

**T310 S2 closure:** the exact CCPU and selected host-control roots compile,
archive and whole-archive forced-link on both supported host architectures.
All residual forms are now a finite source-first workset rather than compiler
noise.  S3 may implement only the copied selection/lifecycle binding; S4 owns
all original SoftPC machine-family source selection and composition.

**T310 scope clarification:** S2 remains an isolated CPU/host-control build
closure only.  The admitted T310 plan now requires a later original SoftPC
machine-composition S covering the selected firmware and device families
before the backend can be called runnable; final acceptance requires the
bounded create/reset/initialize/run/stop/teardown path and explicit device
dispositions.

**T310 S3 P1:** a session now selects exactly one fixed-width backend before
activation; app selection defaults to SoftPC while Bochs is explicit. SoftPC
cannot silently construct or resume a Bochs machine before S4 composition.
Formal Ninja x86/x64 focused fixtures prove selection rejection, default
admission and `host_simulate` non-fallback; see the [S3 evidence]
(etc/evidence/m0-t310-s3-selected-backend-lifecycle-001.md).

**T310 S3 closure:** the selected-backend lifecycle is closed. Its product
default is SoftPC; before S4 makes that original composition runnable the
selection fails explicitly and never changes to Bochs. The x86/x64 tests cover
the default and explicit choice rows, late-selection rejection, source-shaped
`host_simulate` routing and teardown.

**T310 S4 P1:** the original SoftPC machine scope is now a nine-family,
initialization-ordered workset. Firmware, memory/SAS, UMB/VDD, PIC/PIT,
keyboard/mouse, DMA, display, disk/floppy and serial/parallel each have one
source-first disposition; no family is silently substituted or omitted. See
the [family audit](etc/evidence/m0-t310-s4-original-machine-family-audit-001.md).

**T310 S4 P2:** `softpc.new/base/bios` and `base/keymouse` were corrected from
firmware-input placement to byte-exact `mvdm-host` executable source packages.
The firmware mirror now contains only immutable `softpc.new/{bios,roms,data}`
inputs. This makes original `reset`/BIOS/controller source selectable without
claiming a runnable backend; see the [reclassification audit](etc/evidence/m0-t310-s4-p2-softpc-executable-source-reclassification-001.md).

**T310 S4 P3:** the original reset/BIOS/CMOS/keymouse/CCPU-SAS initialization
group is now selected directly from its original `sources` manifests and
archives successfully under formal MSVC `/MT` x86 and x64 Ninja graphs. The
expanded whole-archive audit exposes 264/263 x86/x64 physical unresolved
forms for source-first owner classification; it is not a runnable-backend
claim. See the [P3 build evidence](etc/evidence/m0-t310-s4-p3-original-init-group-build-001.md).

**T310 S4 P4:** every P3 physical unresolved form now has one explicit
source-first disposition in separate x86/x64 ledgers. The pre-T310
Bochs-only assumptions are not reused: SoftPC machine, BOP, XMS and
host-control forms select their original owners first; only six/five forms are
named public-host boundaries and four preserve the `fmstubs.c` debugbreak
default. See the [P4 disposition evidence](etc/evidence/m0-t310-s4-p4-init-group-interface-disposition-001.md).

**T310 S4 P5:** formal x86/x64 Ninja graphs now directly archive the complete
original SoftPC `base/system` and `base/support` manifests: PIC/PIT/CMOS/DMA,
ROM and IOS routing stay in their original source owners.  Two declaration/
include-only divergences preserve those bodies unchanged; P5's non-runnable
forced-link ledgers reduce to 231/230 forms and identify the next original
host-control/fixed-disk group rather than installing local replacements. See
the [P5 composition evidence](etc/evidence/m0-t310-s4-p5-pic-pit-ios-source-composition-001.md).

**T310 S4 closure:** S4 has closed its bounded source-first scope: all nine
original machine families have an initialization-ordered disposition, the
selected reset/BIOS/CMOS/keymouse/CCPU-SAS group is selected from original
manifests on x86/x64, and every P3 unresolved physical form has an explicit
owner. This is composition and interface closure only; it does not claim an
executing CCPU. M0 T310 S5 owns the first original `i386 + CCPU` bounded
execution interval.

**T310 S5 P1:** the exact original `ccpu386` executor now enters and returns
from bounded `D6 FE` intervals through original `nt_cprgs` selection and both
`nt_cpu::host_start_cpu` and `nt_cpu::host_simulate`, after original SAS, CPU,
FPU and access initialization on formal MSVC/Ninja x86 and x64 graphs. Six registered,
source-preserving x64 corrections keep internal SoftPC native allocation
addresses pointer-sized; they do not create guest/host mapping identities.
The fixture deliberately exits before BIOS/BOP/device dispatch and uses only
unreachable test-only link seams, so it proves the original executor rather
than a runnable SoftPC session. The remaining source-shaped product lifecycle,
typed session stop and V86/MONITOR removal audit remain active S5 work; see
the [execution-seam audit](etc/evidence/m0-t310-s5-ccpu-execution-seam-audit-001.md).

**T310 S5 P2:** the exact 133-file x86/x64 CCPU graph has a physical-path
disposition: it contains no `src.old`, Bochs, kernel-VDM or `monitor.lib`
input. Its five `MONITOR` mentions are original inactive conditional guards;
its 38 `V86` mentions are CCPU guest-emulation semantics, not a MONITOR
product path. The resulting owner classifications prevent either premature
source deletion or silent fallback; see the [profile-path audit]
(etc/evidence/m0-t310-s5-ccpu-profile-path-disposition-001.md).

**T310 S5 P3:** a narrow session-owned execution bridge now observes only the
outer return of unchanged original `host_start_cpu`, recording a typed
`SOFTPC_RETURNED` mechanical result after the selected SoftPC CCPU invocation
returns. The x86/x64 fixture proves selection, binding, typed result, unbind,
and the original recursive return separately; it does not treat a nested BIOS
return as session completion or claim a general CPU budget. See the
[execution-seam audit](etc/evidence/m0-t310-s5-ccpu-execution-seam-audit-001.md).

**T310 S5 P4:** the unexercised original `nt_mem` and configuration CPU-state
paths now have a source-first x64 disposition.  The bounded CCPU proof keeps
its original SAS-backed result, while every path that reconstructs a native
address from an Intel `ULONG` is assigned to the one session-owned
guest-memory mapping manager and a later same-shaped SoftPC physical-page
binding.  No pointer-width cast, Bochs fallback, or second mapping table has
been introduced.  See the [memory-boundary audit]
(etc/evidence/m0-t310-s5-ccpu-x64-memory-boundary-audit-001.md).

**T310 S5 P5:** the obsolete Bochs-backed, locally invented DPMI and XMS
memory providers plus their matching overlays/graphs are removed.  The
original `nt_mem.c` and restored original `xmsblock.c` are the only selected
algorithm bodies; their callers remain explicitly deferred until the single
session mapping-manager/SoftPC physical-page binding is recovered.  No old
fixture is retained as a current production claim.  See the [retirement
record](etc/evidence/m0-t310-s5-nonnative-memory-fallback-retirement-001.md).

**T310 S5 P6:** the bounded original CCPU proof now executes an ordinary
register-to-RAM instruction sequence before its original `D6 FE` return.
Both original host entry forms prove `mov al,5Ah; mov [DS:8000h],al` through
the original CCPU/SAS backing on x86 and x64, followed by the typed outer
return.  This remains deliberately below BIOS/BOP/device composition.  See
the [CPU/SAS/RAM evidence](etc/evidence/m0-t310-s5-ccpu-sas-ram-execution-001.md).

**T310 S5 P7:** the same bounded original CCPU proof now also executes the
original `fpu.c` x87 sequence `fld1; fstp dword ptr [DS:8004h]`.  Both formal
x86 and x64 graphs verify its `1.0f` SAS-RAM result before the original `D6
FE` return; no host floating-point substitute, device, BOP, Bochs, or
`src.old` path is selected.  See the [CPU/SAS/RAM/FPU evidence]
(etc/evidence/m0-t310-s5-ccpu-sas-ram-execution-001.md).

**T310 S5 closure:** the selected original `i386 + CCPU` profile now has
formal x86/x64 evidence for initialized entry through original
`host_start_cpu`, recursive original `host_simulate`, ordinary SAS RAM and
original x87 effects, an original `D6 FE` return, typed outer-session result
and teardown.  The source-path audit proves the graph selects no MONITOR,
kernel-VDM, Bochs or `src.old` implementation.  It does not claim firmware or
device composition; S6 owns physical profile cleanup and S7 owns that machine
work.

**T310 S6 P1 / closure:** the selected CCPU profile physically removes the
unselected NT4 `v86/monitor` and `v86/scaffold` executable packages from
`mvdm-host`, relocates their byte-exact sources to indexed historical evidence,
moves `QUIT.COM` to `mvdm-tools`, and removes `monitor.lib` from the legacy
aggregate product manifest.  Original CCPU guest-V86 semantics and named
session/monitor adapter declarations remain correctly distinct.  Formal x86
and x64 CCPU execution fixtures still pass; see the [S6 evidence]
(etc/evidence/m0-t310-s6-v86-monitor-profile-removal-001.md).

**T310 resequencing note:** the former machine-composition S7 is now S8. Its
already recorded P1/P2 evidence remains valid as pre-S8 source-composition
evidence under its original document/commit identifiers; it does not satisfy
the new S7 physical-address exit criterion.

**T310 pre-S8 P1:** deleted the disabled, project-authored SoftPC patch hook and
timer fallback bodies.  The only retained NTVDMx64 patch behavior is the
registered `fmstubs.c` immediate `__debugbreak()` default on both host
architectures; no mapping-backed, cancellation, zero-result, or timer
replacement remains available for accidental activation.

**T310 pre-S8 P2:** the selected original SoftPC machine candidate now compiles
and archives its original-source graph on x64 and x86: 130 original CCPU
units, BIOS, keymouse, system, support, video, and sixteen selected original
host roots, including `nt_eoi.c` and `nt_timer.c`.  Reached non-MVDM OpenNT
declaration subsets retain their original same-named paths under `opennt-host`;

**T310 S7 P1:** source recovery confirms that `VdmSetPhysRecStructs` is a
called but undefined original interface across all three OpenNT source
editions; the NTVDMx64 same-named body is a TODO no-op and is rejected as a
product implementation. The selected CCPU profile declares but does not
consume the physical-record table described by an older `nt_mem.c` comment.
The resulting exact recovery contract assigns stable surrogate identity to
the existing session guest-memory mapping manager and requires a real,
checked selected-SoftPC physical-page operation before this packet can close.
It explicitly excludes the old Bochs SAS test route. See the
[physical-page audit](etc/evidence/m0-t310-s7-physical-page-source-recovery-audit-001.md).

**T310 S7 P2:** `nt_eoi.c` remains byte-identical MVDM host source. Its reached
same-named original NT declaration subsets (`ntexapi.h`, `ntrtl.h`,
`nturtl.h`, and `ntpsapi.h`) now live under `opennt-host`, while only modern
type/import and behavior binding remains in the Win32 adapter. Focused formal
MSVC `/MT` x86/x64 recompilation of original `nt_eoi.c` passed with original
warnings only; the full selected sixteen-root original SoftPC host archive also
passes on both architectures. The S7 physical-page audit records the placement
and result.
the only new code is registered same-shaped Win32/CRT binding needed to make
the source bodies compile.  This remains static composition evidence, not a
controller, timer, mapping, or runnable-backend claim.  See the
[interface build evidence](etc/evidence/m0-t310-s7-p2-original-machine-host-interface-build-001.md).

**T310 S7 P3:** reached non-MVDM platform declarations are restored below
`opennt-host/public/sdk/inc` under their original names: the selected
`ntexapi.h` subset carries the original EOI include and timer counter/tick
declarations, while `ntpsapi.h` carries `NtAlertThread`.  The only adapter
addition is the x86-safe, same-shaped Cdecl-to-WINAPI start-routine binding
required by the original `nt_timer.c` call text.  Focused formal MSVC `/MT`
x86/x64 timer compilation succeeds with original-source diagnostics only.
This P establishes source placement and static ABI composition; heartbeat,
session cancellation, BDA mapping, timer/PIC delivery and controlled-stop
semantics remain S8 work.

**T310 S7 P4:** placement is now enforced at the original-file boundary:
`nt_eoi.c` and `nt_timer.c` remain byte-identical `mvdm-host` roots, while
their non-MVDM original declaration carriers remain same-named source subsets
under `opennt-host`; only non-composable interface behavior belongs in
`adapter-mvdm-host-out`. After the declaration-root correction, the formal
x86 and x64 bounded CCPU fixtures both executed successfully. Their
`VdmSetPhysRecStructs` fixture seam now fails hard if reached, so it cannot
mask the still-unrecovered external physical-page operation. See the
[physical-page audit](etc/evidence/m0-t310-s7-physical-page-source-recovery-audit-001.md).

**T310 S7 P5:** the formerly undefined historical
`VdmSetPhysRecStructs(ULONG, ULONG, ULONG)` seam is now a named same-shaped
SoftPC adapter binding, not a no-op or Bochs fallback. On both x86 and x64,
the existing session `guest_memory` mapping manager publishes the external
backing identity as a stable 32-bit ID; the original `nt_mem.c` add/remove
order reserves/releases its Intel span while original `c_GetPhyAdd` resolves
only the immediate physical byte access. Fresh formal MSVC `/MT` x86/x64
fixtures prove an unaligned live external page write/read and post-remove
mapping release, followed by the existing original CCPU/SAS/FPU recursive
interval. P4's former hard-fail fixture seam is superseded. This does not
claim WOW DIB, full device composition, Bochs, MONITOR or kernel-VDM support;
see the [physical-page audit](etc/evidence/m0-t310-s7-physical-page-source-recovery-audit-001.md).

**T310 S7 closure:** every reached selected-SoftPC `nt_mem`/`sim32`/
`VdmSetPhysRecStructs`/physical-access crossing now has a source-shaped,
mapping-manager-backed disposition on x86 and x64. The formal fixtures prove
positive live-page access and rejection of zero-length, invalid and stale
bindings. The [lease-caller ledger](etc/operations/m0-t310-s7-guest-memory-lease-caller-ledger.tsv)
records all three direct lease consumers as independent owner contracts; none
duplicates this physical route. S8 may consume but may not redefine the
binding.

**T310 S8 P1:** the original machine candidate no longer defines `i386` as a
product-wide host switch. Separate formal MSVC `/MT` x86 and x64 Ninja
source-selected archives now succeed without it; any future truly x86-only
original unit requires a registered target-local exception. This exposes
rather than suppresses each original non-`i386` branch for semantic-owner
audit. The P1 source order is
`reset -> ROM/CMOS -> disk POST -> host reset/event setup`; the immutable
firmware inputs and the complete original `base/disks/sources` plus
`nt_rez`/fixed-disk/real-floppy host group are recorded in the
[architecture-source validation](etc/evidence/m0-t310-s8-p1-i386-architecture-source-validation-001.md).
The original `host_find_file` call shape is now bound to an immutable,
session-owned firmware root rather than the host system directory; formal
Ninja fixtures verify both `bios1.rom` resolution and the original missing
resource failure direction on x86 and x64. App now supplies that root from
the executable-relative `softpc\\roms` package layout before session
activation; `dos/` and `win16/` remain sibling guest-media roots. It is
composition evidence only, not a runnable-machine claim.

**T310 S8 P1 continuation:** original `InitialiseDosEmulation` no longer
looks for `ntio.sys` in the host system directory. Its original open/size/read/
SAS/entry sequence is retained, with only lookup bound to the app-selected
session DOS-media root. Formal x86/x64 fixtures resolve the byte-exact mirrored
`NTIO.SYS`; the full reset/provider path remains active P1 work.

**T310 S8 P1 conditional correction:** source evidence establishes that
`MIPS_BIT_MASK` distinguishes the original x86 and MIPS VDM contracts. MIPS
is outside this product profile, so `nt_msscs.c` is a registered true subset:
it preserves only the original x86 bit-clearing transition on x86 and x64.
This is not a general override for unreviewed non-`i386` paths.

**T310 S8 P2 P1:** the original `nt_timer.c` heartbeat termination path now
has a same-shaped public Win32 binding. `NtAlertThread(HANDLE)` is provided by
the named Win32 adapter through `QueueUserAPC`; the original alertable wait
recognizes its `STATUS_USER_APC` completion as the historical terminal alert.
The existing cdecl worker-start adapter also carries the creator's
thread-bound session only in its private Win32 context and unbinds it on
worker exit; it preserves the original `host_timer_init` call shape.

**T310 S8 P2 P2:** original `nt_eoi.c` now has focused x86/x64 direct runtime
evidence for `InitializeIcaLock` and `host_ica_lock/unlock` mutual exclusion.
The fixture guards all non-lock delayed-IRQ/WOW forms as unreachable test-only
dependencies; it neither substitutes nor claims the remaining PIC/EOI path.
See the [ICA-lock evidence](etc/evidence/m0-t310-s8-p2-original-ica-lock-binding-001.md).
Focused formal x86/x64 Ninja tests prove invalid-target failure and real
alertable-thread wake-up. This does not claim heartbeat/PIC/RTC/BDA execution;
the original timer and ICA source remains the active P2 owner. See the
[heartbeat-alert binding](etc/evidence/m0-t310-s8-p2-heartbeat-alert-binding-001.md).

**T310 S8 P2 P3:** the exact original non-`MONITOR` `DelayHeartBeat` event
contract in `nt_timer.c` now has direct formal MSVC `/MT` x86/x64 evidence:
the selected original body remains pending through suspend and returns
`STATUS_SUCCESS` only after resume. This establishes the source-selected
simple wait contract, not an architecture-derived `#ifndef i386` shortcut.
BDA backing, PIT/RTC, PIC/ICA delivery, `TimerInit`/teardown and every
MONITOR/X86GFX/WOW path remain unclosed original-owner work. See the
[DelayHeartBeat evidence](etc/evidence/m0-t310-s8-p2-original-delay-heartbeat-001.md).

**T310 S8 P2 P4:** source recovery now proceeds by original owner package,
not fixture-local stub expansion. Selecting the byte-exact original
`ccpu386/vglob.c` carrier directly removes 75 physical VGLOB/video-accessor
unresolved forms from both fresh formal x86 and x64 whole-archive ledgers
(262 -> 187; 263 -> 188). The remaining x64 `IUH`/pointer-width diagnostics
are an explicit original video-state composition boundary, not permission to
replace that owner or claim a runnable video device. See the
[VGLOB selection evidence](etc/evidence/m0-t310-s8-p2-original-vglob-provider-selection-001.md).

**T310 S8 P2 P5:** the formal candidate now selects the complete original
`softpc.new/host/src` package (47 manifest units plus two original
architecture-conditional carriers), instead of growing source selection from
fixtures. The entire x86 candidate and forced-link audit pass; `nt_event.c`
and the original CRT diagnostic carrier `fprt.c` compile through narrow,
source-shaped declaration/build bindings. `fprt.c` still exposes a real x64
`DWORD`-to-pointer stream boundary, recorded for a later adapter family rather
than hidden with a cast. See the [complete host package evidence]
(etc/evidence/m0-t310-s8-p2-complete-original-host-package-selection-001.md).

**T310 S8 P4 P1:** the original `C_VID` configuration from `obj.vdm` is now
selected on both formal x86/x64 graphs, so the complete original `cvidc`
package is compiled rather than silently compiled-out. Its generated function
tables expose a real cross-architecture ABI closure: 78 `C_Video` entries and
692 `evidfunc` entries bind typed slots to generic four-`IUH` `S_*` rule
dispatchers. The source-era mismatch produces 167 x86 and 188 x64 incompatible
function-pointer diagnostics. P4 will recover one source-derived generated
private overlay/table-thunk carrier after a full slot/width ledger; it will not
hand-write wrappers, cast away diagnostics, or use a Bochs display route. See
the [generated-interface audit]
(etc/evidence/m0-t310-s8-p4-cvidc-generated-interface-audit-001.md).

**T310 S8 P4 P2:** the complete original CVIDC table family is now selected
through a source-derived private generated carrier: all 81 `VideoVector` slots
and 692 EVID entries retain original ordering while entering the original
generated rules with a native-width private host-word ABI.  Formal MSVC x86 and
x64 archive `original-softpc-cvidc.lib` without C4113/C4133/C4047 from that
773-entry family.  The same audit has isolated 40 distinct x64 C4113 callback
assignments in five `base/video` original units; they are P4's remaining
source-shaped ABI work, so no video runtime enablement is claimed. See the
[generated-interface audit](etc/evidence/m0-t310-s8-p4-cvidc-generated-interface-audit-001.md).

**T310 S8 P4 pre-rebaseline carrier:** the prior source-derived
`base/video` carrier and its zero-row callback ledger are retained as audit
input only. They demonstrate a possible non-casting treatment of several
table declarations, but they do not close CVIDC/base-video or authorize video
runtime enablement. The whole-tree source-contract audit below must first
classify the complete C-video/CCPU table and generated-source package before
any carrier is accepted. See the [pre-rebaseline callback ledger]
(etc/operations/m0-t310-s8-p4-base-video-callback-interface-ledger.tsv).

**T310 S8 P4 rebaseline:** the owner rejected warning-by-warning closure. P4
now begins with a complete static x86/x64 ABI and warning inventory of the
entire original `mvdm-host` and `mvdm-support` trees: all callback/table
contracts, pointer-width and pointer/integer conversions, and compiler type
diagnostics receive original-owner, selected-profile, address-class and
mirror/overlay/adapter/unavailable disposition. The prior CVIDC/base-video
carrier work remains uncommitted audit input only; no individual warning or
partial carrier is a P4 closure until that inventory produces the grouped
repair plan.

**T310 S8 P4 whole-tree baseline:** both formal x86 and x64 selected SoftPC
candidate graphs emit 40,587 source-diagnostic occurrences, retained in a
source-location/architecture ledger and grouped by original interface cluster.
The source-first inventory contains 1,961 ABI candidates across 1,412 retained
original-source files and 35 original responsibility clusters. This is
evidence of an unreviewed workset, not a clean-build or runtime claim.

**T310 S8 P4 architecture-relevance classification:** every unique diagnostic
source location is classified in
`m0-t310-s8-p4-whole-tree-warning-x86-x64-classification.tsv`. The 1,643
records that can affect x86/x64 value width, pointer representation, call ABI,
old-style declaration/result flow, or an x64-only unused formal comprise the
separate compatibility worklist. Warnings that are source-quality-only remain
visible but do not authorize mirror changes or new adapters in this S.

**T310 S8 P4 source reading:** the first complete-contract reading separates
CCPU access-table selection, CCPU executor dispatch, SAS handler tables and
the generated C-video package. It proves that mapping is limited to reached
guest/physical access and cannot replace private SoftPC callbacks or CCPU
dispatch. Its next tranches classify CCPU GDP as private native-width generated
state, memory/EMS/UMB external page binding as the only mapping-manager edge,
timer/ICA as native controller state plus bounded BDA access, and host video
as native DIB state plus bounded regeneration-memory access. The generated
CVIDC body is itself the retained original source identity, so its diagnostic
families need a package-level transform/overlay decision rather than hand
edits. See the [source-contract reading]
(etc/evidence/m0-t310-s8-p4-selected-softpc-contract-reading-001.md).

**T310 S8 P4 CCPU selection-table closure:** all 62 active
`CPU_30_STYLE + CCPU` assignments in `load_sw_cpu_access_functions` now carry
their actual source return and parameter contracts. This repairs the complete
flag/segment return group and the complete register/flag setter group together;
the original `c_reg.c` algorithms, callback order and alternate unselected
engine declarations remain intact. Formal MSVC `/MT /W4` recompilation of
`nt_cprgs.c` passes on x86 and x64 without C4057, and the selection-aware
ledger has no remaining return, parameter or unprototyped-slot mismatch. It is
not a claim that the broader CCPU indirect-call family is finished; that audit
continues under P4.

**T310 S8 P4 configuration guest-profile conditional closure:** original
`config.c` no longer uses the host compiler's `i386` marker to select between
the x86 guest memory calculation and the historical RISC VDM branch.  The
selected x86 calculation is now shared by both supported host targets; the
small source-subset exception is registered as `MVDM-HOST-DIV-056`.  Fresh
formal x86/x64 Ninja builds passed.  See the [conditional closure evidence]
(etc/evidence/m0-t310-s8-p4-configuration-guest-profile-conditional-closure-001.md).

**T310 S8 P4 system-controller callback reading:** the selected CMOS/RTC,
PIC and quick-event subcluster preserves original typed callback contracts on
both hosts.  The unselected `cmos.c` and `quick_ev.c` variants remain untouched
audit inputs; no mapping-manager seam or adapter was invented for host-private
controller callbacks.  See the [subcluster audit]
(etc/evidence/m0-t310-s8-p4-system-controller-callback-subcluster-audit-001.md).

**T310 S8 P4 common `host_simulate` call-ABI closure:** original non-MONITOR
controller sources now see the existing `void host_simulate(void)` form through
the common host declaration carrier, rather than relying on an implicit `int`
declaration through `cpu4.h`.  Fresh formal x86/x64 builds contain no
`host_simulate` diagnostic.  See the [call-ABI closure]
(etc/evidence/m0-t310-s8-p4-host-simulate-call-abi-closure-001.md).

**T310 S8 P4 mouse range callback closure:** the original INT 33h range
notifier now has the same `MOUSE_SCALAR *` endpoint contract at caller and
host definition.  Formal x86/x64 rebuilds have no former C4057 range-call
diagnostics.  See the [range callback evidence]
(etc/evidence/m0-t310-s8-p4-mouse-range-callback-abi-closure-001.md).

**T310 S8 P4 keyboard-vector backing closure:** `kb_setup_vectors` now keeps
NTIO table addresses as guest physical numbers until it combines them with the
existing native SoftPC SAS backing pointer; it no longer round-trips table host
pointers through 32-bit `sys_addr`.  Formal x86/x64 builds removed the reached
pointer-width diagnostics.  This is not a claim that full BOP-5F device
composition is closed.  See the [keyboard backing evidence]
(etc/evidence/m0-t310-s8-p4-keyboard-vector-native-backing-abi-closure-001.md).

**T310 S8 P4 whole-tree ABI baseline refresh:** the complete `mvdm-host` and
`mvdm-support` source scope has a current static census (1,412 files, 2,019
candidates and 33 owner clusters) plus paired formal x86/x64 compiler
cross-check.  All diagnostics remain visible; only width/call-contract
candidates are repair work, and each still requires source-level contract
reading.  See the [current ABI baseline]
(etc/evidence/m0-t310-s8-p4-whole-tree-current-abi-baseline-001.md).

**T310 S8 P4 SAS vector null-slot closure:** the original CCPU SAS table keeps
its source-selected unavailable `Sas_overwrite_memory` slot, but now spells the
null entry with that generated two-argument function-pointer type instead of a
generic K&R callback. No memory provider or adapter was added. See the [SAS
vector evidence](etc/evidence/m0-t310-s8-p4-sas-vector-null-slot-abi-closure-001.md).

**T310 S8 P4 EMM native-width storage closure:** the original EMS allocation
and mapping algorithm now keeps its private host storage IDs as `IHP`, rather
than truncating backing pointers through NT4 `long`. It deliberately does not
use the session mapping manager. See the [EMM storage evidence]
(etc/evidence/m0-t310-s8-p4-emm-storage-id-native-width-closure-001.md).

**T310 S8 P4 CVIDC native callback closure:** the selected original C-video
profile now retains its true callback and generated-global types on both host
architectures, without function/data-pointer coercion. This is compile ABI
closure only, not runtime display validation. See the [CVIDC callback
evidence](etc/evidence/m0-t310-s8-p4-cvidc-native-callback-contract-closure-002.md).

**T310 S8 P4 VDD/UMB numeric-address closure:** the selected original
`nt_msscs.c` now preserves the VDD `PVOID` interface as a numeric guest
physical-address carrier rather than narrowing host pointers.  Its kernel-VDM
fault-base subtraction is removed in favor of the selected user-mode Intel
linear-address contract.  Formal x86/x64 host-root archives have no relevant
width/call-contract diagnostics.  This is not VDD runtime completion; see the
[VDD/UMB evidence](etc/evidence/m0-t310-s8-p4-vdd-umb-numeric-address-abi-closure-001.md).

**T310 S8 P4 copy-engine native-address closure:** the original four-byte
SoftPC fill engine now separates its unchanged 32-bit pattern from private
native pointer alignment and locally derived span arithmetic.  Formal x86/x64
object builds have no relevant width/call-contract diagnostics; no adapter or
mapping identity was added.  See the [copy-engine evidence]
(etc/evidence/m0-t310-s8-p4-copy-fnc-native-alignment-closure-001.md).

**T310 S8 P4 error-dialog native callback closure:** the selected original
`nt_error.c` direct-access suppression and user-dialog paths now retain their
original category/worker ordering while using native-width TLS, `HWND`, dialog
data and public Win32 callback contracts. Formal x86/x64 object builds clear
the selected user-mode width and callback diagnostics. `WOWpSysErrorBox` still
depends on kernel/CSRSS `NtRaiseHardError`; it is recorded as a physical
non-runtime boundary rather than emulated in the mirror. See the
[error-dialog evidence](etc/evidence/m0-t310-s8-p4-error-dialog-native-callback-closure-001.md).

**T310 S8 P4 storage-controller call-contract audit:** clean formal x86/x64
rebuilds prove that the existing original `host_def.h` carrier supplies the
selected `void host_simulate(void)` contract to both floppy-controller
sources.  The two historic undefined-call rows are stale; the remaining 36
K&R default-`int` diagnostics are retained visibly because they are not
host-width ABI faults.  See the [storage-controller evidence]
(etc/evidence/m0-t310-s8-p4-storage-controller-call-contract-audit-001.md).

**T310 S8 P4 CCPU host environment ABI closure:** the original SoftPC
\`host_getenv\` alias now has its standard native pointer declaration in the
selected portability header. This preserves \`c_bsic.c\`'s original
environment predicate while removing its x64 implicit-\`int\` truncation. See
the [CCPU environment evidence]
(etc/evidence/m0-t310-s8-p4-ccpu-host-getenv-abi-closure-001.md).

**T310 S8 P4 CCPU EFLAGS contract closure:** the original CCPU internal
header now declares its own fixed-width \`c_getEFLAGS\` accessor, so interrupt
and PUSHF callers retain the original \`IU32\` EFLAGS result on both host
architectures. See the [CCPU EFLAGS evidence]
(etc/evidence/m0-t310-s8-p4-ccpu-eflags-contract-closure-001.md).

**T310 S8 P4 CCPU generated video-rule storage closure:** the unchanged
original `vglob.c`/`egacpu.h` `IUH*` accessor ABI is restored.  The only
required x86/x64 change is six generated private rule-storage fields, recorded
as `MVDM-HOST-DIV-051`; formal x86/x64 rebuilds clear all twelve associated
`C4057` diagnostics.  See the [video-rule storage evidence]
(etc/evidence/m0-t310-s8-p4-ccpu-video-rule-storage-closure-001.md).

**T310 S8 P4 complete CVIDC generated-table audit:** clean formal x86/x64
rebuilds archive all 41 selected CVIDC objects.  The existing typed-table
overlay closes all 16 actual function/data-pointer conversion records, while
historical generated-source style warnings remain visible and are explicitly
outside the x86/x64-only repair scope.  See the [CVIDC table evidence]
(etc/evidence/m0-t310-s8-p4-cvidc-generated-table-abi-audit-001.md).

**T310 S8 P4 selected C-video direct-route closure:** the selected
`CPU_40_STYLE` profile now excludes only the original uninstalled
`WRT_POINTERS` string paths that carry native callback pointers through
`ULONG`; it retains direct BIOS byte/word callbacks and all selected EVID
logic.  Its remaining private word-buffer alignment operation uses the
original pointer-width `IHPE` carrier.  Focused formal x86 and x64 Ninja
object builds complete without `C4311`/`C4312`; unrelated source-quality
warnings remain visible.  See the [direct-route evidence]
(etc/evidence/m0-t310-s8-p4-cvidc-direct-pointer-profile-closure-001.md).

**T310 S8 P4 original host-fill call closure:** `copy_fnc.c` retains its
original `RtlFillMemoryUlong` buffer-fill order but now publishes the exact
historical `NTAPI` declaration that the selected modern header subset omits.
Formal x86/x64 Ninja rebuilds clear the former implicit-call record without
changing guest addressing, provider selection or warning policy; see the
[call-contract evidence]
(etc/evidence/m0-t310-s8-p4-rtl-fill-memory-ulong-call-contract-closure-001.md).

**T310 S8 P4 original CCPU declaration closure:** selected CCPU FPU,
addressing, interrupt and production-debug source now imports its own existing
original declarations instead of relying on implicit calls. Formal x86/x64
Ninja object builds clear the recovered `d_mem`, `limit_check`, `FLDENV`,
`check_D`, `printf`, `INTx`, and `do_intrupt` records without changing CPU,
FPU, interrupt or debug behavior; see the [closure evidence]
(etc/evidence/m0-t310-s8-p4-ccpu-original-call-declaration-closure-001.md).

**T310 S8 P4 console-menu declaration closure:** original SoftPC mouse and
host-control code now sees the exact `ConsoleMenuControl(HANDLE, UINT, UINT)
-> HMENU` source contract on x86 and x64, eliminating its invalid implicit
`int` return ABI. The original Console Server command-range/event route is not
fabricated through a system-menu shim; its runtime provider remains an explicit
console-control boundary. See the [call-contract evidence]
(etc/evidence/m0-t310-s8-p4-console-menu-call-contract-closure-001.md).

**T310 S8 P4 console-VDM registration declaration closure:** original SoftPC
console initialization and fullscreen-resume callers now see the complete
pointer-bearing `RegisterConsoleVDM` contract on x86 and x64, eliminating the
implicit-call ABI while retaining the original private Console Server/Win32k
provider as an explicit runtime boundary. See the [call-contract evidence]
(etc/evidence/m0-t310-s8-p4-console-vdm-registration-call-contract-closure-001.md).

**T310 S8 P4 console-VDM registration declaration closure:** original SoftPC
console initialization and fullscreen-resume callers now see the complete
pointer-bearing `RegisterConsoleVDM` contract on x86 and x64, eliminating the
implicit-call ABI while retaining the original private Console Server/Win32k
provider as an explicit runtime boundary. See the [call-contract evidence]
(etc/evidence/m0-t310-s8-p4-console-vdm-registration-call-contract-closure-001.md).

**T310 S8 P4 console-menu declaration closure:** original SoftPC mouse and
host-control code now sees the exact `ConsoleMenuControl(HANDLE, UINT, UINT)
-> HMENU` source contract on x86 and x64, eliminating its invalid implicit
`int` return ABI. The original Console Server command-range/event route is not
fabricated through a system-menu shim; its runtime provider remains an explicit
console-control boundary. See the [call-contract evidence]
(etc/evidence/m0-t310-s8-p4-console-menu-call-contract-closure-001.md).

**T310 S8 P4 serial/parallel native transport closure:** original SoftPC COM
and LPT host sources retain their device request order and configuration
semantics while their private pointer-or-scalar ioctl and worker transports
become native-width. Formal x86/x64 object builds clear the selected pointer
truncation and mismatched-width diagnostics without introducing a mapping
manager or a new adapter. See the [serial/parallel evidence]
(etc/evidence/m0-t310-s8-p4-comms-native-transport-closure-001.md).

**T310 S8 P4 fixed-disk private-backing closure:** original `nt_fdisk.c`
retains its lazy aligned scratch-buffer algorithm, but its private host heap
backing is now native-width rather than a truncated `DWORD`. It is not a guest
or external identity, so no mapping-manager route is involved. Forced formal
x86/x64 object builds clear the selected C4311/C4312 diagnostics; unrelated
warnings remain visible. See the [storage backing evidence]
(etc/evidence/m0-t310-s8-p4-storage-private-backing-abi-closure-001.md).

**T310 S8 P4 support x86-conditional closure:** the original `i386` branches
in `mvdm.h` and `suballoc.h` are now correctly recognized as x86-versus-MIPS
guest contracts, not x86-versus-x64 host selectors. The x86 fetch/store and
4-KiB commitment semantics apply to both supported host builds. A forced
formal original DEM build proves the header path on both architectures; no
unrelated warning was hidden. See the [support conditional evidence]
(etc/evidence/m0-t310-s8-p4-support-x86-conditional-contract-closure-001.md).

**T310 S8 P4 host-CRT buffer closure:** original `fprt.c` no longer round-trips
its private CRT buffer through `DWORD`; its source-bounded local length is
explicitly converted to the original signed count. The same original source
builds on x86 and x64 without the selected ABI diagnostics. See the [fprt
evidence](etc/evidence/m0-t310-s8-p4-fprt-native-buffer-abi-closure-001.md).

**T310 S8 P4 formal whole-tree delta:** fresh formal Ninja x86/x64 runs both
complete for the scoped `mvdm-host` and `mvdm-support` roots. Their 126
x64-only excess diagnostics are now evidenced and grouped by original
COMMAND, DEM and XMS owner packages; no diagnostic suppression was added. See
the [formal architecture delta](etc/evidence/m0-t310-s8-p4-whole-tree-formal-architecture-delta-001.md).

**T310 S8 P4 COMMAND RTL environment binding closure:** original `cmdenv.c`
and `cmdexec.c` now compile against the selected `ntrtl.h` declarations and
the same-shaped, host-private RTL environment adapter. x86/x64 object and
forced-link checks clear the former implicit-declaration faults without
altering a COMMAND mirror body. See the [RTL environment evidence]
(etc/evidence/m0-t310-s8-p4-command-rtl-environment-binding-closure-001.md).

**T310 S8 P4 VGlob generated-state closure:** the 152 historical x64
pointer-width rows for original `ccpu386/vglob.c` are verified stale: the
existing generated GDP-slot overlay already supplies native-width private
`GLOBAL_VGAGlobals` storage. Forced formal x86/x64 rebuilds now emit neither
`C4311` nor `C4312`; no mirror body or adapter was added. See the [VGlob
evidence](etc/evidence/m0-t310-s8-p4-vglob-generated-slot-closure-001.md).

**T310 S8 P4 `nt_cpu` generated-GDP debug closure:** the invalid historical
non-`PROD` packed-GDP assertion is removed while the selected source-named
fields retain their original control purpose. Formal and non-product x86/x64
builds clear all 81 historical width/call-contract rows without a mapping
identity or warning suppression. See the [`nt_cpu` evidence]
(etc/evidence/m0-t310-s8-p4-nt-cpu-generated-gdp-debug-closure-001.md).

**T310 S8 P4 selected CCPU access-table closure:** existing source-shaped
generated declarations and selected table slots are freshly verified in both
formal architectures; all four historical `C4057` rows are stale, with no
source change or suppression. See the [access-table evidence]
(etc/evidence/m0-t310-s8-p4-selected-ccpu-access-table-closure-001.md).

**T310 S8 P4 video original-call declaration closure:** the full selected
base/video owner group retains its original functions, tables and algorithms
while nine implicit same-package calls now carry their exact source contracts.
Forced formal x86/x64 builds clear all nine `C4013` rows; unrelated warnings
remain visible. See the [video declaration evidence]
(etc/evidence/m0-t310-s8-p4-video-original-call-declaration-closure-001.md).

**T310 S8 P4 serial/parallel native transport closure:** original SoftPC COM
and LPT host sources retain their device request order and configuration
semantics while their private pointer-or-scalar ioctl and worker transports
become native-width. Formal x86/x64 object builds clear the selected pointer
truncation and mismatched-width diagnostics without introducing a mapping
manager or a new adapter. See the [serial/parallel evidence]
(etc/evidence/m0-t310-s8-p4-comms-native-transport-closure-001.md).

**T310 S8 P4 error-dialog native callback closure:** the selected original
`nt_error.c` direct-access suppression and user-dialog paths now retain their
original category/worker ordering while using native-width TLS, `HWND`, dialog
data and public Win32 callback contracts. Formal x86/x64 object builds clear
the selected user-mode width and callback diagnostics. `WOWpSysErrorBox` still
depends on kernel/CSRSS `NtRaiseHardError`; it is recorded as a physical
non-runtime boundary rather than emulated in the mirror. See the
[error-dialog evidence](etc/evidence/m0-t310-s8-p4-error-dialog-native-callback-closure-001.md).

**T310 S8 P4 EMS page-alias closure:** original `nt_emm.c` retains its exact
`VdmMapDosMemory` / `VdmUnmapDosMemory` page-number calls while the selected
adapter recovers their NT4 kernel-VDM effect as a session-owned reversible
guest-physical alias. The existing mapping manager remains exclusively for
external host-backed spans; no host pointer crosses this EMS boundary. Formal
MSVC `/MT` x86/x64 compilation verifies the mirror callers and adapter without
new width diagnostics. See the [EMS page-alias evidence]
(etc/evidence/m0-t310-s8-p4-ems-page-alias-contract-closure-001.md).

**T310 S8 P4 EMM storage contract closure:** the complete original
`emm_mngr.c` now represents only its private host-dependent storage IDs with
native `IHP`; its independent backfill count remains fixed-width `ULONG`.
Formal x86/x64 compilation clears the affected pointer/allocator ABI forms
without an adapter, mapping manager, or EMM algorithm rewrite. See the [EMM
storage evidence](etc/evidence/m0-t310-s8-p4-emm-storage-contract-closure-001.md).

**T309 S1 P1:** the original `MS_bop_0`/`MS_bop_4` boundary is now a private
source-shaped `mvdm-host` subset, with only the unsafe raw process-pointer
read replaced by existing checked SAS access. Formal x86/x64 fixtures prove
the original service → dispatcher → IP sequence and idle branch; the original
dispatcher package integration is the remaining active S1 work. See the
[ingress audit](etc/evidence/m0-t309-s1-dem-command-ingress-closure-audit-001.md).

**T309 S1 closure:** the unchanged original DEM and COMMAND dispatcher tables
compile under formal current-root MSVC/Ninja x86/x64 graphs. Their only
remaining boundary is whole-provider-package linking: `apfnSVC[]` and
`apfnSVCCmd[]` cannot be satisfied by replacement entries or one-off stubs.
The ingress audit records the exact package-owned continuation.

**T309 S2 P1:** all 16 original DEM and 11 original COMMAND provider source
files now compile and archive on formal x86/x64 Ninja graphs. The same graphs
also compile the selected existing source-shaped Base VDM/SoftPC/monitor/Win32/
Redirector/**session** binding cohort; the x64 forced-link audit drops from 212
to 208 unique external forms and now exposes the lower `adapter-bochs`
component boundary plus later owner families. See the [cohort audit]
(etc/evidence/m0-t309-s2-provider-cohort-build-audit-001.md).

**T309 S2 P2:** the six existing `adapter-bochs` mechanical sources now also
compile/archive in the same formal x86/x64 cohort graphs. Their forced audit
links generate the deliberate non-runnable DLLs and isolate a stable 49-form
`bochs-core` lower boundary plus 25 MSVC C++ runtime forms on both
architectures; no provider or selector semantics entered that adapter.

**T309 S2 P3 admission:** classify each residual by physical caller and
original body before selecting any adapter. Initial inspection already proves
that several residuals have original `mvdm-host` definitions in `nt_rflop`,
`nt_msscs`, `nt_error`, `nt_event`, `nt_lpt`, `nt_pif`, and `nt_hosts`; they
are source-selection candidates, not permission for replacement shims.
The first physical pass records 133 provider-origin residuals (90 DEM, 43
COMMAND), separately from the fixed 49-form `bochs-core` and 25-form toolchain
boundaries; see the [P3 first pass]
(etc/evidence/m0-t309-s2-p3-residual-owner-first-pass-001.md).
Its generated ledger now assigns every provider-origin row a source/adapter/
boundary disposition and is cross-checked against the equal-sized x86 caller
set; no new provider body has been enabled.  **T309 S2 P3 PIF audit:** the
unchanged original `nt_pif.c` now compiles on both architectures with its
historical `WINNT` declaration condition and has an eleven-form finite
immediate boundary.  Its reset neighbor is explicitly transferred intact to
the queued selectable original-SoftPC backend; T309 may not crop it to force a
Bochs-only result.  See the [PIF audit]
(etc/evidence/m0-t309-s2-p3-pif-source-composition-audit-001.md).
The original `nt_event.c`/`nt_hosts.c`/`nt_lpt.c` group is now separately
classified as Console/standard-stream/LPT lifecycle, not a collection of
COMMAND shims: its CPU/BIOS, private-console and direct-port branches remain
named later owners.  See the [event/host/LPT audit]
(etc/evidence/m0-t309-s2-p3-event-host-lpt-source-boundary-audit-001.md).
The original `nt_msscs.c` is a complete host-composition/VDD root (startup,
NTIO load, UMB/VDD hook and PDB lifecycle), so its two residual forms cannot
be detached into current provider shims; it transfers intact to the queued
selectable SoftPC/Bochs host-composition task.  See the [MSSCS audit]
(etc/evidence/m0-t309-s2-p3-msscs-source-boundary-audit-001.md).

**T307 S1 P1:** the local Base VDM facade now covers the reached DOS forms for
first-VDM query, command capture/capacity/reentry, copied one-shot current
directories and session-only `ExitVDM`. Its formal Ninja fixture passes under
MSVC `/MT` on x86 and x64. The unavailable CSRSS, WOW, PIF, cross-process and
second-SoftPC branches remain explicitly outside this local contract; see the
[P1 evidence](etc/evidence/m0-t307-s1-p1-base-vdm-dos-contract-001.md).

**T307 closure:** the one-session DOS Base VDM public family now has one
source-shaped owner in `adapter-mvdm-host-out/basesrv`, with no parallel app
command route. Its CSRSS, WOW, PIF, cross-process and second-executor forms
are explicit later owners rather than local fake successes. See the [closure]
(history/m0-t307-closure-20260828.md).

**T307 S1 P1:** the local Base VDM facade now covers the reached DOS forms for
first-VDM query, command capture/capacity/reentry, copied one-shot current
directories and session-only `ExitVDM`. Its formal Ninja fixture passes under
MSVC `/MT` on x86 and x64. The unavailable CSRSS, WOW, PIF, cross-process and
second-SoftPC branches remain explicitly outside this local contract; see the
[P1 evidence](etc/evidence/m0-t307-s1-p1-base-vdm-dos-contract-001.md).

M0 T301/S1 is closed. Its constrained first-degree audit produced physical
source/binding evidence without turning whole-program BFS into a functional
recovery blocker. The [closure record](history/m0-t301-closure-20260828.md)
retains the exact counts, limits and deferred degree-two seeds.

**T302 S1 closure:** exact source and direct-boundary audit proves that the
client request/capture/size/retry algorithm is original code blocked by CSR
transport, while the corresponding server record/copy algorithm is original
code blocked by BaseSrv/CSRSS product state. The local callback seam is marked
for migration or deletion; no existing mirror diff is retained. See the
[boundary audit](etc/evidence/m0-t302-s1-base-vdm-protocol-boundary-audit-001.md).

**T302 S2 admission:** the owner has directed single-person dual-role recovery
from the queue's first functional package, Base VDM command protocol, through
DPMI/DPMI32. This implementation S recovers only the local one-session DOS
Base VDM protocol through the original public `VDMINFO` contract plus named
source-shaped adapter mechanics; it does not admit CSR/CSRSS reconstruction,
a new BOP service or a second machine executor. See the [T302 S2 plan](etc/operations/m0-t302-s2-base-vdm-local-protocol-recovery-plan-001.md).

**T302 S2 P1:** the temporary app/monitor command path has been migrated into
the named `adapter-mvdm-host-out/basesrv` family. Its source-shaped local DOS
record passes focused formal Ninja x86/x64 tests; direct BaseClient CSR capture
and BaseSrv's CSRSS product shell remain explicit S2 limits. See the
[P1 migration evidence](etc/evidence/m0-t302-s2-p1-local-basesrv-record-migration-001.md).

**T302 S2 P2:** the public BaseClient entry now owns only a caller-local
`VDMINFO` request/result facade before entering the named BaseSrv local record
dispatcher. A direct `vdm.c` composition probe stops at the private Base
DLL/CSR header chain, while the selected facade passes the same x86/x64 formal
fixture. See the [P2 evidence](etc/evidence/m0-t302-s2-p2-baseclient-local-facade-001.md).

**T302 closure:** the admitted local one-session Base VDM protocol is closed;
its retained CSR/CSRSS, first-VDM, cross-process, WOW/PIF and child-lifecycle
branches are explicit future owners, not missing local behavior. See the
[closure record](history/m0-t302-closure-20260828.md).

**T303 S1 admission:** owner-directed queue execution now enters the bounded
Bochs/SoftPC mechanical-contract audit required by the first vertical slice.
It may identify only original machine-facing forms and their existing adapter
seams; no Bochs edit, BOP route or guest trace repair is admitted. See the
[S1 plan](etc/operations/m0-t303-s1-bochs-softpc-mechanical-contract-plan-001.md).

**T303 S1 closure:** the first vertical slice reaches scalar register access,
bounded real-mode locations, the DEM SFT/JFT shadow, and finite CPU resume;
it does not require the historical SoftPC executor, XMS A20/SAS or DPMI
protected mechanics. The [mechanical ledger](etc/operations/m0-t303-s1-first-slice-mechanical-ledger.tsv)
and [audit evidence](etc/evidence/m0-t303-s1-first-slice-mechanical-contract-audit-001.md)
give each form one owner and x86/x64 rule.

**T303 S2 admission:** recover only the missing selector-blind typed finite
resume/stop contract needed by the retained `host_simulate` spelling. It may
make no BOP or MVDM service decision and must leave all source-site pointer
conversion to the DEM/COMMAND owner package. See the [S2 plan](etc/operations/m0-t303-s2-typed-resume-contract-plan-001.md).

**T303 closure:** the historical no-argument `host_simulate` form now enters a
versioned selector-blind finite-resume request and records only its copied
mechanical outcome in the bound session.  The focused formal Ninja fixture
passes on x86 and x64; it proves invalid-input rejection, scalar register
access, bounded guest-memory lease access, and a budget stop without importing
a SoftPC executor or BOP semantics.  See the [closure evidence](etc/evidence/m0-t303-s2-typed-resume-contract-closure-001.md).

**T304 S1 admission:** recover the source-shaped, fixed-width machine-event
ingress contract required before the DEM/COMMAND vertical slice.  The first S
is limited to the reached original event forms, their exact owner/binding
locations, and an implementation-ready selector-blind boundary plan; it does
not admit BOP routing, service behavior, device enablement, or guest-trace
repair. See the [S1 plan](etc/operations/m0-t304-s1-machine-event-ingress-plan-001.md).

**T304 S1 closure:** the audit separates original kernel-VDM `VDMEVENTINFO`
from the Bochs #UD machine-fact record.  It records the seven original event
owners and admits only one mechanical copied-event ABI for S2; BOP decoding,
EIP policy and all other event families remain excluded. See the
[S1 audit](etc/evidence/m0-t304-s1-machine-event-ingress-audit-001.md).

**T304 S2 admission:** consolidate the duplicate private/public #UD layouts
into one versioned fixed-width `adapter-mvdm-host-in` machine-event ABI,
verified on x86/x64.  It remains selector- and service-blind. See the
[S2 plan](etc/operations/m0-t304-s2-fixed-width-machine-event-plan-001.md).

**T304 closure:** the public copied machine-event ABI is now fixed-width on
x86/x64, while the Bochs-private record stays private behind an opaque bounded
copy in `adapter-bochs`. No BOP/provider or `VDMEVENTINFO` semantics entered
the machine boundary. See the [S2 closure evidence](etc/evidence/m0-t304-s2-fixed-width-machine-event-closure-001.md).

**T305 S1 admission:** the owner-directed queue now enters the DEM/COMMAND
vertical slice. This S maps and attempts the smallest original-source closure
through the already-closed Base VDM, SoftPC-shaped and copied machine-event
boundaries; it may not replace OpenNT dispatchers with app-owned behavior.
See the [S1 plan](etc/operations/m0-t305-s1-dem-command-vertical-slice-plan-001.md).

**T305 S1 P1:** both original dispatcher translation units now compile on
x86/x64; the original aggregate `nt_bop.c` reaches a historical PIC/second-
executor boundary before link composition.  The exact source owner map and
the reason this packet must not handwrite a selector route are recorded in the
[boundary audit](etc/evidence/m0-t305-s1-dispatcher-boundary-audit-001.md).

**T305 closure:** the narrow DEM/COMMAND source boundary is closed without
adopting a second SoftPC/PIC executor.  The [closure record]
(history/m0-t305-closure-20260828.md) retains the verified source result and
its explicit non-runtime limit.

**T306 S1 admission:** the owner-directed queue now enters finite MVDM
external-boundary package selection. It consumes the existing `zero-ledger1`
and `one-ledger1` evidence to decide original package imports, named adapter
families and hard boundaries; it does not implement a BOP/provider or restart
unbounded program-wide BFS. See the [S1 plan]
(etc/operations/m0-t306-s1-external-boundary-package-selection-plan-001.md).

**T306 S1 P1:** the direct frontier is now pinned to 7,367 physical external
calls plus 319 retained internal-binding exceptions. Only 251 candidates have
passed the prior physical declaration/signature gate; they seed package-boundary
inspection rather than provider selection. See the [input-integrity record]
(etc/evidence/m0-t306-s1-frontier-input-integrity-001.md).

**T306 S1 P2:** the 251 physically proven seeds are now partitioned without
name merging: 87 return to MVDM closure, 76 remain the excluded SoftPC machine
boundary, and 88 form bounded non-MVDM/leaf cohorts led by BaseClient and
NetAPI. See the [package partition]
(etc/evidence/m0-t306-s1-signature-seed-package-partition-001.md).

**T306 S1 P3:** the 54 physical BaseClient candidates now have a final
source-slice disposition: 30 are public Win32 leaves, 21 retain named
BaseVDM/CSRSS-shaped adapter or future broker ownership, and 3 belong solely
to the future WOW package. No Kernel32/BaseClient product shell is selected;
the existing local DOS BaseSrv seam remains the bounded source-derived route.
See the [cohort audit]
(etc/evidence/m0-t306-s1-baseclient-cohort-boundary-audit-001.md).

**T306 S1 P4:** the 16 physical NetAPI candidates split into an exact small
`netstatus.c` mirror candidate (11 calls), four current public workstation API
calls requiring only bounded Redirector/Win32 buffer binding, and one retained
downlevel password-protocol hard boundary. No historical Workstation/RPC
product shell is selected. See the [cohort audit]
(etc/evidence/m0-t306-s1-netapi-cohort-boundary-audit-001.md).

**T306 S1 P5:** source-level macro/arity review removed the apparent OLEAUT
cohort: both `ReadByte` rows are in-MVDM debug macros targeting local
`read_byte` functions. The remaining CSR row is a genuine NTDLL/CSRSS
hard-boundary call from a WOW debugger path. See the [correction]
(etc/evidence/m0-t306-s1-ole-csr-cohort-correction-001.md).

**T306 S1 P6:** 87 apparent external seeds have returned to existing selected
MVDM source closure; all 76 historical SoftPC machine/device seeds are now
explicitly retained as a no-second-executor Bochs boundary. See the [cohort
evidence](etc/evidence/m0-t306-s1-mvdm-machine-cohort-rebase-001.md).

**T306 S1 P7:** all 15 remaining signature-proven CRT seeds are the standard
`memcpy` public CRT leaf. No historical CRT source or extra adapter is
selected. See the [cohort audit]
(etc/evidence/m0-t306-s1-crt-cohort-boundary-audit-001.md).

**T306 S1 P8:** all 7,367 physical direct external calls now have a safe
adoption disposition. Only identity-proven seeds can select a package or
adapter; 7,116 non-admissible rows are explicit owner-local binding/source
holds, not guessed dependencies. See the [frontier closure]
(etc/evidence/m0-t306-s1-external-frontier-disposition-closure-001.md).

**T306 closure:** the final disposition ledger, updated queue and P02 recovery
program now select one exact non-MVDM source slice and a bounded owner-package
worklist. No provider, BOP route, historical product shell or second executor
was introduced. See the [closure record](history/m0-t306-closure-20260828.md).

**T301 S1 P21 admission:** the owner has approved a bounded mirror-completeness
exception for `opennt-src-2`: import every missing original-relative MVDM path,
including historical build products, into the existing `mvdm-*` component
roots without overwriting selected files.  The supplement is byte-exact
provenance/product evidence only; it does not alter the two-root BFS source
union, build graph or runtime inputs.  A per-file SHA-256 manifest is required.

M0 T300/S1 is closed. T301’s subsequent source-language correction supersedes
its provisional lexical count: the current selected OpenNT-MVDM zero closure
has 12,110 physical definition identities and 4,265 explicitly retained
variant-family edges. The current one-degree frontier has 12,426 physical
non-MVDM, non-proven-macro direct calls and 1,446 spelling groups; each call
retains its physical caller identity. Both approved MVDM source roots are
used; see the [source-union correction
evidence](etc/evidence/m0-t301-s1-p5-mvdm-source-union-correction-001.md).

**T301 S1 P7:** the 122 declaration-shaped spellings now have a physical
original-MVDM definition-form index from both approved source editions: 64
spellings resolve to 128 bodies, including K&R forms that the lightweight
parser did not recognize. This is candidate evidence only; no call has been
reclassified by spelling. See the [P7 definition index]
(etc/evidence/m0-t301-s1-p7-original-mvdm-definition-index-001.md).

**T301 S1 P8:** P7 is now joined back to all 473 declaration-shaped physical
calls: 236 have one or more original-MVDM body candidates and 237 do not. The
236 rows remain candidates pending include-frontier, linkage and source-variant
proof; none has been silently moved into zero degree. See the [P8 crosswalk]
(etc/evidence/m0-t301-s1-p8-physical-call-definition-crosswalk-001.md).

**T301 S1 P9:** include-frontier, linkage and selected-edition gates now leave
217 physical calls with one compatible original-MVDM body and no multi-body
ambiguity; 256 calls remain without a passing MVDM body. The 217 still need
prototype/conditional-form confirmation before zero-degree reclassification.
See the [P9 binding gate]
(etc/evidence/m0-t301-s1-p9-original-mvdm-binding-gate-001.md).

**T301 S1 P10:** source call/header/definition arity is now confirmed for 160
of P9's 217 unique original-MVDM bindings; 57 remain explicit first-degree
records.  Both caller and definition conditional contexts remain physical
evidence, not a spelling merge. See the [P10 signature gate]
(etc/evidence/m0-t301-s1-p10-original-mvdm-signature-gate-001.md).

**T301 S1 P11:** a complete one-degree sweep now finds 380 unique-gated
original-MVDM candidates.  The 202 source-shape-confirmed calls seed 69 bodies;
direct expansion reaches 17 more bodies and records 343 physical direct edges.
These are rebase inputs, not yet a final zero-degree rewrite. See the [P11
expansion evidence](etc/evidence/m0-t301-s1-p11-full-mvdm-rebase-expansion-001.md).

**T301 S1 P12:** 80 of P11's 120 newly expanded internal edges now pass the
same-translation-unit or original header/definition shape gate; 40 remain
explicit unresolved candidates. The P12 edge ledger is input to the
zero-degree rebase, not a silent merge. See the [P12 edge gate]
(etc/evidence/m0-t301-s1-p12-expanded-internal-edge-gate-001.md).

**T301 S1 P13:** the active static audit input is now the rebased 12,487-row
physical first-degree frontier: 202 confirmed old internal calls are removed,
82 original-MVDM definition identities join zero degree, and 263 new exits or
unconfirmed internal calls enter first degree. See the [P13 frontier rebase]
(etc/evidence/m0-t301-s1-p13-rebased-mvdm-degree-frontier-001.md).

**T301 S1 P14:** every rebased first-degree call is now mapped against a
64,059-record local index of both approved original trees: 6,066 CRT, 1,475
host-boundary, 1,557 other non-MVDM source, 3,055 source-absent, and 334 MVDM
variant/parser-gap candidates. These are evidence categories, not provider
selections. See the [P14 global source map]
(etc/evidence/m0-t301-s1-p14-rebased-global-source-map-001.md).

**T301 S1 zero-degree legality correction:** the historical full ledger stays
immutable; P16/P17 form the selected `zero-ledger1.tsv` view.  Its residual
historical body/variant cases are explicit and are not silently reclassified.
The owner has now directed the bounded one-degree definition-presence pass:
resolve only physical original definition candidates for `one-ledger1` direct
calls, without inspecting their bodies or selecting providers.  See the
[P18 plan](etc/operations/m0-t301-s1-one-ledger1-resolution-plan-001.md).

**T301 S1 P15:** source identity/body legality was proven for its then-current
input, but its root-membership premise was too broad and is superseded by P16.

**T301 S1 P16:** the 12,202-entry historical audit ledger is immutable.
`zero-ledger0.tsv` is its new narrow, true-zero view: 11,949 definitions in
the physical `src/mvdm-host` subtrees only. Guest/tool paths and current
non-host support paths remain tracked in the full record but do not enter
ledger0. First-degree work remains paused. See the [P16 root membership
correction](etc/evidence/m0-t301-s1-p16-zero-root-membership-correction-001.md).

**T301 S1 P17:** `zero-ledger1.tsv` now removes the historical SoftPC second
executor from `zero-ledger0` without deleting its source evidence: 452
source-shaped SoftPC host algorithms remain, while 8,229 CPU/FPU/device,
firmware/tool and alternate-product definitions are explicitly extracted.
The resulting true host-plus-provider closure has 3,720 definitions. See the
[P17 SoftPC cut](etc/evidence/m0-t301-s1-p17-softpc-second-executor-cut-001.md).

**T301 S1 P18:** the new `one-ledger1` physical frontier resolves all 7,367
true external direct calls without callee-body inspection: 4,262 calls have
original C/C++ definition candidates and 3,105 have explicit source absence.
The 11,134 candidate rows retain source path/hash/line identity; no package,
provider or runtime route is selected. See the [P18 definition-resolution
evidence](etc/evidence/m0-t301-s1-p18-one-ledger1-definition-resolution-001.md).

**T301 S1 P19 admission:** P18's spelling-level physical candidates now enter
the bounded original declaration/linkage pass. It may reconstruct caller
include closures and declaration identities, but may not inspect a callee body
or begin second-degree BFS. See the [P19 plan]
(etc/operations/m0-t301-s1-one-ledger1-declaration-binding-plan-001.md).

**T301 S1 P19:** all 11,134 P18 physical candidate rows now have an original
declaration/linkage outcome. Only 310 are unique-source candidates with a
reachable declaration token; 3,942 remain multi-candidate, 6,639 have no
reachable declaration token and 243 are invalid cross-translation-unit static
candidates. No callee body was read. See the [P19 binding evidence]
(etc/evidence/m0-t301-s1-p19-one-ledger1-declaration-binding-001.md).

**T301 S1 P20 admission:** only P19's 310 unique/declaration-reachable rows
enter a bounded original call/declaration/definition signature and conditional
context gate. Passing rows become future body-expansion seeds only; P20 still
does not traverse a candidate body. See the [P20 plan]
(etc/operations/m0-t301-s1-one-ledger1-signature-gate-plan-001.md).

**T301 S1 P20:** 251 of the 310 eligible rows now have physical
call/declaration/definition arity confirmation; 59 remain explicit one-degree
signature/condition-form failures. The 251 are future body-expansion seeds,
not package/provider admissions. See the [P20 signature evidence]
(etc/evidence/m0-t301-s1-p20-one-ledger1-signature-gate-001.md).

## Historical Packet Archive

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T309 S2`, Ordinary Mode with single-person dual-role implementation and review; closed. |
| Admission And Approval | Owner-directed sequential queue execution admits the source-package continuation after the S1 ingress/dispatcher closure. |
| Objective | Compose original DEM and COMMAND provider cohorts through their selected named adapter forms, retaining original tables and provider bodies. |
| Non-goals | No trace-led service patch, provider/table replacement, full `nt_bop.c` aggregate, second SoftPC/PIC executor, kernel VDM/CSRSS, XMS/DPMI/Redirector/WOW/VDD/debugger recovery or app-owned service policy. |
| Reference Baseline | [T309 S1 ingress audit](etc/evidence/m0-t309-s1-dem-command-ingress-closure-audit-001.md), [T307 closure](history/m0-t307-closure-20260828.md) and [T308 closure](history/m0-t308-closure-20260828.md). |
| Files And ABI Surface | Original `mvdm-host/dos/{dem,command}` `sources` cohorts, their exact dispatcher tables, named Base VDM/monitor/SoftPC/Win32/session forms and formal cohort graphs. |
| Applicable Rules | `docs/rules/EXECUTION.md`; source policy; architecture/coding mirror rules; source-first recovery gate; mapping and `src.old` exclusion rules. |
| Verification | Original source/cohort audit; formal Ninja x86/x64 cohort compile/link closure; focused package test only after original route composition; documentation governance and `git diff --check`. |
| Expected Markers | Original sources-file cohort map, one named adapter/owner disposition per unresolved external form, and no fake service, replacement table or second executor. |
| Asset Needs | Selected OpenNT MVDM mirror, T309 S1 closure, closed Base VDM and monitor artifacts, current Bochs-facing mechanical facade. |
| Reporting Requirements | Record original source path, provider cohort membership, preserved call/layout/failure rule, every selected binding and deferred product-shell branch. |
| Stop Conditions | A proposed route requires a provider/table rewrite, full historical PIC executor, kernel/CSRSS reconstruction, an unregistered mirror divergence, an app semantic or trace-selected service. |
| Exit Criteria | Met: every remaining original external form has one exact owner/disposition; no provider was replaced merely to obtain a link. |
| Original Owner Request | “单人双角色模式执行构建NTVDM64的队列任务，从 Base VDM 命令协议开始，到 DPMI / DPMI32 复通。” |
| Similar-Issue Sweep | Both original table arrays, every original `sources` member, and all direct external symbols are classified before any local route test. |

**Td closure — OpenNT-host directory boundary:** `opennt-host` is now
explicitly governed as the single mirror root for every separately accepted
non-MVDM OpenNT host package; Base VDM is its first slice, not its limit. See
the [Td record](history/td-opennt-host-package-root-governance-20260827.md).

## Closed Packet Archive

### M0 T299 S4 closure — physical translation-unit closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T299 S4`, ordinary mode with single-person dual-role implementation and review. |
| Result | 152 physical first-degree definitions form the selected-file closure; all 762 second-degree rows leave that file set. |
| Evidence | [S4 closure](etc/evidence/m0-t299-s4-first-degree-translation-unit-closure-001.md) and [plan](etc/operations/m0-t299-s4-first-degree-translation-unit-closure-plan-001.md). |
| Next Admission | Public Win32/CRT source-shaped binding and facade recovery may consume the frozen per-call and per-definition ledgers. |

### M0 T299 S5 closure — parser language-construct correction

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T299 S5`, ordinary mode with single-person dual-role implementation and review. |
| Result | Control constructs are absent from definition and candidate rows; the selected-file closure invariants still hold. |
| Evidence | [S5 closure](etc/evidence/m0-t299-s5-parser-control-construct-correction-001.md) and [plan](etc/operations/m0-t299-s5-parser-control-construct-correction-plan-001.md). |
| Next Admission | Public Win32/CRT source-shaped binding and facade recovery may consume the corrected T299 ledgers. |

### M0 T299 S6 closure — definition-body binding correction

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T299 S6`, ordinary mode with single-person dual-role implementation and review. |
| Result | The false `GetTickCount` call sites are absent; the regenerated selected-file closure has 112 physical definitions and 670 cross-file candidates. |
| Evidence | [S6 closure](etc/evidence/m0-t299-s6-definition-body-binding-correction-001.md); the [S5 plan](etc/operations/m0-t299-s5-parser-control-construct-correction-plan-001.md) applies with the stricter predicate. |
| Next Admission | Public Win32/CRT source-shaped binding and facade recovery may consume the corrected T299 ledgers. |

### M0 T299 S3 closure — missing-definition normalization

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T299 S3`, Ordinary Mode with single-person dual-role implementation and review. |
| Result | Every raw call is a concrete implementation, public-system provider or individual missing original definition; the 528 second-degree rows have concrete callers. |
| Evidence | [T299 S3 closure](etc/evidence/m0-t299-s3-missing-definition-normalization-closure-001.md) and [S2 plan](etc/operations/m0-t299-s2-call-site-identity-normalization-plan-001.md). |
| Next Admission | A second-degree audit may consume only the call-to-implementation and second-degree initial ledgers; it must preserve per-file implementation identities and per-call missing definitions. |

### M0 T298 closure — MVDM-host zero-degree canonicalization

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T298 S1`, Ordinary Mode with single-person dual-role implementation and review. |
| Result | Canonicalized the immutable T295 11,872-identity source audit without replacing it. |
| Evidence | [T298 closure](etc/evidence/m0-t298-s1-zero-degree-canonicalization-closure-001.md) and [plan](etc/operations/m0-t298-s1-zero-degree-canonicalization-plan-001.md). |
| Next Admission | First-degree source audit may consume only the 801 canonical external interface spellings and the complete raw-call resolution ledger. |

### M0 T297 closure — MVDM guest original-root recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T297 S1`, ordinary mode with single-person dual-role implementation and review. |
| Result | Re-rooted the immutable guest inputs without changing their bytes or runtime/build ownership. |
| Evidence | [T297 closure](etc/evidence/m0-t297-s1-guest-root-recovery-closure-001.md) and [plan](etc/operations/m0-t297-s1-guest-root-recovery-plan-001.md). |
| Next admission | Owner queue direction; no source semantics, host/adapters/Bochs or guest build enablement was added. |

**T291 S4 closure:** command, monitor-control and synchronous callback
execution are now distinct typed planes; the generic session operation table
is gone. The owner-specific `host_simulate` and WOW task limits are recorded,
not papered over. See the [closure](etc/evidence/m0-t291-s4-control-plane-separation-closure-001.md).

**T291 S5 P1 correction:** P1 established the original `VDMINFO` request/result
forms, but its source-supply conclusion was limited incorrectly to the selected
MVDM mirror. The complete approved OpenNT source contains the original BaseSrv
server/client package. No authored queue draft is retained. P2 now audits that
package and its owner boundary before any binding is admitted; see the
[P1 audit](etc/evidence/m0-t291-s5-p1-command-broker-source-abi-audit-001.md)
and [P2 source-package audit](etc/evidence/m0-t291-s5-p2-basesrv-source-package-audit-001.md).

**T291 S5 P3:** the selected exact BaseSrv/client VDM package now lives in
`opennt-host`; its ten file rows and five source-interface rows were added to
the shared MVDM/OpenNT host tracker. It remains unlinked pending the named
CSR/private-host binding audit; see the
[P3 import evidence](etc/evidence/m0-t291-s5-p3-basesrv-source-import-001.md).

**T291 S5 P4 pause:** the BaseSrv interface ledger is retained as source
evidence, but no binding is admitted from it. P5 now expands the review from
one discovered BaseSrv slice to the complete `mvdm-host`/external-OpenNT
package boundary; see the
[P5 package-boundary plan](etc/operations/m0-t291-s5-mvdm-opennt-package-boundary-audit-plan-001.md).

**T291 S5 P5 progress:** the immutable T276 final-disposition result has been
reconciled into the live 1,699-row file tracker: all 1,689 selected MVDM rows
and ten current external OpenNT rows now have a final file disposition. The
Base VDM vertical slice is limited to exact declaration carriers plus selected
client/server true subsets; `srvinit.c` is evidence-only, while CSR/CSRSS and
kernel VDM remain stopping boundaries. Reached interface-child, build/profile
and current-route reversion ledgers remain the active exit work; see the
[T276 reconciliation](etc/evidence/m0-t291-s5-p5-t276-live-ledger-reconciliation-001.md)
and [external file audit](etc/evidence/m0-t291-s5-p5-external-file-disposition-001.md).

**T291 S3 closure:** the bounded pointer scope, numeric task projection and
source-shaped non-fast callback mechanics passed their complete formal x86/x64
fixture matrix. `CallBack16` now bypasses session command routing and uses
only the SoftPC-shaped mechanical interval. It remains a foundation, not a
loaded WOW runtime; see the [S3 closure](etc/evidence/m0-t291-s3-wow-frame-callback-foundation-closure-001.md).

**T291 S4 P1:** `NtVdmControl(VdmQueryDir)` now uses a distinct typed
monitor-control capability rather than the command route, with formal x86/x64
proof. `GetNextVDMCommand`, generic `host_simulate` postconditions and full
WOW task lifecycle remain separate source-first groups; see the
[P1 evidence](etc/evidence/m0-t291-s4-p1-vdm-control-capability-separation-001.md)
and [live control-plane ledger](etc/operations/m0-t291-s4-control-plane-disposition-ledger-001.md).

**T291 S4 P2:** `GetNextVDMCommand` now has its own typed monitor command
provider and no longer crosses a generic `session` operation dispatcher.
The single immediate app source remains deliberately bounded: BaseSrv/CSR
capture, wait/wake, WOWEXEC notification and multi-caller arbitration are
not claimed. Formal x86/x64 proof and the named S5 successor are recorded in
the [S4 plan](etc/operations/m0-t291-s4-control-plane-separation-and-reentry-contract-plan-001.md)
and [live ledger](etc/operations/m0-t291-s4-control-plane-disposition-ledger-001.md).

**T291 S4 P3/P4:** the complete direct-call postcondition ledger now prevents
the shared `host_simulate` spelling from masquerading as device, DPMI, DEM,
firmware or WOW completion. The current WOW TD projection is formally limited
to its one bound thread and callback fields. The successor
[S5 plan](etc/operations/m0-t291-s5-source-shaped-command-broker-and-monitor-reentry-plan-001.md)
is now explicitly queued behind S4 for source-shaped command broker and
monitor re-entry recovery.

**T291 S2 closure:** all twelve selected original WOW32 thunk/frame bodies
have source coverage; 214 reached external-interface occurrences and eighteen
source-token dispositions have one owner, bounded lifetime and recovery or
failure result. Existing adapter families are sufficient in ownership but not
yet in implementation. S3 owns the complete bounded pointer/frame/callback
transaction; no original WOW provider body or route is enabled. See the
[S2 evidence](etc/evidence/m0-t291-s2-wow-thunk-frame-interface-recovery-001.md)
and [decision](etc/operations/m0-t291-s2-wow-thunk-frame-recovery-decision-001.md).

**T290 S1 closure:** [the source/ABI audit](etc/evidence/m0-t290-s1-redirector-source-abi-audit-001.md) proves all fifty original `57:00..31` dispatch entries and all twenty-five selected package paths.  The source mirror is exact in both canonical OpenNT editions; lifecycle/named-pipe, mailslot/async, and NetAPI/NetBIOS/DLC groups are the only admissible next owner groups.  No Redirector selector is enabled.

**T290 S2 closure:** [the interface recovery design](etc/evidence/m0-t290-s2-redirector-interface-closure-design-001.md) records sixteen source-first boundary outcomes.  Public pipes/mailslots/selected NetAPI calls use same-shaped outgoing facades; scoped guest copies and opaque identities stay session-owned; private DLC/LLC and VDD/monitor paths retain explicit original unavailable outcomes until their named owners are admitted.  No Redirector selector is enabled.

**T290 S3 P5:** exact original `vrinit.c` now compiles and executes its
source-defined VDD-unavailable initialization branch on formal x86/x64 Ninja
fixtures.  It does not enter NetBIOS, DLC, guest writes or IRQ delivery, and
`VrUninitialize` retains its carry-clear result.  The byte-identical canonical
`dlcapi.h` declaration carrier is now selected in `mvdm-platform-abi`; see the
[P5 evidence](etc/evidence/m0-t290-s3-p5-vrinit-vdd-failure-001.md).

**T290 S3 P6:** exact original `vrnmpipe.c` now has formal x86/x64 evidence
for named-pipe name classification, open-list lifecycle and synchronous
read/write on a local public Win32 named pipe.  Async, callbacks, physical IRQ
and selector ingress remain excluded; see the [P6 evidence](etc/evidence/m0-t290-s3-p6-sync-named-pipe-composition-001.md).

**T290 S3 P7:** exact original `cmdredir.c` now has formal x86/x64 evidence
for host-local standard-handle classification, notification and cleanup
without creating a second COMMAND broker.  Its legacy AX:BX pointer/HANDLE
marshaller and pipe-worker paths are still selector-disabled pending their
named session mapping/lifecycle closure; see the [P7 evidence](etc/evidence/m0-t290-s3-p7-command-redirection-consumer-001.md).

**T290 S3 P8:** the original `VrGetNamedPipeHandleState` BP:BX path now runs
on the same formal x86/x64 pipe fixture through session-owned opaque host
resource identity resolution.  Native handles remain local and are released
before session disposal; guest-buffer pipe forms remain the next bounded
pointer-scope group.  See the [P8 evidence](etc/evidence/m0-t290-s3-p8-named-pipe-handle-state-001.md).

**T290 S3 P9:** original `VrGetNamedPipeInfo` now composes on the same formal
x86/x64 fixture through the existing session guest-memory pointer scope.  It
retains BP:BX opaque identity, DS:SI bounded bounce-lease and packed
`PIPEINFO` output; see the [P9 evidence](etc/evidence/m0-t290-s3-p9-named-pipe-info-guest-span-001.md).

**T290 S3 P10:** original `VrSetNamedPipeHandleState` now runs on the formal
x86/x64 local-pipe fixture with the same BP:BX opaque identity and source
DOS-to-Win32 mode conversion; see the [P10 evidence](etc/evidence/m0-t290-s3-p10-named-pipe-state-set-001.md).

**T290 S3 P11:** original `VrPeekNamedPipe` now composes through the existing
session DS:SI pointer scope and returns source-shaped counts/status before the
same bytes are consumed by original read; see the [P11 evidence](etc/evidence/m0-t290-s3-p11-named-pipe-peek-guest-span-001.md).

**T290 S3 P12:** original `VrTransactNamedPipe` now composes on formal x86/x64
message-mode public pipes through the same session BP:BX opaque identity and
bounded DS:SI/ES:DI guest spans.  It retains the source event/overlapped wait,
byte-count and carry/error sequence; see the [P12 evidence](etc/evidence/m0-t290-s3-p12-named-pipe-transact-guest-span-001.md).

**T290 S3 P13:** original `VrWaitNamedPipe` now composes on formal x86/x64
through the session DS:DX string lease and source BX:CX timeout. It retains
the source `AX=0`/carry-clear success direction; see the [P13
evidence](etc/evidence/m0-t290-s3-p13-named-pipe-wait-guest-string-001.md).

**T290 S3 P14:** original `VrCallNamedPipe` now composes on formal x86/x64
through exact session leases for its source packed structure and every 16:16
input/output span. The original body and source call/error order remain;
see the [P14 evidence](etc/evidence/m0-t290-s3-p14-named-pipe-call-structured-guest-span-001.md).

**T290 S3 P15:** original `VrNetHandleGetInfo`/`VrNetHandleSetInfo` and
`vrputil.c` now execute on formal x86/x64 local-pipe paths. The former keeps
its original mapped local collection failure; the latter's original
pre-decrement no-op success is retained, not repaired; see the [P15
evidence](etc/evidence/m0-t290-s3-p15-named-pipe-handle-info-local-disposition-001.md).

**T290 S3 P16:** original non-pipe `cmdGetStdHandle` now composes on formal
x86/x64 through session-owned AX:BX record and BX:CX handle identities, with
the original cleanup point retiring the record. The full `cmdmisc`/`cmdexec`
producer-consumer handoff remains assigned to S6; see the [P16
evidence](etc/evidence/m0-t290-s3-p16-command-redirection-handle-identity-001.md).

**T290 S3 closure:** the original lifecycle failure, all synchronous
named-pipe forms and local COMMAND group passed the full formal x86/x64 matrix
with explicit source transfers. S4 is now the sole active packet; see the
[closure record](etc/evidence/m0-t290-s3-lifecycle-sync-pipe-command-closure-001.md).

**T290 S4 P18:** original `vrmslot.c` now composes on formal x86/x64 fixtures
through the existing session host-resource mapping and bounded Redirector
pointer scopes. Make/info/write/read/delete, original peek decline and
process-termination stale-handle cleanup are proven; asynchronous named-pipe
completion remains the next S4 sub-group. See the
[P18 evidence](etc/evidence/m0-t290-s4-p18-original-mailslot-composition-001.md).

**T290 S4 P19:** original `VrReadWriteAsyncNmPipe` now has formal x86/x64
safe-decline evidence: its fixed 16:16 request layout parses under a bounded
scope, then the current same-shaped unavailable worker facade preserves the
source carry/error path without queuing I/O or exposing guest memory. See the
[P19 evidence](etc/evidence/m0-t290-s4-p19-async-pipe-safe-decline-001.md).

**T290 S4 closure:** original mailslot bodies are source-composed with stale
and cleanup proof; the async group retains its original safe decline and
empty-queue cancellation direction while physical IRQ completion remains
explicitly unavailable. See the
[closure record](etc/evidence/m0-t290-s4-mailslot-async-closure-001.md).

**T290 S5 P1:** the remaining provider audit freezes NetAPI/remote/NetBIOS/DLC
owner outcomes and identifies the first real source closure prerequisite:
`vrnetapi.c` needs a provenance-selected historical DS declaration package,
not an arbitrary same-name header. No incomplete fixture is retained. See the
[P1 audit](etc/evidence/m0-t290-s5-remaining-provider-source-abi-audit-001.md).

**T290 S5 P2:** the exact original `vrnetapi.c` now compiles on formal x86/x64
fixtures after selecting its DS/public-internal declaration cohorts. Four
original `ERROR_NOT_SUPPORTED` entries and two original no-op entries execute
with their source register outcomes. The fixture's test-only forced link does
not resolve or enable the sibling private Lanman/SMB bodies; see the
[P2 evidence](etc/evidence/m0-t290-s5-p2-vrnetapi-local-source-closure-001.md).

**T290 S5 P3:** the exact original `VrGetComputerName` body now composes on
formal x86/x64 fixtures through its source `LM20_CNLEN` bound and the existing
session-owned `ES:DI` Redirector pointer scope. It retains either the original
`AX=0` carry-clear copy result or `NERR_BufTooSmall` carry-set result, without
creating a larger project buffer or enabling private Lanman/SMB paths; see the
[P3 evidence](etc/evidence/m0-t290-s5-p3-vrnetapi-computer-name-source-closure-001.md).

**T290 S5 P4:** the exact original `VrGetUserName` body now composes on formal
x86/x64 fixtures for its bounded `BX=1` form through public
`NetWkstaUserGetInfo`, `netapi32.lib` and the existing session-owned `ES:DI`
scope. It retains original success and `NERR_BufTooSmall` results; its
unbounded `BX=0` form remains selector-disabled because no checked guest span
exists. See the [P4 evidence](etc/evidence/m0-t290-s5-p4-vrnetapi-user-name-source-closure-001.md).

**T290 S5 closure:** the [27-entry remaining-provider disposition
matrix](etc/operations/m0-t290-s5-remaining-provider-disposition-matrix.tsv)
now gives every NetAPI/NetBIOS/DLC/window/mode branch one source-first result.
The six focused original-body graphs pass together on formal x86/x64 Ninja
runs. Private Lanman, NetBIOS/DLC driver and physical-IRQ contracts remain
source-preserved and selector-disabled, with named future owners; see the
[closure record](etc/evidence/m0-t290-s5-remaining-provider-disposition-closure-001.md).

**T290 S6/T290 closure:** the [complete 50-entry Redirector family
matrix](etc/operations/m0-t290-s6-redirector-family-disposition-matrix.tsv)
has one original-source disposition per entry and reached brokered helper.
The formal 11-graph x86/x64 matrix passes, and the bounded ingress observation
proves that no `57:xx` route is silently enabled. The only non-exact provider
mirror is the already-registered `vrmslot.c` session-identity divergence. See
the [S6 evidence](etc/evidence/m0-t290-s6-redirector-family-closure-001.md)
and [T290 history](history/m0-t290-closure-20260827.md).

**T291 S1 closure:** the current [222-file WOW/FAX
ledger](etc/operations/m0-t291-s1-wow-file-subdomain-disposition-ledger.tsv),
[15-family interface ledger](etc/operations/m0-t291-s1-wow-interface-family-ledger.tsv)
and [mirror identity ledger](etc/operations/m0-t291-s1-wow-mirror-identity-ledger.tsv)
are reproducibly generated and fully cover the selected package. The only
mirror divergence is registered `MVDM-HOST-DIV-007` in `wow32/wkfileio.c`.
No WOW body or route is enabled; see the [S1 evidence](etc/evidence/m0-t291-s1-wow-owner-package-source-abi-audit-001.md).

**T289 S4 closure:** original `dpmi32.c` initialization, `xmem.c` allocation
lifecycle, `dpmiselr.c` descriptor normalization and `dpmimemr.c` memory-info
bodies now compose on formal x86/x64 Ninja fixtures.  Session-owned numeric
resource identities replace only persisted x86 host-pointer coincidences;
checked copied protected spans replace transient aliases.  The historical
host-LDT final operation retains its original failure direction, and no
`53:xx` ingress or dispatcher is enabled.  See the [S4 evidence](etc/evidence/m0-t289-s4-dpmi-composition-001.md) and [ledger](etc/operations/m0-t289-s4-dpmi-composition-ledger.tsv).

**T289 S5 P1:** original `DpmiSetProtectedmodeInterrupt` and
`DpmiSetFaultHandler` now compose through a source-shaped current-worker
`VDM_TIB` and checked fixed `SS:SP` copies. Formal x86/x64 fixtures prove both
registrations and no-mutation invalid indexing; the entire selected S5 source
group also static-compiles on both architectures. S5 remains active: dispatch,
IRET and stack switching require a prevalidated atomic CS/SS/EIP/ESP/EFLAGS
transition rather than field-by-field setters. See [P1 evidence](etc/evidence/m0-t289-s5-p1-dpmi-registration-001.md)
and [ledger](etc/operations/m0-t289-s5-dpmi-stack-interrupt-disposition-ledger.tsv).

**T289 S5 P2:** `adapter-bochs` now has the required selector-blind
same-privilege protected-frame transaction. Its private Bochs overlay
prevalidates all target descriptors and EIP before altering architectural
state; formal x86/x64 fixtures prove both a valid CS/SS/data/GPR/EIP transfer
and invalid-selector no-mutation rejection. No DPMI source path is enabled by
P2; P3 owns its source-shaped composition. See [P2 evidence](etc/evidence/m0-t289-s5-p2-protected-transition-001.md).

**T289 S5 P3:** original `BeginUseLockedPMStack` and
`EndUseLockedPMStack` now retain their source save/restore ordering and
nesting behavior through a copied, same-CPL SoftPC register transaction.
Formal x86/x64 fixtures prove the `ES:0x1000` switch and exact restoration;
the remaining interrupt/IRET/fault bodies still require bounded stack-frame
composition. See [P3 evidence](etc/evidence/m0-t289-s5-p3-locked-stack-transaction-001.md).

**T289 S5 P4 audit:** the remaining original `stack.c` and `dpmiint.c`
dispatch/IRET/fault bodies require a guest-visible VDM-TIB/DOSX projection,
selector descriptor publication and separate real-mode frame mechanics. They
remain compiled but selector-disabled; the source dependency map prohibits
inventing host pointers, a host LDT or a second executor. See the [P4
dependency audit](etc/evidence/m0-t289-s5-p4-stack-frame-dependency-audit-001.md).

**T289 S5 P5:** `adapter-bochs` now provides the selector-blind copied
real-mode CS/SS/SP/IP/FLAGS transition identified by P4. Formal x86/x64
fixtures prove valid, stale and rejected-change outcomes. Original DPMI
stack/interrupt bodies remain selector-disabled because their bounded
guest-stack/IVT copies and descriptor projection are separate requirements;
see the [P5 evidence](etc/evidence/m0-t289-s5-p5-realmode-frame-foundation-001.md).

**T289 S5 P6:** original `DpmiPushRmInt` and `DpmiSimulateIretCF` now compose
through bounded session IVT/stack leases and the P5 real-mode transaction.
Formal x86/x64 original-body fixtures prove exact BOP/FLAGS frame order,
IVT transfer, CF merge and invalid-vector no-change behavior. DOSX stack
projection and protected interrupt/fault paths remain selector-disabled; see
the [P6 evidence](etc/evidence/m0-t289-s5-p6-realmode-stack-composition-001.md).

**T289 S5 closure:** every independently composable stack/interrupt/fault
mechanic has formal x86/x64 evidence; remaining DOSX projection, descriptor
publication and historical monitor branches retain named source-proven
unavailable dispositions. The complete closure record is
[here](etc/evidence/m0-t289-s5-stack-interrupt-fault-closure-001.md). S6 is
now the sole active packet and owns only the package-level `53:00..18` matrix.

**T289 S6 closure:** the [complete `53:00..18` matrix](etc/operations/m0-t289-s6-dpmi-family-disposition-matrix.tsv)
has formal x86/x64 package-runner evidence or an explicit source-proven
unavailable result per entry. The full record is the [S6 closure](etc/evidence/m0-t289-s6-dpmi-family-matrix-closure-001.md). No BOP ingress is enabled.

**T289 S6 P1:** original x86 `DpmiDpmiInUse`/`DpmiDpmiNoLongerInUse` no-op
bodies and original `DpmiVcdPmSvcCall32` public registry branches now run on
formal x86/x64 fixtures.  The S4 fixture also now proves original
`DpmiFreeAppXmem` and `DpmiFreeAllXmem`, including stale opaque-identity carry
failure.  The live all-25 disposition is in the
[S6 matrix](etc/operations/m0-t289-s6-dpmi-family-disposition-matrix.tsv);
no selector ingress is enabled.

**T289 S7 closure:** both canonical OpenNT DPMI editions are identical for
all 25 selected paths; the current mirror has nineteen exact files and six
in-file/README-registered minimal divergences.  Session-owned host-resource
identity use is the only durable mapping route.  All nine selector-disabled
outcomes have a named queued owner, so T289 has no ownerless DPMI branch.
The complete ledger and reproducible source audit are in the
[S7 evidence](etc/evidence/m0-t289-s7-dpmi-identity-transfer-closure-001.md)
and [transfer ledger](etc/operations/m0-t289-s7-dpmi-identity-exception-transfer-ledger.tsv).

**T289 S3 closure:** `adapter-bochs` now exposes a selector-blind copied
protected-frame, copied active-segment inspection and checked protected-span
contract.  A frame commit is comparison-guarded, only changes GPR/EIP/native
FLAGS while the CPU loop is returned, and rejects CR0/mode/segment changes.
Formal x86 and x64 Ninja fixtures execute a real-to-protected transition and
prove valid commit, stale rejection and no-mutation invalid change rejection;
no DPMI source or `53:xx` route was selected. See [S3 evidence](etc/evidence/m0-t289-s3-protected-machine-foundation-001.md) and [ledger](etc/operations/m0-t289-s3-protected-machine-foundation-ledger.tsv).

**T288 S1 closure:** the reproducible union ledger records 78 selected source
files, all exact in both canonical OpenNT roots. It identifies the seven exact
keymouse files still under the temporary `mvdm-host` root for S2 re-rooting,
partitions 15 monitor/firmware interface groups by original owner and recovery
ladder, and transfers SIM16/WOW plus VDD/debugger-specific behavior without
inventing substitutes. See [S1 evidence](etc/evidence/m0-t288-s1-monitor-source-abi-audit-001.md).

**T288 S4 closure:** the source-shaped SAS scalar/span facade now composes
through checked `adapter-bochs` memory mechanics on formal x86 and x64 Ninja
fixtures. Its complete source disposition records the intentionally excluded
low-host-address mapper, raw-pointer forms, memory handler table and CCPU
descriptor stubs; no second allocator, mapper or CPU executor was added. See
[S4 evidence](etc/evidence/m0-t288-s4-softpc-sas-mechanics-001.md) and the
[S4 ledger](etc/operations/m0-t288-s4-softpc-sas-disposition-ledger.tsv).

**T288 S5 closure:** the source-shaped `UpdateKbdInt15` IVT 15h
compare/update/reset contract and its original initialization capture are now
locally proven on formal x86 and x64 Ninja fixtures. The full keyboard/IRQ and
CPU re-entry path is explicitly transferred rather than emulated. See [S5
evidence](etc/evidence/m0-t288-s5-keyboard-ivt-lifecycle-001.md) and the [S5
ledger](etc/operations/m0-t288-s5-keyboard-ivt-lifecycle-ledger.tsv).

**T287 S7/T287 closure:** the exact XMS source package is code-complete and
locally regression-closed. The corrected source audit records eleven exact
files and four registered minimal same-shaped modifications; it confirms the
sole session mapping-manager implementation, the sole same-shaped SoftPC
facade, and the explicit non-runtime direct-pointer backends. Successful UMB
and production keyboard/IVT work transfer to the renamed next queue candidate,
not to an invented XMS path. See [S7 closure evidence](etc/evidence/m0-t287-s7-xms-owner-package-closure-001.md).

**T287 S1 baseline note (superseded by S7):** the original source selection is
identical in both canonical roots. The final per-file SHA-256 ledger initially
overstated the current mirror identity; S7 corrects it to eleven exact files
and four registered minimal same-shaped modifications. The direct-address
`i386/xmsmem86.c` body remains exact but is not a product input on either
architecture; it has a named same-shaped successor in
`adapter-mvdm-host-out/softpc`. The remaining `bx-vdm` wording in the
historical tracker is evidence only, never a live input.

**T287 S2 closure:** the formal MSVC `/MT` Ninja graph rebuilt the six exact
common XMS units as `original-xms-common.lib` on x86 and x64.  Archive members
and SHA-256 manifests match the S1 ledger; `i386/xmsmem86.c` is excluded.
Historical pointer-width warnings remain explicit S4 binding debt, not a
source change or a runtime-success claim; see [S2 evidence](etc/evidence/m0-t287-s2-original-xms-static-composition-001.md).

**T287 S3 closure:** original `xmsa20.c` and the reached `xmsumb.c` binding
now retain AX:BX and A20 ordering while the matching private overlay holds
only numeric guest location data and acquires a fresh one-byte session lease
per write. Formal x86 and x64 `/MT` fixtures both pass valid location,
invalid location, lease-expiry and AX/BL-result checks. No `52:xx` route or
direct-address `xmsmem86.c` backend is enabled; see [S3 evidence](etc/evidence/m0-t287-s3-xms-a20-lease-binding-001.md).

**T287 S4 closure:** original `suballoc.c` and `xmsblock.c` now compose on
x86/x64 through same-shaped numeric XMS commit/decommit/move exports. The
only material source divergence is the matching private bounded-lease overlay
for the source `SS:BP-12` move structure. Formal fixtures cover allocation,
move, malformed input, reallocation/free and capacity/range failures; see
[S4 evidence](etc/evidence/m0-t287-s4-xms-block-binding-001.md).

**T287 S5 closure:** original `xmsumb.c` and `xmsmisc.c` now compile and run
locally on x86/x64. The original empty-physical-span `B1`/`B2` outcomes,
page-size/ext-memory results, and `UpdateKbdInt15(CS,AX)` then `CX` ordering
are proven without enabling a selector route or test-only success policy.
Successful UMB ownership transfers to the queued monitor substrate, and the
real keyboard IVT state algorithm transfers to its existing firmware plan;
see [S5 evidence](etc/evidence/m0-t287-s5-xms-umb-ivt-binding-001.md).

**T287 S6 closure:** exact original `xms.c:XMSInit` and `xmsdisp.c` now invoke
every `52:00..0B` table entry in a single local family matrix on formal x86 and
x64 `/MT` Ninja graphs. A20, move, allocation, free, reallocation,
page/free-memory, UMB and INT15 paths retain their original bodies and their
existing source-proven unavailable or transferred outcomes. No BOP ingress is enabled; see
[S6 evidence](etc/evidence/m0-t287-s6-xms-dispatch-matrix-001.md).

**T286 S1 closure:** the complete [divergence disposition ledger](etc/operations/m0-t286-s1-bochs-core-divergence-disposition-ledger-001.md) measures 29 differing upstream-relative mirror files (431 additions / 353 deletions) and 15 private overlay bodies (1,164 lines). It classifies all groups as build bindings, required private CPU/memory/device hooks, optional diagnostics, or B2/D4 external-composition candidates. No source behavior changed.

**T286 S2 closure:** CPU5 and host-pointer-width macros remain necessary generated Bochs configuration differences. Finite `bx_pc_system_c::Reset/exit` bodies require private Bochs state, so `minimal_product_shell.cc` moved from `adapter-bochs` to the matching private `bochs-core-overlay/pc_system_lifecycle.cc`; the mirror retains only its registered whole-method crop. The formal manifest's retired `session_input`/`session_cancellation` entries were also replaced by the actual neutral session sources before verification. The next stale pre-rebootstrap manifest input prevented graph generation; its exact limitation is recorded in the [S2 evidence](etc/evidence/m0-t286-s2-product-shell-overlay-001.md). No lifecycle semantics changed.

**T286 S3 closure:** all minimal port-space, PIC and headless-8042 bodies are confirmed private Bochs-state mechanics with no safe upstream no-product alternative or duplicate adapter lifecycle. The [S3 review](etc/evidence/m0-t286-s3-device-lifecycle-review-001.md) retains D1--D3 unchanged and transfers the only safe reduction opportunity to unconsumed CPU observers.

**T286 S4 closure:** six unconsumed default-off observer/gate files, every mirror call/declaration and all formal diagnostic switches are deleted. The [S4 evidence](etc/evidence/m0-t286-s4-observer-retirement-001.md) records the retained #UD/profile boundary and post-removal totals: 29 core diffs (393 additions / 356 deletions) and nine overlay bodies (978 lines). Full graph generation remains independently blocked by stale manifest roots.

**T286 S5 closure:** SIM-free allocation and checked ordinary/backing RAM remain the minimum private `BX_MEM_C` mechanics. `adapter-bochs` already owns only size/lifecycle selection and typed copied requests; it cannot safely own A20, MMIO/ROM vetoes, sparse vectors, handler tables or write stamps. The ordinary-RAM boundary fixture now follows the private checked-RAM body; see the [S5 evidence](etc/evidence/m0-t286-s5-memory-boundary-review-001.md).

**T285 S1 closure:** `New-T285S1Wave2CurrentWorkset.ps1` revalidated all 81
selected current mirror paths (22 DEM + 59 host-control), recorded their
SHA-256 identities and canonical adapter owners, and rejected stale root
names. Its disposition counts are 49 adapter-backed, five binding-only, six
overlay-required and 21 not-host-runtime. The complete original DEM static
package and the original `nt_bop.c` static package rebuilt on both x86 and
x64 after the formal Ninja recipes began loading the repository MSVC
environment. Historical compiler warnings remain evidence, not runtime
claims. See the [S1 evidence](etc/evidence/m0-t285-s1-wave2-current-workset-001.md).

**T285 S2 closure:** the exact original `nt_bop.c` has 21 reviewed external
contract groups in its [binding ledger](etc/operations/m0-t285-s2-nt-bop-binding-ledger.tsv).
All dynamic provider, host-control, monitor and machine-facing paths now have
one same-shaped declaration, adapter boundary, or explicit later owner with
its original unavailable behavior.  The only two missing declaration carriers
(`DbgBreakPoint`, `getPE`) are now source-shaped and selector-disabled.  The
formal original BOP static archive rebuilt on x86 and x64; see [S2
evidence](etc/evidence/m0-t285-s2-nt-bop-binding-001.md).


**T284 S1 closure:** the generated owner inventory records 33 final file/
boundary dispositions (22 production inputs, seven root documents and four
empty specialist families), 71 public callable declarations/carriers and all
seven MVDM host consumer directions. No production interface changed. See the
[S1 evidence](etc/evidence/m0-t284-s1-adapter-convergence-inventory-001.md)
and its three TSV inventories.

**T284 S2 closure:** all eight approved component roots, the required support
overlay and matching fixture roots now use their canonical names. Four
owner-authorized Win16 mirror directories moved and were staged with the
component. Active source/build/test/current-document scans find zero obsolete
root references; the formal x86/x64 original DEM/COMMAND carrier archives
both rebuild successfully. See the
[S2 evidence](etc/evidence/m0-t284-s2-component-root-rename-closure-001.md).

**T284 S3 closure:** `adapter-mvdm-host-out` now has the seven required named
family boundaries and no implementation source, build input or enabled
capability. The root's source-facing and Bochs-prohibition contract is fixed
in [S3 evidence](etc/evidence/m0-t284-s3-outgoing-adapter-root-admission-001.md).

**T284 S4 closure:** all nine audited Win32 headers/facades, together with
their two focused fixtures, now live under `adapter-mvdm-host-out/win32`.
The component root `adapter-win32` is absent. The formal x86/x64 original
COMMAND/DEM carriers, RTL support archive, and NT I/O facade fixture rebuild
without an interface or behavior change; see the
[S4 evidence](etc/evidence/m0-t284-s4-win32-family-migration-001.md).

**T284 S5–S8 closure:** the four actual SoftPC/CCPU/SAS façade sources and
five declaration carriers moved to `adapter-mvdm-host-out/softpc`; monitor's
two declaration carriers and two session-bound sources moved to `monitor`.
The empty Redirector, WOW, VDD and debugger roots merged into their named
outgoing families. Twelve focused x86/x64 fixture executables passed. The
intermediate result, including the unselected historical T260 graph debt, is
in the [T284 closure record](history/m0-t284-closure-20260826.md).

**T284 S9 closure:** the two selected guest carries are now one load-only
`mvdm-guest` component with `dos/` and `win16/` interior roots. All 676 DOS,
44 WOW16 and 10 bin86 source paths remain hash-identical; eight selected
Win16 driver artifacts are hash-identical to their `build/output/wow16`
sources. No host link input selects the guest component. See the
[S9 evidence](etc/evidence/m0-t284-s9-mvdm-guest-root-merge-001.md).

The detailed execution plans are [S1 original COMMAND package source surface](etc/operations/m0-t282-s1-original-command-package-surface-plan-001.md), [S2 COMMAND declaration carriers](etc/operations/m0-t282-s2-command-declaration-carrier-plan-001.md), [S3 COMMAND console declaration carrier](etc/operations/m0-t282-s3-command-console-declaration-carrier-plan-001.md), [S4 complete original COMMAND static package](etc/operations/m0-t282-s4-complete-original-command-static-package-plan-001.md), [S5 VDM command-acquisition source audit](etc/operations/m0-t282-s5-vdm-command-acquisition-source-audit-plan-001.md), [S6 local VDM command transport](etc/operations/m0-t282-s6-local-vdm-command-transport-plan-001.md), [S7 original COMMAND initialization composition](etc/operations/m0-t282-s7-original-command-initialization-plan-001.md), [S8 COMMAND owner dependency partition](etc/operations/m0-t282-s8-command-owner-dependency-partition-plan-001.md), [S9 COMMAND environment guest-memory seam audit](etc/operations/m0-t282-s9-command-environment-guest-seam-audit-plan-001.md), [S10 COMMAND register and bounded-lease binding](etc/operations/m0-t282-s10-command-register-and-lease-binding-plan-001.md), [S11 COMMAND OEM support composition](etc/operations/m0-t282-s11-command-oem-support-composition-plan-001.md), and [S13 COMMAND current-directory bounded mirror binding](etc/operations/m0-t282-s13-command-current-directory-bounded-binding-plan-001.md).

**T282 S1 P1:** both static-only architecture probes reached the same original
`vdmapi.h` declaration boundary. The verified 11-unit/17-service ledgers and
[surface evidence](etc/evidence/m0-t282-s1-command-source-surface-001.md)
record the exact original carriers and successor owners; no route was enabled.

**T282 S2 P1:** byte-exact original `vdmapi.h` and `pif.h` are now registered
in the platform-ABI manifest. Focused x86/x64 static probes compile original
`cmdexec.c` and `cmdpif.c` through those carriers. The whole package remains
selector-disabled; `cmdkeyb.c` stops at the separately-owned private
`winconp.h` declaration boundary.

**T282 S2 closure:** both admitted declaration carriers are hash-proven and
selected on x86/x64 without adding any behavior. `GetNextVDMCommand` remains
for the session/monitor binding package; PIF behavior and the `winconp.h`
console-private boundary remain later named work. See the [S2 evidence](etc/evidence/m0-t282-s2-command-declaration-carriers-001.md).

**T282 S3 P1:** byte-exact `winconp.h` is registered in the platform-ABI
manifest and selected through an adapter declaration carrier. Original
`cmdkeyb.c` compiles on x86/x64; keyboard service, registry, console policy
and guest-memory behavior remain unenabled.

**T282 S3 closure:** the complete admitted console declaration carrier is
hash-proven and both architecture probes pass without source-body changes.
The remaining keyboard provider policy belongs to its later COMMAND/session
binding work. See the [S3 evidence](etc/evidence/m0-t282-s3-command-console-declaration-carrier-001.md).

**T282 S4 P1:** both formal MSVC graphs archive the exact eleven original
COMMAND units as `original-command-surface.lib`; archive member and graph
verification reject runtime ingress, executable and Bochs inputs.

**T282 S4 closure:** the complete original COMMAND source package has a
source-identity-preserving static archive on x86/x64. Callable session,
monitor, stream, PIF and child-lifecycle boundaries remain unresolved by
design; no `54:xx` route or provider is enabled. See the [S4 evidence](etc/evidence/m0-t282-s4-complete-original-command-static-package-001.md).

**T282 S5 P1:** the original client/BaseSrv command-acquisition contract is
now partitioned into seven source/ABI slices. Single-session DOS request,
re-entry and retry semantics have a named same-shaped session/monitor owner;
CSR first-VDM and WOW/broker branches remain explicit later owners.

**T282 S5 closure:** no callable behavior is enabled. The next implementation
may recover only the one-session copied request/response binding while
preserving original size, retry and failure semantics. See the [S5 evidence](etc/evidence/m0-t282-s5-vdm-command-acquisition-audit-001.md).

**T282 S6 P1:** the one-session source-shaped `GetNextVDMCommand` transport
now carries only a copied app record through a bound session dispatch and
retains bounded copies, required-size failures, environment-only response,
re-entry operations and the terminal no-command outcome. The focused formal
x86/x64 Ninja fixtures pass with no BOP route, Bochs input or historical CSR
composition.

**T282 S6 closure:** the local DOS request/response branch is closed as an
adapter-backed original contract. First-VDM, BaseSrv/CSR wait/broker, PIF and
WOW branches remain explicit non-routes, not fallback behavior. See the
[S6 evidence](etc/evidence/m0-t282-s6-local-vdm-command-transport-001.md).

**T282 S7 P1:** exact original `cmd.c` and `cmddata.c` now compose directly
as one initialization slice on x86/x64. The fixture verifies the original
globals and `CMDInit` home-drive update; the only diagnostics are the retained
K&R and unused-parameter warnings from the unchanged original source.

**T282 S7 closure:** this initialization slice is directly source-complete;
it does not enable COMMAND dispatch or any guest route. See the [S7 evidence](etc/evidence/m0-t282-s7-original-command-initialization-001.md).

**T282 S8 P1:** all 17 original COMMAND dispatcher services and 13 reached
helper groups now have one source/ABI/recovery disposition in the verified
32-row owner ledger. The resulting order chooses an environment/current-
directory guest-query group next, not a trace-hit leaf service.

**T282 S8 closure:** no implementation was enabled. The partition gives every
reached COMMAND dependency a named owner and legal successor boundary; see
the [S8 evidence](etc/evidence/m0-t282-s8-command-owner-dependency-partition-001.md).

**T282 S9 P1:** all selected COMMAND guest accesses are reduced to four
source-backed records. The existing session `guest_memory` mapping-manager
instance and adapter-mvdm-host-out/softpc bounded leases are the sole legal route.

**T282 S9 closure:** the later group may use only exact, synchronous leases:
64-byte COMSPEC input, 67-byte current-directory output and source-calculated
initial-environment output. Naked `GetVDMAddr` remains forbidden; see the
[S9 evidence](etc/evidence/m0-t282-s9-command-environment-guest-seam-audit-001.md).

**T282 S10 closure:** the reached original `softpc.h` COMMAND register imports
now bind through source-shaped adapter-mvdm-host-out/softpc functions to selector-blind
adapter-bochs mechanics. The formal x86/x64 fixture and real adapter-bochs
source compiles pass; guest memory remains S9's exact bounded lease. See the
[S10 evidence](etc/evidence/m0-t282-s10-command-register-and-lease-binding-001.md).

**T282 S11 closure:** exact original OEM process/current-directory helper
bodies compose and execute on x86/x64 through the existing adapter-mvdm-host-out/win32 RTL
facade and public Kernel32/GDI32 imports. Unreached historical pointer-width
branches remain separately tracked; no COMMAND mirror body changed. See the
[S11 evidence](etc/evidence/m0-t282-s11-command-oem-support-composition-001.md).

**T282 S12 P1:** bounded C-string read and exact host-to-guest write are now
adapter-mvdm-host-out/softpc operations over the sole session guest-memory lease. The formal
x86/x64 fixture covers positive and negative spans; COMMAND mirror source is
still unchanged. See the [S12 P1 evidence](etc/evidence/m0-t282-s12-bounded-copy-binding-001.md).

**T282 S12 closure:** the source-independent bounded C-string and exact-write
forms are verified on x86/x64. The next packet may bind only a source-backed
span with an explicit original failure disposition; S13 admits current
directory output alone.

**T282 S12 P2:** the same reached original register boundary now includes
`getAL`/`setAX`, required by `cmdGetCurrentDir`; the x86/x64 regression passes.

**T282 S13 closure:** the original `cmdGetCurrentDir` now retains its source
drive/environment order and CF/AX contract while replacing only the unsafe
`DS:SI` alias with S12's exact synchronous descriptor write. The original
COMMAND source archive and bounded-copy fixture pass on x86/x64. This is not
a callable `54:04` claim; see the [S13 evidence](etc/evidence/m0-t282-s13-command-current-directory-bounded-binding-001.md).

**T282 S14 closure:** the original `cmdComSpec` has no observable service
failure branch for a malformed/unmappable COMSPEC string; the bounded
descriptor correctly rejects it but cannot choose AL/CF/stop semantics. No
mirror body changed and `54:02` remains disabled; see the [S14 audit](etc/evidence/m0-t282-s14-command-comspec-bounded-input-audit-001.md).

**T282 S15 closure:** original `cmdGetInitEnvironment` preserves an explicit
`BX` shortage result but has no result for a post-capacity guest-lease fault.
No mirror body changed or `54:0F` route was enabled; see the [S15 audit](etc/evidence/m0-t282-s15-command-init-environment-bounded-output-audit-001.md).

**T282 S16 closure and T282 closure:** the full 32-row original COMMAND
ledger has no ownerless or directly recoverable local source item remaining.
T282 closes as source-composition work only; see the [S16 audit](etc/evidence/m0-t282-s16-command-local-package-closure-audit-001.md) and [T282 closure](history/m0-t282-closure-20260827.md).

**T283 S1/S2 P1:** the 29-header platform-ABI inventory and selection ledger
classify 18 one-sided, three byte-identical dual-source and eight conflict
records. All selected declarations are now hash-proven under one logical
`source/public` or `source/private` topology; live source/build/test inputs no
longer name edition roots. Formal x86/x64 Ninja carrier archives passed. See
the [S2 evidence](etc/evidence/m0-t283-s2-platform-abi-canonical-reroot-001.md).

### Closed packet reference — M0 T281 S5 — original host-control package disposition

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T281 S5`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T281/S4 is complete as `252c2a81`; S5 applies the existing complete 56-source host/src owner audit to form the entire ordered control-plane recovery partition. |
| Objective | Give every original `softpc.new/host/src` control-plane source and interface one final near-term disposition: eligible static composition group, explicit existing-adapter prerequisite, later specialist owner package, or Bochs-replacement exclusion. |
| Non-goals | Compiling an unaudited all-host mega-library, enabling BOP selectors, reviving a CCPU/SoftPC executor, adding a generic shim, executing a guest trace, changing a mirror body, or treating one compiler diagnostic as a leaf feature task. |
| Reference Baseline | T276 S12 complete six-family owner ledgers, T277 package-cycle ledger, T280 ingress/machine-facade ledger, T281/S1–S4 DEM/static control evidence, and the Wave 2 plan. |
| Files And ABI Surface | Original `softpc.new/host/src` source and six T276 S12 owner ledgers; adapter-mvdm-host-out/win32, adapter-mvdm-host-out/softpc, adapter-mvdm-host-out/monitor, adapter-mvdm-host-in, session, app, adapter-mvdm-host-out/vdd and adapter-mvdm-host-out/debugger boundary records; S5 consolidation ledger/evidence only. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | Every host/src source appears once in the consolidation ledger; disposition cross-checks the six existing detailed ledgers; no new implementation input is selected; documentation governance and `git diff --check`. |
| Expected Markers | Explicit recovery groups and prerequisite adapters; device/executor exclusions remain separate; no source body is activated by the audit. |
| Asset Needs | Pinned original mirror, T277/T280 ledgers, existing component headers, MSVC x64/x86 and Ninja. |
| Reporting Requirements | Report per-import source form, owner/binding, architecture result, compile/link state, excluded branch and any boundary that requires a re-admission. |
| Stop Conditions | Any row lacks an existing tracker owner, a proposed group would mix Bochs device/executor semantics with MVDM control semantics, or a new adapter is assumed without a named source interface. |
| Exit Criteria | Met: all 56 sources are assigned once to an ordered recovery group or explicit non-eligibility owner and each prerequisite is named. See [M0 T281 closure](history/m0-t281-closure-20260826.md). |
| Original Owner Request | “我都叫你按顺序执行了”。 |
| Similar-Issue Sweep | All six T276 S12 owner families, every host/src path, startup/termination/event/monitor/BOP forms, and all machine/device/external product exclusions. |

**T281 S1 P1:** `558a9626` selected and imported exact OpenNT `vdm.h`, made the three historical function-pointer conversions explicit under `MVDM-HOST-DIV-008`, and proved a selector-disabled original `nt_bop.c` static-library island on x86 and x64. The dispatcher is not linked into a runtime image and no selector/provider was enabled.

**T281 S2 P1:** the formal x86/x64 Ninja islands now compile and archive the
four original DEM common-control units as `original-dem-common-control.lib`.
The graph retains original `WIN_32`/`DEVL` declaration form, uses only existing
same-shaped declaration carriers, and is verifier-proven to exclude provider,
ingress, executable, and Bochs source inputs. No `50:xx` route is enabled.

**T281 S2 closure:** the admitted four-file common-control group has met every
exit criterion. Its scope is closed as a static source composition island;
provider bodies are explicitly transferred to S3, not treated as an unplanned
trace-derived backlog.

**T281 S3 P1:** the complete remaining twelve-unit original DEM provider
matrix now compiles and archives on x86 and x64 as
`original-dem-providers.lib`. The existing same-shaped
former `adapter-mvdm-host-out/monitor/include/vdm.h` declaration carrier closes the only
matrix-specific declaration gap (`NtVdmControl(VdmQueryDir, ...)`) in
`demsrch.c`; its implementation body is excluded. The archive is
selector-disabled and does not invoke a provider or enable `50:xx`.

**T281 S3 closure:** every remaining provider unit has its x86/x64 source
composition disposition and the only matrix-specific declaration gap is closed
by an existing, implementation-excluded same-shaped boundary. Full-package
archive composition transfers to S4.

**T281 S4 P1:** x86 and x64 now each archive exactly the sixteen original DEM
objects as `original-dem-package.lib`. The archive-member verifier proves the
complete source set and rejects executables, runtime rules, BOP ingress,
Bochs, `nt_bop.c`, and the VDM control implementation body.

**T281 S4 closure:** the full original DEM package now has a single
source-identity-preserving static archive per architecture. Runtime dispatch,
provider invocation and host control composition remain deliberately outside
that archive and transfer to the host-control package sequence.

**T281 S5 P1:** the reproducible consolidation ledger now assigns all 56
original `host/src` paths exactly once: 23 `adapter-prerequisite`, 18
`bochs-replacement`, 15 `specialist-owner`, and zero `eligible-next`. The
generator and verifier prove unique source/owner records; no source body,
selector, provider, machine executor, or guest route was enabled. See the
[S5 disposition evidence](etc/evidence/m0-t281-s5-host-control-disposition-001.md).

The detailed execution plans are [S1 original BOP dispatch composition](etc/operations/m0-t281-s1-original-bop-dispatch-composition-plan-001.md), [S2 original DEM common-control composition](etc/operations/m0-t281-s2-original-dem-common-control-composition-plan-001.md), [S3 original DEM provider source composition](etc/operations/m0-t281-s3-original-dem-provider-source-composition-plan-001.md), [S4 complete original DEM static package composition](etc/operations/m0-t281-s4-complete-original-dem-static-package-composition-plan-001.md), and [S5 original host-control package disposition](etc/operations/m0-t281-s5-original-host-control-package-disposition-plan-001.md).

### Closed packet reference — M0 T280 S20

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S20`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S19 proved that original `demgset.c` retains native GetVDMAddr results beyond a legal lease lifetime, blocking a raw-pointer facade. P7 then proved SFT/JFT requires a distinct VDD shadow lifecycle; the owner approved its source-shaped P8 recovery. |
| Objective | Recover the reached DEM persistent guest-location globals as numeric descriptors and fresh per-dereference guest-memory leases. **Closed.** |
| Non-goals | No generic GetVDMAddr raw-pointer shim, DEM provider/BOP route, guest trace, directory backend, new mapping-manager implementation, Bochs change, broker value or async guest-pointer retention. P8 may use the existing session `host_resource` instance only to own host-local SFT shadow/HANDLE identities. |
| Reference Baseline | Original demgset/demsrch consumers, S19 lease ledger/evidence, session guest-memory API and adapter-mvdm-host-out/softpc/adapter-bochs ownership rules. |
| Files And ABI Surface | Reached original DEM globals/consumers, adapter-mvdm-host-out/softpc source-shaped descriptor/lease boundary, focused fixtures/evidence and Status. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | All reached global assignments/uses are inventoried; descriptor persistence, x86/x64 range rejection, lease release and no-native-pointer-retention tests pass. |
| Expected Markers | Persistent guest locations remain numeric; every actual dereference uses a fresh exact read/read-write lease and no source global is a native pointer. |
| Asset Needs | S19 ledger, original DEM source/layouts, current session lease API, adapter-mvdm-host-out/softpc/adapter-bochs contracts and formal Ninja/MSVC matrix. |
| Reporting Requirements | Original global/caller, numeric descriptor layout, lease extent/access/lifetime, source failure preservation, all consumer coverage and remaining provider limit. |
| Stop Conditions | A descriptor is tokenized, a host pointer/HANDLE enters MVDM global state, a lease crosses an async callback, a generic mapper is created, or a provider/BOP route is enabled. |
| Exit Criteria | Met: every reached retained guest pointer is removed/recovered as a numeric descriptor with focused x86/x64 evidence; no actual guest pointer survives its source callback. See [M0 T280 S20 closure](history/m0-t280-s20-closure-20260826.md). |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | All demSetDTALocation globals, every reached dereference in demgset/demsrch/demfile/demfcb, DTA/PDB paths, descriptor arithmetic, lease cleanup and invalid/stale guest address behavior. |

P1 inventory is recorded in the [persistent-location ledger](etc/operations/m0-t280-s20-dem-persistent-guest-location-ledger-001.tsv).
P2 fixes the numeric descriptor, packed-layout and lease conversion contract in
the [descriptor/lease design](etc/operations/m0-t280-s20-p2-descriptor-layout-and-lease-design-001.md).
P3 has independently verified the source-facing bounded lease facade on x86
and x64 in [fixture evidence](etc/evidence/m0-t280-s20-p3-mvdm-guest-location-fixture-001.md).
P4 has converted only the extended-error retained location and records its
source/layout evidence in [extended-error recovery](etc/evidence/m0-t280-s20-p4-extended-error-location-recovery-001.md).
P5 has removed the reached DTA retained pointer and records its focused
source/fixture evidence in [DTA recovery](etc/evidence/m0-t280-s20-p5-dta-location-recovery-001.md).
P6 has removed the reached current-PDB pointer across DEM and selected SoftPC
consumers; its source recovery and transferred product-shell interface gaps
are recorded in [PDB recovery](etc/evidence/m0-t280-s20-p6-current-pdb-source-recovery-001.md).
P7 proves that SFT/JFT is a distinct VDD host-buffer lifecycle boundary, not
a legal scalar lease conversion; see the [SFT boundary audit](etc/operations/m0-t280-s20-p7-sft-chain-boundary-audit-001.md).
P8 recovers that boundary as one session-owned, source-shaped VDD SFT/JFT
shadow/commit contract; x86/x64 evidence is in [SFT/JFT recovery](etc/evidence/m0-t280-s20-p8-vdd-sft-jft-shadow-recovery-001.md).

**S20 closure:** the extended-error, DTA, current-PDB and SFT-chain retained
guest locations now preserve numeric source data and use only fresh bounded
leases or bounded synchronous shadows. The historical raw-pointer body remains
inactive provenance; no provider or BOP route is enabled.

### Closed packet reference — M0 T280 S13

**S13 closure:** the shared NT/RTL declaration surface makes all three original
DEM bodies syntax-clean on x86/x64; remaining host identity casts are
explicitly transferred to S14. See [S13 closure](history/m0-t280-s13-closure-20260826.md).

### Closed packet reference — M0 T280 S14

**S14 closure:** original `demsrch.c` persisted find-list identity is now
classified as a session `host_resource` surrogate, distinct from DTA/FCB
`guest_memory` leases. The existing scoped session binding and the missing
adapter-mvdm-host-out/softpc identity facade are explicit successor evidence; no DEM mirror
body or provider was enabled. See [S14 closure](history/m0-t280-s14-closure-20260826.md).

### Closed packet reference — M0 T280 S15

**S15 closure:** `adapter-mvdm-host-out/softpc` now exposes the one bounded host-resource
surrogate facade over the thread-bound session. Its x86/x64 fixture proves
identity, stale-token, isolation and disposal outcomes; it has no
guest-memory route. The original DEM mirror remains unchanged. See
[S15 closure](history/m0-t280-s15-closure-20260826.md).

### Closed packet reference — M0 T280 S16

**S16 closure:** original DOS DTA/FCB find fields now retain the exact x86
layout on both architectures and persist only session `host_resource`
surrogates. Formal x86/x64 source and layout evidence passed; remaining
unrelated `demsrch.c` width warnings transfer to S17 as one source audit. See
[S16 closure](history/m0-t280-s16-closure-20260826.md).

### Closed packet reference — M0 T280 S17

**S17 closure:** all remaining `demsrch.c` x64 warning/cast groups have a
source-owner, value-class, mapping and follow-on disposition. Host-local
directory state is not tokenized; only the separately bounded `GetVDMAddr`
family is a future guest-memory lease. See
[S17 closure](history/m0-t280-s17-closure-20260826.md).

### Closed packet reference — M0 T280 S18

**S18 closure:** all selected host-local `demsrch.c` x64 groups compile on
x86/x64 without width diagnostics, and the same-shaped VDM control family now
belongs solely to `adapter-mvdm-host-out/monitor`. The guest `GetVDMAddr` boundary
remains deliberately unimplemented for S19 source-first audit. See
[S18 closure](history/m0-t280-s18-closure-20260826.md).

### Closed packet reference — M0 T280 S19

**S19 closure:** `demsrch.c` guest accesses are fully classified as bounded
lease candidates, and the retained pointer bug is traced to
`demSetDTALocation` across owner files. S20 must recover numeric descriptors
before any raw source-facing guest pointer can exist. See
[S19 closure](history/m0-t280-s19-closure-20260826.md).

### Closed packet reference — M0 T280 S12

**S12 closure:** all three original reached DEM bodies have x86/x64 object
observations; shared NT/RTL declarations and the distinct find-list identity
boundary are classified without enabling a provider. See
[S12 closure](history/m0-t280-s12-closure-20260826.md).

### Closed packet reference — M0 T280 S11

**S11 closure:** one source-shaped adapter-mvdm-host-out/win32 NT I/O facade parses original
`dem.h`, compiles, links and runs on x86/x64. It preserves the original
32-bit status record at the host-local `ntdll` boundary and has no guest-memory
mapping route. See [S11 closure](history/m0-t280-s11-closure-20260826.md).

### Closed packet reference — M0 T280 S10

**S10 closure:** the 11-row reachable `ntioapi` declaration/call ledger and
source-first implementation plan are complete and verified. The exact original
carrier remains unchanged; host-local NT records and handles are explicitly
outside every mapping-manager instance. See
[S10 closure](history/m0-t280-s10-closure-20260826.md).

### Closed packet reference — M0 T280 S9

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S9`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S8 closed in `c3359863`; S7/S8 prove a declaration-only, same-shaped binding is the next smallest recovery rung. |
| Objective | Make the exact original storage declaration carrier visible through adapter-mvdm-host-out/win32 so original DEM header form can parse without selecting a modern SDK substitute. |
| Non-goals | No DEM body build/link, provider/BOP route, host API implementation, SoftPC mapping call, private NT import, guest trace or generic shim. |
| Reference Baseline | T280 S7 source-form ledger, S8 exact import manifest, existing `devioctl.h` mirror and adapter-mvdm-host-out/win32 `nt.h` facade. |
| Files And ABI Surface | `adapter-mvdm-host-out/win32/include/nt.h`, adapter README, S9 plan/fixture/verifier and Status. No behavior ABI is introduced. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and recovery-ladder rules. |
| Verification | Focused x86/x64 syntax fixture includes original `dem.h`, verifies source identity of exact carriers and rejects a DEM body/provider/mapping input. |
| Expected Markers | Original `devioctl.h` supplies `DEVICE_TYPE`; original nested `ntddstor.h` is selected; original `dem.h` parses. |
| Asset Needs | S8 exact mirrors, MSVC x86/x64, current adapter-mvdm-host-out/win32 facade and original `dem.h`. |
| Reporting Requirements | Smallest declaration divergence, architecture results, hash/provenance proof, and explicit no-behavior/no-mapping limitation. |
| Stop Conditions | A modern storage header is substituted, an original carrier is edited, a DEM body is admitted, a private API is called, or mapping manager code appears. |
| Exit Criteria | Both architectures pass the declaration-only fixture with exact source carriers; README registers the facade-only divergence; no source body is built. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Include ordering, nested original headers, `DEVICE_TYPE` source, modern SDK collisions, adapter divergence register and mapping isolation. |

**S9 closure:** formal Ninja x64/x86 syntax fixtures select exact
`devioctl.h`, `ntdddisk.h` and `ntddstor.h` through the existing adapter-mvdm-host-out/win32
facade and parse original `dem.h`. The sole divergence removes a modern
same-name macro collision. Full `ntioapi.h` is explicitly not included: it
requires a whole same-shaped NT I/O ABI package, not an unreviewed facade
expansion. No DEM body, provider, host API call or mapping operation is added.

### Closed packet reference — M0 T280 S8

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S8`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S7 closed in `ffe95bcf`; it selected exact OpenNT `ntdddisk.h`, `ntddstor.h` and `ntioapi.h` as the first recovery rung. |
| Objective | Import the three byte-identical original declaration carriers with complete provenance and no body/provider admission. |
| Non-goals | No original DEM edit/link, adapter behavior, Win32/NT API call, SoftPC mapping implementation, BOP route, guest trace or private kernel import. |
| Reference Baseline | T280 S7 five-family ledger, source policy, existing platform ABI import manifest and pinned OpenNT source. |
| Files And ABI Surface | Three exact platform declaration files, import ledger/manifest, plan, Status and documentation inventory. No product ABI behavior is introduced. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and recovery-ladder rules. |
| Verification | Verify byte hashes against pinned sources, source/destination manifest entries, required include roots and no source-body/binding change. |
| Expected Markers | Exact DDK storage carrier pair, exact SDK NT IO carrier and reproducible source provenance. |
| Asset Needs | Pinned OpenNT public DDK/SDK headers and existing `mvdm-platform-abi` source root. |
| Reporting Requirements | Exact source/destination/hash, direct consumer evidence, retained declaration-only role and no-mapping/no-provider limitation. |
| Stop Conditions | A carrier is edited, a modern SDK header substitutes for it, a body is enabled, a host pointer is mapped, or an adapter implementation is smuggled into the import. |
| Exit Criteria | Three files are byte-identical and manifest-backed; all are declaration-only; no DEM source body or provider is built/linked. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Nested original headers, source-tree provenance, existing platform ABI destination paths, host-local record ownership and mapping-manager isolation. |

**S8 closure:** exact OpenNT `ntdddisk.h`, `ntddstor.h` and `ntioapi.h` are
now imported under the platform ABI mirror with source path/hash/consumer
manifest evidence. They are declaration-only carriers. This does not compose
a DEM body, bind a historical NT call, create a mapping-manager route or
enable any BOP.

### Closed packet reference — M0 T280 S7

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S7`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S6 closed in `43d26f8d`; its whole-package evidence requires one source-first ABI recovery design, not a trace-selected implementation. |
| Objective | Recover exact source/header/compile-condition evidence and one owner disposition for the five shared DEM ABI families before any declaration or adapter body is added. |
| Non-goals | No original DEM edit/link, provider/BOP route, adapter implementation, private NT call, generic compatibility layer, guest trace or new mapping manager. |
| Reference Baseline | T280 S4 include ledger, S5 VDM directory-control adapter, S6 compile-surface ledger/evidence, exact OpenNT `dos/dem`, `inc`, SDK and SoftPC sources. |
| Files And ABI Surface | S7 source/consumer/shape/failure ledger, plan, Status and documentation inventory only. No product ABI is introduced. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | Compare every cited spelling/form/condition with the exact original source; verify five unique owner dispositions and mapping-manager isolation; run documentation governance and diff checks. |
| Expected Markers | `DEVL` source form, x86 `GetDiskSpaceInformationA` collision, NT directory/storage declarations, DASD storage boundary and source-proven SoftPC guest-memory operations. |
| Asset Needs | Original OpenNT/OpenNT-4.5 source evidence, current exact mirrors, adapter-mvdm-host-out/win32/session/adapter-mvdm-host-out/softpc contracts and S6 source logs summarized in evidence. |
| Reporting Requirements | Original path/line/shape, DEM consumers, architecture difference, modern availability, failure behavior, future owner and mapping disposition per family. |
| Stop Conditions | A missing shape is guessed, a body is enabled, a host-local pointer enters a mapping manager, a private kernel VDM path is invoked, or a family-private mapper is proposed. |
| Exit Criteria | A five-family source-first ledger identifies the smallest exact-mirror, same-shaped adapter or adapter-mvdm-host-out/softpc follow-on package; no unresolved source definition remains. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Historical build macro forms, x86 public SDK name collisions, NT data-layout sources, host versus guest pointer classification, SoftPC/CCPU mechanical names and all future consumers. |

**S7 closure:** five source-shaped families now have original source paths,
historical compile conditions, consumer sets, current modern findings, unique
future owner and mapping disposition. The next admissible implementation is
one shared package: exact historical declaration carriers where available,
same-shaped adapter-mvdm-host-out/win32 bindings for modern collisions, and one complete
adapter-mvdm-host-out/softpc guest-memory lease surface. No original DEM translation unit is
enabled by this closure.

### Closed packet reference — M0 T280 S6

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S6`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S5 completed and was pushed in `3cddbc0f`; the active owner goal requires a package-led, non-trace-led MVDM recovery. |
| Objective | Derive one complete, source-proven next adapter/declaration workset from a reproducible syntax survey of all sixteen original DEM bodies. |
| Non-goals | No body enablement/edit, provider or BOP route, private NT import, guest trace, Bochs modification, or one-off per-BOP workaround. |
| Reference Baseline | T280 S1 workset, S4 include ledger, S5 VDM control boundary and original `dos/dem/sources`. |
| Files And ABI Surface | S6 plan, 16-row source ledger, formal survey/verifier and Status. No product ABI is introduced. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | MSVC `/Zs /MT /W4` survey for x64/x86 under a disposable run root; verify all selected bodies occur once in the ledger; review each owner/mapping disposition. |
| Expected Markers | Whole-package source selection; original DEM build-form globals; historical NT directory/storage ABI families; explicit `adapter-mvdm-host-out/softpc` guest-memory mapping boundary. |
| Asset Needs | Existing exact host/support/platform mirrors, adapter-mvdm-host-out/win32/session contracts, MSVC x86/x64 and original sources evidence. |
| Reporting Requirements | Architecture-specific syntax outcome, exact blocker families, unique owner, host-local versus guest-numeric classification, and next package disposition. |
| Stop Conditions | A source body is enabled from syntax success, a private NT kernel call is attempted, a new family-private mapper appears, or a compiler error is patched without whole-package classification. |
| Exit Criteria | Both architecture surveys and the 16-row owner/mapping ledger are verified; next implementation is a bounded shared ABI package, not a trace-selected leaf. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | DEM globals/precompiled header assumptions, NT directory/device declaration families, DASD boundaries, CCPU/SAS mechanics, and mapping-manager isolation. |

**S6 closure:** the sixteen exact DEM bodies were syntax-surveyed on x64 and
x86.  The source/ABI ledger reduces every observed blocker to five shared
owner families: original DEM declaration form, one x86 SDK declaration
collision, NT directory/device record forms, DASD public storage forms, and
adapter-mvdm-host-out/softpc guest mechanics.  Syntax-clean bodies are only original-body
candidates—not linked providers.  The next admission must address one shared
source-shaped compatibility package; it may not patch a trace-selected BOP.

### Closed packet reference — M0 T280 S5

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S5`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S4 closed in `33265acd`; recover only the source-proven `demsrch.c` private VDM declaration/control boundary. |
| Objective | Provide same-shaped `vdm.h` declarations and a session-bound `NtVdmControl(VdmQueryDir, VDMQUERYDIRINFO)` adapter contract, plus behavior-free `winbasep.h` include preservation. |
| Non-goals | No original-body edit, full NT4 kernel VDM service, directory-query backend, BOP enablement, DEM body link, app link, guest trace, second executor, or private product-shell import. |
| Reference Baseline | T280/S4 gap evidence, OpenNT `vdm.h`, `ntos/vdm/vdm.c`, selected `demsrch.c`, session lifecycle and adapter-mvdm-host-out/win32 public facade. |
| Files And ABI Surface | Reached VDM declarations, session generic synchronous control seam, `NtVdmControl` adapter and focused contract fixture. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | Original enum/structure/function spelling is available; only `VdmQueryDir` reaches a bound session handler; no handler and all other VDM services return `STATUS_NOT_IMPLEMENTED`; no host-local control pointer enters guest mapping. |
| Expected Markers | Same-shaped `VDMQUERYDIRINFO`; session synchronous control callback; behavior-free `winbasep.h`; focused no-session/unregistered/registered/unsupported fixture. |
| Asset Needs | Existing session component, adapter-mvdm-host-out/win32 facade, public Windows types and OpenNT source evidence. |
| Reporting Requirements | Source semantics, bounded modern divergence, session ownership, negative capability outcomes and formal x86/x64 test results. |
| Stop Conditions | A full private VDM/Base header is imported, an unregistered call succeeds, a raw host pointer is mapped as guest identity, a kernel VDM call is attempted, or a DEM body is enabled. |
| Exit Criteria | Same-shaped reached ABI and session route compile/test on x86/x64; all unimplemented service classes fail explicitly; no provider/backend is falsely claimed. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | DEM vs COMMAND ownership, host-control executors vs façades, overlay transport seams, session/mapping boundary, known adapter ownership, and non-runtime historical product shells. |

### Closed packet reference — M0 T279 S4

### M0 T279 S4 — support-package closure audit

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T279 S4`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T279/S3 is complete and pushed as `7679ae9c`; audit the complete support package before T279 closure. |
| Objective | Prove that all 112 exact support paths retain their correct source/build role, both original libraries have bounded x86/x64 build evidence, and no provider/BOP/guest route was accidentally enabled. |
| Non-goals | No new provider, adapter family, BOP, guest runtime, suballoc callback binding, or host-control implementation. |
| Reference Baseline | T278 exact movement closure plus T279/S1–S3 ledgers, bindings, build plan and build evidence. |
| Files And ABI Surface | Closure verifier, closure history record, Status and documentation inventory only. |
| Verification | Re-run T278, T279/S1, S2 and S3 verifiers; inspect formal artifacts; scan support/overlay build inputs for forbidden provider/guest/component edges. |
| Exit Criteria | Met: one closure record distinguishes source mirror, x86 original library, x64 four-export overlay, static-only callback boundary and excluded tool/test inputs; all gates pass. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |

### Closed packet reference — M0 T279 S3

### M0 T279 S3 — dual-architecture original-library build closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T279 S3`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T279/S2 is source-form closed and pushed as `f6a2a996`; enable only the audited support-library build islands. |
| Objective | Build `oemuni.lib` and `suballoc.lib` from original selected bodies on x86 and x64 with Ninja/MSVC `/MT`; provide the four source-derived x64 OEM wrappers required to avoid pointer narrowing. |
| Non-goals | No edit to original mirror sources, provider/BOP activation, raw pointer mapping, suballoc callback backend, host control plane, or guest trace. |
| Reference Baseline | T279/S1 ledger, T279/S2 binding audit, original `sources` files and exact mirror hashes. |
| Files And ABI Surface | Generated Ninja graph, `mvdm-support-overlay` four exported OEM wrappers, focused host-local contract fixture and formal build evidence. |
| Verification | x86 links only original OEM bodies; x64 compiles renamed original unsafe bodies plus overlay exports; both build `oemuni.lib` and `suballoc.lib`; fixture proves four x64 wrapper contracts without native pointer narrowing. |
| Stop Conditions | An overlay grows beyond the four audited functions, an original mirror is edited, a second mapping manager or guest pointer route appears, or a warning is suppressed to fabricate closure. |
| Exit Criteria | Reproducible Ninja manifests build both architecture library forms with no duplicate exports; original hashes remain exact; x64 overlay behavior has local tests; warning disposition is explicit. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | x86/x64 export uniqueness, offset reconstruction after OEM conversion, find-data header copy size, static-library link order, original compiler warnings, and excluded tests/tools. |

### Closed packet reference — M0 T279 S2

### M0 T279 S2 — same-shaped Win32/RTL binding closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T279 S2`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S1 source-form interface closure is complete; recover only the named same-shaped adapter-mvdm-host-out/win32 bindings before support-library build admission. |
| Objective | Supply the exact reached NT4 RTL/PEB/TEB declaration and implementation surface for byte-exact `oemuni/file.c` and `oemuni/process.c`, using public modern Win32 APIs while keeping native pointers private. |
| Non-goals | No edit to original support-library bodies, provider/BOP route, XMS operation, raw host-pointer boundary, legacy `CreateProcessOem` body currently excluded by its original `#if 0`, or guest trace. |
| Reference Baseline | T279/S1 71-row source-form interface ledger and source-form audit. |
| Files And ABI Surface | `adapter-mvdm-host-out/win32` compatibility headers and implementation; an explicit exception record for x64-only pointer representation seams. |
| Verification | All reached bindings have source-shaped declarations; original bodies compile against them without source edits; x64 pointer-derived output is never narrowed through `DWORD`. |
| Stop Conditions | A binding requires a private NT kernel/CSRSS API, a native pointer crosses guest/session ABI, an original body is modified without mirror approval, or an adapter reimplements OEM provider logic. |
| Exit Criteria | The smallest audited same-shaped facade exists, its PEB/TEB/RTL state is local to the adapter, and the source/build-island admission record distinguishes any required overlay from direct source compilation. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Counted-string allocation/free pairs, thread-local static Unicode buffer, current-directory snapshot/lock, OEM/ANSI conversion error mapping, and `SearchPathOem` x64 pointer reconstruction. |

### Closed packet reference — M0 T279 S1

### M0 T279 S1 — original support-library ABI/build-form closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T279 S1`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner directed single-person dual-role execution of the next dependency-ordered queue task. |
| Objective | Audit the exact original library translation units (`oemuni/file.c`, `oemuni/process.c`, `suballoc/suballoc.c`), their original `sources` selection, headers and all required x86/x64 boundary symbols before any modern build target is enabled. |
| Non-goals | No provider/BOP route, XMS body, MVDM tool/test product, anonymous rehost, runtime execution or guest trace. |
| Reference Baseline | T276 W0/W1 final dispositions, T277 reviewed interface ledger, T278 physical mirror closure and original source/build files. |
| Files And ABI Surface | S1 source-form interface ledger/audit, Status and documentation inventory. No runtime ABI changes. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | Exact original source selections match `sources`; every external declaration/call from the three library units has source location, original shape, unique owner, x86/x64 disposition and failure rule; tests/tools remain excluded. |
| Expected Markers | Two original OEM library bodies adapter-mvdm-host-out/win32-bound; one suballocator body caller-bound through adapter-mvdm-host-out/softpc; no additional runtime translation unit. |
| Asset Needs | Re-rooted support mirror, original `sources`/makefiles, T276 W0/W1 records, T277 interface closure and [S1 plan](etc/operations/m0-t279-s1-support-library-abi-build-form-plan-001.md). |
| Reporting Requirements | Exact original library source list, dependency-symbol list, adapter/mapping owner, excluded tool/test source list and build-form limits. |
| Stop Conditions | A dependency is inferred from only a name, a new generic compatibility layer is proposed, a native pointer enters historical 32-bit storage, or a source body/build target would be enabled before closure. |
| Exit Criteria | Reproducible source-form ledger covers all three library bodies and distinguishes all external interfaces from internal/CRT calls, with no unresolved owner. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Test versus library body, public Win32 versus NT RTL/PEB use, original source order, x86/x64 pointer representation, allocator callback ownership and stale pre-re-root include path. |

### Closed packet reference — M0 T276 S25

### M0 T276 S25 — final expected-integration closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T276 S25`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T276/S24 is closed: every 47-path VDD sample entry is final `not-host-runtime`, not an admitted VDD provider. Earlier S11–S23 package evidence was consolidated by S25. |
| Objective | Replace process-only dispositions for every previously audited package path with one evidence-backed final expected-integration disposition in the global 1,689-row tracker. **Closed.** |
| Non-goals | No source move/edit, adapter implementation, build edge, Ninja run or guest execution. Do not infer `direct` from a file being selected by a historical build. |
| Reference Baseline | T276/S1–S24 ledgers, package baselines, final-disposition taxonomy, source policy and architecture rules. |
| Files And ABI Surface | Global final tracker, package final-disposition evidence, Status and documentation inventory. No runtime ABI changes. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | All 1,689 paths have one taxonomy-valid, non-`unresolved` final disposition with owner/link, change, named-adapter, mapping and source-evidence fields. The closure verifier reruns every package gate. |
| Expected Markers | No previously-audited package remains represented only by a `provisional-not-enabled` record; each final tracker row is independently traceable to original/package evidence. |
| Asset Needs | T276/S11–S24 ledgers, [final-disposition taxonomy](etc/operations/m0-t276-final-integration-disposition-taxonomy-001.md), global tracker and [S25 plan](etc/operations/m0-t276-s25-final-disposition-backfill-plan-001.md). |
| Reporting Requirements | Exact per-package and global final-disposition counts, adapter/mapping consequences and next package admission criterion. |
| Stop Conditions | A path has no selected/build evidence, raw host pointers cross a boundary, a second mapping manager is proposed, a body is enabled from a trace, or classification requires invented adapter behavior. |
| Exit Criteria | Met: all selected paths have a non-`unresolved` final disposition; no path is left with only a current-enable state, and no source body is enabled. See [M0 T276 closure](history/m0-t276-closure-20260826.md). |
| Original Owner Request | “好的 开始审计任务”。 |
| Similar-Issue Sweep | Historical build selection versus final product treatment, mirror change class, adapter ownership, guest/firmware/tool exclusion, legacy private product shells, x86/x64 mapping and unresolved evidence. |

**S21 closure:** all 25 selected `vdmredir` paths have reproducible non-enabled
dispositions. Fifteen selected DLL provider bodies remain one owner package;
any recovery retains its protocol ordering and uses only session-owned handles,
`adapter-mvdm-host-out/win32` and checked mapping-manager copies at the named boundaries.
See the [S21 Redirector baseline](etc/operations/m0-t276-s21-vdmredir-package-disposition-baseline-001.md).

**S22 closure:** all 201 selected `wow32` paths have reproducible non-enabled
dispositions. Seventy-seven active C bodies are separated into eight original
provider subdomains, and nine source-proven interface families name their sole
adapter/session boundary and mapping-manager rule. `gdi32p`, `user32p`, VDD,
NT4 monitor and product-shell dependencies remain explicit historical
requirements; no WOW body is enabled. See the [S22 subdomain baseline](etc/operations/m0-t276-s22-wow32-subdomain-baseline-001.md) and [interface-family ledger](etc/operations/m0-t276-s22-wow32-interface-family-ledger.tsv).

**S23 closure:** all 21 selected `fax` paths have reproducible non-enabled
dispositions. The one historical GDI driver and four printer-UI DLL bodies
remain separate source products, with six explicit printer/WOW/shared-section
interface families. Their pointer-packed NT4 shared-section layout may not
cross x86/x64 directly: a future owner must use session mapping-manager
identity plus copied broker records or retain a source-derived failure. See the
[S23 fax baseline](etc/operations/m0-t276-s23-fax-disposition-baseline-001.md).

**S24 closure:** all 47 selected `vdd` paths are final `not-host-runtime`.
They are sample trees (including sample guest/host halves), not a VDD product
provider. The source mirrors remain evidence, while `adapter-mvdm-host-out/vdd` remains only
the named boundary for a future separately audited product package. See the
[S24 plan](etc/operations/m0-t276-s24-vdd-sample-package-disposition-plan-001.md)
and the global [final expected-integration tracker](etc/operations/m0-t276-final-integration-disposition-ledger.tsv).

**S25 P1:** all 140 W0/W1 paths now have final expected-integration
dispositions: two `adapter-backed` OEM library bodies, 105 `binding-only`
declaration/library carriers, 31 `tool-only` historical products and two
`not-host-runtime` build-control inputs. This is a final planning conclusion,
not a source enablement claim; see the [W0/W1 final baseline](etc/operations/m0-t276-s25-w0-w1-final-disposition-baseline-001.md).

**S25 P2:** 558 conclusively classified `softpc.new` paths now have final
dispositions: 489 Bochs-replaced executors and four profile/build-control
inputs are `not-host-runtime`, 58 ROM/BIOS/data inputs are `firmware-only`,
and seven historical utilities are `tool-only`. The host control, monitor,
declaration and app-composition subdomains remain deliberately unresolved;
see the [conclusive SoftPC baseline](etc/operations/m0-t276-s25-softpc-conclusive-final-disposition-baseline-001.md).

**S25 P3:** all 15 `xms.486` paths now have a final disposition: six selected
original provider bodies are `adapter-backed`, `xms.h` is `binding-only`, and
the direct-host-address/non-x86/build-evidence paths are `not-host-runtime`.
The session `guest_memory` mapping-manager instance is the only XMS address
resolution route; see the [XMS final baseline](etc/operations/m0-t276-s25-xms-final-disposition-baseline-001.md).

**S25 P4:** all 59 `dpmi/*` paths are `guest-only`: the original package is
one 16-bit `DOSX.EXE` guest product, never a host static library. Its future
machine and BOP contracts remain at the declared guest boundary; see the
[DPMI final baseline](etc/operations/m0-t276-s25-dpmi-final-disposition-baseline-001.md).

**S25 P5:** all 13 `sim16`/`sim32` paths now have final dispositions. The
private serial peer/product-shell bodies are `not-host-runtime`; `sim32.h`
and the SIM16 declaration forms are `binding-only` evidence for specialised
monitor/WOW adapters, never a second mapping manager or serial machine.
See the [SIM final baseline](etc/operations/m0-t276-s25-sim-final-disposition-baseline-001.md).

**S25 P6:** all 23 `ieuvddex/*` paths are `not-host-runtime`: they are the
historical NTSD/WinDbg extension product, not a VDD or app host provider. A
future same-shaped debugger feature has one `adapter-mvdm-host-out/debugger` boundary and
uses copied session diagnostic snapshots; see the
[IEUVDDEx final baseline](etc/operations/m0-t276-s25-ieuvddex-final-disposition-baseline-001.md).

**S25 P7:** all 35 `v86/*` paths now have final dispositions: old kernel VDM
monitor and standalone `ntvdm.exe` scaffold sources are `not-host-runtime`,
nine declaration forms are `binding-only`, and the independent quit utility
is `tool-only`. See the [V86 final baseline](etc/operations/m0-t276-s25-v86-final-disposition-baseline-001.md).

**S25 P8:** all 25 `vdmredir/*` paths now have final dispositions: 15 original
provider bodies are `adapter-backed`, six protocol declarations are
`binding-only`, and four DLL build/export inputs are `not-host-runtime`. The
package remains one original provider under `adapter-mvdm-host-out/redir`, not a file-system
shim; see the [Redirector final baseline](etc/operations/m0-t276-s25-vdmredir-final-disposition-baseline-001.md).

**S20 closure:** all 25 selected `dpmi32` paths have reproducible non-enabled
dispositions. Sixteen selected x86 bodies form one host-provider package; all
guest addresses are constrained to the unique session mapping-manager lease or
copy path, and two RISC bodies remain evidence only. See the [S20 DPMI32 baseline](etc/operations/m0-t276-s20-dpmi32-package-disposition-baseline-001.md).

**S19 closure:** all 59 selected `dpmi` paths have reproducible non-enabled
dispositions. The original build produces 16-bit `DOSX.EXE`; its 22 selected
provider bodies and all supporting source remain one guest product prerequisite,
while products and map/sym files remain guest artifacts. No DPMI source is a
host composition input. See the [S19 DPMI baseline](etc/operations/m0-t276-s19-dpmi-package-disposition-baseline-001.md).

**S18 closure:** all 23 selected `ieuvddex` paths have reproducible non-enabled
dispositions. The package is an NTSD/WinDbg debugger extension, not a VDD
runtime provider: all future recovery belongs to `adapter-mvdm-host-out/debugger` through a
copied session diagnostic snapshot. Six Alpha/MIPS/PPC paths remain source
evidence only. See the [S18 IEUVDDEx baseline](etc/operations/m0-t276-s18-ieuvddex-package-disposition-baseline-001.md).

**S17 closure:** all 35 selected `v86` paths have reproducible non-enabled
dispositions. Seven kernel-VDM monitor bodies retain same-shaped monitor API
recovery evidence but remain excluded; six standalone `ntvdm.exe` scaffold
bodies remain app-owned product-shell evidence; and the 16-bit quit utility
remains a tool-only input. Every future monitor guest address resolves through
the unique session mapping manager, never a raw host pointer or a second
manager. See the [S17 V86 baseline](etc/operations/m0-t276-s17-v86-package-disposition-baseline-001.md).

### M0 T276 S11 — W0/W1 file and interface disposition baseline

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T276 S11`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T276/S1–S10 establish exact source identity, boundary evidence, non-body ABI/guard evidence and six non-final package waves. No provider body or adapter ABI is enabled. |
| Objective | For W0/W1 (`dirs`, `makefil0`, `inc`, `oemuni`, `suballoc`, `vdmutils`), classify every selected file's original composition role and reached interface dependencies; identify direct, adapter-bound, minimal-binding-diff or profile-excluded candidates without enabling a build. |
| Non-goals | No provider/adapter implementation, source-body edit, enabled build edge, final recovery admission, BOP behavior, Bochs change or guest execution. |
| Reference Baseline | T276 S1/S2/S3/S4/S5/S10 ledgers, exact selected mirrors and source policy. |
| Files And ABI Surface | W0/W1 file/interface disposition ledger and evidence, Status and documentation inventory. No runtime ABI changes. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Account for every selected W0/W1 file and all reached source declarations/call candidates under those roots; preserve exact original build evidence; reject a file classification derived from path name alone. |
| Expected Markers | One provisional composition class and interface/dependency state per W0/W1 file, with any unresolved ABI explicitly linked to a named later review. No buildability is implied. |
| Asset Needs | T276 S1/S2/S3/S4/S5/S10 ledgers and exact W0/W1 source mirrors. |
| Reporting Requirements | Exact file and source/build-control counts, interface candidates, direct/profile-excluded candidates, unresolved ABI dependencies and W2 handoff. |
| Stop Conditions | A file cannot be tied to selected source/build evidence, a classification hides an unresolved external interface, or a direct classification requires an invented adapter. |
| Exit Criteria | Every W0/W1 selected path has a reproducible provisional composition/disposition record; unresolved interfaces are assigned to a later package/adapter review without changing mirror source. |
| Original Owner Request | “好的 开始审计任务”。 |
| Similar-Issue Sweep | Definition provenance, C/C++ versus assembly forms, unique declaration candidates, source-call aggregation, duplicate IDs and accidental provider/owner claims. |

**S1 closure:** the clean source topology initially contained thirteen
production roots and no premature production service body. The
[owner/source manifest](etc/operations/m0-t272-s1-production-owner-manifest-001.md)
records every root, its provenance/reuse class, intended build role and
owner packet; it also excludes `src.old/` and existing build outputs from
formal inputs.

**S2 closure:** the existing lifecycle reference was audited. The new
`session` component now has explicit caller-owned identity, instance-owned
cancellation, context-bearing LIFO teardown and bounded TLS bind/unbind;
global cancellation and COMMAND input payload were not carried forward. Its
focused lifecycle test passes under both MSVC x64 and Win32/x86 `/MT`.

**T272 closure:** the neutral session/mapping/lease/broker foundation is
complete and pushed through `d94d46ae`; its authoritative record is
[M0 T272 closure](history/m0-t272-closure-20260826.md).

**T274 closure:** the canonical platform ABI, local DOS/WOW16 guest carries,
and all selected MVDM host inputs are exact and hash-verified. This establishes
source supply only; later adapter/composition packets retain all behavior,
build and runtime claims. See [M0 T274 closure](history/m0-t274-closure-20260826.md).

**T276 S1 closure:** all 1,689 selected `mvdm-host` paths now have
reproducible source-to-mirror SHA-256 identity evidence, spanning exactly 23
package roots.  The file ledger records `byte-exact-verified` for every row;
composition and build-profile dispositions deliberately remain `unreviewed`.
See [T276 S1 file identity evidence](etc/operations/m0-t276-s1-file-identity-taxonomy-001.md).
The next S expands original build descriptions, translation units and include
edges; it does not recover interfaces or enable a package.

**T276 S2 closure:** 612 selected source translation units, 127 selected
original build-control inputs and 5,171 literal include edges are now recorded
without macro expansion or artificial build claims.  Of the source files, 487
are observed in 58 original NT `sources` descriptions; 125 are deliberately
recorded as not observed by this literal parser, pending build-description and
symbol analysis.  See [T276 S2 build-input evidence](etc/operations/m0-t276-s2-build-input-include-graph-001.md).
The next S expands reached declarations and external interfaces; it does not
enable a package merely because a textual include was observed.

**T276 S3 closure:** all 857 selected declaration files were scanned into
4,803 source-located lexical `extern` candidates, while the twelve historical
family debts remain explicitly unexpanded.  These are provenance records only:
they do not assert caller reachability, ABI availability, adapter ownership or
buildability.  See [T276 S3 declaration evidence](etc/operations/m0-t276-s3-interface-declaration-baseline-001.md).
The next S must resolve actual translation-unit callers against these candidates
and record ambiguities rather than guessing a package owner.

**T276 S4 closure:** every selected translation unit now has an explicit scan
state.  The case-sensitive C/C++ lexical scan records 89,924 call-or-definition candidates,
split into 3,697 unique-name, 6,920 ambiguous-name and 79,307 unrepresented
declaration correlations; 58 assembly units are explicitly not projected into
that model.  See [T276 S4 caller/callee evidence](etc/operations/m0-t276-s4-caller-callee-correlation-001.md).
S5 reviews only source-proven external boundaries and preserves unresolved
assembly/macro cases as evidence rather than inventing owners.

**T276 S5 closure:** all 3,697 S4 unique-name candidates now have a recorded
definition/boundary disposition: 1,429 have a selected-source definition-name
observation and 2,268 remain external-boundary candidates.  The 12,855
definition records include a comment-stripped K&R form and are deliberately
lexical; they do not make a linkage claim.
See [T276 S5 boundary evidence](etc/operations/m0-t276-s5-external-boundary-baseline-001.md).
S6 must review the 2,268 candidates by original declaration shape and caller
package, grouping real historical interface families before assigning owners.

**T276 S6 closure:** after K&R and function-pointer-aware parser corrections,
2,268 external-boundary calls reduce to 25 original declaration worklist rows:
23 SoftPC-shape and two debugger-shape review families. See
[T276 S6 worklist](etc/operations/m0-t276-s6-boundary-family-worklist-001.md).
The next S reviews each original declaration and its callers before any final
owner, adapter, availability or failure-contract entry is made.

**T276 S7 closure:** all 25 declaration worklist rows now have selected-source
occurrence navigation evidence. This evidence deliberately distinguishes no
source form yet: an occurrence may be a body, call, macro, callback/global or
comment-adjacent token. See [T276 S7 occurrence evidence](etc/operations/m0-t276-s7-provider-occurrence-baseline-001.md).
S8 performs that source-form review before any adapter or recovery conclusion.

**T276 S8 closure:** all 25 worklist rows now have original declaration and
provider-form evidence: 13 selected original body/dispatch forms, two callback
globals, one CRT import, three guarded no-provider historical options, and
six other no-provider forms. A case-sensitive regeneration removed the false
`CMDRebootVDM`/`cmdRebootVDM` correlation before this closure. See [T276 S8 provider-form evidence](etc/operations/m0-t276-s8-original-provider-form-review-001.md).
S9 records the source-derived ABI, guard and failure-contract evidence for the
non-body forms before a recovery wave is selected.

**T276 S9 closure:** all 12 non-body rows now have source-derived calling form,
availability/guard, result/failure evidence and package prerequisite. The five
CPU flag setters remain one CPU-status compatibility surface; the three guarded
optional historical paths remain profile questions; the debugger globals remain
callback bindings. See [T276 S9 ABI/failure evidence](etc/operations/m0-t276-s9-actionable-boundary-abi-failure-baseline-001.md).
S10 derives package waves from those records, not from a guest trace or compile
failure.

**T276 S10 closure:** all 23 selected roots now have an original role,
build/profile disposition, predecessor set and non-final candidate wave. The
six waves make the mixed `softpc.new` machine source and later product
extensions explicit rather than pretending they are one direct library. See
[T276 S10 package-wave evidence](etc/operations/m0-t276-s10-package-profile-wave-baseline-001.md).
S11 begins the required file/interface disposition from W0/W1 rather than
enabling a package on the first compile error.

**T276 S11 P1:** the six W0/W1 roots now have complete provisional file,
source-interface-family and lexical-call accounting: 140 selected paths, 14
source-interface families and 2,270 call candidates.  Only the exact original
`oemuni` library pair and `suballoc.c` remain runtime-support candidates;
all original utility products are explicit profile exclusions.  This remains
audit-only: no mirror source, provider body, adapter ABI or build edge is
enabled.  See [T276 S11 W0/W1 disposition evidence](etc/operations/m0-t276-s11-w0-w1-disposition-baseline-001.md).

**T276 S11 P2:** the three original W1 runtime-support translation units now
have an explicit binding handoff. `oemuni/file.c` and `process.c` retain their
original OEM/NT RTL algorithms pending same-shaped `adapter-mvdm-host-out/win32` bindings;
`suballoc.c` retains its `ULONG`/callback algorithm and defers only its
machine-facing callers to `adapter-mvdm-host-out/softpc`. No binding or source edit is
enabled. See the same S11 disposition evidence and runtime-support ledger.

**T276 S11 closure:** the six W0/W1 source-carrier/support/tool roots have a
complete provisional file owner/disposition record: 112 exact paths are
assigned to the future `mvdm-support` mirror and 28 to
`mvdm-tools`; all 2,270 lexical calls remain accounted without being
mistaken for enabled ABI. The only runtime-support candidates have exact
same-shaped binding handoffs. No source has moved or entered a build. S12 now
audits W2 `softpc.new` under the same source-first constraints.

**T276 S12 P1:** all 998 selected `softpc.new` paths now have a provisional
original-subpackage disposition. The baseline isolates 489 historical machine
execution/device paths as Bochs-replacement exclusions, 59 `host/src` mixed
control/device candidates, 156 `base/inc` machine declaration inputs, 58
firmware/ROM/data inputs assigned to the separate future `mvdm-softpc-firmware`
mirror, plus separate debugger/tool/Unix/composition inputs.
This is a boundary ledger only: no original machine/host source, adapter ABI
or build edge is enabled. See [T276 S12 SoftPC baseline](etc/operations/m0-t276-s12-softpc-disposition-baseline-001.md).

**T276 S12 P2:** the 56 original `softpc.new/host/src` source units are now
split by boundary rather than treated as one recoverable library: one BOP
dispatcher, ten SoftPC/SAS facade units, four monitor/reset/event units, one
session/worker unit, one cross-owner startup-composition unit, one
launch/configuration-composition unit, seven Win32 host-capability units, one
video/machine-composition unit, two VDD/debugger units, 18 device bridges and
ten literal-build-unobserved auxiliaries. No source or
adapter becomes enabled; the next slice audits complete owner groups at the
function/interface level. See [T276 S12 host/src baseline](etc/operations/m0-t276-s12-host-src-disposition-baseline-001.md).

**T276 S12 P3:** all 56 `host/src` source units now have call/definition/
boundary coverage: 7,079 lexical calls, 380 selected-definition observations
and 37 external-boundary candidates. This is a source-review ordering aid only,
not an ABI or reachability claim. Complete monitor/session, SoftPC facade and
Win32 host groups are the next audit order; unobserved auxiliaries remain
non-enabled. See [T276 S12 interface-density baseline](etc/operations/m0-t276-s12-host-src-interface-density-baseline-001.md).

**T276 S12 P4:** all 58 selected original `softpc.new/base/bios`, `bios`,
`roms` and `data` paths are now assigned to the future
`mvdm-softpc-firmware` mirror. The component is an immutable machine-input
carrier only: no source is compiled or linked as host runtime, no parallel
SoftPC machine is admitted, and only a future explicit `adapter-bochs`
manifest can select an input. The regenerated 998-row S12 ledger and verifier
record this owner boundary.

**T276 S12 P5:** the four monitor/reset sources, one session-worker source and
one cross-owner startup-composition source now have fifteen source-span-level
original-interface dispositions. `nt_msscs.c` is explicitly separated from
monitor ownership: it sequences COMMAND/DEM/XMS/debugger startup, guest-image
loading and VDD hooks, so it cannot be independently enabled. The ledger names
the applicable adapter/package predecessor for every group and records no
source or adapter enablement.

**T276 S12 P6:** all ten SoftPC/SAS facade sources now have twelve
source-span-level interface dispositions. `nt_cpu.c` CPU execution and direct
CPU-internal paths are explicit Bochs-replacement exclusions; SIM32/SAS,
memory, EMM and UMB paths retain their original contracts but require named
guest-memory, physical-page, VDD or session prerequisites. Historical build
stubs remain excluded rather than becoming default adapter bodies. No source
or adapter is enabled.

**T276 S12 P7:** the nine original host-capability files now have eleven
source-span-level Win32/NTDLL dispositions. `config.c` and `nt_sec.c` are
explicit cross-owner launch and video/machine composition prerequisites, not
generic Win32 helpers. The remaining diagnostic, NLS, keyboard, serial, PIF,
error and wait paths retain their original interface contracts with named
public-binding/session gates. No source or adapter is enabled; see the
[P7 Win32/NTDLL capability baseline](etc/operations/m0-t276-s12-win32-capability-interface-baseline-001.md).

**T276 S12 P8:** all eighteen mixed device-bridge source files now have
device-family dispositions. Historical CGA/EGA/VGA, FDC/DMA/IRQ, speaker/PPI
and timer/PIC bodies are explicit Bochs-replacement exclusions; presentation,
input, serial/printer, media and resources retain only named public endpoint
or product-owner prerequisites. No generic device adapter, source body or
build edge is enabled; see the [P8 device-bridge baseline](etc/operations/m0-t276-s12-device-bridge-interface-baseline-001.md).

**T276 S12 P9:** original `nt_bop.c`, `nt_vdd.c` and `nt_yoda.c` now have
twelve dispatcher/VDD/debugger interface dispositions. BOP selector meaning
remains in the OpenNT mirror, VDD requires the existing specialist adapter and
typed machine requests, and Yoda stays debugger-profile-gated. No source or
adapter is enabled; see the [P9 BOP/VDD/debugger baseline](etc/operations/m0-t276-s12-bop-vdd-debugger-interface-baseline-001.md).

**T276 S12 P10:** all ten literal-build-unobserved auxiliary sources now have
source-form dispositions. Alternative CCPU/A2CPU access and virtual-FDC bodies
are explicit Bochs-replacement exclusions; error data, historical stubs,
debug/settings UI, cleanup and configuration paths await their named original
build/profile and owner prerequisite. No source or adapter is enabled.

**T276 S12 P11:** the six detailed owner ledgers now cover all 56 original
`host/src` source paths exactly once. The combined gate reruns all family
checks, rejects omissions/duplicates and records the W3 package-wave handoff;
see the [host/src completion baseline](etc/operations/m0-t276-s12-host-src-completion-baseline-001.md).

**T276 S13 P1:** all 40 original `dos` paths now have one package-led
provisional disposition: 27 literal `SOURCES` translation units (16 DEM and
11 COMMAND), seven declarations, four build descriptions, one `dirs` metadata
file and one historical test. DEM and COMMAND remain intact source-owner
packages; no trace-selected BOP, source body or build edge is enabled. See the
[DOS package disposition baseline](etc/operations/m0-t276-s13-dos-package-disposition-baseline-001.md).

**T276 S13 P2:** the retained DOS source reaches eight named boundary families:
BOP transport, SoftPC/SAS guest state, public Win32/NTDLL capability,
session-owned state/identity, Redirector/pipe, VDD, OEM/NLS support and
BIOS-style raw media/IOCTL. Every family has an existing named owner or an
explicit Bochs-machine profile gate. The audit records later package order as
DEM foundation/provider first, then COMMAND on its declared session and
Redirector prerequisites; no provider is enabled.

**T276 S14 P1:** all 15 original `xms.486` paths have one non-enabled
disposition. The exact package contains six top-level literal XMS sources, one
separately selected i386 source, one profile-unselected RISC source, one public
header and six build descriptions. `xmsmem86.c` requires a same-shaped
mapping-manager replacement: its original three exported functions preserve
their contract but resolve 32-bit values to checked Bochs guest ranges.

**T276 S14 P2:** XMS reaches eight owner boundaries: XMS BOP dispatch, A20,
extended-memory/block move, UMB, INT 15, suballoc, historical direct virtual
memory and XMS shared state. A20/guest range/UMB/IVT remain
`adapter-mvdm-host-out/softpc -> adapter-bochs` prerequisites; UMB stays gated until a
durable bounded guest-byte seam is actually proved. No source body is enabled.

**T276 S15 P1:** all eight original `sim16` paths now have a non-enabled
disposition. The original makefile proves a 16-bit host-side WOW simulator DLL
using historical WOW libraries, not a Bochs-loadable guest image. Its two
assembler bodies remain `adapter-mvdm-host-out/wow` profile-gated: serial/PIC mechanics stay
with `adapter-bochs`, VDM-memory transport with `adapter-mvdm-host-out/monitor`, and no
generic protocol or device implementation is created.

**T276 S16 P1:** all five original `sim32` paths now have a non-enabled
disposition. Its original `sources` selects no translation unit: `sim32.c` is
a private serial-IOCTL/`sim16` WOW product shell, not the general SIM32
facade. The historical public `Sim32Get/Set/PSZ/Free/Send` API shape is retained
as recovery evidence for `adapter-mvdm-host-out/monitor`, but every guest address must
resolve through the one session mapping manager to checked Bochs RAM; no raw
pointer, serial product shell or second manager is admitted.

### M0 T275 S1 — Complete interface and reuse ledger

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T275 S1`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | The owner’s standing direction to implement the ordered queue as a globally planned, minimalist recovery program; T274 source supply is closed. |
| Objective | Freeze a complete original-caller/interface/ABI/failure/reuse disposition for BOP, SoftPC/CCPU/SAS, Win32/NTDLL, VDM-monitor and broker families before any adapter body is recovered. |
| Non-goals | No new adapter ABI, source-body import, provider enablement, Bochs change, guest change, broker IPC implementation or host build claim. |
| Reference Baseline | T274 exact MVDM mirror; adapter-recovery proposal; `src.old/adapter-*`, `app` and `session` as non-formal recovery references; BOP tracker and component owner manifest. |
| Files And ABI Surface | A reproducible family/caller/reuse ledger, candidate-file inventory, S1 evidence, Status and documentation inventory. No runtime ABI becomes active. |
| Applicable Rules | Execution, historical recovery, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Enumerate every old adapter candidate; identify its original caller/contract or disposition it as reference-only; scan the exact host mirror for family declarations/callers; prove no source copy or formal build input is enabled. |
| Expected Markers | One owner and one disposition for every candidate file and each reached family; exact source references, calling-convention/layout/failure requirements, reuse decision and successor S. |
| Asset Needs | T274 manifests, `mvdm-host`, `mvdm-platform-abi`, `src.old` reference tree, existing source-adapter proposal and tracker. |
| Reporting Requirements | Candidate and family counts, direct caller evidence, source-first rung decision, retained/rejected recovery candidates, unavailable historical dependencies, boundary and next-S handoff. |
| Stop Conditions | A candidate has no discoverable owner, a caller requires a behavior body before the ledger is complete, an interface crosses a prohibited Bochs boundary, or an ambiguous historical ABI cannot be resolved. |
| Exit Criteria | Every candidate and every reached interface family has a reproducible source/ABI/failure/reuse disposition; S2--S6 can each recover only the frozen family surface without reviving a parallel product shell. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | Existing adapter/app/session mechanics, BOP tracker dependencies, MVDM declarations/callers, source mirror package roots, old-path build leaks and Bochs direct-access risks. |

**S1 P1:** the reproducible recovery-candidate inventory contains 453 files:
61 implementation candidates, 385 declaration candidates and seven supporting
references. It is a source-reference baseline only, not permission to copy a
file or claim an adapter ABI. See the [P1 inventory note](etc/operations/m0-t275-s1-adapter-candidate-inventory-001.md).

**S1 P2:** the original-caller ledger now freezes 11 interface dispositions:
one BOP transport, five SoftPC/CCPU/SAS surfaces, three Win32/NTDLL surfaces,
one VDM-monitor surface and the minimal broker boundary. It explicitly retires
the old machine shell and project-only observation surfaces from adapter
recovery. See the [P2 interface ledger note](etc/operations/m0-t275-s1-historical-interface-ledger-001.md).

**S1 closure:** all 453 historical adapter candidates now have a unique,
reproducible disposition and all 61 implementation candidates are linked to
one of the 11 original-caller interface surfaces. The matrix has no duplicate
path, empty interface ID or empty disposition; the exact host mirror has zero
direct Bochs reference. S2 may establish only the missing-interface inventory
and its component boundaries; adapter-source recovery begins only after that
inventory closes. See the [S1 closure evidence](etc/operations/m0-t275-s1-adapter-reuse-ledger-closure-001.md).

### M0 T275 S2 — specialist adapter component inventory

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T275 S2`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | The owner requires original MVDM source to be imported before function-body alteration; every missing boundary must be assigned to a named adapter and recorded before source recovery. |
| Objective | Establish the complete missing-interface and specialist-adapter inventory: original caller/declaration/contract, unique owner adapter, modern recovery or original-style failure disposition, and successor ownership for every unavailable historical external interface. |
| Non-goals | No adapter source implementation, mirror-source modification, BOP selector/service decode, provider enablement, guest-memory implementation, Bochs change, guest execution or host build claim. |
| Reference Baseline | T275/S1 ledger; exact `mvdm-host` VDMREDIR, WOW32, VDD, debugger/monitor, SoftPC and host packages; current architecture/rules; old adapter references. |
| Files And ABI Surface | Adapter component inventory and architecture/proposal/rule updates only. No runtime ABI becomes active. |
| Applicable Rules | Execution, historical recovery, architecture, coding, mirror-component, source policy and build-hygiene rules. |
| Verification | Scan selected MVDM callers/declarations for unavailable external/product interface families; classify each as existing adapter, new specialist adapter or original internal owner; verify every enabled-source edit is only a source-recorded binding change and no generic catch-all adapter exists. |
| Expected Markers | One named owner adapter or explicit non-adapter owner per external family; each future symbol row records caller, declaration, ABI/layout, failure contract, adapter, binding edit and x86/x64 evidence; `adapter-mvdm-host-out/redir`, `adapter-mvdm-host-out/wow`, `adapter-mvdm-host-out/vdd` and `adapter-mvdm-host-out/debugger` each have original package evidence and directed-boundary rules. |
| Asset Needs | Exact host mirror, source-supply manifests, existing adapter ledger, current architecture/rules and tracker. |
| Reporting Requirements | State component additions, original callers/packages, retained interface shape, public-modern/API or machine dependency, explicit unavailable rule, successor S/T and why no generic adapter is introduced. |
| Stop Conditions | An interface has no original owner, requires a generic adapter, crosses prohibited Bochs/mirror boundaries, or needs source-body implementation before its component boundary is approved. |
| Exit Criteria | Architecture, coding/rule documents, proposal and inventory name every specialist adapter needed by the selected source universe; later implementation imports original MVDM bodies and uses the inventory rather than modifying mirrors to bypass a missing interface. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | VDMREDIR, WOW32, VDD, debugger/monitor, SoftPC, Win32, BOP, broker, existing adapter roots, mirror includes, package build descriptions and forbidden generic adapter roots. |

**S1 closure:** all 453 historical adapter candidates now have a unique,
reproducible disposition and all 61 implementation candidates are linked to
one of the 11 original-caller interface surfaces. The matrix has no duplicate
path, empty interface ID or empty disposition; the exact host mirror has zero
direct Bochs reference. S2 may establish only the missing-interface inventory
and its component boundaries; adapter-source recovery begins only after that
inventory closes. See the [S1 closure evidence](etc/operations/m0-t275-s1-adapter-reuse-ledger-closure-001.md).

**S2 closure:** nine adapter owner boundaries and the twelve-family
missing-interface ledger are recorded. The detailed package/symbol tracker is
the required successor; no adapter ABI or mirror source body was enabled. See
the [S2 inventory](etc/operations/m0-t275-s2-specialist-adapter-inventory-001.md).

**S1 P2:** the original-caller ledger now freezes 11 interface dispositions:
one BOP transport, five SoftPC/CCPU/SAS surfaces, three Win32/NTDLL surfaces,
one VDM-monitor surface and the minimal broker boundary. It explicitly retires
the old machine shell and project-only observation surfaces from adapter
recovery. See the [P2 interface ledger note](etc/operations/m0-t275-s1-historical-interface-ledger-001.md).

**S1 P1:** the reproducible recovery-candidate inventory contains 453 files:
61 implementation candidates, 385 declaration candidates and seven supporting
references. It is a source-reference baseline only, not permission to copy a
file or claim an adapter ABI. See the [P1 inventory note](etc/operations/m0-t275-s1-adapter-candidate-inventory-001.md).

### M0 T274 S6 — MVDM host package source recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S6`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S1 establishes a unique selected source for every host path; S2--S5 are closed. |
| Objective | Recover every S5-deferred, selected original MVDM host provider body and remaining package input under `mvdm-host`, retaining the selected source-relative path and hash. |
| Non-goals | No body modification, compilation, linking, provider enablement, adapter implementation, guest change, or replacement shim. |
| Reference Baseline | S1 union ledger; S5 topology manifest; pinned OpenNT/OpenNT-4.5 MVDM roots; source policy and T274 proposal. |
| Files And ABI Surface | Exact selected body/other host inputs, reproducible S6 manifest/evidence, component README, Status and documentation inventory. No enabled runtime ABI. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Copy only the S5-deferred rows with non-identical-conflict refusal; rehash every destination; prove all selected host paths are represented and guest roots/build inputs remain isolated. |
| Expected Markers | Complete 1,689-row selected host manifest, zero source/destination hash mismatches, no unregistered non-original host file and no guest build/link consumer. |
| Asset Needs | S1 TSV ledger, S5 manifest, both pinned external MVDM roots, `mvdm-host` root and component source policy. |
| Reporting Requirements | State imported/deferred counts, bytes/types/package roots, source selections, conflicts, exactness exceptions and host/guest isolation. |
| Stop Conditions | Ambiguous provenance, a non-identical destination conflict, a selected source unavailable locally, or an attempted behavior-changing edit. |
| Exit Criteria | Every S1-selected `mvdm-host` path is byte-identical to its selected source or explicitly registered as an allowed mirror exception; the source-supply task then closes without claiming compilation or runtime enablement. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | All S5 deferred bodies/non-topology inputs, package roots, source selections, destination conflicts, external reference leaks and host/guest boundaries. |

**S6 closure:** all 639 S5-deferred selected host inputs were directly
recovered and rehashed with zero mismatches: 612 C/C++/assembly provider bodies
and 27 other inputs. Together with S5, `mvdm-host` now contains all
1,689 selected paths across 23 original package roots, with no compilation,
linking or provider enablement claim. See the [S6 closure
evidence](etc/operations/m0-t274-s6-mvdm-host-package-supply-closure-001.md).

### M0 T274 S5 — MVDM host topology recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S5`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved canonical source-supply proposal; S1 selected union ledger and S2–S4 source-supply closures are complete. |
| Objective | Recover the selected original MVDM host header, resource and build-topology inputs under `mvdm-host` with one source/hash disposition per copied path. |
| Non-goals | No provider C/C++/assembly body import, compilation, link closure, adapter implementation, external source mix, guest relocation, or source-body modification. |
| Reference Baseline | S1 union ledger, pinned OpenNT/OpenNT-4.5 MVDM roots, source policy, T274 proposal and current component rules. |
| Files And ABI Surface | Byte-identical headers, declarations, resources and build-topology files in `mvdm-host`; topology manifest, component README, S5 evidence, Status and documentation inventory. No enabled runtime ABI. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Filter the selected host ledger by topology/header/resource class; copy with non-identical-conflict refusal; rehash every destination; verify package-relative paths and no provider body import. |
| Expected Markers | Complete selected topology manifest, zero source/destination hash mismatches, exact source-relative paths and explicit deferred provider-body count. |
| Asset Needs | S1 TSV ledger, both pinned external MVDM roots, `mvdm-host` root and component source policy. |
| Reporting Requirements | State file/byte/type/package counts, provenance selections, deferred body count, conflicts, exceptions and host/guest isolation. |
| Stop Conditions | A selected path has ambiguous provenance, a destination conflicts, a needed item is behavior-bearing provider body, or an import would create a parallel edition root. |
| Exit Criteria | Exact selected host topology/header/resource inputs are mirrored with a reproducible manifest and body import remains explicitly deferred to S6. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | All selected MVDM headers/includes, makefiles/sources/dirs/def/resource forms, package roots, OpenNT edition paths, existing host candidates and guest/host link boundaries. |

**S5 closure:** 1,050 selected topology/header/resource inputs across 23
original package roots were imported and rehashed without a mismatch. 612
provider-body and 27 non-topology rows remain explicit S6 inputs; zero C/C++/
assembly body files entered the S5 destination. See the [S5 closure
evidence](etc/operations/m0-t274-s5-mvdm-host-topology-closure-001.md).

### M0 T274 S4 — WOW16/bin86 direct guest recovery

### M0 T274 S4 — WOW16/bin86 direct guest recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S4`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner direction: directly copy existing guest files; do not compile, reimport external sources, or treat source-cardinality as a blocker. |
| Objective | Recover the existing local WOW16 and bin86 guest carry into `mvdm-guest/win16` and record the unchanged `build/output/wow16` products as immutable load-only inventory. |
| Non-goals | No compilation, external OpenNT/OpenNT-4.5 source recovery, source-completeness claim, host linking, BOP/provider change, or output relocation. |
| Reference Baseline | `src.old/opennt-guest/wow16`, `src.old/opennt-guest/bin86`, `build/output/wow16`, S1 ledger, source policy and S4 local-inventory disposition. |
| Files And ABI Surface | Byte-identical WOW16/bin86 carried files, source/output hash manifests, component README, S4 evidence, Status and documentation inventory. No host ABI. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Copy every local carried file with conflict refusal; rehash source/destination; hash the output tree in place; prove no output move/copy and no host build input. |
| Expected Markers | Two source manifests, one in-place output manifest, zero source/destination hash mismatches and explicit load-only ownership. |
| Asset Needs | Existing local WOW16/bin86 carry, existing output tree, S1 ledger and Git path inventory. |
| Reporting Requirements | State carried source/product counts and bytes, output location/hash inventory, exclusions, source-cardinality limitation and no-host-link result. |
| Stop Conditions | A local file conflicts with its destination, a command attempts output relocation, or a host manifest consumes a guest object/library. |
| Exit Criteria | Every current carried WOW16/bin86 file is byte-identical in its guest root, output products are manifested in place, and the source-completeness limitation is explicit without blocking load-only use. |
| Original Owner Request | “你只是把guest文件直接复制进来，难点是啥 又不用你编译 难点是啥？不是分分钟就做完了？” |
| Similar-Issue Sweep | WOW16/bin86 files, output binaries, source manifests, source/link consumers, external source references and guest-root placement. |

**S4 closure:** 44 WOW16 and 10 bin86 carried files were directly recovered
and rehashed with zero mismatches. The 87 products already in
`build/output/wow16` were hash-manifested in place without copy, move or build.
The 1,261 external-only source paths are explicitly not an S4 requirement. See
the [S4 closure evidence](etc/operations/m0-t274-s4-wow16-guest-recovery-closure-001.md).

### M0 T274 S3 — DOS guest mirror direct recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S3`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved canonical source-supply proposal and explicit direction to reuse the existing complete DOS guest rather than re-copy it from external OpenNT sources. |
| Objective | Recover the complete repository-local DOS/V86 guest mirror into `mvdm-guest/dos`, preserving every carried source/resource/build/intermediate/product input as an immutable guest-only inventory. |
| Non-goals | No external guest reimport, guest-source modification, guest compilation, host linking, BOP/provider enablement, output relocation, or WOW16 recovery. |
| Reference Baseline | `src.old/opennt-guest/dos-v86`, S1 selected union ledger, source policy, T274 source-supply proposal and current build-output isolation. |
| Files And ABI Surface | Byte-identical `mvdm-guest/dos` mirror paths, source hash manifest, component README, S3 evidence, Status and documentation inventory. No host ABI. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Enumerate and SHA-256 every carried source path; copy only into the matching guest root; prove source/destination equality, no host build manifest reference, and unchanged `build/output/dos`/`build/output/wow16` placement. |
| Expected Markers | One source-relative DOS/V86 inventory, zero source/destination hash mismatches, no divergence entries, and explicit guest-load-only ownership. |
| Asset Needs | Existing repository-local DOS/V86 mirror, S1 ledger, Git path inventory and prior build-output roots. |
| Reporting Requirements | Record files/bytes/types, source/destination hashes, excluded output paths, source provenance, any absent local ledger rows and no-host-link conclusion. |
| Stop Conditions | A carried path conflicts with a non-identical destination, a supposed guest item requires host linkage, an external source is needed to fill the local inventory, or a source provenance issue is found. |
| Exit Criteria | Complete byte-identical DOS/V86 mirror and reproducible manifest/evidence; `build/output/*` remains in situ and excluded from formal host inputs; S4 can repeat the same direct route for WOW16. |
| Original Owner Request | “opennt-guest里面的dos和wow16，其实可以直接复制到位，不需要从opennt opennt-4.5再次复制…build/output/dos, build/output/wow16里面的二进制…留着就行。” |
| Similar-Issue Sweep | DOS/V86 source/resource/build/intermediate/product files, guest DPMI forms, carried artifacts, manifest consumers, output paths, host source lists and external-guest references. |

**S3 closure:** all 676 local DOS/V86 inventory rows were copied into
`mvdm-guest/dos` and rehashed without a mismatch. The second recovery run
accepted all 676 destinations as already identical. `build/output/dos` and
`build/output/wow16` remain in place and are not host inputs. See the
[S3 closure evidence](etc/operations/m0-t274-s3-dos-guest-mirror-closure-001.md).

### M0 T274 S2 — Platform ABI mirror audit

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S2`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved canonical OpenNT source-supply proposal; S1 closed with the selected non-guest union ledger. |
| Objective | Establish the smallest exact-declaration platform ABI inventory and production root needed to compile later selected MVDM host packages without inventing replacement behavior. |
| Non-goals | No MVDM provider body import, host compilation, Win32/NTDLL replacement, adapter implementation, BOP execution, guest reimport, or guest-output relocation. |
| Reference Baseline | S1 selected union ledger, pinned OpenNT/OpenNT-4.5 trees, source policy, mirror rules and T274 source-supply proposal. |
| Files And ABI Surface | `mvdm-platform-abi` README, exact declaration/disposition ledger, optional byte-identical upstream declarations, S2 evidence, Status and documentation inventory. No behavior-bearing source file. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Scan the selected `mvdm-host` universe for direct include/declaration needs; distinguish MVDM-local, upstream-platform, public SDK and unresolved rows; hash every imported declaration; run governance and scoped diff review. |
| Expected Markers | Every direct external include/declaration requirement has a unique disposition, every material file is byte-identical upstream and no platform behavior is linked or authored. |
| Asset Needs | S1 TSV ledger, both pinned external source trees, source policy and existing component ownership manifest. |
| Reporting Requirements | State requirement cardinalities, direct versus transitive boundary, selected source path/hash, exclusions, unresolved API/declaration risks and future package consumers. |
| Stop Conditions | A required item needs a behavior body, an ambiguous cross-generation header mix, a proprietary SDK import without source-policy approval, or an ABI source cannot be identified. |
| Exit Criteria | A reproducible complete direct-platform ABI disposition ledger, an audited minimal `mvdm-platform-abi` root and no unregistered declaration import; S5/S6 can consume it without reopening the S1 union decision. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务…追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | All selected MVDM header/include forms, platform declarations, `windows.h`/NTDLL families, internal MVDM headers, source/build descriptions, pre-existing shims and architecture-specific forms. |

**S2 closure:** the reproducible direct-include ledger covers all 637 include
forms found in 1,467 selected MVDM code inputs.  The minimal
`mvdm-platform-abi` mirror contains 22 uniquely resolved, byte-identical
original declaration headers; its import manifest reports zero SHA-256
mismatches on an idempotent second import.  CRT/SDK forms remain toolchain
inputs, while same-name and generated historical forms remain explicit owner
package rows.  See the [S2 closure evidence](etc/operations/m0-t274-s2-platform-abi-mirror-closure-001.md).

### M0 T274 S1 — Non-guest package-scope union ledger

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S1`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved canonical OpenNT source-supply proposal; T273 closed after its S4 boundary audit. |
| Objective | Create the authoritative package-scope path/provenance/hash/disposition ledger for the non-guest OpenNT/OpenNT-4.5 MVDM union. |
| Non-goals | No guest reimport, provider enablement, dormant-body compilation, interface replacement, BOP execution, or host composition. |
| Reference Baseline | `O:\repos.external\OpenNT\base\mvdm`, `O:\repos.external\OpenNT-4.5\nt\private\mvdm`, source policy, T272 owner manifest and source-supply proposal. |
| Files And ABI Surface | Source-supply ledger/evidence, proposal/status/README provenance records and documentation inventory only. No production source body is changed in S1. |
| Applicable Rules | Execution, source policy, mirror-component, architecture, coding, document and build-hygiene rules. |
| Verification | Enumerate both trees; hash matching relative paths; classify every one-sided/conflicting file by owning package/build/resource/artifact lineage; governance and scoped diff review. |
| Expected Markers | One selected source or explicit rejection for each target-relative non-guest path, variant hash evidence, package owner and next-subtask disposition. |
| Asset Needs | Both pinned external source trees, existing guest carry-forward inventories and the repository-local source policy. |
| Reporting Requirements | State cardinalities, conflict rules, selected/rejected variants, unresolved package ambiguities and host/guest isolation. |
| Stop Conditions | A path cannot be assigned to a complete owner package, provenance is ambiguous, or a selection would silently mix incompatible build generations. |
| Exit Criteria | Complete non-guest union ledger with every target path classified; S2 can import exact platform ABI declarations without reopening selection decisions. |
| Original Owner Request | “opennt-guest里面的dos和wow16，其实可以直接复制到位…已有的app, session, bochs-core和adapter-*组件，可作为参考来源…” |
| Similar-Issue Sweep | All MVDM package roots, headers, build files, resources, binaries, tools, duplicate variants, guest/host boundaries and obsolete current roots. |

**S1 closure:** the reproducible 3,671-path union ledger gives every non-guest
host path a unique source/owner disposition. It retains guest/tree evidence
without reimporting it: 688 current carried guest paths match exactly, while
1,261 external WOW paths are explicitly outside the current carried inventory
and cannot become implicit source inputs. See the
[S1 ledger closure](etc/operations/m0-t274-s1-non-guest-mvdm-union-ledger-closure-001.md).

### M0 T273 S4 closure — Boundary and architecture verification

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T273 S4`, closed; Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved Bochs foundation proposal; S3 closed in `f364eccf`; S4 is its final declared verification subtask. |
| Objective | Prove the recovered Bochs foundation has the required directed dependencies, mirror/overlay privacy and x86/x64 focused lifecycle closure. |
| Non-goals | No guest execution, firmware product boot, BOP provider, OpenNT service, SoftPC API recovery, feature enablement, or new Bochs semantic change. |
| Reference Baseline | T273 S1 audit, S2 P2--P4 evidence, S3 closure, current architecture/rules and the pinned Bochs 2.6 baseline. |
| Files And ABI Surface | Source/include/build manifests, component READMEs, boundary tests, status/evidence/history/queue only. No new consumer ABI is admitted. |
| Applicable Rules | Execution, architecture, coding, mirror-component, build-hygiene, documentation and source-policy rules. |
| Verification | Full production include/reference scan, mirror-difference/register reconciliation, overlay reachability audit, focused x64/x86 rerun, governance and scoped diff review. |
| Expected Markers | `adapter-bochs` is the sole `bochs-core` consumer; no project dependency from `bochs-core`; overlay only supports its mirror; app only sees opaque adapter API. |
| Asset Needs | Recovered roots, registered README/evidence records, disposable P3/P4 x64/x86 roots, MSVC toolchains and governance scripts. |
| Reporting Requirements | State command inputs/results, exact remaining registered Bochs differences, negative dependency findings and the boundary of later source-supply work. |
| Stop Conditions | A prohibited consumer, unregistered mirror deviation, overlay external caller or host-width failure is found; it must be separately admitted rather than concealed by S4. |
| Exit Criteria | Both host-width fixtures and all directed-boundary/mirror checks pass, or every found violation is explicitly dispositioned; T273 closure then records only proven minimal-machine facts. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | All production include edges, Bochs/adapter vocabulary, old-root build inputs, overlay external consumers, CPU configuration, host-width assumptions and fixture source lists. |

**T273 closure:** the minimal CPU5/Pentium-MMX Bochs construction/reset/RAM/
port-space/timer-stop/resume foundation is proven under x64 and Win32/x86;
`adapter-bochs` is its only external caller and `bochs-core` has no project
dependency. The closure does not claim guest execution. See the
[S4 audit](etc/operations/m0-t273-s4-bochs-boundary-architecture-audit-001.md)
and [T273 closure](history/m0-t273-closure-20260826.md).

### M0 T273 S3 closure — Minimal app machine shell

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T273 S3`, closed; Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved Bochs foundation proposal; S2 is closed in `9645cdd9`, and the proposal’s next bounded subtask is admitted. |
| Objective | Recover the smallest existing app/session composition that can create and deterministically destroy an opaque `adapter-bochs` machine endpoint. |
| Non-goals | No guest image loading, BOP decoding, OpenNT/DOS/WOW/SoftPC/CCPU behavior, CLI feature surface, Win32 host service, or independent machine scheduler. |
| Reference Baseline | T272 neutral session closure; T273/S2 P4 lifecycle closure; quarantined `src.old/app` and `src.old/session` candidates; current component architecture. |
| Files And ABI Surface | Audited `app`/`session` source candidates, their minimal opaque composition API, one focused fixture, status/evidence/README records. No Bochs object type may enter either public surface. |
| Applicable Rules | Execution, architecture, coding, source policy, component mirror, build-hygiene and documentation rules. |
| Verification | Per-file owner/provenance audit, dependency/vocabulary scan, focused x64/x86 fixture, `git diff --check` and documentation governance. |
| Expected Markers | A caller-owned opaque lifecycle handle, deterministic create/destroy ordering, no `bochs-core` include outside `adapter-bochs`, and explicit rejected candidates. |
| Asset Needs | Existing T272 session sources, audited app candidates, P4 mechanical API, MSVC x64/x86 and disposable T273 build roots. |
| Reporting Requirements | Identify reused/copied candidates, excluded application/service semantics, machine ownership order, host-width result and remaining S4 boundary checks. |
| Stop Conditions | Candidate requires guest/provider semantics, an app/session public ABI exposes a Bochs type, or a needed ownership source lacks provenance. |
| Exit Criteria | A minimal app shell creates/destroys an opaque machine endpoint under both host widths; no app/session file adds Bochs/OpenNT service meaning beyond this assembly boundary. |
| Original Owner Request | “已有的app, session, bochs-core和adapter-*组件，可作为参考来源，在符合大局观的前提下参考实现、或复制进入，加以利用，来节省我们的大量时间。” |
| Similar-Issue Sweep | All current and quarantined app/session lifecycle candidates, direct Bochs includes, cancellation/worker/CLI/service vocabulary and test ownership. |

**S3 closure:** current neutral `session` is retained as the previously
audited replacement; the old app/session composition candidates are rejected
because they carry CLI/BOP/guest/host-service semantics. The new minimal app
shell owns only opaque machine open/teardown and passes focused x64/x86 tests.
See [S3 shell record](etc/operations/m0-t273-s3-minimal-app-machine-shell-closure-001.md).

### M0 T273 S2 closure — Mechanical lifecycle

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T273 S2`, closed; Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved Bochs foundation proposal; S1 provenance and mirror audit is closed in `a4ad30a0`. |
| Objective | Recover the smallest Bochs-only construction, reset, default-port-space, finite-run, stop and resume lifecycle through `adapter-bochs`. |
| Scope | Reviewed exact/subset Bochs files, registered private overlay boundaries, audited `adapter-bochs` lifecycle candidates, focused construction/reset/budget fixtures and evidence. |
| Non-goals | No app shell, guest loading, BOP decoding, OpenNT/DOS/WOW/SoftPC/CCPU provider semantics, Win32 service behavior or full Bochs product-shell build. |
| Verification | Per-file source/reuse audit, adapter vocabulary/dependency scan, focused lifecycle fixture under x64/x86, mirror exception review, governance and `git diff --check`. |
| Exit Criteria | An opaque minimal machine is constructed/reset/run for a finite budget/stopped/resumed by `adapter-bochs`; `bochs-core` stays project-dependency-free and all copied/modified source meets mirror rules. |
| Reference Baseline | T273 S1 recovery audit, pinned Bochs 2.6 baseline, quarantined mechanical candidates and prior Bochs exception evidence. |
| Files And ABI Surface | Reviewed `bochs-core`/private overlay files and `adapter-bochs` lifecycle API only; no caller outside the adapter receives a Bochs type. |
| Applicable Rules | Execution, source policy, mirror-component, architecture, coding, build-hygiene and documentation rules. |
| Expected Markers | Selected source manifest, private overlay isolation, one adapter-owned opaque lifecycle API and x86/x64 focused evidence. |
| Asset Needs | Pinned Bochs source, audited candidate files, CPU5 configuration evidence, MSVC x64/x86 and disposable S2 build roots. |
| Reporting Requirements | Identify every copied source, exception/overlay link, build closure and intentionally excluded Bochs product-shell feature. |
| Stop Conditions | Lifecycle needs an unregistered Bochs semantic intrusion, imports non-Bochs vocabulary, or cannot form a defined minimal object/link closure. |
| Similar-Issue Sweep | SIM/logging/PC-time/CPU decode/RAM/default-port-space/reset/run lifecycle, old adapter candidate dependencies and all core include edges. |
| Original Owner Request | Rebootstrap from zero using existing app/session/Bochs/adapter files only after global provenance and owner review, while retaining a minimal source-audited Bochs machine boundary. |

**S2 P2 closure:** the recovered generated Bochs configuration is now the
owner-approved CPU5/Pentium-MMX, non-x86-64-guest profile and selects native
pointer width correctly for each host architecture. Focused x64/x86 `/MT`
compilation passes; no minimal-machine link or runtime result is claimed. See
[S2 P2 configuration record](etc/operations/m0-t273-s2-p2-cpu5-host-width-config-closure-001.md).

**S2 P3 closure:** the Bochs-only construction/reset/decode/RAM/empty-port
space/timer-stop object closure now links and runs on both x64 and x86. It
contains no `iodev/devices.cc`, product plugin, GUI, guest or OpenNT input.
The proven bounded operation destroys its machine after the budget result, so
resume remains the next explicit S2 boundary. See [S2 P3 link record](etc/operations/m0-t273-s2-p3-minimal-machine-link-closure-001.md).

**S2 P4 closure:** a retained opaque machine can now complete two native
finite-budget runs without reconstruction, then destroy cleanly, under both
MSVC x64 and Win32/x86 `/MT`. The old broad runtime stage was audited but not
copied; only its selector-blind construction/resume skeleton is admitted. See
[S2 P4 lifecycle record](etc/operations/m0-t273-s2-p4-resumable-machine-lifecycle-closure-001.md).

The latest closed packet remains the technical baseline below.

### M0 T271 closure — OpenNT BOP source-recovery and mirror-boundary audit

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T271`, closed; all S1--S5 work was executed in Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T271 plan; final owner direction: revise the exit criterion and close the task at the audited, committed boundary. |
| Objective | Recover the admitted DEM, COMMAND and XMS source-owner packages; audit the complete BOP root and isolate the removable non-mirror bodies, while producing an explicit disposition for residual cross-component work. |
| Non-goals | No new BOP family, no DPMI/WOW/Redirector/VDD/debugger recovery, no new Bochs semantics, no trace-led leaf handler, and no unadmitted replacement of the remaining cross-component interfaces. |
| Scope | `opennt-bop`, `opennt-bop-overlay`, private include boundaries, source provenance, manifests, focused family fixtures, README divergence registers, tracker/evidence and the documented residual-owner disposition. |
| Verification | Per-file source classification and line-ledger; production external-consumer scan; focused DEM/COMMAND/XMS matrices; formal Ninja closure for committed P deliveries; governance and `git diff --check`. |
| Exit Criteria | S1--S4 source-owner recovery is committed; S5 has audited all residual non-mirror bodies/headers, moved every safely private DEM body/header, and recorded the target owner plus a future admission path for every residual. This closure does **not** claim that all residual work is completed. |
| Reference Baseline | T271 proposal; S1--S4 closures; pinned OpenNT sources; current BOP tracker and all component divergence registers. |
| Files And ABI Surface | Source layout/provenance only. Existing public selector-blind ingress and provider ABI are not expanded. |
| Applicable Rules | Source policy, source-first ladder, mirror-component standard, adapter/session boundaries, production-only roots, build hygiene and documentation governance. |
| Expected Markers | Every reviewed non-mirror input has a local marker/register row and target-owner disposition; the eliminated `ingress` tree and moved DEM-private surfaces have no production consumer. |
| Asset Needs | Pinned local OpenNT source tree, S1--S4 closures, component manifest, current README registers, fixtures and formal Ninja graph. |
| Reporting Requirements | Identify every remaining non-original file/body, its target owner, source/evidence baseline, disposition and focused verification; transfer remaining work without a false closure claim. |
| Stop Conditions | A live body lacks source provenance, a move would change a provider failure contract, or a move crosses an unadmitted owner boundary. These became residual/debt records rather than implicit follow-on work. |
| Similar-Issue Sweep | All BOP family roots, ingress, source-derived bridges, composition/session files, private overlays, manifests, tests and READMEs. |
| Original Owner Request | “opennt-bop 应当符合镜像审计标准；附属 overlay 应当极小或消失。” Final direction: “将当前T任务修改退出条件，直接收口。” |

**T271 result:** S1 non-mirror composition extraction, S2 DEM original package
recovery, S3 COMMAND original package recovery, S4 XMS original package
recovery and S5 whole-component audit/disposition are closed. The authoritative
closure record is [M0 T271 closure](history/m0-t271-closure-20260825.md).

**S3 progress:** P1 restores the reached OpenNT `MS_bop_4` entry, `cmddata.c`
globals and `CMDInit` as true source subsets. The native fixture writes an
actual `C4 C4 54 00` instruction to checked RAM and proves source entry into
the original 17-slot `CmdDispatch` table. Remaining COMMAND provider/session
composition recovery is still active; see [S3 P1 evidence](etc/evidence/m0-t271-s3-p1-command-entry-globals-recovery-001.md).

**S3 P2:** same-shaped OEM environment/drive and system-directory facades
now belong to `adapter-mvdm-host-out/win32`, not the OpenNT BOP mirror; focused direct and
native COMMAND fixtures pass. See [S3 P2 evidence](etc/evidence/m0-t271-s3-p2-command-win32-facade-extraction-001.md).

**S3 P3:** `host_lpt_flush_initialize` now belongs to selector-blind
`adapter-mvdm-host-out/softpc`, preserving OpenNT `nt_lpt.c`'s reached three-port state
clear without leaving a COMMAND composition implementation. The remaining
historical host API owner map, including the required future `adapter-mvdm-host-out/win32`
VDM API migration, is recorded in [S3 P3 evidence](etc/evidence/m0-t271-s3-command-host-api-owner-map-001.md).

**S3 P4:** the remaining temporary COMMAND composition now has a complete
per-block final-owner disposition. The next implementation order is neutral
session payload ownership, same-shaped VDM API recovery in `adapter-mvdm-host-out/win32`,
reached OpenNT host subsets, host-child lifecycle, then the generic frame
transaction. See [S3 P4 disposition](etc/evidence/m0-t271-s3-p4-command-composition-owner-disposition-001.md).

**S3 P5:** copied startup/environment/directory payload lifetime is now
neutral `session_input` state, not `opennt-bop` composition fields. Focused
get-next, dynamic-environment and native-entry regressions pass; the next P
moves the historical VDM API definitions themselves into `adapter-mvdm-host-out/win32`.
See [S3 P5 evidence](etc/evidence/m0-t271-s3-p5-session-input-extraction-001.md).

**S3 P6:** the reached historical `GetNextVDMCommand` and
`SetVDMCurrentDirectories` declarations and implementations now belong to
`adapter-mvdm-host-out/win32` as a same-shaped VDM API facade.  It preserves `VDMINFO`,
the re-entry state transitions and original caller/worker order while binding
only neutral copied `session_input` state; it does not claim the unavailable
NT4 BaseSrv/CSR broker.  Formal direct, dynamic-environment, native-session
and local-child regressions pass.  See [S3 P6 evidence](etc/evidence/m0-t271-s3-p6-vdm-api-facade-recovery-001.md).

**S3 P7:** reached original host entries `nt_std_handle_notification`,
`nt_block_event_thread`, `nt_resume_event_thread` and
`cmdPushExitInConsoleBuffer` now come from source-shaped `nt_msscs.c` /
`nt_event.c` subsets in `opennt-host`, rather than COMMAND composition.
Their original state ordering is active; VDD/display/BIOS/console-process
product-shell tails are explicitly deferred and never reported as success.
See [S3 P7 evidence](etc/evidence/m0-t271-s3-p7-command-host-event-recovery-001.md).

**S3 P8a:** the modern child record and pure host capability portions of the
reached `cmdexec.c` path now belong to `opennt-host`: stream endpoint lookup,
ANSI `CreateProcessA`, Job ownership, completion signaling and result
recording.  COMMAND retains its source call ordering and the BOP-facing
worker/re-entry seam.  The direct lifecycle fixture passes; the native
machine fixture currently stops before reaching PENDING and is separately
classified as a machine/dispatch prerequisite.  See [S3 P8a evidence](etc/evidence/m0-t271-s3-p8a-command-child-host-recovery-001.md).

**S3 P8b:** worker/event creation, completion polling/release and
dispose/cancellation now also belong to the same `opennt-host` child
capability. COMMAND retains only the original-body callback and scoped
session-input bind required for `cmdCreateProcess`; it no longer directly
creates or waits on host threads/events. See [S3 P8b evidence](etc/evidence/m0-t271-s3-p8b-command-child-worker-recovery-001.md).

**S3 P9:** the generic copied CPU-frame / checked-RAM / typed-outcome
transaction has moved from COMMAND ingress to selector-blind `adapter-mvdm-host-in`.
COMMAND retains only its original entry envelope, staged service byte and
`MS_bop_4` call. Focused transaction, native COMMAND-entry and local-child
regressions pass after a formal Ninja relink; the remaining source-shaped
COMMAND composition blocks are still active S3 work. See [S3 P9 evidence](etc/evidence/m0-t271-s3-p9-selector-blind-frame-transaction-001.md).

**S3 P10:** the last pure COMMAND Win32 helpers have moved to the existing
same-shaped `adapter-mvdm-host-out/win32` facade: hidden drive-directory environment lookup
and the unavailable private console keyboard-layout export; the existing
error-dialog facade now also exports source-shaped `RcErrorDialogBox` directly.
Console/keyboard, dynamic-environment and `cmdconf` terminal-failure
regressions pass after the formal relink. The remaining CCPU/SAS,
copied-address, worker callback, Redirector and terminal blocks remain
distinct active S3 owner work. See [S3 P10 evidence](etc/evidence/m0-t271-s3-p10-command-win32-facade-completion-001.md).

**S3 P11:** source-shaped `nt_init_event_thread` is now the reached
`opennt-host/nt_event.c` subset, which records only the admitted one-session
event initialization request. COMMAND no longer owns host-console state;
the console/keyboard regression passes after the formal relink. VDD/display/
BIOS/window/thread product-shell tails remain the existing explicit
`HOST-DIV-030` deferral. See [S3 P11 evidence](etc/evidence/m0-t271-s3-p11-command-host-event-initialization-001.md).

**S3 P12:** generic copied CCPU register access, typed result edits and
single-byte `sas_load` now belong to the selector-blind
`adapter-mvdm-host-out/softpc` frame context, not COMMAND composition.  Original source
call shapes remain private macros; COMMAND-specific `GetVDMAddr` allocation
and service ordering remain under the active source-body review.  Focused
CCPU context, direct COMMAND import and native COMMAND-entry regressions pass
after the formal relink. See [S3 P12 evidence](etc/evidence/m0-t271-s3-p12-ccpu-frame-context-extraction-001.md).

**S3 P13:** owner audit confirms that the original `cmdCreateProcess` worker
callback and `GetVDMAddr` pointer/writeback order must remain source-specific
COMMAND glue, while generic host child mechanics remain in `opennt-host`.
The last generic terminal operation, typed controlled-stop, now belongs to
the selector-blind CCPU frame context; only the original non-returning
COMMAND convention remains locally. Redirector and WOW-specific tails retain
their named future owner packages. See [S3 P13 evidence](etc/evidence/m0-t271-s3-p13-command-residual-owner-audit-001.md).

**S3 P14:** `cmdExec32` is now a registered private same-signature overlay,
not a large hidden rewrite inside the `cmdexec.c` mirror.  The mirror retains
the original callers and a small explicit crop boundary; lifecycle, child and
native-entry regressions pass after formal Ninja relink. See [S3 P14 evidence](etc/evidence/m0-t271-s3-p14-command-exec-overlay-isolation-001.md).

**S3 P15:** the generic checked guest multisz-copy loop has moved from
COMMAND composition to the selector-blind CCPU frame facade.  COMMAND retains
only the source-specific `GetVDMAddr` allocation/writeback order; CCPU and
COMMAND lifecycle regressions pass after formal Ninja relink. See [S3 P15 evidence](etc/evidence/m0-t271-s3-p15-ccpu-multisz-extraction-001.md).

**S3 P16:** the remaining project-authored COMMAND composition body is now a
mirror-private overlay behind one registered linkage boundary; no other
component consumes it.  Formal lifecycle, child and native-entry regressions
remain green. See [S3 P16 evidence](etc/evidence/m0-t271-s3-p16-command-composition-overlay-isolation-001.md).

**S3 P17:** binary, config and environment helper bodies are now registered
mirror-private overlays, leaving one-linkage boundaries in the COMMAND mirror.
Config, dynamic-environment and lifecycle regressions remain green.  The PIF
parser is separately audited by P18. See [S3 P17 evidence](etc/evidence/m0-t271-s3-p17-command-helper-overlay-isolation-001.md).

**S3 P18:** the exact OpenNT PIF parser remains in the COMMAND owner-family
mirror because it shares global state with `cmdpif.c` while the two historical
`PIF_DATA` layouts are incompatible.  Its project-authored binding is now a
registered private overlay behind the sole mirror boundary; no invalid
`opennt-host` to COMMAND dependency was introduced.  The formal PIF parser
fixture passes. See [S3 P18 evidence](etc/evidence/m0-t271-s3-p18-command-pif-owner-and-overlay-isolation-001.md).

**S3 P19:** app-only COMMAND startup/session binding has left
`opennt-bop/ingress` for `app/command_session_binding`; it now has app-owned
names and is compiled into the app library.  Original COMMAND source entry,
table and provider ownership remain unchanged. Formal native-session, PIF and
final-app links pass. See [S3 P19 evidence](etc/evidence/m0-t271-s3-p19-command-app-session-binding-extraction-001.md).

**S3 P20 / closure:** the full formal COMMAND source-provider matrix passes.
Fixture-only dialog isolation and opaque callback binding were corrected
without altering product dialogs or BOP routing. The imported `MS_bop_4` and
17-slot COMMAND table are the active dispatcher path; remaining cross-owner
capabilities are explicitly transferred. See [S3 P20 evidence](etc/evidence/m0-t271-s3-p20-command-final-matrix-and-fixture-composition-001.md)
and [S3 closure](history/m0-t271-s3-closure-20260825.md).

**S4 P1:** XMS package configuration/call composition is now a registered
private overlay behind the sole `xms/opennt_xms_composition.c` mirror boundary.
The original 12-slot XMS table and provider bodies are unchanged; formal source
and A20 fixtures pass. See [S4 P1 evidence](etc/evidence/m0-t271-s4-p1-xms-composition-overlay-isolation-001.md).

**S4 P2:** the reached `xmsa20.c` / `xmsumb.c` HIMEM state path now uses the
sole session `guest_memory` mapping-manager instance with bounded leases;
no native guest pointer survives the source call. Both formal XMS fixtures
remain green. See [S4 P2 evidence](etc/evidence/m0-t271-s4-p2-xms-a20-mapping-manager-migration-001.md).

**S4 P3:** source-unit audit classifies `xmsmemr.c` as the non-i386 RISC
variant and `i386/xmsmem86.c` as an NT4-private host-pointer implementation;
the existing same-shaped three-entry SoftPC facade is retained instead of
importing either. See [S4 P3 audit](etc/evidence/m0-t271-s4-p3-xms-source-unit-admission-audit-001.md).

**S4 P4:** non-original XMS package bind/reset lifecycle now belongs to
`app/xms_session_binding`, not `opennt-bop/ingress`; formal XMS fixtures pass
with it linked from `app.lib`. See [S4 P4 evidence](etc/evidence/m0-t271-s4-p4-xms-app-session-binding-extraction-001.md).

**S5 P1:** complete production classification finds 22 remaining project
`ingress` sources. They are now individually grouped by source owner; none is
silently moved into selector-blind `adapter-mvdm-host-in`. See [S5 P1 classification](etc/evidence/m0-t271-s5-p1-opennt-bop-production-classification-001.md).

**S5 P2:** the 13 exceptional overlay bodies each have exactly one matching
`opennt-bop` mirror include boundary; no production component outside that
mirror root includes the private overlay.  This is structural isolation only:
the 22 non-mirror ingress inputs remain active owner-disposition work.  See
[S5 P2 audit](etc/evidence/m0-t271-s5-p2-overlay-consumer-and-boundary-audit-001.md).

**S5 P3:** DEM, COMMAND and XMS one-session entry/context composition now
belongs to `app/bop/`, leaving `adapter-mvdm-host-in` selector-blind and the imported
provider tables unchanged.  Formal Ninja build plus the focused source-path
fixtures pass; the non-mirror ingress inventory drops from 22 to 13 named
deferred owner inputs.  See [S5 P3 extraction](etc/evidence/m0-t271-s5-p3-app-composition-extraction-001.md).

**S5 P4:** the `0x5E` fixed-frame wrapper for the already mirrored OpenNT
`MS_bop_E` is also app composition, not a mirror body.  It now lives in
`app/bop/`; the focused formal fixture passes and 12 non-mirror ingress bodies
remain for owner-package disposition.  See [S5 P4 extraction](etc/evidence/m0-t271-s5-p4-config-complete-app-entry-extraction-001.md).

**S5 P5:** all residual SoftPC/Redirector/top-level fixed-frame and session
wrappers now also belong to `app/bop/`; the test-only selector map moved to
tests, and `opennt-bop/ingress` is gone. Nine focused residual fixtures pass.
The final mirror/compatibility/overlay ledger audit remains active; this is not
yet S5 closure. See [S5 P5 extraction](etc/evidence/m0-t271-s5-p5-mirror-root-ingress-removal-001.md).

**S5 P6:** the root-level CCPU/SAS header was only a redundant alias of the
existing DEM source facade. It is removed; focused formal Redirector rebuild
and regression pass. The family-local compatibility/overlay ledger review
continues. See [S5 P6 audit](etc/evidence/m0-t271-s5-p6-redundant-ccpu-sas-alias-removal-001.md).

**S5 P7:** the complete family ledger confirms that all 13 private overlays
have one mirror boundary and no external consumer, and that the removed
`ingress` tree has no live consumer. It also found task-labelled divergence
markers and five non-original SoftPC fragment inputs still inside the mirror
root; both are mandatory remaining S5 remediation, so this is explicitly not
closure. See [S5 P7 ledger](etc/evidence/m0-t271-s5-p7-family-compatibility-and-overlay-ledger-001.md).

**S5 P8:** P7's remaining SoftPC fragment/marker defects are remediated:
original-name mirror boundaries now include private source fragments, typed
interrupt-06 mechanics belong to selector-blind `adapter-mvdm-host-out/softpc`, and every
remaining marker has a stable register ID. The formal five-fixture matrix,
boundary script and production-tree gate pass. Final full-family verification
and ledger refresh remain before S5 closure. See [S5 P8 evidence](etc/evidence/m0-t271-s5-p8-softpc-mirror-boundary-remediation-001.md).

**S5 P9 / closure admission:** full source-path and private-overlay matrix passes;
every remaining BOP mirror input has a registered source disposition, and no
external overlay consumer or unregistered divergence marker remains. The S5
technical exit evidence is ready for owner closure; see
[P9 final matrix](etc/evidence/m0-t271-s5-p9-final-mirror-closure-matrix-001.md).

**S5 P10:** P9's source classification is corrected: the production BOP root
still contains non-OpenNT composition/facade bodies and app still owns selector
routing. The dormant DPMI experiment has been removed from every formal
production/test input and archived as evidence. The whole-root re-audit gives
each remaining custom class a unique target owner. The subsequent
owner-directed closure transfers those moves and original-dispatcher boundary
recoveries to the debt ledger rather than claiming them complete. See
[P10 re-audit](etc/evidence/m0-t271-s5-p10-opennt-bop-mirror-reaudit-and-disposition-001.md).

**S5 P11:** an attempted test-only extraction of eight DEM composition bodies
was rejected by formal native `MS_bop_0 -> DemDispatch` linking: the original
provider table requires their same-shaped symbols. They remain production
dependencies and must be owner-split, not hidden under tests. The formal graph
now also gives intentionally shared test-support sources fixture-specific
object paths.

**S5 P12:** five non-original COMMAND overlay include stubs have been deleted.
Their private bodies are now included only by original `cmd.c`, `cmdmisc.c`,
`cmdconf.c`, `cmdenv.c` and `cmdpif.c` translation-unit boundaries. Refreshed
formal DEM/COMMAND source-path links and executions pass. The non-original
COMMAND compatibility headers are recorded as transferred source-recovery
work in the final closure.

**S5 P13:** the non-original XMS composition wrapper has been deleted and its
private body is included only by original `xms.c`. Formal XMS source-mirror
and A20 source-mirror fixtures relink and execute successfully. The remaining
`opennt_xms_compat.h` header is transferred owner-split work.

**S5 P14:** all eight non-original DEM composition `.c` bodies now live in
the private BOP overlay and are included solely by their original DEM source
unit (or its original dispatcher overlay). Refreshed formal DEM/COMMAND links
and executions pass. The remaining 17 project-named inputs are compatibility
headers requiring owner recovery, not provider/route bodies.

**S5 P15:** ten DEM-private compatibility/composition headers have joined the
eight P14 bodies below `opennt-bop-overlay`; the root now retains only seven
project-named headers with production cross-component consumers. Focused
formal DEM source-path rebuild and execution pass. Those seven headers and
the remaining app-held selector/service composition are recorded as deferred
owner-recovery work; final closure does not claim they are complete.

**S5 P16:** the two DEM root compatibility headers with no remaining app
consumer now also live under the matching private overlay.  `DemInit` retains
its original allocation/GetSystemDirectory flow through a same-shaped
adapter-mvdm-host-out/win32 facade, so app no longer writes the imported DEM global.  The
root residual set is now five cross-component headers (COMMAND, XMS and
VDMREDIR); focused formal DEM fixture rebuild and execution pass. Tests that
still include private overlay declarations remain a deferred boundary-removal
item; the owner-directed closure records it without claiming resolution.

**S2 closure:** the active `C4 C4 50 xx` route now stages its copied CPU frame
into the imported OpenNT `MS_bop_0` subset, which alone reads the guest service
byte and calls the original `DemDispatch` table.  The former project-authored
DEM ingress and its fixture are archived evidence. Fresh formal Ninja links
the complete graph; original-entry, original-table, checked-RAM, direct-host,
runtime-session and DEM provider fixtures pass. The retained DASD/IOCTL
direct-import fixture times out in its host-volume exercise and is recorded as
a machine/device-owner limitation, not passing dispatcher evidence. See the
[S2 closure](history/m0-t271-s2-closure-20260825.md).

## Closed Packet Archive

### M0 T266 S1 — repository-local `refs` consumer inventory and disposition

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner objective: “执行T任务：消灭refs，实现src闭包和artifacts报告归档。” The owner-selected first queue package is admitted as T266; its former unnumbered row is removed from Queue. |
| Objective | Produce the exhaustive, reproducible ledger of every live `refs/` build, configuration, include, tool, probe, test, source and report consumer; assign each one an owned destination or an explicit deletion/archive disposition without changing runtime semantics. |
| Non-goals | No premature `refs/` deletion; no wholesale import of dormant OpenNT/Bochs trees; no BOP/provider/machine feature; no historical source alteration; no build result claimed before the later migration packets. |
| Reference Baseline | [source-closure proposal](etc/operations/proposal-repository-local-source-closure-refs-retirement-001.md), M0 T265 closure, the current formal Ninja graph, all tracked `refs/` path references, and the source policy. |
| Files And ABI Surface | Status, Queue, the T266 proposal, a machine-readable consumer/disposition manifest, source-closure evidence and documentation inventory only. No production ABI in S1. |
| Applicable Rules | Execution and documentation closure, source policy, mirror-component standard, build-output hygiene, component ownership and no external formal dependency. |
| Verification | Tracked-file and formal-manifest scans; every row has consumer class, exact source path, owner destination, disposition and S-packet; independent count/reachability review; documentation governance and `git diff --check`. |
| Expected Markers | No ambiguous live consumer; every reachable Bochs input maps to `bochs-core`/`adapter-bochs`, OpenNT input to its named component, tool to `tools/`, fixture to `tests/`, and retained report only to `artifacts/research/`. |
| Asset Needs | Existing local `refs/` snapshots for read-only inventory, current owned component roots, tracked build/test/tool sources and formal Ninja generator. No new external import. |
| Reporting Requirements | Separate live, dormant historical, provenance-only and archive/report rows; state exact file counts and migration order; identify blockers and do not call a documentation reference a runtime dependency. |
| Stop Conditions | A reachable input has no valid owner, a proposed move would violate mirror provenance, a binary lacks approval/manifest, a formal path is not reproducible, or deletion would remove an unarchived required report. |
| Exit Criteria | The manifest covers every live `refs/` consumer and all eight snapshot roots, gives a non-destructive disposition/order for each, and its evidence is internally consistent. |
| Original Owner Request | “执行T任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | CMake/Ninja generators, configuration projections, `tools/build`, `tools/probe`, `tools/governance`, tests, component READMEs, literal source provenance comments, `.gitignore`, and archived report inputs. |
**S1 closure:** the consumer ledger covers 49 tracked non-document consumers and all eight local `refs/` snapshot roots. It separates live formal/build/configuration input from retained historical provenance and assigns every live path to S2, S3 or S4 before S5 may remove the tree. See the [S1 ledger](etc/evidence/m0-t266-s1-refs-consumer-disposition-001.md) and [machine-readable disposition](etc/operations/m0-t266-s1-refs-consumer-disposition.tsv).

### M0 T266 S2 — repository-local Bochs formal-input recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T266/S1 closure and the owner-approved T266 objective. |
| Objective | Recover the exact Bochs header/configuration closure proven by the existing formal Ninja dependency database into `src/bochs-core`, then eliminate `refs/bochs` from the current formal generator, manifest and current Bochs boundary fixtures. |
| Non-goals | No Bochs semantic edit, no product-shell/device enablement, no broad Bochs import, no OpenNT migration, no historical-tool migration and no `refs/` deletion. |
| Reference Baseline | T266/S1 ledger; formal `build/M0-T265-S7/r002` Ninja dependency database; pinned local Bochs 2.6 input; `bochs-core` mirror rules. |
| Files And ABI Surface | The exact 11 dependency headers/configuration inputs, `bochs-core` provenance/README records if required, formal Ninja generator/manifest, current Bochs boundary tests and S2 evidence. No runtime ABI change. |
| Applicable Rules | Source policy, mirror-component standard, Bochs adoption/non-rewrite rule, formal-build hygiene and T266 proposal. |
| Verification | SHA-256 identity of each imported input, no `refs/bochs` formal manifest/generator/current-test path, fresh external Ninja closure and focused Bochs boundary fixtures, governance and diff checks. |
| Expected Markers | The formal graph's `showIncludes` dependencies resolve only to `src/bochs-core` plus generated build config; no direct test/formal consumer reads `refs/bochs`. |
| Asset Needs | Existing `refs/bochs` only as the read-only exact import source during S2; existing formal Ninja graph and current fixtures. |
| Reporting Requirements | List all 11 source/destination hashes, prove imported files are exact originals, distinguish current from retained historical tools, and record any missing dependency before broadening scope. |
| Stop Conditions | An input differs from its pinned source, requires an unregistered Bochs edit, exposes an additional missing header outside the known closure, changes formal machine behavior, or a test needs historical product-shell code. |
| Exit Criteria | Every known formal Bochs dependency is an exact owned input; generator/manifest/current fixtures have no `refs/bochs`; fresh formal closure and focused current tests pass. |
| Original Owner Request | “执行T任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | Generated config input, root/nested headers, formal include roots, CMake current target use, top-level Bochs boundary fixtures, current manifest and `showIncludes` dependency list. |

**S2 closure:** The exact eleven formal Bochs header/config inputs are now byte-identical owned src/bochs-core mirrors. The re-rooted formal graph, two focused fixture executables, and the no-work dry-run all exit 0 outside the sandbox. See [S2 evidence](etc/evidence/m0-t266-s2-bochs-formal-input-recovery-001.md).
### M0 T266 S3 — repository-local reached OpenNT input recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Completed T266/S2 and the owner-approved T266 objective. |
| Objective | Re-root every current guest-staging and OpenNT governance/source-inventory consumer identified by the S1 ledger to an owned named `src/opennt-*` component root; create only an auditable controlled-import requirement where the selected original is absent. |
| Non-goals | No broad OpenNT import, no BOP/provider semantic implementation, no migration of historical tools/fixtures, no source alteration, and no `refs/` deletion. |
| Reference Baseline | T266/S1 disposition ledger; source policy; named OpenNT component ownership; current guest-staging and provider-map tools. |
| Files And ABI Surface | Guest staging and governance tools, any exact selected owned original input, S3 controlled-import ledger/evidence and Status. No runtime ABI change. |
| Applicable Rules | Source policy, mirror-component standard, component ownership, build-output hygiene, documentation governance and T266 proposal. |
| Verification | Every S3-owned tool resolves no `refs/opennt`; each selected original maps to one named owner root or an explicit import row; tool outputs remain reproducible; governance and diff checks pass. |
| Expected Markers | Current guest staging, BOP/provider inventory and owner manifest use only owned `src/opennt-*` locations. Historical comparison prose may retain provenance only. |
| Asset Needs | Existing local `refs/opennt` only as read-only selection baseline; current named component roots and tools. No external fetch. |
| Reporting Requirements | List every changed current consumer, selected owner path, exact hash for every new mirror import, and each unresolved source with reason/owner/disposition. |
| Stop Conditions | A required selected original has no justified component owner, requires a non-minimal mirror divergence, changes runtime behavior, or a consumer is actually historical and belongs in S4. |
| Exit Criteria | Current guest staging and OpenNT governance/source-inventory consumers contain no `refs/opennt` input; every reached selected original is owned or explicitly controlled for later import. |
| Original Owner Request | “执行T任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | Staging manifests, BOP/DEM/provider inventories, owner manifest, original-source provenance comments and tool default paths. |

**S3 closure:** 57 exact selected OpenNT inputs now have named owned locations; their identity ledger records all origin paths and SHA-256 values. BOP inventory/provider/DEM map export succeeds from those local inputs; guest staging produces 113 artifacts without a `refs/` path; the current owner manifest succeeds without `refs/opennt`. See [S3 evidence](etc/evidence/m0-t266-s3-opennt-input-recovery-001.md).

### M0 T266 S4 — historical consumer retirement and research-report archive

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Completed T266/S3 and the owner-approved T266 objective. |
| Objective | Eliminate every remaining tracked executable tool, probe, test, CMake/configuration and report consumer of repository-local `refs/`; retain only approved immutable research outputs below `artifacts/research/` with a reproducible archive manifest. |
| Non-goals | No `refs/` tree deletion, no production semantic change, no broad source import, no revival of historical fixture behavior, and no new BOP/machine capability. |
| Reference Baseline | T266/S1 disposition ledger; S2/S3 closures; current historical `tools/`, `tests/`, CMake and report-path scan; source and artifact policy. |
| Files And ABI Surface | Historical tool/probe/test/CMake configuration paths, report archive manifests, source provenance records, Status and evidence only. No production ABI. |
| Applicable Rules | Execution, source policy, build-output hygiene, component ownership, documentation governance and artifact/archive rules. |
| Verification | Tracked executable-consumer scan; current/historical classification review; report source/role/hash/size manifest; prove retained historical scripts no longer resolve `refs/`; governance and diff checks. |
| Expected Markers | No tracked executable tool/probe/test/configuration consumer contains a `refs/` path; each preserved report is under `artifacts/research/` with origin/disposition evidence; S5 gets a zero-consumer deletion precondition. |
| Asset Needs | Existing `refs/` snapshots read-only until S5; local historical scripts/tests/reports and source-closure ledger. No external fetch. |
| Reporting Requirements | List every retired/re-rooted historical consumer, distinguish immutable textual provenance from executable dependency, list archive files/hashes/sizes, and state any source that cannot be safely archived. |
| Stop Conditions | A historical script is still a declared formal input, a proposed archive includes executable build input or unapproved binary, a move changes semantics, or a retained report lacks source/role/hash/size provenance. |
| Exit Criteria | Every S4 ledger row has a verified re-root/retirement/archive disposition; no tracked executable historical consumer reads `refs/`; artifact archive evidence is complete; S5 has no ambiguous residual consumer. |
| Original Owner Request | “单人双角色模式执行任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | CMake, all `tools/` subtrees, tests, fixture preparation, historical manifests, import utilities, report generators, `.gitignore` and docs/report links. |
**S4 closure:** 41 historical inputs have a source/destination/hash disposition: 7 fixture inputs now live under `tests/legacy/reconstruction/`, and 34 former executable scripts/manifests are non-executable `docs/etc/legacy_code` evidence. The archive report/manifest is under `artifacts/research/`. Global CMake and a retained fixture have documented, pre-existing non-refs stale-name failures; neither is claimed passing. See [S4 evidence](etc/evidence/m0-t266-s4-historical-consumer-retirement-001.md).

### M0 T266 S5 — repository-local `refs` removal and zero-reference closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Completed T266/S4 and the owner-approved source-closure objective. |
| Objective | Remove the repository-local `refs/` tree and its configuration treatment after proving every remaining executable/build/configuration consumer has a named local owner or S4 retirement record. |
| Non-goals | No source semantic rewrite, no historical fixture repair, no change to immutable documentation provenance, and no claim that unrelated global CMake stale paths now build. |
| Reference Baseline | T266 S1 consumer ledger; S2/S3/S4 closure evidence; current tracked path/reference scan; formal Ninja graph and source policy. |
| Files And ABI Surface | `refs/` tracked content removal, `.gitignore`, any remaining live literal consumer, Status, zero-reference evidence and documentation inventory. No runtime ABI. |
| Applicable Rules | Execution closure, source policy, build/output hygiene, artifact archive policy and documentation governance. |
| Verification | Pre-delete tracked consumer scan; exact Git removal; post-delete zero-reference scans separated into live code/config versus immutable docs provenance; repository-local formal Ninja closure and focused current fixture checks; governance/diff checks. |
| Expected Markers | No `refs/` directory; no tracked executable/configuration/include/build input names that path; only classified immutable provenance text may retain the token `refs/`. |
| Asset Needs | Current owned source roots, S1 ledger, formal Ninja generator/manifest and S2/S3/S4 evidence. No external fetch. |
| Reporting Requirements | Record deleted root count/bytes, each permitted textual provenance exception, all verification commands/results, and every unrelated failure separately. |
| Stop Conditions | A live consumer remains, a delete target is not the exact repository `refs/` root, a formal path reads external source, or a required artifact/provenance record is missing. |
| Exit Criteria | `refs/` is absent; live-zero-reference scan and repository-local formal Ninja/current fixtures pass; permitted documentation provenance is enumerated; docs/artifact manifests and final T266 closure evidence are complete. |
| Original Owner Request | “单人双角色模式执行任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | Git ignore/configuration, CMake, formal Ninja, `tools/`, `tests/`, component READMEs, archive manifests, root documentation and repository directory scan. |
**S5 / T266 closure:** The exact local `refs/` root is absent: 1,887 tracked files (30,364,329 bytes) were removed, followed by checked cleanup of untracked residue. Current source/tool/configuration scans have no `refs/` input; only enumerated immutable provenance text remains. Fresh formal Ninja r001 dry-run resolves 485 owned local edges. See [S5 evidence](etc/evidence/m0-t266-s5-refs-removal-zero-reference-001.md).

### M0 T265 S1 — Bochs component identity and dependency governance

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner: “准入。” following the approved queue brief for Bochs component identity and mirror-overlay convergence. |
| Objective | Make the component rename, directed dependency rules, and private-overlay ownership rules authoritative before any source-tree move. |
| Non-goals | No `git mv`, source edit, Bochs/OpenNT behavior change, feature admission, or formal build execution. |
| Reference Baseline | [Bochs convergence proposal](etc/operations/proposal-bochs-component-identity-overlay-convergence-001.md), completed T264 closure, current component roots, and the mirror-component review standard. |
| Files And ABI Surface | Architecture/coding/rule authorities, queue, proposal, status, evidence and documentation inventory only; no runtime ABI. |
| Applicable Rules | Task admission and closure; component ownership; mirror/overlay review; source-first and document governance rules. |
| Verification | Cross-authority terminology/dependency scan, proposal consistency review, documentation governance, and `git diff --check`. |
| Expected Markers | Only `app` and `adapter-mvdm-host-out/softpc` call the declared `adapter-bochs` mechanical facade; only `adapter-bochs` calls `bochs-core`; only a native mirror calls its own overlay. |
| Asset Needs | Existing component-layout evidence and source-tree ownership registers; no external source import. |
| Reporting Requirements | State exact renamed component names, permitted directed edges, overlay privacy, and every deferred source move. |
| Stop Conditions | An authority implies a prohibited reverse/direct dependency, a source move becomes necessary to state policy, or an overlay needs a generic/shared consumer. |
| Exit Criteria | All listed authorities and proposal agree on final names/directions; queue no longer contains the admitted candidate; no source move exists in the S1 diff. |
| Original Owner Request | “新增队列最优先T任务，包含以下S任务：S1. 治理文档：组件改名：bx-machine => adapter-bochs，… bx-core => bochs-core，… `*-overlay` 组件仅允许被它的原生镜像组件调用。” |
| Similar-Issue Sweep | Old component names in authorities, direct core consumers, overlay reachability, and accidental OpenNT/Win32 vocabulary in the assembly component. |

**S1 closure:** `ff2d0d70` establishes the target names, direct dependency
directions and overlay privacy rule across all current authorities. The S1
diff contains no production source or build input.

### M0 T265 S2 — Bochs production-root rename

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T265 proposal S2 after completed S1 governance closure. |
| Objective | Rename `src/bx-core` to `src/bochs-core` and `src/bx-machine` to `src/adapter-bochs` with `git mv`, then make every live production/build/document consumer resolve the new roots. |
| Non-goals | No source-body/ABI/behavior change, no test-history mass rename, no component reassignment beyond the two roots, and no overlay implementation. |
| Reference Baseline | T265/S1 authority `ff2d0d70`, formal Ninja manifest, root ownership scan and current exact Git paths. |
| Files And ABI Surface | Both source roots, direct include/build/manifests, focused fixture paths and authority references; pathname/build graph only, no runtime ABI. |
| Applicable Rules | T265 proposal, `git mv`, component ownership and dependency direction, source identity, production-only roots, and build hygiene. |
| Verification | Pre/post live-reference inventory, no old production-root input, regenerated formal Ninja graph, exact target build/no-work, governance and `git diff --check`. |
| Expected Markers | No live source/build input below `src/bx-core` or `src/bx-machine`; all formal component/module names and include paths resolve the renamed roots. |
| Asset Needs | Current formal manifest, generator, focused fixture set and existing Bochs provenance register. |
| Reporting Requirements | Report moved-path counts, every retained historical/test old-name exclusion, build graph repair, and whether any root content changes were required. |
| Stop Conditions | A move would alter imported content, produce a destination collision, expose an unclassified reverse dependency, or require behavior change. |
| Exit Criteria | Both roots are Git renames, source/build consumers are repaired, source identity unchanged, stated formal closure passes, and no prohibited old production path remains. |
| Original Owner Request | “S2. 实施这两个bx组件的重命名工作”. |
| Similar-Issue Sweep | Formal manifest module names, include roots, generator checks, fixture sources, README/doc links, and stale root-path string literals. |

**S2 closure:** Both production roots are recorded by Git as renames.  Live
source, formal build inputs and support-header references use
`bochs-core`/`adapter-bochs`; the historical fixture directory names are not
production roots.  Fresh `build/M0-T265-S2/r001` completed its final 471-step Ninja
closure with exit `0` and 81 linked executables.  See
[S2 evidence](etc/evidence/m0-t265-s2-bochs-production-root-rename-001.md).

### M0 T265 S3 — adapter-Bochs provenance migration

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T265 proposal S3 after S2 closure. |
| Objective | Classify every `adapter-bochs` source by pinned Bochs provenance and return upstream-original, true-subset or same-shaped material to its upstream-relative `bochs-core` path or to `bochs-core-overlay` when the mirror rule requires it. |
| Non-goals | No new CPU/device/BOP/OpenNT/Win32 semantics, no generic compatibility layer, and no expansion beyond the declared `adapter-mvdm-host-out/softpc` mechanical-facade edge to `adapter-bochs`. |
| Reference Baseline | T265 proposal S3, pinned Bochs 2.6 baseline, `adapter-bochs` source inventory and T265/S2 build closure. |
| Files And ABI Surface | `adapter-bochs`, affected `bochs-core`/`bochs-core-overlay` paths, formal manifest, provenance ledger, README exception register and focused fixtures; existing mechanical ABI only. |
| Applicable Rules | Mirror identity/subset/divergence rules; private-overlay rule; `adapter-bochs` Bochs-only vocabulary; source-first provenance and formal-build closure. |
| Verification | File-by-file baseline comparison, destination collision check, include/link edge scan, README/`DIVERGENCE:` review, focused formal Ninja closure, governance and `git diff --check`. |
| Expected Markers | Every retained adapter file is self-authored Bochs assembly; every moved file has an upstream-relative mirror or private-overlay disposition and recorded provenance. |
| Asset Needs | Existing pinned Bochs reference, current component manifest and no new external import. |
| Reporting Requirements | Report each file disposition, all subset/modified markers, retained self-authored assembly, and any deferred overlay move. |
| Stop Conditions | A file needs non-Bochs vocabulary, an overlay would need an external consumer, provenance cannot be established, or behavior expansion is required. |
| Exit Criteria | `adapter-bochs` contains only self-authored Bochs-only assembly; moved mirror/overlay files and all required registration/build edges are verified. |
| Original Owner Request | “S3. adapter-bochs里面的原版镜像和镜像修改送入 bochs-core 和bochs-core-overlay”. |
| Similar-Issue Sweep | Root-level Bochs files, `iodev`/`gui` origin material, true subsets, source list ownership, and accidental direct consumers of a private overlay. |

**S3 progress:** P1/P2 returned `gui/paramtree.{cc,h}`,
`iodev/scancodes.{cc,h}`, and root `logio.cc` to `bochs-core`; see the
[provenance ledger](etc/evidence/m0-t265-s3-adapter-bochs-provenance-ledger-001.md).
The required sweep found direct `adapter-mvdm-host-out/softpc` consumption of
`adapter-bochs`/`bochs-core` objects.  S3 permits `adapter-mvdm-host-out/softpc` to consume
the declared selector-blind, fixed-width `adapter-bochs` mechanical facade,
but it must not import a `bochs-core` header, type, object or global.  This
facade correction is recorded in the proposal rather than deferred as an
undocumented exception.

**S3 closure:** `adapter-bochs` now contains only self-authored Bochs-only
assembly. P1/P2/P3 returned `paramtree`, `scancodes`, `logio`, `pc_system`,
PIC, and keyboard ownership to the proper core/overlay boundary. The focused
PIC and headless-8042 fixtures compile, link and run; the provenance ledger
records each true subset and private-overlay registration. No production
component directly includes `bochs-core-overlay`.

### M0 T265 S4 — bochs-core mirror/overlay convergence

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T265 proposal S4 after S3 provenance closure. |
| Objective | Audit every differing `bochs-core` mirror file and move every over-three-executable-line semantic intrusion or over-50-percent divergence to private `bochs-core-overlay`, retaining only the smallest marked mirror boundary. |
| Non-goals | No BOP, OpenNT, DOS, VDM, WOW, Win32, device expansion, or CPU semantic expansion. |
| Reference Baseline | T265 proposal S4, local pinned Bochs 2.6 source, S3 provenance ledger and established mechanical fixtures. |
| Files And ABI Surface | `bochs-core`, `bochs-core-overlay`, `adapter-bochs` only for existing assembly calls, formal manifest, per-file ledger, README exception indexes and focused fixtures. |
| Applicable Rules | Mirror identity/subset/divergence standard; private-overlay rule; Bochs-only component vocabulary; source-first recovery and no behavior expansion. |
| Verification | Per-file baseline measurement; direct-overlay-consumer scan; `DIVERGENCE:` and README review; focused formal Ninja builds/fixtures; governance and `git diff --check`. |
| Expected Markers | Every non-exact core file is exact, true subset, same-shaped marked change, or a smallest core boundary to a private overlay. |
| Asset Needs | Existing pinned Bochs baseline, S3 formal graph and current fixture set. |
| Reporting Requirements | Record every differing file, ratio, retained core hunk, overlay location, exception ID and verification result. |
| Stop Conditions | An overlay needs a non-core caller, a source needs non-Bochs vocabulary, a behavior expansion is required, or baseline provenance is unavailable. |
| Exit Criteria | Complete per-file ledger and exception register prove no oversized self-authored body remains in `bochs-core`; private overlay is invoked only by core and focused formal evidence passes. |
| Original Owner Request | “S4. 按照镜像组件审计规则，把bochs-core里面的大幅度修改部分，送进 bochs-core-overlay”. |
| Similar-Issue Sweep | CPU #UD/observer paths, memory helpers, real-mode entry/profile, PIC/keyboard lifecycle bridges, build-root redirects and overlay privacy. |

**S4 closure:** all admitted oversized Bochs bodies now live in private `bochs-core-overlay`: #UD/first-fault, SIM-free checked RAM, real/V86 entry/profile, and all admitted copied observers. `bochs-core` has no direct external include; the overlay has no external consumer; `adapter-mvdm-host-out/softpc` has no direct Bochs type/header/global import. Formal changed-object and library links pass, and physical-write, checked-RAM, RAM-preflight and opaque-BOP-ingress fixtures exit zero. The known generic-context diagnostic exit 2 remains non-pass evidence. See the [S4 overlay audit](etc/evidence/m0-t265-s4-bochs-core-overlay-audit-001.md).

### M0 T265 S5 — opennt-host mirror/overlay convergence

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T265 implementation; S4 is closed and S5 is the next approved packet in the admitted proposal. |
| Objective | Audit every differing `opennt-host` mirror file against its pinned OpenNT baseline; move only an oversized new semantic body to private `opennt-host-overlay`, while retaining valid original subsets and same-shaped minimal edits. |
| Non-goals | No BOP enablement, provider expansion, Win32/Redirector/WOW recovery, or removal of valid original source merely to improve a diff statistic. |
| Reference Baseline | T265 proposal S5, `refs/opennt`, current `opennt-host` README register and T262/T263 host evidence. |
| Files And ABI Surface | `opennt-host`, a possible private `opennt-host-overlay`, README/register, formal manifest and existing host-focused fixture only. |
| Applicable Rules | Mirror subset/modification/overlay standard; source recovery ladder; no generic compatibility owner; overlay privacy. |
| Verification | Per-file source comparison, `DIVERGENCE`/README correspondence, source-first disposition, manifest closure, focused host fixture, governance and `git diff --check`. |
| Expected Markers | Every host file is exact original, valid true subset, same-shaped minimal modification, or a smallest mirror call boundary to its own private overlay. |
| Stop Conditions | A move changes reached host behavior, requires a new generic facade, loses original control/failure semantics, or would create an overlay consumer outside `opennt-host`. |
| Exit Criteria | Complete host ledger and README register show no oversized self-authored body remains in `opennt-host`; any overlay is private; formal focused evidence passes. |
| Original Owner Request | “S5. 按照镜像组件审计规则，把 opennt-host 里面的大幅度修改部分，送进 opennt-host-overlay”. |
| Similar-Issue Sweep | Original path identity, cropped product-shell blocks, header locations, adapter boundaries, link names, direct Bochs access and false source-derived providers. |
**S5 closure:** seven host files remain byte-identical OpenNT sources; four are registered source-shaped subsets/minimal edits. Three previously oversized NT4 product-binding declaration blocks are now private `opennt-host-overlay` fragments, with no external consumer. Formal Ninja compiled the changed host objects and `t263-s8-opennt-host-mirror-fixture.exe` linked and exited zero. See the [S5 audit](etc/evidence/m0-t265-s5-opennt-host-overlay-audit-001.md).

### M0 T265 S6 — bochs-core bridge-file retirement

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S6`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner: “加一个S任务做好这一条”, after reviewing the four self-authored bridge files retained in `bochs-core`. |
| Objective | Retire `cpu/opaque_callback.{h,cc}` and `iodev/keyboard_bridge.{h,cc}` from the Bochs mirror. Preserve their required minimal mechanical boundary only in existing pinned-Bochs paths; keep all self-authored state and bodies private to `bochs-core-overlay`. |
| Non-goals | No new #UD/BOP/device behavior, no CPU or keyboard semantic expansion, no public overlay API, and no adapter/OpenNT vocabulary in `bochs-core`. |
| Reference Baseline | Pinned `O:\repos.external\bochs-2.6-compat\bochs-2.6`, T265/S4 overlay audit, T265/S3 keyboard provenance ledger, and the T265 proposal S6. |
| Files And ABI Surface | The four retiring bridge files; their smallest existing Bochs-file replacement boundaries; overlay-private bodies; `adapter-bochs` facade/include consumers; formal manifest and focused fixtures. |
| Applicable Rules | Mirror component review standard; native-mirror-only overlay rule; `adapter-bochs` as sole production caller of `bochs-core`; Bochs-only vocabulary; no behavior expansion. |
| Verification | Baseline file-presence scan proves no non-upstream production `.c/.cc/.h` remains under `bochs-core`; include/link scan proves overlay privacy and no external core consumer; changed-object Ninja build plus opaque-ingress and headless-8042 fixtures; governance and `git diff --check`. |
| Expected Markers | The two bridge pairs are absent; callback registration and keyboard headless boundary are represented only by registered, minimal `DIVERGENCE:` points in original Bochs paths; overlay owns self-authored implementation. |
| Asset Needs | Existing pinned Bochs baseline and existing source/fixture inputs only; no import or new product capability. |
| Reporting Requirements | Report the exact destination original paths, each retained core hunk, deleted bridge paths, resulting non-upstream-file count, source/README exception updates, and focused verification. |
| Stop Conditions | A replacement needs more than a minimal core boundary, makes the overlay externally callable, requires a new non-Bochs file in `bochs-core`, or changes a reached machine result. |
| Exit Criteria | All four self-authored bridge files are removed; the production mirror contains only upstream-relative source names; retained boundaries are locally marked and README-registered; focused build/fixtures and governance checks pass. |
| Original Owner Request | “overlay 的目的就是为了压缩 diff… opaque_callback, keyboard_bridge… 我觉得有点过分” and “加一个S任务做好这一条”. |
| Similar-Issue Sweep | Every file under `bochs-core` absent from the pinned tree, bridge-named public headers, direct overlay consumers, manifest entries, README exception paths, and newly expanded original-file diffs. |

**S6 closure:** `bochs-core` no longer contains self-authored bridge source files. The formal Ninja `bochs-core` and `adapter-bochs` libraries build; opaque BOP ingress and the corrected two-round native 8042 lifecycle fixture exit zero. See the [S6 evidence](etc/evidence/m0-t265-s6-bochs-core-bridge-file-retirement-001.md).
### M0 T264 S1 — project code filename inventory

### M0 T265 S7 — bochs-core retained-diff consolidation

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S7`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved after the complete post-S6 Bochs 2.6 baseline audit. |
| Objective | Move duplicated default-off observer gates/macros into `bochs-core-overlay`; re-evaluate the keyboard creation boundary with the existing two-round lifecycle fixture; retain only required minimal core decisions, declarations and valid true subsets. |
| Non-goals | No guest/BOP/OpenNT semantics, no CPU or device expansion, no restoration of Bochs SIM/GUI/plugin product shell, no generic overlay interface, and no removal of required real/V86, #UD, RAM, PIC or lifecycle mechanics merely to improve a statistic. |
| Reference Baseline | Pinned `O:\repos.external\bochs-2.6-compat\bochs-2.6`, the S6 closure, the retained-diff audit, and the T265 proposal S7. |
| Files And ABI Surface | Existing adopted CPU/memory observer call sites, private overlay gate header and keyboard lifecycle body, formal manifest, README/register, and existing focused observer/8042 fixtures only. No public ABI is added. |
| Applicable Rules | Mirror identity/subset/divergence rule; native-mirror-only overlay rule; Bochs-only vocabulary; source-first recovery; no behavior expansion. |
| Verification | Baseline numstat before/after; direct-overlay-consumer scan; `DIVERGENCE`/README consistency; formal Ninja changed-library build; physical-write/opaque-ingress and two-round headless-8042 fixture execution; governance and `git diff --check`. |
| Expected Markers | Observer call sites no longer define local gates; any keyboard creation boundary in `keyboard.cc` is one marked call; overlay owns all moved bodies and has no external consumer. |
| Stop Conditions | Extraction changes a reached machine result, requires SIM/GUI/plugin facilities, produces a non-core overlay consumer, or cannot preserve the existing fixture lifecycle. |
| Exit Criteria | All duplicated observer scaffolding is private-overlay code; keyboard extraction is either formally verified or retained with a documented source/fixture reason; every remaining core hunk is classified, marked and README-registered; stated checks pass. |
| Original Owner Request | “支持，批准进入S任务实施以上方案，目标是进一步减少不必要的diff，送入overlay”. |
| Similar-Issue Sweep | Repeated feature-gate macros, observer no-op bodies, keyboard creation/teardown split, stale README exception descriptions, and hidden overlay consumers. |

**S7 closure:** duplicated default-off observer gate/no-op scaffolding is private `bochs-core-overlay` code, and the private keyboard lifecycle now owns both creation and teardown. Fresh formal MSVC x64 `/MT` libraries and the opaque-ingress plus two-round headless-8042 fixtures pass; see [S7 evidence](etc/evidence/m0-t265-s7-bochs-core-retained-diff-consolidation-001.md).

**T265 closure:** the S1--S7 package is closed. `adapter-bochs` is now
Bochs-only assembly; `bochs-core` retains only upstream-relative source names
and smallest registered boundaries to its private overlay; and `opennt-host`
has no oversized self-authored product-binding body. The closure makes no BOP
feature, provider, or continuous guest-runtime claim. See the
[T265 closure record](history/m0-t265-closure-20260824.md). No successor is
admitted by this closure.


| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner direction: “请你执行。app, session, adapter-* 这些组件。清理”. |
| Objective | Produce the complete in-scope filename/consumer manifest needed to remove `runner`, `ntvdm64`, `bx`, and `bochs` from project-authored and mechanical-adaptation code filenames. |
| Non-goals | No content/ABI/behavior change; no rename of the six source mirrors or any `*-overlay`; no rename of an imported upstream filename merely because it is temporarily retained as a non-live input. |
| Reference Baseline | [Filename-normalization proposal](etc/operations/proposal-project-code-filename-normalization-001.md), current formal Ninja manifests, and live source/build consumers. |
| Files And ABI Surface | `app`, `session`, `adapter-mvdm-host-in`, `adapter-mvdm-host-out/softpc`, `adapter-mvdm-host-out/win32`, in-scope fixtures/build manifests, filename inventory evidence, status and documentation inventory only; no ABI change. |
| Applicable Rules | Execution closure, component ownership, mirror source identity, `git mv` requirement, and production-only component rules. |
| Verification | Recursive filename scan, include/build/document consumer scan, live-versus-imported classification review, documentation governance and `git diff --check`. |
| Expected Markers | Every in-scope matching filename has one retain/rename disposition, proposed neutral basename and consumer set; excluded original files name their source-identity reason. |
| Asset Needs | Current source tree, Git path history, formal Ninja manifest and source-root ownership registers. |
| Reporting Requirements | Report separate counts for project-authored code, fixtures/build inputs, and original/external identity exclusions; do not silently omit an adapter path. |
| Stop Conditions | A rename would alter an original mirror identity, collide with another path, or require an ABI/content change. |
| Exit Criteria | The manifest completely assigns every matching in-scope file and consumer; S2/S3 can execute only its approved `git mv` rows. |
| Original Owner Request | “我们文件名不应该包含 runner, ntvdm64，bx，bochs之类的；它们的来源都已经规范到各自的组件目录里面了。请你修复所有类似代码名字；6个镜像组件及其overlay不需要管。” |
| Similar-Issue Sweep | Imported SoftPC/MVDM headers accidentally treated as project files, stale build/source-list paths, fixture-only stale includes, forbidden mirror renames, and product artifact names mistaken for code filenames. |

The preceding T263/S9 record is closed historical context, retained below until
the status-history migration supersedes the old compact records.

### M0 T264 S2 — app/session filename normalization

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T264/S1 inventory closure under the owner-approved filename-normalization task. |
| Objective | Rename the 38 `app` and four `session` project-code files from redundant former project-specific basenames to neutral owner-local names and repair direct consumers. |
| Non-goals | No source symbol, ABI, behavior, component-directory, mirror, overlay, or historical-header rename. |
| Reference Baseline | [S1 inventory](etc/evidence/m0-t264-s1-project-code-filename-inventory-001.md) and the formal Ninja source list. |
| Files And ABI Surface | `src/app`, `src/session`, direct runner/fixture/build references and naming evidence only; filename paths, no ABI. |
| Applicable Rules | T264 proposal; `git mv`; no mirror filename change; focused build closure. |
| Verification | No prohibited basename in `app`/`session`; stale-reference scan; affected formal Ninja target; `git diff --check`. |
| Expected Markers | All 42 approved paths are Git renames and all direct includes/source lists name the new paths. |
| Stop Conditions | A collision, non-path semantic replacement, or discovered upstream-source identity file pauses the rename. |
| Exit Criteria | The scoped scan is clean, direct formal targets compile, and all renamed paths remain traceable as Git moves. |
| Original Owner Request | “app, session, adapter-* 这些组件。清理”. |
| Similar-Issue Sweep | Runner tests, Ninja manifests, include paths and stale historical current-path aliases. |

**S2 closure:** all 42 approved paths were moved with `git mv`; 55 direct
consumer paths changed without a symbol or ABI rename. The fresh formal Ninja
graph at `build/M0-T264-S2/r001` compiled to completion and a final dry run had
no work. See the S1/S2 inventory evidence.

### M0 T264 S3 — adapter filename normalization

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T264/S1 inventory and completed S2 under the owner-approved cleanup task. |
| Objective | Rename the 77 project-authored `adapter-mvdm-host-in` and top-level `adapter-mvdm-host-out/softpc` files from redundant former project-specific basenames to neutral owner-local names, then repair direct source, fixture and build consumers. |
| Non-goals | No rename of `adapter-mvdm-host-out/softpc/include/mvdm/**`, `include/softpc.new/**`, original mirror/overlay code, source symbols, ABI, or behavior. |
| Reference Baseline | [S1/S2 inventory](etc/evidence/m0-t264-s1-project-code-filename-inventory-001.md) and `tools/build/t260-s8-component-manifest.json`. |
| Files And ABI Surface | `adapter-mvdm-host-in`, top-level project-owned `adapter-mvdm-host-out/softpc` files, direct tests/build references and evidence only; filename paths, no ABI. |
| Applicable Rules | T264 proposal; `git mv`; source-identity exclusions; no semantic rewrite. |
| Verification | No prohibited basename in scoped project-owned adapter files; stale-reference scan; fresh formal Ninja closure; `git diff --check`. |
| Expected Markers | All 77 approved paths are Git renames and direct consumers name only their new paths. |
| Stop Conditions | A collision, an imported-source identity ambiguity, or a required non-path behavior change pauses the packet. |
| Exit Criteria | The scoped scan is clean, formal Ninja reaches no-work after completion, and historical headers remain unchanged. |
| Original Owner Request | “app, session, adapter-* 这些组件。清理”. |
| Similar-Issue Sweep | Adapter headers, formal source lists, direct component-integration fixtures and imported MVDM/SoftPC headers. |

**S3 closure:** all 77 project-owned adapter paths were moved with `git mv` and
217 live path consumers were updated without source symbol or ABI changes. The
fresh formal graph at `build/M0-T264-S3/r001` compiled to completion and a
final dry run had no work. See the S1/S2/S3 inventory evidence.

### M0 T264 S4 — filename normalization closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T264/S1 inventory plus completed S2 and S3 under the owner-approved cleanup task. |
| Objective | Prove that the in-scope production component roots contain no prohibited project/backend filename prefix and that their live consumers no longer name a moved path. |
| Non-goals | No test-directory or historical-evidence mass rename; no original mirror/overlay filename or content change beyond an unavoidable consumer include-path update; no ABI/behavior change. |
| Reference Baseline | [S1/S2/S3 inventory](etc/evidence/m0-t264-s1-project-code-filename-inventory-001.md), formal Ninja manifests, and committed Git rename set. |
| Files And ABI Surface | In-scope component roots, direct consumers, closure evidence/status/history/document inventory; no ABI. |
| Applicable Rules | T264 proposal, source identity, component boundaries, documentation and closure rules. |
| Verification | Recursive basename scan; stale moved-path scan over live source/test/build inputs; formal Ninja no-work check; governance and diff checks. |
| Expected Markers | Zero prohibited basenames in project-authored in-scope component code and zero live reference to a moved filename. |
| Stop Conditions | A matching path is an unclassified live source input, a consumer cannot be updated without semantics changing, or a mirror filename would need renaming. |
| Exit Criteria | The T264 inventory is closed with scoped zero results, formal build evidence passes, unrelated historical matches are classified, and the reviewed result is committed/pushed. |
| Original Owner Request | “app, session, adapter-* 这些组件。清理”. |
| Similar-Issue Sweep | Build manifests, source-list JSON, test includes, generated graph inputs and imported-interface exclusions. |

**S4 closure:** the final recursive component scan found zero
prohibited basenames in the five in-scope production roots. The 77 staged
adapter historical basenames had zero live reference under `src`, `tests`, or
`tools/build`. The previously recorded S2 and S3 formal Ninja roots both end
at no-work dry runs after their completed builds. Documentation governance and
`git diff --check` pass. The later owner-directed temporary-suffix sweep reopens
T264 as S5; it does not invalidate the S4 product/backend-prefix result.

### M0 T264 S5 — implementation-version filename normalization

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner direction: “本T任务还有一个忘了说了，增加: 文件名里面有v1 v2这玩意的也清理干净”. |
| Objective | Remove standalone temporary generation filename segments, then classify and remove non-semantic project/backend/OpenNT source-provenance identifier prefixes from all in-scope project-authored production code and repair direct consumers. |
| Non-goals | No source-symbol change where an identifier actually names DOS/VDM guest semantics, Bochs/register mechanics, an OpenNT provider/guest contract, a public configuration contract, or a preserved historical interface; no function-contract, ABI, behavior, component-directory, mirror, overlay, historical-interface, or test-directory rename. |
| Reference Baseline | T264 proposal and S1/S4 inventory; the 70-row versioned-filename scan and source token disposition audit. |
| Files And ABI Surface | `src/app`, `src/session`, `src/adapter-mvdm-host-in`, top-level project-owned `src/adapter-mvdm-host-out/softpc`, their direct consumers, and closure evidence only; private project identifiers and filename paths, no ABI. |
| Applicable Rules | `git mv`, component ownership, source-identity exclusions, production-only component rule, and no semantic rewrite. |
| Verification | Scoped recursive basename scan; stale old-path scan; identifier disposition audit; fresh formal Ninja closure; documentation governance and `git diff --check`. |
| Expected Markers | Zero standalone `v1` or `v2` filename segments; no non-semantic project/backend/OpenNT source-provenance prefix remains in scoped private identifiers; and zero live direct reference to moved versioned paths. |
| Stop Conditions | A destination collision, imported-source identity ambiguity, or a consumer that requires a semantic change rather than an exact path replacement. |
| Exit Criteria | All 70 inventoried paths are Git moves; non-semantic identifier prefixes are removed with direct consumers repaired; excluded historical headers remain unchanged; each retained lexical term has an auditable semantic disposition; and stated checks pass. |
| Original Owner Request | “文件名里面有v1 v2这玩意的也清理干净”. |
| Similar-Issue Sweep | Filename-only version suffixes versus actual source symbols, fixture target names, source-list JSON, and retained historical interface paths. |

**S5 P1 result:** all 70 in-scope temporary-suffix filenames were refined with
`git mv`, and the exact source/test/build path consumers were repaired. The
reviewed non-semantic private project-prefixed and source-provenance
`opennt_*` identifier families were normalized while strings/comments,
original-interface names, DOS/VDM/Bochs mechanics, and OpenNT contract-facing
facades were retained. A synchronous formal Ninja attempt reached the genuine
compiler boundary but is blocked before project-owned targets complete by the
pre-existing `FLOAT128` macro collision in Bochs FPU source when the current
Windows SDK's `winnt.h` is present. This preliminary diagnosis was corrected
by S5 P2: the collision was caused by T264's newly generic app-local
`config.h` shadowing Bochs's generated `<config.h>` in the global include
search order. `config.h` was renamed to `bundle_config.h`, the one non-app
consumer was repaired, and the fresh formal graph completed before a final
`ninja -n` reported no work. See the S5 evidence update and the T264 closure
record.

### M0 T263 S9 — OpenNT-host three-file original-body recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S9`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner direction: “opennt-host这3个文件肯定都不及格了，打回重写吧…批准，请你开始实施。” |
| Objective | Rebuild `nt_error.c`, `vrinit.c`, and `vrnmpipe.c` from their exact OpenNT originals: retain the reached original function bodies, associated data and explanatory comments; delete unconnected function blocks with their dedicated support/commentary; use only statement-local same-shaped `adapter-mvdm-host-out/win32` or `adapter-mvdm-host-out/softpc` substitutions where a retained call cannot compose. |
| Non-goals | No VDD/NetBIOS/DLC/ICA/CSR/WOW product-shell recreation; no new BOP service admission; no fabricated `VrInitialize` success; no Bochs semantic change. |
| Reference Baseline | `O:\\repos.external\\OpenNT\\base\\mvdm\\{softpc.new\\host\\src\\nt_error.c,vdmredir\\vrinit.c,vdmredir\\vrnmpipe.c}`, current reached callers, `HOST-DIV-012..025`, and the source-first recovery rules. |
| Files And ABI Surface | The three host mirrors; needed declared `adapter-mvdm-host-out/win32`/`adapter-mvdm-host-out/softpc` same-shaped facades; focused host/Redirector fixtures; host README/evidence/status. `adapter-mvdm-host-in` remains selector-blind ingress only and is not a provider dependency. |
| Applicable Rules | Source-first ladder; original body/order/layout preservation; statement-local divergence/register rule; no fabricated success; component ownership and bounded mapping rules. |
| Verification | Per-retained-block original-span ledger; source diff review; positive and negative focused fixture coverage; formal Ninja closure; documentation governance; `git diff --check`. |
| Expected Markers | Each retained function has a cited original span and unchanged body except registered adapter call sites; every deleted block names its unavailable dependency and owner; `VrInitialize` either retains its required original prerequisites or is not admitted. |
| Asset Needs | Pinned OpenNT sources, existing public-Win32 facade, bounded SoftPC/CCPU facade, session-owned handle/mapping services, formal Ninja graph. |
| Reporting Requirements | Separately report original retained lines, adapter substitutions, deleted unconnected blocks, and any capability that must remain unavailable. Do not call a cropped function connected if its original success prerequisites are absent. |
| Stop Conditions | A retained block would require a new product-shell adapter, an adapter would acquire BOP/Redirector semantics, an original success path cannot be preserved, or a crop bisects retained control flow. |
| Exit Criteria | The three files are reviewable original-body subsets; no project-authored replacement algorithm remains in a retained block; all substitutions are local and README-indexed; no incomplete historical success is exposed; stated checks pass. |
| Original Owner Request | “原版文件，保留大段注释部分，删去不需要接通的那些部分及配套注释；保留所有需要接通的部分；然后，这些部分如果依赖 adapter-mvdm-host-out/win32, adapter-mvdm-host-in, adapter-mvdm-host-out/softpc，就在对应地方实现需要的替代接口。” |
| Similar-Issue Sweep | Rewritten same-name source, blanket divergence claims, include-only but behavior-changing edits, hidden fabricated success, unbounded host pointers, and adapter ownership leaks. |

**S9 closure:** all three files were re-rooted from cited original source
spans. `nt_error.c` retains the original direct-access body through the
same-shaped dialog/TLS facade, with only pointer-width-safe transport casts;
`vrinit.c` retains only original `VrInitialized` and no longer reports an
unsupported initialization success; and `vrnmpipe.c` retains the original
synchronous interceptors and source-order list helpers. The former Redirector
`57:00/57:01` consumers and `LoadVdmRedir` now return explicit failure until
their full original lifecycle can be admitted, rather than reviving deleted
host success stubs.

The fresh `build/M0-T263-S9/r001` formal Ninja graph reached `no work to do`.
Its `opennt-host` mirror, BOP 59, DEM file, and Redirector disposition fixtures
all exited `0`. The detailed retained/deleted/dependency and verification ledger
is [S9 recovery evidence](etc/evidence/m0-t263-s9-three-file-original-body-recovery-ledger-001.md).

### M0 T263 S8 — closed: reachable OpenNT host-source similarity repair

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S8`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner direction: “你现在增开一个S任务做这个修复。” This reopens T263 after S7 closure solely for the four composed host subsets. |
| Objective | Restore OpenNT structure/logic for every currently admitted feature block in `nt_error.c`, `vrinit.c`, `vrnmpipe.c`, and `vrputil.c`; retain only statement-local adapter seams and explicit unconnected feature-block crops. Similarity is judged per needed logical block: restore the original module and algorithm comments together with a retained block where practical, but do not import otherwise-unreachable product code merely to raise a byte count. |
| Non-goals | No dormant VDD/debug/FAX/WOW package import, no new BOP family, no Bochs semantic change, no recreation of NT4 CSR/CCPU product shells and no fabricated unavailable success. |
| Reference Baseline | `refs/opennt/base/mvdm/{softpc.new/host/src/nt_error.c,vdmredir/vrinit.c,vdmredir/vrnmpipe.c,vdmredir/vrputil.c}`, `HOST-DIV-012..024`, and T263/S7 P2 withdrawal record. |
| Files And ABI Surface | The four host mirrors, their original/reached headers, same-shaped `adapter-mvdm-host-out/win32`/`adapter-mvdm-host-out/softpc` facade declarations and implementations, focused fixtures, README/evidence/status; no new cross-component ABI shape. |
| Applicable Rules | Source-first ladder; component production-only rule; original order/layout/failure preservation; adapter ownership; explicit `DIVERGENCE` and README one-to-one register. |
| Verification | Per-file original block/line ledger; include/call graph; source similarity review excluding explicit crops; formal Ninja closure; focused positive/negative fixture for every retained provider block. |
| Expected Markers | Each retained feature block maps to an original span; each platform departure has a same-shaped adapter call and `HOST-DIV` row; each crop names source span, missing dependency and future owner. |
| Asset Needs | Pinned OpenNT source, current adapter/session interfaces, MSVC/Ninja formal graph and existing focused host fixtures. |
| Reporting Requirements | Distinguish exact original blocks, local adapter call substitutions and omitted whole feature blocks; do not state an unconnected historical feature is runnable. |
| Stop Conditions | A required adapter would absorb Redirector/DOS/BOP meaning, a crop bisects a retained control-flow block, an unregistered divergence appears, or a dormant source package would be imported. |
| Exit Criteria | The four source files contain no project-authored replacement algorithm for an admitted block; all retained blocks preserve original control/data/failure structure through same-shaped facades; crops are source-cited and locally tested; formal closure passes. No unneeded historical feature is restored merely to raise line similarity. |
| Original Owner Request | “允许裁剪掉暂时没有联通的部分，但是联通的部分不能大规模 diverge。” |
| Similar-Issue Sweep | Adapter logic leaking into mirrors, partial-block crops, replacement data structures, private Bochs access, hidden host-pointer state and untested original failure paths. |

**S8 P2 complete:** original module/algorithm comments and the reached
direct-access source form are restored for all four host subsets; the four
formal MSVC object targets rebuilt cleanly. The exact retained/deferred split
is recorded in the [P2 recovery
evidence](etc/evidence/m0-t263-s8-p2-retained-host-comment-and-block-recovery-001.md).

**S8 P3 complete:** a fresh formal graph rooted at `build/M0-T263-S8/r001`
linked and ran the focused `opennt-host` mirror fixture with exit `0`. It
exercises the retained path/error helpers, static provider lifecycle, and a
local named-pipe round trip without relying on the unrelated VDMREDIR mailslot
fixture branch. See the [P3 focused-closure
evidence](etc/evidence/m0-t263-s8-p3-focused-host-mirror-fixture-001.md).

**S8 closure:** the four-file repair now satisfies its exit criteria. The
`nt_error.c` composed BOP 59 route and the direct VDMREDIR mirror fixture both
exit `0`; all retained local divergences are marked in source and indexed in
`src/opennt-host/README.md`. Deferred VDD/remote/mailslot work remains out of
scope, source-cited and owner-bound rather than being represented as a failure
of the restored synchronous host subset.

**S8 P4 complete:** the owner-required source-layout sweep restored original
whitespace/layout wherever a retained line has no semantic divergence, and
removed historical includes or optional diagnostics that were merely present
but not actually connected. The resulting exact/semantic/cropped split and
fresh focused fixture result are recorded in the [P4 formatting and dependency
evidence](etc/evidence/m0-t263-s8-p4-mirror-layout-and-reached-dependency-audit-001.md).

**S8 P4 complete:** the owner-required source-layout sweep restored original
whitespace/layout wherever a retained line has no semantic divergence, and
removed historical includes or optional diagnostics that were merely present
but not actually connected. The resulting exact/semantic/cropped split and
fresh focused fixture result are recorded in the [P4 formatting and dependency
evidence](etc/evidence/m0-t263-s8-p4-mirror-layout-and-reached-dependency-audit-001.md).

### M0 T263 S7 — MVDM owner-expansion closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S7`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S7 after completed S1 through S6. |
| Objective | Reconcile all T263 source-preservation imports, component ownership/README registers and default formal-build non-admission. |
| Non-goals | No new runtime feature, BOP implementation, tool invocation, static library, firmware activation or source behavior change. |
| Reference Baseline | T263 S1–S6 evidence; OpenNT Git `5c5b979e`; formal root `build/M0-T262-S5/001`. |
| Files And ABI Surface | T263 closure evidence/history/status and documentation inventory only; no ABI. |
| Applicable Rules | Execution closure, source policy, original mirror rule, production-only roots, no generic component and formal-build hygiene. |
| Verification | Per-packet SHA-256 records, source-owner scan, README register review, `ninja -C build/M0-T262-S5/001 -n`, documentation governance and scoped diff checks. |
| Expected Markers | Four admitted production package groups plus one non-component tool closure, six original component registers, no added formal object/archive/link owner. |
| Asset Needs | Pinned OpenNT source, all T263 evidence, existing formal Ninja root and Git metadata. |
| Reporting Requirements | State exact admitted counts, build-boundary result and deferred original owner packages; ensure dormant host extension packages remain outside production components. |
| Stop Conditions | Missing owner/provenance, unregistered divergence, a generic-root leak or accidental new build edge pauses closure. |
| Exit Criteria | Every admitted package has exact source/hash evidence and named owner; no new component crosses the mechanical/runtime boundary; closure history is recorded. |
| Original Owner Request | “将以上架构修改，进行治理文档修正，并开始着手建立 opennt-guest 完整组件、opennt-softpc、opennt-utils 组件，并做好 tools/opennt 工具增补。” |
| Similar-Issue Sweep | Missing package provenance, source mirror mixed with tests/reference copies, accidental runtime linkage, generic utility growth and firmware/runtime conflation. |

**S7 P1 completed:** T263 package counts, owner registers and the unchanged
formal graph boundary are reconciled in the [closure evidence](etc/evidence/m0-t263-s7-mvdm-owner-expansion-closure-001.md).

**S7 P2 completed:** the mistakenly preserved-but-unconnected VDD/debug/FAX
packages have been withdrawn from `opennt-host`; their future owner remains
recorded without treating source preservation as production admission. See the
[withdrawal record](etc/evidence/m0-t263-s7-unconnected-host-package-withdrawal-001.md).
T263 is closed; see the [history record](history/m0-t263-closure-20260824.md).

### M0 T263 S6 — OpenNT `tools16` historical tool-closure admission

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S6`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S6; owner classified `tools16` under non-component `tools/opennt`. |
| Objective | Preserve and source-verify the complete original `tools16` package as a potential guest-build tool closure. |
| Non-goals | No modern-host link, Ninja edge, implicit tool invocation, guest rebuild or migration of unrelated historical tool material. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\tools16`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | Seven original historical tool inputs, tool README, S6 evidence and status only; no product ABI. |
| Applicable Rules | Source policy, historical-tool isolation, guest build provenance and no-runtime-link rule. |
| Verification | File count plus SHA-256 equality for all seven files; formal graph remains unchanged. |
| Expected Markers | Seven exact inputs; zero runtime consumers; no Ninja graph expansion. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | Record the deferred guest-build consumer and distinguish `tools/opennt` from unrelated historical tool material. |
| Stop Conditions | Hash/path mismatch, tool linked into modern runtime, unapproved invocation or unrelated tool migration pauses the packet. |
| Exit Criteria | Package is exact, README/evidence agree and no runtime/build behavior is inferred. |
| Original Owner Request | “`tools16`：历史编译工具资料…属于 tools/opennt，不是组件。” |
| Similar-Issue Sweep | Historical tool mistaken for host runtime input, hidden tool invocation, provenance collision and broad historical-directory cleanup. |

**S6 P1 completed:** all seven `tools16` files are source-identical and
SHA-256 verified. See the [admission evidence](etc/evidence/m0-t263-s6-opennt-tools16-closure-admission-001.md).

### M0 T263 S5 — OpenNT optional host extension-package preparation

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S5; owner classified VDD, debugger and fax extension packages as `opennt-host`. |
| Objective | Retain qualifying original host product packages source-identically with their original internal paths and an explicit non-link disposition. |
| Non-goals | No VDD/debugger/fax/WOW runtime capability, default link, product-shell recreation, adapter creation or BOP implementation. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\{bde,dbg,fax,ieuvddex,vdd,vdmdbg,vdmexts}`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | 138 source/input files across seven original host packages, host README, S5 evidence and status only; no ABI change. |
| Applicable Rules | Source policy, original host mirror rule, optional-provider ownership and no preemptive adapter rule. |
| Verification | Recursive source-relative counts and SHA-256 equality for all seven packages; no added Ninja edge. |
| Expected Markers | 138 exact files, zero new `HOST-DIV` entries, zero default link/runtime capability claim. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | Record package counts, exact owner and explicit absence of a capability claim. |
| Stop Conditions | Hash/path mismatch, host package mixed into a mechanical adapter, source change without register, new default link or runtime claim pauses the packet. |
| Exit Criteria | All seven original packages are exact and indexed, host README/evidence agree, and no product behavior expands. |
| Original Owner Request | “`vdd`、`ieuvddex`、`fax`…属于 opennt-host；`bde`、`dbg`、`vdmdbg`、`vdmexts`…同理。” |
| Similar-Issue Sweep | Debug source mistaken for Bochs logic, VDD package auto-enable, new generic adapter, host-source divergence and implicit link drift. |

**S5 P1 superseded:** the seven original host extension packages were verified
source-identical, but the owner subsequently ruled that unconnected packages
must not reside in production `opennt-host`. They were withdrawn in T263/S7
P2; their earlier provenance record remains
[historical evidence](etc/evidence/m0-t263-s5-opennt-host-extension-package-admission-001.md).

### M0 T263 S4 — OpenNT reusable utility-package decision

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S4; MVDM census identifies `suballoc` as a reusable original utility and rejects a bulk `vdmutils` move. |
| Objective | Import the complete source-identical `suballoc` package into `opennt-utils` and record its original consumers and deferred compilation boundary. |
| Non-goals | No new allocator, modern replacement, XMS/DPMI implementation, static library, default link or mixed `vdmutils` import. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\suballoc`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | Eight original utility files, component README, S4 evidence and status; no product ABI. |
| Applicable Rules | Source policy, source-first utility ownership, original mirror zero-difference rule and no generic common root. |
| Verification | Same-relative-path count plus SHA-256 equality for all eight files; source consumer scan; no Ninja graph change. |
| Expected Markers | Eight exact files; zero exceptions; `xms.486`/`dpmi32` recorded as future original consumers. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | Record the exact split decision for `vdmutils` as well as `suballoc` provenance. |
| Stop Conditions | Any utility without a named original consumer, mixed-product bulk import, local edit or build/link expansion pauses the packet. |
| Exit Criteria | `suballoc` is exact and indexed; `vdmutils` has explicit per-product disposition; no substitute allocator is introduced. |
| Original Owner Request | “`suballoc`、`vdmutils`：历史辅助库和独立工具…缺少一个 opennt-utils 的组件。” |
| Similar-Issue Sweep | Generic common-code growth, XMS/DPMI semantics moved into utilities, inferred runtime link and mixed guest/host product ownership. |

**S4 P1 completed:** all eight `suballoc` files are source-identical and
SHA-256 verified; mixed `vdmutils` remains explicitly split. See the
[admission evidence](etc/evidence/m0-t263-s4-opennt-utils-suballoc-admission-001.md).

### M0 T263 S3 — OpenNT SoftPC firmware/ROM initial mirror

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S3; owner approved `softpc.new/bios` and `roms` as the new component's first package boundary. |
| Objective | Import and source-verify the original firmware/ROM source/input packages below `opennt-softpc`. |
| Non-goals | No historical SoftPC runtime, Bochs change, adapter behavior, BIOS activation, default load, static library or formal-build edge. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\softpc.new`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | 27 `bios` and 8 `roms` source/input files, component README, S3 evidence and status only; no host ABI. |
| Applicable Rules | Source policy, original mirror zero-difference/README register rule and Bochs/OpenNT hard boundary. |
| Verification | Recursive same-relative-path count plus SHA-256 equality for all 35 files; no local divergence. |
| Expected Markers | 35 exact files, zero local exceptions and zero Ninja graph expansion. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | State source package counts, opaque-input boundary and absence of a machine-runtime assertion. |
| Stop Conditions | Hash/path mismatch, unregistered source change, machine semantic change, `bx-machine` OpenNT dependency or default link admission pauses the packet. |
| Exit Criteria | Both complete original packages are exact, README/evidence agree and architecture boundary remains unchanged. |
| Original Owner Request | “`softpc.new/bios`、`roms`：属于机器固件/装配输入…应该放在 opennt-softpc 模块。” |
| Similar-Issue Sweep | Firmware mistaken for an emulator, default BIOS activation, OpenNT naming leaked into machine/core and build-input provenance drift. |

**S3 P1 completed:** 27 `bios` plus 8 `roms` files were copied
source-identically and recursively SHA-256 verified. See the [admission
evidence](etc/evidence/m0-t263-s3-opennt-softpc-firmware-rom-admission-001.md).

### M0 T263 S2 — OpenNT `bin86` guest-package admission

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S2; owner classified `bin86` as guest-owned. |
| Objective | Import the complete source-identical original `base/mvdm/bin86` package below `opennt-guest` and record its source/hash identity. |
| Non-goals | No driver/configuration auto-loading, guest behavior change, host provider, build-graph expansion or source edit. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\bin86`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | Ten guest companion/input files, guest README, S2 evidence and status only; no host ABI. |
| Applicable Rules | Source policy, guest mirror zero-difference rule, production-only root rule and artifact/input provenance rule. |
| Verification | File count plus SHA-256 equality for every source/target pair; no local divergence row. |
| Expected Markers | Ten exact files; zero guest-source divergence entries; zero formal-Ninja edges added. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | Record every file role and state that runtime use remains separately admitted. |
| Stop Conditions | Missing/hash-mismatched file, non-guest owner, local edit or accidental runtime-link input pauses the packet. |
| Exit Criteria | Complete package is source-identical, README/evidence agree and no default runtime behavior changes. |
| Original Owner Request | “`bin86` => 如果属于 opennt-guest，应予搬迁。” |
| Similar-Issue Sweep | Configuration input mistaken for an auto-loaded boot file, host-provider leakage, source/build companion loss and artifact provenance drift. |

**S2 P1 completed:** all ten `bin86` files were copied source-identically and
SHA-256 verified. See the [admission evidence](etc/evidence/m0-t263-s2-bin86-guest-input-admission-001.md).

### M0 T263 S1 — OpenNT MVDM owner/provenance census and component-root admission

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner approved the addition of complete `opennt-guest`, `opennt-softpc`, `opennt-utils` and `tools/opennt` recovery, following the MVDM ownership discussion. |
| Objective | Establish a reproducible owner/provenance census, ratify the 12-component architecture and create empty production-only roots with local exception registers. |
| Non-goals | No wholesale MVDM import, BOP implementation, Bochs semantic change, guest behavior change or default Ninja-link expansion. |
| Reference Baseline | `O:\\repos.external\\opennt` Git `5c5b979e`; OpenNT-4.5 Git `98bd914d2`; source policy; T262 mirror closure. |
| Files And ABI Surface | Architecture/coding rules, T263 proposal/evidence, new component/tool READMEs and status only; no product ABI. |
| Applicable Rules | Source-first recovery, original mirror/README exception rules, production-only component roots and formal-build hygiene. |
| Verification | 24-package MVDM census, source identity check, component owner scan, documentation governance and scoped diff checks. |
| Expected Markers | Six original-code component READMEs; 12 named component owners; zero source bodies admitted by skeleton creation. |
| Asset Needs | Pinned OpenNT and OpenNT-4.5 comparison trees and Git provenance. |
| Reporting Requirements | Record every top-level MVDM package's owner/disposition and identify the next per-package import steps. |
| Stop Conditions | An import without unique owner/provenance, an unregistered mirror divergence, a generic catch-all component, or a new default link dependency pauses the packet. |
| Exit Criteria | Census/evidence, proposal, architecture/rule updates and empty roots agree; every future import has an explicit packet and owner. |
| Original Owner Request | “建立 opennt-guest 完整组件、opennt-softpc、opennt-utils 组件，并做好 tools/opennt 工具增补。” |
| Similar-Issue Sweep | Misowned `mvdm` files, firmware mistaken for a second emulator, utility catch-all growth, host-extension auto-linking and historical tools leaking into runtime. |

**S1 P1 completed:** the 24-package MVDM provenance census, 12-component
ownership model and empty component/tool roots are recorded in the
[S1 evidence](etc/evidence/m0-t263-s1-mvdm-owner-provenance-census-001.md).

### M0 T262 S5 — cross-component final mirror audit and formal-build regression

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T262 proposal S5, after completed S2, S3 P2 and S4. |
| Objective | Re-run the three mirror classifications and local exception-index checks, then source-build the formal nine-module graph without changing component boundaries. |
| Non-goals | No BOP/provider implementation, no Bochs semantic edit, no guest rebuild, no host-subset expansion, no cleanup of unrelated historical tools or artifacts. |
| Reference Baseline | Pinned Bochs 2.6; OpenNT and OpenNT-4.5 guest roots; named OpenNT host paths; S2/S3/S4 evidence and `tools/build/t260-s8-component-manifest.json`. |
| Files And ABI Surface | The three component trees, their READMEs, T262 evidence/status/history and the disposable formal Ninja root only; no product ABI change. |
| Applicable Rules | Mirror classification/marker/register rules, source policy, formal-build hygiene and documentation-governance closure rules. |
| Verification | Full hash/origin scans; bx-core local-marker scan; host local-marker/README set equality; new `build/M0-T262-S5/001` formal Ninja graph; #UD boundary and focused host fixtures; documentation governance and scoped diff checks. |
| Expected Markers | 164/18/0 bx-core exact/edit/non-mirror classification; 678/0 matched/unmatched guest inputs; 7/4/0 host exact/subset/extra classification; equal HOST-DIV code/README sets. |
| Asset Needs | Pinned Bochs/OpenNT trees, the component manifest, Ninja/MSVC x64 `/MT`, focused fixture executables and Git metadata. |
| Reporting Requirements | Record exact audit totals, build root/edge result, passed fixtures and any failed fixture with an owner-qualified limitation. |
| Stop Conditions | An unmatched path, unmarked/local-register drift, component-boundary change, or source-build failure pauses closure. |
| Exit Criteria | Every production file has an allowed mirror classification; no component has an unregistered exception/non-mirror file; formal graph compiles, archives and links; evidence records any unrelated runtime fixture limitation without misclassifying it as a mirror defect. |
| Original Owner Request | “审核已知的3个镜像包： bx-core, opennt-guest, opennt-host…所有子集文件或者少部分修改的点位，需要在本文件中加入注释，并且README.md提供该例外的索引。” |
| Similar-Issue Sweep | Imported guest binaries/objects, source-root version collision, local README/register drift, stale component paths, build-output leakage and fixture/environment conflation. |

**S5 P1 completed:** 182 `bx-core` source files classify as 164 exact plus
18 locally indexed Bochs edits; all 678 `opennt-guest` inputs match an OpenNT
or OpenNT-4.5 peer; `opennt-host` remains seven exact plus four
marker/index-equal subsets.  A fresh formal graph completed all 469 edges and
emitted 80 executables.  The #UD boundary and BOP59 fixtures pass.  The
Redirector mailslot fixture returns its pre-existing environment-sensitive
exit `8` in both the new and prior formal roots, which is recorded rather than
misattributed to the mirror task.  See the [S5 closure evidence](etc/evidence/m0-t262-s5-final-component-mirror-and-build-closure-001.md).

### M0 T262 S4 — opennt-host subset and minimal-edit closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T262 proposal S4; T262/S1 identified four original-host subsets/minimal edits and 13 existing HOST-DIV rows. |
| Objective | Prove one-to-one code-marker/README coverage for every non-exact `opennt-host` file, while retaining exact files byte-identical and excluding non-mirror additions. |
| Non-goals | No provider behavior, BOP route, Win32 facade, SoftPC seam or historical source expansion. |
| Reference Baseline | T261/S12 header closure, T262/S1 ledger, `refs/opennt` source paths, and `opennt-host/README.md`. |
| Files And ABI Surface | `nt_error.c`, `vrinit.c`, `vrnmpipe.c`, `vrputil.c`, exact host headers/bodies, README and evidence/status only. |
| Applicable Rules | Reachable original subsets retain original order; each omitted/replaced expression has local `DIVERGENCE(<ID>)`; each ID has exactly one README row. |
| Verification | Hash exact files; enumerate marker IDs per subset; compare every README HOST-DIV ID to source; negative scan for non-mirror files/compat headers; focused host fixtures remain linked. |
| Expected Markers | Seven exact files, four subset/edit files, 13 matching HOST-DIV entries, and zero non-mirror host files. |
| Asset Needs | Current host mirror, original OpenNT files and existing S12 fixture graph. |
| Reporting Requirements | Report one-to-one marker/index result and preserve any unresolved source span as a later owner dependency rather than silently omitting it. |
| Stop Conditions | A README ID lacks a local marker, a local marker lacks a README row, or a subset contains an unmarked project-authored branch. |
| Exit Criteria | All host subset/edit deviations are locally marked/indexed and exact files remain hash-identical. |
| Original Owner Request | “所有子集文件或者少部分修改的点位，需要在本文件中加入注释，并且README.md提供该例外的索引。” |
| Similar-Issue Sweep | `*_compat` names, static-DLL selectors, TLS/dialog state, VDMREDIR lifecycle, private-header replacements and test-only declarations. |

**S4 P1 completed:** exact-host hashes, subset marker IDs, README IDs and
production-tree inventory agree one-to-one. See the [S4
closure](etc/evidence/m0-t262-s4-opennt-host-subset-marker-closure-001.md).

### M0 T262 S3 — opennt-guest zero-difference mirror closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T262 proposal S3 and the owner-approved mirror standard; T262/S1 proves `sysmac.lib` is exact OpenNT material but is untracked. |
| Objective | Make the `opennt-guest` mirror complete and versioned while preserving byte-identical DOS/V86 and WOW16 source/input trees and their original deployable binaries. |
| Non-goals | No guest source edit, guest rebuild, artifact replacement, toolchain change, packaging policy or use of unrelated historical tools. |
| Reference Baseline | T262/S1 hash audit; `refs/opennt/base/mvdm/dos/v86` and `refs/opennt/base/mvdm/wow16`; `opennt-guest/README.md`. |
| Files And ABI Surface | The exact `sysmac.lib` input plus owner-copied original OpenNT DOS/V86 `*.COM`, `*.EXE`, `*.SYS`, `*.BIN`, `*.LIB`, `*.MAP`, `*.SYM` and `*.OBJ` inputs under the guest source root; guest README, T262 evidence/status and documentation inventory only. |
| Applicable Rules | Original guest inputs are byte-identical; zero source changes means zero local divergence entries; artifact provenance remains explicit. |
| Verification | Full guest-path hash comparison, direct hash proof for `sysmac.lib` and every owner-copied binary, build library and MAP/SYM companion, Git tracked-file check, no `DIVERGENCE` marker scan, documentation governance and scoped diff check. |
| Expected Markers | 580/580 source/input paths and all 99 admitted DOS/V86 package/build inputs match OpenNT or OpenNT-4.5 and are tracked; no non-mirror or modified guest file exists. |
| Asset Needs | Existing OpenNT reference tree, Git index and current guest README. |
| Reporting Requirements | Record `sysmac.lib` source path/hash, distinguish it from unrelated untracked artifacts, and state that the empty divergence register remains correct. |
| Stop Conditions | A byte mismatch, unknown binary provenance, or a build-generated file masquerading as original input. |
| Exit Criteria | All guest files are exact/recorded mirrors, the sole untracked original input is governed, and no guest exception row is needed. |
| Original Owner Request | “大部分文件应当1-1一致；部分文件可以是原版文件的子集，或者是少部分修改；所有子集文件或者少部分修改的点位，需要在本文件中加入注释，并且README.md提供该例外的索引。” |
| Similar-Issue Sweep | DOS/V86 and WOW16 source roots, untracked binary/source-build inputs, build-output leakage and README exception count. |

**S3 P1 completed:** all 580 guest files hash-match OpenNT; the exact
`sysmac.lib` build input is now governed as a mirror file, with no guest
source edit or local exception. See the [S3 evidence](etc/evidence/m0-t262-s3-opennt-guest-zero-difference-closure-001.md).

**S3 P2 completed:** the owner-copied DOS/V86 package binaries, build
libraries, objects and MAP/SYM companions are each byte-identical to an
OpenNT or OpenNT-4.5 source-root peer; their 99-file provenance ledger is
[S3 binary-import evidence](etc/evidence/m0-t262-s3-opennt-guest-binary-import-001.md).

### M0 T262 S2 — bx-core local-exception and non-mirror cleanup

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T262/S1 audit plus owner direction that non-mirror files “不应该存在于该组件内，我们需要逐个审核并决定它们的去向.” |
| Objective | Remove the unused project-authored the former project-specific exception-intercept header from `bx-core`, and make every retained modified Bochs file locally marked and individually indexed in `bx-core/README.md`. |
| Non-goals | No change to any Bochs executable statement, feature predicate, include dependency, build target, runtime result, or central exception decision. |
| Reference Baseline | [T262/S1 audit](etc/evidence/m0-t262-s1-original-component-mirror-audit-001.md), the pinned Bochs 2.6 tree, and the existing central external-intrusion register. |
| Files And ABI Surface | The 18 audited `src/bx-core/cpu/*` and `memory/*` files, `src/bx-core/README.md`, the obsolete header and its focused boundary fixture, T262 evidence/status. |
| Applicable Rules | `bx-core` is original Bochs with only registered minimum intrusions; every retained changed expression carries a nearby `DIVERGENCE(<ID>)`; a component README indexes each source file/exception; non-mirror files leave the component. |
| Verification | Re-run file hash comparison, prove the obsolete header has no consumer before deletion, scan all non-identical files for local marker coverage, compare README file rows with diff files, run the existing #UD boundary fixture and formal affected Ninja closure. |
| Expected Markers | Zero unmatched non-mirror `bx-core` files; 18/18 modified files have an explicit local marker and README index; 164 unchanged files retain exact source hashes. |
| Asset Needs | Pinned Bochs 2.6 source, central exception register, existing #UD boundary fixture and formal Ninja manifest. |
| Reporting Requirements | State whether each marker covers an existing central exception or a newly registered documentation-only index; report deletion of the obsolete header and every changed fixture path. |
| Stop Conditions | Any purportedly unused header has a generated/indirect consumer, or any required marker would need a new CPU semantic exception rather than documenting an existing one. |
| Exit Criteria | The obsolete non-mirror header is deleted or relocated to a proven owner; all 18 changed Bochs files have local markers and exact README rows; focused boundary verification and formal closure pass. |
| Original Owner Request | “大部分文件应当1-1一致；部分文件可以是原版文件的子集，或者是少部分修改；所有子集文件或者少部分修改的点位，需要在本文件中加入注释，并且README.md提供该例外的索引。” |
| Similar-Issue Sweep | Generated/include-only references, test-only headers, central/register drift, observation-only code, segment-limit profile branches, copied #UD state, and memory lifecycle helpers. |

**S2 P1 completed:** the zero-consumer non-mirror header is deleted; all 18
retained modified Bochs files have local markers and README rows. The focused
boundary test and a 130-edge formal Ninja relink pass. See the [S2
closure](etc/evidence/m0-t262-s2-bx-core-local-exception-closure-001.md).

### M0 T262 S1 — original-component mirror conformance audit

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner: “审核已知的3个镜像包： bx-core, opennt-guest, opennt-host…审核并治理所有涉及的代码文件和README并汇报。” Owner clarification: non-mirror files do not belong in an original component and require an individual destination decision. |
| Objective | Establish a complete, reproducible per-file mirror ledger and repair plan for the three original components: exact original, original subset, original with minimal edits, or non-mirror relocation/delete. |
| Non-goals | No BOP feature work, no Bochs semantic expansion, no source import merely to raise file counts, no modification to an original file before its source/diff and owner disposition are recorded. |
| Reference Baseline | Bochs 2.6 at `O:\repos.external\bochs-2.6-compat\bochs-2.6`; OpenNT at `refs/opennt`; T261/S12 exact-host-header closure; component READMEs and external-intrusion register. |
| Files And ABI Surface | `src/bx-core/**`, `src/opennt-guest/**`, `src/opennt-host/**`, their three READMEs, one audit evidence ledger, and the T262 proposal/status governance only. |
| Applicable Rules | Source-first recovery, original-component ownership, statement-local `DIVERGENCE(<ID>)`, README one-to-one exception index, and no non-mirror product file under an original-component root. |
| Verification | Hash every retained file against its recorded origin; inspect every non-identical diff hunk and marker; compare README IDs against local markers; enumerate Git-untracked production inputs; run governance and scoped diff checks. |
| Expected Markers | A ledger with every file's origin/disposition; an explicit list of all 18 current bx-core modifications and its one non-mirror header; zero-difference guest result; host subset/exception matrix. |
| Asset Needs | Existing Bochs/OpenNT source roots, component READMEs, `adapter-external-intrusion-exceptions.md`, Git status and formal source-tree inventory. |
| Reporting Requirements | Distinguish observed facts from planned remediation; every non-mirror file gets one of move-to-owner, merge-into-original, delete, or retain only after a new architecture decision. |
| Stop Conditions | An unmatched source identity, a change that would alter machine/BOP semantics, a proposed new adapter, or a non-mirror file with no safe owner pauses implementation and is recorded for owner decision. |
| Exit Criteria | All three component trees have a complete file-level ledger; every exception has an origin, local-marker/README status and a concrete remediation disposition; no audit result is inferred from directory naming alone. |
| Original Owner Request | “对于无法和原始文件完全一致的：镜像文件可以是原始文件的子集；不同的实现部分可以存在但是要加注释并标记例外。” |
| Similar-Issue Sweep | New headers, imported build inputs, untracked guest files, generated artifacts, `*_compat` paths, central versus component exception registers, include-path-only edits and source subsets. |

**S1 baseline observation:** 164/183 non-README `bx-core` files are
byte-identical to the pinned Bochs source; 18 differ and
the former project-specific exception-intercept header is non-mirror. All 580 `opennt-guest`
files match their OpenNT path byte-for-byte, including one untracked original
input. `opennt-host` has seven exact files and four declared source
subsets/minimally edited files. The detailed ledger is the required first P.

**S1 P1 completed:** the complete per-file result and remediation disposition
are recorded in the [T262 mirror audit](etc/evidence/m0-t262-s1-original-component-mirror-audit-001.md).
The audit admits no source alteration: `bx-core` marker/non-mirror repair is
S2, the guest input decision is S3, and host hunk validation is S4.

### M0 T261 S12 — closed: OpenNT-host original-header and mirror replacement

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S12`, Ordinary Mode with a single-person dual-role review. |
| Objective | Eliminate project-authored `nt_error_compat.h`, `vrputil_compat.h` and `vrnmpipe_compat.h` from `opennt-host` by recovering their original OpenNT header paths, names and applicable content. |
| Scope | `opennt-host` header/source/include users, `adapter-mvdm-host-out/softpc` mis-rooted exact header, `adapter-mvdm-host-out/win32` direct-access UI state, BOP consumers, fixture-only controls, build manifest and local README divergence register. |
| Non-goals | No new BOP capability, no full historical CSR/VDD/ICA/SoftPC shell, no Bochs change, no unbounded pointer/handle state and no generic compatibility component. |
| Required Direction | A mirror may be a reachable original-file subset when the complete translation unit cannot compose. Each retained line/record/order remains original; every omitted/unavailable/replaced expression is statement-local `DIVERGENCE(<ID>)` and has one `opennt-host/README.md` row. A new declaration belongs only to its real adapter/session/test owner, never an OpenNT-mirror path. |
| Verification | Exact hash comparison for full imported headers; source-span/exception ledger for a subset; include and old-path negative scans; original ABI assertion for `VrTerminateNamedPipes(WORD DosPdb)`; focused BOP59/VDMREDIR fixtures; formal Ninja closure, documentation governance and `git diff --check`. |
| Exit Criteria | No `*_compat.h` remains below `src/opennt-host`; all consumers include original header paths or their actual non-original owner; original header/subset and every divergence are one-to-one registered; formal closure and focused tests pass or an explicitly owned non-host failure is reproduced. |
| Admission | Owner direction: “要求你增开一个S任务完成此事。” |

**S12 P1 completed:** exact OpenNT `error.h`, `vrputil.h`, `vrnmpipe.h` and
the newly reached `oemuni.h` now replace all three project-authored host
compatibility headers.  The BOP59 fixture passes after non-original dialog
state moved to `adapter-mvdm-host-out/win32`.  A fresh `002` formal Ninja graph compiled,
archived and linked both focused fixtures; BOP59 and the Redirector fixture
each return `0`.  Full details and exact hashes are in the [S12
evidence](etc/evidence/m0-t261-s12-opennt-host-original-header-replacement-001.md).

**M0 T261 closed:** all twelve admitted source-layout packets are complete.
The resulting `opennt-host` tree has no project-authored compatibility header;
it retains only re-rooted original source, declared reachable subsets, and
README-registered statement-level divergences. The final closure record is in
[M0 T261 history](history/m0-t261-closure-20260824.md).

### M0 T261 S11 — closed: final OpenNT-host manifest, include and exception closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S11`, Ordinary Mode with a single-person dual-role review. |
| Objective | Prove that the final production `opennt-host` layout, public includes, static-library manifest and local exception register are coherent as one original-source-first build closure. |
| Scope | `src/opennt-host`, its README, current consumers/includes, component manifest/Ninja graph, all reached focused fixtures and S1/S8/S10 evidence; recover original `error.h`, `vrputil.h` and `vrnmpipe.h` rather than retaining project-authored compatibility headers. |
| Non-goals | No new host provider/BOP capability, no configuration or machine feature, no Bochs change, no import merely to increase source count, and no VDD/CSR/WOW product-shell recovery. |
| Required Direction | Each `opennt-host` production body and header is either a re-rooted original OpenNT file or carries one statement-level README exception. No `*_compat.h` may replace an existing original OpenNT header; modern BOP/fixture/session state belongs to its actual adapter, `session` or test owner. The formal manifest may compile only the admitted reached host bodies; exact retained originals remain production source but are not falsely claimed as reached. |
| Verification | One-to-one source/README divergence audit, include/old-path/manifest scan, full existing-Ninja graph execution, direct host fixtures and explicit classification of cross-owner fixture tails, documentation governance and `git diff --check`. |
| Exit Criteria | All production `opennt-host` paths meet the re-rooted-original or explicit-exception rule; original `error.h`, `vrputil.h` and `vrnmpipe.h` replace the three project-authored compatibility headers; manifest/includes/tests agree; no generic or unclassified host shim remains; the formal graph passes; direct host portions pass or a later `opennt-bop`/machine tail is explicitly separated. |
| Admission | T261 proposal S11, after S10 residual-source closure. |

**S11 P2 completed:** the formal graph executed all pending
affected compile/link work and ended with `ninja -n all` reporting no work;
the final closure compiled no `bx-core` source. Documentation governance,
component-manifest, production-tree and generic-UD boundary gates pass. The
technical-closure record distinguishes the two remaining cross-owner fixture
tails from host-layout evidence: [T261 technical closure](etc/evidence/m0-t261-s11-final-technical-closure-20260824.md).

**S11 P3 superseded by S12:** the owner rejected the interim
`nt_error_compat.h`, `vrputil_compat.h` and `vrnmpipe_compat.h` headers. The
prior technical-closure evidence is superseded as a final source-fidelity
claim; S12 imports their original OpenNT header counterparts, restores
original ABI declarations, and moves non-original BOP choice/fixture state to
its actual owner before rebuilding the affected closure.

**S11 P1 completed:** the final one-to-one audit finds three byte-identical
re-rooted PIF files; four reached original host C bodies; three include-only
same-shaped compatibility declarations; and exactly the README-registered
`HOST-DIV-012..023` edits. The refreshed formal graph compiles no `bx-core`
source. PIF and BOP59 fixtures pass; the Redirector mailslot (`exit=8`) and
configuration DASD/FDC (`exit=2`) fixture tails are explicitly classified as
separate `opennt-bop`/machine work, not host closure. See the
[S11 audit](etc/evidence/m0-t261-s11-opennt-host-final-manifest-exception-audit-001.md).

### M0 T261 S10 — closed: configuration and residual host-source-truth closure

S10 proves that no configuration or unclassified host shim remains. Its
reached `UMBNotify` fragment preserves the original no-op ABI with one
registered `/W4 /WX` adjustment; it does not represent UMB allocation or a
machine feature. See the [S10 audit](etc/evidence/m0-t261-s10-configuration-residual-source-truth-audit-001.md).

### M0 T261 S10 — historical packet

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S10`, Ordinary Mode with a single-person dual-role review. |
| Objective | Close the proposal's residual configuration and later-discovered host-source-truth check without enabling a new provider or recreating a product shell. |
| Scope | The S1/S2 host ledger, `src/opennt-host`, the reached configuration-complete `UMBNotify` fragment and its BOP-owned caller, README exception registration, formal build manifest and focused fixture. |
| Non-goals | No UMB allocator, FDC/DASD, VDD/ICA/CSR product shell, new BOP enablement, Bochs change, or generic configuration adapter. |
| Required Direction | Configuration route meaning remains in `opennt-bop`; the original empty `UMBNotify` body must remain source-identifiable and must not be presented as UMB allocation/device setup. Host-only source rows use the same four-rung disposition as S8. |
| Verification | Full current host-tree/manifest scan, source comparison with `nt_umb.c`, direct caller review, focused configuration fixture, refreshed formal Ninja target, documentation governance and `git diff --check`. |
| Exit Criteria | No generic configuration or unclassified host shim remains; every discovered row has a direct original path, statement-level registered divergence, declared adapter seam, or explicit later-owner disposition. |
| Admission | T261 proposal S10, following S9 closure. |

**S10 P1 completed:** the residual host-tree and configuration audit finds
eleven live `opennt-host` files, no generic configuration root or `*_shim`
path, and only the README-registered `HOST-DIV-012..023` source edits. The
reached `UMBNotify(UCHAR)` fragment now preserves its original signature and
empty result with the sole MSVC `/W4 /WX` `(void)code` adjustment
(`BOP-DIV-081`). The incrementally rebuilt configuration fixture reproduces
its prior `exit=2`: its later original `demDasdInit` invokes the explicitly
unimplemented FDC/INT13 owner. That cross-owner tail is not a configuration
or host-layout success claim. See the [S10 audit](etc/evidence/m0-t261-s10-configuration-residual-source-truth-audit-001.md).

### M0 T261 S9 — closed: adapter production-root semantic-residue cleanup

S9 moved every admitted source-owned INT06/DEM/DPMI/BOP interpretation to
`opennt-bop`, deleted fixture-only production providers, and left both
mechanical adapter roots selector/service/family/DOS/WOW blind. Its final
evidence is the [S9 residue closure audit](etc/evidence/m0-t261-s9-final-adapter-residue-closure-001.md).

### M0 T261 S9 — historical packet

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S9`, Ordinary Mode with a single-person dual-role review. |
| Objective | Remove selector-aware BOP/NTDOS/COMMAND observation and fixture-only residue from adapter production roots; retain all source-owned interpretation in `opennt-bop` and generic test hooks in `tests/`. |
| Scope | `adapter-mvdm-host-in` and `adapter-mvdm-host-out/softpc` production sources, their public headers, build inputs and focused fixtures. |
| Non-goals | No provider semantic change, no BOP enablement, no OpenNT source rewrite, no new adapter, no Bochs change, and no VDD/Redirector lifecycle implementation. |
| Required Direction | The mechanical adapters remain selector/service/family/DOS/WOW blind. `opennt-bop` owns every source/provider interpretation; fixture-only code belongs under `tests/`. |
| Verification | Owner/function map, negative semantic-token scan, focused fixtures, formal Ninja closure, `git diff --check`, and documentation governance. |
| Exit Criteria | No adapter production root owns selector/service/family/DOS/WOW interpretation or a fixture-only provider; all moved interfaces retain their prior fixed-width mechanical contract. |
| Admission | T261 proposal S9, after S8 final audit. |

**S9 P1 completed:** a complete source/consumer owner map is recorded in the
[S9 adapter-residue map](etc/evidence/m0-t261-s9-adapter-residue-owner-map-001.md).
It separates eight BOP-family observation pairs for path-only re-rooting,
three fixture-only observations for production deletion, the DEM/DPMI
source-interpretation splits, and the retained mechanical ABI closure.

**S9 P2 completed:** the eight selector-aware BOP/COMMAND/DEM/NTDOS
observation pairs now live under `opennt-bop/observation`; their route, CLI,
Win32-facade and fixture includes plus formal manifest ownership moved with
them without an ABI or behavior change. A fresh immutable `s9-r001` formal
Ninja graph completed its build. The next P handles only fixture-only
production residue, not the DEM/DPMI semantic split.

**S9 P3 completed:** the three observation implementations used solely by
their own isolated fixtures (`normal_return`, `normal_terminal_sequence` and
`terminal`) have been removed from production, manifest and obsolete fixture
inputs. The direct engine fixture now asserts only the machine-engine terminal
contract it actually owns; it no longer introduces BOP observation state.

**S9 P4 completed:** DEM direct-context/CCPU-SAS source interpretation and
the reached DPMI source closure now live under `opennt-bop/dem` and
`opennt-bop/dpmi`; the old `adapter-mvdm-host-out/softpc` aliases and fixture-only XMEM
record have left production. The cached formal Ninja graph refresh rebuilt the
affected closure without recompiling `bx-core`; focused DEM and DPMI fixtures
pass. The pre-existing VDMREDIR mailslot fixture exit `8` was reproduced from
both pre- and post-move graphs and remains separately recorded as non-P4
behavior evidence.

**S9 P5a completed:** the legacy `50:xx`/`54:xx` host-transaction classifier
had one fixture consumer and no formal production-graph input. It now lives
entirely under that fixture's `tests/.../support/` directory, leaving
`adapter-mvdm-host-out/softpc` without this selector/family knowledge. P5b remains to
isolate the separate generic-UD test controls in `adapter-mvdm-host-in`.

**S9 P5b completed:** generic-UD v2 context injection and the finite-run
UD-stop switch have left production. `adapter-mvdm-host-in` now has only its default
mechanical decline plus selector-blind accepted-outcome observations; the
context fixture uses an explicitly linked test-support object and the
cancellation fixture now supplies its own v1 callback. Its separate
`RUNTIME_CANCELLATION_TESTING` session-library variant is not falsely added
to the ordinary formal graph and remains a later test-governance item.

**S9 P5c completed:** the source-derived OpenNT INT 06 provider and its
`02h`/`06h` dispatcher now live in `opennt-bop`, leaving
`adapter-mvdm-host-out/softpc` with typed RAM/port mechanics only. Its focused provider and
ingress fixtures pass; the final ingress rebuild took two cached Ninja edges,
not a Bochs rebuild. The final ownership scan accepts only historical ABI
headers, source-provenance/negative-boundary comments and selector-blind
same-shaped facades beneath adapter production roots.

### M0 T261 S8 — closed: OpenNT-host original-source exception closure

S8 restored every admitted host exception through its original owner body or
an already-declared same-shaped seam, removed the drive policy and obsolete
host shims, and established statement-level README registration for each
remaining host difference.  The result and deliberate VDMREDIR/VDD lifecycle
boundary are recorded in the [S8 final closure audit](etc/evidence/m0-t261-s8-final-opennt-host-exception-closure-001.md).

### M0 T261 S8 — historical packet

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S8`, Ordinary Mode with a single-person dual-role review. |
| Objective | Replace every currently admitted non-original `opennt-host` exception by its original OpenNT owner body and the smallest same-shaped existing adapter; remove the obsolete drive-filter feature and delete the replaced shims. |
| Scope | Delete include/exclude-drive policy; use the original DEM source path in place of the project namespace; delete the misplaced DEM policy facade; migrate XMS machine mechanics to `adapter-mvdm-host-out/softpc`; import reached VDMREDIR units; import `nt_error.c`; then delete `namespace`, `xms`, `redir`, and top-level shim files as their declared replacements take over. |
| Non-goals | No new BOP family/service, no Bochs semantic change, no new specialized or generic adapter, no virtual boot drive, and no recreation of an NT4 CSR/BaseSrv/CCPU product shell. |
| Reference Baseline | Closed T261/S7 session foundation; `refs/opennt/base/mvdm/dos/dem/`, `vdmredir/`, `xms.486/`, and `softpc.new/host/src/nt_error.c`; the S1/S2 host source-truth ledger. |
| Required Direction | Original BOP meaning stays in `opennt-bop`; original host VDMREDIR/`nt_error.c` bodies belong in re-rooted `opennt-host`; unavailable historical Win32 calls use `adapter-mvdm-host-out/win32`; A20/IVT/RAM/SoftPC mechanics use `adapter-mvdm-host-out/softpc`; neutral lifecycle uses `session`; `app` remains composition owner. |
| Verification | Per-file replacement ledger; original path/name/hash or statement-level divergence proof; route/use-site and negative old-path scans; focused DEM/XMS/Redirector/top-level tests; formal Ninja closure; `git diff --check`; documentation governance. |
| Exit Criteria | No include/exclude-drive policy remains; all host drives use the OpenNT-compatible default projection; the named namespace/XMS/Redirector/top-level shims and misplaced header are gone; original providers and only declared existing-adapter seams are linked and tested; every imported edit is README-registered. |
| Admission | Owner explicitly directed this reprioritization and source replacement. The former adapter-root cleanup is deferred intact to S9. |

**S8 P1 progress:** the self-authored host namespace and all include/exclude
drive-policy production/test inputs have been removed.  The preserved original
DEM paths now directly query present host drives; the unrelated configuration-
complete fixture remains a separately recorded `exit=2` limitation.  See the
[P1 removal evidence](etc/evidence/m0-t261-s8-p1-host-namespace-drive-policy-removal-001.md).

**S8 P2 progress:** XMS source package lifecycle and `52:xx` route composition
now live in `opennt-bop`; its reached CCPU/SAS/A20/IVT/checked-RAM mechanics
are a selector-blind same-shaped `adapter-mvdm-host-out/softpc` facade.  Fresh formal Ninja
root `r003` completed 467/467 edges and both XMS focused fixtures passed; see
the [P2 owner-split evidence](etc/evidence/m0-t261-s8-p2-xms-softpc-owner-split-001.md).

**S8 P3 progress:** the reached `nt_error.c:host_direct_access_error(ULONG)`
is re-rooted under its original OpenNT host path; its three unavoidable NT4
product-shell divergences are registered in `opennt-host/README.md`.  Fresh
formal Ninja root `r004` completed 467/467 edges and the BOP 59 focused fixture
passed; see the [P3 `nt_error` evidence](etc/evidence/m0-t261-s8-p3-nt-error-original-owner-recovery-001.md).

**S8 P4 progress:** Redirector selector/session composition has left
`opennt-host`; the reached original VDMREDIR named-pipe helper surface is
re-rooted under `opennt-host/vdmredir/`.  The fresh uncontended formal Ninja
root `r008` completed 463/463 edges, and Redirector plus declared top-level,
XMS and DEM focused fixtures pass.  The historical VDD/ICA/DLC and DOS-mailslot
product dependencies remain explicitly owned by their original units rather
than being misrepresented as a direct host import; the selector/session
composition remains in `opennt-bop`.  See the [P4 VDMREDIR owner-split
evidence](etc/evidence/m0-t261-s8-p4-vdmredir-owner-split-001.md).

**S8 P5 completed:** P4 did not initially close the original VDMREDIR dispatcher or
mailslot bodies.  The source-owner intake places `vrdisp.c` and the
guest-facing portion of `vrmslot.c` in `opennt-bop/vdmredir/`, with only their
mechanical and modern-API seams below the declared adapters.  Its recovery
order and explicit limitations are recorded in the [P5 VDMREDIR
intake](etc/evidence/m0-t261-s8-p5-vdmredir-source-owner-intake-001.md).
The original `vrdisp.c` table is now the actual bounded `57:xx` route source;
its admitted local provider bodies were subsequently recovered.  The first
direct `vrmslot.c` body, `VrPeekMailslot`, now replaces the former cached-peek
rehost with OpenNT's explicit `ERROR_NOT_SUPPORTED` result; formal `r010`
compiled it into `opennt-bop` and the Redirector fixture passed.  The original
`VR_MAILSLOT_INFO` record/list, token lookup and process-cleanup lifecycle now
replace the former fixed array.  `BOP-DIV-058` retains the one session-owned
opaque handle map rather than duplicating OpenNT's private bitmap; the focused
r011 lifecycle fixture passes.  See the [P5 lifecycle
evidence](etc/evidence/m0-t261-s8-p5-vrmslot-record-lifecycle-recovery-001.md).

**S8 P6 completed:** P5 closes the admitted local `vrmslot.c` body group, but the
live `opennt-host/vdmredir/vrnmpipe.c` and `vrinit.c` files remain reached
surfaces rather than complete original translation units. P6 records each
function's exact direct-recovery or later-package disposition. Its first two
direct bodies, `VrIsNamedPipeName` and `VrConvertLocalNtPipeName`, now retain
original flow and pass their focused formal fixture checks; only the necessary
x64-safe spellings are `HOST-DIV-020` registered. Its original named-pipe
metadata record/list and `VrAdd/RemoveOpenNamedPipeInfo` bodies are now active
too. `VrReadNamedPipe` and `VrWriteNamedPipe` now retain their original
synchronous `OVERLAPPED_PIPE_IO` issue/wait/result/error ordering; only their
VDD raw-pointer cancellation registration and timed-out native-handle
retirement remain `HOST-DIV-015/016` lifecycle divergences. The historical
empty `VrTerminateNamedPipes(DosPdb)` is not misrepresented as the current
session all-record cleanup. VDD/ICA/NetBIOS/DLC and async cancellation
lifecycle are not silently folded into this helper work. See the [P6 host-body
disposition](etc/evidence/m0-t261-s8-p6-vdmredir-host-body-disposition-001.md).
The retained `VrTerminateMailslots` body now also closes the PDB-owned record
cleanup with the original no-AX-write result; its copied-frame/token divergence
is `BOP-DIV-059`.  `VrGetMailslotInfo` now uses its original body, backed by
the re-rooted original `opennt-host/vdmredir/vrputil.c`; fresh r012 completed
314/314 actions and the Redirector fixture passes. `VrDeleteMailslot` is now also direct, retaining original PDB,
ES:DI/DX and no-AX-write behavior; `BOP-DIV-061` replaces only its obsolete
private handle-bitmap release. `VrMakeMailslot` now also retains its original
allocation, creation and record-link order; `BOP-DIV-062` copies its ASCIZ
input through the existing bounded CCPU/SAS facade and `BOP-DIV-063` replaces
only the obsolete private handle bitmap with the shared session mapper.
Incremental formal r012 rebuilt and passed. `VrReadMailslot` now retains its
original `SetMailslotInfo` → `ReadFile` → `GetMailslotInfo` ordering through a
record-sized output lease. `VrWriteMailslot` now retains its original
validation → local-name formation → `CreateFile` → `SetMailslotInfo` →
`WriteFile` → close ordering; its composition predeclares only the exact
eight-byte descriptor and independently addressed source span. The former
hand-written write provider is deleted, and r012 compiles, links and executes
the Redirector fixture successfully. See the [P5 lifecycle
evidence](etc/evidence/m0-t261-s8-p5-vrmslot-record-lifecycle-recovery-001.md).

### M0 T261 S7 — closed: dependency-free session owner split

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S7`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | S6 closed the component-count, ownership and terminology governance. The owner directed that S7, rather than S6, implements the `session` owner split. |
| Objective | Establish `src/session` as the dependency-free per-VDM lifecycle foundation and move only neutral lifecycle, resource/token, capability-registration, completion/event and teardown ownership into it. |
| Non-goals | No BOP selector/provider implementation, no OpenNT host-source rewrite, no Win32 facade semantics, no SoftPC/CCPU or Bochs mechanics change, no guest change, and no preemptive `adapter-mvdm-host-out/vdd`, `adapter-mvdm-host-out/debugger`, `adapter-mvdm-host-out/redir`, `adapter-mvdm-host-out/wow`, generic `adapter-common`, `adapter-host`, or `compat` root. |
| Reference Baseline | T261/S5 closure `ab49a2d4`; current `app`, `adapter-mvdm-host-in`, `adapter-mvdm-host-out/softpc`, `adapter-mvdm-host-out/win32`, `opennt-bop`, and `opennt-host` session-facing sources; T260 mapping-manager boundary. |
| Files And ABI Surface | New `src/session/*`, precisely inventoried neutral state currently held by component composition/adapters, component public declarations, formal module manifest/generator, and focused session fixtures. |
| Applicable Rules | `session` has no product-component dependency and no BOP/DOS/WOW/VDD/Redirector/Win32/Bochs vocabulary; `app` creates and wires the instance; source-owned behavior stays in its original owner; cross-component data remains fixed-width or opaque. |
| Stop Conditions | A candidate extraction requires service/provider/guest interpretation, an OpenNT-body semantic edit, a raw Bochs object or Win32 handle in the public session ABI, an upward import of `app`, or a second mapping-manager implementation. Pause and record the owner conflict. |
| Verification | [S7 session-foundation evidence](etc/evidence/m0-t261-s7-session-foundation-001.md); former/new owner map, source-token scans, focused identity/teardown fixture, formal MSVC x64 Ninja native link, `git diff --check`, documentation inventory/governance verification. |
| Exit Criteria | Closed: `session` builds as a dependency-free ninth host-side module; `app` alone creates and wires the instance; cancellation has one neutral owner; the three adapter-mvdm-host-out/softpc mapping implementations bind to a unique session identity and teardown through its lifecycle; existing source/machine semantics are unchanged; no specialized/generic adapter was introduced. |
| Original Owner Request | “非常好，请你治理好文档，以S6身份提交，我们让S7来实施session逻辑抽离。” |
| Similar-Issue Sweep | Session records, token/handle/event tables, mapping-manager instance lifecycle, app-owned composition callbacks, adapter callbacks, thread/child completion state, build manifest and fixtures. |

### M0 T261 S6 — closed: architecture contract-consistency governance

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S6`, Ordinary Mode with a single-person dual-role review. |
| Objective | Reconcile the newly admitted `session` component across current architecture, goal, coding and rule authorities without moving production code or claiming that `session` is already built. |
| Scope | Correct component cardinality and adapter terminology; distinguish ten source components from the current eight linkable modules plus the guest-image input; define `session` as lifecycle/identity owner and `adapter-mvdm-host-out/softpc` as mapping-facade implementation owner; correct production/test-root terminology; and record remaining selector-aware observation/fixture residue as later cleanup rather than silently legitimizing it. |
| Non-goals | No `src/session` implementation, source move, BOP/provider change, Bochs change, adapter semantic migration, Ninja module addition, or test behavior change. |
| Verification | [S6 governance evidence](etc/evidence/m0-t261-s6-architecture-contract-consistency-001.md); authority cross-read, targeted terminology/dependency scan, documentation inventory regeneration, documentation-governance verification and `git diff --check`. |
| Exit Criteria | Architecture, goal, coding and rules agree on the ten-component target; S7 is the sole active implementation packet for `session`; no current build claim is inflated; and the follow-on cleanup has an explicit S owner. |
| Follow-on | S7 implements only dependency-free `session`; S8 performs the owner-directed OpenNT-host source replacement; the former adapter-root cleanup is deferred to S9. |

### M0 T261 S5 — closed: adapter-mvdm-host-in generic ingress ownership split

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | The owner explicitly authorized S5: “好的 接下来S5开工？”.  T261’s approved proposal admits this exact split after S4 closure. |
| Objective | Establish `adapter-mvdm-host-in` as the sole selector-blind fixed-width generic-UD ingress/completion owner; retain every BOP selector/service recognition, provider dispatch and BOP-aware observation in `opennt-bop`. |
| Non-goals | No new BOP enablement, no provider semantic change, no new CPU-frame ABI, no Bochs change, and no migration of family/session/provider logic into `adapter-mvdm-host-in`. |
| Reference Baseline | T261/S4 P7 closure `694cf087`; existing the former generic-UD bridge header; `opennt-bop/ingress/dem_generic_ud_bridge.c`; T260 formal component manifest and architecture/coding authorities. |
| Files And ABI Surface | New `src/adapter-mvdm-host-in/*`; the generic-UD external bridge symbol; exported `opennt-bop` route callback; app session-composition bind/reset calls; manifest/generator module and link order; focused bridge fixture. |
| Applicable Rules | `adapter-mvdm-host-in` is selector/service/family/DOS/WOW/provider blind; OpenNT route semantics remain in `opennt-bop`; `app` owns wiring; `adapter-mvdm-host-out/softpc` remains the only SoftPC/CCPU-to-Bochs boundary; fixed-width copied data only. |
| Verification | Function-level owner map; selector-token negative scan of `adapter-mvdm-host-in`; focused bind/decline/dispatch/reset fixture; formal Ninja closure; component-direction scan; `git diff --check`; documentation governance. |
| Expected Markers | No selector/service literal or provider include in `adapter-mvdm-host-in`; no exported CPU generic-UD bridge implementation in `opennt-bop`; an unbound bridge declines, a bound route receives the unchanged copied event/outcome, and reset removes the binding. |
| Asset Needs | Existing fixed-width generic-UD ABI, current route entry, app engine lifecycle, formal Ninja manifest/generator and focused C fixtures. |
| Reporting Requirements | Exact function ownership map; no-new-ABI proof; retained route/observation source; binding lifetime; source/list/link changes; focused and formal results. |
| Stop Conditions | A proposed adapter file reads a selector/service/window payload for meaning, names a provider/family/DOS/WOW concept, requires a raw guest/Bochs object pointer, or forces a provider semantic change. Pause and record the boundary conflict. |
| Exit Criteria | `adapter-mvdm-host-in` owns the externally called generic ingress symbol and only generic registration/validation/forwarding; `opennt-bop` owns one exported route callback and all selector-aware logic; `app` binds/unbinds around one engine session; formal graph names eight linkable libraries and passes stated focused checks. |
| Original Owner Request | “好的 接下来S5开工？” |
| Similar-Issue Sweep | Generic-UD v1/v2 symbol definitions, handler registration lifetime, BOP observations, fixtures with bridge stubs, manifest module/source/link lists, old `bx-vdm`/ingress path references, and component READMEs. |

### M0 T261 S4 — closed: DEM host-support original-layout recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T261/S3 is closed by the formal layout evidence; the owner-approved T261 proposal admits its next DEM owner-recovery sequence. |
| Objective | Recover or relocate DEM host support by original source owner while preserving host-file API shape, error order, opaque-handle mapping and guest-memory lease contracts. |
| Non-goals | No new DEM BOP enablement, redirector protocol completion, DASD/FDC implementation, new mutation policy, guest/Bochs change, or source-provider redesign. |
| Reference Baseline | T261/S1 source truth, T261/S2 binding ledger rows for `dem/*`, T230 direct-source mirrors, T232 shared handle manager and T261/S3 formal root. |
| Files And ABI Surface | `opennt-host/dem/*`, direct OpenNT `dem*.c` callers in `opennt-bop/dem`, their host file/directory/error/handle declaration seams, and their adapter-mvdm-host-out/win32/adapter-mvdm-host-out/softpc includes. |
| Applicable Rules | Original source and call shape first; every imported-body edit has `DIVERGENCE`; adapter-mvdm-host-out/win32 owns historical public API facades; adapter-mvdm-host-out/softpc owns mapping/mechanics; no component may depend upward on BOP routing. |
| Verification | Per-group original-source/ABI/failure ledger, component-direction scan, focused DEM fixtures, fresh formal Ninja build, `git diff --check` and documentation governance. |
| Expected Markers | Each surviving host file has a direct original path or named source-derived exception; no generic DEM shim remains merely because it combines BOP state with host capability; original source callers preserve failure order. |
| Asset Needs | Existing OpenNT DEM mirror/reference sources, adapter interfaces, shared mapping manager, focused fixtures and formal Ninja generator. |
| Reporting Requirements | Exact caller/function map, retained original interface/layout/order, extracted adapter boundary, source divergence/exception, focused result and any deferred machine/Redirector owner. |
| Stop Conditions | A split requires BOP service interpretation in `opennt-host`, a host component call upward into `opennt-bop`, a second mapping/handle manager, or a semantic rewrite of an imported body. Pause that group and record the dependency. |
| Exit Criteria | Every DEM host-support file has moved to an original-owner path, a declared adapter, BOP composition, or an explicit source-derived host exception; no reverse dependency is introduced; all repaired consumers build and declared focused tests pass or retain an owner-specific limitation. |
| Original Owner Request | “实施opennt-host组件的原生化”。 |
| Similar-Issue Sweep | DEM file/dir/gset/handle/error/search/DASD seams, VDD/debug/FDC exceptions, Redirector cross-calls, app and fixture include paths, static manifest and component README rows. |

**Current progress:** S1 closed in `25f817ad`; S2 P1 closed in `6c0313a8`
with a 47/47 recovery ledger. S3 P1 has re-rooted the byte-identical PIF
triplet and compiled the parser object through its new exact include path; see
[PIF evidence](etc/evidence/m0-t261-s3-p1-pif-original-re-root-001.md). The
full-graph run root was interrupted externally and is explicitly non-evidence;
S3 P2 has moved the parser layout bridge to `opennt-bop` and its OEM Win32
facade to `adapter-mvdm-host-out/win32`; both moved objects compile in fresh `r002`. S3 will
use that fresh root for later formal closure. No provider behavior has changed.
S3 P3 has additionally moved the COMMAND native session state machine from
`opennt-host` to `opennt-bop/ingress`, compiled its exact object in fresh
`r003`, and passed the component-manifest and production-tree gates; see
[session-ingress evidence](etc/evidence/m0-t261-s3-p3-command-session-ingress-rehome-001.md).
S3 P4 has moved the remaining project-owned COMMAND composition sources to
`opennt-bop/command`; fresh `r004` builds all five moved objects and the full
`opennt-bop` static library.  The remaining `opennt-host` COMMAND input is
only the unchanged PIF triplet; see [composition-owner evidence](etc/evidence/m0-t261-s3-p4-command-composition-owner-rehome-001.md).
S3 P5 has extracted the reached RTL/NTDLL compatibility surface from that
composition to `adapter-mvdm-host-out/win32` without a reverse BOP dependency.  Fresh
`r005` builds both affected libraries and passes the direct check-binary and
dynamic-environment fixtures; see [RTL-facade evidence](etc/evidence/m0-t261-s3-p5-command-rtl-facade-extraction-001.md).
S3 P6 has moved DEM session and dispatcher composition to `opennt-bop`; fresh
`r006` compiles its production and affected fixture objects while retaining
host drive/namespace capability in `opennt-host`; see [DEM composition evidence](etc/evidence/m0-t261-s3-p6-dem-composition-owner-rehome-001.md).
S3 P7 has similarly moved the XMS native `#UD` session to `opennt-bop/ingress`;
fresh `r007` compiles it and passes component gates; see [XMS session evidence](etc/evidence/m0-t261-s3-p7-xms-session-owner-rehome-001.md).
S3 P8 has moved the source-parity configuration-complete `UMBNotify` unit to
`opennt-bop/softpc`; r008 compiles it with component gates passing; see [configuration-complete evidence](etc/evidence/m0-t261-s3-p8-config-complete-owner-rehome-001.md).
S3 P9 has now completed fresh formal root `r009` (458 Ninja edges), confirmed
the PIF/COMMAND-focused fixtures and recorded the two unrelated DASD/Redirector
fixture limitations without treating either as passing evidence; see the
[formal layout closure](etc/evidence/m0-t261-s3-p9-formal-layout-closure-001.md).
S4 P1 has completed the function-level DEM host boundary map: it separates
source-visible host APIs, BOP composition, adapter-mvdm-host-out/softpc mechanics and
unavailable VDD/FDC paths, and fixes the dependency order before any source
move; see the [DEM boundary map](etc/evidence/m0-t261-s4-p1-dem-host-boundary-map-001.md).
S4 P2 has split the independent DEM directory group: its source-body invoke
now belongs to `opennt-bop`, same-named OEM directory calls belong to
`adapter-mvdm-host-out/win32`, and fresh r010 formal build plus the direct-import fixture
pass; see the [DEM directory split](etc/evidence/m0-t261-s4-p2-demdir-owner-split-001.md).
S4 P3 has split the DEM file/label composition from same-shaped OEM Win32
facades while retaining Redirector as its separately owned lower host
capability; fresh r011 completes 462 Ninja edges and both direct file/OEM
fixtures pass; see the [DEM file split](etc/evidence/m0-t261-s4-p3-demfile-owner-split-001.md).
S4 P4 has split DEM global-setting service composition/DTA state from the
same-shaped public-Win32 host queries while retaining host drive admission;
fresh r012 completes 460 Ninja edges and the direct `demgset` fixture passes;
see the [DEM global-setting split](etc/evidence/m0-t261-s4-p4-demgset-owner-split-001.md).
S4 P5 has split DEM search/FCB BOP composition and historical find/NT facade;
the removed invasive `NtVdmControl` remains explicit `STATUS_NOT_IMPLEMENTED`
for the imported fallback. Fresh r013 completes 461 Ninja edges and the
direct search/FCB fixture passes; see the [DEM search split](etc/evidence/m0-t261-s4-p5-demsrch-fcb-owner-split-001.md).
S4 P6 has moved the common DEM CCPU/SAS frame, mapping and handle facade to
`adapter-mvdm-host-out/softpc`; a generic post-body hook removes its former direct demerror
link. Fresh r014 completes 461 Ninja edges and focused handle/error fixtures
pass; see the [CCPU/SAS migration](etc/evidence/m0-t261-s4-p6-dem-ccpu-sas-facade-migration-001.md).
S4 P7 has completed the final owner cleanup: the remaining DEM
compatibility/composition surfaces now reside under `opennt-bop/dem`, while
`opennt-host/dem` retains only named drive/namespace capabilities and its
drive-policy facade.  Fresh r016 completes 462 Ninja edges; the DASD/IOCTL,
error/lock and MISC direct-import fixtures pass.  The old r015 concurrent
build root is explicitly excluded from evidence.  See the
[S4 owner closure](etc/evidence/m0-t261-s4-p7-dem-host-owner-closure-001.md).
S5 is now admitted as the next bounded owner split.  It must not interpret a
selector in `adapter-mvdm-host-in` or change the already-closed DEM host boundary.
S5 P1 is closed in `ab49a2d4`: `adapter-mvdm-host-in` owns the generic CPU entry and
the unchanged copied record; `opennt-bop` retains the renamed selector route;
and `app` owns bind/reset around one engine run.  The actual formal r002 native
link and focused ingress fixture pass; see
[S5 ingress evidence](etc/evidence/m0-t261-s5-adapter-mvdm-host-in-ingress-split-001.md).

## Previous Active Packet — M0 T260

T260 is closed. Its retained detail below is historical closure evidence and
does not authorize follow-on implementation.

### M0 T260 S8 P2 — closed: production-tree purification, tests and final closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T260 S8`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | S7 is closed. Owner requires one complete migration of tests, formal Ninja inputs, generated manifests and import/governance scripts to the eight component roots before any build result is claimed. |
| Objective | Make every eight-component `src/` root production-only: externalize reference/example/fixture/legacy inputs, place live OpenNT code by its original owner-family composition, individually annotate/register every production divergence, then retain the formal Ninja graph and final link closure. |
| Non-goals | No BOP behavior implementation or enablement; no new Bochs semantic intrusion; no source-provider redesign. |
| Reference Baseline | S1 manifest, source policy, current component READMEs, the completed r014 formal graph, and the now-superseded original/mirror/overlay staging tree. |
| Files And ABI Surface | All eight component roots: seven linkable host-side components plus the `opennt-guest` artifact input; their component manifest, formal Ninja graph, test ownership and original OpenNT provenance/include edges. |
| Applicable Rules | Component roots are production-only; references/examples/fixtures belong outside `src`; every nonzero imported-source edit is source-marked `DIVERGENCE:` and individually registered; BOP owns route/meaning but never Bochs-object mechanics. |
| Verification | Git-rename/content identity scan; route/mirror/original owner scan; retained-shim negative scan; `git diff --check`; documentation governance verification. |
| Expected Markers | No legacy source root remains; no `original/`, `mirror/`, `overlay/` or generic capability staging tree remains as a product-code category; source-shaped host/SoftPC/Win32 seams name their owner directly. |
| Asset Needs | OpenNT source roots, current BOP mirror/route tree, owner manifest and formal module manifest. |
| Reporting Requirements | Record exact moved roots/counts, no-source-edit proof, retained later-owner shim/observation inventory and remaining S8 build switch. |
| Stop Conditions | A file combines BOP meaning with host capability/Win32/SoftPC mechanics such that it cannot be split without behavior change, or an original source identity cannot be proved. Pause for a targeted split decision. |
| Exit Criteria | Formal graph, tests and tools use only target component paths; all eight static library boundaries and final-link ordering are explicit; no component root contains non-production reference/example/fixture/overlay code; every retained divergence has source and README evidence; no legacy `src/opennt`, `src/bx-vdm` or `src/cli` root remains; documentation, static gates and an actual formal Ninja build verdict are recorded. |
| Original Owner Request | “应使用 1 个 T 任务，例如：M0 T260：七/八组件源码布局重整与构建闭合……这个 T 应拆成约 8 个 S。” |
| Similar-Issue Sweep | Headers, fixtures, generated Ninja input, include roots, legacy v1/test-only code, OpenNT source provenance and the shared mapping-manager placement. |

### Planned T260 S sequence

| S | Bounded outcome |
| --- | --- |
| S1 | Complete owner/provenance/build manifest. |
| S2 | Target roots, READMEs, exception registers and public library/include boundaries. |
| S3 | `bx-core` confirmation plus `bx-machine` purification: retain Bochs-only code and assign every foreign-semantics file to its target owner. |
| S4 | `app` and `opennt-guest` migration. |
| S5 | `opennt-bop` mirror, ingress and route migration. |
| S6 | `opennt-host` and `adapter-mvdm-host-out/win32` classification/split migration. |
| S7 | `adapter-mvdm-host-out/softpc` same-shaped SoftPC/CCPU boundary and shared-mapping-manager migration. |
| S8 | Tests, formal Ninja graph, legacy-path exit and whole-package acceptance. |

### T260 S5 completion record

S5 moved OpenNT BOP identity, minimal-change mirrors and BOP routes into
`src/opennt-bop/{original,mirror,route,overlay}` without a source-body edit.
The content-preserving move and inherited-overlay register are recorded in the
[S5 layout evidence](etc/evidence/m0-t260-s5-opennt-bop-layout-migration-001.md).
The formal Ninja source declarations are intentionally still historical until
S8; S6/S7 next classify and migrate the remaining host/API and SoftPC/mechanical
seams.

### T260 S6 completion record

S6 separated the pre-existing OpenNT host-capability files and same-shaped
Win32/NTDLL facade from the former generic shim tree. All affected DEM,
COMMAND and XMS mirrors now name their host component dependency directly.
The S6 evidence records the deliberately deferred formal Ninja-graph update
and the exclusive S7 ownership of the remaining mechanical seam files; see
[S6 layout evidence](etc/evidence/m0-t260-s6-host-win32-layout-migration-001.md).

### T260 S7 completion record

S7 established `adapter-mvdm-host-out/softpc` as the unique same-shaped mechanical facade,
including the one mapping-manager implementation with its three session
instances. `src/bx-vdm` contains no live product file after the move. The
remaining old-path references are exclusively S8 test/tool/build-graph work;
see [S7 layout evidence](etc/evidence/m0-t260-s7-softpc-adapter-layout-migration-001.md).

**S8 P2 amendment:** the prior staging labels `original/`, `mirror/`,
`overlay/` and generic `capability/` are no longer acceptable inside a
component root when they contain reference-only or non-linkable code.  S8 now
purifies those roots, relocates reference/example/test-only material to
`docs/etc/legacy_code/` or `tests/`, and records every retained production
divergence at statement level.  The r014 formal Ninja link is a pre-purification
baseline, not S8 closure evidence.

**S8 P2 verification:** the reclassified graph passed its manifest gate at
291 sources, 78 fixtures and one native target. Formal Ninja run `r018`
linked all seven static libraries, every declared fixture and
`bin/ntvdm64.exe`; final actual incremental and dry-run invocations
reported no work. The production-tree gate also passed: no non-production
staging root or legacy build input remains, and all 46 source divergences are
individually README-registered. See the [T260 closure](history/m0-t260-closure-20260823.md).

**Current progress:** S1 is closed. The current reproducible manifest covers
all 3,542 `src/` files in 3,735 rows with zero missing paths, duplicate paths
or unresolved owners; 2,332 non-product OpenNT inputs are explicitly destined
for `refs/opennt`. See the
[baseline record](etc/operations/m0-t260-s1-owner-manifest-baseline-001.md).

S2 P1 has created all eight target-root README contracts. They declare the
exception registers for `bx-core`, `opennt-guest`, `opennt-bop` and
`opennt-host`; restrict machine to Bochs-only mechanics; and declare each future
library/public-include boundary. No product source or formal Ninja input has
moved.

S3 has moved all 42 former project-prefixed bridge files to
`adapter-mvdm-host-out/softpc` with Git history, repaired product include ownership, and
made the remaining machine source scan free of external product semantics.
The local bx-core exception index now points to every active exception family.
The one coherent formal Ninja/fixture path switch remains S8 work; see the
[S3 evidence](etc/evidence/m0-t260-s3-machine-purification-001.md).

S4 has moved the 24 former `src/cli` implementation files into `app` and 579
original DOS V86/WOW16 source files into `opennt-guest`, all as 100% Git
renames.  The live owner map now records 580 guest candidates with no
DEM/COMMAND entry and separately classifies 38 DEM/COMMAND source inputs for
future `opennt-bop` migration.  Formal Ninja paths remain intentionally
unchanged until S8; see the
[S4 evidence](etc/evidence/m0-t260-s4-app-guest-migration-001.md).

## Previous Active Packet — M0 T259

### M0 T259 S4 — Host/machine source-shaped facade reconciliation

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T259 S4`, Ordinary Mode with a single-person dual-role review: source-first facade reconciliation. |
| Admission And Approval | T259 S1 catalog, S2 RTL/environment seams and S3 machine-contract closure are established. S4 reconciles the remaining reached host/machine facades and their formal static-link closure. |
| Objective | Recover each reached original OpenNT/SoftPC interface shape and its observable semantics through the smallest source-shaped compatibility facade or direct public API, while independently minimizing edits to OpenNT mirrors and `bx-core`/`bx-machine`. |
| Non-goals | No BOP/provider enablement; no wholesale CCPU/SAS/CSR/BaseSrv product-shell recreation; no Bochs semantic intrusion; no invented behavior without source evidence. A facade is not permission to substitute a convenient new ABI. |
| Reference Baseline | `MACH-01..09` in the T259 catalog; OpenNT `softpc.new/host/src/{nt_cpu.c,nt_cprgs.c,nt_aorc.c}`, `keybd_io.c`; active SoftPC/DEM/XMS source mirrors; T258 mapping manager. |
| Files And ABI Surface | The source-shaped SoftPC/CCPU facade contract in `bx-vdm`; affected `host_simulate`, register-frame, checked-memory, A20, IVT/keyboard and typed machine-action seams. |
| Applicable Rules | Source-first recovery, public-Win32/x64 decision rule, guest/machine hard boundary, one shared mapping manager, and no family-private product shell. |
| Verification | Original call-site and fixed-width ABI audit; focused seam fixture; negative selector/DOS/CCPU boundary scan of bx-core/bx-machine; formal Ninja build/link; documentation gate and independent review. |
| Expected Markers | Reached historical spelling remains at the mirror boundary; `bx-vdm` performs copied-state translation; bx-core/machine remain selector/DOS/CCPU blind; no CCPU loop or host pointer crosses the boundary. |
| Asset Needs | OpenNT SoftPC/CCPU source and active mirrors, current bx-vdm/machine API, T258 mapping manager, catalog, exception register and formal build manifest. |
| Reporting Requirements | Record original interface spelling and call site, calling convention/width/lifetime, typed mechanical request/result, no-op/failure disposition and any exception ID. |
| Stop Conditions | A helper exposes host pointers to the guest, changes imported OpenNT control flow, recreates a CCPU/product shell, imports BOP/DOS/SoftPC terms into bx-core/machine, or lacks a source/ABI evidence entry. |
| Exit Criteria | Every remaining reached `HOST-*`/`MACH-*` seam has a source-shaped facade, direct public-API composition, or explicit source-proven decline; the formal static-library closure is sound for each affected fixture; all residual product-shell semantics are queued rather than silently recreated. |
| Original Owner Request | “接下来，是关于我们全面恢复opennt的win32api宿主方面和 softpc/ccpu 核心机方面的接口规范的任务。你已经审计过。” |
| Similar-Issue Sweep | `ANSI_STRING`, `OEM_STRING`, `UNICODE_STRING`, `Rtl*` conversion/heap/environment helpers, `NTSTATUS` to DOS error mapping and existing COMMAND/DEM declaration copies. |

**Current progress:** S1 closed the canonical
[`HOST-*`/`MACH-*` interface catalog](etc/evidence/m0-t259-s1-host-machine-interface-catalog-001.md)
and [active-facade reconciliation](etc/evidence/m0-t259-s1-current-facade-reconciliation-001.md).
S2 P1 has centralized the active COMMAND `HOST-07` declaration surface and
recovered source-visible NTSTATUS-to-DOS error differentiation through the
current same-named `ntdll` export; its formal 286-node link and source-mirror
fixture pass are recorded in [P1 evidence](etc/evidence/m0-t259-s2-p1-rtl-contract-status-recovery-001.md).
P2 has restored caller-owned ANSI output semantics, and P3 proves the imported
`cmdenv.c` dynamic session-environment transform in a fresh formal graph. S2's
COMMAND slice is closed; any additional DEM consumer migration is tracked as
S4 reconciliation work rather than a reason to fork another RTL contract.
S3 now records the SoftPC/CCPU mechanical contract before correcting its first
reached facade. This remains neither a new BOP survey nor an NT4 product-shell
recreation.

S3 P1 has recovered the call-local CCPU register/flag accessor semantics used
by directly imported DEM code: writes to CF/ZF now update the copied image as
well as the existing typed result, and CS:IP staging remains explicitly local
until a future source-proven continuation contract consumes it. A fresh
286-object formal graph and the direct DASD/IOCTL fixture pass; see
[S3 P1 evidence](etc/evidence/m0-t259-s3-p1-ccpu-copied-state-facade-001.md).
This is deliberately not a claim that the recursive `host_simulate` lifecycle
is already restored.

S3 P2 closes the reached machine-facade disposition audit.  It proves that
original CCPU `host_cpu_reset`/`host_cpu_interrupt` are no-ops, that the
recursive `host_simulate` branch is a truthful unavailable result rather than
a false CPU success, and that finite SAS/IVT/A20 paths retain source-shaped
checked facades.  It also records the combined recovery rule: retaining the
original interface contract and minimizing intrusion are simultaneous gates.
See [S3 P2 evidence](etc/evidence/m0-t259-s3-p2-machine-facade-disposition-closure-001.md).

S4 P1 has replaced the false DEM hard-error NTDLL stubs with same-named,
host-only `ntdll.dll` facades for Unicode append/compare and DOS-device
symbolic-link open/query. The original `demerror.c` ordering remains intact;
a fresh 286-object formal graph and source-mirror fixture pass. See
[S4 P1 evidence](etc/evidence/m0-t259-s4-p1-demerror-ntdll-facade-001.md).

S4 P2 closes formal static-library reconciliation: a fresh 453-node Ninja
graph links all 78 fixtures plus the native target using exact per-fixture
module closures, and focused regressions pass.  The canonical RTL facade now
keeps its counted-string layout checks in both C11 and C++14; no OpenNT mirror
or Bochs source changed. See [S4 P2 evidence](etc/evidence/m0-t259-s4-p2-formal-static-closure-001.md).

S4 P3 restores reached `RcErrorDialogBox`/`RcMessageBox` host facades through
public Win32 and makes `demmisc` reuse the existing OpenNT-shaped
OEM-to-Unicode file facade. The fresh 454-node formal graph, three focused
fixtures and all 79 outputs pass; custom editable PIF UI, VDD/debugger,
printer and floppy product semantics remain explicitly outside this facade.
See [S4 P3 evidence](etc/evidence/m0-t259-s4-p3-host-dialog-oem-facade-closure-001.md).

S4 P4 completes the final reached-seam reconciliation.  It restores the
directly composable `FdiskTerminatePDB` half of the original
`HostTerminatePDB` order, makes the unavailable VDD/floppy halves explicit,
and records direct parity for the original empty `UMBNotify` body.  The fresh
454-node r16 formal graph is clean; focused DEM search/FCB and DASD fixtures
pass.  S4 is complete; see [S4 P4 evidence](etc/evidence/m0-t259-s4-p4-final-host-machine-reconciliation-001.md).

S4 P5 closes a reverse-audit correction in the directly imported OpenNT PIF
parser: its `RcMessageBox` wrapper now uses the one shared source-shaped
facade, and the common historical `RMB_ABORT` option/result value is restored
from the erroneous `2` to OpenNT's `1`.  A full fresh r17 graph produces all
79 declared outputs, and PIF parser, COMMAND misc and DEM hard-error fixtures
pass; see [S4 P5 evidence](etc/evidence/m0-t259-s4-p5-command-pif-dialog-facade-closure-001.md).

**T259 technical closure state:** all admitted S1--S4 exit criteria are met,
and the [formal T259 closure](history/m0-t259-closure-20260823.md) is
recorded. T260 succeeds it; this retained record does not reopen T259's scope.

## Latest Closure

M0 T260 closes the eight-component production source layout.  All component
roots are production-only, legacy/reference code is externalized under
`docs/etc/legacy_code`, and the formal Ninja closure plus source/README
divergence register gate pass. See [T260 closure](history/m0-t260-closure-20260823.md).

M0 T259 closes the source-first host/machine compatibility-facade baseline.
Every reached `HOST-*`/`MACH-*` seam now has direct original composition, a
smallest OpenNT-shaped public-API facade, or an explicit source-proven
decline; imported OpenNT mirrors and bx-core/machine remain unmodified. See
[T259 closure](history/m0-t259-closure-20260823.md).

M0 T258 closes the shared 16/32 guest-to-32/64 host mapping-manager
prerequisite. One implementation supplies the session's separate
`guest_memory`, `host_handle`, and `session_data` instances; the outer engine
owns their reset. Its 16:16/32-linear lease, opaque host-handle/data IDs,
private-instance rejection and affected DEM/COMMAND regressions have focused
proof. See [T258 closure](history/m0-t258-closure-20260823.md).

M0 T257 closes as a source-first design rejection. Td S2 P7 proves that a
new generic protected-frame ABI would bypass unresolved DPMI pointer,
frame/IRET, LDT and memory-lifecycle owner dependencies. Its retained
source-import and failure-map evidence transfers to a future whole DPMI
package; it enables no new `53:xx` route. M0 T258 now owns only the
cross-family synchronous mapping prerequisite.

M0 Td S2 P1 closes the governing source-recovery decision: an original OpenNT
translation unit now requires direct composition or a recorded OpenNT-shaped
facade before a project-defined replacement is permitted. It also fixes the
single bounded guest-pointer mapping-lease contract. S5 is frozen pending the
separate Td S2 P2 whole-tracker interface-conformance audit; see
[guest-pointer governance](etc/operations/td-guest-pointer-mapping-governance-001.md).

M0 Td S2 P3 closes the shared-interface audit baseline: the tracker now has
321 unique rows (203 BOP plus 118 dependencies), including the previously
untracked historical guest-pointer interface family.  It classifies the
historical dispatcher, bounded keyboard facade, host-token exception, generic
DPMI mechanics and pointer migration debt without enabling a new route; see
the [P3 audit](etc/evidence/td-s2-p3-shared-interface-conformance-audit-001.md).
T257 S5 remains frozen until P4--P7 complete the whole-tracker audit.

M0 Td S2 P4 closes the DEM/COMMAND interface audit: all 90 BOP slots and 33
direct dependencies have an explicit source-interface disposition.  It finds
64 pointer-mapping migration debts, 43 retained OpenNT-shaped facades, six
approved opaque-handle exception routes and ten explicitly deferred product
paths; see the [P4 audit](etc/evidence/td-s2-p4-dem-command-interface-conformance-audit-001.md).
No BOP was enabled or reimplemented.  P5 next audits XMS/DPMI; S5 remains
frozen.

M0 Td S2 P5 closes the XMS/DPMI interface audit: all 37 BOP slots and 27
direct dependencies have an explicit disposition.  It retains only generic
ordinary-RAM mechanics under existing exceptions, records 17 pointer/seam
migration debts, and defers all 30 DPMI product-shell/frame/LDT/VDD paths;
see the [P5 audit](etc/evidence/td-s2-p5-xms-dpmi-interface-conformance-audit-001.md).
No `53:xx` ingress is admitted and S5 remains frozen.

M0 Td S2 P6 closes the remaining owner-package audit: every remaining 76 BOP
and 53 dependency rows has an explicit disposition.  It separates 28
guest-owned source/build rows from host composition, records 21 active
source-derived migration debts, and keeps 69 unavailable product-shell paths
deferred; see the [P6 audit](etc/evidence/td-s2-p6-remaining-interface-conformance-audit-001.md).
P7 must reconcile the whole 321-row matrix before S5 can be reconsidered.

M0 Td S2 P7 closes the full interface-conformance audit: all 321 tracker IDs
are uniquely classified and exception references reconcile to the register.
It rejects re-admission of T257 S5 because DPMI still has migration debt plus
whole-package deferred frame/IRET/LDT dependencies; see the
[P7 closure](etc/evidence/td-s2-p7-full-interface-conformance-closure-001.md).

M0 T256 closes the selector-blind protected virtual-access and guest-linear
substrate. Its final fresh 278-node formal Ninja build and XMEM fixture pass;
it deliberately does not claim DPMI activation. See [S15 evidence](etc/evidence/m0-t256-s15-protected-substrate-closure-001.md) and the [T256 closure](history/m0-t256-closure-20260823.md).

M0 T257 S1 closes the complete non-WOW DOSX/`dpmi32` owner, ABI and failure
map: all `53:00..18` enter one package, with historical NT4 hooks identified
as explicit source-composition gaps rather than authorization for leaf shims.
See [S1 evidence](etc/evidence/m0-t257-s1-dpmi-dosx-owner-abi-failure-map-001.md).

M0 T257 S2 closes the DOSX/DPMI source import boundary.  It fixes the exact
future mirror set and names every NT4 product import that requires a
source-derived seam or explicit refusal; the boundary scanner proves no DPMI
body or mirror is already an accidental product input.  See the
[S2 ledger](etc/operations/m0-t257-s2-dpmi-dosx-source-import-ledger-001.md).

M0 T257 S3 closes DOSX/DPMI startup and descriptor-table source composition.
`dpmi32.c` and `dpmiselr.c` now formally link through bounded named seams;
native Bochs consumes guest GDT/LDT entries directly, and no `53:xx` ingress
was enabled. See the [S3 closure](etc/evidence/m0-t257-s3-dosx-startup-table-closure-006.md).

M0 T257 S4 closes the DOSX/DPMI protected interrupt, fault and IRET source,
ABI and failure map. It proves that the current result ABI cannot atomically
restore the source frame, refuses direct old-NT4 hooks, and retains native
Bochs as the architectural exception/IRET owner; see the [S4 map](etc/evidence/m0-t257-s4-dosx-protected-frame-map-001.md).

M0 T256 S14 closes protected interrupt/fault/IRET classification. OpenNT uses
historical `NtVdmControl`, emulator hooks and locked DPMI stack frames, while
native Bochs remains CPU exception/IRET owner; see [S14 evidence](etc/evidence/m0-t256-s14-protected-interrupt-fault-iret-continuity-map-001.md).

M0 T256 S13 closes native descriptor-table lifecycle admission. DOSX `selGDT`
is a historical process-LDT alias: table mutation plus `53:00` host install is
DPMI package semantics, not a selector-blind machine lifecycle. Native Bochs
retains GDT/LDT/LLDT behavior; see [S13 evidence](etc/evidence/m0-t256-s13-native-descriptor-table-lifecycle-admission-001.md).

M0 T256 S12 closes the DPMI descriptor/guest-linear identity map. OpenNT's
`FlatAddress[]`, `Ldt`, `IntelBase` and x86 process-LDT calls are historical
host composition, while native Bochs already owns guest descriptor mechanics.
The next question is guest GDT/LDT lifecycle only; see [S12 evidence](etc/evidence/m0-t256-s12-dpmi-descriptor-guest-linear-identity-map-001.md).

M0 T254 closes the premature DPMI implementation admission.  The retained
OpenNT `dpmi32` package requires protected selector/LDT/context/IRET and
guest-linear memory substrate which the current copied-state boundary does not
provide; no `53:xx` leaf is independently admissible.  See the
[T254 closure](history/m0-t254-closure-20260823.md).

M0 T253 closes the selector-blind local async named-pipe completion slice.
`57:23/24` and the pipe-only `57:26` disposition recover the retained
descriptor/queue/ANR contract through a bounded bx-vdm seam; the native PIC
remains unchanged and NetBIOS/DLC remain deferred. See the
[T253 closure](history/m0-t253-closure-20260823.md).

M0 T251 closes the composable Redirector lifecycle/mailslot slice and the
source-shaped async-pipe boundary.  `57:09..0F` are fully recovered through a
session/PDB-owned provider; `57:23/24` correctly remain explicit failure
terminals until guest SFT opaque-token and `int5c` delivery owners exist. See
the [T251 closure](history/m0-t251-closure-20260823.md).

M0 T250 closes the apparent low-memory far-target question.  `0x0A87` is
original relocated `DOSDATA:OPENBUF+0x0F`, not an NTDOS code entry; the
subsequent far call is accidental execution of uninitialised guest storage.
It selects no BOP, Bochs or machine repair.  See the
[T250 closure](history/m0-t250-closure-20260823.md).

M0 T249 closes low-memory writer provenance.  The initial `0x0A87 <- FF`
mutation is a source-built original NTDOS `DOSCODE` CPU write at
`CS:base:EIP=9386:93860:7424`, not a Bochs, adapter or BOP-provider write.
See the [T249 closure](history/m0-t249-closure-20260823.md).

M0 T248 closes selector-blind post-EXEC real-mode continuity attribution. The
source-built checkpoint reaches accepted `50:36` and `50:1E`, then makes a
guest far indirect call from runtime-modified low NTIO memory
`0000:0A87 → 5859:5F04`; the target is zero RAM and reaches the bounded
execution budget. This is neither a BOP provider nor a missing Bochs machine
mechanic. See the [T248 closure](history/m0-t248-closure-20260823.md).

M0 T247 closes the NTDOS guest EXEC entry/parent-return boundary.  Its
source-built native checkpoint proves original `$Exec` PDB construction and
the accepted `50:36` child-entry handoff; the pre-return unexpected-loop
terminal transfers to active M0 T248's selector-blind machine-continuity
package, not to `50:1E demWrite` or an adapter DOS implementation. See the
[T247 closure](history/m0-t247-closure-20260822.md).

M0 T246 closes the shared SoftPC `02/06` machine interrupt package. Its
adapter-only active route preserves the source-derived stack/PIC/BDA mechanics
and records typed native resume before the distinct later `50:1E` owner. Later
native continuity is transferred to active M0 T247; see the [T246 closure](history/m0-t246-closure-20260822.md).

M0 T246 S1 closes the `02/06` owner/ABI map. The repeated `06` is an active
registry omission: the original shared operation stream, its named provider,
typed RAM/port actions and local transaction fixture already exist. No Bochs
intrusion is justified; see [S1 evidence](etc/evidence/m0-t246-s1-softpc-machine-interrupt-owner-map-001.md).

M0 T246 S2 closes active generic-ingress recovery for the shared original
SoftPC `02/06` pair.  The adapter-only bridge converts the existing typed
provider result; its native fixture proves both source paths, controlled
preflight stop and selector rejection.  S3 alone owns the bounded
source-built checkpoint; see [S2 evidence](etc/evidence/m0-t246-s2-softpc-machine-interrupt-active-ingress-closure-001.md).

M0 T246 S3 closes package regression and the bounded native checkpoint. Every
observed `06` now has typed resume disposition and the first group reaches a
later `50:1E` owner. Later `06` calls and the final unexpected-loop return
are not a registry-miss recurrence; see [S3 evidence](etc/evidence/m0-t246-s3-softpc-machine-interrupt-native-checkpoint-001.md).

M0 T245 closes source-built permanent-command bootstrap continuity through the
first distinct post-COMMAND machine edge. It fixes the actual Direct host path
admission without guest rewrite/copy or Bochs change, then transfers the
independent BOP-06 machine owner to M0 T246; see [T245 closure](history/m0-t245-closure-20260822.md).

M0 T245 S22 closes the session-scoped DOS-safe startup alias.  The CLI first
retains a suitable host short spelling, otherwise creates only a temporary
directory link to the real bundle root and removes that link on teardown.
The focused fixture exercises the no-symlink-privilege junction fallback; the
bounded native source-built run clears the former permanent-command
`ERROR_PATH_NOT_FOUND` and reaches original COMMAND services before an
independent machine `BOP 06` edge. See [S22 evidence](etc/evidence/m0-t245-s22-dos-safe-bootstrap-alias-closure-001.md).

M0 T245 S21 closes the OpenNT-shaped OEM `CreateFileOem` recovery. The
imported DEM body now uses the original OEM-to-Unicode/`CreateFileW` ordering
and generic-read attribute retry through a named public-API shim; fixtures
pass. Native call facts prove the remaining `AX=3` comes earlier: DOS
canonicalizes `REPOS.HOBBY` to nonexistent `REPOS.HOB` and the volume has no
short alias. The next owner is CLI/startup alias admission, not DEM/BOP/Bochs;
see [S21 evidence](etc/evidence/m0-t245-s21-opennt-oem-createfile-recovery-001.md).

M0 T245 S20 closes permanent-command DEM-open attribution. The bounded
adapter-only record retains four accepted `50:12` calls; the final original
`demOpen` receives the full source-built `COMMAND.COM` host path but returns
source-shaped `AX=3/CF`. The path exists and equivalent public host open
succeeds, assigning the issue to the current OEM file shim rather than BOP,
guest EXEC or Bochs; see [S20 evidence](etc/evidence/m0-t245-s20-permanent-command-dem-open-observation-001.md).

M0 T245 S19 closes post-CONFIG interrupt classification.  After the
successful temporary SCS open, the permanent `commnd` `INT 21h/AX=3D00h` is
still the final file operation: no seek, close or `4B00` EXEC follows before
the original `comerr → 50:3D` terminal.  The remaining owner is the guest
DOS-name to Direct-host namespace boundary, not cmdExec32 or Bochs; see
[S19 evidence](etc/evidence/m0-t245-s19-post-config-interrupt-classification-001.md).

M0 T245 S18 closes native DEM `50:12` observation. It opens the original
temporary SCS configuration file with CF clear, so neither `demOpen` nor
COMMAND current-directory is the retained failure; see [S18 evidence](etc/evidence/m0-t245-s18-dem-open-observation-001.md).

M0 T245 S17 closes native `54:04` output observation. The existing imported
COMMAND body returns Direct-host `O:\repos.hobby\runner`, then source-built
execution reaches/resumes `50:12 demOpen`; see [S17 evidence](etc/evidence/m0-t245-s17-command-current-dir-observation-001.md).

M0 T245 S16 closes the relative-root bootstrap defect. CLI admission now
canonicalizes DOS/WOW16 roots before the unchanged original COMMAND source
route; a relative-input native run observes an absolute guest `commnd` and
newly reaches/resumes `54:04`. The later `50:3D` is a distinct unmodified
path; see [S16 evidence](etc/evidence/m0-t245-s16-canonical-command-bootstrap-root-001.md).

M0 T245 S15 closes classification of the `comerr` incoming edge: existing
default-off real-mode interrupt observation shows `INT 21h AX=3D00h` at
`DS:DX=commnd`, with no `AX=4B00h` before `50:3D`.  Thus the current failure
is permanent COMMAND *open*, caused by the relative bundle-root descriptor;
see [S15 evidence](etc/evidence/m0-t245-s15-command-open-failure-classification-001.md).

M0 T245 S14 closes the passive source-mapped `commnd` observation.  A formal
fixture and one source-built native Direct run prove the original S13
`SHELL=` value reaches guest `sysinit2.asm:commnd` before accepted `5E`;
the retained `50:3D` maps to `sysinit1.asm:comerr → stall`.  The observation
does not distinguish the original pre-EXEC open-error and EXEC-error incoming
branches, so that later guest interrupt/error classification remains separate;
see [S14 evidence](etc/evidence/m0-t245-s14-command-bootstrap-observation-001.md).

M0 T245 S13 closes source-built `COMMAND.COM` publication through the original
`cmdconf.c → SHELL= → sysconf.asm:commnd` route. Its formal source-mirror
fixture passes bounds/grammar cases and the full native graph links. The one
native run remains at the pre-existing `5E → 50:3D` source terminal, so it
transfers only the guest `commnd`/`INT 21h` predecessor observation to S14;
see [S13 evidence](etc/evidence/m0-t245-s13-command-bootstrap-shell-recovery-001.md).

M0 T245 S12 closes the bootstrap-path attribution only: the original
`50:3D` `demExitVDM` route is a typed source terminal following accepted `5E`,
not a missing BOP result. Its former claim that publication necessarily
requires a new path policy is superseded by active S13's newly verified
`cmdconf.c → SHELL= → sysconf.asm:commnd` source route.

M0 T245 S11 closes the source-shaped `5E` config-complete body: the active
Direct DEM session executes `UMBNotify(0); demDasdInit();`, resumes at `RIP+3`
and preserves the source zero-floppy path.  One source-built run reaches and
accepts `5E`; its subsequent original `50:3D` terminal is separately owned by
the active S12 bootstrap-path disposition, not a 5E defect.

M0 T244 closes the headless native 8042 lifecycle. Its formal fixture proves
ports `60h/64h`, timer, ordinary IRQ1/PIC and two clean lifecycles without the
Bochs product shell; see the [T244 closure](history/m0-t244-closure-20260822.md).

M0 T243 closes the source-shaped, headless `STREAM_IO=2` BOP-5F sub-contract:
the `AX=BEEF` table guard and original `useHostInt10` publication now reach the
source-built path, then resume with CF to the original NTIO branch. Physical
keyboard/IVT/PIC/timer behavior remains explicitly transferred to M0 T244;
see the [T243 closure](history/m0-t243-closure-20260822.md).

M0 T242 closes the local guest EXEC prerequisite recovery: original
`INT 15h AH=88h` and x86 `50:42` fast-read source groups are composed and
observed, while the reached `5F/AX=BEEF` handoff is transferred intact to the
now-active keyboard/IVT machine package. See the
[T242 closure](history/m0-t242-closure-20260822.md).

M0 T241 closes the XMS `52:09` IVT-watch dependency: the direct
`UpdateKbdInt15` fragment now preserves original compare/clear/update order
through selector-blind stage state, and `xmsNotifyHookI15` returns source CX.
The formal native fixture passes; the one product-bundle run stops before any
BOP and makes no continuity claim. See the [T241 closure](history/m0-t241-closure-20260822.md).

M0 T240 closes the OpenNT-shaped SoftPC conventional-memory query:
the active `mem_size.c` mirror reads mutable BDA `0040:0013` through the
existing selector-blind checked-RAM action, then returns AX/RIP+3. No Bochs
intrusion was required. Its one bounded native run stopped before a BOP was
reached, so it is not a guest-continuity claim; see the
[T240 closure](history/m0-t240-closure-20260822.md).

M0 T239 closes the working-tree retirement of every experimental BOP-v1
provider, catalog, facade, legacy fixture and dead build/probe helper. The
sole useful fixed-width request record is current provider-neutral ABI, not a
legacy route. See the [T239 closure](history/m0-t239-closure-20260822.md).

M0 T238 closes the no-external OpenNT BOP validation slice: the frozen 75 BOP
and 30 dependency local contracts have a fresh 353-edge formal Ninja build and
31/31 focused fixture sweep, with no selected `bop-v1` product route. Its
Common Controls v6 activation repair is confined to the source-derived BOP 59
dialog seam. See the [T238 closure](history/m0-t238-closure-20260822.md).

M0 T237 closes the XMS source-mirror package: `52:06` now reaches the
imported `xmsInitUMB` source body through a checked AX:BX state-byte seam;
later original A20 calls publish to that byte; and `52:07/08` retain their
empty-map `B1/B2` behavior. Fresh formal Ninja r7, both XMS fixtures, the
v1-retirement gate and the machine-semantic boundary gate pass. The detailed
record is [T237 S5 closure evidence](etc/operations/m0-t237-s5-xms-umb-a20-closure-001.md);
the closed package is in [T237 history](history/m0-t237-closure-20260822.md).

Successful physical UMB reservation and `52:09` keyboard/IVT behavior remain
explicitly transferred to the future selector-blind machine/BIOS owner.

T237 P2 closes top-level `59`: `MS_bop_9` retains the original per-thread
AX-category warning and Terminate/Ignore result, using public Win32 UI for the
non-WOW single-session path. Ignore resumes at `RIP+3`; Terminate is a typed
bx-vdm controlled stop, never a host-process termination.

## Current Work Record

M0 T236 S4 is closed as the direct OpenNT DEM DASD/IOCTL recovery: imported
`50:21`/`50:29`/`50:2A` source now has a full direct/source-failure/machine
transfer ledger, a guarded malformed-IOCTL boundary, and a real disposable
VHDX fixed-volume lifecycle witness. See the [S4 closure](history/m0-t236-s4-closure-20260821.md).
M0 T236 S6 is closed as the OpenNT DEM search fallback disposition: the
removed `VdmQueryDir` fast path is explicitly unavailable, while the imported
slow reset resumes and exhausts search correctly after a source-owned handle
eviction. See the [S6 closure](history/m0-t236-s6-closure-20260821.md).
M0 T236 S5 is closed as the COMMAND WOW source disposition: direct imported
`cmdGetNextCmd` remains the normal local path; its original `VDMForWOW` branch
has a verified source-shaped controlled stop rather than a fabricated WOW
launch, while kernel publication and shortcut consumption transfer to the
queued WOW16 owner package. See the [S5 closure](history/m0-t236-s5-closure-20260821.md).
M0 T236 S3 is closed as the direct OpenNT DEM `50:32`/`50:33` recovery: its
single imported dispatcher table, copied-register retry seam and unified
opaque-handle route are recorded in the [S3 closure](history/m0-t236-s3-closure-20260821.md).
M0 T234 is closed as the OpenNT-shaped dynamic COMMAND-environment and
32-bit opaque-handle correction package; see the [T234 closure](history/m0-t234-closure-20260820.md).
T235 subsequently closed the PIF CONFIG/AUTOEXEC startup selection and fixed
guest bundle. M0 T236 now owns the remaining minimum-modification DEM/COMMAND
source recovery. M0 T231 closed as the COMMAND `54:00..10` code-complete/v2
product-route package; see the [T231 closure](history/m0-t231-closure-20260820.md).
Native `54:xx` reachability remains transferred to the queued bx machine/BIOS
owner package. XMS remains a candidate package.

M0 T232 is closed as the initial unified host-handle cleanup package; its
16-bit ID conclusion was corrected by closed M0 T234. M0 T233/S1 was admitted
but received no source or evidence work before the owner superseded it with
this ABI correction; XMS returns to the unnumbered candidate queue.
## Current Technical Baseline

- T95 is closed by owner-authorized reprioritization. Its incomplete file/search
  capability and full real-path closure remain uncompleted.
- M0 T197 S6 has a reproducible CPU5/non-x86-64 native link frontier: r4
  compiles the explicit SIM/machine/memory and source-proven reset/decode CPU
  candidates, then stops at the original complete instruction-handler table
  and retained product-shell methods.  No GUI, plugin/device archive, adapter
  or OpenNT input entered the probe; see
  [T197 S6 link probe 001](etc/research/t197-s6-cpu5-minimal-link-probe-001.md).
- M0 T197 S6 r11 now source-builds and links the whole CPU5 adopted core with
  the finite machine, without a Bochs product target.  This is a native link
  closure only; machine initialization/execution still requires separate
  lifecycle verification.
- M0 T197 S6 r12 verifies the native bare-machine lifecycle in process:
  `initialize(1 MiB, 1 MiB) -> CPU5 reset/APIC/CPUID -> cleanup()` returns
  zero.  No instruction-loop, BIOS/reset image, BOP or guest-running claim is
  made by that lifecycle witness.
- M0 T198 S1 finds the retained BOP runtime is a historical product derivative:
  it retains `main.cc`, GUI/device archives and product configuration.  It is
  not link-compatible with the finite machine.  The first second-stage closure
  is therefore a selector-blind native execution contract, not a per-BOP
  implementation; see
  [T198 S1 composition map](etc/research/t198-s1-bop-runtime-composition-map-001.md).
- M0 T198 S2 is complete: native CPU and PC-time sources prove a
  machine-private reset/copy/entry/run contract.  Its bound is a finite
  instruction-tick budget rather than an exact retired-instruction counter;
  the existing product `benchmarkTimer` is rejected because it also mutates
  GUI exit state.  See
  [T198 S2 execution-contract map](etc/research/t198-s2-native-execution-contract-map-001.md).
- M0 T198 S3 is complete: r8 source-builds and runs neutral `HLT` bytes
  through CPU5 reset, checked RAM copy, real-mode entry, 64-tick native stop
  and cleanup.  The needed A20 initialization is now a finite machine
  lifecycle invariant; no guest/BOP runtime claim follows.  See
  [T198 S3 finite-run witness](etc/research/t198-s3-finite-native-run-witness-001.md).
- M0 T198 S4 is complete: the current `#UD` seam map separates retained
  copied-state/resume mechanics from legacy `bx-vdm` dispatch, raw callback,
  environment and product-lifecycle coupling.  S5 will extract one fixed
  core/machine bridge before any BOP plane attachment; see
  [T198 S4 generic #UD seam map](etc/research/t198-s4-generic-ud-seam-map-001.md).
- M0 T198 S5 P1 is complete: machine now owns a standalone fixed generic-UD
  copied record and default-decline bridge, independently MSVC/x86 compiled
  with no adapter/product vocabulary or dependency.  Core has not yet been
  switched; see
  [T198 S5 P1 bridge record](etc/research/t198-s5-p1-generic-ud-bridge-record-001.md).
- M0 T198 S5 P2 is complete: a new default-off core gate source-builds under
  CPU5/MSVC and invokes only the fixed machine record/typed outcome path; its
  stop uses the finite PC stop latch.  Historical parallel gates remain for
  P3 removal and are not part of the new composition; see
  [T198 S5 P2 core hook](etc/research/t198-s5-p2-core-machine-ud-hook-001.md).
- M0 T198 S5 P3 is complete: the legacy machine-composition callback,
  product-stop path and every quarantined legacy exception implementation are
  physically removed from core.  Fresh CPU5 r3 source-builds the minimal
  fixture and traverses `UD2` through the fixed bridge to native controlled
  stop; see [T198 S5 P3 physical cleanup witness](etc/research/t198-s5-p3-physical-core-cleanup-witness-001.md).
- M0 T198 S6 P1 is complete: the alternative `bx-vdm` bridge converts the
  copied generic record through common ingress and provider registry while
  declining every outcome.  Its UCRT64 C-island test covers one mapped entry
  and three negative routes; see [T198 S6 ingress bridge witness](etc/research/t198-s6-global-ingress-bridge-witness-001.md).
- M0 T198 S6 P2 is complete: the toolchain audit proves the available MinGW
  producer is x64-only, so the classification bridge is not an in-process
  MSVC/x86 artifact.  A next composed bridge requires an owner-selected x86
  producer or a separately designed process ABI; see [T198 S6 x86 toolchain
  audit](etc/research/t198-s6-x86-vdm-toolchain-audit-001.md).
- M0 T198 S7 P1 is complete: eight i686 MinGW C objects replace the default
  bridge in the MSVC/x86 CPU5 finite link; the macro-enabled HLT fixture
  links and returns zero without entering #UD.  See [T198 S7 cross-island
  witness](etc/research/t198-s7-cross-island-link-witness-001.md).
- M0 T198 S8 P1 is complete: the first adapter-local pending descriptor was
  corrected into a machine-owned, selector-blind copied ordinary-RAM action
  ABI.  A native CPU5/x86 fixture proves read, max-size bulk write/read and a
  rejected out-of-aperture multi-range write with no partial write; an i686 C
  witness proves the fixed layout.  No provider is attached; see [T198 S8
  mechanical action boundary witness](etc/research/t198-s8-mechanical-action-boundary-witness-001.md).
- M0 T198 S8 is complete: its generic action ABI is source-built and native
  CPU5/x86 verified, while provider and legacy-runtime linkage remain absent;
  see [T198 S8 closure](history/m0-t198-s8-closure-20260813.md).
- M0 T198 S9 P1 is complete: the whole boot-namespace facade, its exact
  read/write action dispositions and its sole pathname prerequisite are
  source-mapped; the leaf closure is i686 source-built without the retained
  runtime.  See [T198 S9 composition closure map](etc/research/t198-s9-boot-namespace-composition-closure-map-001.md).
- M0 T198 S9 is complete: the complete family closure and DTA prerequisite
  are recorded without linking the historical runtime; see [T198 S9
  closure](history/m0-t198-s9-closure-20260813.md).
- M0 T198 S10 P1 is complete: a source-built i686 whole-family plane owns the
  immutable namespace and pending read state through common classification;
  it covers COMMAND boot write, DEM open/read and no-DTA pathname deferral.
  See [T198 S10 P1 witness](etc/research/t198-s10-p1-provider-plane-witness-001.md).
- M0 T198 S10 P2 is complete: the same plane fixture now verifies DEM seek
  and close direct-result routes after the open/read lifecycle; valid-DTA
  pathname and malformed-action cases remain P3.
- M0 T198 S10 P3 is complete: explicit valid DTA now admits pathname first
  `50:09` through its 134-byte read and provider-produced write.
- M0 T198 S10 P4 is complete: pathname next `50:0B` now consumes the first
  provider write as an addressed DTA image, rejects a mismatched action ID
  without consuming the pending read, then completes the valid fixed action.
  See [T198 S10 P4 witness](etc/research/t198-s10-p4-path-next-witness-001.md).
- M0 T198 S10 is complete: its selected whole-family plane is source-built
  and tested without legacy runtime linkage; its closure transfers only the
  generic-UD composition question to S11. See [T198 S10 closure](history/m0-t198-s10-closure-20260813.md).
- M0 T198 S11 P1 is complete: the generic event/outcome and mechanical action
  records already close the synchronous inner transaction. The only gaps are
  adapter-owned explicit session lifetime and action-result propagation; no
  core/machine ABI or semantic addition is indicated. See [T198 S11 P1 map](etc/research/t198-s11-p1-synchronous-composition-map-001.md).
- M0 T198 S12 P1 is complete: an explicit adapter-owned session now executes
  copied write/read actions synchronously and returns typed generic outcomes;
  unbound and executor-failure paths decline. COMMAND write results now resume
  instead of re-faulting. See [T198 S12 P1 witness](etc/research/t198-s12-p1-adapter-composition-witness-001.md).
- M0 T198 S12 P2 is complete: the core-facing alternative bridge is now a
  single adapter-session delegate, retaining default decline until bind. Its
  i686 transaction fixture and updated boundary scan pass. See [T198 S12 P2
  witness](etc/research/t198-s12-p2-composed-bridge-witness-001.md).
- M0 T198 S12 P3 reached and records the true native cross-island frontier:
  complete MinGW provider objects cannot safely link into the MSVC/x86 CPU5
  executable because they import a second CRT/helper universe. The probe did
  not run; no Bochs defect is inferred. See [T198 S12 P3 frontier](etc/research/t198-s12-p3-native-cross-island-link-frontier-001.md).
- M0 T198 S13 is complete: the full CPU5/Pentium-MMX source membership and
  finite machine source-build, link and run as one MSVC x64 `/MT` PE32+ process
  lifecycle. The x64 configuration projection corrects host pointer width
  without changing guest CPU identity; see [T198 S13 witness](etc/research/t198-s13-msvc-x64-minimal-lifecycle-witness-001.md).
- M0 T198 S14 is complete: a neutral one-byte HLT fixture enters the CPU5
  instruction loop and reaches finite controlled stop under the same MSVC x64
  `/MT` closure, with no adapter/provider or product input; see [T198 S14
  witness](etc/research/t198-s14-msvc-x64-finite-hlt-witness-001.md).
- M0 T198 S15 is complete: r6 repeats the neutral UD2 fixture under MSVC x64
  `/MT` and observes the selector-blind copied-event bridge's typed controlled
+  stop. Earlier r1--r5 observations are retained as rejected diagnostics;
  no BOP/provider/OpenNT claim follows. See [T198 S15 observation](etc/research/t198-s15-msvc-x64-generic-ud2-observation-001.md)
  and [closure](history/m0-t198-s15-closure-20260813.md).
- M0 T198 S16 is complete: the retained boot-namespace composition and its
  full session fixture now source-build, link and run as one MSVC x64 `/MT` C
  closure. The only platform library is `bcrypt.lib`, required by the existing
  CLI BYOB startup-contract model; no native-machine or historical runtime
  object entered. See [T198 S16 witness](etc/research/t198-s16-msvc-x64-boot-namespace-composition-witness-001.md)
  and [closure](history/m0-t198-s16-closure-20260813.md).
- M0 T198 S17 is complete: the existing adapter session is now attached to
  the CPU5 native generic-`#UD` mechanics under one MSVC x64 `/MT` process.
  A real `C4 C4 54:0C` BOP form completes its existing checked ordinary-RAM
  transaction and resumes to the bounded terminal fixture. This is not a DOS
  continuity claim; see [T198 S17 witness](etc/research/t198-s17-msvc-x64-native-bop-composition-witness-001.md)
  and [closure](history/m0-t198-s17-closure-20260813.md).
- M0 T198 S18 P1 is complete: the existing DEM readonly lifecycle `50:12 ->
  50:16 -> 50:00 -> 50:02` now executes as one native real-mode transaction
  under the S17 x64 closure. See [S18 DEM witness](etc/research/t198-s18-native-dem-lifecycle-witness-001.md).
- M0 T198 S18 P2 is complete: the sibling COMMAND `54:0D` boot-file service
  now has a separate parameterized native fixture and x64 zero-exit witness;
  see [S18 COMMAND witness](etc/research/t198-s18-native-command-bootfile-witness-001.md).
- M0 T198 S18 P3 is complete: the existing DTA registration and pathname
  search branches run natively as `50:1B -> 50:09 -> 50:0B -> HLT`, including
  their checked guest-RAM reads/writes and typed resumes.  See [S18 DTA/search
  witness](etc/research/t198-s18-native-dta-search-witness-001.md) and the
  updated [S18 matrix](etc/research/t198-s18-boot-namespace-native-matrix-001.md).
- M0 T198 S19 P1 establishes the first original NTDOS guest-entry map:
  `50:11` is a live `DI:0000` request after NTIO entry, not an initial preload;
  its unavailable historical composition has one bounded source-derived
  provider boundary.  See [S19 guest-entry map](etc/research/t198-s19-first-ntdos-guest-entry-map-001.md).
- M0 T198 S20 P1 composes source-derived DEM `50:11` into the one active
  `bx-vdm` session. Its x64 C closure and CPU5 native fixture prove one checked
  NTDOS publication, typed resume and negative controlled stop; see [S20
  witness](etc/research/t198-s20-native-dem-load-dos-witness-001.md).
- M0 T198 S21 P1 closes the NTIO pre-entry contract: pre-entry publication is
  a new selector-blind machine request, not a startup BOP or Bochs semantic;
  entry changes only CS:IP and stops at the already-admitted `50:11` boundary.
  See [S21 pre-entry map](etc/research/t198-s21-ntio-preentry-contract-map-001.md).
- M0 T198 S22 closes the selector-blind pre-entry primitive: a versioned,
  copied v2 request preflights a one-MiB aperture, captures/restores opaque
  bytes across image publication, and then applies CS:IP.  The full CPU5
  MSVC x64 `/MT` fixture proves restoration and rejection before mutation; see
  [S22 witness](etc/research/t198-s22-machine-preentry-witness-001.md).
- M0 T198 S23 P1 source-builds the `bx-vdm` NTIO request builder under MSVC
  x64 `/MT`: it copies only the exact `0x8400` image into machine v2 and
  source-locks `0x700`, `0x714`, `0070:0000`; execution is pending. See
  [S23 P1 builder record](etc/research/t198-s23-p1-ntio-request-builder-001.md).
- M0 T198 S23 P2 native-runs the exact source-built NTIO through `0070:0000`
  and observes real `50:11` before a later adapter STOP; see [S23 P2
  witness](etc/research/t198-s23-p2-native-ntio-first-boundary-001.md).
- M0 T198 S23 P3 restores generic accepted-STOP result observability: the
  finite CPU5 x64 UD2 regression returns typed controlled stop rather than
  conflating it with the watchdog timer.
- M0 T198 S23 now has a strict native first-handoff result: exact NTIO reaches
  `50:11`, then a later unimplemented boundary returns
  `COMPLETED_UD_STOP`; no DOS-continuity claim follows.
- M0 T198 S23 P4 maps that later boundary as top-level BIOS `BOP 12h`
  `memory_size`; the logged trailing `B1` is the next guest instruction, not
  a service ID. See [S23 P4 map](etc/research/t198-s23-p4-post-dem-bop12-map-001.md).
- M0 T198 S23 closes the exact NTIO first-handoff slice: its admitted `12h`
  result is native-resumed under MSVC x64 `/MT`, while `15h/AH=88h` remains
  explicitly declined; the next real boundary is DEM `50:3B`. See [S23
  closure](history/m0-t198-s23-closure-20260813.md).
- M0 T198 S24 P1 moves original DEM `50:3B` into a dedicated DEM-MISC plane
  entry and proves its non-debug `AL=0`/`RIP+4` result through the current
  MSVC x64 `/MT` CPU5 composition. Its post-resume HLT is now known to be the
  fixture mock NTDOS endpoint, not a later guest boundary; see [S24
  P1](etc/research/t198-s24-p1-dem-isdebug-native-resume-001.md).
- M0 T198 S24 P2 retains the two original HLT source sites as static facts,
  but its runtime attribution is superseded by the mock-NDTOS finding; see
  [S24 P2](etc/research/t198-s24-p2-native-hlt-source-triage-001.md).
- M0 T198 S24 P3 admits and verifies a default-off machine-private terminal
  CS:IP snapshot. The corrected `956B:0001` result is the deliberate mock
  NTDOS endpoint, not an NTIO/sysinit failure; see [S24
  P3](etc/research/t198-s24-p3-terminal-snapshot-001.md).
- M0 T198 S25 P1 embeds the exact locked source-built NTDOS input and reaches
  the first post-NDTOS real boundary, `54:05`, under MSVC x64 `/MT` CPU5;
  see [S25 P1](etc/research/t198-s25-p1-source-built-ntdos-next-boundary-001.md).
- M0 T198 S25 P2 composes original COMMAND `54:05` through a bounded launch
  state plane and reaches the next original boundary `50:1B`; see [S25
  P2](etc/research/t198-s25-p2-command-setinfo-native-001.md).
- M0 T198 S25 P3 corrects the existing DTA prerequisite's original DEM GSET
  gate: exact NTIO/NTDOS now completes `50:1B` through its checked two-byte
  guest-RAM read and typed resume, then reaches unimplemented original DEM
  `50:32`; see [S25 P3](etc/research/t198-s25-p3-dem-gset-dta-native-001.md).
- M0 T198 S26 P1 composes original DEM error-lock registration `50:32` through
  its existing two-locator provider and reaches the next unimplemented
  boundary `50:45`; see [S26 P1](etc/research/t198-s26-p1-dem-error-lock-native-001.md).
- M0 T198 S26 P2 composes non-debug `50:45` and snapshot-backed `50:0F`, then
  reaches the fixture's controlled stop; a separately retained extension run
  exposes an unmapped VGA machine frontier rather than authorizing devices.
  See [S26 P2](etc/research/t198-s26-p2-dem-init-trio-native-001.md).
- M0 T198 S27 establishes that `50:0F` itself cannot justify VGA: the retained
  extension reaches real-mode `LSL` before its `0xA2E63` VGA-aperture prefetch
  veto, while the original DOS init order is not yet proven continuous. See
  [S27 audit](etc/research/t198-s27-post-drive-machine-audit-001.md).
- M0 T198 S28 proves the retained unrestricted run does not first reach the
  proposed post-drive capture point: it hits six real-mode `LSL` reports and
  the same VGA-aperture panic before any observed drive resume. The temporary
  nonpassing probe was removed. See [S28 probe](etc/research/t198-s28-post-drive-generic-ud-probe-001.md).
- M0 T198 S29 captures the actual first non-BOP fault after a `50:0F` resume:
  real-mode `0000:0A84`, vector 6, with a copied all-`FF` window. It stops
  before the former VGA panic and moves the frontier to low-RAM control flow.
  See [S29 capture](etc/research/t198-s29-first-generic-ud-capture-001.md).
- M0 T198 S30 proves `0x0A84` lies inside the loaded `0x0700..0x8AFF` NTIO
  aperture, whose source bytes there are not all `FF`; the current CS also
  differs from the entry CS. The frontier is a guest mutation/transfer target,
  not an IVT, firmware, VGA, or device-admission request. See
  [S30 audit](etc/research/t198-s30-low-ram-frontier-ownership-audit-001.md).
- M0 T198 S31 adds a one-shot, opaque, checked machine terminal-RAM snapshot.
  The exact CPU5 run records `0x0A80..0x0A8F` as differing from its published
  NTIO bytes and confirms `0x0A84..0x0A87 = FF FF FF FF` at generic stop; no
  core, BOP, device, or CLI behavior changed. See
  [S31 snapshot](etc/research/t198-s31-terminal-ram-snapshot-001.md).
- M0 T198 S32 captures the complete existing copied event without pointer
  retention: its first generic fault has `SS:SP=00A7:090A` (physical
  `0x137A`), separate DS/ES state, and the same all-FF window/snapshot. The
  next evidence target is the raw stack range, not a device or service. See
  [S32 state](etc/research/t198-s32-first-generic-event-state-001.md).
- M0 T198 S33 captures the raw stack range around that pointer:
  `00A7 090E FFFF BF1F 0046 4D58 9346 76F6`. It proves a nonempty stack but
  cannot safely label a return pair; next is a read-only audit for existing
  selector-blind native instruction-history facilities. See
  [S33 stack](etc/research/t198-s33-first-generic-stack-snapshot-001.md).
- M0 T198 S34 rejects all existing instrumentation candidates: the CPU5 build
  compiles `BX_INSTRUMENTATION=0`, while the available active modules require
  a broader feature closure and raw CPU/logging coupling. A future trace must
  be a separately admitted fixed-width core-to-machine mechanic, never a BOP or
  debugger path. See [S34 audit](etc/research/t198-s34-instruction-history-seam-audit-001.md).
- M0 T198 S36 is complete: the default-off registered chained-step history
  rebuilt the full CPU5 source closure under MSVC x64 `/MT` and captured the
  first non-BOP generic fault `0000:0A84` with immediate predecessor
  `0000:0A82` on consecutive counts. This confirms a low-RAM
  mutation/control-transfer frontier, not a missing BOP handler. See
  [S36 witness](etc/research/t198-s36-chained-step-history-witness-001.md).
- M0 T198 S37 is complete: OpenNT `msinit` and DEM source prove current
  `50:0F` returns into legitimate guest low-memory/stack/IVT initialization,
  while current T198 evidence proves the old unhandled-`50:0F` explanation is
  stale. The exact CS-to-zero transfer and low-RAM writer remain unproven; no
  device, firmware or BOP change is admitted. See
  [S37 attribution](etc/research/t198-s37-low-ram-transfer-attribution-001.md).
- M0 T198 S38 is complete: the registered, default-off machine-only query
  proves the latest retained code-segment transition before the generic stop
  is `9346:4D53 -> 0000:0000`; the known `0000:0A82 -> 0000:0A84` pair occurs
  later. The transfer instruction and owner remain unproven, and no machine
  capability was added. See
  [S38 witness](etc/research/t198-s38-cs-transition-witness-001.md).
- M0 T198 S39 is complete: source/evidence audit proves the finite fixture
  restores an undefined bare-RAM virtual-interrupt span and omits the other
  evidenced NTIO pre-entry inputs. Its startup trace is therefore input
  contaminated, not evidence for a missing device or BOP. See
  [S39 attribution](etc/research/t198-s39-preentry-contract-attribution-001.md).
- M0 T198 S40 is complete: the selected profile's complete currently
  evidenced pre-entry snapshot is atomically installed through the existing
  machine mechanical action. The old all-`FF` `0A84` frontier is gone; the new
  reproducible `0000:0047` frontier follows the still-unexplained
  `9346:4D53 -> 0000:0000` transfer. See
  [S40 closure](etc/research/t198-s40-preentry-input-closure-001.md).
- M0 T198 S41 is complete: the registered copied position record proves that
  the unexpected CS transition changes SS:SP from `00A7:0918` to
  `00A7:090E`. The frame owner remains unproven. See
  [S41 witness](etc/research/t198-s41-stack-position-witness-001.md).
- M0 T198 S42 is complete: the registered bounded machine provenance record
  proves that the preceding guest instruction is `SS: CALL FAR [037A]` and
  retains the matching `9346:4D58` far return address; S44 corrects its
  non-direct `0000:0000` operand inference. It does not establish a BOP,
  device, or adapter requirement. See [S42 control-transfer
  provenance witness](etc/research/t198-s42-control-transfer-provenance-001.md).
- M0 T198 S43 is complete: the original NTDOS `DEVIOCALL2` source and locked
  image map identify `SS:[037A]` as the DOSDATA `CALLDEVAD` scratch slot;
  S44 corrects the earlier unproven missing-device-chain inference. See
  [S43 pointer attribution](etc/research/t198-s43-devio-call-pointer-attribution-001.md).
- M0 T198 S44 is complete: the existing terminal snapshot observes
  `CALLDEVAD=BF1F:FFFF`, moving the remaining question to original
  end-of-segment execution continuity. See [S44 terminal
  witness](etc/research/t198-s44-call-dev-ad-terminal-witness-001.md).
- M0 T198 S45 is complete: the original split-at-`FFFF` strategy layout and
  unchanged upstream CPU prefetch path correlate with the `EIP=0x10000`
  diagnostic; a neutral reproduction is required before repair. See [S45
  source audit](etc/research/t198-s45-realmode-ip-wrap-source-audit-001.md).
- M0 T198 S46 is complete: the neutral split-instruction fixture repeats the
  exact `EIP [00010000] > CS.limit [0000ffff]` prefetch diagnostic with no
  guest composition input. S47 is therefore a generic core repair, not an
  adapter workaround. See [S46 neutral witness](etc/research/t198-s46-neutral-realmode-ip-wrap-witness-001.md).
- M0 T198 S47 is complete: one selector-blind code-offset-width helper repairs
  the neutral `CS:FFFF` instruction path; its HLT and generic UD2 STOP
  regressions pass after finite early-stop cleanup. See [S47 repair
  witness](etc/research/t198-s47-realmode-ip-width-repair-001.md).
- M0 T189 S5 is complete: the four-object source closure proves that the
  previous `54:0C` preparation decline came from stale retained provider and
  command-service objects, not a proven OpenNT or Bochs defect.  The valid
  frozen run commits the existing boot-file transaction and reaches controlled
  stop; it does not claim continuous execution.
- M0 T189 is closed: its valid trace transfers original COMMAND `54:02`
  `cmdComSpec` rather than a Bochs/machine issue; T190 begins with the paired
  `54:02`/`54:0F` source contract map.
- T96 S1 P1 is complete locally: all inventoried selector, service, and BIOS
  forms now have source routing/owner evidence without an enabled provider.
- Bochs remains generic-machine owner. The adapter owns a bounded copied BOP
  ingress and typed result transition, not DOS, BIOS, CCPU, or device behavior.
- Provider precedence is mandatory: original OpenNT host provider; original
  provider with contained CLI backend; source-derived provider only after a
  recorded blocker; otherwise unavailable/deferred.
- M0 T96 S3 is closed: its DEM plane classifies every callable service, and
  all admitted DEM slices are plane-gated. Legacy runtime endpoint migration
  remains expressly deferred to S8.
- M0 T96 S5 is closed: all XMS/DPMI services have a machine-owner disposition;
  only DPMI VCD is explicitly unavailable, and no native-machine operation is
  represented as adapter behavior.
- M0 T96 S6 is closed: all Redirector and debugger services have source-backed
  profile dispositions; selector-level VDD, mouse and device boundaries remain
  explicitly outside adapter execution.
- M0 T96 S7 is closed: the complete SoftPC BIOS inventory has native-owner,
  unavailable or collision disposition, without importing a BIOS dispatcher.
- M0 T96 S8 is closed: legacy runtime identities are registered and gated or
  rejected, with a scan preventing unapproved direct BOP recognizers.
- M0 T96 S9 is closed: complete inventory/profile regression covers all
  selectors, service identities and BIOS forms without claiming runtime.
- M0 T96 is closed: its real-path checkpoint exposes the original-unavailable
  DEM FASTREAD edge, without changing Bochs or fabricating a BOP outcome.
- M0 T198 S78 corrects the retained #UD display interpretation:
  `window=15:ffffffff` is a 15-byte window beginning `FF FF FF FF`, not BOP
  `15h`. Its ROM/IVT closure map identifies absent approved ROM payloads as
  the current machine-input gap; no BIOS/BOP route is added. See [S78 closure
  map](etc/research/t198-s78-minimum-rom-ivt-closure-map-001.md).
- M0 T198 S79 finds no current source-authorized C000 image producer: OpenNT
  retains names only, while Bochs VGABIOS is prebuilt and requires excluded
  VGA lifecycle. S80 therefore audits the original display-mode selection
  before any profile or machine decision. See [S79 producer
  audit](etc/research/t198-s79-machine-image-producer-audit-001.md).
- M0 T198 S80 establishes the original display-state chain: BOP `5F` supplies
  the resident mode byte, and only fullscreen jumps to the saved native INT 10
  vector. S81 is observation only; it will not select a CLI mode or attach a
  provider. See [S80 chain audit](etc/research/t198-s80-display-mode-vector-chain-audit-001.md).
- M0 T198 S81 confirms that the current source-built full trace does not reach
  BOP `5F` before the C000 transfer. S82 now audits the guest-image and
  initialization path, without forcing a mode byte or adding a provider. See
  [S81 passive observation](etc/research/t198-s81-bop5f-passive-observation-001.md).
- M0 T198 S82 proves that `073B:0939` is a dynamically patched SpcKbd native
  INT10 far jump, not random Bochs control flow. M0 T198 S83 brackets the
  pointer write after first `50:11`; M0 T198 S84 proves BOP-`5F` was consumed
  before the passive recorder; M0 T198 S85 classifies its host-contract delta;
  M0 T198 S86 selects the explicit stream profile. S87 implements only
  its source-derived K.SYS state publication. See [S86 profile design](etc/research/t198-s86-cli-stream-profile-design-001.md).
- M0 T198 S87 is complete: an explicit `STREAM_IO=2` input permits only the
  exact BOP-`5F` two-byte K.SYS offset read followed by one typed ordinary-RAM
  byte write and the retained CF/RIP continuation. The new API is intentionally
  unbound because no BYOB profile field exists yet; see [S87 transaction
  record](etc/research/t198-s87-spckbd-stream-state-transaction-001.md).
- M0 T198 S88 is complete: profile v7 now selects only `stream-io-v1`, which
  becomes the immutable value `2` in composition v2; that composition performs
  S87's exact BOP-`5F` transaction or declines it rather than falsely resuming.
  See [S88 profile/composition record](etc/research/t198-s88-profile-stream-composition-001.md).
- M0 T198 S89/S90 are complete: a current-object MSVC x64 native run proves
  that the exact BOP-`5F` CS:SI table / DS-relative state transaction selects
  the guest stream branch and avoids the former C000 transfer.  Its next
  boundary is `50:3D`, which S91 source-classifies as fatal `demExitVDM`, not a
  normal return; no provider or runtime behavior was added.  See [S89/S90
  closure](etc/research/t198-s89-s90-native-spckbd-cs-si-closure-001.md) and
  [S91 classification](etc/research/t198-s91-dem-exit-vdm-classification-001.md).
- M0 T198 S92 is complete: the profile-declared SHARE normal-return candidate
  is present and hash-verified, but the current native fixture contains only
  synthetic COMMAND/target bytes.  Repeating it cannot test `50:3C` or
  `54:11`; S93 owns identity-locked fixture inputs before any next run. See
  [S92 prerequisite map](etc/research/t198-s92-normal-return-candidate-prerequisite-map-001.md).
- M0 T198 S93 is complete: the exact source-built COMMAND.COM and SHARE.EXE
  inputs, plus an explicit v7 stream/TARGET.EXE profile, source-build into the
  native fixture branch without link or guest execution.  S94 may make one
  bounded exact-input observation; see [S93 preparation](etc/research/t198-s93-source-built-normal-return-fixture-preparation-001.md).
- M0 T97 S1 is complete: no original standalone provider exists; the frozen
  read-only and search candidates are only implementation inventory.
- M0 T97 S2 is complete: partial runtime dispatch and the contradictory
  FASTREAD experiment were withdrawn; the immutable token design is retained.
- M0 T97 S3 is complete: pathname and FCB searches have separate source
  contracts, and the host-directory fixture is expressly excluded.
- M0 T97 S3 P1 is complete: pathname has a source-shaped future-provider
  boundary; FCB remains explicitly deferred pending its distinct ABI.
- M0 T97 S3 P2 records that the existing host-directory search fixture is
  outside profile and not linkable on the current SDK; it will not be patched.

## Recent Progress

| Scope | Compact result |
| --- | --- |
| M0 T95 | Closed/replanned; closure facts, transferred backlog, and limitations are in history/m0-t95-closure-20260811.md. |
| M0 T189 S5 | Complete: four-object derivative and one manifest-bound observation commit `54:0C` and reach controlled stop; S6 is source-only audit. |
| M0 T189 | Closed: stale object closure is proven; `54:02` COMMAND bootstrap contract transfers to T190. |
| M0 T96 S1 P1 | Complete static BOP selector/service/BIOS provider and module map; local commit pending because no Git remote is configured. |
| M0 T96 S2 P1--P2 | Complete shared ingress, typed pass-through gate, and provider selection registry; legacy endpoint migration remains S8. |
| M0 T96 S3 | Complete: all 73 callable DEM services have a component/disposition, and every admitted DEM slice is plane-gated; legacy runtime migration remains S8. |
| M0 T96 S4 | P1--P5 complete: COMMAND map/plane/no-op are closed; CONFIG/AUTOEXEC and target-launch legacy endpoints are reconciled as S8 migration objects. |
| M0 T96 S5 | Complete: 12 XMS and 25 DPMI services are source-mapped and plane-classified without linking historical dispatchers. |
| M0 T96 S6 | Complete: Redirector is source-proven unavailable; debugger and native device selectors remain deferred to their proper owners. |
| M0 T96 S7 | Complete: 41 definitions and 349 historical BIOS table forms have a mechanically verified ownership disposition. |
| M0 T96 S8 | Admitted in Ordinary Mode; existing endpoint migration and no-bypass verification begins. |
| M0 T96 S8 P1 | Complete: legacy endpoint register and no-bypass scanner are active; `host_service_catalog` now consumes common ingress rather than decoding BOP bytes. |
| M0 T96 S8 P2 | Complete: a generic DEM/COMMAND plane gate is verified; the first six runtime endpoints are gate-only migrations with no semantic expansion. |
| M0 T96 S8 P3 | Complete: all DEM/COMMAND legacy endpoint entry stages with a proven service identity now pass the common plane gate; runtime syntax verification passed. |
| M0 T96 S8 P4 | Complete: the conflicting legacy DEM FASTREAD path is removed from runtime, preserving its S3 original-no-op disposition. |
| M0 T96 S8 | Complete: legacy recognizers are plane-gated, frozen or rejected; no-bypass verification is active. |
| M0 T96 S9 | Admitted in Ordinary Mode; full inventory/provider/capability regression begins. |
| M0 T96 S9 | Complete: 192 callable services traverse ingress, registry and plane classification; inventory counts are independently verified. |
| M0 T96 S10 P1 | Complete checkpoint: r49 linked all T96 plane/gate objects and a fresh bounded NTIO/NTDOS run reached the original-unavailable `50:42` boundary; see etc/research/t96-s10-real-path-checkpoint-001.md. |
| M0 T96 | Closed: source ownership, the holistic BOP plane, and the current real-path boundary are recorded in history/m0-t96-closure-20260812.md. |
| M0 T97 S1 | Complete: component/ABI/failure and composition map records the source-derived replacement boundary. |
| M0 T97 S2 | Complete: provider withdrawal and immutable token/resource contract recorded; no FASTREAD helper remains. |
| M0 T97 S3 | Active: pathname/FCB directory-search ABI audit. |
| M0 T97 S3 P1 | Complete: separate DTA and FCB buffer/error/continuation map recorded; no search runtime provider is enabled. |
| M0 T97 S3 P2 | Complete limitation: host-directory fixture is excluded from T97 and its SDK-native query branch is not repaired. |
| M0 T97 S4 P1 | Complete: v4 BYOB profiles admit an exact, immutable four-entry DOS metadata snapshot; pathname routing remains disabled pending the common-plane integration slice. |
| M0 T97 S4 P2 | Complete focused closure: common DEM-plane `50:09/0B` routing consumes only copied guest inputs and the immutable v4 snapshot; FCB and ambient host namespace remain excluded. The legacy runtime fixture needs replacement because it asserts the withdrawn file-service path. |
| M0 T97 S4 P3 | Complete: default runtime regression no longer asserts withdrawn file-token services; its focused v4 mode passes DTA registration, `50:09` success, and `50:0B` no-more. |
| M0 T97 S5 | Complete: v4 acceptance/rejection fixtures pass; the retained engine rejects the v4 execution plan before CPU loop, so zero BOP/transaction is correctly classified as stale engine composition. |
| M0 T97 | Closed: finite profile namespace and pathname provider are source/runtime closed; engine recomposition is transferred to T98. |
| M0 T98 S1 | Complete: retained engine/adapter object and link-closure inventory. |
| M0 T98 S1 P1 | Complete: r5 manifest/makefile/map establishes the exact missing current adapter/profile object set; a narrow derivative relink is the only admitted next build candidate. |
| M0 T98 S1 P2 | Complete: r2 generator/dry-run retains r5 Bochs inputs and exposes only 65 adapter/CLI compilations plus one final link; inherited archive recursion was rejected. |
| M0 T98 S2 | Complete: r3 compiled 66 adapter/CLI objects (including BOP observation) and linked the current composition without a Bochs rebuild. |
| M0 T98 S3 | Complete: bounded v4 trace passed the stale pre-CPU rejection, observed 15 BOPs and three commits, and ended at the benchmark budget. |
| M0 T98 | Closed: current adapter-to-engine composition is source-built and bounded-runtime observed; continuous guest execution remains unproven. |
| M0 T99 S1 | Complete: reached service families map to original components; finite read-only boot namespace is the selected whole provider slice. |
| M0 T99 S2 P1 | Complete: one provider-owned finite namespace state now combines paired config-path production, normal read-only lifecycle and search state; focused producer/consumer test rejects FCB routing. |
| M0 T99 S2 P2 | Complete: v4 runtime now consumes the provider for CONFIG/AUTOEXEC and pathname-search state; narrow engine relink and one bounded `54:0C` trace pass without a Bochs rebuild. |
| M0 T99 S2 P3 | Complete: normal DEM open/seek/read/close moved into the same provider/session family; focused lifecycle passes and the r2 bounded run reaches `54:0C`, `50:12`, `50:00`, then original-unavailable `50:42`. |
| M0 T99 | Closed: finite read-only boot namespace provider is source/test/build closed; one real consumer reaches the next original-unavailable DEM boundary, with no continuous-execution claim. |
| M0 T100 S1 | Complete: whole original-unavailable DEM provider composes through ingress/registry/plane and source/test/build/runtime evidence. |
| M0 T100 | Closed: all eight original-unavailable DEM slots compose through common selection; trace proves `50:42` resume and later close before the separate Bochs prefetch-map stop. |
| M0 T101 | Closed: `0xAF6AF` is a native VGA-aperture execution veto, not a missing mapping/device; upstream transfer remains unobserved. |
| M0 T102 | Closed: retained original diagnostics already prove the zero INT10 vector/direct-entry cause; no new Bochs diagnostic is needed. |
| M0 T103 | Closed: profile payload and handoff pairing are correct; direct/deferred consumer mutual exclusion and the two-object closure are proven. |
| M0 T104 | Closed: current-adapter deferred derivative rebuilt exactly two Bochs objects and reached the real native POST BOP path without an aperture panic. |
| M0 T105 | Closed: one longer unchanged-binary observation reached a stable source-owned INT6/BOP-06 re-entry loop; no service behavior was changed. |
| M0 T106 | Closed: common gather/read/write/resume bridge is connected; the repeated predecessor is original `MS_bop_2` VdmRedir-unavailable behavior. |
| M0 T107 | Closed: exact unavailable result is source/test/build closed; short trace did not reach it. |
| M0 T108 | Closed: selector 02 is runtime-reachable, but its accepted result exposed a source-identity error: `02` is XMS, not VdmRedir. |
| M0 T109 | Closed: selector 02 is unclaimed; the focused static test proves `52h` XMS and `57h` VdmRedir identities, with no runtime claim. |
| M0 T110 | Closed: selector 02 is SoftPC `unexpected_int` plus guest IRET, with PIC/BIOS-data side effects; it is not an adapter service. |
| M0 T111 | Closed: original semantics require Bochs-owned PIC port dispatch and RAM; direct link and adapter reimplementation are both rejected. |
| M0 T112 | Closed: source closure is finite but current ABI cannot synchronously transport multi-step native I/O; generic port API is rejected pending original compile proof. |
| M0 T113 | Closed: unmodified original handler compiles in fresh clang-cl/x86 OBJECT island; only `_inb`, `_outb`, `_c_sas_store` remain semantically unresolved. |
| M0 T114 | Closed: three wrapper ABI is finite and same-binary viable, but no owner may currently select/enter an original machine BOP handler. |
| M0 T115 | Closed: T115 selects a catalogue-driven machine-composition component distinct from the adapter host-service plane and records its exact future seam. |
| M0 T116 | Closed: `02h` contract, original object and three wrappers are static/x86-link closed without handler invocation. |
| M0 T117 | Closed: selector-blind default-off seam source-builds in a two-object derivative with unchanged decline behavior. |
| M0 T118 | Closed: original `02h` caller/IRET continuation and generic three-byte resume contract are source-proven. |
| M0 T119 | Closed: original `unexpected_int` and its exact three mechanics imports link into the default-off, selector-blind derivative; no runtime result is claimed. |
| M0 T120 | Closed: the one direct-start r3 observation ends at the known aperture veto with BOP identity observation disabled; it neither proves nor disproves `02h` reachability. |
| M0 T121 | Closed: deferred POST, the selector-blind machine seam and existing BOP listener source-build in one no-archive-rebuild derivative. |
| M0 T122 | Closed: the frozen deferred image reaches 31 observed BOPs and normal CPU-loop shutdown; `02h` is not observed and no machine handler result is claimed. |
| M0 T123 | Closed: thin CLI-to-Bochs shim startup descriptor is frozen. |
| M0 T124 | Closed: CLI-only shim source/test closure verifies the fixed, manifest-identified bundle contract with a fake native parser; real Bochs remains T125 evidence. |
| M0 T125 | Closed: fixed bundle reaches the native parser/POST and a repeated `50:11` BOP path; watchdog terminates the bounded process tree, so no continuous-execution claim is made. |
| M0 T126 | Closed: `50:11` is original `demLoadDos`; its existing source-derived transaction is linked, but T121 explicitly disabled the generic CPU result bridge required to commit/resume it. |
| M0 T127 | Closed: one source-identical derivative enables only the generic CPU-result bridge; `cpu/exception.o` and final map build with no archive or device expansion. |
| M0 T128 | Closed: CPU-result bridge alone remains inert because its call site is nested below `STARTUP_TRANSACTION=0`; the observed `50:11` loop remains a composition fact, not a new BOP. |
| M0 T129 | Closed: the obsolete parallel startup/result bridge is rejected; its unresolved imports cannot become a second adapter composition. |
| M0 T130 | Closed: selector-blind machine composition reaches original `unexpected_int` and resumes through native Bochs mechanics; continuous execution remains unproven. |
| M0 T131 | Closed: full SoftPC BIOS inventory has verified native, unavailable, or collision ownership without a BIOS dispatcher. |
| M0 T132 | Closed: the sole original handler island has an unresolved CRT-model conflict and cannot be copied as a general provider recipe. |
| M0 T133 | Closed: the deferred composition already has native POST/IVT/INT10 lifecycle; no reset/device correction is admitted. |
| M0 T134 | Closed: the late fault is original INT 0Dh/BOP 02 return looping on guest control state; its upstream control transfer remains unobserved. |
| M0 T145 | Closed: no retained exact COMMAND source-offset map/listing binds the controlled COM image tail to a source label; heuristic attribution remains rejected. |
| M0 T146 S1 | Complete: one r5-pinned CPU archive enables only BX-TRACE-056 and links against byte-identical T130 non-CPU/mechanics inputs; see etc/research/t146-s1-single-diagnostic-cpu-archive-closure-001.md. |
| M0 T146 | Closed: its single observation stopped early at `50:36`, so it neither reaches nor explains the late prefetch fault; retry and semantic changes are rejected. |
| M0 T147 | Closed: full CPU archive recompilation leaves an unresolvable provenance variable; exactly-one archive-member replacement is the next minimal comparison. |
| M0 T148 | Closed: a one-member CPU archive derivative retains 92 byte-identical CPU members and enables only BX-TRACE-056. |
| M0 T149 | Closed: the sole extended observation reaches and repeats the known prefetch/#GP/BOP02/IRET loop; watchdog prevents a clean completion claim. |
| M0 T150 | Closed: original INT 0Dh/BOP02/IRET source proves replay of the existing fault frame, not a new adapter or machine-service failure. |
| M0 T151 | Closed: exact staged COMMAND image ends at `0BC1:C5CF`; the `0BC1:FFFF` fault lies outside it. |
| M0 T152 | Closed: original x86 fast-read selects the visible no-op `50:42` result, explaining why the current profile cannot populate the COM image. |
| M0 T153 | Closed: source-derived fast-I/O contract selects contained `50:42` read only and retains `50:43` unavailable. |
| M0 T154 | Closed: adapter-only `50:42` fast-read provider is source/test/build and runtime-plane closed; `50:43` remains original-unavailable. |
| M0 T155 | Closed: a source-built current composition reaches full COMMAND fast-read and EOF; a later stack-prefetch triple fault is separately admitted to T156. |
| M0 T156 | Closed: recursive stack #SS is correctly enforced by Bochs; the unobserved first vector/control predecessor is transferred to T157 diagnostic design. |
| M0 T157 | Closed: existing BX-TRACE-049 provides the required generic real-mode vector record; T158 may enable it without a new source intrusion. |
| M0 T158 | Closed: generic vector trace proves the known direct-pre-POST zero-INT10 path; T159 audits the existing POST/deferred alternative. |
| M0 T159 | Closed: current adapter retains a no-source-change native POST/deferred closure with exactly two Bochs object replacements; T160 may build and observe that form once. |
| M0 T160 | Closed: fresh x86 two-object composition reaches full COMMAND fast-read and normal `50:36` resume through native POST; T161 may observe the unchanged binary once at a longer budget. |
| M0 T161 | Closed: unchanged composition reaches accepted `FE` controlled stop after `50:36`; T162 must source-classify its CLI completion meaning. |
| M0 T198 S126 | Complete: the explicit CLI launch plan, `54:05` registration, CMDINFO gather, checked response writes and one-slot `54:01` commit are source/test/build closed in the adapter-only composition. |
| M0 T198 S127 | Complete: the sole locked native observation resumes past assembled `54:01` and reaches `54:0E`; it does not claim target completion. |
| M0 T198 S128 | Complete: `54:0E` is mapped to original `cmdGetKbdLayout`; the existing finite `DX=0` helper is admissible only as a globally classified CLI rehost. |
| M0 T198 S129 | Complete: the finite `54:0E` provider is composed after global classification; the focused adapter-only closure verifies `DX=0`, `+4` resume and protected-mode decline. |
| M0 T198 S130 | Complete: the sole locked native observation resumes past composed `54:0E` and reaches `54:04`; it does not claim target or console completion. |
| M0 T198 S131 | Complete: `54:04` is mapped to original `cmdGetCurrentDir`; its immutable-root candidate is admissible only as a globally classified CLI rehost. |
| M0 T198 S132 | Complete: the finite `54:04` provider is composed after global classification; its focused closure verifies root write, typed no-write failure and protected-mode decline. |
| M0 T198 S133 | Withdrawn before acceptance by owner policy correction; no native result is retained. |
| M0 T198 S134 | One native observation stopped at `54:01`, not `54:04`; it is non-closure evidence only. |
| M0 T198 | Closed by owner-directed reprioritization; it does not claim complete BOP, COMMAND or runtime closure. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |

**S25 P9:** all 25 `dpmi32/*` paths now have final dispositions: the 16
original x86 DPMI host-provider bodies are `adapter-backed`, five private
declaration forms are `binding-only`, and two RISC-only bodies plus two build
descriptions are `not-host-runtime`. The original 32-bit pointer-shaped
contracts bind only to the session `guest_memory` mapping-manager instance and
the `adapter-mvdm-host-out/softpc -> adapter-bochs` mechanical path; see the [DPMI32 final
baseline](etc/operations/m0-t276-s25-dpmi32-final-disposition-baseline-001.md).

**S25 P10:** all 40 `dos/*` paths now have final dispositions: all 27 original
DEM/COMMAND provider bodies are `adapter-backed`, seven declaration carriers
are `binding-only`, and package build/test metadata is `not-host-runtime`.
Recovery remains whole original DEM and COMMAND provider packages—not
trace-selected services—through the named BOP, SoftPC, Win32, session and
Redirector boundaries; see the [DOS final baseline](etc/operations/m0-t276-s25-dos-final-disposition-baseline-001.md).

**S25 P11:** all 21 `fax/*` paths now have final dispositions: the four
original user-mode WOWFAXUI providers are `adapter-backed`, four declarations
are `binding-only`, and the private CSRSS/GDI driver plus all product/build
resources are `not-host-runtime`. The UI package keeps its original flow while
`adapter-mvdm-host-out/wow`, `adapter-mvdm-host-out/win32` and session copied broker records replace only
the private shared-section transport; see the [FAX final baseline](etc/operations/m0-t276-s25-fax-final-disposition-baseline-001.md).

**S25 P12:** all 47 `bde`, `dbg`, `vdmdbg` and `vdmexts` paths are `tool-only`:
they are independent original NTSD/WinDbg extensions, debug-support library or
debugging-support DLL, never a main host-runtime library. Their remote-process,
symbol and fixed-image-address logic remains byte-exact evidence for a future,
separately admitted debugger-tool package; see the [debugger-tools final baseline](etc/operations/m0-t276-s25-debugger-tools-final-disposition-baseline-001.md).

**S25 P13:** all 339 selected `softpc.new` declaration carriers are
`binding-only`. They retain the original interface shapes for source-parity
MVDM bodies and same-shaped adapters but never authorize a second SoftPC
executor or a direct Bochs-object dependency; see the [SoftPC declaration final baseline](etc/operations/m0-t276-s25-softpc-declarations-final-disposition-baseline-001.md).

**S25 P14:** the 25 remaining `softpc.new` build/resource rows now have final
dispositions: debugger/disassembler inputs are `tool-only`, while standalone
SoftPC/NTVDM launch, link and resource inputs are `not-host-runtime` because
`app` owns final composition. No source body is enabled; see the [SoftPC resource final baseline](etc/operations/m0-t276-s25-softpc-resources-final-disposition-baseline-001.md).

**S25 P15:** the twelve original `softpc.new` debugger/disassembler source
bodies are `tool-only`; the two historical `obj.vdm` standalone product-entry
sources are `not-host-runtime`, with `app` owning final composition. Remaining
machine/control sources stay under their source-span owner evidence rather than
being inferred from a directory name; see the [SoftPC tool/product-source final baseline](etc/operations/m0-t276-s25-softpc-tool-source-final-disposition-baseline-001.md).

**S25 P16:** `softpc.new/base/support/get_env.c` is an `adapter-backed`
original environment helper. The other five support bodies are
`not-host-runtime`: they own an alternate IO table, CPU/register state, BIOS
timer or standalone SoftPC lifecycle and would create a second executor beside
Bochs; see the [SoftPC support-source final baseline](etc/operations/m0-t276-s25-softpc-support-source-final-disposition-baseline-001.md).

**S25 P17:** the 18 original SoftPC device bridge bodies now have final owner
dispositions: ten native video/FDC/DMA/timer/PIC/PPI-sound executors are
`not-host-runtime` because Bochs owns those devices; eight host
endpoint/control providers are `adapter-backed` through public host capability
and typed `adapter-bochs` operations. See the [SoftPC device-source final baseline](etc/operations/m0-t276-s25-softpc-device-source-final-disposition-baseline-001.md).

**S25 P18:** the remaining 38 `softpc.new/host/src` control bodies have final
dispositions: 25 original monitor/BOP/session/host-capability providers are
`adapter-backed`; six mixed CPU/RAM/page/section control bodies are
`overlay-required`; and seven alternate-executor/profile/product-shell bodies
are `not-host-runtime`. No source is enabled by the audit; see the [SoftPC control-source final baseline](etc/operations/m0-t276-s25-softpc-control-source-final-disposition-baseline-001.md).

**S25 P19:** the initial WOW32 carrier split is final: all 105 declaration
carriers are `binding-only`, while eight condition-unselected source files and
eleven build/resource inputs are `not-host-runtime`. The remaining 77 active
original provider bodies stay grouped by their eight original subdomains for
the next package decision; see the [WOW32 carrier final baseline](etc/operations/m0-t276-s25-wow32-carrier-final-disposition-baseline-001.md).

**S25 P20:** all 77 active WOW32 provider bodies now have final dispositions:
61 private frame/message/GDI/kernel-session/USER bodies are
`overlay-required`, while 16 shell/OLE/printing, multimedia and network
providers are `adapter-backed`. The overlays retain original provider flow and
replace only private NT4 transport or unsafe identity conversion; see the [WOW32 provider final baseline](etc/operations/m0-t276-s25-wow32-provider-final-disposition-baseline-001.md).
