# T423 S13 text geometry evidence

## Question and inputs

Does DOS/native handoff preserve logical text extent independently of the
physical hidden Console viewport, and apply real backend geometry before input
resumes? Baseline is S12 P2 fbbbe4870; admission is 3da12318c. The active
authority is [CURRENT](../../states/CURRENT.md). Entries below are chronological:
later verified dispositions supersede earlier open/failed experiments. Publication
and final gate reconciliation are recorded explicitly, never inferred from an
intermediate pass.

**Current disposition:** the r106 eight-file publication and its r107-r112
post-publication gates pass. The earlier r79 failure and coherent rollback
remain recorded below; r79's exact emitting stage was not traced, so its
source must not be asserted retroactively. S13 still requires governance,
commit and push before closure; T423 awaits owner acceptance.

## Source audit

- Original `mvdm/softpc.new/host/src/nt_fulsc.c` calcScreenParams and
  DoFullScreenResume select 80 columns and 22/25/28/43/50 rows on the audited
  Console-return path. This is not a claim that every DOS video mode has that
  shape. Original mode, BIOS, copyConsoleToRegen and event-resume logic remain
  in place; no mirror or guest changes have been made for this candidate.
- Existing `opennt-abi/host-compat/console_grid.c` provides the source-shaped
  non-reflow buffer conversion. The candidate reuses it, rather than adding
  another cell-reflow engine.
- NTCON seeds from the copied frontend screen. Its existing exact-size seed
  can shrink native history when DOS returns a smaller page. Preserving that
  history is now implemented in the candidate and covered by repeated-seed
  tests below; complete live scrolling acceptance remains open.

## Procedure and observations

All builds are x86, under `build/M0-T423/S13` or the existing S1 formal cache.
Runtime evidence is under `O:/winnt/Logs2`. Private desktop tests do not switch
the user's desktop or use Computer Use. The candidate package is
`build/M0-T423/S13/p`, mounted as V:. During the early candidate experiments
in this section, O:/winnt remained the S12 publication; r106 later changed it.

| Evidence | Observation | Limit |
| --- | --- | --- |
| Published r135-r137 geometry probes | Same Window switched DOS 80x22/font16 to native 53x14/font16. Native buffer remained 80x22 but physical viewport was 53x14. | Root-cause evidence, not repair acceptance. |
| `t423-s13-console-geometry-r1` through r6 | Actual fixed 2x4 hidden carrier font permits the tested larger viewports; ordinary and extended Console setters have different minimum-size behavior. | Single-host API experiments, not universal support. |
| Candidate r11 | Existing presentation 336 checks passed; existing capture checks passed. | Targeted fixture, not full regression. |
| Candidate r14 | Active Console applied 80x25, 80x50, 120x40, 20x8, 80x43; readback and right/bottom cells passed. Existing inactive-buffer checks retained. | Active-buffer geometry feasibility, not real application resize acceptance. |
| Candidate r15 | DOS conversion selected supported modes, clipped without reflow, retained the cursor-containing row slice, and tested invalid-handle/default behavior. | Unit coverage, not live DOS mode proof. |
| Candidate r16 | Standard real Window native-interactive-return passed, including CMD marker and MEM output checks. | One standard case. |
| `t423-s13-geometry-r18.txt` and `.frames.txt` | Real DOS -> CMD -> DOS exited zero. Read-only Window probe observed 534 samples: one HWND, client 304x190, logical 80x25/font16 throughout. | No intentional native resize; reverse/native-first path still pending. |
| `t423-s13-history-r19.txt` and `-fixture.txt` | Presentation fixture passed 352 checks. Repeated import retained native history prefix/tail, copied the source page and kept viewport/cursor offsets stable. | Fixture, not all live scrolling paths. |
| `t423-s13-reverse-r20.txt` and `.frames.txt` | CMD -> DOS -> CMD exited zero; 580 samples retained 80x25/font16 and client 304x190. | Default dimensions, no deliberate resize. |
| `t423-s13-resize-r21.txt` and `.frames.txt` | Native MODE 120x40 left buffer 120x40 but viewport 43x33; reproduced inherited hidden-font clipping. | Failed geometry acceptance despite process exit zero. |
| `t423-s13-resize-r22.txt` and `.frames.txt` | After fixed hidden-carrier initialization, MODE 120x40 produced the complete 960x640 raster. DOS selected 80x25, and Win32 returned at 80x25 with its 40-row backing capacity preserved; DOS-CONVERTED and NATIVE-RETURNED appear in captured output. | Supports unsupported-mode fallback; not the entire regression matrix. |
| `t423-s13-supported-r23.txt` and `.frames.txt` | MODE 80x43 run disconnected with exit 109 before the DOS stage. | Failed; cause not established, must not count as mode support. |
| `t423-s13-supported-r24.txt` | Fresh candidate workers, native-only MODE 80x43 then echo/exit completed zero with actual buffer 80x43 and RESIZED-80-43 output. | Does not reproduce r23; not a complete 43-row DOS round trip. |
| `t423-s13-resize-r25-errors.txt` | Failed capture returned 87 during an application buffer resize, before frontend frame transmission. | Failure-stage evidence, not a pass. |
| `t423-s13-resize-r26.txt` and `.frames.txt` | Both 120x40 fallback and 80x43 DOS handoff completed zero; actual DOS frame became 80x43/font8. | Exposed a stale cached 80x25 frame during ownership return. |
| `t423-s13-race-r27-fixture.txt` | Deterministic API-edge tests: changed geometry retries; unchanged access-denied and parameter errors remain errors; success still reads 80 cells. | Injected API race, complements real logs. |
| `t423-s13-channel-r28.txt` / r29 | r28 caught spurious input after no-op geometry changes. Skipping identical real geometry corrected r29: 85 channel lifetimes pass, handles 432 before/after. | r28 remains failed evidence. |
| `t423-s13-unit-r30-*` | Existing real capture, frontend conversion and 352 presentation assertions passed. | Focused regression. |
| `t423-s13-resize-r31.txt`, `.frames.txt`, `-errors.txt` | Started with a 200-column host buffer but logical 80x25. Completed 120x40 -> DOS25 -> native43 -> DOS43/font8, without the stale25 frame. Log explicitly hit `resized-during-read error=87` and recovered. | Native resize race and logical geometry are proven for this sequence, not all modes. |
| `t423-s13-frame-r32-*` | Formal-graph race fixture, native frame encoding (63 checks) and shared-library renderer passed; added 80x50/font16 direct text rendering without changing the library. | DIB raster capacity remains separate from direct text capacity. |
| `t423-s13-console-r33-*` | Full Console suite stopped at nested-mem: only two of three outputs remained after initial DOS activation shrank history to25. | Failed regression, not a fixture waiver. |
| `t423-s13-console-r34-nested-mem.txt` | After limiting conversion to a published native page returning to DOS, the unchanged three-output assertion passed. | Focused repair confirmation; full suites rerun as r35/r36. |

Earlier r4/r7/r8/r10/r12/r13 experiments failed and are not accepted results.
r17 supplied literal escape text instead of carriage returns; its timeout is
not evidence that the product handoff passed or regressed. The corrected r18
used actual carriage returns. Reports and transcripts are retained.

The uncommitted candidate separates NTKVM logical viewport state from its
physical viewport, adds real NTCON geometry application/readback, and prepares
DOS-compatible geometry before the original worker return path. It does not
change shared libraries or guest media. The latest native-first font change
uses the same pinned V7VGA 16-scanline table as default VGA DOS; native-first
r20-r22 runtime probes include it. The earlier candidate manifest
`build/M0-T423/S13/candidate-manifest-r16.json` does not cover this latest edit.

NTCON now initializes its own hidden Console carrier with fixed 2x4 font
metrics before targets exist. It never queries display dimensions to select
logical geometry. This carrier font is distinct from copied V7VGA presentation
fonts. Ordinary application resize remains an actual Console operation; r21
proved that a fallback used only during seed was insufficient, so r22 tests
the startup change. Native screen reseed retains previous storage rows and
offsets a returning page into the prior viewport when needed; it invalidates
the differential publication cache if history remains outside the copied page.

Resize capture now rechecks actual geometry even when ReadConsoleOutputW
fails: only a confirmed intervening geometry change returns ERROR_RETRY.
The optional NTCON_GEOMETRY_ERROR_LOG records phase/error/actual carrier
geometry and never selects dimensions or changes recovery policy. Expected
configuration-not-found and post-frontend pipe closure can appear alongside
actual failures; they are not independently failed task evidence.

On ownership release the channel discards stale frame storage but retains its
anti-replay serial; only a newly completed frame can replace the common view.
Identical geometry is a no-op. Initial DOS activation preserves the original
scrollback path; conversion applies when an actual native page is handed back.
Native initial logical width is80 even if the inherited host backing is wider.
The NTCON source audit finds no GetSystemMetrics/GetLargestConsoleWindowSize/
MonitorFrom/GetMonitorInfo or use of dwMaximumWindowSize for selecting geometry.
The mirror and shared-library trees remain unchanged.

## Latest regression and pointer review

r35 Console and r36 Window summaries each pass17/17 with guest output
assertions. r37 channel lifetime/anti-replay checks pass85 lifetimes, handles435
before/after. r38 independently preserves WINMINE's main window, SOL's original
OOM modal and WRITE's original OOM modal; these are headless baseline checks,
not full SOL/WRITE acceptance. All logs use the t423-s13 prefixes under Logs2.

The new r39 production decoder -> pointer test fails with error31 on valid
80x50/font16 TEXT (t423-s13-pointer-r39-fixture.txt). window_frame.c's pointer
routine unconditionally reads the graphics union arm. The older test-local
raster conversion hid this production mismatch. This is an open defect, not
a passing mouse result. Direct TEXT supports800 scanlines, whereas unchanged
graphics transport permits768: repair must not silently crop, shrink fonts,
weaken the test, or change shared libraries to mask the issue.

## Remaining gates

- Repair and verify direct text-frame pointer composition and mouse mapping.

- Verify complete live scrollback/continuous output after the tested reseed fix.
- Explicit supported resize and broader native-first/reverse handoff,
  nested chains, independent sessions, font and mouse coordinates.
- Expand resize-race regression beyond the proven r31 hit and deterministic
  test; retain original error assertions and verify repeated handoffs.
- Confirm geometry revision/acknowledgment, partial failure, stale-frame
  rejection and input-resume ordering; no metadata-only acceptance.
- Audit initial buffer width versus logical width and raster capacity limits.
- Full Console/Window DOS regression, required WOW frontier and affected
  lifecycle tests; coherent eight-file publication and post-publication checks.
- Governance review, production commit/push and owner-facing results. S14
  remains unadmitted; T423 is not closed by this work.

## Pointer repair and logical input audit r40-r41

window_frame.c now validates the active union arm and uses the existing
frontend_text_frame_rasterize/library glyph renderer before GDI arrow
composition. The test adapter no longer performs this conversion on behalf of
the production pointer call. Formal x86 NTKVM and renderer fixture link.
The r40 fixture proves direct80x25/font16 conversion with unchanged640x400
extent. Its strict80x50 assertion still fails, now with ERROR_NOT_SUPPORTED50
at the explicit768-line graphics limit rather than an inactive-union read.
No shared-library edit or runtime publication has occurred. A minimal800-line
capacity exception has been requested from the owner; it is not yet approved.

r41 builds tests/component-integration/frontend_window_mouse_test.c together
with the production window_mouse.c (/MT /TC /std:c11, src and NTKVM include
roots). Output is build/M0-T423/S13/mouse-r41/test.exe; log is
O:/winnt/Logs2/t423-s13-mouse-r41-fixture.txt. Existing DOS/native/reset/failure
assertions and the added22/25/28/43/50-row matrix pass: logical viewport
origin100, bottom/right bounds, no invented button, font16->8 and smaller
viewport clamping. This is an input-converter test, not a real mouse roundtrip.

Source-order review: NTKVM apply_binding returns preparation errors before
installing DOS ownership; bind_worker waits for handoff_done and returns the
actual error. NTCON presentation_begin applies/readbacks screen state and
writes imported cells before successful return; a seed error releases the
native route. Existing per-channel generation/sequence and published serial
remain the ordering primitives. This review does not replace the outstanding
partial-failure and real backend resume evidence.

## Approved backend-owned text pointer (supersedes r40 composition)

Owner selected the reverse-video text-cell cursor, retaining text-frame ABI
and shared library. The800-line exception is withdrawn. NTKVM's native mouse
coordinate functions and GDI arrow compositor are removed. NTCON text_frame.c
owns logical mouse position, move-before-button translation, release and output
copy colour inversion. Canonical CHAR_INFO/hidden Console cells, caret and
handoff font state are not painted with the block. End hides/releases it before
the final screen receipt. Original DOS mouse code remains unchanged.

Copied input protocol18 adds POINTER motion/modifier records without frontend
geometry. Original DOS relative records retain their geometry notification;
NTKVM shares the routing mechanism, with finite encoding selection only.
Private pointer records are converted to native MOUSE_EVENTs by NTCON, never
sent as private tags to native applications. Failed delivery remains terminal
and is not replayed. Absolute Console mouse records remain native records.

r42 NTCON frame tests pass86 checks including80x50/font16 TEXT, underlying
cells/caret preservation, old-cell restoration on move, clamping/modifiers,
move-before-click, malformed input and leave. Common renderer tests pass.
The obsolete frontend arrow assertions have been replaced at their approved
backend owner, not waived. r43 old pipe/presentation checks pass352. r44 new
case19 failed because the test peer's >=17 layout-reply branch overwrote its
reply; narrowing that test branch to17/18 yields r45 with378 checks/zero
failures, including raw ENTER/MOVE/LEAVE through the production pipe into real
Console input records. These fixtures do not establish full Window acceptance.

Formal x86 product targets link and WOW's incremental graph reports no work.
The unused build candidate was updated as one eight-file set. Full Console
r46 and Window r47 regressions are running; publication remains the S12 set.
Earlier complete regressions must be rerun for these changed production inputs.

r46 Console and r47 Window have now each completed17/17 passing cases.
r48 adds tests/observation/ntcon_mouse_probe.c, an ordinary native Console
application launched by candidate run16. It requests80x50 and waits for real
MOUSE_EVENT records. The existing private desktop observer uses WINDOW_INPUT,
TEXT_CURSOR and S7MOUSE.dll to switch through CAF, then injects motion/press/
release at the library output boundary. The downstream frontend queue, wire,
NTCON translation and hidden Console are production paths. Report
t423-s13-native-mouse-r48.txt exited0; console text contains NTCON-MOUSE-PASS
and mouse-window report confirms input-sink-acknowledged=yes. The source tests
movement before press, release and bounded80x50 coordinates. It is not physical
Raw Input/capture proof. The separately tested frame-copy assertions establish
the approved block composition; r48 is not an image comparison. Only exact
candidate processes were cleaned afterwards; the dependent processes exited
when the candidate broker was stopped. r49 WOW frontier checks preserve
WINMINE's main window and SOL/WRITE's separately observed OOM frontiers.

r50 runs native CMD MODE80x50 -> DOS COMMAND -> native CMD MODE120x40 -> DOS
COMMAND. The private Window probe records one HWND across1038 samples:
80x25/font16 ->80x50/font16 ->80x50/font8 ->960x320 native raster ->80x50/font8.
DOS-50, NATIVE-50 and DOS-RESTORE-50 markers are present and the observer records
normal exit0. The last known DOS50 mode, not a universal25 default, is restored.
Font8 is the original DOS50 mode choice, not frontend font shrinking to bypass
a transport limit. The native return inherits that font. This deliberate
native-mode test does not promise pixel-identical windows when backend font
metrics legitimately change; default compatible handoffs have separate probes.

## Final affected-fixture sweep r51-r52

The owner explicitly confirmed reverse-video text cells, not a pixel arrow.
The common TEXT frame and shared library remain unchanged; the input channel's
version18 change is for worker-owned pointer motion, not a new text renderer.

r51 rebuilt the x86 product and affected fixtures from the current graph.
All six isolated-desktop reports record normal exit0. The presentation fixture
passes378 checks, text-frame fixture86 checks, and capture-race fixture retains
the changed-geometry RETRY versus unchanged5/87 errors. Frontend encoding now
tests full signed32-bit native motion, modifiers, malformed buttons/actions
and absence of frontend-selected dimensions. Channel lifetime tests complete85
iterations with432 handles before/after, keeping stale-frame and input-order
assertions. The common library renderer fixture also passes. Reports use
O:/winnt/Logs2/t423-s13-r51-<fixture>.txt with .console.txt or -fixture.txt.

r52 found a missing test-link dependency, not a product-link failure:
frontend-text-handoff-test lacked console_frontend.obj and console_grid.obj
after the S13 geometry preparation call was added. The source graph generator
now supplies both original production dependencies. After regeneration and
incremental build, the real storage/font handoff fixture passes32 checks,
the real Console capture/resize fixture exits0 with its assertions, and the
expanded NTCON text-frame fixture passes130 checks. The latter retains all
previous86 checks and adds22/25/28/43/50 logical-height bounds, font8/14/16,
viewport offsets, full signed motion, modifiers, both-button release,
malformed leave non-mutation and independent pointer-instance state.
Reports use the corresponding t423-s13-r52 prefixes. This corrects the missing
fixture linkage without changing runtime production behavior.

Publication, final S13 gate reconciliation and the broader geometry/render
capacity audit remain open. In particular, passing direct80x50/font16 TEXT
does not prove every optional-style raster fallback fits the unchanged768-line
graphics carrier. Do not generalize the block-pointer result into that claim.
O:/winnt remains the S12 package; no S13 production P is claimed yet.

## Render-capacity and real-history closure r53-r63

r53 adds a strict production common-decoder pixel assertion for80x50/font16
with a single underlined cell. It fails: the optional-style raster fallback
exceeds768 lines even though the direct TEXT carrier supports800. r54/r55
repair only ntkvm-exe/text_frame.c, retaining the library and copied protocol.
For an otherwise directly representable page whose styled raster is too tall,
the adapter assigns used (font bank, glyph, underline) variants to the library's
existing512 text glyph slots. Colors, caret, dimensions and original wire
fonts/cells remain unchanged. Other frames retain their existing path. The
unchanged library renders the pixels. Tests check the last underline pixel,
adjacent unstyled cells, all512 variants byte-for-byte, and explicit rejection
of a513th distinct dual-font variant rather than cropping or losing style.
The existing bounded renderer is not claimed to support arbitrary resolutions,
fonts or unlimited simultaneous variants. Raw render logs are
build/M0-T423/S13/render-r53.log through render-r55.log.

With this production renderer, complete Console r56 and Window r57 each pass
17/17 output-gated DOS cases. WOW r63 independently preserves the three
previous frontiers: WINMINE main window, SOL OOM, WRITE OOM. Physical play,
foreground activation and clipping remain owner-waived, not proved by these
background observations.

New tests/observation/ntcon_geometry_handoff_probe.c is an ordinary Win32
target, built by the formal ntcon-geometry-handoff-probe.exe target and copied
only to candidate tests/GEOMH.EXE. It observes its inherited80x25 logical view,
deliberately applies80x43 or80x50 with a200-row history prefix, and performs
three real run16 COMMAND /c MEM and run16 CMD /c echo calls. It asserts each
direct result, full inherited dimensions after each return and retained native
history. No guest, process-memory or backend substitution is involved.

r58/r59 are failed fixture attempts: CreateProcess used FALSE inheritance,
discarding the handles named by the authenticated frontend/execution locators;
the nested launcher returned1727 before DOS ran. Matching CMD and the existing
chain fixture's TRUE inheritance restores the intended launch contract.
r60 then failed its oldest-row assertion because its buffer ended at the
active viewport: the native child's ordinary newline legitimately scrolled
out the oldest row. A speculative production seed-range edit was immediately
reverted before building/testing it. r61 leaves1000 rows of actual scrollback
capacity and passes all three cycles with unchanged production history logic.
This distinguishes ordinary finite-buffer eviction from handoff data loss;
the failed runs do not establish a product history defect.

r62 starts two private-desktop observers concurrently, with
MVDM_OBSERVER_PRIVATE_DESKTOP=1 and MVDM_OBSERVER_BUFFER_COLUMNS=200. Each runs
V:/run16.exe with target V:/tests/GEOMH.EXE, arguments V:/run16.exe,43 or50,
and its unique O:/winnt/Logs2/t423-s13-isolation-r62-<rows>-fixture.txt path.
Both reports exit0, all six cycles preserve their respective dimensions and
distinct history marker, and each captured .console.txt contains exactly three
MEM memory-summary markers and three NATIVE-CHAIN-CONTINUED markers. Thus
width comes from the logical contract despite the200-column observer buffer,
and repeated native/DOS/native calls do not cross-contaminate the two sessions.
Physical presentation remains smaller and scrollable; it is not the backend
logical extent. r61/r62 are actual program/Console runs, complementing the
mode-selection, failure/acknowledgment and source-order fixtures above.

## Actual BIOS mode activation r64-r67

r64 repeats the real Window DDWWDDWW chain: sixteen output/input checkpoints,
the same frontend and reusable workers, original DOS depth restoration, direct
completion results and final empty completion all pass. The existing chain
fixture and its assertions are unchanged.

The new disposable text_geometry_guest_probe.asm reads BIOS columns at0040:004A
and rows-minus-one at0040:0084. r65/r66 initially inspected them without asking
for a video service:22/28/43/50 reported25, while25 passed. This was insufficient
to establish a production geometry defect. Original spckbd.asm sw_video_io
handles stream character output without activating VGA; sw_mode_change issues
the original13FE BOP on other INT10 services. video.c disable_stream_io then
calls nt_fulsc.c host_disable_stream_io -> ConsoleInit -> calcScreenParams,
and original ega_vide.c recalc_text updates BIOS rows and CRTC state together.

r67 adds only INT10/AH=0F (query current mode, not set mode) to the test probe
before checking BDA. No production/mirror/guest-media edit was needed. All
five actual modes22/25/28/43/50 now pass three real native -> DOS probe ->
COMMAND/MEM -> native echo cycles each, with history and dimensions preserved.
The observers still start with a200-column host buffer. Reports are
O:/winnt/Logs2/t423-s13-modes-r67-<rows>.txt and -fixture.txt, with captured
guest S13-GUEST-GRID-PASS output. Earlier failures remain recorded; dormant
stream-mode BDA is not represented as a patched guest or a newly fixed bug.
The query exercises the original activation contract, not a host metadata-only
substitute or a guest mode-setting workaround.

## Publication and gate reconciliation

Final formal x86 /MT incremental build succeeds; WOW32 has no changed inputs.
The first invocation used the wrong lowercase Ninja target vdmredir.dll and
was rejected before building; the corrected VDMREDIR.dll target succeeds.
Only NTCON required final relinking after the restored unchanged presentation
object. r68 Console and r69 Window each pass the complete17/17 text-gated suite
against that final candidate. The strict generated-link ownership audit passes:
no frontend execution/backend/helper and both workers use the common client.
Source review finds zero changes in mvdm, opennt-host or imported shared lib.

r70 publishes the complete eight-file set, not individual locked-file copies.
No product process was running. Original COMMAND/MEM/EDIT/WINMINE/SOL/WRITE and
CONFIG.NT/AUTOEXEC.NT/system.ini hashes match the candidate; none is overwritten.
NTVDM.REG/user state is untouched. Previous eight files plus source-input and
old/new SHA-256 manifest are retained at
build/M0-T423/S13/publication-backup-r70/manifest.json. Every published hash
matches the tested formal output; copying has whole-set rollback on failure.

Published Console r71 passes17/17. The initial published Window r72 stops on
the missing-command case because its middle observer snapshot reports1237
(ERROR_RETRY). Original observer_console_snapshot rejects four unstable
geometry/content samples; Merge-ConsoleTextSnapshots correctly refuses that
capture. Final output and process exit exist, but this run is not accepted.
No product or assertion is changed for it; complete Window verification is
repeated with a fresh r75 evidence prefix. r80 independently repeats the real
hidden-Console unread-key-return fixture (--input-return):676 checks, zero
failures, normal process exit0. It supplements real production handoffs, not
substitutes for them.

r75 completes all17 published Window cases. r76 preserves WINMINE's window,
SOL's OOM and WRITE's OOM individually; r77 passes DOS mouse burst1000,
retire1000 and latency200. r78 ordinary NTCON80x50 mouse target receives real
motion before press and a release, with sink acknowledgment and exit0.
These passed gates do not override the subsequent r79 failure.

The new reusable tests/observation/verify-text-geometry-handoff.ps1 runs the
formal ntcon-geometry-handoff-probe.exe and NASM-built GRID.COM from existing
package tests/, in private desktops with a200-column observer buffer. It
requires nine child completions, three history/size checks, actual guest/MEM/
native output and normal observer exit for each of the five modes. No assertion
was weakened. Published r79 fails in22-row cycle0: GRID and MEM return0, but
the following native CMD returns1237 and its marker is absent from the final
visible snapshot. This is a real delivery failure, unlike r72's observer-only
snapshot failure. The coherent eight S12 binaries were immediately restored
and compared with all previous hashes in the r70 backup. No guest/configuration
or user file was rolled back or changed.

Candidate r81 passes the complete five-mode wrapper, insufficient to clear the
intermittent failure. Candidate-only diagnostic additions retain ERROR_RETRY
in the existing optional NTCON log and label begin-io/end-io failures. There
is no retry-policy change or swallowed error. r82 repeats the five-mode matrix
five times (75 real cycles); r83 adds the native Window mouse prelude before
each of three more matrices (45 cycles), and enables existing MVDM_S34_TRACE_PATH
launcher tracing. All pass without begin/end1237. r84's180000ms timeout exceeds
the observer's accepted range and is rejected before a product run. r85 uses
the valid60000ms bound and the authored probe's optional12-cycle argument;
all12 same-instance22-row cycles pass, also without a begin/end1237.

The failed r79 lacked this phase tracing, so the precise source of its1237 is
not yet proven. Seed/readback, final capture and broker Console-membership
discovery can each return that code; do not label it a specific producer bug
or fix it by merely ignoring ERROR_RETRY. Source review and these bounded runs
are diagnostic progress, NOT S13 closure. The current candidate has tracing
changes after the last full regression and must be rebuilt/revalidated before
another publication. There is no S13 production P2 yet.

## Locked-screen follow-up and similar-race audit

After the owner rejected polling as the primary answer, source review found
that NTKVM already held one `io_lock` per Console RPC, but NTCON read a
multi-tile frontend image over many RPCs. A DOS write or the NTKVM presenter
could run between tiles. Protocol19 now adds native-only SNAPSHOT_BEGIN/END:
the channel thread holds the same recursive frontend lock across the whole
read; all DOS Console writes and the presenter wait for the first acquirer.
The NTCON client releases it before applying the copied page to its hidden
Console. Channel EOF and invalid operations release the lock on that same
thread. Activation or input waits during a held snapshot are rejected to
avoid a lock-order deadlock. Original MVDM and guest code remain untouched.

The other audited sites have different ownership. NTCON's own hidden Console
can be written by arbitrary native targets which do not acquire this project
lock; its before/after snapshot validation and bounded retry remain necessary,
with permanent I/O failures propagated. NTCON membership sampling observes
the public Console process list, which provides no attachment-change event;
it is lifecycle observation, not the screen-copy synchronization mechanism.
Run16's registration wait is also separate from frame publication. No
additional screen race was repaired by an unconditional Sleep or by weakening
the output assertions.

Formal x86 protocol19 build succeeds. `t423-s13-snapshot-lock-r94` exits0:
102 real channel lifetimes, blocked contender during snapshot, END release,
EOF and forbidden activation release, and exact post-warm-up handle equality.
The isolated presentation fixture records385/0 checks; input-return677/0,
text-frame130/0 and capture-race0 failures. Candidate Console r97 and Window
r98 each pass all17 strict cases. WOW r99 preserves the three distinct
WINMINE/SOL/WRITE frontiers. The first mouse-pressure invocation r100 failed
before entering the guest because the isolated candidate lacked its authored
WMS7.COM fixture; no product bug is inferred. After copying only authored
tests into V:/tests, r101 passes1000/1000/200 DOS mouse events. r102 passes
the native NTCON move/press/release probe with sink acknowledgment. Final
candidate r103 passes five actual DOS modes with three DOS/native cycles each;
r105 passes the full DDWWDDWW sixteen-checkpoint chain. These are candidate
checks, not evidence that O:/winnt has been updated. The prior r79 failed
publication remains a real historical failure until the new full-package
post-publication gates pass.

## Coherent r106 publication and post-publication gates

`build/M0-T423/S13/publish-r106.ps1` verified the candidate against formal
x86 output, verified COMMAND/MEM/EDIT/WINMINE/SOL/WRITE plus CONFIG.NT,
AUTOEXEC.NT and system.ini matched O:/winnt, backed up the previous eight
files under `build/M0-T423/S13/publication-backup-r106`, then replaced the
eight binaries together. NTVDM.REG and original guest media were not written.
The manifest records old/new SHA-256 and current source-input hashes. Final
published hashes were independently rechecked and all match formal output.

Published `t423-s13-published-r107` passes all five actual DOS mode dimensions
and three DOS/MEM/native cycles each, including the former r79 22-row failing
case. Published r108 Console and r109 Window each pass all17 strict cases,
checking output markers and exit codes. Published r110 passes DOS mouse
pressure1000/1000/200; r111 passes NTCON move/press/release and sink ACK.
Published r112 preserves WINMINE's main window and the known SOL/WRITE OOM
frontiers, without claiming those two applications are complete. No begin-io
or end-io ERROR_RETRY appears in the r107 opt-in phase log. r79 did not have
that phase log, so its precise emitting function remains historically
unproven; the cross-RPC frontend screen race has been corrected and the
previously failing publication sequence now passes. This is a tested repair,
not a claim that every possible native writer can be locked by this product.

S13 production/test delivery is pushed as P1 `307c4a1b5`. The subsequent
documentation-only closure records the delivered revision without changing
the tested binary inputs or reopening T423/S14.
