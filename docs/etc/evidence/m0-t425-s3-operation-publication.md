# T425 S3 operation-based publication and worker clients

## Scope and current state

Baseline is delivered/pushed T425 S2 P1 36372657a, APP 0.0.425,
protocol/RPC35 I/O23, its coherent eight files at O:/winnt. S3 is active;
the current partial implementation is not a delivered P or published package.
S2 remains the owner's usable package. T425 is not complete.

## Source, contract and owner inventory

| Logic / current location | Provenance and shared decision | Target / independent reason |
| --- | --- | --- |
| Removed activation kind tag, console_channel.c kind_selected/native and console_frontend.c DOS_ACTIVE validation | Project-added dispatch policy; not original execution. Candidate removes worker identity from frontend authorization. | common/protocol ACTIVATE, reserved mode=0; channel identity authenticates ownership. |
| prepare_logical_surface, native_console_frontend.c | Project-added clone/rebase/current-viewport storage conversion. Reuse with explicit requested dimensions, not a DOS flag. | NTCON keeps cell storage/locking; NTVDM requests dimensions at its device boundary. |
| Removed run16_console_prepare_dos/opennt_dos_return_height, formerly console_frontend.c | Project adaptation of original nt_fulsc.c calcScreenParams/MID_VAL. Not an original mirror body. | Candidate moves height selection to NTVDM win32/console_geometry.h and its production Console client; all ten thresholds retained. Original mirror unchanged; NTVWM selects no VGA mode. |
| run16_console_prepare_text, console_frontend.c | Existing project-added storage conversion separated from the preceding VGA selector; same original cell-grid primitive, exact requested dimensions. | NTCON owns neutral storage conversion. New PREPARE_TEXT_REGION callback requests exact dimensions, not worker identity. The old frontend selector/wrapper is removed. |
| opennt_console_resize_grid, opennt-abi/host-compat | Existing shared source-shaped row-preserving primitive. Reuse, do not reimplement or relocate. | Original no-reflow/cursor-containing-tail rules remain; only explicit geometry requests invoke it. |
| Native-only publication/snapshot checks, console_channel.c | Project-added restrictions. Active-owner dispatch and transaction state are the relevant authority, not worker kind. | One callback/lock/rollback path for any authenticated active channel. |
| Video mutation lock and standalone import/rebind, console_channel.c | Project-added renderer/grid orchestration. Same atomicity requirement for both callers. | Lock dispatch plus complete-frame import and output-handle rebind together for both. |
| import_dos_text, native_console_frontend.c | Project-added bounded PC-glyph-to-cell storage decode, not original guest video execution. | Operation-neutral import_text_frame; pair/triple styles follow the same contract. Unicode cell publications retain their staged grid and are not reconstructed from glyphs. |
| ntcon_worker_client/exchange/video/prepend/decode, common/console/client | Existing common provider linked by both workers. No EXE-private client dependency or second provider is needed. | Keep common transport/codec instances, real peer/cancel handles and caller endpoint lock. Worker-base remains worker-only connection/liveness adaptation. |
| NTVDM win32/console_client and NTVWM presentation | Project-owned bindings use that same provider. Device mouse/VGA/original API adaptation and native hidden Console capture are genuinely different operations. | Preserve local original execution/device semantics and native capture. Neither becomes NTCON worker policy or an original mirror reverse call. |

Original OpenNT has no independent NTCON/NTVWM frontend to import. Reuse the
already selected project-owned storage, channel, snapshot and transport code;
do not create another service, helper, scheduler or provider. Original mirrors,
imported libraries, guest and configuration remain unchanged.

## Implemented partial path

Removed native-only snapshot/publication rejection and native-only frame-kind
rejection in the channel. Dispatcher ownership and existing state/length/serial
validation remain required. A staged text transaction still requires a complete
text frame matching its grid/viewport/cursor at END; a graphics payload is not
made a valid text transaction merely by removing the worker-kind check.

Both callers now hold the same root io_lock around video dispatch, standalone
complete-text import and output-handle replacement. Batch publication keeps its
previous committed grid/frame until END; EOF/ABORT rollback is unchanged.
Standalone imports use the same pair/triple decoder and retain optional
underline in cell attributes. Staged native Unicode cells are not overwritten
by the lossy PC-glyph frame. Snapshot lock remains held across read tiles and
is released on END, EOF or illegal operation. It owns no task completion.

Removed the no-op root screen_begin/screen_end API. Dispatch retains its
paired callbacks and tested begin/end error contract; the channel already
holds enter's I/O lock and projects after writes. There is no second screen
mutex. Exact requested storage resizing now uses run16_console_prepare_text.
Following the preparatory split, the candidate removes frontend VGA selection
entirely. NTVDM first acquires via the same common activation client as NTVWM,
reads the current logical viewport and then selects/request its original VGA
height locally. Preparation must succeed before guest input/output producers
resume. Failure releases the acquired owner; a failed release poisons the
channel rather than silently acknowledging a reusable route. Explicit region
conversion detaches an obsolete borrowed frame and retains copied font metadata.
NTVWM inherits current state without requesting VGA conversion. No mirror,
original device algorithm, execution scheduler or lifecycle policy is changed.

## Actual build and focused evidence

New run root: build/M0-T425/S3/r001. build.cmd runs the affected x86 /MT
CCPU40 product and fixture closure using the identity-validated dependency
cache build/M0-T424/S2/r001. These early runs retain wire35/I/O23; the later
operation-only candidate below advances it to36/24. The affected NTCON
and two fixtures link; warnings are retained pre-existing imported types and
Console DWORD/uint32_t declaration warnings, not newly weakened checks.

console_channel_lifetime_test.exe --private-desktop-full
O:/repos.hobby/ntvdm64/build/M0-T425/S3/r001/channel-r002.txt passes:

- Both channel kinds exercise snapshots with concurrent lock contention,
  END, EOF and illegal activation; real-pipe publication EOF preserves the
  previous committed cells for both kinds.
- Both kinds run the complete/partial/abort publication fixture, including
  offset viewport/cursor, four canvas intersections and stale ownership.
- Four standalone pair/triple/kind combinations retain glyphs/attributes and
  underline; malformed styles preserve the last committed frame/grid.
- Retained original VGA threshold, geometry/cursor, multi-pending FIFO,
  cancellation, real peer-death, park barrier and handle checks pass.
  Handle count is 437 before and after the measured acquisition/failure loop.

The first run channel-r001 fails the second publication instance's cursor
assertion because the first instance intentionally leaves a 60x20 physical
canvas. The repeated fixture now explicitly establishes the same 80x30 canvas
before each instance. The cursor, clipping and storage assertions are unchanged;
no production delay, redraw workaround or weakened assertion was introduced.

run-focused.ps1 uses the existing observer on unswitched private desktops:
presentation 426/0, pipe input-return 688/0 and text-handoff 32/0 pass. Link
ownership, including deliberate leakage controls, passes. These are
production-linked Console/transport fixtures, not guest/RPC/physical RDP
acceptance. Production activation is not certified by the presentation mock.

Latest affected-closure rebuild (same declared incremental cache/run root)
links nine affected compile/link steps successfully. The new explicit-size
cases in console_frontend_test.c execute on an unswitched private desktop
with actual Console handles, exit=0: exact 100x35 and 80x30 storage, invalid
zero/negative dimensions and invalid handle, unchanged failure acknowledgement
and all existing VGA/no-reflow/error assertions. No wire version changes yet.
channel-r003.txt passes after root no-op wrapper removal, with 435 handles
before/after. channel-r004.txt passes after explicit storage separation, with
439 handles before/after; these are per-process baseline counts, not a claim
that separate runs must have identical absolute counts. run-focused.ps1 -Run
r002 reruns the rebuilt binaries: presentation 426/0, input-return 688/0,
text-handoff 32/0 and link-ownership negative controls pass, terminal exit=0.
The build log is live incremental evidence, not a sealed release manifest.

## Required remaining work

- Validate final production handoffs and retained acceptance after the candidate
  removes activation's worker kind/prepare_vga flag and moves the VGA selector.
  The fixture header only composes the actual NTVDM selector and actual storage
  primitive; it is not linked as a product provider. The old unused frontend
  DOS-size predicate is deleted, not revived as a second device policy.
- Candidate app/IDL36 and I/O24 are synchronized and MIDL regenerated/relinked.
  Five real fail-closed version negatives pass for this candidate; repeat for
  inputs changed by the remaining publication work before final delivery.
- Audit complete standalone-frame failure atomicity: video_data currently
  replaces frame storage before logical import can fail. Do not claim generic
  locking alone proves rollback of an import/projection failure. Maintain
  explicit staged/committed ownership and detach before disposal; add negative
  assertions for that boundary rather than adding a retry/redraw heuristic.
- Remove dead/no-op screen wrapper layers and replaced client/type branches
  only after their actual callers and failure contracts are covered.
- Run the full S2 retained guest/native/RPC/version/lifecycle gates and
  Console17/Window17 against the final coherent candidate, review source and
  library identities, recoverably publish all eight files, postpublication
  smoke, commit/push and clean main. None of those final S3 gates is claimed
  by this partial fixture result.

S4 then audits the complete objective and stops for owner T verification.
Worker font/extent repair, imported-library changes, polling cleanup and
physical RDP capture acceptance remain outside this stage's claims.

## Operation-only activation candidate evidence

Same declared run root/cache; candidate protocol/RPC36 I/O24 is not published.
Initial v36 build failed because explicit generated v35 ifspec symbols in the
RPC client/server were not advanced. All three production uses and the identity
fixture now match v36; MIDL's generated service.h exposes client/server v36.
Affected x86 products and fixtures link. This failed build is not a pass.

channel-r005/r006 pass, including neutral activation, exact 100x35 request,
inactive-owner rejection, reserved kind rejection and invalid geometry retaining
committed storage. r006 handle count is 433 before/after. Existing ten VGA
thresholds now compile the same NTVDM selector as production. Console API fixture
retains no-reflow, cursor-containing-tail and blank-growth assertions.
run-focused.ps1 -Run r003/r004 passes presentation 426/0, input-return 688/0,
text-handoff 33/0 and link-ownership negative controls. The extra handoff
assertion requests explicit text geometry; no original assertion was weakened.

console-client-r002 uses the actual NTVDM project client and common transport
against real Console dispatch (only broker delivery/session binding substituted).
Its preparation failure injection requires the active owner to be released,
then proves successful reacquisition. All assertions reach the deliberate
original-shaped close callback: exit 73 with the final copied-text and close
markers, not an accidental zero from an observer. This is client integration,
not guest or real RPC authentication acceptance.

Runtime staging first failed because WOW32 is below cache/wow32 rather than
the cache root. No product publication occurred. ResumeIncomplete verifies
the already copied six EXEs against current linked inputs, then completes all
eight from the correct formal outputs. The isolated package is now used for
Console17/Window17 run. run-matrices.ps1 exits zero; both exact summary lists
have 17 rows with Actual=Expected, including nested COMMAND/MEM and EDIT exit
followed by MEM. The selected ordinary guest/Window observer assertions remain
unchanged. The temporary Z: mapping was removed on terminal completion.
run-version-negative.ps1 also terminates with exit zero: old application,
wrong request protocol, wrong reply application, wrong reply protocol and
legacy RPC interface all reject launcher and NTVDM before delivery, with no
launcher retry. Reports and generated test-only peers remain below
version-negative-r001; v36 client/server ifspec matches APP_PROTOCOL_VERSION.
The script removes its separate temporary Z: mapping. This is the actual
tested actor scope, not a claim that an independently launched NTVWM negative
peer was also exercised. O:/winnt remains the coherent S2 package,
and standalone frame import-failure atomicity remains open. These runs are
candidate evidence and must be repeated for inputs changed by the remaining
publication work; they do not prematurely close S3.

## Standalone publication transaction candidate

Project-owned NTCON adaptation only; original mirror, guest and imported
library code is unchanged. The old codec retired pixels on the last chunk,
before a fallible grid import. Reception now has explicit stage/validate,
abort and allocation-free commit operations. A validation stamp prevents
partial or unchecked staging from committing. The serial high-water survives
abort, preventing replay. The automatic-commit wrapper retains the same
validator for explicitly private staging and standalone codec fixtures.

Production channel VIDEO_DATA uses that staging path; the frontend prepares
an inactive decoded grid and prospective font configuration before committing
pixels/grid/window/font under the existing I/O lock. It never retains a pointer
to its stack-local prospective view. A precommit failure discards staging and
keeps old published storage; after logical commit a projection/handle-duplication
failure is an explicit terminal-channel failure, not a claimed rollback.
Batch publication and postcommit terminal failures still require final review
and negative evidence; this section does not close the whole atomicity row.

Commands and observations (same declared cache/run roots):

- `cmd /c build/M0-T425/S3/r001/build.cmd`: affected x86 product/fixture links
  pass, existing warnings retained in build-products.txt.
- `build/M0-T424/S2/r001/console-video-test.exe`: frame-staging-r001.txt,
  exit zero. Actual codec staged complete/partial/configuration, cancellation,
  malformed styles and serial high-water assertions pass, along with all prior
  dispatcher bounds/identity/retirement assertions. No rendering claim.
- Channel `--private-desktop channel-r007.txt` was inadvertently geometry-only:
  it passes geometry, not the full channel lifetime gate. Corrected command
  `console-channel-lifetime-test.exe --private-desktop-full <run>/channel-r008.txt`
  exits zero with all existing lifetime/pending/publication cases and standalone
  channel commit cases. Handle count is equal at 439 before/after.
- `run-focused.ps1 -Run r005`: observer and actors terminate successfully;
  presentation 426/0, input return 688/0, text handoff 33/0, link ownership and
  leakage-negative controls pass. No runtime guest or physical RDP claim.
- `tests/app/console_frame_failure_test.c` compiles the actual frontend with a
  test-only thread-local HeapAlloc substitution, then runs the unchanged full
  channel fixture plus pair/triple import-failure cases. No product hook or
  replacement renderer. Initial direct x86 `/MT /std:c11` compile/link succeeds
  (build-frame-failure-r001.txt); frame-failure-test.exe
  `--private-desktop-full <run>/frame-failure-r001.txt` exits zero. Allocation
  failure and revision exhaustion preserve old frame pointer/serial, exact
  cells/styles/grid/cursor/font metadata, cancel staging, retain high-water and
  permit the next legal publication. Full retained fixture assertions pass;
  before/after handle equality is 433. Broker attach alone is substituted by
  the fixture; this is not real RPC authentication or guest acceptance.

The formal graph generator now selects `console-frame-failure-test.exe` from
that source, frontend-window/common transport and the original grid primitive,
without also linking the normal frontend object (the test compiles that actual
source itself). Formal generator, affected product closure and the added target
build terminate successfully (generate-frame-failure-r001.txt and
build-frame-formal-r001.txt). The formal console-frame-failure-test.exe
`--private-desktop-full <run>/frame-failure-r002.txt` repeats all assertions,
exits zero and measures 439 handles before/after. Documentation governance,
relative links and diff whitespace checks pass. O:/winnt remains coherent S2;
earlier candidate matrices/version reports predate these changed inputs and
must not be used as final S3 publication evidence.

## Batch and terminal-projection review

Review found batch END still committed the grid before fallible font/revision
publication. Both standalone and batch now call the same frontend transaction;
batch supplies its already prepared private surface. The explicit committed
result defines surface ownership: precommit failure leaves staging caller-owned,
postcommit failure leaves the coherent new grid/frame/font frontend-owned.
Channel END releases old frame storage only after the shared transaction has
retargeted the renderer. Postcommit errors close the actual endpoint after its
error reply; no worker-kind branch or retry/repaint policy was added.

build-batch-commit-r001 succeeds but the new test initially failed at line 871
with error 998: its WriteConsoleOutputCharacterW argument was a wide character
value, not a string pointer. Both test call sites now use L"Y"; no production
failure was inferred and no assertion was removed. r002 rebuild and
frame-failure-r004 pass pair/triple batch revision exhaustion, unchanged old
grid/frame/font, abort/high-water and subsequent successful batch.

r003 rebuild and formal frame-failure-r005 private-full run terminate with zero.
Test-only thread-local WriteConsoleOutputW injection fails the actual canonical
projection, not the private staging writes. Both standalone and batch travel
through real production channel threads/pipes: caller receives ERROR_WRITE_FAULT,
thread exits with that error, pipe reports ERROR_BROKEN_PIPE, and the committed
logical glyph/frame remain coherent. All original full fixture cases still pass,
with 433 handles before/after. Fault substitutions exist only in the test source;
product APIs, renderer and runtime contain no test hook.

Fresh final candidate root is build/M0-T425/S3/r002. stage-final.ps1 copies the
accepted S2 guest/configuration and all eight current linked products, verifies
copy hashes and records candidate-manifest.json without changing O:/winnt.
Initial service-gate build incorrectly named observer.exe as a Ninja target;
Ninja rejected it before building. Observer is the existing independently built
tool, not a target in this graph. Corrected r002 service-gate build succeeds,
relinking actual protocol36 client/bootstrap/reservation/management/GUI fixtures.
Read-only service occupancy check was denied inside the sandbox, then succeeds
with the required permission and finds no existing service. No ACL or unrelated
process change was made. r002 run-regressions.ps1 starts the final Console17 and
Window17 against the fresh package; its result remains pending until its live
session terminates. Later retained/version/WOW/publication gates remain open.

Focused r006 rerun terminates with zero: presentation 426/0, input return 688/0,
text handoff 33/0 and link-ownership negative controls. Current source sweep
finds none of CONSOLE_IO_WORKER_*, kind_selected, native_channel, dos_channel,
prepare_vga, native_active or dos_active in the frontend/common/worker-base
production trees. Both actual worker clients call common ntcon_worker_activate;
the original VGA threshold selection is in the NTVDM project client, not NTCON.
Git diff has no mvdm, opennt-host or imported NTCON lib changes. This is bounded
source evidence; it does not substitute for final runtime gates or S4 audit.

Final r002 matrix process terminates with exit zero. Both fresh summary JSONs
contain exactly 17 cases and zero Actual/Expected mismatches (comparison is
per-row numeric values, not a comparison against a literal property name).
The retained regression process is separately live; its EDIT-return report
already records real modern EDIT, Ctrl+Q, CMD echo, DOS MEM and launcher
completion passing. That partial report is not a claim that the retained suite
has finished. O:/winnt remains the accepted S2 package.

Adjacent transaction review finds that prepare_logical_surface commits its new
grid before projecting it, while prepare_text clears the prior video reference
only when projection succeeds. A postcommit projection error could therefore
leave a reference describing the previous grid. This boundary still needs a
production/failure-contract review before S3 publication; no speculative
repaint, delay, worker-kind branch or assertion weakening is authorized.

Conversion boundary is now corrected in the common production frontend: grid
commit always detaches the prior frame, independently of projection success.
The local prepare operation returns a commit indication; its channel treats
postcommit projection or handle-refresh errors as terminal. Precommit failures
leave existing state intact. No wire revision or worker-kind branch is added.
build-conversion-r001 terminates zero with affected x86 production and fixture
links. frame-failure-conversion-r001 terminates zero: pair/triple tests also
inject actual projection failure after geometry commit and assert 80x28
storage, preserved glyph, detached frame and explicit terminal error. The
original full suite remains intact; handles before/after equal 439.

r002 retained suite terminates zero: modern EDIT return, repeated frontend
bootstrap, independent native sessions and broker-directed NTVDM/NTVWM worker
loss/frontend loss retirement pass. These reports precede the conversion
correction and are not silently re-labelled final evidence. RPC gates remain
live. A fresh r003 package is required for final runtime verification after
the correction. O:/winnt remains S2.

## Final input identity review in progress

RPC phase r002 terminates zero: all eleven authentication/startup/loss/grace/
management/command/registry cases and all five GUI startup/wait/carrier-retirement/
text-return cases pass. Focused conversion rerun terminates zero with 426/0
presentation, 688/0 input return, 33/0 text handoff and link-ownership negative
controls. Source review confirms both real worker callers use the same common
activation client; only NTVDM's project binding requests its original VGA region.

Graph refresh records current source inputs. Its dependency build recompiles
generated/base_vdm_config.c and relinks five products, changing their hashes.
The other three retain identity. The sealed r003 runtime is not overwritten:
its live matrices remain evidence for that package, not the later relink.
Fresh r004 contains the eight latest linked products with verified cache/stage
hash identity and x86 PE validation. freeze-inputs.ps1 records 265 source and
generated input hashes, including untracked new sources/tests and build graph.
All 47 pinned imported-library inputs still match S2; git diff shows no original
MVDM/OpenNT mirror or imported-library delta. No inference that physical RDP
capture is fixed follows from these checks. r004 final runtime/publication gates
remain open; O:/winnt remains the accepted S2 set.

r003 matrices terminate zero, with exactly 17 rows and zero per-row result
mismatches in each summary. That sealed package remains supplemental evidence.
Latest r004 run-final.ps1 is now live, serializing matrices, retained lifecycle,
RPC/GUI, versions/WOW and strict repeated DIR to avoid the shared BaseSrv/Z:
endpoint conflict. No final phase is declared passed before its child exits.
The guarded r004 publication script refuses missing phase passes, changed
source/generated hashes, candidate/cache hash mismatch or changed accepted S2
publication. It preserves/verifies all eight S2 recovery files before exact
product-process shutdown, copies only the eight product files, verifies the
result and restores S2 on copy/validation failure. It has not been executed;
neither guest/configuration nor O:/winnt has been modified.

Submission review checks the actual activation, dispatch, publication BEGIN/
END/ABORT, borrowed-video lifetime and revision changes. Preparation happens
before mutation; the shared lock covers final frame/grid/font commit, and
postcommit external failure is terminal rather than a claimed rollback. Batch
Unicode cells retain their prepared grid; standalone byte-glyph decode retains
pair/triple attributes. Snapshot operations still exclude blocking activation/
input waits while holding their lock. Worker-specific execution/VGA selection
and hidden Console capture remain local worker responsibilities.

Broader NTCON/common-client/worker-base source search finds no worker-kind
dispatch predicate. window_frame.c still has a historical dos_text_frame helper
name, but selects it by CONSOLE_VIDEO_TEXT_FRAME for every source, not by worker
type; it is not evidence of a retained DOS-only renderer. The renderer/library
is unchanged in S3. 55 non-product guest/configuration files in the sealed r004
package exactly match S2. Final pipeline is still live in its Window matrix;
no uncompleted phase or physical desktop observation is labelled passed.

r004 matrices now terminate zero: both summaries contain exactly 17 cases and
zero per-row expected/actual mismatches, with the retained text/interaction
assertions unchanged. The same live pipeline has advanced to retained lifecycle
checks; its first actual modern EDIT/CMD/DOS return case passes. Final RPC,
version/WOW, strict-DIR and publication checks are still pending. The S2
postpublication smoke procedure is reproduced in r004 without overwriting S2
evidence or weakening assertions, and has not yet been executed.

r004 retained lifecycle and RPC phases now terminate zero: both workers'
worker/frontend loss, session isolation, EDIT/native/DOS return, all eleven
RPC fixtures and all five GUI routes pass against the selected production
inputs. The pipeline advances to version negatives/WOW; these are not yet
declared passed. Supplemental affected codec/dispatcher/client fixture build
terminates zero. console-video-test exercises the production staged codec and
dispatcher with atomic commit/cancel/style/configuration/high-water assertions.
Private real Console dispatcher exits zero; real NTVDM client fixture reaches
its deliberate original-close substitute exit73, not a normal zero exit.
Its captured output includes the actual worker-local VGA acquisition/failure
release and 80x50 dual-font/43-row replacement/malformed retention assertions.
The changed RPC interface identity fixture compiles against generated v36;
this is compile evidence only, not a new runtime acceptance claim. Source
search finds no stale v35 ifspec or protocol/IDL35 declaration in src/tests.

## Selected r004 release and postpublication results

All five selected-release phases in r004/final-phases-r001.txt terminate
successfully: matrices, retained lifecycle, RPC, versions/WOW and strict DIR.
Console and Window each contain exactly 17 passing expected/actual cases.
RPC11, GUI5 and five real version rejection cases pass. Strict DIR records
entered-dos=1, dirty-prompt=0 and final=0; its child remains alive after DIR.
Earlier r001/r002/r003 results are supplemental, not this release identity.

The initial manual WOW comparison incorrectly applied SOL's localized error
signature to WRITE. Reviewing the preceding S2 observations shows WRITE's own
English memory error. r004/verify-wow-frontiers.ps1 checks each application's
complete baseline-specific UTF16 signature and observation boundary; all three
pass that comparison. This preserves existing frontiers, not functional SOL/
WRITE acceptance or a new interactive WINMINE gameplay claim.

r004/publication-r001.txt records a coherent eight-file publication to O:/winnt.
published-manifest.json retains each SHA256; accepted-s2-recovery and
recovery-manifest.json preserve all eight preceding products. Publication
validates 265 frozen source/generated inputs, cache/stage identity and the
accepted S2 destination before replacement. No guest/configuration is changed.

r004/postpublication-smoke-r001.txt terminates zero and records:

- 12 immediate native/DOS relaunch pairs with final output and outer exit19.
- 12 interactive CMD exit/relaunches with final output and outer exit19.
- Same outer CMD DOS MEM/EXIT, native VER, DOS MEM/EXIT and cooked exit19.
- GUI startup result0 and explicit GUI wait result37.
- All eight published hashes retained after these tests.

S3's publication/input contracts are production-wired through both actual
worker clients. No original mirror or imported library changes are included.
Worker-local VGA/device and hidden Console interpretation remain independent.
Physical RDP capture and worker font/extent repair are not claimed completed.
The containing reviewed P delivers S3; S4 must still audit the whole objective.
