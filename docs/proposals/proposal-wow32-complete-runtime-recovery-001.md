# WOW32 complete runtime recovery

## Superseded execution plan

Owner closes T422 on 2026-09-24 as initial WINMINE capability and planning
preservation, not complete WOW32 acceptance. This document is retained design
and research provenance. All unfinished S3--S22 execution moves to S1--S20
of the [new successor candidate](proposal-wow32-production-completion-002.md),
which owns the live plan and final acceptance. Old admission/frontier/numbering
statements below are historical; no T422 S remains active after this handoff.

## Objective and admission

Owner-directed WOW32 successor to T420, dated 2026-09-21. The revised queue
places DLL compilation-boundary reorganization first and this candidate second.
This changes execution order only; the audit-first and three-application
acceptance requirements remain unchanged. This candidate
has no numeric T identifier until admission. Recover the complete selected
WOW32 provider and its immutable WOW16 consumers, beginning with a complete dependency/contract audit and ending with real
WRITE.EXE, WINMINE.EXE and SOL.EXE startup, use and exit. Inherit all unfinished WOW32 work and research from
T420 S40--S42 and all former S43--S46 obligations; none becomes a passing
result merely because its task owner changes.

T420 retains debugger, VDD and non-WOW32 aggregate acceptance. Its S42 closes
only as a reviewed research/code handoff with DOS regression, not W1/W2 or
WRITE acceptance. The successor must review retained implementation candidates
in place before extending them; it must not restart the research or treat a
saved source file, a linked symbol or a native fixture as a completed feature.

## Research and implementation inheritance

These records form the complete starting reading set, including their linked
source hashes, original owners, test procedures and superseded hypotheses:

- [S40 research conclusion](../etc/evidence/m0-t420-s40-wow32-research-replan.md).
- [S40 USER profile and experiments](../etc/evidence/m0-t420-s40-wow-user-profile.md):
  pinned immutable USER.EXE, 47 consumer patterns, 26 ordinary-thunk mappings,
  PMODE32 versus thunk paths and the 21-input/20-output registration ABI.
- [W1--W7 detailed checklist](../etc/operations/m0-t420-s40-wow32-closure-checklist.md).
- [S41 worker-domain evidence](../etc/evidence/m0-t420-s41-wow-registration-frontier.md),
  delivered at `801ec6e39`; adoption baseline `158f1f1a6` admits S42.
- [S42 original-owner and binding ledger](../etc/evidence/m0-t420-s42-w1-w2-lifecycle-binding-ledger.md):
  original task/class/window owners, registration slots, code and artifact
  identities, observations and explicit remaining production gaps.
- [S42 handoff and workspace disposition](../etc/evidence/m0-t420-s42-wow32-successor-handoff.md).
- [Earlier matching WOW32 provider](../etc/evidence/m0-t404-s5-wow32-matching-provider-build-001.md),
  [positional startup](../etc/evidence/m0-t404-s4-positional-wow-bootstrap-run-001.md),
  [P15 entry recovery](../etc/evidence/m0-t404-s3-p15-positional-ne-wow-entry-recovery-001.md),
  [hard-error recovery](../etc/evidence/m0-t404-s1-wow-hard-error-recovery-001.md)
  and [original command carrier](../etc/evidence/m0-t416-s2-wow-command-carrier-restoration.md).
  Historical execution depth is comparative evidence, not current acceptance.
- [S37 OEMUNI obligations](../etc/evidence/m0-t420-s37-oemuni-capability-review.md),
  [S38 DPMI32](../etc/evidence/m0-t420-s38-dpmi32-capability-review.md) and
  [S39 DOSX](../etc/evidence/m0-t420-s39-dpmi-guest-capability-review.md).
- [Source policy](../etc/operations/policy/source-policy.md),
  [MVDM divergence register](../../src/mvdm/README.md),
  [OpenNT-host provenance](../../src/opennt-host/README.md), and
  [worker WOW boundary](../../src/ntvdm-exe/wow/README.md).

Research artifacts remain under `build/M0-T420/S40`, `S41`, `S42`; runtime
observations remain under `O:/winnt/logs`. These disposable roots are not the
only record: tracked evidence and test sources carry conclusions, hashes and
reproduction requirements. Rebuild from committed sources before adoption.

## Proven frontier and unresolved production contracts

The local registrar is selected and its 20 outputs are non-null, but some are
explicit failure placeholders. Modern USER32 registration returned C0000022
in the retained probe. The worker uses the S41 guest-visible CCPU/DOSX domain;
native USER pointers cannot substitute for the pinned guest's data layout.
Original USER structures and algorithms remain the source of truth.

The protected-return GDT repair and checked USER build-layout identity have
advanced startup. The current proven failure is USER GetDesktopWindow at
0337:011F: guest TEB pDeskInfo and client delta are zero; desktop+8 therefore
reads DOS IVT bytes 055A0016, followed by an invalid WND dereference. It is a
missing host client-view publication, not evidence of a guest or CCPU bug.
The exact r20 witness and instruction signatures are in the S42 ledger and
`tests/observation/verify-wow-user-profile.ps1`. WOWInitTask is ordinal 011F;
010F is DosWowInit. A 010F trace is not proof of task initialization.

Outstanding work, each assigned below:

- Complete original-layout desktop/WND/handle graph, authoritative live
  ownership, relocation, updates, cache invalidation and retirement. The
  recovered WND=176 and DESKTOPINFO=100 layouts are schema proof only.
  `set_client_desktop` is a tested transport without a production producer.
- Replace the temporary active-guest rejection of identity-only handle
  publication with complete object publication. The rejected implementation
  left a free-list successor in phead while reporting a live typed handle.
  Neither that old success nor the present refusal is functional acceptance.
- Connect task/thread/worker destruction to the original lifecycle owners.
  The cleanup helper's failure/retry fixture passes, but cleanup/dispose have
  no production caller. WOWCleanup also serves module release, so it cannot
  unconditionally destroy a live thread. Inspect worker detach ordering
  before memory release, callback cancellation and per-thread withdrawal.
- Finish real guest callback, scheduler, message, class/window/dialog paths;
  native tests with substituted callbacks do not establish guest closure.
- Finish graphics/resources/fonts/menu publication, clipboard/DDE, remaining
  thunks, hard errors and all registrar placeholders. Reuse already recovered
  bodies and fixtures after current-source rebuild, rather than duplicating them.
- Prove both selected PMODE32 direct-data consumers and ordinary thunk
  consumers; immutable guest compile-time selection cannot be changed by a
  host compiler flag. No unsupported alternate guest profile is implied.

## Capability scope and architecture boundary

The original [sources manifest](../../src/mvdm/wow32/sources) selects 77 C
translation units; [wowtbl.c](../../src/mvdm/wow32/wowtbl.c) assembles ten
dispatch-table families. The scope includes initialization/BOP/thunk dispatch,
16/32-bit structures and aliases, callbacks, KERNEL/DOS and module/memory/file
services, USER classes/windows/messages/dialogs/input/hooks/timers, GDI
drawing/DC/fonts/DIB/metafiles/printing, resources, clipboard/DDE, Shell,
Winsock, ToolHelp, sound/multimedia and common-dialog/related OLE interfaces.
The USER 21-input/20-output registration is one dependency surface, not the
whole WOW32 package. WRITE coverage alone cannot define selected functionality.

Original MVDM conversion and execution policy stays in mvdm/wow32. Required
non-MVDM original algorithms, layouts and WOW-specific policy belong in
opennt-host at original paths. Worker-local bindings supply native resources,
CCPU-visible backing and finite unavailable host mechanisms. Modern Windows
continues to own native windows, drawing and queues; no parallel USER server
or adapter-owned replacement scheduler is admitted.

### Shadow-registry receiver

The product reads every admitted Windows Registry root only through the
worker-local layered registry overlay; it never writes the system Registry.
`NTVDM.REG` is consulted first and overrides the admitted read-only host value.
The successor reuses that provider rather than adding a WOW-only Registry
facade. Its initial audit enumerates every selected WOW registry consumer and
its root. Read-only configuration consumers (including PMAP_COMPAT/profile
mappings) retain their original reader and bind it to that same-shaped provider.
A missing key/value in both layers reaches the original caller unchanged, which
alone determines the original default or failure.

The Win16 Shell `RegOpen/Create/Set/Enum/Delete` family is not a configuration
read. If selected consumers require it, S14 extends the same provider with a
finite worker-local mutable shadow tree seeded by the layered read view; every
read, creation, write, deletion and enumeration uses that tree, never writes
HKLM/HKCU. Successful creates, sets and deletes atomically commit only the
product `NTVDM.REG` overlay, including tombstones that hide a deleted host
key/value. S14 must prove close, failure, enumeration, task cleanup,
cross-worker serialization and persistence. It must not reduce these APIs to
unconditional defaults or a read-only parser.

The original `wow32/wshell.c` route is the behavioral reference: its seven
Win16 SHELL thunks call the historical Win32 Registry, translate errors through
`ConvertToWin31Error`, apply `Remove_Classes` for the default classes root, and
implement recursive deletion because Win3.1 permits deleting a non-empty key
where the Win32 API did not. S14 preserves those original bodies and binds their
Registry calls to the shared tree. A native pointer/handle must not be written
into guest memory: the existing DWORD Win16 HKEY slots use a bounded
worker-local surrogate table with stale-handle rejection and task teardown.

The immutable PMODE32 guest also reads objects directly. Audit both this data
ABI and ordinary thunk calls, including all 26 identified mappings and 47
pinned consumer checks; expand that inventory if complete-source review finds
more. A host compiler flag or CallCsrFlag cannot redirect all immutable guest
reads. One logical object lifecycle must own native identity and its original
guest representation, with updates visible during creation callbacks, changes,
destruction, handle reuse and thread switching.

The inherited ntuser mirror currently has 22 files. The S42 handoff added 19
USER files and two other original carriers, often selected subsets; these
counts are inventory, not proof of sufficient dependency closure. S1 must
determine the complete additional file/function ranges and their size.
Do not promise a final import count before that audit or recursively import
all USER/GDI/CSR merely to satisfy unresolved symbols.

## Shared-data and execution contract

Recovery preserves the existing Win16 guest algorithms and original WOW32
conversion code. NTVDM/CCPU executes guest instructions on every route;
direct USER16 memory reads bypass WOW32 dispatch, not the worker. Original
NT shared selected USER data with its clients; it did not expose arbitrary
kernel memory. A modern host pointer is usable only through a proved mapping
and compatible layout/lifetime, never merely because it fits in 32 bits.

S1 records these three routes separately. A passing route cannot stand in for
another:

| Route | Required original contract and evidence |
| --- | --- |
| Guest to WOW32 to host API | Original thunk conversion, guest-buffer access, handle identity, return/error and release. Reuse existing original bodies and guest-memory bindings. |
| Host to guest callback | Original CallBack16, CCPU entry/return, task/thread context, nested calls and cancellation/teardown. A substituted native callback is only a fixture. |
| Guest direct shared-data access | Original consumer instructions, mapped addresses and layout, authoritative data producer, publication/update ordering, permissions and object lifetime. A non-null callback table does not prove this route. |

For each consumed field or coherent field group in SHAREDINFO, SERVERINFO,
HANDLEENTRY, CLIENTINFO/TEB, DESKTOPINFO, WND, CLS, MENU and shared time data,
S1's ledger must identify the original consumer and owner; authoritative
native object/API or WOW-owned state; guest representation and address
relocation; read/write direction; creation, mutation and invalidation events;
and the synchronization and storage-lifetime contract. Preserve any original
guest-writable state explicitly; do not assume the whole view is read-only.
Public API availability is a candidate mechanism, not proof that its result
matches the historical field or its update ordering.

The update design must cover host-originated changes and guest execution that
does not enter a thunk: window movement, resizing, style/parent/owner changes,
menu mutation, cursor position, system settings and clock progression as
required by the selected consumers. State exactly when each update becomes
observable and how consistency is preserved. Refreshing only at BOP/message
entry or periodically is insufficient unless evidence proves the required
observable contract. Callbacks must see required creation state before entry;
reentrant mutation/destruction must not expose partial objects or free storage
still borrowed by an active caller. Prove handle reuse, cache invalidation,
thread switching and withdrawal before backing release.

S1 must also settle object coverage: actual desktop, native/system controls,
other WOW tasks and externally owned windows or menus reachable through the
original APIs. Neither an own-created-windows-only assumption nor an
unbounded copy of the host desktop graph is an accepted default. Name each
required relationship, identity rule and observable failure contract.

Modern Windows remains authoritative for native window/menu operations,
drawing, input delivery and native queues. Original WOW algorithms own the
Win16 compatibility policy; guest-visible records represent those same
objects. Audit reusable original semantic slices by their finite dependencies,
including slices located under ntuser/kernel; directory naming alone does
not decide composability or authorize import. Existing source-policy stopping
boundaries still prohibit recursive USER/CSRSS runtime import. A missing
contract must not grow into an autonomous window manager, input router,
drawing engine or replacement NT scheduler.

## Revised sequential S packets

Owner-directed regrouping on 2026-09-24 supersedes the former S2--S8
allocation. S1 remains closed. S2 closes only as preserved implementation/
research and planning handoff, not complete USER acceptance. Status alone
admits the active S; the sequence below does not create concurrent packets.

| S | Complete scope | Acceptance and inherited owner |
| --- | --- | --- |
| S1 | Completed whole-package audit | Preserve its 77-source, ten-table, 21/20 registration and direct-data inventory; no audit restart. |
| S2 | Preservation and planning handoff | Preserve committed code/tests and E1--E95 evidence, reconcile worktree, transfer every C01--C12 obligation, verify documentation, commit/push. No new product or guest pass is claimed. |
| S3 | Messages, callbacks and task execution | C01--C03 plus C09 and execution-related C10: native send/post/reply, early/duplicate reply, waits, cancellation, reentry, original task init/yield/hung registration, task loss and callback frame/lease cleanup. Modern USER alone owns transport. Prove real guest and native-peer interactions, not only the E92/E93 fixtures. |
| S4 | USER objects and shared view | C04--C07 plus object-related C10: desktop/WND/CLS/handles, registration, publication/update/direct reads, stale reuse, geometry, mutation, nested destruction and rollback. Own-created, system/native and other-task object contracts are explicit. Objects do not close without their cleanup. |
| S5 | Modules, memory and resource aliases | Former S6 loader/memory slice: original module/loader-facing services, allocation/rollback and alias lifetimes, module release without live-task destruction. Consume S3 execution and S4 identities. |
| S6 | Files, directories, environment and OEM | Former S6 file/OEM slice: original operations, both OEM-WOW-DIR directions and OEM-WOW-DELETE branches, rename/delete rollback, retained-file/font fallback and real non-ASCII guest consumers. |
| S7 | Resource discovery and loading foundation | Original lookup/load/lock/unlock/free and module association; finite aliases owned by S5. Prove original guest names/IDs, bounds, missing/partial loads and lifetime before GDI/menu consumers. No resource-specific drawing or duplicate memory manager. |
| S8 | Dialogs, input, hooks and timers | C08 and former S7 USER families: real dialog init/control/procedure/cancel/release; selected input, caret, keyboard, hook and timer operation families, callbacks and task-loss cleanup. Reuse S3/S4, never a second input router or scheduler. |
| S9 | GDI identity, DC and drawing | Former S3 foundation: bounded original-shape handle carrier, stock/select/delete/release, transforms, drawing, bitmap/DIB and palette. Verify original current-slot reuse semantics, positive/negative content and resource cleanup; no invented generation/type policy. |
| S10 | Fonts, text and metafiles | Former S3 remaining families: font/text conversion and drawing, font lifetime, metafile creation/playback/release and failure paths; consume S9 identities. |
| S11 | Resource conversion, menus and accelerators | Former S4 consumer slice: conversion, icon/cursor/bitmap loading, menu graph/owner-draw mutation, accelerators, partial-load rollback and release. Consume S4 publication and S9 bitmap services, not duplicate providers. |
| S12 | Clipboard | Former S5 clipboard slice: format conversion, delayed rendering, ownership transfer, rejected/abandoned data and task loss with real guest/native peers. |
| S13 | DDE | Former S5 DDE slice: conversation, data/reply, reentry, cancellation, peer/module/task death and release. Original FreeDDEData alone is not acceptance. |
| S14 | Shell and layered Registry | Former S7 Shell family: all selected Shell thunks; shared NTVDM.REG read/create/set/enum/delete/close, original error conversion/recursive delete, tombstones, atomic persistence, cross-worker serialization and cleanup; never write system Registry. |
| S15 | Winsock | Former S7 network family: selected socket operation/conversion, async notification, failure/cancel/peer loss and task cleanup, real guest/local controlled peer tests. |
| S16 | COMM | Former S7 serial family: selected open/config/read/write/status/events/close, cancellation and task cleanup. Hardware absence requires exact retained unavailable result, TODO and protocol-accurate mock, not a success stub. |
| S17 | Printing and spool | Former S7 print family: selected guest print/spool operations, conversions, job/DC lifecycle, failure/cancel/cleanup using S9/S10; document any unavailable external device with mock evidence. |
| S18 | Sound and multimedia | Former S7 multimedia family: selected operation families, callbacks, buffers, async completion/cancel and device/task cleanup. Track family-specific tests rather than link-only acceptance. |
| S19 | ToolHelp and WOW debugger interfaces | Former S7 ToolHelp/debugger slice: enumerate/query and selected callbacks/notifications, OEM-DBG-PATH WOW receiver, stale identity, task loss and release; preserve completed non-WOW debugger evidence. |
| S20 | Common dialogs and related OLE interfaces | Former S7 related UI/OLE slice: selected conversion, callback, cancel/error and ownership contracts. Use S8 dialogs and completed resource/service owners; enumerate both families separately and close every selected edge. |
| S21 | WOW hard-error responses | Former S7 ERROR-01: original response mapping, cancellation, fatal task/worker tail and cleanup. Reconcile the queued general error-dialog proposal without duplicating its provider or silently excluding WOW callers. |
| S22 | Whole-provider and three-application acceptance | Former S8: final same-artifact WRITE/WINMINE/SOL scenarios, repeated/concurrent tasks, abnormal cleanup, integrated OEM and subsequent DOS usability. Reconcile C11/C12 and all registration/dispatch/direct-data edges; no implementation backlog is deferred here. |

C11 (edge/slot/minimal-diff review) and C12 (build, guest tests, DOS17,
commit/push) apply to every implementation S for its own slice. S22 aggregates
their evidence and repeats integrated acceptance; it cannot accept unimplemented
predecessor edges. CORE-INIT-01 is distributed: each new owner immediately
wires its initialized slots and rollback/teardown, with S22 reconciling the
complete table. C10 cleanup follows each resource owner, not a late cleanup S.
Each complete owner keeps positive, negative and lifetime tests; a prerequisite
in a later row is promoted explicitly and integrated before the consumer closes.

The [coverage ledger](../etc/operations/wow32-recovery-coverage-ledger.md)
preserves stable source/edge IDs and maps old assignments to these receivers.
The [S2 handoff](../etc/evidence/m0-t422-s2-planning-handoff.md) preserves the
C checklist and known failures. Do not restart valid research or count an
old native fixture as real guest acceptance.

### Mandatory inherited gates

- S4 tests direct guest reads after host-originated changes without a WOW32
  thunk, creation callback visibility, external/native objects and withdrawal.
- S9 owns the retired NT GDI shared handle-table boundary and every selected
  HDC/HBITMAP/HBRUSH/HFONT/HMETAFILE/HRGN/HPALETTE/HPEN/HGDIOBJ producer,
  consumer and retirement site. Retain bounded 14-bit current-slot behavior:
  zero maps to NULL; successful native release retires; later allocation may
  reuse. Native GDI remains the type validator, not an invented guest policy.
- S11 owns menu/item/submenu relationships and creation-time visibility.
- Package-path/bootstrap work inherited from S2 remains preserved: WIN16DIR
  derives from the executable's installed sibling directory and enters only
  the original BaseCreateVDMEnvironment child block. Never hardcode O:/winnt,
  change host SYSTEMROOT or silently suppress original network.drv failure.
  Any still-needed bootstrap prerequisite belongs to S3, not an orphan.
- Every S immediately wires completed inputs/outputs; typed failure placeholders
  can represent only named later owners, never a completed owner's capability.
- Modern USER is the sole message transport for all Win16/Win32 directions.
  Local SMS delivery was rejected; historical private identity assertions are
  not product gates. Original task algorithms remain behind finite bindings.

### WINMINE visual findings received from S2

E53 preserves owner-confirmed gameplay, not whole-provider acceptance.

- S9 investigates monochrome rendering: guest DC BITSPIXEL/PLANES/NUMCOLORS,
  requested/native bitmap formats and actual pixels, including valid monochrome
  resources. No fabricated capabilities or forced coloring.
- S11 owns resource-selection/load: paired color IDs 410/420/430 (4 bpp) and
  monochrome IDs 411/421/431 (1 bpp), actual production LoadBitmap/registration,
  conversion failures and original fallback. S9/S11 share one bitmap pipeline.
- S4 owns small/cropped-window geometry, metrics, client coordinates and
  resize/callback correctness. Historical small dimensions alone are no bug.
- Owner-accepted Chinese mojibake in that observation is not permission to
  mutate guest or host locale/Registry, nor waiver of S6 OEM contracts.
- S22 repeats final integrated display/input/exit tests for all three apps.

### Dependency and single-owner rules

This dependency-first revision is the owner's additional S2 planning P;
S3 admission is preserved but implementation resumes only after this S2 P.
S3/S4 initially use S2's retained working foundations, not assumed complete
objects. Before their tests, verify the exact prerequisite paths. Promote
a demonstrated missing prerequisite by named capability/edge; do not build
a temporary substitute or absorb the whole later package.

| Consumer | Required completed contract | Unique implementation owner |
| --- | --- | --- |
| S3 message/callback execution | Existing window/task identity and CCPU lease paths used by its tests | S3 owns execution/frame mechanics; S4 owns object publication. S3 proves callback round-trip, not every object's behavior. |
| S4 USER objects | S3 callback/execution contract and retained module/lease foundation | S4 owns WND/CLS/handle graph including destruction; module/alias mechanics remain S5. |
| S7 resource foundation | S5 module/memory/aliases and S6 file access | S7 owns lookup/load/lock/free policy; S5 owns shared alias allocation/lifetime; no second resource handle table. |
| S8 dialog/input | S3 execution, S4 objects, S7 template loading | S8 owns dialog/hook/timer semantics and cancellation, consuming S3 callbacks. |
| S9/S10 GDI/text | S5--S7 foundational services, S4 required identities | S9 owns native GDI handle carrier and bitmap conversion; S10 consumes it for fonts/text/metafiles. |
| S11 menus/resources | S7 loading, S9 bitmap services, S4 object publication | S11 owns menu/icon/cursor/accelerator behavior; no duplicate loader or bitmap implementation. |
| S12/S13 clipboard/DDE | S3 message/reply and completed data/handle services | S12 owns shared clipboard format/data transfer primitives where source ownership proves sharing; S13 owns DDE conversation/reply and consumes them. |
| S14--S21 services | Their specific previously completed callback/resource/service contracts | Each family owns its normal/failure cleanup; S21 does not defer another family's error or cleanup behavior. |
| S22 aggregate | All owner-specific accepted contracts | Integration only, not unfinished wiring or repair collection. |

The existing S1 source/edge ledger supplies function-level ownership;
physical source-file sharing does not authorize duplicate implementation.
Named prerequisites are planning contracts, not a claim every runtime edge
has been reverified in this documentation pass. Record counterevidence and
update the bounded edge, not a new whole-project audit.

Each prior S implements the original error/rollback/termination behavior
needed by its own calls immediately. S21 covers remaining WOW hard-error
entrypoints and aggregate response semantics, never an excuse for an earlier
false success or unsafe cleanup. Winsock/COMM/printing/multimedia remain
required selected-family scope, but are not assumed startup prerequisites of
WRITE/SOL/WINMINE without a demonstrated call edge. The per-P three-app
matrix continues throughout, not only after these service packages.

## Audit deliverables and finite import plan

S1 starts from the retained research and tests, recording what remains valid
and what was disproved. It must inspect all selected entrypoints, not only
those reached by the three applications. Include indirect callbacks, function
tables, dynamically resolved imports, data/global readers and all initialization,
mutation, error, thread/module/task and worker termination edges.

For each external edge, freeze: original caller and owner path/function/hash;
current production selection; directly composable original source or exact
unavailable dependency; original ABI/layout/order/error/lifetime contract;
smallest binding owner; imported file/function range; current duplicate to
delete; responsible S and concrete positive/negative/cleanup test.

Classify every edge as original source reuse, original source with finite
binding, available modern mechanism retaining original policy, or evidenced
external unavailability. "Unknown", generic "wiring later", a symbol hit or
a successful fixture is not a completed disposition. Runtime verification
may remain pending with a named owner and executable test plan; S1 must not
invent certainty that requires later execution.

Produce complete grouped import and removal lists with separate counts for
new original files, expanded subsets, retained mirror modifications and
adapter code. Import an entire directly composable original unit where possible;
otherwise select its complete required functional slice with a finite outgoing
boundary. Batch by the dependency graph and original ownership, never by the
next crash, missing symbol or individual API. Source recovery precedes extending
an autonomous replacement. A discovered omitted dependency triggers an explicit
ledger/design correction, not another silent one-off implementation.

## Per-S ledger and checklist governance

Start S1 from the prepared [coverage ledger](../etc/operations/wow32-recovery-coverage-ledger.md)
and [S1 execution checklist](../etc/operations/wow32-recovery-s1-checklist.md).
They seed the 77-source, ten-table, 21/20-callback and 26-mapping inventories
with existing evidence and stable IDs. Reconcile them against the admission
revision and expand their entry/field dependencies; preparation ticks are not
audit or runtime acceptance. Preserve IDs and record reasons for regrouping.

Each S maintains one indexed supporting evidence record under docs/etc/evidence
(or a linked bounded ledger/checklist pair). The T-wide dependency matrix is
the cross-S index; records link to original evidence instead of copying long
logs or introducing a competing status/queue. Create the S inventory at
admission, update it during work and review every row before closure.

Each stable row records:

- Checked-in test source/script under tests/, exact entrypoint/case/arguments,
  prerequisites and expected assertions. Disposable experiments alone do not
  satisfy this deliverable. Tests accompany implementation in the repository.
- Proof that the tested caller/provider is the selected production path,
  including real guest calls, callbacks/direct reads as applicable, failure
  and cleanup. Substituted providers, trace hits and unused wrappers are not
  production acceptance; missing tests keep the capability checkbox open.

- Capability/entry and original source/hash, including guest data consumers.
- Required dependency, responsible owner/S and current production caller.
- Disposition: reuse, finite binding, replace/delete, or justified exclusion.
- Implementation and build-selection location; exact retained divergence.
- Positive, negative, reentrant/asynchronous where applicable, and cleanup
  tests, commands, source revision, artifact hashes and observed output.
- Separate status for source recovery, build, production wiring, focused
  tests, real guest execution and teardown; evidence links and remaining work.
- Completion decision and, on reopening, the new counterevidence and reason.

A checkbox is complete only when its stated production contract and acceptance
are proved. Compilation, non-null slots, host-only fixtures and progress to a
later crash have distinct labels. No success stub or unbound cleanup may
remain in a closed implementation scope. Missing external media and approved
immutable-guest limitations remain explicit non-pass outcomes with TODO and
mock coverage where feasible; they do not silently waive the final applications.

Before repeating research, consult the row and its preserved evidence. Reopen
only for changed source/artifact/profile, a failed test or contradictory
evidence, recording the reason. Reuse correct implementations and reproducible
tests. Batch focused testing at completed owner boundaries; run the mandatory
DOS suite at each implementation S closure and after changes that justify it.
After every reviewed delivery, commit/push evidence and source together and
verify a clean worktree. No new S starts by rediscovering the previous S's
unrecorded decisions.

## Final three-application acceptance

S1 locates and pins provenance/hash/version of the original immutable
WRITE.EXE, WINMINE.EXE and SOL.EXE plus their required guest media/resources.
Missing media is a recorded prerequisite, never permission to substitute an
application, patch media or declare a pass. Launch through the ordinary
run16 product entry without diagnostic switches changing behavior.

| Application | Required visible and functional outcomes |
| --- | --- |
| WRITE.EXE | Start to an interactive document; type/edit/select text, use menus/dialogs, save to a disposable file and reopen to verify content, repaint/resize, close and restart. Verify save failure/cancel and task/resource cleanup. |
| WINMINE.EXE | Start a real board, reveal cells and flag/unflag with mouse input, observe timer/counter and repaint, use menu/difficulty/new-game operations, close and restart with no stuck input/callback. |
| SOL.EXE | Start/deal, select and move a legal card by supported mouse interaction, use draw/new-game and applicable options, repaint/resize, close and restart without stale cards/resources or callbacks. |

All three are mandatory; window appearance or process exit alone is not
acceptance. Capture observable content/state and actual interaction outcomes,
not merely an exit code. Repeat task launch/use/exit, supported concurrent
task interactions and controlled abnormal cleanup, then run COMMAND/MEM/EDIT
on the same artifact set. Test guest task/message/resource release as well
as worker/broker survival and exit semantics. The selected-family suites must
also cover capabilities these applications do not exercise.

S22 owns the integrated OEM-WIN16-INTEGRATION retest and final consolidated
report. Failed implementation contracts reopen their source-owner ledger and
receive regression tests; S22 must not become a backlog of wiring left by
earlier S packets. T closure requires the owner's final acceptance audit.

## Mandatory gates and limits

The [every-P gate](../rules/EXECUTION.md#every-p-regression-and-side-test-publication-gate)
applies before every production-code P, not just S closure: capability tests, DOS17, previous
P's deepest verified behavior and retained capabilities, followed by coherent
publication of four EXEs and two DLLs plus required original guest/configuration
to O:/winnt before commit/push. Pure documentation P commits require only
documentation governance, link and diff review/checks; no runtime tests or
redeployment. These six files are delivery artifacts, not new components.

For production changes, republish all six tested binaries for owner side
testing, including valid unchanged artifacts. Prefer dependency-driven
incremental compilation with recorded build-input identity and cached
intermediates; rebuild affected dependents rather than blindly rebuilding the
whole tree. Preserve sealed evidence separately from mutable build caches.

### Mandatory three-application frontier matrix for every P

Every production-code P executes WINMINE.EXE, SOL.EXE and WRITE.EXE on the same
identified four-EXE/two-DLL set, through the ordinary run16 entry. This is not
deferred to S22 and is not a demand that all three become fully functional
in S3. Before full recovery, an existing failure remains a recorded failure;
the gate requires no loss of previously verified behavior in any application.

Keep one row per application per P in that S's evidence ledger, linking raw
run records below O:/winnt/logs. Use this fixed record schema:

| Field | Required evidence |
| --- | --- |
| Identity | P candidate source revision plus exact uncommitted input diff/hash, run ID, all six EXE/DLL hashes, guest/dependency and configuration hashes. Bind the final commit to those tested inputs after commit. |
| Conditions | Ordinary command, cwd, package paths, relevant environment/profile and observer actions; separate diagnostic controls from product acceptance. |
| Verified behavior | Initialization milestones; window creation and actual visible content; interaction outcomes; normal exit/restart and cleanup. Each has its own observed status, not an inferred success from the next stage. |
| Stop or failure | Exact error text, first established failing call/result or last verified boundary, exit versus live wait, trace/log references and timeout. A cause not yet established is stated explicitly; neither "did not start" nor a zero exit is sufficient evidence. |
| Comparison | Previous P row and historical best reproducible row for this application; advanced, unchanged, regressed or unverified, with specific preserved/lost behaviors and comparable input differences. |
| Disposition | Responsible capability/S, next concrete diagnostic or repair for retained failures, and gate verdict. No new regression or unverified required comparison can be accepted. |

Maintain independent WINMINE, SOL and WRITE baselines. A window appearing is
not equivalent to playable WINMINE or editable WRITE. A deeper startup trace
does not compensate for lost interaction or exit cleanup. Never reset the
baseline downward after a bad P. Historical evidence with unknown artifacts
is retained as an observation until reconciled by a controlled replay, not
discarded or falsely called reproducible. Trace a failure's cause before
assigning it to a component; errors such as "Not enough memory" do not by
themselves prove host memory exhaustion.

The final published O:/winnt package must match the tested matrix, with
ordinary configuration restored and required original guest files available.
If GUI access is unavailable, record the pending rows and continue safe
background work; do not claim a delivered P or overwrite the side-test
baseline with an unverified candidate. S22 still requires full functionality
of all three applications, not simply stable historical failures.

### Build, ownership and final acceptance

Every implementation S rebuilds formal MSVC x86 `/MT` CCPU40 targets, tests
its actual production owners, and passes all 17 text-gated direct/nested
COMMAND/MEM/EDIT routes, including EDIT followed by MEM at the same prompt.
Record exact source revision, artifact hashes, commands, markers, failures
and cleanup. Earlier green runs on different hashes are not final evidence.
Commit/push each reviewed delivery; keep build products under `build/`, test
inputs under `O:/winnt/tests`, and runtime observations under `O:/winnt/logs`.

For every family retain the source-first decision: original owner, exact
source/hash, composability, unavailable dependency, smallest same-shaped
binding, ABI/layout/ordering/failure and retirement condition. Original files
may be imported at original paths; invented mirror files/overlays, recursive
USER/CSRSS server import and parallel provider policy remain prohibited.
Guest media is immutable. Proven original-guest defects are registered with
reproducers and limitations, never silently repaired or counted as passes.

Report `mvdm` and `opennt-host` mirror additions/deletions separately, restore
format-only drift to upstream bytes, and separately measure retained/removed
adapter and other autonomous code. Importing an original file and deleting
a duplicate are different metrics. Completion requires working selected
functionality as well as minimum necessary differences. The owner performs
the final T acceptance; reaching the next startup fault does not close it.
