# WOW32 complete runtime recovery

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

The product must not read or write the Windows system registry.  Its only
registry configuration medium is immutable `NTVDM.REG` beside `ntvdm.exe`,
loaded at worker startup.  The successor's initial audit must enumerate every
selected WOW registry consumer.  Read-only configuration consumers (including
PMAP_COMPAT/profile mappings) must retain their original reader and bind it to
the same-shaped shadow provider.  The Win16 Shell `RegOpen/Create/Set/Enum/
Delete` family is not a configuration read: if selected consumers require it,
the owning S packet must deliver a finite shadow handle/tree/writeback
provider with close, failure, enumeration and concurrent-worker tests.  It
must not fall back to native HKLM/HKCU or be reduced to unconditional defaults.

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

## Proposed sequential S packets

Numbers below are proposed local S positions, activated only at T admission.
S1 audits the entire package before implementation batching. The following
seven packets define coherent ownership boundaries, not trace-derived repairs.
S1 must resolve their dependency graph and record any necessary regrouping
before implementation; later findings amend that graph with evidence.

| S | Complete scope | Required closure evidence |
| --- | --- | --- |
| S1 | Whole WOW32 dependency audit and implementation design | Reconcile all 77 units, ten dispatch tables, exports, dynamic lookups, callbacks, 21/20 registration and guest direct-data consumers. Inventory every required non-MVDM owner, imported subset, missing body and autonomous substitute. Produce exact import/binding/deletion lists, counts, source hashes, complete lifecycle/dependency graph and per-S tests. Classify every boundary; no unassigned or unexplored required edge may be deferred as "investigate later". This is design closure, not runtime acceptance. |
| S2 | USER client view plus W1/W2 task/window/message lifecycle | Treat backing, descriptors/TEB/desktop/WND/CLS/handles, publication/update/retirement, thread context, task scheduling, hung-app registration, classes/windows/dialogs, send/post and real CallBack16 as one coupled closure. Connect module/task/thread/worker teardown before backing release. Pass bootstrap and real guest create/query/change/callback/destroy, invalid/stale identity, failure rollback and repeated cleanup. Native-only fixtures cannot close this lifecycle. |
| S3 | GDI drawing, DC, fonts, bitmaps and DIB | Complete selected GDI transforms, text/font/glyph, palette, bitmap/DIB/mapping and metafile families; real guest drawing/content, native resource ownership, invalid input, allocation failure and repeated release. Use settled S2 identities; no deferred resource cleanup. |
| S4 | USER resources and menus | Complete resource lookup/conversion, accelerators, icons/cursors/bitmaps, menu/item/submenu graphs and default-window resource operations. Bind S2 callbacks and S3 bitmap services; test actual guest load/create/query/mutate/use/free, creation-time visibility and failed/partial resource loading. |
| S5 | Clipboard and DDE | Complete guest/native format conversion, data ownership transfer, message/reply and peer/module/task death. Test real guest exchanges, rejected/abandoned transfers, reentry, cancellation and repeated cleanup; original FreeDDEData alone is insufficient. |
| S6 | KERNEL/DOS, module/memory/file and OEM services | Complete selected loader-facing services, memory/module/resource aliases, file/directory/environment and related kernel thunks. Own OEM-WOW-DIR and OEM-WOW-DELETE, including all branches and real Win16 consumer tests; test failure, rollback and task/module release. Shared task policy consumes S2, not a second owner. |
| S7 | Remaining original interface families and registration completeness | Close every S1-assigned remaining Shell, Winsock, ToolHelp, COMM, print/spool, sound/multimedia, hooks, common-dialog/OLE and hard-error family. Use earlier owners for their shared mechanisms. Verify all selected dispatch entries, 21 inputs/20 outputs, startup rollback and family-specific teardown; remove residual selected placeholders. Each family has its own ledger rows and tests, never a blanket link-only pass. |
| S8 | Whole-provider and three-application acceptance | On one final artifact set run immutable WRITE.EXE, WINMINE.EXE and SOL.EXE through the scenarios below; verify original WOW16 loader, task/callback/exit, integrated OEM behavior, repeated tasks, failure/worker cleanup and subsequent DOS usability. Reconcile every preceding checklist, measured mirror/non-mirror changes and owner final acceptance report. No selected operation remains fixture-only or unbound. |

The following additions are mandatory closure gates for the corresponding
rows above:

- S1 delivers the complete producer/consumer/update matrix and uses bounded
  experiments to establish feasibility of the highest-risk shared-object,
  reentry and host-originated update contracts. Reuse retained experiments
  where their inputs remain valid. Exact mappings, ordering and test plans
  must be settled before dependent implementation; "public APIs should supply
  these fields" is not a design result. A proved unavailable contract names
  its precise dependency and design conflict; resolve that conflict before
  implementing the affected scope. Do not silently exclude it or claim full
  compatibility. Design feasibility is distinct from final runtime acceptance.
- S2 proves real guest queries during creation callbacks, mutation followed
  by direct reads, destruction inside a callback, stale/reused handles,
  thread/task switching and relevant external/native-object interactions.
  Exercise required changing data during guest execution without a WOW32
  call, and verify failure/retirement without partially published records.
- S4 applies the same contract to menu/item/submenu identity, relationships,
  owner-draw data, dynamic mutation and release. It consumes S2's settled
  publication/lifetime mechanism and owns menu-specific implementation/tests.
- S8 repeats the integrated direct-read, ordinary-thunk and real-callback
  routes on the same final artifacts. Application success supplements the
  per-family tests; it cannot waive an uncovered shared-data contract.

S2 deliberately combines the client view with its task/window/message owners:
a separately "finished" snapshot structure without a live producer and teardown
is prohibited. S3--S7 consume completed predecessor contracts. A required
dependency must be completed in its owner before the consumer closes, or the
coupled work must be explicitly regrouped; changing the label is not closure.
All deployable slots and entrypoints implemented by an S are wired immediately.
Typed explicit-failure placeholders may represent only identified later owners
and cannot satisfy a completed S's own contract.

OEM-WOW-DIR includes non-ASCII directory/environment synchronization in both
reached directions and rejected updates without false state. OEM-WOW-DELETE
includes ordinary and retained-file branches, rename/delete rollback, temporary
file cleanup and font-removal fallback with branch witnesses. S6 owns their
implementation and real consumer tests; S8 repeats integrated guest task/
callback/exit acceptance. OEM-DBG-PATH stays with T420 S43; any specifically
WOW-dependent integration found there is assigned to successor S7/S8 explicitly.

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

S8 owns the integrated OEM-WIN16-INTEGRATION retest and final consolidated
report. Failed implementation contracts reopen their source-owner ledger and
receive regression tests; S8 must not become a backlog of wiring left by
earlier S packets. T closure requires the owner's final acceptance audit.

## Mandatory gates and limits

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
