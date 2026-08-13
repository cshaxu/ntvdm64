# Project Status

## Current Work

**Binding active packet: M0 T188 S4 — copied session-state diagnostic design.**

**Active: M0 T176 S22 — normal-return and post-termination machine-BOP recovery.**

> **Governance correction:** The title is a retained encoding-damaged locator.
> The binding active packet below is **M0 T188 S4**. M0 T186 and T187 are closed; all pre-admission T177/T178/T179/T180-labelled files
> files are historical evidence locators and do not allocate a current task;
> the active packet is defined only by the table below.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T188 S4, Ordinary Mode. |
| Admission And Approval | S3 proves no existing general state observer and rejects all transfer/reset accessors. Owner authorization permits autonomous sequencing; S4 admits design and static validation of one adapter-owned copied diagnostic ABI before any Bochs source/build or runtime action. |
| Objective | Specify the smallest layout-neutral adapter state snapshot that can distinguish installed/provider/pending lifecycle state after generic dispatch, without exposing pointers, BOP/service identity or mutable session contents. |
| Non-goals | No Bochs/CLI/engine change, build, guest observation, direct session-layout export, selector/service field, state mutation, condition reordering, provider/DOS/COMMAND semantic change, host capability or completion claim. |
| Reference Baseline | T188 S3 finds the runtime session private and all generic pending transfers consuming; T188 S2 proves the static route succeeds. |
| Files And ABI Surface | Adapter diagnostic header/source and focused static test/evidence only; no Bochs source in this S. |
| Applicable Rules | rules/EXECUTION.md, rules/ARCHITECTURE.md, rules/CODING.md, rules/DOCUMENT.md, design/ARCHITECTURE.md, design/CODING.md, and etc/operations/policy/source-policy.md. |
| Verification | Define fixed-width validity rules and static tests that reject pointers, service/selector fields, state-changing calls and direct `host_session` layout use; run documentation inventory/governance and `git diff --check`. |
| Expected Markers | One copied record with versioned fields sufficient only for lifecycle classification, plus explicit non-mutation proof. |
| Asset Needs | Current adapter runtime/session source and static adapter test island; no Bochs build or runtime input. |
| Reporting Requirements | Separate adapter-state ownership from BOP/service semantics. Record the future Bochs trace as a registered, default-off consumer of the copied record only. |
| Stop Conditions | Any Bochs/CLI change, build or guest run; any pointer/selector/service/memory content in the record; any state mutation or production behavior change; these require re-admission. |
| Exit Criteria | Source/test closed copied-state ABI or a design rejection; no Bochs or guest action. |
| Original Owner Request | Holistic BOP recovery with original OpenNT semantics, a minimum Bochs boundary, non-invasive CLI capabilities, and no one-off patches. |
| Similar-Issue Sweep | Audit all #UD consumers in `exception.cc`, including listener, startup transaction, CPU-result bridge, deferred plan, generic interceptor and machine composition; reject selector recognizers, parallel startup paths, direct Bochs/DOS integration and any runtime retry. |

## Current Technical Baseline

- T95 is closed by owner-authorized reprioritization. Its incomplete file/search
  capability and full real-path closure remain uncompleted.
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
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |
