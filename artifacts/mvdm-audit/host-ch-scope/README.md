# Provisional host C/H scope snapshot

`inventory.csv` enumerates the 1,815 unique `.c`/`.h` paths in the four recorded shard CSVs. `observe.py` computes it from those CSVs, with each source file hashed in `evidence.json`; it reads and hashes the same input bytes, rejects duplicate paths, and does not change the shared ledgers. It reads each current shard decision field rather than stale imported reporting_class. Run from the repository root with `python artifacts/mvdm-audit/host-ch-scope/observe.py`. Two consecutive runs against the same input bytes produced byte-identical outputs. Running it against later shard states intentionally creates a **new** snapshot; compare the input hashes before interpreting counts or overwriting a frozen result. This is not a replacement for the authoritative per-file ledgers.

The 1,270 `host-candidate` rows exclude the path buckets below for triage only. Their recorded ledger statuses group as 187 pending, 768 partial, 262 excluded, and 53 full. These numbers describe recorded statuses, not demonstrated runtime coverage or a final scope decision. In particular host-candidate includes unselected historical architectures, fax/VDD code and standalone module boundaries that need source and build ownership review. Mirror diffs require partial classification under the project rule.

The 473 `role-pending` rows (`wow16/`, `dos/v86/`) are **not excluded** from the task: some guest trees can contain host-consumed headers, shared ABI declarations, build inputs, or dual-use source. Each such case needs per-file consumer and build-dependency evidence before classification. In particular a path name or historical guest role alone cannot rule out a host dependency.

The 72 `tool-or-scaffold-candidate` rows (`vdmutils/`, `vdmexts/`, `bde/`, `ieuvddex/`, `v86/scaffold/`) are provisionally separated as utility, debugger-extension or scaffold paths, not asserted unnecessary: verify per-file linkage and any shared runtime dependencies. `v86/monitor/` remains a host candidate because the original monitor describes itself as user-mode host code; `dbg/` and `vdmdbg/` also remain candidates as possible host DLL capability.

Output counts and classifications are evidence-bound snapshots. They are neither final scope approval nor evidence that every listed capability executes or passes tests.

The [FCB open ownership review](dem-fcb-open-ownership-review.md) distinguishes helper and caller handle ownership and records failure-path evidence for S12. The [WOW local parameter retirement review](wow-local-parameter-retirement-review.md) records original mapping ownership and a conditional same-key cross-task boundary; its reachability and contract remain unproved.

The [FCB metadata actual-object fixture](dem-fcb-metadata-failure-review.md) executes seven controlled API outcomes, including two ignored failures and a successful size sentinel rejected by the original helper. It does not exercise the complete caller or guest path.

The [FCB caller fixture](dem-fcb-callers-reachability-review.md) runs both selected original callers under controlled acquisition and failure boundaries and traces original guest service routes. Its fake handle and error translator do not establish an installed guest failure or real error mapping.

The [FCB real-file sentinel witness](dem-fcb-error-sentinel-review.md) reproduces the valid `FFFFFFFFh` low-size false rejection with a real sparse host file; it does not exercise the installed guest or original error converter.

The [FCB ignored-time-failure caller fixture](dem-fcb-ignored-errors-review.md) shows both original DEM callers taking their controlled success paths after either injected API failure. The [WOW internal table declaration recheck](wowint-original-consumer-recheck.md) classifies one unconsumed physical header while retaining any reachable service and guest ABI in S14.

The [floppy provider review](dos-floppy-pending-owner-review.md), [video diagnostic family review](../shard-3/pending-video-diagnostic-family-review.md), and [WOW multimedia pending review](wow-multimedia-three-pending-recheck.md) distinguish unselected original profiles from selected production owners. Their file roles and original capability obligations remain pending where the selected profile and consumer evidence is insufficient; controlled close-failure observations do not establish a live guest failure.

The [legacy CPU header selection](legacy-cpu-header-selection.md) excludes two older physical interface paths for the captured NT profile while retaining CPU behavior in S11. The [GFI](gfi-scoped-physical-disposition-review.md) and [WOW](wow-profile-excluded-three-candidates.md) scoped candidates remain pending until their original feature applicability is decided; an inactive body alone is insufficient to exclude an unverified capability.

The [redirector diagnostic review](vdmredir-debug-physical-owner-review.md) establishes the selected non-DBG `vrdebug.h` macro role, recorded partial; live diagnostic and redirector behavior remain unproved. The [SoftWindows device interface review](../shard-3/softwindows-device-family-ledger-review.md) and [cursor/icon owner reconciliation](wow-cursor-function-owner-reconciliation.md) retain original-profile applicability and guest behavior boundaries.

The [original multimedia direct converter review](wow-original-media-disabled-decisions.md) records two original literal-disabled physical bodies as excluded; the distinct MCI and wave/MIDI capabilities remain S16. The [PIG environment](dos-pig-env-physical-owner-review.md) and [SLAVEPC floppy](../shard-3/slavepc-floppy-nt-applicability-review.md) reviews retain original-profile applicability rather than inferring global exclusion from the release build.

The initial snapshot at 0ccfd0acf used historical reporting_class for shards 1–3, so its status counts were incorrect. The corrected observer reads audit_status, decision and review_class respectively; candidate scope counts did not change.

The [GFI provider review](gfi-alternative-provider-review.md) distinguishes compiled empty objects from usable device behavior. The [DEM lock object fixture](../host-dem/demlock-selected-object-review.md) exercises real host locks with controlled guest-register and error seams; guest and lifecycle boundaries remain open.

The [guest-tree consumer snapshot](guest-tree-host-consumer-review.md) compares ambiguous C/H paths to four recorded build/dependency sets; zero exact hits do not establish all configurations or justify bulk exclusion.

The [GFI header review](gfi-header-provider-review.md) traces selected and alternative declarations. The [DEM directory fixture](../host-dem/demdir-selected-object-review.md) tests selected object success and controlled errors without claiming guest integration.

Replaying check-mirror-diffs.py against the recorded inventory and retention ledger found 202 host-candidate C/H mirror-diff rows and zero with a non-partial status. This only checks recorded classifications, not behavioral recovery or completeness of diff detection.

The independent `check-frozen-mirror-coverage.py` checks that all 1,794 current MVDM C/H paths occur in the inventory, then compares current bytes of all host candidates against the frozen original hashes (primary edition when present). At this checkpoint, 1,263 present host candidates were compared, with no differences absent from the retention table; seven historical CPU30 monitor paths are absent and recorded as physically excluded. A missing host candidate without an explicit excluded disposition or a frozen original identity fails the check; in-memory negative and control cases verified these distinctions. All 202 recorded differences have a textual estimate and their stored current hash matches the current file. The check also rejects stale source/original hashes, absent estimates and duplicate table rows; a temporary isolated CSV fixture verified a valid control and five corresponding negative cases. Path coverage does not settle host/guest role; this is not an independent validation of frozen original hashes, supplemental editions, restored behavior or code semantics.

The [CD-ROM provider review](cdrom-provider-boundary-review.md) separates unselected original interfaces from independent MSCDEX/VDD behavior. The [DEM IOCTL object fixture](../host-dem/demioctl-selected-object-review.md) exercises seven controlled drive-type results without claiming real device or guest coverage.

The [input alternate headers](keyboard-mouse-alternate-header-review.md) and [display profiles](display-profile-interface-review.md) retain per-profile boundaries. DEM selected-object fixtures also cover [current-directory failure](../host-dem/demcwd-selected-object-review.md) and [cross-process lock retirement](../host-dem/demlock-process-selected-object-review.md); guest integration remains open.

The [tool and scaffold role review](tool-scaffold-review.md) checks 72 candidates against recorded graphs and original target metadata; standalone paths and missing historical NTVDM scaffold sources require separate ownership decisions.

The [C-VID CPU metadata review](cvidc-metadata-review.md) moves one source role from pending to partial based on the generated binding and a bounded selected-object fixture; slot and deployed-runtime coverage remain open.

The [COMMAND host review](../host-command/pending-command-owner-review.md) bounds selected exit and keyboard declarations; the [error/config review](error-config-interface-review.md) identifies an empty physical header without closing configuration behavior.

The [DEM source review](../host-dem/pending-dem-owner-review.md) records current statuses and leaves [debug-only message support](../host-dem/demmsg-profile-role-review.md) pending. The [close object probe](../host-dem/demclose-selected-object-review.md) checks handle and callback sequencing with controlled seams.

The [PIF and OEM review](../host-command/pif-and-oem-config-review.md) distinguishes a profile-specific duplicate header from selected OEM declarations and bounded historical conversion behavior.

The [build ID review](build-id-generated-allocation-review.md) includes a selected-object known/unknown module fixture. [XMS and redirector headers](../host-inc/xms-redir-export-role-review.md) have bounded selected-provider and historical guest evidence; [Hunter notice](hunter-rename-resource-review.md) is excluded only as a non-code physical file.

Selected error and debug declarations have bounded [error/config](error-config-interface-review.md), [error/profile](error-profile-hfx-review.md), and [debug/display](selected-debug-display-profile-review.md) evidence; compilation evidence is not behavioral acceptance. The [redirector initialization review](../host-inc/redir-init-shared-role-review.md) retains unsupported RAP transport and lifecycle boundaries.

The [multimedia and Winsock declaration review](../host-inc/multimedia-winsock-declaration-review.md) separates historical bridge witnesses from sound's static-only selected branch and unimplemented entries.

The [CCPU/C-VID vector interface review](ccpu-evidgen-review.md) records selected header consumers and incompatible field layouts; actual cross-object access and ABI safety are not yet proved.

The [host memory/fullscreen declarations](host-memory-fullscreen-interface-review.md) and [WOW task/kernel/Shell declarations](../host-inc/wow-task-kernel-shell-declaration-review.md) record selected roles and bounded historical witnesses without asserting full runtime coverage.

The [debug/fax declarations](wow-debug-fax-declarations-review.md), [fax/DDE review](../host-inc/wow-fax-dde-debug-declaration-review.md), and [trace profile review](debug-trace-source-profile-review.md) distinguish selected static paths from untested debugger, device and trace behavior.

The [CCPU vglob comparison](ccpu-vglob-review.md) records a byte-identical selected provider at a different path without claiming that the original CCPU profile ran. The [terminal/X review](terminal-x-host-provider-review.md) retains platform-specific provider boundaries.

The [WOW dialog, DIB and printer review](wow-dialog-dib-print-review.md) records original selected providers and unverified callback, selector, resolver and device boundaries.

The [WOW common declarations](wow-common-declarations-review.md) bind representative GDI, kernel and USER dispatch to selected original objects; the [CCPU production shim](ccpu-debug-pig-provider-review.md) records its original no-op behavior without claiming debugger support.

The [legacy CPU/redirector declarations](../host-inc/legacy-cpu-redir-declaration-review.md) and [unselected WOW declarations](../host-inc/wow-unselected-profile-followup.md) record observed profile limits without declaring their features globally excluded.

The [VGA accessor and alternate trace review](vga-accessor-env-trace-review.md) is a source-role assessment; the coordinator's partial decision for byte-identical vglob is recorded in the linked vglob review.

The [WOW kernel table review](wow-kernel-table-review.md) identifies the selected original table, including entries that are still local or unimplemented; table selection does not establish every API.

The [monitor register consumer check](../host-inc/monregs-selected-object-review.md) examines one actual object without ruling out other consumers; [C-VID/SAS alternate owners](cvid-sas-alternate-owner-review.md) are not assumed equivalent to selected same-name files.

The [empty WOW GDI header](wow-gdi-empty-header-review.md) is excluded solely as an empty physical file; GDI behavior remains independently in scope.

The [disk geometry and ROM map review](disk-geometry-rommap-provider-review.md) keeps distinct original and current host paths separate pending provider and device validation.

The [local heap and multimedia follow-up](../host-wow/local-multimedia-declaration-followup.md) distinguishes inactive historical declarations from active guest and host owners, with alternate-profile and lifecycle questions retained.

The [remaining WOW32 owner handoff](wow-remaining-owner-handoff.md) maps a historical snapshot of 24 source roles to bounded S14–S17 recovery and verification groups; current statuses are in the live inventory. The [legacy dispatch handoff](../host-wow/legacy-dispatch-functional-handoff.md) separates original thunk families from obsolete declarations and unimplemented table entries.

The private `wctlm32.h` declarations have since been classified with their unselected incomplete fragment; the selected original message owners and S14/S15 conversion, reentry and lifecycle obligations remain as documented in the [control-message review](excluded-control-message-owner-review.md).

The [remaining WOW32 owner handoff](wow-remaining-owner-handoff.md), [legacy dispatch owners](../host-wow/legacy-dispatch-functional-handoff.md), and [excluded C source spot-check](excluded-host-c-source-spot-review.md) map pending and sampled physical roles to bounded recovery; their evidence does not establish full runtime coverage.

The [memory/DPMI/CMOS exclusion review](excluded-memory-cmos-provider-review.md) checks five alternate physical source roles against their selected NT or CPU40 providers; remaining device and memory behavior stays in scope.

The [GFI functional handoff](../host-inc/gfi-functional-handoff.md) separates real, empty and SLAVEPC providers and records bounded close-failure observations. The [excluded UI/video source review](excluded-ui-video-scaffold-review.md) examines physical roles without closing their selected functionality.

The [selected debugger provider](debugger-provider-review.md) has a bounded historical real worker event witness. The [SAS/error/alternate CPU review](excluded-sas-error-altcpu-review.md) preserves functional obligations independently of source-role selection.

The [missing/empty excluded C review](excluded-missing-empty-c-source-review.md) verifies original identities and records the separate CPU40 obligations; it does not establish their runtime completion.

The [DDEML route review](ddeml-route-review.md) binds bounded historical guest initialization evidence, without claiming transactions or callbacks. The [keyboard/mouse handoff](../host-inc/keyboard-mouse-functional-handoff.md) retains controller, guest and video-owner boundaries.

The [OEM private header review](oemuni-private-header-review.md) binds selected providers and bounded historical conversion witnesses. The [PIG header correction](ccpupig-header-reopen-review.md) reopens an exclusion whose implementation owner remains unresolved.

The [EGA trace header correction](egatrace-header-reopen-review.md) reopens a file whose associated original trace profile remains unresolved; no EGA trace behavior is claimed as executed.

The [HUNTER provider correction](hunter-provider-reopen-review.md) reopens the original conditional provider while its diagnostic profile remains unresolved.

The [remaining excluded header review](excluded-host-header-final-group-review.md) reopens GISP diagnostic applicability while retaining the separate SoftWindows-only interface outside the NTVDM product profile. The [WINMM boundary](wow-winmm-boundary-review.md) and [legacy networking boundary](../host-inc/legacy-network-functional-boundary.md) distinguish source/image observations from runtime acceptance.

The [SoftWindows input/audio profile review](softwindows-input-audio-profile-review.md) separates its conditional guest-driver BOPs from selected NT input and WOW multimedia paths; required selected-path behavior still needs validation.

The [control message owner review](excluded-control-message-owner-review.md) traces selected original table and handlers without treating the static chain as a guest behavior pass.

The [planar presentation handoff](../host-inc/planar-presentation-functional-handoff.md) and [virtual/real floppy owner review](virtual-real-floppy-owner-review.md) separate selected original owners from project adapters and retained device/lifecycle validation.

The [guest-target candidate review](guest-target-candidate-review.md) identifies 98 C files with observed 16-bit target recipes and retains 375 roles for further host/shared review, without excluding corresponding applications. The [WINMM selected-object witness](wow-winmm-resolve-object-review.md) records controlled resolver failures and output overwrites; guest reachability and actual provider contract remain unproved.

The [MEM and SETVER target review](dos-v86-target-current-review.md) identifies another 13 guest-target C/H candidates in its bounded snapshot, with exact host dependency and source-consumer checks; its input must be reconciled to the current ledger before a scope decision. The [C-VID SAS family review](../shard-3/cvidc-generated-sas-family-recheck.md) distinguishes selected and historical vector layouts and preserves the unresolved provider and behavior obligations.

The [DDEML guest source recheck](wow16-ddeml-guest-role-review.md) inspects seven sources across 34 captured Ninja graphs; these are already included in the 13 DDEML candidates above and are not seven additional files. Guest transactions, callbacks and shared ABI remain separate obligations.

The [suballocator private-header review](dos-suballocator-private-header-review.md) establishes actual selected bitmap, granularity and address-macro uses, recorded partial. The [input profile handoff](../shard-3/pending-input-profile-ledger-handoff.md) retains separate NT and historical keyboard/mouse capability obligations.

The [selected suballocator object fixture](suballoc-selected-object-review.md) runs normal controls and injected callback failures against the current cached worker object; the injected failure observations are not real guest or installed-image acceptance.

The [DPMI XMEM selected-object fixture](dpmi-xmem-selected-review.md) checks controlled allocation, reallocation and free-failure branches with fake register/provider boundaries; actual XMS pool, guest movement and task lifecycle remain S11/S12 work.

The [DPMI XMEM owner cleanup fixture](dpmi-xmem-owner-review.md) observes that one controlled provider failure stops cleanup after earlier blocks were freed; an explicit fixture retry clears the remaining owner block while a second owner stays separate. Real task teardown, retry and native memory are not covered.

The [DEM default-drive selected-object fixture](dem-default-drive-selected-review.md) verifies original direct-service argument and failure branches with controlled callbacks. The traced active DOS default-drive path does not establish guest reachability of that service.

The [DEM clock selected-object fixture](dem-clock-review.md) checks date/time query, preservation and controlled set-error branches without modifying the host clock. Guest dispatch, real privilege behavior and date boundaries remain unverified.

The [DEM computer-name selected-object fixture](dem-computername-review.md) checks mocked host-name results, guest register mapping, padding and the 15-byte boundary; actual guest mapping validity and lifecycle remain unverified.

The [DEM boot-drive selected-object fixture](dem-bootdrive-review.md) checks controlled registry and drive outcomes without modifying the registry or inspecting real disks. Malformed values, actual guest startup and current released-image binding remain unverified.

The [DEM DOS/WOW FILES selected-object fixture](dem-wowfiles-review.md) checks the original service's DOS-preserve and WOW-255 branches with synthetic mode state; actual guest configuration and handle limits remain unverified.

The [X keyboard table review](../shard-3/m-keybdp-original-nt-profile-review.md) bounds one physical file exclusion to the NT worker. The [legacy multimedia alias witness](old-media-alias-object-review.md) exercises the original cached object with synthetic single-threaded handles; no selected product consumer or guest behavior is thereby established. The [A3 CPU register provider review](dos-a3cpu-register-owner-review.md) retains alternate-profile scope separately from the selected CCPU register owner.

The [historical keyboard declarations review](../shard-3/legacy-keyboard-declaration-target-review.md) bounds two more physical declarations to alternate profiles. The selected NT input/IRQ behavior remains S13, and the SoftWindows provider is still unresolved.

The [historical display declaration review](../shard-3/legacy-display-rom-icon-stats-review.md) distinguishes a selected header from its inactive GISP branch and retains old icon/provider applicability separately; current Video7 tests do not prove those paths.

The [WOWFAX property-sheet owner review](wowfax-property-sheets-owner-review.md) separates a historical printer UI from ordinary dialogs and records unconfirmed allocation and cleanup risks for S16.

The [C-VID alternative-host constants](cvidc-alternate-host-register-constants.md) classify four physical non-x86 host-register headers; guest CPU/FPU and generated-vector behavior remain separate obligations.

The same C-VID review also classifies `bodge_c.h`, a non-x86 host branch-size constant carrier. The [historical licensing declaration review](legacy-licensing-header-review.md) bounds three physical header decisions to the captured NT worker; a separate original licensing profile and external providers remain S17 applicability questions.

The [CPU/SAS generated-header recheck](cpu-sas-generated-pending-role-review.md) identifies exact selected object dependencies and the selected x86 SAS layout, while retaining four unresolved header roles and vector/fallback semantics.

The [register/schema header recheck](ccpu-register-generated-roles-recheck.md) records specific GDP offset differences and the observed quoted include; the actual expanded consumer and ABI remain unproved.

The [disassembler and historical UI review](disassembler-legacy-ui-pending-review.md) separates three pending headers and retains their original profile and feature-ownership questions in S17.

The [disassembler header follow-up](disassembler-header-profile-followup.md) bounds three original declarations to the observed production profile; their original disassembler/debugger capability remains pending and is not claimed restored.

The [original NT disassembler target recheck](../shard-3/disassembler-nt-target-recheck.md) confirms the original target was selected for non-x86 NT, not a non-NT-only component. The [XMS header correction](xms-header-host-role-correction.md) records the guest-facing XMS header as an actual selected host dependency; its XMS behavior remains partial.

The historical CPU30 `inc/v86def.h` carrier is excluded only from the selected CPU40/CCPU profile as explained in the [CPU declaration review](../host-inc/legacy-cpu-redir-declaration-review.md). The [remaining DOS host check](dos-host-remaining-partial-check.md) retains two selected header dependencies as pending where active behavior has not been established.

The [HFX protocol review](dos-hfx-protocol-owner-review.md) separates original HFX and SWIN_HFX BOPs from ordinary DOS file and redirector services. Original guest/provider applicability remains pending; captured illegal-BOP bindings are not a working HFX witness.

The [WOW internal table declaration review](witbl-original-consumer-recheck.md) and [GFI header review](gfi-header-selected-physical-review.md) distinguish unused physical declarations from selected original owners. WOW dispatch and real floppy behavior remain independent S14 and S13 obligations.

The [global-memory](../cross-369/kernel-memory-header-disposition.md) and [local-memory](../cross-369/local-memory-header-disposition.md) reviews classify two old host-thunk declaration carriers; the local header is included by precomp, but its old thunk provider is unselected. Guest heap and callback behavior remain S14/S15 obligations.

The [spool declaration review](../cross-369/spool-header-disposition.md) classifies the unconsumed historical `winspldl.h` carrier. Selected shared spooler and OLE resolver behavior, including the [failure boundaries](../shard-3/wow-spool-loader-callers-review.md), remains S16 work.

The [guest-target follow-up](guest-target-followup-review.md) adds 45 observed-profile guest C source roles, while retaining 330 ambiguous paths and all host-side application behavior. The [original WINMM contract](wow-winmm-original-contract-review.md) supplies the historical provider contract for the controlled resolver witness, without proving modern provider behavior or guest reachability.

The [DOS absolute I/O lease-error witness](dos-absolute-io-lease-error-review.md) exercises four selected-object null-session paths with controlled seams; the DOS guest error and reachability remain unverified.

The [third guest-target snapshot](guest-target-round3-review.md) raises observed-profile guest C source candidates to 182, with 291 still requiring role review; application host behavior is not excluded. The [local WINMM comparison](wow-winmm-real-null-review.md) exercises the selected cached wrapper and this host provider under controlled invalid inputs.

The [guest header candidate review](guest-header-round4-review.md) adds 66 observed-profile header roles with original guest-source references and no captured host consumer; 225 ambiguous C/H roles remain. This is not proof of all build configurations or application behavior.

The [SoftWindows display-owner review](softwindows-display-owner-review.md) keeps two historical host interface headers pending while distinguishing the selected DOS video and WOW GDI paths. The [FDC single-owner assessment](../shard-3/fdc-session-single-owner-design.md) records a verified thread-local registration gap and a candidate design; its required guest-memory synchronization and teardown are not yet proven.

The [legacy GFI profile review](../shard-3/gfi-legacy-provider-review.md) distinguishes remote and diagnostic providers from the selected local floppy path. The [old WOW multimedia review](wow-old-multimedia-files-review.md) separates a disabled converter from an object containing alias functions with no demonstrated caller in the observed release. Profile-specific nonselection does not close the corresponding capabilities.

The [disk and ROM declarations](../shard-3/disk-rom-declaration-review.md) and [old WOW table declarations](wow-old-four-table-roles-review.md) distinguish physical header roles from the selected service owners; their observations do not establish complete device or API behavior. The historical standalone `sim32/sim32.h` is excluded as a tool header, not as an exclusion of the current SIM32 runtime provider.

The [ANSI header review](wow-wuansi-role-review.md) excludes only a comment-only historical header; six selected thunk owners and their guest/DBCS tests remain in scope. The [floppy provider review](../shard-3/vflop-real-header-selection-review.md) keeps original VFLOPPY pending and separates the selected physical-floppy path and its unverified media and teardown obligations.

The [DDEML old host-file review](wow-ddeml-three-file-roles-review.md) distinguishes three unselected historical thunk files from the separate guest DDEML route and leaves transaction and callback testing in S14. The [debug-profile selection review](../shard-3/legacy-debug-profile-selection-review.md) records the observed inactive YODA, profiler and EGA-dump bodies; it does not establish their wider applicability or restored behavior.

The [ROM, monochrome and Hercules declarations](../shard-3/rom-mono-hercules-selection-review.md) preserve separate device applicability and port requirements. The [internal-table and spool declaration review](wow-witbl-winspldl-roles-review.md) records the selected spool provider without treating a static call chain as proof of printing.

The [comment-only host system header](host-system-empty-header-review.md) is excluded as a physical file only; Hercules remains an independent open capability. The [disk/input header reconciliation](../shard-3/disk-input-pending-header-reconciliation.md) and [WOW cursor/global declaration review](wow-wcurcash-wkglobal-roles-review.md) retain distinct original and selected-provider obligations.

The [four C-VID header review](../shard-3/cvidc-four-header-provider-review.md) separates historical generated-header paths from selected CPU/video implementations. No exact selected dependency was established for those four paths; their instruction and video behaviors remain separately scoped.

The [three WOW declaration review](wow-three-table-structure-recheck.md) identifies one included multimedia table declaration, an included legacy conversion header with bounded disabled consumers, and an unselected duplicate table declaration; actual multimedia and dispatch behavior remain separate obligations.

The [device header review](../shard-3/pending-device-headers-partial-check.md) distinguishes exact selected header paths and gated declarations from independently selected device implementations. A separate [MEM/SETVER source-role observation](dos-target-round5-evidence.json) identifies 12 historical guest-target C/H candidates with no observed host dependency in the recorded graphs; this is supporting scope evidence, not a guest source audit or proof about other build configurations.

The [CCPU/C-VID pending-header dependency snapshot](ccpu-cvidc-pending-header-review.md) records ten exact CCPU dependency hits among 55 pending headers. It also corrects the interpretation of an older bounded inclusion snapshot; inclusion does not prove guarded 486 or PIG code executes.

The [scratch disposition](superseded-untracked-scratch-disposition.md) records ten reviewed, unreferenced historical scratch files removed after their evidence was superseded or could not be replayed. It does not change host classifications or close functionality.

Grouped snapshots index remaining [INC/DOS/DPMI roles](inc-dos-dpmi-pending-owner-review.md), [WOW/fax/redirector/debug roles](wow-fax-redir-debug-pending-owner-table.md), and [host device headers](../shard-3/pending-host-header-crosswalk.md). The latter suggests stage ownership by filename only; actual paths and call chains must be checked before applying it.

The [diagnostic/tool header review](../shard-3/pending-tool-header-disposition-review.md) distinguishes the excluded historical harness input from live keyboard, graphics and debugging capabilities, which remain separately scoped.

The [CCPU physical-role review](ccpu-pending-physical-role-review.md), [WOW duplicate candidates](wow-excluded-file-candidates.md), and [legacy host declarations](../shard-3/legacy-host-declaration-reconciliation.md) group current-profile source decisions without excluding the associated CPU, multimedia, XMS or display capabilities. Only `wowtable.h` and `host/inc/xmsexp.h` were subsequently excluded from these new candidates; the other recommendations remain snapshots.

The [WOW owner recheck](wow-four-excluded-owner-recheck.md) establishes matching classic DDE structures for the subsequently excluded `wowdde.h` physical role and keeps classic DDE, DDEML, printing and guest heap behavior separate. The [CCPU/C-VID exact-duplicate observation](ccpu-cvidc-selected-duplicate-evidence.json) found no different selected header with matching bytes for its 55-file historical snapshot; it does not rule out alternate profiles or equivalent behavior.

The [five WOW header recheck](wow-five-pending-conservative-recheck.md) retains pending dispositions where a distinct guest owner, different dispatch mechanism or inactive legacy macros do not establish an exact duplicate or behavior of that physical header.

The [four legacy host header recheck](../shard-3/four-legacy-host-headers-recheck.md) documents selected and unselected owner boundaries; its recommendations have not been applied to the ledger.

The [GLOBALFREEALL owner trace](globalfreeall-guest-owner-review.md) corrects a host-table inference: original guest kernel paths directly implement and call it. The [memory-bit-index path review](ccpu-bit-index-shared-path-review.md) bounds the previously reproduced BTC defect and keeps the rest of the instruction matrix unproven.

The [15-header review](../shard-3/pending-header-15-disposition-review.md) and [VDMREDIR review](vdmredir-five-role-recheck.md) distinguish exact historical declarations from selected device and DLC owners. Subsequent ledger updates classify only the explicitly identified physical paths; real device and DLC behavior remains independently scoped.
