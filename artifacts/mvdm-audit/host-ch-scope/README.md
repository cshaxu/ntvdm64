# Provisional host C/H scope snapshot

`inventory.csv` enumerates the 1,815 unique `.c`/`.h` paths in the four recorded shard CSVs. `observe.py` computes it from those CSVs, with each source file hashed in `evidence.json`; it reads and hashes the same input bytes, rejects duplicate paths, and does not change the shared ledgers. It reads each current shard decision field rather than stale imported reporting_class. Run from the repository root with `python artifacts/mvdm-audit/host-ch-scope/observe.py`. Two consecutive runs against the same input bytes produced byte-identical outputs. Running it against later shard states intentionally creates a **new** snapshot; compare the input hashes before interpreting counts or overwriting a frozen result. This is not a replacement for the authoritative per-file ledgers.

The 1,270 `host-candidate` rows exclude the path buckets below for triage only. Their recorded ledger statuses group as 234 pending, 759 partial, 224 excluded, and 53 full. These numbers describe recorded statuses, not demonstrated runtime coverage or a final scope decision. In particular host-candidate includes unselected historical architectures, fax/VDD code and standalone module boundaries that need source and build ownership review. Mirror diffs require partial classification under the project rule.

The 473 `role-pending` rows (`wow16/`, `dos/v86/`) are **not excluded** from the task: some guest trees can contain host-consumed headers, shared ABI declarations, build inputs, or dual-use source. Each such case needs per-file consumer and build-dependency evidence before classification. In particular a path name or historical guest role alone cannot rule out a host dependency.

The 72 `tool-or-scaffold-candidate` rows (`vdmutils/`, `vdmexts/`, `bde/`, `ieuvddex/`, `v86/scaffold/`) are provisionally separated as utility, debugger-extension or scaffold paths, not asserted unnecessary: verify per-file linkage and any shared runtime dependencies. `v86/monitor/` remains a host candidate because the original monitor describes itself as user-mode host code; `dbg/` and `vdmdbg/` also remain candidates as possible host DLL capability.

Output counts and classifications are evidence-bound snapshots. They are neither final scope approval nor evidence that every listed capability executes or passes tests.

The initial snapshot at 0ccfd0acf used historical reporting_class for shards 1–3, so its status counts were incorrect. The corrected observer reads audit_status, decision and review_class respectively; candidate scope counts did not change.

The [GFI provider review](gfi-alternative-provider-review.md) distinguishes compiled empty objects from usable device behavior. The [DEM lock object fixture](../host-dem/demlock-selected-object-review.md) exercises real host locks with controlled guest-register and error seams; guest and lifecycle boundaries remain open.

The [guest-tree consumer snapshot](guest-tree-host-consumer-review.md) compares ambiguous C/H paths to four recorded build/dependency sets; zero exact hits do not establish all configurations or justify bulk exclusion.

The [GFI header review](gfi-header-provider-review.md) traces selected and alternative declarations. The [DEM directory fixture](../host-dem/demdir-selected-object-review.md) tests selected object success and controlled errors without claiming guest integration.

Replaying check-mirror-diffs.py against the recorded inventory and retention ledger found 202 host-candidate C/H mirror-diff rows and zero with a non-partial status. This only checks recorded classifications, not behavioral recovery or completeness of diff detection.

The independent `check-frozen-mirror-coverage.py` compares current bytes of all host candidates against the frozen original hashes (primary edition when present). At this checkpoint, 1,263 present files were compared, with no differences absent from the retention table; seven historical CPU30 monitor paths are absent from the current tree and require their own role dispositions. All 202 recorded differences have a textual estimate and their stored current hash matches the current file. This is not an independent validation of frozen original hashes, supplemental editions, restored behavior or code semantics.

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

The [remaining WOW32 owner handoff](wow-remaining-owner-handoff.md) maps the 24 pending source roles to bounded S14–S17 recovery and verification groups. The [legacy dispatch handoff](../host-wow/legacy-dispatch-functional-handoff.md) separates original thunk families from obsolete declarations and unimplemented table entries.

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

The [guest-target follow-up](guest-target-followup-review.md) adds 45 observed-profile guest C source roles, while retaining 330 ambiguous paths and all host-side application behavior. The [original WINMM contract](wow-winmm-original-contract-review.md) supplies the historical provider contract for the controlled resolver witness, without proving modern provider behavior or guest reachability.

The [DOS absolute I/O lease-error witness](dos-absolute-io-lease-error-review.md) exercises four selected-object null-session paths with controlled seams; the DOS guest error and reachability remain unverified.

The [third guest-target snapshot](guest-target-round3-review.md) raises observed-profile guest C source candidates to 182, with 291 still requiring role review; application host behavior is not excluded. The [local WINMM comparison](wow-winmm-real-null-review.md) exercises the selected cached wrapper and this host provider under controlled invalid inputs.

The [guest header candidate review](guest-header-round4-review.md) adds 66 observed-profile header roles with original guest-source references and no captured host consumer; 225 ambiguous C/H roles remain. This is not proof of all build configurations or application behavior.

The [SoftWindows display-owner review](softwindows-display-owner-review.md) keeps two historical host interface headers pending while distinguishing the selected DOS video and WOW GDI paths. The [FDC single-owner assessment](../shard-3/fdc-session-single-owner-design.md) records a verified thread-local registration gap and a candidate design; its required guest-memory synchronization and teardown are not yet proven.

The [legacy GFI profile review](../shard-3/gfi-legacy-provider-review.md) distinguishes remote and diagnostic providers from the selected local floppy path. The [old WOW multimedia review](wow-old-multimedia-files-review.md) separates a disabled converter from an object containing alias functions with no demonstrated caller in the observed release. Profile-specific nonselection does not close the corresponding capabilities.

The [disk and ROM declarations](../shard-3/disk-rom-declaration-review.md) and [old WOW table declarations](wow-old-four-table-roles-review.md) distinguish physical header roles from the selected service owners; their observations do not establish complete device or API behavior. The historical standalone `sim32/sim32.h` is excluded as a tool header, not as an exclusion of the current SIM32 runtime provider.

The [ANSI header review](wow-wuansi-role-review.md) excludes only a comment-only historical header; six selected thunk owners and their guest/DBCS tests remain in scope. The [floppy provider review](../shard-3/vflop-real-header-selection-review.md) keeps original VFLOPPY pending and separates the selected physical-floppy path and its unverified media and teardown obligations.

The [DDEML old host-file review](wow-ddeml-three-file-roles-review.md) distinguishes three unselected historical thunk files from the separate guest DDEML route and leaves transaction and callback testing in S14. The [debug-profile selection review](../shard-3/legacy-debug-profile-selection-review.md) records the observed inactive YODA, profiler and EGA-dump bodies; it does not establish their wider applicability or restored behavior.
