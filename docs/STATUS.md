# Project Status

## Current Work

> **Current effective packet: M0 T225 S8.** Its governing brief appears immediately below.

**Active: M0 T225 S8 -- primary source-built four-image cross-validation.**

## Active Packet

> **T scope:** T225 is the Queue-selected Guest DOS multi-program execution
> closure: a declared sequence of guest DOS programs must use guest DOS
> `EXEC`/PSP/return semantics inside one VDM, never nested host NTVDM processes.

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T225 S8, Ordinary Mode. |
| Admission And Approval | S6 has the complete Direct/Readonly installation fixture and a deliberately separate approved reference sequence. S7 has replaced the custom object cache with the formal Ninja graph. |
| Objective | Restore and validate the physically distinct primary original-toolchain NTIO/NTDOS/COMMAND/SHARE input sequence, then run the existing installation fixture independently in Direct and Readonly and compare it with the secondary reference sequence. |
| Non-goals | No guest trace, BOP implementation, substitute artifact, reference-to-primary reclassification, Bochs feature, DOS loader, or host-system mutation. |
| Reference Baseline | DOS/WOW16 source-closure inventory; T225 S6 input admission and fixture record; T225 S7 Ninja graph; four locked source-built identity records. |
| Files And ABI Surface | A provenance-checking primary-input stager, source-artifact records, and existing fixture/profile ABI only. No runtime ABI or guest/Bochs semantic surface. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, source-policy.md and design authorities. |
| Verification | Each source-built stage manifest and output identity must be present and hash-match; fresh Ninja-linked Direct/Readonly installation runs must pass; Overlay and retired Virtual must reject; the result must compare primary and reference provenance separately. |
| Expected Markers | Four distinct original-toolchain stage roots, explicit manifest paths, exact bytes/SHA-256, no `refs/opennt` fallback, two accepted primary installation modes, and two rejected unavailable modes. |
| Asset Needs | The original-toolchain outputs under `artifacts/toolchain-runs/ntio-tools16-opennt-v1`, `ntdos-tools16-historical-v1`, `command-tools16-opennt-v1`, and `share-tools16-opennt-v1`, each with its declared build manifest. |
| Reporting Requirements | Record stage presence/absence, manifest and artifact identities, Direct/Readonly results, mode rejections, and a provenance-preserving comparison to the reference sequence. |
| Stop Conditions | Any missing primary manifest/artifact, identity mismatch, source-tree/reference fallback, unreviewed binary import, native guest execution, or change to BOP/Bochs/guest semantics. |
| Exit Criteria | A reproducible primary-sequence staging and Direct/Readonly installation comparison, or a precise artifact-restoration record that refuses execution until all four primary source-built roots are available. |
| Original Owner Request | "原始文件当然更好更权威了；属于两个不同序列的guest镜像，可交叉验证！" |
| Similar-Issue Sweep | NTIO, NTDOS, COMMAND and SHARE source-build provenance, staging manifests, fixture profile identity, Direct/Readonly policy, and reference/primary non-conflation. |
> **T225 S8 P1 availability:** the strict [availability record](etc/evidence/t225-s8-primary-source-built-input-availability-001.md) confirms the primary rtifacts/toolchain-runs manifests are absent; its new stager refuses before any copy or fallback. Restore the four manifest-bearing stage roots before the Direct/Readonly cross-validation run.
> **T225 S8 P2 NTIO bootstrap:** the [NOSRVBLD audit](etc/evidence/t225-s8-ntio-nosrvbld-bootstrap-audit-001.md) proves the tracked DOS tool emits all four classes but not the required OpenNT-compatible bytes. It is rejected as a primary NTIO build input; no assembly, link, fixture, or fallback is admitted.
> **T225 S8 P3 archived-generator:** the [extractor rejection](etc/evidence/t225-s8-archived-nosrvbld-extractor-rejection-001.md) eliminates the only retained source-derived NOSRVBLD candidate: all four class outputs differ. It remains evidence-only; no fallback or primary stage is admitted.
> **T225 S8 P4 OpenNT NOSRVBLD:** the [successor resolution](etc/evidence/t225-s8-opennt-nosrvbld-bootstrap-resolution-001.md) proves the distinct pinned OpenNT 4.5 tools16 `NOSRVBLD.EXE` regenerates all four MSBIO classes byte-exactly. The MS-DOS 4 tool and archived helper remain rejected; NTIO may now proceed only through the original OpenNT tools16 closure.
> **T225 S8 P5 DOSBox completion:** the [runner record](etc/evidence/t225-s8-dosbox-completion-runner-001.md) admits only a bounded stage-batch completion wait. Its first source-tool verification regenerated all MSBIO classes byte-exactly; it supplies no guest/build semantics.
> **T225 S8 P6 primary NTIO:** the [source-build result](etc/evidence/t225-s8-primary-ntio-source-build-result-001.md) proves a fresh manifest-bearing original-tools16 NTIO stage at the locked 33,792-byte identity. It is provenance only; NTDOS, COMMAND and SHARE still gate fixture admission.
> **T225 S8 P7 primary SHARE:** the [source-build result](etc/evidence/t225-s8-primary-share-source-build-result-001.md) proves a fresh manifest-bearing original-tools16 SHARE stage at its locked identity. COMMAND and NTDOS remain the only missing primary S8 inputs.
> **T225 S8 P8 primary COMMAND:** the [source-build result](etc/evidence/t225-s8-primary-command-source-build-result-001.md) proves a fresh manifest-bearing original-tools16 COMMAND stage at its locked 50,384-byte identity. NTDOS is now the only missing primary S8 input.
> **T225 S4 P0 admission:** S3 is closed in [its file-I/O source/ABI record](etc/research/t225-s3-exec-file-io-source-abi-fastread-map-001.md). S4 begins with the complete COM branch source and prerequisite map; it must not implement a substitute loader.

> **T225 S4 P1 progress:** the [COM control-flow/prerequisite map](etc/research/t225-s4-com-exec-control-flow-prerequisite-map-001.md) confirms `$Exec` remains the only COM loader. Artifact and S3 file evidence exist; real guest arena/PSP/entry reach is not yet proven and requires a bounded guest fixture.

> **T225 S4 P2 admission result:** current native startup stages only NTIO; NTDOS is identity-only and COMMAND remains namespace input. The [map](etc/research/t225-s4-com-exec-control-flow-prerequisite-map-001.md) records machine/startup continuity as the first missing owner package. S4 does not authorize a synthetic loader or native COM trace.

> **T225 S4 P3 handoff:** the same record now assigns the first native continuity requirements by owner: lifecycle/port-space, x87/IRQ13, PIC/IVT, and the reached `0xA1800` map are all Bochs-machine questions, not adapter work. The next package begins from those reached facts only.

> **T225 S5 P0 admission:** [the native continuity ledger](etc/research/t225-s5-minimal-native-ntio-continuity-closure-001.md) converts the S4 handoff into one owner-scoped sequence. Direct/Readonly BOP capability stays stable; Overlay/Virtual receive neither fallback nor new behavior.

> **T225 S6 P8 policy amendment:** product profiles are Direct, Readonly and deferred Overlay. Current CLI/engine composition enables only Direct/Readonly; Overlay remains ABI-compatible but explicitly unselected, never substituted. Virtual is retired from the product contract: its code/evidence are retained without deletion, but it is never selected and receives no new behavior. The [admission record](etc/research/t225-s6-guest-dos-one-shot-fixture-admission-001.md) carries the fixed engine ABI and focused x64 `/MT` evidence. Exact source-built fixture installation remains the next gate.

> **T225 S6 P9 source-link closure:** `Invoke-T225S6FourImageProfileFixture.ps1 -CompileOnly` rebuilds and links the current CPU5/P bx-core/bx-mantle/bx-vdm closure plus the four-image profile fixture under MSVC x64 `/MT`. It passes without S74/prebuilt composition inputs; its result is explicitly compile-only, not guest execution. The exact locked NTIO/NTDOS/COMMAND/SHARE input root is currently absent, so profile execution remains an artifact gate rather than a BOP or Bochs failure.

> **T225 S6 P10 approved reference-input staging:** the owner approved a controlled local OpenNT reference-binary staging set whose four hashes and sizes exactly match the locked source-built identities. Its [provenance record](etc/evidence/t225-s6-reference-guest-input-admission-001.md) permits only this fixture's installation verification; it is neither a new source build nor a default runtime/release input, and does not admit a guest trace.

> **T225 S6 P10 installation closure:** the current-source x64 `/MT` four-image fixture accepts Direct and Readonly against the approved hash-locked reference sequence, prepares the NTIO startup plan, and rejects deferred Overlay plus retired Virtual. The [result](etc/research/t225-s6-reference-direct-readonly-installation-result-001.md) attributes the prior Readonly failure to unconditional Direct-only CLI-stream admission, not BOP or Bochs behavior. This remains installation evidence only, never a guest trace or source-build claim.
> **T225 S6 closure:** the reviewed one-shot fixture plan now has declared artifact identities, entry/order, package BOP dispositions, Direct/Readonly installation evidence, and explicit Overlay/Virtual rejection. Its approved reference sequence remains installation-only; source-built primary-sequence cross-validation and every guest trace remain separate future work.


> **T225 S7 P1 closure:** the [incremental composition-build result](etc/research/t225-s7-incremental-composition-build-result-001.md) proves a manifest-bound `bx-vdm` cold/hot/invalidation cache with a fresh x64 `/MT` fixture link on every run. S6 resumes; its runtime/profile diagnosis remains unchanged.

> **T225 S7 P2 closure:** the [Ninja graph result](etc/research/t225-s7-p2-ninja-build-graph-result-001.md) proves `deps = msvc` header propagation, `bx-vdm.lib`, fresh fixture linking and no-op rebuild behavior under MSVC x64 `/MT`. The custom P1 cache remains evidence only until a complete admitted module graph replaces it. S6 resumes.

> **T225 S7 P3 verification:** the [full graph result](etc/research/t225-s7-full-ninja-module-graph-result-001.md) records a clean 303-edge MSVC x64 /MT Ninja build, four static archives, fresh fixture/native-CLI links, actual MSVC header invalidation, no-op rebuild, and the admitted Direct/Readonly installation regression. The graph now supersedes the P1 executor; only its two implementation scripts remain to be removed, while P1/P2 evidence stays indexed.

> **T225 S7 P4 retirement:** the [retirement record](etc/research/t225-s7-custom-cache-retirement-001.md) confirms removal of exactly the two superseded custom-cache scripts. Historical P1/P2 evidence is retained and indexed; all new incremental builds use the full Ninja graph.

> **T225 S7 closure:** its exit criteria are met: the explicit full Ninja graph has cold-build, header-invalidation, no-op, static-archive, fresh-link, and admitted fixture evidence; the superseded cache implementation is removed and its evidence retained. Selection of a follow-on S remains a separate owner/governance action.

> **T225 S3 P2 closure:** the x64 `/MT` whole-provider fixture passes Direct and Readonly `50:12/00/42/16/02` compatibility. `50:42` now enters the one typed file owner, returns AX bytes/CF clear, and leaves SFT position to guest DOS. S3 closes file I/O only, not EXEC or parent return.

> **T225 S2 P3 closure:** the source-built Direct/Readonly whole-provider fixture passes under MSVC x64 `/MT`: `50:3C` invalidates the target Direct PDB token but retains the Readonly session token, while preserving void resume. The separate lifecycle fixture passes; `50:41` remains an independently proven baseline failure. S2 closes only DEM notification cleanup, not guest EXEC or parent return.
> **T221 S2 P2 progress:** all 29 identities now have one five-subfamily
> provider disposition, a shared four-mode backend contract, current-code
> migration decision, and family regression matrix. Pipes remain Redirector
> deferred. See the [whole-provider disposition](etc/research/t221-s2-file-provider-disposition-001.md).


> **T221 S3 P1 progress:** `dem_whole_provider` now owns one selector-blind
> file-view authority. Direct/Readonly local-file admission migrated through
> it; Overlay/Virtual explicitly require later package-owned backends and
> cannot fall through to Direct. A focused MSVC x64 `/MT` fixture also rejects
> mismatched token-session/view profiles. This is boundary progress only, not
> a subfamily or package closure; see the [P1 record](etc/research/t221-s3-file-view-boundary-001.md).

> **T221 S3 P2 progress:** opaque file-session tokens now carry and validate
> `DIRECT_WIN32_HANDLE` kind before any `HANDLE` lookup; stale tokens and
> session/view-profile mismatch remain rejected. This is the required typed
> token precondition, not an Overlay/Virtual implementation; see the [P2
> record](etc/research/t221-s3-typed-direct-token-001.md).

> **T221 S3 P3 progress:** declared readonly startup images now enter the
> same opaque session token boundary as Direct files under a checked
> `READONLY_NAMESPACE` kind; only the provider sees its private backend token.
> Direct PDB-owner admission now occurs only after the Direct view is chosen,
> restoring Readonly source-shaped refusal. The full source-built whole-provider
> fixture passes under MSVC x64 `/MT`; Overlay/Virtual remain unimplemented.
> See the [P3 record](etc/research/t221-s3-readonly-token-migration-001.md).

> **T221 S3 P4 progress:** the whole provider now owns a dynamic volatile
> COW/tombstone store with teardown lifecycle; focused and full provider
> source-built fixtures pass. It is storage only—no BOP yet routes through it,
> and Overlay/Virtual remain unimplemented. See the [P4 record](etc/research/t221-s3-overlay-store-foundation-001.md).

> **T221 S3 P5 progress:** the same provider now owns a selector-blind,
> opaque-token Overlay file object with private offset/read/write/seek/truncate
> lifecycle.  Its focused x64 `/MT` fixture proves copy-on-write data isolation
> and stale-token refusal; the full whole-provider fixture remains green.  It
> has no namespace resolver or BOP route yet, so Overlay and Virtual remain
> unimplemented. See the [P5 record](etc/research/t221-s3-overlay-file-object-001.md).

> **T221 S3 P6 progress:** a selector-blind resolver now copies an admitted
> host file through a read-only root handle into the provider COW object and
> returns only its private backend token.  Its focused x64 `/MT` fixture proves
> an Overlay modification leaves the real temporary host file unchanged.  No
> namespace or handle BOP route adopts this token yet. See the [P6 record](etc/research/t221-s3-overlay-resolver-001.md).

> **T221 S3 P7 progress:** the Overlay handle-operation subfamily
> (`50:00/02/16/1E/27`) now routes through one typed private file backend;
> it proves COW gather/write/readback, flush, close and stale-token refusal
> without a Direct fallback. `50:08` remains explicit unsupported because no
> Overlay metadata owner is admitted. Namespace/FCB/search and Virtual remain
> open, so this is only a subfamily checkpoint. See the [P7 record](etc/research/t221-s3-overlay-handle-subfamily-001.md).

> **T221 S3 P8 progress:** source/ABI review of the Overlay namespace
> open/create subfamily proves that `50:03/12/22` must wait for one shared
> private access/share/disposition/attribute request, rather than attaching
> the existing base-file resolver to individual BOPs. The unresolved
> volume-label and Redirector paths remain explicit exclusions. See the [P8
> audit](etc/research/t221-s3-overlay-namespace-source-abi-audit-001.md).

> **T221 S3 P9 progress:** Overlay `50:03/12/22` now share one source-shaped
> open/create backend. Whole-provider evidence covers typed PDB-owned tokens,
> create-new conflict, private share conflict, create attributes, close and
> host-base preservation. This closes only that Overlay opening subfamily, not
> the wider namespace or DEM package. See the [P9 record](etc/research/t221-s3-overlay-namespace-open-closure-001.md).

> **T221 S3 P10 progress:** source review confirms the Overlay namespace
> mutation subfamily (`50:04/05/06/17`) needs a single private directory
> relocation/tombstone/merged-child model: original `demRename` can move a
> directory, including unmaterialized host-base descendants. No mutation BOP
> route changed; this prevents a file-only or Direct-fallback implementation.
> See the [P10 state model](etc/research/t221-s3-overlay-namespace-mutation-state-model-001.md).

> **T221 S3 P11 progress:** the private Overlay store now has bounded lazy
> directory relocation records and longest-prefix resolution, with focused
> chained-relocation and cycle-rejection evidence. It remains below BOP
> routing; old-source masking and merged child enumeration are still required
> before the namespace-mutation family can be bound. See the [P11 record](etc/research/t221-s3-overlay-directory-relocation-foundation-001.md).

> **T221 S3 P12 progress:** one selector-blind Overlay visible-node view now
> applies COW nodes, tombstones and lazy relocation before a bounded read-only
> host-root query; old relocation sources are masked while destinations retain
> their effective source. It has no BOP route and no merged-child operation
> yet. See the [P12 record](etc/research/t221-s3-overlay-visible-node-foundation-001.md).

> **T221 S3 P13 progress:** the same Overlay view now has a bounded merged
> child/empty-directory query: it combines host-root enumeration, tombstones,
> COW children and relocation before deciding emptiness. It remains below the
> four namespace mutation services. See the [P13 record](etc/research/t221-s3-overlay-merged-child-foundation-001.md).

> **T221 S3 P14 progress:** one selector-blind Overlay mutation backend now
> owns create/delete/rmdir/file-rename/directory-rename private transactions;
> its family fixture proves no host-base mutation. It has no BOP binding yet:
> EA/error/result mapping and all profile dispositions remain the next
> four-service integration unit. See the [P14 record](etc/research/t221-s3-overlay-mutation-backend-001.md).

> **T221 S3 P15 progress:** Overlay `50:04/05/06/17` now enter that one
> backend through the normal bounded namespace route after shared profile
> selection. Direct/Readonly/Virtual retain their prior dispositions, and the
> full provider fixture covers Overlay create/remove/rename/delete without a
> host mutation. This closes only the Overlay mutation binding; see the [P15
> record](etc/research/t221-s3-overlay-namespace-mutation-binding-001.md).

> **T221 S3 P16 progress:** Overlay `50:01/44` now use the shared visible
> namespace: COW attributes replace host mutation, and check-path replaces the
> historical temporary-NUL create with a visible directory query. The provider
> regression proves attribute set/readback and host-base preservation. See the
> [P16 record](etc/research/t221-s3-overlay-metadata-check-path-binding-001.md).

> **T221 S3 P17 progress:** the remaining FCB identities are now admitted as
> two original-owner packages: wildcard mutation (`50:07/20`) and one FCB
> handle/info/I-O lifecycle (`50:2C..31`). Current Overlay refusal remains
> explicit; the next implementation must add a typed FCB Overlay seam before
> either family is bound. See the [P17 record](etc/research/t221-s3-fcb-owner-package-admission-001.md).

> **T221 S3 P18 progress:** Overlay file tokens now expose private
> attributes/size/DOS time/date, the prerequisite information seam for the FCB
> lifecycle; no FCB service is yet bound. See the [P18 record](etc/research/t221-s3-overlay-file-info-foundation-001.md).

> **T221 S3 P19 progress:** one selector-blind FCB Overlay open/info/close
> backend now source-builds and passes the whole-provider fixture; FCB BOP
> services remain unbound until the full lifecycle/register/DTA family is
> integrated. See the [P19 record](etc/research/t221-s3-fcb-overlay-lifecycle-foundation-001.md).

> **T221 S3 P20 progress:** the complete Overlay FCB lifecycle binding
> (`50:2C..31`) now routes through the one private provider, including its
> checked-DTA `2F` transfer. The corrected source-derived FCB result layouts
> and private share translation pass the full x64 `/MT` provider fixture;
> wildcard mutation, search and Virtual remain open. See the [P20 record]
> (etc/research/t221-s3-fcb-overlay-lifecycle-binding-001.md).

> **T221 S3 P21 progress:** source review admits `50:07/20` only as one
> wildcard owner family. Its missing prerequisite is a selector-blind merged
> Overlay directory enumeration; Direct fallback is expressly rejected. See
> the [P21 audit](etc/research/t221-s3-fcb-wildcard-overlay-enumeration-audit-001.md).

> **T221 S3 P22 progress:** the required selector-blind Overlay visible
> enumeration now merges host base, COW nodes, tombstones and relocations into
> adapter-local DOS entries. Its focused whole-provider evidence passes; the
> two wildcard services are still deliberately unbound. See the [P22 record]
> (etc/research/t221-s3-overlay-visible-enumeration-foundation-001.md).

> **T221 S3 P23 progress:** `50:07/20` now select the shared Overlay
> wildcard provider without Direct fallback. Delete isolation and same-name
> rename failure pass; a distinct-template rename success regression remains
> required before this owner package is closed. See the [P23 record]
> (etc/research/t221-s3-fcb-wildcard-overlay-binding-001.md).

> **T221 S3 P24 progress:** the FCB wildcard owner family (`50:07/20`) is
> closed for Direct/Readonly/Overlay/Virtual disposition. Overlay delete and
> distinct-template rename prove private state transitions and host isolation;
> no native trace has been run. See the [P24 closure]
> (etc/research/t221-s3-fcb-wildcard-overlay-closure-001.md).

> **T221 S3 P25 progress:** `50:09..0C` are admitted as one stateful
> `demsrch.c` owner package. Existing checked DTA/SRCHBUF transport is
> retained, but Overlay needs one selected search-entry source before any
> individual service binding may change. See the [P25 admission]
> (etc/research/t221-s3-search-owner-package-admission-001.md).

> **T221 S3 P26 progress:** Overlay `50:09/0A` now choose the shared visible
> enumeration before the retained pathname/FCB transaction encoders; `0B/0C`
> retain typed continuation. It source-builds, but dedicated four-service
> Overlay regression is still required. See the [P26 binding]
> (etc/research/t221-s3-overlay-search-entry-binding-001.md).

> **T221 S3 P27 progress:** the `demsrch.c` pathname/FCB owner package
> (`50:09..0C`) is closed for Direct/Readonly/Overlay disposition. A real
> Overlay first/next fixture proves tombstone/COW visibility and shared DOS
> ordering; Virtual remains explicitly unavailable. See the [P27 closure]
> (etc/research/t221-s3-overlay-search-family-closure-001.md).

> **T221 S3 P28 progress:** explicit Virtual is admitted as one default-off
> virtual boot-volume namespace package, not a leaf fallback. Existing profile
> and CWD state are reusable, but namespace/token/search/startup backends do
> not yet exist; current Virtual unavailable results remain correct. See the
> [P28 admission](etc/research/t221-s3-explicit-virtual-boot-volume-package-admission-001.md).

> **T221 S3 P29 progress:** the frozen T200 203-identity set has been
> rechecked against current bridge, composition and package-session source.
> It records 125 unchanged routes, 77 later-shim changes, and the retained
> `50:3C` direct-resource lifecycle gap. The audit is static route evidence,
> not provider or runtime closure; see the [all-family audit]
> (etc/research/t221-s3-t200-current-bound-route-audit-001.md).

> **T221 S3 P30 progress:** the default-off Virtual boot-volume implementation
> is now constrained to one selector-blind private-root view. It reuses the
> existing profile, private-token and checked-transaction contracts but may
> not call an admitted host root or reuse Overlay COW materialization. CLI and
> engine admission remain outside the package until its full family regression;
> see the [source/ABI map](etc/research/t221-s3-virtual-namespace-source-abi-map-001.md).

> **T221 S3 P31 progress:** the first selector-blind Virtual private-root
> view now queries and enumerates only private bounded store entries; its x64
> `/MT` probe proves `hostIo=false`. It is a foundation only: no BOP, token,
> startup or CLI admission is bound, so executable Virtual remains unavailable.
> See the [foundation record](etc/research/t221-s3-virtual-private-root-foundation-001.md).

> **T221 S3 P32 progress:** one selector-blind Virtual open/create backend
> now returns only `VIRTUAL_FILE` opaque tokens over private storage. Its x64
> `/MT` probe proves create/read/write/stale-token/not-found behavior with no
> host I/O. It remains below BOP routing and does not extend the Direct-only
> PDB cleanup path; see the [token foundation]
> (etc/research/t221-s3-virtual-private-token-foundation-001.md).

> **T221 S3 P33 progress:** Virtual directory tombstones now mask private
> descendants for both query and enumeration, preserving a single future
> namespace/search/wildcard/rmdir view. The no-host-I/O x64 fixture remains green;
> see the [visibility record]
> (etc/research/t221-s3-virtual-directory-tombstone-visibility-001.md).

> **T221 S3 P34 progress:** Virtual now has one no-host-I/O private mutation
> backend for create/delete/rmdir, with a passing x64 `/MT` fixture. Rename
> and BOP binding remain open pending the complete directory relocation unit;
> see the [mutation foundation](etc/research/t221-s3-virtual-mutation-foundation-001.md).

> **T221 S3 P35 progress:** Virtual now has one selector-blind private
> file/directory rename transaction. It moves the complete private subtree,
> including tombstones, rejects cross-drive and descendant targets, and has no
> host-root input or I/O. The focused x64 `/MT` `hostIo=false` fixture passes;
> DEM BOP binding, CLI/engine admission and native trace remain open. See the
> [rename record](etc/research/t221-s3-virtual-directory-rename-transaction-001.md).

> **T221 S3 P36 progress:** the whole DEM namespace/path partition now
> selects the completed private Virtual backend for `01,03,04,05,06,12,17,22,44`
> as one view, including private metadata and check-path. The full x64 `/MT`
> provider fixture passes; Virtual handle, FCB and search subfamilies remain
> deliberately unbound. See the [partition binding]
> (etc/research/t221-s3-virtual-namespace-partition-binding-001.md).

> **T221 S3 P37 progress:** `VIRTUAL_FILE` tokens now enter private handle
> mechanics and cannot be claimed by the Direct Win32-handle path. The x64
> `/MT` fixture proves open/seek/close/stale-token refusal; controlled I/O,
> truncate and commit remain open within the same handle subfamily. See the
> [typed routing record](etc/research/t221-s3-virtual-handle-token-routing-001.md).

> **T221 S3 P38 progress:** Virtual handle I/O now proves the two checked
> directions: `1E` gathers guest bytes into the private file and `16` emits a
> checked guest write after seek. The x64 `/MT` fixture also retains close and
> stale-token refusal. File-time/commit/truncate remain in this subfamily's
> final sweep; see the [I/O transaction record]
> (etc/research/t221-s3-virtual-handle-io-transaction-001.md).

> **T221 S3 P39 progress:** private Overlay/Virtual file objects now preserve
> the original `50:08` DOS time/date get/set contract through typed store
> state. Virtual set→get passes under x64 `/MT` without host mutation; see the
> [file-time record](etc/research/t221-s3-private-file-times-001.md).

> **T221 S3 P40 progress:** the six-service Virtual handle owner family
> (`00,02,08,16,1E,27`) now has typed-token lifecycle, checked I/O, file-time,
> truncate and commit evidence under x64 `/MT`; no Direct handle fallback is
> possible. This closes that subfamily only—not FCB or search. See the
> [handle closure](etc/research/t221-s3-virtual-handle-family-closure-001.md).

> **T221 S3 P41 progress:** the eight-service Virtual FCB owner family
> (`07,20,2C..31`) now uses the one private Virtual view/token lifecycle,
> including checked `2F` DTA gather and no-host-I/O wildcard rename/delete.
> The fresh x64 `/MT` whole-provider fixture proves no host namespace fallback.
> This closes that subfamily only—not Virtual search or the complete DEM
> package. See the [FCB closure]
> (etc/research/t221-s3-virtual-fcb-family-closure-001.md).

> **T221 S3 P42 progress:** the four-service Virtual `demsrch.c` family
> (`09..0C`) now selects one private entry view for both pathname and FCB
> first-search, then retains typed next-search continuation. Dedicated and
> whole-provider x64 `/MT` fixtures prove that private `GAMMA/OMEGA` results
> hide present host `ALPHA/ZETA` entries. This closes that subfamily only—not
> the complete DEM package. See the [search closure]
> (etc/research/t221-s3-virtual-search-family-closure-001.md).

> **T221 S3 P43 progress:** a distinct package-session fixture now proves
> the installed `dem_whole_provider` is reached through normal ingress,
> registry and session dispatch: its `50:00` invalid-token result is distinct
> from generic unavailable, while `50:47/48` retain Redirector-deferred
> disposition. MSVC x64 `/MT` source closure, link and run all pass. This is
> an installation-seam witness only; it does not close the 29-identity,
> four-profile DEM provider matrix. See the [P43 record]
> (etc/research/t221-s3-whole-provider-session-integration-001.md).

> **T221 S3 P44 progress:** the 29 identities are now mapped to the one
> package-session's five routes and their shared transport prerequisites:
> bounded RAM gather/write, copied CurrentPDB, and registered DTA. The next
> matrix fixture will install those prerequisites once per profile rather than
> add partition-local BOP shims. See the [P44 transport map]
> (etc/research/t221-s3-package-session-matrix-transport-map-001.md).

> **T221 S3 P45 progress:** boot-namespace plane initialization now clears
> every borrowed session field before reconstructing its owned provider. A
> source-built MSVC x64 `/MT` regression creates isolated Direct, Readonly,
> Overlay and Virtual sessions and sends `50:00`, `50:47` and `50:48` through
> normal ingress/registry/session dispatch in each. This closes only the
> four-profile installation lifecycle, not the 29-identity matrix; see the
> [P45 record](etc/research/t221-s3-package-session-profile-lifecycle-001.md).
> **T221 S3 P46 progress:** one positive package-session chain now registers
> DTA/CurrentPDB through `50:1B`, opens declared `COMMAND.COM` through `50:12`,
> copies one byte by `50:16`, and closes by `50:02`, in isolated Direct,
> Readonly, Overlay and Virtual sessions. It proves normal ingress, checked RAM
> transport and opaque-token lifecycle only; the broader 29-identity matrix
> remains open. See the [P46 record]
> (etc/research/t221-s3-package-session-startup-handle-chain-001.md).
> **T221 S3 P47 progress:** Direct and Readonly now send original-shaped
> `50:09`/`50:0B` pathname search through the package session, including the
> DTA far output pointer, copied CurrentPDB and 43-byte continuation. Overlay
> and Virtual intentionally remain outside this declared-snapshot path pending
> their private-view legs. See the [P47 record]
> (etc/research/t221-s3-package-session-declared-search-001.md).
> **T221 S3 P48 progress:** all four profile sessions now cover the original
> FCB date (`50:30`) and zero-token close (`50:2E`) terminals. The declared
> pathname search pattern is narrowed to the two declared COM images and proves
> continuation by a successful `50:0B`, not random-token byte values. FCB
> file/I-O and private search remain open. See the [P48 record]
> (etc/research/t221-s3-package-session-fcb-terminals-001.md).
> **T221 S3 P49 progress:** Overlay and Virtual now create `C:\\FCBTEST.COM`
> through `50:03`, open it through `50:2D`, and close it through `50:2E` in the
> normal package session. The fixture is entirely private—no host file
> mutation or backend-token injection occurs. This is not FCB I/O/wildcard
> closure; see the [P49 record]
> (etc/research/t221-s3-package-session-private-fcb-chain-001.md).
> **T221 S3 P50 progress:** the private Overlay/Virtual FCB chain now includes
> `50:2F` write and read through the DTA guest-RAM address between `50:2D` open
> and `50:2E` close. The original `AX:BP` token ABI and mechanical action path
> are proven without host I/O. FCB wildcard/metadata and other profile legs
> remain open; see the [P50 record]
> (etc/research/t221-s3-package-session-private-fcb-io-001.md).
> **T221 S3 P51 progress:** Overlay and Virtual now create two private COM
> files through `50:03` and enumerate them through `50:09`/`50:0B` using their
> private namespace views, DTA output and copied CurrentPDB. Neither host
> namespace nor declared snapshot supplies those results. FCB search and other
> owner families remain open; see the [P51 record]
> (etc/research/t221-s3-package-session-private-search-001.md).
> **T221 S3 P52 progress:** Overlay and Virtual now run the original-shaped
> namespace mutation quartet through the package session: `50:17` rename,
> `50:05` delete, `50:04` mkdir, and `50:06` rmdir. The source-defined
> delete/delete-directory service numbers were verified against `dossvc.h`;
> private state changes do not mutate the host. This is a planned namespace
> family leg, not DEM-package closure; see the [P52 record]
> (etc/research/t221-s3-package-session-private-namespace-mutation-001.md).
> **T221 S3 P53 progress:** Direct and Readonly now open one existing host
> `C:` system file through `50:2D`, read one byte to the registered DTA by
> `50:2F`, then close its opaque FCB token by `50:2E`. This is source-shaped
> permitted Win32 host integration, with no host mutation; it is not FCB-family
> or package closure. See the [P53 record]
> (etc/research/t221-s3-package-session-direct-readonly-fcb-001.md).
> **T221 S3 P54 scope amendment:** new Virtual provider/session/CLI work is
> deferred. Existing Virtual implementation and evidence remain retained; this
> pass completes the Direct/Readonly/Overlay matrix and must not claim
> four-profile closure. The later package-level resumption is in `TODO.md`; see
> the [P54 decision](etc/research/t221-s3-virtual-work-deferral-001.md).
> **T221 S3 P55 progress:** Direct and Readonly now send original-shaped FCB
> file information (`50:31`) through the normal package session for an existing
> host `C:` system file. `AX/CX/DX/BX:DI` result layout and no-mutation host
> integration pass under MSVC x64 `/MT`; Virtual receives no new behavior. See
> the [P55 record](etc/research/t221-s3-package-session-direct-readonly-fcb-info-001.md).
> **T221 S3 P56 scope amendment:** new Overlay and Virtual provider/session/CLI
> work is deferred. Existing code, typed file-view/token seams, profile
> authority and evidence remain retained; Direct/Readonly completion may not
> remove, bypass or change their selector-blind contracts. This pass therefore
> seeks only two-profile closure; the later resumption is in `TODO.md`.
> **T221 S3 P57 progress:** Direct now runs a self-cleaning temporary-host
> package-session chain through `03,1E,00,16,08,27,02,05`: create/write/seek/
> read/time/commit/close/delete. It proves typed token and checked RAM transport
> without touching ambient host files; Readonly and remaining owner families
> stay open. See the [P57 record]
> (etc/research/t221-s3-package-session-direct-handle-namespace-chain-001.md).
> **T221 S3 P58 progress:** Readonly now refuses normal-session handle write
> and namespace mutation `1E,03,22,04,05,06,17` with source-shaped `CF/AX=5`
> on valid test-owned paths. It changes no Overlay/Virtual behavior; see the
> [P58 record](etc/research/t221-s3-package-session-readonly-mutation-refusal-001.md).
> **T221 S3 P59 progress:** Direct now also proves the remaining normal
> namespace-positive chain in its self-owned temporary C: root:
> `22,17,01,04,44,06,05` (create-new, rename, attribute query, mkdir,
> check-path, rmdir and delete). This uses the same checked RAM and typed-token
> seams, while Readonly's policy refusal and frozen Overlay/Virtual contracts
> remain unchanged; see the [P59 record]
> (etc/research/t221-s3-package-session-direct-namespace-positive-001.md).
> **T221 S3 P60 progress:** Direct now proves FCB wildcard delete/rename
> (`07,20`) over files in its self-owned temporary C: root; Readonly returns
> `CF/AX=5` for the same pair before host enumeration. The rename witness
> follows the original wildcard-template rule (`FCBR1.DAT → RENR1.TMP`), and
> Overlay/Virtual behavior is unchanged; see the [P60 record]
> (etc/research/t221-s3-package-session-direct-readonly-fcb-wildcard-001.md).
> **T221 S3 P61 progress:** Direct and Readonly now both prove the FCB search
> continuation pair `0A→0C` through one package session: checked SRCHBUF/path/
> PDB gather, returned continuation state, then consumed next state. The witness
> deliberately does not assume ambient host C: sort order; frozen Overlay/
> Virtual behavior remains unchanged. See the [P61 record]
> (etc/research/t221-s3-package-session-direct-readonly-fcb-search-001.md).
> **T221 S3 P62 progress:** Direct now proves `2C→2E→05` FCB create/close/
> cleanup in its self-owned temporary C: root, while Readonly receives
> `CF/AX=5` from the shared file-view admission before creation. No FCB-local
> profile exception was introduced, and Overlay/Virtual remain frozen; see the
> [P62 record](etc/research/t221-s3-package-session-direct-readonly-fcb-create-001.md).
> **T221 S3 P63 progress:** Direct/Readonly now share `01` read-only metadata
> query and `44` actual-host-path query evidence; valid 8.3 no-match requests
> for pathname/FCB first-search (`09,0A`) return original-shaped `CF/AX=18`.
> The fixture uses its empty owned C: root, not ambient-directory ordering;
> Overlay/Virtual remain frozen. See the [P63 record]
> (etc/research/t221-s3-package-session-direct-readonly-query-search-terminal-001.md).
> **T221 S3 P64 progress:** Readonly FCB I/O write (`2F`, `BX=0`) now reaches
> shared file-view admission before any Win32 `WriteFile` and returns `CF/AX=5`.
> Direct and retained private-backend routes are unchanged; see the [P64 record]
> (etc/research/t221-s3-readonly-fcb-write-admission-001.md).
> **T221 S3 P65 progress:** Direct `08` set-time now runs on a self-owned
> temporary file, while the Readonly declared-image set-time branch returns
> `CF/AX=5`; both retain their existing get-time behavior. See the [P65 record]
> (etc/research/t221-s3-direct-readonly-file-times-001.md).

> **T221 S3 P66 progress:** a closed Direct or Readonly opaque file token now
> proves the shared original-shaped stale-handle terminal: `50:00` returns
> `CF/AX=6` rather than accessing a host handle or falling through to a profile
> default. This adds only two-profile regression evidence; see the [P66 record]
> (etc/research/t221-s3-direct-readonly-stale-token-001.md).
> **T221 S3 P67 progress:** the Direct/Readonly `demhndl.c` family now has a
> complete shared matrix: valid/released seek, close, time, read, write and
> commit have explicit source-shaped outcomes. Overlay/Virtual receive no new
> behavior; see the [P67 record]
> (etc/research/t221-s3-direct-readonly-handle-family-matrix-001.md).
> **T221 S3 P68 closure evidence:** all 29 selected identities now have an
> original-owner map, one package-provider route and explicit Direct/Readonly
> family evidence; pipes remain unavailable and Overlay/Virtual remain frozen.
> See the [29-identity audit]
> (etc/research/t221-s3-direct-readonly-29-identity-closure-audit-001.md).
> **T221 S1 P1 progress:** the T219/T200 73-row DEM ledger has been checked
> against the current generic bridge, composition, package session, facade,
> whole-provider, and plane source. All ten static route classes remain
> present; this is deliberately not a provider or runtime claim. The next
> P adds the source/ABI/failure and four-rung recovery ledger. See the
> [current route audit](etc/research/t221-s1-dem-current-bound-route-audit-001.md).

> **T221 S1 P2 progress:** each of the 73 historical handlers now has one
> source-owner location across twelve DEM translation units. This is the
> module-level decomposition for the source/ABI/failure ledger, not a claim
> that the existing bx-vdm partitions compose those units. See the
> [source-owner map](etc/research/t221-s1-dem-source-owner-map-001.md).

> **T221 S1 P3 progress:** dependency evidence separates ordinary filesystem
> capability from the pervasive historical CCPU/SAS/SoftPC/VDD/engine and
> raw-device dependencies. No complete original DEM translation unit is
> directly admitted into the modern x64 graph; recovery must use the smallest
> checked seam or a documented source-derived contract. See the
> [dependency map](etc/research/t221-s1-dem-dependency-map-001.md).

> **T221 S1 P4 progress:** a 73-row source/ABI/failure recovery ledger now
> assigns every DEM service to one of seven non-overlapping source-owner
> groups while retaining its current bound route and shim/fallback evidence.
> It does not upgrade historical fixture evidence into current package
> closure. See the [recovery ledger](etc/research/t221-s1-dem-recovery-ledger-001.md).

> **T221 S1 P5 closure:** the current shim/fallback action is now explicit
> for all 73 DEM services: 55 migrate, seven replace, and eleven retain; no
> deletion is yet justified. S1 is complete and recommends a package-wide
> file/handle/FCB/search provider disposition next, rather than a trace-led
> endpoint patch. See the [workaround disposition](etc/research/t221-s1-dem-workaround-disposition-001.md).

> **T220 S1 P1 closure:** OpenNT proves `50:36` is only DOS-entry/VDD
> notification.  The historical resource contract instead roots in the
> guest-published `pusCurrentPDB`, PDB/JFT/SFT mappings and a guarded
> `50:3C` termination sequence.  Current direct DEM resources lack that
> owner identity, while the existing lifecycle provider releases only finite
> boot state.  The next seam is a bounded copied guest-PDB read contract, but
> it requires a new implementation S; no runtime change was made.  See the
> [ownership map](etc/research/t220-s1-dem-process-resource-ownership-map-001.md).

> **T220 S2 P1 closure:** the existing `50:1B` source-derived registration
> already holds the checked `CurrentPDB` physical word needed for the first
> owner association.  T220 therefore admits only a two-byte copied
> `CurrentPDB` helper, not an independent PSP scanner or PDB/JFT/SFT walker.
> Profile policy remains outside that identity helper.  See the
> [bounded-read design](etc/research/t220-s2-bounded-dem-process-read-design-001.md).

> **T220 S3 P1 closure:** the isolated DEM-private helper now source-builds
> under MSVC x64 `/MT` and proves exact two-byte `CurrentPDB` copying plus
> malformed/absent refusal.  It is deliberately not bound to a BOP, a host
> handle or `50:3C`; the next S must map the full DEM owner-association
> transaction before integration.  See the
> [result](etc/research/t220-s3-dem-current-pdb-helper-result-001.md).

> **T220 S4 P1 closure:** the direct namespace-handle transaction must copy
> `CurrentPDB` before host open/adopt, commit that owner with the opaque token,
> and later release only equal-PDB tokens.  FCB/search and `50:3C` binding
> remain explicitly outside this package.  See the
> [transaction map](etc/research/t220-s4-direct-dem-resource-transaction-map-001.md).

> **T220 S5 P1 closure:** `50:03/12/22` now perform the registered exact
> two-byte `CurrentPDB` read before the owned direct backend can open or
> publish a token. Focused x64 `/MT` owner/session and local-backend fixtures
> pass, and 138 current composition sources plus the complete CPU5
> core/mantle seed link as a fresh source-only audit DLL. `50:3C`, FCB and
> search ownership remain unbound. See the
> [result](etc/research/t220-s5-direct-dem-namespace-owner-result-001.md).

> **T220 S6 P1 closure:** original `50:3C` is a guarded search/VDD/host
> termination notification before guest DOS runs `DOS_ABORT`'s JFT/FCB/SFT
> close path. The current finite provider preserves only that no-result
> notification. S5's direct-token `release_owner` must therefore **not** be
> bound to `50:3C`; S7 subsequently confirmed that an adapter JFT/SFT map
> would duplicate guest DOS. See the
> [admission map](etc/research/t220-s6-dem-pdb-termination-admission-map-001.md).

> **T220 S7 P1 closure:** direct DEM tokens preserve OpenNT's `AX:BP`
> host-handle placement as the guest-owned JFT/SFT close path's private
> substitute. Reconstructing JFT/SFT in the adapter would duplicate guest DOS;
> no `release_owner` lifecycle caller is admitted. See the
> [representation map](etc/research/t220-s7-direct-token-jft-sft-representation-map-001.md).

> **T216 S1 P1 closure:** all five declared readonly namespace slots already
> exist, but only CONFIG/AUTOEXEC are claimed by the installed direct DEM
> provider; command and targets fall through to host open/search. Original
> SYSINIT/DEM contracts select a single source-derived all-declared-image
> provider seam, not a `50:3D` patch or Bochs change. See the [complete
> map](etc/research/t216-s1-guest-executable-image-visibility-map-001.md).

> **T216 S2 P1 progress:** the direct provider now treats every ready declared
> namespace slot as immutable, including `COMMAND.COM`; a fresh x64 `/MT`
> provider probe proves its opaque `50:12` token/size path and preserves the
> existing CONFIG read/refusal checks. Host-plus-declared search merge remains
> open, so this is not package closure. See the [P1 record](etc/research/t216-s2-declared-image-handle-p1-001.md).

> **T216 S2 P2 progress:** pathname search now merges the declared root-image
> snapshot over host entries. A fresh x64 `/MT` test proves root `COMMAND.COM`
> uses declared metadata while a non-root host search keeps `ALPHA/ZETA` host
> results. FCB search remains open; no package-closure claim is made.

> **T216 S2 P3 progress:** FCB FindFirst now uses the same merged input; fresh
> x64 `/MT` search evidence returns declared `COMMAND.COM` name/size, and the
> complete DEM provider probe recompiles and passes. Target-image family
> coverage and final composition regression remain open.

> **T216 S2 P4 progress:** the whole-provider fixture now proves readonly
> open/token lifecycle for declared COMMAND, first target and optional terminal
> target images. Its x64 `/MT` run passes. A separate full-composition attempt
> emitted only partial compile evidence and no link/run result, so composition
> regression remains explicitly open.

> **T216 S2 P5 closure:** all five declared image slots now share the readonly
> provider and merged pathname/FCB-search seam. Fresh focused x64 `/MT`
> regression passes; the full composition links and reaches the unchanged
> source-backed `50:3D` controlled terminal. This closes visibility only, not
> COMMAND startup; see the [closure record](history/m0-t216-closure-20260815.md).

> **T217 S1 P1 finding:** `cmdenv.c` cannot link without the historical
> CCPU/SoftPC/product shell, but it supplies the normative initial-environment
> algorithm. The former 4,023-byte adapter ceiling was removed; its ANSI-to-OEM sequence
> requires one source-derived seam correction before it can be treated as
> source-first compliant. The distinct AUTOEXEC merge remains unclaimed. See
> the [source-reuse audit](etc/research/t217-s1-command-environment-source-reuse-audit-001.md).

> **T217 S2 P1 closure:** the initial-environment seam now follows the
> OpenNT input/conversion order (`GetEnvironmentStringsA` then per-entry
> `CharToOemBuffA`) while retaining its filters, default PROMPT, COMSPEC
> prepend and paragraph retry. Focused x64 `/MT`, 5,001-byte capacity, and
> full-current-source composition regressions pass. The native terminal is
> unchanged at `50:3D`; neither AUTOEXEC merge nor COMMAND execution is
> claimed. See the [source-reuse audit](etc/research/t217-s1-command-environment-source-reuse-audit-001.md).

> **T217 S3 P1 finding:** OpenNT SYSINIT reaches its second normal DEM Open
> after successful DOS allocation, then branches directly to `comerr`/`50:3D`
> without a second seek/read/close. The next work is a bounded integration
> verification of the installed whole DEM file/handle package—not a `50:12`,
> EXEC, or terminal implementation. The map also records the separate 32 KiB
> DOS EXEC environment-scan condition. See the [whole-package map](etc/research/t217-s3-sysinit-dos-exec-whole-package-map-001.md).

> **T217 S4 P1 closure:** the second SYSINIT DEM Open is accepted by the
> installed whole provider but resumes with `AX=2` and carry set, so SYSINIT
> correctly reaches `comerr`/`50:3D`. This invalidates any claim that focused
> declared-image visibility alone proves native identity closure. The selected
> next package is DEM declared-image namespace identity, not EXEC/Bochs/BOP
> work; see the [integration result](etc/research/t217-s4-dem-normal-file-integration-result-001.md).

> **T217 S5 P1 closure:** source mapping proves profile declaration, one-based
> boot drive, guest default drive, DEM session default drive, rooted-path
> canonicalization, declared namespace ownership, and fallback error mapping
> all align. The sole missing fact is the existing copied normal-Open identity;
> S6 therefore admits only a default-off package-scoped classification record,
> not a route or behavior change. See the [identity map](etc/research/t217-s5-dem-declared-image-namespace-identity-map-001.md).

> **T217 S6 P1 closure:** the normal Open is on admitted C: with a live
> declared namespace, but is not any declared image identity; the existing
> fallback correctly returns DOS file-not-found. Focused whole-provider and
> full-current-source x64 `/MT` evidence pass without a semantic change. The
> next package is guest SYSINIT command-name realization, not a DEM or Bochs
> repair; see the [classification result](etc/research/t217-s6-dem-namespace-identity-classification-result-001.md).

> **T217 S8 P1 closure:** the initial COMMAND environment seam now preserves
> the source's `COMSPEC=` and `WINDIR` first-prefix filtering state, retained
> later/malformed entries, ANSI-to-OEM order and name-only uppercasing. Fresh
> x64 `/MT` source-built evidence passes; the historical translation unit
> remains non-composable and AUTOEXEC merge/COMMAND execution remain open. S7
> SYSINIT mapping is paused rather than closed; see the [updated recovery
> audit](etc/research/t217-s8-command-initial-environment-semantic-repair-001.md).

> **T217 S9 P1 closure:** source-built NTIO provenance, `commnd`, NTDOS
> relocation and original `$Open`/`TransPath` prove that BOP `DS:SI` must point
> to the relocated DOSDATA WFP buffer rather than raw SYSINIT data. The
> original root-path result and current declaration/resolver agree; no DEM
> workaround is admitted. S10 may observe only the existing copied request's
> symbolic shape. See the [realization map](etc/research/t217-s9-sysinit-command-name-realization-map-001.md).

> **T217 S10 P1 closure:** the default-off native observation reports a C:
> backslash-rooted but three-component, 32-byte WFP where original
> `\\COMMAND.COM` requires one component and 15 bytes. This excludes a
> separator/drive mismatch and moves the evidence boundary to guest SYSINIT
> command-name relocation/lifetime; no DEM workaround is admitted. See the
> [shape observation](etc/research/t217-s10-sysinit-wfp-shape-observation-001.md).

> **T217 S11 P1 closure:** the three-component WFP is the exact source-derived
> CONFIG `SHELL=%SystemRoot%\\System32\\command.com` override, not a BIOS
> relocation fault. The historical command file is absent from both current
> host system directories; no missing Win32 API is involved. S12 admits one
> explicit COMMAND configuration capability seam that retains the original
> override only when it is actually available. See the [disposition](etc/research/t217-s11-command-shell-capability-disposition-001.md).

> **T217 S12 P1 closure:** the copied two-state COMMAND shell capability
> preserves OpenNT's host shell only when present and otherwise omits that
> unavailable override. Focused x64 closures pass; the native run crosses the
> former `50:3D` terminal and reaches `50:36`. The ordinal Open observer is
> not a final command-load identity claim. S13 maps the full DOS loader/EXEC
> package before any new selector implementation. See the [result](etc/research/t217-s12-command-shell-capability-result-001.md).

> **T217 S13 P1 closure:** `50:36` is the original no-VDD DOS-entry
> notification, not a loader or launch implementation point. The complete
> map assigns the current next seam to the shared `50:12/00/16/02[/42]`
> image-loader handle family; S14 may regress that family but may not run a
> trace or add a leaf handler. See the [package map](etc/research/t217-s13-dos-loader-exec-owner-package-map-001.md).

> **T217 S14 P1 closure:** the full x64 composition closure uses an explicit
> 8 MiB host stack reserve because its existing bounded transaction buffers
> nest beyond the MSVC default 1 MiB reserve. A dedicated source-built
> loader-family regression now proves one declared-image token across
> `50:12 → 50:00 → 50:42 → 50:16 → 50:02`, including original ZF/seek ABI and
> forged-token refusal. It neither claims DOS EXEC/COMMAND completion nor runs
> a native trace. See the [family result](etc/research/t217-s14-dem-loader-family-result-001.md).

> **T215 S1 P1 progress:** the re-exported historical inventory retains all
> 203 identities. Current-source reconciliation corrects T200's stale XMS
> and COMMAND composition descriptions, preserves deferred/unavailable family
> boundaries, and selects DEM direct-host file/handle/FCB/search recovery as
> the next whole package—without selecting any BOP from T214's terminal
> observation. See the [reconciliation record](etc/research/t215-s1-current-bop-owner-package-reconciliation-001.md).

> **T215 S2 P1 correction:** the candidate DEM direct-host package was
> already completed by T202 S4's source map and r41/r42 atomic current-source
> route switch. Current DEM session source confirms that no legacy fixture
> route may claim an installed whole-provider identity. The duplicate package
> is removed rather than reimplemented; see the [current-state correction](etc/research/t215-s2-dem-direct-host-current-state-correction-001.md).

> **T215 S3 P1 closure:** the fresh full-source x64 `/MT` run copied
> `C4 C4 50 3D` at `8DC8:0984` with typed STOP. This is OpenNT
> `SVC_DEMEXITVDM` / `demExitVDM`, mapped by the current profile to an honest
> controlled terminal—not a watchdog, missing `50:11`, or normal completion.
> The observer returns to disabled after the fixture. See the [causal
> record](etc/research/t215-s3-native-controlled-terminal-causal-map-001.md).

> **T215 S4 P1 finding:** current `bx_ntvdm_composition_runtime_v1` omits the
> existing T204 reader and `set_startup_configuration` call entirely, despite
> the CLI already propagating the paired sources and the composition exposing
> the bind seam. The zero-byte declared CONFIG/AUTOEXEC namespace can thus
> select the source-backed `50:3D` fatal path. S4 recovers this whole existing
> package attachment; it is not a new BOP implementation.

> **T215 S4 P2 progress:** the existing T204 reader/provider is now compiled
> and attached before composition bind; the full x64 `/MT` closure succeeds
> with paired generated inputs. The native terminal remains `50:3D` because
> T202's direct-host DEM provider has priority over the T204 contained images.
> The remaining work is a whole startup-image overlay contract at that provider
> boundary, not a parser or leaf-BOP change. See the [attachment
> record](etc/research/t215-s4-startup-configuration-attachment-001.md).

> **T215 S5 P1 progress:** the direct provider now borrows only ready T204
> `CONFIG.SYS`/`AUTOEXEC.BAT` images and intercepts their exact root-relative
> paths ahead of Win32 open/read/seek/close/time/mutation operations. The
> strict x64 `/MT` package probe proves copied reads and write refusal without
> a host-file side effect. A fresh full composition build still ends at the
> same typed `50:3D` stop, so the overlay is a closed provider seam but not
> evidence that the native startup path consumed it or that COMMAND is ready.
> See the [S5 record](etc/research/t215-s5-startup-image-direct-provider-overlay-001.md).

> **T215 S6 P1 closure:** a fixture-only 128-record observer captured 27
> accepted BOP identities with no overflow. `54:0C -> 50:12/00/16/02` proves
> that the attached CONFIG image is consumed in the native path; the final
> `8DC8:0984 50:3D` is higher-confidence SYSINIT command-interpreter-load
> failure, not a configuration-read failure or asserted COMMAND completion.
> No handler or machine behavior changed. See the [causal
> map](etc/research/t215-s6-startup-bop-sequence-causal-map-001.md).

> **T209 closed:** declared CONFIG/AUTOEXEC entries can now exist unbound,
> bind to provider-owned images and reproject their metadata without synthetic
> content; see the [closure record](history/m0-t209-closure-20260815.md).

> **T210 S1 closure:** original `50:13/18/1A` and their COMMAND consumers
> form one host-context package; all required modern Win32 APIs are available
> to a normal CLI process. See the [owner map](etc/research/t210-s1-host-context-owner-package-map-001.md).

> **T210 S2 closure:** one session-owned provider design now defines the
> shared state, Win32 seam, four profile modes, migrations and full DEM/COMMAND
> family regression before implementation. See the [design record](etc/research/t210-s2-host-context-provider-design-001.md).

> **T210 S3 P1 progress:** a single bounded session host-context provider now
> source-builds under MSVC x64 `/MT`; it validates direct admitted roots,
> preserves readonly refusal, and projects overlay/virtual state to COMMAND.
> The root-directory validation repair is covered by the focused fixture. This
> is substrate only: DEM/COMMAND consumers have not yet migrated. See the
> [P1 evidence](etc/research/t210-s3-session-host-context-provider-p1-001.md).

> **T210 S3 P2 progress:** DEM's old CWD type now wraps the one session
> provider, and COMMAND dynamically projects the same state for `54:04` with
> no synthetic-root fallback. Focused and full composition x64 `/MT` fixtures
> pass; the deliberately namespace-free DEM package fixture still returns
> `197` and remains negative evidence. See the [P2 migration
> record](etc/research/t210-s3-session-host-context-provider-p2-migration-001.md).

> **T210 closed:** one source-built family now covers DEM `13/18/1A` and
> COMMAND `02/04/0F` through one session context. A later native observer
> rejected its prior execution plan before CPU entry; it recorded no BOP and
> does not narrow the next package. See the [closure
> record](history/m0-t210-closure-20260815.md).

> **T211 S1 closure:** all 17 COMMAND services and the dispatcher sentinel
> now have one owner component, source/ABI/failure disposition and workaround
> action. The first implementation packet is bootstrap/environment/config;
> endpoint evidence is not component closure. See the [complete
> map](etc/research/t211-s1-command-owner-package-source-abi-disposition-map-001.md).

> **T211 S2 P1 progress:** one versioned COMMAND bootstrap/environment/config
> provider now owns the copied state for `01/02/04/05/0C/0D/0F/10`; two fresh
> MSVC x64 `/MT` fixtures prove exact component membership and migrated
> composition. `54:10` remains an explicit deferred member because its
> historical `DosSessionId` input has no admitted CLI ABI. See the [P1
> record](etc/research/t211-s2-command-bootstrap-provider-p1-001.md).

> **T211 S2 P2 progress:** `54:10` no longer shares the host-child execution
> path with `54:08`; its package-owned deferred disposition now yields a
> checked controlled stop, verified by the fresh integrated x64 `/MT` r7
> fixture. No historical `DosSessionId` result was fabricated.

> **T211 S2 P3 progress:** the source-built component fixture now includes
> malformed COMSPEC/SetInfo/boot-path rejection and `54:04` no-context failure
> with zero guest writes (r8). The remaining S2 work is a final full-member
> result matrix and provider/facade cleanup review, not new BOP selection.

> **T211 S2 closed:** the versioned provider owns all eight
> bootstrap/environment/config members; r7/r8 supply integrated and negative
> source-built x64 `/MT` evidence. `54:10` remains explicit deferred rather
> than fabricated. See the [closure record](history/m0-t211-s2-closure-20260815.md).

> **T211 S3 P1 closure:** original `cmdredir.c`/`cmdexec.c` and current
> stream-child/lifecycle routes now have one four-member source/ABI/failure
> disposition map. Modern Win32 APIs are available, but BaseSrv/CSR, raw
> VDM pointers/handles, WOW/PIF, Redirector and ambient-state dependencies
> remain explicit blockers rather than excuses for a simplified leaf handler.
> See the [S3 map](etc/research/t211-s3-command-launch-execution-source-abi-map-001.md).

> **T211 S3 P2 progress:** one versioned launch/execution provider now owns
> the existing stream-child and execution-lifecycle state. The full bx-vdm
> composition remains source-built and passing under MSVC x64 `/MT`; member
> outcome reconciliation and family regression remain open.

> **T211 S3 P3 progress:** the new source-built component fixture and whole
> composition fixture pass under x64 `/MT`. They cover all four memberships,
> non-direct `54:06` no-token failure, `54:07` DOS-domain result and
> `54:08/0A` unsupported form. Direct terminal-child failure/lifecycle and
> final provider routing cleanup remain open.

> **T211 S3 P4 progress:** launch/execution direct-profile selection now
> belongs to its provider, not the COMMAND session coordinator; fresh x64
> `/MT` r4 composition evidence passes. The remaining work is the component
> failure/lifecycle ledger and final family regression.

> **T211 S3 closed:** one launch/execution provider owns `06/07/08/0A`,
> resolves its own profile, and passes source-built component plus composition
> regressions. See the [closure record](history/m0-t211-s3-closure-20260815.md).

> **T205 S1 closure:** current OpenNT and bx-vdm/mantle sources map all
> twelve XMS slots, the partial routes and the necessary C3/C5 machine gates
> into one ordered owner-package plan. No selector implementation or trace was
> added; see the [whole-package plan](etc/operations/t205-xms-whole-package-plan-001.md).

> **T205 S2 P1 progress:** the selector-blind mantle allocator now exposes
> bounded total/largest-free statistics, and the XMS session routes original
> `52:04` page geometry and `52:0A` allocator statistics alongside its existing
> A20/block paths. The strict adapter package regression passes. The current
> broad native machine recipe compiles the changed mantle source but has an
> inherited unrelated CPU object-link closure failure; this is negative build
> evidence, not a passing native result. `52:01` is covered by P2; `06..09`
> remain deferred.

> **T205 S2 P2 progress:** `52:01` reads the original 12-byte `SS:BP-12`
> move record through the existing synchronous copied mechanical-action seam
> and submits a selector-blind checked physical backing-RAM copy to mantle. The
> strict package regression proves the request mapping; native machine closure
> remains pending the existing broad CPU link failure. UMB/INT15 remain out of
> scope.

> **T205 S2 P3 progress:** the package regression now covers zero-size
> allocation and stale free/reallocate as explicit source-derived safe failures
> (`AX=0` or `CX=0`). OpenNT leaves some zero/same-size temporaries
> uninitialized, so S2 records no invented success result; the current bounded
> provider fails those indeterminate requests rather than exposing undefined
> state.

> **T205 S2 closure:** the admitted eight-slot group is complete at its
> source-built package boundary; see the [closure
> record](history/m0-t205-s2-closure-20260815.md). `06..09` remain deferred
> pending separately admitted C3/C5 mechanics.

> **T205 S3 closure:** the complete C3 source and no-device machine-map
> comparison rejects a synthetic UMB capability. `52:06..08` remain one
> deferred XMS subgroup; their conditional provider S4 is not admitted. See
> the [C3 decision](etc/research/t205-s3-c3-umb-machine-admission-decision-001.md).

> **T205 S5 closure:** the original `52:09` handler requires IVT validation,
> keyboard/BIOS callout state and historical recursive execution. The no-device
> machine admits none of them, so C5 and `52:09` remain deferred. See the
> [C5 decision](etc/research/t205-s5-c5-int15-machine-admission-decision-001.md).

> **T205 S6 closure:** one strict source-built package fixture covers all
> callable `52:00..0B`; `06..09` give typed stop and `52:0C` is the original
> non-callable `XMS_LASTSVC` boundary. See the [convergence record](etc/research/t205-s6-xms-whole-package-convergence-001.md).

> **T205 closed:** the current-profile XMS owner package is routed and
> source-built, with C3/C5-gated functionality honestly deferred rather than
> fabricated. See the [closure record](history/m0-t205-closure-20260815.md).

> **T206 S1 closure:** original `54:0B` is currently blocked by an incorrect
> `0x11` adapter gate; even a repaired return remains package-local until a
> later engine/CLI ABI exists. See the [source/ABI map](etc/research/t206-s1-one-shot-normal-terminal-source-abi-map-001.md).

> **T206 S2 closure:** exact `54:0B` routing now creates a versioned copied
> COMMAND terminal record only at declared-plan exhaustion; the strict x64
> `/MT` package regression covers one/two-slot return, repeat, sentinel and
> low-DX behavior. This remains package-local, not engine or CLI completion;
> see the [handoff record](etc/research/t206-s2-command-terminal-handoff-001.md).

> **T204 S1 closure:** the declared-profile map identifies the current
> `minimal-comment-v1` / empty boot files and `54:0C/0D` C:-path answers as one
> synthetic contained-profile workaround, not historical OpenNT configuration
> closure.  Original transformation, `5E` owner actions and the fatal `50:3D`
> disposition are separately owned.  See the [startup configuration map](etc/research/t204-s1-contained-guest-startup-configuration-map-001.md).

> **T204 S2 closure:** one fixed-width copied configuration-source policy now
> takes its mode only through the common COMMAND session-context registration;
> it performs no host or guest action.  See the [ABI evidence](etc/research/t204-s2-startup-configuration-policy-abi-001.md).

> **T204 S3 closure:** original selection, transform, merge, publication,
> cleanup and fatal contracts now map to one session-owned contained provider;
> normal user-mode APIs are available but temporary-file composition is
> deliberately rejected.  See the [source contract](etc/research/t204-s3-configuration-transform-provider-source-contract-001.md).

> **T204 S4 closure:** one copied, session-owned provider now transforms the
> admitted CONFIG/AUTOEXEC contract and yields bounded images/merge state with
> no external I/O.  See the [core evidence](etc/research/t204-s4-contained-configuration-provider-core-001.md).

> **T204 S5 P1 closure:** the provider owns paired generated image bytes
> in the composable namespace and COMMAND copies its paired paths before bind.
> The later S5 installation path removes the synthetic initializer; this P
> alone does not claim default-route replacement. See the
> [integration record](etc/research/t204-s5-configuration-image-path-integration-001.md).

> **T204 S5 P3 closure:** CLI admits only paired configuration-source options
> and carries them through a child-only environment without reading source
> bytes. A fresh strict MSVC x64 `/MT` runner/probe exercise proves the pair
> originates in the supplied CLI options, alongside the profile/root, drive
> policy and versioned launch plan. See the [CLI admission
> record](etc/research/t204-s5-cli-paired-source-admission-001.md).

> **T204 S5 P4 closure:** adapter installation reads paired sources once,
> copies bounded OEM input and binds the ready provider; strict x64 `/MT`
> reader and runtime-installation regressions pass (including explicit empty
> AUTOEXEC, source-file deletion after install and checked `54:0C` path
> preparation). The retained profile descriptors now declare only paired paths
> and metadata before bind; they materialize no startup bytes. See the
> [reader record](etc/research/t204-s5-adapter-installation-reader-001.md).

> **T204 S5 P5 closure:** readonly namespace initialization retains only
> declared CONFIG/AUTOEXEC paths and metadata; it no longer installs fixed
> synthetic boot bytes.  The ready provider remains the sole byte owner after
> bind. The direct namespace fixture now proves unbound declared boot files
> have zero content, while the bound-provider fixture proves explicit content;
> the targeted strict x64 `/MT` regressions pass. A fresh full
> configuration-integration object closure also compiles, links and runs under
> MSVC x64 `/MT` after synthetic-byte removal. Together with P3/P4, this
> satisfies S5's configuration-boundary exit evidence; T204 remains active
> until its final governance handoff is committed and pushed.

> **T203 S1 correction:** `54:11` is `SVC_CMDLASTSVC`, an original dispatcher
> sentinel, not a callable normal-return BOP. The normal `cmdReturnExitCode`
> target is `54:0B`; see the [selector source map](etc/research/t203-s1-command-terminal-selector-source-map-001.md).

> **T203 S1 observation correction:** the fresh MSVC x64 `/MT` source build
> compiled and linked, but its run exit `2` is the inherited fixture's failed
> generic-UD diagnostic assertion—not valid evidence that `54:0B` was or was
> not reached. The engine still has no implementation that produces
> `ORDINARY_GUEST_COMPLETION`; no terminal-result ABI is admitted. See the
> [validity correction](etc/research/t203-s1-normal-terminal-observation-validity-correction-001.md).

> **T203 S1 harness map:** no existing engine or native fixture can observe
> normal `54:0B` without either requiring a controlled `54:01` stop or
> converting an unrelated generic fault into one. A later admitted observer
> must be default-off, passive and copied-data-only; see the
> [harness map](etc/research/t203-s1-normal-return-observation-harness-map-001.md).

> **T203 S1 passive observation:** a fresh full-manifest MSVC x64 `/MT` CPU5
> run now uses a default-off copied `54:0B` resume observer and records no
> normal return before the existing `54:01` declared-plan STOP. This is a
> stronger native non-observation, but the finite-run fixture is not the
> engine/CLI lifecycle and therefore admits no terminal-result ABI. See the
> [observation record](etc/research/t203-s1-passive-normal-return-observation-001.md).

> **T203 S1 engine admission boundary:** `ORDINARY_GUEST_COMPLETION` has no
> current engine producer, and `54:0B` records only package-local copied state.
> A refreshed engine fixture now proves one product-accepted `50:3D` STOP and
> cleanup across two runs under an explicitly selected bounded environment;
> this is neither `54:01` nor `54:0B`, and permits no ABI fabrication. See the
> [engine admission boundary](etc/research/t203-s1-engine-terminal-admission-boundary-001.md).

> **T200 closed:** T200 delivers a safe global BOP admission audit and route
> replan, not BOP execution closure. See the [T200 closure record](history/m0-t200-closure-20260814.md).

> **T201 S1 closure:** source-built MSVC x64 `/MT` evidence proves the common
> session mode and owner-enforcement ABI, not host behavior; see the
> [ABI evidence](etc/research/t201-s1-mutation-profile-abi-001.md).

> **T201 S2 closure:** source-built MSVC x64 `/MT` evidence proves a bounded,
> session-owned overlay lifecycle with teardown, not an overlay backend; see
> the [lifecycle evidence](etc/research/t201-s2-mutation-overlay-lifecycle-001.md).

> **T201 closed:** common profile selection, owner enforcement, overlay
> lifecycle and four-mode policy resolution are complete; see the
> [T201 closure record](history/m0-t201-closure-20260814.md).

> **T202 S7 decision:** the complete original `cmdExec32` /
> `cmdReturnExitCode` / `cmdGetNextCmd` lifecycle map rejects a new
> CLI-owned host-child-to-DOS queue for this profile. `GetNextVDMCommand` is
> the unavailable historical BaseSrv/CSR broker, not a local command list;
> the retained bounded child transaction is therefore terminal-only. See the
> [admission decision](etc/research/t202-s7-command-reentry-admission-decision-001.md).

> **T202 S1 closure:** DEM now consumes the shared profile through one copied,
> selector-blind package seam; see the
> [composition evidence](etc/research/t202-s1-dem-profile-consumer-composition-001.md).

> **T202 S2 closure:** direct-host namespace/CWD/volume is closed at its
> admitted boundary, including source-shaped drive failure and legacy fallback
> removal; see the [closure evidence](etc/research/t202-s2-dem-namespace-cwd-volume-closure-001.md).

> **T202 S3 progress:** the complete 29-identity DEM file/handle/FCB/search
> boundary is inventoried by original owner, host API and current workaround,
> with checked range/token, search continuation and hard-error result rules;
> this is admission evidence, not provider implementation. See the
> [source/ABI map](etc/research/t202-s3-dem-file-handle-fcb-search-source-abi-map-001.md).

> **T202 S3 closure:** all 29 identities have exact source/ABI/failure and
> workaround dispositions. It admits S4's whole provider recovery; it does
> not claim a recovered provider or a native trace. See the
> [admission closure](etc/research/t202-s3-dem-file-handle-fcb-search-admission-closure-001.md).

> **T202 S4 progress:** the adapter-private opaque file-token session now
> source-builds under x64 `/MT`; its first focused lifecycle probe proves
> stale-token rejection without exposing a `HANDLE`. This is provider
> substrate only, not a completed BOP. See the
> [token evidence](etc/research/t202-s4-dem-file-session-token-001.md).

> **T202 S4 progress:** an admitted-root-relative local-file open now reaches
> a real host file without ambient path or handle exposure; `..` is rejected.
> This is a provider primitive, not a BOP completion. See the
> [open evidence](etc/research/t202-s4-host-namespace-root-relative-open-001.md).

> **T202 S4 progress:** a shared bounded OEM DOS-path resolver now selects
> CWD/default-drive and rejects dot or long-name escape before host access.
> It is provider substrate only; see the [path evidence](etc/research/t202-s4-dem-path-resolution-001.md).

> **T202 S4 progress:** one package-local local-file backend now composes the
> admitted root, bounded DOS path and opaque token session. Its focused MSVC
> x64 `/MT` regression proves direct and readonly reads, profile-enforced
> readonly write refusal, and explicit overlay/virtual non-backend results;
> it is provider substrate only, not a completed BOP. See the
> [backend evidence](etc/research/t202-s4-dem-local-file-backend-001.md).

> **T202 S4 plan:** the complete one-provider state, transaction, profile,
> partition and regression model is now recorded before legacy routes are
> replaced. It identifies `demClientErrorEx` hard-error ownership as a single
> package limitation rather than an endpoint exception; see the
> [whole-provider plan](etc/operations/t202-s4-dem-whole-provider-implementation-plan-001.md).

> **T202 S4 progress:** a single DEM whole-provider session now owns the S3
> 29-identity membership and profile/root/token/backend lifetime under MSVC
> x64 `/MT`. This is membership and state closure only, not BOP dispatch or
> service completion; see the [session evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S4 integration observation:** the atomic provider composition now
> gives `50:47/48` an explicit Redirector-deferred `AX=6`/CF terminal, rather
> than a readonly-fallback route. A refreshed current-source CPU5 native
> fixture compiles and links under MSVC x64 `/MT`, then records one bounded run
> (exit `2`, first canonical BOP `50:11`). This is integration evidence only:
> it neither selects a new BOP task nor claims DEM, hard-error, overlay, or
> native-guest closure. See the [session evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S5 progress:** the complete COMMAND `54:00..10` source/ABI/mutation
> audit distinguishes ordinary modern user-mode APIs (available) from the
> historical CCPU/SAS, VDD, raw-handle and VDM re-entry composition gaps. It
> is admission evidence only; no `54:xx` implementation or trace was created.
> See the [COMMAND capability map](etc/research/t202-s5-command-host-capability-source-abi-map-001.md).

> **T202 S5 completion candidate:** the atomic COMMAND ownership and complete
> package regression design now separate ordinary API use from the three true
> composition boundaries: raw-handle streams, VDM child lifecycle and VDD
> console/KEYB. It is ready for closure/admission review; see the
> [whole-provider plan](etc/operations/t202-s5-command-whole-provider-regression-plan-001.md).

> **T202 S6 progress:** all callable `54:00..10` identities now enter the
> same COMMAND package session before their no-op, terminal or deferred
> disposition is emitted. The refreshed host-installed MSVC x64 `/MT`
> composition fixture passes. This is an atomic owner-routing baseline, not
> stream, child-lifecycle, VDD/KEYB or native-guest closure.

> **T202 S6 progress:** COMMAND now consumes a copied, selector-blind shared
> mutation profile for `SESSION_CONTEXT` and `HOST_GLOBAL` capability classes.
> The current runtime installs both registrations; the x64 `/MT` composition
> regression proves their direct-policy resolution without adding host I/O,
> handles, pointers or BOP-specific policy. This is capability substrate only.

> **T202 S6 progress:** COMMAND now owns a fixed-width copied current-directory
> context captured once at CLI composition admission. `54:04` consumes it via
> the package's checked RAM transaction: the selected host drive receives the
> copied OEM directory, another admitted drive receives its source-shaped
> root, and an unadmitted drive retains `AX=0, CF=1`. The MSVC x64 `/MT`
> bound-package regression passes; this is one context-capability foundation,
> not complete COMMAND environment/lifecycle closure or a native trace. See
> the [host-context evidence](etc/research/t202-s6-command-host-context-001.md).

> **T202 S6 progress:** COMMAND's copied host context now contains a bounded,
> double-NUL OEM initial-environment snapshot. `54:02`, `54:0F`, and `54:01`
> consume one COMSPEC/environment/CMDINFO contract with source-shaped
> filtering, prompt default and paragraph retry; no BOP reads ambient process
> environment. The MSVC x64 `/MT` bound-package regression and runtime-source
> compilation pass. This advances only the bootstrap/context slice, not full
> COMMAND or native-guest closure; see the [environment evidence](etc/research/t202-s6-command-environment-snapshot-001.md).

> **T202 S6 progress:** `54:09`, `54:0E`, and `54:10` now share one
> session-owned console capability with the source-shaped no-console/no-KEYB
> disposition. The MSVC x64 `/MT` bound-package regression passes; this does
> not claim VDD, interactive console, keyboard installation, streams or
> native-guest closure. See the [console evidence](etc/research/t202-s6-command-console-capability-001.md).

> **T202 S6 progress:** `54:0C/0D` now consume one COMMAND-owned copied
> boot-input record rather than borrowing mutable DEM/boot-namespace state at
> dispatch time. A bound x64 `/MT` regression mutates the latter after binding
> and still proves the selected CONFIG/AUTOEXEC guest-path results, with no
> host temporary or system-file operation. This closes only the default
> boot-input pathname boundary, not host-backed expansion, streams, lifecycle
> or native-guest closure; see the [boot-input evidence](etc/research/t202-s6-command-boot-input-001.md).

> **T202 S6 progress:** one COMMAND execution-lifecycle provider now owns
> `54:07/08/0A/0B`. The declared DOS binary and typed return paths retain their
> source-shaped results; the two external-execution paths now return a checked
> `CF=0, AL=ERROR_NOT_SUPPORTED` deferred outcome rather than a false machine
> stop. The x64 `/MT` common-ingress regression passes. No child process,
> raw handle, VDD or VDM re-entry is claimed; see the [lifecycle evidence](etc/research/t202-s6-command-execution-lifecycle-001.md).

> **T202 S6 admission detail:** `54:06`, `54:08` and `54:0A` are one pending
> COMMAND stream-and-child capability, not three trace-selected endpoints.
> Their original packed ABI, shared `HOST_GLOBAL` profile disposition,
> opaque-token boundary, explicit unsupported modes and typed-lifecycle
> prerequisite are recorded in the [capability plan](etc/operations/t202-s6-command-stream-child-capability-plan-001.md).  Ordinary Win32 APIs are
> available; the remaining prerequisite is a replacement lifecycle contract,
> not a missing API.

> **T202 S6 progress:** the first stream-and-child substrate is now one
> session-private COMMAND stream table.  Under the shared direct
> `HOST_GLOBAL` grant, `54:06` returns only a generation-bound opaque token
> in its historical register shape; it never returns a `HANDLE`.  The MSVC
> x64 `/MT` 17-service common-ingress regression passes.  Child execution,
> guest `STD_HANDLES` gathering, VDD/VDM broker behavior and native-guest
> closure remain unclaimed; see the [stream-token evidence](etc/research/t202-s6-command-stream-token-001.md).

> **T202 S6 progress:** the same COMMAND provider now gathers the original
> 12-byte `54:08` `STD_HANDLES` shape from checked `SS:BP` guest RAM and
> validates its copied stderr/stdout/stdin token order without retaining a
> guest address.  The x64 `/MT` regression passes.  This is input substrate:
> `54:08` still returns the existing deferred lifecycle result and no child
> is launched; see the [gather evidence](etc/research/t202-s6-command-stream-gather-001.md).

> **T202 S6 progress:** CLI composition now captures a bounded host COMSPEC
> equivalent once into copied COMMAND context for the future `54:0A` path;
> the ambient general environment remains filtered and no BOP recaptures it.
> x64 `/MT` fixture and runtime-translation-unit compilation pass.  This is
> an input capability only: no child process or execution closure is claimed;
> see the [processor-context evidence](etc/research/t202-s6-command-processor-context-001.md).

> **T202 S6 progress:** direct-profile COMMAND child execution now has one
> typed terminal transaction for `54:08/0A`: checked copied command/
> environment inputs, opaque stream validation where applicable, private
> duplicated inherited handles, wait/close, and source-shaped CF-clear exit
> result.  The x64 `/MT` regression proves `cmd.exe /c exit 7` returns
> `AL=7`.  It has no VDM re-entry queue, VDD, `SetStdHandle`, raw handle/PID
> transport or native trace claim; see the [typed-child evidence](etc/research/t202-s6-command-typed-child-lifecycle-001.md).

> **T202 S6 integration observation:** one current-source CPU5 native run
> rebuilt and linked the entire manifest, then retained its existing DEM
> frontier (`50:11` and subsequent DEM lifecycle) with fixture exit `2`; no
> `54:xx` identity was reached.  This is negative reach evidence only and
> creates no endpoint task; see the [native observation](etc/research/t202-s6-command-native-integration-observation-001.md).

> **T202 S6 package ledger:** all 17 `54:00..10` identities now have one
> COMMAND-session owner and one implemented or source-shaped deferred result.
> The remaining child re-entry, Redirector pipe and interactive VDD/KEYB
> boundaries are explicit package limitations, not unowned BOPs; see the
> [closure ledger](etc/research/t202-s6-command-package-closure-ledger-001.md).

> **T202 S4 progress:** the whole-provider now owns one checked copied gather
> continuation with exact-boundary completion and stale/concurrent refusal;
> r7 source-builds under MSVC x64 `/MT`. It is common transaction substrate,
> not a BOP implementation; see the same [session evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S4 progress:** the DEM whole-provider now contains one complete
> source-derived local-handle partition (`50:00/02/08/16/1E/27`). Its r10
> MSVC x64 `/MT` regression uses a real host file while retaining opaque
> tokens, and covers seek/read/write/truncate/file-and-device-time/commit/
> close plus stale-token rejection. It is not an ingress route, does not
> close the 29-identity provider, and leaves VDMREDIR pipe specialization and
> historic hard-error ownership explicit; see the [session evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S4 progress:** the selector-blind admitted-root open primitive now
> retains a copied Win32 failure code for the namespace/metadata provider;
> r3 root-open and r12 whole-provider regressions pass under MSVC x64 `/MT`.
> This is shared host-capability plumbing, not a BOP dispatch or route switch;
> see the [root-open evidence](etc/research/t202-s4-host-namespace-root-relative-open-001.md).

> **T202 S4 progress:** the same root-relative boundary now supplies copied
> file attribute query/set and deletion primitives, with r4/r13 source-built
> MSVC x64 `/MT` coverage. They are namespace/metadata substrate only: the
> whole provider has not yet received a BOP ingress route or claimed this
> partition complete; see the [root-open evidence](etc/research/t202-s4-host-namespace-root-relative-open-001.md).

> **T202 S4 progress:** root-relative directory create/remove and same-drive
> file rename now join the namespace substrate. r8/r14 MSVC x64 `/MT`
> regressions pass; rename retains the root-relative invariant through the
> available `NtSetInformationFile` user-mode form rather than an ambient
> absolute-path fallback. This remains shared provider groundwork, not a BOP
> route or a namespace/metadata completion claim; see the [root-open evidence](etc/research/t202-s4-host-namespace-root-relative-open-001.md).

> **T202 S4 progress:** the nine-identity namespace/metadata group now has
> one source-built provider skeleton that centralizes membership, copied path
> resolution and explicit mutation-class profile selection. r15 proves source
> closure only; it is not an ingress route or service-completion claim; see
> the [whole-provider evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S4 progress:** the same nine-identity group now has one
> source-derived direct-host namespace/metadata partition beneath the whole
> provider. r21 verifies its copied-path/opaque-token operation forms against
> self-created host objects, including a no-side-effect `50:44` directory
> check. It is one source partition only—not full DEM, BOP ingress, overlay
> support, or native guest closure; see the [whole-provider evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S4 progress:** r22 additionally proves this partition never turns a
> readonly or presently unbacked overlay profile into a direct-host mutation.
> This is a profile boundary regression, not overlay implementation or whole
> DEM closure; see the same [whole-provider evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S4 progress:** r25 adds the unified local-handle `demfcb.c` partition
> for `50:2C/2D/2E/2F/30/31`, with opaque-token, FCB register-layout and
> source-built regression evidence. It deliberately leaves DTA/SRCHBUF search
> state and FCB wildcard namespace work to the remaining search/FCB partition;
> it is not a BOP route or whole DEM closure.

> **T202 S4 progress:** r12 proves real admitted-host pathname first/next and
> FCB-first search through a self-created directory. The recovered directory
> validation right fixes a false access denial without broadening mutation
> authority; guest ingress remains unconnected.

> **T202 S4 progress:** r26 moves the verified search transaction into the
> single DEM provider lifecycle under x64 `/MT`. This is ownership closure for
> the search state, not ingress routing or full DEM closure.

> **T202 S4 progress:** r27 adds one provider-owned `demsrch.c` partition for
> all four `50:09/0A/0B/0C` pathname/FCB first/next forms. Its dedicated r3
> x64 `/MT` regression covers copied DTA/SRCHBUF gathering, direct admitted-host
> enumeration, and continuation results; BOP ingress and native trace remain
> deliberately unconnected.

> **T202 S4 progress:** the DEM package session now constructs and tears down
> that provider with its admitted host namespace, and its four-service route
> is selected together ahead of the legacy boot snapshot. The retained r2
> composition closure passes under MSVC x64 `/MT`; a host-installed route
> regression is still required before asserting behavioral migration.

> **T202 S4 progress:** r28 adds exact-continuation cancellation for a
> declined mechanical search gather, with a provider regression proving that a
> later gather remains admissible. This is failure-lifecycle protection, not a
> host-installed route completion.

> **T202 S4 progress:** r7 now proves the host-installed four-service search
> route through ordinary BOP ingress and checked RAM: `50:09→0B` and
> `50:0A→0C` return two self-created admitted-host entries in order. A retained
> guest-physical resolved-DTA continuation fixes the first/next state gap;
> r30 whole-provider and r4 search-partition closures also pass under MSVC x64
> `/MT`. This closes only this four-service route migration, not full DEM or a
> native guest trace.

> **T202 S4 progress:** the existing session-owned `demsrch.c` search
> transaction now admits a root-relative direct-host first-search backend and
> remains source-built in the retained x64 closure. Dedicated direct-host
> search regression and the guest DTA/SRCHBUF route remain required before
> this can be treated as a completed partition.

> **T202 S4 progress:** r31 routes the complete local `demhndl.c` group
> (`50:00/02/08/16/1E/27`) through the DEM whole-provider for only its own
> opaque tokens. Its source-built MSVC x64 `/MT` host-installed regression
> exercises seek/read/write/file-time/commit/close through ordinary BOP
> ingress and checked RAM, while proving that retained legacy tokens stay with
> their former owner during the atomic-package migration. This is one route
> migration evidence block, not whole DEM, profile, hard-error, Redirector or
> native-trace closure; see the [whole-provider evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S4 progress:** r32 corrects readonly/overlay/virtual profile
> disposition for the existing local `demfcb.c` handle subset
> (`50:2C/2D/2E/2F/30/31`): readonly read-open is admitted, readonly create
> returns `AX=5,CF=1`, and overlay create remains explicit unavailable. This
> is provider-internal profile evidence only; wildcard `50:07/20`, full FCB
> record transactions, BOP routing and DEM closure remain pending. See the
> [whole-provider evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S4 progress:** r33 adds provider-internal `demfcb.c` wildcard
> namespace handling for `50:07/20`: bounded 8.3 inputs, admitted-root
> enumeration, normal/extended deletion, `?` rename substitution and explicit
> profile disposition. The r9 MSVC x64 `/MT` fixture proves direct host
> mutation only where admitted, cross-drive rejection, and no host mutation in
> readonly/overlay/virtual modes. It is not an ingress route, a final FCB/path
> package switch, or FCB/DEM closure; same-name rename still needs a dedicated
> short-name witness. See the [wildcard plan](etc/operations/t202-s4-dem-fcb-wildcard-provider-plan-001.md).

> **T202 S4 progress:** r34 adds the source-derived `50:2F demFCBIO` DTA
> transaction. It may route only when the whole provider issued the opaque
> AX:BP token and a checked DTA registration exists; all data movement is a
> typed guest-RAM action. r5 whole-provider and the retained host-installed
> MSVC x64 `/MT` fixture pass. This is a conditional FCB transition, not the
> final pathname/FCB switch, full FCB evidence, DEM closure or native trace;
> see the [whole-provider evidence](etc/research/t202-s4-dem-whole-provider-session-001.md).

> **T202 S4 correction:** direct `demfcb.c` ABI review found and corrected
> copied result-register layouts for `2C/2D/2F/30/31` (including `2F` byte
> count in `CX`, not `BX`). r2 locks those layouts in the source-built
> whole-provider fixture. This is an FCB ABI correction only; it does not
> expand host capability or alter the package-transition limits above.

> **T202 S4 correction:** direct `demfile.c` ABI review likewise corrects
> `01` attribute return (`CX`), local `12` size/pipe return (`BX` high, `CX`
> low, `DX=0`) and `44` success (`DX=0`). r1 locks those source layouts in the
> whole-provider fixture. This remains provider-internal and does not admit
> partial pathname ingress ahead of the final package switch.

> **T202 S4 progress:** r37 adds one provider-internal pathname transaction
> for all nine `demfile.c`/`demdir.c` identities. It preserves original
> DS:DX/DS:SI/ES:DI input selection and makes `demRename`'s two bounded
> `MAX_PATH` strings explicit; generic gather ABI v2 raises its ceiling only
> to the required 520 bytes. r1 source-builds under MSVC x64 `/MT`. This is
> preparation for one final pathname package switch—not an individual BOP
> route or native trace.

> **T202 S4 progress:** r38 expands that transaction's source-built matrix to
> all nine pathname identities, including `17`'s two-record shape and a
> no-NUL cleanup refusal. It is still provider-internal transaction evidence,
> not partial pathname routing or a package-switch claim.

> **T202 S4 progress:** r39 adds the five-service `demfcb.c` pathname
> transaction (`07/20/2C/2D/31`), including original DS/ES shapes, two-path
> rename and no-NUL cleanup. It delegates to the existing FCB providers only.
> This is still provider-internal preparation; no FCB pathname BOP route is
> enabled before the family-wide atomic gate.

> **T202 S4 progress:** r40 closes the non-direct mutation matrix for the
> namespace and FCB-wildcard write surface: all `01/03/04/05/06/17/22` and
> `07/20` cases refuse before host mutation under readonly/overlay/virtual,
> with retained source/profile error forms. This is profile evidence only, not
> an overlay backend, route migration or DEM closure.

> **T202 S4 progress:** r41 installs the atomic local DEM package switch:
> all 27 S3 local identities now select the whole provider, with no fallback
> from a new request/token into boot or readonly fixture leaves. Legacy tokens
> receive new-owner failure; `47/48` remain Redirector deferred. The r1
> whole-provider and r4 host-installed MSVC x64 `/MT` fixtures pass. This
> closes route migration only; hard-error ownership, an overlay backend and
> native guest execution remain open.

> **T202 S2 progress:** the first package primitive validates an admitted
> real-host DOS-relative directory beneath retained root handles without host
> path/handle exposure or ambient-CWD mutation; see the
> [validation evidence](etc/research/t202-s2-host-namespace-directory-validation-001.md).

> **T202 S2 progress:** DEM now has one session-private CWD/default-drive
> context governed by the shared direct/readonly/overlay/virtual profile;
> this is package infrastructure, not a current-directory BOP closure. See
> the [profile-context evidence](etc/research/t202-s2-dem-cwd-profile-context-001.md).

> **T202 S2 progress:** the real direct composition now installs the same
> shared DEM profile and CWD context before bind; complete source closure and
> pointer-free startup-plan regressions pass. See the
> [installation evidence](etc/research/t202-s2-dem-runtime-profile-installation-001.md).

> **T202 S2 progress:** `50:13`, `50:18`, and `50:1A` now share a
> source-mapped DEM CWD/default-drive service with checked guest-memory
> transactions and common profile outcomes; focused direct/readonly/overlay
> regression passes. See the [family evidence](etc/research/t202-s2-dem-cwd-family-service-001.md).

> **T202 S2 progress:** `50:0D` now selects an actual CLI-admitted boot
> drive and never synthesizes C: when it is excluded; see the
> [boot-drive evidence](etc/research/t202-s2-dem-boot-drive-admission-001.md).

> **T202 S2 progress:** `50:0F` now preserves sparse admitted DOS drive
> letters rather than hiding an admitted later drive behind an excluded gap;
> see the [GSET projection evidence](etc/research/t202-s2-dem-drive-projection-gset-001.md).

> **T202 S2 progress:** the GSET/DPB volume-observation group now maps an
> excluded drive to OpenNT's `demClientError`-shaped `AX=15, CF=1`, while
> preserving the original media-set CF-only branch; see the same
> [GSET evidence](etc/research/t202-s2-dem-drive-projection-gset-001.md).

> **T202 S2 progress:** obsolete root-only CWD/default-drive fallbacks are
> removed from both the runtime plane and composition closure; only the
> session-owned v2 family may answer those services. See the
> [CWD family evidence](etc/research/t202-s2-dem-cwd-family-service-001.md).

> **T200 S30 closure:** corrected real-host capability policy and startup-drive
> admission, bound the adapter-private real-root namespace to composition
> lifecycle, repaired its strict C11 source closure, and recorded the separate
> virtual boot-volume contract. The host-root fixture passed under MSVC x64
> `/MT`; see the [source/ABI map](etc/research/t200-s30-real-host-capability-source-abi-map-001.md).

> **T200 S9 closure:** exhausted `54:01` is accepted as typed stop in a native
> run with no raw #UD; see the [terminal observation](etc/research/t200-s9-command-terminal-native-observation-001.md).

> **T200 S10 closure:** the S9 process exit is a fixture assertion, not a
> product CLI or guest result.  Native CLI/engine composition is the next
> owner package; see the [terminal-owner boundary](etc/research/t200-s10-cli-terminal-owner-boundary-001.md).

> **T200 S11 closure:** the retained `bochs.exe` shell and product `main.cc`
> hook have a source-mapped replacement boundary: a C-only native engine
> contract in `bx-mantle`.  See the [ABI map](etc/research/t200-s11-native-engine-entry-abi-map-001.md).

> **T200 S12 closure:** the copied native-engine contract has a passing x64
> `/MT` validation fixture with no machine or guest dependency.  See the
> [contract evidence](etc/research/t200-s12-native-engine-contract-001.md).

> **T200 S13 closure:** the native-engine skeleton directly enters and resets
> bx-vdm composition through copied inputs, with no environment rewrite and no
> machine/guest execution.  See the [lifecycle evidence](etc/research/t200-s13-native-engine-lifecycle-skeleton-001.md).

> **T200 S14 closure:** the direct path binds and releases the real,
> source-built whole composition twice without environment mutation or machine
> execution.  See the [direct closure](etc/research/t200-s14-direct-composition-closure-001.md).

> **T200 S15 closure:** the native engine lifecycle map rejects the old
> product startup hook and fixture runner, and admits a direct-composition
> startup-plan provider.  See the [lifecycle map](etc/research/t200-s15-native-engine-machine-lifecycle-map-001.md).

> **T200 S16 closure:** direct composition owns its profile-selected NTIO image
> and exposes a pointer-free startup plan whose payload is invalidated by
> reset; see the [startup-plan closure](etc/research/t200-s16-direct-composition-startup-plan-closure-001.md).

> **T200 S17 closure:** OpenNT source locks the narrow NTIO handoff, but the
> minimal mantle has no bound IVT/BDA/virtual-interrupt input provider; see the
> [preentry map](etc/research/t200-s17-product-preentry-source-abi-map-001.md).

> **T200 S18 closure:** bx-vdm has a copied evidence-identified initial-state
> ABI with valid-but-unpublishable absent state; see the [ABI record](etc/research/t200-s18-initial-state-declaration-abi-001.md).

> **T200 S19 closure:** r18 stock-ROM/POST evidence is explicitly not the
> minimal-mantle reset state; see the [binding decision](etc/research/t200-s19-initial-state-evidence-binding-decision-001.md).

> **T200 S21 closure:** bx-vdm publishes the admitted external record only as
> a selector-blind mantle action. Its initialized-machine fixture proves both
> copied range application and all-range preflight; see the [publication
> closure](etc/research/t200-s21-initial-state-publication-closure-001.md).

> **T200 S22 closure:** native engine direct composition has a
> mantle-private mechanical preentry stage: external state, preserve, NTIO
> write, restore, and cleanup all pass in current x64 `/MT` source closure;
> see the [machine-stage closure](etc/research/t200-s22-native-engine-machine-stage-closure-001.md).

> **T200 S23 closure:** the active stage accepts only a checked real-mode
> CS:IP delta; `0070:0000` is copied back after arm and invalid EIP has no
> mutation. See the [entry arming closure](etc/research/t200-s23-real-mode-entry-arming-closure-001.md).

> **T200 S24 closure:** the new engine lifecycle admits a finite native CPU
> loop and reports typed budget/controlled-stop results. Its first
> source-built profile run reached controlled stop twice; see the [bounded
> execution closure](etc/research/t200-s24-bounded-native-execution-closure-001.md).

> **T200 S25 closure:** default-off bx-vdm observation captured the first
> accepted source-built engine STOP as COMMAND `54:01` at `95AB:03C1`; its
> source-backed exhausted-plan meaning does not close COMMAND or admit an
> endpoint patch. See the [terminal observation](etc/research/t200-s25-first-controlled-terminal-observation-001.md).

> **T200 S26 closure:** the OpenNT COMMAND startup, registration, CMDINFO,
> normal-repeat, shell-out-return, and exhausted-plan state map is reconciled.
> Current source-built package regressions and one bounded native engine run
> confirm that the observed `54:01` is the declared one-slot terminal, not a
> missing BOP. See the [lifecycle reconciliation](etc/research/t200-s26-command-initial-lifecycle-reconciliation-001.md).

> **T200 S27 closure:** the new MSVC x64 `/MT` native CLI constructs and calls
> the copied engine ABI directly for an admitted BYOB target; no legacy child
> engine, `bochs.exe`, or `NTDOS64_ADAPTER_*` environment transport is used.
> See the [handoff evidence](etc/research/t200-s27-native-cli-engine-handoff-001.md).

> **T200 S28 closure:** native CLI admission now rejects empty drive lists and
> foreign targets before engine entry; a validation-only route proves that
> conflicting include/exclude masks remain explicit for the snapshot layer's
> exclusion precedence. See the [regression evidence](etc/research/t200-s28-native-cli-admission-regression-001.md).

> **T200 S29 closure:** profile-owned guest C: and filtered host-drive
> projection are distinct ownership domains; current COMMAND output exposes
> their coupling without assigning the prior timeout's cause. See the
> [owner map](etc/research/t200-s29-excluded-drive-owner-map-001.md).

> **T200 S7/S8 closure:** original no-next-command behavior is a contained
> typed stop after the declared plan is exhausted; see the [S8 evidence](etc/research/t200-s8-command-exhausted-plan-stop-001.md).

> **T200 S6 closure:** the native guest follows ordinary `54:01` re-entry,
> not the repaired shell-out `54:0B` path; see the [observation](etc/research/t200-s6-command-lifecycle-native-observation-001.md).

> **T200 S5 closure:** `54:0B`, session dispatch and declared-plan lifecycle
> now pass x64 regression; see the [lifecycle evidence](etc/research/t200-s5-command-return-next-lifecycle-001.md).

> **T200 S4 closure:** its one allowed run proves the CMDINFO contract and
> assigns the decline to COMMAND return/next-command lifecycle; see the
> [native observation](etc/research/t200-s4-command-cmdinfo-native-observation-001.md).

> **T200 S3 closure:** the fixture-only CMDINFO surface compiled in the full
> x64 closure with `runs: 0`; see the [fixture record](etc/research/t200-s3-command-cmdinfo-observation-fixture-001.md).

> **T200 S2 closure:** source comparison proves `54:01` is a full CMDINFO
> lifecycle contract but S1 lacks the live inputs needed to identify its
> declined guard.  S3 is the bounded fixture-only observation surface; see the
> [diagnosis](etc/research/t200-s2-command-cmdinfo-native-frontier-diagnosis-001.md).

> **T200 S1 closure:** one source-built CPU5 native run reached controlled
> `54:01` after established DEM and machine-handoff traffic. It is a COMMAND
> package frontier, not a patch admission; see the [native baseline](etc/research/t200-s1-native-cli-integrated-baseline-001.md).

> **T199 closed:** the [global BOP audit](etc/research/t199-s50-global-bop-package-closure-audit-001.md) confirms every admitted family has one package owner/disposition and current source-built coverage. This does not claim unavailable/deferred host features are implemented; it authorizes T200's controlled native integration observation.

> **T199 S49 closure:** machine/BIOS BOP ownership is current: profile memory
> values and checked handoff remain bounded, while monitor and transition forms
> are typed stops. Fresh focused and bound x64 `/MT` runs pass; see the
> [machine revalidation](etc/research/t199-s49-machine-bios-package-revalidation-001.md).

> **T199 S48 closure:** the top-level host-service package has explicit CLI
> dispositions, including the corrected source-derived `5E` nonzero no-op
> continuation. Focused and bound x64 `/MT` fixtures pass; machine selectors
> remain with their owner package. See the [top-level revalidation](etc/research/t199-s48-top-level-host-service-package-revalidation-001.md).

> **T199 S47 closure:** `56h` is source-proven three-byte debugger ingress
> with stack-resident mode/arguments, and is a deferred typed stop for the CLI
> profile. Fresh x64 `/MT` composition evidence preserves a following guest
> byte; see the [Debugger revalidation](etc/research/t199-s47-debugger-package-revalidation-001.md).

> **T199 S46 closure:** all fifty Redirector slots retain their source-derived,
> bound-composition unavailable result (`RIP+4`, `CF=1`, `AX=1`); `57:32`
> remains rejected. A fresh x64 `/MT` fixture passes without a host network or
> filesystem capability; see the [revalidation](etc/research/t199-s46-redirector-package-revalidation-001.md).

> **T199 S45 closure:** all twenty-five DPMI slots have a source-mapped owner
> and typed explicit-unavailable disposition. A fresh x64 `/MT` global
> composition run proves each reaches controlled stop instead of raw `#UD`;
> no native trace ran. See the [DPMI map](etc/research/t199-s45-dpmi-owner-abi-failure-map-001.md).

> **T199 S44 closure:** all twelve XMS slots have explicit owner/disposition;
> a fresh x64 `/MT` family run proves admitted paths resume and all remaining
> paths stop rather than leak a raw #UD. See the [XMS map](etc/research/t199-s44-xms-owner-abi-failure-map-001.md).

> **T199 S43 closure:** all seventeen callable COMMAND slots now have one
> original-owner disposition and fresh x64 `/MT` bound-composition family
> evidence. It does not claim historical console/process/redirection recovery
> or native-run closure; see the [COMMAND ledger](etc/research/t199-s43-command-family-owner-disposition-ledger-001.md).

> **T199 S42 closure:** CMDINFO is now a source-mapped contained subpackage:
> `54:0F` environment capacity crosses to `54:01` as the original CF/AX retry
> contract, while command/executable and environment ranges remain separately
> checked. Fresh x64 `/MT` helper and bound-composition regressions pass; no
> native trace ran. See the [S42 CMDINFO map](etc/research/t199-s42-command-cmdinfo-source-abi-map-001.md).

> **T199 S41 closure:** its one admitted source-built CPU5 guest run compiled and
> linked the full current composition, completed established DEM traffic, and
> stopped at unhandled `54:01`. This is a precise COMMAND CMDINFO package
> frontier—not #UD failure or component closure—and creates no endpoint patch.
> See the [S41 observation](etc/research/t199-s41-command-native-integration-observation-001.md).

> **T199 S40 completion:** COMMAND entry lifecycle no longer treats absent
> `54:05` registration as a generic `54:01` decline. It completes the copied
> CMDINFO transaction without registration-only writes, while the registered
> `54:05 → 54:01` lifecycle remains covered by source-built session regression.
> See the [S40 source/ABI map](etc/research/t199-s40-command-entry-lifecycle-source-abi-map-001.md).

> **T199 S39 completion:** one source-built native guest execution reached
> the existing controlled `54:01` frontier after DEM package traffic.  The
> evidence assigns it to COMMAND bootstrap/launch/lifecycle as a complete
> component problem—specifically its prerequisite `54:05` registration and
> `54:01` CMDINFO lifecycle—not a follow-on endpoint patch.  See [S39 native
> baseline](etc/research/t199-s39-package-complete-native-integration-baseline-001.md).

> **T199 S38 completion:** the global selector ledger now covers each
> admitted BOP identity by its original owner, bx-vdm package, disposition,
> regression and runtime-composition member.  It found and repaired one
> composition omission: the normal CLI runtime had compiled but not bound the
> native XMS/DPMI session.  A fresh x64 `/MT` source-built composition fixture
> binds both sessions and proves `52:00` falls through the boot namespace to
> its native XMS owner.  This is package-routing evidence, not a guest trace
> or a claim that deferred capabilities are implemented; see the [global
> ledger](etc/research/t199-s38-global-bop-owner-composition-ledger-001.md).

> **T198 transfer fact:** S134's single authorized run stopped at `54:01`, not `54:04`. It is a non-closure observation retained in T198 history; it neither closes COMMAND nor admits a follow-on endpoint task.

> **T199 S4 completion:** the compiled DEM package facade now classifies all 73
> callable members plus sentinel, preserves seven original no-ops and the
> separate `50:42` exception, and gates retained providers without a trace;
> see [S4 regression](etc/research/t199-s4-dem-package-facade-regression-001.md).

> **T199 S30 completion:** DEM now has one source-built package session and
> one composition dispatch path for all 73 callable identities; existing DEM
> providers and `50:36` are session-gated without a trace.  See [S30
> regression](etc/research/t199-s30-dem-package-session-regression-001.md).

> **T199 S37 correction in progress:** `demdisp.c` slot audit is the
> controlling DEM identity source. It corrected an unaccepted clock-slot
> error and FastRead precedence before package closure; whole-family
> provider-decline leaks remain active implementation work, not trace work.
> See [canonical audit](etc/research/t199-s37-dem-canonical-dispatch-audit-001.md).

> **T199 S37 regression milestone:** an unbound DEM-only x64 `/MT` fixture
> now exercises every canonical `50:00..48` identity through the package
> session, with no raw pass-through result. It also distinguishes `50:02`'s
> original token-zero success from `50:1B`'s contained invalid-address
> failure. This is routing/containment evidence, not whole-business-semantics
> closure; no native trace was run. See [family regression]
> (etc/research/t199-s37-dem-package-family-regression-001.md).

> **T199 S37 namespace progress:** immutable namespace mutations `50:01`,
> `03-06`, `08`, `17-18`, and `22` now use a dedicated source-derived
> readonly-provider failure contract (AX=5, CF=1), rather than the common DEM
> fallback. The r8 full-family regression passes; no native trace was run.

> **T199 S37 FCB progress:** the FCB provider now retains `50:2E` null-handle
> close success and `50:30 demDate16` packed-time semantics; open/search/write
> FCB operations remain explicit contained failures pending a complete FCB
> capability. The r9 full-family regression passes; no native trace was run.

> **T199 S37 error/lock progress:** `50:32`, `50:33`, and `50:3F` now belong
> to the one DEM error/lock session plane: copied registration, no-retry
> AX=FF/CF failure, and contained lock failure respectively. The r10
> full-family regression passes; no native trace was run.

> **T199 S37 misc progress:** the non-debug, non-WOW, no-device CLI branch
> for `50:1D`, `23`, `34-3A`, and `3E` is now a source-derived misc-plane
> normal return, rather than generic failure. The r11 full-family regression
> passes; no native trace was run.

> **T199 S37 FCB/search admission:** the next package is defined as the
> `50:0A/0C` SRCHBUF search capability, with its shared FCB layout and
> per-PDB continuation state. Its source/ABI/failure map is in
> [FCB search design](etc/research/t199-s37-fcb-search-capability-design-001.md);
> no native trace admits or narrows it.

> **T199 S37 FCB/search BOP milestone:** the immutable profile-entry FCB
> first/next transaction is now attached to the DEM package's checked
> copied-memory path.  The r27 source-built fixture proves `50:0A` first,
> `50:0C` continuation, and exhausted `50:0C` no-more-files (AX=12, CF=1)
> over the profile's two `.COM` entries, plus a no-match `50:0A` search with
> the same AX=12/CF=1 and cleared continuation contract.  Malformed/foreign-
> token and complete FCB-family cases remain active S37 work; r29 also proves
> that a foreign-PDB continuation is rejected as AX=12/CF=1. No native trace
> was run.

> **T199 S37 raw-media/IOCTL progress:** r28 attaches `50:21`, `29`, and
> `2A` to one source-derived no-device provider after the admitted `50:21`
> metadata capability. Unsupported IOCTL returns the original
> `ERROR_INVALID_FUNCTION` AX=1/CF result; absolute read/write return the
> original no-BDS `DOS_DRIVE_NOT_READY` AX=21/CF result. No device, INT13,
> raw host drive, or native trace was introduced.

> **T199 S37 volume-capability foundation:** r31 source-builds a new
> adapter-local immutable host-volume snapshot. It captures only volume
> metadata for CLI-admitted drives during setup and retains no host path or
> handle; a deterministic regression proves non-admitted metadata is removed
> at that boundary. r32 attaches `50:0E demGetDriveFreeSpace`: its
> source-built package regression verifies the DOS-form AL/BX/CX/DX/SI result
> from an admitted immutable volume record. r34 attaches `50:10` get through
> checked guest-RAM writes of serial, volume label, and filesystem; its set
> branch retains the original CF-only unsupported result.

> **T199 S37 volume capability r35:** the full DEM family fixture now
> separately asserts `50:10` set's no-GPR-write/CF-only contract, and the
> formal boot-namespace composition exposes a pre-bind immutable-volume
> snapshot setter compiled in the same x64 `/MT` closure. No native trace was
> run.

> **T199 S37 DPB ABI correction:** source layout audit established that
> `50:46 demGetDPBList` uses a 35-byte packed `DPB` stride and writes `Next`
> at original offset 27, after `MediaID` and `FirstAccess`; prior 25-byte
> placement and 33-byte stride were both incorrect. `50:25` full-DPB
> disposition remains active work.

> **T199 S37 DPB provider r41:** `50:25 demGetDPB` now restores OpenNT's
> no-BDS fake-DPB success branch from an admitted immutable volume snapshot,
> including the original selective writes within the packed 35-byte record.
> An unadmitted drive reaches the documented source-derived AX=5/CF terminal
> result. Full DEM x64 `/MT` regression passes; no native trace was run.

> **T199 S37 computer-name disposition r42:** `50:41` now takes the original
> `GetComputerNameOem` failure branch because the CLI profile declares no
> stable guest identity: it writes NUL, clears only CH, and never queries the
> live host name. Full DEM x64 `/MT` regression passes; no native trace was
> run.

> **T199 S37 default-drive disposition r45:** `50:1A` now preserves
> `demSetDefaultDrive`'s checked first-byte `DL+'A'` guard and its
> AX=1/CF mismatch result. A matching path requires the historical NT-host
> current-directory operation, which this CLI profile does not admit, so it
> reaches an explicit AX=5/CF contained-capability terminal result instead of
> touching the host process directory. Full DEM x64 `/MT` regression passes;
> no native trace was run. See [default-drive disposition]
> (etc/research/t199-s37-dem-gset-default-drive-disposition-001.md).

> **T199 S37 FCB handle-family disposition r46:** `demfcb.c`'s delete,
> rename, create/open, I/O and file-info operations now name their shared
> missing NT-handle/writable-filesystem backend explicitly, each with AX=5/CF.
> The historical zero-handle close success remains separate. This is not FCB
> package closure: immutable search and layout/continuation edge cases remain
> active S37 work. Full DEM x64 `/MT` regression passes; no native trace was
> run. See [FCB handle-family disposition]
> (etc/research/t199-s37-dem-fcb-handle-family-disposition-001.md).

> **T199 S37 FCB search boundary r47:** the shared `50:0A` FCB-search
> provider rejects an aperture-crossing 128-byte pathname before any
> mechanical read or namespace query, then reaches the explicit FCB AX=5/CF
> result. Full DEM x64 `/MT` regression passes; no native trace was run.

> **T199 S37 miscellaneous lifecycle r48:** `50:3D demExitVDM` is verified
> as a typed controlled stop, with no synthetic guest resume, register delta
> or flags delta. This supersedes T198's then-current deferred-disposition
> statement without changing its historical fatal-path observation. Full DEM
> x64 `/MT` regression passes; no native trace was run.

> **T199 S37 working ledger:** all `50:00..48` identities now have a current
> owner/package disposition and shared next-step classification in the
> [working service ledger]
> (etc/research/t199-s37-dem-working-service-ledger-001.md). It is a control
> record for implementation, not a DEM-closure claim.

> **T199 S37 pre-trace coverage audit:** the seven source/ABI evidence
> families now mechanically cover each of the 73 callable `50:00..48`
> identities exactly once, with no generic raw pass-through in fresh r87
> all-DEM regression.  This is static package evidence, not a native-run or
> runtime-closure claim; see [pre-trace ledger]
> (etc/research/t199-s37-dem-pretrace-evidence-ledger-001.md).

> **T199 S37 native integration r94:** the current composition manifest now
> has a one-MSVC-environment build recipe and reaches one bounded CPU5 native
> guest run after the DEM audit.  The run records completed DEM transactions
> and stops under the fixture's existing controlled boundary at `54:01`; that
> COMMAND observation creates no endpoint task or runtime-closure claim.  See
> [native integration observation]
> (etc/research/t199-s37-dem-native-integration-observation-001.md).

> **T199 S37 closed:** the DEM whole-package provider implementation meets
> its identity, family-regression and single bounded integration-observation
> exit criteria.  Its retained closure is [S37 history]
> (history/m0-t199-s37-closure-20260814.md); the `54:01` stop is not a
> trace-led follow-on.

> **T199 S37 immutable-root CDS capability r52:** one adapter provider now
> serves `50:13`, `50:18`, and `50:1A` from CLI-admitted stable roots only.
> It repairs a 71-byte CDS to `X:\`, accepts set/default only for that same
> root, and retains explicit failure for all other paths or drives. It never
> reads or changes the host process CWD. Full DEM x64 `/MT` regression passes;
> no native trace was run.

> **T199 S37 DASD/IOCTL regression r53:** the all-DEM fixture now proves the
> admitted fixed-drive success contracts for IOCTL `44:08` and `44:09`, while
> ordinary IOCTL remains `AX=1/CF` and never gains a device backend. Full DEM
> x64 `/MT` regression passes; no native trace was run.

> **T199 S37 runtime-composition wiring r54:** successful adapter-runtime
> lifecycle now captures the admitted immutable volume snapshot, prepares the
> one DEM/COMMAND composition with its drive, volume and launch snapshots, and
> binds it before guest execution; reset unbinds it before releasing image
> state. It is deliberately separate from the legacy adapter runtime and is
> called only through the existing default-off pre-loop Bochs hook. MSVC x64
> `/MT` source compilation and the unchanged full DEM family regression pass.
> The current minimal Bochs recipe has not yet linked this expanded composition
> closure, so this is not a native CLI-run claim and no trace was run. See
> [wiring record](etc/research/t199-s37-runtime-composition-wiring-001.md).

> **T199 S37 native-core composition-link evidence r55:** a fresh MSVC x64
> `/MT` CPU5/Pentium-MMX bx-core/bx-mantle closure and all 94 current
> composition-provider objects now link together with no unresolved project
> symbols.  This is a static audit DLL only, not a CLI run or native trace;
> see [link-closure record](etc/research/t199-s37-native-core-composition-link-closure-001.md).

> **T199 S37 GSET family regression r60:** the immutable drive/volume state
> component now has a source-built x64 `/MT` two-drive regression covering
> boot drive, DOS geometry, drive count, full DPB, DPB-list stride/link, and
> bad guest range.  It corrected `50:46`'s final result from SI to BP in the
> fixed-width CPU delta; see [GSET regression](etc/research/t199-s37-dem-gset-family-regression-002.md).

> **T199 S37 GSET boundary regression r87:** the same component's fresh
> all-DEM x64 `/MT` fixture now proves unadmitted volume requests cannot read
> host volume state or write the caller buffer, and that date/time setters keep
> the original AL-only failure contract.  This closes the recorded GSET
> boundary sweep, not DEM or native runtime closure; see [GSET regression]
> (etc/research/t199-s37-dem-gset-family-regression-002.md).

> **T199 S37 namespace/FCB admission r61:** the next two DEM packages are
> governed by an original source-owner/ABI map.  The first namespace family
> regression proves all eight writable pathname operations reach one contained
> immutable-namespace AX=5/CF provider, not a generic fallback.  Readonly
> handle/search and FCB continuation work remains active; see [package map](etc/research/t199-s37-dem-namespace-fcb-source-abi-map-001.md).

> **T199 S37 FCB ABI correction r62:** `50:30 demDate16` now returns packed
> time through DX, matching `demfcb.c`, rather than the previously incorrect
> BX delta slot.  The complete DEM x64 `/MT` family fixture passes; FCB search
> continuation and layout closure remain active work.

> **T199 S37 MISC owner-routing correction r66:** source review and a bound
> composition regression repaired two unreachable original MISC providers:
> `50:11 demLoadDos` now copies only the admitted NTDOS image, and `50:3C
> demTerminatePDB` now releases profile-search state.  Composition and all-DEM
> x64 `/MT` fixtures pass; see [owner-routing record](etc/research/t199-s37-dem-misc-owner-routing-correction-001.md).

> **T199 S37 PDB-search lifecycle r67:** the bound composition now proves that
> a `50:3C` termination releases that PDB's existing search continuation: its
> subsequent continuation attempt is rejected.  This is still DEM component
> evidence, not a native guest trace.

> **T199 S37 lifecycle fixture r68:** the standalone `demTerminatePDB` fixture
> is now source-built, linked, and run under the corrected MISC/original-
> OpenNT contract, including matching-PDB release, foreign-PDB retention, and
> idempotence evidence.

> **T199 S37 FCB continuation boundaries r69:** the source-built x64 `/MT`
> all-DEM fixture now verifies `50:0A/0C` as one bounded FCB-search family for
> multi-entry, single-entry, exhausted, empty-token, altered-token and
> foreign-PDB continuations, as well as its retained checked-aperture boundary.
> An altered guest token reaches the source-shaped `AX=12`/`CF` no-more-files
> result; it is never a host handle.  This is FCB-family evidence, not a DEM
> closure or native trace; see [FCB search design]
> (etc/research/t199-s37-fcb-search-capability-design-001.md).

> **T199 S37 MISC routing correction r71:** whole-family source regression
> found and repaired `50:1D demSetV86KernelAddr`'s false V86-bridge
> classification.  Its original `demdisp.c` body is an ordinary debugger
> callout return, so it now joins the contained no-debug/no-VDD/no-WOW/no-
> device MISC result family without adding V86 or Bochs semantics.  The full
> x64 `/MT` DEM fixture passes; see [MISC routing record]
> (etc/research/t199-s37-dem-misc-family-routing-correction-001.md).

> **T199 S37 DASD/IOCTL family r73:** `50:21` now preserves the complete
> `demIoctlChangeable` 08/09 metadata slice, including AX=15/CF invalid-drive
> rather than misreporting it as an unsupported function.  The immutable
> snapshot also covers removable and remote forms; raw IOCTL and absolute I/O
> remain explicit no-device terminals.  The complete x64 `/MT` DEM fixture
> passes; see [DASD/IOCTL record]
> (etc/research/t199-s37-dem-dasd-ioctl-family-regression-001.md).

> **T199 S37 hard-error locator boundary r74:** the focused x64 `/MT`
> `demSetHardErrorInfo` fixture now proves both copied real-mode locator
> registration and rejection of an aperture-crossing locator.  The adapter
> retains no host pointer; `50:33/3F` keep their recorded no-retry/no-handle
> terminal contracts.  No native trace was run.

> **T199 S37 current family baseline r77:** a fresh current-worktree MSVC x64
> `/MT` build, link and execution of the complete 73-member DEM fixture pass
> after the MISC and DASD/IOCTL owner corrections.  It is source-built
> package-containment evidence only, not a native Bochs trace or a DEM closure
> claim; see [family regression]
> (etc/research/t199-s37-dem-package-family-regression-001.md).

> **T199 S37 handle-component correction r78:** source review of
> `demhndl.c` found that `50:27 demCommit` ignores `FlushFileBuffers` failure
> and clears CF.  Its contained readonly provider now preserves that exact
> typed completion without manufacturing a writable host handle; the complete
> 73-member x64 `/MT` family fixture passes.  No native trace was run.

> **T199 S37 pipe-family disposition r79:** `50:47/48` now explicitly reject
> an absent CLI pipe projection as `AX=6`/`CF=1`.  Original OpenNT calls them
> only through COMMAND's pipe table and projected NT handle; no pipe, event or
> host handle is synthesized.  The complete 73-member x64 `/MT` family fixture
> passes.  This is an unavailable-pipe contract, not pipe support or a native
> trace.

> **T199 S37 device-path disposition r81:** `50:44 demCheckPath` now restores
> OpenNT's self-contained `\\DEV\\` success branch through a checked copied
> guest path, with DX=0 and CF clear.  The original host `NUL` creation probe
> for other paths is deliberately excluded from the CLI profile, which returns
> DX=0 plus AX=5/CF instead.  The complete 73-member x64 `/MT` fixture passes;
> this neither creates a host device nor runs a native trace.

> **T199 S37 file-times disposition r84:** `50:08 demFileTimes` now returns
> immutable profile DOS time/date for an opened readonly handle, while its set
> branch returns AX=5/CF and never creates a writable host handle.  The focused
> and complete x64 `/MT` fixtures pass; no native trace was run.

> **T199 S37 N1 family baseline r85:** fresh paired x64 `/MT` package and
> readonly-file fixtures now regress the readonly namespace component as one
> provider family, including no-bypass routing and handle/path contracts.  It
> is component evidence only, not DEM closure or a native trace; see [N1
> regression](etc/research/t199-s37-dem-n1-readonly-namespace-family-regression-001.md).

> **T199 S37 S1 FCB/search baseline r85:** the current all-DEM fixture
> consolidates bounded profile search, opaque per-PDB continuations and the
> complete unavailable FCB-handle family.  It is not writable FCB support or
> a native trace; see [S1 baseline]
> (etc/research/t199-s37-dem-s1-fcb-search-family-baseline-001.md).

> **T199 S37 execution proposal:** the remaining DEM work is organized by
> complete original component package and final disposition, not trace order;
> see [DEM completion proposal]
> (etc/operations/t199-s37-dem-package-completion-proposal-001.md).

> **T199 S31 completion:** XMS now has one source-built package session/router
> for all twelve callable identities. Five source-derived operations use typed
> mantle capabilities; the remainder retain explicit package-deferred
> outcomes, with no trace. See [S31 regression](etc/research/t199-s31-xms-package-provider-regression-001.md).

> **T199 S32 completion:** DPMI now has one source-built package router for
> all 25 callable identities: `53:12` is controlled unavailable and every
> other identity is package-deferred without CPU/DOS semantics. See [S32
> regression](etc/research/t199-s32-dpmi-package-provider-regression-001.md).

> **T199 S33 completion:** the existing Redirector package route was rebuilt
> and rerun in the current composition: all fifty identities retain the one
> explicit unavailable result. See [S33 reverification](etc/research/t199-s33-redirector-composition-reverification-001.md).

> **T199 S34 completion:** the existing selector-only Debugger package route
> was reverified in composition with both three- and four-byte windows. See
> [S34 reverification](etc/research/t199-s34-debugger-composition-reverification-001.md).

> **T199 S5 completion:** the complete COMMAND table, ABI dependencies,
> sentinel safety rule, and partial endpoint evidence are in
> [S5 matrix](etc/research/t199-s5-command-source-abi-failure-matrix-001.md).

> **T199 S6 completion:** every COMMAND member has one package capability and
> failure disposition; see [S6 disposition](etc/research/t199-s6-command-whole-package-disposition-001.md).

> **T199 S7 completion:** COMMAND has a compiled package facade and all-service regression without a trace; see [S7 regression](etc/research/t199-s7-command-package-facade-regression-001.md).

> **T199 S29 completion:** COMMAND now has one source-built package session
> and one composition dispatch path for all seventeen callable identities;
> bounded providers, no-op, unavailable and deferred outcomes are verified
> without a trace.  See [S29 regression](etc/research/t199-s29-command-package-session-regression-001.md).

> **T199 S36 completion:** COMMAND now has no generic deferred callable
> route: every `54:00..10` identity is a bounded provider, original no-op or
> explicit unavailable result.  Its all-family x64 regression and a bounded
> CPU5 #UD integration witness both pass; see [S36 implementation](etc/research/t199-s36-command-whole-package-provider-implementation-001.md).

> **T199 S8 completion:** all XMS identities, original ABI and failure boundary are recorded in [S8 matrix](etc/research/t199-s8-xms-source-abi-failure-matrix-001.md).

> **T199 S9 completion:** XMS implementation is held behind four native machine capabilities; see [S9 disposition](etc/research/t199-s9-xms-whole-package-disposition-001.md).

> **T199 S10 completion:** all DPMI identities, original ABI and machine dependencies are recorded in [S10 matrix](etc/research/t199-s10-dpmi-source-abi-failure-matrix-001.md).

> **T199 S11 completion:** XMS and DPMI now share one selector-blind machine-capability roadmap; see [S11 plan](etc/research/t199-s11-shared-xms-dpmi-machine-capability-plan-001.md).

> **T199 S12 completion:** the Redirector map corrects the `57:31` omission,
> confirms 50 callable members and records the single unavailable-package
> disposition; see [S12 matrix](etc/research/t199-s12-redirector-source-abi-failure-matrix-001.md).

> **T199 S13 completion:** one common source-derived facade now returns the
> original unavailable Redirector result for all 50 members; see [S13
> regression](etc/research/t199-s13-redirector-unavailable-package-regression-001.md).

> **T199 S14 completion:** Debugger is mapped as a three-byte BOP plus a
> stack-mode protocol, not a fourth-byte service family; see [S14
> matrix](etc/research/t199-s14-debugger-source-abi-failure-matrix-001.md).

> **T199 S15 completion:** Debugger now has one selector-only deferred-stop
> facade, preserving byte four as guest code; see [S15
> regression](etc/research/t199-s15-debugger-deferred-package-regression-001.md).

> **T199 S16 completion:** the remaining top-level selectors have explicit
> original owners and boundary constraints; see [S16
> matrix](etc/research/t199-s16-top-level-bop-source-owner-matrix-001.md).

> **T199 S17 completion:** top-level selectors now have grouped provider
> dispositions and a no-bypass migration rule; see [S17
> disposition](etc/research/t199-s17-top-level-grouped-disposition-001.md).

> **T199 S18 completion:** one facade routes every admitted top-level selector
> through common ingress; focused and composition regression preserve the
> bounded terminal, idle, config, and machine-action results without a trace;
> see [S18 regression](etc/research/t199-s18-top-level-package-facade-regression-001.md).

> **T199 S19 completion:** all machine/BIOS identities now have an owner and
> disposition without selecting a historical `BIOS[]` branch; see [S19
> map](etc/research/t199-s19-machine-bios-source-abi-owner-map-001.md).

> **T199 S20 completion:** one machine facade now routes the bounded BIOS and
> machine handoff group after common ingress; see [S20
> regression](etc/research/t199-s20-machine-bop-facade-regression-001.md).

> **T199 S21 completion:** C1 A20 now has a source-backed native owner,
> selector-blind ABI design, and registered mantle exception; see [S21
> admission](etc/research/t199-s21-c1-a20-capability-admission-001.md).

> **T199 S24 completion:** C2 now provides a selector-blind bounded native
> aperture allocator with query/allocate/free/resize/move regression and a
> registered mantle exception; see [S24
> regression](etc/research/t199-s24-c2-extended-memory-regression-001.md).

> **T199 S25 completion:** C3 is explicitly deferred: the retained historical
> UMB map depends on NT/SoftPC product composition, while the no-device native
> machine has no source-proven allocatable UMB layout; see [S25
> map](etc/research/t199-s25-c3-umb-owner-abi-failure-map-001.md).

> **T199 S26 completion:** C5 is explicitly deferred: the reached XMS INT15
> path is a SoftPC keyboard/firmware callout contract, not a generic native
> interrupt facility; see [S26
> map](etc/research/t199-s26-c5-int15-owner-abi-failure-map-001.md).

> **T199 S27 completion:** C4 is explicitly deferred: OpenNT DPMI descriptor
> and protected-mode operations are a coupled LDT/session component, not a
> safely exportable Bochs CPU API; see [S27
> map](etc/research/t199-s27-c4-protected-mode-owner-abi-failure-map-001.md).

> **T199 S28 completion:** all 37 XMS/DPMI callable identities now have one
> reconciled whole-package disposition (36 deferred and `53:12` unavailable),
> verified by a source-built x64 facade regression; see [S28
> reconciliation](etc/research/t199-s28-xms-dpmi-whole-package-reconciliation-001.md).

## Current Technical Baseline

- T95 is closed by owner-authorized reprioritization. Its incomplete file/search
  capability and full real-path closure remain uncompleted.
- M0 T197 S6 has a reproducible CPU5/non-x86-64 native link frontier: r4
  compiles the explicit SIM/mantle/memory and source-proven reset/decode CPU
  candidates, then stops at the original complete instruction-handler table
  and retained product-shell methods.  No GUI, plugin/device archive, adapter
  or OpenNT input entered the probe; see
  [T197 S6 link probe 001](etc/research/t197-s6-cpu5-minimal-link-probe-001.md).
- M0 T197 S6 r11 now source-builds and links the whole CPU5 adopted core with
  the finite mantle, without a Bochs product target.  This is a native link
  closure only; machine initialization/execution still requires separate
  lifecycle verification.
- M0 T197 S6 r12 verifies the native bare-machine lifecycle in process:
  `initialize(1 MiB, 1 MiB) -> CPU5 reset/APIC/CPUID -> cleanup()` returns
  zero.  No instruction-loop, BIOS/reset image, BOP or guest-running claim is
  made by that lifecycle witness.
- M0 T198 S1 finds the retained BOP runtime is a historical product derivative:
  it retains `main.cc`, GUI/device archives and product configuration.  It is
  not link-compatible with the finite mantle.  The first second-stage closure
  is therefore a selector-blind native execution contract, not a per-BOP
  implementation; see
  [T198 S1 composition map](etc/research/t198-s1-bop-runtime-composition-map-001.md).
- M0 T198 S2 is complete: native CPU and PC-time sources prove a
  mantle-private reset/copy/entry/run contract.  Its bound is a finite
  instruction-tick budget rather than an exact retired-instruction counter;
  the existing product `benchmarkTimer` is rejected because it also mutates
  GUI exit state.  See
  [T198 S2 execution-contract map](etc/research/t198-s2-native-execution-contract-map-001.md).
- M0 T198 S3 is complete: r8 source-builds and runs neutral `HLT` bytes
  through CPU5 reset, checked RAM copy, real-mode entry, 64-tick native stop
  and cleanup.  The needed A20 initialization is now a finite mantle
  lifecycle invariant; no guest/BOP runtime claim follows.  See
  [T198 S3 finite-run witness](etc/research/t198-s3-finite-native-run-witness-001.md).
- M0 T198 S4 is complete: the current `#UD` seam map separates retained
  copied-state/resume mechanics from legacy `bx-vdm` dispatch, raw callback,
  environment and product-lifecycle coupling.  S5 will extract one fixed
  core/mantle bridge before any BOP plane attachment; see
  [T198 S4 generic #UD seam map](etc/research/t198-s4-generic-ud-seam-map-001.md).
- M0 T198 S5 P1 is complete: mantle now owns a standalone fixed generic-UD
  copied record and default-decline bridge, independently MSVC/x86 compiled
  with no adapter/product vocabulary or dependency.  Core has not yet been
  switched; see
  [T198 S5 P1 bridge record](etc/research/t198-s5-p1-generic-ud-bridge-record-001.md).
- M0 T198 S5 P2 is complete: a new default-off core gate source-builds under
  CPU5/MSVC and invokes only the fixed mantle record/typed outcome path; its
  stop uses the finite PC stop latch.  Historical parallel gates remain for
  P3 removal and are not part of the new composition; see
  [T198 S5 P2 core hook](etc/research/t198-s5-p2-core-mantle-ud-hook-001.md).
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
  corrected into a mantle-owned, selector-blind copied ordinary-RAM action
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
  core/mantle ABI or semantic addition is indicated. See [T198 S11 P1 map](etc/research/t198-s11-p1-synchronous-composition-map-001.md).
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
  finite mantle source-build, link and run as one MSVC x64 `/MT` PE32+ process
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
  a new selector-blind mantle request, not a startup BOP or Bochs semantic;
  entry changes only CS:IP and stops at the already-admitted `50:11` boundary.
  See [S21 pre-entry map](etc/research/t198-s21-ntio-preentry-contract-map-001.md).
- M0 T198 S22 closes the selector-blind pre-entry primitive: a versioned,
  copied v2 request preflights a one-MiB aperture, captures/restores opaque
  bytes across image publication, and then applies CS:IP.  The full CPU5
  MSVC x64 `/MT` fixture proves restoration and rejection before mutation; see
  [S22 witness](etc/research/t198-s22-mantle-preentry-witness-001.md).
- M0 T198 S23 P1 source-builds the `bx-vdm` NTIO request builder under MSVC
  x64 `/MT`: it copies only the exact `0x8400` image into mantle v2 and
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
- M0 T198 S24 P3 admits and verifies a default-off mantle-private terminal
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
- M0 T198 S31 adds a one-shot, opaque, checked mantle terminal-RAM snapshot.
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
  be a separately admitted fixed-width core-to-mantle mechanic, never a BOP or
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
- M0 T198 S38 is complete: the registered, default-off mantle-only query
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
  mantle mechanical action. The old all-`FF` `0A84` frontier is gone; the new
  reproducible `0000:0047` frontier follows the still-unexplained
  `9346:4D53 -> 0000:0000` transfer. See
  [S40 closure](etc/research/t198-s40-preentry-input-closure-001.md).
- M0 T198 S41 is complete: the registered copied position record proves that
  the unexpected CS transition changes SS:SP from `00A7:0918` to
  `00A7:090E`. The frame owner remains unproven. See
  [S41 witness](etc/research/t198-s41-stack-position-witness-001.md).
- M0 T198 S42 is complete: the registered bounded mantle provenance record
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
