# MVDM cross-audit: first parallel review

The active review follows the original four disjoint partitions in
`manifest.json`. Three reviewers and the coordinator work on separate shards;
shared ledger changes are merged only after independent verification. The
external census remains a separate obligation.

The [nine-way part 2 census](nine-way/part-2/dos-tools-dispatch-review.md)
also records [DOS shared inputs](nine-way/part-2/dos-shared-inputs-review.md),
[locales](nine-way/part-2/dos-locales-review.md), [fax boundaries](nine-way/part-2/fax-boundary-review.md),
and separate external [printer implementations](nine-way/part-2/printer-implementation-review.md),
[PPDs](nine-way/part-2/psprint-ppd-review.md), and [model resources](nine-way/part-2/rasprint-mini-review.md).
Its coverage observer checks key accounting only; external relevance and runtime
equivalence remain open and must not be inferred from file identities.
The [part 6 guest driver and common-dialog review](nine-way/part-6/wow16-drivers-dialog-role-review.md)
and [external GDI inventory](nine-way/part-6/external-role-review.md) record
separate image identities and unresolved provider boundaries; these are not
runtime or binary-to-source provenance proof.
The [part 5 BIOS resources](nine-way/part-5/bios-resource-family-review.md),
[host inputs](nine-way/part-5/host-input-family-review.md),
[VDD samples](nine-way/part-5/vdd-samples-family-review.md),
[debugger](nine-way/part-5/debugger-family-review.md) and
[multimedia provider](nine-way/part-5/multimedia-external-review.md) records
separate linked-image evidence from unresolved behavior. The remaining
[external inventory](nine-way/part-5/external-envelope-review.md) is a source
identity and recipe census, not an applicability decision.
The [part 9 WOW32 provider review](nine-way/part-9/wow32-provider-role-review.md)
and [guest utility review](nine-way/part-9/wow16-utilities-write-role-review.md)
separate build-graph selection from execution evidence; the packaged WRITE.EXE
identity differs from the retained mirror and its provenance remains open.
The [part 3 CPU instruction](nine-way/part-3/ccpu-instructions-role-review.md),
[simulator](nine-way/part-3/simulator-role-review.md), and
[printer resource](nine-way/part-3/hppcl5ms-role-review.md) reviews retain
their unresolved execution and provider selection obligations.
The [cross-part 3/6/9 owner worklist](cross-369/function-owner-worklist.md)
maps concrete findings to the planned recovery stages and traces WRITE image
identity separately from the limited historical runtime witness.
The [part 8 EDIT boundary](nine-way/part-8/edit-guest-host-review.md) separates
guest source from the selected host control, and the [ToolHelp review](nine-way/part-8/toolhelp-family-review.md)
distinguishes export and dispatch observations from guest reachability.
The [ToolHelp semantic reconciliation](nine-way/part-8/toolhelp-semantic-reconciliation.md)
binds its original unimplemented class entries to the published dispatch table;
task and module queries remain separate guest-owned validation work.
Its [remaining guest inventory](nine-way/part-8/guest-family-inventory-review.md)
and [external raster resources](nine-way/part-8/rasprint-mini-review.md)
retain unresolved provider and relevance decisions.
The [cursor/icon alias review](cross-369/cursor-cache-disposition.md) and
[DDEML thunk review](cross-369/ddeml-thunk-disposition.md) compare selected
original providers with historical alternatives; role recommendations remain
subject to coordinator disposition and do not close runtime obligations.
The [OLE alias interface review](cross-369/ole-alias-header-disposition.md)
records selected dependencies and a bounded alias witness, with observation
failure and full guest interoperability still open.
The [selected CMOS and ROM owner review](nine-way/part-5/cmos-rom-semantic-reconciliation.md)
and [WOW fax review](cross-369/fax-provider-disposition.md) retain their
specific published bindings and identify untested failure paths for S13 and
S16/S17 respectively.
The [DOSX transition review](nine-way/part-2/dosx-transition-semantic-reconciliation.md)
traces the guest continuation frame into the selected host adapter. The
[MCI converter review](cross-369/mci-converter-disposition.md) distinguishes
the inactive historical converter from the separate guest multimedia route;
neither report claims current end-to-end device behavior is verified.
The [multimedia callback review](nine-way/part-5/multimedia-semantic-reconciliation.md),
[SHELL owner review](nine-way/part-8/shell-semantic-reconciliation.md), and
[combo/list message review](cross-369/control-message-disposition.md) trace
selected original paths and distinguish bounded historical tests from missing
current-artifact behavior and lifecycle evidence.
The [DOS IOCTL review](nine-way/part-2/dos-ioctl-semantic-reconciliation.md),
[REGEDIT review](nine-way/part-8/regedit-semantic-reconciliation.md) and
[spool interface review](cross-369/spool-header-disposition.md) distinguish
historical roles and bounded existing tests from unverified real-device and
failure cleanup paths.

The [mirror-difference retention estimate](mirror-diff-code-retention-review.md)
provides reproducible current-source percentages and identifies the non-code
build input; this is not behavioral coverage.

The [parallel native review](parallel-native-role-review.md) merges five
independently replayed original-source/image bindings as partial.
The [parallel C-VID review](parallel-cvidc-role-review.md) adds five retained
compiler declaration excerpts and one linked initial vector, all partial.
The [parallel WOW GDI review](shard-2/parallel-review-third.md) binds 28
font, text and metafile targets; guest execution and callback lifetimes remain open.
The [DOS directory declarations](shard-2/curdir-abi-review.md),
[DPMI interrupt declarations](shard-2/dpmi-interrupt-interface-review.md),
[storage interfaces](shard-3/storage-interfaces-role-review.md) and
[audio VDD samples](shard-1/vdd-audio-samples-review.md) record selected
dependencies and bounded exclusions. The [directory and interrupt checkpoint](directory-interrupt-headers-role-review.md)
reconciles three selected declarations and two device sources; remaining dispositions and
runtime behavior remain open.
The [bus mouse and PPI review](shard-2/dos-mouse-ppi-device-review.md),
[floppy lifecycle review](shard-3/floppy-lifecycle-role-review.md), and
[MMSYSTEM legacy review](shard-1/guest-mmsystem-legacy-review.md) retain
source-level findings and explicitly separate untested behavior from evidence.
The [CPU486 guarded-input review](shard-2/cpu486-guarded-inputs-review.md)
records three compiled inputs with no code in the captured SPC386 profile;
it does not treat their library membership as restored instruction behavior.
The [floppy lease follow-up](shard-3/floppy-lease-failure-role-review.md)
traces the actual FDC thread creation and identifies a host-side session
binding gap, with device triggering and a repair still awaiting verification.
The [Win16 KERNEL legacy review](shard-1/guest-kernel-legacy-review.md)
retains the separate installed-kernel provenance boundary.
The [MSCDEX package review](shard-0/mscdex-package-review.md) records a
guest TSR and missing scoped VDD-provider candidate without claiming a
runtime or optical-device failure.
The [legacy disk provider](shard-2/dos-disk-legacy-provider-review.md),
[base host profile](shard-3/base-host-profile-role-review.md), and
[WIN87EM variants](shard-1/guest-win87-variants-review.md) reviews retain
configuration, image identity and runtime boundaries for later reconciliation.
The [legacy monitor interfaces](shard-3/monitor-legacy-interfaces-role-review.md)
and [keyboard mapping headers](shard-2/dos-keyboard-legacy-headers-review.md)
reviews distinguish profile-specific declarations from selected behavior.
The [monitor profile checkpoint](monitor-profile-headers-role-review.md)
records four inactive header roles and three selected declarations, without
excluding selected memory, video, CPU or debugging capabilities.
The [WINOLDAP owner review](shard-1/guest-winoldap-owner-review.md) records
the original non-Win16 launch/return chain and scoped module availability gap.
The [SYSTEM variant review](shard-1/guest-system-variants-review.md) records
historical Vectra declarations and the observed SYSTEM.DRV image identities.
The [DOS BIOS and firmware review](shard-2/dos-bios-firmware-review.md),
[RAS/WINSPOOL review](shard-1/guest-ras-winspool-review.md) and
[Novell/TCP interface review](shard-3/novell-tcp-interface-role-review.md)
preserve profile, guest-image and runtime limitations for later reconciliation.
The [COMMDLG resource](shard-1/guest-commdlg-resources-review.md),
[debug profile](shard-2/dos-debug-profile-review.md), and
[GISP/VGA interface](shard-3/gisp-vgastate-interface-role-review.md) reviews
retain their source and build boundaries without asserting runtime behavior.
The [USER historical review](shard-1/guest-user-historical-review.md) and
[host keyboard/ROM review](shard-3/legacy-host-matrix-rommap-role-review.md)
retain further selected-versus-historical source boundaries.
The [C-VID header batch](shard-2/cvidc-header-batch-review.md) records 17
reviewed candidates; its [guard closure](shard-2/cvidc-guard-closure-review.md)
reconciles 11 empty file roles while leaving profile and capability questions open.
The [historical tools16 chain](shard-0/tools16-chain-review.md) and
[retained-tool review](shard-3/tools16-build-input-role-review.md) distinguish
absent mirror paths from hash-matched historical copies.
The [33-file checkpoint](tools16-retained-role-review.md) reconciles those
absent mirror roles while preserving the relocated tools and their uses.
The [Win16 timer](shard-1/guest-timer-review.md),
[EMS/Sim32 headers](shard-2/dos-emm-sim32-headers-review.md), and
[Win16 sound resources](shard-3/wow-sound-resources-role-review.md) reviews
retain their separate image, provider and runtime proof boundaries.
The [DOS video profile review](shard-2/dos-video-profile-headers-review.md)
distinguishes selected EGA declarations from unproved rendering behavior.
The [XMS runtime owners](shard-0/xms-runtime-owner-review.md) bind selected
initialization, A20, UMB and query functions to released worker code while
retaining guest and lifecycle obligations.
The [C-VID selected core](shard-2/cvidc-selected-core-image-review.md),
[VDMREDIR DLC recheck](shard-1/vdmredir-dlc-image-recheck.md),
[CPU profile review](shard-3/ccpu-486-debug-provider-role-review.md) and
[host platform headers](shard-2/dos-host-platform-headers-review.md) separate
image samples from unverified runtime and alternative-profile behavior.
The [C-VID core checkpoint](cvidc-selected-core-role-verification.json)
reconciles three source roles; the [WOW32 resource review](shard-1/wow32-resource-image-review.md)
retains its resource ID, language and failure-path gaps.
The [DPMI debug profile](shard-2/dpmi-debug-selected-image-review.md) and
[WOW DIB lifecycle](shard-3/wow-dib-provider-lifecycle-review.md) reviews
retain their build selection and unproved failure-path boundaries.
The [DPMI debug checkpoint](dpmi-debug-profile-role-verification.json)
classifies only the inactive release debug body; DPMI remains in scope.
The [multimedia alias follow-up](shard-3/wow-multimedia-alias-consumer-followup.md)
and [multimedia header review](shard-1/wow-multimedia-headers-review.md)
distinguish disabled original conversion code from the active bridge.
The [CCPU 486 instruction review](shard-2/ccpu-486-instructions-review.md)
records gated CMPXCHG/XADD bodies and an image-bound interpreter entry.
The [WOW keyboard thunk review](shard-3/wow-keyboard-thunks-lifecycle-review.md)
binds eleven selected bodies to released code while retaining guest and
pointer-lifetime acceptance.
The [redirector dispatch review](shard-0/redir-dispatch-release-review.md)
binds all 50 original service-table targets to the released DLL without
claiming guest or network execution.
The [redirector service owners](shard-0/redir-service-owners-review.md)
bind 22 original initialization, cleanup, mailslot and utility functions.
The [DLC post owner](shard-0/redir-dlc-post-image-review.md) binds eleven
additional event and interrupt bodies while retaining provider obligations.
The [DLC command owner](shard-0/redir-dlc-command-image-review.md) binds 17
additional bodies and event references, without proving provider reachability.
The [interface and callback checkpoint](selected-interface-callback-role-review.md)
reconciles seven selected source and resource roles, not complete behavior.
The [generic thunk published binding](shard-3/wow-generic-thunk-published-binding.md)
relates earlier mismatch evidence to the currently released code section.
The [generic thunk convention review](shard-3/wow-generic-thunk-call-convention-review.md)
records the current-object versus historical-assembly selection boundary.
The [debug object](shard-2/base-debug-objects-review.md) and
[GDI header](shard-1/wow-gdi-headers-review.md) reviews retain bounded
profile and declaration evidence.
The [SLAVEPC disk review](shard-2/disk-slavepc-review.md) and
[WOW callback declarations](shard-1/wow-callback-control-headers-review.md)
preserve alternate profile and reentrancy boundaries for later work.
The [KillWOW review](shard-1/guest-killwow-review.md) and [keyboard variant
review](shard-3/dos-keyboard-variants-role-review.md) retain their worker-wide
exit and layout selection boundaries.
Additional [tools16 checks](shard-1/historical-tools16-recheck-review.md),
[DOS build inputs](shard-2/tools16-unmirrored-review.md) and
[Win16 assembly declarations](shard-1/guest-inc-asm-review.md) retain their
specific build and provenance boundaries.
The [parallel USER and BIOS review](parallel-cross-role-review.md) binds menu,
COMM and BIOS time owners while retaining their runtime obligations.
The [parallel DPMI review](shard-3/dpmi-register-stack-role-review.md) binds
register/stack owners and selected call edges; guest transitions remain open.
The [tabbed-text and DEM error review](parallel-text-dem-role-review.md)
binds their selected original entries; guest and failure behavior remains open.
The [resource and redirector review](parallel-resource-redir-role-review.md)
binds seven further owners; guest and external-provider lifetimes remain open.
The [heap and video-accessor review](parallel-heap-video-role-review.md)
retains bounded heap evidence and unresolved concurrency/lifetime obligations.
The [control and directory review](parallel-controls-directory-role-review.md)
records selected bindings and a bounded original label-read counterexample.
The [USER client and SOUND review](parallel-user-sound-role-review.md) retains
guest local-path, audio and lifecycle boundaries.
The [USER interaction checkpoint](parallel-user-interaction-role-review.md)
records three partial source roles and retains guest behavior and teardown boundaries.

The [empty declaration checkpoint](shard-0/cvidc-empty-guards-review.md)
excludes eleven empty file roles while preserving CPU, video and network scope.

The [guest resource checkpoint](parallel-guest-resource-role-review.md) excludes
seven historical guest build-input roles, not their guest capabilities.

The [GDI and guest-input checkpoint](parallel-gdi-ctl3d-role-review.md) records
one partial image and eight historical input roles without excluding features.

The [bit and rotate checkpoint](shard-0/ccpu-bit-rotate-image-review.md) binds
two CCPU instruction bodies and retains S11 guest execution obligations.

The [DDEML and WFWNET checkpoint](parallel-ddeml-wfwnet-role-review.md)
reconciles twelve guest-input and editor-state roles without excluding features.

The [DEBUG and WRITE checkpoint](parallel-debug-write-role-review.md) retains
the deployed-versus-source WRITE image distinction and guest feature obligations.

The [SHLD and guest tools checkpoint](parallel-shld-tools-role-review.md) binds
the CPU body and GRAPHICS image while retaining their runtime obligations.

The [DPMI buffer/state checkpoint](shard-0/dpmi-buffer-state-review.md) binds
the original translation owners while retaining guest and failure obligations.

The [DOSX guest checkpoint](dosx-guest-role-review.md) records the shipped
guest image as partial and historical build inputs as file roles only.

The [DOS kernel image checkpoint](shard-0/dos-kernel-image-review.md) records
two partial images and one historical intermediate without excluding DOS.

The [WOW USER declaration checkpoint](wow-user-header-role-review.md) records
five selected headers as partial, not full guest behavior.
The [multimedia and dispatch checkpoint](wow-media-dispatch-header-role-review.md)
adds four selected original declaration roles with guest behavior still open.
The [cross-language header checkpoint](wow-cross-language-header-role-review.md)
records selected C declarations and retains guest ABI and table obligations.
The [shared thunk header checkpoint](wow-shared-thunk-header-role-review.md)
records six further selected declarations with runtime obligations open.
The [guest service header checkpoint](guest-service-header-role-review.md)
records three selected host declarations while retaining guest service obligations.
The [XMS and suballocator checkpoint](shard-0/xms-suballoc-image-review.md)
binds selected original bodies while retaining guest failure and lifecycle work.
The [command and input header checkpoint](selected-command-input-headers-role-review.md)
records four selected declarations without claiming guest or device completion.
The [VDMREDIR image comparison](shard-0/vdmredir-package-identity-review.md)
retains the cached-versus-release resource and read-only data identity boundary.
The [guest-input dependency recheck](recent-guest-input-dependency-review.md)
rechecks relative Ninja paths for 37 recent file-role exclusions.
The [excluded-role sweep](excluded-dependency-sweep.md) inventories and
reviews 13 graph or dependency hits among 2,727 excluded roles at its recorded
baseline; release-image and indirect-caller limits remain explicit.
The [additional empty guards](shard-3/cvidc-empty-guard-role-review.md) exclude
twelve file roles without excluding CPU or video functionality.

The [DDEML test-input checkpoint](shard-0/ddeml-test-input-review.md)
excludes individual historical sample/build roles, never DDE functionality.

The [synchronous Winsock checkpoint](shard-0/wow-winsock-raw-review.md)
retains guest, network failure and socket-lifetime obligations.

The [selected empty WGMAN file role](shard-0/wow-gdi-empty-owner-review.md)
excludes no GDI functionality and retains its real linker-directive boundary.

The [GDI core and WOW table checkpoint](parallel-wow-gdi-table-role-review.md)
retains guest, device and lifecycle boundaries despite 119 linked GDI slots.

The [embedding and Win16 sound checkpoint](parallel-embedding-sound-role-review.md)
retains guest execution and lifecycle boundaries.

The [WOW heap, memory and spool checkpoint](parallel-wow-memory-spool-role-review.md)
retains their guest behavior, shared handle and failure boundaries.

The [WOW palette checkpoint](shard-0/wow-palette-image-review.md) binds
initial GDI and USER entries while retaining guest and copyback boundaries.

The [C-VID declaration checkpoint](shard-0/cvidc-declaration-review.md)
retains four selected compiler contexts with runtime boundaries open.

The [guest delivery checkpoint](parallel-guest-delivery-role-review.md) records
six partial image/data roles and four file-only historical exclusions.

The [parallel source and package observations](parallel-source-package-observations.md)
retain 105 independently replayed investigations; proposed dispositions remain pending.

This is an in-progress S10 preparation record, not S9 closure or T acceptance.
The [planar pixel contract](planar-pixel-contract-review.md) adds bounded
actual-object evidence without changing the pending source-role ledger.
The owner authorized four parallel roles; three agents and the primary session
used the disjoint frozen path shards described by `manifest.json`.

`frozen-file-reconciliation.csv` freezes the 4,684 unique MVDM union paths.
`combined-initial-status.csv` is the current merged index; detailed per-file
reasoning and limitations are in each shard directory. The historical retired-host
checkpoint recorded 448 partial, 2,643 excluded, 53 full and 1,540 pending;
the live counts are in the current index and project status.
Each checkpoint's exact-delta verifier is tied to its named base/delivery;
run historical ones in their matching checkout. The current ledger gate is
recorded by each checkpoint rather than a prior snapshot's fixed counts.
Exclusion of an object or provider does not prove its guest
functionality works; see the WOW32 object audits for [shard 0](shard-0/historical-wow-objects.md),
[shard 1](shard-1/historical-wow-objects.md), [shard 2](shard-2/historical-wow-objects.md)
and [shard 3](shard-3/historical-wow-objects.md).
The [CCPU foundation review](ccpu-core-source-review.md) adds original source
contracts and an actual-object NEG64 witness with signed-caller image checks.
Its [nine-row checker result](ccpu-foundation-role-verification.json) records
partial integrations, with no new full acceptance or candidate qualification.
The [remaining CCPU C-source review](ccpu-remaining-c-source-review.md) covers
all 22 still-pending C files, distinguishing 13 mapped source roles from
six inactive 486 bodies and three alternative/inline roles.
The [control guest checkpoint](ccpu-control-review.md) subsequently resolves
ten of those roles: nine partial owners and the unused standalone NOP helper
file, with the original inline NOP instruction retained and tested.
The [header/macro checkpoint](ccpu-header-macro-review.md) adds eight real
preprocessing contexts and ten file dispositions, retains the reproduced
negative bit-index failure and separates inconclusive x87 experiments.
The [remaining twelve-header review](ccpu-remaining-header-source-review.md)
uses complete worker dependencies to distinguish selected declarations from
different generated schemas; all twelve remain pending.
The [DOS command input review](dos-command-input-review.md) reconciles all 85
remaining command-source roles through real guest link/include chains,
a message-tool recipe and explicit obsolete/history records; guest capability
and shipped-image provenance remain independent obligations.
The [BIOS checkpoint](bios-native-role-review.md) adds seven partial owners,
52 image-bound functions, all 256 BIOS/30 EMS vector targets and two actual
836-case guest dispatch runs with wrong-oracle controls. Wider BIOS, EMS and
device lifecycle acceptance remains open.
The [system-provider review](system-native-role-review.md) reconciles eight
more roles through the original NT recipe, 77 mapped functions, ten real
guest/oracle contracts and 24 native PIC assertions. Only the complete
no-state dummy leaf becomes full; five devices remain partial and two
alternative/revision files are excluded without excluding their capabilities.
The [disk-provider review](disk-native-role-review.md) adds seven partial
owners, 102 image-bound functions, 47 actual BIOS initializer targets, 433
controlled GFI assertions and four real guest reset/oracle contracts. Three
remote/diagnostic profiles remain pending; real-media lifecycles stay open.
The [video-entry review](video-native-role-review.md) adds five partial
owners, one alternative CPU-interface file exclusion, 21 image-bound functions
and 67 actual initial pointers. Six guest/oracle runs prove bounded invalid
entry semantics and preserve the default BIOS workload; tracing remains pending.
The [input/NLS review](input-native-role-review.md) adds three partial owners
and two original-NT generic-table file exclusions. Actual objects pass bounded
268,322 keycode/LED, 3,645 NLS and 70 IOS assertions. A strict original keycode
bound witness fails and remains a concrete S13 repair, not a passing runtime case.
Its current gate is [delivery verification](input-native-delivery-verification.ps1).
The [pending-role census](pending-role-census.md) separates initial inventory
hints from final dispositions; it does not remove any pending path from scope.

The [187 historical guest source-role dispositions](guest-source-role-review.md)
trace original guest compile/link/include/resource edges. They exclude host
input roles only; guest functionality and image provenance remain separate.

The [100 Kernel 3.1 source roles](kernel31-source-role-review.md) separately
trace KRNL386, historical 286P, stub/resource and SWAP tool inputs; they do not
activate another profile or establish immutable-image derivation/behavior.

The [DOS/device and export-source checkpoint](dos-core-source-role-review.md)
adds 179 proven guest-input roles and two partial export-source records. The
[supplemental identity reconciliation](supplemental-original-provenance.md)
accounts for every frozen current-only path without selecting new runtime
code; the active original export-template gap is in the S11 worklist.

The [build-carrier review](build-carrier-review.md) distinguishes configuration
reads from Ninja execution dependencies. Its 100 additional exclusions concern
historical recipe files only, never the capabilities of their source members.
The [19 partial build carriers](partial-build-carriers.csv) have an explicit
non-numeric estimate basis; executable code-retention measurements for the
existing 205 source rows are unchanged.

The [architecture-header checkpoint](architecture-header-review.md) accounts
for 125 additional nonselected MIPS/PPC declaration variants without excluding
their common algorithms or the selected C-VID/x86 declarations.

The live hash sweep records the six source/registry drifts listed in
`initial-summary.json`; the [live candidate observation](live-candidate-source-observation.md)
accounts for two newly modified original video sources. These are observed
baseline drifts, not accepted runtime repairs from this audit. This review changed no product
source or guest media. `frozen-external-audit-rows.csv` preserves 42,963 extra WOW audit
paths outside the MVDM union for relevance and dependency review.

The [complete external triage](external-obligation-triage.md) joins all
42,963 IDs to their source-audit roles and actual pinned Git indexes. It
separates 36,736 device-context, 6,134 host-envelope and 93 explicit-boundary
rows; only nine reviewed recipe file roles are excluded, not their features.

Review breadth differs by shard. Shard 0 has metadata for 62 diff files, nine
source-level detailed reviews, initial source-manifest scans of 43 unselected
host files and a focused generic-thunk branch analysis. Shard 1 has 48
differential owner initial reviews, 34 unselected-role reviews and selected
owner analysis for 73 of 73 pending product sources. Shard 2 has recovered
the previously interrupted CSV, renewed initial traces of all 48 diff files
and checked 24 CCPU and 10 WOW32 product-source linkages. Shard 3 has 47 differential,
23 unselected-role and 73 selected-source initial reviews, plus a FastWOW
comparison. These investigations do not replace semantic or runtime proof.

Read `findings.md` for actionable cross-group discrepancies. Source-line
matching percentages in the frozen input are textual heuristics, not the
requested functional restored-code percentage. Calculate and justify the
latter only when original owner, selected edition and actual behavior are
known. Regenerate the combined index after any shard changes and verify
uniqueness, shard ownership and frozen identity before relying on counts.

`external-priority-boundaries.csv` retains the original audit's 93 explicit
non-MVDM boundary rows, including its reasons and reviewed slices. The
remaining 42,870 external rows were still marked provider-membership pending
by that audit. Neither category is automatically admitted as product code
or discarded; each requires relevance and current-owner analysis. The
accompanying summary records the exact source-catalog hash and the narrow
local-path comparison; a missing same-shaped path is not proof that no
modern host or differently located adapter supplies a capability.

The first separately traced external provider row is the WINMM callback
layout: see `external-winmm-callback-boundary.md`. It remains pending
behavioral and compatibility verification.

Shard 0 additionally records historical link-response and include evidence for
28 pending COMMAND guest files in `shard-0/command-guest-build-evidence.csv`;
this does not identify or validate the deployed guest binary.

A separate pending relevance census for the external rasprint tree is in
`external-rasprint-boundary.md`; it does not bulk-exclude driver resources.

The 205 baseline source-difference paths have a reproducible source-text retention
metric in `partial-code-retention.csv`; its limitations are in
`partial-code-retention.md`. It is not a functional completeness percentage.

The [provisional recovery worklist](recovery-worklist.md) groups evidenced
findings by approved stage while keeping unresolved inventory in S10 scope.

The [additional historical-build cohort](shard-0/historical-objects-reconciliation.md)
excludes 75 verified retained output files as current product inputs only.

The [kernel VDM boundary trace](external-kernel-vdm-boundary.md) separately
identifies the directory-reset service and its selected DEM fallback.

Further history-only output reviews are [shard 1](shard-1/historical-object-batch.md), [shard 2](shard-2/historical-i386-artifacts-review.md), and [shard 3](shard-3/historical-i386-binaries-review.md); each excludes the retained file, not its capability.

The dormant original [multimedia host body](shard-2/wow-multimedia-inactive-body-review.md) is excluded as a source-file body, not as a guest multimedia capability.

The focused unselected-source records are [shard 0](shard-0/nonruntime-source-disposition.md), [shard 1](shard-1/unselected-finite-worklist.md), [shard 2](shard-2/unselected-host-role-disposition.md), and [shard 3](shard-3/unselected-host-tooling-disposition.md). Independent component nonselection alone is not a capability exclusion.

The [26 full selected leaf contracts](full-selected-leaf.csv) combine
unchanged original source, actual published code/decoder targets and
[real guest positive and negative evidence](ccpu-sign-flags-contracts.md).
Their status does not close the broader CPU decoder or execution owners.

The [twelve arithmetic/logic source contracts](ccpu-alu-contracts.md) add
native-reference data and 3,260,544 actual guest instruction checks across
register and memory forms. Their full status leaves decoder/fault owners open.

The [guest-build checkpoint](guest-build-reconciliation.md) separates 840
historical guest objects, symbols/listings and recipes from source and runtime
capability. Guest sources, resources, libraries and unproved binary lineage
remain explicitly in scope; no original guest was rebuilt.

The [26 full selected declaration files](full-selected-declarations.csv)
have a [separate ABI closure](ccpu-leaf-header-abi.md); they do not double
the number of verified instructions or close a broader CPU contract.

The [NOT/XCHG/LEA pointer contracts](ccpu-transfer-contracts.md) add
3,276,800 actual guest checks and three complete source/declaration pairs.
The 486 profile findings remain a finite S11 recovery group, not an exclusion.

The [six-file loader ownership review](external-loader-owner-review.md)
traces the original byte-return consumer through the current public loader
and CRT bridge. It resolves source ownership without claiming actual DLL
lifecycle acceptance or changing external/MVDM classification counts.

The [Win16 core source-role review](wow16-core-source-role-review.md) resolves
85 original guest build-input roles without excluding their capabilities.
Its independent 32-file library inspection corrects a same-basename COFF/OMF
inference; library dispositions and source-to-image lineage remain open.

The [103 extension/core roles](wow16-extension-source-role-review.md) add
exact guest build edges and recover LIBH's macro-qualified consumer. The
same review distinguishes dual-purpose metafile code and original ToolHelp
unimplemented thunks from actual missing product functionality.

The [guest-library role checkpoint](guest-library-role-review.md) resolves
27 exact-original archives and one macro-source input, retaining four
unconfirmed library paths. Source membership and runtime behavior remain
distinct; the format reproducer preserves the full frozen 32-file cohort.

The [108 support-source roles](wow16-support-source-role-review.md) retain
WIN87EM target conditions and distinguish DDEML native tests. The same
record indexes 454 pending native-owner proposals without claiming their
functional closure or changing those statuses.

The [quick-event owner checkpoint](quick-event-owner-review.md) establishes
the original NT provider selection and tests the actual cached queue object
under controlled host boundaries. It partially verifies two files and
excludes one generic alternative, without claiming full device/guest or
published-image acceptance.

The [IOS/CCPU owner checkpoint](ios-owner-review.md) adds 70 controlled
assertions against three actual cached objects and repeats the 37-assertion
quick-event control through shared test plumbing. Six unchanged original
source/declaration files become partial; instruction privilege, real devices,
concurrency/retirement and published-image equivalence remain open.

The [declaration/resource checkpoint](guest-declaration-resource-role-review.md)
reviews 434 exact pending DOS/WOW16 paths and resolves 129 positive historical
guest/tool input roles. Its other 305 paths remain pending, including actual
host declarations, dual-purpose GDI, CTL3D profiles and ambiguous include roots.
Guest capability, current-image lineage and runtime proof remain separate.

The [generated-message/artifact checkpoint](generated-message-role-review.md)
resolves 76 historical file roles through actual MASM macros, original build
recipes and bounded SYM/OMF/PCH/text observations. It separates generated
source from binary artifacts and explicitly retains unproved image lineage;
no corresponding message, debugging, VDD or DPMI capability is excluded.

The [shared-include checkpoint](shared-guest-include-role-review.md) resolves
44 of 104 declaration/macro roles through original include roots and targets,
distinguishing native SDK names and real local shadows. Sixty files retain
specific unresolved consumers or profile boundaries; no API is excluded.

The subsequent shared-include proposals for [shard 1](shard-1/shared-guest-include-followup.md),
[shard 2](shard-2/shared-guest-include-followup.md) and
[shard 3](shard-3/shared-guest-include-followup.md) retain exact per-file
questions and ten additional file-role candidates. They are review proposals,
not merged dispositions or additional restored capabilities.

The [current-source candidate review](candidate-runtime-review.md) records
the completed build, selected Full, game, WRITE and Video7 checks, alongside
two failed SOL Options witnesses and two passing accepted-baseline controls.
The diagnostic-only observation does not fix or waive those production
failures; candidate publication remains unqualified.

The [native-source checkpoint](native-source-role-review.md) reviews 90 more
paths, records 36 partial integrations and two original PIG test-source roles,
and retains 52 concrete unresolved files. Text retention is not behavior
coverage; no row is promoted to full and no new runtime pass is claimed.

[Exact-byte evidence storage](evidence-byte-preservation.md) records the
core.autocrlf discrepancy and the narrow byte-preservation rule. It changes
Git's storage of captured audit bytes, not product or guest behavior.

The [diagnostic follow-up](candidate-runtime-followup-review.md) records the
CPL ablation failure and additional error-only observation passes. None
qualifies the production candidate or changes the retained failure verdict.

The next native proposals for [shard 1](shard-1/native-pending-followup-2.md),
[shard 2](shard-2/native-pending-followup-2.md) and
[shard 3](shard-3/native-pending-followup-2.md) cover 180 additional paths.
Their [identity check](native-next-proposal-verification.json) does not merge
the 47 partial/one file-role proposals or accept functionality. The
[selected slave-BIOS EOI finding](shard-0/slave-bios-eoi-source-review.md) is
also a source-level S13 obligation, not a runtime diagnosis or patch.

The [failure-only creation observation](creation-negative-only-review.md)
removes success-path error interception and passes one isolated Options
observation. No failure branch was captured; the production failure remains
unresolved and the diagnostic is not publishable.

The subsequent [main review](native-source-next-role-review.md) merges those
47 partial integrations and one empty original file role, retaining all 132
other proposals as pending. Per-file source, map/graph and evidence limits are
recorded; no new full row or runtime capability is accepted. The total ledger
now has 1,804 pending, 319 partial, 52 full and 2,509 excluded paths.

The [remaining historical recipe review](historical-build-role-review.md)
resolves 36 physical build-control roles after checking actual configuration
consumers separately from compiler inputs. Their listed source/capability
obligations remain in scope. The current total is 1,768 pending, 319 partial,
52 full and 2,545 excluded file roles; no runtime capability is newly accepted.

The [native CBT metadata sample](native-cbt-info-review.md) checks 5,000
creation callbacks without a failed native metadata query. It constrains a
specific diagnostic hypothesis; it does not locate or waive the SOL failures.

The [selected CCPU declaration checkpoint](selected-ccpu-header-role-review.md)
uses the actual production preprocessor output to classify 71 original
declaration slices as partial; 25 other headers remain pending. The current
ledger is 1,697 pending, 390 partial, 52 full and 2,545 excluded file roles.
The separate [SOL debugger observations](sol-creation-debugger-review.md)
distinguish sentinel error writes from actual failure branches; none qualifies
the held candidate or selects a repair.

The [latest-source delivery](latest-source-release-review.md) records the owner-directed ten-image publication correction and independent SZDD/W31PATCH tools; the whole-source reconciliation remains open.

The [VDD owner checkpoint](vdd-native-role-review.md) binds nine original bodies and two partial file roles; 305 normal checks pass while strict uninstall and DMA boundary failures are preserved for S13.

The owner-directed [add-on layout delivery](addon-layout-review.md) records twelve source/build components, single-copy releases and exact naming; the [tools directory review](tool-directory-review.md) separates current owners from archive candidates. This does not close MVDM source recovery.

The [host-tail checkpoint](host-tail-role-review.md) classifies seven unchanged
NT host sources: four selected roles remain partial and three narrowly scoped
historical profiles are excluded; serial and frontend lifecycles remain open.

The [base-header checkpoint](base-inc-role-review.md) records five selected
original declaration roles as partial. The [device/video](base-inc-multi-role-review.md)
and [dependency-selected](base-inc-wide-role-review.md) extensions add 30 and
19 respectively; the remaining 92 headers in that cohort stay pending for
other consumers and profiles.

The [alternate host interfaces](base-inc-alternate-role-review.md) checkpoint
excludes six Unix or SoftWindows headers without excluding the corresponding
selected NT or guest capabilities.

The [generated C-VID review](cvidc-generated-role-review.md) binds 17 more
selected original objects to the released worker. Plane and mode behavior
remain open; object identity is not behavioral acceptance.

The [C-VID vector review](cvidc-vector-role-review.md) resolves 692 initial
image pointers to original generated owners; runtime selection remains open.

Recent bounded reviews cover [WOW shell, hook and subclass bodies](shard-0/wow-shell-hook-image-review.md), [file-cache flush](shard-1/wow-file-cache-flush-review.md), [keyboard variant selection](shard-2/keyboard-variant-link-review.md), and [DIB mapping providers](shard-3/wow-dib-selected-mapping-provider-review.md). These are source and image evidence, not end-to-end acceptance; their remaining obligations are recorded in the recovery worklist.

Additional source-chain reviews cover [DIB descriptor retirement](shard-3/wow-dib-descriptor-retirement-review.md), [WFWNET printing](shard-1/guest-wfwnet-print-review.md), [Dr. Watson resources](shard-1/guest-sherlock-review.md), [DOS kernel wrapper selection](shard-2/msdata-wrapper-owner-review.md), [redirector constant ownership](shard-2/redir-fudgedef-owner-review.md), and [COUNTRY executable lineage](shard-3/dos-country-exe-lineage-review.md). These observations do not prove guest build lineage or runtime behavior and do not themselves classify pending ledger rows.

Further bounded reviews cover [DOS locale inputs](shard-2/dos-locale-source-chain-review.md), [MSCDEX input roles](shard-3/mscdex-guest-vdd-input-role-review.md), [WOW keyboard build inputs](shard-3/wow-keyboard-driver-build-role-review.md) and [CTL3D resources](shard-1/guest-ctl3d-review.md). Guest source-to-image provenance and application behavior remain open.

The [WOW USER header family](shard-1/wow-user-header-family-review.md) and [core header family](shard-0/wow-header-family-review.md) record declaration paths and current build dependencies, without proving callback behavior. [GRAPHICS image lineage](shard-3/graphics-exc-lineage-review.md) relates a retained intermediate to the installed COM bytes; [DOS build-tool inputs](shard-2/dos-build-tool-auxiliary-review.md) remain pending where their historical dependency resolution is unknown.

New source-family reviews cover [BIOS and CMOS resources](shard-2/bios-family-resource-review.md), [bin86 guest artifacts](shard-1/bin86-runtime-family-review.md), and [DDEML test resources](shard-3/ddeml-client-test-resources-review.md). Published ROM content and historical test inputs do not prove runtime or source-to-image behavior.

The [WOW multimedia bridge and resource thunk review](shard-0/wow-media-resource-image-review.md) binds ten original function bodies to released executable code; their guest dispatch and lifecycle remain separate obligations.

The [C-VID declaration family](shard-1/cvidc-header-family-review.md), [generated carriers](shard-3/cvidc-generated-carriers-role-review.md), and [VDD sample family](shard-2/vdd-sample-family-review.md) retain separate profile, provenance and runtime boundaries; missing current graph dependencies do not exclude device capabilities.

The [WOW message table review](shard-0/wow-message-table-image-review.md) binds all 1,024 initial native-to-guest message table pointers to the released DLL; runtime dispatch and message behavior remain open.

The [SoftPC header family](shard-1/softpc-header-family-review.md), [standalone VDM utilities](shard-2/vdmutils-family-review.md) and [WIN87EM build inputs](shard-3/win87em-build-input-role-review.md) distinguish selected objects, standalone tools and guest-image identity from runtime proof.

The [disabled multimedia structure sources](shard-0/wow-multimedia-disabled-structures-review.md) retain their original inactive file-body roles; multimedia behavior remains in scope.

Additional bounded reviews cover [SoftPC host declarations](shard-1/softpc-host-header-family-review.md), [WOWFAX inputs](shard-2/wowfax-family-review.md) and [Win16 multimedia inputs](shard-3/multimedia-guest-input-role-review.md). Their source or graph evidence does not establish runtime device and callback behavior.

The [host declaration dependency cohort](shard-0/softpc-host-headers-review.md) binds a dependent worker body for each of 13 selected headers; three other candidates remain pending. This is not device behavior acceptance.

Parallel source reviews add [DOS locale data](shard-1/dos-locale-family-review.md), [COMMDLG resource identity](shard-2/commdlg-ne-resource-review.md), and [SYSTEM driver variants](shard-3/wow-system-variants-role-review.md). Matching resources and guest binaries do not prove guest execution or source-to-image provenance.

The [shared MVDM declaration family](shard-0/mvdm-inc-header-family-review.md) links selected dependent objects to the three published NTVDM, VDMREDIR and WOW32 images; behavioral obligations remain open.

The [DOS guest header family](shard-1/dos-header-family-review.md), [multimedia source family](shard-2/multimedia-family-owner-review.md) and [TOOLHELP inputs](shard-3/toolhelp-guest-input-role-review.md) record selected guest build chains and unresolved image, provider and runtime boundaries.

The [COUNTRY data comparison](shard-0/country-bin86-identity-review.md) identifies nonidentical original and installed country/code-page indices; actual guest selection and semantics remain open in S12.

Further bounded reviews trace [DOS utilities](shard-1/dos-utility-family-review.md), [WRITE DBCS build selection](shard-2/write-dbcs-family-review.md), and [RASAPI16 inputs](shard-3/rasapi-guest-input-role-review.md). Original recipes and retained images do not establish live guest behavior or provider availability.

The [guest stub and WFWNET review](win16-guest-image-payload-role-review.md) records bounded deployed-byte identities; the [VDD sample family](shard-1/vdd-sample-family-review.md) preserves separate package and external-provider boundaries.

The [OLE server image review](shard-0/ole-server-image-review.md) matches the original retail image to the guest archive while recording the absent exact installed path; actual loading and OLE behavior remain unverified.

The [PIFEDIT/FORCEDOS](shard-1/pifedit-forcedos-family-review.md), [SYSTEM NE](shard-2/system-ne-family-review.md), and [WRITE alternate source](shard-3/write-alternate-owners-role-review.md) reviews retain separate editor, driver and guest build-profile boundaries.

The [guest package image family](shard-0/guest-package-image-family-review.md) compares five original DOS and Win16 images with the archive and exact installed paths; load and execution remain unverified.

The [USER and timer image review](guest-user-timer-image-role-review.md) records bounded installed/archive identities and NE timer exports. [Win16 common headers](shard-1/win16-common-headers-review.md) and [GDI/dialog inputs](shard-3/wow-gdi-dialog-input-role-review.md) remain source/build evidence, not guest runtime proof.

The [DOSX x86 input cohort](shard-0/dosx-x86-inputs-review.md) reconciles twelve historical guest-build roles while retaining all DPMI and protected-mode runtime obligations.

The current [nine-way assignment](nine-way-plan.md) freezes all remaining MVDM file roles and external relevance rows, without changing the original physical ledger ownership or claiming those rows are resolved.

The first [nine-way bin86 image cohort](nine-way/part-1/bin86-review.md) records nine original guest binaries matching the archive; their installed search paths and runtime behavior remain separate obligations.

The [non-ROM read failure fixture](rom-read-failure-review.md) demonstrates an original-source oversized-input close omission with simulated file I/O and binds its reader to the released worker; actual session reachability remains open.

Pre-partition reviews also record [ROM/CMOS resource identity](shard-1/rom-cmos-family-review.md), [SOUND import metadata](shard-2/sound-import-family-review.md), and [kernel bootstrap inputs](shard-3/kernel-boot-build-input-role-review.md), without guest runtime acceptance.

Additional reviews cover [486 profile source gates](shard-1/ccpu-486-family-review.md), [original debugger tools](shard-2/debugger-tools-family-review.md) and [Win16 debug declarations](shard-3/wow-debug-input-role-review.md). Current nonselection or original unimplemented entries do not establish full feature recovery.

The [DOS kernel build records](shard-1/dos-kernel-build-family-review.md), [DDEML test/version inputs](shard-2/ddeml-test-version-review.md) and [WOW USER guest inputs](shard-3/wow-user-guest-input-role-review.md) distinguish historical recipes from actual guest runtime evidence.
